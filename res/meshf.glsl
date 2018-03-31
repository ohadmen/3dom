#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform int u_txt;
varying vec3 v_nrml;
varying vec4 v_xyz;
varying vec3 v_rgb;

//! [0]
void main()
{

    vec3 ec_pos=vec3(v_xyz[0]/v_xyz[3],v_xyz[1]/v_xyz[3],v_xyz[2])/v_xyz[3];
    vec3 base3 = vec3(0.99, 0.96, 0.89);
    vec3 base2 = vec3(0.92, 0.91, 0.83);
    vec3 base00 = vec3(0.40, 0.48, 0.51);
    
    vec3 ec_normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    
    ec_normal = normalize(ec_normal);
    
    float a = dot(ec_normal, vec3(0.0, 0.0, 1.0));
    float b = dot(ec_normal, vec3(-0.57, -0.57, 0.57));
    
    if(u_txt==0)
		gl_FragColor = vec4(v_rgb[0],v_rgb[1],v_rgb[2], 1.0); 
	else if(u_txt==1)
	{
		float p = 1.0;
		float q = 1.0-p;
		vec3 col;
		
		col += vec3(p,q,q)*(1.0-v_nrml[0])*0.5;
		col += vec3(q,p,q)*(1.0-v_nrml[1])*0.5;
		col += vec3(q,q,p)*(1.0-v_nrml[2])*0.5;
		
		gl_FragColor = vec4(col  ,1.0);
	}
	else
		gl_FragColor = vec4(((a*base2 + (1.0-a)*base00) + (b*base3 + (1.0-b)*base00))*0.5, 1.0); 
    
    
}
//! [0]

