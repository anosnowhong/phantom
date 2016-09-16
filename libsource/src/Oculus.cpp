/******************************************************************************/
/*                                                                            */
/* MODULE  : Oculus.cpp                                                       */
/*                                                                            */
/* PURPOSE : Oculus Rift HMD module.                                          */
/*                                                                            */
/* DATE    : 09/Jan/2015                                                      */
/*                                                                            */
/* CHANGES                                                                    */
/*                                                                            */
/* V1.0  JNI 09/Jan/2015 - Initial development.                               */
/*                                                                            */
/* V1.1  JNI 10/Sep/2015 - Re-visit to solve rotation / workspace issue.      */
/*                                                                            */
/******************************************************************************/

#define MODULE_NAME     "OCULUS"
#define MODULE_TEXT     "Oculus Rift HMD Module"
#define MODULE_DATE     "10/09/2015"
#define MODULE_VERSION  "1.1"
#define MODULE_LEVEL    3

/******************************************************************************/

#include <motor.h>

/******************************************************************************/

struct DepthBuffer
{
	GLuint        texId;

	DepthBuffer(ovrSizei size, int sampleCount)
	{
		//OVR_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GLenum internalFormat = GL_DEPTH_COMPONENT24;
		GLenum type = GL_UNSIGNED_INT;
		/*
		if (GLE_ARB_depth_buffer_float)
		{
		internalFormat = GL_DEPTH_COMPONENT32F;
		type = GL_FLOAT;
		}
		*/

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.w, size.h, 0, GL_DEPTH_COMPONENT, type, NULL);
	}
	~DepthBuffer()
	{
		if (texId)
		{
			glDeleteTextures(1, &texId);
			texId = 0;
		}
	}
};

struct TextureBuffer
{
	ovrHmd              hmd;
	ovrSession			session;
	ovrSwapTextureSet*  TextureSet;
	GLuint              texId;
	GLuint              fboId;
	ovrSizei               texSize;

	TextureBuffer(ovrHmd hmd, bool rendertarget, bool displayableOnHmd, ovrSizei size, int mipLevels, unsigned char * data, int sampleCount) :
		hmd(hmd),
		TextureSet(nullptr),
		texId(0),
		fboId(0)
		//texSize(0, 0)
	{
		//OVR_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

		texSize = size;

		if (displayableOnHmd)
		{
			// This texture isn't necessarily going to be a rendertarget, but it usually is.
			//OVR_ASSERT(hmd); // No HMD? A little odd.
			//OVR_ASSERT(sampleCount == 1); // ovr_CreateSwapTextureSetD3D11 doesn't support MSAA.

			ovrResult result = ovr_CreateSwapTextureSetGL(hmd, GL_SRGB8_ALPHA8, size.w, size.h, &TextureSet);

			if (OVR_SUCCESS(result))
			{
				for (int i = 0; i < TextureSet->TextureCount; ++i)
				{
					ovrGLTexture* tex = (ovrGLTexture*)&TextureSet->Textures[i];
					glBindTexture(GL_TEXTURE_2D, tex->OGL.TexId);

					if (rendertarget)
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					}
					else
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					}
				}
			}
		}
		else
		{
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);

			if (rendertarget)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texSize.w, texSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

		if (mipLevels > 1)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glGenFramebuffers(1, &fboId);
	}

	~TextureBuffer()
	{
		if (TextureSet)
		{
			ovr_DestroySwapTextureSet(hmd, TextureSet);
			TextureSet = nullptr;
		}
		if (texId)
		{
			glDeleteTextures(1, &texId);
			texId = 0;
		}
		if (fboId)
		{
			glDeleteFramebuffers(1, &fboId);
			fboId = 0;
		}
	}

	ovrSizei GetSize() const
	{
		return texSize;
	}

	void SetAndClearRenderSurface(DepthBuffer* dbuffer)
	{
		auto tex = reinterpret_cast<ovrGLTexture*>(&TextureSet->Textures[TextureSet->CurrentIndex]);

		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->OGL.TexId, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dbuffer->texId, 0);

		glViewport(0, 0, texSize.w, texSize.h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_FRAMEBUFFER_SRGB);
	}

	void UnsetRenderSurface()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	}
};

struct VertexBuffer
{
	GLuint    buffer;

	VertexBuffer(void* vertices, size_t size)
	{
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}
	~VertexBuffer()
	{
		if (buffer)
		{
			glDeleteBuffers(1, &buffer);
			buffer = 0;
		}
	}
};

struct IndexBuffer
{
	GLuint    buffer;

	IndexBuffer(void* indices, size_t size)
	{
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}
	~IndexBuffer()
	{
		if (buffer)
		{
			glDeleteBuffers(1, &buffer);
			buffer = 0;
		}
	}
};

struct ShaderFill
{
	GLuint            program;
	TextureBuffer   * texture;

	ShaderFill(GLuint vertexShader, GLuint pixelShader, TextureBuffer* _texture)
	{
		texture = _texture;

		program = glCreateProgram();

		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);

		glLinkProgram(program);

		glDetachShader(program, vertexShader);
		glDetachShader(program, pixelShader);

		GLint r;
		glGetProgramiv(program, GL_LINK_STATUS, &r);
		if (!r)
		{
			GLchar msg[1024];
			glGetProgramInfoLog(program, sizeof(msg), 0, msg);
			//OVR_DEBUG_LOG(("Linking shaders failed: %s\n", msg));
		}
	}

	~ShaderFill()
	{
		if (program)
		{
			glDeleteProgram(program);
			program = 0;
		}
		if (texture)
		{
			delete texture;
			texture = nullptr;
		}
	}
};

struct Model
{
	struct Vertex
	{
		Vector3f  Pos;
		DWORD     C;
		float     U, V;
	};

	Vector3f        Pos;
	OVR::Quatf           Rot;
	Matrix4f        Mat;
	int             numVertices, numIndices;
	Vertex          Vertices[2000]; // Note fixed maximum
	GLushort        Indices[2000];
	ShaderFill    * Fill;
	VertexBuffer  * vertexBuffer;
	IndexBuffer   * indexBuffer;

	Model(Vector3f pos, ShaderFill * fill) :
		numVertices(0),
		numIndices(0),
		Pos(pos),
		Rot(),
		Mat(),
		Fill(fill),
		vertexBuffer(nullptr),
		indexBuffer(nullptr)
	{}

	~Model()
	{
		FreeBuffers();
	}

	Matrix4f& GetMatrix()
	{
		Mat = Matrix4f(Rot);
		Mat = Matrix4f::Translation(Pos) * Mat;
		return Mat;
	}

