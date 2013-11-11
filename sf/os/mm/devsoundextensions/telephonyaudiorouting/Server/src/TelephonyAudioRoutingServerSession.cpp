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
* Description:   Server-side session implementation
*
*/



#include <e32std.h>
#include <e32svr.h>
#include <s32mem.h>  // for stream classes
#include "TelephonyAudioRoutingServerSession.h"

const TInt KMaxNumberOfOutputs = 100; 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::CTelephonyAudioRoutingServerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTelephonyAudioRoutingServerSession::CTelephonyAudioRoutingServerSession(
	TInt aSessionId)
    :iSessionId(aSessionId), 
    iMessageArray()
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::CTelephonyAudioRoutingServerSession "),this);
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t iMessageArrayCount = %d "),iMessageArray.Count());	
		
	}

// Destructor
CTelephonyAudioRoutingServerSession::~CTelephonyAudioRoutingServerSession()
	{
	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::~CTelephonyAudioRoutingServerSession "),this);

    if	(&Server() != NULL)
		{
		Server().RemoveSession(iSessionId);
		}
		
	iSessionAvailableOutputs.Close();
	iMessageArray.Close();

	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::Server
// Creates the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CTelephonyAudioRoutingServer& CTelephonyAudioRoutingServerSession::Server()
	{
	return *static_cast<CTelephonyAudioRoutingServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::ServiceError
// Handle an error from ServiceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::ServiceError(
	const RMessage2& aMessage,
	TInt aError)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::ServiceError - %d "), aError);
	PanicClient (aMessage, EPanicIllegalFunction); 
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::ServiceL
// Handles the servicing of a client request that has been passed to the server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::ServiceL (
	const RMessage2& aMessage)
	{
		    
    switch (aMessage.Function())
		{
		case ETelAudRtngServDoSetOutput:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServDoSetOutput "),this);
			DoSetOutputL(aMessage);
			break;
			
		case ETelAudRtngServNotifyIfOutputChanged:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServNotifyIfOutputChanged "),this);
			NotifyIfOutputChangedL(aMessage);
			break;
			
		case ETelAudRtngServNotifyIfAvailOutputsChanged:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServNotifyIfAvailOutputsChanged "),this);
			NotifyIfAvailOutputsChangedL(aMessage);
			break;
			
		case ETelAudRtngServGetNoOutputs:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServGetNoOutputs "),this);
			GetNoOfAvailableOutputs(aMessage);
			break;
			
		case ETelAudRtngServGetAvailableOutputs:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServGetAvailableOutputs "),this);
			GetAvailableOutputsL(aMessage);
			break;
			
		case ETelAudRtngServMonitorOutputChange:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServMonitorOutputChange "),this);
			MonitorOutputChangeL(aMessage);
			break;
			
		case ETelAudRtngServSetPolicySessionId:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServSetPolicySessionId "),this);
			SetPolicySessionId(aMessage);
			break;
			
		case ETelAudRtngServOutputChangeComplete:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServOutputChangeComplete "),this);
			OutputChangeCompleteL(aMessage);
			break;
			
		case ETelAudRtngDoAvailableOutputsChanged:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngDoAvailableOutputsChanged "),this);
			DoAvailableOutputsChangedL(aMessage);
			break;
			
		case ETelAudRtngServCancelRequest:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServCancelRequest "),this);
			CancelRequestL(aMessage);
			break;

		case ETelAudRtngServInitialize:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServInitialize "),this);
			InitializeL(aMessage);
			break;
			
		case ETelAudRtngServOutputChangedByPolicy:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServOutputChangedByPolicy "),this);
			OutputChangedL(aMessage);
			break;
			
		case ETelAudRtngServGetDefaultValues:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - ETelAudRtngServGetDefaultValues "),this);
			GetDefaultValuesL(aMessage);
			break;
						
		default:
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL - default !! "),this);
			PanicClient (aMessage, EPanicIllegalFunction);
		}
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::ServiceL EXIT"), this);		
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::InitializeL
// Session initialization.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::InitializeL(
	const RMessage2& aMessage)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::IntitializeL enter"),this);	
	iConnected = ETrue;
	Server().AddSession();
	aMessage.Complete(KErrNone);
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::IntitializeL exit"),this);
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::CancelRequestL
// Cancel the outstanding request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::CancelRequestL(
	const RMessage2& aMessage)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::CancelRequestL"),this);	
	TTelAudRtngServRqst req;
	req = (TTelAudRtngServRqst) aMessage.Int1();
	TPckgBuf<TInt> count;
	aMessage.ReadL( 0, count);
	req = (TTelAudRtngServRqst)count();
	aMessage.Complete(KErrNone);
	TInt messageCount = iMessageArray.Count();
	TInt messageIndex = 0;
	
	if(messageCount > 0)
		{
		for(TInt i=0;i< messageCount; i++)
			{
			if(iMessageArray[i].Function()== req)
				{
				messageIndex=i;
				break;
				}
			
			}
		iMessageArray[messageIndex].Complete(KErrCancel);
		iMessageArray.Remove(messageIndex);
		}
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::DoSetOutputL 
// Send a request to audio policy to set audio output.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::DoSetOutputL (
	const RMessage2& aMessage)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::DoSetOutputL "),this);
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t Appending aMessage to iMessageArray. Current count = %d "),iMessageArray.Count());	
	iMessageArray.AppendL(aMessage);
	Server().DoSetOutputL(iSessionId,aMessage);
    }

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::NotifyIfOutputChangedL 
// Add a message to message queue. Complete the message when output changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::NotifyIfOutputChangedL (
	const RMessage2& aMessage)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::NotifyIfOutputChangedL "),this);
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t Appending aMessage to iMessageArray. Current count = %d "),iMessageArray.Count());		
    iMessageArray.AppendL(aMessage);
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::NotifyIfAvailOutputsChangedL
// Add a message to message queue. Complete the message when available audio 
// outputs change
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::NotifyIfAvailOutputsChangedL (
	const RMessage2& aMessage)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::NotifyIfAvailOutputsChangedL "),this);
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t Appending aMessage to iMessageArray. Current count = %d "),iMessageArray.Count());			
    iMessageArray.AppendL(aMessage);
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::SetPolicySessionId
// Set the policy session Id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::SetPolicySessionId(
	const RMessage2& aMessage)
	{
	if(!iPolicyFlag)
		{
		Server().SetPolicySessionId(iSessionId);
		iPolicyFlag = ETrue;
		}

	aMessage.Complete(KErrNone);
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::OutputChangeCompleteL
// Used by policy session to indicate that the SetOutput request is complete.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::OutputChangeCompleteL(
	const RMessage2& aMessage)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::OutputChangeCompleteL "),this);
	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> xPackage;
	aMessage.ReadL( 0, xPackage);		
   	CTelephonyAudioRouting::TAudioOutput audioOutput = xPackage();
   	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::OutputChangeComplete: AudioOutput = %d"), audioOutput);      	
	   	
 	TPckgBuf<TInt> errPkg;
  	aMessage.ReadL(1, errPkg);
   	TInt err = errPkg();	

  	if (Server().CurrentAudioOutput() != CTelephonyAudioRouting::ENone)
	{
		Server().PreviousAudioOutput() = Server().CurrentAudioOutput();
		
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::OutputChangeComplete: iPreviousAudioOutput set to: %d"), Server().PreviousAudioOutput());	
	}
	
	Server().CurrentAudioOutput() = audioOutput; 	
   	   	
   	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::OutputChangeCompleteL: Server will be sent audioOutput = %d"), audioOutput);
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::OutputChangeCompleteL: previousAudioOutput = %d"), Server().PreviousAudioOutput());   	
	aMessage.Complete(KErrNone);
	Server().SetOutputCompleteL(audioOutput, err); 		
	}
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::MonitorOutputChangeL
// Used by policy session to listen to any SetOutput request from other sessions.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::MonitorOutputChangeL (
	const RMessage2& aMessage)
	{
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::MonitorOutputChangeL "),this);
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t Appending aMessage to iMessageArray. Current count = %d "),iMessageArray.Count());
			
		TRAPD(err, iMessageArray.AppendL(aMessage));
		if (err != KErrNone)
		{
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t MonitorOutputChangeL ERROR: %d "),err);	
			PanicClient (aMessage, EPanicIllegalFunction); 	
		}	
	}
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::GetNoOfAvailableOutputs
// Returns the number of available outputs to client-side session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::GetNoOfAvailableOutputs (
	const RMessage2& aMessage)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::GetNoOfAvailableOutputs"),this);
	TPckgBuf<TInt> xPackage;
	xPackage() = (Server().AvailableOutputs()).Count();

	TInt res = aMessage.Write(0, xPackage);	
    if (res != KErrNone)
    	{
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t GetNoOfAvailableOutputs WRITE ERROR: %d"),res);
    	PanicClient(aMessage, EPanicIllegalFunction);
    	}
       
    else   
 		aMessage.Complete(KErrNone);

	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::GetAvailableOutputsL
