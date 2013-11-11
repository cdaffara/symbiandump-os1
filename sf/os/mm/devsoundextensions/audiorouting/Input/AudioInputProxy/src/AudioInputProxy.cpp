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
* Description: Audio input proxy implementation
*
*/



// INCLUDE FILES
#include "AudioInputProxy.h"
#include "AudioInputMessageTypes.h"
#include <CustomCommandUtility.h>
#include <CustomInterfaceUtility.h>



    
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CAudioInputProxy::CAudioInputProxy(TMMFMessageDestinationPckg aMessageHandler, 
												MCustomCommand& aCustomCommand,
												CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(&aCustomCommand),
	iInputArray(4),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }

// Two-phased constructor.
EXPORT_C CAudioInputProxy* CAudioInputProxy::NewL(TMMFMessageDestinationPckg aMessageHandler, 
												MCustomCommand& aCustomCommand,
												CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CAudioInputProxy* self = new(ELeave) CAudioInputProxy(aMessageHandler,aCustomCommand,aCustomInterfaceUtility);

    return self;
    }

    
// Destructor
CAudioInputProxy::~CAudioInputProxy()
    {
  
    	iInputArray.Reset();
        if(iCustomCommand)
            iCustomCommand->CustomCommandSync(iMessageHandler, EAifDelete, KNullDesC8, KNullDesC8);
        if(iCustomInterfaceUtility)
          iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
        delete iCustomInterfaceUtility;
    }

EXPORT_C const TUid CAudioInputProxy::Uid()
	{
	return KUidAudioInput;
	}

// ---------------------------------------------------------
// CAudioInput::AudioInput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAudioInput::TAudioInputArray CAudioInputProxy::AudioInput()
	{
	return iInputArray.Array();
	}

// ---------------------------------------------------------
// CAudioInput::SetAudioInputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioInputProxy::SetAudioInputL(TAudioInputArray aAudioInputs)
	{
	iInputArray.Reset();
	TInt count = aAudioInputs.Count();
	
	
	for(int i=0; i<count; i++)
		{
		TAudioInputPreference audioInput = aAudioInputs[i];
		iInputArray.AppendL(audioInput);		
		}
		
	TInt length = iInputArray.Length() * count;
	TPtrC8 inputArrayPtr((const TUint8*)&iInputArray[0], length);
	TPckgC<TInt> countPckg(count);
	
	User::LeaveIfError(iCustomCommand->CustomCommandSync(iMessageHandler, EAifSetInputs, countPckg, inputArrayPtr));
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================




// End of File
