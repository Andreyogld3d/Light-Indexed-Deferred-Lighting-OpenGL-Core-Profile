/***********      .---.         .-"-.      *******************\
* -------- *     /   ._.       / ´ ` \     * ---------------- *
* Author's *     \_  (__\      \_°v°_/     * humus@rogers.com *
*   note   *     //   \\       //   \\     * ICQ #47010716    *
* -------- *    ((     ))     ((     ))    * ---------------- *
*          ****--""---""-------""---""--****                  ********\
* This file is a part of the work done by Humus. You are free to use  *
* the code in any way you like, modified, unmodified or copy'n'pasted *
* into your own work. However, I expect you to respect these points:  *
*  @ If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  @ For use in anything commercial, please request my approval.      *
*  @ Share your work and ideas too as much as you can.                *
\*********************************************************************/

#include "OpenGLExtensions.h"

#include <stdlib.h>
#include <string.h>
#include "../Util/String.h"
#include "wglew.h"

#pragma comment(lib, "glew32.lib")

bool extensionSupported(const char *extStr, const char *extension){
	if (extStr){
		size_t len = strlen(extension);
		while ((extStr = strstr(extStr, extension)) != NULL){
			extStr += len;
			if (*extStr == ' ' || *extStr == '\0') return true;
		}
	}
	return false;
}


int GLMajorVersion = 1;
int GLMinorVersion = 0;
int GLReleaseVersion = 0;

#define wglxGetProcAddress wglGetProcAddress

#define isExtensionSupported(str) extensionSupported(extensions, str)

#define isWGLXExtensionSupported(str) (extensionSupported(wglxExtensions, str) || extensionSupported(extensions, str))

void initExtensions(HDC hdc){
	glewInit();
	char *wglxExtensions = (char *) wglGetExtensionsStringARB(hdc);

	const char* extensions = 0;// (const char*)glGetString(GL_EXTENSIONS);
	String Ext;
	//if (!extensions) 
	{
#define GL_NUM_EXTENSIONS 0x821D 
		GLint n = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);

		typedef const GLubyte* (GLAPIENTRY* PFNGLGETSTRINGIPROC) (GLenum name, GLuint index);
		PFNGLGETSTRINGIPROC glGetStringi = reinterpret_cast<PFNGLGETSTRINGIPROC>(wglxGetProcAddress("glGetStringi"));
		for (int i = 0; i < n; ++i) {
			const char* ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
			Ext += ext;
			Ext += " ";
		}
		extensions = Ext;
	}


	const char *version = (const char *) glGetString(GL_VERSION);
	GLMajorVersion = atoi(version);
	version = strchr(version, '.') + 1;
	GLMinorVersion = atoi(version);
	version = strchr(version, '.');
	if (version) GLReleaseVersion = atoi(version + 1);

	outputDebugString(version);
	outputDebugString(extensions);
}