// Returns the available audio outputs to client-side session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::GetAvailableOutputsL (
	const RMessage2& aMessage)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::GetAvailableOutputsL "),this);
	TInt KBufExpandSize8 = 8;//two TInts
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	
	// Write out server's available outputs to caller:
	CTelephonyAudioRouting::TAudioOutput output;
	TInt count = (Server().AvailableOutputs()).Count();
	for (TInt i=0;i<count;i++)
		{
		output = ( Server().AvailableOutputs() )[i];
		stream.WriteInt32L(output);
		}
	aMessage.WriteL(0, dataCopyBuffer->Ptr(0));
	stream.Close();
	CleanupStack::PopAndDestroy(2); // dataCopyBuffer, stream

	aMessage.Complete(KErrNone);
	
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::DoAvailableOutputsChangedL
// Notify the client-side session that available outputs have changed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::DoAvailableOutputsChangedL (
	const RMessage2& aMessage)
{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::DoAvailableOutputsChangedL"),this);
	TInt err(KErrNone);
	TBool validData(ETrue);
	TPckgBuf<TInt> count;
	TRAP(err,aMessage.ReadL( 0, count));
	if (err != KErrNone)
	{
		TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t DoAvailableOutputsChangedL(1) ERROR: %d "),err);	
		PanicClient (aMessage, EPanicIllegalFunction); 
		return;		
	}
	
	if ((count() <0) || (count() > KMaxNumberOfOutputs))
	{	
		validData = EFalse;	
		iSessionAvailableOutputs.Reset();		
	}
		
	if (validData)
	{
		HBufC8* buf = HBufC8::NewLC(count()*sizeof(CTelephonyAudioRouting::TAudioOutput));	
		TPtr8 ptr = buf->Des();

		TRAP(err,aMessage.ReadL(1, ptr ));
		if (err != KErrNone)
		{
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t DoAvailableOutputsChangedL(2) ERROR: %d "),err);	
			PanicClient (aMessage, EPanicIllegalFunction); 
			return;		
		}
		
		RDesReadStream stream(ptr);
		CleanupClosePushL(stream);
		iSessionAvailableOutputs.Reset();
		TInt element(0);
			
		for (TInt i=0; i<count(); i++)
		{
	   
	   		TRAP(err, element = stream.ReadInt16L());
			if (err != KErrNone)
			{
				TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t DoAvailableOutputsChangedL(3) ERROR: %d "),err);	
				PanicClient (aMessage, EPanicIllegalFunction); 
				return;	
			}	   	
			
			err = iSessionAvailableOutputs.Append((CTelephonyAudioRouting::TAudioOutput)element);
			if (err)
			{
				TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t DoAvailableOutputsChangedL(4) ERROR: %d "),err);			
				iSessionAvailableOutputs.Reset();
				PanicClient (aMessage, EPanicIllegalFunction); 
				return;
			}
			
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t DoAvailableOutputsChanged: Avail Outputs to be sent to server: iSessionAvailableOutputs[i] = %d "),iSessionAvailableOutputs[i]);			
		} // end For
				
		stream.Close();
		CleanupStack::PopAndDestroy(2); // buf, stream
				
	} // end if
	
	aMessage.Complete(KErrNone);
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t [%x]DoAvailableOutputsChangedL: calling Server::AvailableOutputsChanged"),this);	
	Server().AvailableOutputsChangedL(iSessionAvailableOutputs.Array());	
}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::SetOutputComplete
// Notifies the client-side session that the SetOutput request is complete.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::SetOutputComplete(
	CTelephonyAudioRouting::TAudioOutput aOutput,
	TInt aError,
	TBool aShowNote)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::SetOutputComplete "),this);
	
	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> xPackage;
	xPackage() = aOutput;
	
	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::SetOutputComplete: iPreviousAudioOutput Write out: %d"), Server().PreviousAudioOutput());		
		
	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> yPackage;
	yPackage() = Server().PreviousAudioOutput();	
	
	TPckgBuf<TInt> errorPackage;
	errorPackage() = aError;
	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::SetOutputComplete: ShowNote Val to pass to client: %d"), aShowNote);	
	TPckgBuf<TBool> showNotePackage;
	showNotePackage() = aShowNote;	
	
	TInt messageCount = iMessageArray.Count();
	TInt messageIndex = -1;
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t iMessageArrayCount = %d "),iMessageArray.Count());	
	
	for(TInt i=0;i< messageCount; i++)
		{
		if(iMessageArray[i].Function()== ETelAudRtngServDoSetOutput)
			{
				messageIndex = i;
				break;
			}
		}		

	if (messageIndex != -1)
		{
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t Current Index = %d "), messageIndex);	
		
			TInt res = iMessageArray[messageIndex].Write(0, xPackage);
			TInt res2 = iMessageArray[messageIndex].Write(1, yPackage);
			TInt res3 = iMessageArray[messageIndex].Write(2, errorPackage);	
			TInt res4 = iMessageArray[messageIndex].Write(3, showNotePackage);								
	
			if ( (res != KErrNone) || (res2 != KErrNone) || (res3 != KErrNone) || (res4 != KErrNone))
    			{	
    				PanicClient(iMessageArray[messageIndex], EPanicIllegalFunction);
    			}
    		else
    		{
    			iMessageArray[messageIndex].Complete(ETelAudRtngServSetOutputComplete);
    			iMessageArray.Remove(messageIndex);
    		}
    	}
	}
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::OutputChangeRequested
// Notification that client is requesting to change audio output
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::OutputChangeRequested(
	CTelephonyAudioRouting::TAudioOutput aOutput)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::OutputChangeRequested"),this);
		
	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> xPackage;
	xPackage() = aOutput;
	
	TInt messageCount = iMessageArray.Count();
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t iMessageArrayCount = %d "),iMessageArray.Count());	
	
	TInt messageIndex = -1;
	
	for(TInt i=0;i< messageCount; i++)
		{
		if(iMessageArray[i].Function()== ETelAudRtngServNotifyIfOutputChanged && !iPolicyFlag)
			{
				messageIndex=i;
				break;
			}
			
		if(iMessageArray[i].Function()== ETelAudRtngServMonitorOutputChange && iPolicyFlag)
			{
				messageIndex=i;
				break;
			}
		}
		
	if (messageIndex != -1) // Don't perform if message not found 		
		{
			TInt res = iMessageArray[messageIndex].Write(0, xPackage);	
			if (res != KErrNone)			
    			{
     				PanicClient(iMessageArray[messageIndex], EPanicIllegalFunction);
    			}
    	
    		else
    		{
				iMessageArray[messageIndex].Complete(ETelAudRtngServOutputChangeRequested);
    			iMessageArray.Remove(messageIndex);  
    		}
		}
