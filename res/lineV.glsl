#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;

attribute vec4 a_xyz;
attribute vec3 a_rgb;
attribute vec3 a_col;


varying vec4 v_xyz;
varying vec3 v_rgb;
varying vec3 v_col;

//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_xyz;

    //v_xyz = a_xyz;
	//v_rgb = a_rgb;
}
//! [0]
