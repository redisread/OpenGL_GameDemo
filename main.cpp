#include "Angel.h"
#include "mesh.h"
#include "Mesh_Painter.h"
#include "myRobot.h"
#include <string>
#include <algorithm>
#include "stb_image.h"
#include "AllFiles.h"
#include "camera.h"
#include <windows.h>

// #include <mmsystem.h>
// #include <conio.h>
 #pragma comment(lib, "Winmm.lib")


// #pragma comment(lib, "glew32.lib")

/********************************************期末大作业--绿巨人环游世界*********************************************

场景概要：
	1. 天空盒进行场景渲染
	2. 层级建模机器人
	3. 相机的视角切换
	4. 雪人的帧运动
	5. 绿巨人换脸、换肤

文件模块及功能：
	1. myRobot.h文件包含机器人所需要的函数和数据
	2. AllFiles.h文件里面包含许多文件路径的初始化信息及函数
	3. camera.h文件里面定义了相机的相关操作及信息
	4. mesh.h定义每个需要绘制的obj模型
	5. Mesh_painter.h定义了一个可以绘制mesh类的一个画图类
	6. 使用三套着色器：
		(1) 机器人着色器: vshaer_robot.glsl fshaser_robot.glsl
		(2) 场景绘制着色器: vshader_texture.glsl fshader_texture.glsl
		(3) 阴影绘制着色器: vshader_shadow.glsl fshader_shader.glsl

功能及实现：
	1. 鼠标
		按住左键更改光源位置
	2. 键盘交互
		‘w’：视角向上  
		‘s‘：视角向下
		‘a’：镜头左转 
		‘d’: 镜头右转
		‘z’:视线距离变小；‘Z’视线距离变大
		‘x’:透视距离变远；’X‘透视距离变近
		GLUT_UP_KEY：机器人前进 
		GLUT_DOWN_KEY：机器人后退
		GLUT_LEFT_KEY：机器人左转
		GLUT_RIGHT_KEY机：器人右转
		‘Y’:开启跟随模式;’I’:进入上帝视角；‘u’机器人跳舞，‘*’相机到天空盒外部
		‘f’:换脸
		‘F’:换肤
		’t’场景切换
		按’m’或者’j’观看雪人的运动
		‘*’：进入天空盒

*******************************************-----------end-------------***********************************************/

// 窗口大小
struct Window
{
	int width = 800;
	int height = 800;
};
Window window;
//热气球
My_Mesh* balloon;
//热气球参数
float balloon_angle = 0;
// 光源
vec3 lightPos(3, 8, 3);	//坐标位置
GLuint lightPosID;			//着色器位置

//初始化，设置OpenGL全局设置
void init()
{
	// 生成机器人的所有点
	colorcube();
	// Create a vertex array object
	glGenVertexArrays(1, &vao[0]);
	glBindVertexArray(vao[0]);
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(nomal) + sizeof(vt) + sizeof(face_point),
		NULL, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors),
		colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(nomal), nomal);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(nomal),sizeof(vt),vt);glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

	program = InitShader("vshader_robot.glsl", "fshader_robot.glsl");

	ModelViewMatrixID = glGetUniformLocation(program, "modelViewMatrix");
	shadowMatrixID = glGetUniformLocation(program, "shadowMatrix");

	glUseProgram(program);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	draw_color = glGetUniformLocation(program, "draw_color");
	lightPosID = glGetUniformLocation(program, "lightPos");
	sflag = glGetUniformLocation(program, "sflag");

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	// 机器人纹理坐标
	// 初始化贴图位置,传入纹理坐标
	vTexCoord = glGetAttribLocation(program,"vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)+ sizeof(nomal)));
	// 更新绑定纹理图
	update_texture(skin_file[face_Index]);

	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);

	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	//glClearColor(0.25f,0.25f,0.25f,1.0);
    glClearColor(1.0, 1.0, 1.0, 1.0);	
}

