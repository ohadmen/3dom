#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform vec4 u_col;

attribute vec4 a_xyz;
attribute vec4 a_rgb;

varying vec4 v_xyz;
varying vec4 v_col;


//! [0]
void main()
{
    gl_Position = mvp_matrix * a_xyz;

    v_xyz = a_xyz;
    v_col=a_rgb;
	
}
//! [0]
