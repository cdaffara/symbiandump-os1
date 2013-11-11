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
#include "ListenerLocationMessageHandler.h"
#include "ListenerLocationMessageTypes.h"
#include <ListenerLocationBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CListenerLocationMessageHandler::CListenerLocationMessageHandler(
	CListenerLocation* aListenerLocation )
    :	CMMFObject(KUidListenerLocationEffect),
    	iListenerLocation(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
    iListenerLocation = aListenerLocation;
    }


EXPORT_C CListenerLocationMessageHandler* CListenerLocationMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CListenerLocation* bassboost = (CListenerLocation*)aCustomInterface;
    CListenerLocationMessageHandler* self = new (ELeave) CListenerLocationMessageHandler(bassboost);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CListenerLocationMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CListenerLocationMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CListenerLocationMessageHandler::~CListenerLocationMessageHandler
// Before going away, unregister with the CI ListenerLocation object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CListenerLocationMessageHandler::~CListenerLocationMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CListenerLocationMessageHandler::~CListenerLocationMessageHandler"));
#endif
    if(iListenerLocation)
	    iListenerLocation->UnRegisterObserver(*this);
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

     delete iListenerLocation;

	}


// ---------------------------------------------------------
// CListenerLocationMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CListenerLocationMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidListenerLocationEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CListenerLocationMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CListenerLocationMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case ELlfInitialize: // Request to initialize the bassboost
			{
			DoInitializeL(aMessage);
			break;
			}
		case ELlfApply: // Request to apply the bassboost settings
			{
			DoApplyL(aMessage);
			break;
			}
		case ELlfObserve: // Observation request
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
// CListenerLocationMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CListenerLocationMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
	aMessage.WriteDataToClient(iListenerLocation->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CListenerLocationMessageHandler::DoApplyL
// Extracts the data from the message. The client bassboost data
// is applied to the CI bassboost object.
// ---------------------------------------------------------
//
void CListenerLocationMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
    TEfLocationDataPckg locationPckgFromClient;
    aMessage.ReadData1FromClient(locationPckgFromClient);

	iListenerLocation->SetEffectData(locationPckgFromClient);
	iListenerLocation->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CListenerLocationMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CListenerLocationMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CListenerLocationMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered ) // Don't register again if we're registered.
		{
		iListenerLocation->RegisterObserverL(*this);
		iRegistered = ETrue;
		}

	if ( iEffectDataQue->IsEmpty() )
		{
		// Message is saved and completed when an event occurs
		iMessage = new(ELeave) TMMFMessage(aMessage);
		}
	else
		{
		TEfLocationDataPckg dataPckg;
		CEffectDataQueItem* item = iEffectDataQue->First();
		dataPckg.Copy(item->EffectData());
		aMessage.WriteDataToClient(dataPckg);
		aMessage.Complete(KErrNone);
		iEffectDataQue->Remove(*item);
		delete item;
		}
	}

// ---------------------------------------------------------
// CListenerLocationMessageHandler::EffectChanged
// The CI bassboost object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI bassboost object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CListenerLocationMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	 TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerLocationMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CListenerLocation*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else // no message pending and there is no event queued up
		{
		// Saves the data and complete an observation message next time when
		// there is a pending message.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CListenerLocation*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CListenerLocationMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================




// End of File
