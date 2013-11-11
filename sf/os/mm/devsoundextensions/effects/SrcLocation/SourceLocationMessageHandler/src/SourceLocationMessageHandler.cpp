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
#include "SourceLocationMessageHandler.h"
#include "SourceLocationMessageTypes.h"
#include <SourceLocationBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSourceLocationMessageHandler::CSourceLocationMessageHandler(
	CSourceLocation* aSourceLocation )
    :	CMMFObject(KUidSourceLocationEffect),
    	iSourceLocation(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
    iSourceLocation = aSourceLocation;
    }


EXPORT_C CSourceLocationMessageHandler* CSourceLocationMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CSourceLocation* bassboost = (CSourceLocation*)aCustomInterface;
    CSourceLocationMessageHandler* self = new (ELeave) CSourceLocationMessageHandler(bassboost);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CSourceLocationMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CSourceLocationMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CSourceLocationMessageHandler::~CSourceLocationMessageHandler
// Before going away, unregister with the CI SourceLocation object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CSourceLocationMessageHandler::~CSourceLocationMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CSourceLocationMessageHandler::~CSourceLocationMessageHandler"));
#endif
    if(iSourceLocation)
	    iSourceLocation->UnRegisterObserver(*this);
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

     delete iSourceLocation;

	}


// ---------------------------------------------------------
// CSourceLocationMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSourceLocationMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidSourceLocationEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CSourceLocationMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CSourceLocationMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case ESlfInitialize: // Request to initialize the bassboost
			{
			DoInitializeL(aMessage);
			break;
			}
		case ESlfApply: // Request to apply the bassboost settings
			{
			DoApplyL(aMessage);
			break;
			}
		case ESlfObserve: // Observation request
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
// CSourceLocationMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CSourceLocationMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
	aMessage.WriteDataToClient(iSourceLocation->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CSourceLocationMessageHandler::DoApplyL
// Extracts the data from the message. The client bassboost data
// is applied to the CI bassboost object.
// ---------------------------------------------------------
//
void CSourceLocationMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
    TEfLocationDataPckg locationPckgFromClient;
    aMessage.ReadData1FromClient(locationPckgFromClient);

	iSourceLocation->SetEffectData(locationPckgFromClient);
	iSourceLocation->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CSourceLocationMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CSourceLocationMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CSourceLocationMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered ) // Don't register again if we're registered.
		{
		iSourceLocation->RegisterObserverL(*this);
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
// CSourceLocationMessageHandler::EffectChanged
// The CI bassboost object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI bassboost object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CSourceLocationMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	 TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CSourceLocationMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CSourceLocation*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else // no message pending and there is no event queued up
		{
		// Saves the data and complete an observation message next time when
		// there is a pending message.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CSourceLocation*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CSourceLocationMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
