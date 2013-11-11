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
* Description: Audio output msg handler implementation
*
*/



// INCLUDE FILES
#include    "AudioOutput.h"
#include    "AudioOutputMessageHandler.h"
#include    "AudioOutputMessageTypes.h"
#include	"MAudioOutputObserver.h"

//reason for defining this secure id here but not using from the mmfbase.hrh is that 
//lot of macros in mmfbase.hrh collide with base.hrh
#define KUidMmfDrmPluginServerDefine            0x10283439

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CAudioOutputMessageHandler::CAudioOutputMessageHandler(CAudioOutput*        aAudioOutput,
                                                       CMMFObjectContainer& aContainer) :
	CMMFObject(KUidAudioOutput),
	iContainer(aContainer)
    {
    iAudioOutput = aAudioOutput;
    }

// Two-phased constructor.
EXPORT_C CAudioOutputMessageHandler* CAudioOutputMessageHandler::NewL(TAny*                aCustomInterface,
                                                             CMMFObjectContainer& aContainer)
    {
    CAudioOutput* audioOutput = (CAudioOutput*)aCustomInterface;
    CAudioOutputMessageHandler* self = new (ELeave) CAudioOutputMessageHandler(audioOutput,
                                                                               aContainer);

	self->ConstructL();
    return self;
    }

    
// Destructor
CAudioOutputMessageHandler::~CAudioOutputMessageHandler()
	{
	delete iAudioOutput;
	delete iCallbackMessage;
	}


// ---------------------------------------------------------
// CAudioOutputMessageHandler::AudioOutputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TUid CAudioOutputMessageHandler::Uid()
	{
	return KUidAudioOutput;
	}

// ---------------------------------------------------------
// CAudioOutputMessageHandler::SetAudioOutputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputMessageHandler::HandleRequest(TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidAudioOutput);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CAudioOutputMessageHandler::DoHandleRequestL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputMessageHandler::DoHandleRequestL(TMMFMessage& aMessage)
	{

	switch(aMessage.Function())
		{
		case EAofDelete:
			{
			DoDeleteL(aMessage);
			break;
			}
		case EAofRegisterObserver:
			{
			iCallbackMessage = new (ELeave) TMMFMessage( aMessage );
			DoRegisterObserverL();
			break;
			}
		case EAofSetAudioOutput:
			{
			DoSetAudioOutputL(aMessage);
			break;
			}
		case EAofGetAudioOutput:
			{
			DoGetAudioOutputL(aMessage);
			break;
			}			
		case EAofSetSecureOutput:
			{
			DoSetSecureOutputL(aMessage);
			break;
			}
		case EAofUnregisterObserver:
			{
			DoUnregisterObserverL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

// ---------------------------------------------------------
// CAudioOutputMessageHandler::DoDeleteL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputMessageHandler::DoDeleteL(TMMFMessage& aMessage)
	{
	aMessage.Complete(KErrNone);
	iContainer.RemoveAndDestroyMMFObject(*this);
	}

// ---------------------------------------------------------
// CAudioOutputMessageHandler::DoRegisterObserverL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputMessageHandler::DoRegisterObserverL()
	{
	iAudioOutput->RegisterObserverL(*this);
	}

// ---------------------------------------------------------
// CAudioOutputMessageHandler::DoSetAudioOutputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputMessageHandler::DoSetAudioOutputL(TMMFMessage& aMessage)
	{
	TPckgBuf<CAudioOutput::TAudioOutputPreference> outputPckg;
	aMessage.ReadData1FromClient(outputPckg);
	CAudioOutput::TAudioOutputPreference output = outputPckg();
	iAudioOutput->SetAudioOutputL(output);
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CAudioOutputMessageHandler::DoSetSecureOutputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputMessageHandler::DoSetSecureOutputL(TMMFMessage& aMessage)
	{
	//Since we allow the creation of CAudioOutput from Custom interface builder. 
	//We have to make sure that this message is blocked in case we are running in Secure DRM process
	const TSecureId KSecureDRMSID(KUidMmfDrmPluginServerDefine);
	RThread currentThread;
	RProcess currentProcess;
	CleanupClosePushL(currentThread);
    CleanupClosePushL(currentProcess);
	User::LeaveIfError(currentThread.Process(currentProcess));
	TSecureId curProcessSID = currentProcess.SecureId();
	CleanupStack::PopAndDestroy();//calls currentThread.Close()
	CleanupStack::PopAndDestroy();//calls currentProcess.Close()
	if (curProcessSID == KSecureDRMSID)
	    {
	    //since we are in secure DRM process, completing request with KErrPermissionDenied.
	    aMessage.Complete(KErrPermissionDenied);
	    return;
	    }
	TPckgBuf<TBool> secureOutputPckg;
	aMessage.ReadData1FromClient(secureOutputPckg);
	TBool secureOutput = secureOutputPckg();
	iAudioOutput->SetSecureOutputL(secureOutput);
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CAudioOutputMessageHandler::DoUnregisterObserverL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputMessageHandler::DoUnregisterObserverL(TMMFMessage& aMessage)
	{
	iAudioOutput->UnregisterObserver(*this);
	if ( iCallbackMessage )
		{
		if ( !iCallbackMessage->IsCompleted() )
			{
			iCallbackMessage->Complete(KErrNone);
			}
		}
	aMessage.Complete(KErrNone);
	delete iCallbackMessage;
	iCallbackMessage = NULL;
	}


// ---------------------------------------------------------
// CAudioOutputMessageHandler::DefaultAudioOutputChanged
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputMessageHandler::DefaultAudioOutputChanged( CAudioOutput& /*aAudioOutput*/,
		                                CAudioOutput::TAudioOutputPreference aNewDefault )
	{
	TPckgBuf<CAudioOutput::TAudioOutputPreference> outputPckg(aNewDefault);

	if ( iCallbackMessage )
		{
		if ( !iCallbackMessage->IsCompleted() )
			{
			iCallbackMessage->WriteDataToClient( outputPckg );
			iCallbackMessage->Complete( KErrNone );
			}
		}
	delete iCallbackMessage;
	iCallbackMessage = NULL;
	}


// -----------------------------------------------------------------------------
// CAudioOutputMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioOutputMessageHandler::ConstructL()
	{
	}

// ---------------------------------------------------------
// CAudioOutputMessageHandler::DoSetAudioOutputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputMessageHandler::DoGetAudioOutputL(TMMFMessage& aMessage)
	{
#ifdef _DEBUG
    RDebug::Print(_L("CAudioOutputMessageHandler::DoGetAudioOutputL"));
#endif
	CAudioOutput::TAudioOutputPreference outPut = iAudioOutput->AudioOutput();
	TPckgBuf<CAudioOutput::TAudioOutputPreference> outPutPckg(outPut);
	aMessage.WriteDataToClient(outPutPckg);
	aMessage.Complete(KErrNone);
	}
// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
