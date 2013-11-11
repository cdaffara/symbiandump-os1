// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// OOB Data Manager.
// 
//

/**
 @file
 @internalComponent
*/

#include <e32base.h>
#include <bttypes.h>

#include <bluetooth/pairingservershared.h>
#include <bluetooth/hcicommandqueue.h>

#include <bluetooth/hci/event.h>
#include <bluetooth/hci/readlocaloobdatacommand.h>
#include <bluetooth/hci/readlocaloobdatacompleteevent.h>

#include "oobdata.h"
#include "pairingserver.h"

#include <bluetooth/logger.h>
#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAIRING_SERVER);
#endif

#include "secman.h"

//
// COobDataManager
//

COobDataManager* COobDataManager::NewL(CBTSecMan& aSecMan)
	{
	LOG_STATIC_FUNC
	COobDataManager* self = new(ELeave) COobDataManager(aSecMan);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

COobDataManager::COobDataManager(CBTSecMan& aSecMan)
	: CBase()
	, iLocalOobState(ELocalOobInvalid)
	, iLocalOobObservers(_FOFF(XOobLocalDataObserver, iLink))
	, iSecMan(aSecMan)
	{
	LOG_FUNC
	}

void COobDataManager::SetHCICommandQueue(MHCICommandQueue& aCommandQueue)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iCommandQueue, PANIC(KOobDataPanicCat, EOobDataPanicCommandQueueAlreadyProvided));
	iCommandQueue = &aCommandQueue;
	}

void COobDataManager::ClearHCICommandQueue()
	{
	LOG_FUNC
	if(iCommandQueue)
		{
		iCommandQueue->MhcqRemoveAllCommands(*this);
		}
	iCommandQueue = NULL;
	}

MHCICommandQueue& COobDataManager::CommandQueueL() const
	{
	LOG_FUNC
	if(!iCommandQueue)
		{
		LEAVEL(KErrNotReady);
		}
	return *iCommandQueue;
	}

void COobDataManager::ConstructL()
	{
	LOG_FUNC
	}

COobDataManager::~COobDataManager()
	{
	LOG_FUNC
	}

void COobDataManager::ProvideRemoteOobDataL(const TOobData& aData)
	{
	LOG_FUNC
	TInt ix = iOobDataFromPeers.Find(aData.Addr(), TOobData::IsForAddress);
	if(ix == KErrNotFound)
		{
		iOobDataFromPeers.AppendL(aData);
		}
	else
		{
		__ASSERT_DEBUG(ix >= 0, PANIC(KOobDataPanicCat, EOobDataPanicErrorWhenFindingOobDataForProvision));
		iOobDataFromPeers[ix] = aData;
		}
	}

void COobDataManager::ClearRemoteOobData(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	TInt ix = iOobDataFromPeers.Find(aAddr, TOobData::IsForAddress);
	if(ix != KErrNotFound) // ignore if we cannot find an existing entry.
		{
		__ASSERT_DEBUG(ix >= 0, PANIC(KOobDataPanicCat, EOobDataPanicErrorWhenFindingOobDataForClear));
		iOobDataFromPeers.Remove(ix);
		iOobDataFromPeers.GranularCompress();
		}
#ifdef _DEBUG
	// When in debug builds, check that there is no entry for the referenced device
	// address once one has been removed (there should only be a single entry per
	// remote device).
	ix = iOobDataFromPeers.Find(aAddr, TOobData::IsForAddress);
	__ASSERT_DEBUG(ix == KErrNotFound, PANIC(KOobDataPanicCat, EOobDataPanicMultipleOobEntriesForDevice));
#endif // _DEBUG
	}


void COobDataManager::ReadLocalOobData(XOobLocalDataObserver& aObserver)
	{
	switch(iLocalOobState)
		{
		case ELocalOobValid:
			// synchronous complete.
			aObserver.XoldoLocalOobDataRetrieved(KErrNone, iOobHashC, iOobRandomizerR);
			break;
			
		case ELocalOobUpdating:
			// The update is outstanding and so the observer can be completed when the
			// result comes in.
			__ASSERT_DEBUG(!aObserver.iLink.IsQueued(), PANIC(KOobDataPanicCat, EOobDataPanicLocalOobObserverIsAlreadyQueuedDuringUpdate));
			iLocalOobObservers.AddLast(aObserver);
			break;
			
		case ELocalOobInvalid:
			// If invalid then we should issue a request to retrieve the data.
			__ASSERT_DEBUG(!aObserver.iLink.IsQueued(), PANIC(KOobDataPanicCat, EOobDataPanicLocalOobObserverIsAlreadyQueuedWhileInvalid));
			iLocalOobObservers.AddLast(aObserver);
			MakeLocalOobRequestToController();
			break;
		default:
			__ASSERT_DEBUG(EFalse, PANIC(KOobDataPanicCat, EOobDataPanicUnknownState));
			break;
		}
	}

void COobDataManager::CancelReadLocalOobData(XOobLocalDataObserver& aObserver)
	{
	LOG_FUNC
	// This requests doesn't actually cancel the retrieval of the local OOB data
	// from the controller.  Instead it merely removes the observer from being called.
	// back.
	aObserver.iLink.Deque();
	// Callback with to indicate the cancel. 
	aObserver.XoldoLocalOobDataRetrieved(KErrCancel, iOobHashC, iOobRandomizerR);
	}

