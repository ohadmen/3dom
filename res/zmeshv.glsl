#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 u_mvp;
uniform mat3 u_kinv;




attribute vec3 a_rgb;
attribute vec3 a_nrml;

attribute float a_u;
attribute float a_v;
attribute float a_z;

varying vec4 v_xyz;
varying vec3 v_rgb;
varying vec4 v_nrml;




//! [0]
void main()
{

	
	vec4  xyz;
	xyz[0]=u_kinv[0][0]*a_u+u_kinv[0][1]*a_v+u_kinv[0][2];
	xyz[1]=u_kinv[1][0]*a_u+u_kinv[1][1]*a_v+u_kinv[1][2];
	xyz[2]=a_z;
	xyz[3]=1.0;
    // Calculate vertex position in screen space
    
	gl_Position = u_mvp * xyz;
	

    v_xyz = xyz;
    v_rgb = vec3(a_u/639.0,a_v/479.0,a_z);
	v_nrml= normalize(u_mvp*vec4(a_nrml,0.0));


}
//! [0]