/*--------------------调用my_Robot.h的函数进行绘制机器人------------------------*/
void show_robot()
{
	glUseProgram(program);
	glBindVertexArray(vao[0]);

	// 画的时候激活
 	update_texture("texture/" + robotName[name_index]+"/body.png");
	glActiveTexture(GL_TEXTURE0);
 	glBindTexture(GL_TEXTURE_2D, textures);// 该语句必须，否则将只使用同一个纹理进行绘制

	
	model_view = Translate(runX * sin(pAngle*DegreesToRadians) + runZ, 0, runX * cos(pAngle*DegreesToRadians)) 
		* Translate(0.0, increase + UPPER_LEG_HEIGHT + LOWER_LEG_HEIGHT, 0.0)*RotateY(theta[Torso]);//躯干变换矩阵
	torso();//躯干绘制

	mvstack.push(model_view);//保存躯干变换矩阵
	model_view *= (Translate(0.0, TORSO_HEIGHT, 0.0) *RotateY(theta[Head1]));
	head();//头部绘制

	// 脸部绘制 新生成一个极扁的正方形 贴在机器人前面
	update_texture_face(current_face);
	glActiveTexture(GL_TEXTURE0);
 	glBindTexture(GL_TEXTURE_2D, textures);// 该语句必须，否则将只使用同一个纹理进行绘制
	r_face();

	// 重新使用当前的纹理进行绘制
	update_texture_face(current_skin);
	glActiveTexture(GL_TEXTURE0);
 	glBindTexture(GL_TEXTURE_2D, textures);


	model_view = mvstack.pop();//恢复躯干变换矩阵

	glBindVertexArray(vao[0]);


	mvstack.push(model_view); //保存躯干变换矩阵
	//乘以上臂的变换矩阵(注意此处最后乘了RotateX，代表改变theta[LeftUpperArm]会改变上臂的旋转角度，以X轴为旋转轴)
	model_view *= (Translate(0.5*(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.9 * TORSO_HEIGHT, 0.0) *RotateX(theta[LeftUpperArm])*RotateZ(180));
	
	update_texture("texture/" + robotName[name_index]+"/a.png");
	left_upper_arm();//左上臂绘制
	//乘以下臂的变换矩阵(注意此处最后乘了RotateX，意义同上)
	model_view *= (Translate(0.0, UPPER_ARM_HEIGHT, 0.0) *RotateX(theta[LeftLowerArm]));
	update_texture("texture/" + robotName[name_index]+"/arm.png");
	left_lower_arm();//左下臂绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(-0.5*(TORSO_WIDTH + UPPER_ARM_WIDTH), 0.9 * TORSO_HEIGHT, 0.0) *RotateX(theta[RightUpperArm])*RotateZ(180));
	update_texture("texture/" + robotName[name_index]+"/a.png");
	right_upper_arm();//右上臂绘制
	model_view *= (Translate(0.0, UPPER_ARM_HEIGHT, 0.0) *RotateX(theta[RightLowerArm]));
	update_texture("texture/" + robotName[name_index]+"/arm.png");
	right_lower_arm();//右下臂绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(TORSO_HEIGHT / 6, 0, 0.0) *RotateX(theta[LeftUpperLeg])*RotateZ(180));
	update_texture("texture/" + robotName[name_index]+"/leg.png");
	left_upper_leg();//左上腿绘制
	model_view *= (Translate(0.0, UPPER_LEG_HEIGHT, 0.0) *RotateX(theta[LeftLowerLeg]));
	left_lower_leg();//左下腿绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

	mvstack.push(model_view); //保存躯干变换矩阵
	model_view *= (Translate(-TORSO_HEIGHT / 6, 0, 0.0) *RotateX(theta[RightUpperLeg])*RotateZ(180));
	update_texture("texture/" + robotName[name_index]+"/leg.png");
	right_upper_leg();//右上腿绘制
	model_view *= (Translate(0.0, UPPER_LEG_HEIGHT, 0.0) *RotateX(theta[RightLowerLeg]));
	right_lower_leg();//右下腿绘制
	model_view = mvstack.pop();//恢复躯干变换矩阵

}

// 重新设置窗口
void reshape(GLsizei w, GLsizei h)
{
	window.width = w;
	window.height = h;
	glViewport(0, 0, w, h);
	GLfloat left = -10.0, right = 10.0;
	GLfloat bottom = -5.0, top = 15.0;
	GLfloat zNear = -10.0, zFar = 10.0;
	GLfloat asp = GLfloat(w) / h;

	if (asp > 1.0) {
		left *= asp;
		right *= asp;
	}

	else {
		bottom /= asp;
		top /= asp;
	}
	
	mat4 projection = Ortho(left, right, bottom, top, zNear, zFar);
//	projection = myCamera::perspective(fov,float(w)/h,zNear,zFar);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	// 重新设置透视投影的宽高比
	aspect = asp;
	model_view = mat4(1.0);   // An Identity matrix

}



float PI = 3.14;
//视角控制
float look_x = 0,
look_y = 0,
look_z = 0,
look_x_temp = 0,
look_z_temp = 0;
float screenrate_x, screenrate_y;//鼠标屏幕坐标相对于中心点移动的比例
float r = 20;
//相机位置
float cpos_x = 0,
cpos_y = 35,
cpos_z = 10;
 
int centerpoint_x = window.width/ 2,
centerpoint_y = window.height / 2;
mat4 glLookAt()
	{
		
		return LookAt(vec4(cpos_x,cpos_y, cpos_z,1),vec4(cpos_x + look_x, cpos_y + look_y, cpos_z - look_z,1),
				vec4(0,1,0,0));
	};



// 调用mesh_painter类进行绘制，全部操作在该类中
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 每次获取eye
	float x = myCamera::eye.x;float y = myCamera::eye.y;float z = myCamera::eye.z;
	// at 的xyz参数
	float atx = myCamera::at.x;
	float aty = myCamera::at.y;		//this change
	float atz = myCamera::at.z;

	// 如果第一视角行走，eye需跟着机器人走，eye与at保持距离不变
	if (run3D == 1) {
		x += runX*sin(pAngle*DegreesToRadians);// + runZ * cos(pAngle*DegreesToRadians);
		y += 0;
		z += runX*cos(pAngle*DegreesToRadians);// + runZ * sin(pAngle*DegreesToRadians);

		atx += runX*sin(pAngle*DegreesToRadians);// + runZ * cos(pAngle*DegreesToRadians);
		aty += 0;
		atz += runX*cos(pAngle*DegreesToRadians);// + runZ * sin(pAngle*DegreesToRadians);
		
	}
	myCamera::at = vec4(atx,aty,atz,1);
	if(One_D)
	{
		// eye = (snow_x, snow_y, snow_z)平移
		// // csnowX = snow_x;csnowY=snow_y;csnowZ=snow_z;
		// cpos_x = snow_x;cpos_y = snow_y;cpos_z = snow_z;
		// at = (snow_x, snow_y, snow_z)
		cpos_x -= runX*sin(pAngle*DegreesToRadians) + runZ * cos(pAngle*DegreesToRadians);// + r*cos(snow_yAngle*DegreesToRadians);
		//look_y
		cpos_z -= runX*cos(pAngle*DegreesToRadians)+ runZ * sin(pAngle*DegreesToRadians);// + r*sin(snow_yAngle*DegreesToRadians);

	}
	//if(view_3D==One_D)
	//{
		// cpos_x =  x;
		// cpos_z =  z;
		//cpos_y = rad * sin(tAngle);

	//}
	// 第三人称相机坐标
	//myCamera::eye = vec4(x,y,z,1);

	eye = vec4(x, y, z, 1);
	at = vec4(atx, aty, atz, 1);
	up = vec4(0, 1, 0, 0);
	// at = vec4(0,0,0,1);
	// 光源位置
	float lx = lightPos[0];
	float ly = lightPos[1];
	float lz = lightPos[2];

	//函数方程为 Ax+By+Cz+D=0 ,所以n表示阴影所需要投影的平面
	GLfloat n[] = { 0,1,0,-1 };
	shadow = mat4(
		vec4(-ly*n[1] - lz*n[2], lx*n[1], lx*n[2], lx*n[3]),
		vec4(ly*n[0], -lx*n[0] - lz*n[2], ly*n[2], ly*n[3]),
		vec4(lz*n[0], lz*n[1], -lx*n[0] - ly*n[1], lx*n[3]),
		vec4(0, 0, 0, -lx*n[0] - ly*n[1] - lz*n[2]));

	myCamera::modelMatrix = mat4(1.0);
	if(view_3D == One_D)
		myCamera::viewMatrix = glLookAt();//第一人称
	if(view_3D == Three_D)
		myCamera::viewMatrix = myCamera::lookAt(eye, at, up);//设置照相机

	myCamera::projMatrix = myCamera::perspective(fov, aspect, zN, 500);//设置透视投影，距离要足够长
	  
	glUniformMatrix4fv(Projection, 1, GL_TRUE, &myCamera::projMatrix[0][0]);	//传进透视投影矩阵

	glUniform3fv(lightPosID, 1, &lightPos[0]);	// 将光源信息传入顶点着色器
	
	mp_->setCameraMatrix(myCamera::modelMatrix, myCamera::viewMatrix, myCamera::projMatrix);
	mp_->setShadowMateix(shadow);
	mp_->setLightPos(lightPos);
	mp_->draw_meshes();

	myCamera::modelMatrix =  mat4(1.0);//Scale(1.5,1.5,1.5) * Translate(0,0,0);
	ModelViewMatrix = myCamera::viewMatrix * myCamera::modelMatrix;
	glUseProgram(program);

	
	show_robot();	// 绘制机器人

	
	glutSwapBuffers();
};