	void AddVertex(const Vertex& v) { Vertices[numVertices++] = v; }
	void AddIndex(GLushort a) { Indices[numIndices++] = a; }
	void AllocateBuffers()
	{
		vertexBuffer = new VertexBuffer(&Vertices[0], numVertices * sizeof(Vertices[0]));
		indexBuffer = new IndexBuffer(&Indices[0], numIndices * sizeof(Indices[0]));
	}

	void FreeBuffers()
	{
		delete vertexBuffer; vertexBuffer = nullptr;
		delete indexBuffer; indexBuffer = nullptr;
	}

	void AddSolidColorBox(float x1, float y1, float z1, float x2, float y2, float z2, DWORD c)
	{
		Vector3f Vert[][2] =
		{
			Vector3f(x1, y2, z1), Vector3f(z1, x1), Vector3f(x2, y2, z1), Vector3f(z1, x2),
			Vector3f(x2, y2, z2), Vector3f(z2, x2), Vector3f(x1, y2, z2), Vector3f(z2, x1),
			Vector3f(x1, y1, z1), Vector3f(z1, x1), Vector3f(x2, y1, z1), Vector3f(z1, x2),
			Vector3f(x2, y1, z2), Vector3f(z2, x2), Vector3f(x1, y1, z2), Vector3f(z2, x1),
			Vector3f(x1, y1, z2), Vector3f(z2, y1), Vector3f(x1, y1, z1), Vector3f(z1, y1),
			Vector3f(x1, y2, z1), Vector3f(z1, y2), Vector3f(x1, y2, z2), Vector3f(z2, y2),
			Vector3f(x2, y1, z2), Vector3f(z2, y1), Vector3f(x2, y1, z1), Vector3f(z1, y1),
			Vector3f(x2, y2, z1), Vector3f(z1, y2), Vector3f(x2, y2, z2), Vector3f(z2, y2),
			Vector3f(x1, y1, z1), Vector3f(x1, y1), Vector3f(x2, y1, z1), Vector3f(x2, y1),
			Vector3f(x2, y2, z1), Vector3f(x2, y2), Vector3f(x1, y2, z1), Vector3f(x1, y2),
			Vector3f(x1, y1, z2), Vector3f(x1, y1), Vector3f(x2, y1, z2), Vector3f(x2, y1),
			Vector3f(x2, y2, z2), Vector3f(x2, y2), Vector3f(x1, y2, z2), Vector3f(x1, y2)
		};

		GLushort CubeIndices[] =
		{
			0, 1, 3, 3, 1, 2,
			5, 4, 6, 6, 4, 7,
			8, 9, 11, 11, 9, 10,
			13, 12, 14, 14, 12, 15,
			16, 17, 19, 19, 17, 18,
			21, 20, 22, 22, 20, 23
		};

		for (int i = 0; i < sizeof(CubeIndices) / sizeof(CubeIndices[0]); ++i)
			AddIndex(CubeIndices[i] + GLushort(numVertices));

		// Generate a quad for each box face
		for (int v = 0; v < 6 * 4; v++)
		{
			// Make vertices, with some token lighting
			Vertex vvv; vvv.Pos = Vert[v][0]; vvv.U = Vert[v][1].x; vvv.V = Vert[v][1].y;
			float dist1 = (vvv.Pos - Vector3f(-2, 4, -2)).Length();
			float dist2 = (vvv.Pos - Vector3f(3, 4, -3)).Length();
			float dist3 = (vvv.Pos - Vector3f(-4, 3, 25)).Length();
			int   bri = rand() % 160;
			float B = ((c >> 16) & 0xff) * (bri + 192.0f * (0.65f + 8 / dist1 + 1 / dist2 + 4 / dist3)) / 255.0f;
			float G = ((c >> 8) & 0xff) * (bri + 192.0f * (0.65f + 8 / dist1 + 1 / dist2 + 4 / dist3)) / 255.0f;
			float R = ((c >> 0) & 0xff) * (bri + 192.0f * (0.65f + 8 / dist1 + 1 / dist2 + 4 / dist3)) / 255.0f;
			vvv.C = (c & 0xff000000) +
				((R > 255 ? 255 : DWORD(R)) << 16) +
				((G > 255 ? 255 : DWORD(G)) << 8) +
				(B > 255 ? 255 : DWORD(B));
			AddVertex(vvv);
		}
	}

	void AddSphere()
	{
	}

	void Render(Matrix4f view, Matrix4f proj)
	{
		Matrix4f combined = proj * view * GetMatrix();

		glUseProgram(Fill->program);
		glUniform1i(glGetUniformLocation(Fill->program, "Texture0"), 0);
		glUniformMatrix4fv(glGetUniformLocation(Fill->program, "matWVP"), 1, GL_TRUE, (FLOAT*)&combined);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Fill->texture->texId);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->buffer);

		GLuint posLoc = glGetAttribLocation(Fill->program, "Position");
		GLuint colorLoc = glGetAttribLocation(Fill->program, "Color");
		GLuint uvLoc = glGetAttribLocation(Fill->program, "TexCoord");

		glEnableVertexAttribArray(posLoc);
		glEnableVertexAttribArray(colorLoc);
		glEnableVertexAttribArray(uvLoc);

		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, Pos));
		glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, C));
		glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, U));

		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, NULL);

		glDisableVertexAttribArray(posLoc);
		glDisableVertexAttribArray(colorLoc);
		glDisableVertexAttribArray(uvLoc);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
	}
};

struct Scene
{
	int     numModels;
	Model * Models[10];

	void    Add(Model * n)
	{
		Models[numModels++] = n;
	}

	void Render(Matrix4f view, Matrix4f proj)
	{
		for (int i = 0; i < numModels; ++i)
			Models[i]->Render(view, proj);
	}

	GLuint CreateShader(GLenum type, const GLchar* src)
	{
		GLuint shader = glCreateShader(type);

		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);

		GLint r;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
		if (!r)
		{
			GLchar msg[1024];
			glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
			if (msg[0]) {
				//OVR_DEBUG_LOG(("Compiling shader failed: %s\n", msg));
			}
			return 0;
		}

