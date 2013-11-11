// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32mem.h>
#include "mmfaudioclientserver.h"
#include "mmfaudioserver.h"
#include "mmfdevsoundserver.h"
#include "mmfdevsoundsession.h"
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>
#include <mm/mmpluginutils.h>
#ifdef _DEBUG
#include "e32debug.h"

#define SYMBIAN_DEBPRN0(str)                RDebug::Print(str, this)
#define SYMBIAN_DEBPRN1(str, val1)          RDebug::Print(str, this, val1)
#define SYMBIAN_DEBPRN2(str, val1, val2)    RDebug::Print(str, this, val1, val2)
#define SYMBIAN_DEBPRN3(str, val1, val2, val3)    RDebug::Print(str, this, val1, val2, val3)
#else
#define SYMBIAN_DEBPRN0(str)
#define SYMBIAN_DEBPRN1(str, val1)
#define SYMBIAN_DEBPRN2(str, val1, val2)
#define SYMBIAN_DEBPRN3(str, val1, val2, val3)
#endif //_DEBUG

const TInt KMaxQueueRequest = 6;

//	MEMBER FUNCTIONS 

TMMFDevSoundRequest::TMMFDevSoundRequest() 
	: iMessageCompleted(EFalse), 
	iRequestType(EUndefinedType),
	iCallBackPF(KCallbackNone)
	{
	}

TMMFDevSoundRequest::TMMFDevSoundRequest(TInt aIsCallBack)
	: iMessageCompleted(EFalse), 
	iRequestType(ECallBackType),
	iCallBackPF(aIsCallBack)
	{
	}

TMMFDevSoundRequest::TMMFDevSoundRequest(const TMMFDevSoundRequest& aRequest)
	: iMessageCompleted(EFalse), 
	iMessage(aRequest.iMessage),
	iCallBackPF(aRequest.iCallBackPF)
	{
	iRequestType = ResolveType();
	}

TBool TMMFDevSoundRequest::operator==(const TMMFDevSoundRequest& aRequest) const
	{
	TBool retval = EFalse;
	if ( aRequest.Function() == Function() )
		{
		retval = ETrue;
		}
	else
		{
		retval = EFalse;
		}
	return retval;
	}

const RMmfIpcMessage& TMMFDevSoundRequest::Message() 
	{ 
	return iMessage; 
	}

void TMMFDevSoundRequest::SetMessage(const RMmfIpcMessage& aMessage) 
	{
	iMessageCompleted = EFalse;
	iMessage = aMessage;
	iRequestType = ResolveType();
	}
	
void TMMFDevSoundRequest::SetMessageCallback() 
	{
	iMessageCompleted = EFalse;
	iRequestType = ECallBackType;
	}	

TInt TMMFDevSoundRequest::IsCallBack() const
	{
	return iCallBackPF;	
	}

TMMFDevSoundRequest::TA3FDevSoundRequestType TMMFDevSoundRequest::ResolveType()
	{
	TA3FDevSoundRequestType type = EUndefinedType;
	switch(iMessage.Function())
		{
		case EMMFDevSoundProxyInitialize1:
		case EMMFDevSoundProxyInitialize2:
		case EMMFDevSoundProxyInitialize4:
		case EMMFDevSoundProxyPlayInit:
		case EMMFDevSoundProxyRecordInit:
		case EMMFDevSoundProxyPlayTone:
		case EMMFDevSoundProxyPlayDualTone:
		case EMMFDevSoundProxyPlayDTMFString:
		case EMMFDevSoundProxyPlayToneSequence:
		case EMMFDevSoundProxyPlayFixedSequence:
			type = EAction_PseudoAsynchronous;
			break;

		case EMMFDevSoundProxyStop:
		case EMMFDevSoundProxyPause:
		case EMMFDevSoundProxyClose:
		case EMMFDevSoundProxyCancelInitialize:
		case EMMFDevSoundProxyResume:
		case EMMFDevSoundProxyEmptyBuffers:
			type = EAction_Asynchronous;
			break;

		case EMMFDevSoundProxySetConfig:
		case EMMFDevSoundProxyPostOpen: // Although this is not a configure operation technically, it has same calling pattern.
		case EMMFDevSoundProxySetVolume:
		case EMMFDevSoundProxySetGain:
		case EMMFDevSoundProxySetPlayBalance:
		case EMMFDevSoundProxySetRecordBalance:
		case EMMFDevSoundProxySetVolumeRamp:
		case EMMFDevSoundProxySetPrioritySettings:
			type = EConfigure_Asynchronous;
			break;
			
		case EMMFDevSoundProxySetDTMFLengths:
		case EMMFDevSoundProxySetToneRepeats:
			type = EConfigure_Synchronous;	
			break;
		case EMMFDevSoundProxyCapabilities:
			type = EQuery_Asynchronous;
			break;
		case EMMFDevSoundProxyMaxVolume:
		case EMMFDevSoundProxyMaxGain:
		case EMMFDevSoundProxyConfig:
		case EMMFDevSoundProxyVolume:
		case EMMFDevSoundProxyGain:
		case EMMFDevSoundProxyPlayBalance:
		case EMMFDevSoundProxyRecordBalance:
		case EMMFDevSoundProxyGetSupportedInputDataTypes:
		case EMMFDevSoundProxyGetSupportedOutputDataTypes:
		case EMMFDevSoundProxyFixedSequenceName:
		case EMMFDevSoundProxyFixedSequenceCount:
		case EMMFDevSoundProxySamplesRecorded:
		case EMMFDevSoundProxySamplesPlayed:
		case EMMFDevSoundProxyCopyFourCCArrayData:
		case EMMFDevSoundProxyGetTimePlayed:
		case EMMFDevSoundProxyIsResumeSupported:
			type = EQuery_Synchronous;
			break;
		
		case EMMFDevSoundProxyBTBFData:
		case EMMFDevSoundProxyBTBEData:
		case EMMFDevSoundProxyPlayData:
		case EMMFDevSoundProxyRecordData:
			type = EBufferExchangeRelated;
			break;

		// Custom Interfaces
		case EMMFDevSoundProxySyncCustomCommand:
		case EMMFDevSoundProxySyncCustomCommandResult:
		case EMMFDevSoundProxyAsyncCustomCommand:
		case EMMFDevSoundProxyAsyncCustomCommandResult:
		case EMMFDevSoundProxyCustomInterface:
			type = ECustomInterfacesRelated;
			break;
		case RMessage2::EDisConnect:
			type = ESessionEvents;
			break;
		default:
			break;
		}
	return type;
	}


void TMMFDevSoundRequest::Complete(TInt aError)	
	{
	if(!iMessageCompleted && iRequestType != EUndefinedType && iRequestType != ECallBackType)	
		{
		iMessage.Complete(aError);
		iMessageCompleted = ETrue;
		iRequestType = EUndefinedType;
		}
	}

TInt TMMFDevSoundRequest::Function() const 
	{
	return iMessage.Function(); 
	}

TMMFDevSoundRequest::TA3FDevSoundRequestType TMMFDevSoundRequest::Type() const
	{
	return iRequestType;
	}


// 
// CMMFDevSoundSession::CreateL
// Creates a new object
// 
void CMMFDevSoundSession::CreateL(const CMmfIpcServer& aServer)
	{
	CMmfIpcSession::CreateL(aServer);
	CMMFDevSoundServer& server =
	const_cast<CMMFDevSoundServer&>(
			static_cast<const CMMFDevSoundServer&>(aServer));
	server.IncrementSessionId();
	iDevSoundSessionId = server.DevSoundSessionId();
	}

//
// NeedToQueue - mid-commit cycle or async queue start AO is active
//
TBool CMMFDevSoundSession::NeedToQueue() const
    {
    return iOperationCompletePending || iAsyncQueueStart->IsActive();
    }

//
// CMMFDevSoundSession::ServiceL
// (other items were commented in a header).
//
void CMMFDevSoundSession::ServiceL(const RMmfIpcMessage& aMessage)
	{
	SYMBIAN_DEBPRN2(_L("\nCMMFDevSoundSession[0x%x] NEW REQUEST %02x while pending=%d"),
	        aMessage.Function(), NeedToQueue());
	
	if(NeedToQueue())
		{
		// if not possible to service now, then queue request
		EnqueueRequest(aMessage);
		}
	else
		{
		// If there is no oustanding operation service inmediately
		TRAPD(err, DoServiceRequestL(aMessage));
		if (err)
		    {
            aMessage.Complete(err); // repeat normal ServiceL() behaviour since we may keep going
		    }
	    if (!iOperationCompletePending && iQueuedRequests.Count() != 0)
	        {
	        //dequeue next
	        DequeueRequest();
	        }
		}
	}
	
