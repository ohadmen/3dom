#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform vec4 u_col;
uniform vec4 u_bcol;
uniform float u_doVarCol;

attribute vec4 a_xyz;


varying vec4 v_xyz;
varying vec4 v_col;


//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_xyz;



    v_xyz = a_xyz;
	float a =  min(1.0,max(0.0,10.0*(gl_Position[2])));
	a = min(a,u_doVarCol);
    v_col=u_bcol*a+u_col*(1.0-a);
	
}
//! [0]
