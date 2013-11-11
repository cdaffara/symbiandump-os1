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
* Description:   Implementation of the Distance Attenuation proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "DistanceAttenuationProxy.h"
#include "DistanceAttenuationEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDistanceAttenuationProxy::CDistanceAttenuationProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDistanceAttenuationProxy::CDistanceAttenuationProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CDistanceAttenuationProxy::~CDistanceAttenuationProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iDistanceAttenuationEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuationProxy::NewL
// Static function for creating an instance of the Distance Attenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuationProxy* CDistanceAttenuationProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CDistanceAttenuationProxy* self = new (ELeave) CDistanceAttenuationProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuationProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CDistanceAttenuationProxy::ConstructL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationProxy::ConstructL"));
#endif
    iDistanceAttenuationEventObserver = CDistanceAttenuationEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();
    // sends a message to fetch initial data.
    TEfDistanceAttenuationDataPckg dataPckgFrom;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EDaefInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);
	}


// -----------------------------------------------------------------------------
// CDistanceAttenuationProxy::ApplyL
// Apply the Distance Attenuation settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CDistanceAttenuationProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationProxy::Apply"));
#endif

	if ( !iHaveUpdateRights )
		{
		User::Leave(KErrAccessDenied);
		}

	iDistanceAttenuationData.iEnabled = iEnabled;
	iDistanceAttenuationData.iEnforced = iEnforced;
	iDistanceAttenuationData.iHaveUpdateRights = iHaveUpdateRights;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EDaefApply, DoEffectData(), KNullDesC8);
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuationProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CDistanceAttenuationProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationProxy::StartObserver"));
#endif

	iDistanceAttenuationEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuationProxy::DistanceAttenuationEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CDistanceAttenuationProxy::DistanceAttenuationEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationProxy::DistanceAttenuationEvent"));
#endif

	TEfDistanceAttenuationDataPckg dataPckgFrom;
	dataPckgFrom.Copy(aBuffer);
	TEfDistanceAttenuation newDistanceAttenuationData = dataPckgFrom();

	TUint8 event = 0;

		if ( newDistanceAttenuationData.iEnabled != iDistanceAttenuationData.iEnabled )
			{
			iDistanceAttenuationData.iEnabled = newDistanceAttenuationData.iEnabled;
			iEnabled = newDistanceAttenuationData.iEnabled;
			if ( iDistanceAttenuationData.iEnabled )
				{
				event = MAudioEffectObserver::KEnabled;
				}
			else
				{
				event = MAudioEffectObserver::KDisabled;
				}
			}
		else if ( newDistanceAttenuationData.iEnforced != iDistanceAttenuationData.iEnforced )
			{
			iDistanceAttenuationData.iEnforced = newDistanceAttenuationData.iEnforced;
			iEnforced = newDistanceAttenuationData.iEnforced;
			if ( iDistanceAttenuationData.iEnforced )
				{
				event = MAudioEffectObserver::KEnforced;
				}
			else
				{
				event = MAudioEffectObserver::KNotEnforced;
				}
			}
		else if ( newDistanceAttenuationData.iHaveUpdateRights != iDistanceAttenuationData.iHaveUpdateRights )
			{
			iDistanceAttenuationData.iHaveUpdateRights = newDistanceAttenuationData.iHaveUpdateRights;
			iHaveUpdateRights = newDistanceAttenuationData.iHaveUpdateRights;
			if ( iDistanceAttenuationData.iHaveUpdateRights )
				{
				event = MAudioEffectObserver::KGainedUpdateRights;
				}
			else
				{
				event = MAudioEffectObserver::KLostUpdateRights;
				}
			}

		else if ( newDistanceAttenuationData.iRMin != iDistanceAttenuationData.iRMin )
			{
			iDistanceAttenuationData.iRMin = newDistanceAttenuationData.iRMin;
			event = MDistanceAttenuationObserver::KDistanceAttenuationChanged;
			}
		else if ( newDistanceAttenuationData.iRMax != iDistanceAttenuationData.iRMax )
			{
			iDistanceAttenuationData.iRMax = newDistanceAttenuationData.iRMax;
			event = MDistanceAttenuationObserver::KDistanceAttenuationChanged;
			}
		else if ( newDistanceAttenuationData.iMuteAfterMax != iDistanceAttenuationData.iMuteAfterMax )
			{
			iDistanceAttenuationData.iMuteAfterMax = newDistanceAttenuationData.iMuteAfterMax;
			event = MDistanceAttenuationObserver::KDistanceAttenuationChanged;
			}
		else if ( newDistanceAttenuationData.iRollOffFactor != iDistanceAttenuationData.iRollOffFactor )
			{
			iDistanceAttenuationData.iRollOffFactor = newDistanceAttenuationData.iRollOffFactor;
			event = MDistanceAttenuationObserver::KDistanceAttenuationChanged;
			}
		else if ( newDistanceAttenuationData.iRoomRollOffFactor != iDistanceAttenuationData.iRoomRollOffFactor )
			{
			iDistanceAttenuationData.iRoomRollOffFactor = newDistanceAttenuationData.iRoomRollOffFactor;
			event = MDistanceAttenuationObserver::KDistanceAttenuationChanged;
			}
		else if ( newDistanceAttenuationData.iRollOffFactorMax != iDistanceAttenuationData.iRollOffFactorMax )
			{
			iDistanceAttenuationData.iRollOffFactorMax = newDistanceAttenuationData.iRollOffFactorMax;
			event = MDistanceAttenuationObserver::KDistanceAttenuationChanged;
			}
		else if ( newDistanceAttenuationData.iRoomRollOffFactorMax != iDistanceAttenuationData.iRoomRollOffFactorMax )
			{
			iDistanceAttenuationData.iRoomRollOffFactorMax = newDistanceAttenuationData.iRoomRollOffFactorMax;
			event = MDistanceAttenuationObserver::KDistanceAttenuationChanged;
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

