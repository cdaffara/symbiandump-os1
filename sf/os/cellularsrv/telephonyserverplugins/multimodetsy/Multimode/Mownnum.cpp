// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Deal with Network & Operator information
// 
//

#include <et_phone.h>
#include "Mownnum.h"
#include "mSLOGGER.H"
#include "ATIO.H"
#include "Matstd.h"
#include <etelmm.h>
#include <mmlist.h>
#include "et_phone_util.h"

// Get the own numbers from the phone
// NB - not supported by all phones.

_LIT8(KGetOwnNumbersCommand,"AT+CNUM");
_LIT8(KGetOwnNumbersResponse,"+CNUM");

CATOwnNumbers::CATOwnNumbers(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CATCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}

CATOwnNumbers* CATOwnNumbers::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATOwnNumbers* self = new(ELeave) CATOwnNumbers(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATOwnNumbers::~CATOwnNumbers()
	{
	if (iIo != NULL)
		{
		iIo->RemoveExpectStrings(this);
		}
	delete iOwnNumbers;
	}

void CATOwnNumbers::Start(TTsyReqHandle aTsyReqHandle, TAny* /*aParam*/)
	{
	LOGTEXT(_L8("MMTsy:\tCATOwnNumbers:\tStarting Get Own Number"));
	iReqHandle = aTsyReqHandle;
	WriteExpectingResults(KGetOwnNumbersCommand, 3);	
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL,Panic(EGeneral));
	iState=EATImsiWaitForWriteComplete;
	}

void CATOwnNumbers::EventSignal(TEventSource aSource)
	{
	if (aSource==ETimeOutCompletion)
		{
		LOGTEXT(_L8("MMTsy:\tCATOwnNumbers:\tTimeout Error during Get Own Number read"));
		RemoveStdExpectStrings();
		Complete(KErrTimedOut,aSource);
		return;
		}

	switch(iState)
		{
		case EATImsiWaitForWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATImsiReadComplete;
			}
		break;

		case EATImsiReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret==KErrNone)
				TRAP(ret,ParseResponseL());
			Complete(ret,aSource);
			}
			break;
	
		case EATWaitForStopState:
			__ASSERT_ALWAYS(aSource==EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			Complete(KErrCancel, aSource);
			}
			break;


		default:
			break;
		}//switch
	}//EventSignal

void CATOwnNumbers::Stop(TTsyReqHandle aTsyReqHandle)
	{
	if(iReqHandle == TSY_HANDLE_INIT_VALUE) // default value
		{
		// it seems the Own numbers service has not been started
		// just return
		LOGTEXT(_L8("MMTsy:\tCATOwnNumbers:\tCancel called, Error - invalid request handle."));
		return;
		}
	
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle, Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("MMTsy:\tCATOwnNumbers:\tCancel called."));
	switch(iState)
		{
		case EATNotInProgress:
		case EATImsiWaitForWriteComplete:
			{
			LOGTEXT2(_L8("Current state TSY is cancelling from %d"), iState);
			Complete(KErrCancel, EReadCompletion);
			}
			break;

		default:
			{
			LOGTEXT(_L8("MMTsy:\tCATOwnNumbers:\tStop, now waiting for expected modem response"));
			AddStdExpectStrings();
			iIo->SetTimeOut(this);
			iState = EATWaitForStopState;
			}
			break;

		}
	}


void CATOwnNumbers::Complete(TInt aError,TEventSource aSource)
	{
	if (aError==KErrNone || aError==KErrNotFound)
		iState = EATCompleted;
	else if (aError==KErrGeneral)
		{	
		iState = EATNotSupported;
		aError = KErrNotSupported;
		}
	else
		iState = EATNotInProgress;
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	iOKExpectString = NULL;
	iErrorExpectString = NULL;
	CATCommands::Complete(aError,aSource);
	LOGTEXT2(_L8("GsmTsy:CATOwnNumbers:\tCATOwnNumbers completed with error code : %d"), aError);
	iTelObject->ReqCompleted(iReqHandle, aError);
	if (aSource==EWriteCompletion)
		iIo->Read();
	}

