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
* Description:   Implementation of the audio equalizer proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "AudioEqualizerProxy.h"
#include "AudioEqualizerEventObserver.h"
#include <CustomInterfaceUtility.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAudioEqualizerProxy::CAudioEqualizerProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAudioEqualizerProxy::CAudioEqualizerProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility)

    {
    }

// Destructor
CAudioEqualizerProxy::~CAudioEqualizerProxy()
    {
    // Remove the custom interface message handler before we destroy the proxy.
    if(iCustomInterfaceUtility)
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
	delete iAudioEqualizerEventObserver;
	delete iCustomInterfaceUtility;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizerProxy::NewL
// Static function for creating an instance of the Audio Equalizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerProxy* CAudioEqualizerProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility )
    {
    CAudioEqualizerProxy* self = new (ELeave) CAudioEqualizerProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizerProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CAudioEqualizerProxy::ConstructL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerProxy::ConstructL"));
#endif
    iAudioEqualizerEventObserver = CAudioEqualizerEventObserver::NewL(iMessageHandler, *iCustomCommand, *this);
    StartObserver();
    // sends a message to fetch initial data.
    TEfAudioEqualizerDataPckg dataPckgFrom;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EAefInitialize, KNullDesC8, KNullDesC8, dataPckgFrom);
	SetEffectData(dataPckgFrom);

	TUint8 band = 0;
	TPckgBuf<TUint8> bandPckg;
	bandPckg() = 0;

	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EAefGetNumberOfBands, KNullDesC8, KNullDesC8, bandPckg);
	band = bandPckg();

	if ( !band ) // Number of bands should be > 0.
		{
		User::Leave(KErrNotReady);
		}

	TEfAudioEqualizerBand bandData;
	TEfAudioEqualizerBandDataPckg bandDataPckg;
	TInt8 numberOfBands = band;
	for(TInt i=1; i<=numberOfBands; i++)
		{
		band = i;
		bandPckg() = band;
		iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EAefInitializeBand, bandPckg, KNullDesC8, bandDataPckg);
		bandData = bandDataPckg();
		if(iBandsData.Append(bandData))
			{
			User::Leave(KErrNoMemory);
			}
		}

	}


// -----------------------------------------------------------------------------
// CAudioEqualizerProxy::ApplyL
// Apply the Audio Equalizer settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEqualizerProxy::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerProxy::Apply"));
#endif

	if ( !iHaveUpdateRights )
		{
		User::Leave(KErrAccessDenied);
		}

//	TUint32 mask = 1;
	iAudioEqualizerData.iEnabled = iEnabled;
	iAudioEqualizerData.iEnforced = iEnforced;
	iAudioEqualizerData.iHaveUpdateRights = iHaveUpdateRights;
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EAefApply, DoEffectData(), KNullDesC8);

#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerProxy::Apply - BandChange = %x"), iBandChange);
#endif
	iCustomCommand->CustomCommandSync(iMessageHandler, (TInt)EAefApplyBand, DoBandDataL(1, ETrue), KNullDesC8);

		iBandChange = 0; // clear flags
	}

// -----------------------------------------------------------------------------
// CAudioEqualizerProxy::StartObserver
// Starts the event observer. The event observer monitors asynchronous events
// from the message handler.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerProxy::StartObserver()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerProxy::StartObserver"));
#endif

	iAudioEqualizerEventObserver->Start();
	}

// -----------------------------------------------------------------------------
// CAudioEqualizerProxy::AudioEqualizerEvent
// Checks which data member has changed and notify the observers.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerProxy::AudioEqualizerEventL(
	const TDesC8& aBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerProxy::AudioEqualizerEventL"));
#endif

	TEfAudioEqualizerCombinedDataPckg dataPckgFrom;
	dataPckgFrom.Copy(aBuffer);
	TEfAudioEqualizerCombined newEqualizerCombinedData = dataPckgFrom();

	TUint8 event = 0;

	if ( newEqualizerCombinedData.iAudioEqualizerDataValid ) // Equalizer Data Changed
		{
		if ( newEqualizerCombinedData.iEnabled != iAudioEqualizerData.iEnabled )
			{
			iAudioEqualizerData.iEnabled = newEqualizerCombinedData.iEnabled;
			iEnabled = newEqualizerCombinedData.iEnabled;
			if ( iAudioEqualizerData.iEnabled )
				{
				event = MAudioEffectObserver::KEnabled;
				}
			else
				{
				event = MAudioEffectObserver::KDisabled;
				}
			}
		else if ( newEqualizerCombinedData.iEnforced != iAudioEqualizerData.iEnforced )
			{
			iAudioEqualizerData.iEnforced = newEqualizerCombinedData.iEnforced;
			iEnforced = newEqualizerCombinedData.iEnforced;
			if ( iAudioEqualizerData.iEnforced )
				{
				event = MAudioEffectObserver::KEnforced;
				}
			else
				{
				event = MAudioEffectObserver::KNotEnforced;
				}
			}
		else if ( newEqualizerCombinedData.iHaveUpdateRights != iAudioEqualizerData.iHaveUpdateRights )
			{
			iAudioEqualizerData.iHaveUpdateRights = newEqualizerCombinedData.iHaveUpdateRights;
			iHaveUpdateRights = newEqualizerCombinedData.iHaveUpdateRights;
			if ( iAudioEqualizerData.iHaveUpdateRights )
				{
				event = MAudioEffectObserver::KGainedUpdateRights;
				}
			else
				{
				event = MAudioEffectObserver::KLostUpdateRights;
				}
			}

		}

	else if ( newEqualizerCombinedData.iAudioEqualizerBandDataValid ) // Equalizer band data changed
		{
		TInt bandId = newEqualizerCombinedData.iBandId-1;
		iBandsData[bandId].iBandLevel = newEqualizerCombinedData.iBandLevel;
		iBandsData[bandId].iBandWidth = newEqualizerCombinedData.iBandWidth;
		iBandsData[bandId].iCenterFrequency = newEqualizerCombinedData.iCenterFrequency;
		iBandsData[bandId].iCrossoverFrequency = newEqualizerCombinedData.iCrossoverFrequency;
		event = MAudioEffectObserver::KSpecificEffectBase + newEqualizerCombinedData.iBandId;
		}



	if (!event)
		return;

	for ( TInt i = 0; i < iObservers.Count(); i++ )
		{
		iObservers[i]->EffectChanged(this, event);
		}
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