// 菜单函数
void menu(int option)
{
	if (option == Quit) {
		exit(EXIT_SUCCESS);
	}

	angle = option;
}




//不按鼠标移动
void onMouseMovePassive(int screen_x,int screen_y) {
	//相对于屏幕中心点的偏移量
	float offsetx = screen_x - centerpoint_x;
	float offsety = screen_y - centerpoint_y;
    //偏移量转化为角度
	screenrate_x = offsetx / centerpoint_x *PI;//水平方向
	screenrate_y = offsety / centerpoint_y *PI/2;//竖直方向
	//水平方向
	look_x_temp = r * sin(screenrate_x);
	look_z_temp = r * cos(screenrate_x);//最后使用时要和相机坐标相加/减
	//竖直方向

	look_y = r * sin(-screenrate_y);
	float r_castlenght = abs(r * cos(screenrate_y));//投影在xz面的长度
	//根据长度计算真正的look_x,look_z
	look_x = r_castlenght * look_x_temp / r;
	look_z = r_castlenght * look_z_temp / r;

	std::cout<<cpos_x<<" "<<cpos_y<<" "<<cpos_z<<"camera eye"<<std::endl;
	std::cout<<look_x<<" "<<look_y<<" "<<look_z<<"at "<<std::endl;

	std::cout<<"steve"<<" X "<<runX<<" "<<runY<<" "<<runZ<<std::endl;
}


//鼠标函数
void mouse(int button, int state, int x, int y)
{
	//检测鼠标是否按下的状态，并记录下鼠标坐标
	mouseX = x;
	mouseY = y;
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftDown = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseRightDown = true;
		}
		else if (state == GLUT_UP)
			mouseRightDown = false;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		theta[angle] += 5.0;
		//cout << theta[angle] << endl;
		if (theta[angle] > 360.0) { theta[angle] -= 360.0; }
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		theta[angle] -= 5.0;
		//cout << theta[angle] << endl;
		if (theta[angle] < 0.0) { theta[angle] += 360.0; }
	}

	glutPostRedisplay();
}

void mouseMotionCB(int x, int y)
{
	std::cout<<"lightPos: X:"<<lightPos[0]<<" Y:"<<lightPos[1]<<" Z:"<<lightPos[2]<<std::endl;
	if (mouseLeftDown)
	{
		cameraAngleY += (x - mouseX);
		cameraAngleX += (y - mouseY);
		if (y>mouseY)
		{
			lightPos += vec3(0, (mouseY - y) / 50, 0);

		}
		else if (y < mouseY)
		{
			lightPos += vec3(0, (mouseY - y) / 50, 0);

		}
		if (x>mouseX)
		{
			lightPos += vec3((x - mouseX) / 50, 0, 0);

		}
		else if (x<mouseX)
		{
			lightPos += vec3((x - mouseX) / 50, 0, 0);

		}
		mouseX = x;
		mouseY = y;
	}
	if (mouseRightDown)
	{
		cameraDistance += (y - mouseY) * 0.2f;
		mouseY = y;
	}

	if (lightPos[1] <= 1)
		lightPos[1] = 1;

	glutPostRedisplay();
}

// 滚轮响应函数
void mouseWheel(int button, int dir, int x, int y)
{
    // 实现滚轮更改光源的远近
	r += dir;
	glutPostRedisplay();
}

// 空闲回调
void idle(void)
{
	balloon_angle += 5.0;
		float ba,by,bz;
		balloon->get_theta(ba,by,bz);
		balloon->set_theta(ba,by,5*cos(balloon_angle*DegreesToRadians));
	glutPostRedisplay();
}


