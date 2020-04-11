#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform int u_txt;
uniform vec3 u_colmap[256];
varying vec4 v_xyz;
varying vec3 v_rgb;

//! [0]

vec4 colFromBase(vec3 base00,vec3 nrml)
{
		vec3 base3 = vec3(0.99, 0.96, 0.89);
		vec3 base2 = vec3(0.92, 0.91, 0.83);
		vec3 va = vec3(0.0, 0.0, 1.0);
		vec3 vb = vec3(-0.57, -0.57, 0.57);
		float a = dot(nrml, va)*0.8+0.2;
		float b = dot(nrml, vb)*0.8+0.2;
		vec4 c = vec4(((a*base2 + (1.0-a)*base00) + (b*base3 + (1.0-b)*base00))*0.5, 1.0); 
		return c;
}

void main()
{
	vec3 ec_pos=vec3(v_xyz[0]/v_xyz[3],v_xyz[1]/v_xyz[3],v_xyz[2])/v_xyz[3];
     vec3 nrml = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    
	if(u_txt==2)
	{
		//normal
		gl_FragColor  = vec4((1.0-nrml)*0.5,1.0);
	}
	else if(u_txt==3)
	{
		//sythetic color
		vec3 base00 = vec3(0.40, 0.48, 0.51);
		gl_FragColor = colFromBase(base00,nrml);
	}
	else if(u_txt==4)
	{
		int ii=int(max(min(v_xyz[0]/2000.0+0.5,1.0),0.0)*255.0);
		
		gl_FragColor = colFromBase(u_colmap[ii],nrml);
	}
	else if(u_txt==5)
	{
		int ii=int(max(min(v_xyz[1]/2000.0+0.5,1.0),0.0)*255.0);
		gl_FragColor = colFromBase(u_colmap[ii],nrml);
	}
	else if(u_txt==6)
	{
		int ii=int(max(min((v_xyz[2]-300.0)/2000.0,1.0),0.0)*255.0);
		gl_FragColor = colFromBase(u_colmap[ii],nrml);

	}
	else
	{
		//rgbcolor
		gl_FragColor = vec4(v_rgb, 1.0); 
		
	}

    
    
    
}
//! [0]

