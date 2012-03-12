// simple.frag
varying vec3 position;
varying vec3 normal;
uniform sampler2D texture0;
uniform sampler2D texture1;

void main (void){
	// 法線ベクトル出力
/*	
	gl_FragColor.rgb = (normal.xyz + 1.0) * 0.5;
	gl_FragColor.a = 1.0;
*/
//	gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	vec4 tex0 = texture2D(texture0, gl_TexCoord[0].st);
	vec4 tex1 = texture2D(texture1, gl_TexCoord[1].st);
//	gl_FragColor = tex0 * tex1;

	vec3 fnormal = normalize(normal);
	vec3 light = normalize(gl_LightSource[0].position.xyz - position);
	float diffuse = dot(light, fnormal);

	gl_FragColor = gl_FrontLightProduct[0].ambient;
	if(diffuse > 0.0){
		vec3 view = normalize(position);
		vec3 halfway = normalize(light - view);
		float specular = pow(max(dot(fnormal, halfway), 0.0), gl_FrontMaterial.shininess);
		gl_FragColor += gl_LightSource[0].diffuse * tex0 * diffuse + gl_FrontLightProduct[0].specular * specular;
	}
}
