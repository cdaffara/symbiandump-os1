 // Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
 // All rights reserved.
 // This component and the accompanying materials are made available
 // under the terms of "Eclipse Public License v1.0"
 // which accompanies this distribution, and is available
 // at the URL "http://www.eclipse.org/legal/epl-v10.html".
 //
 // Initial Contributors:
 // Nokia Corporation - initial contribution.
 //
 // Contributors:
 //
 // Description:
 //

#ifndef __EGLRENDERING_H__
#define __EGLRENDERING_H__


#include <EGL/egl.h>
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <w32std.h>
#include <coedef.h>


class MEngine;


class CEGLRendering : public CBase
	{
public:
	static CEGLRendering* NewL(RWindow& aWindow);
	static CEGLRendering* NewLC(RWindow& aWindow);

	~CEGLRendering();

	static void EGLCheckError();
	static void EGLCheckReturnError(EGLBoolean aBool);
	static void VGCheckError();
	
	void UpdateDisplay();
	
private:
	CEGLRendering(RWindow& aWindow);
	void ConstructL();	
	
private:
	RWindow& iWindow;			
	MEngine* iCurrentDemo;
	
	EGLDisplay iDisplay;
	EGLSurface iSurface;
	EGLContext iContextVG;
	
	TInt iCount;
	TBool iShowMirrorToggled;
	TBool iBusySwapping;
	};


#endif 
