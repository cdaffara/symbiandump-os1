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
* Description:   Implementation of the DistanceAttenuation effect message handler class.
*
*/



// INCLUDE FILES
#include "DistanceAttenuationMessageHandler.h"
#include "DistanceAttenuationMessageTypes.h"
#include <DistanceAttenuationBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CDistanceAttenuationMessageHandler::CDistanceAttenuationMessageHandler(CDistanceAttenuation* aDistanceAttenuation)
    :	CMMFObject(KUidDistanceAttenuationEffect),
    	iDistanceAttenuation(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
  	  iDistanceAttenuation = aDistanceAttenuation;
    }


EXPORT_C CDistanceAttenuationMessageHandler* CDistanceAttenuationMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CDistanceAttenuation* aDistanceAttenuation = (CDistanceAttenuation*)aCustomInterface;
    CDistanceAttenuationMessageHandler* self = new (ELeave) CDistanceAttenuationMessageHandler(aDistanceAttenuation);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CDistanceAttenuationMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CDistanceAttenuationMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CDistanceAttenuationMessageHandler::~CDistanceAttenuationMessageHandler
// Before going away, unregister with the CI DistanceAttenuation object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CDistanceAttenuationMessageHandler::~CDistanceAttenuationMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationMessageHandler::~CDistanceAttenuationMessageHandler"));
#endif
    if(iDistanceAttenuation)
	    iDistanceAttenuation->UnRegisterObserver(*this);
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

    delete iDistanceAttenuation;

	}


// ---------------------------------------------------------
// CDistanceAttenuationMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CDistanceAttenuationMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidDistanceAttenuationEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CDistanceAttenuationMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CDistanceAttenuationMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case EDaefInitialize:
			{
			DoInitializeL(aMessage);
			break;
			}
		case EDaefApply:
			{
			DoApplyL(aMessage);
			break;
			}
		case EDaefObserve:
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
// CDistanceAttenuationMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CDistanceAttenuationMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationMessageHandler::DoInitializeL"));
#endif
	aMessage.WriteDataToClient(iDistanceAttenuation->DoEffectData());
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CDistanceAttenuationMessageHandler::DoApplyL
// Extracts the data from the message. The client DistanceAttenuation
// data is applied to the CI DistanceAttenuation object.
// ---------------------------------------------------------
//
void CDistanceAttenuationMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationMessageHandler::DoApplyL"));
#endif
    TEfDistanceAttenuationDataPckg distanceAttenuationPckgFromClient;
    aMessage.ReadData1FromClient(distanceAttenuationPckgFromClient);
	iDistanceAttenuation->SetEffectData(distanceAttenuationPckgFromClient);
	iDistanceAttenuation->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CDistanceAttenuationMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CDistanceAttenuationMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered )
		{
		iDistanceAttenuation->RegisterObserverL(*this);
		iRegistered = ETrue;
		}

	if ( iEffectDataQue->IsEmpty() )
		{
		//iMessage = &aMessage;
		iMessage = new(ELeave) TMMFMessage(aMessage);
		}
	else
		{
		TEfDistanceAttenuationDataPckg dataPckg;
		CEffectDataQueItem* item = iEffectDataQue->First();
		dataPckg.Copy(item->EffectData());
		aMessage.WriteDataToClient(dataPckg);
		aMessage.Complete(KErrNone);
		iEffectDataQue->Remove(*item);
		delete item;
		}
	}

// ---------------------------------------------------------
// CDistanceAttenuationMessageHandler::EffectChanged
// The CI DistanceAttenuation object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI DistanceAttenuation object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CDistanceAttenuationMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CDistanceAttenuationMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CDistanceAttenuation*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else
		{
		// Saves the data and complete an observation message next time around.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CDistanceAttenuation*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CDistanceAttenuationMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}

	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
