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
//

/**
 @file
 @test
*/

#ifndef EGLTEST_IMAGE_NEGATIVE_H
#define EGLTEST_IMAGE_NEGATIVE_H

#include "eglteststep.h"

_LIT(KEGL_Image_eglCreateImage_Bad_Parameter, "EGL_Image_eglCreateImage_Bad_Parameter");
NONSHARABLE_CLASS(CEglTest_EGL_Image_eglCreateImage_Bad_Parameter) : public CEglTestStep
	{
public:	
	TVerdict doTestStepL();	
	};

_LIT(KEGL_Image_UsageBits_Enforcement, "EGL_Image_UsageBits_Enforcement");
NONSHARABLE_CLASS(CEglTest_EGL_Image_UsageBits_Enforcement) : public CEglTestStep
	{
public:	
	TVerdict doTestStepL();	
	};

_LIT(KEGL_Image_DestroyImageKHR, "EGL_Image_DestroyImageKHR");
NONSHARABLE_CLASS(CEglTest_EGL_Image_DestroyImageKHR) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};

_LIT(KEGL_Image_VGImage_From_Invalid_EGLHandle, "EGL_Image_VGImage_From_Invalid_EGLHandle");
NONSHARABLE_CLASS(CEglTest_EGL_Image_VGImage_From_Invalid_EGLHandle) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};

_LIT(KEGL_Image_Self_Drawing, "EGL_Image_Self_Drawing");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Self_Drawing) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};

#endif