//
// CMMFDevSoundSession::DoServiceL
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoServiceRequestL(const RMmfIpcMessage& aMessage)
	{
	iRequestBeingServiced.SetMessage(aMessage);
	iAsyncQueueStart->Cancel(); // just in case.
    ResetNotifiedError();

    if (aMessage.Function() == RMessage2::EDisConnect)
        {
        TBool complete = iAdapter->CloseDevSound();
        if(!complete)
            {
            iRequestBeingServiced.SetMessage(aMessage);
            iOperationCompletePending = ETrue;
            ResetNotifiedError();
            }
        else
            {
            // if we get here, iClosing wait will have been started and we'd be waiting
            iClosingWait->AsyncStop();
            }
        return;
        }
    
    TMMFMessageDestinationPckg destinationPckg;
	User::LeaveIfError(MessageRead(aMessage, 0, destinationPckg));
	SYMBIAN_DEBPRN2(_L("CMMFDevSoundSession[0x%x]::DoServiceRequestL - DestinationHandle [%d] InterfaceId [%d] "), destinationPckg().DestinationHandle(), destinationPckg().InterfaceId());
	if ((destinationPckg().DestinationHandle() == KMMFObjectHandleDevSound) &&
	    (destinationPckg().InterfaceId() == KUidInterfaceMMFDevSound))
		{
        SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoServiceRequestL - Request [%d]"), aMessage.Function());
		TBool complete = EFalse;
		switch(aMessage.Function())
			{
			case EMMFDevSoundProxyPostOpen:
				complete = DoPostOpenL(aMessage);
				break;
			case EMMFDevSoundProxyInitialize1:
				complete = DoInitialize1L(aMessage);
				break;
			case EMMFDevSoundProxyInitialize2:
				complete = DoInitialize2L(aMessage);
				break;
			case EMMFDevSoundProxyInitialize4:
				complete = DoInitialize4L(aMessage);
				break;
			case EMMFDevSoundProxyCapabilities:
				complete = DoCapabilitiesL(aMessage);
				break;
			case EMMFDevSoundProxyConfig:
				complete = DoConfigL(aMessage);
				break;
			case EMMFDevSoundProxySetConfig:
				complete = DoSetConfigL(aMessage);
				break;
			case EMMFDevSoundProxyMaxVolume:
				complete = DoMaxVolumeL(aMessage);
				break;
			case EMMFDevSoundProxyVolume:
				complete = DoVolumeL(aMessage);
				break;
			case EMMFDevSoundProxySetVolume:
				complete = DoSetVolumeL(aMessage);
				break;
			case EMMFDevSoundProxyMaxGain:
				complete = DoMaxGainL(aMessage);
				break;
			case EMMFDevSoundProxyGain:
				complete = DoGainL(aMessage);
				break;
			case EMMFDevSoundProxySetGain:
				complete = DoSetGainL(aMessage);
				break;
			case EMMFDevSoundProxyPlayBalance:
				complete = DoGetPlayBalanceL(aMessage);
				break;
			case EMMFDevSoundProxySetPlayBalance:
				complete = DoSetPlayBalanceL(aMessage);
				break;
			case EMMFDevSoundProxyRecordBalance:
				complete = DoGetRecordBalanceL(aMessage);
				break;
			case EMMFDevSoundProxySetRecordBalance:
				complete = DoSetRecordBalanceL(aMessage);
				break;
			case EMMFDevSoundProxyBTBFData:
				complete = DoBufferToBeFilledDataL(aMessage);
				break;
			case EMMFDevSoundProxyBTBEData:
				complete = DoBufferToBeEmptiedDataL(aMessage);
				break;
			case EMMFDevSoundProxyPlayInit:
				complete = DoPlayInitL(aMessage);
				break;
			case EMMFDevSoundProxyRecordInit:
				complete = DoRecordInitL(aMessage);
				break;
			case EMMFDevSoundProxyPlayData:
				complete = DoPlayDataL(aMessage);
				break;
			case EMMFDevSoundProxyRecordData:
				complete = DoRecordDataL(aMessage);
				break;
			case EMMFDevSoundProxyStop:
				complete = DoStopL(aMessage);
				break;
			case EMMFDevSoundProxyPause:
				complete = DoPauseL(aMessage);
				break;
			case EMMFDevSoundProxyPlayTone:
				complete = DoPlayToneL(aMessage);
				break;
			case EMMFDevSoundProxyPlayDualTone:
				complete = DoPlayDualToneL(aMessage);
				break;
			case EMMFDevSoundProxyPlayDTMFString:
				complete = DoPlayDTMFStringL(aMessage);
				break;
			case EMMFDevSoundProxyPlayToneSequence:
				complete = DoPlayToneSequenceL(aMessage);
				break;
			case EMMFDevSoundProxyPlayFixedSequence:
				complete = DoPlayFixedSequenceL(aMessage);
				break;
			case EMMFDevSoundProxySetDTMFLengths:
				complete = DoSetDTMFLengthsL(aMessage);
				break;
			case EMMFDevSoundProxySetVolumeRamp:
				complete = DoSetVolumeRampL(aMessage);
				break;
			case EMMFDevSoundProxyGetSupportedInputDataTypes:
				complete = DoGetSupportedInputDataTypesL(aMessage);
				break;
			case EMMFDevSoundProxyGetSupportedOutputDataTypes:
				complete = DoGetSupportedOutputDataTypesL(aMessage);
				break;
			case EMMFDevSoundProxyCopyFourCCArrayData:
				complete = DoCopyFourCCArrayDataL(aMessage);
				break;
			case EMMFDevSoundProxySamplesRecorded:
				complete = DoSamplesRecordedL(aMessage);
				break;
			case EMMFDevSoundProxySamplesPlayed:
				complete = DoSamplesPlayedL(aMessage);
				break;
			case EMMFDevSoundProxySetToneRepeats:
				complete = DoSetToneRepeatsL(aMessage);
				break;
			case EMMFDevSoundProxySetPrioritySettings:
				complete = DoSetPrioritySettingsL(aMessage);
				break;
			case EMMFDevSoundProxyFixedSequenceCount:
				complete = DoFixedSequenceCountL(aMessage);
				break;
			case EMMFDevSoundProxyCancelInitialize:
				complete = DoCancelInitializeL(aMessage);
				break;
			case EMMFDevSoundProxyEmptyBuffers:
				complete = DoEmptyBuffersL(aMessage);
				break;
			case EMMFDevSoundProxyGetTimePlayed:
				complete = DoGetTimePlayedL(aMessage);
				break;
			case EMMFDevSoundProxyIsResumeSupported:
				complete = DoQueryResumeSupportedL(aMessage);
				break;
			case EMMFDevSoundProxyResume:
				complete = DoResumeL(aMessage);
				break;

			// DevSound custom command support
			case EMMFDevSoundProxySyncCustomCommand:
			case EMMFDevSoundProxySyncCustomCommandResult:
			case EMMFDevSoundProxyAsyncCustomCommand:
			case EMMFDevSoundProxyAsyncCustomCommandResult:
				complete = DoCustomCommandL(aMessage);
				break;
			case EMMFDevSoundProxyClose:
				complete = DoPrepareCloseL(aMessage);
				break;
			case EMMFDevSoundProxyRequestResourceNotification:
				complete = DoRegisterAsClientL(aMessage);
				break;
			case EMMFDevSoundProxyCancelRequestResourceNotification:
				complete = DoCancelRegisterAsClientL(aMessage);
				break;
			case EMMFDevSoundProxyGetResourceNotificationData:
				complete = DoGetResourceNotificationDataL(aMessage);
				break;
			case EMMFDevSoundProxyWillResumePlay:
				complete = DoWillResumePlayL(aMessage);
				break;
			case EMMFDevSoundProxySetClientThreadInfo:
				complete = DoSetClientThreadInfoL(aMessage);
				break;						
			default:
				User::Leave(KErrNotSupported);
				break;
			}

		// Check if can complete the message now
		if (complete)
			{
			// Complete the message
			// Synchronous requests & Pseudo-asynchronous
			aMessage.Complete(KErrNone);

			// Store function if we need to re-apply it again due to pre-emption clash
			if(iRequestBeingServiced.Type() == TMMFDevSoundRequest::EAction_PseudoAsynchronous)
				{
				iRedoFunction = aMessage.Function();
				}
			}
		}
	else
		{
		// If there's a CI extension, see if that handles this request
		TInt err = KErrNotSupported;
		if (iCIExtension)
			{
            SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoServiceRequestL - CIExtensionRequest [%d]"), aMessage.Function());
            iOperationCompletePending = ETrue;
			iHandlingExtdCI = ETrue;
			TRAPD(err2, err = iCIExtension->HandleMessageL(aMessage));
			if (err2)
				{
				err = err2;
				}
			iOperationCompletePending = EFalse;
			iHandlingExtdCI = EFalse;
			SYMBIAN_DEBPRN2(_L("CMMFDevSoundSession[0x%x]::DoServiceRequestL - CIExtensionRequest[%d] - Exit with Error[%d] "), aMessage.Function(),err);
			}

		if (err != KErrNone)
			{
			// Not been handled, the request is not supported
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

void CMMFDevSoundSession::DoServiceAlreadyCompletedRequestL(const TInt aFunction)
	{
    ResetNotifiedError();

	switch(aFunction)
		{
		case EMMFDevSoundProxyInitialize1:
			DoAlreadyCompletedInitialize1L();
			break;
		case EMMFDevSoundProxyInitialize2:
			DoAlreadyCompletedInitialize2L();
			break;
		case EMMFDevSoundProxyInitialize4:
			DoAlreadyCompletedInitialize4L();
			break;
		case EMMFDevSoundProxyPlayInit:
			DoAlreadyCompletedPlayInitL();
			break;
		case EMMFDevSoundProxyRecordInit:
			DoAlreadyCompletedRecordInitL();
			break;
		case EMMFDevSoundProxyPlayTone:
			DoAlreadyCompletedPlayToneL();
			break;
		case EMMFDevSoundProxyPlayDualTone:
			DoAlreadyCompletedPlayDualToneL();
			break;
		case EMMFDevSoundProxyPlayDTMFString:
			DoAlreadyCompletedPlayDTMFStringL();
			break;
		case EMMFDevSoundProxyPlayToneSequence:
			DoAlreadyCompletedPlayToneSequenceL();
			break;
		case EMMFDevSoundProxyPlayFixedSequence:
			DoAlreadyCompletedPlayFixedSequenceL();
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}

	}

void CMMFDevSoundSession::HandleAlreadyCompletedRequest()
	{
	TRAPD(err,DoServiceAlreadyCompletedRequestL(iRedoFunction));

	if (err != KErrNone)
		{
		switch(iRedoFunction)
			{
			case EMMFDevSoundProxyInitialize1:
			case EMMFDevSoundProxyInitialize2:
			case EMMFDevSoundProxyInitialize4:
				InitializeComplete(err);
				break;
			case EMMFDevSoundProxyPlayInit:
				PlayError(err);
				break;
			case EMMFDevSoundProxyRecordInit:
				RecordError(err);
				break;
			case EMMFDevSoundProxyPlayTone:
			case EMMFDevSoundProxyPlayDualTone:
			case EMMFDevSoundProxyPlayDTMFString:
			case EMMFDevSoundProxyPlayToneSequence:
			case EMMFDevSoundProxyPlayFixedSequence:
				ToneFinished(err);
				break;
			default:
				break;
			}
		}
	}

void CMMFDevSoundSession::EnqueueRequest(const RMmfIpcMessage& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::EnqueueRequest - Enter"));
	// Encapsule the request
	TMMFDevSoundRequest request;
	request.SetMessage(aMessage);
	// Append
	TInt error = iQueuedRequests.Append(request);
	__ASSERT_DEBUG(error == KErrNone, Panic(EQueueRequestsFailedToAppend));
	SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::EnqueueRequest - Exit"));
	}

//
// CMMFDevSoundSession::DoPostOpenL
//
TBool CMMFDevSoundSession::DoPostOpenL(const RMmfIpcMessage& /*aMessage*/)
	{
	iAdapter->PostOpenL();
	iOperationCompletePending = ETrue;
	return EFalse;
	}

//
// CMMFDevSoundSession::DoInitialize1L
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoInitialize1L(const RMmfIpcMessage& aMessage)
	{
    iMsgQueue.Close(); // close if already open
	TInt err = iMsgQueue.Open(aMessage, 2, EOwnerThread); // a global queue but owned by thread
	User::LeaveIfError(err);
	DoSetClientConfigL();// added here instead of the CreateL()
	TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage,1,devSoundBuf));
	iCachedClientData = devSoundBuf;
	TMMFState mode = devSoundBuf().iMode;
	iAdapter->InitializeL(mode);
	iBufferPlay = NULL;
	iPlayErrorOccured = EFalse;
	// Flag to queue any further request 
	// but the message can be completed now
	iOperationCompletePending = ETrue;
	return ETrue;
	}

//
// CMMFDevSoundSession::DoAlreadyCompletedInitialize1L
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoAlreadyCompletedInitialize1L()
	{
	TMMFState mode = iCachedClientData().iMode;
	iAdapter->InitializeL(mode);
	iBufferPlay = NULL;
	iPlayErrorOccured = EFalse;
	// Flag to queue any further request
	iOperationCompletePending = ETrue;
	}

//
// CMMFDevSoundSession::DoInitialize2L
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoInitialize2L(const RMmfIpcMessage& aMessage)
	{
    iMsgQueue.Close(); // close if already open
	TInt err = iMsgQueue.Open(aMessage, 2, EOwnerThread); // a global queue but owned by thread
	User::LeaveIfError(err);
	DoSetClientConfigL();// added here instead of the CreateL()
	TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage,1,devSoundBuf));
	iCachedClientData = devSoundBuf;
	TUid HWDev = devSoundBuf().iHWDev;
	TMMFState mode = devSoundBuf().iMode;
	iAdapter->InitializeL(HWDev, mode);
	iBufferPlay = NULL;
	iPlayErrorOccured = EFalse;
	return ETrue;
	}

