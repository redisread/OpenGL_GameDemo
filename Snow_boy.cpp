#include "Snow_boy.h"
#include <sstream>
using namespace std;

namespace snow_boy{

    vector<My_Mesh*> snow_meshes;
    vector<string>  sv;
    vector<string>  sf;
    Mesh_Painter* snow_painter;
    int snow_index = 0;
    int Num = 1;

    void init()
    {       
        snow_painter = new Mesh_Painter();
        stringstream ss;
        for(int i = 0;i < Num;i++)
        {
            ss<<i;
            My_Mesh * my_mesh1 = new My_Mesh;
            my_mesh1->load_obj("snow_boy/snow_boy"+ss.str()+".obj");
            my_mesh1->set_texture_file("snow_boy/snow_boy.jpg");	// 指定纹理图像文件
	        my_mesh1->set_normalization(false);				// 不进行尺寸归一化
	        my_mesh1->set_translate(0.0,0.0, 0.0);			// 平移
	        my_mesh1->set_theta(0, 0.0, 0.0);				// 旋转
	        my_mesh1->set_theta_step(0, 0.0, 0);				// 旋转动画
            my_mesh1->setModel(Scale(10,10,10));
	        // my_meshes.push_back(my_mesh1);
	        snow_meshes.push_back(my_mesh1);
            ss.clear();
        }
        sv.push_back("vshader_texture.glsl");
	    sf.push_back("fshader_texture.glsl");
        snow_painter->add_mesh(snow_meshes[0]);
        snow_painter->init_shaders(sv,sf);
    }

    void update()
    {
        snow_painter->dele_mesh();
        snow_painter->add_mesh(snow_meshes[(snow_index++) % Num]);
        // snow_painter->init_shaders(sv,sf);
    }

    void draw()
    {
        snow_painter->update_texture();
        snow_painter->update_vertex_buffer();
        snow_painter->draw_meshes();
    }
    void dele(){

    }
}