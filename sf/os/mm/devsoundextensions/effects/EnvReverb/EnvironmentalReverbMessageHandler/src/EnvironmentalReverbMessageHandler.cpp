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
* Description:   Implementation of the EnvironmentalReverb effect message handler class.
*
*/



// INCLUDE FILES
#include "EnvironmentalReverbMessageHandler.h"
#include "EnvironmentalReverbMessageTypes.h"
#include <EnvironmentalReverbBase.h>
#include "EffectDataQueItem.h"
#ifdef _DEBUG
#include <e32svr.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CEnvironmentalReverbMessageHandler::CEnvironmentalReverbMessageHandler(
	CEnvironmentalReverb* aEnvironmentalReverb )
    :	CMMFObject(KUidEnvironmentalReverbEffect),
    	iEnvironmentalReverb(NULL),
    	iMessage(NULL),
    	iRegistered(EFalse),
    	iEffectDataQue(NULL)
    {
    iEnvironmentalReverb = aEnvironmentalReverb;
    }


EXPORT_C CEnvironmentalReverbMessageHandler* CEnvironmentalReverbMessageHandler::NewL(
	TAny* aCustomInterface )
    {
    CEnvironmentalReverb* environmentalReverb = (CEnvironmentalReverb*)aCustomInterface;
    CEnvironmentalReverbMessageHandler* self = new (ELeave) CEnvironmentalReverbMessageHandler(environmentalReverb);
	self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// Create and initializes the effect data queue.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbMessageHandler::ConstructL()
	{
	iEffectDataQue = new(ELeave) TSglQue<CEffectDataQueItem>(_FOFF(CEffectDataQueItem, iLink));
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverbMessageHandler::~CEnvironmentalReverbMessageHandler
// Before going away, unregister with the CI EnvironmentalReverb object.
// The observation message must be completed if outstanding.
// The effect data queue must be emptied and destroyed.
// -----------------------------------------------------------------------------
//
CEnvironmentalReverbMessageHandler::~CEnvironmentalReverbMessageHandler()
	{

#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbMessageHandler::~CEnvironmentalReverbMessageHandler"));
#endif
    if(iEnvironmentalReverb)
	    iEnvironmentalReverb->UnRegisterObserver(*this);
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

    delete iEnvironmentalReverb;

	}


// ---------------------------------------------------------
// CEnvironmentalReverbMessageHandler::HandleRequest
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CEnvironmentalReverbMessageHandler::HandleRequest(
	TMMFMessage& aMessage )
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidEnvironmentalReverbEffect);
	TRAPD(error,DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CEnvironmentalReverbMessageHandler::DoHandleRequestL
// Dispatches the message to the appropriate handler.
// ---------------------------------------------------------
//
void CEnvironmentalReverbMessageHandler::DoHandleRequestL(
	TMMFMessage& aMessage )
	{
	switch( aMessage.Function() )
		{
		case EErfInitialize:
			{
			DoInitializeL(aMessage);
			break;
			}
		case EErfApply:
			{
			DoApplyL(aMessage);
			break;
			}
		case EErfObserve:
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
// CEnvironmentalReverbMessageHandler::DoInitializeL
// ---------------------------------------------------------
//
void CEnvironmentalReverbMessageHandler::DoInitializeL(TMMFMessage& aMessage)
	{
#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbMessageHandler::DoInitializeL"));
#endif
	aMessage.WriteDataToClient(iEnvironmentalReverb->DoEffectData());
	aMessage.Complete(KErrNone);
	}



// ---------------------------------------------------------
// CEnvironmentalReverbMessageHandler::DoApplyL
// Extracts the data from the message. The client EnvironmentalReverb
// data is applied to the CI EnvironmentalReverb object.
// ---------------------------------------------------------
//
void CEnvironmentalReverbMessageHandler::DoApplyL(
	TMMFMessage& aMessage )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbMessageHandler::DoApplyL"));
#endif
    TEfEnvReverbDataPckg environmentalReverbPckgFromClient;
    aMessage.ReadData1FromClient(environmentalReverbPckgFromClient);
	iEnvironmentalReverb->SetEffectData(environmentalReverbPckgFromClient);
	iEnvironmentalReverb->ApplyL();
	aMessage.Complete(KErrNone);
	}

// ---------------------------------------------------------
// CEnvironmentalReverbMessageHandler::DoObserveL
// Receives the observation request message and depending
// on the status of the effect data queue, the message is
// completed immediately or saved for later completion.
// ---------------------------------------------------------
//
void CEnvironmentalReverbMessageHandler::DoObserveL(
	TMMFMessage& aMessage )
	{

#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbMessageHandler::DoObserveL"));
#endif

	if ( !iRegistered )
		{
		iEnvironmentalReverb->RegisterObserverL(*this);
		iRegistered = ETrue;
		}

	if ( iEffectDataQue->IsEmpty() )
		{
		//iMessage = &aMessage;
		iMessage = new(ELeave) TMMFMessage(aMessage);
		}
	else
		{
		TEfEnvReverbDataPckg dataPckg;
		CEffectDataQueItem* item = iEffectDataQue->First();
		dataPckg.Copy(item->EffectData());
		aMessage.WriteDataToClient(dataPckg);
		aMessage.Complete(KErrNone);
		iEffectDataQue->Remove(*item);
		delete item;
		}
	}

// ---------------------------------------------------------
// CEnvironmentalReverbMessageHandler::EffectChanged
// The CI EnvironmentalReverb object has changed state.
// The observation message is completed if no data has been
// queued up. Otherwise, the CI EnvironmentalReverb object's data is
// packaged and queued.
// ---------------------------------------------------------
//
void CEnvironmentalReverbMessageHandler::EffectChanged(
	const CAudioEffect* aAudioEffect,
	TUint8 /*aEvent*/ )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CEnvironmentalReverbMessageHandler::EffectChanged"));
#endif

	if ( iMessage && !iMessage->IsCompleted() && iEffectDataQue->IsEmpty() )
		{
		iMessage->WriteDataToClient(((CEnvironmentalReverb*)aAudioEffect)->DoEffectData());
		iMessage->Complete(KErrNone);
		delete iMessage;
		iMessage = NULL;
		}
	else
		{
		// Saves the data and complete an observation message next time around.
		HBufC8* data = NULL;
		TRAPD(err1,data = ((CEnvironmentalReverb*)aAudioEffect)->DoEffectData().AllocL());
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
				RDebug::Print(_L("CEnvironmentalReverbMessageHandler::EffectChanged Error Allocating Memory %d"),err1);
#endif
			}
		}

	}



// ========================== OTHER EXPORTED FUNCTIONS =========================


// End of File
