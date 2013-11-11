/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   AudioOutput Proxy Active Object
*
*/


#include "AudioOutput.h"
#include "AudioOutputMessageTypes.h"
#include <e32svr.h>
#include "AudioOutputProxyAO.h"
#include <CustomCommandUtility.h>
#include <MAudioOutputObserver.h>


// Two-phased constructor.
CAudioOutputProxyAO* CAudioOutputProxyAO::NewL(CAudioOutput *aOutputProxy,MAudioOutputObserver& aObserver,MCustomCommand* aUtility)
	{
	CAudioOutputProxyAO* self = new(ELeave) CAudioOutputProxyAO(aOutputProxy,aObserver,aUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// Destructor
CAudioOutputProxyAO::~CAudioOutputProxyAO()
	{
	Cancel();
	}

// C++ default constructor can NOT contain any code, that
// might leave.
//
CAudioOutputProxyAO::CAudioOutputProxyAO(CAudioOutput *aOutputProxy,MAudioOutputObserver& aObserver,MCustomCommand* aUtility):
	CActive(CActive::EPriorityStandard),
		iAudioOutputProxy(aOutputProxy),
		iCustomCommandUtility(aUtility),
		iObserver(&aObserver)
	{
	}

void CAudioOutputProxyAO::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

// ---------------------------------------------------------
// CAudioOutputProxyAO::RunL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputProxyAO::RunL()
	{
#ifdef _DEBUG
	RDebug::Print(_L("CAudioOutputProxyAO::RunL"));
#endif
	if ( iRegistered != EFalse )
		{
		iCustomCommandUtility->CustomCommandAsync( *iDestination,iFunction,KNullDesC8,KNullDesC8,iCallbackData,iStatus );
		SetActive();
		iObserver->DefaultAudioOutputChanged(*iAudioOutputProxy,iCallbackData());
		}
	}

// ---------------------------------------------------------
// CAudioOutputProxyAO::DoCancel
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputProxyAO::DoCancel()
	{
       if ( iRegistered != EFalse )
       	{
       	iCustomCommandUtility->CustomCommandSync( *iDestination, EAofUnregisterObserver, KNullDesC8, KNullDesC8);
       	}
	}

// ---------------------------------------------------------
// CAudioOutputProxyAO::SendAsyncMessage
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputProxyAO::SendAsyncMessage(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt aFunction)
	{
	iDestination = &aDestination;
	iFunction = aFunction;
	iCustomCommandUtility->CustomCommandAsync( aDestination,aFunction,KNullDesC8,KNullDesC8,iCallbackData,iStatus );
	SetActive();
	}


// ---------------------------------------------------------
// CAudioOutputProxyAO::SetRegisterFlag
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputProxyAO::SetRegisterFlag(TBool aFlag)
	{
	iRegistered = aFlag;
	}
// ---------------------------------------------------------
// CAudioOutputProxyAO::SetObserver
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioOutputProxyAO::SetObserver(MAudioOutputObserver& aObserver)
    {
    iObserver = &aObserver;
    }
