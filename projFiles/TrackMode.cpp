#include "TrackMode.h"
#include "Trackball.h"
#include "TrackUtils.h"

// Track mode implementation, dummy.
void TrackMode::Apply(Trackball&, float) {}

void TrackMode::Apply(Trackball&, const QVector3D&) {}

void TrackMode::Draw(Trackball&) {}

void TrackMode::SetAction() {}

void TrackMode::Reset() {}

bool TrackMode::IsAnimating(const Trackball&) {
	return false;
}

void TrackMode::Animate(unsigned int, Trackball&) {
}

bool TrackMode::isSticky() {
	return false;
}

void TrackMode::Undo() {}

// draw an inactive trackball
void InactiveMode::Draw(Trackball& tb) {
	TrackUtils::drawSphereIcon(tb, false);
}

// Sphere mode implementation.
// the most important function; given a new point in window coord,
// it update the transformation computed by the trackball.
// General scheme : the transformation is a function of just
//   the begin and current mouse positions, with greater precision
//   is function of just two 3d points over the manipulator.
void SphereMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	QVector3D hitOld = TrackUtils::hitSphere(tb, tb.getLastPoint());
	QVector3D hitNew = TrackUtils::hitSphere(tb, new_point);
	tb.pushHit(hitNew);

	QVector3D center = tb.getCenter();
	QVector3D axis = QVector3D::crossProduct(hitNew - center, hitOld - center);
	axis.normalize();


	//  Figure out how much to rotate around that axis.
	//  float phi = Distance (hitNew, hitOld) / tb->radius;
	//  float phi = vcg::Angle(hitNew - center,hitOld - center)*(Distance(hitNew,center)/tb->radius);
	float phi = std::max(TrackUtils::vvAngle(hitNew - center, hitOld - center), ((hitNew, hitOld).length() / tb.getRadius()));
	tb.track().rotation() = QQuaternion(-phi, axis)*tb.getTrackPrev().rotation();
	//->track.rot = Quaternionf(-phi, axis) * tb->last_track.rot;
}

void SphereMode::Draw(Trackball& tb) {
	TrackUtils::TrackUtils::drawSphereIcon(tb, true);
}

// Pan mode implementation.
void PanMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	QVector3D hitOld = TrackUtils::hitViewPlane(tb.getCamera(), tb.getCenter(), tb.getLastPoint());
	QVector3D hitNew = TrackUtils::hitViewPlane(tb.getCamera(), tb.getCenter(), new_point);
	tb.translate(hitNew - hitOld);
}

