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
#include "SourceOrientationMessageHandler.h"
#include "SourceOrientationMessageTypes.h"
#include <SourceOrientationBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSourceOrientationMessageHandler::CSourceOrientationMessageHandler(
	CSourceOrientation* aSourceOrientation )
    :	CMMFObject(KUidSourceOrientationEffect),
    	iSourceOrientation(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
    iSourceOrientation = aSourceOrientation;
    }


EXPORT_C CSourceOrientationMessageHandler* CSourceOrientationMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CSourceOrientation* bassboost = (CSourceOrientation*)aCustomInterface;
    CSourceOrientationMessageHandler* self = new (ELeave) CSourceOrientationMessageHandler(bassboost);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CSourceOrientationMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CSourceOrientationMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CSourceOrientationMessageHandler::~CSourceOrientationMessageHandler
// Before going away, unregister with the CI SourceOrientation object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CSourceOrientationMessageHandler::~CSourceOrientationMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CSourceOrientationMessageHandler::~CSourceOrientationMessageHandler"));
#endif
    if(iSourceOrientation)
	    iSourceOrientation->UnRegisterObserver(*this);
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

     delete iSourceOrientation;

	}


// ---------------------------------------------------------
// CSourceOrientationMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSourceOrientationMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidSourceOrientationEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CSourceOrientationMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CSourceOrientationMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case ESofInitialize: // Request to initialize the bassboost
			{
			DoInitializeL(aMessage);
			break;
			}
		case ESofApply: // Request to apply the bassboost settings
			{
			DoApplyL(aMessage);
			break;
			}
		case ESofObserve: // Observation request
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
// CSourceOrientationMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CSourceOrientationMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
	aMessage.WriteDataToClient(iSourceOrientation->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CSourceOrientationMessageHandler::DoApplyL
// Extracts the data from the message. The client bassboost data
// is applied to the CI bassboost object.
// ---------------------------------------------------------
//
void CSourceOrientationMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
    TEfOrientationDataPckg orientationPckgFromClient;
    aMessage.ReadData1FromClient(orientationPckgFromClient);

	iSourceOrientation->SetEffectData(orientationPckgFromClient);
	iSourceOrientation->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CSourceOrientationMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CSourceOrientationMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CSourceOrientationMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered ) // Don't register again if we're registered.
		{
		iSourceOrientation->RegisterObserverL(*this);
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
// CSourceOrientationMessageHandler::EffectChanged
// The CI bassboost object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI bassboost object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CSourceOrientationMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	 TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceOrientationMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CSourceOrientation*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else // no message pending and there is no event queued up
		{
		// Saves the data and complete an observation message next time when
		// there is a pending message.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CSourceOrientation*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CSourceOrientationMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
