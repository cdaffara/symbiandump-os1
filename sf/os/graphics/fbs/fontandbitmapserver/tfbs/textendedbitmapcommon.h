// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TEXTENDEDBITMAPCOMMON_H
#define TEXTENDEDBITMAPCOMMON_H

#include "test/TGraphicsHarness.h"

// Uid for use when testing an extended bitmap that does not need to be
// drawn using the example rasterizer.
const TUid KUidTestExtendedBitmap	= TUid::Uid(0xFFFFFFFF);

// Macros to automatically test error condition and report when test fails.
#define TESTCREATEEXTENDEDBITMAPL(sizeInPixels, displayMode, typeUid, data, dataSize, aExpectedError) \
	{\
	CFbsBitmap* bmp = new(ELeave)CFbsBitmap;\
	TInt res = bmp->CreateExtendedBitmap(sizeInPixels, displayMode, typeUid, data, dataSize);\
	delete bmp;\
	TESTEXPECTEDERRORL(res, aExpectedError);\
	}

#define TESTCREATEEXTENDEDBITMAPINITIALIZERL(sizeInPixels, displayMode, typeUid, data, dataSize, aExpectedError) \
	{\
	CFbsBitmap* bmp2 = new(ELeave)CFbsBitmap;\
	CleanupStack::PushL(bmp2);\
	CTestExtendedBitmapInitializer* initializer = new(ELeave) CTestExtendedBitmapInitializer(data, dataSize);\
	TInt res2 = bmp2->CreateExtendedBitmap(sizeInPixels, displayMode, typeUid, dataSize, *initializer);\
	delete initializer;\
	CleanupStack::PopAndDestroy(bmp2);\
	TESTEXPECTEDERRORL(res2, aExpectedError);\
	}


class CTestExtendedBitmapInitializer : public CBase, public MFbsExtendedBitmapInitializer
	{
public:
	CTestExtendedBitmapInitializer(const TUint8* aTestData, const TInt aTestDataSize);
	
	// From MFbsExtendedBitmapInitializer
	TInt InitExtendedBitmap(TAny* aData, TInt aDataSize);
private:
	const TInt iTestDataSize;
	const TUint8* iTestData;
	};

inline CTestExtendedBitmapInitializer::CTestExtendedBitmapInitializer(const TUint8* aTestData, const TInt aTestDataSize)
: iTestDataSize(aTestDataSize), iTestData(aTestData)
	{
	}

inline TInt CTestExtendedBitmapInitializer::InitExtendedBitmap(TAny* aData, TInt aDataSize)
	{
	if (!iTestData || iTestDataSize < aDataSize)
		{
		// Used to test handling of initializer error by CreateExtendedBitmap() and for
		// GRAPHICS-FBSERV-EXTENDEDBITMAP-0014 - CTExtendedBitmapNegative::CreateNullDataL()
		// to be returned the correct error on providing NULL data
		return KErrUnderflow;
		}
	Mem::Copy(aData, iTestData, aDataSize);
	return KErrNone;
	}


#endif // TEXTENDEDBITMAPCOMMON_H
