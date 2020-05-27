#version 330 core

in  vec3 vPosition;
in  vec3 vColor;
in  vec3 vNormal;
in  vec2 vTexCoord;
in  vec3 vFaceIndecies;

// 传给片元着色器的变量
out vec3 pos;
out vec3 l_pos;
out vec4 normal;

uniform mat4 ModelMatrix;       // 模型矩阵，转换到世界空间
uniform mat4 ViewMatrix;        // 观察矩阵，转换到观察空间
uniform mat4 ProjectionMatrix;  //投影矩阵，转换到裁剪空间

uniform vec4 LightPosition; 
//uniform mat4 shadowProjMatrix;

out vec4 color;
out vec2 texCoord;


uniform vec3 theta;
uniform vec3 translation;

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
    normal = vec4(vNormal, 0);
    
 	gl_Position = vec4(vPosition, 1.0);
    gl_Position = rz * ry * rx * gl_Position;
    vec4 t = rz * ry * rx * vec4(translation, 1.0);

    gl_Position = gl_Position + vec4(translation, 0.0);

    vec4 gp = gl_Position;

    vec4 v1 = ProjectionMatrix * ViewMatrix * ModelMatrix * gl_Position;
    gl_Position = vec4(v1.xyz/v1.w,1.0);

    mat4 ModelView = ViewMatrix * ModelMatrix;
    pos = (ModelView * gp).xyz;
    l_pos = (ModelView * LightPosition).xyz;
    normal = (ModelView * vec4(vNormal,0.0)).xyzw;
}
