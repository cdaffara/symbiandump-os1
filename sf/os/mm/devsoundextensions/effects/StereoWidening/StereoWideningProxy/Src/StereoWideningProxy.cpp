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
* Description:   Implementation of the Stereo Widening proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "StereoWideningProxy.h"
#include "StereoWideningEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStereoWideningProxy::CStereoWideningProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CStereoWideningProxy::CStereoWideningProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CStereoWideningProxy::~CStereoWideningProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iStereoWideningEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CStereoWideningProxy::NewL
// Static function for creating an instance of the Stereo Widening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWideningProxy* CStereoWideningProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CStereoWideningProxy* self = new (ELeave) CStereoWideningProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CStereoWideningProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CStereoWideningProxy::ConstructL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CStereoWideningProxy::ConstructL"));
#endif
    iStereoWideningEventObserver = CStereoWideningEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();
    // sends a message to fetch initial data.
    TEfStereoWideningDataPckg dataPckgFrom;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EAswefInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);
	}


// -----------------------------------------------------------------------------
// CStereoWideningProxy::ApplyL
// Apply the Stereo Widening settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CStereoWideningProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CStereoWideningProxy::Apply"));
#endif

	if ( !iHaveUpdateRights )
		{
		User::Leave(KErrAccessDenied);
		}

	iStereoWideningData.iEnabled = iEnabled;
	iStereoWideningData.iEnforced = iEnforced;
	iStereoWideningData.iHaveUpdateRights = iHaveUpdateRights;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EAswefApply, DoEffectData(), KNullDesC8);
	}

// -----------------------------------------------------------------------------
// CStereoWideningProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CStereoWideningProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CStereoWideningProxy::StartObserver"));
#endif

	iStereoWideningEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CStereoWideningProxy::StereoWideningEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CStereoWideningProxy::StereoWideningEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CStereoWideningProxy::StereoWideningEvent"));
#endif

	TEfStereoWideningDataPckg dataPckgFrom;
	dataPckgFrom.Copy(aBuffer);
	TEfStereoWidening newStereoWideningData = dataPckgFrom();

	TUint8 event = 0;

		if ( newStereoWideningData.iEnabled != iStereoWideningData.iEnabled )
			{
			iStereoWideningData.iEnabled = newStereoWideningData.iEnabled;
			iEnabled = newStereoWideningData.iEnabled;
			if ( iStereoWideningData.iEnabled )
				{
				event = MAudioEffectObserver::KEnabled;
				}
			else
				{
				event = MAudioEffectObserver::KDisabled;
				}
			}
		else if ( newStereoWideningData.iEnforced != iStereoWideningData.iEnforced )
			{
			iStereoWideningData.iEnforced = newStereoWideningData.iEnforced;
			iEnforced = newStereoWideningData.iEnforced;
			if ( iStereoWideningData.iEnforced )
				{
				event = MAudioEffectObserver::KEnforced;
				}
			else
				{
				event = MAudioEffectObserver::KNotEnforced;
				}
			}
		else if ( newStereoWideningData.iHaveUpdateRights != iStereoWideningData.iHaveUpdateRights )
			{
			iStereoWideningData.iHaveUpdateRights = newStereoWideningData.iHaveUpdateRights;
			iHaveUpdateRights = newStereoWideningData.iHaveUpdateRights;
			if ( iStereoWideningData.iHaveUpdateRights )
				{
				event = MAudioEffectObserver::KGainedUpdateRights;
				}
			else
				{
				event = MAudioEffectObserver::KLostUpdateRights;
				}
			}
		else if ( newStereoWideningData.iLevel != iStereoWideningData.iLevel )
			{
			iStereoWideningData.iLevel = newStereoWideningData.iLevel;
			event = MStereoWideningObserver::KStereoWideningLevelChanged;
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


