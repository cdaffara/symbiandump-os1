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

#include "ListenerLocationProxy.h"
#include "ListenerLocationEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CListenerLocationProxy::CListenerLocationProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CListenerLocationProxy::CListenerLocationProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CListenerLocationProxy::~CListenerLocationProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iListenerLocationEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CListenerLocationProxy::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocationProxy* CListenerLocationProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CListenerLocationProxy* self = new (ELeave) CListenerLocationProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CListenerLocationProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CListenerLocationProxy::ConstructL()
    {
    iListenerLocationEventObserver = CListenerLocationEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();

    TEfLocationDataPckg dataPckgFrom;
    // sends a message to fetch initial data.
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ELlfInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);
	}


// -----------------------------------------------------------------------------
// CListenerLocationProxy::ApplyL
// Apply the bassboost settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CListenerLocationProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerLocationProxy::Apply"));
#endif

	if (iHaveUpdateRights )
		{
		iLocationData.iEnabled = iEnabled;
		iLocationData.iEnforced = iEnforced;
		iLocationData.iHaveUpdateRights = iHaveUpdateRights;

		iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ELlfApply, DoEffectData(), KNullDesC8);
		}
	else
		{
		User::Leave(KErrAccessDenied);
		}
	}

// -----------------------------------------------------------------------------
// CListenerLocationProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CListenerLocationProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerLocationProxy::StartObserver"));
#endif

	iListenerLocationEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CListenerLocationProxy::ListenerLocationEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CListenerLocationProxy::ListenerLocationEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerLocationProxy::ListenerLocationEvent"));
#endif

	TEfLocationDataPckg dataPckgFrom;
	dataPckgFrom.Copy(aBuffer);
	TEfLocation newLocationData = dataPckgFrom();

	TUint8 event = 0;

	if ( newLocationData.iEnabled != iLocationData.iEnabled )
		{
		iLocationData.iEnabled = newLocationData.iEnabled;
		iEnabled = newLocationData.iEnabled;
		if ( iLocationData.iEnabled )
			{
			event = MAudioEffectObserver::KEnabled;
			}
		else
			{
			event = MAudioEffectObserver::KDisabled;
			}
		}
	else if ( newLocationData.iEnforced != iLocationData.iEnforced )
		{
		iLocationData.iEnforced = newLocationData.iEnforced;
		iEnforced = newLocationData.iEnforced;
		if ( iLocationData.iEnforced )
			{
			event = MAudioEffectObserver::KEnforced;
			}
		else
			{
			event = MAudioEffectObserver::KNotEnforced;
			}
		}
	else if ( newLocationData.iHaveUpdateRights != iLocationData.iHaveUpdateRights )
		{
		iLocationData.iHaveUpdateRights = newLocationData.iHaveUpdateRights;
		iHaveUpdateRights = newLocationData.iHaveUpdateRights;
		if ( iLocationData.iHaveUpdateRights )
			{
			event = MAudioEffectObserver::KGainedUpdateRights;
			}
		else
			{
			event = MAudioEffectObserver::KLostUpdateRights;
			}
		}
	else if ( newLocationData.iXCoordinate != iLocationData.iXCoordinate )
			{
			iLocationData.iXCoordinate = newLocationData.iXCoordinate;
			event = MListenerLocationObserver::KLocationCartesianChanged;
			}
	else if ( newLocationData.iYCoordinate != iLocationData.iYCoordinate )
			{
			iLocationData.iYCoordinate = newLocationData.iYCoordinate;
			event = MListenerLocationObserver::KLocationCartesianChanged;
			}
	else if ( newLocationData.iZCoordinate != iLocationData.iZCoordinate )
			{
			iLocationData.iZCoordinate = newLocationData.iZCoordinate;
			event = MListenerLocationObserver::KLocationCartesianChanged;
			}
	else if ( newLocationData.iAzimuth != iLocationData.iAzimuth )
			{
			iLocationData.iAzimuth = newLocationData.iAzimuth;
			event = MListenerLocationObserver::KLocationSphericalChanged;
			}
	else if ( newLocationData.iElevation != iLocationData.iElevation )
			{
			iLocationData.iElevation = newLocationData.iElevation;
			event = MListenerLocationObserver::KLocationSphericalChanged;
			}
	else if ( newLocationData.iRadius != iLocationData.iRadius )
			{
			iLocationData.iRadius = newLocationData.iRadius;
			event = MListenerLocationObserver::KLocationSphericalChanged;
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

