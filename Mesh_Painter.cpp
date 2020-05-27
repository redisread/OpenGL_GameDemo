#include "Mesh_Painter.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Mesh_Painter::Mesh_Painter(){}
Mesh_Painter::~Mesh_Painter(){}
void Mesh_Painter::add_mesh(My_Mesh* m)
{
	this->m_my_meshes_.push_back(m);
};

GLuint modelViewMatrixID;

mat4 modelMatrix;
mat4 viewMatrix;
mat4 projMatrix;
mat4 shadowMatrix;
vec3 lightP;

void Mesh_Painter::init_shaders(std::vector<std::string>  vs, std::vector<std::string> fs)
{
	this->program_all.clear();
	this->theta_all.clear();
	this->trans_all.clear();
	// this->my_phong.clear();
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		// 添加着色器
		GLuint program = InitShader(vs[i].c_str(), fs[i].c_str());
		this->program_all.push_back(program);
		//if(!m_my_meshes_[i]->get_withShadow())		
		GLuint 	theta = glGetUniformLocation(program, "theta");
		GLuint  trans = glGetUniformLocation(program, "translation");
		theta_all.push_back(theta);
		trans_all.push_back(trans);

		this->m_my_meshes_[i]->setProgram(program);
	}
};


void Mesh_Painter::setCameraMatrix(mat4 model,mat4 view,mat4 pro)
{
	modelMatrix = model;
	viewMatrix = view;
	projMatrix = pro;
}

void Mesh_Painter::setShadowMateix(mat4 shadowMat)
{
	shadowMatrix = shadowMat;
}
void Mesh_Painter::setLightPos(vec3 pos)
{
	lightP = pos;
}

