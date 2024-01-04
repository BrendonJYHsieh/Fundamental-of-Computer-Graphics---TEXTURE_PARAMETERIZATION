#include "OpenGLPanel.h"

void Mesh::OpenGLPanel::makeContextCurrent()
{
	if (glInitialized) {
		if (wglMakeCurrent(hDC, hRC) == false) {
			DWORD error = GetLastError();
			MessageBox::Show("[OpenGLPanel] wglMakeCurrent Error: " + error);
		}
	}
	else {
		MessageBox::Show("[OpenGLPanel] OpenGL context not initialized.");
	}
}

HGLRC Mesh::OpenGLPanel::getGlContext()
{
	return hRC;
}

void Mesh::OpenGLPanel::initOpenGL()
{
	// Get Handle
	hWnd = (HWND)this->Handle.ToInt32();
	hDC = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int nFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, nFormat, &pfd);

	if (createContext) {
		// Create OpenGL Rendering Context
		hRC = (wglCreateContext(hDC));
		if (hRC == 0)
			MessageBox::Show("[OpenGLPanel] wglCreateContext Error");
		// Assign OpenGL Rendering Context
		if (wglMakeCurrent(hDC, hRC) == false)
			MessageBox::Show("[OpenGLPanel] wglMakeCurrent Error");

		GLenum res = glewInit();
		if (res != GLEW_OK) {
			fprintf(stderr, "[OpenGLPanel] glewInit Error: '%s'\n", glewGetErrorString(res));
		}
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//GL.glEnable(GL.GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else {
		hRC = existingContext;
	}
	glInitialized = true;
}

System::Void Mesh::OpenGLPanel::OnLoad(System::EventArgs^ e)
{
	if (!DesignMode)
	{
		initOpenGL();
	}
	UserControl::OnLoad(e);
}

System::Void Mesh::OpenGLPanel::OnPaint(System::Windows::Forms::PaintEventArgs^ e)
{
	if (DesignMode)
	{
		UserControl::OnPaint(e);
	}
	if (glInitialized)
	{
		wglMakeCurrent(hDC, hRC);

		UserControl::OnPaint(e);

		glFlush();
		SwapBuffers(hDC);
	}
	return System::Void();
}

System::Void Mesh::OpenGLPanel::OnResize(System::EventArgs^ e)
{
	wglMakeCurrent(hDC, hRC);
	UserControl::OnResize(e);
	return System::Void();
}