//
// CMMFDevSoundSession::DoAlreadyCompletedInitialize2L
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoAlreadyCompletedInitialize2L()
	{
	TUid HWDev = iCachedClientData().iHWDev;
	TMMFState mode = iCachedClientData().iMode;
	iAdapter->InitializeL(HWDev, mode);
	iBufferPlay = NULL;
	iPlayErrorOccured = EFalse;
	}

//
// CMMFDevSoundSession::DoInitialize4L
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoInitialize4L(const RMmfIpcMessage& aMessage)
	{
    iMsgQueue.Close();
	TInt err = iMsgQueue.Open(aMessage, 2, EOwnerThread); // a global queue but owned by thread
	User::LeaveIfError(err);
	DoSetClientConfigL();// added here instead of the CreateL()
	TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage, TInt(1), devSoundBuf));
	iCachedClientData = devSoundBuf;
	TFourCC desiredFourCC = devSoundBuf().iDesiredFourCC;
	TMMFState mode = devSoundBuf().iMode;
	iAdapter->InitializeL(desiredFourCC, mode);
	iBufferPlay = NULL;
	iPlayErrorOccured = EFalse;
	// Flag to queue any further request 
	// but the message can be completed now
	iOperationCompletePending = ETrue;
	return ETrue;
	}

//
// CMMFDevSoundSession::DoAlreadyCompletedInitialize4L
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoAlreadyCompletedInitialize4L()
	{
	TFourCC desiredFourCC = iCachedClientData().iDesiredFourCC;
	TMMFState mode = iCachedClientData().iMode;
	iAdapter->InitializeL(desiredFourCC, mode);
	iBufferPlay = NULL;
	iPlayErrorOccured = EFalse;
	// Flag to queue any further request
	iOperationCompletePending = ETrue;
	}

//
// CMMFDevSoundSession::DoCancelInitialize
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoCancelInitializeL(const RMmfIpcMessage& aMessage)
	{
	TInt err=iAdapter->CancelInitialize();

	if (err != KErrNone)
		{
		aMessage.Complete(err);
		iOperationCompletePending = EFalse;
		return ETrue;
		}
	else
		{
		iOperationCompletePending = ETrue;
		}
	return EFalse;
	}

//
// CMMFDevSoundSession::DoCapabilitiesL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoCapabilitiesL(const RMmfIpcMessage& aMessage)
	{
	TInt err = iAdapter->Capabilities(iDevSoundCapabilities);
	if(err != KErrNone)
		{
		aMessage.Complete(err);
		iOperationCompletePending = EFalse;
		}
	else
		{
		iOperationCompletePending = ETrue;
		}
	return EFalse;
	}

//
// CMMFDevSoundSession::DoConfigL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoConfigL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings devSoundSet;
	devSoundSet.iConfig = iAdapter->Config();
	TMMFDevSoundProxySettingsPckg pckg(devSoundSet);
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));
	return ETrue;
	}

//
// CMMFDevSoundSession::DoSetConfigL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSetConfigL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),devSoundBuf));
	TMMFCapabilities config = devSoundBuf().iConfig;
	iAdapter->SetConfigL(config);
	iOperationCompletePending = ETrue;
	return EFalse;
	}

//
// CMMFDevSoundSession::axVolumeL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoMaxVolumeL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings devSoundSet;
	devSoundSet.iMaxVolume = iAdapter->MaxVolume();
	TMMFDevSoundProxySettingsPckg pckg(devSoundSet);
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));
	return ETrue;
	}

//
// CMMFDevSoundSession::DoVolumeL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoVolumeL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings devSoundSet;
	devSoundSet.iVolume = iAdapter->Volume();
	TMMFDevSoundProxySettingsPckg pckg(devSoundSet);
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));
	return ETrue;
	}

//
// CMMFDevSoundSession::DoSetVolumeL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSetVolumeL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage, TInt(1),devSoundBuf));
	TInt volume = devSoundBuf().iVolume;
	TBool asyncOperation;
	User::LeaveIfError(iAdapter->SetVolume(volume, asyncOperation));
	iOperationCompletePending = asyncOperation;
	return !asyncOperation;
	}

//
// CMMFDevSoundSession::DoMaxGainL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoMaxGainL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings devSoundSet;
	devSoundSet.iMaxGain = iAdapter->MaxGain();
	TMMFDevSoundProxySettingsPckg pckg(devSoundSet);
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));
	return ETrue;
	}

//
// CMMFDevSoundSession::DoGainL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoGainL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings devSoundSet;
	devSoundSet.iGain = iAdapter->Gain();
	TMMFDevSoundProxySettingsPckg pckg(devSoundSet);
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));
	return ETrue;
	}

//
// CMMFDevSoundSession::DoSetGainL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSetGainL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),devSoundBuf));
	TInt gain = devSoundBuf().iGain;
	TBool asyncOperation;
	User::LeaveIfError(iAdapter->SetGain(gain, asyncOperation));
	iOperationCompletePending = asyncOperation;
	return !asyncOperation;
	}

//
// CMMFDevSoundSession::DoGetPlayBalanceL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoGetPlayBalanceL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings devSoundSet;
	iAdapter->GetPlayBalanceL(devSoundSet.iLeftPercentage, devSoundSet.iRightPercentage);
	TMMFDevSoundProxySettingsPckg pckg(devSoundSet);
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));
	return ETrue;
	}

//
// CMMFDevSoundSession::DoSetPlayBalanceL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSetPlayBalanceL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg devSoundBuf;
    User::LeaveIfError(MessageRead(aMessage,TInt(1),devSoundBuf));
	TInt leftPercentage = devSoundBuf().iLeftPercentage;
	TInt rightPercentage = devSoundBuf().iRightPercentage;
	TBool asyncOperation;
	iAdapter->SetPlayBalanceL(leftPercentage, rightPercentage, asyncOperation);
	iOperationCompletePending = asyncOperation;
	return !asyncOperation;
	}

//
// CMMFDevSoundSession::DoGetRecordBalanceL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoGetRecordBalanceL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettings devSoundSet;
	iAdapter->GetRecordBalanceL(devSoundSet.iLeftPercentage, devSoundSet.iRightPercentage);
	TMMFDevSoundProxySettingsPckg pckg(devSoundSet);
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));
	return ETrue;
	}

//
// CMMFDevSoundSession::DoSetRecordBalanceL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSetRecordBalanceL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),devSoundBuf));
	TInt leftPercentage = devSoundBuf().iLeftPercentage;
	TInt rightPercentage = devSoundBuf().iRightPercentage;
	TBool asyncOperation;
	iAdapter->SetRecordBalanceL(leftPercentage, rightPercentage, asyncOperation);
	iOperationCompletePending = asyncOperation;
	return !asyncOperation;
	}

//
// CMMFDevSoundSession::DoPlayInitL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoPlayInitL(const RMmfIpcMessage& /*aMessage*/)
	{
	iAdapter->PlayInitL();
	iOperationCompletePending = ETrue;
	return ETrue;
	}

//
// CMMFDevSoundSession::DoAlreadyCompletedPlayInitL
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoAlreadyCompletedPlayInitL()
	{
	iAdapter->PlayInitL();
	iOperationCompletePending = ETrue;
	}

//
// CMMFDevSoundSession::DoRecordInitL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoRecordInitL(const RMmfIpcMessage& /*aMessage*/)
	{
	iAdapter->RecordInitL();
	iOperationCompletePending = ETrue;
	return ETrue;
	}

//
// CMMFDevSoundSession::DoAlreadyCompletedRecordInitL
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoAlreadyCompletedRecordInitL()
	{
	iAdapter->RecordInitL();
	iOperationCompletePending = ETrue;
	}

//
// CMMFDevSoundSession::DoPlayDataL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoPlayDataL(const RMmfIpcMessage& aMessage)
	{
	SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoPlayDataL - Enter"));

	if( iPlayErrorOccured )
		{
		SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoPlayDataL - Ignore and Exit"));
		return ETrue;
		}

	TMMFDevSoundProxyHwBufPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),devSoundBuf));
	iBufferPlay->SetLastBuffer(devSoundBuf().iLastBuffer);

	TPtr8 dataPtr(iChunk.Base(), devSoundBuf().iBufferSize, devSoundBuf().iBufferSize);
	// Copy data over from chunk
	iBufferPlay->Data().Copy(dataPtr);
	iAdapter->PlayData();
	SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoPlayDataL - Exit"));
	return ETrue;
	}

//
// CMMFDevSoundSession::DoRecordDataL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoRecordDataL(const RMmfIpcMessage& /*aMessage*/)
	{
	iAdapter->RecordData();
	return ETrue;
	}

//
// CMMFDevSoundSession::DoStopL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoStopL(const RMmfIpcMessage& /*aMessage*/)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoStopL - Enter"));
    // Sometimes Stop is not involved on a commit cycle
	TBool completed = iAdapter->Stop();
	if (completed)
		{
        FlushQueuedRequests();
		FlushEventQueue(); // completed returned here means we were idle to start with. TODO could possibly skip this flush
		iChunk.Close();
		}
	iOperationCompletePending = !completed;
	SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoStopL - Exit. Return value is [%d]"), completed);
	return completed;
	}

//
// CMMFDevSoundSession::DoPauseL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoPauseL(const RMmfIpcMessage& /*aMessage*/)
	{
	User::LeaveIfError(iAdapter->Pause());
	iOperationCompletePending = ETrue;
	return EFalse;
	}

//
// CMMFDevSoundSession::DoPlayToneL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoPlayToneL(const RMmfIpcMessage& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoPlayToneL - Enter"));
    TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),devSoundBuf));
	iCachedClientData = devSoundBuf;
	TInt frequency = devSoundBuf().iFrequencyOne;
	TTimeIntervalMicroSeconds duration(devSoundBuf().iDuration);
	iAdapter->PlayToneL(frequency, duration);
	iOperationCompletePending = ETrue;
	SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoPlayToneL - Exit. Return value is [%d]"), ETrue);
	return ETrue;
	}

