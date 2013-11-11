// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __EGLTEST_IMAGE_MULTIPROCESS_H__
#define __EGLTEST_IMAGE_MULTIPROCESS_H__

#include "eglteststep.h"


_LIT(KEGL_Image_Multi_Process_Sibling_Basic, "EGL_Image_Multi_Process_Sibling_Basic");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_Sibling_Basic) : public CEglTestStep
	{
public:
	// from CTestStep
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doProcessFunctionL(TInt aIdx,const TSgDrawableId& aSgId);
	};

_LIT(KEGL_Image_Multi_Process_Sibling_CheckContents, "EGL_Image_Multi_Process_Sibling_CheckContents");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_Sibling_CheckContents) : public CEglTestStep
	{
public:
	// from CTestStep
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doProcessFunctionL(TInt aIdx,const TSgDrawableId& aSgId);
	};

_LIT(KEGL_Image_Multi_Process_Parallel, "EGL_Image_Multi_Process_Parallel");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_Parallel) : public CEglTestStep
	{
public:
	// from CTestStep
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doProcessFunctionL(TInt aIdx);
	};

_LIT(KEGL_Image_Multi_Process_VgImage_Source, "EGL_Image_Multi_Process_VgImage_Source");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_VgImage_Source) : public CEglTestStep
    {
public:
	// from CTestStep
	TVerdict doTestStepL();
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
    };
	
_LIT(KEGL_Image_Multi_Process_VgImage_DrawAfterTerminate, "EGL_Image_Multi_Process_VgImage_DrawAfterTerminate");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_VgImage_DrawAfterTerminate) : public CEglTestStep
    {
public:
	// from CTestStep
	TVerdict doTestStepL();
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
    };

_LIT(KEGL_Image_Multi_Process_FontServer_Upfront, "EGL_Image_Multi_Process_FontServer_Upfront");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_FontServer_Upfront) : public CEglTestStep
	{
public:	
	// from CTestStep
	TVerdict doTestStepL();
public:
	TVerdict doTestPartialStepL();
protected:
	// from CEglTestStep
	void doProcessFunctionL(TInt aIdx, const TSgDrawableId& aSgId);
	};

_LIT(KEGL_Image_Multi_Process_FontServer_Deferred, "EGL_Image_Multi_Process_FontServer_Deferred");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_FontServer_Deferred) : public CEglTestStep
	{
public:	
	// from CTestStep
	TVerdict doTestStepL();
public:
	TVerdict doTestPartialStepL();
protected:
	// from CEglTestStep
	void doProcessFunctionL(TInt aIdx, const TSgDrawableId& aSgId);
	};

_LIT(KEGL_Image_Multi_Process_ThemeServer, "EGL_Image_Multi_Process_ThemeServer");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_ThemeServer) : public CEglTestStep
	{
public:	
	// from CTestStep
	TVerdict doTestStepL();
public:
	TVerdict doTestPartialStepL();
protected:
	// from CEglTestStep
	void doProcessFunctionL(TInt aIdx, const TSgDrawableId& aSgId);
	};

_LIT(KEGL_Image_Multi_Process_VgImage_ProcessTerminate, "EGL_Image_Multi_Process_VgImage_ProcessTerminate");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminate) : public CEglTestStep
    {
public:
	// from CTestStep
	TVerdict doTestStepL();
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
    };

_LIT(KEGL_Image_Multi_Process_VgImage_ProcessTerminateNegative, "EGL_Image_Multi_Process_VgImage_ProcessTerminateNegative");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_VgImage_ProcessTerminateNegative) : public CEglTestStep
    {
public:
	// from CTestStep
	TVerdict doTestStepL();
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
    };

_LIT(KEGL_Image_Multi_Process_VgImage_ReadWrite, "EGL_Image_Multi_Process_VgImage_ReadWrite");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Multi_Process_VgImage_ReadWrite) : public CEglTestStep
    {
public:
	// from CTestStep
	TVerdict doTestStepL();
protected:
    // from CEglTestStep
    void doProcessFunctionL(TInt aIdx);
    };

#endif //__EGLTEST_IMAGE_MULTIPROCESS_H__