void Mesh_Painter::update_snow_beterx_buffer(My_Mesh *mm)
{
	int num_face = mm->get_num_faces();
	int num_vertex = mm->get_num_vertices();

const std::vector<float>& vertex_list			= mm->get_vertices();
const std::vector<float>& normal_list			= mm->get_normals();
const std::vector<float>& color_list			= mm->get_colors();
const std::vector<float>& vt_list				= mm->get_vts();
const std::vector<unsigned int>& vertex_index	= mm->get_vertice_index_();
const std::vector<unsigned int>& normal_index	= mm->get_normal_index();
const std::vector<unsigned int>& color_index	= mm->get_color_index();
const std::vector<unsigned int>& vt_index		= mm->get_vt_index();

glBindVertexArray(mm->getVao());
glBindBuffer(GL_ARRAY_BUFFER, mm->getVbo());

// glBufferData(GL_ARRAY_BUFFER,
// 					sizeof(vec3)*num_face * 3		// 顶点坐标
// 					+ sizeof(vec3)*num_face * 3		// 法线
// 					+ sizeof(vec3)*num_face * 3		// 颜色
// 					+ sizeof(vec2)*num_face * 3,	// 贴图坐标
// 					NULL, GL_STATIC_DRAW);

GLintptr offset = 0;
		point3f  p_center_;
		float d;
		// 判断是否进行归一化
		if (mm->whether_to_normalize())
		{
			p_center_ = mm->get_center();	// bounding box的中心点坐标
			point3f p_min_box_, p_max_box_;
			mm->get_boundingbox(p_min_box_, p_max_box_);
			d = p_min_box_.distance(p_max_box_);				// bounding box的对角线长度
		}
		else
		{
			p_center_ = point3f(0.0, 0.0, 0.0);
			d = 1.0;
		}

		// -------------------- 实现顶点到shader的映射 ------------------------------
		vec3* points = new vec3[num_face * 3];
		// 逐面片存储每个面片的三个顶点坐标
		for (int i = 0; i < num_face; i++)
		{
			int index = vertex_index[3 * i]; // 数组标号从0开始
			points[3 * i] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,	// 平移到中心并缩放
								(vertex_list[index * 3 + 1] - p_center_.y) / d,
								(vertex_list[index * 3 + 2] - p_center_.z) / d );
			
			index = vertex_index[3 * i + 1];
			points[3 * i + 1] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
									(vertex_list[index * 3 + 1] - p_center_.y) / d,
									(vertex_list[index * 3 + 2] - p_center_.z) / d );
			
			index = vertex_index[3 * i + 2];
			points[3 * i + 2] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
									(vertex_list[index * 3 + 1] - p_center_.y) / d,
									(vertex_list[index * 3 + 2] - p_center_.z) / d );
			
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------
		
		// -------------------- 实现法线到shader的映射 ------------------------------
		points = new vec3[num_face * 3];
		// 逐面片存储每个面片的三个法线
		for (int i = 0; i < num_face; i++)
		{
			int index = normal_index[3 * i];
			points[3 * i] = vec3((normal_list[index * 3 + 0]),
								(normal_list[index * 3 + 1]),
								(normal_list[index * 3 + 2]) );

			index = normal_index[3 * i + 1];
			points[3 * i + 1] = vec3((normal_list[index * 3 + 0]),
									(normal_list[index * 3 + 1]),
									(normal_list[index * 3 + 2]) );
			
			index = normal_index[3 * i + 2];
			points[3 * i + 2] = vec3((normal_list[index * 3 + 0]),
									(normal_list[index * 3 + 1]),
									(normal_list[index * 3 + 2]) );
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------

		// -------------------- 实现颜色到shader的映射 ------------------------------
		points = new vec3[num_face * 3];
		// 逐面片存储每个面片的三个颜色
		for (int i = 0; i < num_face; i++)
		{
			int index = color_index[3 * i];
			points[3 * i] = vec3((color_list[index * 3 + 0]),
								(color_list[index * 3 + 1]),
								(color_list[index * 3 + 2]) );
			
			index = color_index[3 * i + 1];
			points[3 * i + 1] = vec3((color_list[index * 3 + 0]),
									(color_list[index * 3 + 1]),
									(color_list[index * 3 + 2]) );
			
			index = color_index[3 * i + 2];
			points[3 * i + 2] = vec3((color_list[index * 3 + 0]),
									(color_list[index * 3 + 1]),
									(color_list[index * 3 + 2]) );
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------

		// ------------------- 实现纹理坐标到shader的映射 -----------------------------
		vec2* points_2 = new vec2[num_face * 3];
		// 逐面片存储每个面片的三个纹理坐标
		for (int i = 0; i < num_face; i++)
		{
			int index = vt_index[3 * i];
			points_2[i * 3] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);
			
			index = vt_index[3 * i + 1];
			points_2[i * 3 + 1] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);

			index = vt_index[3 * i + 2];
			points_2[i * 3 + 2] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec2) * num_face * 3, points_2);
		offset += sizeof(vec2) * num_face * 3;
		delete[] points_2;
		// ------------------------------------------------------------------------

		// 加载着色器并使用生成的着色器程序
		offset = 0;
		// 指定vPosition在shader中使用时的位置
		GLuint vPosition;
		vPosition = glGetAttribLocation(mm->getProgram(), "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;

		// 指定vNormal在shader中使用时的位置
		GLuint vNormal;
		vNormal = glGetAttribLocation(mm->getProgram(), "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		
		// 指定vColor在shader中使用时的位置
		GLuint vColor;
		vColor = glGetAttribLocation(mm->getProgram(), "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		
		//指定vTexCoord在shader中使用时的位置
		//if(mm->get_withShadow())	// 假如是阴影，不需要纹理

		GLuint vTexCoord;
		vTexCoord = glGetAttribLocation(mm->getProgram(), "vTexCoord");
		glEnableVertexAttribArray(vTexCoord);
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));

}	
void Mesh_Painter::update_vertex_buffer()
{
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();

	// 顶点坐标，法线，颜色，纹理坐标到shader的映射
	for (unsigned int m_i = 0; m_i < this->m_my_meshes_.size(); m_i++)
	{
		int num_face = this->m_my_meshes_[m_i]->get_num_faces();
		int num_vertex = this->m_my_meshes_[m_i]->get_num_vertices();

		const std::vector<float>& vertex_list			= this->m_my_meshes_[m_i]->get_vertices();
		const std::vector<float>& normal_list			= this->m_my_meshes_[m_i]->get_normals();
		const std::vector<float>& color_list			= this->m_my_meshes_[m_i]->get_colors();
		const std::vector<float>& vt_list				= this->m_my_meshes_[m_i]->get_vts();
		const std::vector<unsigned int>& vertex_index	= this->m_my_meshes_[m_i]->get_vertice_index_();
		const std::vector<unsigned int>& normal_index	= this->m_my_meshes_[m_i]->get_normal_index();
		const std::vector<unsigned int>& color_index	= this->m_my_meshes_[m_i]->get_color_index();
		const std::vector<unsigned int>& vt_index		= this->m_my_meshes_[m_i]->get_vt_index();

		// 创建顶点数组对象
		GLuint vao;
		#ifdef __APPLE__
			glGenVertexArraysAPPLE(1, &vao);
			glBindVertexArrayAPPLE(vao);
		#else
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
		#endif

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER,
					sizeof(vec3)*num_face * 3		// 顶点坐标
					+ sizeof(vec3)*num_face * 3		// 法线
					+ sizeof(vec3)*num_face * 3		// 颜色
					+ sizeof(vec2)*num_face * 3,	// 贴图坐标
					NULL, GL_STATIC_DRAW);

		// ------------------------------------------------------------------------
		// 获得足够的空间存储坐标，颜色，法线以及纹理坐标等，并将它们映射给shader
		// Specify an offset to keep track of where we're placing data in our
		// vertex array buffer.  We'll use the same technique when we
		// associate the offsets with vertex attribute pointers.
		GLintptr offset = 0;
		point3f  p_center_;
		float d;
		// 判断是否进行归一化
		if (this->m_my_meshes_[m_i]->whether_to_normalize())
		{
			p_center_ = this->m_my_meshes_[m_i]->get_center();	// bounding box的中心点坐标
			point3f p_min_box_, p_max_box_;
			this->m_my_meshes_[m_i]->get_boundingbox(p_min_box_, p_max_box_);
			d = p_min_box_.distance(p_max_box_);				// bounding box的对角线长度
		}
		else
		{
			p_center_ = point3f(0.0, 0.0, 0.0);
			d = 1.0;
		}

		// -------------------- 实现顶点到shader的映射 ------------------------------
		vec3* points = new vec3[num_face * 3];
		// 逐面片存储每个面片的三个顶点坐标
		for (int i = 0; i < num_face; i++)
		{
			int index = vertex_index[3 * i]; // 数组标号从0开始
			points[3 * i] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,	// 平移到中心并缩放
								(vertex_list[index * 3 + 1] - p_center_.y) / d,
								(vertex_list[index * 3 + 2] - p_center_.z) / d );
			
			index = vertex_index[3 * i + 1];
			points[3 * i + 1] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
									(vertex_list[index * 3 + 1] - p_center_.y) / d,
									(vertex_list[index * 3 + 2] - p_center_.z) / d );
			
			index = vertex_index[3 * i + 2];
			points[3 * i + 2] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
									(vertex_list[index * 3 + 1] - p_center_.y) / d,
									(vertex_list[index * 3 + 2] - p_center_.z) / d );
			
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------
		
		// -------------------- 实现法线到shader的映射 ------------------------------
		points = new vec3[num_face * 3];
		// 逐面片存储每个面片的三个法线
		for (int i = 0; i < num_face; i++)
		{
			int index = normal_index[3 * i];
			points[3 * i] = vec3((normal_list[index * 3 + 0]),
								(normal_list[index * 3 + 1]),
								(normal_list[index * 3 + 2]) );

			index = normal_index[3 * i + 1];
			points[3 * i + 1] = vec3((normal_list[index * 3 + 0]),
									(normal_list[index * 3 + 1]),
									(normal_list[index * 3 + 2]) );
			
			index = normal_index[3 * i + 2];
			points[3 * i + 2] = vec3((normal_list[index * 3 + 0]),
									(normal_list[index * 3 + 1]),
									(normal_list[index * 3 + 2]) );
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------

		// -------------------- 实现颜色到shader的映射 ------------------------------
		points = new vec3[num_face * 3];
		// 逐面片存储每个面片的三个颜色
		for (int i = 0; i < num_face; i++)
		{
			int index = color_index[3 * i];
			points[3 * i] = vec3((color_list[index * 3 + 0]),
								(color_list[index * 3 + 1]),
								(color_list[index * 3 + 2]) );
			
			index = color_index[3 * i + 1];
			points[3 * i + 1] = vec3((color_list[index * 3 + 0]),
									(color_list[index * 3 + 1]),
									(color_list[index * 3 + 2]) );
			
			index = color_index[3 * i + 2];
			points[3 * i + 2] = vec3((color_list[index * 3 + 0]),
									(color_list[index * 3 + 1]),
									(color_list[index * 3 + 2]) );
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------

		// ------------------- 实现纹理坐标到shader的映射 -----------------------------
		vec2* points_2 = new vec2[num_face * 3];
		// 逐面片存储每个面片的三个纹理坐标
		for (int i = 0; i < num_face; i++)
		{
			int index = vt_index[3 * i];
			points_2[i * 3] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);
			
			index = vt_index[3 * i + 1];
			points_2[i * 3 + 1] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);

			index = vt_index[3 * i + 2];
			points_2[i * 3 + 2] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec2) * num_face * 3, points_2);
		offset += sizeof(vec2) * num_face * 3;
		delete[] points_2;
		// ------------------------------------------------------------------------

		// 加载着色器并使用生成的着色器程序
		offset = 0;
		// 指定vPosition在shader中使用时的位置
		GLuint vPosition;
		vPosition = glGetAttribLocation(this->program_all[m_i], "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;

		// 指定vNormal在shader中使用时的位置
		GLuint vNormal;
		vNormal = glGetAttribLocation(this->program_all[m_i], "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		
		// 指定vColor在shader中使用时的位置
		GLuint vColor;
		vColor = glGetAttribLocation(this->program_all[m_i], "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		
		//指定vTexCoord在shader中使用时的位置
		//if(m_my_meshes_[m_i]->get_withShadow())	// 假如是阴影，不需要纹理

		GLuint vTexCoord;
		vTexCoord = glGetAttribLocation(this->program_all[m_i], "vTexCoord");
		glEnableVertexAttribArray(vTexCoord);
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));		
		
		this->m_my_meshes_[m_i]->setVao(vao);
		this->m_my_meshes_[m_i]->setVbo(buffer);
		

		this->vao_all.push_back(vao);
		this->buffer_all.push_back(buffer);
		this->vPosition_all.push_back(vPosition);
		this->vColor_all.push_back(vColor);
		this->vTexCoord_all.push_back(vTexCoord);
		this->vNormal_all.push_back(vNormal);
	}
};

