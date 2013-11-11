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
* Description: Audio output proxy implementation
*
*/



// INCLUDE FILES
#include "AudioOutput.h"
#include "AudioOutputProxyAO.h"
#include "AudioOutputProxy.h"
#include "AudioOutputMessageTypes.h"
#include <CustomCommandUtility.h>
#include "CustomInterfaceUtility.h"



    
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CAudioOutputProxy::CAudioOutputProxy(TMMFMessageDestinationPckg aMessageHandler,
						MCustomCommand& aCustomCommand,
	       					CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(&aCustomCommand),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    iOutput        = ENoPreference;
    iDefaultOutput = ENoPreference;
    iSecureOutput  = EFalse;
    iRegistered = EFalse;
    }

// Two-phased constructor.
EXPORT_C CAudioOutputProxy* CAudioOutputProxy::NewL(TMMFMessageDestinationPckg aMessageHandler, 
							MCustomCommand& aCustomCommand,
							CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CAudioOutputProxy* self = new(ELeave) CAudioOutputProxy(aMessageHandler,aCustomCommand,aCustomInterfaceUtility);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CAudioOutputProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioOutputProxy::ConstructL()
	{
	}

// Destructor
CAudioOutputProxy::~CAudioOutputProxy()
    {
    	if (iRegistered != EFalse)
    	{
    	    if(iObserver)
    		    UnregisterObserver(*iObserver);
    	}
       
        if (iCustomCommand)
        {
            iCustomCommand->CustomCommandSync(iMessageHandler, EAofDelete, KNullDesC8, KNullDesC8);
    	}
    	
    	if(iAsyncSender)
    	{    
    	    delete iAsyncSender;
    	    iAsyncSender = NULL;
    	}
    	
    	if(iCustomInterfaceUtility)
    	{
    	    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    	    delete iCustomInterfaceUtility;
            iCustomInterfaceUtility = NULL;
        }
    }

// ---------------------------------------------------------
// CAudioOutput::Uid
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TUid CAudioOutputProxy::Uid()
	{
	return KUidAudioOutput;
	}

// ---------------------------------------------------------
// CAudioOutput::AudioOutput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAudioOutput::TAudioOutputPreference CAudioOutputProxy::AudioOutput()
	{

	TPckgBuf<TAudioOutputPreference> outPutPckg;
	TInt error = iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EAofGetAudioOutput, KNullDesC8, KNullDesC8, outPutPckg);
	
	if (KErrNone == error)
	   iOutput = outPutPckg();
	else
       iOutput = ENoPreference;	
	
	return iOutput;
	}
// ---------------------------------------------------------
// CAudioOutput::DefaultAudioOutput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAudioOutput::TAudioOutputPreference CAudioOutputProxy::DefaultAudioOutput()
	{
	return iDefaultOutput;
	}
// ---------------------------------------------------------
// CAudioOutput::RegisterObserverL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputProxy::RegisterObserverL(MAudioOutputObserver& aObserver)
	{
	if(iRegistered)
		{
	       iObserver = &aObserver;
	       iAsyncSender->SetObserver(aObserver);
	       }
	else
		{
		iRegistered = ETrue;
		iObserver = &aObserver;
		delete iAsyncSender;
		iAsyncSender = NULL;

		iAsyncSender = CAudioOutputProxyAO::NewL(this,aObserver,iCustomCommand);
		iAsyncSender->SetRegisterFlag(ETrue);
		iAsyncSender->SendAsyncMessage(iMessageHandler,EAofRegisterObserver);
		}
	}

// ---------------------------------------------------------
// CAudioOutput::SecureOutput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CAudioOutputProxy::SecureOutput()
	{
	return iSecureOutput;
	}

// ---------------------------------------------------------
// CAudioOutput::SetAudioOutputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputProxy::SetAudioOutputL(TAudioOutputPreference aAudioOutput)
	{
	iOutput = aAudioOutput;
	TPckgC<TAudioOutputPreference> outputPckg(aAudioOutput);
	iCustomCommand->CustomCommandSync(iMessageHandler, EAofSetAudioOutput, outputPckg, KNullDesC8);
	}
// ---------------------------------------------------------
// CAudioOutput::SetAudioOutputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputProxy::SetSecureOutputL(TBool aSecureOutput)
	{
	iSecureOutput = aSecureOutput;
	TPckgC<TBool> outputPckg(aSecureOutput);
        TInt err = KErrNone;
        //ou1cimx1#454515 CAudioOutputConfigurator::SetSecureOutputL() is deprecated
	err = iCustomCommand->CustomCommandSync(iMessageHandler, EAofSetSecureOutput, outputPckg, KNullDesC8);
	if(err != KErrNone)
		{
			User::Leave(err);
		}
	
	}

// ---------------------------------------------------------
// CAudioOutput::RegisterObserverL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputProxy::UnregisterObserver(MAudioOutputObserver& aObserver)
	{
	iRegistered = EFalse;
	if (iObserver == &aObserver)
		{
		if (iAsyncSender)
			{
			iAsyncSender->SetRegisterFlag(EFalse);
			iCustomCommand->CustomCommandSync(iMessageHandler, EAofUnregisterObserver, KNullDesC8, KNullDesC8);
			iObserver = NULL;
			}
		}
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
