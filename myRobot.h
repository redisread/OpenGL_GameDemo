#include "Angel.h"
#include "stb_image.h"

// 机器人信息
typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

// program 和 vao Roboot
GLuint program;
GLuint vao[2];

GLuint vTexCoord;//贴图位置
GLuint textures;//纹理地址

// modelView
GLuint ModelViewMatrixID;	//模视矩阵位置
mat4 ModelViewMatrix;		//模式矩阵

// shadow
mat4 shadow;
GLuint shadowMatrixID;
// 控制机器人行走的参数
int runGesture = 1;					// 控制切换行走姿势
float runX = 0, runY = 0, runZ = 0;	// 控制移动的坐标
int trunTogeter = 0;				//控制机器人的身体是否跟着镜头一起转动 
int run3D = 0;						// 控制切换第一视角行走模式
int stepSize = 2;					// 控制机器人每一步的步长
float increase = 1.0;				// 绘制机器人时的位置偏移值
// 用于传入片元着色器，以控制阴影部分不添加漫反射、镜面反射和环境光 
GLuint sflag;

const int NumVertices = 36; // 6个面，每个面包含2个三角面片，每个三角面片包含3个顶点

point4 points[NumVertices];	// 点坐标
color4 colors[NumVertices];	// 点颜色
point4 nomal[NumVertices];	// 法向量
vec2   vt[NumVertices];     //贴图坐标

point4 face_point[6];	//脸的点

// 机器人肢体运动参数
float Leg_angle = 30;
float Arm_angle = 40;

float r_step = 3.1415926/180 * 5;

//current
float current_lLeg_angle = 0;
float current_rLeg_angle = 0;
float current_lArm_angle = 0;
float current_rArm_angle = 0;

// 是否跟随转动
float walk_step = 0.4;





float snow_z = 0;
float snow_x = 6;
float snow_y = 0;

float snow_xAngle = 0;
float snow_yAngle = 0;
float snow_zAngle = 0;

float csnowX = 0;
float csnowY = 0;
float csnowZ = 0;




// 是否第一视角
bool Robot_3D = false;

// 生成一个正方体
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// 生成一个正方形
point4 face_vertices[4] = {
	point4(-0.5,-0.5,0,1.0),
	point4(-0.5,0.5,0,1.0),
	point4(0.5,0.5,0,1.0),
	point4(0.5,-0.5,0,1.0)
};

// RGBA olors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};


// 纹理坐标
vec2 image_vt[4] = {
    vec2(0,1),
    vec2(1,1),
    vec2(1,0),
    vec2(0,0)
};

color4 mcolor = color4(135.0,206.0,235.0,1.0);

// 给机器人的各个部位设置颜色
point4 color_torso = vertex_colors[4];
point4 color_head = vertex_colors[3];
point4 color_upper_arm = vertex_colors[2];
point4 color_lower_arm = vertex_colors[1];
point4 color_upper_leg = vertex_colors[0];
point4 color_lower_leg = vertex_colors[5];




void generate_face()
{
	//0 1 2 0 2 3
	face_point[0] = face_vertices[0];face_point[1] = face_vertices[1];face_point[2] = face_vertices[2];
	face_point[4] = face_vertices[0];face_point[5] = face_vertices[2];face_point[0] = face_vertices[3];
}

//----------------------------------------------------------------------------

class MatrixStack {
	int    _index;
	int    _size;
	mat4*  _matrices;

public:
	MatrixStack(int numMatrices = 100) :_index(0), _size(numMatrices)
	{
		_matrices = new mat4[numMatrices];
	}

	~MatrixStack()
	{
		delete[]_matrices;
	}

	void push(const mat4& m) {
		assert(_index + 1 < _size);
		_matrices[_index++] = m;

	}

	mat4& pop(void) {
		assert(_index - 1 >= 0);
		_index--;
		return _matrices[_index];
	}
};

MatrixStack  mvstack;
mat4         model_view;
GLuint       ModelView, Projection;
GLuint       draw_color;
//----------------------------------------------------------------------------

