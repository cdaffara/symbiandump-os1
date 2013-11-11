// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <hal.h>
#include <u32hal.h>

#include <graphics/surfaceupdateclient.h>
#include <graphics/suerror.h>
#include <limits.h>
#include "surfaceupdatetest.h"
#include "tsurfaceupdate.h"
#include "surfaceupdate.h"


CTSurfaceUpdate::CTSurfaceUpdate(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	}

CTSurfaceUpdate::~CTSurfaceUpdate()
	{
	while (iReceivers.Count())
		{
		CTContentUpdateReceiver *receiver = iReceivers[0];
		iReceivers.Remove(0);
		CloseTestUpdateReceiver(receiver);
		}
	iReceivers.Reset();	
	}

void CTSurfaceUpdate::ConstructL()
	{
	SetupContentUpdateReceiversL();
	}

void CTSurfaceUpdate::SetupContentUpdateReceiversL()
	{
	//first receiver
	CTContentUpdateReceiver *receiver;
	User::LeaveIfError(StartTestUpdateReceiver(receiver, 1));
	iReceivers.AppendL(receiver);
	
	//second receiver
	CTContentUpdateReceiver *receiver2;
	User::LeaveIfError(StartTestUpdateReceiver(receiver2, 2));
	iReceivers.AppendL(receiver2);
	
	//third receiver
	CTContentUpdateReceiver *receiver3;
	User::LeaveIfError(StartTestUpdateReceiver(receiver3, 3));
	iReceivers.AppendL(receiver3);
	
	User::LeaveIfError(Provider()->Register(0, receiver, -1));
	User::LeaveIfError(Provider()->Register(1, receiver2, 1));
	User::LeaveIfError(Provider()->Register(2, receiver3, 2));
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0001


  @SYMPREQ 1007

  @SYMREQ 8223

  @SYMTestCaseDesc Initialization of the server environment twice.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Attempt to start the server for the second time.

  @SYMTestExpectedResults Must return KErrAlreadyExists.
*/
void CTSurfaceUpdate::TestCase1()
	{
	INFO_PRINTF1(_L("Attempt to start the server for the second time"));
	MSurfaceUpdateServerProvider *surfaceUpdateProvider = NULL;
	TInt res = StartSurfaceUpdateServer(surfaceUpdateProvider);
	TEST(res==KErrNone);
	TEST(surfaceUpdateProvider!=NULL);
	TEST(surfaceUpdateProvider==Provider());
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0002

  @SYMPREQ 1007

  @SYMREQ 8223

  @SYMTestCaseDesc Submit update commands and subscribe for notifications. 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create connection to the server. 
  		Subscribe for different notifications (consumed, displayed, displayedXTimes). 
  		Submit update command. Wait for notifications to arrive. 
  		Repeat for a few times in a row .

  @SYMTestExpectedResults Errors, which will be conveyed in 
  TRequestStatus must be equal to KErrNone. The order must be consistent with 
  before and after submission (Before submission < composition < after submission). 
*/
void CTSurfaceUpdate::TestCase2L()
	{
	INFO_PRINTF1(_L("Submit update and subscribe for notifications"));
	const TInt KMaxSubmissionNumber = 60; //it should be divisible by 3!
	RSurfaceUpdateSession session;
	TInt res = session.Connect();
	User::LeaveIfError(res);
	
	iSurfaceOrder = CRequestOrder::NewL(KMaxSubmissionNumber);
	    
	iReceivers[0]->SetCompositionOrder(iSurfaceOrder);
	iReceivers[1]->SetCompositionOrder(iSurfaceOrder);
	iReceivers[2]->SetCompositionOrder(iSurfaceOrder);
	
	TInt screen = 0;
	TInt buffer = 2;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	TRect rc[2] =
			{
			TRect(1,2, 10, 20),
			TRect(5,6, 30, 30)
			};
	RRegion region(2, rc);

	// Test that submitting for an update before any noitfications causes no problems
	TEST(KErrNone == session.SubmitUpdate(screen, surface, buffer, &region));

	iSurfaceOrder->ResetAll();
	for(TInt submissionNumber = 0; submissionNumber < KMaxSubmissionNumber/3; submissionNumber++)
		{
		INFO_PRINTF2(_L("Submission number %d"), submissionNumber+1);	

		TRequestStatus status = KRequestPending;
		TRequestStatus status1 = KRequestPending;
		TRequestStatus status2 = KRequestPending;
		TTimeStamp timeStamp; 
	
		iSurfaceOrder->SetOrder(EOrderBefore);
		session.NotifyWhenAvailable(status);	
		session.NotifyWhenDisplayed(status1, timeStamp);	
		session.NotifyWhenDisplayedXTimes(10, status2);
		
		res = session.SubmitUpdate(screen, surface, buffer, &region);
		TEST(res==KErrNone);
		
		User::WaitForRequest(status);
		User::WaitForRequest(status1);
		User::WaitForRequest(status2);
		iSurfaceOrder->SetOrder(EOrderAfter);

		TEST(status == KErrNone);
		TEST(status1 == KErrNone);
		TEST(status2 == KErrNone);
		}
	
    for(TInt i=0; i < KMaxSubmissionNumber/3; i++)
        {
        TInt positionBefore=-1;
        TInt positionComposition=-1;
        TInt positionAfter=-1;
        for(TInt j=0; j < KMaxSubmissionNumber; j++)
            {
            if((iSurfaceOrder->GetOrderNumber(j) == i) && (iSurfaceOrder->GetOrderType(j) == EOrderBefore))
                {
                positionBefore=j;
                }
            
            if((iSurfaceOrder->GetOrderNumber(j) == i) && (iSurfaceOrder->GetOrderType(j) == EOrderComposition))
                {
                positionComposition=j;
                }
                
            if((iSurfaceOrder->GetOrderNumber(j) == i) && (iSurfaceOrder->GetOrderType(j) == EOrderAfter))
                {
                positionAfter=j;
                }
            }
        TEST((positionBefore != -1) && (positionComposition != -1) && (positionAfter != -1));
        TEST(positionBefore < positionComposition);
        TEST(positionAfter > positionComposition);
        }
	
	delete iSurfaceOrder;
	iSurfaceOrder = NULL;
	iReceivers[0]->SetCompositionOrder(NULL);
	iReceivers[1]->SetCompositionOrder(NULL);
	iReceivers[2]->SetCompositionOrder(NULL);
	
	session.Close();
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0003

  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Submit a few requests, then consume them. 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions A. Request notifications of the same type consecutively 
  	(no submit in-between) and check that the behaviour is correct.
    B. Create connection to the server with sufficient number of 
  	outstanding asynchronous message slots. 
  	1. Subscribe for different notifications (consumed, displayed, 
  	displayedXTimes). 
  	2. Submit update command. 
  	3 Repeat step 1 & 2 a few times. 
  	Wait for notifications (from all requests) to arrive. 
  	Repeat 1& 2 & 3 a few times in a row

  @SYMTestExpectedResults A. The last of each notification request type 
  to be issued before the submission will return with KErrNone, as will 
  the SubmitUpdate. B. Errors, which will be conveyed in TRequestStatus 
  must be equal to KErrNone. The order must be consistent with 
  before and after submission (Before submission < composition < after submission).  
*/
void CTSurfaceUpdate::TestCase3L()
	{
	INFO_PRINTF1(_L("Submission a few update requests in  a row, then waiting for notifications to arrive"));
	RSurfaceUpdateSession session;
	
	const TInt KNumberAsynchronousRequests = 42; //it should be divisible by 3!
	TInt res = session.Connect(KNumberAsynchronousRequests);
	INFO_PRINTF2(_L("Set number of outstanding asynchronous requests as: %d"), KNumberAsynchronousRequests);	
	User::LeaveIfError(res);
	
	iSurfaceOrder = CRequestOrder::NewL(KNumberAsynchronousRequests);
	
	iReceivers[0]->SetCompositionOrder(iSurfaceOrder);
	iReceivers[1]->SetCompositionOrder(iSurfaceOrder);
	iReceivers[2]->SetCompositionOrder(iSurfaceOrder);
	
	TInt screen = 0;
	TInt screen1 = 1;
	TInt buffer = 2;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	
	TRect rc[] =
			{
			TRect(1,2, 3, 4),
			TRect(5,6, 7, 8),
			TRect(9, 10, 11, 12),
			TRect(13, 14, 15, 16),
			};
	RRegion region(4, rc);
	
	// Test calling two notifies in a row followed by a submit
	TRequestStatus stat[6];
	session.NotifyWhenAvailable(stat[0]);
	session.NotifyWhenAvailable(stat[1]);
	TTimeStamp tStamp[2];
	session.NotifyWhenDisplayed(stat[2], tStamp[0]);
	session.NotifyWhenDisplayed(stat[3], tStamp[1]);
	session.NotifyWhenDisplayedXTimes(10, stat[4]);
	session.NotifyWhenDisplayedXTimes(10, stat[5]);	
	TEST(KErrNone == session.SubmitUpdate(0 , surface, buffer, &region));
	// The last requests for notification will be fulfilled, the others ignored
	User::WaitForRequest(stat[1]);
	User::WaitForRequest(stat[3]);
	User::WaitForRequest(stat[5]);	
	TEST(stat[1] == KErrNone);
	TEST(stat[3] == KErrNone);
	TEST(stat[5] == KErrNone);
	
	// Submit a few requests, then consume them
	for(TInt index1 = 0; index1 < 5; index1++)
		{
		INFO_PRINTF2(_L("Attempt number %d"), index1+1);	

		TRequestStatus status[KNumberAsynchronousRequests];
		TTimeStamp timeStamp[KNumberAsynchronousRequests];
		
		// Reset all values
		iSurfaceOrder->ResetAll();
		
		for(TInt index2 = 0; index2 < KNumberAsynchronousRequests; index2 += 3)
			{
			INFO_PRINTF2(_L("Submission number %d"), index2 / 3 + 1);	
			status[index2]=KRequestPending;
			session.NotifyWhenAvailable(status[index2]);	
			status[index2 + 1]=KRequestPending;
			session.NotifyWhenDisplayed(status[index2 + 1], timeStamp[index2 / 3]);	
			status[index2 + 2]=KRequestPending;
			session.NotifyWhenDisplayedXTimes(10, status[index2 + 2]);
			
			iSurfaceOrder->SetOrder(EOrderBefore);
			
			res = session.SubmitUpdate((index2 % 2) ? screen : screen1, surface, buffer, &region);
			TEST(res==KErrNone);
			}
		
		for(TInt submissionNumber = 0; submissionNumber < KNumberAsynchronousRequests; submissionNumber++)
			{
			User::WaitForRequest(status[submissionNumber]);
			TEST(status[submissionNumber] == KErrNone);
			
			if(((submissionNumber - 1) % 3) == 0)
				{
				iSurfaceOrder->SetOrder(EOrderAfter);
				}
			}
		
		for(TInt i=0; i < KNumberAsynchronousRequests/3; i++)
			{
			TInt positionBefore=-1;
			TInt positionComposition=-1;
			TInt positionAfter=-1;
			for(TInt j=0; j < KNumberAsynchronousRequests; j++)
				{
				if((iSurfaceOrder->GetOrderNumber(j) == i) && (iSurfaceOrder->GetOrderType(j) == EOrderBefore))
					{
					positionBefore=j;
					}
				
				if((iSurfaceOrder->GetOrderNumber(j) == i) && (iSurfaceOrder->GetOrderType(j) == EOrderComposition))
					{
					positionComposition=j;
					}
					
				if((iSurfaceOrder->GetOrderNumber(j) == i) && (iSurfaceOrder->GetOrderType(j) == EOrderAfter))
					{
					positionAfter=j;
					}
				}
			TEST((positionBefore != -1) && (positionComposition != -1) && (positionAfter != -1));
			TEST(positionBefore < positionComposition);
			TEST(positionAfter > positionComposition);
			}
		}
		
	delete iSurfaceOrder;
	iSurfaceOrder = NULL;
	iReceivers[0]->SetCompositionOrder(NULL);
	iReceivers[1]->SetCompositionOrder(NULL);
	iReceivers[2]->SetCompositionOrder(NULL);
	
	session.Close();
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0004

  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Cancellation of the requests. 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create connection to the server.
  	1. Test cancellation without SubmitUpdate
    2. Check cancellation before submission does nothing bad.
  	3. Subscribe for different notifications (consumed, displayed, 
  	displayedXTimes). 
  	4. Submit update command. 5. Repeat steps 3 & 4 a few times. 
  	Cancel all outstanding notifications. Wait for cancelled notifications 
  	to arrive.

  @SYMTestExpectedResults Notifications must arrive straightway with 
  KErrCancel error code or KErrNone, if events have already occurred.
*/
void CTSurfaceUpdate::TestCase4L()
	{
	INFO_PRINTF1(_L("Submission a few requests in a row and then cancelling them"));
	RSurfaceUpdateSession session;
	
	const TInt KNumberAsynchronousRequests = 99; //should be divisible by 3!
	TInt res = session.Connect(KNumberAsynchronousRequests);
	INFO_PRINTF1(_L("Test CancelAllUpdateNotifications without SubmitUpdate"));
	// Test CancelAllUpdateNotifications without SubmitUpdate
	TRequestStatus status1, status2, status3;
	TTimeStamp ts;
	session.NotifyWhenAvailable(status1);
	session.NotifyWhenDisplayed(status2, ts);
	session.NotifyWhenDisplayedXTimes(5, status3);
	session.CancelAllUpdateNotifications();
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	TEST(status1==KErrCancel);
	TEST(status2==KErrCancel);
	TEST(status3==KErrCancel);
	
	INFO_PRINTF2(_L("Set number of outstanding asynchronous requests as: %d"), KNumberAsynchronousRequests);	
	User::LeaveIfError(res);
	
	TInt screen = 0;
	TInt screen1 = 1;
	TInt buffer = 2;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	TRect rc[] =
			{
			TRect(1,2, 3, 4),
			TRect(5,6, 7, 8),
			TRect(9, 10, 11, 12),
			TRect(13, 14, 15, 16),
			};
	RRegion region(4, rc);
	
	// Check that cancelling notifications before any submission does nothing bad
	session.CancelAllUpdateNotifications();
	
	for(TInt index1 = 0; index1 < 2; index1++)
		{
		INFO_PRINTF2(_L("Attempt number %d"), index1+1);	

		TRequestStatus status[KNumberAsynchronousRequests];
		
		TTimeStamp timeStamp[KNumberAsynchronousRequests];
		TUint64 timestampComposition[KNumberAsynchronousRequests];
		TUint64 timestampBefore[KNumberAsynchronousRequests];
		TUint64 timestampAfter[KNumberAsynchronousRequests];
		for(TInt index2 = 0; index2 < KNumberAsynchronousRequests; index2 += 3)
			{
			INFO_PRINTF2(_L("Submission number %d"), index2 / 3 + 1);	
			status[index2]=KRequestPending;
			session.NotifyWhenAvailable(status[index2]);	
			status[index2 + 1]=KRequestPending;
			session.NotifyWhenDisplayed(status[index2 + 1], timeStamp[index2 / 3]);	
			status[index2 + 2]=KRequestPending;
			session.NotifyWhenDisplayedXTimes(10, status[index2 + 2]);	
			
			timestampBefore[index2 / 3] = User::FastCounter();	
			res = session.SubmitUpdate((index2 % 2) ? screen : screen1, surface, buffer, &region);
			TEST(res==KErrNone);
			}
		TBool cancel=EFalse;
		session.CancelAllUpdateNotifications();
		for(TInt submissionNumber = 0; submissionNumber < KNumberAsynchronousRequests; submissionNumber++)
			{
			User::WaitForRequest(status[submissionNumber]);
			TEST((status[submissionNumber] == KErrCancel) || (status[submissionNumber] == KErrNone) );
			if(status[submissionNumber] == KErrCancel)
				{
				cancel = ETrue;
				}
			
			if((status[submissionNumber] == KErrNone) && ((submissionNumber - 1) % 3) == 0)
				{
				TInt index22 = submissionNumber / 3;
				timestampAfter[index22] = User::FastCounter();	
		
				if(timestampAfter[index22] < timestampBefore[index22])
					{
					timestampAfter[index22] += UINT_MAX;
					}
				timestampComposition[index22] = timeStamp[index22]();
				if(timestampComposition[index22] < timestampBefore[index22])
					{
					timestampComposition[index22] += UINT_MAX;
					}

				TEST(timestampComposition[index22] != 0);
				TEST(timestampAfter[index22] >= timestampComposition[index22]);
				TEST(timestampBefore[index22] <= timestampComposition[index22]);
				}
			}
		TEST(cancel);		
		}
		session.Close();
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0005

  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Submission from multiple sessions. 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create a few connections to the server. For each connection: 
  		1. Subscribe for different notifications (consumed, displayed, 
  		displayedXTimes). 
  		2. Submit update command. Wait for notifications to arrive.

  @SYMTestExpectedResults Errors, which will be conveyed in 
  TRequestStatus must be equal to KErrNone. Timestamps must be consistent 
  with time before and after submissions.
*/
void CTSurfaceUpdate::TestCase5L()
	{
	INFO_PRINTF1(_L("Submission a few update requests in  a row from different sessions, then waiting for notifications to arrive"));
	
	const TInt KNumberAsynchronousRequests = 9;
	const TInt KNumberSessions = 5;
	TInt res = KErrNone;	
	
	RSurfaceUpdateSession sessions[KNumberSessions];
	INFO_PRINTF2(_L("Set number of outstanding asynchronous requests per each session as: %d"), KNumberAsynchronousRequests);	
	INFO_PRINTF2(_L("Set number of sessions: %d"), KNumberSessions);	
	
	TInt screen = 0;
	TInt screen1 = 1;
	TInt buffer = 2;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	TRect rc[] =
			{
			TRect(1,2, 3, 4),
			TRect(5,6, 7, 8),
			TRect(9, 10, 11, 12),
			TRect(13, 14, 15, 16),
			};
	RRegion region(4, rc);

	TRequestStatus status[KNumberAsynchronousRequests * KNumberSessions];
	
	for(TInt index2 = 0; index2 < KNumberSessions; index2++)
		{
		res = sessions[index2].Connect(KNumberAsynchronousRequests);
		User::LeaveIfError(res);
		}
	
	for(TInt index1 = 0; index1 < 3; index1++)
		{
		INFO_PRINTF2(_L("Attempt number %d"), index1+1);	

		for(TInt index2 = 0; index2 < KNumberSessions; index2++)
			{
			TTimeStamp timeStamp;
			INFO_PRINTF2(_L("Session number %d"), index2 + 1);	
		 
			for(TInt index3 = index2 * KNumberAsynchronousRequests; 
					index3 < (index2 + 1) * KNumberAsynchronousRequests; index3 += 3)
				{
				INFO_PRINTF2(_L("Submission number %d"), index3 / 3 + 1);	
				status[index3]=KRequestPending;
				sessions[index2].NotifyWhenAvailable(status[index3]);	
				status[index3 + 1]=KRequestPending;
				sessions[index2].NotifyWhenDisplayed(status[index3 + 1], timeStamp);	
				status[index3 + 2]=KRequestPending;
				sessions[index2].NotifyWhenDisplayedXTimes(10, status[index3 + 2]);	
				res = sessions[index2].SubmitUpdate((index3 % 2) ? screen : screen1, surface, buffer, &region);
				if(res != KErrNone)
				    {
	                ERR_PRINTF2(_L("Returned SubmitUpdate code %d"), res);
	                TEST(EFail);
				    }
				}
			}

		for(TInt submissionNumber = 0; submissionNumber < KNumberAsynchronousRequests * KNumberSessions; submissionNumber++)
			{
			User::WaitForRequest(status[submissionNumber]);
			if(status[submissionNumber] != KErrNone)
			    {
			    ERR_PRINTF2(_L("Returned request status %d"), status[submissionNumber].Int());
			    TEST(EFail);
			    }
			}
		}
		
	for(TInt index5 = 0; index5 < KNumberSessions; index5++)
		{
		sessions[index5].Close();
		}
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0023


  @SYMPREQ 1007/11596

  @SYMREQ 8223

  @SYMTestCaseDesc SubmitUpdate() to cope with Global Updates 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 
  		1.	Submit for a global update before subscribing for any notifications.
		From this point onwards, all further cases assume that we’ve subscribed 
		for different notifications (available, displayed, displayedXTimes) 
		before submitting a global update:
		2.	Submit a global update for a surface that is not visible on any screen.
		3.	Submit a global update for a surface that is only visible on one screen.
		4.	Submit a global update for a surface that is visible on different screens.

  @SYMTestExpectedResults 
  	1.	Submitting for an update before any notifications causes no problems.
	2.	SubmitUpdate() signals KErrNotVisible
	3.	TRequestStatus signals KErrNone in all cases. Timestamps must be valid.
	4.	TRequestStatus signals KErrNone in all cases. Timestamps must be valid.
*/
void CTSurfaceUpdate::TestCase15()
	{
	INFO_PRINTF1(_L("SubmitUpdate() to cope with Global Updates"));

	RSurfaceUpdateSession session;
	TInt res = session.Connect();
	TEST(res == KErrNone);
	
	TInt buffer = 2;
	TSurfaceId surface = 
		{
		1,2,3,4
		};

	// Test that submitting for an update before any noitfications causes no problems
	TInt numReceivers = iReceivers.Count();

	//make all screens invisible
	INFO_PRINTF1(_L("make all screens invisible"));	
	for(TInt index = 0; index < numReceivers; index++)
		{
		CTContentUpdateReceiver* receiver = iReceivers[index];
		receiver->SetVisible(EFalse);
		}
	
	for(TInt submissionNumber = 0; submissionNumber < 5; submissionNumber++)
		{
		INFO_PRINTF2(_L("Submission number %d"), submissionNumber+1);	

		TRequestStatus status = KRequestPending;
		TRequestStatus status1 = KRequestPending;
		TRequestStatus status2 = KRequestPending;
		TTimeStamp timeStamp; 
	
		session.NotifyWhenAvailable(status);	
		session.NotifyWhenDisplayed(status1, timeStamp);	
		session.NotifyWhenDisplayedXTimes(10, status2);
		
		res = session.SubmitUpdate(KAllScreens, surface, buffer, NULL);
		TEST(res==KErrNone);
		
		User::WaitForRequest(status);
		User::WaitForRequest(status1);
		User::WaitForRequest(status2);

		TEST(status == KErrNotVisible);
		TEST(status1 == KErrNotVisible);
		TEST(status2 == KErrNotVisible);
		}
	
	//make only one screen visible
	CTContentUpdateReceiver* receiver = iReceivers[0];
	receiver->SetVisible(ETrue);
	INFO_PRINTF1(_L("Make visible one screen"));	

	
	for(TInt submissionNumber = 0; submissionNumber < 5; submissionNumber++)
		{
		INFO_PRINTF2(_L("Submission number %d"), submissionNumber+1);	

		TRequestStatus status = KRequestPending;
		TRequestStatus status1 = KRequestPending;
		TRequestStatus status2 = KRequestPending;
		TUint64 timestampComposition = 0;
		TTimeStamp timeStamp; 
	
		TUint64 timestampBefore = User::FastCounter();

		session.NotifyWhenAvailable(status);	
		session.NotifyWhenDisplayed(status1, timeStamp);	
		session.NotifyWhenDisplayedXTimes(10, status2);
		
		res = session.SubmitUpdate(KAllScreens, surface, buffer, NULL);
		TEST(res==KErrNone);
		
		User::WaitForRequest(status);
		User::WaitForRequest(status1);
		User::WaitForRequest(status2);

		TUint64 timestampAfter = User::FastCounter();
		if(timestampAfter < timestampBefore)
			{
			timestampAfter += UINT_MAX;
			}
		timestampComposition = timeStamp();
		if(timestampComposition < timestampBefore)
			{
			timestampComposition += UINT_MAX;
			}
	
		TEST(status == KErrNone);
		TEST(status1 == KErrNone);
		TEST(status2 == KErrNone);
		
		TEST(timestampComposition != 0);
		TEST(timestampAfter >= timestampComposition);
		TEST(timestampBefore <= timestampComposition);
		
		INFO_PRINTF2(_L("Time before composition: %lu"), timestampBefore);	
		INFO_PRINTF2(_L("Time at composition: %lu"), timestampComposition);	
		INFO_PRINTF2(_L("Time after composition: %lu"), timestampAfter);
		}
	
	//make another screen visible
	INFO_PRINTF1(_L("make another screen visible"));	
	receiver->SetVisible(EFalse);
	receiver = iReceivers[1];
	receiver->SetVisible(ETrue);
	
	for(TInt submissionNumber = 0; submissionNumber < 5; submissionNumber++)
		{
		INFO_PRINTF2(_L("Submission number %d"), submissionNumber+1);	

		TRequestStatus status = KRequestPending;
		TRequestStatus status1 = KRequestPending;
		TRequestStatus status2 = KRequestPending;
		TUint64 timestampComposition = 0;
		TTimeStamp timeStamp; 
	
		TUint64 timestampBefore = User::FastCounter();

		session.NotifyWhenAvailable(status);	
		session.NotifyWhenDisplayed(status1, timeStamp);	
		session.NotifyWhenDisplayedXTimes(10, status2);
		
		res = session.SubmitUpdate(KAllScreens, surface, buffer, NULL);
		TEST(res==KErrNone);
		
		User::WaitForRequest(status);
		User::WaitForRequest(status1);
		User::WaitForRequest(status2);

		TUint64 timestampAfter = User::FastCounter();
		if(timestampAfter < timestampBefore)
			{
			timestampAfter += UINT_MAX;
			}
		timestampComposition = timeStamp();
		if(timestampComposition < timestampBefore)
			{
			timestampComposition += UINT_MAX;
			}
	
		TEST(status == KErrNone);
		TEST(status1 == KErrNone);
		TEST(status2 == KErrNone);
		
		TEST(timestampComposition != 0);
		TEST(timestampAfter >= timestampComposition);
		TEST(timestampBefore <= timestampComposition);
		
		INFO_PRINTF2(_L("Time before composition: %lu"), timestampBefore);	
		INFO_PRINTF2(_L("Time at composition: %lu"), timestampComposition);	
		INFO_PRINTF2(_L("Time after composition: %lu"), timestampAfter);
		}
	
	//make all screens visible
	for(TInt index = 0; index < numReceivers; index++)
		{
		CTContentUpdateReceiver* receiver = iReceivers[index];
		receiver->SetVisible(ETrue);
		}

	session.Close();
	}


/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0024

  @SYMPREQ 1007/11596

  @SYMREQ 8223

  @SYMTestCaseDesc CancelAllUpdateNotifications() to deal with global updates 
  correctly. 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 
  		The test consists of a single surface, visible on screens A (master), B & C. 
		The test will submit for different notifications (available, displayed, 
		displayedXTimes) and then submit a global update. 
		These actions will be repeated a number of times.
		Finally, the test will cancel all outstanding notifications via 
		CancelAllUpdateNotifications() and wait for the cancelled notifications to arrive.
  		
  @SYMTestExpectedResults 
  	Notifications to arrive straight away with KErrCancel, otherwise with 
  	KErrNone if the events have already occurred.
*/
void CTSurfaceUpdate::TestCase16L()
	{
	INFO_PRINTF1(_L("CancelAllUpdateNotifications() to deal with global updates correctly"));
	RSurfaceUpdateSession session;
	
	const TInt KNumberAsynchronousRequests = 33; //should be divisible by 3!
	TInt res = session.Connect(KNumberAsynchronousRequests);
	INFO_PRINTF1(_L("Test CancelAllUpdateNotifications without SubmitUpdate"));
	// Test CancelAllUpdateNotifications without SubmitUpdate
	TRequestStatus status1, status2, status3;
	TTimeStamp ts;
	
	INFO_PRINTF2(_L("Set number of outstanding asynchronous requests as: %d"), KNumberAsynchronousRequests);	
	User::LeaveIfError(res);
	
	TInt buffer = 2;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	
	for(TInt index1 = 0; index1 < 2; index1++)
		{
		INFO_PRINTF2(_L("Attempt number %d"), index1+1);	

		TRequestStatus status[KNumberAsynchronousRequests];
		
		TTimeStamp timeStamp[KNumberAsynchronousRequests];
		TUint64 timestampComposition[KNumberAsynchronousRequests];
		TUint64 timestampBefore[KNumberAsynchronousRequests];
		TUint64 timestampAfter[KNumberAsynchronousRequests];
		for(TInt index2 = 0; index2 < KNumberAsynchronousRequests; index2 += 3)
			{
			INFO_PRINTF2(_L("Submission number %d"), index2 / 3 + 1);	
			status[index2]=KRequestPending;
			session.NotifyWhenAvailable(status[index2]);	
			status[index2 + 1]=KRequestPending;
			session.NotifyWhenDisplayed(status[index2 + 1], timeStamp[index2 / 3]);	
			status[index2 + 2]=KRequestPending;
			session.NotifyWhenDisplayedXTimes(10, status[index2 + 2]);	
			
			timestampBefore[index2 / 3] = User::FastCounter();	
			res = session.SubmitUpdate(KAllScreens, surface, buffer);
			TEST(res==KErrNone);
			}
		TBool cancel=EFalse;
		session.CancelAllUpdateNotifications();
		for(TInt submissionNumber = 0; submissionNumber < KNumberAsynchronousRequests; submissionNumber++)
			{
			User::WaitForRequest(status[submissionNumber]);
			TEST((status[submissionNumber] == KErrCancel) || (status[submissionNumber] == KErrNone) );
			if(status[submissionNumber] == KErrCancel)
				{
				cancel = ETrue;
				}
			
			if((status[submissionNumber] == KErrNone) && ((submissionNumber - 1) % 3) == 0)
				{
				TInt index22 = submissionNumber / 3;
				timestampAfter[index22] = User::FastCounter();	
		
				if(timestampAfter[index22] < timestampBefore[index22])
					{
					timestampAfter[index22] += UINT_MAX;
					}
				timestampComposition[index22] = timeStamp[index22]();
				if(timestampComposition[index22] < timestampBefore[index22])
					{
					timestampComposition[index22] += UINT_MAX;
					}

				TEST(timestampComposition[index22] != 0);
				TEST(timestampAfter[index22] >= timestampComposition[index22]);
				TEST(timestampBefore[index22] <= timestampComposition[index22]);
				}
			}
		TEST(cancel);		
		}
		session.Close();
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0027

  @SYMPREQ 1007/11598

  @SYMREQ 8223

  @SYMTestCaseDesc NotifyWhenAvailable() to deal with global updates and be signalled correctly
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 
	The test consists of a single surface, visible on screens B (master) and C, 
	but not visible on A. 
	1. Screens are ordered in the following priority : A > B > C
	2. Set priority of the receiver threads as follows A < B < C.
	3. Set receiver A invisible.
	4. Make global submit update with notify when displayed and notify 
	when displayed X times.
	5. Repeat step 4 a few times.
  		
  @SYMTestExpectedResults 
  	Notification to arrive with error code KErrNone initiated by the 
  	master display.
*/
void CTSurfaceUpdate::TestCaseNotifyWhenAvailableGlobal()
	{
	INFO_PRINTF1(_L("NotifyWhenAvailable() to deal with global updates and be signalled correctly"));

	RSurfaceUpdateSession session;
	TInt res = session.Connect();
	TEST(res == KErrNone);
	
	TInt buffer = 2;
	TSurfaceId surface = 
		{
		1,2,3,4
		};

	TEST(iReceivers[2]->OutstandingRequest()==EFalse);
	TEST(iReceivers[1]->OutstandingRequest()==EFalse);
	TEST(iReceivers[0]->OutstandingRequest()==EFalse);
	
	//change the order.
	//higher screen number means that the screen is higher priority. 
	//So Receiver0 is lowest screen priority
	res = iReceivers[2]->SetInternalPriority(EPriorityLess);
	TEST(res == KErrNone);
	res = iReceivers[1]->SetInternalPriority(EPriorityNormal);
	TEST(res == KErrNone);
	res = iReceivers[0]->SetInternalPriority(EPriorityAbsoluteHighNormal);
	TEST(res == KErrNone);
	iReceivers[2]->SetVisible(EFalse);
	User::After(TTimeIntervalMicroSeconds32(100000)); //make sure that priority takes place
	
	for(TInt submissionNumber = 0; submissionNumber < 20; submissionNumber++)
		{
		INFO_PRINTF2(_L("Submission number %d"), submissionNumber+1);	

		TRequestStatus status = KRequestPending;
	
		session.NotifyWhenAvailable(status);	
		
		res = session.SubmitUpdate(KAllScreens, surface, buffer, NULL);
		TEST(res==KErrNone);
		
		User::WaitForRequest(status);

		TEST(status == KErrNone);
		TEST(iReceivers[2]->OutstandingRequest()==EFalse);
		TEST(iReceivers[1]->OutstandingRequest()==EFalse);
		TEST(iReceivers[0]->OutstandingRequest()==EFalse);
		}

	//restore envorienment
	iReceivers[0]->SetInternalPriority(EPriorityNormal);
	iReceivers[1]->SetInternalPriority(EPriorityNormal);
	iReceivers[2]->SetInternalPriority(EPriorityNormal);
	iReceivers[2]->SetVisible(ETrue);
	User::After(TTimeIntervalMicroSeconds32(100000)); //make sure that priority takes place
	WaitUntilAllNotificationsProcessed();
	session.Close();
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0028

  @SYMPREQ 1007/11599, 1007/11600, 1007/11601

  @SYMREQ 8223

  @SYMTestCaseDesc NotifyWhenDisplayed() & NotifyWhenDisplayedXTimes() to deal with global updates and be signalled correctly
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 
	The test consists of a single surface, visible on screens B (master) and C, but not visible on A. 
	1. Screens are ordered in the following priority : A > B > C.
	2. Set priority of the receiver threads as follows A < B < C.
	3. Set receiver A invisible.
	4. Make global submit update with notify when displayed and notify when displayed X times.
	Repeat step 4 a few times.

	5. Screens are ordered in the following priority : A > B > C.
	6. Set priority of the receiver threads as follows A > B > C.
	7. Set receiver A invisible.
	8. Make global submit update with notify when displayed and notify when displayed X times.
	Repeat step 4 a few times.

  		
  @SYMTestExpectedResults 
  	Notification to arrive with error code KErrNone initiated by the master display.
  	There is no outstanding request for receiver A & B after steps 4 and 8.
*/
void CTSurfaceUpdate::TestCase17()
	{
	INFO_PRINTF1(_L("NotifyWhenDisplayed() && NotifyWhenDisplayedXTimes() to deal with global updates and be signalled correctly"));

	RSurfaceUpdateSession session;
	TInt res = session.Connect();
	TEST(res == KErrNone);
	
	TInt buffer = 2;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	
//change the order, receiver with highest priority has a lowest internal priority 
	res = iReceivers[2]->SetInternalPriority(EPriorityLess);
	TEST(res == KErrNone);
	res = iReceivers[1]->SetInternalPriority(EPriorityNormal);
	TEST(res == KErrNone);
	res = iReceivers[0]->SetInternalPriority(EPriorityAbsoluteHighNormal);
	TEST(res == KErrNone);
	iReceivers[2]->SetVisible(EFalse);
	User::After(TTimeIntervalMicroSeconds32(100000)); //make sure that priority setting takes place
	
	for(TInt submissionNumber = 0; submissionNumber < 10; submissionNumber++)
		{
		INFO_PRINTF2(_L("Submission number %d"), submissionNumber+1);	

		TRequestStatus status = KRequestPending;
		TRequestStatus status1 = KRequestPending;
		TTimeStamp timeStamp; 
	
		session.NotifyWhenDisplayed(status, timeStamp);	
		session.NotifyWhenDisplayedXTimes(10, status1);
		
		res = session.SubmitUpdate(KAllScreens, surface, buffer, NULL);
		TEST(res==KErrNone);
		
		User::WaitForRequest(status);
		User::WaitForRequest(status1);

		TEST(status == KErrNone);
		TEST(status1 == KErrNone);
		TEST(iReceivers[0]->OutstandingRequest() == EFalse);
		TEST(iReceivers[1]->OutstandingRequest() == EFalse);
		TEST(iReceivers[2]->OutstandingRequest() == EFalse);
		}

	//change the order, receiver with highest priority has a highest internal priority 
		res = iReceivers[0]->SetInternalPriority(EPriorityLess);
		TEST(res == KErrNone);
		res = iReceivers[1]->SetInternalPriority(EPriorityNormal);
		TEST(res == KErrNone);
		res = iReceivers[2]->SetInternalPriority(EPriorityAbsoluteHighNormal);
		TEST(res == KErrNone);
		iReceivers[2]->SetVisible(EFalse);
		User::After(TTimeIntervalMicroSeconds32(100000)); //make sure that priority takes place
		
		TInt numUnprocessedSignals = 0;
		for(TInt submissionNumber = 0; submissionNumber < 10; submissionNumber++)
			{
			INFO_PRINTF2(_L("Submission number %d"), submissionNumber+1);	

			TRequestStatus status = KRequestPending;
			TRequestStatus status1 = KRequestPending;
			TTimeStamp timeStamp; 
		
			session.NotifyWhenDisplayed(status, timeStamp);	
			session.NotifyWhenDisplayedXTimes(10, status1);
			
			res = session.SubmitUpdate(KAllScreens, surface, buffer, NULL);
			TEST(res==KErrNone);
			
			User::WaitForRequest(status);
			User::WaitForRequest(status1);

			TEST(status == KErrNone);
			TEST(status1 == KErrNone);
			if(iReceivers[0]->OutstandingRequest())
				{		
				numUnprocessedSignals++;
				INFO_PRINTF1(_L("Unprocessed signals"));	

				}
			TEST(iReceivers[1]->OutstandingRequest() == EFalse);
			TEST(iReceivers[2]->OutstandingRequest() == EFalse);
			TEST(iReceivers[0]->OutstandingRequest()); //receiver is signalled last, in since its priority is lowest, 
														//SUS shouldn’t wait for it before the signal goes back to client  
			}

	INFO_PRINTF2(_L("Number unprocessed signals %d"), numUnprocessedSignals);	
	TEST(numUnprocessedSignals > 0);
		
	//restore envorienment
	iReceivers[0]->SetInternalPriority(EPriorityNormal);
	iReceivers[1]->SetInternalPriority(EPriorityNormal);
	iReceivers[2]->SetInternalPriority(EPriorityNormal);
	iReceivers[2]->SetVisible(ETrue);
	User::After(TTimeIntervalMicroSeconds32(100000)); //make sure that priority takes place
	WaitUntilAllNotificationsProcessed();
	
	session.Close();
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0029


  @SYMPREQ 1007/11596

  @SYMREQ 8223

  @SYMTestCaseDesc Register with identical priorities. 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 
		1.	Start the server.
		2.	Create two content update receivers
		3.	Register receiverA using Register() with a priority
		4.	Register receiverB using Register() with the same priority
		5.	Unregister receiverA
		6.	Register receiverB using Register() with the same priority as before

  @SYMTestExpectedResults 
		Step 4 should fail, returning KErrAlreadyExists.
		Step 6 should succeed.
*/
void CTSurfaceUpdate::TestCaseRegisterIdenticalPrioritiesL()
	{
	INFO_PRINTF1(_L("Register with identical priorities test"));
	const TInt KIdenticalPriority = 100;
	
	// Start two content update receivers for register test
	CTContentUpdateReceiver* receiverA;
	CTContentUpdateReceiver* receiverB;
	User::LeaveIfError(StartTestUpdateReceiver(receiverA, 8));
	iReceivers.AppendL(receiverA);
	User::LeaveIfError(StartTestUpdateReceiver(receiverB, 9));
	iReceivers.AppendL(receiverB);
		
	INFO_PRINTF2(_L("Registering receiver A with priority %i"), KIdenticalPriority);	
	User::LeaveIfError(Provider()->Register(receiverA->Screen(), receiverA, KIdenticalPriority));

	INFO_PRINTF2(_L("Registering receiver B with the same priority (%i), which should return KErrAlreadyExists"), KIdenticalPriority);	
	TInt ret = Provider()->Register(receiverB->Screen(), receiverB, KIdenticalPriority);
	if(KErrAlreadyExists == ret)
		{
		INFO_PRINTF1(_L("KErrAlreadyExists returned as expected"));
		}
	else
		{
		ERR_PRINTF3(_L("Test failed - expected Register() to return KErrAlreadyExists(%i) but instead returned %i"), KErrAlreadyExists, ret);
		User::Leave(ret);
		}

	INFO_PRINTF1(_L("Unregistering receiver A"));
	User::LeaveIfError(Provider()->Register(receiverA->Screen(), NULL, KIdenticalPriority));

	INFO_PRINTF2(_L("Registering receiver B with the same priority (%i), which should succeed"), KIdenticalPriority);
	User::LeaveIfError(Provider()->Register(receiverB->Screen(), receiverB, KIdenticalPriority));
	}

/**
negative testing
*/


/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0006

  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Negative testing - various cases

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 6. The client mustn't try to send any command to the server before 
     establishing a connection.
     7. Attempt to connect twice
     8. Attempt notify then submit with invalid parameters
     9. Submission with screen which doesn't exist in the system
     10. Submission with invalid number of counts in NotifyWhenDisplayedXTimes()
     11. Number of outstanding requests exceeds pre-defined value
     12. Submission of region with empty rectangle list
     13. Assigning receiver to the screen with negative number

  @SYMTestExpectedResults 6. Shouldn't panic and subsequent session connect should succeed.
     7. Second attempt to connect should return KErrAlreadyExists
     8. With invalid parameters we get KErrArgument; with a screen that doesn't exist we
        get KErrUpdateReceiverNotAvailable
     9. Too big a screen number results in KErrUpdateReceiverNotAvailable; -ve buffer results in KErrArgument.
     10. With a negative diplayedXtimes number, we expect KErrArgument
     11. When we exceed the number of allowed outstanding requests, we get KErrBusy
     12. With empty region, all return codes are KErrNone.
     13. Registering with negative screen gives KErrArgument
*/
void CTSurfaceUpdate::TestCase6L()
	{
	INFO_PRINTF1(_L("negative testing"));
	RSurfaceUpdateSession session;
	TRequestStatus status;
	TRequestStatus status1;
	TRequestStatus status2;
	TRequestStatus status3;
	TRequestStatus status4;
	TRequestStatus status5;
	TInt res;
	TTimeStamp timeStamp; 
	
	//@SYMTestCaseID GRAPHICS-SURFACEUPDATE-0006 apply operation on disconnected session
	TInt screen = 0;
	TInt buffer = 2;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	TRect rc[2] =
			{
			TRect(1,2, 10, 20),
		
			TRect(5,6, 30, 30)
			};
		
	RRegion region(2, rc);

	//next command will be ignored, but shouldn't cause a panic
	INFO_PRINTF1(_L("close disconnected session"));
	session.Close();
	
	User::LeaveIfError(session.Connect());
	
	//@SYMTestCaseID GRAPHICS-SURFACEUPDATE-0007 Attempt to connect twice
	INFO_PRINTF1(_L("try to connect twice"));
	res = session.Connect();
	TEST(res == KErrAlreadyExists);
	
	//@SYMTestCaseID GRAPHICS-SURFACEUPDATE-0008 wrong parameters
	INFO_PRINTF1(_L("specifying wrong parametrs"));
	INFO_PRINTF1(_L("specify a negative screen number"));
	screen = -10;
	session.NotifyWhenAvailable(status);	
	session.NotifyWhenDisplayed(status1, timeStamp);	
	session.NotifyWhenDisplayedXTimes(10, status2);	
	res = session.SubmitUpdate(screen, surface, buffer, &region);
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	
	TEST(status == KErrArgument);
	TEST(status1 == KErrArgument);
	TEST(status2 == KErrArgument);
	TEST(res == KErrArgument);

	INFO_PRINTF1(_L("receiver for the following screen doesn't exist"));
	screen = 10;
	session.NotifyWhenAvailable(status);	
	session.NotifyWhenDisplayed(status1, timeStamp);	
	session.NotifyWhenDisplayedXTimes(10, status2);	
	res = session.SubmitUpdate(screen, surface, buffer, &region);
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);

	TEST(status == KErrUpdateReceiverNotAvailable);
	TEST(status1 == KErrUpdateReceiverNotAvailable);
	TEST(status2 == KErrUpdateReceiverNotAvailable);
	TEST(res == KErrUpdateReceiverNotAvailable);

	//@SYMTestCaseID GRAPHICS-SURFACEUPDATE-0009 Submission with screen which doesn't exist in the system
	INFO_PRINTF1(_L("too big screen number"));
	screen = 100000;
	session.NotifyWhenAvailable(status);	
	session.NotifyWhenDisplayed(status1, timeStamp);	
	session.NotifyWhenDisplayedXTimes(10, status2);	
	res = session.SubmitUpdate(screen, surface, buffer, &region);
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);

	TEST(status == KErrUpdateReceiverNotAvailable);
	TEST(status1 == KErrUpdateReceiverNotAvailable);
	TEST(status2 == KErrUpdateReceiverNotAvailable);
	TEST(res == KErrUpdateReceiverNotAvailable);
	
	INFO_PRINTF1(_L("negative buffer"));
	screen = 0;
	buffer = -10;
	session.NotifyWhenAvailable(status);	
	session.NotifyWhenDisplayed(status1, timeStamp);	
	session.NotifyWhenDisplayedXTimes(10, status2);	
	res = session.SubmitUpdate(screen, surface, buffer, &region);
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);

	TEST(status == KErrArgument);
	TEST(status1 == KErrArgument);
	TEST(status2 == KErrArgument);
	TEST(res == KErrArgument);

	//@SYMTestCaseID GRAPHICS-SURFACEUPDATE-0010 Submission with invalid number of counts in NotifyWhenDisplayedXTimes()
	INFO_PRINTF1(_L("displaying for negative number of times"));
	screen = 0;
	buffer = 0;
	TInt displayedXTimes = -10;	
	session.NotifyWhenAvailable(status);	
	session.NotifyWhenDisplayed(status1, timeStamp);	
	session.NotifyWhenDisplayedXTimes(displayedXTimes, status2);	
	res = session.SubmitUpdate(screen, surface, buffer, &region);
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);

	TEST(status == KErrNone);
	TEST(status1 == KErrNone);
	TEST(status2 == KErrArgument);
	TEST(res == KErrNone);
	
	//@SYMTestCaseID GRAPHICS-SURFACEUPDATE-0011 Number of outstanding requests exceeds pre-defined value
	INFO_PRINTF1(_L("number of outstanding requests exceeds pre-defined value"));
	session.Close();
	User::LeaveIfError(session.Connect(4));
	buffer = 0;
	session.NotifyWhenAvailable(status);	
	session.NotifyWhenDisplayed(status1, timeStamp);	
	session.NotifyWhenDisplayedXTimes(10, status2);	
	res = session.SubmitUpdate(screen, surface, buffer, &region);
	TEST(res == KErrNone);

	session.NotifyWhenAvailable(status3);	
	session.NotifyWhenDisplayed(status4, timeStamp);	
	session.NotifyWhenDisplayedXTimes(10, status5);	
	res = session.SubmitUpdate(screen, surface, buffer, &region);
	TEST(res == KErrNone);

	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	User::WaitForRequest(status4);
	User::WaitForRequest(status5);
	
	TEST(status == KErrNone);
	TEST(status1== KErrNone);
	TEST(status2== KErrNone);
	TEST(status3== KErrNone);
	TEST(status4== KErrServerBusy);
	TEST(status5== KErrServerBusy);
	
	//@SYMTestCaseID GRAPHICS-SURFACEUPDATE-0012 Submission of region with empty rectangle list
	INFO_PRINTF1(_L("specify empty region"));
	RRegion region1;
	session.NotifyWhenAvailable(status);	
	session.NotifyWhenDisplayed(status1, timeStamp);	
	session.NotifyWhenDisplayedXTimes(10, status2);	
	res = session.SubmitUpdate(screen, surface, buffer, &region1);
	TEST(res == KErrNone);

	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	TEST(status == KErrNone);
	TEST(status1== KErrNone);
	TEST(status2== KErrNone);
	
	//register with negative screen
	//@SYMTestCaseID GRAPHICS-SURFACEUPDATE-0013 Assigning receiver to the screen with negative number
	INFO_PRINTF1(_L("register with negative screen"));
	res = Provider()->Register(-1, NULL, 1);
	TEST(res==KErrArgument);
	session.Close();
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0014

  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc OOM testing. 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create connection to the server. 
  	Set server in special mode which simulates heap allocation failure. 
  	1. Subscribe for notifications. 
  	2. Submit update command with not empty region.
  	
  @SYMTestExpectedResults Submit return code and notifications must arrive 
  with KErrNoMemory in the case of memory allocation failures.
*/
void CTSurfaceUpdate::TestCase7L()
	{
	RMisbehavingSession session;
	TRequestStatus status;
	TRequestStatus status1;
	TRequestStatus status2;
	TTimeStamp timeStamp;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	TRect rc[2] =
		{
		TRect(1,2, 10, 20),
		TRect(5,6, 30, 30)
		};
	
	RRegion region(2, rc);
	TInt buffer = 0;
	TInt screen = 0;
	User::LeaveIfError(session.Connect());
	TInt res = KErrNone;
	
	//create another receiver
	CTContentUpdateReceiver *receiver4;
	User::LeaveIfError(StartTestUpdateReceiver(receiver4, 4));
	iReceivers.AppendL(receiver4);
	
	TInt ii = 1;
	const TInt screenNo = 16;
	// Cause a failure on all the allocs on server side
	for(ii = 1; ; ++ii)
		{
		session.SetFailRate(ii);
		res = Provider()->Register(screenNo, receiver4, -100);
		if(res != KErrNoMemory)
			break;
		} 
	TEST(res == KErrNone);
	//unregister receiver
	User::LeaveIfError(Provider()->Register(screenNo, NULL, 0));
	//try to do the same same thing again, to make sure that it won't panic
	User::LeaveIfError(Provider()->Register(screenNo, NULL, 0)); //unregister the function
	TInt index = iReceivers.Find(receiver4);
	TEST(index != KErrNotFound);
	CloseTestUpdateReceiver(receiver4);	
	iReceivers.Remove(index);
	//end register testing
	
	// Cause a failure on all the allocs on server side
	for(ii = 1; ; ++ii)
		{
		// Test 1st alloc failure
		session.SetFailRate(ii);
		session.NotifyWhenAvailable(status);
		session.NotifyWhenDisplayed(status1, timeStamp);
		session.NotifyWhenDisplayedXTimes(1, status2);
		res = session.SubmitUpdate(screen, surface, buffer, &region);
		User::WaitForRequest(status);
		User::WaitForRequest(status1);
		User::WaitForRequest(status2);
		
		// Whenever we hit error codes that aren't no memory, we're finished
		if(res != KErrNoMemory || status.Int() != KErrNoMemory || status1.Int() != KErrNoMemory || status2.Int() != KErrNoMemory)
			break;
		
		TEST(res == KErrNoMemory); // Since the first alloc fail kills the server
		TEST(status.Int() == KErrNoMemory);
		}
		
	TEST(res == KErrNone);
	TEST((status.Int() == KErrNone) || (status.Int() == KErrNoMemory));
	TEST((status1.Int() == KErrNone) || (status1.Int() == KErrNoMemory));
	TEST((status2.Int() == KErrNone) || (status2.Int() == KErrNoMemory));
	session.SetFailRate(0); // turns off oom failures
	session.Close();
	}
	
/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0015

  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Causing panic of client 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create second thread which connects to the server. 
  	Pass an invalid opcode to the server which will cause a client panic.
  	
  @SYMTestExpectedResults The thread will die with the correct exit reason
    (EUpdateServPanicBadRequest) and the correct exit category (SurfUpServ).
*/
void CTSurfaceUpdate::TestCase8()
 	{
 	// Create another thread in which we will panic the client
 	_LIT(KThreadName, "TestPanicClientThread");
 	RThread thread;
	TEST(KErrNone == thread.Create(KThreadName,
			PanicClientThread,
			KDefaultStackSize,
			&User::Heap(),
			NULL));
	TRequestStatus rendezvousStatus;
	thread.Rendezvous(rendezvousStatus);
	thread.Resume();
	User::WaitForRequest(rendezvousStatus);
	_LIT(KExitCategory,"SurfUpServ");
	TBuf<16> exitCategory(KExitCategory);
	TEST(thread.ExitReason() == EUpdateServPanicBadRequest);
	TEST(thread.ExitCategory() == exitCategory);
	thread.Close();
 	}
 	
/**
Thread function called from TestCase8L in order to panic the client.
*/ 	
TInt CTSurfaceUpdate::PanicClientThread(TAny*)
	{
	User::SetJustInTime(EFalse);
	RMisbehavingSession session;
	TInt err = session.Connect();
 	if(KErrNone != err)
 		return err;
 	session.TestPassingInvalidOpcode();
 	// Exit reason will be the panic number due to the invalid op code:
 	// returning zero here is redundant
 	return 0;
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0016


  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Calling submit before any notify with invalid arguments. 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Set an invalid screen number and call SubmitUpdate (before a notify).
    Set an invalid buffer number and call SubmitUpdate (before a notify).
  	Set a Null surfaceId and call SubmitUpdate (before a notify).
  	
  @SYMTestExpectedResults SubmitUpdate will return KErrArgument in both cases.
*/
void CTSurfaceUpdate::TestCase9L()
	{
	RSurfaceUpdateSession session;	
	User::LeaveIfError(session.Connect());
	
	// Test submit before notify with -ve screen
	TInt screen = -1;
	TSurfaceId surface = 
			{
			1,2,3,4
			};
	TInt buffer = 1;
	TInt res = session.SubmitUpdate(screen, surface, buffer, NULL);
	TEST(res == KErrArgument);
	
	// Test submit before notify with -ve buffer
	screen = 1;
	buffer = -1;
	res = session.SubmitUpdate(screen, surface, buffer, NULL);
	TEST(res == KErrArgument);

	//Test submit before notify with Null surfaceId
	surface = TSurfaceId::CreateNullId();
	screen = 1;
	buffer = 1;
	res = session.SubmitUpdate(screen, surface, buffer, NULL);
	TEST(res == KErrArgument);
	session.Close();
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0017


  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Using a rogue client to call multiple notifies in a row. 

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create a second thread which will call multiple notifies
   in succession (not interleaved with Submits).
  	
  @SYMTestExpectedResults The thread should have an exit category
   of SurfUpServ and and exit reason of EUpdateServPanicDataIntegrity in
   all cases.
*/
void CTSurfaceUpdate::TestCase10()
	{
 	for(TInt testNum = 0; testNum < 3; ++testNum)
 		{
 		_LIT(KThreadName, "TestMultipleNotify%d");
 		TBuf<19> threadName;
 		threadName.Format(KThreadName, testNum);
 		RThread thread;
 		TRequestStatus status;
	 	TMultipleNotify statusCol(status, testNum);
		TEST(KErrNone == thread.Create(threadName,
				TestMultipleNotifyThreadL,
				KDefaultStackSize,
				&User::Heap(),
				&statusCol));
		TRequestStatus rendezvousStatus;
		thread.Rendezvous(rendezvousStatus);
		thread.Resume();
		User::WaitForRequest(rendezvousStatus);
		// We expect the first request to be cancelled.
		//The request can also be on a pending state if the connection is closed before the 
		//cancellation has been transferred to client 
		TEST((KErrCancel == status.Int()) || (status.Int() == KRequestPending));
		// We expect the second request to Panic the client
		_LIT(KExitCategory,"SurfUpServ");
		TBuf<16> exitCategory(KExitCategory);
		TEST(thread.ExitReason() == EUpdateServPanicDataIntegrity);
		TEST(thread.ExitCategory() == exitCategory);
		thread.Close();
 		}
	}
	
/**
Thread function called from TestCase10 in order to submit multiple notifies.
*/	
TInt CTSurfaceUpdate::TestMultipleNotifyThreadL(TAny* aStatusCol)
	{
	User::SetJustInTime(EFalse);
	RMisbehavingSession badSession;
	User::LeaveIfError(badSession.Connect());
	TMultipleNotify* statusCol = static_cast<TMultipleNotify*>(aStatusCol);
	TRequestStatus status;
	switch(statusCol->iTestNum)
		{
	case 0:
		badSession.NotifyWhenAvailableNoChecking(statusCol->iStatus);
		badSession.NotifyWhenAvailableNoChecking(status);
		break;
	case 1:
		badSession.NotifyWhenDisplayedNoChecking(statusCol->iStatus);
		badSession.NotifyWhenDisplayedNoChecking(status);
		break;
	case 2:
		badSession.NotifyWhenDisplayedXTimesNoChecking(10, statusCol->iStatus);
		badSession.NotifyWhenDisplayedXTimesNoChecking(10, status);
		break;
	default:
		_LIT(KMultipleNotifyPanic, "MultNotifyPanic");
		User::Panic(KMultipleNotifyPanic, 1);
		break;
		}
	User::WaitForRequest(statusCol->iStatus);
	User::WaitForRequest(status);
	// Exit reason will be the panic number due to the invalid op code:
 	// returning zero here is redundant
	return 0;
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0018

  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Use a rogue client to misbehave in various ways.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 1. Attempt to connect with an incorrect version number.
    Connect to the server, communicate with server without proper client side checking:
    2. Submit update with a negative buffer number (no client side checking).
    3. Submit update with a negative screen number (no client side checking).
    4. Call NotifyWhenDisplayedXTimes with no client side checking and a count of 0.
  	
  @SYMTestExpectedResults 1. KErrNotSupported
    2. KErrArgument
    3. KErrArgument
    4. Status int is KErrArgument (SubmitUpdate == KErrNone).
*/
void CTSurfaceUpdate::TestCase11()
	{
	// Attempt to connect with the wrong version number
	RMisbehavingSession session;
	TEST(KErrNotSupported == session.Connect(KDefaultMessageSlot, ETrue));
	
	TEST(KErrNone == session.Connect());
	TInt screen = 0;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	TInt buffer = -1;
	
	// Call RMisbehavingSession::SubmitUpdate which doesn't do the correct
	// client side checking
	TEST(KErrArgument == session.SubmitUpdateNoChecking(screen, surface, buffer, NULL));
	
	// Test with -ve screen
	screen = -1;
	buffer = 0;
	TEST(KErrArgument == session.SubmitUpdateNoChecking(screen, surface, buffer, NULL));
	
	// Test calling NotifyWhenDisplayedXTimes with no client side checking and a count of 0
	screen = 0;
	buffer = 1;
	TRequestStatus status;
	session.NotifyWhenDisplayedXTimesNoChecking(0, status);
	TEST(KErrNone == session.SubmitUpdateNoChecking(screen, surface, buffer, NULL));
	User::WaitForRequest(status);
	TEST(KErrArgument == status.Int());
	
	session.Close();
	}


/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0019


  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Starting the surface update server in two different threads.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Start the surface update server in two different threads.
    
  @SYMTestExpectedResults Thread 1: KErrNone
     Thread 2: KErrNone
     The "provider" returned to both threads should also be the same value!
*/
void CTSurfaceUpdate::TestCase12L()
    {
    MSurfaceUpdateServerProvider *surfaceUpdateProvider = NULL;
    TInt res = StartSurfaceUpdateServer(surfaceUpdateProvider);
    TEST(res == KErrNone);
    User::LeaveIfError(res);

    _LIT(KThreadName, "TestServerStartupTwoThreads");
    TTime tm;
    TBuf<32> buf;
    tm.UniversalTime();
    TRAP(res, tm.FormatL(buf, _L("_%H%T%S%C")));
    TEST(res == KErrNone);
    User::LeaveIfError(res);
    TBuf<128> threadName(KThreadName);
    threadName.Append(buf); //guarantee uniqueness  of the thread name
    RThread thread;
    MSurfaceUpdateServerProvider *surfaceUpdateProvider2 = NULL;
    res = thread.Create(threadName,
            TestServerStartupTwoThreads,
            KDefaultStackSize,
            &User::Heap(),
            &surfaceUpdateProvider2);
    TEST(res == KErrNone);
    User::LeaveIfError(res);
    TRequestStatus rendezvousStatus;
    thread.Rendezvous(rendezvousStatus);
    thread.Resume();
    User::WaitForRequest(rendezvousStatus);
    TEST(KErrNone == rendezvousStatus.Int());
    // Check that we get the same provider for both threads!
    TEST(surfaceUpdateProvider2 == surfaceUpdateProvider);
    }


/**
Starting the update server in another thread - called from TestCase12
*/
TInt CTSurfaceUpdate::TestServerStartupTwoThreads(TAny *aArgPtr)
	{
	MSurfaceUpdateServerProvider **surfaceUpdateProviderPtr = 
	        reinterpret_cast<MSurfaceUpdateServerProvider **>(aArgPtr);
	TInt res = StartSurfaceUpdateServer(*surfaceUpdateProviderPtr);
	return res;
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0020

  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Requesting notifies, leaving enough time for them to 
  complete before requesting the subsequent notification.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Execute some notifies and submits, wait for a short period (100us), 
  then cancel them all.
  	
  @SYMTestExpectedResults Two results are acceptable: KErrNone (notify 
  completed before it was cancelled) or KErrCancel (notify sucessfully cancelled).
*/	
void CTSurfaceUpdate::TestCase13L()
	{
	RSurfaceUpdateSession session;
	const TInt numNotifies = 10;
	User::LeaveIfError(session.Connect(numNotifies));	
	
	TRequestStatus statusArray[numNotifies];
	TInt screen = 0;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	TInt buffer = 1;
	
	// Send the same type of notification request multiply, cancelling after each submission
	for(TInt ii = 0; ii < numNotifies; ++ii)
		{
		session.NotifyWhenAvailable(statusArray[ii]);
		TEST(KErrNone == session.SubmitUpdate(screen, surface, buffer, NULL));
		session.CancelAllUpdateNotifications();
		User::After(100); // Enough time for the notify to be processed
		}
	
	// Check that the error codes are acceptable
	for(TInt ii = 0; ii < numNotifies; ++ii)
		{
		TInt ret = statusArray[ii].Int();
		TEST(statusArray[ii].Int() == KErrNone || statusArray[ii].Int() == KErrCancel);
		}
	
	session.Close();
	}
	
/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0021

  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Terminating and restarting the server.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Terminate and restart the server
  	
  @SYMTestExpectedResults Test that trying to connect the session after 
  	termination of the server results in KErrNotFound, and that the server 
  	can be restarted with result KErrNone.
*/
void CTSurfaceUpdate::TestCaseTerminateServer()
	{
	(static_cast <CTSurfaceUpdateStep*> (iStep)) ->TerminateServer();
	
	//at that point server has been terminated
	RSurfaceUpdateSession session;
	TInt res = session.Connect();
	TEST(res == KErrNotFound);
	session.Close();
	
	res = (static_cast <CTSurfaceUpdateStep*> (iStep)) ->StartServer();
	TEST(res == KErrNone);
	res = session.Connect();
	TEST(res == KErrNone);
	session.Close();
	}
	
/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0022

  @SYMPREQ 1007
  
  @SYMREQ 8223

  @SYMTestCaseDesc Terminating the thread while some outstanding request in progress.

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Terminate the thread which has outstanding request to 
  the server for surface update notifications. 
  	
  @SYMTestExpectedResults Surface update surface should process the request 
  and try to send notification without hanging.
  
*/
void CTSurfaceUpdate::TestCase14()
	{
 	_LIT(KThreadNameTemplate, "TestPanicClientThread1_%d");
 	
 	RThread thread;
	const TInt KMaxIndex = 50;
 	for(TInt index = 0; index < KMaxIndex; index++)
 		{
 		const TInt KUnexpectedLeavePanic = 194;
 		TBuf<128> threadName;
 		threadName.Format(KThreadNameTemplate, index);
 		TInt res = thread.Create(threadName,
			PanicClientThread1,
			KDefaultStackSize,
			&User::Heap(),
			this);
		TEST(res == KErrNone);	
 		TRequestStatus status = KRequestPending;
 		thread.Logon(status);
 		thread.Resume();
 		User::WaitForRequest(status);

 		res = thread.ExitReason();
 		TEST(res == KUnexpectedLeavePanic);
 		if(res != KUnexpectedLeavePanic)
 			{
 			INFO_PRINTF2(_L("Thread exits with error code %d"), res);
 			}
 		thread.Close();
 		}
	}

/**
  @SYMTestCaseID GRAPHICS-SURFACEUPDATE-0025

  @SYMPREQ 1007/11599, 1007/11600, 1007/11601

  @SYMREQ 8223

  @SYMTestCaseDesc SubmitUpdate() with a global and a per-display 
  update cannot be mixed.
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 
	Create the session and make a global submit update.
	Subscribe for all notifications and make per screen submit update. 
	Create another session and make a per screen update.
	Subscribe for all notifications and make a global submit update. 
	
  		
  @SYMTestExpectedResults 
  	Mixing two different types of submit update should fail with an error KErrNotSupported.
*/
void CTSurfaceUpdate::TestCase18()
	{
	INFO_PRINTF1(_L("SubmitUpdate() to deal with both a global and a per-display update in the same session"));

	RSurfaceUpdateSession session;
	TInt buffer = 2;
	TSurfaceId surface = 
		{
		1,2,3,4
		};

	TInt res = session.Connect();
	TEST(res == KErrNone);

	res = session.SubmitUpdate(KAllScreens, surface, buffer, NULL);
	TEST(res==KErrNone);
	
	TRequestStatus status = KRequestPending;
	TRequestStatus status1 = KRequestPending;
	TRequestStatus status2 = KRequestPending;
	TTimeStamp timeStamp; 
	
	session.NotifyWhenAvailable(status);	
	session.NotifyWhenDisplayed(status1, timeStamp);	
	session.NotifyWhenDisplayedXTimes(10, status2);
		
	res = session.SubmitUpdate(1, surface, buffer, NULL);
	TEST(res==KErrNotSupported);
		
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);

	TEST(status == KErrNotSupported);
	TEST(status1 == KErrNotSupported);
	TEST(status2 == KErrNotSupported);
	
	session.Close();
	//make per screen update first
	res = session.Connect();
	TEST(res == KErrNone);

	res = session.SubmitUpdate(1, surface, buffer, NULL);
	TEST(res==KErrNone);
	
	status = KRequestPending;
	status1 = KRequestPending;
	status2 = KRequestPending;
	
	session.NotifyWhenAvailable(status);	
	session.NotifyWhenDisplayed(status1, timeStamp);	
	session.NotifyWhenDisplayedXTimes(10, status2);
		
	res = session.SubmitUpdate(KAllScreens, surface, buffer, NULL);
	TEST(res==KErrNotSupported);
		
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);

	TEST(status == KErrNotSupported);
	TEST(status1 == KErrNotSupported);
	TEST(status2 == KErrNotSupported);
	
	session.Close();
	}

/**
Thread function called from TestCase14 in order to panic the client
*/ 	
TInt CTSurfaceUpdate::PanicClientThread1(TAny* aAny)
	{
	CTSurfaceUpdate* surfaceUpdateTest = (CTSurfaceUpdate*) aAny;
	surfaceUpdateTest->GenerateMultipleUpdateWithPanic();
	
	return 0;
	}

/**
Helper function for test case described in TestCase14
*/ 	
void CTSurfaceUpdate::GenerateMultipleUpdateWithPanic()
	{
	TRequestStatus status = KRequestPending;
	TTimeStamp timeStamp; 
	RSurfaceUpdateSession session;
	TInt screen = 0;
	TSurfaceId surface = 
		{
		1,2,3,4
		};
	TInt buffer = 0;
	
	TInt res = session.Connect();
	TEST(res == KErrNone); 
	session.NotifyWhenDisplayed(status, timeStamp);	
	res = session.SubmitUpdate(screen, surface, buffer);
	TEST(res == KErrNone); 
	User::After(TTimeIntervalMicroSeconds32(100000));
	
	User::PanicUnexpectedLeave();
	}

void CTSurfaceUpdate::ResetSusL()
	{
	while (iReceivers.Count() > 0)
		{
		CTContentUpdateReceiver *receiver = iReceivers[0];
		iReceivers.Remove(0);
		CloseTestUpdateReceiver(receiver);
		}
	(static_cast<CTSurfaceUpdateStep*>(iStep))->TerminateServer();
	User::LeaveIfError((static_cast<CTSurfaceUpdateStep*>(iStep))->StartServer());
	}

void CTSurfaceUpdate::SetupTestCaseRegisterPanicL()
	{
	ResetSusL();
	// Start two content update receivers for register test
	CTContentUpdateReceiver* receiverA;
	CTContentUpdateReceiver* receiverB;
	User::LeaveIfError(StartTestUpdateReceiver(receiverA, 0));
	iReceivers.AppendL(receiverA);
	User::LeaveIfError(StartTestUpdateReceiver(receiverB, 1));
	iReceivers.AppendL(receiverB);
	}

void CTSurfaceUpdate::DoTestCaseRegisterPanicL(TThreadFunction aThreadFunction)
	{
	_LIT(KRegisterThreadName, "TestPanicRegisterThread");
	RThread thread;
	TInt res = thread.Create(KRegisterThreadName, aThreadFunction,
							 KDefaultStackSize,	&User::Heap(), this);
	TEST(res == KErrNone);
	User::LeaveIfError(res);
	TRequestStatus status = KRequestPending;
	thread.Logon(status);
	thread.Resume();
	User::WaitForRequest(status);

	res = thread.ExitReason();
	TEST(res == EUpdateServPanicRegister);
	if(res != EUpdateServPanicRegister)
		{
		ERR_PRINTF2(_L("Thread exits with error code %d"), res);
		}
	thread.Close();
	}

/**
 * Ensure that there is no outstanding request from all receivers
 */
void CTSurfaceUpdate::WaitUntilAllNotificationsProcessed()
	{
	const TInt KMaxIterationNumber = 10;
	TInt ii = 0;
	for(; ii < KMaxIterationNumber; ii++)
		{
		if(((iReceivers[0]->OutstandingRequest() == EFalse)) &&
				((iReceivers[1]->OutstandingRequest() == EFalse)) &&
				((iReceivers[2]->OutstandingRequest() == EFalse)))
				break;
		INFO_PRINTF2(_L("Wait until all notification processed, iteration %d"), ii);
		User::After(KCompositionIntervalLong);
		}
	TEST(ii < KMaxIterationNumber);//We have waited enough, if it fails, something weird going on
	}

void CTSurfaceUpdate::RunTestCaseL(TInt aCurTestCase)
	{
	((CTSurfaceUpdateStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0001"));
		if((static_cast <CTSurfaceUpdateStep*> (iStep)) -> iGce)
			{
			INFO_PRINTF1(_L("TestCase1"));
			TRAPD(ret, TestCase1());
			TEST(ret == KErrNone);
			}
		else
			{
			TestComplete();		
			}	
		break;
		}
	case 2:
		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0002"));
		INFO_PRINTF1(_L("TestCase2L"));
		TRAPD(ret, TestCase2L());
		TEST(ret == KErrNone);
		break;
		}
	case 3:
		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0003"));
		INFO_PRINTF1(_L("TestCase3L"));
		TRAPD(ret, TestCase3L());
		TEST(ret == KErrNone);
		break;
		}
	case 4:
		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0004"));
		INFO_PRINTF1(_L("TestCase4L"));
		TRAPD(ret, TestCase4L());
		TEST(ret == KErrNone);
		break;
		}
	case 5:
		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0005"));
		INFO_PRINTF1(_L("TestCase5L"));
		TRAPD(ret, TestCase5L());
		TEST(ret == KErrNone);
		break;
		}
	case 6:
		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0006"));
		INFO_PRINTF1(_L("TestCase6L"));
		TRAPD(ret, TestCase6L());
		TEST(ret == KErrNone);
		break;
		}
	case 7:
		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0014"));
		INFO_PRINTF1(_L("TestCase7L"));
		TRAPD(ret, TestCase7L());
		TEST(ret == KErrNone);
		break;
		}
	case 8:
		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0015"));
		INFO_PRINTF1(_L("TestCase8"));
		TRAPD(ret, TestCase8());
		TEST(ret == KErrNone);
		break;
		}
	case 9:
 		{
 		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0016"));
 		INFO_PRINTF1(_L("TestCase9L"));
 		TRAPD(ret, TestCase9L());
 		TEST(ret == KErrNone);
 		break;
		}
	case 10:
 		{
 		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0017"));
 		INFO_PRINTF1(_L("TestCase10"));
 		TRAPD(ret, TestCase10());
 		TEST(ret == KErrNone);
 		break;
		}
	case 11:
 		{
 		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0018"));
 		INFO_PRINTF1(_L("TestCase11"));
 		TRAPD(ret, TestCase11());
 		TEST(ret == KErrNone);
 		break;
		}
    case 12:
        {
        ((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0019"));
        INFO_PRINTF1(_L("TestCase12L"));
        TRAPD(ret, TestCase12L());
        TEST(ret == KErrNone);
        break;
        }
	case 13:
 		{
 		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0020"));
 		INFO_PRINTF1(_L("TestCase13L"));
 		TRAPD(ret, TestCase13L());
 		TEST(ret == KErrNone);
 		break;
		}
	case 14:
 		{
 		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0022"));
 		INFO_PRINTF1(_L("TestCase14"));
 		TestCase14();
 		break;
		}
	case 15:
 		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0023"));
 		INFO_PRINTF1(_L("TestCase15"));
 		TRAPD(ret, TestCase15());
 		TEST(ret == KErrNone);
 		break;
		}
	case 16:
 		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0024"));
 		INFO_PRINTF1(_L("TestCase16"));
 		TRAPD(ret, TestCase16L());
 		TEST(ret == KErrNone);
 		break;
		}
	case 17:
 		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0028"));
 		INFO_PRINTF1(_L("TestCase17"));
 		TRAPD(ret, TestCase17());
 		TEST(ret == KErrNone);
 		break;
		}
	case 18:
 		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0025"));
 		INFO_PRINTF1(_L("TestCase18"));
 		TRAPD(ret, TestCase18());
 		TEST(ret == KErrNone);
 		break;
		}
	case 19:
		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0027"));
	 	INFO_PRINTF1(_L("TestCaseNotifyWhenAvailableGlobal"));
 		TRAPD(ret, TestCaseNotifyWhenAvailableGlobal());
 		TEST(ret == KErrNone);
 		break;
		}
	case 20:
		{
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0029"));
	 	INFO_PRINTF1(_L("TestCaseRegisterIdenticalPrioritiesL"));
 		TRAPD(ret, TestCaseRegisterIdenticalPrioritiesL());
 		TEST(ret == KErrNone);
 		break;
		}
	case 21:
 		{//should be the last test case
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEUPDATE-0021"));
 		INFO_PRINTF1(_L("TestCaseTerminateServer"));
 		TRAPD(ret, TestCaseTerminateServer());
 		TEST(ret == KErrNone);
 		break;
		}
	default:
		((CTSurfaceUpdateStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTSurfaceUpdateStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	((CTSurfaceUpdateStep*)iStep)->RecordTestResultL();
	}

//--------------
__CONSTRUCT_STEP__(SurfaceUpdate)

void CTSurfaceUpdateStep::TestSetupL()
	{
#ifdef __WINS__
	UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalBoolProperty,  (TAny*)"symbian_graphics_use_gce",  &iGce);
#else
	iGce = ETrue;
// we would need to check here whether it is gce-mode or not.	
#endif	
	if(iGce)
		{
		User::LeaveIfError(StartServer());
		}
	else
		{//it should work for both cases, but that would be waste of time
		INFO_PRINTF1(_L("This test must be run in the GCE environment"));
		}	
	}
	
