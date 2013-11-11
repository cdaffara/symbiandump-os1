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

#include "SourceOrientationProxy.h"
#include "SourceOrientationEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSourceOrientationProxy::CSourceOrientationProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSourceOrientationProxy::CSourceOrientationProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CSourceOrientationProxy::~CSourceOrientationProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iSourceOrientationEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CSourceOrientationProxy::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientationProxy* CSourceOrientationProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CSourceOrientationProxy* self = new (ELeave) CSourceOrientationProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSourceOrientationProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CSourceOrientationProxy::ConstructL()
    {
    iSourceOrientationEventObserver = CSourceOrientationEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();

    TEfOrientationDataPckg dataPckgFrom;
    // sends a message to fetch initial data.
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ESofInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);
	}


// -----------------------------------------------------------------------------
// CSourceOrientationProxy::ApplyL
// Apply the bassboost settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSourceOrientationProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceOrientationProxy::Apply"));
#endif

	if (iHaveUpdateRights )
		{
		iOrientationData.iEnabled = iEnabled;
		iOrientationData.iEnforced = iEnforced;
		iOrientationData.iHaveUpdateRights = iHaveUpdateRights;

		iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ESofApply, DoEffectData(), KNullDesC8);
		}
	else
		{
		User::Leave(KErrAccessDenied);
		}
	}

// -----------------------------------------------------------------------------
// CSourceOrientationProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CSourceOrientationProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceOrientationProxy::StartObserver"));
#endif

	iSourceOrientationEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CSourceOrientationProxy::SourceOrientationEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CSourceOrientationProxy::SourceOrientationEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceOrientationProxy::SourceOrientationEvent"));
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
			event = MSourceOrientationObserver::KOrientationChanged;
			}
	else if ( newOrientationData.iPitch != iOrientationData.iPitch )
			{
			iOrientationData.iPitch = newOrientationData.iPitch;
			event = MSourceOrientationObserver::KOrientationChanged;
			}
	else if ( newOrientationData.iRoll != iOrientationData.iRoll )
			{
			iOrientationData.iRoll = newOrientationData.iRoll;
			event = MSourceOrientationObserver::KOrientationChanged;
			}
	else if ( newOrientationData.iFrontX != iOrientationData.iFrontX )
			{
			iOrientationData.iFrontX = newOrientationData.iFrontX;
			event = MSourceOrientationObserver::KOrientationVectorsChanged;
			}
	else if ( newOrientationData.iFrontY != iOrientationData.iFrontY )
			{
			iOrientationData.iFrontY = newOrientationData.iFrontY;
			event = MSourceOrientationObserver::KOrientationVectorsChanged;
			}
	else if ( newOrientationData.iFrontZ != iOrientationData.iFrontZ )
			{
			iOrientationData.iFrontZ = newOrientationData.iFrontZ;
			event = MSourceOrientationObserver::KOrientationVectorsChanged;
			}
	else if ( newOrientationData.iAboveX != iOrientationData.iAboveX )
			{
			iOrientationData.iAboveX = newOrientationData.iAboveX;
			event = MSourceOrientationObserver::KOrientationVectorsChanged;
			}
	else if ( newOrientationData.iFrontY != iOrientationData.iAboveY )
			{
			iOrientationData.iAboveY = newOrientationData.iAboveY;
			event = MSourceOrientationObserver::KOrientationVectorsChanged;
			}
	else if ( newOrientationData.iAboveZ != iOrientationData.iAboveZ )
			{
			iOrientationData.iAboveZ = newOrientationData.iAboveZ;
			event = MSourceOrientationObserver::KOrientationVectorsChanged;
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
