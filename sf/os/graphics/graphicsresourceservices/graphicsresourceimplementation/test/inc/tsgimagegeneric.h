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

#ifndef TSGIMAGEGENERIC_H
#define TSGIMAGEGENERIC_H

#include "tgraphicsresourceteststepbase.h"

/**
Class containing all the positive, negative and panic tests for RSgImage.

KErrNoMemory testing can optionally be enabled.
 */
class CTSgImageGeneric : public CTSgTestStepBase
{
public:
	CTSgImageGeneric(TBool aConformanceTests);
	~CTSgImageGeneric();
	
	//positive tests
	void TestGetPixelFormatsL();
	void TestCreateImageUninitializedL();
	void TestCreateImageL();
	void TestCreateImageFromExistingImageL();	
	void TestGetImageInfoL();
	void TestGetImageDrawableIdL();	
	void TestOpenImageL();
	void TestGetInterfaceL();
	void TestCreateImageAllPixelFormatsL();
	void TestCreateImageDataStrideL();
	void TestOpenImageMultithreadedL();
	
	//negative tests
	void TestGetPixelFormatsInvalidL();
	void TestOpenImageInvalidL();
	void TestCloseImageManyTimesL();
	void TestCloseImageWithoutOpenL();
	void TestCreateImageInvalidL();
	void TestGetInfoImageInvalidL();
	void TestGetAttributesImageInvalidL();
		
	//stress tests
	void TestStress1L();
	void TestStress2L();

private:
	//panic tests
	void TestPanicImageGetInterfaceInvalidHandleL();
	void TestPanicImageGetInterfaceNoDriverL();
	void TestPanicImageCloseInvalidHandleL();
	void TestPanicImageCloseNoDriverL();
	void TestPanicImageIdInvalidHandleL();
	void TestPanicImageIdNoDriverL();
	void TestPanicImageCreateInvalidHandleL();
	void TestPanicImageGetInfoInvalidHandleL();
	void TestPanicImageGetInfoNoDriverL();
	void TestPanicImageOpenNoDriverL();
	void TestPanicImageCreateNoDriver1L();
	void TestPanicImageCreateNoDriver2L();
	void TestPanicImageDrawableTypeNoDriverL();
	void TestPanicImageDrawableTypeInvalidHandleL();
	void TestPanicImageGetAttributeInvalidHandleL();
	void TestPanicImageGetAttributeNoDriverL();
	void TestPanicImageAttributeArrayInvalidIndexL();
	void TestPanicImageAttributeArrayInvalidIndex2L();
	void TestPanicDriverCloseOpenResourcesL();
	void TestCreateOpenCloseImageAndDriverInSecondThreadL();

	// utility methods
	void DestroyImages();
    
	// From CTSgTestStepBase
	virtual TVerdict doTestStepL();
		
private:
    RArray<RSgImage> iTestImages;
};

_LIT(KTSgImageGeneric,"TSgImageGeneric");

#endif /*TSGIMAGEGENERIC_H*/
