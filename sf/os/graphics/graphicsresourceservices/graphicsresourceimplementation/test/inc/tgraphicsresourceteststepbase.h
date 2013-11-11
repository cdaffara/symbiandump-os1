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

#ifndef TGRAPHICSRESOURCETESTSTEPBASE_H
#define TGRAPHICSRESOURCETESTSTEPBASE_H

#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <test/ttmsgraphicsstep.h>
#include <sgresource/sgimage.h>
#include "tgraphicsresourcemultiprocessthread.h"

_LIT(KSgTestGenericPanicCategory, "SGRES");

/**
Default data stride used by RSgImage Create tests. 
 */
const TInt KCrossImageDataStride = 16;

const TInt64 KFakeDrawableId = 0xFFFFFFFFFFFFFFFF;

const TSgImageInfo KSgImageInfo1(TSize(), 0, 0);
const TSgImageInfo KSgImageInfo2(TSize(), 0, 0);
const TSgDrawableId KFakeSgDrawableId = {0xFFFFFFFFFFFFFFFF};

const TInt KSecondThreadMinHeapSize = 0x1000;
const TInt KSecondThreadMaxHeapSize = 0x5000;

class CTSgTestStepBase : public CTTMSGraphicsStep
	{
public:
	CTSgTestStepBase(TBool aConformanceTests);
	~CTSgTestStepBase();
	
	// common utility functions
	static TBool CompareInfos(TSgImageInfo& info1, TSgImageInfo& info2);
	static TInt CreateImageAndReturnCopy(RSgImage& aSgImage);
		
	// from CTTMSGraphicsStep
	inline void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine);
	inline void testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine);
	
protected:
	// From CTestStep
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();	
	
	//common utility functions
	void CreateImageL(RSgImage& aImage);
	TInt CreateSecondProcessAndDoTestL(const TDesC &aProcessName, TSgProcessTestInfo& aTestInfo);
	TInt CreateSecondThreadAndDoTestL(TSgThreadTestInfo aTestInfo);
	void CreateSecondThreadAndCheckPanicL(TSgThreadTestInfo aTestInfo, TInt aPanicCode, TExitCategoryName aExitCategory, const TDesC &aThreadName);
	void CreateSecondProcessAndCheckPanicL(TSgProcessTestInfo& aTestInfo, TInt aPanicCode, TExitCategoryName aExitCategory, const TDesC &aProcessName);
	void CallGetPixelFormatsL(TUint32 aUsage);
	void TestGetPixelFormatCompatibilityGuaranteesL();
	TBool CheckPixelFormatPresent(TSgPixelFormat aPixelFormat);
	void TestOpenDriverL();
	void TestCloseDriver();
	void CheckErrorL(TInt aExpectedErrorCode, TInt aActualErrorCode, const TText8* aFile, TInt aLine);

protected:	
	RThread iSecondThread;
	TUint32* iDiagonalImageData;
	TBool iEnableConformanceTests; //if ETrue runs the conformance tests, otherwise runs the OOM tests
private:
	RArray<TInt> iPixelFormatArray;
	RSgDriver iSgDriver;
	};

/**
Utility class that contains the behaviour for second-thread testing.
 */
class SgTestSecondThread 
	{
public:
	static TInt ThreadMainL(TSgThreadTestInfo* aInfo);
	static TInt ThreadStart(TAny* aInfo);

	//Second Thread Test Functions	
	static TInt OpenImage(TSgThreadTestInfo* aInfo, RSgDriver& aSgDriver);
	static TInt OpenDrawable(TSgThreadTestInfo* aInfo);
	static TInt OpenImageInvalid(TSgThreadTestInfo* aInfo);
	static TInt OpenDrawableInvalid(TSgThreadTestInfo* aInfo);
	static TInt PanicImageGetInterfaceInvalidHandle(RSgDriver& aSgDriver);
	static TInt PanicImageGetInterfaceNoDriver(RSgDriver& aSgDriver);
	static TInt PanicImageCloseInvalidHandle(RSgDriver& aSgDriver);
	static TInt PanicImageCloseNoDriver(RSgDriver& aSgDriver);
	static TInt PanicImageIdInvalidHandle(RSgDriver& aSgDriver);
	static TInt PanicImageIdNoDriver(RSgDriver& aSgDriver);
	static TInt PanicImageDrawableTypeInvalidHandle(RSgDriver& aSgDriver);
	static TInt PanicImageDrawableTypeNoDriver(RSgDriver& aSgDriver);
	static TInt PanicImageCreateInvalidHandle(RSgDriver& aSgDriver);
	static TInt PanicImageGetInfoInvalidHandle(RSgDriver& aSgDriver);
	static TInt PanicImageGetInfoNoDriver(RSgDriver& aSgDriver);
	static TInt PanicImageGetAttributeInvalidHandle(RSgDriver& aSgDriver);
	static TInt PanicImageGetAttributeNoDriver(RSgDriver& aSgDriver);
	static TInt MultipleThreadStressTest(TSgThreadTestInfo* aInfo);
	static void PanicAttributeArrayInvalidIndexL();
	static void PanicAttributeArrayInvalidIndex2L();
	static TInt CreatePassedImageL(RSgImage* aSgImage);
	};
/**
Test Interface used by RSgImage::GetInterface() tests.
 */
class MTSgImage_Interface
	{
public:
	enum { EInterfaceUid = 0x102858F0 };
public:
	virtual TAny* DataAddress() const = 0;
	virtual TInt DataStride() const = 0;
	};

//common variables and functions
const TInt KDiagonalImageSize = 400; //size in pixels
const TInt KDiagonalImageDataStride = 400 * 4;

const TUint16 KCrossImageData[] = {
			0x7E0,0x7E0,0x7E0,0x000,0x000,0x7E0,0x7E0,0x7E0,
			0x7E0,0x7E0,0x7E0,0x000,0x000,0x7E0,0x7E0,0x7E0,
			0x7E0,0x7E0,0x7E0,0x000,0x000,0x7E0,0x7E0,0x7E0,
			0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,
			0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,
			0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,
			0x7E0,0x7E0,0x7E0,0x000,0x000,0x7E0,0x7E0,0x7E0,
			0x7E0,0x7E0,0x7E0,0x000,0x000,0x7E0,0x7E0,0x7E0
			};

#define TESTWITHFILENAMEANDLINENUMBERL(a, f, l) \
	{\
	testBooleanTrue((a), f, l); \
	}

inline void CTSgTestStepBase::testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine) 
	{
	MQCTest(aCondition, aFile, aLine);
	}

inline void CTSgTestStepBase::testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine)
	{
	MQCTestWithErrorCode(aCondition, aErrorCode, aFile, aLine);
	}

_LIT(KSecondProcess, "tgraphicsresourcesecondprocess2.exe");

_LIT(KSecondThread, "Test_GraphicsResource_SecondThread");
_LIT(KSecondThreadSemaphore, "Second Thread Semaphore");

#endif
