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

#include "SourceLocationProxy.h"
#include "SourceLocationEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSourceLocationProxy::CSourceLocationProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSourceLocationProxy::CSourceLocationProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CSourceLocationProxy::~CSourceLocationProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iSourceLocationEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CSourceLocationProxy::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocationProxy* CSourceLocationProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CSourceLocationProxy* self = new (ELeave) CSourceLocationProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSourceLocationProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CSourceLocationProxy::ConstructL()
    {
    iSourceLocationEventObserver = CSourceLocationEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();

    TEfLocationDataPckg dataPckgFrom;
    // sends a message to fetch initial data.
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ESlfInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);
	}


// -----------------------------------------------------------------------------
// CSourceLocationProxy::ApplyL
// Apply the bassboost settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSourceLocationProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceLocationProxy::Apply"));
#endif

	if (iHaveUpdateRights )
		{
		iLocationData.iEnabled = iEnabled;
		iLocationData.iEnforced = iEnforced;
		iLocationData.iHaveUpdateRights = iHaveUpdateRights;

		iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ESlfApply, DoEffectData(), KNullDesC8);
		}
	else
		{
		User::Leave(KErrAccessDenied);
		}
	}

// -----------------------------------------------------------------------------
// CSourceLocationProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CSourceLocationProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceLocationProxy::StartObserver"));
#endif

	iSourceLocationEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CSourceLocationProxy::SourceLocationEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CSourceLocationProxy::SourceLocationEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceLocationProxy::SourceLocationEvent"));
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
			event = MSourceLocationObserver::KLocationCartesianChanged;
			}
	else if ( newLocationData.iYCoordinate != iLocationData.iYCoordinate )
			{
			iLocationData.iYCoordinate = newLocationData.iYCoordinate;
			event = MSourceLocationObserver::KLocationCartesianChanged;
			}
	else if ( newLocationData.iZCoordinate != iLocationData.iZCoordinate )
			{
			iLocationData.iZCoordinate = newLocationData.iZCoordinate;
			event = MSourceLocationObserver::KLocationCartesianChanged;
			}
	else if ( newLocationData.iAzimuth != iLocationData.iAzimuth )
			{
			iLocationData.iAzimuth = newLocationData.iAzimuth;
			event = MSourceLocationObserver::KLocationSphericalChanged;
			}
	else if ( newLocationData.iElevation != iLocationData.iElevation )
			{
			iLocationData.iElevation = newLocationData.iElevation;
			event = MSourceLocationObserver::KLocationSphericalChanged;
			}
	else if ( newLocationData.iRadius != iLocationData.iRadius )
			{
			iLocationData.iRadius = newLocationData.iRadius;
			event = MSourceLocationObserver::KLocationSphericalChanged;
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