//
// CMMFDevSoundSession::DoAlreadyCompletedPlayToneL
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoAlreadyCompletedPlayToneL()
	{
	TInt frequency = iCachedClientData().iFrequencyOne;
	TTimeIntervalMicroSeconds duration(iCachedClientData().iDuration);
	iAdapter->PlayToneL(frequency, duration);
	iOperationCompletePending = ETrue;
	}

//
// CMMFDevSoundSession::DoPlayDualToneL
// (other items were commented in a header).	
//
TBool CMMFDevSoundSession::DoPlayDualToneL(const RMmfIpcMessage& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoPlayDualToneL - Enter"));
    TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),devSoundBuf));
	iCachedClientData = devSoundBuf;
	TInt frequencyOne = devSoundBuf().iFrequencyOne;
	TInt frequencyTwo = devSoundBuf().iFrequencyTwo;
	TTimeIntervalMicroSeconds duration(devSoundBuf().iDuration);
	iAdapter->PlayDualToneL(frequencyOne, frequencyTwo, duration);
	iOperationCompletePending = ETrue;
	SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoPlayDualToneL - Exit. Return value is [%d]"), ETrue);
	return ETrue;
	}

//
// CMMFDevSoundSession::DoAlreadyCompletedPlayDualToneL
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoAlreadyCompletedPlayDualToneL()
	{
	TInt frequencyOne = iCachedClientData().iFrequencyOne;
	TInt frequencyTwo = iCachedClientData().iFrequencyTwo;
	TTimeIntervalMicroSeconds duration(iCachedClientData().iDuration);
	iAdapter->PlayDualToneL(frequencyOne, frequencyTwo, duration);
	iOperationCompletePending = ETrue;
	}

//
// CMMFDevSoundSession::DoPlayDTMFStringL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoPlayDTMFStringL(const RMmfIpcMessage& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoPlayDTMFStringL - Enter"));
    TInt dtmfLength = aMessage.GetDesLength(2);
	
	if(iDtmfString)
		{
		delete iDtmfString;
		iDtmfString = NULL;
		}

	iDtmfString = HBufC::NewL(dtmfLength);
	TPtr dtmfPtr = iDtmfString->Des();
    User::LeaveIfError(MessageRead(aMessage, TInt(2), dtmfPtr));
	iAdapter->PlayDTMFStringL(*iDtmfString);
	iOperationCompletePending = ETrue;
	SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoPlayDTMFStringL - Exit. Return value is [%d]"), ETrue);
	return ETrue;
	}

//
// CMMFDevSoundSession::DoAlreadyCompletedPlayDTMFStringL
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoAlreadyCompletedPlayDTMFStringL()
	{
	iAdapter->PlayDTMFStringL(*iDtmfString);
	iOperationCompletePending = ETrue;
	}

//
// CMMFDevSoundSession::DoPlayToneSequenceL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoPlayToneSequenceL(const RMmfIpcMessage& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoPlayToneSequenceL - Enter"));
    TInt toneLength = aMessage.GetDesLength(1);

	if(iToneSeqBuf)
		{
		delete iToneSeqBuf;
		iToneSeqBuf = NULL;
		}

	iToneSeqBuf = HBufC8::NewL(toneLength);
	TPtr8 toneSeqPtr = iToneSeqBuf->Des();
	User::LeaveIfError(MessageRead(aMessage,TInt(1), toneSeqPtr));

	iAdapter->PlayToneSequenceL(*iToneSeqBuf);
	iOperationCompletePending = ETrue;
	SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoPlayToneSequenceL - Exit. Return value is [%d]"), ETrue);
	return ETrue;
	}

//
// CMMFDevSoundSession::DoAlreadyCompletedPlayToneSequenceL
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoAlreadyCompletedPlayToneSequenceL()
	{
	iAdapter->PlayToneSequenceL(*iToneSeqBuf);
	iOperationCompletePending = ETrue;
	}

//
// CMMFDevSoundSession::DoPlayFixedSequenceL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoPlayFixedSequenceL(const RMmfIpcMessage& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoPlayFixedSequenceL - Enter"));
    TPckgBuf<TInt> buf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),buf));
	TInt seqNum = buf();
	iSeqNum = seqNum;
	iAdapter->PlayFixedSequenceL(seqNum);
	iOperationCompletePending = ETrue;
	SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoPlayFixedSequenceL - Exit. Return value is [%d]"), ETrue);
	return ETrue;
	}

//
// CMMFDevSoundSession::DoAlreadyCompletedPlayFixedSequenceL
// (other items were commented in a header).
//
void CMMFDevSoundSession::DoAlreadyCompletedPlayFixedSequenceL()
	{
	iAdapter->PlayFixedSequenceL(iSeqNum);
	iOperationCompletePending = ETrue;
	}

//
// CMMFDevSoundSession::DoSetDTMFLengthsL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSetDTMFLengthsL(const RMmfIpcMessage& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoSetDTMFLengthsL - Enter"));
    TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),devSoundBuf));
	TTimeIntervalMicroSeconds32 toneOnLength = devSoundBuf().iToneOnLength;
	TTimeIntervalMicroSeconds32 toneOffLength = devSoundBuf().iToneOffLength;
	TTimeIntervalMicroSeconds32 pauseLength = devSoundBuf().iPauseLength;
	User::LeaveIfError(iAdapter->SetDTMFLengths(toneOnLength, toneOffLength, pauseLength));
    SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoSetDTMFLengthsL - Exit. Return value is [%d]"), ETrue);
	return ETrue;
	}

//
// CMMFDevSoundSession::DoSetVolumeRampL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSetVolumeRampL(const RMmfIpcMessage& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoSetVolumeRampL - Enter"));
    TMMFDevSoundProxySettingsPckg devSoundBuf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),devSoundBuf));
	TTimeIntervalMicroSeconds duration = devSoundBuf().iDuration;
	User::LeaveIfError(iAdapter->SetVolumeRamp(duration));
	iOperationCompletePending = EFalse; // Volume ramp doesn't result on commit
    SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoSetVolumeRampL - Exit. Return value is [%d]"), ETrue);
	return ETrue; // operation complete
	}

//
// CMMFDevSoundSession::DoGetSupportedInputDataTypesL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoGetSupportedInputDataTypesL(
							const RMmfIpcMessage& aMessage)
	{
	iArray.Reset();

	TMMFPrioritySettingsPckg prioritySetBuf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),prioritySetBuf));
	TMMFPrioritySettings prioritySet = prioritySetBuf();

	iAdapter->GetSupportedInputDataTypesL(iArray, prioritySet);

	TPckgBuf<TInt> pckg;
	pckg() = iArray.Count();
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));

	return ETrue;
	}

//
// CMMFDevSoundSession::DoGetSupportedOutputDataTypesL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoGetSupportedOutputDataTypesL(
							const RMmfIpcMessage& aMessage)
	{
	iArray.Reset();

	TMMFPrioritySettingsPckg prioritySetBuf;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),prioritySetBuf));
	TMMFPrioritySettings prioritySet = prioritySetBuf();

	iAdapter->GetSupportedOutputDataTypesL(iArray, prioritySet);

	TPckgBuf<TInt> pckg;
	pckg() = iArray.Count();
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));

	return ETrue;
	}

//
// CMMFDevSoundSession::DoSamplesRecordedL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSamplesRecordedL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	pckg() = iAdapter->SamplesRecorded();
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));
	return ETrue;
	}

//
// CMMFDevSoundSession::DoSamplesPlayedL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSamplesPlayedL(const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	pckg() = iAdapter->SamplesPlayed();
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),pckg));
	return ETrue;
	}

//
// CMMFDevSoundSession::DoSetToneRepeatsL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSetToneRepeatsL(const RMmfIpcMessage& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoSetToneRepeatsL - Enter"));
    TPckgBuf<TInt> countRepeat;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),countRepeat));

	TPckgBuf<TTimeIntervalMicroSeconds> repeatTS;
	User::LeaveIfError(MessageRead(aMessage,TInt(2),repeatTS));
	User::LeaveIfError(iAdapter->SetToneRepeats(countRepeat(), repeatTS()));
    SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoSetToneRepeatsL - Exit. Return value is [%d]"), ETrue);
	return ETrue;
	}

//
// CMMFDevSoundSession::DoSetPrioritySettingsL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoSetPrioritySettingsL(
						const RMmfIpcMessage& aMessage)
	{
	TPckgBuf<TMMFPrioritySettings> prioritySet;
	User::LeaveIfError(MessageRead(aMessage,TInt(1),prioritySet));

	User::LeaveIfError(iAdapter->SetPrioritySettings(prioritySet()));
	iOperationCompletePending = EFalse;
	return ETrue;
	}

//
// CMMFDevSoundSession::DoFixedSequenceCountL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoFixedSequenceCountL(
					const RMmfIpcMessage& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DoFixedSequenceCountL - Enter"));
    TPckgBuf<TInt> fixSeqCountPckg;
	TInt fixSeqCount = iAdapter->FixedSequenceCount();
	fixSeqCountPckg = fixSeqCount;

	User::LeaveIfError(MessageWrite(aMessage,TInt(2),fixSeqCountPckg));
    SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::DoFixedSequenceCountL - Exit. Return value is [%d]"), ETrue);
	return ETrue;
	}


//
// CMMFDevSoundSession::DoCopyFourCCArrayDataL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoCopyFourCCArrayDataL(
						const RMmfIpcMessage& aMessage)
	{
	const TInt KBufExpandSize8 = 8;//two TInts
	CBufFlat* dataCopyBuffer = CBufFlat::NewL(KBufExpandSize8);
	CleanupStack::PushL(dataCopyBuffer);
	RBufWriteStream stream;
	stream.Open(*dataCopyBuffer);
	CleanupClosePushL(stream);
	
	TInt i = 0;
	TInt count = iArray.Count();
	
	while (i < count)
		{
		stream.WriteInt32L(iArray[i].FourCC());
		i++;
		}
	User::LeaveIfError(MessageWrite(aMessage, TInt(2), dataCopyBuffer->Ptr(0)));
	stream.Close();
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(dataCopyBuffer);
	return ETrue;
	}


//
// CMMFDevSoundSession::DoBufferToBeFilledDataL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoBufferToBeFilledDataL(
							const RMmfIpcMessage& aMessage)
	{
	// if CMMFDevSoundSession::PlayError() has been called, RChunk would have got closed.
	// Need to check if Chunk Handle is still valid. If it is not,complete the message immediately and send a error to the Client.
	if(!iChunk.Handle())
		{
		aMessage.Complete(KErrBadHandle);
		return EFalse;	
		}
	TPckgBuf<TInt> requestChunkBuf;
	User::LeaveIfError(MessageRead(aMessage, TInt(1), requestChunkBuf));
	TBool requestChunk = requestChunkBuf();
	if (requestChunk)
		{
		// if the client requests, always do EOpen
		iHwBufPckgFill().iChunkOp = EOpen;
		}
	TInt err = MessageWrite(aMessage, TInt(2), iHwBufPckgFill);
	if ( (err == KErrNone) && (iHwBufPckgFill().iChunkOp == EOpen) )
		{
		aMessage.Complete(iChunk);
		}
	else
		{
		aMessage.Complete(err);
		}
	return EFalse;
	}