void PanMode::Draw(Trackball& tb) {
	TrackUtils::TrackUtils::drawSphereIcon(tb, true);
	//DrawSphereAxis(tb);
	//DrawUglyPanMode(tb);
}
/*
// Z mode implementation.
void ZMode::Apply(Trackball& tb, float WheelNotch)
{
	QVector3D dir = (GetViewPlane(tb->camera, tb->center)).Direction();
	dir.Normalize();
	tb->Translate(dir * (-WheelNotch));
}

void ZMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	QVector3D dir = (GetViewPlane(tb->camera, tb->center)).Direction();
	dir.Normalize();
	tb->Translate(dir * (-2.0f * getDeltaY(tb, new_point)));
}

void ZMode::Draw(Trackball& tb) {
	TrackUtils::drawSphereIcon(tb, true);
	DrawUglyZMode(tb);
}

// Scale mode implementation.
void ScaleMode::Apply(Trackball& tb, float WheelNotch)
{
	tb->track.sca *= pow(1.2f, -WheelNotch);
}

void ScaleMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	tb->track.sca = tb->last_track.sca * pow(3.0f, -(getDeltaY(tb, new_point)));
}

void ScaleMode::Draw(Trackball& tb) {
	TrackUtils::drawSphereIcon(tb, true);
	DrawUglyScaleMode(tb);
}

// Axis mode implementation.
void AxisMode::Apply(Trackball& tb, float WheelNotch)
{
	tb->Translate(axis.Direction() * (WheelNotch / 10.0f));
}

void AxisMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	std::pair< QVector3D, bool > hitOld = HitNearestPointOnAxis(tb, axis, tb->last_point);
	std::pair< QVector3D, bool > hitNew = HitNearestPointOnAxis(tb, axis, new_point);
	if (hitOld.second && hitNew.second) {
		tb->Translate(hitNew.first - hitOld.first);
	}
}

void AxisMode::Draw(Trackball& tb) {
	TrackUtils::drawSphereIcon(tb, true);
	DrawUglyAxisMode(tb, axis);
}

// Plane mode implementation.
void PlaneMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	std::pair< QVector3D, bool > hitOld = HitPlane(tb, tb->last_point, plane);
	std::pair< QVector3D, bool > hitNew = HitPlane(tb, new_point, plane);
	if (hitOld.second && hitNew.second) {
		tb->Translate(hitNew.first - hitOld.first);
	}
}

void PlaneMode::Draw(Trackball& tb) {
	TrackUtils::drawSphereIcon(tb, true);
	DrawUglyPlaneMode(tb, plane);
}

// Cylinder mode implementation.
void CylinderMode::Apply(Trackball& tb, float WheelNotch)
{
	const float PI2 = 6.283185307179586232f;
	float angle = (snap == 0.0) ? WheelNotch / (tb->radius * PI2) : WheelNotch * snap;
	tb->track.rot = tb->last_track.rot * Quaternionf(angle, axis.Direction());
}

void CylinderMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	Plane3f viewplane = GetViewPlane(tb->camera, tb->center);
	Line3f axisproj;
	axisproj = ProjectLineOnPlane(axis, viewplane);
	float angle;
	const float EPSILON = 0.005f; // this IS scale independent
	if (axisproj.Direction().Norm() < EPSILON) {
		angle = (10.0f * getDeltaY(tb, new_point)) / tb->radius;
	}
	else {
		QVector3D hitOld = HitViewPlane(tb, tb->last_point);
		QVector3D hitNew = HitViewPlane(tb, new_point);
		axisproj.Normalize();
		QVector3D plusdir = viewplane.Direction() ^ axisproj.Direction();
		float distOld = signedDistance(axisproj, hitOld, plusdir);
		float distNew = signedDistance(axisproj, hitNew, plusdir);
		angle = (distNew - distOld) / tb->radius;
	}
	if (snap > 0.0) {
		angle = ((angle < 0) ? -1 : 1)* floor((((angle < 0) ? -angle : angle) / snap) + 0.5f)*snap;
	}
	//  tb->track.rot = tb->last_track.rot * Quaternionf (angle,axis.Direction());
	tb->track.rot = Quaternionf(-angle, axis.Direction()) * tb->last_track.rot;
}

void CylinderMode::Draw(Trackball& tb) {
	TrackUtils::drawSphereIcon(tb, true);
	DrawUglyCylinderMode(tb, axis);
}

// Path mode implementation.
void PathMode::Init(const std::vector < QVector3D > &pts)
{
	unsigned int npts = int(pts.size());
	assert(npts >= 2);
	points.reserve(npts);
	for (unsigned int i = 0; i < npts; i++) {
		points.push_back(pts[i]);
	}
	path_length = 0.0f;
	min_seg_length = Distance(points[0], points[1]);
	float seg_length;
	for (unsigned int i = 1; i < npts; i++) {
		seg_length = Distance(points[i - 1], points[i]);
		path_length += seg_length;
		min_seg_length = (std::min)(seg_length, min_seg_length);
	}
	if (wrap) {
		seg_length = Distance(points[npts - 1], points[0]);
		path_length += seg_length;
		min_seg_length = (std::min)(seg_length, min_seg_length);
	}
}

void PathMode::Reset()
{
	current_state = initial_state;
}

QVector3D PathMode::SetStartNear(QVector3D point)
{
	float p0_state = 0;
	QVector3D p0, p1;
	float nearest_state = 0;
	QVector3D nearest_point = points[0];
	float nearest_distance = Distance(nearest_point, point);
	unsigned int npts = int(points.size());
	for (unsigned int i = 1; i <= npts; i++) {
		if (i == npts) {
			if (wrap) {
				p0 = points[npts - 1];
				p1 = points[0];
			}
			else {
				break;
			}
		}
		else {
			p0 = points[i - 1];
			p1 = points[i];
		}
		//QVector3D segment_point=ClosestPoint(Segment3f(p0,p1),point);
		QVector3D segment_point;
		float distance;
		vcg::SegmentPointDistance<float>(Segment3f(p0, p1), point, segment_point, distance);
		// float distance=Distance(segment_point,point);
		if (distance < nearest_distance) {
			nearest_point = segment_point;
			nearest_distance = distance;
			nearest_state = p0_state + (Distance(p0, nearest_point) / path_length);
		}
		float segment_norm = Distance(p0, p1) / path_length;
		p0_state += segment_norm;
	}
	assert(nearest_state >= 0.0);
	if (nearest_state > 1.0) {
		nearest_state = 1.0;
		nearest_point = (wrap ? points[0] : points[npts - 1]);
	}
	initial_state = nearest_state;
	return nearest_point;
}

void PathMode::GetPoints(float state, QVector3D & point, QVector3D & prev_point, QVector3D & next_point)
{
	assert(state >= 0.0f);
	assert(state <= 1.0f);
	float remaining_norm = state;
	QVector3D p0(0, 0, 0), p1(0, 0, 0);
	unsigned int npts = int(points.size());
	for (unsigned int i = 1; i <= npts; i++) {
		if (i == npts) {
			if (wrap) {
				p0 = points[npts - 1];
				p1 = points[0];
			}
			else {
				break;
			}
		}
		else {
			p0 = points[i - 1];
			p1 = points[i];
		}
		float segment_norm = Distance(p0, p1) / path_length;
		if (segment_norm < remaining_norm) {
			remaining_norm -= segment_norm;
			continue;
		}
		prev_point = p0;
		next_point = p1;
		float ratio = remaining_norm / segment_norm;
		point = prev_point + ((next_point - prev_point) * ratio);
		const float EPSILON = min_seg_length * 0.01f;
		if (Distance(point, prev_point) < EPSILON) {
			point = prev_point;
			if (i > 1) {
				prev_point = points[i - 2];
			}
			else if (wrap) {
				prev_point = points[npts - 1];
			}
		}
		else if (Distance(point, next_point) < EPSILON) {
			point = next_point;
			if (i < (npts - 1)) {
				next_point = points[i + 1];
			}
			else {
				if (wrap) {
					next_point = points[1];
				}
				else {
					next_point = points[npts - 1];
				}
			}
		}
		return;
	}
	// rounding errors can lead out of the for..
	prev_point = p0;
	point = p1;
	if (wrap) {
		next_point = points[1];
	}
	else {
		next_point = points[npts - 1];
	}
}

void PathMode::Apply(Trackball& tb, float WheelNotch)
{
	undo_current_state = current_state;
	undo_old_hitpoint = old_hitpoint;

	const float STEP_COEFF = min_seg_length * 0.5f;
	float delta = (WheelNotch*STEP_COEFF) / path_length;
	QVector3D old_point, new_point, prev_point, next_point;
	GetPoints(current_state, old_point, prev_point, next_point);
	current_state = Normalize(current_state + delta);
	GetPoints(current_state, new_point, prev_point, next_point);
	tb->Translate(new_point - old_point);
}

float PathMode::Normalize(float state)
{
	if (wrap) {
		double intpart;
		float fractpart;
		fractpart = (float)modf(state, &intpart);
		if (fractpart < 0.0f)
			fractpart += 1.0f;
		return fractpart;
	}
	if (state < 0.0f)
		return 0.0f;
	if (state > 1.0f)
		return 1.0f;
	return state;
}

int PathMode::Verse(QVector3D reference_point, QVector3D current_point, QVector3D prev_point, QVector3D next_point)
{
	QVector3D reference_dir = reference_point - current_point;
	QVector3D prev_dir = prev_point - current_point;
	QVector3D next_dir = next_point - current_point;
	const float EPSILON = min_seg_length * 0.005f;
	if (reference_dir.Norm() < EPSILON)
		reference_dir = QVector3D(0, 0, 0);
	if (prev_dir.Norm() < EPSILON)
		prev_dir = QVector3D(0, 0, 0);
	if (next_dir.Norm() < EPSILON)
		next_dir = QVector3D(0, 0, 0);
	reference_dir.Normalize();
	prev_dir.Normalize();
	next_dir.Normalize();
	float prev_coeff, next_coeff;
	prev_coeff = prev_dir.dot(reference_dir);
	next_coeff = next_dir.dot(reference_dir);
	if (prev_coeff < 0.0f)
		prev_coeff = 0.0f;
	if (next_coeff < 0.0f)
		next_coeff = 0.0f;
	if ((prev_coeff == 0.0f) && (next_coeff == 0.0f)) {
		return 0;
	}
	if (prev_coeff <= next_coeff) {
		return 1;
	}
	return -1;
}

float PathMode::HitPoint(float state, Ray3fN ray, QVector3D &hit_point)
{
	QVector3D current_point, next_point, prev_point;
	GetPoints(state, current_point, prev_point, next_point);

	QVector3D closest_point;
	closest_point = ray.ClosestPoint(current_point);
	int verse = Verse(closest_point, current_point, prev_point, next_point);
	if (verse == 0) {
		hit_point = current_point;
		return 0.0f;
	}

	Segment3f active_segment;
	if (verse > 0) {
		active_segment = Segment3f(current_point, next_point);
	}
	else {
		active_segment = Segment3f(current_point, prev_point);
	}

	//hit_point=ClosestPoint(active_segment,closest_point);
	float dist;
	vcg::SegmentPointDistance<float>(active_segment, closest_point, hit_point, dist);

	return verse * ((hit_point - current_point).Norm() / path_length);
}

void PathMode::SetAction() {
	QVector3D temp1, temp2;
	GetPoints(current_state, old_hitpoint, temp1, temp2);
}

void PathMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	undo_current_state = current_state;
	undo_old_hitpoint = old_hitpoint;

	Ray3fN ray = line2ray(tb->camera.ViewLineFromWindow(new_point));
	QVector3D hit_point;
	float delta_state = HitPoint(current_state, ray, hit_point);
	current_state = Normalize(current_state + delta_state);
	tb->Translate(hit_point - old_hitpoint);
}

bool PathMode::isSticky() {
	return true;
}

void PathMode::Undo() {
	current_state = undo_current_state;
	old_hitpoint = undo_old_hitpoint;
}

void PathMode::Draw(Trackball& tb) {
	TrackUtils::drawSphereIcon(tb, true);
	QVector3D current_point, prev_point, next_point;
	GetPoints(current_state, current_point, prev_point, next_point);
	DrawUglyPathMode(tb, points, current_point, prev_point,
		next_point, old_hitpoint, wrap);
}

// Area mode implementation.
void AreaMode::Init(const std::vector < QVector3D > &pts)
{
	unsigned int npts = int(pts.size());

	assert(npts >= 3);
	//get the plane
	QVector3D p0 = pts[0];
	unsigned int onethird = (unsigned int)floor(npts / 3.0);
	const float EPSILON = 0.005f;
	bool pts_not_in_line = false;
	QVector3D a, b;
	for (unsigned int i = 0; i < onethird; i++) {
		a = (pts[(i + onethird) % npts] - pts[i%npts]).normalized();
		b = (pts[(i + (2 * onethird)) % npts] - pts[i%npts]).normalized();
		pts_not_in_line = (a ^ b).Norm() > EPSILON;
		if (pts_not_in_line) {
			plane.Init(pts[i%npts],
				pts[(i + (onethird)) % npts],
				pts[(i + (2 * onethird)) % npts]);
			break;
		}
	}
	assert(pts_not_in_line);
	float ncx, ncy, ncz;
	ncx = fabs(plane.Direction()[0]);
	ncy = fabs(plane.Direction()[1]);
	ncz = fabs(plane.Direction()[2]);
	if ((ncx > ncy) && (ncx > ncz)) {
		first_coord_kept = 1;
		second_coord_kept = 2;
	}
	else if ((ncy > ncx) && (ncy > ncz)) {
		first_coord_kept = 0;
		second_coord_kept = 2;
	}
	else {
		first_coord_kept = 0;
		second_coord_kept = 1;
	}
	points.reserve(npts);
	for (unsigned int i = 0; i < npts; i++) {
		points.push_back(plane.Projection(pts[i]));
	}
	min_side_length = Distance(points[0], points[1]);
	for (unsigned int i = 1; i < npts; i++) {
		min_side_length = (std::min)(Distance(points[i - 1], points[i]), min_side_length);
	}
	rubberband_handle = old_status = status = initial_status = p0;
}

void AreaMode::Reset()
{
	rubberband_handle = old_status = status = initial_status;
	path.clear();
}

void AreaMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	undo_begin_action = begin_action;
	undo_status = status;
	undo_delta_mouse = delta_mouse;
	undo_old_status = old_status;
	undo_rubberband_handle = rubberband_handle;
	undo_path_index = path.size();

	if (begin_action) {
		delta_mouse = tb->camera.Project(status) - new_point;
		begin_action = false;
	}
	std::pair< QVector3D, bool > hitNew = HitPlane(tb, new_point + delta_mouse, plane);
	if (!hitNew.second) {
		return;
	}
	QVector3D hit_point = hitNew.first;
	QVector3D delta_status = Move(status, hit_point);
	status += delta_status;
	tb->Translate(status - old_status);
	rubberband_handle = hit_point;
}


void AreaMode::SetAction()
{
	begin_action = true;
	old_status = status;

	path.clear();
	path.push_back(status);
	rubberband_handle = status;
}

QVector3D AreaMode::Move(QVector3D start, QVector3D end)
{
	const float EPSILON = min_side_length*0.001f;
	QVector3D pt = start;
	bool done = false;
	bool end_inside = Inside(end);
	while (!done) {
		path.push_back(pt);
		Segment3f segment(pt, end);
		bool p_on_side = false;
		bool hit = false;

		QVector3D pside(0, 0, 0), phit(0, 0, 0);
		bool slide = false, mid_inside = false;

		int np = int(points.size()), i, j;
		for (i = 0, j = np - 1; i < np; j = i++) {
			Segment3f side(points[i], points[j]);
			QVector3D pseg, psid;
			//std::pair<float,bool> res=SegmentSegmentDistance(segment,side,pseg,psid);
			std::pair<float, bool> res;
			vcg::SegmentSegmentDistance(segment, side, res.first, res.second, pseg, psid);
			if (res.first < EPSILON && !res.second) {
				float dist = Distance(pt, pseg);
				if (dist < EPSILON) {
					//QVector3D pn=ClosestPoint(side,end);
					QVector3D pn;
					float dist;
					vcg::SegmentPointDistance<float>(side, end, pn, dist);
					if (!p_on_side || (Distance(pn, end) < Distance(end, pside))) {
						pside = pn;
						p_on_side = true;
					}
				}
				else {
					if (!hit || Distance(pt, pseg) < Distance(pt, phit)) {
						phit = pseg;
						hit = true;
					}
				}
			}
		}
		if (p_on_side)
			slide = Distance(pside, pt) > EPSILON;

		if (hit)
			mid_inside = Inside(pt + ((phit - pt) / 2));

		if (!hit && end_inside) {
			pt = end;
			done = true;
		}
		else if (hit && (!p_on_side || (p_on_side && mid_inside))) {
			pt = phit;
		}
		else if (p_on_side && slide) {
			pt = pside;
		}
		else {
			done = true;
		}
	}
	path.push_back(pt);
	return pt - start;
}

// adapted from the original C code by W. Randolph Franklin
// http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
bool AreaMode::Inside(QVector3D point)
{
	bool inside = false;
	float x = point[first_coord_kept];
	float y = point[second_coord_kept];
	float yi, yj, xi, xj;
	int i, j, np = int(points.size());
	for (i = 0, j = np - 1; i < np; j = i++) {
		xi = points[i][first_coord_kept];
		yi = points[i][second_coord_kept];
		xj = points[j][first_coord_kept];
		yj = points[j][second_coord_kept];
		if ((((yi <= y) && (y < yj)) || ((yj <= y) && (y < yi))) &&
			(x < (xj - xi) * (y - yi) / (yj - yi) + xi))
		{
			inside = !inside;
		}
	}
	return inside;
}

QVector3D AreaMode::SetStartNear(QVector3D point)
{
	QVector3D candidate = plane.Projection(point);
	if (Inside(candidate)) {
		initial_status = candidate;
		return initial_status;
	}
	QVector3D nearest_point = initial_status;
	float nearest_distance = Distance(nearest_point, candidate);
	int i, j, np = int(points.size());
	for (i = 0, j = np - 1; i < np; j = i++) {
		Segment3f side(points[i], points[j]);
		//QVector3D side_point=ClosestPoint(side,candidate);
		//float distance=Distance(side_point,candidate);
		QVector3D side_point;
		float distance;
		vcg::SegmentPointDistance<float>(side, candidate, side_point, distance);
		if (distance < nearest_distance) {
			nearest_point = side_point;
			nearest_distance = distance;
		}
	}
	initial_status = nearest_point;
	return initial_status;
}

bool AreaMode::isSticky() {
	return true;
}

void AreaMode::Undo() {
	begin_action = undo_begin_action;
	status = undo_status;
	delta_mouse = undo_delta_mouse;
	old_status = undo_old_status;
	rubberband_handle = undo_rubberband_handle;
	for (size_t i = path.size() - 1; i > undo_path_index; --i)
		path.pop_back();
}

void AreaMode::Draw(Trackball& tb)
{
	TrackUtils::drawSphereIcon(tb, true);
	DrawUglyAreaMode(tb, points, status, old_status, plane, path, rubberband_handle);
}

// Polar mode implementation.
void PolarMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	QVector3D hitOld = HitViewPlane(tb, tb->last_point);
	QVector3D hitNew = HitViewPlane(tb, new_point);
	float dx = (hitNew.X() - hitOld.X());
	float dy = (hitNew.Y() - hitOld.Y());

	const float scale = float(0.5*M_PI); //sensitivity of the mouse
	const float top = float(0.9*M_PI / 2); //maximum top view angle

	float anglex = dx / (tb->radius * scale);
	float angley = -dy / (tb->radius * scale);
	enda = alpha + anglex;
	endb = beta + angley;
	if (endb > top) endb = top;
	if (endb < -top) endb = -top;
	tb->track.rot = Quaternionf(endb, QVector3D(1, 0, 0)) *
		Quaternionf(enda, QVector3D(0, 1, 0));

}

void PolarMode::SetAction() {
	alpha = enda;
	beta = endb;
}

void PolarMode::Reset() {
	alpha = beta = enda = endb = 0;
}


void PolarMode::Draw(Trackball& tb) {
	TrackUtils::drawSphereIcon(tb, true);
}


// Navigator WASD implementation

NavigatorWasdMode::NavigatorWasdMode() {
	_flipH = 1; _flipV = 1;
	SetTopSpeedsAndAcc(1, 1, 4);
	step_height = step_length = 0;
	Reset();
};

void NavigatorWasdMode::Reset() {
	alpha = 0;
	beta = 0;
	current_speed.SetZero();
	step_x = 0.0f;

	step_current = step_last = 0.0;
}

void NavigatorWasdMode::FlipH() {
	_flipH *= -1;
}

void NavigatorWasdMode::FlipV() {
	_flipV *= -1;
}


void NavigatorWasdMode::SetAction() {

}

bool NavigatorWasdMode::IsAnimating(const Trackball& tb) {
	const unsigned int MOVEMENT_KEY_MASK = (const unsigned int)(~Trackball::MODIFIER_MASK);
	if (tb->current_button & MOVEMENT_KEY_MASK) return true;
	if (current_speed != QVector3D(0, 0, 0)) return true;
	if (step_current > 0.0) return true;
	return false;
}

void NavigatorWasdMode::Animate(unsigned int msec, Trackball& tb) {
	vcg::QVector3D acc(0, 0, 0);

	float sa = sin(-alpha);
	float ca = cos(-alpha);
	if (tb->current_button & Trackball::KEY_UP) acc += vcg::QVector3D(sa, 0, ca)*(accY*_flipH);
	if (tb->current_button & Trackball::KEY_DOWN) acc -= vcg::QVector3D(sa, 0, ca)*(accY*_flipH);
	if (tb->current_button & Trackball::KEY_LEFT) acc -= vcg::QVector3D(-ca, 0, sa)*accX;
	if (tb->current_button & Trackball::KEY_RIGHT) acc += vcg::QVector3D(-ca, 0, sa)*accX;
	if (tb->current_button & Trackball::KEY_PGUP) acc -= vcg::QVector3D(0, 1, 0)*accZ;
	if (tb->current_button & Trackball::KEY_PGDOWN) acc += vcg::QVector3D(0, 1, 0)*accZ;

	float sec = msec / 1.0f;
	current_speed += acc*sec;
	tb->track.tra += current_speed*sec;

	// compute step height.
	QVector3D current_speed_h = current_speed;
	current_speed_h[1] = 0;
	float vel = current_speed_h.Norm();
	if (vel < topSpeedH*0.05) {
		// stopped: decrease step heigth to zero
		step_current *= pow(dumping, sec);
		if (step_current < step_height*0.06) { step_current = 0; step_x = 0.0f; }
	}
	else {
		// running: rise step heigth
		vel = current_speed.Norm();
		step_x += vel*sec;
		float step_current_min = (float)fabs(sin(step_x*M_PI / step_length))*step_height;
		if (step_current < step_current_min) step_current = step_current_min;
	}

	current_speed *= pow(dumping, sec);
	if (current_speed.Norm() < topSpeedH*0.005) current_speed.SetZero(); // full stop

	tb->track.tra[1] += step_last;
	tb->track.tra[1] -= step_current;
	step_last = step_current;

	//tb->track.tra[1]+=0.01;
}

void NavigatorWasdMode::Apply(Trackball& tb, const QVector3D& new_point)
{
	QVector3D hitOld = tb->last_point;
	QVector3D hitNew = new_point;
	tb->last_point = new_point;
	float dx = (hitNew.X() - hitOld.X());
	float dy = (hitNew.Y() - hitOld.Y());

	const float scale = float(150 * M_PI); //sensitivity of the mouse
	const float top = float(0.9f*M_PI / 2); //maximum top view angle

	float anglex = dx / (tb->radius * scale);
	float angley = -dy / (tb->radius * scale * 0.5f);
	alpha += anglex*_flipH;
	beta += angley*_flipV;
	if (beta > +top) beta = +top;
	if (beta < -top) beta = -top;

	QVector3D viewpoint = tb->track.InverseMatrix()*QVector3D(0, 0, 0);
	tb->track.tra = tb->track.rot.Inverse().Rotate(tb->track.tra + viewpoint);
	tb->track.rot = Quaternionf(beta, QVector3D(1, 0, 0)) *
		Quaternionf(alpha, QVector3D(0, 1, 0));
	tb->track.tra = tb->track.rot.Rotate(tb->track.tra) - viewpoint;

	tb->track.tra[1] += step_last;
	tb->track.tra[1] -= step_current;

	step_last = step_current;

}

void NavigatorWasdMode::SetTopSpeedsAndAcc(float hspeed, float vspeed, float acc) {
	// conversion to msec
	hspeed /= 1000;
	vspeed /= 1000;
	acc /= 1000000;

	accX = accY = acc;
	dumping = hspeed / (hspeed + acc);
	accZ = (vspeed / dumping) - vspeed;
	if (acc == 0) {
		accX = accY = hspeed;
		accZ = vspeed;
		dumping = 0.0;
	}
	topSpeedH = hspeed;  topSpeedV = vspeed;

}

void NavigatorWasdMode::SetStepOnWalk(float width, float height) {
	step_length = width;
	step_height = height;
}

void NavigatorWasdMode::Apply(Trackball& tb, float WheelNotch)
{
	tb->Translate(QVector3D(0, topSpeedV, 0)*(-WheelNotch * 100));
}


bool NavigatorWasdMode::isSticky() {
	return false;
}
*/