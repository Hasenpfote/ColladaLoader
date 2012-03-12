// simple.frag
uniform sampler2D texture0;

void main (void){
	vec4 tex0 = texture2D(texture0, gl_TexCoord[0].st);
	gl_FragColor = tex0;
}