void Mesh_Painter::update_texture()
{
	this->textures_all.clear();

	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		GLuint textures;
		// 调用stb_image生成纹理
		glGenTextures(1, &textures);
		std::string texture_file;
		this->m_my_meshes_[i]->get_texture_file(texture_file);
		load_texture_STBImage(texture_file, textures);

		// 将生成的纹理传给shader
		glUniform1i(glGetUniformLocation(this->program_all[i], "texture"), 0);
		this->textures_all.push_back(textures);
	}
};

void Mesh_Painter::load_texture_STBImage(std::string file_name, GLuint& m_texName)
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

// 绘制，在display()中调用
void Mesh_Painter::draw_meshes()
{
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		// 指定使用渲染器，不同的模型可以指定不同的渲染器
		// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
		glUseProgram(this->program_all[i]);
		
		#ifdef __APPLE__
			glBindVertexArrayAPPLE(this->vao_all[i]);
		#else
			glBindVertexArray(this->vao_all[i]);
		#endif

 		glActiveTexture(GL_TEXTURE0);
 		glBindTexture(GL_TEXTURE_2D, this->textures_all[i]);// 该语句必须，否则将只使用同一个纹理进行绘制
		
		float x, y, z;
		this->m_my_meshes_[i]->get_theta(x, y, z);
		GLfloat  Theta[3] = {x, y, z};
		this->m_my_meshes_[i]->add_theta_step();
 		glUniform3fv(theta_all[i], 1, Theta);

		this->m_my_meshes_[i]->get_translate(x, y, z);
		GLfloat  vTranslation[3] = { x, y, z };
		glUniform3fv(trans_all[i], 1, vTranslation);

		//
		// if(m_my_meshes_[i]->get_withShadow())
		// {
		// 	GLuint modelViewID = glGetUniformLocation(this->program_all[i],"modelViewMatrix");
		// 	GLuint proID = glGetUniformLocation(this->program_all[i],"projectMatrix;");
		// 	mat4 mV = viewMatrix * modelMatrix;
		// 	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &mV[0][0]);
		// 	glUniformMatrix4fv(proID, 1, GL_TRUE, &projMatrix[0][0]);	
		// }
		// else
		modelViewMatrixID = glGetUniformLocation(this->program_all[i], "modelViewMatrix");
		
		mat4 modelViewMatrix = mat4(1.0);

		// 区分阴影与模型
		// if(m_my_meshes_[i]->get_withShadow())
		// 	{
		// 	modelViewMatrix = projMatrix * viewMatrix* shadowMatrix  * modelMatrix;
		// 	// GLuint scaleMatrixID = glGetUniformLocation(this->program_all[i],"scaleMatrix");
		// 	// mat4 sM = Scale(5,5,5);
		// 	// glUniformMatrix4fv(scaleMatrixID, 1, GL_TRUE, &sM[0][0]);
		// 	// GLuint lightPosID = glGetUniformLocation(this->program_all[i],"lightPos");
		// 	// glUniform3fv(lightPosID, 1, &lightP[0]);
		// 	}
		// else
		// 	modelViewMatrix = projMatrix * viewMatrix * modelMatrix;
		
		modelViewMatrix = projMatrix * viewMatrix * modelMatrix;

		//应该设置为modelMatrix
		GLuint changeID = glGetUniformLocation(this->program_all[i],"change");
		mat4 c = m_my_meshes_[i]->getModel();
		glUniformMatrix4fv(changeID,1, GL_TRUE,&c[0][0]);

		// 传入矩阵
		glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &modelViewMatrix[0][0]);

		if(m_my_meshes_[i]->get_withShadow())
		{
			point3f min_p,max_p;
			m_my_meshes_[i]->get_boundingbox(min_p,max_p);
			// 光源位置
			//10.02, 10.28, 10
			float lx = 10.02;
			float ly = 10.28;
			float lz = 10;
			lx = lightP[0];
			ly = lightP[1];
			lz = lightP[2];
			float tx,ty,tz;
			m_my_meshes_[i]->get_translate(tx,ty,tz);
			// 设置投影矩阵
			GLfloat n[] = { 0,1,0,0};	//(min_p.y+ty-6)
			mat4 sd = mat4(
				vec4(-ly*n[1] - lz*n[2], lx*n[1], lx*n[2], lx*n[3]),
				vec4(ly*n[0], -lx*n[0] - lz*n[2], ly*n[2], ly*n[3]),
				vec4(lz*n[0], lz*n[1], -lx*n[0] - ly*n[1], lx*n[3]),
				vec4(0, 0, 0, -lx*n[0] - ly*n[1] - lz*n[2]));
			mat4 ssd = mat4(
				vec4(ly*n[1] +lz*n[2], -lx*n[1], -lx*n[2], -lx*n[3]),
				vec4(-ly*n[0], +lx*n[0] +lz*n[2], -ly*n[2], -ly*n[3]),
				vec4(-lz*n[0], -lz*n[1], lx*n[0] + ly*n[1], -lx*n[3]),
				vec4(0, 0, 0, lx*n[0]+  ly*n[1] + lz*n[2]));
				//std::cout<<"minp_y:"<<min_p.y+ty<<std::endl;
			GLuint shadowID = glGetUniformLocation(this->program_all[i],"shadowMatrix");
			glUniformMatrix4fv(shadowID, 1, GL_TRUE, &ssd[0][0]);
			
			GLuint modelID = glGetUniformLocation(this->program_all[i],"modelMatrix");
			GLuint viewID = glGetUniformLocation(this->program_all[i],"viewMatrix");
			GLuint proID = glGetUniformLocation(this->program_all[i],"projMatrix");
			glUniformMatrix4fv(modelID, 1, GL_TRUE, &modelMatrix[0][0]);
			glUniformMatrix4fv(viewID, 1, GL_TRUE, &viewMatrix[0][0]);
			glUniformMatrix4fv(proID, 1, GL_TRUE, &projMatrix[0][0]);

		}
		glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->get_num_faces() * 3);
		glUseProgram(0);
	}
};

void Mesh_Painter::dele_mesh()
{
	this->m_my_meshes_.erase(this->m_my_meshes_.end());
}
void Mesh_Painter::clear_meshes()
{
	this->m_my_meshes_.clear();
	this->textures_all.clear();
	this->program_all.clear();
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();
};
