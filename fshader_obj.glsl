#version 330 core

in vec4 color;  // * 
in vec2 texCoord;   // *
in vec3 normal; // *
in vec3 faceIndecies;
in vec3 V;  // *
// in vec3 lightPos_new; 


out vec4 fColor;    
out vec4 fNormal;

// uniform int sflag;  //是否是阴影
uniform sampler2D texture;
//uniform vec3 lightPos;      // 需要传入！！！！！！！！*****

void main()
{
    // 阴影着色
	fColor = vec4(0.0471, 0.0392, 0.0392, 0.0); //* texture2D( texture, texCoord );
    
    //fColor = color;
    //fColor = vec4(0, 0, 0, 0 );
    //fNormal = normal;

    // TODO
	// vec3 ambiColor = vec3(0.1, 0.1, 0.1);
	// vec3 diffColor = vec3(0.0667, 0.0471, 0.0471);
	// vec3 specColor = vec3(0.3, 0.3, 0.3); 
	
	// // TODO 
	// vec3 N_norm = normalize(normal);
	// vec3 L_norm = normalize(lightPos - V);
	// vec3 V_norm = normalize(-V);
	// vec3 R_norm = reflect(-L_norm, N_norm);
	
	// // TODO
	// float lambertian = clamp(dot(L_norm, N_norm), 0.0, 1.0);
	// float specular =  clamp(dot(R_norm, V_norm), 0.0, 1.0);
	
	// float shininess = 10.0;
		
	// // TODO 
	// vec4 fragmentColor = vec4(ambiColor + 
	// 			   diffColor * lambertian +
	// 			   specColor * pow(specular, 5.0), 1.0);

    // fColor = color + fragmentColor;
    // if(sflag == 1)
    //     fColor = vec4(0.1, 0.1, 0.1,1);
    // else
    //     fColor = texture2D( texture, texCoord) + fragmentColor;

}

