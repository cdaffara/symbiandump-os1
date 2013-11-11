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

#ifndef __EGLTEST_IMAGE_H__
#define __EGLTEST_IMAGE_H__

#include "eglteststep.h"


_LIT(KEGL_Image_RSgImage_UseOpenVG_PersistImageData, "EGL_Image_RSgImage_UseOpenVG_PersistImageData");
NONSHARABLE_CLASS(CEglTest_EGL_Image_RSgImage_UseOpenVG_PersistImageData) : public CEglTestStep
	{
public:	
	TVerdict doTestStepL();	
	};
	
_LIT(KEGL_Image_Consistent_Linked_List, "EGL_Image_Consistent_Linked_List");
NONSHARABLE_CLASS(CEglTest_EGL_Image_Consistent_Linked_List) : public CEglTestStep
	{
public:	
	TVerdict doTestStepL();	
	};

_LIT(KEGL_Image_SgDriverHandle, "EGL_Image_SgDriverHandle");
NONSHARABLE_CLASS(CEglTest_EGL_Image_SgDriverHandle) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};

#endif // __EGLTEST_KHR_IMAGE_H__