//----------------------------------------------------------------------------
// 控制机器人姿势的函数
void robotChangeGesture() {
	if (runGesture == 1) {
		theta[LeftUpperArm] = 40;
		theta[LeftLowerArm] = 115;
		theta[RightUpperArm] = 340;
		theta[RightLowerArm] = 105;
		theta[RightUpperLeg] = 20;
		theta[RightLowerLeg] = 265;
		theta[LeftUpperLeg] = 340;
		theta[LeftLowerLeg] = 0.0;
		runGesture = 0;
	}
	else {
		theta[LeftUpperArm] = 340;
		theta[LeftLowerArm] = 105;
		theta[RightUpperArm] = 40;
		theta[RightLowerArm] = 115;
		theta[RightUpperLeg] = 340;
		theta[RightLowerLeg] = 0;
		theta[LeftUpperLeg] = 20;
		theta[LeftLowerLeg] = 265;
		runGesture = 1;
	}
}

void TimeCall(int value)
{
	;
}
void special_key(int key,int x,int y)
{
	switch (key)
	{
	// 控制机器人移动
	case GLUT_KEY_LEFT:	//theta[Torso] += 1.0;// 左移
		if(run3D)
		{pAngle += 1.0; //跟随转动
		theta[Torso] = (int)pAngle % 360 + 180.0 + 90.0;
		}
		else{
			runZ -= stepSize*0.1;	// 走出一步，步长为stepSize
			theta[Torso] = 180.0 + 90.0;
		}
		doWalk();
		break;
	case GLUT_KEY_RIGHT:	// 右移
		if(run3D)
		{theta[Torso] += -1.0;
		pAngle -= 1.0;
		theta[Torso] = (int)pAngle % 360 + 180.0 - 90.0;}
		else{
			runZ += stepSize*0.1;
			theta[Torso] = 180.0 - 90.0;
		}
		doWalk();
		break;
	case GLUT_KEY_UP:	// 前进
		
		
		runX -= walk_step;	// 走出一步，步长为stepSize
		//at -= walk_step;
		if(runX < -63)
			{
				runX = 0;
				mp_->update_texture();	
				setskyFile((++face_Index) % skyBoxFile.size()/6);
				mp_->update_vertex_buffer();
			}
		theta[Torso] = (int)pAngle % 360 + 180.0;
		doWalk();
	//	tAngle -= 0.1;
		break;
	case GLUT_KEY_DOWN:	// 后退
		runX += walk_step;
		theta[Torso] = (int)pAngle % 360;
		doWalk();	
		break;
	
	default:
		break;
	}
	// std::cout<<"run_x:"<<runX<<"run_y:"<<runY<<"run_z:"<<runZ<<std::endl;
	// std::cout<<"x:"<<x<<" y:"<<y<<std::endl;
	setCameraEye();
	std::cout << "tangle:"<<tAngle<<" pAngle:"<<pAngle<<" rad:"<<rad<<std::endl;
	std::cout<<"eye:  "<<"x:"<<myCamera::eye.x<<" y:"<<myCamera::eye.y<<" z:"<<myCamera::eye.z<<std::endl;	
	std::cout<<"at:  "<<"x"<<myCamera::at.x<<" y:"<<myCamera::at.y<<" z:"<<myCamera::at.z<<std::endl;
	glutPostRedisplay();
}


