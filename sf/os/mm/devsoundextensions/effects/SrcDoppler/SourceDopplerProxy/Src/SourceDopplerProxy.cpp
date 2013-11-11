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
* Description:   Implementation of the Source Doppler Proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "SourceDopplerProxy.h"
#include "SourceDopplerEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSourceDopplerProxy::CSourceDopplerProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSourceDopplerProxy::CSourceDopplerProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CSourceDopplerProxy::~CSourceDopplerProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iDopplerEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CSourceDopplerProxy::NewL
// Static function for creating an instance of the SourceDoppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDopplerProxy* CSourceDopplerProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CSourceDopplerProxy* self = new (ELeave) CSourceDopplerProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSourceDopplerProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CSourceDopplerProxy::ConstructL()
    {
    iDopplerEventObserver = CSourceDopplerEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();

    TEfDopplerDataPckg dataPckgFrom;
    // sends a message to fetch initial data.
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ESourceDopplerOfInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);
	}


// -----------------------------------------------------------------------------
// CSourceDopplerProxy::ApplyL
// Apply the SourceDoppler settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSourceDopplerProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceDopplerProxy::Apply"));
#endif

	if (iHaveUpdateRights )
		{
		iDopplerData.iEnabled = iEnabled;
		iDopplerData.iEnforced = iEnforced;
		iDopplerData.iHaveUpdateRights = iHaveUpdateRights;

		iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ESourceDopplerOfApply, DoEffectData(), KNullDesC8);
		}
	else
		{
		User::Leave(KErrAccessDenied);
		}
	}

// -----------------------------------------------------------------------------
// CSourceDopplerProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CSourceDopplerProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceDopplerProxy::StartObserver"));
#endif

	iDopplerEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CSourceDopplerProxy::DopplerEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CSourceDopplerProxy::SourceDopplerEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceDopplerProxy::SourceDopplerEvent"));
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
			event = MSourceDopplerObserver::KCartesianVelocityChanged;
			}
		else if ( newDopplerData.iVelocityY != iDopplerData.iVelocityY )
			{
			iDopplerData.iVelocityY = newDopplerData.iVelocityY;
			event = MSourceDopplerObserver::KCartesianVelocityChanged;
			}
		else if ( newDopplerData.iVelocityZ != iDopplerData.iVelocityZ )
			{
			iDopplerData.iVelocityZ = newDopplerData.iVelocityZ;
			event = MSourceDopplerObserver::KCartesianVelocityChanged;
			}
		else if ( newDopplerData.iAzimuth != iDopplerData.iAzimuth )
			{
			iDopplerData.iAzimuth = newDopplerData.iAzimuth;
			event = MSourceDopplerObserver::KSphericalVelocityChanged;
			}
		else if ( newDopplerData.iElevation != iDopplerData.iElevation )
			{
			iDopplerData.iElevation = newDopplerData.iElevation;
			event = MSourceDopplerObserver::KSphericalVelocityChanged;
			}
		else if ( newDopplerData.iRadius != iDopplerData.iRadius )
			{
			iDopplerData.iRadius = newDopplerData.iRadius;
			event = MSourceDopplerObserver::KSphericalVelocityChanged;
			}
		else if ( newDopplerData.iFactor != iDopplerData.iFactor )
			{
			iDopplerData.iFactor = newDopplerData.iFactor;
			event = MSourceDopplerObserver::KFactorChanged;
			}
		else if ( newDopplerData.iMaxFactor != iDopplerData.iMaxFactor )
			{
			iDopplerData.iMaxFactor = newDopplerData.iMaxFactor;
			event = MSourceDopplerObserver::KMaxFactorChanged;
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