// 定义机器人各个部位的宽和高
#define TORSO_HEIGHT 4.0
#define TORSO_WIDTH 4.0
#define UPPER_ARM_HEIGHT 1.8
#define LOWER_ARM_HEIGHT 2.3
#define UPPER_ARM_WIDTH  1.2
#define LOWER_ARM_WIDTH  1.1
#define UPPER_LEG_WIDTH  1.3
#define LOWER_LEG_WIDTH  1.5
#define LOWER_LEG_HEIGHT 0.1
#define UPPER_LEG_HEIGHT 5.5

#define HEAD_HEIGHT 1.5
#define HEAD_WIDTH 2



// Set up menu item indices, which we can alos use with the joint angles
enum {
	Torso,
	Head1,
	Head2,
	RightUpperArm,
	RightLowerArm,
	LeftUpperArm,
	LeftLowerArm,
	RightUpperLeg,
	RightLowerLeg,
	LeftUpperLeg,
	LeftLowerLeg,
	NumJointAngles,
	Quit
};

// Joint angles with initial values
GLfloat
theta[NumJointAngles] = {
	0.0,    // Torso
	0.0,    // Head1
	0.0,    // Head2
	0.0,    // RightUpperArm
	0.0,    // RightLowerArm
	0.0,    // LeftUpperArm
	0.0,    // LeftLowerArm
	0.0,  // RightUpperLeg
	0.0,    // RightLowerLeg
	0.0,  // LeftUpperLeg
	0.0     // LeftLowerLeg
};

GLint angle = Head2;

//----------------------------------------------------------------------------

int Index = 0;

void
quad(int a, int b, int c, int d)    //加上计算纹理坐标
{
	vec3 nomalab = vec3(vertices[a].x - vertices[b].x, vertices[a].y - vertices[b].y, vertices[a].z - vertices[b].z);
	vec3 nomalbc = vec3(vertices[b].x - vertices[c].x, vertices[b].y - vertices[c].y, vertices[b].z - vertices[c].z);
	vec3 nomalcd = vec3(vertices[c].x - vertices[d].x, vertices[c].y - vertices[d].y, vertices[c].z - vertices[d].z);
	vec3 nomalabc = cross(nomalab, nomalbc);
	//vec3 nomalbcd = cross(nomalbc, nomalcd);

	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; nomal[Index] = nomalabc;vt[Index]=image_vt[1];  Index++;
	//cout << "V: " << vertices[a] << endl;
	//cout << "nomal: " << vec3(vertices[a].x, vertices[a].y, vertices[a].z) << endl;
	
	
	colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; nomal[Index] = nomalabc;vt[Index]=image_vt[0];  Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; nomal[Index] = nomalabc;vt[Index]=image_vt[3];  Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; nomal[Index] = nomalabc;vt[Index]=image_vt[1];  Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; nomal[Index] = nomalabc;vt[Index]=image_vt[3];  Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; nomal[Index] = nomalabc;vt[Index]=image_vt[2];  Index++;
}

void
colorcube(void)//这里生成单位立方体的六个表面
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}


// 加载纹理图片
void load_texture_STBImage(std::string file_name, GLuint& m_texName)
{
	int width, height, channels = 0;
	unsigned char *pixels = NULL;
	stbi_set_flip_vertically_on_load(true);
	pixels = stbi_load(file_name.c_str(), &width, &height, &channels, 0);

	// 调整行对齐格式
	if(width*channels%4!=0) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLenum format = GL_RGB;
	// 设置通道格式
	switch (channels){
	case 1: format=GL_RED;break;
	case 3: format=GL_RGB;break;
	case 4: format=GL_RGBA;break;
	default: format=GL_RGB;break;
	}

	// 绑定纹理对象
	glBindTexture(GL_TEXTURE_2D, m_texName);

	// 指定纹理的放大，缩小滤波，使用线性方式，即当图片放大的时候插值方式
	// 将图片的rgb数据上传给opengl
	glTexImage2D(
		GL_TEXTURE_2D,	// 指定目标纹理，这个值必须是GL_TEXTURE_2D
		0,				// 执行细节级别，0是最基本的图像级别，n表示第N级贴图细化级别
		format,			// 纹理数据的颜色格式(GPU显存)
		width,			// 宽度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
		height,			// 高度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
		0,				// 指定边框的宽度。必须为0
		format,			// 像素数据的颜色格式(CPU内存)
		GL_UNSIGNED_BYTE,	// 指定像素数据的数据类型
		pixels			// 指定内存中指向图像数据的指针
	);
	
	// 生成多级渐远纹理，多消耗1/3的显存，较小分辨率时获得更好的效果
	 glGenerateMipmap(GL_TEXTURE_2D);

	// 指定插值方法
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// 恢复初始对齐格式
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	// 释放图形内存
	stbi_image_free(pixels);
};

