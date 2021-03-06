// simple.vert
varying vec3 position;
varying vec3 normal;

void main(void){
	// 通常
	position = vec3(gl_ModelViewMatrix * gl_Vertex);
	normal = normalize(gl_NormalMatrix * gl_Normal);
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

/*
	// 法線方向に太らせる
	vec4 newpos;
	newpos.xyz = gl_Vertex.xyz + gl_Normal * 0.01;
	newpos.w = gl_Vertex.w;
	position = vec3(gl_ModelViewMatrix * newpos);
	normal = normalize(gl_NormalMatrix * gl_Normal);
	gl_Position = gl_ModelViewProjectionMatrix * newpos;
*/
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
}
