// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef __T_WINUTILS_H__
#define __T_WINUTILS_H__

#include <e32std.h>
#include <w32std.h>
#include <displaymode.h>
#include <fbs.h>
#include <bitdev.h>
#include <iniparser.h>

class CTestBitmap : public CBase
	{
public:
	IMPORT_C static CTestBitmap* NewLC(const TSize& aSizeInPixels,TDisplayMode aDispMode);
	IMPORT_C static CTestBitmap* NewL(const TSize& aSizeInPixels,TDisplayMode aDispMode);
	IMPORT_C static CTestBitmap* NewL(TInt aHandle);
	inline CFbsBitmap& Bitmap() const {return *iBitmap;}
	inline CFbsBitmapDevice& Device() const {return *iDevice;}
	inline CFbsBitGc& Gc() const {return *iGc;}
	~CTestBitmap();

private:
	static CTestBitmap* NewLC(TInt aHandle,const TSize& aSizeInPixels,TDisplayMode aDispMode);
	void ConstructL(TInt aHandle,const TSize& aSizeInPixels,TDisplayMode aDispMode);

private:
	CFbsBitmap* iBitmap;
	CFbsBitmapDevice* iDevice;
	CFbsBitGc* iGc;
	};

class TImportPseudoAppConfig
	{
public:
	IMPORT_C static void ImportPseudoAppConfigL(TInt aScreenNo, TInt& aFrameDuration, TInt& aFrameDelay,
	                                            TDisplayMode& aMode, TSize& aScreenSize, CIniData* aUtils);
	};

#endif //__T_WINUTILS_H__