// CMMFDevSoundSession::DoBufferToBeEmptiedDataL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoBufferToBeEmptiedDataL(
						const RMmfIpcMessage& aMessage)
	{
	// if CMMFDevSoundSession::RecordError() has been called, RChunk would have got closed.
	// Need to check if Chunk Handle is still valid. If it is not,complete the message immediately and send a error to the Client.
	if(!iChunk.Handle())
		{
		aMessage.Complete(KErrBadHandle);
		return EFalse;	
		}
	
	TInt err = MessageWrite(aMessage, TInt(2), iHwBufPckgEmpty);
	if ( (err == KErrNone) && (iHwBufPckgEmpty().iChunkOp == EOpen) )
		{
		aMessage.Complete(iChunk);
		}
	else
		{
		aMessage.Complete(err);
		}
	return EFalse;
	}

//
// CMMFDevSoundSession::DoEmptyBuffersL
// (other items were commented in a header).
//

TBool CMMFDevSoundSession::DoEmptyBuffersL(const RMmfIpcMessage& aMessage)
	{
	TInt err = KErrNone;
	FilterQueueEvent(EMMFDevSoundProxyBTBFEvent);
	// This is now asynchronous
	err = iAdapter->EmptyBuffers();
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;
		}
	iOperationCompletePending = ETrue;
	return EFalse;
	}
//
// CMMFDevSoundSession::DoGetTimePlayedL
// (other items were commented in a header).
//
TBool CMMFDevSoundSession::DoGetTimePlayedL(const RMmfIpcMessage& aMessage)
	{
	TInt err = KErrNone;
	TTimeIntervalMicroSeconds time(0);
	TPckgBuf<TTimeIntervalMicroSeconds> timePckg(time);
	err = iAdapter->GetTimePlayed(timePckg());
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),timePckg));
	return ETrue;
	}

TBool CMMFDevSoundSession::DoQueryResumeSupportedL(const RMmfIpcMessage& aMessage)
	{
	TBool isSupported = EFalse;
	TPckgBuf<TBool> isSupportedPckg(isSupported);
	isSupportedPckg() = iAdapter->IsResumeSupported();
	User::LeaveIfError(MessageWrite(aMessage,TInt(2),isSupportedPckg));
	return ETrue;
	}

TBool CMMFDevSoundSession::DoResumeL(const RMmfIpcMessage& /*aMessage*/)
	{
	User::LeaveIfError( iAdapter->Resume() );
	iOperationCompletePending = ETrue;
	FilterQueueEvent(EMMFDevSoundProxyPausedRecordCompleteEvent);
	return EFalse;
	}

TBool CMMFDevSoundSession::DoPrepareCloseL(const RMmfIpcMessage& /*aMessage*/)
	{
	TBool complete = iAdapter->CloseDevSound();
	if(!complete)
		{
		iOperationCompletePending = ETrue;	
		}
	return complete;
	}


TBool CMMFDevSoundSession::DoCustomCommandL(const RMmfIpcMessage& aMessage)
	{
	TInt retVal = KErrNone;
	TRAPD(err, retVal = iDeMuxUtility->ProcessCustomInterfaceCommandL(aMessage));
	if (err != KErrNone)
		{
		// the framework left with an error condition
		// so we complete the message with this error
		// irrespective of whether its a Sync or Async custom command
		aMessage.Complete(err);
		}
	else
		{
		TInt messageType = aMessage.Function();
		if ((messageType == EMMFDevSoundProxySyncCustomCommand) ||
			(messageType == EMMFDevSoundProxySyncCustomCommandResult))
			{
			// If its a sync custom command
			// we can pass back valid values here since command
			// has been handled by the DeMux framework
			aMessage.Complete(retVal);	
			}
		}
	
	// we complete our own message so don't need the framework to do so
	return EFalse;
	}
	

//
// CMMFDevSoundSession::CMMFDevSoundSession
// (other items were commented in a header).
//
CMMFDevSoundSession::CMMFDevSoundSession() :
	iSetClientConfigApplied (EFalse),
	iDisconnecting (EFalse)
	{
	}

//
// CMMFDevSoundSession::~CMMFDevSoundSession
// (other items were commented in a header).
//
CMMFDevSoundSession::~CMMFDevSoundSession()
	{
	delete iAsyncQueueStart;
	// clear the array of custom interfaces
	TInt count = iCustomInterfaceArray.Count();
	for (TInt i = 0; i < count; i++)
		{
		// we could have already deleted interfaces without
		// removing them from the array so check for this
		// and only delete release plugin if non-null
		MMMFDevSoundCustomInterfaceDeMuxPlugin* ptr = iCustomInterfaceArray[i].iInterface;
		if (ptr)
			{
			iCustomInterfaceArray[i].iInterface->Release();	
			}
		}
	iCustomInterfaceArray.Reset();
	iCustomInterfaceArray.Close();
		
	delete iDeMuxUtility;
	
	if (iCIExtension)
		{
		iCIExtension->Release();
		iCIExtension = NULL;
		}

	iMsgQueue.Close();
	iArray.Close();
	iQueuedRequests.Close();
	delete iDtmfString;
	delete iToneSeqBuf;
	delete iAdapter;
	delete iClosingWait;

	CMMFDevSoundServer* server =
			const_cast<CMMFDevSoundServer*>(
			static_cast<const CMMFDevSoundServer*>(Server()));

	if (server)
		{
		server->DecrementSessionId();
		}

//	delete iCustomCommandParserManager;
//	delete iMMFObjectContainer;

	// Close chunk
	iChunk.Close();
	}

//
// CMMFDevSoundSession::FlushEventQueue()
//
void CMMFDevSoundSession::FlushEventQueue()
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::FlushEventQueue - Enter"));
    if(iMsgQueue.Handle() != 0)
		{
		TMMFDevSoundQueueItem queueItem;
		TInt err = KErrNone;
		while(err != KErrUnderflow)
			{
			err = iMsgQueue.Receive(queueItem);
			}
		}
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::FlushEventQueue - Exit"));
	}

void CMMFDevSoundSession::FilterQueueEvent(TMMFDevSoundProxyRequest aRequest)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::FilterQueueEvent - Enter"));
    if(iMsgQueue.Handle() != 0)
		{
		// Pop and push events result at the queue 
		// can be seen as "circular list"
		// set a mark to traverse it safely 
		TMMFDevSoundQueueItem markItem;
		markItem.iRequest = EMMFDevSoundProxyMarkEvent;
		// assumes sufficient space in the queue
		TInt err = iMsgQueue.Send(markItem);
		__ASSERT_DEBUG(err == KErrNone, Panic(EMsgQueueFailedToSendMsg));
		
		while(ETrue)
			{
			// At least the markEvent is at the queue so ignore the error
			TMMFDevSoundQueueItem queueItem;
			err = iMsgQueue.Receive(queueItem);
			if(queueItem.iRequest == EMMFDevSoundProxyMarkEvent)
				{
				break;
				}
			// Look for the specific event
			else if(queueItem.iRequest != aRequest)
				{
				// assumes sufficient space in the queue
				err = iMsgQueue.Send(queueItem);
				}
			}
		}
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::FilterQueueEvent - Exit"));
	}

//
// CMMFDevSoundSession::Disconnect
// (other items were commented in a header).
//
void CMMFDevSoundSession::Disconnect(const RMessage2& aMessage)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::Disconnect - Enter"));
    iDisconnecting = ETrue;
	
    if (NeedToQueue())
        {
        // if we are in the middle of something, enqueue and wait
        SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::Disconnect - Add to queue"));
        EnqueueRequest(aMessage);
        iClosingWait->Start();
        }
    else
        {
        // else do now. Enter ActiveSchedulerWait to wait for AsyncOpComplete
        TBool complete = iAdapter->CloseDevSound();
        if(!complete)
            {
            iRequestBeingServiced.SetMessage(aMessage);
            iOperationCompletePending = ETrue;
            ResetNotifiedError();
            iClosingWait->Start();
            }
        }
	CSession2::Disconnect(aMessage);
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::Disconnect - Exit"));
	}


//
// CMMFDevSoundSession::NewL
// (other items were commented in a header).
//
CMMFDevSoundSession* CMMFDevSoundSession::NewL(MGlobalProperties& aGlobalProperties)
	{
	CMMFDevSoundSession* self = new (ELeave) CMMFDevSoundSession;
	CleanupStack::PushL(self);
	self->ConstructL(aGlobalProperties);
	CleanupStack::Pop(self);
	return self;
	}

//
// CMMFDevSoundSession::ConstructL
// (other items were commented in a header).
//
void CMMFDevSoundSession::ConstructL(MGlobalProperties& aGlobalProperties)
	{
	iAdapter = CMMFDevSoundAdaptation::NewL(*this, aGlobalProperties);

	iClosingWait = new(ELeave) CActiveSchedulerWait();
	
	// Create the Custom Interface DeMux Utility
	iDeMuxUtility = CMMFDevSoundCIDeMuxUtility::NewL(this);
	
	// Create the Custom Interface extension
	TUid implUid = {KMmfUidCIServerExtensionImpl};
	TInt uidAsInteger = implUid.iUid;
	const TInt KCIExtTempBufferSize = 20;
	TBuf8<KCIExtTempBufferSize> tempBuffer;
	tempBuffer.Num(uidAsInteger, EHex);
	TUid interfaceUid = {KUidDevSoundCIServerExtension};
	TUid destructorKey;
	TRAPD(err, iCIExtension = static_cast<MDevSoundCIServerExtension*>
		 (MmPluginUtils::CreateImplementationL(interfaceUid, destructorKey, tempBuffer, KRomOnlyResolverUid)));
	if (KErrNotSupported == err)
		{
		iCIExtension = NULL;
		}
	else
		{
		User::LeaveIfError(err);
		}
	if (iCIExtension)
		{
		// Extension exists. Complete the setup
		iCIExtension->PassDestructorKey(destructorKey);
		User::LeaveIfError(iCIExtension->Setup(*this));
		}

	iQueuedRequests.ReserveL(KMaxQueueRequest);
	iAsyncQueueStart = new (ELeave) CAsyncCallBack(CActive::EPriorityStandard);
	TCallBack asyncCallback(AsyncQueueStartCallback, this);
	iAsyncQueueStart->Set(asyncCallback);
	}

