#include "mesh.h"
#include <sstream>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <math.h>
#include <array>
#include <vector>
using namespace std;

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <gl/GL.h>
#endif

#define MAX  999999.99
#define MIN -999999.99

My_Mesh::My_Mesh()
{
	Axis = Xaxis;
	vTranslation[0] = Theta[0] = Theta_step[0] = 0;
	vTranslation[1] = Theta[1] = Theta_step[1] = 0;
	vTranslation[2] = Theta[2] = Theta_step[2] = 0;
	Theta[0] = 45;
	toNormalize = true;
	withShadow = false;
	model = mat4(1.0);
}

My_Mesh::~My_Mesh(){}


int My_Mesh::findKey(string str, char key) {
    int value = 0;
    for (int i = 0; i < str.length(); i++)
        if (str[i] == key)
            value++;
    return value;
}

void My_Mesh::load_obj(std::string obj_File)
{
	if(obj_File.empty()) return;	//假如文件名为空则不执行
		std::ifstream in;	//使用文件流读取
	in.open(obj_File.c_str(), std::ios::in);

	string sign;
	float x,y,z;
	unsigned int ix,iy,iz;
	char c;
	float max_X,max_Y,min_X,min_Y;
	max_X = max_Y = min_X = min_Y = 0;

	/*******************清除数据**********************/
	this->clear_data();
	string line;
	string str;
	/*********************开始读取数据**************************/
	while(!in.eof())
	{
		//获取每行数据
		getline(in,line);
		if(line.size() == 0) continue;
		if(line[0] == '#') 	 continue;
		else if(line[0] == 'v')
		{
			if(line[1] == 't')	//vt坐标 //存储点的贴图坐标
				{
					istringstream IL(line);
					IL >> str >> x >> y >> z;
					m_vt_list_.push_back(x);
					m_vt_list_.push_back(y);
				}
			else if(line[1] == 'n')	//vn坐标 //存储点的法线数据
				{
					istringstream IL(line);
					IL >> str >> x >> y >> z;
					m_normal_list_.push_back(x);
					m_normal_list_.push_back(y);
					m_normal_list_.push_back(z);
					float r;
					float g;
					float b;
					My_Mesh::normal_to_color(x, y, z, r, g, b);	//存储顶点颜色数据
					m_color_list_.push_back(r);
					m_color_list_.push_back(g);
					m_color_list_.push_back(b);
				}
			else	//v定点坐标 //存储顶点坐标
				{
					istringstream IL(line);
					IL >> str >> x >> y >> z;
					if(x>max_X)	max_X = x;
					if(x<min_X) min_Y = y;
					if(y>max_Y) max_Y = y;
					if(y<min_Y) min_Y = y;
					m_vertice_list_.push_back(x);
					m_vertice_list_.push_back(y);
					m_vertice_list_.push_back(z);
				}
			
		}
		else if(line[0] == 'f'){	//存储片元信息
			istringstream IL(line);
			//  格式为V1/VT1/VN1 V2/VT2/VN2 V3/VT3/VN3
			IL >> str;
            for(int i = 0;i < 3;i++)
			{	
				IL  >> ix >> c>> iy >> c >>iz;
				m_vertice_index_.push_back(ix-1);
				m_vt_index_.push_back(iy-1);
				m_normal_index_.push_back(iz-1);
				m_color_index_.push_back(iz-1); //"f 顶点索引/uv点索引/法线索引"。

			}	
		}
	}
	/*******************设置中心和边框大小**********************/
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(min_X, min_Y,min_X);
	this->m_max_box_ = point3f(max_X, max_Y, max_X);
	in.close();	
	/*********************读取数据完毕**************************/
}


// 生成圆柱体
void My_Mesh::generate_cylinder(int num_division, float radius, float height)
{
	this->clear_data();
	this->m_center_  = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-radius, -radius, -height);
	this->m_max_box_ = point3f( radius,  radius,  height);

	int num_samples = num_division;
	float step = 2 * M_PI / num_samples; // 每个切片的弧度
	
	// 按cos和sin生成x，y坐标，z为负，即得到下表面顶点坐标
	float z = -height;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = radius * cos(r_r_r);
		float y = radius * sin(r_r_r);
		m_vertice_list_.push_back(x);
		m_vertice_list_.push_back(y);
		m_vertice_list_.push_back(z);
		
		m_normal_list_.push_back(x);
		m_normal_list_.push_back(y);
		m_normal_list_.push_back(0);
		// 法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		// 这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}

	// 按cos和sin生成x，y坐标，z为正，即得到上表面顶点坐标
	z = height;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = radius * cos(r_r_r);
		float y = radius * sin(r_r_r);
		m_vertice_list_.push_back(x);
		m_vertice_list_.push_back(y);
		m_vertice_list_.push_back(z);

		m_normal_list_.push_back(x);
		m_normal_list_.push_back(y);
		m_normal_list_.push_back(0);
		// 法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		// 这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);

	}

	// 生成三角面片，每个矩形由两个三角形面片构成
	for (int i = 0; i < num_samples; i++)
	{
		// 面片1
		m_vertice_index_.push_back(i);
		m_vertice_index_.push_back((i + 1) % num_samples);
		m_vertice_index_.push_back((i) + num_samples);
		// 面片2
		m_vertice_index_.push_back((i ) + num_samples);
		m_vertice_index_.push_back((i + 1) % num_samples);
		m_vertice_index_.push_back((i + num_samples + 1) % (num_samples) + num_samples);
		
		// 将0-360度映射到UV坐标的0-1
		// 纹理坐标
		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(0.0);
		m_vt_index_.push_back(6 * i);
		// 纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		m_vt_index_.push_back(6 * i + 1);
		// 纹理坐标
		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		m_vt_index_.push_back(6 * i + 2);

		// 纹理坐标
		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		m_vt_index_.push_back(6 * i + 3);
		// 纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		m_vt_index_.push_back(6 * i + 4);
		// 纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(1.0);
		m_vt_index_.push_back(6 * i + 5);
	}

	m_normal_index_ = m_vertice_index_;
	m_color_index_  = m_vertice_index_;
};

