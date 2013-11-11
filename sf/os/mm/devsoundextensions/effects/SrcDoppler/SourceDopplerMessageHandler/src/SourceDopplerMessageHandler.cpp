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
* Description:   Implementation of the SourceDoppler effect message handler class.
*
*/



// INCLUDE FILES
#include "SourceDopplerMessageHandler.h"
#include "SourceDopplerMessageTypes.h"
#include <SourceDopplerBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSourceDopplerMessageHandler::CSourceDopplerMessageHandler(
	CSourceDoppler* aDoppler )
    :	CMMFObject(KUidSourceDopplerEffect),
    	iDoppler(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
    iDoppler = aDoppler;
    }


EXPORT_C CSourceDopplerMessageHandler* CSourceDopplerMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CSourceDoppler* doppler = (CSourceDoppler*)aCustomInterface;
    CSourceDopplerMessageHandler* self = new (ELeave) CSourceDopplerMessageHandler(doppler);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CSourceDopplerMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CSourceDopplerMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CSourceDopplerMessageHandler::~CSourceDopplerMessageHandler
// Before going away, unregister with the CI Doppler object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CSourceDopplerMessageHandler::~CSourceDopplerMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CSourceDopplerMessageHandler::~CSourceDopplerMessageHandler"));
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
// CSourceDopplerMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSourceDopplerMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidSourceDopplerEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CSourceDopplerMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CSourceDopplerMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case ESourceDopplerOfInitialize: // Request to initialize the SourceDoppler
			{
			DoInitializeL(aMessage);
			break;
			}
		case ESourceDopplerOfApply: // Request to apply the SourceDoppler settings
			{
			DoApplyL(aMessage);
			break;
			}
		case ESourceDopplerOfObserve: // Observation request
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
// CSourceDopplerMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CSourceDopplerMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
	aMessage.WriteDataToClient(iDoppler->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CSourceDopplerMessageHandler::DoApplyL
// Extracts the data from the message. The client doppler data
// is applied to the CI doppler object.
// ---------------------------------------------------------
//
void CSourceDopplerMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
    TEfDopplerDataPckg dopplerPckgFromClient;
    aMessage.ReadData1FromClient(dopplerPckgFromClient);

	iDoppler->SetEffectData(dopplerPckgFromClient);
	iDoppler->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CSourceDopplerMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CSourceDopplerMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CSourceDopplerMessageHandler::DoObserveL"));
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
// CSourceDopplerMessageHandler::EffectChanged
// The CI doppler object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI doppler object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CSourceDopplerMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	 TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceDopplerMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CSourceDoppler*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else // no message pending and there is no event queued up
		{
		// Saves the data and complete an observation message next time when
		// there is a pending message.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CSourceDoppler*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CSourceDopplerMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================


// End of File
