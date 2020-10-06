import zview_module
import numpy as np


class PyZview:
    @staticmethod
    def _get_trimesh_indices(sz):
        indx = np.arange(sz[0] * sz[1], dtype=np.int32).reshape(sz[:2])
        triA = np.stack((indx[:-1, 1:], indx[:-1, :-1], indx[1:, 1:]), axis=2).reshape(-1, 3)
        triB = np.stack((indx[:-1, :-1], indx[1:, :-1], indx[1:, 1:]), axis=2).reshape(-1, 3)
        tri = np.r_[triA, triB]
        return tri

    @staticmethod
    def _rgba2floatcol(rgba):
        rgba = np.clip(rgba, 0, 1)
        rgbafloat = (rgba * 255).astype(np.uint8)
        rgbafloat = rgbafloat.flatten()
        rgbafloat = rgbafloat.view(np.float32)
        return rgbafloat

    @staticmethod
    def _str2rgb(str):
        if str == 'r':
            col = [1, 0, 0]
        elif str == 'g':
            col = [0, 1, 0]
        elif str == 'b':
            col = [0, 0, 1]
        elif str == 'y':
            col = [1, 1, 0]
        elif str == 'w':
            col = [1, 1, 1]
        elif str == 'k':
            col = [0, 0, 0]
        elif str == 'R':
            col = list(np.random.rand(3))
        else:
            raise RuntimeError("unknown color name")
        return col

    @classmethod
    def _get_pts_arr(cls, xyz, color, alpha):
        if len(xyz.shape) == 3:
            xyz = xyz.reshape(-1, xyz.shape[2])
        n, ch = xyz.shape
        assert ch >= 3
        if ch == 3:
            xyzrgba = np.c_[xyz, np.ones((n, 4))]
        elif ch == 4:
            xyzrgba = np.c_[xyz[:, :3], xyz[:, 3:4] * [1, 1, 1], np.ones((n, 1))]
        elif ch == 6:
            xyzrgba = np.c_[xyz, np.ones((n, 1))]
        elif ch == 7:
            xyzrgba = xyz
        else:
            raise RuntimeError("unknown number of channels")

        assert (xyzrgba.shape[1] == 7)

        if alpha:
            xyzrgba[:, -1] = alpha
        if color is not None:
            if isinstance(color, str):
                xyzrgba[:, 3:6] = np.array(cls._str2rgb(color))
            elif hasattr(color, '__len__'):
                if len(color) == 3:
                    xyzrgba[:, 3:6] = np.array(color)
                elif len(color) == n:
                    xyzrgba[:, 3:6] = np.array(color)
                elif color.size // 3 == n:
                    xyzrgba[:, 3:6] = color.reshape(-1, 3)
                elif color.size // 4 == n:
                    xyzrgba[:, 3:7] = color.reshape(-1, 4)
                else:
                    raise RuntimeError("unknown color option")
        rgba = cls._rgba2floatcol(xyzrgba[:, 3:])
        return np.c_[xyz[:, :3].astype(np.float32), rgba]

    def _set_cam_look_at(self, e, c, u):
        e, c, u = [x.tolist() if isinstance(x, np.ndarray) else x for x in [e, c, u]]
        return self.zv.setCameraLookAt(e, c, u)

    def __init__(self):
        self.zv = zview_module.interface()  # get interface
        s = np.sqrt(3) / 2
        self.marker = {'v': np.array([[-1, -s, 0], [0, 2 * s, 0], [1, -s, 0], [0, 0, 2 * s]]) / 2,
                       'f': np.array([[0, 3, 1], [1, 3, 2], [0, 2, 3], [0, 2, 1]]),
                       'counter': 0}
        self.rect = {'v': np.array(
            [[0, 0, 0], [0, 1, 0], [1, 1, 0], [1, 0, 0], [0, 0, 1], [0, 1, 1], [1, 1, 1], [1, 0, 1]]) * 2 - 1,
                     'f': np.array(
                         [[3, 1, 0], [3, 1, 2], [3, 6, 2], [3, 7, 6], [0, 1, 5], [0, 5, 4], [0, 7, 4], [0, 3, 7],
                          [1, 2, 6],
                          [1, 6, 5],
                          [5, 6, 7], [4, 5, 7]])}

    def add_rectangle(self, string, tform, color=None, alpha=None):
        tform = np.array(tform)
        assert tform.shape == (4, 4)
        v = self.rect['v'] @ tform[:3, :3].T + tform[:3, -1]
        return self.add_trimesh(string, v, self.rect['f'], color, alpha)

    def add_trimesh(self, string, xyz, faces, color=None, alpha=None):
        if len(xyz.shape) != 2:
            raise RuntimeError("expecting nxD for D>=3")
        xyzf = self._get_pts_arr(xyz, color, alpha)
        k = self.zv.addColoredMesh(string, xyzf, faces)
        if k == -1:
            raise RuntimeWarning("could not get response from zview app")
        return k

    def add_mesh(self, string, xyz, color=None, alpha=None):
        if len(xyz.shape) != 3:
            raise RuntimeError("expecting nxmxD for D>=3")
        faces = self._get_trimesh_indices(xyz.shape)
        return self.add_trimesh(string, xyz.reshape(-1, xyz.shape[2]), faces, color, alpha)

    def remove_shape(self, k):
        return self.zv.removeShape(k)

    def add_marker(self, pt, color=None, scale=1):
        xyzf = self._get_pts_arr(self.marker['v'] * scale + pt, color, 1)
        faces = self.marker['f']
        k = self.zv.addColoredMesh('marker-{}'.format(self.marker['counter']), xyzf, faces)
        self.marker['counter'] = self.marker['counter'] + 1
        if k == -1:
            raise RuntimeWarning("could not get response from zview app")
        return k

    def update_marker(self, handle, pt, color=None, scale=1):
        xyzf = self._get_pts_arr(self.marker['v'] * scale + pt, color, 1)
        self.zv.updateColoredPoints(handle, xyzf)

    def add_points(self, string, xyz, color=None, alpha=None):
        xyzf = self._get_pts_arr(xyz, color, alpha)
        k = self.zv.addColoredPoints(string, xyzf)
        return k

    def update_points(self, handle, xyz, color=None, alpha=None):
        xyzf = self._get_pts_arr(xyz, color, alpha)
        self.zv.updateColoredPoints(handle, xyzf)

    KEY_ESC = 16777216
    KEY_ENTER = 16777220

    def get_last_keystroke(self):
        return self.zv.getLastKeyStroke()