// 生成地板
void My_Mesh::generate_floor(double scale,char type)
{
	this->clear_data();
	this->m_center_  = point3f(0, 0, 0);
	int num_samples = 4;
	float x,y,z;
	float angle = 45 * M_PI/180.0;
	for(int i = 0;i < num_samples;i++)
	{
		double currentAngle = angle + i * (M_PI/2);
		x = scale * cos(currentAngle);
		y = 0;
		z = scale * sin(currentAngle);
		m_vertice_list_.push_back(x);
		m_vertice_list_.push_back(y);
		m_vertice_list_.push_back(z);

		m_normal_list_.push_back(x);
		m_normal_list_.push_back(0);
		m_normal_list_.push_back(z);
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
	
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}

	// 顶点面片，两个
	m_vertice_index_.push_back(0);
	m_vertice_index_.push_back(1);
	m_vertice_index_.push_back(2);
	m_vertice_index_.push_back(0);
	m_vertice_index_.push_back(3);
	m_vertice_index_.push_back(2);

	// 法向面片
	
	// 纹理坐标及面片
	m_vt_list_.push_back(1);m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);m_vt_list_.push_back(0);
	m_vt_index_ = m_vertice_index_;

	m_color_index_ = m_vertice_index_;
	m_normal_index_ = m_vertice_index_;


}

// 生成左右墙壁
void My_Mesh::generate_wall(double scale,char type)
{
	this->clear_data();
	double radius = 2,height=2;
	 this->m_center_  = point3f(0, 0, 0);
	
	int num_samples = 4;
	float x,y,z;
	float angle = 45 * M_PI/180.0;
	for(int i = 0;i < num_samples;i++)
	{
		double currentAngle = angle + i * (M_PI/2);
		x = 0;
		y =  scale * sin(currentAngle);
		z = scale * cos(currentAngle);

		m_vertice_list_.push_back(x);
		m_vertice_list_.push_back(y);
		m_vertice_list_.push_back(z);

		m_normal_list_.push_back(x);
		m_normal_list_.push_back(0);
		m_normal_list_.push_back(z);

		// 法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		// 这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);

	}

	// 顶点面片，两个
	m_vertice_index_.push_back(0);
	m_vertice_index_.push_back(1);
	m_vertice_index_.push_back(2);
	m_vertice_index_.push_back(0);
	m_vertice_index_.push_back(3);
	m_vertice_index_.push_back(2);

	// 法向面片
	
	// 纹理坐标及面片
	m_vt_list_.push_back(1);m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);m_vt_list_.push_back(0);

	m_vt_index_ = m_vertice_index_;
	// if(type == 'r')
	// {
	// 	m_vt_index_ = m_vertice_index_;
		
	// }
	// if(type == 'l')
	// {
	// 	m_vt_index_.push_back(1);m_vt_index_.push_back(0);m_vt_index_.push_back(3);
	// 	m_vt_index_.push_back(1);m_vt_index_.push_back(3);m_vt_index_.push_back(2);
	// }
	
	m_color_index_ = m_vertice_index_;
	m_normal_index_ = m_vertice_index_;

}

