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
* Description:   Implementation of the StereoWidening effect message handler class.
*
*/



// INCLUDE FILES
#include "StereoWideningMessageHandler.h"
#include "StereoWideningMessageTypes.h"
#include <StereoWideningBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CStereoWideningMessageHandler::CStereoWideningMessageHandler(CStereoWidening* aStereoWidening)
    :	CMMFObject(KUidStereoWideningEffect),
    	iStereoWidening(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
  	  iStereoWidening = aStereoWidening;
    }


EXPORT_C CStereoWideningMessageHandler* CStereoWideningMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CStereoWidening* aStereoWidening = (CStereoWidening*)aCustomInterface;
    CStereoWideningMessageHandler* self = new (ELeave) CStereoWideningMessageHandler(aStereoWidening);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CStereoWideningMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CStereoWideningMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CStereoWideningMessageHandler::~CStereoWideningMessageHandler
// Before going away, unregister with the CI StereoWidening object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CStereoWideningMessageHandler::~CStereoWideningMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CStereoWideningMessageHandler::~CStereoWideningMessageHandler"));
#endif
    if(iStereoWidening)
	    iStereoWidening->UnRegisterObserver(*this);
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

    delete iStereoWidening;

	}


// ---------------------------------------------------------
// CStereoWideningMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CStereoWideningMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidStereoWideningEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CStereoWideningMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CStereoWideningMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case EAswefInitialize:
			{
			DoInitializeL(aMessage);
			break;
			}
		case EAswefApply:
			{
			DoApplyL(aMessage);
			break;
			}
		case EAswefObserve:
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
// CStereoWideningMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CStereoWideningMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
#ifdef _DEBUG
    RDebug::Print(_L("CStereoWideningMessageHandler::DoInitializeL"));
#endif
	aMessage.WriteDataToClient(iStereoWidening->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CStereoWideningMessageHandler::DoApplyL
// Extracts the data from the message. The client StereoWidening
// data is applied to the CI StereoWidening object.
// ---------------------------------------------------------
//
void CStereoWideningMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CStereoWideningMessageHandler::DoApplyL"));
#endif
    TEfStereoWideningDataPckg stereoWideningPckgFromClient;
    aMessage.ReadData1FromClient(stereoWideningPckgFromClient);
	iStereoWidening->SetEffectData(stereoWideningPckgFromClient);
	iStereoWidening->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CStereoWideningMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CStereoWideningMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CStereoWideningMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered )
		{
		iStereoWidening->RegisterObserverL(*this);
		iRegistered = ETrue;
		}

	if ( iEffectDataQue->IsEmpty() )
		{
		//iMessage = &aMessage;
		iMessage = new(ELeave) TMMFMessage(aMessage);
		}
	else
		{
		TEfStereoWideningDataPckg dataPckg;
		CEffectDataQueItem* item = iEffectDataQue->First();
		dataPckg.Copy(item->EffectData());
		aMessage.WriteDataToClient(dataPckg);
		aMessage.Complete(KErrNone);
		iEffectDataQue->Remove(*item);
		delete item;
		}
	}

// ---------------------------------------------------------
// CStereoWideningMessageHandler::EffectChanged
// The CI StereoWidening object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI StereoWidening object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CStereoWideningMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CStereoWideningMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CStereoWidening*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else
		{
		// Saves the data and complete an observation message next time around.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CStereoWidening*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CStereoWideningMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}

	}


// ========================== OTHER EXPORTED FUNCTIONS =========================


// End of File