		return shader;
	}

	void Init(int includeIntensiveGPUobject)
	{
		static const GLchar* VertexShaderSrc =
			"#version 150\n"
			"uniform mat4 matWVP;\n"
			"in      vec4 Position;\n"
			"in      vec4 Color;\n"
			"in      vec2 TexCoord;\n"
			"out     vec2 oTexCoord;\n"
			"out     vec4 oColor;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = (matWVP * Position);\n"
			"   oTexCoord   = TexCoord;\n"
			"   oColor.rgb  = pow(Color.rgb, vec3(2.2));\n"   // convert from sRGB to linear
			"   oColor.a    = Color.a;\n"
			"}\n";

		static const char* FragmentShaderSrc =
			"#version 150\n"
			"uniform sampler2D Texture0;\n"
			"in      vec4      oColor;\n"
			"in      vec2      oTexCoord;\n"
			"out     vec4      FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = oColor * texture2D(Texture0, oTexCoord);\n"
			"}\n";

		GLuint    vshader = CreateShader(GL_VERTEX_SHADER, VertexShaderSrc);
		GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);

		// Make textures
		ShaderFill * grid_material[4];
		for (int k = 0; k < 4; ++k)
		{
			static DWORD tex_pixels[256 * 256];
			for (int j = 0; j < 256; ++j)
			{
				for (int i = 0; i < 256; ++i)
				{
					if (k == 0) tex_pixels[j * 256 + i] = (((i >> 7) ^ (j >> 7)) & 1) ? 0xffb4b4b4 : 0xff505050;// floor
					if (k == 1) tex_pixels[j * 256 + i] = (((j / 4 & 15) == 0) || (((i / 4 & 15) == 0) && ((((i / 4 & 31) == 0) ^ ((j / 4 >> 4) & 1)) == 0)))
						? 0xff3c3c3c : 0xffb4b4b4;// wall
					if (k == 2) tex_pixels[j * 256 + i] = (i / 4 == 0 || j / 4 == 0) ? 0xff505050 : 0xffb4b4b4;// ceiling
					if (k == 3) tex_pixels[j * 256 + i] = 0xffffffff;// blank
				}
			}
			TextureBuffer * generated_texture = new TextureBuffer(nullptr, false, false, OVR::Sizei(256, 256), 4, (unsigned char *)tex_pixels, 1);
			grid_material[k] = new ShaderFill(vshader, fshader, generated_texture);
		}

		glDeleteShader(vshader);
		glDeleteShader(fshader);

		// Construct geometry
		Model * m = new Model(Vector3f(0, 0, 0), grid_material[2]);  // Moving box
		m->AddSolidColorBox(0, 0, 0, +1.0f, +1.0f, 1.0f, 0xff404040);
		m->AllocateBuffers();
		Add(m);

		m = new Model(Vector3f(0, 0, 0), grid_material[1]);  // Walls
		m->AddSolidColorBox(-10.0f, -0.1f, -20.1f, 10.0f, 4.0f, -20.0f, 0xff808080); // Back Wall
		m->AddSolidColorBox(10.0f, -0.1f, -20.0f, 10.1f, 4.0f, 20.0f, 0xff808080); // Right Wall
		m->AllocateBuffers();
		Add(m);

		/*
		if (includeIntensiveGPUobject)
		{
			m = new Model(Vector3f(0, 0, 0), grid_material[0]);  // Floors
			for (float depth = 0.0f; depth > -3.0f; depth -= 0.1f)
				m->AddSolidColorBox(9.0f, 0.5f, -depth, -9.0f, 3.5f, -depth, 0x10ff80ff); // Partition
			m->AllocateBuffers();
			Add(m);
		}

		m = new Model(Vector3f(0, 0, 0), grid_material[0]);  // Floors
		m->AddSolidColorBox(-10.0f, -0.1f, -20.0f, 10.0f, 0.0f, 20.1f, 0xff808080); // Main floor
		m->AddSolidColorBox(-15.0f, -6.1f, 18.0f, 15.0f, -6.0f, 30.0f, 0xff808080); // Bottom floor
		m->AllocateBuffers();
		Add(m);

		m = new Model(Vector3f(0, 0, 0), grid_material[2]);  // Ceiling
		m->AddSolidColorBox(-10.0f, 4.0f, -20.0f, 10.0f, 4.1f, 20.1f, 0xff808080);
		m->AllocateBuffers();
		Add(m);

		m = new Model(Vector3f(0, 0, 0), grid_material[3]);  // Fixtures & furniture
		m->AddSolidColorBox(9.5f, 0.75f, 3.0f, 10.1f, 2.5f, 3.1f, 0xff383838);   // Right side shelf// Verticals
		m->AddSolidColorBox(9.5f, 0.95f, 3.7f, 10.1f, 2.75f, 3.8f, 0xff383838);   // Right side shelf
		m->AddSolidColorBox(9.55f, 1.20f, 2.5f, 10.1f, 1.30f, 3.75f, 0xff383838); // Right side shelf// Horizontals
		m->AddSolidColorBox(9.55f, 2.00f, 3.05f, 10.1f, 2.10f, 4.2f, 0xff383838); // Right side shelf
		m->AddSolidColorBox(5.0f, 1.1f, 20.0f, 10.0f, 1.2f, 20.1f, 0xff383838);   // Right railing   
		m->AddSolidColorBox(-10.0f, 1.1f, 20.0f, -5.0f, 1.2f, 20.1f, 0xff383838);   // Left railing  
		for (float f = 5.0f; f <= 9.0f; f += 1.0f)
		{
			m->AddSolidColorBox(f, 0.0f, 20.0f, f + 0.1f, 1.1f, 20.1f, 0xff505050);// Left Bars
			m->AddSolidColorBox(-f, 1.1f, 20.0f, -f - 0.1f, 0.0f, 20.1f, 0xff505050);// Right Bars
		}
		m->AddSolidColorBox(-1.8f, 0.8f, 1.0f, 0.0f, 0.7f, 0.0f, 0xff505000); // Table
		m->AddSolidColorBox(-1.8f, 0.0f, 0.0f, -1.7f, 0.7f, 0.1f, 0xff505000); // Table Leg 
		m->AddSolidColorBox(-1.8f, 0.7f, 1.0f, -1.7f, 0.0f, 0.9f, 0xff505000); // Table Leg 
		m->AddSolidColorBox(0.0f, 0.0f, 1.0f, -0.1f, 0.7f, 0.9f, 0xff505000); // Table Leg 
		m->AddSolidColorBox(0.0f, 0.7f, 0.0f, -0.1f, 0.0f, 0.1f, 0xff505000); // Table Leg 
		m->AddSolidColorBox(-1.4f, 0.5f, -1.1f, -0.8f, 0.55f, -0.5f, 0xff202050); // Chair Set
		m->AddSolidColorBox(-1.4f, 0.0f, -1.1f, -1.34f, 1.0f, -1.04f, 0xff202050); // Chair Leg 1
		m->AddSolidColorBox(-1.4f, 0.5f, -0.5f, -1.34f, 0.0f, -0.56f, 0xff202050); // Chair Leg 2
		m->AddSolidColorBox(-0.8f, 0.0f, -0.5f, -0.86f, 0.5f, -0.56f, 0xff202050); // Chair Leg 2
		m->AddSolidColorBox(-0.8f, 1.0f, -1.1f, -0.86f, 0.0f, -1.04f, 0xff202050); // Chair Leg 2
		m->AddSolidColorBox(-1.4f, 0.97f, -1.05f, -0.8f, 0.92f, -1.10f, 0xff202050); // Chair Back high bar
		*/

		/*
		for (float f = 3.0f; f <= 6.6f; f += 0.4f)
			m->AddSolidColorBox(-3, 0.0f, f, -2.9f, 1.3f, f + 0.1f, 0xff404040); // Posts
			*/

		m->AllocateBuffers();
		Add(m);
	}

	Scene() : numModels(0) {}

	Scene(bool includeIntensiveGPUobject) :
		numModels(0)
	{
		Init(includeIntensiveGPUobject);
	}

	void Release()
	{
		while (numModels-- > 0)
			delete Models[numModels];
	}

	~Scene()
	{
		Release();
	}
};