void keyboard(unsigned char key, int mouse_x, int mouse_y)
{
	//std::cout<<key<<std::endl;
	switch (key)
	{
	case 'G':	//换模型皮肤
		name_index++;
		if(name_index >= name_size)	name_index-=name_size;
		break;
	case '1':
		view_3D = One_D;
		break;
	case '3':
		view_3D = Three_D;
		break;
	// case '.':
	// 	myCamera::eye = camGod;
	// 	cpos_x = 0;
	// 	cpos_y = 600;
	// 	cpos_z = 0;
	// 	break;
	case '-':
		cpos_x = runX;
		cpos_y = runY;
		cpos_z = runZ;
		break;
	case 'c':	//前进
		cpos_x += look_x_temp * step;
		cpos_z -= look_z_temp * step;
		break;
	case 'v':	//后退
		cpos_x -= look_x_temp * step;
		cpos_z += look_z_temp * step;
		break;
	case 'C':	//左移
		cpos_x += -look_z_temp * step;
		cpos_z -= look_x_temp * step;
		break;
	case 'V':	//右移
		cpos_x += look_z_temp * step;
		cpos_z -= -look_x_temp * step;
		break;
	case 'b':
		
		//theta[Torso] = (int)pAngle % 360 + 180.0;
		cpos_x = runX-6;
		//runX -= walk_step;
		cpos_y+=3;
		cpos_z = runZ;
		look_x = runX+3;
		look_y = cpos_y;
		look_z = runZ;
		//r*sin(theta[Torso]);
		//look_z += runX*cos(pAngle*DegreesToRadians);
		//doWalk();
		//look_y = cpos_y;
		//theta[Torso] =  pAngle; 
		break;
	case 'B':
		runX+=1;theta[Torso]=90; break;
	case '0':
		cpos_x = 0;cpos_y=100;cpos_z=0;
		break;
	// 控制相机变换
	case 'd':
		pAngle += 5;	// 镜头转（下同）
		if (trunTogeter == 1) {
			theta[Torso] += 5;	// 机器人随镜头一起转（下同）
			robotChangeGesture();	// 换姿势（下同）
		}
		break;
	case 'a':
		if (trunTogeter == 1) {
			theta[Torso] -= 5;
			robotChangeGesture();
		}
		pAngle -= 5;
		break;
	case 'w': tAngle += 5;
		break;
	case 's': tAngle -= 5;//if(tAngle<0) tAngle=0;
		break;
	case 'z': rad += 1;
		break;
	case 'Z': rad -= 1;
		break;
	case 'x': fov += 5;	//视距
		break;
	case 'X': fov -= 5;
		break;

	case 'f':	//换脸
		current_face = face_file[(++face_Index) % face_file.size()];
		break;
	case 'F': //换肤cre
		current_skin = skin_file[(++skin_Index) % skin_file.size()];
		break;

	case '+':
		balloon_angle += 5.0;
		float ba,by,bz;
		balloon->get_theta(ba,by,bz);
		balloon->set_theta(ba,by,5*cos(balloon_angle*DegreesToRadians));
		break;

	// 控制机器人动作
	case 'k'://躯干转身
		theta[Torso] += 5.0;
		theta[RightUpperArm] += 5.0;
		theta[LeftUpperArm] -= 5.0;
		theta[LeftUpperLeg] -= 5.0;
		theta[RightUpperLeg] +=0.5;
		if (theta[angle] > 360.0)
            theta[angle] -= 360.0;
		show_robot();
		pAngle+=5.0;
		break;
	case '4':	// 左转
		robotChangeGesture();	// 摆姿势
		// 第一视角移动方式
		if (run3D == 1) {
			runX -= stepSize;	// 走出一步，步长为stepSize
			pAngle += 5;
			theta[Torso] = (int)pAngle % 360 + 180.0 + 45.0;// 机器人身体转一下角度
		    // pAngle += 5;
		}

		// 第三视角移动方式
		else {
			runZ -= stepSize;	// 走出一步，步长为stepSize
			theta[Torso] = 180.0 + 90.0;// 机器人身体转一下角度
		}
		break;
		
		//theta[LeftLowerLeg] = 0;
		
	case '6':
	robotChangeGesture();
		if (run3D == 1) {
			runX -= stepSize;
			theta[Torso] = (int)pAngle % 360 + 180.0 - 45.0;
			pAngle -= 5;
		}
		else {
			runZ += stepSize;
			theta[Torso] = 180.0 - 90.0;
		}
		break;

		
	case '8':
		robotChangeGesture();
		runX -= stepSize;
		theta[Torso] = (int)pAngle % 360 + 180.0;
		break;
		
	
	case '2':
		robotChangeGesture();
		runX += stepSize;
		if (run3D == 1) {
			theta[Torso] = (int)pAngle % 360 + 180;
		}
		else {
			theta[Torso] = (int)pAngle % 360;
		}
		break;
	case '7':
		theta[Torso] += 1.0;
		doWalk();
		break;	
	case '9':
		theta[Torso] -= 1.0;
		doWalk();
		break;
	
	// case '1':
	// 	snow_boy->set_translate(10,60,-6);
	// 	mp_->update_snow_beterx_buffer(snow_boy);
	// 	break;
	// case '3':
	// 	snow_boy->set_translate(10,0,-6);
	// 	mp_->update_snow_beterx_buffer(snow_boy);
	// 	break;
	case '/':
		if(Robot_3D == true)
			{
			Robot_3D = false;
			runZ = 0; 
			runX = 0;
			rad = 20.0;
			tAngle = 25.0;
			pAngle = 0.0;
			}
		else if(Robot_3D == false)
		{
			Robot_3D =true;
			runZ = 0;
			runX = 0;
			rad = 0;
			tAngle = 0;
			myCamera::eye = vec4(runX+0.1,runY,runZ-5,1);
			setCameraAt(0,myCamera::eye.y +10,-20);
			theta[Torso] = (int)pAngle % 360 + 180.0;
		}

		
		break;
	case '*':
		//myCamera::eye = vec4(-460,556,460,1);
		tAngle = 40;
		pAngle = 45;
		rad = 850;
		glutTimerFunc(20,TimerCal_Into_SkyBox,int(rad));
		break;
		
		
	case 'u':
		theta[RightUpperArm] += 5.0;
		theta[LeftUpperArm]  -= 5.0;
		theta[LeftUpperLeg]  -= 5.0;
		theta[RightUpperLeg] += 0.5;
		break;
	case 'U':
		glutTimerFunc(20, TimerCal, 100);
		break;
	case 'j':
		glutTimerFunc(20,TimerCal_snow,50);
		break;
	case 'L':
		glutTimerFunc(20,TimerCal_Camera_Roate,360);
		break;
	// 切换控制机器人是否和镜头一起转
	case 'p':
		if (trunTogeter == 0) {
			trunTogeter = 1;
		}
		else if (trunTogeter == 1) {
			trunTogeter = 0;
		}
		break;
	case 'y':	// 切换机器人第一视角和第三视角走路
		if (run3D == 0) {
			// run3D = 1;
			// runZ = 0;

			// //
			// rad = 10.0;
			// tAngle = 20.0;
			// camera_height = 1;
			// //setCameraAt(runX,myCamera::eye.y,myCamera::eye.z);
			// //
			// theta[Torso] = (int)pAngle % 360 + 180.0;
			run3D = 1;
			runZ = 0;
			theta[Torso] = (int)pAngle % 360 + 180.0;

		}
		else if (run3D == 1) {
			run3D = 0;
			runZ = 0; 
			runX = 0;
			rad = 43.0;
			tAngle = .0;
			pAngle = 0.0;
		}
		break;

		// 重置

		// 机器人姿势重置
	case ' ':
		theta[Torso] = 0;
		theta[Head1] = 0;
		theta[Head2] = 0;
		theta[LeftUpperArm] = 0;
		theta[LeftLowerArm] = 0;
		theta[RightUpperArm] = 0;
		theta[RightLowerArm] = 0;
		theta[RightUpperLeg] = 0;
		theta[RightLowerLeg] = 0;
		theta[LeftUpperLeg] = 0;
		theta[LeftLowerLeg] = 0;
		break;

		// 所有信息重置
	case 'r':
		// camera
		rad = 50.0;
		tAngle = 25.0;
		pAngle = 0.0;

		// perspective
		fov = 80.0;
		aspect = 1.0;
		zN = 0.1;
		zF = 100.0;
		theta[Torso] = 0.0;
		runX = 0;
		runZ = 0;
		lightPos = vec3(5, 5, 10);

		// 机器人姿势
		theta[Torso] = 0;
		theta[Head1] = 0;
		theta[Head2] = 0;
		theta[LeftUpperArm] = 0;
		theta[LeftLowerArm] = 0;
		theta[RightUpperArm] = 0;
		theta[RightLowerArm] = 0;
		theta[RightUpperLeg] = 0;
		theta[RightLowerLeg] = 0;
		theta[LeftUpperLeg] = 0;
		theta[LeftLowerLeg] = 0;

		// control
		trunTogeter = 0;
		run3D = 0;
		break;
	case 't':	//换场景
		mp_->update_texture();	
		setskyFile((++face_Index) % skyBoxFile.size()/6);
		break;
	case 'm':	//雪人前进运动
		snow_boy->load_obj(snow_f[(snow_i++) % snow_num]);
		
		mp_->update_snow_beterx_buffer(snow_boy);
		snow_boy->get_theta(snow_xAngle,snow_yAngle,snow_zAngle);	
		snow_boy->set_theta(snow_xAngle,snow_yAngle,snow_zAngle);
		
		
		snow_boy->get_translate(snow_x,snow_y,snow_z);
		snow_z -= 1.5 * cos(snow_yAngle*DegreesToRadians);
		snow_x += 1.5 * sin(snow_yAngle*DegreesToRadians);
		snow_boy->set_translate(snow_x,snow_y,snow_z);
		cpos_x -= 1.5 * sin(snow_yAngle*DegreesToRadians);
		cpos_z += 1.5 * cos(snow_yAngle*DegreesToRadians);
		break;
	case 'M': //雪人后退
		snow_boy->load_obj(snow_f[(snow_i++) % snow_num]);
		
		mp_->update_snow_beterx_buffer(snow_boy);
		snow_boy->set_theta(snow_xAngle,snow_yAngle+180,snow_zAngle);
		//snow_yAngle+=180;
		snow_boy->get_translate(snow_x,snow_y,snow_z);
		snow_z += 1.5 * cos(snow_yAngle*DegreesToRadians);
		snow_x -= 1.5 * sin(snow_yAngle*DegreesToRadians);
		snow_boy->set_translate(snow_x,snow_y,snow_z);
		break;

	case ',':
		snow_yAngle -= 5.0;
		
		snow_boy->set_theta(snow_xAngle,snow_yAngle,snow_zAngle);
		snow_boy->load_obj(snow_f[(snow_i++) % snow_num]);
		
		mp_->update_snow_beterx_buffer(snow_boy);
		break;
	case '.':
		snow_yAngle += 5.0;
		snow_boy->set_theta(snow_xAngle,snow_yAngle,snow_zAngle);
		// cpos_x += -look_z_temp * step;
		// cpos_z -= look_x_temp * step;
		snow_boy->load_obj(snow_f[(snow_i++) % snow_num]);
		
		mp_->update_snow_beterx_buffer(snow_boy);
		break;
	case 'n':
		snow_boy->load_obj(snow_f[(snow_i++) % snow_num]);
		
		mp_->update_snow_beterx_buffer(snow_boy);	
		snow_boy->set_theta(snow_xAngle,screenrate_y,snow_zAngle);
		
		
		snow_boy->get_translate(snow_x,snow_y,snow_z);
		// snow_yAngle = screenrate_y;
		snow_z -= 1.5 * cos(snow_yAngle*DegreesToRadians);
		snow_x += 1.5 * sin(snow_yAngle*DegreesToRadians);
		snow_boy->set_translate(snow_x,snow_y,snow_z);
		cpos_x +=  1.5 * sin(snow_yAngle*DegreesToRadians);
		cpos_z -= 1.5 * cos(snow_yAngle*DegreesToRadians);;
		break;
	default:
		break;
	}

	setCameraEye();
	if(key == 'g'){
		myCamera::eye = camGod;//-34,490,0,1
		 setCameraAt(0,100,0);
		
		//setCameraUp(0.5,0.5,0.5);
	}
	std::cout << "tangle:"<<tAngle<<" pAngle:"<<pAngle<<" rad:"<<rad<<std::endl;
	std::cout<<"eye:  "<<"x:"<<myCamera::eye.x<<" y:"<<myCamera::eye.y<<" z:"<<myCamera::eye.z<<std::endl;	
	std::cout<<"at:  "<<"x"<<myCamera::at.x<<" y:"<<myCamera::at.y<<" z:"<<myCamera::at.z<<std::endl;

	glutPostRedisplay();
	
	
}


