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
 @internalComponent - Internal Symbian test code 
*/
 
#ifndef TGRAPHICSRESOURCE_H
#define TGRAPHICSRESOURCE_H

#include "te_graphicsperformanceSuiteStepBase.h"
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <graphics/sgimage.h>
#endif
// The size of the image data in TUints. 
const TInt KMaxArraySize = 360000;
// The datastride to use when creating an RSgImage.
const TInt KImageDataStride = 16;

class CTGraphicsResource : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTGraphicsResource();
	~CTGraphicsResource();

private:
	static void CloseAndDestroyImageHandle(TAny* aPtr);
	void ImageCreationL(const TSize aSizes[], TInt aMaxNumImages);
	void ImageCreationSimpleL(const TInt aWidth, const TInt aHeight, const TDesC& aTestDescription);
	void SmallImageCreationSimpleL();
	void LargeImageCreationSimpleL();
	void ImageDuplicateL();
	void ImageDuplicateHandleL();
	void ImageMapL();
	
	// From CTestStep
	virtual TVerdict doTestStepL();

private:
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	TSgImageInfo iImageInfo;
#endif
	TUint16 iImageData[KMaxArraySize];
	};

_LIT(KTGraphicsResource,"tgraphicsresourceperformance");

#endif
