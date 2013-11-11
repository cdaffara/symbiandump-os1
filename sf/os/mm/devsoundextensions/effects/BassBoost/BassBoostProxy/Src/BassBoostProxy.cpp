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
* Description:   Implementation of the bassboost proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "BassBoostProxy.h"
#include "BassBoostEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBassBoostProxy::CBassBoostProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBassBoostProxy::CBassBoostProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CBassBoostProxy::~CBassBoostProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iBassBoostEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CBassBoostProxy::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoostProxy* CBassBoostProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CBassBoostProxy* self = new (ELeave) CBassBoostProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CBassBoostProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CBassBoostProxy::ConstructL()
    {
    iBassBoostEventObserver = CBassBoostEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();

    TEfBassBoostDataPckg dataPckgFrom;
    // sends a message to fetch initial data.
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EBfInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);
	}


// -----------------------------------------------------------------------------
// CBassBoostProxy::ApplyL
// Apply the bassboost settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CBassBoostProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CBassBoostProxy::Apply"));
#endif

	if (iHaveUpdateRights )
		{
		iBassBoostData.iEnabled = iEnabled;
		iBassBoostData.iEnforced = iEnforced;
		iBassBoostData.iHaveUpdateRights = iHaveUpdateRights;

		iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EBfApply, DoEffectData(), KNullDesC8);
		}
	else
		{
		User::Leave(KErrAccessDenied);
		}
	}

// -----------------------------------------------------------------------------
// CBassBoostProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CBassBoostProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CBassBoostProxy::StartObserver"));
#endif

	iBassBoostEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CBassBoostProxy::BassBoostEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CBassBoostProxy::BassBoostEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CBassBoostProxy::BassBoostEvent"));
#endif

	TEfBassBoostDataPckg dataPckgFrom;
	dataPckgFrom.Copy(aBuffer);
	TEfBassBoostData newBassBoostData = dataPckgFrom();

	TUint8 event = 0;

	if ( newBassBoostData.iEnabled != iBassBoostData.iEnabled )
		{
		iBassBoostData.iEnabled = newBassBoostData.iEnabled;
		iEnabled = newBassBoostData.iEnabled;
		if ( iBassBoostData.iEnabled )
			{
			event = MAudioEffectObserver::KEnabled;
			}
		else
			{
			event = MAudioEffectObserver::KDisabled;
			}
		}
	else if ( newBassBoostData.iEnforced != iBassBoostData.iEnforced )
		{
		iBassBoostData.iEnforced = newBassBoostData.iEnforced;
		iEnforced = newBassBoostData.iEnforced;
		if ( iBassBoostData.iEnforced )
			{
			event = MAudioEffectObserver::KEnforced;
			}
		else
			{
			event = MAudioEffectObserver::KNotEnforced;
			}
		}
	else if ( newBassBoostData.iHaveUpdateRights != iBassBoostData.iHaveUpdateRights )
		{
		iBassBoostData.iHaveUpdateRights = newBassBoostData.iHaveUpdateRights;
		iHaveUpdateRights = newBassBoostData.iHaveUpdateRights;
		if ( iBassBoostData.iHaveUpdateRights )
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


