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
* Description:   Implementation of the AudioEqualizer effect message handler class.
*
*/



// INCLUDE FILES
#include "AudioEqualizerMessageHandler.h"
#include "AudioEqualizerMessageTypes.h"
#include <AudioEqualizerBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CAudioEqualizerMessageHandler::CAudioEqualizerMessageHandler(
	CAudioEqualizer* aAudioEqualizer )
    :	CMMFObject(KUidAudioEqualizerEffect),
    	iAudioEqualizer(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
    iAudioEqualizer = aAudioEqualizer;
    }


EXPORT_C CAudioEqualizerMessageHandler* CAudioEqualizerMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CAudioEqualizer* audioEqualizer = (CAudioEqualizer*)aCustomInterface;
    CAudioEqualizerMessageHandler* self = new (ELeave) CAudioEqualizerMessageHandler(audioEqualizer);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CAudioEqualizerMessageHandler::~CAudioEqualizerMessageHandler
// Before going away, unregister with the CI AudioEqualizer object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CAudioEqualizerMessageHandler::~CAudioEqualizerMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerMessageHandler::~CAudioEqualizerMessageHandler"));
#endif

    if(iAudioEqualizer)
	    iAudioEqualizer->UnRegisterObserver(*this);
	iRegistered = EFalse;

	if(iMessage)
	{
		if ( !iMessage->IsCompleted() )
			{
			iMessage->Complete(KErrCancel);
			delete iMessage;
			}
	}

    if ( iEffectDataQue )
        {
        CEffectDataQueItem* queItem;
        while ( !iEffectDataQue->IsEmpty() )
            {
            queItem = iEffectDataQue->First();
            iEffectDataQue->Remove(*queItem);
            delete queItem;
            }

        delete iEffectDataQue;
        }

    delete iAudioEqualizer;

	}