void CTSurfaceUpdateStep::TestClose()
	{
	TerminateServer();
	}

TInt CTSurfaceUpdateStep::StartServer()
	{
	return StartSurfaceUpdateServer(iSurfaceUpdateProvider);
	}
	
void CTSurfaceUpdateStep::TerminateServer()
	{
	if(iSurfaceUpdateProvider)
		{
		iSurfaceUpdateProvider->Terminate();
		iSurfaceUpdateProvider = NULL;
		}
	}
	
//-------------
// Code for client that doesn't quite behave as it should

TInt RMisbehavingSession::Connect(TInt aMessageSlots, TBool aWrongVersion /*= false*/)
	{
	if(Handle())
		{
		return KErrAlreadyExists; 
		}
	iStatusAvailable = NULL;
	iStatusDisplayed = NULL;
	iStatusDisplayedXTimes = NULL;
	if(aWrongVersion)
		return CreateSession(KTestSurfaceUpdateServerName, WrongVersion(), aMessageSlots);
	else
		return CreateSession(KTestSurfaceUpdateServerName, Version(), aMessageSlots);
	}
	

void RMisbehavingSession::Close()
	{
	if(Handle())
		{
		RHandleBase::Close();
		}
	iStatusAvailable = NULL;
	iStatusDisplayed = NULL;
	iStatusDisplayedXTimes = NULL;
	}
	
