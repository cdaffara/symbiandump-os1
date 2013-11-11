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
* Description:   Implementation of the Loudness effect message handler class.
*
*/



// INCLUDE FILES
#include "LoudnessMessageHandler.h"
#include "LoudnessMessageTypes.h"
#include <LoudnessBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CLoudnessMessageHandler::CLoudnessMessageHandler(CLoudness* aLoudness)
    :	CMMFObject(KUidLoudnessEffect),
    	iLoudness(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
  	  iLoudness = aLoudness;
    }


EXPORT_C CLoudnessMessageHandler* CLoudnessMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CLoudness* aLoudness = (CLoudness*)aCustomInterface;
    CLoudnessMessageHandler* self = new (ELeave) CLoudnessMessageHandler(aLoudness);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CLoudnessMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CLoudnessMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CLoudnessMessageHandler::~CLoudnessMessageHandler
// Before going away, unregister with the CI Loudness object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CLoudnessMessageHandler::~CLoudnessMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessMessageHandler::~CLoudnessMessageHandler"));
#endif
    if(iLoudness)
	    iLoudness->UnRegisterObserver(*this);
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

    delete iLoudness;

	}


// ---------------------------------------------------------
// CLoudnessMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CLoudnessMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidLoudnessEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CLoudnessMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CLoudnessMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case ELefInitialize:
			{
			DoInitializeL(aMessage);
			break;
			}
		case ELefApply:
			{
			DoApplyL(aMessage);
			break;
			}
		case ELefObserve:
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
// CLoudnessMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CLoudnessMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessMessageHandler::DoInitializeL"));
#endif
	aMessage.WriteDataToClient(iLoudness->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CLoudnessMessageHandler::DoApplyL
// Extracts the data from the message. The client Loudness
// data is applied to the CI Loudness object.
// ---------------------------------------------------------
//
void CLoudnessMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessMessageHandler::DoApplyL"));
#endif
    TEfLoudnessDataPckg loudnessPckgFromClient;
    aMessage.ReadData1FromClient(loudnessPckgFromClient);
	iLoudness->SetEffectData(loudnessPckgFromClient);
	iLoudness->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CLoudnessMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CLoudnessMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered )
		{
		iLoudness->RegisterObserverL(*this);
		iRegistered = ETrue;
		}

	if ( iEffectDataQue->IsEmpty() )
		{
		//iMessage = &aMessage;
		iMessage = new(ELeave) TMMFMessage(aMessage);
		}
	else
		{
		TEfLoudnessDataPckg dataPckg;
		CEffectDataQueItem* item = iEffectDataQue->First();
		dataPckg.Copy(item->EffectData());
		aMessage.WriteDataToClient(dataPckg);
		aMessage.Complete(KErrNone);
		iEffectDataQue->Remove(*item);
		delete item;
		}
	}

// ---------------------------------------------------------
// CLoudnessMessageHandler::EffectChanged
// The CI Loudness object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI Loudness object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CLoudnessMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CLoudnessMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CLoudness*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else
		{
		// Saves the data and complete an observation message next time around.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CLoudness*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CLoudnessMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}

	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
