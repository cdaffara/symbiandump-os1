// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Retrieves information on ME <mem1>, <mem2> storage slots (total, used) ....AT+CPMS?
// Also tracks the number of preferred memories.
// 
//

#include "NOTIFY.H"
#include "mSMSMESS.H"
#include "mSMSMEM.H"
#include "mSLOGGER.H"
#include "ATIO.H"
#include "ATINIT.H"
#include "mSMSSTOR.H"

_LIT8(KOpenBracketCharacter,"(");
_LIT8(KCloseBracketCharacter,")");


CATSmsMemoryStorage* CATSmsMemoryStorage::NewL (CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,
							CPhoneGlobals* aGsmStatus)
	{
	CATSmsMemoryStorage* self = new (ELeave) CATSmsMemoryStorage(aIo, aTelObject, aInit, aGsmStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATSmsMemoryStorage::CATSmsMemoryStorage(CATIO* aIo, CTelObject* aTelObject,
										 CATInit* aInit, CPhoneGlobals* aGsmStatus)
	:CATSmsCommands(aIo, aTelObject, aInit, aGsmStatus), 
	 iSmsStore(REINTERPRET_CAST(CMobileSmsStore*,aTelObject)), 
	 iCancelled(EFalse)
	{}

void CATSmsMemoryStorage::ConstructL()
	{
	CATCommands::ConstructL();
	iClientPreferredMem = 1;
	}

CATSmsMemoryStorage::~CATSmsMemoryStorage()
	{}

void CATSmsMemoryStorage::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)
//
// Start function called by CMobileSmsMessaging::GetMessageStoreInfo
//
	{
	iReqHandle = aTsyReqHandle;
	iCancelled=EFalse;
	if (aParams)
		iInfoPckg=static_cast<RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg*>(aParams);

	if (ComparePrefMem(iPeekPrevStorage[iClientPreferredMem].iStoreName))
		{
		// no need to set preferred memory as correct one is selected already
		iTxBuffer.Copy(KGetPrefMemTestCommand);	// Preferred Memory Storage cmd. ETSI std GSM 07.05, May 1996
		iIo->Write(this, iTxBuffer);
		iIo->SetTimeOut(this, 5000);
		iState = EATWaitForSendingCPMSRequestComplete;
		}
	else //ie: the compared memories are different, must change phone mem to temporary mem
		{
		iRequestedMem=iPeekPrevStorage[iClientPreferredMem].iStoreName;
		SetCurrentPrefMem(iRequestedMem);
		iState = EATWaitForSettingMem;
		}
	}

void CATSmsMemoryStorage::StartGetInfo(TTsyReqHandle aTsyReqHandle, TAny* aParams)
//
// Start function called by CMobileSmsStore::GetInfo
//
	{
	LOGTEXT(_L8("CATSmsMemoryStorage:\tClient has called a GetInfo"));
	iReqHandle = aTsyReqHandle;
	iCancelled=EFalse;

	if (aParams)
		iInfoPckg=static_cast<RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg*>(aParams);
	
	if (ComparePrefMem(iSmsStore->iStoreName))
		{
		// no need to set preferred memory as correct one is selected already
		iTxBuffer.Copy(KGetPrefMemTestCommand);	// Preferred Memory Storage cmd. ETSI std GSM 07.05, May 1996
		iIo->Write(this, iTxBuffer);
		iIo->SetTimeOut(this, 5000);
		iState = EATWaitForSendingCPMSRequestComplete;
		}
	else
		{
		iRequestedMem=iSmsStore->iStoreName;
		SetCurrentPrefMem(iRequestedMem);
		iState = EATWaitForSettingMem;
		}
	}
	

void CATSmsMemoryStorage::EventSignal(TEventSource aSource)
//
// Finite State Machine for handling events from the comm port
//
	{
	LOGTEXT2(_L8("CATSmsMemoryStorage:\tiState = %D"), iState);
	if (aSource == ETimeOutCompletion)
		{
		LOGTEXT(_L8("CATSmsMemoryStorage:\tTimeout Error during AT+CPMS query of ME"));
		iPhoneGlobals->iPhoneStatus.iModemDetected = RPhone::EDetectedNotPresent;
		Complete(KErrTimedOut, aSource);
		return;
		}

	
	TInt ret(KErrNone);

	switch (iState)
		{
	case EATWaitForSettingMem:
		iIo->WriteAndTimerCancel(this);
		StandardWriteCompletionHandler(aSource, 5);
		iState = EATWaitForSettingMemComplete;
		break;

	case EATWaitForSettingMemComplete:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));

		iIo->WriteAndTimerCancel(this);
		ret=ValidateExpectString();
		if (ret==KErrNone)
			{
			iPhoneGlobals->iPhonePrefMem=iRequestedMem;
			if (iCancelled)
				Complete(KErrCancel, aSource);
			else
				{
				iTxBuffer.Copy(KGetPrefMemTestCommand);	// Preferred Memory Storage cmd. ETSI std GSM 07.05, May 1996
				iIo->Write(this, iTxBuffer);
				iIo->SetTimeOut(this, 5000);
				iState = EATWaitForSendingCPMSRequestComplete;
				}
			}
		else
			Complete(ret, aSource);
		break;
	
	case EATWaitForSendingCPMSRequestComplete:
		iIo->WriteAndTimerCancel(this);
		StandardWriteCompletionHandler(aSource,5);
		iState = EATWaitForCPMSResponseComplete;
		break;
	
	case EATWaitForCPMSResponseComplete:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
		iIo->WriteAndTimerCancel(this);
		TRAPD(ret2, CPMSResponseL());
		Complete(ret2,aSource);
		break;

	default:		// Catches illegal states
		LOGTEXT2(_L8("CATSmsMemoryStorage:\tiIllegal State = %D"), iState);
		break;
		}
	}