TVersion RMisbehavingSession::Version() const
	{
	return (TVersion(KSurfaceUpdateServMajorVersionNumber, KSurfaceUpdateServMinorVersionNumber, KSurfaceUpdateServBuildVersionNumber));
	}
	
TVersion RMisbehavingSession::WrongVersion() const
	{
	return (TVersion(KSurfaceUpdateServMajorVersionNumber + 1, KSurfaceUpdateServMinorVersionNumber + 1, KSurfaceUpdateServBuildVersionNumber + 1));
	}

/**
Send the NotifyWhenAvailable request directly to the server (without waiting for submit)
*/
void RMisbehavingSession::NotifyWhenAvailableNoChecking(TRequestStatus& aStatus)
	{
	SendReceive(EUpdateServNotifyWhenAvailable, aStatus);
	}

/**
Send the NotifyWhenDisplayed request directly to the server (without waiting for submit)
*/
void RMisbehavingSession::NotifyWhenDisplayedNoChecking(TRequestStatus& aStatus)
	{
	TTimeStamp* timeStamp = NULL;	
	TIpcArgs args(timeStamp);
	SendReceive(EUpdateServNotifyWhenDisplayed, args, aStatus);
	}

/**
Send the NotifyWhenDisplayedXTimes request directly to the server (without waiting for submit)
*/
void RMisbehavingSession::NotifyWhenDisplayedXTimesNoChecking(TInt aCount, TRequestStatus& aStatus)
	{
	TIpcArgs args(aCount);
	SendReceive(EUpdateServNotifyWhenDisplayedXTimes, args, aStatus);
	}

