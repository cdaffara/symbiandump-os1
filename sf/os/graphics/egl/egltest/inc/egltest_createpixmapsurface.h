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

#include "eglteststep.h"

// Concrete tests for testing use of eglCreateSurfacePixmap()

_LIT(KCreatePixmapSurface_RSgImage_Negative_RSgImage_NotInitialised, "CreatePixmapSurface_RSgImage_Negative_RSgImage_NotInitialised");
NONSHARABLE_CLASS(CEglTest_CreatePixmapSurface_RSgImage_Negative_RSgImage_NotInitialised) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};
	

_LIT(KCreatePixmapSurface_Negative_Invalid_Pixmap_Type, "CreatePixmapSurface_Negative_Invalid_Pixmap_Type");
NONSHARABLE_CLASS(CEglTest_CreatePixmapSurface_Negative_Invalid_Pixmap_Type) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};


_LIT(KCreatePixmapSurface_Alpha_bit_RSgImage, "CreatePixmapSurface_Alpha_bit_RSgImage");
NONSHARABLE_CLASS(CEglTest_CreatePixmapSurface_Alpha_bit_RSgImage) : public CEglTestStep
	{
public:
	TVerdict doTestStepL();
	};


_LIT(KCreatePixmapSurface_RSgImage_Positive_MantadoryFormatUsageSupport, "CreatePixmapSurface_RSgImage_Positive_MantadoryFormatUsageSupport");
NONSHARABLE_CLASS(CEglTest_CreatePixmapSurface_RSgImage_Positive_MantadoryFormatUsageSupport) : public CEglTestStep
    {
public:
    TVerdict doTestStepL();
    };
