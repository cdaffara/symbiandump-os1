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

#ifndef EGLTEST_SIBILING_H_
#define EGLTEST_SIBILING_H_

#include "eglteststep.h"

_LIT(KEGL_Image_Sibling_Basic, "EGL_Image_Sibling_Basic");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Sibling_Basic) : public CEglTestStep
	{
public:	
	TVerdict doTestStepL();	
	};

_LIT(KEGL_Image_Sibling_VGImage, "EGL_Image_Sibling_VGImage");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Sibling_VGImage) : public CEglTestStep
	{
public:	
	TVerdict doTestStepL();	
	};	

_LIT(KEGL_Image_Sibling_Two_VGImages, "EGL_Image_Sibling_Two_VGImages");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Sibling_Two_VGImages) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};

_LIT(KEGL_Image_Sibling_VGImage_Child, "EGL_Image_Sibling_VGImage_Child");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Sibling_VGImage_Child) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};

_LIT(KEGL_Image_CreatePBufferFromClient_With_Sibling_VGImage, "EGL_Image_CreatePBufferFromClient_With_Sibling_VGImage");
NONSHARABLE_CLASS(CEglTest_EGL_Image_CreatePBufferFromClient_With_Sibling_VGImage) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};

_LIT(KEGL_Image_Sibling_VGImage_Child_CheckContents, "EGL_Image_Sibling_VGImage_Child_CheckContents");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Sibling_VGImage_Child_CheckContents) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};

_LIT(KEGL_Image_Sibling_VGImage_Child_CheckContents_CloseSgEarlier, "EGL_Image_Sibling_VGImage_Child_CheckContents_CloseSgEarlier");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Sibling_VGImage_Child_CheckContents_CloseSgEarlier) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};

#endif /*EGLTEST_SIBILING_H_*/
