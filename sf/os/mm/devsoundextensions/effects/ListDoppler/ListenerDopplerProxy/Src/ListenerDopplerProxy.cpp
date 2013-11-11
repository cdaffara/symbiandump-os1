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
* Description:   Implementation of the Listener Doppler Proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "ListenerDopplerProxy.h"
#include "ListenerDopplerEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CListenerDopplerProxy::CListenerDopplerProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CListenerDopplerProxy::CListenerDopplerProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CListenerDopplerProxy::~CListenerDopplerProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iDopplerEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CListenerDopplerProxy::NewL
// Static function for creating an instance of the ListenerDoppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDopplerProxy* CListenerDopplerProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CListenerDopplerProxy* self = new (ELeave) CListenerDopplerProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CListenerDopplerProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CListenerDopplerProxy::ConstructL()
    {
    iDopplerEventObserver = CListenerDopplerEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();

    TEfDopplerDataPckg dataPckgFrom;
    // sends a message to fetch initial data.
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EListenerDopplerOfInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);
	}


// -----------------------------------------------------------------------------
// CListenerDopplerProxy::ApplyL
// Apply the ListenerDoppler settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CListenerDopplerProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerDopplerProxy::Apply"));
#endif

	if (iHaveUpdateRights )
		{
		iDopplerData.iEnabled = iEnabled;
		iDopplerData.iEnforced = iEnforced;
		iDopplerData.iHaveUpdateRights = iHaveUpdateRights;

		iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EListenerDopplerOfApply, DoEffectData(), KNullDesC8);
		}
	else
		{
		User::Leave(KErrAccessDenied);
		}
	}

// -----------------------------------------------------------------------------
// CListenerDopplerProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CListenerDopplerProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerDopplerProxy::StartObserver"));
#endif

	iDopplerEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CListenerDopplerProxy::DopplerEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CListenerDopplerProxy::ListenerDopplerEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerDopplerProxy::ListenerDopplerEvent"));
#endif

	TEfDopplerDataPckg dataPckgFrom;
	dataPckgFrom.Copy(aBuffer);
	TEfDoppler newDopplerData = dataPckgFrom();

	TUint8 event = 0;

	if ( newDopplerData.iEnabled != iDopplerData.iEnabled )
		{
		iDopplerData.iEnabled = newDopplerData.iEnabled;
		iEnabled = newDopplerData.iEnabled;
		if ( iDopplerData.iEnabled )
			{
			event = MAudioEffectObserver::KEnabled;
			}
		else
			{
			event = MAudioEffectObserver::KDisabled;
			}
		}
	else if ( newDopplerData.iEnforced != iDopplerData.iEnforced )
		{
		iDopplerData.iEnforced = newDopplerData.iEnforced;
		iEnforced = newDopplerData.iEnforced;
		if ( iDopplerData.iEnforced )
			{
			event = MAudioEffectObserver::KEnforced;
			}
		else
			{
			event = MAudioEffectObserver::KNotEnforced;
			}
		}
	else if ( newDopplerData.iHaveUpdateRights != iDopplerData.iHaveUpdateRights )
		{
		iDopplerData.iHaveUpdateRights = newDopplerData.iHaveUpdateRights;
		iHaveUpdateRights = newDopplerData.iHaveUpdateRights;
		if ( iDopplerData.iHaveUpdateRights )
			{
			event = MAudioEffectObserver::KGainedUpdateRights;
			}
		else
			{
			event = MAudioEffectObserver::KLostUpdateRights;
			}
		}

		else if ( newDopplerData.iVelocityX != iDopplerData.iVelocityX )
			{
			iDopplerData.iVelocityX = newDopplerData.iVelocityX;
			event = MListenerDopplerObserver::KCartesianVelocityChanged;
			}
		else if ( newDopplerData.iVelocityY != iDopplerData.iVelocityY )
			{
			iDopplerData.iVelocityY = newDopplerData.iVelocityY;
			event = MListenerDopplerObserver::KCartesianVelocityChanged;
			}
		else if ( newDopplerData.iVelocityZ != iDopplerData.iVelocityZ )
			{
			iDopplerData.iVelocityZ = newDopplerData.iVelocityZ;
			event = MListenerDopplerObserver::KCartesianVelocityChanged;
			}
		else if ( newDopplerData.iAzimuth != iDopplerData.iAzimuth )
			{
			iDopplerData.iAzimuth = newDopplerData.iAzimuth;
			event = MListenerDopplerObserver::KSphericalVelocityChanged;
			}
		else if ( newDopplerData.iElevation != iDopplerData.iElevation )
			{
			iDopplerData.iElevation = newDopplerData.iElevation;
			event = MListenerDopplerObserver::KSphericalVelocityChanged;
			}
		else if ( newDopplerData.iRadius != iDopplerData.iRadius )
			{
			iDopplerData.iRadius = newDopplerData.iRadius;
			event = MListenerDopplerObserver::KSphericalVelocityChanged;
			}
		else if ( newDopplerData.iFactor != iDopplerData.iFactor )
			{
			iDopplerData.iFactor = newDopplerData.iFactor;
			event = MListenerDopplerObserver::KFactorChanged;
			}
		else if ( newDopplerData.iMaxFactor != iDopplerData.iMaxFactor )
			{
			iDopplerData.iMaxFactor = newDopplerData.iMaxFactor;
			event = MListenerDopplerObserver::KMaxFactorChanged;
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
