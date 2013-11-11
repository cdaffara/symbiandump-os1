/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This class is the main interface to the TelephonyAudioRoutingServer.
*			   : Specifically handles TelephonyAudioRoutingManager sessions.
*
*/


                                                           
#include <e32math.h>    
#include <s32mem.h> 
#include "TelephonyAudioRoutingClientServer.h"          
#include "TelephonyAudioRoutingManagerSession.h"      
#include "TelephonyAudioRoutingPolicyRequest.h"     

                                                                                                                                                                      
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingManagerSession::RTelephonyAudioRoutingManagerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//                                                                                                 
EXPORT_C RTelephonyAudioRoutingManagerSession::RTelephonyAudioRoutingManagerSession()                                                         
	: RSessionBase(), iConnected(EFalse)                                                                    
    {                                                                                            
    // No implementation required  
                                                         
    }                                                                                            
                                                                                                    
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::Connect
// Connects a policy client to the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RTelephonyAudioRoutingManagerSession::Connect(
	CTelephonyAudioRoutingManager& aAudioRoutingManager,
	MTelephonyAudioRoutingPolicyObserver& aPolicyObserver)                                                               
    {
    	
    TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingManagerSession::Connect"));
    
	TInt retry=2;
  	TInt err = KErrGeneral;
  
	for (;;)
		{
		err=CreateSession(KTelAudRtngServName,TVersion(0,0,0)); // RSessionBase::CreateSession
		
		if (err!=KErrNotFound && err!=KErrServerTerminated && err!=KErrPermissionDenied)
		break;
		
		if (--retry==0)
		break;
		} 
	
	if ( err == KErrNone )
		{
		// Create active object receive handlers and add it to scheduler
		TRAP(err, StartPolicyRequestHandlersL(aAudioRoutingManager, aPolicyObserver));
		if ( err == KErrNone )
			{
			iConnected = ETrue;
			}

		SendReceive(ETelAudRtngServInitialize, TIpcArgs());

		}
	
	return err;                                                                                           
                                                                              
    }     
  
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingManagerSession::Close
// Closes connection to the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
EXPORT_C void RTelephonyAudioRoutingManagerSession::Close()
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingManagerSession::Close"));	
	if ( iConnected )
		{

		delete iPolicyRequest;
		RSessionBase::Close();
		iConnected = EFalse;
		
		
		}
		
	}                                                                                       
                                                                                                 
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingManagerSession::OutputChangeCompleted
// Used by policy to notify server that requested output change completed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RTelephonyAudioRoutingManagerSession::OutputChangeCompleted(
	CTelephonyAudioRouting::TAudioOutput aOutput,
	TInt aError)                                        
    { 
 	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> audioOutputPkg;
	audioOutputPkg() = aOutput;  
	TPckgBuf<TInt> errPkg;
	errPkg() = aError;
	 
 	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingManagerSession::OutputChangeCompleted to %d"), aOutput); 	                 
	SendReceive(ETelAudRtngServOutputChangeComplete, TIpcArgs(&audioOutputPkg, &errPkg));  	                                         	    
    }
    
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingManagerSession::OutputChanged
// Used by policy to notify server of audio output changes initiated by policy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RTelephonyAudioRoutingManagerSession::OutputChanged(
	CTelephonyAudioRouting::TAudioOutput aOutput)                                        
    { 
 	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> audioOutputPkg;    
	audioOutputPkg() = aOutput;  
//	iAudioOutput = aOutput;  
 	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingManagerSession::OutputChanged to %d"), aOutput); 	                 
	SendReceive(ETelAudRtngServOutputChangedByPolicy, TIpcArgs(&audioOutputPkg));  
	                                         	    
    }
    
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingManagerSession::AvailableOutputsChangedL
// Used by policy session to notify server if the available audio output change.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
EXPORT_C void RTelephonyAudioRoutingManagerSession::AvailableOutputsChangedL(
	const TArray<CTelephonyAudioRouting::TAudioOutput>& aOutputs)
    {
    TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingManagerSession::AvailableOutputsChangedL"));  	       
	TPckgBuf<TInt> numOfOutputs;
	numOfOutputs()= aOutputs.Count();
	
	TInt count = aOutputs.Count();
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t AvailableOutputsChangedL: Count = %d "),count);
	for(TInt i = 0; i < count; i++)
	{
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t AvailableOutputsChangedL: aOutputs[i] = %d "),aOutputs[i]);
	}	
	
	TInt KBufExpandSize8 = 8;//two TInts
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	
	for (TInt i=0;i<aOutputs.Count();i++)
		{
		stream.WriteInt16L(aOutputs[i]);		
		}
	
	TPtr8 ptr = dataCopyBuffer->Ptr(0);
	
	SendReceive(ETelAudRtngDoAvailableOutputsChanged, TIpcArgs(&numOfOutputs, &ptr));
	stream.Close();
	CleanupStack::PopAndDestroy(2);//stream, buf                    
       	    
    } 
    
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingManagerSession::SetPolicySessionIdL
// Sets policy session Id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RTelephonyAudioRoutingManagerSession::SetPolicySessionIdL()
	{
	User::LeaveIfError(SendReceive(ETelAudRtngServSetPolicySessionId, TIpcArgs()));
	}

// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingManagerSession::MonitorOutputChangeRequest
// Used by policy session to get notification whenever any of the clients sends
// a SetOutput request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RTelephonyAudioRoutingManagerSession::MonitorOutputChangeRequest()
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingManagerSession::MonitorOutputChangeRequest "));

	iPolicyRequest->Cancel();
	
	SendReceive(ETelAudRtngServMonitorOutputChange, TIpcArgs(&iAudioOutputPkg), iPolicyRequest->iStatus);
	iPolicyRequest->SetActive();
		
	}

// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingManagerSession::CancelRequest
// Cancel a previously send request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void RTelephonyAudioRoutingManagerSession::CancelRequest(TTelAudRtngServRqst aRequest)
	{
	TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingManagerSession::CancelRequest "));	
	TPckgBuf<TInt> request;
   	request() = aRequest;
	SendReceive(ETelAudRtngServCancelRequest, TIpcArgs(&request));
	}

// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::AudioOutputPkg
// Accessor method returns iAudioOutputPkg to caller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C TPckgBuf<CTelephonyAudioRouting::TAudioOutput>& RTelephonyAudioRoutingManagerSession::AudioOutputPkg()
	{
		return iAudioOutputPkg;
	}

// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingManagerSession::StartPolicyRequestHandlersL
// Start asynchronous request handlers.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RTelephonyAudioRoutingManagerSession::StartPolicyRequestHandlersL( 
	CTelephonyAudioRoutingManager& aAudioRoutingManager, 
	MTelephonyAudioRoutingPolicyObserver& aPolicyObserver )
	{
		iPolicyRequest = CTelephonyAudioRoutingPolicyRequest::NewL(*this, aPolicyObserver, aAudioRoutingManager, ETelAudRtngServMonitorOutputChange);
	}
