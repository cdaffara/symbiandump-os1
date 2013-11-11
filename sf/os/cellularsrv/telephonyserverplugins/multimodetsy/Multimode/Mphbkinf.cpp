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
// \file MPHBKINF.CPP
// Multimode TSY Phone book Implementation file.
// This file contains the implementation of the CATPhoneBookInfo class, which is used
// to obtain Phone book information. 
// This class does all three AT+CPBS commands and an AT+CPBR:
// AT+CPBS=? to get the supported storage types;
// AT+CPBS=<storage> to set the storage type;
// AT+CPBS? to get the 'used' and 'total' slots;
// AT+CPBR=? to get the maximum length of the number and text fields;
// A TPhoneBookInfo struct containing all the relevant information about the 
// specified phone book is returned.
// 
//


#include "Mphbkcom.h"
#include "mSLOGGER.H"
#include "mPHBOOK.H"
#include "ATIO.H"
#include "Matstd.h"
#include "mphbkinf.h"


// AT command
_LIT8(KPhoneBookReadTest,"AT+CPBR=?");

//
const TUint KHyphenChar='-';

//
GLDEF_C void ConvertStorageTypeToName(const TStorageType& aStorageType,TDes& aName)
/**
 * This function converts a storage type (phone book memory) to its name equivalent.
 */
	{
	if (aStorageType==KFDStorage)
		aName.Copy(KETelIccFdnPhoneBook);
	else if (aStorageType==KMEStorage)
		aName.Copy(KETelMeAdnPhoneBook);
	else if (aStorageType==KMTStorage)
		aName.Copy(KETelCombinedAdnPhoneBook);
	else if (aStorageType==KSMStorage)
		aName.Copy(KETelIccAdnPhoneBook);
	else if (aStorageType==KTAStorage)
		aName.Copy(KETelTaAdnPhoneBook);
	else if (aStorageType==KBMStorage)
		aName.Copy(KETelIccBdnPhoneBook);
	else if (aStorageType==KDCStorage)
		aName.Copy(KETelMeDialledPhoneBook);
	else if (aStorageType==KENStorage)
		aName.Copy(KETelEmergencyNumberStore);
	else if (aStorageType==KMCStorage)
		aName.Copy(KETelMeMissedPhoneBook);
	else if (aStorageType==KRCStorage)
		aName.Copy(KETelMeReceivedPhoneBook);
	else if (aStorageType==KLDStorage)
		aName.Copy(KETelIccLndPhoneBook);
	else if (aStorageType==KONStorage)
		aName.Copy(KETelOwnNumberStore);
	else if (aStorageType==KSNStorage)
		aName.Copy(KETelIccSdnPhoneBook);
	else
		// we don't know any other storage type
		aName.SetLength(0);
	}

//
// CATPhoneBookInfo Implementation
//