void CATOwnNumbers::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iState = EATNotInProgress;
		iTelObject->ReqCompleted(iReqHandle, aStatus);
		}
	}

void CATOwnNumbers::ParseResponseL()
	{
// +CNUM: [text],number,type[,speed,service[,itc]]
// etc.

	
	CMobilePhoneONList* oNList=CMobilePhoneONList::NewL();
	CleanupStack::PushL(oNList);	

//
	ParseBufferLC(EFalse, ':');
//

	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++;
	while (entry)
		{
		if (entry->iResultPtr!=KGetOwnNumbersResponse)
			User::Leave(KErrGeneral);
		//
		// provide some default values
		RMobileONStore::TMobileONEntryV1 info;
		info.iMode = RMobilePhone::ENetworkModeGsm; // Hard coded since this tsy just handles GSM.
		
		entry=iter++;
		if (!entry)
			User::Leave(KErrGeneral);
		info.iText.Copy(entry->iResultPtr); 

		entry=iter++;
		if (!entry)
			User::Leave(KErrGeneral);
		info.iNumber.iTelNumber.Copy(entry->iResultPtr);

		entry=iter++;
		info.iNumber.iTypeOfNumber = NumberTypefromGSMToMM(CATParamListEntry::EntryValL(entry)); // How is this to be used

		// now the optional values
		entry=iter++;
		if (entry && entry->iResultPtr!=KGetOwnNumbersResponse)
			{
			//The baerer speed that should have been set here is not supported in the TMobileONEntryV1 class
			entry=iter++;
			TInt service=CATParamListEntry::EntryValL(entry);
			switch (service)
				
				{
			case KServiceVoice:
				info.iService = RMobilePhone::EVoiceService;
				break;
			case KServiceFax:
				info.iService = RMobilePhone::EFaxService;
				break;
			case KAsynchModem:
			case KSynchModem:
			case KPADAccess:
			case KPacketAccess:
				info.iService = RMobilePhone::ECircuitDataService;
				break;

			default:
				info.iService = RMobilePhone::EServiceUnspecified;
				break;
				}
			//
			// further optionality
			entry=iter++;
			if (entry && entry->iResultPtr!=KGetOwnNumbersResponse)
				{
				// Information about the transfer capabilities are not used in 
				// TMobileONEntryV1(the "itc" field is optionaly reurned by AT+CNUM).
				// If it had been available it hade been set here.
				entry=iter++;
				} 
			}
		oNList->AddEntryL(info);
		}
	
	CleanupStack::PopAndDestroy();		// results
	CleanupStack::Pop();				// numbers
	iOwnNumbers = oNList;

	switch (iCommand)
		{
	case EInfo:
		Info();
		break;
	case EEntry:
		(void)User::LeaveIfError(Entry(iResultIndex));
		break;
		}
	}

void CATOwnNumbers::Info() const
/**
 * This method sets the information about the own number store to be passed 
 * back to the client with cached inforamtion.
 */
	{
	RMobilePhoneStore::TMobilePhoneStoreInfoV1& storeInfo = (*iInfoPckg)();

	storeInfo.iType=RMobilePhoneStore::EOwnNumberStore;
	storeInfo.iUsedEntries = iOwnNumbers->Enumerate();
	storeInfo.iTotalEntries = iOwnNumbers->Enumerate();
	storeInfo.iName.Copy(KETelOwnNumberStore);
	storeInfo.iCaps=RMobilePhoneStore::KCapsIndividualEntry | RMobilePhoneStore::KCapsReadAccess;

	if (storeInfo.ExtensionId()==RMobilePhoneStore::KETelMobileONStoreV1)
		{
		RMobileONStore::TMobileONStoreInfoV1Pckg* onPckg 
			= reinterpret_cast<RMobileONStore::TMobileONStoreInfoV1Pckg*>(iInfoPckg);
		RMobileONStore::TMobileONStoreInfoV1& onInfo = (*onPckg)();
		onInfo.iNumberLen=RMobilePhone::KMaxMobileTelNumberSize;
		onInfo.iTextLen=RMobileONStore::KOwnNumberTextSize;
		}
	}

