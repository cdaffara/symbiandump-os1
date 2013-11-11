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
* Description:   Implementation of the Environmental Reverb proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "EnvironmentalReverbProxy.h"
#include "EnvironmentalReverbEventObserver.h"
#include "RoomLevelProxy.h"
#include <CustomInterfaceUtility.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEnvironmentalReverbProxy::CEnvironmentalReverbProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEnvironmentalReverbProxy::CEnvironmentalReverbProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CEnvironmentalReverbProxy::~CEnvironmentalReverbProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    
    TInt numOfRoomLevel = iCRoomLevelProxyList.Count();
    if (numOfRoomLevel > 0)
    {
    	for (TInt i = 0; i < numOfRoomLevel; ++i)
    	{
    	iCRoomLevelProxyList[i]->DettachReverb(*this);
    	}
    };
    iCRoomLevelProxyList.Close();
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iEnvironmentalReverbEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverbProxy::NewL
// Static function for creating an instance of the Environmental Reverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverbProxy* CEnvironmentalReverbProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CEnvironmentalReverbProxy* self = new (ELeave) CEnvironmentalReverbProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverbProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbProxy::ConstructL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbProxy::ConstructL"));
#endif
    iEnvironmentalReverbEventObserver = CEnvironmentalReverbEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();
    // sends a message to fetch initial data.
    TEfEnvReverbDataPckg dataPckgFrom;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EErfInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);

	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverbProxy::GetUniqueId
// return a TInt 32 integer
// -----------------------------------------------------------------------------
//
TUint32 CEnvironmentalReverbProxy::GetUniqueId() const
	{
		return iReverbData.iEnvironmentalReverbId;
     }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbProxy::AttachRoomLevel
// Add the reference of aProxy to array
// -----------------------------------------------------------------------------
//
TInt CEnvironmentalReverbProxy::RoomLevelAttached(CRoomLevelProxy& aProxy)
	{   
	 
	    TInt error = iCRoomLevelProxyList.Find(&aProxy);
	    if (error == KErrNotFound)
	    {
	      error = iCRoomLevelProxyList.Append(&aProxy);
	    }

	    
	    return error;

	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverbImpl::DetachRoomLevel
// Remove the reference of aProxy from array
// -----------------------------------------------------------------------------
//
TInt CEnvironmentalReverbProxy::RoomLevelDetached(CRoomLevelProxy& aProxy)
	{
	   TInt found = iCRoomLevelProxyList.Find(&aProxy);
	   if (found != KErrNotFound)
	   {
	   	 iCRoomLevelProxyList.Remove(found);				
	     found = KErrNone;
	   }
	   return found;
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverbProxy::ApplyL
// Apply the Environmental Reverb settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverbProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbProxy::Apply"));
#endif

	if ( !iHaveUpdateRights )
		{
		User::Leave(KErrAccessDenied);
		}

	iReverbData.iEnabled = iEnabled;
	iReverbData.iEnforced = iEnforced;
	iReverbData.iHaveUpdateRights = iHaveUpdateRights;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EErfApply, DoEffectData(), KNullDesC8);

	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverbProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbProxy::StartObserver"));
#endif

	iEnvironmentalReverbEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverbProxy::EnvironmentalReverbEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbProxy::EnvironmentalReverbEvent(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbProxy::EnvironmentalReverbEvent"));
#endif

	TEfEnvReverbDataPckg dataPckgFrom;
	dataPckgFrom.Copy(aBuffer);
	TEfEnvironmentalReverb newReverbData = dataPckgFrom();

	TUint8 event = 0;

	if ( newReverbData.iEnabled != iReverbData.iEnabled )
		{
		iReverbData.iEnabled = newReverbData.iEnabled;
		iEnabled = newReverbData.iEnabled;
		if ( iReverbData.iEnabled )
			{
			event = MAudioEffectObserver::KEnabled;
			}
		else
			{
			event = MAudioEffectObserver::KDisabled;
			}
		}
	else if ( newReverbData.iEnforced != iReverbData.iEnforced )
		{
		iReverbData.iEnforced = newReverbData.iEnforced;
		iEnforced = newReverbData.iEnforced;
		if ( iReverbData.iEnforced )
			{
			event = MAudioEffectObserver::KEnforced;
			}
		else
			{
			event = MAudioEffectObserver::KNotEnforced;
			}
		}
	else if ( newReverbData.iHaveUpdateRights != iReverbData.iHaveUpdateRights )
		{
		iReverbData.iHaveUpdateRights = newReverbData.iHaveUpdateRights;
		iHaveUpdateRights = newReverbData.iHaveUpdateRights;
		if ( iReverbData.iHaveUpdateRights )
			{
			event = MAudioEffectObserver::KGainedUpdateRights;
			}
		else
			{
			event = MAudioEffectObserver::KLostUpdateRights;
			}
		}
	else if ( newReverbData.iDecayTime != iReverbData.iDecayTime )
		{
		iReverbData.iDecayTime = newReverbData.iDecayTime;
		event = MEnvironmentalReverbObserver::KDecayTimeChanged;
		}
	else if ( newReverbData.iDecayHFRatio != iReverbData.iDecayHFRatio )
		{
		iReverbData.iDecayHFRatio = newReverbData.iDecayHFRatio;
		event = MEnvironmentalReverbObserver::KDecayHFRatioChanged;
		}
	else if ( newReverbData.iDensity != iReverbData.iDensity )
		{
		iReverbData.iDensity = newReverbData.iDensity;
		event = MEnvironmentalReverbObserver::KDensityChanged;
		}
	else if ( newReverbData.iDiffusion != iReverbData.iDiffusion )
		{
		iReverbData.iDiffusion = newReverbData.iDiffusion;
		event = MEnvironmentalReverbObserver::KDiffusionChanged;
		}
	else if ( newReverbData.iReflectionsDelay != iReverbData.iReflectionsDelay )
		{
		iReverbData.iReflectionsDelay = newReverbData.iReflectionsDelay;
		event = MEnvironmentalReverbObserver::KReflectionsDelayChanged;
		}
	else if ( newReverbData.iReflectionsLevel != iReverbData.iReflectionsLevel )
		{
		iReverbData.iReflectionsLevel = newReverbData.iReflectionsLevel;
		event = MEnvironmentalReverbObserver::KReflectionsLevelChanged;
		}
	else if ( newReverbData.iReverbDelay != iReverbData.iReverbDelay )
		{
		iReverbData.iReverbDelay = newReverbData.iReverbDelay;
		event = MEnvironmentalReverbObserver::KReverbDelayChanged;
		}
	else if ( newReverbData.iReverbLevel != iReverbData.iReverbLevel )
		{
		iReverbData.iReverbLevel = newReverbData.iReverbLevel;
		event = MEnvironmentalReverbObserver::KReverbLevelChanged;
		}
	else if ( newReverbData.iRoomLevel != iReverbData.iRoomLevel )
		{
		iReverbData.iRoomLevel = newReverbData.iRoomLevel;
		event = MEnvironmentalReverbObserver::KRoomLevelChanged;
		}
	else if ( newReverbData.iRoomHFLevel != iReverbData.iRoomHFLevel )
		{
		iReverbData.iRoomHFLevel = newReverbData.iRoomHFLevel;
		event = MEnvironmentalReverbObserver::KRoomHFLevelChanged;
		}
	else if ( newReverbData.iEnvironmentalReverbId != iReverbData.iEnvironmentalReverbId )
		{
		iReverbData.iEnvironmentalReverbId = newReverbData.iEnvironmentalReverbId;
//		event = MEnvironmentalReverbObserver::KRoomHFLevelChanged;
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


