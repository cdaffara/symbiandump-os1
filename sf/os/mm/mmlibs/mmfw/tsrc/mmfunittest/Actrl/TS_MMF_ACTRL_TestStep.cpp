// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>

// Test system includes
#include <testframework.h>

#include "TS_MMF_ACTRL_TestStep.h"
#include "TS_MMF_ACTRL_TestSuite.h"

//_LIT(KImageTestSourceScaledFileName,"ref\\scaled");	// EABI warning removal
//_LIT(KImageTestRefExt,".mbm");						// EABI warning removal

//const TUid KDummyId = { -1 };   // sentinel value		// EABI warning removal
const TInt KDelay = 10000000;   // arbitrary delay
const TInt KMaxRetries = 10;    // arbitrary number of retries


/**
*
* CTestStepActrl constructor 
*
* 
*/
CTestStepActrl::CTestStepActrl() 
:  iActrlSuite(NULL)
	{
	}

/**
*
* CTestStepActrl destructor
*
* 
*/
CTestStepActrl::~CTestStepActrl()
	{
	}

/**
* SetSuite is a set function
* @param  aPtr is a ptr to a CTestSuiteActrl
* @returns void
*/
void CTestStepActrl::SetSuite( CTestSuiteActrl* aPtr )
	{
	iActrlSuite = aPtr;
	}


/**
DoTestStepPreambleL is the preamble function for test steps
It creates an active scheduler and installs it. This is necessary
since we are multithreading and each test step runs in a separate thread.
@returns TVerdict which is either EPass or Efail
 
The function can leave if there is insufficient memeory
*/
TVerdict CTestStepActrl::DoTestStepPreambleL(void)
	{
	iPriority.iPriority = 11;  // set the priority to some default value
	iPriority.iPref = EMdaPriorityPreferenceQuality;
	return EPass;
	}

/**
The post amble step cleans up the allocated memory in the preamble
@returns TVerdict EPass or EFail
@leaves can leave due to insufficient memory
@lib "TS_Actrl.lib"
*/
TVerdict CTestStepActrl::DoTestStepPostambleL(void)
	{
	//[ lets give the system time 
	//( under all envs ) to unload the controller]
	User::After(1500000); 

	return EPass;
	}

/**
*
* CTestStepActrl GetSuite accessor function
*
*/
CTestSuiteActrl* CTestStepActrl::GetSuite()
	{
	return iActrlSuite;
	}

/*
 SearchForEvent
 Searches for an event
 returns whether the event has been found
*/
TVerdict CTestStepActrl::SearchForEvent( TMMFEvent& aEvent )
	{
	TVerdict result = EFail;
	
	// [ status of events ]
	TRequestStatus timerStatus    ;
	TRequestStatus eventStatus    ;
	
	// [ due to problems with the timer as a member variable 
	//   added a local timer here ]
	RTimer  myTimer ;
	TInt err = myTimer.CreateLocal();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("CreateLocal failed, error code %d"), err);
		return EFail;
		}
	
    //[ create an event and initialise with unknown marker ]
	
    TMMFEventPckg receivedEvent;
	
	//[ for KMaxRetries attempt to find the event ]
	for( TInt retries = 0; retries < KMaxRetries; retries++ )
		{
		// [post receive event to controller ]
		iController.ReceiveEvents( receivedEvent, eventStatus );
		// [ start break out timer to escape ]
		myTimer.After( timerStatus, KDelay );
		//StartTheTimer( timerStatus,  KDelay );
		
		// [ wait for an event to mature ]
		User::WaitForRequest( eventStatus, timerStatus );
		
		// [ check if the command is the event 
		//   and that a timeout did not occur]
        if( IsTimeOut( eventStatus ))
			{
			// cancel the receive events
			CancelReceivedEvents();
			}
		else if( IsSoughtEvent( aEvent, receivedEvent ) )
			{
			// [ cancel the outstanding timer ]
			User::Check();
			// [ due to problems with rtimer as member variable ]
			myTimer.Cancel();
			//CancelTheTimer();
			//[ set status to pass since we have found the event ]
			result = EPass;
			break;
			}
		else
			{
			// [ We rx'd an event other than expected ]
			// [ due to problems with RTimer as mv]
			myTimer.Cancel();
			//CancelTheTimer();
			}
		}
	
    return result;
	}

/*
*
* IsTimeOut
*
*/
TBool CTestStepActrl::IsTimeOut( TRequestStatus& aEventStatus )
	{
     return ( aEventStatus == KRequestPending);
	}

/*
*
* IsSoughtEvent
*
*/
TBool CTestStepActrl::IsSoughtEvent( TMMFEvent& aExpectedEvent, TMMFEventPckg& aReceivedEvent )
	{
	
	 TInt expectedUid = aExpectedEvent.iEventType.iUid;
	 TInt receivedUid = aReceivedEvent().iEventType.iUid;

	 INFO_PRINTF3(_L("Expecting Event: 0x%08x error code %d"), 
					 expectedUid, aExpectedEvent.iErrorCode);
	 INFO_PRINTF3(_L("Event received : 0x%08x error code %d"), 
					 aReceivedEvent().iEventType.iUid, aReceivedEvent().iErrorCode);

	 return ((expectedUid == receivedUid ) &&
	 	     ( aExpectedEvent.iErrorCode == aReceivedEvent().iErrorCode ));
	}

/*
CancelReceivedEvents
This function cancels an existing rx events
*/
void CTestStepActrl::CancelReceivedEvents()
	{
	iController.CancelReceiveEvents();
	}

/*
*
* SetInputFileNameL
*
*/
void CTestStepActrl::SetReadFileNameL( const TDesC& aPathNameAndExtn )
{
//[ append the filename relative to the default path ]
 User::LeaveIfError(iFs.Connect());
 TFileName fileName = GetSuite()->DefaultPath();
 fileName.Append(aPathNameAndExtn);
 iFileConfigRead().iPath = fileName ;
}

/*
*
* SetOutputFileNameL
*
*/
void CTestStepActrl::SetWriteFileNameL( const TDesC& aPathNameAndExtn )
	{
	//[ append the filename relative to the default path ]
	User::LeaveIfError(iFs.Connect());
	TFileName fileName = GetSuite()->DefaultPath();
	fileName.Append(aPathNameAndExtn);
	TInt ret = iFs.Delete(fileName);
	if (ret != KErrNone && ret != KErrNotFound)
		User::Leave(ret);
	iFileConfigWrite().iPath = fileName ;
	
	Log( fileName );
	}


TMMFDescriptorParams SetDescriptor( TPtr8* aDescriptor )
{
	RThread thread;
	TMMFDescriptorParams params;

	params.iDes = aDescriptor;			
	params.iDesThreadId = thread.Id(); // This thread

	return params;
}

void CTestStepActrl::SetReadDescriptor( TPtr8* aDescriptor )
{
	// The read descriptor's length will be correctly set by RFile::Read().
	iDescConfigRead = SetDescriptor(aDescriptor);
}

void CTestStepActrl::SetWriteDescriptor( TPtr8* aDescriptor )
{
	// Must explicitly set the write descriptor's length to zero.
	aDescriptor->Zero();
	iDescConfigWrite = SetDescriptor(aDescriptor);
}

