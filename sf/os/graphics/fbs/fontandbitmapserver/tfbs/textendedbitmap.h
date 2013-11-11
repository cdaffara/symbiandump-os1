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

#ifndef TEXTENDEDBITMAP_H
#define TEXTENDEDBITMAP_H

#include "tfbsbase.h"
#include <graphics/fbsrasterizerclearcache.h>

/** Class used for positive creation and scanline tests of extended bitmaps.
 */
class CTExtendedBitmap : public CTFbsBase
	{
public:
	CTExtendedBitmap(CTestStep* aStep);
	~CTExtendedBitmap();
	
protected:
	// From CTFbsBase
	virtual void RunFbsTestL(TInt aCurTestCase);
	void ConstructL();
	
private:
	void CreateExtendedBitmapL();
	void CreateLargeExtendedBitmapL();
	void CreateUsingInitializerL();
	void ScanLineL(TBool aUseDuplicateBitmap);
	void TestGetPixelL(TBool aUseDuplicateBitmap);
	static void ClearRasterizerCache(TAny* aPtr);
	void TestGetVerticalScanLineL(TBool aUseDuplicateBitmap);
	
private:
	TBool iRasterizerAvailable;	
	TUint8* iLargeData;
	MFbsRasterizerClearCache* iRasterizerClearCache;
	};


class CTExtendedBitmapStep : public CTGraphicsStep
	{
public:
	CTExtendedBitmapStep();
	
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();	
	};

_LIT(KTExtendedBitmapStep,"TExtendedBitmap");

#endif // TEXTENDEDBITMAP_H