void My_Mesh::generate_wall2(double scale,char type)
{
	this->clear_data();

	 this->m_center_  = point3f(0, 0, 0);
	// this->m_min_box_ = point3f(-radius, -radius, -height);
	// this->m_max_box_ = point3f( radius,  radius,  height);
	
	int num_samples = 4;
	float x,y,z;
	float angle = 45 * M_PI/180.0;
	//z = sin(getSquareAngle(2))*scale;
	for(int i = 0;i < num_samples;i++)
	{
		double currentAngle = angle + i * (M_PI/2);
		x = scale * cos(currentAngle);
		y = scale * sin(currentAngle);
		z = 0;

		m_vertice_list_.push_back(x);
		m_vertice_list_.push_back(y);
		m_vertice_list_.push_back(z);

		m_normal_list_.push_back(x);
		m_normal_list_.push_back(0);
		m_normal_list_.push_back(z);

		// 法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		// 这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);

	}

	// 顶点面片，两个
	m_vertice_index_.push_back(0);
	m_vertice_index_.push_back(1);
	m_vertice_index_.push_back(2);
	m_vertice_index_.push_back(0);
	m_vertice_index_.push_back(3);
	m_vertice_index_.push_back(2);

	// 法向面片
	
	// 纹理坐标及面片
	m_vt_list_.push_back(1);m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);m_vt_list_.push_back(1);
	m_vt_list_.push_back(0);m_vt_list_.push_back(0);
	m_vt_list_.push_back(1);m_vt_list_.push_back(0);

	//if(type=='b')
		m_vt_index_ = m_vertice_index_;
	// if(type=='f')
	// {
	// 	m_vt_index_.push_back(1);m_vt_index_.push_back(0);m_vt_index_.push_back(3);
	// 	m_vt_index_.push_back(1);m_vt_index_.push_back(3);m_vt_index_.push_back(2);
	// }
	m_color_index_ = m_vertice_index_;
	m_normal_index_ = m_vertice_index_;

}

// 生成圆盘
void My_Mesh::generate_disk(int num_division, float radius)
{
	// @TODO: 请在此添加代码生成圆盘
	
	
};

// 生成圆锥体
void My_Mesh::generate_cone(int num_division, float radius, float height)
{
	// @TODO: 请在此添加代码生成圆锥体
	

};

void My_Mesh::normal_to_color(float nx, float ny, float nz, float& r, float& g, float& b)
{
	r = float(std::min(std::max(0.5 * (nx + 1.0), 0.0), 1.0));
	g = float(std::min(std::max(0.5 * (ny + 1.0), 0.0), 1.0));
	b = float(std::min(std::max(0.5 * (nz + 1.0), 0.0), 1.0));
};

const std::vector<float>& My_Mesh::get_vertices()
{
	return this->m_vertice_list_;
};

const std::vector<float>& My_Mesh::get_normals()
{
	return this->m_normal_list_;
};

const std::vector<float>&  My_Mesh::get_colors()
{
	return this->m_color_list_;
};

const std::vector<float>&  My_Mesh::get_vts()
{
	return this->m_vt_list_;
};

const std::vector<unsigned int>& My_Mesh::get_vertice_index_()
{
	return this->m_vertice_index_;
};

const std::vector<unsigned int>& My_Mesh::get_normal_index()
{
	return this->m_normal_index_;
};

const std::vector<unsigned int>& My_Mesh::get_color_index()
{
	return this->m_color_index_;
};

const std::vector<unsigned int>& My_Mesh::get_vt_index()
{
	return this->m_vt_index_;
};

const int My_Mesh::get_num_vertices()
{
	return this->m_vertice_list_.size() / 3;
};

const int My_Mesh::get_num_faces()
{
	return this->m_vertice_index_.size() / 3;
};

const point3f& My_Mesh::get_center()
{
	return this->m_center_;
};

const bool My_Mesh::whether_to_normalize()
{
	return this->toNormalize;
};

void My_Mesh::set_normalization	(bool  toNormalize)
{
	this->toNormalize = toNormalize;
};

void My_Mesh::set_texture_file(std::string s)
{
	this->texture_file_name = s;
};

void My_Mesh::get_texture_file(std::string& s)
{
	s = this->texture_file_name;
};

void My_Mesh::set_translate(float x, float y, float z)
{
	vTranslation[0] = x;
	vTranslation[1] = y;
	vTranslation[2] = z;
};

void My_Mesh::get_translate(float& x, float& y, float& z)
{
	x = vTranslation[0];
	y = vTranslation[1];
	z = vTranslation[2];
};

void My_Mesh::set_theta(float x, float y, float z)
{
	Theta[0] = x;
	Theta[1] = y;
	Theta[2] = z;
};

void My_Mesh::get_theta(float& x, float& y, float& z)
{
	x = Theta[0];
	y = Theta[1];
	z = Theta[2];
};

void My_Mesh::set_theta_step(float x, float y, float z)
{
	Theta_step[0] = x;
	Theta_step[1] = y;
	Theta_step[2] = z;
};

void My_Mesh::add_theta_step()
{
	Theta[0] = Theta[0] + Theta_step[0];
	Theta[1] = Theta[1] + Theta_step[1];
	Theta[2] = Theta[2] + Theta_step[2];
};

void My_Mesh::get_boundingbox(point3f& min_p, point3f& max_p)
{
	min_p = this->m_min_box_;
	max_p = this->m_max_box_;
};

void My_Mesh::clear_data()
{
	m_vertice_list_.clear();
	m_normal_list_.clear();
	m_color_list_.clear();
	m_vt_list_.clear();
	m_vertice_index_.clear();
	m_normal_index_.clear();
	m_color_index_.clear();
	m_vt_index_.clear();
};