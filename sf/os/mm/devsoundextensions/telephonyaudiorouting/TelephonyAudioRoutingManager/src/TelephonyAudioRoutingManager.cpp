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
* Description:   Methods for CTelephonyAudioRoutingManager class
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <e32std.h>

#include "TelephonyAudioRoutingManager.h"
#include "TelephonyAudioRoutingClientServer.h"
#include "TelephonyAudioRoutingServer.h"
#include "TelephonyAudioRoutingManagerSession.h"

const TUid KTelAudRtngServerUid = {0x10207A93};

const TInt KTelAudRtngServerStackSize=0x2000;			//  8KB
const TInt KTelAudRtngServerInitHeapSize=0x1000;		//  4KB
const TInt KTelAudRtngServerMaxHeapSize=0x1000000;		// 16MB
    
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CTelephonyAudioRoutingManager::CTelephonyAudioRoutingManager(
	MTelephonyAudioRoutingPolicyObserver& aObserver)
	: iObserver(aObserver)
    {
    }
    
    
// -----------------------------------------------------------------------------
// CAudioOutputProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingManager::ConstructL(
const TArray<CTelephonyAudioRouting::TAudioOutput>& aArray)
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingManager::ConstructL "));

	User::LeaveIfError(StartServer());
	iMgrSession = new (ELeave)RTelephonyAudioRoutingManagerSession;
	User::LeaveIfError(iMgrSession->Connect(*this, iObserver));
	
	iMgrSession->SetPolicySessionIdL();	
	iMgrSession->MonitorOutputChangeRequest();
	iMgrSession->AvailableOutputsChangedL(aArray); 
	
	}

// Two-phased constructor.
EXPORT_C CTelephonyAudioRoutingManager* CTelephonyAudioRoutingManager::NewL(
MTelephonyAudioRoutingPolicyObserver& aObserver,
const TArray<CTelephonyAudioRouting::TAudioOutput>& aArray)
    {
	CTelephonyAudioRoutingManager* self = new(ELeave) CTelephonyAudioRoutingManager(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aArray);
	CleanupStack::Pop(self);
	return self;
    }


// Destructor
CTelephonyAudioRoutingManager::~CTelephonyAudioRoutingManager()
    { 
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingManager::~CTelephonyAudioRoutingManager "));    
    if (iMgrSession) 
        {
        iMgrSession->Close();        
        }
    delete iMgrSession;
    iServer.Close();
    }

// ---------------------------------------------------------
// CAudioOutput::OutputChanged
// Policy is sending out notification that the output has changed 
// (a change initiated by policy)
// (other items were commented in a header).
// ---------------------------------------------------------
//

EXPORT_C void CTelephonyAudioRoutingManager::OutputChanged(CTelephonyAudioRouting::TAudioOutput aOutput)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingManager::OutputChanged to: %d"), aOutput);	
    iMgrSession->OutputChanged(aOutput);
	}
	
// ---------------------------------------------------------
// CAudioOutput::AvailableOutputsChanged
// Policy is sending out notification that the available outputs have changed
// (other items were commented in a header).
// ---------------------------------------------------------
//
	
EXPORT_C void CTelephonyAudioRoutingManager::AvailableOutputsChanged(const TArray<CTelephonyAudioRouting::TAudioOutput>& aArray)
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingManager::AvailableOutputsChanged "));
    TRAPD(err,iMgrSession->AvailableOutputsChangedL(aArray));
   	if (err != KErrNone)
		{
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingManager::AvailableOutputsChanged: ERROR from Adaptation: %d "), err);			
		}		
	}
	
// ---------------------------------------------------------
// CAudioOutput::OutputChangeRequestCompleted
// Policy is sending out notification that the request change in output has been completed
// (other items were commented in a header).
// ---------------------------------------------------------
//	
EXPORT_C void CTelephonyAudioRoutingManager::OutputChangeRequestCompleted(CTelephonyAudioRouting::TAudioOutput aOutput, TInt aError)
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingManager::OutputChangeRequestCompleted "));	
	if (aError != KErrNone)
	{
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingManager::OutputChangeRequestCompleted: ERROR from Adaptation: %d "), aError);			
	}

    iMgrSession->OutputChangeCompleted(aOutput, aError);
	}
	
		
// -----------------------------------------------------------------------------
// CStartTelephonyAudioRoutingServer::StartServer
// Start TelephonyAudioRouting Server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CTelephonyAudioRoutingManager::StartServer()                                                                        
    {       
    TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingManager::StartServer "));
    	
	const TUidType serverUid(KNullUid,KNullUid,KTelAudRtngServerUid);
		
	TThreadFunction serverFunc=CTelephonyAudioRoutingServer::StartThreadL;
	
	TInt err(KErrNone);
	TName name(KTelAudRtngServName);
			
	err = iServer.Create(name, serverFunc, KTelAudRtngServerStackSize,
	        	KTelAudRtngServerInitHeapSize, KTelAudRtngServerMaxHeapSize,
	        	/*&start*/ NULL, EOwnerProcess);
				

	if(err != KErrNone)
		return err;
	// Synchronise with the server
	TRequestStatus reqStatus;
	iServer.Rendezvous(reqStatus);
	
	if (reqStatus!=KRequestPending)
		{
		iServer.Kill(0);
		}
	else
		{
		// Start the test harness
		iServer.Resume();
		// Server will call the reciprocal static synchronise call	
		}

	User::WaitForRequest(reqStatus); // wait for start or death
	if(reqStatus.Int() != KErrNone)
		{
		iServer.Close();
		return reqStatus.Int();
		}
		
	return KErrNone;
   	                                                                    
    }    

// End of File