/**
Submit an update to the server without doing any checking of values such as screen number and buffer
*/
TInt RMisbehavingSession::SubmitUpdateNoChecking(TInt aScreen, const TSurfaceId& aSurfaceId, 
									TInt aBuffer, const TRegion* aDirtyRegion)
	{
	TPckgC<TSurfaceId> pckgSurfaceId(aSurfaceId);
	TIpcArgs theArgs(aScreen, &pckgSurfaceId, aBuffer);
	if(aDirtyRegion)
		{
		const TInt regionCount = aDirtyRegion->Count();
		if(regionCount > 0)
			{
			TPtrC8 ptrRect(reinterpret_cast<const TUint8*> (aDirtyRegion->RectangleList()), regionCount * sizeof(TRect));
			theArgs.Set(3, &ptrRect);
			}
		}
	return SendReceive(EUpdateServSubmitUpdate, theArgs);
	}

/**
Pass an invalid opcode to the surface update server
*/
void RMisbehavingSession::TestPassingInvalidOpcode(TInt aInvalidOpcode)
	{
	SendReceive(aInvalidOpcode);
	}

/**
Set the fail rate for OOM checking, server side.
*/	
void RMisbehavingSession::SetFailRate(TInt aFailRate)
	{
	TIpcArgs args(aFailRate);
	SendReceive(EUpdateServOOM, args);
	}

