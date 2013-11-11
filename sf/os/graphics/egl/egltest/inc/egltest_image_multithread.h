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

#ifndef __EGLTEST_IMAGE_MULTITHREAD_H__
#define __EGLTEST_IMAGE_MULTITHREAD_H__

#include "eglteststep.h"

NONSHARABLE_CLASS(CEglTest_MThread_Image_Base) : public CEglTestStep
	{
public:
	CEglTest_MThread_Image_Base();
	~CEglTest_MThread_Image_Base();

protected:
	RSgImage 			iSgImageShared;
	EGLImageKHR			iEGLImageShared;
	};


_LIT(KEGL_Image_Multi_Thread_Parallel, "EGL_Image_Multi_Thread_Parallel");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Thread_Parallel) : public CEglTest_MThread_Image_Base
	{
public:
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KEGL_Image_Multi_Thread_Sibling_Basic, "EGL_Image_Multi_Thread_Sibling_Basic");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Thread_Sibling_Basic) : public CEglTest_MThread_Image_Base
	{
public:
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KEGL_Image_Multi_Thread_Sibling_VGImage, "EGL_Image_Multi_Thread_Sibling_VGImage");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage) : public CEglTest_MThread_Image_Base
	{
public:
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KEGL_Image_Multi_Thread_Sibling_VGImage_PassingEGLImage, "EGL_Image_Multi_Thread_Sibling_VGImage_PassingEGLImage");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Thread_Sibling_VGImage_PassingEGLImage) : public CEglTest_MThread_Image_Base
	{
public:
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KEGL_Image_Multi_Thread_Exit_Thread, "EGL_Image_Multi_Thread_Exit_Thread");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Thread_Exit_Thread) : public CEglTest_MThread_Image_Base
	{
public:
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KEGL_Image_Multi_Thread_DrawAfterTerminate, "EGL_Image_Multi_Thread_DrawAfterTerminate");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Thread_DrawAfterTerminate) : public CEglTest_MThread_Image_Base
	{
public:
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

#endif