void COobDataManager::RefreshLocalOobData()
	{
	LOG_FUNC
	// Don't do much, just set the state as invalid.  If there is an outstanding
	// new set of OOB data then don't do anything (as we are being refreshed.)
	if(iLocalOobState == ELocalOobValid)
		{
		iLocalOobState = ELocalOobInvalid;
		}
	}

void COobDataManager::MakeLocalOobRequestToController()
	{
	LOG_FUNC
	TRAPD(err, MakeLocalOobRequestToControllerL());
	if(err != KErrNone)
		{
		CompleteQueuedLocalOobObservers(err);
		}
	}

void COobDataManager::MakeLocalOobRequestToControllerL()
	{
	LOG_FUNC
	if (!iSecMan.LocalSimplePairingMode())
		{
		User::Leave(KErrNotSupported);
		}
	CReadLocalOOBDataCommand* cmd = CReadLocalOOBDataCommand::NewL();
	CleanupStack::PushL(cmd);
	MHCICommandQueue& commandQueue = CommandQueueL();
	CleanupStack::Pop(cmd);
	commandQueue.MhcqAddCommandL(cmd, *this);
	}


void COobDataManager::MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* __DEBUG_ONLY(aRelatedCommand))
	{
	LOG_FUNC
	__ASSERT_DEBUG(aRelatedCommand && (aRelatedCommand->Opcode() == KReadLocalOOBDataOpcode),
			PANIC(KOobDataPanicCat, EOobDataPanicUnexpectedHciCommandAssociated));
	__ASSERT_DEBUG(aEvent.EventCode() == ECommandCompleteEvent,
			PANIC(KOobDataPanicCat, EOobDataPanicUnexpectedHciEventReceived));
	const THCICommandCompleteEvent& completeEvent = THCICommandCompleteEvent::Cast(aEvent);
	__ASSERT_DEBUG(completeEvent.CommandOpcode() == KReadLocalOOBDataOpcode,
			PANIC(KOobDataPanicCat, EOobDataPanicUnexpectedHciCommandCompleteEventReceived));
	const TReadLocalOOBDataCompleteEvent& event = TReadLocalOOBDataCompleteEvent::Cast(completeEvent);
	
	iOobHashC = event.OOBDataC();
	iOobRandomizerR = event.OOBDataR();
	
	iLocalOobState = ELocalOobValid;
	CompleteQueuedLocalOobObservers(KErrNone);
	}

void COobDataManager::MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* __DEBUG_ONLY(aCommand))
	{
	LOG_FUNC
	__ASSERT_DEBUG(aCommand && (aCommand->Opcode() == KReadLocalOOBDataOpcode),
			PANIC(KOobDataPanicCat, EOobDataPanicUnexpectedHciCommandAssociatedWithError));
	iLocalOobState = ELocalOobInvalid;
	CompleteQueuedLocalOobObservers(aErrorCode);
	}

void COobDataManager::CompleteQueuedLocalOobObservers(TInt aResult)
	{
	LOG_FUNC
	TDblQueIter<XOobLocalDataObserver> iter(iLocalOobObservers);
	while(XOobLocalDataObserver* obs = iter++)
		{
		obs->iLink.Deque();
		obs->XoldoLocalOobDataRetrieved(aResult, iOobHashC, iOobRandomizerR);
		}
	}




TBool COobDataManager::GetRemoteOobData(const TBTDevAddr& aAddr, TBluetoothSimplePairingHash& aHashC, TBluetoothSimplePairingRandomizer& aRandomizerR)
	{
	LOG_FUNC
	TBool found = EFalse;
	TInt ix = iOobDataFromPeers.Find(aAddr, TOobData::IsForAddress);
	if(ix == KErrNotFound)
		{
		aHashC = KNullDesC8;
		aRandomizerR = KNullDesC8;
		}
	else
		{
		__ASSERT_DEBUG(ix >= 0, PANIC(KOobDataPanicCat, EOobDataPanicErrorWhenFindingOobDataForGet));
		found = ETrue;
		aHashC = iOobDataFromPeers[ix].HashC();
		aRandomizerR = iOobDataFromPeers[ix].RandomizerR();
		// Now invalidate (remove) this OOB data; only allow retrieval once
		iOobDataFromPeers.Remove(ix);
		iOobDataFromPeers.GranularCompress();
		}
	return found;
	}

TBool COobDataManager::HasRemoteOobData(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	TInt ix = iOobDataFromPeers.Find(aAddr, TOobData::IsForAddress);
	__ASSERT_DEBUG((ix >= 0 || ix == KErrNotFound), 
			PANIC(KOobDataPanicCat, EOobDataPanicErrorWhenFindingOobDataForHas));
	return ix != KErrNotFound;
	}


//
// TOobData
//

TOobData::TOobData(const TBTDevAddr& aAddr, const TBluetoothSimplePairingHash& aHashC, const TBluetoothSimplePairingRandomizer&  aRandomizerR)
	: iAddr(aAddr)
	, iHashC(aHashC)
	, iRandomizerR(aRandomizerR)
	{
	LOG_FUNC
	}
	
const TBTDevAddr& TOobData::Addr() const
	{
	LOG_FUNC
	return iAddr;
	}

const TBluetoothSimplePairingHash& TOobData::HashC() const
	{
	LOG_FUNC
	return iHashC;
	}

const TBluetoothSimplePairingRandomizer& TOobData::RandomizerR() const
	{
	LOG_FUNC
	return iRandomizerR;
	}

TBool TOobData::IsForAddress(const TBTDevAddr* aAddr, const TOobData& aOobData)
	{
	LOG_STATIC_FUNC
	return aAddr && *aAddr == aOobData.Addr();
	}

