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

#include "ListenerOrientationProxy.h"
#include "ListenerOrientationEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CListenerOrientationProxy::CListenerOrientationProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CListenerOrientationProxy::CListenerOrientationProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CListenerOrientationProxy::~CListenerOrientationProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iListenerOrientationEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CListenerOrientationProxy::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientationProxy* CListenerOrientationProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CListenerOrientationProxy* self = new (ELeave) CListenerOrientationProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CListenerOrientationProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CListenerOrientationProxy::ConstructL()
    {
    iListenerOrientationEventObserver = CListenerOrientationEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();

    TEfOrientationDataPckg dataPckgFrom;
    // sends a message to fetch initial data.
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ELofInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);
	}


// -----------------------------------------------------------------------------
// CListenerOrientationProxy::ApplyL
// Apply the bassboost settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CListenerOrientationProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerOrientationProxy::Apply"));
#endif

	if (iHaveUpdateRights )
		{
		iOrientationData.iEnabled = iEnabled;
		iOrientationData.iEnforced = iEnforced;
		iOrientationData.iHaveUpdateRights = iHaveUpdateRights;

		iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ELofApply, DoEffectData(), KNullDesC8);
		}
	else
		{
		User::Leave(KErrAccessDenied);
		}
	}

// -----------------------------------------------------------------------------
// CListenerOrientationProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CListenerOrientationProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerOrientationProxy::StartObserver"));
#endif

	iListenerOrientationEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CListenerOrientationProxy::ListenerOrientationEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CListenerOrientationProxy::ListenerOrientationEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerOrientationProxy::ListenerOrientationEvent"));
#endif

	TEfOrientationDataPckg dataPckgFrom;
	dataPckgFrom.Copy(aBuffer);
	TEfOrientation newOrientationData = dataPckgFrom();

	TUint8 event = 0;

	if ( newOrientationData.iEnabled != iOrientationData.iEnabled )
		{
		iOrientationData.iEnabled = newOrientationData.iEnabled;
		iEnabled = newOrientationData.iEnabled;
		if ( iOrientationData.iEnabled )
			{
			event = MAudioEffectObserver::KEnabled;
			}
		else
			{
			event = MAudioEffectObserver::KDisabled;
			}
		}
	else if ( newOrientationData.iEnforced != iOrientationData.iEnforced )
		{
		iOrientationData.iEnforced = newOrientationData.iEnforced;
		iEnforced = newOrientationData.iEnforced;
		if ( iOrientationData.iEnforced )
			{
			event = MAudioEffectObserver::KEnforced;
			}
		else
			{
			event = MAudioEffectObserver::KNotEnforced;
			}
		}
	else if ( newOrientationData.iHaveUpdateRights != iOrientationData.iHaveUpdateRights )
		{
		iOrientationData.iHaveUpdateRights = newOrientationData.iHaveUpdateRights;
		iHaveUpdateRights = newOrientationData.iHaveUpdateRights;
		if ( iOrientationData.iHaveUpdateRights )
			{
			event = MAudioEffectObserver::KGainedUpdateRights;
			}
		else
			{
			event = MAudioEffectObserver::KLostUpdateRights;
			}
		}
	else if ( newOrientationData.iHeading != iOrientationData.iHeading )
			{
			iOrientationData.iHeading = newOrientationData.iHeading;
			event = MListenerOrientationObserver::KOrientationChanged;
			}
	else if ( newOrientationData.iPitch != iOrientationData.iPitch )
			{
			iOrientationData.iPitch = newOrientationData.iPitch;
			event = MListenerOrientationObserver::KOrientationChanged;
			}
	else if ( newOrientationData.iRoll != iOrientationData.iRoll )
			{
			iOrientationData.iRoll = newOrientationData.iRoll;
			event = MListenerOrientationObserver::KOrientationChanged;
			}
	else if ( newOrientationData.iFrontX != iOrientationData.iFrontX )
			{
			iOrientationData.iFrontX = newOrientationData.iFrontX;
			event = MListenerOrientationObserver::KOrientationVectorsChanged;
			}
	else if ( newOrientationData.iFrontY != iOrientationData.iFrontY )
			{
			iOrientationData.iFrontY = newOrientationData.iFrontY;
			event = MListenerOrientationObserver::KOrientationVectorsChanged;
			}
	else if ( newOrientationData.iFrontZ != iOrientationData.iFrontZ )
			{
			iOrientationData.iFrontZ = newOrientationData.iFrontZ;
			event = MListenerOrientationObserver::KOrientationVectorsChanged;
			}
	else if ( newOrientationData.iAboveX != iOrientationData.iAboveX )
			{
			iOrientationData.iAboveX = newOrientationData.iAboveX;
			event = MListenerOrientationObserver::KOrientationVectorsChanged;
			}
	else if ( newOrientationData.iFrontY != iOrientationData.iAboveY )
			{
			iOrientationData.iAboveY = newOrientationData.iAboveY;
			event = MListenerOrientationObserver::KOrientationVectorsChanged;
			}
	else if ( newOrientationData.iAboveZ != iOrientationData.iAboveZ )
			{
			iOrientationData.iAboveZ = newOrientationData.iAboveZ;
			event = MListenerOrientationObserver::KOrientationVectorsChanged;
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

