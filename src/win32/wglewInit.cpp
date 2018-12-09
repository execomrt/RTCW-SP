/*
** The OpenGL Extension Wrangler Library
** Copyright (C) 2008-2015, Nigel Stewart <nigels[]users sourceforge net>
** Copyright (C) 2002-2008, Milan Ikits <milan ikits[]ieee org>
** Copyright (C) 2002-2008, Marcelo E. Magallon <mmagallo[]debian org>
** Copyright (C) 2002, Lev Povalahev
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the following conditions are met:
** 
** * Redistributions of source code must retain the above copyright notice, 
**   this list of conditions and the following disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, 
**   this list of conditions and the following disclaimer in the documentation 
**   and/or other materials provided with the distribution.
** * The mName of the author may be used to endorse or promote products 
**   derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
** THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <windows.h>
#include "GL/glew.h"
#include "GL/wglew.h"
#include "wglewInit.h"

#pragma comment(lib, "opengl32.lib")

static void CALLBACK _wglewCallbackDebug(GLenum source, GLenum type, GLuint id, GLenum severity,
										 GLsizei length, const GLchar* message, const void* userParam)
{
	userParam;
	length;

	wglewLog("\n---------------------opengl-callback-start------------\n");
	wglewLog("message: %s\n", message);
	wglewLog("source: %d\n", source);
	wglewLog("\ttype: ");
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		wglewLog("\tERROR\n");
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		wglewLog("\tDEPRECATED_BEHAVIOR\n");
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		wglewLog("\tUNDEFINED_BEHAVIOR\n");
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		wglewLog("\tPORTABILITY\n");
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		wglewLog("\tPERFORMANCE\n");
		break;
	case GL_DEBUG_TYPE_OTHER:
		wglewLog("\tOTHER\n");
		break;
	}

	wglewLog("\t\tid: %d\n", id);
	wglewLog("\t\tseverity: ");
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		wglewLog("LOW\n");
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		wglewLog("MEDIUM\n");
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		wglewLog("HIGH\n");
		break;
	default:
		wglewLog("OTHER\n");
		break;

	}

	wglewLog("---------------------opengl-callback-end--------------\n");


}


static int wglewChoosePixelFormat(wglewContext* context, HDC hDC)
{
#define ADD_ATTRIBi(a, b) iAttrbList[iAttrbCount++] = a; iAttrbList[iAttrbCount++] = b;
#define ADD_ATTRIBf(a, b) fAttrbList[iAttrbCount++] = a; fAttrbList[iAttrbCount++] = b;

	GLfloat fAttrbList[32] = { 0 };
	GLint iAttrbList[32] = { 0 };
	GLint iCompatiblesFormats[32] = { 0 };
	for (;;)
	{
		GLint iAttrbCount = 0;

		if (WGLEW_ARB_multisample)
		{
			int iNumPixelFormats = 0;
			int sampleCountMax = 0;
			GLint values[2];

			iAttrbList[0] = WGL_NUMBER_PIXEL_FORMATS_ARB;
			iAttrbList[1] = 0;

			BOOL ret = wglGetPixelFormatAttribivARB(hDC, 0, 0, 1, iAttrbList, values);
			iNumPixelFormats = values[0];
			for (int i = 0; i < iNumPixelFormats; i++)
			{
				iAttrbList[0] = WGL_SAMPLES_ARB;
				iAttrbList[1] = 0;
				ret = wglGetPixelFormatAttribivARB(hDC, i + 1, 0, 1, iAttrbList, values);
				sampleCountMax = max(sampleCountMax, values[0]);
			}
		}

		GLuint numFormats = 0;

		if (WGLEW_ARB_pixel_format)
		{
			ADD_ATTRIBi(WGL_SUPPORT_OPENGL_ARB, GL_TRUE);
			ADD_ATTRIBi(WGL_DOUBLE_BUFFER_ARB, GL_TRUE);
			ADD_ATTRIBi(WGL_ACCELERATION_ARB, context->noAcceleration ? WGL_NO_ACCELERATION_ARB : WGL_ACCELERATION_ARB);

			ADD_ATTRIBi(WGL_COLOR_BITS_ARB, context->colorBits);
			ADD_ATTRIBi(WGL_DEPTH_BITS_ARB, context->depthBits);

			if (context->stencilBits != 0) {
				ADD_ATTRIBi(WGL_STENCIL_BITS_ARB, context->stencilBits);
			}

			if (context->stereo != 0) {
				ADD_ATTRIBi(WGL_STEREO_ARB, GL_TRUE);
			}

			if (context->sampleCount > 1) {
				ADD_ATTRIBi(WGL_SAMPLE_BUFFERS_ARB, GL_TRUE);
				ADD_ATTRIBi(WGL_SAMPLES_ARB, context->sampleCount);
			}

			if (wglChoosePixelFormatARB(hDC,
				iAttrbList,
				fAttrbList,
				1,
				iCompatiblesFormats,
				&numFormats) == GL_TRUE) {

				break;
			}
		}
		else if (WGLEW_EXT_pixel_format)
		{
			ADD_ATTRIBi(WGL_DOUBLE_BUFFER_EXT, GL_TRUE);
			ADD_ATTRIBi(WGL_DRAW_TO_WINDOW_EXT, GL_TRUE);
			ADD_ATTRIBi(WGL_ACCELERATION_EXT, context->noAcceleration ? WGL_NO_ACCELERATION_EXT : WGL_ACCELERATION_EXT);
			ADD_ATTRIBi(WGL_COLOR_BITS_EXT, context->colorBits);
			ADD_ATTRIBi(WGL_DEPTH_BITS_EXT, context->depthBits);
			if (context->sampleCount > 1) {
				ADD_ATTRIBi(WGL_SAMPLE_BUFFERS_EXT, GL_TRUE);
				ADD_ATTRIBi(WGL_SAMPLES_EXT, context->sampleCount);
			}

			if (wglChoosePixelFormatEXT(hDC,
				iAttrbList,
				fAttrbList,
				1,
				iCompatiblesFormats,
				&numFormats) == GL_TRUE) {

				break;
			}

		}
		else
		{
			// back in the days ...
			PIXELFORMATDESCRIPTOR pfd =			// pfd Tells Windows How We Want Things To Be
			{
				sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
				1,								// Version Number
				PFD_SUPPORT_OPENGL | 
				PFD_DRAW_TO_WINDOW |				
				PFD_DOUBLEBUFFER,				// Must Support Double Buffering
				PFD_TYPE_RGBA,					// Request An RGBA Format
				(BYTE) context->colorBits,				// Select Our Color Depth
				0, 0, 0, 0, 0, 0,				// Color Bits Ignored
				0,								// No Alpha Buffer
				0,								// Shift Bit Ignored
				0,								// No Accumulation Buffer
				0, 0, 0, 0,						// Accumulation Bits Ignored
				(BYTE) context->depthBits,				// 16Bit Z-Buffer (Depth Buffer)
				(BYTE) context->stencilBits,			// No Stencil Buffer
				0,								// No Auxiliary Buffer
				PFD_MAIN_PLANE,					// Main Drawing Layer
				0,								// Reserved
				0, 0, 0							// Layer Masks Ignored
			};

			iCompatiblesFormats[0] = ChoosePixelFormat(hDC, &pfd);

			if (iCompatiblesFormats[0]) {
				break;
			}

		}


		if (context->stereo) {
			context->stereo = 0;
		}
		else if (context->sampleCount) {
			context->sampleCount = 0;
		}
		else if (context->colorBits == 32) {
			context->colorBits = 16;
		}
	}


	return iCompatiblesFormats[0];

}

int wglewDescribePixelFormat(wglewContext* context, HDC hDC)
{
	DescribePixelFormat(hDC, context->pixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR), &context->pixelFormatDescriptor);
	return 0;
}


HGLRC wglewCreateContext(wglewContext* context, HDC hDC)
{
	HGLRC ret;
	
	if (context->majorVersion >= 3)
	{
		int iAttrbCount = 0;
		GLint iAttrbList[32] = { 0 };

		ADD_ATTRIBi(WGL_CONTEXT_MAJOR_VERSION_ARB, context->majorVersion);
		ADD_ATTRIBi(WGL_CONTEXT_MINOR_VERSION_ARB, context->minorVersion);

		ADD_ATTRIBi(WGL_CONTEXT_FLAGS_ARB, (context->debugBits ? WGL_CONTEXT_DEBUG_BIT_ARB : 0) | (context->compatibilityBits ? 0 : WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB));
		ADD_ATTRIBi(WGL_CONTEXT_PROFILE_MASK_ARB, context->compatibilityBits ? WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB : WGL_CONTEXT_CORE_PROFILE_BIT_ARB);


		if (!SetPixelFormat(hDC, context->pixelFormatIndex, &context->pixelFormatDescriptor))				// Are We Able To Set The Pixel Format?
		{
			goto onError;
		}

		auto hGLRCTemp = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hGLRCTemp);

		ret = wglCreateContextAttribsARB(hDC, 0, iAttrbList);
		if (ret && context->debugBits) {

			if (GLEW_KHR_debug)
			{
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback(_wglewCallbackDebug, NULL);
				GLuint unusedIds = 0;
				glDebugMessageControl(
					GL_DONT_CARE,
					GL_DONT_CARE,
					GL_DONT_CARE,
					0,
					&unusedIds,
					true);				
			}
		}
		

		wglDeleteContext(hGLRCTemp);
	}
	else
	{
		ret = wglCreateContext(hDC);

	}

	

	return ret;

onError:

	return NULL;
		


}
int wglewInitContext(wglewContext* context)
{

	// Create dummy windows and class. Access to WGL extensions
	WNDCLASS wcTemp;
	HWND hWndTemp = 0;
	HDC hDCTemp = 0;
	HGLRC hGLRCTemp;
	int pfdTemp;
	ZeroMemory(&wcTemp, sizeof(wcTemp));

	wcTemp.hInstance = GetModuleHandle(NULL);
	wcTemp.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcTemp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcTemp.lpfnWndProc = DefWindowProc;
#ifdef UNICODE
	wcTemp.lpszClassName = L"DummyGL";
#else
	wcTemp.lpszClassName = "DummyGL";
#endif

	if (!RegisterClass(&wcTemp))
	{
		return -1;
	}

	hWndTemp = CreateWindowEx(0, wcTemp.lpszClassName, wcTemp.lpszClassName, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1,
		0, 0, wcTemp.hInstance, NULL);

	if (!hWndTemp)
	{
		return -2;
	}

	hDCTemp = GetDC(hWndTemp);
	if (!hWndTemp)
	{
		return -3;
	}

	pfdTemp = ChoosePixelFormat(hDCTemp, &context->pixelFormatDescriptor);
	SetPixelFormat(hDCTemp, pfdTemp, &context->pixelFormatDescriptor);
	hGLRCTemp = wglCreateContext(hDCTemp);
	wglMakeCurrent(hDCTemp, hGLRCTemp);

	glewInit();
		
	context->pixelFormatIndex = wglewChoosePixelFormat(context, hDCTemp);

	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hWndTemp, hDCTemp);
	wglDeleteContext(hGLRCTemp);
	UnregisterClass(wcTemp.lpszClassName, wcTemp.hInstance);
	DestroyWindow(hWndTemp);
		

	return 0;


}

#if 0


// Choose best pixel format
GLint wglCoreChoosePixelFormat(HDC pHdc, GLint *pBpp, GLint *pDepth, GLint *pStencil, GLint *pDbl, GLint *pAcc, GLint *pDib)
{
	GLint wbpp = pBpp == NULL ? -1 : *pBpp;
	GLint wdepth = pDepth == NULL ? 16 : *pDepth;
	GLint wstencil = pStencil == NULL ? 0 : *pDepth;
	GLint wdbl = pDbl == NULL ? -1 : *pDbl;
	GLint wacc = pAcc == NULL ? 1 : *pAcc;
	GLint wdib = pDib == NULL ? -1 : *pDib;
	GLint num;
	GLuint maxqual = 0;
	GLint maxindex = 0;
	GLint max_bpp, max_depth, max_dbl, max_acc, max_dib, max_stencil;
	GLint i;

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;

	num = DescribePixelFormat(pHdc, 1, sizeof(pfd), &pfd);
	if (num == 0)
		return 0;

	for (i = 1; i <= num; i++)
	{
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		DescribePixelFormat(pHdc, i, sizeof(pfd), &pfd);
		{
			GLint bpp = pfd.cColorBits;
			GLint depth = pfd.cDepthBits;
			GLint stencil = pfd.cStencilBits;

			GLboolean pal = !!(pfd.iPixelType == PFD_TYPE_COLORINDEX);
			GLboolean mcd = !!((pfd.dwFlags & PFD_GENERIC_FORMAT) && (pfd.dwFlags & PFD_GENERIC_ACCELERATED));
			GLboolean soft = !!((pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
			GLboolean icd = (!(pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
			GLboolean opengl = !!(pfd.dwFlags & PFD_SUPPORT_OPENGL);
			GLboolean window = !!(pfd.dwFlags & PFD_DRAW_TO_WINDOW);
			GLboolean bitmap = !!(pfd.dwFlags & PFD_DRAW_TO_BITMAP);
			GLboolean dbuff = !!(pfd.dwFlags & PFD_DOUBLEBUFFER);

			GLuint q = 0;

			if (opengl && window)
				q += 0x10000;
			if (wdepth == -1 || (wdepth > 0 && depth > 0))
				q += 0x8000;
			if (wstencil == -1 || (wstencil > 0 && stencil > 0))
				q += 0x4000;
			if (wdbl == -1 || (wdbl == 0 && !dbuff) || (wdbl == 1 && dbuff))
				q += 0x2000;
			if (wacc == -1 || (wacc == 0 && soft) || (wacc == 1 && (mcd || icd)))
				q += 0x1000;
			if (mcd || icd)
				q += 0x0040;
			if (icd)
				q += 0x0002;
			if (wbpp == -1 || (wbpp == bpp))
				q += 0x0800;
			if ((wdib == -1) || (wdib == 0 && !bitmap) || (wdbl == 1 && bitmap))
				q += 0x10000;
			if (bpp >= 16)
				q += 0x0020;
			if (bpp == 16)
				q += 0x0008;
			if (wdepth == -1 || (wdepth == depth))
				q += 0x0400;

			if (depth >= 16)
				q += 0x0010;
			if (depth == 16)
				q += 0x0004;
			if (!pal)
				q += 0x0080;

			if (q > maxqual)
			{
				maxqual = q;
				maxindex = i;
				max_bpp = bpp;
				max_depth = depth;
				max_stencil = stencil;
				max_dbl = dbuff ? 1 : 0;
				max_acc = soft ? 0 : 1;
				max_dib = bitmap ? 1 : 0;
			}
		}
	}
	if (maxindex == 0)
		return maxindex;

	if (pBpp != NULL)
		*pBpp = max_bpp;
	if (pDepth != NULL)
		*pDepth = max_depth;
	if (pStencil != NULL)
		*pStencil = max_stencil;
	if (pDbl != NULL)
		*pDbl = max_dbl;
	if (pAcc != NULL)
		*pAcc = max_acc;
	if (pDib != NULL)
		*pDib = max_dib;

	return maxindex;
}

#endif