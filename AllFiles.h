#include "Angel.h"
#include "Mesh_Painter.h"
// 存储要贴纹理的物体
std::vector<My_Mesh*>	my_meshes;
// 存储纹理图片
Mesh_Painter*			mp_;


/*************************着色器*******************************/
// 顶点着色器和片元着色器的地址
std::vector<std::string> vshader_all;
std::vector<std::string> fshader_all;

/**************************END******************************/


/*************************脸部*******************************/
// 脸部文件 
std::vector<std::string> face_file; //脸部文件路径
std::string current_face;           //当前的脸部路径
int face_Index = 0;                 //当前脸部         
void generate_init_face_files()
{
    face_file.push_back("texture/face/steve.png");
	face_file.push_back("texture/face/smile.jpg");
	face_file.push_back("texture/face/ljr.png");
	face_file.push_back("texture/face/baby.jpg");
	current_face = face_file[0];
}
/*************************END*******************************/

/*************************皮肤*******************************/
std::vector<std::string> skin_file;	//皮肤文件路径
std::string current_skin;
int skin_Index = 0;
void generate_init_skin_files()
{
	skin_file.push_back("texture/skin/green.jpg");
	skin_file.push_back("texture/skin/ljr.png");
	skin_file.push_back("texture/skin/stone.png");
	//skin_file.push_back("texture/skin/y.");
	
	current_skin = skin_file[0];

}


/*************************END*******************************/

/*************************纹理*******************************/

int name_index = 0;
std::string robotName[] = {"victor","alex"};
int name_size = 2;

/*************************END*******************************/


/*************************天空盒*******************************/
std::vector<std::string> skyBoxFile;
void generate_skyBoxF() // 初始化天空盒文件路径
{
	// one
	skyBoxFile.push_back("skyBox/dn.jpg");skyBoxFile.push_back("skyBox/up.jpg");
	skyBoxFile.push_back("skyBox/lf.jpg");skyBoxFile.push_back("skyBox/rt.jpg");
	skyBoxFile.push_back("skyBox/bk.jpg");skyBoxFile.push_back("skyBox/ft.jpg");

	// two
	skyBoxFile.push_back("texture/sky1/dn.tga");skyBoxFile.push_back("texture/sky1/up.tga");
	skyBoxFile.push_back("texture/sky1/lf.tga");skyBoxFile.push_back("texture/sky1/rt.tga");
	skyBoxFile.push_back("texture/sky1/bk.tga");skyBoxFile.push_back("texture/sky1/ft.tga");

	// Three
	skyBoxFile.push_back("texture/sky2/dn.tga");skyBoxFile.push_back("texture/sky2/up.tga");
	skyBoxFile.push_back("texture/sky2/lf.tga");skyBoxFile.push_back("texture/sky2/rt.tga");
	skyBoxFile.push_back("texture/sky2/bk.tga");skyBoxFile.push_back("texture/sky2/ft.tga");

	// Three
	skyBoxFile.push_back("texture/sky3/dn.tga");skyBoxFile.push_back("texture/sky3/up.tga");
	skyBoxFile.push_back("texture/sky3/lf.tga");skyBoxFile.push_back("texture/sky3/rt.tga");
	skyBoxFile.push_back("texture/sky3/bk.tga");skyBoxFile.push_back("texture/sky3/ft.tga");

	// Three
	skyBoxFile.push_back("texture/sky4/dn.tga");skyBoxFile.push_back("texture/sky4/up.tga");
	skyBoxFile.push_back("texture/sky4/lf.tga");skyBoxFile.push_back("texture/sky4/rt.tga");
	skyBoxFile.push_back("texture/sky4/bk.tga");skyBoxFile.push_back("texture/sky4/ft.tga");

	// Three
	skyBoxFile.push_back("texture/sky7/dn.tga");skyBoxFile.push_back("texture/sky7/up.tga");
	skyBoxFile.push_back("texture/sky7/lf.tga");skyBoxFile.push_back("texture/sky7/rt.tga");
	skyBoxFile.push_back("texture/sky7/bk.tga");skyBoxFile.push_back("texture/sky7/ft.tga");


}
void setskyFile(int index)  // 设置天空盒子文件
{
	for(int i = 0;i < 6;i++)
		{
			my_meshes[i]->set_texture_file(skyBoxFile[index*6+i]);
		}
}
int skyBox_Index = 0;
/*************************END*******************************/



/*************************雪人*******************************/
My_Mesh *snow_boy;	// 雪人
std::vector<std::string> snow_f;
int snow_i = 0;
int snow_num  = 19;

void generate_snowBoyFile()
{
	snow_f.push_back("snow_boy/snow_boy0.obj");
	snow_f.push_back("snow_boy/snow_boy1.obj");
	snow_f.push_back("snow_boy/snow_boy2.obj");
	snow_f.push_back("snow_boy/snow_boy3.obj");
	snow_f.push_back("snow_boy/snow_boy3.obj");
	snow_f.push_back("snow_boy/snow_boy5.obj");
	snow_f.push_back("snow_boy/snow_boy6.obj");
	snow_f.push_back("snow_boy/snow_boy7.obj");
	snow_f.push_back("snow_boy/snow_boy8.obj");
	snow_f.push_back("snow_boy/snow_boy9.obj");
	snow_f.push_back("snow_boy/snow_boy10.obj");
	snow_f.push_back("snow_boy/snow_boy11.obj");
	snow_f.push_back("snow_boy/snow_boy12.obj");
	snow_f.push_back("snow_boy/snow_boy13.obj");
	snow_f.push_back("snow_boy/snow_boy14.obj");
	snow_f.push_back("snow_boy/snow_boy15.obj");
	snow_f.push_back("snow_boy/snow_boy16.obj");
	snow_f.push_back("snow_boy/snow_boy17.obj");
	snow_f.push_back("snow_boy/snow_boy18.obj");
}
/*************************END*******************************/





