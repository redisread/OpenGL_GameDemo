
#include "Angel.h"
//相机参数
#undef near		//近裁剪平面
#undef far		//远裁剪平面
#undef top		//顶部

/*************************相机*******************************/
// 拖动鼠标控制光源位置的参数
bool mouseLeftDown;		// 鼠标左键按下
bool mouseRightDown;	// 鼠标右键按下
float mouseX, mouseY;	// 捕捉鼠标的位置
float cameraDistance;	// 相机距离
float cameraAngleX;		// 相机X旋转
float cameraAngleY;		// 相机Y轴旋转


float step = 0.1; //第一人称移动的步长

// 三种视角
enum{
	One_D,
	Two_D,
	Three_D
};

int view_3D = Three_D;	//默认第三人称


//tangle:15 pAngle:-5 rad:83

// perspective 参数
float fov = 100.0;
float aspect = 1.0;	//宽高比
float zN = 0.1;
float zF = 500.0;
float camera_height = 20.0;
// 控制视角的角度初始值
float rad = 43.0;		// 控制远近
float tAngle = 0.0;	// 控制绕X轴旋转的角度
float pAngle = 0.0;		// 控制绕y轴旋转的角度
vec4 eye;
vec4 at;
vec4 up(0, 1, 0, 0);
namespace myCamera
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projMatrix;			//
	double near = 0.01;			//近裁剪平面
	double far = 500.0;			//远裁剪平面

    vec4 eye = vec4(0,1,0,1);
	vec4 at  = vec4(0,0,0,1);
    vec4 up  = vec4(0,1,0,0);

	// 正交投影
	mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar)
	{
		return Ortho(left,right,bottom,top,zNear,zFar);
	}
	 
	// 透视投影，近大远小
	mat4 perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		return Perspective(fovy,aspect,zNear,zFar);
		
	}
	// 相机观察函数
	mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up)
	{
		return LookAt(eye,at,up);
	}
	mat4 glLookAt()
	{
		return LookAt(eye,at,up);
	}
};

vec4 cam3 = vec3(0,360,0);	// 第三人称视角 相机的eye: y: z:
vec4 camGod = vec4(0,200,0,1); // 上帝视角eye
// 三个函数设置相机

void setCameraEye()
{
	float x = rad*cos(tAngle*DegreesToRadians)*sin(pAngle*DegreesToRadians);
	float y = rad*sin(tAngle*DegreesToRadians) + camera_height;
	float z = rad*cos(tAngle*DegreesToRadians)*cos(pAngle*DegreesToRadians);

	myCamera::eye = vec4(x,y,z,1);	// 设置相机的坐标

}
// 设置at
void setCameraAt(float x,float y,float z)
{
	myCamera::at = vec4(x,y,z,1);
}

// 设置up
void setCameraUp(float x, float y,float z)
{
	myCamera::up = vec4(x,y,z,0);
}

/*************************END*******************************/



/*************************定时器*******************************/

void TimerCal(int value){
		if(value == 0)
			return;
		theta[RightUpperArm] += 5.0;
		theta[LeftUpperArm]  -= 5.0;
		theta[LeftUpperLeg]  -= 5.0;
		theta[RightUpperLeg] +=5.0;
		glutPostRedisplay();
		glutTimerFunc(20, TimerCal, value-1);
}

void TimerCal_snow(int value){
		if(value == 0)
			return;
		snow_boy->load_obj(snow_f[(snow_i++) % snow_num]);
		mp_->update_snow_beterx_buffer(snow_boy);
		glutPostRedisplay();
		glutTimerFunc(20, TimerCal_snow, value-1);
}

void TimerCal_Camera_Roate(int value)
{
    if(value == 0)
        return;
    pAngle += 5.0;
    tAngle += 1.2;
    setCameraEye();
    glutPostRedisplay();
    glutTimerFunc(20, TimerCal_Camera_Roate, value-5);
}

void TimerCal_Into_SkyBox(int value)
{
    if(value < 20)
        return;
    rad = value-5;
    setCameraEye();
    glutPostRedisplay();
    glutTimerFunc(20,TimerCal_Into_SkyBox,value-3);
    
}

/*************************END*******************************/