/******************************************************************************/

BOOL    OCULUS_API_started = FALSE;

/******************************************************************************/

PRINTF  OCULUS_PRN_messgf = NULL;                 // General messages printf function.
PRINTF  OCULUS_PRN_errorf = NULL;                 // Error messages printf function.
PRINTF  OCULUS_PRN_debugf = NULL;                 // Debug information printf function.

/******************************************************************************/
/* API print functions for different message types...                         */
/******************************************************************************/

int OCULUS_messgf(const char *mask, ...)
{
	va_list args;
	static  char  buff[PRNTBUFF];

	va_start(args, mask);
	vsprintf(buff, mask, args);
	va_end(args);

	return(STR_printf(OCULUS_PRN_messgf, buff));
}

/******************************************************************************/

int OCULUS_errorf(const char *mask, ...)
{
	va_list args;
	static  char  buff[PRNTBUFF];

	va_start(args, mask);
	vsprintf(buff, mask, args);
	va_end(args);

	return(STR_printf(OCULUS_PRN_errorf, buff));
}

/******************************************************************************/

int OCULUS_debugf(const char *mask, ...)
{
	va_list args;
	static  char  buff[PRNTBUFF];

	va_start(args, mask);
	vsprintf(buff, mask, args);
	va_end(args);

	return(STR_printf(OCULUS_PRN_debugf, buff));
}

/******************************************************************************/

BOOL OCULUS_API_start(PRINTF messgf, PRINTF errorf, PRINTF debugf)
{
	if (OCULUS_API_started)               // Start the API once...
	{
		return(TRUE);
	}

	OCULUS_PRN_messgf = messgf;            // General API message print function.
	OCULUS_PRN_errorf = errorf;            // API error message print function.
	OCULUS_PRN_debugf = debugf;            // Debug information print function.

	ATEXIT_API(OCULUS_API_stop);           // Install stop function.
	OCULUS_API_started = TRUE;             // Set started flag.

	MODULE_start(OCULUS_PRN_messgf);       // Register module.

	return(TRUE);
}

/******************************************************************************/

void OCULUS_API_stop(void)
{
	if (!OCULUS_API_started)          // API not started in the first place...
	{
		return;
	}

	OCULUS_Close();

	OCULUS_API_started = FALSE;        // Clear started flag.
	MODULE_stop();                     // Register module stop.
}

/******************************************************************************/

BOOL OCULUS_Check(void)
{
	BOOL ok = TRUE;

	// Start module automatically...
	if (!OCULUS_API_started)
	{
		//ok = OCULUS_API_start(MODULE_messgf(),MODULE_errorf(),MODULE_debugf());
		ok = OCULUS_API_start(printf, printf, NULL);
		OCULUS_debugf("OCULUS_Check() Start %s.\n", ok ? "OK" : "Failed");
	}

	return(ok);
}

/******************************************************************************/

STRING OCULUS_Name = "";
BOOL   OCULUS_OpenFlag = FALSE;
int    OCULUS_WinWidth, OCULUS_WinHeight;
int    OCULUS_FB_Width, OCULUS_FB_Height;

GLuint OCULUS_FB_Object = 0;
GLuint OCULUS_FB_Texture = 0;
GLuint OCULUS_FB_Depth = 0;

int    OCULUS_FB_TextureWidth, OCULUS_FB_TextureHeight;
BOOL   OCULUS_GridFlag = TRUE;
float  OCULUS_RotateX = 0.0;
float  OCULUS_Scale = 1.0;
BOOL   OCULUS_StartOnHMD = TRUE;
float  OCULUS_EyeSeparation = 0.03;
float  OCULUS_DisplaySize[GRAPHICS_3D][GRAPHICS_RANGE];

matrix OCULUS_Size(3, 1);
matrix OCULUS_Centre(3, 1);

BOOL   OCULUS_DisplayedOnHMD = FALSE;
int    OCULUS_DisplayPrimaryX, OCULUS_DisplayPrimaryY;
int    OCULUS_DisplaySecondaryX, OCULUS_DisplaySecondaryY;

//ovrHmd            OCULUS_Handle=NULL;
ovrEyeRenderDesc  OCULUS_EyeRendDesc[2];
ovrGLTexture      OCULUS_Texture[2];
ovrSizei          OCULUS_EyeResolution[2];

ovrSession		  session;
ovrHmdDesc		  HmdDesc;
ovrResult		  result;
ovrGraphicsLuid   luid;

ovrSizei		  windowSize_mirror;
GLuint			  FBO_mirror = 0;
ovrGLTexture*     Texture_mirror = nullptr;
//Scene *roomScene = nullptr;

float Yaw(3.141592f);
float Pitch(0.0f);
Vector3f Pos2(0.0f, 1.6f, -5.0f);
//Vector3f Pos2(0.0f, 0.0f, 0.0f);
Matrix4f combined;
Matrix4f  view;

TIMER_Frequency   OCULUS_DisplayFrequencyTimer("OCULUS_DisplayFrequency");
TIMER_Interval    OCULUS_DisplayLatencyTimer("OCULUS_DisplayLatency");
TIMER_Interval    OCULUS_TextLatencyTimer("OCULUS_TextLatency");
TIMER_Interval    OCULUS_DrawLatencyTimer("OCULUS_DrawLatency");
TIMER_Interval    OCULUS_DrawGridLatencyTimer("OCULUS_DrawGridLatency");
TIMER_Interval    OCULUS_BeginFrameLatencyTimer("OCULUS_BeginFrameLatency");
TIMER_Interval    OCULUS_EndFrameLatencyTimer("OCULUS_EndFrameLatency");
TIMER_Interval    OCULUS_EyeLoopLatencyTimer("OCULUS_EyeLoopLatency");