// 更新纹理图片
void update_texture(std::string texture_file)
{
	// 调用stb_image生成纹理 这里1个纹理
	glGenTextures(1, &textures);
	load_texture_STBImage(texture_file, textures);
	// 将生成的纹理传给shader
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
}

void update_texture_face(std::string texture_file)
{
	load_texture_STBImage(texture_file, textures);
	// 将生成的纹理传给shader
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
	
}





// 为机器人添加阴影的函数
void add_robot_shadow()
{
	//将阴影矩阵传入着色器
	glUniformMatrix4fv(shadowMatrixID, 1, GL_TRUE, &shadow[0][0]);
	//设置颜色
	glUniform4fv(draw_color, 1, vertex_colors[0]);
	// 标记其为阴影
	glUniform1i(sflag, 1);
	//绘制
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

// 初始化阴影矩阵
void init_shadowMatrix()
{
	mat4 shadowMatrix = mat4(1.0);
	glUniformMatrix4fv(shadowMatrixID, 1, GL_TRUE, &shadowMatrix[0][0]);
	glUniformMatrix4fv(ModelViewMatrixID, 1, GL_TRUE, &ModelViewMatrix[0][0]);
}
void torso()
{
	init_shadowMatrix();

	

	mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) *
		Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_WIDTH / 2));// 本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);// 父节点矩阵 * 本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_torso);
	
	
	glUniform1i(sflag, 0);	// 标记其不为阴影（下同）
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow(); // 为此部分绘制阴影（下同）


}

void head()
{
	init_shadowMatrix();
	

	mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) *
	Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH));// 本节点局部变换矩阵
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);// 父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_head);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	add_robot_shadow();

}


void r_face()
{
	init_shadowMatrix();
	mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.5 * HEAD_WIDTH) *
	Scale(HEAD_WIDTH, HEAD_HEIGHT, 0.001) * RotateZ(-90));// 本节点局部变换矩阵
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);// 父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_head);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}


void
left_upper_arm()
{
	init_shadowMatrix();



	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH + 0.3, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH + 0.3));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_upper_arm);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();


}

void
left_lower_arm()
{
	init_shadowMatrix();// 初始化阴影矩阵

	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//本节点局部变换矩阵
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_lower_arm);	// 绑定绘制颜色
	glUniform1i(sflag, 0);	 // 标记此不是阴影
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);	// 绘制
	add_robot_shadow();	// 为这部分添加阴影效果

}

void
right_upper_arm()
{
	init_shadowMatrix();



	mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
		Scale(UPPER_ARM_WIDTH + 0.3, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH + 0.3));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_upper_arm);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

}

void
right_lower_arm()
{
	init_shadowMatrix();



	mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_lower_arm);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

}

void
left_upper_leg()
{
	init_shadowMatrix();



	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH + 0.3, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH + 0.3));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_upper_leg);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

}

void
left_lower_leg()
{
	init_shadowMatrix();



	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_lower_leg);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

}

void
right_upper_leg()
{
	init_shadowMatrix();



	mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
		Scale(UPPER_LEG_WIDTH + 0.3, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH + 0.3));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_upper_leg);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();

}

void
right_lower_leg()
{
	init_shadowMatrix();



	mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));//本节点局部变换矩阵

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view * instance);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_lower_leg);
	glUniform1i(sflag, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	add_robot_shadow();


}