// CMMFDevSoundSession::InitializeComplete
// (other items were commented in a header).
//
void CMMFDevSoundSession::InitializeComplete(TInt aError)
	{
	// this may be a re-initialization and so we need to
	// re-get our custom interfaces on the DeMux plugins
	TInt count = iCustomInterfaceArray.Count();
	for (TInt i = 0; i < count; i++)
		{
		// we could have already deleted interfaces without
		// removing them from the array so check for this
		// and only refresh plugin if non-null
		MMMFDevSoundCustomInterfaceDeMuxPlugin* ptr = iCustomInterfaceArray[i].iInterface;
		if (ptr)
			{
			// we can't keep track of..
			// 1. where a custom interface is implemented
			// 2. the uid of the custom interface to be refreshed
			// so assume all have to be refreshed
			TRAPD(err, ptr->RefreshL());	
			
			// Error indicates this is no longer a valid interface
			if (err != KErrNone)
				{
				TMMFEvent event;
				TMMFDevSoundQueueItem item;
				item.iRequest = EMMFDevSoundCustomCommandCloseMuxDemuxPair;
				item.iErrorCode = err;
				event.iEventType.iUid = i+1;
				item.iEventPckg() = event;
				TInt lErr = iMsgQueue.Send(item);
				__ASSERT_DEBUG(lErr == KErrNone, Panic(EMsgQueueFailedToSendMsg));

// NB proper panic code required here for this part.
				}
			}
		}
	TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxyICEvent;
	item.iErrorCode = aError;
	// assumes sufficient space in the queue so ignores the return value
	iMsgQueue.Send(item); 
	}

// 
// CMMFDevSoundSession::ToneFinished
// (other items were commented in a header).
//
void CMMFDevSoundSession::ToneFinished(TInt aError)
	{
    SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::ToneFinished - Enter. Error [%d]"), aError);
    TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxyTFEvent;
	item.iErrorCode = aError;
	// assumes sufficient space in the queue so ignores the return value
	iMsgQueue.Send(item);
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::ToneFinished - Exit"));
	}

//
// CMMFDevSoundSession::BufferToBeFilled
// (other items were commented in a header).
//
void CMMFDevSoundSession::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::BufferToBeFilled - Enter"));

	// Set play error flag to false 
	iPlayErrorOccured = EFalse;

	// Store pointer to the buffer to use it later with PlayData
	iBufferPlay = reinterpret_cast<CMMFDataBuffer*>(aBuffer);
	TInt status = CreateChunk(iHwBufPckgFill, iBufferPlay->RequestSize());
	iHwBufPckgFill().iRequestSize = iBufferPlay->RequestSize();
	iHwBufPckgFill().iBufferSize = iBufferPlay->Data().MaxLength();
	iHwBufPckgFill().iLastBuffer = iBufferPlay->LastBuffer();
	TMMFDevSoundQueueItem queueItem;
	if ( status != KErrNone )
		{
		BufferErrorEvent();
		PlayError(status);
		}
	else
		{
		queueItem.iRequest = EMMFDevSoundProxyBTBFEvent;
		// assumes sufficient space in the queue so ignores the return value
		status = iMsgQueue.Send(queueItem);
		}

	SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::BufferToBeFilled - Exit [%d]"), status);
	}

//
// CMMFDevSoundSession::PlayError
// (other items were commented in a header).
//
void CMMFDevSoundSession::PlayError(TInt aError)
	{
    SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::PlayError - Enter [%d]"), aError);

	// Set play error flag to ignore following PlayData requests
	iPlayErrorOccured = ETrue;

	TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxyPEEvent;
	item.iErrorCode = aError;
	iChunk.Close();
	// assumes sufficient space in the queue so ignores the return value
	iMsgQueue.Send(item);
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::PlayError - Exit"));
	}

//
// CMMFDevSoundSession::BufferToBeEmptied
// (other items were commented in a header).
//
void CMMFDevSoundSession::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	// Store pointer to the buffer to use it later with RecordData
	iBufferRecord = reinterpret_cast<CMMFDataBuffer*>(aBuffer);
	TInt status = CreateChunk(iHwBufPckgEmpty, iBufferRecord->RequestSize());
	
	if ( status != KErrNone )
		{
		BufferErrorEvent();
		RecordError(status);
		}
	else
		{
		iHwBufPckgEmpty().iRequestSize = iBufferRecord->RequestSize();
		iHwBufPckgEmpty().iBufferSize = iBufferRecord->Data().MaxLength();
		iHwBufPckgEmpty().iLastBuffer = iBufferRecord->LastBuffer();
		//copy the data into the chunk    
		Mem::Copy(iChunk.Base(),iBufferRecord->Data().Ptr(),iBufferRecord->RequestSize());
	    TMMFDevSoundQueueItem queueItem;
		queueItem.iRequest = EMMFDevSoundProxyBTBEEvent;
		// assumes sufficient space in the queue so ignores the return value
		iMsgQueue.Send(queueItem);
		}
	}

// CMMFDevSoundSession::RecordError
// (other items were commented in a header).
//
void CMMFDevSoundSession::RecordError(TInt aError)
	{
	SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::Record Error [%d]"), aError);
	TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxyREEvent;
	item.iErrorCode = aError;
	iChunk.Close();
	// assumes sufficient space in the queue so ignores the return value
	iMsgQueue.Send(item);
	}

//
// CMMFDevSoundSession::DeviceMessage
// (other items were commented in a header).
//
void CMMFDevSoundSession::DeviceMessage(TUid /*aMessageType*/,
									const TDesC8& /*aMsg*/)
	{
	// Not used
	}

void CMMFDevSoundSession::InterfaceDeleted(TUid aInterfaceId)
	{
	MMMFDevSoundCustomInterfaceDeMuxPlugin* ptr = InterfaceFromUid(aInterfaceId);
	if (ptr == NULL)
		{
		// Not found
		return;
		}
	TRAPD(err, ptr->RefreshL());
	if (err != KErrNone)
		{
		// Refresh failed, so tear down Mux/DeMux pair
		TMMFEvent event;
		TMMFDevSoundQueueItem item;
		item.iRequest = EMMFDevSoundCustomCommandCloseMuxDemuxPair;
		item.iErrorCode = err;
		event.iEventType = aInterfaceId;
		item.iEventPckg() = event;
		iMsgQueue.Send(item);
		}
	}

//
// CMMFDevSoundSession::CallbackFromAdaptorReceived
// (other items were commented in a header).
//
void CMMFDevSoundSession::CallbackFromAdaptorReceived(TInt aType, TInt aError)
	{
    SYMBIAN_DEBPRN2(_L("CMMFDevSoundSession[0x%x]::CallbackFromAdaptorReceived - Enter. Type[%d] Error[%d]"), aType, aError);
	if(aType == KCallbackRecordPauseComplete)
		{
		TMMFDevSoundQueueItem item;
		item.iRequest = EMMFDevSoundProxyPausedRecordCompleteEvent;
		item.iErrorCode = KErrNone;
		TInt status = iMsgQueue.Send(item);
		}
	else if(aType == KCallbackAutoPauseResume)
		{
		TMMFEvent event;
		event.iErrorCode = KErrNone;
		event.iEventType = KMMFEventCategoryAudioResourceAvailable;
		SendEventToClient(event);
		//coverity[uninit_use_in_call]
		// Disabled Coverity warning, since it complains about iReserved1 member in TMMFEvent being uninitialised
		}
	else if (aType == KCallbackFlushComplete)
		{
		if(!iHandlingExtdCI && iRequestBeingServiced.Function()==EMMFDevSoundProxyEmptyBuffers)
			{
			//If we come here then it is due to a EMMFDevSoundProxyEmptyBuffers request from client.
			SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::CallbackFromAdaptorReceived - Calling TMMFDevSoundRequest::Complete on iRequestBeingServiced"));
		    iRequestBeingServiced.Complete(aError);
		    iOperationCompletePending = EFalse;
			}
		}
	else
		{
		if( NeedToQueue() )
			{
			// If not possible to service now, then queue request
			// Encapsule the request
			TMMFDevSoundRequest request(aType);
			// assumes sufficient space in the queue so ignores the return value
			iQueuedRequests.Insert(request, 0);
			}
		else
			{
			// If there is no oustanding operation service inmediately
			if (aType == KCallbackProcessingFinished)
				{
				DoProcessingFinished();
				}
			else if(aType == KCallbackProcessingUnitError)
				{
				DoProcessingError();
				}
			}
		}
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::CallbackFromAdaptorReceived - Exit"));
	}


//
// CMMFDevSoundSession::PreemptionStartedCallbackReceived
// (other items were commented in a header).
//
void CMMFDevSoundSession::PreemptionStartedCallbackReceived()
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::PreemptionStartedCallbackReceived - Enter"));
    // Solution: Enqueue any request that arrives before preemption is completed
	iOperationCompletePending = ETrue;
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::PreemptionStartedCallbackReceived - Exit"));
	}

//
// CMMFDevSoundSession::PreemptionFinishedCallbackReceived
// (other items were commented in a header).
//
void CMMFDevSoundSession::PreemptionFinishedCallbackReceived(TBool aCanStartNewOperation)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::PreemptionFinishedCallbackReceived - Enter"));
    if (iHandlingExtdCI)
        {
        // we are in the middle of handling a CI, so ignore - will handle later when unwinding
        SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::PreemptionFinishedCallbackReceived - Exit. Exiting from if block for CI"));
		return;
        }
	iOperationCompletePending = EFalse;
	if ( aCanStartNewOperation && iQueuedRequests.Count() != 0 )
		{
		DequeueRequest();
		}
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::PreemptionFinishedCallbackReceived - Exit"));
	}

//
// CMMFDevSoundSession::PreemptionClash
// (other items were commented in a header).
//
void CMMFDevSoundSession::PreemptionClash()
	{
	//assumes sufficient space in the queue so ignore the return value
	iQueuedRequests.Insert(iRequestBeingServiced, 0);
	iPreemptionClash=ETrue;
	}

//
// CMMFDevSoundSession::PreemptionClashWithStateChange
// (other items were commented in a header).
//
void CMMFDevSoundSession::PreemptionClashWithStateChange()
	{
	#ifdef _DEBUG
		TMMFDevSoundRequest msg = iQueuedRequests[0];
		// message being removed should be the one we previously pushed via PreemptionClash()
		__ASSERT_DEBUG(iRequestBeingServiced==msg, Panic(ERequestBeingServicedMismatch));
	#endif
	// remove without processing request with AsynchronousOperationComplete() completing the message
	iQueuedRequests.Remove(0);
	iPreemptionClash=EFalse;
	}

//
// CMMFDevSoundSession::AdaptorControlsContext()
//

TBool CMMFDevSoundSession::AdaptorControlsContext() const
    {
    return !iHandlingExtdCI;
    }