// ---------------------------------------------------------
// CAudioEqualizerMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAudioEqualizerMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidAudioEqualizerEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CAudioEqualizerMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CAudioEqualizerMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case EAefInitialize:
			{
			DoInitializeL(aMessage);
			break;
			}
		case EAefInitializeBand:
			{
			DoInitializeBandL(aMessage);
			break;
			}
		case EAefGetNumberOfBands:
			{
			DoGetNumberOfBandsL(aMessage);
			break;
			}
		case EAefApply:
			{
			DoApplyL(aMessage);
			break;
			}
		case EAefApplyBand:
			{
			DoApplyBandL(aMessage);
			break;
			}
		case EAefObserve:
			{
			DoObserveL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

// ---------------------------------------------------------
// CAudioEqualizerMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CAudioEqualizerMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerMessageHandler::DoInitializeL"));
#endif
	aMessage.WriteDataToClient(iAudioEqualizer->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CAudioEqualizerMessageHandler::DoInitializeBandL
// ---------------------------------------------------------
//
void CAudioEqualizerMessageHandler::DoInitializeBandL(TMMFMessage& aMessage)
	{
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerMessageHandler::DoInitializeBandL"));
#endif
	TUint8 band = 0;
	TPckgBuf<TUint8> bandPckg;
    aMessage.ReadData1FromClient(bandPckg);
    band = bandPckg();
	aMessage.WriteDataToClient(iAudioEqualizer->DoBandDataL(band, EFalse));
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CAudioEqualizerMessageHandler::DoGetNumberOfBandsL
// ---------------------------------------------------------
//
void CAudioEqualizerMessageHandler::DoGetNumberOfBandsL(TMMFMessage& aMessage)
	{
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerMessageHandler::DoGetNumberOfBandsL"));
#endif
	TUint8 band = iAudioEqualizer->NumberOfBands();
	TPckgBuf<TUint8> bandPckg(band);
	aMessage.WriteDataToClient(bandPckg);
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CAudioEqualizerMessageHandler::DoApplyL
// Extracts the data from the message. The client AudioEqualizer
// data is applied to the CI AudioEqualizer object.
// ---------------------------------------------------------
//
void CAudioEqualizerMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerMessageHandler::DoApplyL"));
#endif
    TEfAudioEqualizerDataPckg audioEqualizerPckgFromClient;
    aMessage.ReadData1FromClient(audioEqualizerPckgFromClient);
	iAudioEqualizer->SetEffectData(audioEqualizerPckgFromClient);
//	iAudioEqualizer->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CAudioEqualizerMessageHandler::DoApplyBandL
// Extracts the data from the message. The client Audio Equalizer
// band data is applied to the CI AudioEqualizer object.
// ---------------------------------------------------------
//
void CAudioEqualizerMessageHandler::DoApplyBandL(
	TMMFMessage& aMessage )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerMessageHandler::DoApplyBandL"));
#endif
    TEfAudioEqualizerBandDataPckg audioEqualizerBandDataPckgFromClient;
    HBufC8* bandDataBuf = HBufC8::NewL(iAudioEqualizer->NumberOfBands() * sizeof(TEfAudioEqualizerBand));
	TPtr8 des = bandDataBuf->Des();
	des.SetLength(bandDataBuf->Size());

	aMessage.ReadData1FromClient(des);
    //aMessage.ReadData1FromClient(audioEqualizerBandDataPckgFromClient);
	//iAudioEqualizer->SetBandDataL(1,audioEqualizerBandDataPckgFromClient);
	iAudioEqualizer->SetBandDataL(2,des);
	delete bandDataBuf;
	iAudioEqualizer->ApplyL();
	aMessage.Complete(KErrNone);
	}
// ---------------------------------------------------------
// CAudioEqualizerMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CAudioEqualizerMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered )
		{
		iAudioEqualizer->RegisterObserverL(*this);
		iRegistered = ETrue;
		}

	if ( iEffectDataQue->IsEmpty() )
		{
		//iMessage = &aMessage;
		iMessage = new(ELeave) TMMFMessage(aMessage);
		}
	else
		{
		TEfAudioEqualizerCombinedDataPckg dataPckg;
		CEffectDataQueItem* item = iEffectDataQue->First();
		dataPckg.Copy(item->EffectData());
		aMessage.WriteDataToClient(dataPckg);
		aMessage.Complete(KErrNone);
		iEffectDataQue->Remove(*item);
		delete item;
		}
	}

// ---------------------------------------------------------
// CAudioEqualizerMessageHandler::EffectChanged
// The CI AudioEqualizer object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI AudioEqualizer object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CAudioEqualizerMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	TUint8 aEvent )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CAudioEqualizerMessageHandler::EffectChanged"));
#endif

	TEfAudioEqualizerCombined data;
	data.iAudioEqualizerDataValid = EFalse;
	data.iAudioEqualizerBandDataValid = EFalse;

	if ( aEvent < MAudioEffectObserver::KSpecificEffectBase )
		{
		data.iEnabled = ((CAudioEqualizer*)aAudioEffect)->IsEnabled();
		data.iHaveUpdateRights = ((CAudioEqualizer*)aAudioEffect)->HaveUpdateRights();
		data.iEnforced = ((CAudioEqualizer*)aAudioEffect)->IsEnforced();
		data.iAudioEqualizerDataValid = ETrue;
		}
	else
		{
		TUint8 band = aEvent - MAudioEffectObserver::KSpecificEffectBase;
		data.iBandId = band;
		data.iBandLevel = ((CAudioEqualizer*)aAudioEffect)->BandLevel(band);
		data.iBandWidth = ((CAudioEqualizer*)aAudioEffect)->BandWidth(band);
		data.iCenterFrequency = ((CAudioEqualizer*)aAudioEffect)->CenterFrequency(band);
		data.iCrossoverFrequency = ((CAudioEqualizer*)aAudioEffect)->CrossoverFrequency(band);
		data.iAudioEqualizerBandDataValid = ETrue;
		}

	TEfAudioEqualizerCombinedDataPckg dataPckg(data);

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(dataPckg);
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else
		{
		// Saves the data and complete an observation message next time around.
		HBufC8* data = NULL;
		TRAPD(err1,data = dataPckg.AllocL());
		if(!err1)
			{
				//CleanupStack::PushL(data);
				CEffectDataQueItem* item = NULL;
				TRAPD(err2,item = CEffectDataQueItem::NewL(data));
				if(!err2)
				{
					iEffectDataQue->AddLast(*item);
				}
				else
				{
					delete data;
				}
			}
		else
			{
#ifdef _DEBUG
				RDebug::Print(_L("CAudioEqualizerMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}

	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
