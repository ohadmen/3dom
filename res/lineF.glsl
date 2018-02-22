#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif



varying vec4 v_xyz;
varying vec4 v_col;

//! [0]
void main()
{


    gl_FragColor = vec4(v_col[0],v_col[1],v_col[2], v_col[3]); 
    
}
//! [0]