CATPhoneBookInfo* CATPhoneBookInfo::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
/**
 * Standard 2 phase constructor. 
 */
	{
	CATPhoneBookInfo* r=new(ELeave) CATPhoneBookInfo(aIo,aTelObject,aInit,aPhoneGlobals);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CATPhoneBookInfo::CATPhoneBookInfo(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CATCommands(aIo,aTelObject,aInit,aPhoneGlobals), iCompleted(EFalse)
	{
	}

CATPhoneBookInfo::~CATPhoneBookInfo()
/**
 * Destructor.
 */
	{
	delete iStorageTypeArray;
	}

void CATPhoneBookInfo::ConstructL()
/**
 * This function calls the ConstructL() function of the CATCommands class. 
 * It also allocates memory for an array of supported phonebook types.
 */
	{
	CATCommands::ConstructL();
	iStorageTypeArray=new(ELeave) CArrayFixFlat<TStorageType>(5);
	}	

void CATPhoneBookInfo::Start(TTsyReqHandle aTsyReqHandle,TAny* aParams)
/** 
 * Start function; initializes TPhoneBookInfo struct members to Unknown
 * and starts the AT+CPBS? query command.
 */
	{
	LOGTEXT(_L8("Starting AT+CPBS? Command"));
	
	iReqHandle=aTsyReqHandle;

	if (aParams)
		iInfoPckg=static_cast<RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg*>(aParams);

// if this is the first time then get a list of the supported types, otherwise, just
// get info for the required index

	if (iStorageTypeArray->Count()>0)
		{
		SetIndexByStorageType();
		StartSelect();
		}
	else
		{
		WriteExpectingResults(KPhoneBookStorageTest(),3);
		__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
		iState=EListWaitForWriteComplete;
		}
	}

void CATPhoneBookInfo::EventSignal(TEventSource aEventSource)
/** 
 * This function contains the State machine for retrieving phone book information.
 *
 * Waits for the AT+CPBS=? command to be written to the comm port, parses the phone's
 * response and initializes an array containing the supported phone book memories. It
 * then sets a phonebook type, and retrieves information pertaining to the phone book:
 * total no. of slots, no. of used slots, the maximum length of the text and number fields.
 */
	{
	LOGTEXT2(_L8("CATPhoneBookInfo::EventSignal, aEventSource=%d"),aEventSource);

	if (aEventSource==ETimeOutCompletion)
		{
		LOGTEXT(_L8("Timeout Error during getting phone book info"));
		RemoveStdExpectStrings();
		Complete(KErrTimedOut,aEventSource);
		return;
		}
	
	switch (iState)
		{
	case EListWaitForWriteComplete:
		WriteComplete(aEventSource,EListWaitForReadComplete);
		LOGTEXT(_L8("CATPhoneBookInfo::EventSignal processed EListWaitForWriteComplete"));
		break;

	case EListWaitForReadComplete:
		__ASSERT_ALWAYS(aEventSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret!=KErrNone)
				{
				Complete(ret,aEventSource);
				return;
				}
			TRAP(ret,ParseListResponseL()); // parse the list of supported phone book types.
			if (ret!=KErrNone)
				{
				Complete(ret,aEventSource);
				return;
				}
			}
		SetIndexByStorageType();		
		StartSelect();	// Set a phone book memory.
		LOGTEXT(_L8("CATPhoneBookInfo::EventSignal processed EListWaitForReadComplete"));
		break; 
		
	case ESelectWaitForWriteComplete:
		WriteComplete(aEventSource,ESelectWaitForReadComplete);
		LOGTEXT(_L8("CATPhoneBookInfo::EventSignal processed ESelectWaitForWriteComplete"));
		break;

	case ESelectWaitForReadComplete:
		__ASSERT_ALWAYS(aEventSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret!=KErrNone)
				{
				Complete(ret,aEventSource);
				return;
				}
			// no parsing this time
			} 
		iPhoneGlobals->iPhoneStatus.iLastAccessedPhoneBook=iStorageType;
		StartGetInfo();	// Transmit AT+CPBS? cmd; get the phone book's total and used slots
		LOGTEXT(_L8("CATPhoneBookInfo::EventSignal processed ESelectWaitForReadComplete"));
		break;

	case EGetInfoWaitForWriteComplete:
		WriteComplete(aEventSource,EGetInfoWaitForReadComplete);
		LOGTEXT(_L8("CATPhoneBookInfo::EventSignal processed EGetInfoWaitForWriteComplete"));
		break;

	case EGetInfoWaitForReadComplete:
		__ASSERT_ALWAYS(aEventSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret!=KErrNone)
				{
				Complete(ret,aEventSource);
				return;
				}
			TRAP(ret,ParseGetInfoResponseL());	// Parse the phone's response to the AT+CPBS? command
			if (ret!=KErrNone)
				{
				Complete(ret,aEventSource);
				return;
				}
			}
		StartGetMaxLength();  // Transmit AT+CPBR=? cmd; get the maximum text and number length.
		LOGTEXT(_L8("CATPhoneBookInfo::EventSignal processed EGetInfoWaitForReadComplete"));
		break;

	case EGetMaxLengthWaitForWriteComplete:
		WriteComplete(aEventSource,EGetMaxLengthWaitForReadComplete);
		LOGTEXT(_L8("CATPhoneBookInfo::EventSignal processed EGetMaxLengthWaitForWriteComplete"));
		break;

	case EGetMaxLengthWaitForReadComplete:
		__ASSERT_ALWAYS(aEventSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret!=KErrNone)
				{
				Complete(ret,aEventSource);
				return;
				}
			TRAP(ret,ParseGetMaxLengthResponseL()); // Parse the phone's response to the AT+CPBR=? command.
			Complete(ret,aEventSource);
			}
		LOGTEXT(_L8("CATPhoneBookInfo::EventSignal processed EGetMaxLengthWaitForReadComplete"));
		break;
			
	case EIdle:
	default:
		break;
		}
	}
	
