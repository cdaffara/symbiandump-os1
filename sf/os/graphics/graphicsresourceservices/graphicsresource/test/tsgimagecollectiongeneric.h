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

#ifndef TSGIMAGECOLLECTIONGENERIC_H
#define TSGIMAGECOLLECTIONGENERIC_H

#include "tgraphicsresourceteststepbase.h"

class CTSgImageCollectionGeneric : public CTSgTestStepBase
	{
public:
	CTSgImageCollectionGeneric();
	~CTSgImageCollectionGeneric();
private:
	// From CTestStep
	virtual TVerdict doTestStepL();
private:
	//positive tests
	void TestCreateImageCollectionL();
	void TestGetImageCollectionSurfaceIdL();
	void TestGetCollectionImageNumberL();
	void TestGetImageInfoInCollectionL();
	void TestOpenImageInCollectionL();
	void TestMapImageInCollectionL();
	void TestCreateMultipleImageCollectionL();
	
	//negative tests
	void TestCreateImageCollectionInvalidL();
	void TestCreateMultipleImageCollectionsInvalidL();
	void TestGetImageCollectionInfoInvalidL();
	void TestOpenImageInCollectionInvalidL();	
	void TestMapUnmapImagesInCollectionInvalidL();	
	void TestPanicImageCollectionCreateNoDriver1L();
	void TestPanicImageCollectionCreateNoDriver2L();
	
	//out of memory tests
	void DoMemoryTestsL();
	
	//panic tests
	void TestPanicImageCollectionCloseInvalidHandleL();
	void TestPanicImageCollectionCloseNoDriverL();
	void TestPanicImageCollectionSurfaceIdInvalidHandleL();
	void TestPanicImageCollectionSurfaceIdNoDriverL();
	void TestPanicImageCollectionGetInfoInvalidHandleL();
	void TestPanicImageCollectionGetInfoNoDriverL();
	void TestPanicImageCollectionCountInvalidHandleL();
	void TestPanicImageCollectionCountNoDriverL();
	void TestPanicImageCollectionOpenImageInvalidHandleL();
	void TestPanicImageCollectionOpenImageNoDriverL();
	};

_LIT(KTSgImageCollectionGeneric,"TSgImageCollectionGeneric");

#endif /*TSGIMAGECOLLECTIONGENERIC_H*/
