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
* Description: Audio input msg handler implementation
*
*/



// INCLUDE FILES
#include    "AudioInputMessageHandler.h"
#include    "AudioInputMessageTypes.h"
#include    "AudioInput.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CAudioInputMessageHandler::CAudioInputMessageHandler(CAudioInput*         aAudioInput,
                                                     CMMFObjectContainer& aContainer) :
	CMMFObject(KUidAudioInput),
	iContainer(aContainer)
    {
    iAudioInput = aAudioInput;
    }

// Two-phased constructor.
EXPORT_C CAudioInputMessageHandler* CAudioInputMessageHandler::NewL(TAny*                aCustomInterface,
                                                           CMMFObjectContainer& aContainer)
    {
    CAudioInput* audioInput = (CAudioInput*)aCustomInterface;
    CAudioInputMessageHandler* self = new (ELeave) CAudioInputMessageHandler(audioInput,
                                                                             aContainer);
    
    return self;
    }

    
// Destructor
CAudioInputMessageHandler::~CAudioInputMessageHandler()
	{
	delete iAudioInput;
	}


// ---------------------------------------------------------
// CAudioInput::AudioInput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TUid CAudioInputMessageHandler::Uid()
	{
	return KUidAudioInput;
	}

// ---------------------------------------------------------
// CAudioInput::SetAudioInputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioInputMessageHandler::HandleRequest(TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidAudioInput);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CAudioInput::SetAudioInputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioInputMessageHandler::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EAifDelete:
			{
			DoDeleteL(aMessage);
			break;
			}
		case EAifSetInputs:
			{
			DoSetInputsL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

// ---------------------------------------------------------
// CAudioInput::SetAudioInputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioInputMessageHandler::DoDeleteL(TMMFMessage& aMessage)
	{
	aMessage.Complete(KErrNone);
	iContainer.RemoveAndDestroyMMFObject(*this);
	}

// ---------------------------------------------------------
// CAudioInput::SetAudioInputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioInputMessageHandler::DoSetInputsL(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> countPckg;
	aMessage.ReadData1FromClient(countPckg);
	TInt count = countPckg();
	
	CArrayFixFlat<CAudioInput::TAudioInputPreference>* inputArray =
		new(ELeave) CArrayFixFlat<CAudioInput::TAudioInputPreference>(4);
	CleanupStack::PushL(inputArray);
	inputArray->ResizeL(count);
	
	TInt length = count * inputArray->Length();
	TPtr8 inputArrayPtr((TUint8*)&(*inputArray)[0],length, length);
	aMessage.ReadData2FromClient(inputArrayPtr);

	iAudioInput->SetAudioInputL(inputArray->Array());
	CleanupStack::PopAndDestroy(inputArray);//inputArray
	
	aMessage.Complete(KErrNone);
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
