// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Simple dummy test, to be used as example for new tests
// 
//

#include <e32std.h>
#include "W32STD.H"
#include "../tlib/testbase.h"
#include "TMAN.H"

class TDummyTest : public CTestBase
	{
public:
	TDummyTest();
	TestState DoTestL();
	void ConstructL();
private:
	TSize iWinSize;
	TInt iState;
	};

GLDEF_C CTestBase *CreateDummyTest()
	{
	return(new(ELeave) TDummyTest());
	}

TDummyTest::TDummyTest() : CTestBase(_L("Dummy"))
	{}

void TDummyTest::ConstructL()
	{
	}

TestState TDummyTest::DoTestL()
	{
	switch(iState)
		{
		case 0:
			LogSubTest(_L("Dummy 1"),1);
			TestL(ETrue);
			iState++;
			break;
		default:
			return(EFinished);
		}
	return(ENext);
	}

