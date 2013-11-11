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
// TE_ETELRECORDER.CPP
// 
//

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "testdef.h"
#include <e32debug.h>
#include <s32file.h>
#include <f32file.h>

#include <etelext.h>

#include "TE_MMEtelRecorder.h"


CTestRecorder::CTestRecorder()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestRecorder"));
	}

/** Force an invalid message with NULL descriptors into etel.
*/
enum TVerdict CTestRecorder::doTestStepL()
	{
	// Test is TE_ETELMM because I could create a test on the server side, because the 
	// server requires correct initialisation of the framework, and then cordination of the
	// finite state machine before it would be safe to log.
	//
	// The alternative was to put the test in TE_ETel but it proved hard to find a SendRecive Function 
	// that would accepted NULL pointers, and actually get to the logging function that we wanted to test.
	// In the end RMobilePhone (part of MMEtel) and EMobilePhoneGetPhoneStoreInfo provided a solution.
	// 
	
	INFO_PRINTF1(_L("Test Recorder"));

	// Lets get access to protected fn's.
	class RMobilePhone1:public RMobilePhone
		{	
			public: 
				void SendReceive1(TInt aFunction, const TIpcArgs &aArgs, TRequestStatus &aStatus) const
					{
						SendReceive(aFunction, aArgs, aStatus);					
					}
		};
		
	RMobilePhone1 mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);
	
	TIpcArgs args;

	args.Set(0,0);
	args.Set(2,0);
	args.Set(1,EIsaNarrowDesToSetAndGet);
		// Does not really matter since, we pass 0 ptr.  
		// But should be something that makes sense.
		// Otherwise the server will reject it before we attempt to
		// output the NULL descriptor.
		// Note. 
		// It is a deliberate choice to pass the descriptor to the server this way.
		// This is just in case there are other issues with NULL pts.
	
	TRequestStatus reqStatusSendRec;
	mmPhone.SendReceive1(EMobilePhoneGetPhoneStoreInfo, args, reqStatusSendRec);
		// EMobilePhoneGetPhoneStoreInfo is tested in TE_mmphone.cpp (CTestPhone::TestCapabilityL),
		// if you need to compare behaviour.
	User::WaitForRequest(reqStatusSendRec);
		// Let us fake a simple ptr falure.
		// Note this will panic the client for both the original run, 
		// and for the player, but we should still get a log record output.
		// The player should, get the record, and note the null records.
		// The null records will then cause a panic, when passed into the etel system.
	

	return TestStepResult();
	}

