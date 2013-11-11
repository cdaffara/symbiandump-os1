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
* Description:   Implementation of the ListenerDoppler effect message handler class.
*
*/



// INCLUDE FILES
#include "ListenerDopplerMessageHandler.h"
#include "ListenerDopplerMessageTypes.h"
#include <ListenerDopplerBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CListenerDopplerMessageHandler::CListenerDopplerMessageHandler(
	CListenerDoppler* aDoppler )
    :	CMMFObject(KUidListenerDopplerEffect),
    	iDoppler(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
    iDoppler = aDoppler;
    }


EXPORT_C CListenerDopplerMessageHandler* CListenerDopplerMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CListenerDoppler* doppler = (CListenerDoppler*)aCustomInterface;
    CListenerDopplerMessageHandler* self = new (ELeave) CListenerDopplerMessageHandler(doppler);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CListenerDopplerMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CListenerDopplerMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CListenerDopplerMessageHandler::~CListenerDopplerMessageHandler
// Before going away, unregister with the CI Doppler object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CListenerDopplerMessageHandler::~CListenerDopplerMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CListenerDopplerMessageHandler::~CListenerDopplerMessageHandler"));
#endif
    if(iDoppler)
	    iDoppler->UnRegisterObserver(*this);
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

     delete iDoppler;

	}


// ---------------------------------------------------------
// CListenerDopplerMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CListenerDopplerMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidListenerDopplerEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CListenerDopplerMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CListenerDopplerMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case EListenerDopplerOfInitialize: // Request to initialize the ListenerDoppler
			{
			DoInitializeL(aMessage);
			break;
			}
		case EListenerDopplerOfApply: // Request to apply the ListenerDoppler settings
			{
			DoApplyL(aMessage);
			break;
			}
		case EListenerDopplerOfObserve: // Observation request
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
// CListenerDopplerMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CListenerDopplerMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
	aMessage.WriteDataToClient(iDoppler->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CListenerDopplerMessageHandler::DoApplyL
// Extracts the data from the message. The client doppler data
// is applied to the CI doppler object.
// ---------------------------------------------------------
//
void CListenerDopplerMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
    TEfDopplerDataPckg dopplerPckgFromClient;
    aMessage.ReadData1FromClient(dopplerPckgFromClient);

	iDoppler->SetEffectData(dopplerPckgFromClient);
	iDoppler->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CListenerDopplerMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CListenerDopplerMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CListenerDopplerMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered ) // Don't register again if we're registered.
		{
		iDoppler->RegisterObserverL(*this);
		iRegistered = ETrue;
		}

	if ( iEffectDataQue->IsEmpty() )
		{
		// Message is saved and completed when an event occurs
		iMessage = new(ELeave) TMMFMessage(aMessage);
		}
	else
		{
		TEfDopplerDataPckg dataPckg;
		CEffectDataQueItem* item = iEffectDataQue->First();
		dataPckg.Copy(item->EffectData());
		aMessage.WriteDataToClient(dataPckg);
		aMessage.Complete(KErrNone);
		iEffectDataQue->Remove(*item);
		delete item;
		}
	}

// ---------------------------------------------------------
// CListenerDopplerMessageHandler::EffectChanged
// The CI doppler object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI doppler object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CListenerDopplerMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	 TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CListenerDopplerMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CListenerDoppler*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else // no message pending and there is no event queued up
		{
		// Saves the data and complete an observation message next time when
		// there is a pending message.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CListenerDoppler*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CListenerDopplerMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
