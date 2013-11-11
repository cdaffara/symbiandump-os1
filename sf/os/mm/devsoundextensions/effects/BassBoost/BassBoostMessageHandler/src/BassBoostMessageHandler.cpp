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
* Description:   Implementation of the bassboost effect message handler class.
*
*/



// INCLUDE FILES
#include "BassBoostMessageHandler.h"
#include "BassBoostMessageTypes.h"
#include "EffectDataQueItem.h"
#include <BassBoostBase.h>
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CBassBoostMessageHandler::CBassBoostMessageHandler(
	CBassBoost* aBassBoost )
    :	CMMFObject(KUidBassBoostEffect),
    	iBassBoost(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
    iBassBoost = aBassBoost;
    }


EXPORT_C CBassBoostMessageHandler* CBassBoostMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CBassBoost* bassboost = (CBassBoost*)aCustomInterface;
    CBassBoostMessageHandler* self = new (ELeave) CBassBoostMessageHandler(bassboost);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CBassBoostMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CBassBoostMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CBassBoostMessageHandler::~CBassBoostMessageHandler
// Before going away, unregister with the CI BassBoost object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CBassBoostMessageHandler::~CBassBoostMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CBassBoostMessageHandler::~CBassBoostMessageHandler"));
#endif

    if(iBassBoost)
    	iBassBoost->UnRegisterObserver(*this);
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

     delete iBassBoost;

	}


// ---------------------------------------------------------
// CBassBoostMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBassBoostMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidBassBoostEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CBassBoostMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CBassBoostMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case EBfInitialize: // Request to initialize the bassboost
			{
			DoInitializeL(aMessage);
			break;
			}
		case EBfApply: // Request to apply the bassboost settings
			{
			DoApplyL(aMessage);
			break;
			}
		case EBfObserve: // Observation request
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
// CBassBoostMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CBassBoostMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
	aMessage.WriteDataToClient(iBassBoost->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CBassBoostMessageHandler::DoApplyL
// Extracts the data from the message. The client bassboost data
// is applied to the CI bassboost object.
// ---------------------------------------------------------
//
void CBassBoostMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
    TEfBassBoostDataPckg bassboostPckgFromClient;
    aMessage.ReadData1FromClient(bassboostPckgFromClient);

	iBassBoost->SetEffectData(bassboostPckgFromClient);
	iBassBoost->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CBassBoostMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CBassBoostMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CBassBoostMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered ) // Don't register again if we're registered.
		{
		iBassBoost->RegisterObserverL(*this);
		iRegistered = ETrue;
		}

	if ( iEffectDataQue->IsEmpty() )
		{
		// Message is saved and completed when an event occurs
		iMessage = new(ELeave) TMMFMessage(aMessage);
		}
	else
		{
		TEfBassBoostDataPckg dataPckg;
		CEffectDataQueItem* item = iEffectDataQue->First();
		dataPckg.Copy(item->EffectData());
		aMessage.WriteDataToClient(dataPckg);
		aMessage.Complete(KErrNone);
		iEffectDataQue->Remove(*item);
		delete item;
		}
	}

// ---------------------------------------------------------
// CBassBoostMessageHandler::EffectChanged
// The CI bassboost object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI bassboost object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CBassBoostMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	 TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CBassBoostMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CBassBoost*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else // no message pending and there is no event queued up
		{
		// Saves the data and complete an observation message next time when
		// there is a pending message.
		// Saves the data and complete an observation message next time around.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CBassBoost*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CBassBoostMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================




// End of File
