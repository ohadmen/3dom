#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform int u_txt;

varying vec4 v_xyz;
varying vec4 v_rgb;
varying vec3 v_eyeDir;

//! [0]
void main()
{

	    
    
    if(u_txt==1)
	{
		gl_FragColor =v_rgb;
	}
	else
	{
		gl_FragColor=vec4(0.0,0.0,0.0,1.0);	
	}
    
    
}
//! [0]