/**
Implementation of NotifyWhenAvailable. Intended to behave the same as 
RSurfaceUpdateSession::NotifyWhenAvailable except it assumes that the handle
to the RClass is never a Null handle.
*/
void RMisbehavingSession::NotifyWhenAvailable(TRequestStatus& aStatus)
	{
	iStatusAvailable = &aStatus;
	}

/**
Implementation of NotifyWhenDisplayed. Intended to behave the same as 
RSurfaceUpdateSession::NotifyWhenDisplayed except it assumes that the handle
to the RClass is never a Null handle.
*/
void RMisbehavingSession::NotifyWhenDisplayed(TRequestStatus& aStatus, TTimeStamp& aTimeStamp)
	{
	iStatusDisplayed = &aStatus;
	iTimeStamp = &aTimeStamp;
	}

/**
Implementation of NotifyWhenDisplayedXTimes. Intended to behave the same as 
RSurfaceUpdateSession::NotifyWhenDisplayedXTimes except it assumes that the handle
to the RClass is never a Null handle.
*/
void RMisbehavingSession::NotifyWhenDisplayedXTimes(TInt aCount, TRequestStatus& aStatus)
	{
	iStatusDisplayedXTimes = &aStatus;
	iCount = aCount;
	}

/**
Implementation of NotifyWhenAvailable. Intended to behave the same as 
RSurfaceUpdateSession::NotifyWhenAvailable except it assumes that the user
will not pass invalid parameters to the function.
*/
TInt RMisbehavingSession::SubmitUpdate(TInt aScreen, const TSurfaceId& aSurfaceId, 
											TInt aBuffer, const TRegion* aDirtyRegion/* = NULL*/)
	{
	if(iStatusAvailable)
		{
		SendReceive(EUpdateServNotifyWhenAvailable, *iStatusAvailable);
		iStatusAvailable= NULL;
		}
	
	if(iStatusDisplayed)
		{
		TIpcArgs args(iTimeStamp);
		SendReceive(EUpdateServNotifyWhenDisplayed, args, *iStatusDisplayed);
		iStatusDisplayed=NULL;
		}
		
	if(iStatusDisplayedXTimes)
		{
		TIpcArgs args(iCount);
		SendReceive(EUpdateServNotifyWhenDisplayedXTimes, 
                     	args, *iStatusDisplayedXTimes);	
		iStatusDisplayedXTimes=NULL;
		}

	TPckgC<TSurfaceId> pckgSurfaceId(aSurfaceId);
	TIpcArgs theArgs(aScreen, &pckgSurfaceId, aBuffer);

	if(aDirtyRegion)
		{
		const TInt regionCount = aDirtyRegion->Count();
		if(regionCount > 0)
			{
			TPtrC8 ptrRect(reinterpret_cast<const TUint8*> (aDirtyRegion->RectangleList()), regionCount * sizeof(TRect));
			theArgs.Set(3, &ptrRect);
			}
		}

	//final request for composition 
	return SendReceive(EUpdateServSubmitUpdate, theArgs);
	}

/**
Implementation of NotifyWhenAvailable. Intended to behave the same as 
RSurfaceUpdateSession::CancelAllUpdateNotifications except it assumes that the handle
to the RClass is never a Null handle.
*/
void RMisbehavingSession::CancelAllUpdateNotifications()
	{
	SendReceive(EUpdateServCancelAllNotifications);
	}


