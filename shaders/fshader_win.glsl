#version 330 core

in vec4 color;
in vec2 texCoord;

in vec3 faceIndecies;

in vec3 pos;
in vec3 l_pos;
in vec4 normal;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

// 输出颜色
out vec4 fColor;
out vec4 fNormal;

uniform sampler2D texture;

void main()
{
    // 计算四个归一化的向量 N,V,L,R
    vec3 N = normalize(normal.xyz);                  //归一化的法向量 
    vec3 V = normalize(vec3(0,0,0) - pos);   //视线方向
    vec3 L = normalize(l_pos - pos);           //入射向量
    vec3 R = reflect(-L,N);                    //计算反射向量

    // 环境光分量I_a
    vec4 I_a = AmbientProduct;

    // 计算漫反射系数alpha和漫反射分量I_d
    float alpha = 0.0;
    alpha = max(dot(L,N),0);
    vec4 I_d = alpha*DiffuseProduct;

    // 计算高光系数beta和镜面反射分量I_s
    float beta = 0.0;
    beta = max(pow(dot(R,V),Shininess),0);
    vec4 I_s = beta * SpecularProduct;

    // 注意如果光源在背面则去除高光
    if( dot(L, N) < 0.0 ) {
	    I_s = vec4(0.0745, 0.051, 0.051, 1.0);
    }

	// fColor = vec4(1, 0, 0, 0 ) * texture2D( texture, texCoord );
    fColor = texture2D( texture, texCoord );// + I_a + I_d + I_s;
    // fColor = color;
    // fColor = vec4(0, 0, 0, 0 );
    fNormal = normal;
}

