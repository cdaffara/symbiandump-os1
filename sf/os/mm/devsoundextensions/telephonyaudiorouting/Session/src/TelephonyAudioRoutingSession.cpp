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
* Description:   This class is the main interface to the TelephonyAudioRoutingServer. 
*				 It implements the client-side session.
*
*/


                                                           
#include <e32math.h>    
#include <s32mem.h> 
#include "TelephonyAudioRoutingClientServer.h"          
#include "TelephonyAudioRoutingSession.h"      
#include "TelephonyAudioRoutingRequest.h"
#include "TelephonyAudioRoutingPolicyRequest.h"     

const TInt KReqSetOutput					= 0;
const TInt KReqNotifyIfOutputChanged		= 1;
const TInt KReqNotifyIfAvailOutputsChanged	= 2; 
const TInt KReqMaxSize 						= 3;    
                                       
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::RTelephonyAudioRoutingSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//                                                                                                 
EXPORT_C RTelephonyAudioRoutingSession::RTelephonyAudioRoutingSession()                                                         
	: RSessionBase(), iConnected(EFalse)                                                                    
    {                                                                                            
    // No implementation required  
                                                         
    }                                                                                            
                                                                                                 
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::ConnectL
// Connects a client to the  server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                                                                                                
EXPORT_C TInt RTelephonyAudioRoutingSession::ConnectL(
	CTelephonyAudioRouting& aAudioRouting,
	MTelephonyAudioRoutingObserver& aObserver)                                                               
    {
    	
    TELAUDRTNG_RDEBUG(_L("[TELAUDRTNG]\t TelephonyAudioRoutingSession::ConnectL "));
    
	TInt retry=2;
  	TInt err = KErrGeneral;
  	
 
	for (;;)
		{
		err=CreateSession(KTelAudRtngServName,TVersion(0,0,0));		
		if (err!=KErrNotFound && err!=KErrServerTerminated && err!=KErrPermissionDenied)
		break;
		
		if (--retry==0)
		break;
		} 
	
	if ( err == KErrNone )
		{
		// Create active object receive handlers and add it to scheduler
		TRAP(err, StartRequestHandlersL(aAudioRouting, aObserver));
		if ( err == KErrNone )
			{
			iConnected = ETrue;
			}
			
		SendReceive(ETelAudRtngServInitialize, TIpcArgs());

		}
	
	return err;                                                                                           
                                                                              
    }        
  
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::Close
// Closes connection to the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
EXPORT_C void RTelephonyAudioRoutingSession::Close()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingSession[%x]::Close "),this);		
	if ( iConnected )
		{
				
		for (TInt i = 0; i < KReqMaxSize; i++)
		{
			delete iRequests[i];
		}
		iRequests.Close();		
		iAvailableOutputs.Close();
		RSessionBase::Close();
		iConnected = EFalse;
		
		}
		
	}                                                                                       
                                                                                                 
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::Version
// Gets the client side version number.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                                                                                            
EXPORT_C TVersion RTelephonyAudioRoutingSession::Version() const                                                     
    {                                                                                            
    return(TVersion(0,0,0));                                               
    }   
           
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::GetDefaultValues
// Gets default values for current audio output, previous audio output, and 
// show note mode from the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                                                                                         
EXPORT_C void RTelephonyAudioRoutingSession::GetDefaultValuesL()                                        
    {
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingSession[%x]::GetDefaultValuesL "),this);	   
		
	User::LeaveIfError( SendReceive(ETelAudRtngServGetDefaultValues, TIpcArgs(&iDefaultValuePkg)) ); 
	
	// Set session attributes with values retrieved from Server:
	TTelephonyAudioRoutingDefaultParams defParams = iDefaultValuePkg();
	
	CTelephonyAudioRouting::TAudioOutput currentAudioOutput = defParams.iCurrentAudio;
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t GetDefaultValuesL: current audio output = %d "), currentAudioOutput);	
	iCurrentAudioOutputPkg() = currentAudioOutput;	 
	
	CTelephonyAudioRouting::TAudioOutput previousAudioOutput = defParams.iPreviousAudio;
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t GetDefaultValuesL: Previous audio output = %d "), previousAudioOutput);	
	iPreviousAudioOutputPkg() = previousAudioOutput;
	
	TBool showNoteMode = defParams.iShowNoteMode;
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t GetDefaultValuesL: showNoteMode = %d "), showNoteMode);	
	iShowNoteModePkg() = showNoteMode;	 		 	                           
    
 	} 
                                                                                                    
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::SetOutput
// Send a SetOutput request to server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                                                                                         
EXPORT_C void RTelephonyAudioRoutingSession::SetOutput(
	CTelephonyAudioRouting::TAudioOutput aOutput, TBool aShowNote)                                        
    {
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingSession[%x]::SetOutput "),this);	   

	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingSession::SetOutput: ShowNote = %d "), aShowNote);	   

	iRequests[KReqSetOutput]->Cancel();
                                                                          
	iCurrentAudioOutputPkg() = aOutput;  
	iPreviousAudioOutputPkg() = CTelephonyAudioRouting::ENone; 
	iErrPkg() = KErrNone;  	
	iShowNoteModePkg() = aShowNote;
	
  	iRequests[KReqSetOutput]->SetActive();	
	SendReceive(ETelAudRtngServDoSetOutput, TIpcArgs(&iCurrentAudioOutputPkg, &iPreviousAudioOutputPkg, &iErrPkg, &iShowNoteModePkg), iRequests[KReqSetOutput]->iStatus);  	                            
    
    } 

// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::GetAvailableOutputsL
// Get the available audio outputs from the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RTelephonyAudioRoutingSession::GetAvailableOutputsL()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingSession[%x]::GetAvailableOutputsL"),this);		
	iAvailableOutputs.Reset();

	TPckgBuf<TInt> numOfOutputs;
	User::LeaveIfError(SendReceive(ETelAudRtngServGetNoOutputs, TIpcArgs(&numOfOutputs)));
	HBufC8* buf = HBufC8::NewLC(numOfOutputs()*sizeof(CTelephonyAudioRouting::TAudioOutput));
	TPtr8 ptr = buf->Des();
	
	User::LeaveIfError(SendReceive(ETelAudRtngServGetAvailableOutputs, TIpcArgs(&ptr)));
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	for (TInt i=0; i<numOfOutputs(); i++)
		{
		TInt err = iAvailableOutputs.Append((CTelephonyAudioRouting::TAudioOutput)stream.ReadInt32L());
		if (err)
			{
			iAvailableOutputs.Reset();
			User::Leave(err);
			}
		}
	
	CleanupStack::PopAndDestroy(2, buf);//stream, buf
		
	}
	
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::NotifyIfOutputChanged
// Send a request to server to receive notification whenever current audio output changes.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RTelephonyAudioRoutingSession::NotifyIfOutputChanged()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingSession[%x]::NotifyIfOutputChanged"),this);	 

	iRequests[KReqNotifyIfOutputChanged]->Cancel();
	
	SendReceive(ETelAudRtngServNotifyIfOutputChanged, TIpcArgs(&iCurrentAudioOutputPkg, &iPreviousAudioOutputPkg, &iErrPkg, &iShowNoteModePkg), iRequests[KReqNotifyIfOutputChanged]->iStatus);
	iRequests[KReqNotifyIfOutputChanged]->SetActive();
	
	}

// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::NotifyIfAvailableOutputsChanged
// Send a request to server to receive notification whenever available audio output changes.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RTelephonyAudioRoutingSession::NotifyIfAvailableOutputsChanged()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingSession[%x]::NotifyIfAvailbleOutputsChanged"),this);	
	iRequests[KReqNotifyIfAvailOutputsChanged]->Cancel();
	     
	SendReceive(ETelAudRtngServNotifyIfAvailOutputsChanged, TIpcArgs(&iCurrentAudioOutputPkg), iRequests[KReqNotifyIfAvailOutputsChanged]->iStatus);
	iRequests[KReqNotifyIfAvailOutputsChanged]->SetActive();
	
	}
	
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::CancelRequest
// Cancel a previously sent request.
// aRequestOpcode = index into array as to which request object to cancel. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void RTelephonyAudioRoutingSession::CancelRequest(TTelAudRtngServRqst aRequestOpcode)
	{
	TPckgBuf<TInt> request;   
  	request() = aRequestOpcode;

	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingSession::CancelRequest Number: %d"), aRequestOpcode);
	SendReceive(ETelAudRtngServCancelRequest, TIpcArgs(&request));	
	
	}
	
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::CurrentAudioOutputPkg
// Accessor method returns iCurrentAudioOutputPkg to caller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C TPckgBuf<CTelephonyAudioRouting::TAudioOutput>& RTelephonyAudioRoutingSession::CurrentAudioOutputPkg()
	{
		return iCurrentAudioOutputPkg;
	}
	
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::PreviousAudioOutputPkg
// Accessor method returns iPreviousAudioOutputPkg to caller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C TPckgBuf<CTelephonyAudioRouting::TAudioOutput>& RTelephonyAudioRoutingSession::PreviousAudioOutputPkg()
	{
		return iPreviousAudioOutputPkg;
	}
	
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::ErrPkg
// Accessor method returns iErrPkg to caller. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C TPckgBuf<TInt>& RTelephonyAudioRoutingSession::ErrPkg()
	{
		return iErrPkg;
	}
	
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::AvailableOutputs
// Accessor method returns iAvailableOutputs to caller.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C RArray<CTelephonyAudioRouting::TAudioOutput>& RTelephonyAudioRoutingSession::AvailableOutputs()
	{
		return iAvailableOutputs;
	}					

// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::StartRequestHandlersL
// Start asynchronous request handlers.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RTelephonyAudioRoutingSession::StartRequestHandlersL(
	CTelephonyAudioRouting& aAudioRouting,	
	MTelephonyAudioRoutingObserver& aObserver )
	{
	
	CTelephonyAudioRoutingRequest* req = NULL;

	req = CTelephonyAudioRoutingRequest::NewL(*this, aObserver, aAudioRouting, ETelAudRtngServDoSetOutput);
	User::LeaveIfError( iRequests.Append(req) );

	req = CTelephonyAudioRoutingRequest::NewL(*this, aObserver, aAudioRouting, ETelAudRtngServNotifyIfOutputChanged);
	User::LeaveIfError( iRequests.Append(req) );

	req = CTelephonyAudioRoutingRequest::NewL(*this, aObserver, aAudioRouting, ETelAudRtngServNotifyIfAvailOutputsChanged);
	User::LeaveIfError( iRequests.Append(req) );

	}
	
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::SetShowNoteMode
// Sets session flag iOkToGetShowNoteMode to indicate that it is OK to retrieve value of
// iShowNoteMode from the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void RTelephonyAudioRoutingSession::SetShowNoteMode()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingSession[%x]::SetShowNoteMode"),this);	
	iOkToGetShowNoteMode = ETrue;
	}
	
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::ShowNoteMode
// Provide caller with value of iShowNoteModePkg from server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C TPckgBuf<TBool>& RTelephonyAudioRoutingSession::ShowNoteMode()
	{	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingSession[%x]::ShowNoteMode"),this);
	
	return iShowNoteModePkg;
	}
	
	
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::ResetShowNoteMode
// Resets the session flag iOkToGetShowNoteMode to indicate it's not ok to retrieve
// value of iSetShowNoteMode from server. (Client can only retrieve it once after a
// SetOutputComplete call). (But it can be set internally from an OutputChanged call).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void RTelephonyAudioRoutingSession::ResetShowNoteMode()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t RTelephonyAudioRoutingSession[%x]::ResetShowNoteMode"),this);	
	iOkToGetShowNoteMode = EFalse;
	}
// -----------------------------------------------------------------------------
// RTelephonyAudioRoutingSession::OkToGetShowNoteMode
// Provide caller with value of iOkToGetShowNoteMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool RTelephonyAudioRoutingSession::OkToGetShowNoteMode()
	{		
	return iOkToGetShowNoteMode;
	}	
// End of File
