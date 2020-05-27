#version 150

in  vec4 vPosition;
in  vec4 vColor;
in vec3 vNormal;
in  vec2 vTexCoord;//


out vec4 color;
out vec3 V;
out vec3 N;
out vec3 lightPos_new;
out vec2 texCoord;//

uniform vec3 lightPos;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 draw_color;
uniform mat4 modelViewMatrix;
uniform mat4 shadowMatrix;

void main() 
{
    color = draw_color;
    
	vec4 v1 = Projection * modelViewMatrix * shadowMatrix * ModelView * vPosition;
	vec4 v2 = vec4(v1.xyz / v1.w, 1.0);
	gl_Position = v2;


	vec4 vertPos_cameraspace = ModelView * vPosition;


	V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
	

	vec4 lightPos_cameraspace = ModelView * vec4(lightPos, 1.0);

	lightPos_new = lightPos_cameraspace.xyz / lightPos_cameraspace.w;
	

	N = (ModelView * vec4(vNormal, 0.0)).xyz;

	texCoord    = vTexCoord;

} 