void CATPhoneBookInfo::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
/**
 * This function is called if an error occurs. It cancels the relevant timer and 
 * completes the request before setting the state to Idle.
 */
	{
	if (iState!=EIdle)
		{
		iIo->WriteAndTimerCancel(this);
		if (iReqHandle) 
			{
			iTelObject->ReqCompleted(iReqHandle,aStatus);
			}
		iState=EIdle;
		}
	}

void CATPhoneBookInfo::SetStorageType(TStorageType aStorageType)
/**
 * Sets the client's desired phonebook type. 
 */
	{
	iStorageType=aStorageType;
	}

void CATPhoneBookInfo::SetIndexByStorageType()
/**
 * Sets the client index using the storage type supplied by the client 
 */
	{
	iIndex=KErrUnknown;
	for (TInt i=0; i<iStorageTypeArray->Count(); i++)
		{
		if ((*iStorageTypeArray)[i] == iStorageType)
			{
			iIndex=i;
			return;
			}
		}
	}

void CATPhoneBookInfo::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This function is used to prematurely stop the state machine.  This would usually 
 * occur following a client cancel request.
 */
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("Cancelling phone book get info command"));

	switch (iState)
		{
	case EListWaitForWriteComplete:
	case ESelectWaitForWriteComplete:
	case EGetInfoWaitForWriteComplete:
	case EGetMaxLengthWaitForWriteComplete:
		Complete(KErrCancel,EWriteCompletion);
		return;
	
	case EListWaitForReadComplete:
	case ESelectWaitForReadComplete:
	case EGetInfoWaitForReadComplete:
	case EGetMaxLengthWaitForReadComplete:
		Complete(KErrCancel,EReadCompletion);
		return;

	case EIdle:
	default:
		return;
		}
	}


void CATPhoneBookInfo::StartSelect()
/** 
 * This function selects a phonebook memory.
 */
	{
	TBuf8<KGenericBufferSize> buf;
	
	if (iPhoneGlobals->iPhoneStatus.iLastAccessedPhoneBook!=iStorageType)
		{
		// We need to select the phonebook using AT+CPBS=<storage>
		if (iIndex>=0)
			buf.Format(KPhoneBookStorageSet,&(*iStorageTypeArray)[iIndex]);
		else
			Panic(EUnknownPhoneBookStorageTypeIndex);

		WriteExpectingResults(buf,3);
		__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
		iState=ESelectWaitForWriteComplete;
		}
	else
		// We already have correct phonebook selected
		StartGetInfo();
	}

void CATPhoneBookInfo::StartGetInfo()
/**
 * This function transmits an AT+CPBS? command. 
 */
	{
	WriteExpectingResults(KPhoneBookStorageRead(),3);
	
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iState=EGetInfoWaitForWriteComplete;
	}

void CATPhoneBookInfo::StartGetMaxLength()
/** 
 * This function transmits an AT+CPBR=? command.
 */
	{	
	WriteExpectingResults(KPhoneBookReadTest(),3);
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iState=EGetMaxLengthWaitForWriteComplete;
	}

