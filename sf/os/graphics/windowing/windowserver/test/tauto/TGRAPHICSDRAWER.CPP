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
// Tests for the TWsGraphicMsgBufParser 
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TGRAPHICSDRAWER.H"
#include "graphics/WSGRAPHICMSGBUF.H"

CTGraphicsDrawer::CTGraphicsDrawer(CTestStep* aStep):
	CTGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("Testing TWsGraphicMsgBufParser"));
	}
		
void CTGraphicsDrawer::RunTestCaseL(TInt aCurTestCase)
	{	
	((CTGraphicsDrawerStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:		
		((CTGraphicsDrawerStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0449"));
		INFO_PRINTF1(_L("TWsGraphicMsgBufParser Indefinite Loop Test"));		
		IndefiniteLoopL();
		break;
		
	case 2:
		((CTGraphicsDrawerStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTGraphicsDrawerStep*)iStep)->CloseTMSGraphicsStep();
		INFO_PRINTF1(_L("Test complete\n"));
		TestComplete();
		break;
		}
	((CTGraphicsDrawerStep*)iStep)->RecordTestResultL();
	}


void CTGraphicsDrawer::ConstructL()
	{
	}
	
CTGraphicsDrawer::~CTGraphicsDrawer()
	{
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0449

@SYMDEF  			DEF110513, PDEF112710

@SYMTestCaseDesc    Tests that a zero length buffer does not cause an indefinate loop during verify 

@SYMTestPriority    Normal

@SYMTestStatus      Implemented

@SYMTestActions    	Creates a buffer, puts it into a TWsGraphicMsgBufParser and calls verify on it
					then removes the message from the buffer and re-tests verify.

@SYMTestExpectedResults Test should pass and not timeout
*/

void CTGraphicsDrawer::IndefiniteLoopL()
	{
	RWsGraphicMsgBuf msgBuffer;
	msgBuffer.CleanupClosePushL();
	msgBuffer.Append(TUid::Uid(0x12345670),KNullDesC8);
	TWsGraphicMsgBufParser msgParser(msgBuffer.Pckg());
	
	// test the verify method with a single message in the buffer
	if(msgParser.Verify() != KErrNone)
		{
		INFO_PRINTF1(_L("TWsGraphicMsgBufParser::Verify() failed for buffer length 1"));
		TEST(EFalse);
		}
		
	// remove the (only) message so the message buffer now has a length of zero
	msgBuffer.Remove(0);
	TEST(msgBuffer.Count()==0);
	
	// test the verify method with an zero length buffer
	if(msgParser.Verify() != KErrNone)
		{
		INFO_PRINTF1(_L("TWsGraphicMsgBufParser::Verify() failed for zero length buffer"));
		TEST(EFalse);
		}
	
	CleanupStack::PopAndDestroy(&msgBuffer);
	}

//--------------
__CONSTRUCT_STEP__(GraphicsDrawer)