TInt CATOwnNumbers::Entry(TInt aIndex) const
/**
 * This method returnes the own number specified by aIndex.
 * If aIndex should be out of range KErrNotFound is returned.
 */	
	{
	__ASSERT_DEBUG(iOwnNumbers!= NULL, PanicClient(KErrGeneral));

	RMobileONStore::TMobileONEntryV1& oNEntry = (*iEntryPckg)();
	if (TUint(aIndex)>=TUint(iOwnNumbers->Enumerate()))
		{
		// If the index is out of range
		return KErrNotFound;
		}

	// Get the entry specified by the aIndex.
	TRAPD(ret,(oNEntry = iOwnNumbers->GetEntryL(aIndex)));
		
	return ret;
	}


void CATOwnNumbers::GetInfo(TTsyReqHandle aTsyReqHandle, TDes8* aInfo)
/**
 * Execute the command if we have not yet been run
 * Report the own number info if we have already evaluated them
 */
	{
	iInfoPckg=static_cast<RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg*>(aInfo);
	switch (iState)
		{
	case EATCompleted:
		Info();
		iTelObject->ReqCompleted(aTsyReqHandle, KErrNone);
		break;
	case EATNotSupported:
		iTelObject->ReqCompleted(aTsyReqHandle, KErrNotSupported);
		break;
	default:
		iCommand=EInfo;
		ExecuteCommand(aTsyReqHandle,aInfo);
		break;
		}
	}


void CATOwnNumbers::GetEntry(TTsyReqHandle aTsyReqHandle, TDes8* aEntry)
/** 
 * Execute the command if we have not yet been run.
 * @param aTsyReqHandle The request ID
 * @param aEntry This argument is of type TMobileONEntryV1Pckg.
 */
	{
	// Unpack the the TMobileONEntryV1 (aEntry)
	iEntryPckg = static_cast<RMobileONStore::TMobileONEntryV1Pckg*>(aEntry);
	RMobileONStore::TMobileONEntryV1& oNEntry = (*iEntryPckg)();

	// Extract the index of the own number to get.
	// Subtract 1 because SIM slots start at 1 but our list will start at 0
	// Client has asked for SIM slot number
	iResultIndex = oNEntry.iIndex - 1 ;

	switch (iState)
		{
	case EATCompleted:
		{
		TInt r=Entry(iResultIndex);
		if (r!=KErrNone)
			{
			iTelObject->ReqCompleted(aTsyReqHandle, r);
			return;
			}
		iTelObject->ReqCompleted(aTsyReqHandle, KErrNone);
		}
		break;
	case EATNotSupported:
		iTelObject->ReqCompleted(aTsyReqHandle, KErrNotSupported);
		break;
	default:
		iCommand=EEntry;
		ExecuteCommand(aTsyReqHandle, &oNEntry);
		break;
		}
	}//End of GetEntry


RMobilePhone::TMobileTON CATOwnNumbers::NumberTypefromGSMToMM(TUint aTypeOfNumberGSM)
/** Maping from MM Number Type To GSM Number Type
 * 
 * This method maps the GSM way of representing a type of telefon number 
 * to the MM standard.
 * 
 */
	{
	switch (aTypeOfNumberGSM)
		{
	case 129: // Nationality unknown (GSM)		
		return RMobilePhone::EUnknownNumber; //  (MM)
	case 145: // International Number (GSM)	
		return RMobilePhone::EInternationalNumber; //  (MM)
	case 161: // National Number (GSM)
		return RMobilePhone::ENationalNumber; //  (MM)
	default:
        return RMobilePhone::EUnknownNumber; //  (MM)
		}
	}
