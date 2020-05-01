#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform float u_ptSize;

attribute vec4 a_xyz;
attribute vec4 a_rgb;

varying vec4 v_xyz;
varying vec4 v_rgb;
varying vec3 v_eyeDir;


//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_xyz;
	gl_PointSize = u_ptSize/max(1.0f,abs(gl_Position[2]));
    v_xyz = a_xyz;
    v_rgb = a_rgb;
	v_eyeDir  = -1.0 * normalize(vec3(gl_Position));


}
//! [0]
