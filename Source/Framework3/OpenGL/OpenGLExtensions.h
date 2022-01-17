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

#ifndef _OPENGLEXTENSIONS_H_
#define _OPENGLEXTENSIONS_H_

#include "../Platform.h"
#define GLEW_NO_GLU
#include "glew.h"

extern int GLMajorVersion;
extern int GLMinorVersion;
extern int GLReleaseVersion;

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *) NULL + (i))
#endif

#if defined(_WIN32)
void initExtensions(HDC hdc);
#elif defined(LINUX)
void initExtensions(Display *display);
#elif defined(__APPLE__)
void initExtensions();
#endif

#endif // _OPENGLEXTENSIONS_H_
