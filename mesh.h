#pragma once

#include <vector>
#include <string>
#include "Angel.h"
#include <assert.h>

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

struct  point3f
{
	float x;
	float y;
	float z;
	
	point3f()
	{
		x = 0;
		y = 0;
		z = 0;
	};

	point3f(float xx, float yy, float zz)
	{
		x = xx;
		y = yy;
		z = zz;
	};

	float distance(const point3f& p)
	{
		float d = (x - p.x)*(x - p.x);
		d += (y - p.y)*(y - p.y);
		d += (z - p.z)*(z - p.z);
		return sqrt(d);
	};
};
class My_Mesh
{
private:

	void clear_data();

	std::vector<float>			m_vertice_list_;
	std::vector<float>			m_normal_list_;
	std::vector<float>			m_color_list_;
	std::vector<float>			m_vt_list_;

	std::vector<unsigned int>	m_vertice_index_;
	std::vector<unsigned int>	m_normal_index_;
	std::vector<unsigned int>	m_color_index_;
	std::vector<unsigned int>	m_vt_index_;

	point3f 					m_center_;
	point3f 					m_min_box_;
	point3f 					m_max_box_;

	std::string texture_file_name;
	enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
	int		Axis;
	GLfloat	Theta[3];
	GLfloat	Theta_step[3];
	GLfloat vTranslation[3];
	bool	toNormalize;
	bool 	withShadow;
	mat4 model;

public:

	My_Mesh();
	~My_Mesh();

	void load_obj				(std::string obj_File);
	void generate_cylinder		(int num_division = 100, float radius = 1, float height = 3);
	void generate_disk			(int num_division = 100, float radius = 1);
	void generate_cone			(int num_division = 100, float radius = 1, float height = 2);
	
	
	void generate_floor(double scale,char type);	// 生成地板或者天花板
	void generate_wall(double scale,char type);		//生成墙
	void generate_wall2(double scale,char type);
	void generate_Robot();	//生成机器人
	static void normal_to_color	(float, float, float, float&, float&, float&);
	int findKey(std::string str, char key);
	
	const std::vector<float>&			get_vertices();
	const std::vector<float>&			get_normals();
	const std::vector<float>&			get_colors();
	const std::vector<float>&			get_vts();
	const std::vector<unsigned int>&	get_vertice_index_();
	const std::vector<unsigned int>&	get_normal_index();
	const std::vector<unsigned int>&	get_color_index();
	const std::vector<unsigned int>&	get_vt_index();
	const int							get_num_vertices();
	const int							get_num_faces();
	const point3f&						get_center();
	const bool							whether_to_normalize();

	void set_normalization	(bool toNormalize);
	void set_texture_file	(std::string  s);
	void get_texture_file	(std::string& s);
	void set_translate		(float  x, float  y, float  z);
	void get_translate		(float& x, float& y, float& z);
	void set_theta			(float  x, float  y, float  z);
	void get_theta			(float& x, float& y, float& z);
	void set_theta_step		(float  x, float  y, float  z);
	void add_theta_step		();
	void get_boundingbox	(point3f& min_p, point3f& max_p);
	
	void set_withShadow(bool isWith){this->withShadow=isWith;};
	bool get_withShadow(){return withShadow;}
	mat4 getModel(){return model;}
	void setModel(mat4 m){model = m;}

	GLuint vao;
	GLuint vbo;
	GLuint program;
	void setVao(GLuint a){this->vao = a;}
	void setVbo(GLuint b){this->vbo = b;}
	void setProgram(GLuint p){this->program = p;}
	GLuint getVao(){return vao;}
	GLuint getVbo(){return vbo;}
	GLuint getProgram(){return this->program;}
};