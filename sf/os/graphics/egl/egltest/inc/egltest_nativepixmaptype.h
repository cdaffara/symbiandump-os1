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

#ifndef __EGLTEST_NATIVEPIXMAPTYPE_H__
#define __EGLTEST_NATIVEPIXMAPTYPE_H__

#include "eglteststep.h"

// Concrete tests for testing use of eglSetNativePixmap()

// Multi Threaded

_LIT(KNativePixmapType_SingleThread_Positive_RSgImage, "NativePixmapType_SingleThread_Positive_RSgImage");
NONSHARABLE_CLASS(CEglTest_NativePixmapType_SingleThread_Positive_RSgImage) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};
	
_LIT(KNativePixmapType_MultiThread_Positive_RSgImage_InitTwice, "NativePixmapType_MultiThread_Positive_RSgImage_InitTwice");
NONSHARABLE_CLASS(CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KNativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous, "NativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous");
NONSHARABLE_CLASS(CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

_LIT(KNativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly, "NativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly");
NONSHARABLE_CLASS(CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doThreadFunctionL(TInt aIdx);
	};

	
// Multi Process	
_LIT(KNativePixmapType_MultiProcess_Both_RSgImage, "NativePixmapType_MultiProcess_Both_RSgImage");
NONSHARABLE_CLASS(CEglTest_NativePixmapType_MultiProcess_Both_RSgImage) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
protected:
	// from CEglTestStep
	void doProcessFunctionL(TInt aIdx);
	};

_LIT(KChooseConfig_EGL_MATCH_NATIVE_PIXMAP_KHR, "ChooseConfig_EGL_MATCH_NATIVE_PIXMAP_KHR");
NONSHARABLE_CLASS(CEglTest_ChooseConfig_EGL_MATCH_NATIVE_PIXMAP_KHR) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	TVerdict doTestPartialStepL();
private:
	TUint32	iSgImageSurfaceUsage;
	};

_LIT(KCFbsBitmap_EGL_MATCH_NATIVE_PIXMAP_KHR, "CFbsBitmap_EGL_MATCH_NATIVE_PIXMAP_KHR");
NONSHARABLE_CLASS(CEglTest_CFbsBitmap_EGL_MATCH_NATIVE_PIXMAP_KHR) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	TVerdict doTestPartialStepL();
	};

#endif // __EGLTEST_SETNATIVEPIXMAPTYPE_H__