void CATSmsMemoryStorage::CPMSResponseL()
//
// Parse result of AT+CPMS? ....used by GetMessageStoreInfo()
//
	{
	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	
	// +CPMS: <mem1>,<used1>,<total1>,<mem2>,<used2>,<total2>,<mem3>,<used3>,<total3>
	// but we don't care after <total1>

	entry = iter++;		
	if (!entry || entry->iResultPtr != KCPMSResponseString)
		User::Leave(KErrNotFound);

	entry = iter++;
	TPtrC8 name(entry->iResultPtr);
	entry=iter++;
	TInt used = CATParamListEntry::EntryValL(entry);
	entry=iter++;
	TInt total = CATParamListEntry::EntryValL(entry);

	RMobilePhoneStore::TMobilePhoneStoreInfoV1& storeInfo = (*iInfoPckg)();

	storeInfo.iType=RMobilePhoneStore::EShortMessageStore;
	storeInfo.iUsedEntries=used;
	storeInfo.iTotalEntries=total;

	if (name == KMEStorage)
		storeInfo.iName=KETelMeSmsStore;
	else if (name == KSMStorage)
		storeInfo.iName=KETelIccSmsStore; 
	else if (name == KMTStorage)
		storeInfo.iName=KETelCombinedSmsStore; 
	else
		User::Leave(KErrGeneral);

	storeInfo.iCaps=SMSStoreCaps(storeInfo.iName);   

	LOGTEXT3(_L8("CPMSResponseL\tPreferred store used=%d, total=%d"), used, total);
	CleanupStack::PopAndDestroy();
	}

void CATSmsMemoryStorage::EnumerateCPMSResponseL()
//
// Parse result of AT+CPMS=?
//
	{
	TInt count;
	for (count = 0; count < KStorageArraySize ; count++)
		{
		iPeekPrevStorage[count].iStoreName.Zero();
		iPeekPrevStorage[count].iMems = 0;
		}
	iStoreCounter = 0;
	
	ParseBufferLC(ETrue);		// we need the list separators
	RemoveUnsolicitedStrings(); // Ensure we remove any unsolicited messages
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);

	// +CPMS: (list of supported mem1), (list of supported mem2), (list of supported mem3)

	entry=iter++;
	if (entry==NULL || entry->iResultPtr != KCPMSResponseString)
		User::Leave(KErrGeneral);

	TUint32 memtype = KMemTypeReadAndDeleted;
	TInt listCounter = 0;

	while (entry = iter++, entry != NULL)
		{
		TPtrC8 param(entry->iResultPtr);
		if (param == KOpenBracketCharacter)
			{
			if (listCounter>2)
				User::Leave(KErrGeneral);
			continue;
			}
		if (param == KCloseBracketCharacter)
			{
			listCounter++;
			memtype <<= 1;
			continue;
			}
		// Should be a known mem type
		if (param == KMTStorage)
			continue;			// Ignore MT storage
		if (param != KBMStorage &&
			param != KMEStorage &&
			param != KSMStorage &&
			param != KTAStorage)
			{
			continue;
			}
		for (count = 0 ; count < iStoreCounter ; count++)
			{
			if (param == iPeekPrevStorage[count].iStoreName)
				break;
			}
		if (count == iStoreCounter) // ie: The current <mem> being looked at (param) has not been seen before
			{
			iPeekPrevStorage[count].iStoreName = param;
			iStoreCounter++;
			}
		iPeekPrevStorage[count].iMems |= memtype;
		}
	CleanupStack::PopAndDestroy();
	LOGTEXT2(_L8("CATSmsMemoryStorage\t%d memory stores found"), iStoreCounter);
	for (count=0; count<iStoreCounter; count++)
		{
		LOGTEXT3(_L8("CATSmsMemoryStorage\t\tStore %S supports %x"), 
			&iPeekPrevStorage[count].iStoreName, iPeekPrevStorage[count].iMems);
		}
	}

