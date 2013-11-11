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
* Description:   Implementation of the RoomLevel proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "RoomLevelProxy.h"
#include "RoomLevelEventObserver.h"
#include <CustomInterfaceUtility.h>
#include "EnvironmentalReverbProxy.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRoomLevelProxy::CRoomLevelProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRoomLevelProxy::CRoomLevelProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CRoomLevelProxy::~CRoomLevelProxy()
  {

    // Remove the custom interface message handler before we destroy the proxy.
    if (iClientReverb)
       ((CEnvironmentalReverbProxy*)iClientReverb)->RoomLevelDetached(*this);
    if (iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
 	delete iRoomLevelEventObserver;
	delete iCustomInterfaceUtility;

	}

// -----------------------------------------------------------------------------
// CRoomLevelProxy::NewL
// Static function for creating an instance of the Environmental RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevelProxy* CRoomLevelProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CRoomLevelProxy* self = new (ELeave) CRoomLevelProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
     
    return self;
	}

// -----------------------------------------------------------------------------
// CRoomLevelProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CRoomLevelProxy::ConstructL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CRoomLevelProxy::ConstructL"));
#endif
    iRoomLevelEventObserver = CRoomLevelEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();
    // sends a message to fetch initial data.
    TEfRoomLevelDataPckg dataPckgFrom;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ERoomInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);

	}


// -----------------------------------------------------------------------------
// CRoomLevelProxy::ApplyL
// Apply the Environmental RoomLevel settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRoomLevelProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CRoomLevelProxy::Apply"));
#endif

	if ( !iHaveUpdateRights )
		{
		User::Leave(KErrAccessDenied);
		}
	
	iRoomLevelData.iEnabled = iEnabled;
	iRoomLevelData.iEnforced = iEnforced;
	iRoomLevelData.iHaveUpdateRights = iHaveUpdateRights;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)ERoomApply, DoEffectData(), KNullDesC8);

	}

// -----------------------------------------------------------------------------
// CRoomLevelProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CRoomLevelProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CRoomLevelProxy::StartObserver"));
#endif

	iRoomLevelEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CRoomLevelProxy::RoomLevelEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CRoomLevelProxy::RoomLevelEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CRoomLevelProxy::RoomLevelEvent"));
#endif

	TEfRoomLevelDataPckg dataPckgFrom;
	dataPckgFrom.Copy(aBuffer);
	TEfRoomLevel newRoomLevelData = dataPckgFrom();  

	TUint8 event = 0;

	if ( newRoomLevelData.iEnabled != iRoomLevelData.iEnabled )
		{
		iRoomLevelData.iEnabled = newRoomLevelData.iEnabled;
		iEnabled = newRoomLevelData.iEnabled;
		if ( iRoomLevelData.iEnabled )
			{
			event = MAudioEffectObserver::KEnabled;
			}
		else
			{
			event = MAudioEffectObserver::KDisabled;
			}
		}
	else if ( newRoomLevelData.iEnforced != iRoomLevelData.iEnforced )
		{
		iRoomLevelData.iEnforced = newRoomLevelData.iEnforced;
		iEnforced = newRoomLevelData.iEnforced;
		if ( iRoomLevelData.iEnforced )
			{
			event = MAudioEffectObserver::KEnforced;
			}
		else
			{
			event = MAudioEffectObserver::KNotEnforced;
			}
		}
	else if ( newRoomLevelData.iHaveUpdateRights != iRoomLevelData.iHaveUpdateRights )
		{
		iRoomLevelData.iHaveUpdateRights = newRoomLevelData.iHaveUpdateRights;
		iHaveUpdateRights = newRoomLevelData.iHaveUpdateRights;
		if ( iRoomLevelData.iHaveUpdateRights )
			{
			event = MAudioEffectObserver::KGainedUpdateRights;
			}
		else
			{
			event = MAudioEffectObserver::KLostUpdateRights;
			}
		}
		
	else if ( newRoomLevelData.iEnvironmentalReverbId != iRoomLevelData.iEnvironmentalReverbId )
		{
		iRoomLevelData.iEnvironmentalReverbId = newRoomLevelData.iEnvironmentalReverbId;
//		event = MRoomLevelObserver::KRoomLevelChanged;
		}
		
	else if ( newRoomLevelData.iStreamRoomLevel != iRoomLevelData.iStreamRoomLevel )
		{
		iRoomLevelData.iStreamRoomLevel = newRoomLevelData.iStreamRoomLevel;
		event = MRoomLevelObserver::KRoomLevelChanged;
		}
	else if ( newRoomLevelData.iStreamMinRoomLevel != iRoomLevelData.iStreamMinRoomLevel )
		{
		iRoomLevelData.iStreamMinRoomLevel = newRoomLevelData.iStreamMinRoomLevel;
//		event = MRoomLevelObserver::KMinRoomLevelChanged;
		}
	else if ( newRoomLevelData.iStreamMaxRoomLevel != iRoomLevelData.iStreamMaxRoomLevel )
		{
		iRoomLevelData.iStreamMaxRoomLevel = newRoomLevelData.iStreamMaxRoomLevel;
//		event = MRoomLevelObserver::KMinRoomLevelChanged;
		}

  
	if (!event)
		return;

	for ( TInt i = 0; i < iObservers.Count(); i++ )
		{
		iObservers[i]->EffectChanged(this, event);
		}
	}


// -----------------------------------------------------------------------------
// CRoomLevelProxy::AttachReverb
// Set room level's reverb ID
// -----------------------------------------------------------------------------
//
TInt CRoomLevelProxy::AttachReverb(CEnvironmentalReverb& aReverb)
{
	TInt err(KErrNone);
	iRoomLevelData.iEnvironmentalReverbId = ((CEnvironmentalReverbProxy&)aReverb).GetUniqueId();
    TRAP(err,(((CEnvironmentalReverbProxy&)(aReverb)).RoomLevelAttached(*this)));

    return err;
}



// -----------------------------------------------------------------------------
// CRoomLevelProxy::DettachReverb
// 
// -----------------------------------------------------------------------------
//
TInt CRoomLevelProxy::DettachReverb(CEnvironmentalReverb& aReverb)
{
    TInt status(KErrNone);
    iClientReverb = NULL;
    if ( iCustomInterfaceUtility )
        {
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
        }
    TRAP(status,(((CEnvironmentalReverbProxy&)(aReverb)).RoomLevelDetached(*this)));
    return status;
}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File