#define NumFace  6
class robot_face
{
	public:
		point4 points[NumFace];	// 点坐标
		color4 colors[NumFace];	// 点颜色
		point4 nomal[NumFace];	// 法向量
		vec2   vt[NumFace];     //贴图坐标

		GLuint program;	// 着色器
		GLuint vao;		// 
		GLuint buffer;	//vbo
	
	robot_face(){
					// 生成一个正方形
					point4 face_vertices[4] = {
						point4(-0.5,-0.5,0,1.0),
						point4(-0.5,0.5,0,1.0),
						point4(0.5,0.5,0,1.0),
						point4(0.5,-0.5,0,1.0)
					};
					// 纹理坐标
					vec2 image_vt[4] = {
						vec2(0,1),
						vec2(1,1),
						vec2(1,0),
						vec2(0,0)
					};
	
	// 计算法向
	vec3 nomalab = vec3(face_vertices[0].x - face_vertices[1].x, face_vertices[0].y - face_vertices[1].y, face_vertices[0].z - face_vertices[1].z);
	vec3 nomalbc = vec3(face_vertices[1].x - face_vertices[2].x, face_vertices[1].y - face_vertices[2].y, face_vertices[1].z - face_vertices[2].z);
	vec3 nomalcd = vec3(face_vertices[2].x - face_vertices[3].x, face_vertices[2].y - face_vertices[3].y, face_vertices[2].z - face_vertices[3].z);
	vec3 nomalabc = cross(nomalab, nomalbc);

	int Index = 0;
	points[Index] = face_vertices[0];colors[Index] = vertex_colors[6];nomal[Index] = nomalabc;vt[Index] = image_vt[Index];
	Index++;
	points[Index] = face_vertices[Index];colors[Index] = vertex_colors[6];nomal[Index] = nomalabc;vt[Index] = image_vt[Index];
	Index++;
	points[Index] = face_vertices[Index];colors[Index] = vertex_colors[6];nomal[Index] = nomalabc;vt[Index] = image_vt[Index];
	
	Index++;
	points[Index] = face_vertices[0];colors[Index] = vertex_colors[6];nomal[Index] = nomalabc;vt[Index] = image_vt[0];
	Index++;
	points[Index] = face_vertices[2];colors[Index] = vertex_colors[6];nomal[Index] = nomalabc;vt[Index] = image_vt[2];
	Index++;
	points[Index] = face_vertices[3];colors[Index] = vertex_colors[6];nomal[Index] = nomalabc;vt[Index] = image_vt[3];	
	}

	void init()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(nomal) + sizeof(vt) + sizeof(face_point),
		NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors),
		colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(nomal), nomal);
	glBufferSubData(GL_ARRAY_BUFFER,sizeof(points) + sizeof(colors) + sizeof(nomal),sizeof(vt),vt);
	program = InitShader("vshader_robot.glsl", "fshader_robot.glsl");




	};


};

void doWalk()
{
		current_lArm_angle += r_step;
		theta[LeftUpperArm] = Arm_angle* sin(current_lArm_angle);
		//theta[LeftLowerArm] = 0;
		current_rArm_angle -= r_step;
		theta[RightUpperArm] = Arm_angle * sin(current_rArm_angle);
		//theta[RightLowerArm] = 0;
		current_rLeg_angle += r_step;
		theta[RightUpperLeg] = Leg_angle * sin(current_rLeg_angle);
		//theta[RightLowerLeg] = 0;
		current_lLeg_angle -= r_step;
		theta[LeftUpperLeg] = Leg_angle * sin(current_lLeg_angle);
}
// void face()
// {

// 	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);//父节点矩阵*本节点局部变换矩阵
// 	glUniform4fv(draw_color, 1, color_lower_leg);
// 	glUniform1i(sflag, 0);
// 	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

// }

// void generate_face()
// {
// 	//0 1 2 0 2 3
// 	face_point[0] = face_vertices[0];face_point[1] = face_vertices[1];face_point[2] = face_vertices[2];
// 	face_point[4] = face_vertices[0];face_point[5] = face_vertices[2];face_point[0] = face_vertices[3];
// }