void CATPhoneBookInfo::ParseListResponseL()
/**
 * This function parses the phone's response to the AT+CPBS=? command.
 * It initializes an array with the supported phone book types.
 */
	{
	TInt count=iStorageTypeArray->Count();
	if (count>0)
		iStorageTypeArray->Delete(0,count);

	ParseBufferLC(EFalse, ':');
	if (iRxResults.IsEmpty())
		User::Leave(KErrNotFound);
	else
		{
		TDblQueIter<CATParamListEntry> iter(iRxResults);
		CATParamListEntry* entry=iter++;
		if (entry!=NULL)
			{
			entry->Deque();			// skip the +CPBS
			delete entry;
			}
		else
			User::Leave(KErrNotFound);
		
		entry=iter++;
		TStorageType storageType;
		while (entry!=NULL)
			{
			if (entry->iResultPtr.Length()>2)
				User::Leave(KErrArgument);
			storageType=entry->iResultPtr;
			iStorageTypeArray->AppendL(storageType);
			entry->Deque();
			delete entry;
			entry=iter++;
			}
		}
	CleanupStack::PopAndDestroy();		// parsed buffer
	}

void CATPhoneBookInfo::ParseGetInfoResponseL()
/** 
 * This function parses the phone's response to the AT+CPBS? command.
 * The variables containing the used and total slots are assigned the
 * corresponding values returned by the phone.
 */
	{
	ParseBufferLC(EFalse, ':');
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++;	
	if (entry!=NULL)
		{
		entry->Deque();			// skip the +CPBS
		delete entry;
		}
	else
		User::Leave(KErrNotFound);

	entry=iter++;
	if (entry!=NULL)
		{
		entry->Deque();			// skip the storage name
		delete entry;
		}
	else
		User::Leave(KErrNotFound);
	
	entry=iter++;
	if (entry!=NULL)
		{
		CATParamListEntry::EntryValL(entry,iPhbkInfo.iUsedEntries);
		entry=iter++;
		}

	if (entry!=NULL)
		{
		CATParamListEntry::EntryValL(entry,iPhbkInfo.iTotalEntries);
		}

	CleanupStack::PopAndDestroy();		// parsed buffer
	}

void CATPhoneBookInfo::ParseGetMaxLengthResponseL()
/** 
 * This function parses the phone's response to the AT+CPBR=? command -> e.g. 
 * +CPBR: (101-200),020,014 or +CPBR: (1-10),32,14.
 * The variables containing the maximum text and number lengths are assigned 
 * the corresponding values returned by the phone.
 */
	{
	ParseBufferLC(EFalse, ':');
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++;	
	if (entry==NULL)
		User::Leave(KErrNotFound);
	
	// skip the +CPBR
	entry=iter++;

	// T28 returns CPBR:(),20,8. () should be tackled correctly.
	if (entry->iResultPtr.Locate(KHyphenChar)==KErrNotFound)
		entry=iter++;
	else
		{
		while (entry!=NULL && entry->iResultPtr.Locate(KHyphenChar)>KErrNone)
			{
			const TInt hyphenPos=entry->iResultPtr.Locate(KHyphenChar);
			TPtrC8 beforeHyphen=entry->iResultPtr.Left(hyphenPos);
			TPtrC8 afterHyphen=entry->iResultPtr.Mid(hyphenPos+1);

			// Get the index offset that'll need to be used for
			// read, write and delete phone book store operations
			{		// curly brackets used to scope TLex8 object
			TLex8 lex(beforeHyphen);
			(void)User::LeaveIfError(lex.Val(iIndexOffset));
			}
			iIndexOffset--;		// decrement offset by one as it is an offset and not a starting index value

			LOGTEXT2(_L8("CATPhoneBookInfo::ParseGetMaxLengthResponseL iIndexOffset=%d"),iIndexOffset);

			if (iPhbkInfo.iTotalEntries<=0)
				{	
				// The optional <used> and <total> parameters were not present in AT+CPBS=?
				// Try to get this info from AT+CPBR? instead
				TLex8 lex(afterHyphen);
				(void)User::LeaveIfError(lex.Val(iPhbkInfo.iTotalEntries));
				}
			entry=iter++;
			}
		}
	
	if (entry!=NULL)
		{
		CATParamListEntry::EntryValL(entry,iPhbkInfo.iMaxNumLength);
		entry=iter++;
		}
	else
		// not supported by this ME
		iPhbkInfo.iMaxNumLength=KErrNotFound;

	if (entry!=NULL)
		CATParamListEntry::EntryValL(entry,iPhbkInfo.iMaxTextLength);
	else
		// not supported by this ME
		iPhbkInfo.iMaxTextLength=KErrNotFound;

	CleanupStack::PopAndDestroy();		// parsed buffer
	}

