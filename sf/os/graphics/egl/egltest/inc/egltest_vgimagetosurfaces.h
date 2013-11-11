// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// egltest_vgimagetosurface.h
// 
//

/**
 @file
 @test
*/

#ifndef __EGLTEST_VGIMAGETOSURFACE_H__
#define __EGLTEST_VGIMAGETOSURFACE_H__

#include "eglteststep.h"


_LIT(KEGL_Image_VgImage_To_pBuffer, "EGL_Image_VgImage_To_pBuffer");
NONSHARABLE_CLASS(CEglTest_EGL_Image_VgImage_To_pBuffer) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	TVerdict doTestPartialStepL();
	};

_LIT(KEGL_Image_VgImage_To_Window, "EGL_Image_VgImage_To_Window");
NONSHARABLE_CLASS(CEglTest_EGL_Image_VgImage_To_Window) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	TVerdict doTestPartialStepL();
	};

_LIT(KEGL_Image_VgImage_To_Pixmap, "EGL_Image_VgImage_To_Pixmap");
NONSHARABLE_CLASS(CEglTest_EGL_Image_VgImage_To_Pixmap) : public CEglTestStep
	{
public:	
	TVerdict doTestStepL();	
	TVerdict doTestPartialStepL();
	};

_LIT(KEGL_Image_VGImage_To_Pixmap_CFbs, "EGL_Image_VGImage_To_Pixmap_CFbs");
NONSHARABLE_CLASS(CEglTest_EGL_Image_VGImage_To_Pixmap_CFbs) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	TVerdict doTestPartialStepL();
	};

#endif	//__EGLTEST_VGIMAGETOSURFACE_H__
