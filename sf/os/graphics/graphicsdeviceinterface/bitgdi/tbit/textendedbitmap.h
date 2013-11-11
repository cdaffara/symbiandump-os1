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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef TEXTENDEDBITMAP_H
#define TEXTENDEDBITMAP_H

#include <test/tgraphicsharness.h>
#include <test/testharnesscallback.h>
#include <test/textendedbitmapgc.h>
#include <s32mem.h>
#include <w32std.h>

/** Defines whether the tests will write their results to a screen device or
to a bitmap device (if not defined). Comment out to write to a bitmap device. 
 */
#define USE_SCREEN_DEVICE

/** Test class for testing the operation of extended bitmaps when used with BitGdi methods.
This class creates a graphics context that is passed to a separate DLL that draws the tests
to the passed graphics context (textendedbitmapgc.dll). textendedbitmapgc.dll is built as a
test component by FbServ.
 */
class CTExtendedBitmap : public CTGraphicsBase, public MTestHarnessCallBack
	{
public:
	CTExtendedBitmap(CTestStep* aStep);
	~CTExtendedBitmap();

	//from MTestHarnessCallBack
	virtual CTestExecuteLogger& Logger();
	virtual void TestTrue(TBool aCondition);
	virtual void TestComplete();

protected:	
	//from CTGraphicsBase
	virtual void ConstructL();
	//from MTestCases (via CTGraphicsBase)
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
#ifndef USE_SCREEN_DEVICE
	CFbsBitmap* iBitmap;
#endif
	CFbsDevice* iDevice;
	CFbsBitGc* iBitGc;
	CTExtendedBitmapGc* iExtendedBitmapGcTests;
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



class CTExtendedBitmapNegative : public CTGraphicsBase
	{
public:
	CTExtendedBitmapNegative(CTestStep* aStep);

protected:	
	//from MTestCases (via CTGraphicsBase)
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
	// Test cases
	void TestCFbsBitmapDeviceNewLLeavesL();
	};

class CTExtendedBitmapNegativeStep : public CTGraphicsStep
	{
public:
	CTExtendedBitmapNegativeStep();
	
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTExtendedBitmapNegativeStep,"TExtendedBitmapNegative");

#endif // TEXTENDEDBITMAP_H
