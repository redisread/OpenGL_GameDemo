#version 330 core

in  vec3 vPosition; // *
in  vec3 vColor;    // *
in  vec3 vNormal;   // *
in  vec2 vTexCoord; // *
in  vec3 vFaceIndecies; //..


out vec4 color;
out vec2 texCoord;
out vec3 normal;
out vec3 V;
// out vec3 lightPos_new;


uniform vec3 theta;
uniform vec3 translation;

uniform mat4 modelViewMatrix;
uniform mat4 shadowMatrix;  
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
// uniform vec3 lightPos;

uniform mat4 change;


// 上面必须
void main()
{
    const float  DegreesToRadians = 3.14159265 / 180.0;

    vec3 c = cos( DegreesToRadians * theta );
    vec3 s = sin( DegreesToRadians * theta );

    mat4 rx = mat4( 1.0, 0.0,  0.0, 0.0,
		    0.0, c.x, -s.x, 0.0,
		    0.0, s.x,  c.x, 0.0,
		    0.0, 0.0,  0.0, 1.0);

    mat4 ry = mat4(   c.y, 0.0, s.y, 0.0,
		      0.0, 1.0, 0.0, 0.0,
		     -s.y, 0.0, c.y, 0.0,
		      0.0, 0.0, 0.0, 1.0 );

    // Workaround for bug in ATI driver
    ry[1][0] = 0.0;
    ry[1][1] = 1.0;

    mat4 rz = mat4( c.z, -s.z, 0.0, 0.0,
		    s.z,  c.z, 0.0, 0.0,
		    0.0,  0.0, 1.0, 0.0,
		    0.0,  0.0, 0.0, 1.0 );

    // Workaround for bug in ATI driver
    rz[2][2] = 1.0;

    color       = vec4(vColor, 0);
    texCoord    = vTexCoord;
    normal = vNormal;
    V = vPosition;
 	gl_Position = vec4(vPosition, 1.0);
    gl_Position = rz * ry * rx * change * modelMatrix * gl_Position;
    vec4 t = rz * ry * rx * vec4(translation, 1.0);
    
    //gl_Position =   (change * gl_Position + vec4(translation, 0.0));
    gl_Position =  shadowMatrix * (gl_Position  + vec4(translation, 0.0));// + vec4(translation, 0.0); //this change
    gl_Position =   vec4(gl_Position.xyz / gl_Position.w, 1.0);
    //gl_Position = vec4(translation, 0.0) * shadowMatrix*(change * gl_Position) ;
    //上面计算模型矩阵

    //进行透视除法
    vec4 v1 = gl_Position;
	//vec4 v3 = modelViewMatrix * v1;
    vec4 v3 = projMatrix * viewMatrix * v1;
    gl_Position = v3;
    //gl_Position = vec4(v3.xyz / v3.w, 1.0);
	//gl_Position = gl_Position + vec4(translation, 0.0);
}