#ifdef  _DEBUG		
	else
		{
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::OutputChangeRequested: ServerSession NOT Handling this msg! "),this);		
		}
#endif
		
	}

// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::OutputChangedL
// Telephony Audio Routing Manager uses to notify TAR that policy has initiated an output change.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::OutputChangedL(
	const RMessage2& aMessage)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::OutputChangedL Enter"),this);
		
	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> xPackage;

	aMessage.ReadL( 0, xPackage);
   	CTelephonyAudioRouting::TAudioOutput audioOutput = xPackage();
   		
	Server().PreviousAudioOutput() = Server().CurrentAudioOutput();
		
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::OutputChangedL: iCurrentAudioOutput was: %d"), Server().CurrentAudioOutput());		
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::OutputChangedL: iPreviousAudioOutput set to: %d"), Server().PreviousAudioOutput());	
		
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::OutputChangedL: iCurrentAudioOutput set to: %d"), audioOutput);	
	Server().CurrentAudioOutput() = audioOutput; 
	Server().SetShowNoteMode(ETrue);  // Always set if output changes initiated by adaptation
	
    aMessage.Complete(KErrNone);
    
  	// Server needs to send notification of output change to clients
	Server().OutputChanged(audioOutput); 
	  	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::OutputChangedL Exit"),this);    	
	
	}
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::OutputChanged
// Notification to be sent to all clients about the change of audio output.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::OutputChanged(
	CTelephonyAudioRouting::TAudioOutput aOutput,
	TBool aShowNote)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::OutputChanged"), this);

	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> xPackage;
	TPckgBuf<CTelephonyAudioRouting::TAudioOutput> yPackage;
	
	TPckgBuf<TInt> errorPackage;
	errorPackage() = KErrNone;
	
	TPckgBuf<TBool> zPackage;
				
	xPackage() = aOutput;
	
	// Provide client with previous output value:
	CTelephonyAudioRouting::TAudioOutput prevOutput = Server().PreviousAudioOutput();	
	yPackage() = prevOutput;
	
	// Provide client with showNote:	
	zPackage() = aShowNote;	
	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::OutputChanged Current output: %d"), aOutput);
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::OutputChanged Previous output: %d"), yPackage());
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession::OutputChanged aShowNote: %d"), zPackage());		
		
	TInt messageCount = iMessageArray.Count();
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t iMessageArrayCount = %d "),iMessageArray.Count());	
	
	TInt messageIndex = -1;
		
	for(TInt i=0;i< messageCount; i++)
		{
		if(iMessageArray[i].Function()== ETelAudRtngServNotifyIfOutputChanged)
			{
				messageIndex = i;
				break;
			}
		}		

	if (messageIndex != -1)
	{
		TInt res = iMessageArray[messageIndex].Write(0, xPackage);
		TInt res2 = iMessageArray[messageIndex].Write(1, yPackage);
		TInt res3 = iMessageArray[messageIndex].Write(2, errorPackage);	
		TInt res4 = iMessageArray[messageIndex].Write(3, zPackage);			
	
		if ( (res != KErrNone) || (res2 != KErrNone) || (res3 != KErrNone) || (res3 != KErrNone) )
    	{
    		PanicClient(iMessageArray[messageIndex], EPanicIllegalFunction);
    	}

		else
		{
    		iMessageArray[messageIndex].Complete(ETelAudRtngServOutputChanged);
    		iMessageArray.Remove(messageIndex);
		}
	}