/******************************************************************************/

BOOL OCULUS_Config(char *file)
{
	int i, j;

	for (i = 0; (i < GRAPHICS_3D); i++)
	{
		for (j = 0; (j < GRAPHICS_RANGE); j++)
		{
			OCULUS_DisplaySize[i][j] = 0.0;
		}
	}

	CONFIG_reset();

	// Set up variable table for configuration...
	CONFIG_set("Xmin,Xmax", OCULUS_DisplaySize[GRAPHICS_X], GRAPHICS_RANGE);
	CONFIG_set("Ymin,Ymax", OCULUS_DisplaySize[GRAPHICS_Y], GRAPHICS_RANGE);
	CONFIG_set("Zmin,Zmax", OCULUS_DisplaySize[GRAPHICS_Z], GRAPHICS_RANGE);
	CONFIG_set("RotateX", OCULUS_RotateX);
	CONFIG_set("Scale", OCULUS_Scale);
	CONFIG_setBOOL("StartOnHMD", OCULUS_StartOnHMD);
	CONFIG_set("EyeSeparation", OCULUS_EyeSeparation);
	CONFIG_setBOOL("GridFlag", OCULUS_GridFlag);

	// Load configuration file...
	if (!CONFIG_read(file))
	{
		OCULUS_errorf("OCULUS_Config(%s) Cannot read file.\n", file);
		return(FALSE);
	}

	OCULUS_debugf("OCULUS_Config(%s) Loaded.\n", file);
	CONFIG_list(OCULUS_debugf);

	return(TRUE);
}

/******************************************************************************/

BOOL OCULUS_Diagnose(ovrResult error)
{
	if (OVR_FAILURE(error)){
		ovrErrorInfo error_info;
		ovr_GetLastErrorInfo(&error_info);
		OCULUS_debugf("[ERROR] %s", error_info.ErrorString);
		return FALSE;
	}
	else
		return TRUE;
}
/***************************************/

/***************************************/
BOOL OCULUS_Open(void)
{
	char *path = NULL, *file = OCULUS_CONFIG;

	if (!OCULUS_Check())
	{
		return(FALSE);
	}

	if ((path = FILE_Calibration(file)) == NULL)
	{
		OCULUS_errorf("OCULUS_Open() %s file not found.\n", file);
		return(FALSE);
	}

	// Load configuration file...
	if (!OCULUS_Config(path))
	{
		return(FALSE);
	}

	result = ovr_Initialize(nullptr);
	if (OCULUS_Diagnose(result) == FALSE)
	{
		OCULUS_errorf("OCULUS_Open() Cannot init Oculus HMD\n");
		OCULUS_Close();
		return(FALSE);
	}

	result = ovr_Create(&session, &luid);
	if (OCULUS_Diagnose(result) == FALSE)
	{
		OCULUS_errorf("OCULUS_Open() Cannot open Oculus HMD\n");
		OCULUS_Close();
		return(FALSE);
	}

	HmdDesc = ovr_GetHmdDesc(session);

	strncpy(OCULUS_Name, STR_stringf("%s %s", HmdDesc.Manufacturer, HmdDesc.ProductName), STRLEN);

	OCULUS_messgf("OCULUS_Open() HMD Initialized [%s]\n", OCULUS_Name);
	OCULUS_messgf("OCULUS_Open() Resolution %d x %d pixels.\n", HmdDesc.Resolution.w, HmdDesc.Resolution.h);

	OCULUS_OpenFlag = TRUE;

	return(TRUE);
}

/******************************************************************************/

BOOL OCULUS_Opened(void)
{
	BOOL flag;

	if (OCULUS_Check())
	{
		flag = OCULUS_OpenFlag;
	}

	return(flag);
}

/******************************************************************************/

void OCULUS_Close(void)
{
	if (!OCULUS_Check())
	{
		return;
	}

	OCULUS_Stop();

	if (session != NULL)
	{
		ovr_Destroy(session);
		session = NULL;
	}

	ovr_Shutdown();

	OCULUS_OpenFlag = FALSE;
}

/******************************************************************************/

