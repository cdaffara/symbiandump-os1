// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TCOMPRESSED_H__
#define __TCOMPRESSED_H__

#include <fbs.h>
#include "TFBS.H"
#include "test/TGraphicsHarness.h"

class CTCompressed : public CTGraphicsBase
	{
public:
	CTCompressed(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void BlankBitmap(CFbsBitmapEx& aBitmap);
	void RunTestL();
	void DefectFix_EXT5DXGK6_L();
	void DefectFix_INC082713_L(const TDisplayMode aDispMode, const TSize aBitmapSize);
	void TestBitBltL(CFbsBitmap& aBmp, TDisplayMode aDispMode);
	void INC088856_TestL();
	void DoUpdateOnPadding(const TSize aSize, const TDisplayMode aDispMode);
	void TestDestroyBitmapWhileBeingBackgroundCompressedL();
	static TInt BgCompThreadFunc(TAny* aArg);
	void TestBackgroundCompressionThreadPriorityInheritanceL();
	static TInt CompressBgThreadFunc(TAny* aArg);
	};

class CTCompressedStep : public CTGraphicsStep
	{
public:
	CTCompressedStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTCompressedStep,"TCompressed");




#endif