MMMFDevSoundCustomInterfaceDeMuxPlugin* CMMFDevSoundSession::InterfaceFromUid(TUid aUid)
	{
	TInt count = iCustomInterfaceArray.Count();
	TInt id = aUid.iUid;
	MMMFDevSoundCustomInterfaceDeMuxPlugin* interface = NULL;
	for (TInt i = 0; i < count; i++)
		{
		if (id == iCustomInterfaceArray[i].iId.iUid)
			{
			interface = iCustomInterfaceArray[i].iInterface;
			break;
			}
		}
	return interface;
	}

//
// CMMFDevSoundSession::SendEventToClient
// (other items were commented in a header).
//
void CMMFDevSoundSession::SendEventToClient(const TMMFEvent& aEvent)
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::SendEventToClient - Enter"));
    TMMFDevSoundQueueItem item;
	item.iRequest = EMMFDevSoundProxySETCEvent;
	item.iErrorCode = KErrNone;
	item.iEventPckg() = aEvent;
	// assumes sufficient space in the queue so ignores the return value
	TInt err = iMsgQueue.Send(item);
    __ASSERT_DEBUG(err == KErrNone, Panic(EMsgQueueFailedToSendMsg));
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::SendEventToClient - Exit"));
	}

void CMMFDevSoundSession::AsynchronousOperationComplete(TInt aError, TBool aCanStartNewOperation)
	{
    __ASSERT_DEBUG(!iHandlingExtdCI, Panic(EUnexpectedAsyncOpCompleteHandlingCI));
        // when handling CIs we should not reach here

    TInt error = aError;
    if (!error)
        {
        // if have no error payload, use notified error. It will be KErrNone if not set, so just use.
        error = NotifiedError();
        }

	switch (iRequestBeingServiced.Type())
		{
		case TMMFDevSoundRequest::ESessionEvents:
			{
			SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x] ==== ClosingDueException ==== "));
			iOperationCompletePending = EFalse;
			if(iClosingWait->IsStarted())
				{
				iClosingWait->AsyncStop();
				}
			return;
			}
		// Complete the message for asynchronous requests
		case TMMFDevSoundRequest::EConfigure_Asynchronous:
		case TMMFDevSoundRequest::EAction_Asynchronous:
		case TMMFDevSoundRequest::EQuery_Asynchronous:
		case TMMFDevSoundRequest::ECustomInterfacesRelated:
			{
			if(iOperationCompletePending && aCanStartNewOperation)
				{
				if (iRequestBeingServiced.Function()==EMMFDevSoundProxyStop)
					{
					// flush the queue - will have removed any stale items added between initial call and MMRC's reaction
                    FlushQueuedRequests();
					FlushEventQueue();
					iChunk.Close();
					}
					
				if(iRequestBeingServiced.Function()==EMMFDevSoundProxyCapabilities)
					{
					TMMFDevSoundProxySettings devSoundSet;
					devSoundSet.iCaps = iDevSoundCapabilities;
					TMMFDevSoundProxySettingsPckg pckg(devSoundSet);
					MessageWrite(iRequestBeingServiced.Message(),TInt(2),pckg);
					}
				
				if(iRequestBeingServiced.Function()==EMMFDevSoundProxyCancelInitialize)
					{
					FlushEventQueue();
					}
				
				iRequestBeingServiced.Complete(error);
				iOperationCompletePending = EFalse;
				}
			}
			break;
		case TMMFDevSoundRequest::EAction_PseudoAsynchronous:
			{
			if(iOperationCompletePending && aCanStartNewOperation)
				{
				iOperationCompletePending = EFalse;
				}
			}
			break;
		case TMMFDevSoundRequest::EQuery_Synchronous:
		case TMMFDevSoundRequest::EConfigure_Synchronous:
		case TMMFDevSoundRequest::EBufferExchangeRelated:
			break;
		case TMMFDevSoundRequest::ECallBackType:
			{
			if(iOperationCompletePending && aCanStartNewOperation)
				{
				iOperationCompletePending = EFalse;
				}	
			}
			break;
		default:
			break;
		}
	
	if(iRequestBeingServiced.Type() == TMMFDevSoundRequest::ECallBackType )
	    {	    
	    SYMBIAN_DEBPRN2(_L("CMMFDevSoundSession[0x%x] AsynchronousOperationComplete CallbackPF=%d pending=%d"),
	            iRequestBeingServiced.IsCallBack(), iOperationCompletePending );
	    }
	else
	    {
	    SYMBIAN_DEBPRN3(_L("CMMFDevSoundSession[0x%x] AsynchronousOperationComplete %x pending=%d Requestype=%d"),
	            iRequestBeingServiced.Function(), iOperationCompletePending, iRequestBeingServiced.Type() );
	    }

	    
	if ( aCanStartNewOperation && iQueuedRequests.Count() != 0 )
		{
		DequeueRequest();
		}
	}

void CMMFDevSoundSession::DequeueRequest()
	{
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DequeueRequest - Enter"));
    iAsyncQueueStart->Cancel(); // if we're in here cancel any background request

	TMMFDevSoundRequest msg = iQueuedRequests[0];

	if (msg.IsCallBack() > 0)
		{
		iRequestBeingServiced.SetMessageCallback();
		//Call iPf function
		SYMBIAN_DEBPRN0(_L("\n CMMFDevSoundSession[0x%x] ======== Service a queued request\n"));
		if (msg.IsCallBack() == KCallbackProcessingFinished)
			{
			iQueuedRequests.Remove(0);
			DoProcessingFinished();
			}
		else if(msg.IsCallBack() == KCallbackProcessingUnitError)
		    {
		    iQueuedRequests.Remove(0);
		    DoProcessingError();
		    }

		}

	if (iQueuedRequests.Count()>0)
		{
		// Some rules about what request can be followed
		SYMBIAN_DEBPRN0(_L("\n CMMFDevSoundSession[0x%x]======== Flag can service new request\n"));
		iAsyncQueueStart->CallBack();
		}
	SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::DequeueRequest - Exit"));
	}
	
// 	AsyncQueueStartCallback


TInt CMMFDevSoundSession::AsyncQueueStartCallback(TAny* aPtr)
	{
	CMMFDevSoundSession* self = static_cast<CMMFDevSoundSession*>(aPtr);
	self->AsyncQueueStartCallback();
	return KErrNone;
	}
	
void CMMFDevSoundSession::AsyncQueueStartCallback()
	{
	SYMBIAN_DEBPRN0(_L("\n CMMFDevSoundSession[0x%x]======== Service a queued request\n"));
	TMMFDevSoundRequest msg = iQueuedRequests[0];
	iQueuedRequests.Remove(0);
	TInt err = KErrNone;
	TBool doRequest = ETrue;
	if(iPreemptionClash)
		{
		SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::AsyncQueueStartCallback - Re-applying client request due to Pre-emption clash"));
		iPreemptionClash=EFalse; // clear flag before reading next request in queue
		iAdapter->RollbackAdaptorActiveStateToBeforeCommit();
		if ((iRequestBeingServiced.Type() == TMMFDevSoundRequest::EAction_PseudoAsynchronous))
			{
		    doRequest = EFalse;
		    HandleAlreadyCompletedRequest();
			}
		}

	if (doRequest)
		{
		TRAP(err,DoServiceRequestL(msg.Message()));
		if(err != KErrNone)
			{
			msg.Complete(err);
			}
		}

	if (!iOperationCompletePending && iQueuedRequests.Count() != 0)
		{
		//dequeue next
		DequeueRequest();
		}
	}

// CMMFDevSoundSession::CustomInterface()
// Returns a pointer reference to custom interface implementation returned by
// adaptation::CustomInterface method.
// Note this method is called indirectly by CI server-side plugins - both DeMux and 
// CIServerExtension call this variously via MMMFDevSoundCustomInterfaceTarget or MCustomInterface 
//
TAny* CMMFDevSoundSession::CustomInterface(TUid aInterfaceId)
	{
	TInt err = DoSetClientConfig(); // if required, this will connect to MMRC etc
	if (err)
		{
		return NULL; // on any error, return NULL - not much more we can do
		}
	return iAdapter->CustomInterface(aInterfaceId);
	}

//
// CMMFDevSoundSession::DoProcessingFinished()
//
void CMMFDevSoundSession::DoProcessingFinished()
	{
    ResetNotifiedError();

	TBool asyncOperation = EFalse;
	//ProcessingFinished should never fail
	__ASSERT_ALWAYS(KErrNone, iAdapter->ProcessingFinishedReceived(asyncOperation));
	iOperationCompletePending = asyncOperation;
	if (iOperationCompletePending)
		{
		iRequestBeingServiced.SetMessageCallback();
		}
	}

//
// CMMFDevSoundSession::DoProcessingError()
//
void CMMFDevSoundSession::DoProcessingError()
    {
    ResetNotifiedError();

    TBool asyncOperation = EFalse;
    //ProcessingFinished should never fail
    __ASSERT_ALWAYS(KErrNone, iAdapter->ProcessingError(asyncOperation));
    iOperationCompletePending = asyncOperation;
    if (iOperationCompletePending)
        {
        iRequestBeingServiced.SetMessageCallback();
        }
    }

//
// CMMFDevSoundSession::DoSetClientConfigL()
// Sets client configuration information to Adaptation.
//
TInt CMMFDevSoundSession::DoSetClientConfig()
	{
	TInt err = KErrNone;
	if(!iSetClientConfigApplied)
		{
		CMMFDevSoundServer* server =
			const_cast<CMMFDevSoundServer*>(
				static_cast<const CMMFDevSoundServer*>(Server()));
			
		ASSERT(server); // session should always have a server!
		
		TProcessId actualProcessId = server->ActualProcessId();
		TProcessId processId = server->ProcessId();
		
		if (actualProcessId!=processId)
		    {
            // we have a differing actual process id, so pass that to the adaptor too
            err = iAdapter->SetClientConfig(actualProcessId, processId);
		    }
		else
		    {
            err = iAdapter->SetClientConfig(processId);
		    }

		if (!err)
			{
			iSetClientConfigApplied = ETrue;
			}
		}
	return err;
	}

// CMMFDevSoundSession::DoSetClientConfigL()
// Sets client configuration information to Adaptation.
//
void CMMFDevSoundSession::DoSetClientConfigL()
	{
	User::LeaveIfError(DoSetClientConfig());
	}