void OCULUS_RenderTarget(int width, int height)
{
	// Create (or re-size) render target used to draw the two stereo views.

	// Creat FBO if it does not already exist.
	if (!OCULUS_FB_Object)
	{
		glGenFramebuffers(1, &OCULUS_FB_Object);
		glGenTextures(1, &OCULUS_FB_Texture);
		glGenRenderbuffers(1, &OCULUS_FB_Depth);
		glBindTexture(GL_TEXTURE_2D, OCULUS_FB_Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, OCULUS_FB_Object);

	// Calculate the next power of two in both dimensions and use that as a texture size */
	OCULUS_FB_TextureWidth = OCULUS_NextPower2(width);
	OCULUS_FB_TextureHeight = OCULUS_NextPower2(height);

	// Create and attach the texture that will be used as a color buffer.
	glBindTexture(GL_TEXTURE_2D, OCULUS_FB_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, OCULUS_FB_TextureWidth, OCULUS_FB_TextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, OCULUS_FB_Texture, 0);

	// Create and attach the renderbuffer that will serve as our z-buffer */
	glBindRenderbuffer(GL_RENDERBUFFER, OCULUS_FB_Depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, OCULUS_FB_TextureWidth, OCULUS_FB_TextureHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, OCULUS_FB_Depth);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		OCULUS_errorf("OCULUS_RenderTarget() Incomplete framebuffer.\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	OCULUS_messgf("OCULUS_RenderTarget() Render target: %d x %d (texture size: %d x %d)\n", width, height, OCULUS_FB_TextureWidth, OCULUS_FB_TextureHeight);
}

/******************************************************************************/
TextureBuffer* eyeRenderTexture[2] = { nullptr, nullptr };
DepthBuffer   * eyeDepthBuffer[2] = { nullptr, nullptr };
//===============================

//Scene *roomScene = nullptr;

BOOL OCULUS_Start(void)
{
	int i, x, y;
	unsigned int flags, dcaps;

	if (!OCULUS_Opened())
	{
		OCULUS_errorf("OCULUS_Start() HMD not opened.\n");
		return(FALSE);
	}

	if (OCULUS_StartOnHMD)
	{
		OCULUS_DisplayOnHMD();
	}
	else
	{
		OCULUS_DisplayOnDeskTop();
	}

	glewInit();

	windowSize_mirror.w = HmdDesc.Resolution.w / 2;
	windowSize_mirror.h = HmdDesc.Resolution.h / 2;

	for (int eye = 0; eye < 2; ++eye)
	{
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(session, ovrEyeType(eye), HmdDesc.DefaultEyeFov[eye], 1.0f);
		OCULUS_EyeResolution[eye] = idealTextureSize;
		eyeRenderTexture[eye] = new TextureBuffer(session, true, true, idealTextureSize, 1, NULL, 1);
		eyeDepthBuffer[eye] = new DepthBuffer(eyeRenderTexture[eye]->GetSize(), 0);

		if (!eyeRenderTexture[eye]->TextureSet)
		{
			OCULUS_errorf("OCULUS_Start() failed to create texture.\n");
			return FALSE;
		}
	}

	for (i = 0; (i < 2); i++)
	{
		OCULUS_messgf("OCULUS_Start() Eye[%d] Resolution: wid=%d hgt=%d\n", i, OCULUS_EyeResolution[i].w, OCULUS_EyeResolution[i].h);
	}

	// Create a single render target texture to encompass both eyes
	OCULUS_FB_Width = OCULUS_EyeResolution[0].w + OCULUS_EyeResolution[1].w;
	OCULUS_FB_Height = (OCULUS_EyeResolution[0].h > OCULUS_EyeResolution[1].h) ? OCULUS_EyeResolution[0].h : OCULUS_EyeResolution[1].h;
	//******************************************************************

	//create mirror texture and FBO to copy mirror texture to back buffer
	result = ovr_CreateMirrorTextureGL(session, GL_SRGB8_ALPHA8, windowSize_mirror.w, windowSize_mirror.h, reinterpret_cast<ovrTexture**>(&Texture_mirror));
	if (OCULUS_Diagnose(result) == FALSE)
	{
		OCULUS_errorf("OCULUS_Start() Cannot open create mirror texture and FBO\n");
		OCULUS_Close();
		return FALSE;
	}

	OCULUS_EyeRendDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left, HmdDesc.DefaultEyeFov[0]);
	OCULUS_EyeRendDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, HmdDesc.DefaultEyeFov[1]);

	//******************************************************************
	//******************************************************************
	for (int eye = 0; (eye < 2); eye++)
	{
		OCULUS_messgf("OCULUS_Start() Eye[%d]: x=%.2lf,y=%.2lf,z=%.2lf\n", eye, OCULUS_EyeRendDesc[eye].HmdToEyeViewOffset.x, OCULUS_EyeRendDesc[eye].HmdToEyeViewOffset.y, OCULUS_EyeRendDesc[eye].HmdToEyeViewOffset.z);
	}

	OCULUS_Size(1, 1) = OCULUS_MaxX - OCULUS_MinX;
	OCULUS_Size(2, 1) = OCULUS_MaxY - OCULUS_MinY;
	OCULUS_Size(3, 1) = OCULUS_MaxZ - OCULUS_MinZ;

	OCULUS_Centre(1, 1) = OCULUS_MinX + (OCULUS_Size(1, 1) / 2.0);
	OCULUS_Centre(2, 1) = OCULUS_MinY + (OCULUS_Size(2, 1) / 2.0);
	OCULUS_Centre(3, 1) = OCULUS_MinZ + (OCULUS_Size(3, 1) / 2.0);

	OCULUS_messgf("OCULUS_Start() OK.\n");
	disp(OCULUS_Centre);

	OCULUS_GridBuild();

	//add offset for the scale read from cfg file
	OCULUS_Scale = OCULUS_Scale + 0.07;

	//initial height info
	Pos2.y = ovr_GetFloat(session, OVR_KEY_EYE_HEIGHT, Pos2.y);

	//roomScene = new Scene(false);

	return(TRUE);
}

/******************************************************************************/

void OCULUS_Stop(void)
{
}