TInt CATSmsMemoryStorage::GetNumberOfMessageStores()
//
// Returns to client, the number of different message stores (mem1, mem2 etc)
//
	{
	return iStoreCounter;
	}

TInt CATSmsMemoryStorage::WhichPreferredMem(TInt aIndex)
//
//	Information from Client, telling TSY which is the preferred memory.
//
	{
	TInt ret(KErrNone);

	iClientPreferredMem = aIndex;
	if (iClientPreferredMem > (iStoreCounter - 1))
		ret=KErrNotSupported;

	return ret;
	}

void CATSmsMemoryStorage::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempts to halt the process
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));		
	LOGTEXT(_L8("CATSmsMemoryStorage:\tCancelling Command"));

	LOGTEXT2(_L8("Current state TSY is cancelling from %d"), iState);
	// Wait for convenient place in state machine to cancel
	iCancelled=ETrue;
	}

void CATSmsMemoryStorage::Complete(TInt aError)
	{
	Complete(aError,EReadCompletion);
	}

void CATSmsMemoryStorage::Complete(TInt aError,TEventSource aSource)
	{
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	iOKExpectString = NULL;
	iErrorExpectString = NULL;
	CATCommands::Complete(aError,aSource);
	iTelObject->ReqCompleted(iReqHandle, aError);
	if (aSource==EWriteCompletion)
		iIo->Read();
	iState = EATNotInProgress; 
	}	

void CATSmsMemoryStorage::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iTelObject->ReqCompleted(iReqHandle, aStatus);
		iState = EATNotInProgress;
		}
	}

void CATSmsMemoryStorage::CopyDataFromCATInit(CATInit* aInit)
//
// CATInit creates a separate instance of CATSmsMemoryStorage and uses this to get
// enumeratestore info during initialisation of the TSY. CMobileSmsMessaging creates another
// instance of CATSmsMemoryStorage, but this new instance requires a lot of the information
// contained within the CATInit instance. Due to iTelObject problems, we can't just do
// a iATSmsMemoryStorage = iInit->iEnumerate; during the ConstructL() of CMobileSmsMessaging.
//
	{
	iStoreCounter = aInit->iEnumerate->iStoreCounter;
	for (TInt i = 0; i < KStorageArraySize ; i++)
		iPeekPrevStorage[i] = aInit->iEnumerate->iPeekPrevStorage[i];
	}

void CATSmsMemoryStorage::GetPrefMemL()
//
// Parse result of AT+CPMS?
// this will allocate the phone's pref'd memory to the iPhoneGlobals->iPhonePrefMem local storage.
//
	{
	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	
	// +CPMS: <mem1>,<used1>,<total1>,<mem2>,<used2>,<total2>,<mem3>,<used3>,<total3>

	entry = iter++;		
	if (entry==NULL || entry->iResultPtr != KCPMSResponseString)
		User::Leave(KErrNotFound);

	entry = iter++;
	if (entry==NULL || 
	    (entry->iResultPtr != KBMStorage &&
		  entry->iResultPtr != KMEStorage &&
		  entry->iResultPtr != KSMStorage &&
		  entry->iResultPtr != KTAStorage))
		{
		User::Leave(KErrNotFound);
		}

	iPhoneGlobals->iPhonePrefMem = entry->iResultPtr;

	LOGTEXT2(_L8("CATSmsMemoryStorage:\tPreferred Memory %S"), &iPhoneGlobals->iPhonePrefMem);
	CleanupStack::PopAndDestroy();
	}

TInt CATSmsMemoryStorage::SMSStoreCaps(TDes& aName)
/**
 * This method returns the capabilities for a specific SMS store.
 */
	{
	TInt caps = 0;

	if ((aName.CompareF(KETelMeSmsStore)==KErrNone) ||
		(aName.CompareF(KETelIccSmsStore)==KErrNone))
		{
		caps |= RMobileSmsStore::KCapsUnreadMessages;
		caps |= RMobileSmsStore::KCapsReadMessages;
		caps |= RMobileSmsStore::KCapsUnsentMessages;
		caps |= RMobileSmsStore::KCapsSentMessages;
		caps |= RMobileSmsStore::KCapsGsmMessages;
		caps |= RMobilePhoneStore::KCapsIndividualEntry;
		caps |= RMobilePhoneStore::KCapsReadAccess;
		caps |= RMobilePhoneStore::KCapsWriteAccess;
		return caps;
		}
	return caps; // If the store was not found zero is returned
	}	