void CATPhoneBookInfo::WriteComplete(TEventSource aSource,TPhoneBookInfoState aState)
/**
 * This function is used by the Write states in the State machine. Performs common
 * write routines and sets the new state.
 */
	{
	__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
	AddStdExpectStrings();
	iIo->SetTimeOut(this);
	iState=aState;	
	}

void CATPhoneBookInfo::Complete(TInt aError, TEventSource aSource)
/** 
 * This function completes the Client request.
 * It sets the Write Access for the phone book and sets the state to Idle.
 */
	{
	LOGTEXT2(_L8("CATPhoneBookInfo::Complete aError=%d"),aError);
	
	if (aError==KErrNone)
		{
		iPhbkInfo.iCaps = RMobilePhoneStore::KCapsReadAccess; 
		if (CPhoneGlobals::IsWriteAccess(iStorageType))
			iPhbkInfo.iCaps |= RMobilePhoneStore::KCapsWriteAccess;

		ConvertStorageTypeToName(iStorageType, iPhbkInfo.iName);

		iIo->WriteAndTimerCancel(this);
		iIo->RemoveExpectStrings(this);

		iState=EIdle;
		// Mark the information retrieval as completed - so it is not repeated
		iCompleted=ETrue;

		// Write back data if started by a client request
		if (iReqHandle)
			{
			// Check if client was actually asking for RMobilePhoneStore::TMobilePhoneStoreInfoV1
			// rather than larger RMobilePhonebookStore::TMobilePhoneBookInfoV1

			RMobilePhoneStore::TMobilePhoneStoreInfoV1& storeInfo = (*iInfoPckg)();

			storeInfo.iType=RMobilePhoneStore::EPhoneBookStore;
			storeInfo.iUsedEntries=iPhbkInfo.iUsedEntries;
			storeInfo.iTotalEntries=iPhbkInfo.iTotalEntries;
			storeInfo.iName=iPhbkInfo.iName;
			storeInfo.iCaps=iPhbkInfo.iCaps;

			if (storeInfo.ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV1)
				{
				RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg* phbkPckg 
					= reinterpret_cast<RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg*>(iInfoPckg);
				RMobilePhoneBookStore::TMobilePhoneBookInfoV1& phbkInfo = (*phbkPckg)();
				phbkInfo.iMaxNumLength=iPhbkInfo.iMaxNumLength;
				phbkInfo.iMaxTextLength=iPhbkInfo.iMaxTextLength;
				phbkInfo.iLocation=RMobilePhoneBookStore::ELocationUnknown; // could change this later!
				}
		
			CATCommands::Complete(aError,aSource);
			iTelObject->ReqCompleted(iReqHandle,aError);
			}
		}
	else
		{
		// There was an error or the request was cancelled
		iIo->WriteAndTimerCancel(this);
		iIo->RemoveExpectStrings(this);
		iState=EIdle;

		CATCommands::Complete(aError,aSource);
		if (iReqHandle)
			iTelObject->ReqCompleted(iReqHandle,aError);
		}
	}
