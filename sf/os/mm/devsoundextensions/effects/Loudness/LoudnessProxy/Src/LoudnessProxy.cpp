/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the Loudness proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "LoudnessProxy.h"
#include "LoudnessEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLoudnessProxy::CLoudnessProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLoudnessProxy::CLoudnessProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CLoudnessProxy::~CLoudnessProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)    
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iLoudnessEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CLoudnessProxy::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudnessProxy* CLoudnessProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CLoudnessProxy* self = new (ELeave) CLoudnessProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CLoudnessProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CLoudnessProxy::ConstructL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessProxy::ConstructL"));
#endif
    iLoudnessEventObserver = CLoudnessEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();
    // sends a message to fetch initial data.
    TEfLoudnessDataPckg dataPckgFrom;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ELefInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);
	}


// -----------------------------------------------------------------------------
// CLoudnessProxy::ApplyL
// Apply the Loudness settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CLoudnessProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessProxy::Apply"));
#endif

	if ( !iHaveUpdateRights )
		{
		User::Leave(KErrAccessDenied);
		}

	iLoudnessData.iEnabled = iEnabled;
	iLoudnessData.iEnforced = iEnforced;
	iLoudnessData.iHaveUpdateRights = iHaveUpdateRights;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ELefApply, DoEffectData(), KNullDesC8);
	}

// -----------------------------------------------------------------------------
// CLoudnessProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CLoudnessProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessProxy::StartObserver"));
#endif

	iLoudnessEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CLoudnessProxy::LoudnessEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CLoudnessProxy::LoudnessEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessProxy::LoudnessEvent"));
#endif

	TEfLoudnessDataPckg dataPckgFrom;
	dataPckgFrom.Copy(aBuffer);
	TEfLoudnessData newLoudnessData = dataPckgFrom();

	TUint8 event = 0;

		if ( newLoudnessData.iEnabled != iLoudnessData.iEnabled )
			{
			iLoudnessData.iEnabled = newLoudnessData.iEnabled;
			iEnabled = newLoudnessData.iEnabled;
			if ( iLoudnessData.iEnabled )
				{
				event = MAudioEffectObserver::KEnabled;
				}
			else
				{
				event = MAudioEffectObserver::KDisabled;
				}
			}
		else if ( newLoudnessData.iEnforced != iLoudnessData.iEnforced )
			{
			iLoudnessData.iEnforced = newLoudnessData.iEnforced;
			iEnforced = newLoudnessData.iEnforced;
			if ( iLoudnessData.iEnforced )
				{
				event = MAudioEffectObserver::KEnforced;
				}
			else
				{
				event = MAudioEffectObserver::KNotEnforced;
				}
			}
		else if ( newLoudnessData.iHaveUpdateRights != iLoudnessData.iHaveUpdateRights )
			{
			iLoudnessData.iHaveUpdateRights = newLoudnessData.iHaveUpdateRights;
			iHaveUpdateRights = newLoudnessData.iHaveUpdateRights;
			if ( iLoudnessData.iHaveUpdateRights )
				{
				event = MAudioEffectObserver::KGainedUpdateRights;
				}
			else
				{
				event = MAudioEffectObserver::KLostUpdateRights;
				}
			}

	if (!event)
		return;

	for ( TInt i = 0; i < iObservers.Count(); i++ )
		{
		iObservers[i]->EffectChanged(this, event);
		}
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File


