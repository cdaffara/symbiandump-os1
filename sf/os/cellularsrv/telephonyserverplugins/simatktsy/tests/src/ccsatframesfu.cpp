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
 @file The TEFUnit test suite for Frames in the SAT.
*/

#include "ccsatframesfu.h"
#include <etel.h>
#include <etelmm.h>
#include <test/tmockltsydata.h>

CTestSuite* CCSatFramesFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	
	ADD_TEST_STEP_ISO_CPP(CCSatFramesFU, TestNotifySetFramesPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatFramesFU, TestNotifyGetFramesStatusPCmd0001L);
	
	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-FRMS-SFRMPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetFramesPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetFramesPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatFramesFU::TestNotifySetFramesPCmd0001L()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
		
	TRequestStatus requestStatus;	
	// Post Notify itself
	RSat::TSetFramesV6 frames;
	RSat::TSetFramesV6Pckg framesPck(frames);
	// SET FRAMES proactive command is not supported in current version of Sat
	iSat.NotifySetFramesPCmd(requestStatus, framesPck);		
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this

	}


/**
@SYMTestCaseID BA-CSAT-FRMS-GFRMSPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyGetFramesStatusPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyGetFramesStatusPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatFramesFU::TestNotifyGetFramesStatusPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
			
	TRequestStatus requestStatus;	
	// Post Notify itself
	RSat::TGetFramesStatusV6 frames;
	RSat::TGetFramesStatusV6Pckg framesPck(frames);
	// GET FRAME STATUS proactive command is not supported in current version of Sat
	iSat.NotifyGetFramesStatusPCmd(requestStatus, framesPck);		
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this
	}