#ifdef _DEBUG				
	else
		{
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::OutputChanged: ServerSession NOT Handling this msg! "),this);		
		}
#endif		

	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::OutputChanged Exit"), this);    				
	}	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::AvailableOutputsChanged
// Notifies the client-side session that available audio outputs have changed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::AvailableOutputsChanged( 
	const TArray<CTelephonyAudioRouting::TAudioOutput>& aOutputs)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::AvailableOutputsChanged Enter "),this);	 
    TInt count = aOutputs.Count();
    iSessionAvailableOutputs.Reset();
    
    for(TInt i=0;i<count;i++)
    	iSessionAvailableOutputs.Append(aOutputs[i]);
    	
	TInt messageCount = iMessageArray.Count();
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t iMessageArrayCount = %d "),iMessageArray.Count());	
	
	TInt messageIndex = -1;
	
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::AvailableOutputsChanged: Find msg "),this);	
	
	for(TInt i=0;i< messageCount; i++)
		{
		if(iMessageArray[i].Function()== ETelAudRtngServNotifyIfAvailOutputsChanged)
			{
				messageIndex=i;
				break;
			}
		}
		
	if (messageIndex != -1) // Don't perform if message not found
		{
			iMessageArray[messageIndex].Complete(ETelAudRtngServAvailableOutputsChanged);
    		iMessageArray.Remove(messageIndex);		
		}
		
