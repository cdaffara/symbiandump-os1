// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __THASREFERENCEIMAGES_H__
#define __THASREFERENCEIMAGES_H__

#include <test/testexecutestepbase.h>
#include <w32std.h>


class CTestStep;

/**
 Class defines all the API's which are used to test outline and shadow effects
 */
class CTHashReferenceImages : public CBase
	{
public:
	IMPORT_C static CTHashReferenceImages* NewL(CTestStep* aStep, RFbsSession* aFbs, const TDesC *aPath);
	IMPORT_C virtual ~CTHashReferenceImages();
	CTHashReferenceImages(CTestStep* aStep, RFbsSession* aFbs);
	IMPORT_C void CompareHashValuesL(const TDesC& aIndexStr);
	IMPORT_C void GenerateHashAndReturnInHexFormatL(TDes &aHexString);
	IMPORT_C void SetScreenDeviceAndBitmap(CBitmapDevice* aBitmapDevice, CFbsBitmap* aBitmap, CFbsBitGc* aGc );
	IMPORT_C HBufC* GenerateHashIdStringLC(const TDesC& aTestCase, TInt aSubTestNumber, const TPtrC aName[], TInt aNameIndex,
								TInt aDisplayMode, TInt aOrientation);
	IMPORT_C void CopyScreenToBitmapL(const TDesC& aHashIndex);
protected:
	void ConstructL(const TDesC *aPath);
private:
	CFbsBitGc* iGc; //not owned
	CBitmapDevice* iBitmapDevice; //now owned
	RFbsSession* iFbs; //not owned
	CFbsBitmap* iBitmap; //not owned
	CTestStep* iStep; //not owned
	HBufC *iPath;
	};

#endif
