// simple.frag
varying vec3 position;
varying vec3 normal;

void main (void){
	// 法線ベクトル出力
	gl_FragColor.rgb = (normal.xyz + 1.0) * 0.5;
	gl_FragColor.a = 1.0;
}