// 创建天空盒
void createSkyBox(int index)
{
	int i = 0;
	double scale = 500;
	double a = cos(M_PI/4.0);
	double sa = scale * cos(M_PI/4.0);
	My_Mesh* balloon = new My_Mesh;
	balloon->generate_floor(scale,'d');
	balloon->set_texture_file(skyBoxFile[index * 6 + i]);i++;
	balloon->set_normalization(false);
	balloon->set_translate(0,-sa,0);
	balloon->set_theta(0,90,180);
	balloon->set_theta_step(0,0,0);
	my_meshes.push_back(balloon);
	mp_->add_mesh(balloon);
	vshader_all.push_back("vshader_texture.glsl");
	fshader_all.push_back("fshader_texture.glsl");

	 My_Mesh* h = new My_Mesh;
	h->generate_floor(scale,'u');
	h->set_texture_file(skyBoxFile[index * 6 + i]);i++;
	h->set_normalization(false);
	h->set_translate(0,sa,0);
	h->set_theta(0,90,0);
	h->set_theta_step(0,0,0);
	my_meshes.push_back(h);
	mp_->add_mesh(h);
	vshader_all.push_back("vshader_texture.glsl");
	fshader_all.push_back("fshader_texture.glsl");

 	My_Mesh* my_mesh3 = new My_Mesh;
	
 	//my_mesh3->load_obj("texture/f.obj");
	my_mesh3->generate_wall(scale,'l');
	my_mesh3->set_texture_file(skyBoxFile[index * 6 + i]);i++;
	my_mesh3->set_normalization(false);
	my_mesh3->set_translate(-sa, 0, 0);
	my_mesh3->set_theta(0.0, 180.0, 0.0);
	my_mesh3->set_theta_step(0.0, 0.0, 0.0);	
	my_meshes.push_back(my_mesh3);
	mp_->add_mesh(my_mesh3);
	vshader_all.push_back("vshader_texture.glsl");
	fshader_all.push_back("fshader_texture.glsl");

	My_Mesh* my_mesh4 = new My_Mesh;
	my_mesh4->generate_wall(scale,'r');
	my_mesh4->set_texture_file(skyBoxFile[index * 6 + i]);i++;
	my_mesh4->set_normalization(false);
	my_mesh4->set_translate(sa,0,0);
	my_mesh4->set_theta(0,0,0);
	my_mesh4->set_theta_step(0,0,0);
	my_meshes.push_back(my_mesh4);
	mp_->add_mesh(my_mesh4);
	vshader_all.push_back("vshader_texture.glsl");
	fshader_all.push_back("fshader_texture.glsl");

	My_Mesh* my_mesh5 = new My_Mesh;
	my_mesh5->generate_wall2(scale,'b');
	my_mesh5->set_texture_file(skyBoxFile[index * 6 + i]);i++;
	my_mesh5->set_normalization(false);
	my_mesh5->set_translate(0,0,-sa);
	my_mesh5->set_theta(0,0,0);
	my_mesh5->set_theta_step(0,0,0);
	my_meshes.push_back(my_mesh5);
	mp_->add_mesh(my_mesh5);
	vshader_all.push_back("vshader_texture.glsl");
	fshader_all.push_back("fshader_texture.glsl");

	My_Mesh* my_mesh6 = new My_Mesh;
	my_mesh6->generate_wall2(scale,'f');
	my_mesh6->set_texture_file(skyBoxFile[index * 6 + i]);i++;
	my_mesh6->set_normalization(false);
	my_mesh6->set_translate(0,0,sa);
	my_mesh6->set_theta(0,180,0);
	my_mesh6->set_theta_step(0,0,0);
	my_meshes.push_back(my_mesh6);
	mp_->add_mesh(my_mesh6);
	vshader_all.push_back("vshader_texture.glsl");
	fshader_all.push_back("fshader_texture.glsl");
}


