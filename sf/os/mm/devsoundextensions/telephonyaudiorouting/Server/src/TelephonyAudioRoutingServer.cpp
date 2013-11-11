/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   TelephonyAudioRoutingServer implementation
*
*/

 

#include <e32svr.h>
#include <e32math.h>
#include <data_caging_path_literals.hrh>
#include <barsc.h>
#include <barsread.h>
#include "TelephonyAudioRoutingServer.h"
#include "TelephonyAudioRoutingServerSession.h"
#include "TelephonyAudioRoutingClientServer.h"


void PanicClient(
	const RMessage2& aMessage,
	TTelephonyAudioRoutingPanic aPanic)
	{
	_LIT(KPanic,"TelephonyAudioRoutingServer");
	aMessage.Panic(KPanic,aPanic);
	}
	
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// StartThreadL
// Start the TelephonyAudioRoutingServer thread.
// Returns: TInt: error code
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTelephonyAudioRoutingServer::StartThreadL(
	TAny* /*aParms*/)  
/**
Thread entry-point function.
The TServerStart objects is passed as the thread parameter
**/
	{
		
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::StartThreadL "));
	
	TInt err = KErrNone;
	__UHEAP_MARK;
	
	CTrapCleanup* cleanup = CTrapCleanup::New();

	if (!cleanup)
	err = KErrNoMemory;
	
	if (!err)
		{
		CActiveScheduler* sched=NULL;
		sched=new(ELeave) CActiveScheduler;
		CActiveScheduler::Install(sched);
		CTelephonyAudioRoutingServer* server = NULL;
		TRAPD(err,server = CTelephonyAudioRoutingServer::NewL()); 		
	
		if(!err)
		{
		// Sync with the client and enter the active scheduler
			RThread::Rendezvous(KErrNone);
			sched->Start();
		}
	
		delete server;
		delete sched;	
		}
	
	delete cleanup;

	__UHEAP_MARKEND;
	return err;
	}
	
	
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::CTelephonyAudioRoutingServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTelephonyAudioRoutingServer::CTelephonyAudioRoutingServer()
	:CServer2(0,EUnsharableSessions),
	iSessionIdsInUse()
	{
	iSessionCount = 0;
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServer::ConstructL()
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::ConstructL "));
	TName name(RThread().Name());
	StartL(name);
	
	// Set default values
   	iCurrentAudioOutput = CTelephonyAudioRouting::ENotActive;
   	iPreviousAudioOutput = CTelephonyAudioRouting::ENotActive;
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CTelephonyAudioRoutingServer* CTelephonyAudioRoutingServer::NewL()
	{
	CTelephonyAudioRoutingServer* self=new(ELeave) CTelephonyAudioRoutingServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// Destructor
CTelephonyAudioRoutingServer::~CTelephonyAudioRoutingServer()
    {
 	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::~CTelephonyAudioRoutingServer "));   
	iSessionIdsInUse.Close();
	iSetOutputRequests.Close();
	iAvailableOutputs.Close();
	}
	

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::NewSessionL
// Create a new client session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSession2* CTelephonyAudioRoutingServer::NewSessionL(
	const TVersion&,
	const RMessage2& /*aMessage*/) const
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::NewSessionL "));	
		 
	CTelephonyAudioRoutingServer& nonConstThis = *const_cast<CTelephonyAudioRoutingServer*>(this);
	const TInt sessionId = nonConstThis.IdentifyAndAllocateNextFreeSessionIdL();
	return new (ELeave) CTelephonyAudioRoutingServerSession(sessionId);
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::DoSetOutputL 
// Notify the policy session about a request to change output.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServer::DoSetOutputL (
	TInt aSessionId, 
	const RMessage2& aMessage)
	{
		
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::DoSetOutputL "));
	
	//Check for multimedia capability:
	RThread clientThread;
	aMessage.ClientL(clientThread);
	RProcess clientProcess;
	User::LeaveIfError(clientThread.Process(clientProcess));

	TProcessId clientProcessID(clientProcess.Id());

	
	TBool clientHasCapabilities = clientProcess.HasCapability(ECapabilityMultimediaDD, KSuppressPlatSecDiagnostic);

	if (!clientHasCapabilities)
		{
		TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::DoSetOutputL ERROR: Client failed Capability Check"));			
		aMessage.Complete(KErrPermissionDenied);
		return;
		}
	
	clientThread.Close();
	clientProcess.Close();
	
	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> xPackage;
	aMessage.ReadL( 0, xPackage);
	
    CTelephonyAudioRouting::TAudioOutput response = xPackage();
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::DoSetOutputL Audio Output Value to set on the sessions: %d"), response); 	
	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> yPackage; // space for previous output
	aMessage.ReadL( 1, yPackage);
	
	TPckgBuf<TInt> zPackage; // space for err
	aMessage.ReadL( 2, zPackage);		

	TPckgBuf<TBool> showNotePkg;
  	aMessage.ReadL(3, showNotePkg);
   	iShowNoteMode = showNotePkg();	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::DoSetOutputL ShowNoteMode Value Sent to server = %d"), iShowNoteMode);    	
   	
	iSetOutputRequests.AppendL(aSessionId);
	
	// Verify requested audio change exists in available outputs (unless is ENone or ENotActive)	
	if ( (response != CTelephonyAudioRouting::ENone) && (response != CTelephonyAudioRouting::ENotActive))
	{
		TBool found = IsAvailableOutput(response);
		if (!found)
		{
			TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::DoSetOutputL: ERROR, Requested output not in availableOutputs!"));	
			SetOutputCompleteL(response, KErrPermissionDenied);
			return;  
		}	
	}

    iSessionIter.SetToFirst();

	CTelephonyAudioRoutingServerSession* serverSession = static_cast<CTelephonyAudioRoutingServerSession*>(iSessionIter++);

	while (serverSession != NULL)
		{
		if(serverSession->SessionId() == iPolicySessionId)
			{
			serverSession->OutputChangeRequested(response);
			break;
			}
		serverSession = static_cast<CTelephonyAudioRoutingServerSession*>(iSessionIter++);
		}
    
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::IdentifyAndAllocateNextFreeSessionIdL
// Assign a unique session Id to a new session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTelephonyAudioRoutingServer::IdentifyAndAllocateNextFreeSessionIdL()
	{
	
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::IdentifyAndAllocateNextFreeSessionIdL "));

	// The aim of this method is to locate a session identifier which is not already
	// in use....
	TInt sessionId = 0;
	TInt errorOrIndex;
	
	// Only 256 (KMaxNumberOfSessions) are allowed
	const TInt numberOfUsedSessionIds = iSessionIdsInUse.Count();
	if (numberOfUsedSessionIds > KMaxNumberOfSessions)
		{
		User::LeaveIfError(KErrDied);
		}

	// Session Id's indexing begins at 1, not 0
	for (TInt count = 1; count < KMaxNumberOfSessions+1; count++)
	{
		sessionId = count;
		errorOrIndex = iSessionIdsInUse.FindInOrder(sessionId);

		// If sessionId=count not currently being used, assign it.
		// Save the session in the array of allocated ids. We use InsertInOrder
		// since it effectively allows a binary search when trying to find
		// free ids:
		if (errorOrIndex == KErrNotFound)
			{
			errorOrIndex = iSessionIdsInUse.InsertInOrder(sessionId);
			if (errorOrIndex < 0)  // Handle error 
				{
				User::LeaveIfError(KErrDied);
				}
			break;
			}

		// Prevent value returned by RArray<T>::FindInOrder from being propagated
		// to the client side in response to a RSessionBase::Connect() request.
		if ((errorOrIndex != KErrNotFound) && (errorOrIndex < 0))
		{
			User::LeaveIfError(KErrDied);
		}

	} // End for

	return sessionId;

	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::AddSession
// Add a new Session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServer::AddSession()
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::AddSession "));
	iSessionCount++;
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::RemoveSession
// Remove an existing session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServer::RemoveSession(
	TInt aSessionId)
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::RemoveSession"));
	FreeSessionId(aSessionId);
	iSessionCount--;
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::FreeSessionId
// Free the session Id or a delated session for re-use.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServer::FreeSessionId(
	TInt aSessionId)
	{
		
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::FreeSessionId: ID: %d"), aSessionId);

	const TInt indexOrError = iSessionIdsInUse.FindInOrder(aSessionId);

	if	(indexOrError >= 0)
		{
		// This session id can now be reused...
		iSessionIdsInUse.Remove(indexOrError);
		}
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::SetPolicySessionId
// Set the Id of the policy session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServer::SetPolicySessionId(
	TInt aSessionId)
	{
		iPolicySessionId = aSessionId;
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::AvailableOutputsChangedL
// Used by policy session to indicated to all other sessions that available outputs have changed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServer::AvailableOutputsChangedL(
	const TArray<CTelephonyAudioRouting::TAudioOutput>& aOutputs)
	{
		
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::AvailableOutputsChangedL "));
	
	iAvailableOutputs.Reset();
	TInt count = aOutputs.Count();
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t AvailableOutputsChangedL: Count = %d "),count);
	for(TInt i = 0; i < count; i++)
	{
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t AvailableOutputsChangedL: aOutputs[i] = %d "),aOutputs[i]);
		iAvailableOutputs.AppendL(aOutputs[i]);		
	}
			
	iSessionIter.SetToFirst();

	CTelephonyAudioRoutingServerSession* serverSession = static_cast<CTelephonyAudioRoutingServerSession*>(iSessionIter++);
	while (serverSession != NULL)
		{
		if(serverSession->SessionId() != iPolicySessionId)
			serverSession->AvailableOutputsChanged(aOutputs);
		serverSession = static_cast<CTelephonyAudioRoutingServerSession*>(iSessionIter++);
		}

	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::SetOutputComplete
// Used by policy session to indicate the session that requested a SetOutput() that
// the request is complete, all the other session get OutputChanged() notification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServer::SetOutputCompleteL( 
	CTelephonyAudioRouting::TAudioOutput aOutput,
	TInt aError)
	{
	
	CTelephonyAudioRouting::TAudioOutput output = aOutput;	
	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::SetOutputComplete called with: = %d"), output);
	
	TInt requestCount = iSetOutputRequests.Count();

	if(requestCount>0)
		{
	
		TInt sessionToAlert = iSetOutputRequests[0];
		iSessionIter.SetToFirst();
	
		CTelephonyAudioRoutingServerSession* serverSession = static_cast<CTelephonyAudioRoutingServerSession*>(iSessionIter++);
		while (serverSession != NULL)
			{
			if(serverSession->SessionId() != iPolicySessionId)
				{
									
				if (serverSession->SessionId() == sessionToAlert )
					{
					TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::SetOutputComplete: Calling serverSession::SetOutputComplete with showNote: %d"), iShowNoteMode);					
					serverSession->SetOutputComplete(output, aError, iShowNoteMode);					
					}
				else
					{
					if (aError == KErrNone)  
						{
						serverSession->OutputChanged(output, iShowNoteMode);	
						}
					
					}
				}
				
			serverSession = static_cast<CTelephonyAudioRoutingServerSession*>(iSessionIter++);
			}
		
		for(TInt i=0;i<iSetOutputRequests.Count()-1;i++)
			{
			iSetOutputRequests[i] = iSetOutputRequests[i+1];
			}
		iSetOutputRequests.Remove(iSetOutputRequests.Count()-1);
		}
	}
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::OutputChanged
// Used by policy session to notify all sessions that the output has changed (from the policy side)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServer::OutputChanged( 
	CTelephonyAudioRouting::TAudioOutput aOutput)
	{
	
	CTelephonyAudioRouting::TAudioOutput output = aOutput;	
	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::OutputChanged called with: = %d"), output);

	iSessionIter.SetToFirst();
	
	CTelephonyAudioRoutingServerSession* serverSession = static_cast<CTelephonyAudioRoutingServerSession*>(iSessionIter++);
	while (serverSession != NULL)
		{
		if(serverSession->SessionId() != iPolicySessionId)
			{
				// Always send ETrue if outputChanged initiated by Policy:
				iShowNoteMode = ETrue;
				serverSession->OutputChanged(output, iShowNoteMode);	
				
			}				
			serverSession = static_cast<CTelephonyAudioRoutingServerSession*>(iSessionIter++);
		}	
	}
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::IsAvailableOutput
// Method used to determine if requested audio output is one of the
// audio outputs in the available output array.
// -----------------------------------------------------------------------------
//
TBool CTelephonyAudioRoutingServer::IsAvailableOutput( 
	CTelephonyAudioRouting::TAudioOutput aOutput)
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::IsAvailableOutput "));
	
	TBool found = EFalse;
	CTelephonyAudioRouting::TAudioOutput arrayElement;
    TInt count = iAvailableOutputs.Count();

    for(TInt i=0;i<count;i++)
    {
    	arrayElement = iAvailableOutputs[i];
    	if (aOutput == arrayElement)
    	{
     		found = ETrue;
    		break;   		
    	}

    }

	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServer::IsAvailableOutput: Found? = %d "), found);
	return found;
		
	}
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::CurrentAudioOutput
// Accessor method returns iCurrentAudioOutput to caller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
CTelephonyAudioRouting::TAudioOutput& CTelephonyAudioRoutingServer::CurrentAudioOutput()
	{
		return iCurrentAudioOutput;
	}
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::ShowNoteMode
// Accessor method returns iShowNoteMode to caller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TBool& CTelephonyAudioRoutingServer::ShowNoteMode()
	{
		return iShowNoteMode;
	}
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::SetShowNoteMode
// Accessor method allows caller to set iShowNoteMode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CTelephonyAudioRoutingServer::SetShowNoteMode(TBool aShowNoteMode)
	{
		iShowNoteMode = aShowNoteMode;
	}
	
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::PreviousAudioOutput
// Accessor method returns iPreviousAudioOutput to caller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
CTelephonyAudioRouting::TAudioOutput& CTelephonyAudioRoutingServer::PreviousAudioOutput()
	{
		return iPreviousAudioOutput;
	}	
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServer::AvailableOutputs
// Accessor method returns iAvailableOutputs to caller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
RArray<CTelephonyAudioRouting::TAudioOutput>& CTelephonyAudioRoutingServer::AvailableOutputs()
	{
		return iAvailableOutputs;
	}					
			
	
//End of File
