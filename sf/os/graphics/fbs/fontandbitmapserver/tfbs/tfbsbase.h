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

#ifndef TFBSBASE_H
#define TFBSBASE_H

#include "test/TGraphicsHarness.h"
#include <f32file.h>

#define INFO_PRINTF9(p1, p2, p3, p4, p5, p6, p7, p8, p9) \
	{\
	Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8), (p9));\
	}

#define ERR_PRINTF9(p1, p2, p3, p4, p5, p6, p7, p8, p9) \
	{\
	Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8), (p9));\
	}

#define TESTTRUEL(a) \
	{\
	TBool b = a;\
	if(!b){\
		if(iCurrentRunIsLowMemory)\
			{\
			INFO_PRINTF1(_L("TESTTRUEL failed."));\
			}\
		else\
			{\
			ERR_PRINTF1(_L("TESTTRUEL failed."));\
			}\
		User::Leave(KErrGeneral);\
		}\
	}

#define TESTEQUALL(actual, expected) \
	{\
	TBool b = (actual == expected);\
	if(!b){\
		if(iCurrentRunIsLowMemory)\
			{\
			INFO_PRINTF3(_L("TESTEQUALL failed. Expected %i, got %i"), expected, actual);\
			}\
		else\
			{\
			ERR_PRINTF3(_L("TESTEQUALL failed. Expected %i, got %i"), expected, actual);\
			}\
		User::Leave(KErrGeneral);\
		}\
	}

#define TESTSIZEEQUALL(actual, expected) \
	{\
	TSize a = actual;\
	TSize b = expected;\
	TBool c = (actual == expected);\
	if(!c){\
		if(iCurrentRunIsLowMemory)\
			{\
			INFO_PRINTF5(_L("TESTEQUALL failed. Expected (%i,%i), got (%i,%i)"), b.iHeight, b.iWidth, a.iHeight, a.iWidth);\
			}\
		else\
			{\
			ERR_PRINTF5(_L("TESTEQUALL failed. Expected (%i,%i), got (%i,%i)"), b.iHeight, b.iWidth, a.iHeight, a.iWidth);\
			}\
		User::Leave(KErrGeneral);\
		}\
	}

#define TESTCOLOREQUALL(actual, expected) \
	{\
	TRgb a = actual;\
	TRgb b = expected;\
	TBool c = (actual == expected);\
	if(!c){\
		if(iCurrentRunIsLowMemory)\
			{\
			INFO_PRINTF9(_L("TESTEQUALL failed. Expected (RGBA) (%i,%i,%i,%i), got (%i,%i,%i,%i)"), b.Red(), b.Green(), b.Blue(), b.Alpha(), a.Red(), a.Green(), a.Blue(), a.Alpha());\
			}\
		else\
			{\
			ERR_PRINTF9(_L("TESTEQUALL failed. Expected (RGBA) (%i,%i,%i,%i), got (%i,%i,%i,%i)"), b.Red(), b.Green(), b.Blue(), b.Alpha(), a.Red(), a.Green(), a.Blue(), a.Alpha());\
			}\
		User::Leave(KErrGeneral);\
		}\
	}

#define TESTNOTEQUALL(actual, notexpected) \
	{\
	TBool b = (actual == notexpected);\
	if(b){\
		if(iCurrentRunIsLowMemory)\
			{\
			INFO_PRINTF2(_L("TESTNOTEQUALL failed. Expected not to get %i but did"), notexpected);\
			}\
		else\
			{\
			ERR_PRINTF2(_L("TESTNOTEQUALL failed. Expected not to get %i but did"), notexpected);\
			}\
		User::Leave(KErrGeneral);\
		}\
	}

#define TESTNOERRORL(a) \
	{\
	TInt b = a;\
	if(KErrNone != b){\
		if(iCurrentRunIsLowMemory)\
			{\
			INFO_PRINTF2(_L("Error code returned: %i"), b);\
			}\
		else\
			{\
			ERR_PRINTF2(_L("Unexpected error code returned: %i"), b);\
			}\
		User::Leave(b);\
		}\
	}
	
#define TESTEXPECTEDERRORL(a, aExpectedError) \
	{\
	TInt b = a;\
	if(b != aExpectedError){\
		if(iCurrentRunIsLowMemory)\
			{\
			INFO_PRINTF3(_L("Expected error %i, but instead returned: %i"), aExpectedError, b);\
			}\
		else\
			{\
			ERR_PRINTF3(_L("Expected error %i, but instead returned: %i"), aExpectedError, b);\
			}\
		User::Leave(b);\
		}\
	}

/** Base class used by all fbs extended bitmap test classes. Allows out of memory testing to
be carried out easily for each test case. 
 */
class CTFbsBase : public CTGraphicsBase
	{
public:
	CTFbsBase(CTestStep* aStep, TBool aRunWithLowMemory);
	~CTFbsBase();
	
	static TRgb ExtractRgb(TUint8* aBuffer, TInt aPixelOffset, TDisplayMode aDispMode);

protected:
	// From CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	
	// Fbs test - to be implemented by test class
	virtual void RunFbsTestL(TInt aCurTestCase) = 0;
	
	void SetLastTestCase();

private:
	CTestStep&	iTestStep;
	TBool		iLastTestCase;

protected:
	TBool		iRunWithLowMemory;
	TBool		iCurrentRunIsLowMemory;

	};

#endif //TFBSBASE_H