int main( int argc, char **argv )
{
	 

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window.width, window.height);
	glutCreateWindow("2017153040_吴嘉鸿_期末大作业");

	#ifdef WIN32
		glutInitContextVersion(3, 2);
		glutInitContextProfile(GLUT_CORE_PROFILE);
		glewExperimental = GL_TRUE;
		glewInit();
	#endif

	
	//初始化天空盒路径
	generate_skyBoxF();
	//初始化雪人动作帧
	generate_snowBoyFile();
	//生成脸部文件和初始化脸部
	generate_init_face_files();
	//生成批复文件和初始化皮肤
	generate_init_skin_files();
	//初始化
	init();

	// 新建一个画图类
	mp_ = new Mesh_Painter;
	createSkyBox(1);	// 先使用的天空盒
	//开始的雪人
	snow_boy = new My_Mesh;
	snow_boy->load_obj("snow_boy/snow_boy0.obj");
	snow_boy->set_texture_file("snow_boy/snow_boy.jpg");
	snow_boy->set_translate(-5,-5 , 3);	
	snow_boy->set_normalization(false);
	snow_boy->set_theta(0.0, 180, 0.0);	
	snow_boy->set_theta_step(0, 0.0, 0);
	snow_boy->set_withShadow(false);
	snow_boy->setModel(Scale(20,20,20));
	my_meshes.push_back(snow_boy);
	mp_->add_mesh(snow_boy);
	vshader_all.push_back("vshader_texture.glsl");
	fshader_all.push_back("fshader_texture.glsl");

	balloon = new My_Mesh;
	balloon->load_obj("texture/rqq.obj");
	balloon->set_texture_file("texture/Balloon.jpg");	// 指定纹理图像文件
	balloon->set_normalization(false);				// 不进行尺寸归一化
	balloon->set_translate(0,0, -100);			// 平移
	balloon->set_theta(0, 0.0, 0.0);				// 旋转
	balloon->set_theta_step(0, 0.0, 0);				// 旋转动画
	balloon->setModel(Scale(2,2,2));
	my_meshes.push_back(balloon);
	mp_->add_mesh(balloon);
	vshader_all.push_back("vshader_texture.glsl");
	fshader_all.push_back("fshader_texture.glsl");
	//着色器
 	// My_Mesh* h = new My_Mesh;
	// // @TODO: 生成圆盘并贴图、使其旋转
	// h->load_obj("texture/ba.obj");
	// h->set_texture_file("texture/ba.jpg");
	// h->set_translate(-2, 0.52, -0.9);	
	// h->set_normalization(false);
	// h->set_theta(0.0, 0.0, 0.0);	
	// h->set_theta_step(0, 0.0, 0);	
	// h->setModel(Scale(4,4,4));
	// my_meshes.push_back(h);
	// mp_->add_mesh(h);
	// vshader_all.push_back("vshader_texture.glsl");
	// fshader_all.push_back("fshader_texture.glsl");
	

	// My_Mesh* my_mesh3 = new My_Mesh;
	// // @TODO: 生成圆盘并贴图、使其旋转
	// my_mesh3->load_obj("texture/tuzi.obj");
	// my_mesh3->set_texture_file("texture/map.jpg");
	// my_mesh3->set_translate(-100, 0, 0);	
	// my_mesh3->set_normalization(false);
	// my_mesh3->set_theta(0.0, -90.0, 0.0);	
	// my_mesh3->set_theta_step(0, 0.0, 0);
	// my_mesh3->set_withShadow(false);
	// my_mesh3->setModel(Scale(0.8,0.8,0.8));	
	// my_meshes.push_back(my_mesh3);
	// mp_->add_mesh(my_mesh3);
	// vshader_all.push_back("vshader_texture.glsl");
	// fshader_all.push_back("fshader_texture.glsl");

	// My_Mesh* my_mesh4 = new My_Mesh;
	// // @TODO: 生成圆盘并贴图、使其旋转
	// my_mesh4->load_obj("texture/tuzi.obj");
	// my_mesh4->set_texture_file("texture/map.jpg");
	// my_mesh4->set_translate(-20, 0, 0);	
	// my_mesh4->set_normalization(false);
	// my_mesh4->set_theta(0.0, 180, 0.0);	
	// my_mesh4->set_theta_step(0, 0.0, 0);
	// my_mesh4->set_withShadow(true);
	// my_mesh4->setModel(Scale(0.4,0.4,0.4));	
	// my_meshes.push_back(my_mesh4);
	// mp_->add_mesh(my_mesh4);
	// vshader_all.push_back("vshader_obj.glsl");
	// fshader_all.push_back("fshader_obj.glsl");


	// My_Mesh* my_mesh5 = new My_Mesh;
	// // @TODO: 生成圆盘并贴图、使其旋转
	// my_mesh5->load_obj("texture/h.obj");
	// my_mesh5->set_texture_file("texture/house.jpg");
	// my_mesh5->set_translate(190, 0, 0);	
	// my_mesh5->set_normalization(false);
	// my_mesh5->set_theta(0, -90, 0.0);	
	// my_mesh5->set_theta_step(0, 0.0, 0);
	// my_mesh5->set_withShadow(false);
	// my_mesh5->setModel(Scale(2,2,2));	
	// my_meshes.push_back(my_mesh5);
	// mp_->add_mesh(my_mesh5);
	// vshader_all.push_back("vshader_texture.glsl");
	// fshader_all.push_back("fshader_texture.glsl");
	

	// My_Mesh* my_mesh6 = new My_Mesh;
	// //
	// my_mesh6->load_obj("texture/wk.obj");
	// my_mesh6->set_texture_file("texture/wk.jpg");
	// my_mesh6->set_translate(15,8, 0);	
	// my_mesh6->set_normalization(false);
	// my_mesh6->set_theta(0.0, 0.0, 0.0);	
	// my_mesh6->set_theta_step(0, 0.0, 0);
	// my_mesh6->set_withShadow(false);
	// my_mesh6->setModel(Scale(2,2,2));
	// //my_mesh6	
	// my_meshes.push_back(my_mesh6);
	// mp_->add_mesh(my_mesh6);
	// vshader_all.push_back("vshader_texture.glsl");
	// fshader_all.push_back("fshader_texture.glsl");

	// My_Mesh* my_mesh8 = new My_Mesh;
	// // 
	// my_mesh8->load_obj("texture/table.obj");
	// my_mesh8->set_texture_file("texture/table.png");
	// my_mesh8->set_translate(15, 8, 0);	
	// my_mesh8->set_normalization(false);
	// my_mesh8->set_theta(90.0, 0.0, 0.0);	
	// my_mesh8->set_theta_step(0, 0.0, 0);
	// my_mesh8->set_withShadow(true);
	// my_mesh8->setModel(Scale(8,8,8));
	// //my_mesh8
	// my_meshes.push_back(my_mesh8);
	// mp_->add_mesh(my_mesh8);
	// vshader_all.push_back("vshader_obj.glsl");
	// fshader_all.push_back("fshader_obj.glsl");

	// My_Mesh* my_mesh9 = new My_Mesh;
	// // 
	// my_mesh9->load_obj("texture/table.obj");
	// my_mesh9->set_texture_file("texture/table.png");
	// my_mesh9->set_translate(0, 0, 0);	
	// my_mesh9->set_normalization(false);
	// my_mesh9->set_theta(90.0, 0.0, 0.0);	
	// my_mesh9->set_theta_step(0, 0.0, 0);
	// my_mesh9->set_withShadow(true);
	// my_mesh9->setModel(Scale(8,8,8));
	// //my_mesh8
	// my_meshes.push_back(my_mesh9);
	// mp_->add_mesh(my_mesh9);
	// vshader_all.push_back("vshader_texture.glsl");
	// fshader_all.push_back("fshader_texture.glsl");
	
	//My_Mesh* snow_boy = new My_Mesh;
	// 
	


	mp_->init_shaders(vshader_all,fshader_all);
	mp_->update_texture();	
	mp_->update_vertex_buffer();
	
	// xinjian
	// std::cout<<"bug"<<std::endl;

	/***********初始进入天空盒动画***********/
	setCameraEye();
	// tAngle = 40;
	// pAngle = 45;
	// rad = 850;
	// glutTimerFunc(20,TimerCal_Into_SkyBox,int(rad));

	glutDisplayFunc(display);

	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glutPassiveMotionFunc(onMouseMovePassive);

	glutMouseWheelFunc(mouseWheel);
	glutMotionFunc(mouseMotionCB);
	glutSpecialFunc(special_key);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	// glutCreateMenu(menu);
    // glutAddMenuEntry("Quit", Quit);
  
    // glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	mp_->clear_meshes();
	for (unsigned int i = 0; i < my_meshes.size(); i++)
	{
		delete my_meshes[i];
	}
	delete mp_;
	return 0;
}
