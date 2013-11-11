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

class CTSgImageGeneric : public CTSgTestStepBase
{
public:
	CTSgImageGeneric();
	~CTSgImageGeneric();
private:
	// From CTestStepCTSgImageGeneric
	virtual TVerdict doTestStepL();
	
private:
	//positive tests
	void TestGetPixelFormatsL();
	void TestCreateImageUninitializedL();
	void TestCreateImageL();
	void TestCreateImageFromExistingImageL();	
	void TestGetImageInfoL();
	void TestGetImageDrawableIdL();
	void TestMapImageL();	
	void TestOpenImageL();
	void TestOpenImageInternalL();
	void TestGetInterfaceL();
	
	//negative tests
	void TestGetPixelFormatsInvalidL();
	void TestOpenImageInvalidL();
	void TestCloseImageManyTimesL();
	void TestCloseImageWithoutOpenL();
	void TestCreateImageInvalidL();
	void TestGetInfoInvalidImageL();
	void TestMapImageInvalidL();
	void TestUnmapImageInvalidL();
	
	//out of memory test
	void DoMemoryTestsL();
	
	//stress tests
	void TestStress1L();
	void TestStress2L();

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
	void TestPanicImageMapReadOnlyInvalidHandleL();
	void TestPanicImageMapReadOnlyNoDriverL();
	void TestPanicImageMapWriteOnlyInvalidHandleL();
	void TestPanicImageMapWriteOnlyNoDriverL();
	void TestPanicImageMapReadWriteInvalidHandleL();
	void TestPanicImageMapReadWriteNoDriverL();
	void TestPanicImageUnmapInvalidHandleL();
	void TestPanicImageUnmapNoDriverL();	
	void TestPanicImageOpenNoDriver1L();
	void TestPanicImageOpenNoDriver2L();
	void TestPanicImageCreateNoDriver1L();
	void TestPanicImageCreateNoDriver2L();
	void TestPanicImageGetPixelFormatsNoDriverL();
	void TestPanicImageDrawableTypeNoDriverL();
	void TestPanicImageDrawableTypeInvalidHandleL();
	
	RArray<RSgImage> iTestImages;
};

_LIT(KTSgImageGeneric,"TSgImageGeneric");

#endif /*TSGIMAGEGENERIC_H*/
