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
#include "ListenerOrientationMessageHandler.h"
#include "ListenerOrientationMessageTypes.h"
#include <ListenerOrientationBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CListenerOrientationMessageHandler::CListenerOrientationMessageHandler(
	CListenerOrientation* aListenerOrientation )
    :	CMMFObject(KUidListenerOrientationEffect),
    	iListenerOrientation(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
    iListenerOrientation = aListenerOrientation;
    }


EXPORT_C CListenerOrientationMessageHandler* CListenerOrientationMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CListenerOrientation* bassboost = (CListenerOrientation*)aCustomInterface;
    CListenerOrientationMessageHandler* self = new (ELeave) CListenerOrientationMessageHandler(bassboost);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CListenerOrientationMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CListenerOrientationMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CListenerOrientationMessageHandler::~CListenerOrientationMessageHandler
// Before going away, unregister with the CI ListenerOrientation object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CListenerOrientationMessageHandler::~CListenerOrientationMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CListenerOrientationMessageHandler::~CListenerOrientationMessageHandler"));
#endif
    if(iListenerOrientation)
	    iListenerOrientation->UnRegisterObserver(*this);
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

     delete iListenerOrientation;

	}


// ---------------------------------------------------------
// CListenerOrientationMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CListenerOrientationMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidListenerOrientationEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CListenerOrientationMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CListenerOrientationMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case ELofInitialize: // Request to initialize the bassboost
			{
			DoInitializeL(aMessage);
			break;
			}
		case ELofApply: // Request to apply the bassboost settings
			{
			DoApplyL(aMessage);
			break;
			}
		case ELofObserve: // Observation request
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
// CListenerOrientationMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CListenerOrientationMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
	aMessage.WriteDataToClient(iListenerOrientation->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CListenerOrientationMessageHandler::DoApplyL
// Extracts the data from the message. The client bassboost data
// is applied to the CI bassboost object.
// ---------------------------------------------------------
//
void CListenerOrientationMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
    TEfOrientationDataPckg orientationPckgFromClient;
    aMessage.ReadData1FromClient(orientationPckgFromClient);

	iListenerOrientation->SetEffectData(orientationPckgFromClient);
	iListenerOrientation->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CListenerOrientationMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CListenerOrientationMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CListenerOrientationMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered ) // Don't register again if we're registered.
		{
		iListenerOrientation->RegisterObserverL(*this);
		iRegistered = ETrue;
		}

	if ( iEffectDataQue->IsEmpty() )
		{
		// Message is saved and completed when an event occurs
		iMessage = new(ELeave) TMMFMessage(aMessage);
		}
	else
		{
		TEfOrientationDataPckg dataPckg;
		CEffectDataQueItem* item = iEffectDataQue->First();
		dataPckg.Copy(item->EffectData());
		aMessage.WriteDataToClient(dataPckg);
		aMessage.Complete(KErrNone);
		iEffectDataQue->Remove(*item);
		delete item;
		}
	}

// ---------------------------------------------------------
// CListenerOrientationMessageHandler::EffectChanged
// The CI bassboost object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI bassboost object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CListenerOrientationMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	 TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerOrientationMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CListenerOrientation*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else // no message pending and there is no event queued up
		{
		// Saves the data and complete an observation message next time when
		// there is a pending message.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CListenerOrientation*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CListenerOrientationMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