#ifdef _DEBUG		
	else
		{
			TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::AvailableOutputsChanged: ServerSession NOT Handling this msg! "),this);		
		}
#endif

	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::AvailableOutputsChanged Exit "),this);
			
	}

TInt CTelephonyAudioRoutingServerSession::SessionId()
	{
	return iSessionId;
	}
	
// -----------------------------------------------------------------------------
// CTelephonyAudioRoutingServerSession::GetDefaultValuesL
// Gets default values from server for the client-side session 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CTelephonyAudioRoutingServerSession::GetDefaultValuesL(
	const RMessage2& aMessage)
	{
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t CTelephonyAudioRoutingServerSession[%x]::GetDefaultValuesL "),this);		
		
	TTelephonyAudioRoutingDefaultParams defaultParams;
	
	// Fill class with default values from server:
	defaultParams.iCurrentAudio = Server().CurrentAudioOutput();
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t GetDefaultValuesL: Current audio output = %d"),defaultParams.iCurrentAudio);	
	
	defaultParams.iPreviousAudio = Server().PreviousAudioOutput();
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t GetDefaultValuesL: Previous audio output = %d"),defaultParams.iPreviousAudio);
		
	defaultParams.iShowNoteMode = Server().ShowNoteMode();
	TELAUDRTNG_RDEBUG1(_L("[TELAUDRTNG]\t GetDefaultValuesL: Show Note Mode = %d"),defaultParams.iShowNoteMode);
	
	// Package default params and write out:
	TPckgBuf<TTelephonyAudioRoutingDefaultParams> defaultParamsPkg;
	defaultParamsPkg() = defaultParams;	
	aMessage.WriteL(0, defaultParamsPkg);
	aMessage.Complete(KErrNone);
	
	}
	
	
// End of file.