/******************************************************************************/
BOOL OCULUS_GetViewProjection(Matrix4f& M, float &scale)
{	
	M = view;
	scale = OCULUS_Scale;
	return TRUE;
}
/******************************************************************************/
void OCULUS_GraphicsDisplay(void(*draw)(void))
{
	// Animate the cube
	static float cubeClock = 0;
	//roomScene->Models[0]->Pos = Vector3f(9 * sin(cubeClock), 3, 9 * cos(cubeClock += 0.015f));

	int i;
	ovrPosef pose[2];
	float rot_mat[16];

	glGenFramebuffers(1, &FBO_mirror);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO_mirror);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture_mirror->OGL.TexId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	GRAPHICS_GetError("OCULUS_GraphicsDisplay(...)");

	// Get eye poses, feeding in correct IPD offset
	ovrVector3f               ViewOffset[2] = { OCULUS_EyeRendDesc[0].HmdToEyeViewOffset, OCULUS_EyeRendDesc[1].HmdToEyeViewOffset };
	ovrPosef                  EyeRenderPose[2];

	double           ftiming = ovr_GetPredictedDisplayTime(session, 0);
	// Keeping sensorSampleTime as close to ovr_GetTrackingState as possible - fed into the layer
	double           sensorSampleTime = ovr_GetTimeInSeconds();
	ovrTrackingState hmdState = ovr_GetTrackingState(session, ftiming, ovrTrue);
	ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);

	for (int eye = 0; eye < 2; ++eye)
	{
		// Increment to use next texture, just before writing
		eyeRenderTexture[eye]->TextureSet->CurrentIndex = (eyeRenderTexture[eye]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[eye]->TextureSet->TextureCount;

		// Switch to eye render target
		eyeRenderTexture[eye]->SetAndClearRenderSurface(eyeDepthBuffer[eye]);

		// Get view and projection matrices
		Matrix4f rollPitchYaw1 = Matrix4f::RotationY(Yaw);
		Matrix4f rollPitchYaw2 = Matrix4f::RotationX(Pitch);
		Matrix4f finalRollPitchYaw = rollPitchYaw2 * rollPitchYaw1 * Matrix4f(EyeRenderPose[eye].Orientation);
		Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
		Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
		Vector3f shiftedEyePos = Pos2 + rollPitchYaw1.Transform(EyeRenderPose[eye].Position);

		view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
		Matrix4f proj = ovrMatrix4f_Projection(HmdDesc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_RightHanded);

		combined =  proj * view;

		glMatrixMode(GL_PROJECTION);
		//glLoadTransposeMatrixf(proj.M[0]);
		glLoadTransposeMatrixf(combined.M[0]);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		if (OCULUS_GridFlag)
		{
			OCULUS_GridDraw();
		}

		glRotatef(-90, 1.0, 0.0, 0.0);
		glRotatef(-180, 0.0, 0.0, 1.0);
		glScalef(OCULUS_Scale, OCULUS_Scale, OCULUS_Scale);

		GRAPHICS_TextDraw(&OCULUS_TextLatencyTimer);
		// Finally draw the application-defined scene for this eye...
		if (draw != NULL)
		{
			(*draw)();
		}

		// Avoids an error when calling SetAndClearRenderSurface during next iteration.
		// Without this, during the next while loop iteration SetAndClearRenderSurface
		// would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
		// associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
		eyeRenderTexture[eye]->UnsetRenderSurface();
	}

	// Set up positional data.
	ovrViewScaleDesc viewScaleDesc;
	viewScaleDesc.HmdSpaceToWorldScaleInMeters = 0.01f;
	viewScaleDesc.HmdToEyeViewOffset[0] = ViewOffset[0];
	viewScaleDesc.HmdToEyeViewOffset[1] = ViewOffset[1];

	ovrLayerEyeFov ld;
	ld.Header.Type = ovrLayerType_EyeFov;
	ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

	for (int eye = 0; eye < 2; ++eye)
	{
		ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureSet;
		ld.Viewport[eye] = OVR::Recti(eyeRenderTexture[eye]->GetSize());
		ld.Fov[eye] = HmdDesc.DefaultEyeFov[eye];
		ld.RenderPose[eye] = EyeRenderPose[eye];
		ld.SensorSampleTime = sensorSampleTime;
	}

	ovrLayerHeader* layers = &ld.Header;

	// Blit mirror texture to back buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO_mirror);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	GLint w = Texture_mirror->OGL.Header.TextureSize.w;
	GLint h = Texture_mirror->OGL.Header.TextureSize.h;
	glBlitFramebuffer(0, h, w, 0,
		0, 0, w, h,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	OCULUS_EyeLoopLatencyTimer.After();

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDeleteTexture();
	//glDeleteRenderbuffersEXT();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &FBO_mirror);

	GRAPHICS_SwapBufferBefore();
	ovrResult result = ovr_SubmitFrame(session, 0, &viewScaleDesc, &layers, 1);
}

/******************************************************************************/

void OCULUS_QuaternionToRotationMatrix(float *quat, float *mat)
{
	mat[0] = 1.0 - 2.0 * quat[1] * quat[1] - 2.0 * quat[2] * quat[2];
	mat[4] = 2.0 * quat[0] * quat[1] + 2.0 * quat[3] * quat[2];
	mat[8] = 2.0 * quat[2] * quat[0] - 2.0 * quat[3] * quat[1];
	mat[12] = 0.0f;

	mat[1] = 2.0 * quat[0] * quat[1] - 2.0 * quat[3] * quat[2];
	mat[5] = 1.0 - 2.0 * quat[0] * quat[0] - 2.0 * quat[2] * quat[2];
	mat[9] = 2.0 * quat[1] * quat[2] + 2.0 * quat[3] * quat[0];
	mat[13] = 0.0f;

	mat[2] = 2.0 * quat[2] * quat[0] + 2.0 * quat[3] * quat[1];
	mat[6] = 2.0 * quat[1] * quat[2] - 2.0 * quat[3] * quat[0];
	mat[10] = 1.0 - 2.0 * quat[0] * quat[0] - 2.0 * quat[1] * quat[1];
	mat[14] = 0.0f;

	mat[3] = mat[7] = mat[11] = 0.0f;
	mat[15] = 1.0f;
}

void OCULUS_QuaternionToRotationMatrix(double *quat, double *mat)
{
	mat[0] = 1.0 - 2.0 * quat[1] * quat[1] - 2.0 * quat[2] * quat[2];
	mat[4] = 2.0 * quat[0] * quat[1] + 2.0 * quat[3] * quat[2];
	mat[8] = 2.0 * quat[2] * quat[0] - 2.0 * quat[3] * quat[1];
	mat[12] = 0.0f;

	mat[1] = 2.0 * quat[0] * quat[1] - 2.0 * quat[3] * quat[2];
	mat[5] = 1.0 - 2.0 * quat[0] * quat[0] - 2.0 * quat[2] * quat[2];
	mat[9] = 2.0 * quat[1] * quat[2] + 2.0 * quat[3] * quat[0];
	mat[13] = 0.0f;

	mat[2] = 2.0 * quat[2] * quat[0] + 2.0 * quat[3] * quat[1];
	mat[6] = 2.0 * quat[1] * quat[2] - 2.0 * quat[3] * quat[0];
	mat[10] = 1.0 - 2.0 * quat[0] * quat[0] - 2.0 * quat[1] * quat[1];
	mat[14] = 0.0f;

	mat[3] = mat[7] = mat[11] = 0.0f;
	mat[15] = 1.0f;
}
/******************************************************************************/

UINT OCULUS_NextPower2(UINT x)
{
	x -= 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;

	return(x + 1);
}

/******************************************************************************/

#define OCULUS_GRID_FLOOR  11
#define OCULUS_GRID_BACK   12
#define OCULUS_GRID_LEFT   13
#define OCULUS_GRID_RIGHT  14

int     OCULUS_GridCallList[] = { OCULUS_GRID_FLOOR, OCULUS_GRID_BACK, OCULUS_GRID_LEFT, OCULUS_GRID_RIGHT, -1 };

#define OCULUS_GRID_XY      1
#define OCULUS_GRID_XZ      2
#define OCULUS_GRID_YZ      3

#define OCULUS_GRID_SIZE   10

/******************************************************************************/

void OCULUS_GridBuild(float xmin, float xmax, float ymin, float ymax, float zpos, int grid, int ID)
{
	GLint i, g[OCULUS_GRID_SIZE + 1];
	GLfloat ypos[OCULUS_GRID_SIZE + 1], xpos[OCULUS_GRID_SIZE + 1];

	// Create list of vertices...
	for (i = 0; (i <= OCULUS_GRID_SIZE); i++)
	{
		if ((float)(i / 2) == (0.5*(float)i))
		{
			g[i] = 0;
		}
		else
		{
			g[i] = OCULUS_GRID_SIZE - 1;
		}

		xpos[i] = xmin + (xmax - xmin)*i / (OCULUS_GRID_SIZE - 1);
		ypos[i] = ymin + (ymax - ymin)*i / (OCULUS_GRID_SIZE - 1);
	}

	// Start a new GL compile list...
	glNewList(ID, GL_COMPILE);
	GRAPHICS_ColorSet(DARKGREY);
	glBegin(GL_LINE_STRIP);

	switch (grid)
	{
	case OCULUS_GRID_XY:
		for (i = 0; (i < OCULUS_GRID_SIZE); i++)
		{
			glVertex3f(xpos[g[i]], ypos[i], zpos);
			glVertex3f(xpos[g[i + 1]], ypos[i], zpos);
		}

		for (i = 0; (i < OCULUS_GRID_SIZE); i++)
		{
			glVertex3f(xpos[i], ypos[g[i]], zpos);
			glVertex3f(xpos[i], ypos[g[i + 1]], zpos);
		}
		break;

	case OCULUS_GRID_XZ:
		for (i = 0; (i < OCULUS_GRID_SIZE); i++)
		{
			glVertex3f(xpos[g[i]], zpos, ypos[i]);
			glVertex3f(xpos[g[i + 1]], zpos, ypos[i]);
		}

		for (i = 0; (i < OCULUS_GRID_SIZE); i++)
		{
			glVertex3f(xpos[i], zpos, ypos[g[i]]);
			glVertex3f(xpos[i], zpos, ypos[g[i + 1]]);
		}
		break;

	case OCULUS_GRID_YZ:
		for (i = 0; (i < OCULUS_GRID_SIZE); i++)
		{
			glVertex3f(zpos, xpos[g[i]], ypos[i]);
			glVertex3f(zpos, xpos[g[i + 1]], ypos[i]);
		}

		for (i = 0; (i < OCULUS_GRID_SIZE); i++)
		{
			glVertex3f(zpos, xpos[i], ypos[g[i]]);
			glVertex3f(zpos, xpos[i], ypos[g[i + 1]]);
		}
		break;
	}

	glEnd();
	glEndList();
}

/******************************************************************************/

void OCULUS_GridBuild(void)
{
	double Xmin, Xmax;
	double Ymin, Ymax;
	double Zmin, Zmax;

	Xmin = OCULUS_MinX - OCULUS_Centre(1, 1);
	Xmax = OCULUS_MaxX - OCULUS_Centre(1, 1);

	Ymin = OCULUS_MinY - OCULUS_Centre(2, 1);
	Ymax = OCULUS_MaxY - OCULUS_Centre(2, 1);

	Zmin = OCULUS_MinZ - OCULUS_Centre(3, 1);
	Zmax = OCULUS_MaxZ - OCULUS_Centre(3, 1);

	OCULUS_GridBuild(Xmin, Xmax, Ymin, Ymax, Zmax, OCULUS_GRID_XY, OCULUS_GRID_BACK);
	OCULUS_GridBuild(Xmin, Xmax, Zmin, Zmax, Ymin, OCULUS_GRID_XZ, OCULUS_GRID_FLOOR);
	OCULUS_GridBuild(Ymin, Ymax, Zmin, Zmax, Xmin, OCULUS_GRID_YZ, OCULUS_GRID_LEFT);
	OCULUS_GridBuild(Ymin, Ymax, Zmin, Zmax, Xmax, OCULUS_GRID_YZ, OCULUS_GRID_RIGHT);
}

/******************************************************************************/

void OCULUS_GridDraw(void)
{
	int i;

	GRAPHICS_LightingDisable();

	for (i = 0; (OCULUS_GridCallList[i] != -1); i++)
	{
		glPushMatrix();
		glCallList(OCULUS_GridCallList[i]);
		glPopMatrix();
	}

	GRAPHICS_LightingEnable();
}

/******************************************************************************/

void OCULUS_DisplayPosition(int x, int y)
{
	int w, h;

	w = HmdDesc.Resolution.w;
	h = HmdDesc.Resolution.h;

	OCULUS_messgf("OCULUS_DisplayPosition() WindowPosition: x=%d, y=%d\n", x, y);
	OCULUS_messgf("OCULUS_DisplayPosition() WindowSize: w=%d, h=%d\n", w, h);

	GRAPHICS_SetWindow();
	glutPositionWindow(x, y);
	glutReshapeWindow(w, h);
}

/******************************************************************************/

void OCULUS_DisplayOnHMD(void)
{
	OCULUS_DisplaySecondaryX = 0;//HmdDesc.WindowsPos.x;
	OCULUS_DisplaySecondaryY = 0;//HmdDesc.WindowsPos.y;

	OCULUS_DisplayPosition(OCULUS_DisplaySecondaryX, OCULUS_DisplaySecondaryY);
	OCULUS_DisplayedOnHMD = TRUE;
}

/******************************************************************************/

void OCULUS_DisplayOnDeskTop(void)
{
	OCULUS_DisplayPrimaryX = 1;
	OCULUS_DisplayPrimaryY = 1;

	OCULUS_DisplayPosition(OCULUS_DisplayPrimaryX, OCULUS_DisplayPrimaryY);
	OCULUS_DisplayedOnHMD = FALSE;
}

/******************************************************************************/

void OCULUS_DisplayToggle(void)
{
	if (!OCULUS_DisplayedOnHMD)
	{
		OCULUS_DisplayOnHMD();
	}
	else
	{
		OCULUS_DisplayOnDeskTop();
	}
}

/******************************************************************************/

BOOL OCULUS_GetPose(ovrPosef& pose)
{
	ovrTrackingState ts  = ovr_GetTrackingState(session, ovr_GetTimeInSeconds(), ovrTrue);
	if(ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
	{
		pose = ts.HeadPose.ThePose;
		return TRUE;
	}
	else
	{
		printf("No Tracking INFO available!");
		return FALSE;
	}

}
/******************************************************************************/

void OCULUS_TimingResults(void)
{
	OCULUS_DisplayFrequencyTimer.Results();
	OCULUS_DisplayLatencyTimer.Results();
	OCULUS_DrawGridLatencyTimer.Results();
	OCULUS_TextLatencyTimer.Results();
	OCULUS_DrawLatencyTimer.Results();
	OCULUS_BeginFrameLatencyTimer.Results();
	OCULUS_EndFrameLatencyTimer.Results();
	OCULUS_EyeLoopLatencyTimer.Results();
}

/******************************************************************************/

BOOL OCULUS_GlutKeyboard(BYTE key, int x, int y)
{
	BOOL flag = FALSE;

	// Press 'f' to move the window to the HMD.
	if (toupper(key) == 'F')
	{
		OCULUS_DisplayToggle();
		flag = TRUE;
	}
	
	//translation(up and down)
	if (toupper(key) == 'W')	Pos2.y += 0.05f;
	if (toupper(key) == 'S')	Pos2.y += -0.05f;
	//rotation(up and down)
	if (toupper(key) == 'E')	Pitch -= 0.2f;
	if (toupper(key) == 'D')	Pitch += 0.2f;
	//translation(forward and backward)
	if (toupper(key) == 'Q')	Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, -0.05f));
	if (toupper(key) == 'A')	Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, +0.05f));
	//if (toupper(key) == 'D')	Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(+0.05f, 0, 0));
	//if (toupper(key) == 'A')	Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(-0.05f, 0, 0));
	
	

	return(flag);
}

/******************************************************************************/

//#endif