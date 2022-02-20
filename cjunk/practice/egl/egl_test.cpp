#include <EGL/egl.h> 
#include <variant>
#include <future>
#include <iostream>

//raii container for EGLNativeWindowType
class NativeWindow
{
public:
	NativeWindow(EGLNativeWindowType window)
		: m_window(window)
	{
	}

	~NativeWindow()
	{
		if (m_window)
		{
            //eglDestroySurface(m_display, m_window);
		}
	}

	EGLNativeWindowType get() const
	{
		return m_window;
	}

private:
    //EGLSurface m_surface;
	EGLNativeWindowType m_window;
	EGLDisplay m_display;
};

//raii container for EGLDisplay
class MyDisplay
{
public:
	MyDisplay()
		: m_display(eglGetDisplay(EGL_DEFAULT_DISPLAY))
	{
		if (m_display == EGL_NO_DISPLAY)
		{
			throw std::runtime_error("eglGetDisplay failed");
		}

		if (eglInitialize(m_display, nullptr, nullptr) == EGL_FALSE)
		{
			throw std::runtime_error("eglInitialize failed");
		}
	}

	~MyDisplay()
	{
		if (m_display != EGL_NO_DISPLAY)
		{
			eglTerminate(m_display);
		}
	}

	EGLDisplay get() const
	{
		return m_display;
	}

private:
	EGLDisplay m_display;
};

//raii container for EGLConfig
class Config
{
public:
	Config()
		: m_config(nullptr)
	{
		const EGLint config_attribs[] =
		{
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_NONE
		};

		EGLint num_configs;
		if (eglChooseConfig(m_display, config_attribs, &m_config, 1, &num_configs) == EGL_FALSE)
		{
			throw std::runtime_error("eglChooseConfig failed");
		}

		if (num_configs == 0)
		{
			throw std::runtime_error("eglChooseConfig returned no configs");
		}
	}

	~Config()
	{
		if (m_config)
		{
//			eglDestroyConfig(m_display, m_config);
		}
	}

	EGLConfig get() const
	{
		return m_config;
	}

private:
	EGLDisplay m_display;
	EGLConfig m_config;
};

//raii container for EGLSurface
class Surface
{
public:
	Surface(EGLNativeWindowType window)
		: m_surface(nullptr)
	{
		if (eglCreateWindowSurface(m_display, m_config, window, nullptr, &m_surface) == EGL_FALSE)
		{
			throw std::runtime_error("eglCreateWindowSurface failed");
		}
	}

	~Surface()
	{
		if (m_surface)
		{
			eglDestroySurface(m_display, m_surface);
		}
	}

	EGLSurface get() const
	{
		return m_surface;
	}

private:
	EGLDisplay m_display;
	EGLConfig m_config;
	EGLSurface m_surface;
};

//raii container for EGLContext
class Context
{
public:
	Context()
		: m_context(nullptr)
	{
		const EGLint context_attribs[] =
		{
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
		};

		if (eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, context_attribs) == EGL_FALSE)
		{
			throw std::runtime_error("eglCreateContext failed");
		}
	}

	~Context()
	{
		if (m_context)
		{
			eglDestroyContext(m_display, m_context);
		}
	}

	EGLContext get() const
	{
		return m_context;
	}

private:
	EGLDisplay m_display;
	EGLConfig m_config;
	EGLContext m_context;
};

//raii container for EGLDisplay
class EGL
{
public:
	EGL()
		: m_display(new Display)
	{
	}

	~EGL()
	{
		delete m_display;
	}

	EGLNativeWindowType create_window(int width, int height)
	{
		NativeWindow window(nullptr);
		if (eglCreateWindowSurface(m_display->get(), m_config->get(), window.get(), nullptr, &window.get()) == EGL_FALSE)
		{
			throw std::runtime_error("eglCreateWindowSurface failed");
		}

		return window.get();
	}

	EGLNativeWindowType create_window(NativeWindow& window)
	{
		if (eglCreateWindowSurface(m_display->get(), m_config->get(), window.get(), nullptr, &window.get()) == EGL_FALSE)
		{
			throw std::runtime_error("eglCreateWindowSurface failed");
		}

		return window.get();
	}

	EGLSurface create_surface(EGLNativeWindowType window)
	{
		Surface surface(window);
		return surface.get();
	}

	EGLContext create_context()
	{
		Context context;
		return context.get();
	}

	EGLSurface create_surface(EGLNativeWindowType window, EGLContext context)
	{
		Surface surface(window);
		if (eglMakeCurrent(m_display->get(), surface.get(), surface.get(), context.get()) == EGL_FALSE)
		{
			throw std::runtime_error("eglMakeCurrent failed");
		}

		return surface.get();
	}

	EGLConfig get_config()
	{
		return m_config->get();
	}

private:
	std::unique_ptr<MyDisplay> m_display;
	std::unique_ptr<Config> m_config;
};
// draw a triangle
void triangle(GLuint program)
{
	glUseProgram(program);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

// draw a square
void square(GLuint program)
{
	glUseProgram(program);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 4);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}
void test_egl(Window window)
{
	// create a program
	GLuint program = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const char* vs_source =
		"#version 300 es\n"
		"in vec3 position;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(position, 1.0);\n"
		"}\n";
	glShaderSource(vs, 1, &vs_source, nullptr);
	glCompileShader(vs);
	GLint compiled;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen)
		{
			char* buf = (char*)malloc(infoLen);
			if (buf)
			{
				glGetShaderInfoLog(vs, infoLen, nullptr, buf);
				printf("%s\n", buf);
				free(buf);
			}
		}
		glDeleteShader(vs);
		return;
	}
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fs_source =
		"#version 300 es\n"
		"precision mediump float;\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";
	glShaderSource(fs, 1, &fs_source, nullptr);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen)
		{
			char* buf = (char*)malloc(infoLen);
			if (buf)
			{
				glGetShaderInfoLog(fs, infoLen, nullptr, buf);
				printf("%s\n", buf);
				free(buf);
			}
		}
		glDeleteShader(fs);
		return;
	}
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen)
		{
			char* buf = (char*)malloc(infoLen);
			if (buf)
			{
				glGetProgramInfoLog(program, infoLen, nullptr, buf);
				printf("%s\n", buf);
				free(buf);
			}
		}
		glDeleteProgram(program);
		return;
	}
	glUseProgram(program);

	// create a window
	NativeWindow window(window.get());
	EGLNativeWindowType native_window = window.get();
	EGLNativeDisplayType native_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	EGLConfig config = m_egl.get_config();
	EGLDisplay display = m_egl.m_display->get();
	EGLSurface surface = m_egl.create_surface(native_window, m_egl.m_context->get());

	// draw a triangle
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	triangle(program);

	// swap buffers
	eglSwapBuffers(display, surface);
}

// main
int main(int argc, char* argv[])
{
	// create a window
	Window window(800, 600);

	// create an EGL instance
	EGL egl;

	// draw a triangle
	egl.test_egl(window);

	// wait for a key press
	std::cin.get();

	return 0;
}
