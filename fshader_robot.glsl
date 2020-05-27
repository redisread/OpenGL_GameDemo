#version 150

in  vec4 color;
in vec3 N;
in vec3 V;
in vec3 lightPos_new;
in vec2 texCoord;	//

out vec4 fColor;

uniform int sflag;
uniform sampler2D texture;	//

void main() 
{
	// 分量
	vec3 ambiColor = vec3(0.1, 0.1, 0.1);
	vec3 diffColor = vec3(0.8, 0.5, 0.1);
	vec3 specColor = vec3(0.3, 0.3, 0.3); 
	
	vec3 N_norm = normalize(N);
	vec3 L_norm = normalize(lightPos_new - V);
	vec3 V_norm = normalize(-V);
	vec3 R_norm = reflect(-L_norm, N_norm);
	
	float lambertian = clamp(dot(L_norm, N_norm), 0.0, 1.0);
	float specular =  clamp(dot(R_norm, V_norm), 0.0, 1.0);
	
	float shininess = 10.0;
		
	// 计算
	vec4 fragmentColor = vec4(ambiColor + 
				   diffColor * lambertian +
				   specColor * pow(specular, 5.0), 1.0);

    //fColor = color + fragmentColor;
	if(sflag == 1)
		fColor = color;
	else
		fColor =  texture2D( texture, texCoord ) + 0.5 *fragmentColor;// + 0.2*color;//fragmentColor + color;
} 