// 
// CMMFDevSoundSession::CreateChunk()
// Requests kernel to create global RChunk
// 
TInt CMMFDevSoundSession::CreateChunk(TMMFDevSoundProxyHwBufPckg& aBufPckg, TInt aRequestedSize)
	{
	TInt status(KErrNone);
	
	if ( iChunk.Handle() )
		{
		// If the DevSound Adaptation component requests a buffer size
		// that can fit into current chunk's size, then re-use chunk.
		if ( aRequestedSize <= iChunk.Size() )
			{
			if (iForceSendOfChunkHandle)
				{
				iForceSendOfChunkHandle = EFalse;
				aBufPckg().iChunkOp = EOpen;
				}
			else
				{
				aBufPckg().iChunkOp = ENull;
				}
			return status;
			}
		// The new request size exceeds the current chunk's area, close it. We
		// will be creating new one in the following sequences. Note we could
		// try to Adjust() the chunk, and see if the existing chunk could be
		// extended instead, but this is assumed too rare an event for this 
		// optimisation
		else
			{
			iChunk.Close();
			}
		}
	
	// Request kernel to create global RChunk if needed
	if ( !iChunk.Handle() )
		{
		status = iChunk.CreateGlobal(KNullDesC, aRequestedSize, aRequestedSize, EOwnerThread);
		if ( status == KErrNone )
			{
			aBufPckg().iChunkOp = EOpen;
			}
		else
			{
			aBufPckg().iChunkOp = ENull;
			}
		}
	iForceSendOfChunkHandle = EFalse;
	return status;
	}


// Custom Interface //
TInt CMMFDevSoundSession::DoOpenSlaveL(TUid aInterface, const TDesC8& aPackageBuf)
	{
	// it shouldn't be necessary to check if we have already instantiated this
	// interface since the client would already know - however this is something
	// that a licensee could implement if they required additional functionality
	// e.g. many : 1 mappings between client and DevSound.

	MMMFDevSoundCustomInterfaceDeMuxPlugin* ptr = NULL;
		
	// try and instantiate a plugin tunnelling
	// pair to support this Custom Interface
	ptr = iDeMuxUtility->CreateCustomInterfaceDeMuxL(aInterface);
	
	TInt handle = KNullHandle;
	
	if (ptr)
		{
		TMMFDevSoundCustomInterfaceDeMuxData data;
		data.iInterface = ptr;
		data.iId = aInterface;
			
		CleanupReleasePushL(*ptr);
			
		// setup demux plugin
		ptr->SetInterfaceTarget(this);
			
		// try and open interface
		// this will fetch the interface from the svr implementation
		ptr->DoOpenSlaveL(aInterface, aPackageBuf);
		User::LeaveIfError(iCustomInterfaceArray.Append(data));
			
		CleanupStack::Pop();	// ptr
			
		handle = iCustomInterfaceArray.Count();
		return handle;
		}

	// we couldn't set up the interface correctly so return a NULL
	// handle to the client
	return KNullHandle;
	}
	
void CMMFDevSoundSession::DoCloseSlaveL(TInt aHandle)
	{
	if (aHandle==KNullHandle)
		{
		// null-handle -> NOP
		return;
		}
		
	if (aHandle<KNullHandle || aHandle > iCustomInterfaceArray.Count())
		{
		// handle out of range - should not happen, but leave to show error
		User::Leave(KErrBadHandle);
		}
		
	TMMFDevSoundCustomInterfaceDeMuxData& data = iCustomInterfaceArray[aHandle-1];
	
	// close and delete the plugin
	MMMFDevSoundCustomInterfaceDeMuxPlugin* ptr = data.iInterface;
	ptr->DoCloseSlaveL(aHandle);
	ptr->Release();
	
	// clear the entry
	data.iInterface = NULL;
	data.iId.iUid = 0;
	}
	
TInt CMMFDevSoundSession::DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage)
	{
	// use the demux utility to get the handle
	TMMFDevSoundCIMessageData data;
	iDeMuxUtility->GetSyncMessageDataL(aMessage, data);
	
	TInt handle = data.iHandle;
	
	if ((handle <= 0) || (handle > (iCustomInterfaceArray.Count())))
		{
		
		User::Leave(KErrBadHandle);
		}
	
	// call on demux plugin
	return iCustomInterfaceArray[handle-1].iInterface->DoSendSlaveSyncCommandL(aMessage);	
	}
	
TInt CMMFDevSoundSession::DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	// use the demux utility to get the handle
	TMMFDevSoundCIMessageData data;
	iDeMuxUtility->GetSyncMessageDataL(aMessage, data);
	
	TInt handle = data.iHandle;
	
	if ((handle <= 0) || (handle > (iCustomInterfaceArray.Count())))
		{
		
		User::Leave(KErrBadHandle);
		}
	
	// call on demux plugin
	return iCustomInterfaceArray[handle-1].iInterface->DoSendSlaveSyncCommandResultL(aMessage);	
	}
	
void CMMFDevSoundSession::DoSendSlaveAsyncCommandL(const RMmfIpcMessage& aMessage)
	{
	// use the demux utility to get the handle
	TMMFDevSoundCIMessageData data;
	iDeMuxUtility->GetAsyncMessageDataL(aMessage, data);
	
	TInt handle = data.iHandle;
	
	if ((handle <= 0) || (handle > (iCustomInterfaceArray.Count())))
		{
		User::Leave(KErrBadHandle);
		}
	
	// call on demux plugin
	iCustomInterfaceArray[handle-1].iInterface->DoSendSlaveAsyncCommandL(aMessage);	
	}
	
void CMMFDevSoundSession::DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& aMessage)
	{
	// use the demux utility to get the handle
	TMMFDevSoundCIMessageData data;
	iDeMuxUtility->GetAsyncMessageDataL(aMessage, data);
	
	TInt handle = data.iHandle;
	
	if ((handle <= 0) || (handle > (iCustomInterfaceArray.Count())))
		{
		User::Leave(KErrBadHandle);
		}
	
	// call on demux plugin
	iCustomInterfaceArray[handle-1].iInterface->DoSendSlaveAsyncCommandResultL(aMessage);	
	}


TBool CMMFDevSoundSession::DoRegisterAsClientL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	User::LeaveIfError(MessageRead(aMessage,0,buf));
	HBufC8* notificationRegistrationData = NULL;
	notificationRegistrationData = HBufC8::NewLC(User::LeaveIfError(aMessage.GetDesLengthL(1)));
	TPtr8 dataPtr(notificationRegistrationData->Des());  	
	User::LeaveIfError(MessageRead(aMessage,1,dataPtr));
	DoSetClientConfigL();// added here instead of the CreateL()
	TInt err = KErrNone;
	err = iAdapter->RegisterAsClient(buf().iNotificationEventUid,dataPtr);
	CleanupStack::PopAndDestroy(1); // Notification Registeration data
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	return ETrue;
	}
	
TBool CMMFDevSoundSession::DoCancelRegisterAsClientL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	User::LeaveIfError(MessageRead(aMessage,0,buf));
	TInt err = KErrNone;
	err = iAdapter->CancelRegisterAsClient(buf().iNotificationEventUid);
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	return ETrue;
	}

TBool CMMFDevSoundSession::DoGetResourceNotificationDataL(const RMmfIpcMessage& aMessage)
	{
	TMMFDevSoundProxySettingsPckg buf;
	User::LeaveIfError(MessageRead(aMessage,0,buf));
	HBufC8* notificationData = NULL;
	notificationData = HBufC8::NewLC(User::LeaveIfError(aMessage.GetDesMaxLengthL(2)));
	TPtr8 dataPtr(notificationData->Des());  	
	User::LeaveIfError(MessageRead(aMessage,2,dataPtr));
	TInt err = KErrNone;
	err = iAdapter->GetResourceNotificationData(buf().iNotificationEventUid,dataPtr);
	User::LeaveIfError(MessageWrite(aMessage,2,*notificationData));
	CleanupStack::PopAndDestroy(1); // Notification data
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	return ETrue;
	}

TBool CMMFDevSoundSession::DoWillResumePlayL(const RMmfIpcMessage& aMessage)
	{
	TInt err = KErrNone;
	err = iAdapter->WillResumePlay();
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return EFalse;		
		}
	return ETrue;
	}

TBool CMMFDevSoundSession::DoSetClientThreadInfoL(const RMmfIpcMessage& aMessage)
	{
	if(!iSetClientConfigApplied)
		{
		if (aMessage.HasCapability(ECapabilityMultimediaDD) && aMessage.HasCapability(ECapabilityUserEnvironment))
			{
			TPckgBuf<TThreadId> threadId;
			User::LeaveIfError(MessageRead(aMessage, 1, threadId));
			
			CMMFDevSoundServer* server = 
				const_cast<CMMFDevSoundServer*>(static_cast<const CMMFDevSoundServer*>(Server()));
			server->SetClientProcessIdL(threadId()); 
			}
		else
			{
			User::Leave(KErrPermissionDenied);
			}
		}
	else
		{
		User::Leave(KErrNotReady);
		}
	return ETrue;
	}
	
void CMMFDevSoundSession::Panic(TMMFDevSoundSessionPanicCodes aCode)
	{
    User::Panic(KMMFDevSoundSessionPanicCategory, aCode);
	}

void CMMFDevSoundSession::BufferErrorEvent()
	{
	// this will generate an processing error event and callback
	iAdapter->BufferErrorEvent();
	}

void CMMFDevSoundSession::FlushQueuedRequests()
    {
    for (TInt queueIndex = (iQueuedRequests.Count() - 1); queueIndex >= 0; --queueIndex)
        {
        if ((iQueuedRequests[queueIndex].Type() == TMMFDevSoundRequest::ESessionEvents) && 
                (iQueuedRequests[queueIndex].Function() == RMessage2::EDisConnect))
            {
            continue;
            }
        iQueuedRequests.Remove(queueIndex);
        }    
    }

TInt CMMFDevSoundSession::MessageRead(const RMmfIpcMessage& aMessage, TInt aParam, TDes8& aResult)
    {
    if (!iDisconnecting)
        {
        return MmfMessageUtil::Read(aMessage, aParam, aResult);    
        }
    return KErrBadHandle;
    }

TInt CMMFDevSoundSession::MessageRead(const RMmfIpcMessage& aMessage, TInt aParam, TDes16& aResult)
    {
    if (!iDisconnecting)
        {
        return aMessage.Read(aParam, aResult);    
        }
    return KErrBadHandle;
    }

TInt CMMFDevSoundSession::MessageWrite(const RMmfIpcMessage& aMessage, TInt aParam, const TDesC8& aValue)
    {
    if (!iDisconnecting)
        {
        return MmfMessageUtil::Write(aMessage, aParam, aValue);
        }
    return KErrBadHandle;
    }

void CMMFDevSoundSession::ResetNotifiedError()
// called at beginning of commit cycle, so any error will be from callbacks
    {
    SYMBIAN_DEBPRN0(_L("CMMFDevSoundSession[0x%x]::ResetNotifiedError"));
    iNotifiedError = KErrNone;
    }

TInt CMMFDevSoundSession::NotifiedError() const
// NotifiedError property
    {
    SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::NotifiedError(%d)"), iNotifiedError);
    return iNotifiedError;
    }

void CMMFDevSoundSession::NotifyError(TInt aError)
// cache notified error
    {
    SYMBIAN_DEBPRN1(_L("CMMFDevSoundSession[0x%x]::NotifyError(%d)"), aError);
    iNotifiedError = aError;
    }

// End of file
