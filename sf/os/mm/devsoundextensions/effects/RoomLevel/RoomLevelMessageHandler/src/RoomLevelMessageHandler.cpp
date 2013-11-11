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
* Description:   Implementation of the RoomLevel effect message handler class.
*
*/



// INCLUDE FILES
#include "RoomLevelMessageHandler.h"
#include "RoomLevelMessageTypes.h"
#include <RoomLevelBase.h>
#include "EffectDataQueItem.h"
#include <RoomLevelBase.h>
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CRoomLevelMessageHandler::CRoomLevelMessageHandler(
	CRoomLevel* aRoomLevel )
    :	CMMFObject(KUidRoomLevelEffect),
    	iRoomLevel(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
    iRoomLevel = aRoomLevel;
    }


EXPORT_C CRoomLevelMessageHandler* CRoomLevelMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CRoomLevel* roomLevel = (CRoomLevel*)aCustomInterface;
    CRoomLevelMessageHandler* self = new (ELeave) CRoomLevelMessageHandler(roomLevel);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CRoomLevelMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CRoomLevelMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CRoomLevelMessageHandler::~CRoomLevelMessageHandler
// Before going away, unregister with the CI RoomLevel object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CRoomLevelMessageHandler::~CRoomLevelMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CRoomLevelMessageHandler::~CRoomLevelMessageHandler"));
#endif
    if(iRoomLevel)
	    iRoomLevel->UnRegisterObserver(*this);
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

    delete iRoomLevel;

	}


// ---------------------------------------------------------
// CRoomLevelMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CRoomLevelMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidRoomLevelEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CRoomLevelMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CRoomLevelMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case ERoomInitialize:
			{
			DoInitializeL(aMessage);
			break;
			}
		case ERoomApply:
			{
			DoApplyL(aMessage);
			break;
			}
		case ERoomObserve:
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
// CRoomLevelMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CRoomLevelMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
#ifdef _DEBUG
    RDebug::Print(_L("CRoomLevelMessageHandler::DoInitializeL"));
#endif
	aMessage.WriteDataToClient(iRoomLevel->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CRoomLevelMessageHandler::DoApplyL
// Extracts the data from the message. The client RoomLevel
// data is applied to the CI RoomLevel object.
// ---------------------------------------------------------
//
void CRoomLevelMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CRoomLevelMessageHandler::DoApplyL"));
#endif
    TEfRoomLevelDataPckg RoomLevelPckgFromClient;
    aMessage.ReadData1FromClient(RoomLevelPckgFromClient);
    iRoomLevel->SetEffectData(RoomLevelPckgFromClient);
	iRoomLevel->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CRoomLevelMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CRoomLevelMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CRoomLevelMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered )
		{
		iRoomLevel->RegisterObserverL(*this);
		iRegistered = ETrue;
		}

	if ( iEffectDataQue->IsEmpty() )
		{
		//iMessage = &aMessage;
		iMessage = new(ELeave) TMMFMessage(aMessage);
		}
	else
		{
		TEfRoomLevelDataPckg dataPckg;
		CEffectDataQueItem* item = iEffectDataQue->First();
		dataPckg.Copy(item->EffectData());
		aMessage.WriteDataToClient(dataPckg);
		aMessage.Complete(KErrNone);
		iEffectDataQue->Remove(*item);
		delete item;
		}
	}

// ---------------------------------------------------------
// CRoomLevelMessageHandler::EffectChanged
// The CI RoomLevel object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI RoomLevel object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CRoomLevelMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CRoomLevelMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CRoomLevel*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else
		{
		// Saves the data and complete an observation message next time around.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CRoomLevel*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CRoomLevelMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}

	}


// ========================== OTHER EXPORTED FUNCTIONS =========================


// End of File
