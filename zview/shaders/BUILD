load("//third_party:qt.bzl", "qt_resource")

qt_resource(
    name = "shaders",
    file_list = glob(["*.glsl"]),
    visibility = ["//zview:__subpackages__"],
    linkstatic = True
)