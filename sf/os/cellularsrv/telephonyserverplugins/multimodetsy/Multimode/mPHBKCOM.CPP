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
// Multimode TSY Phone book Implementation file.
// This file contains the implementation of the CATPhoneBookCommands, CATPhoneBookInit,
// CATPhoneBookWrite and CATPhoneBookDelete classes. 
// 
//

/**
 @file
*/

#include "Mphbkcom.h"
#include "mSLOGGER.H"
#include "mPHBOOK.H"
#include "NOTIFY.H"
#include "ATIO.H"
#include "Matstd.h"

// AT commands

_LIT8(KPhoneBookReadAll,"AT+CPBR=");	// Modified from "AT+CPBR=1,n; 1 is not always the starting index
_LIT8(KPhoneBookReadResponse,"+CPBR:*");

_LIT8(KPhoneBookWriteWithIndex,"AT+CPBW=%d,\"%S\",%d,\"%S\"");
_LIT8(KPhoneBookWriteWithIndexPrependedPlus,"AT+CPBW=%d,\"+%S\",%d,\"%S\"");
_LIT8(KPhoneBookDelete,"AT+CPBW=%d");
_LIT8(KPhoneBookAlternativeDelete,"AT+CPBW=%d,\"\",,\"\"");


const TInt KPhoneBookTimeForExtraRxData=100;

// Time-outs

const TInt KPhoneBookReadTimeout=30;	//< Time-out used when reading the phone books (in seconds). For sizeable phonebooks, this can be large.
										//< The Siemens S25 is the slowest phone observed. The Nokia 8210 can take 14s to read an empty 200 entry phonebook.

//
// CATPhoneBookCommands definitions
//

CATPhoneBookCommands::CATPhoneBookCommands(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CATCommands(aIo,aTelObject,aInit,aPhoneGlobals), iState(EATPhoneBookCommandIdle)
	{}

CATPhoneBookCommands::~CATPhoneBookCommands()
/**
 * Destructor.
 */
	{
	iIo->RemoveExpectStrings(this);
	}

void CATPhoneBookCommands::EventSignal(TEventSource aEventSource)
/**
 * This function contains the State machine for the phonebook commands whose classes
 * inherit from the CATPhoneBookCommands class.
 *
 * Note: The cases EATPhoneBookCommandWaitForReadComplete & EATPhoneBookCommandWait-
 * ForAlternativeReadComplete are overriden by similar cases in CATPhoneBookInit::
 * EventSignal and CATPhoneBookRead::EventSignal.
 */
	{
	if (aEventSource==ETimeOutCompletion && iState!=EATPhoneBookCommandExtendedRead)
		{
		LOGTEXT(_L8("Timeout Error during phone book command"));
		RemoveStdExpectStrings();
		Complete(KErrTimedOut,aEventSource);
		return;
		}

	TInt ret=KErrNone;
	switch (iState)
		{
	case EATPhoneBookStorageSelectWaitForWriteComplete:
		__ASSERT_ALWAYS(aEventSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		AddStdExpectStrings();
		iIo->SetTimeOut(this);
		iState=EATPhoneBookStorageSelectWaitForReadComplete;
		return;

	case EATPhoneBookStorageSelectWaitForReadComplete:
		__ASSERT_ALWAYS(aEventSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			ret=ValidateExpectString();
			RemoveStdExpectStrings();
			if (ret!=KErrNone)
				{
				Complete(ret,aEventSource);
				return;
				}
			CMobilePhonebookStore* phoneBook=REINTERPRET_CAST(CMobilePhonebookStore*,iTelObject);
			iPhoneGlobals->iPhoneStatus.iLastAccessedPhoneBook=phoneBook->StorageType();
			}
		StartThisCommand();
		return;
		
	case EATPhoneBookCommandWaitForWriteComplete:
		__ASSERT_ALWAYS(aEventSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		iIo->SetTimeOut(this); 
		AddStdExpectStrings();
		iState=EATPhoneBookCommandWaitForReadComplete;
		return;

	case EATPhoneBookCommandWaitForReadComplete:
	case EATPhoneBookCommandWaitForAlternativeReadComplete:
		__ASSERT_ALWAYS(aEventSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		ret=ValidateExpectString();
		RemoveStdExpectStrings();
		if (ret!=KErrNone)
			{
			Complete(ret,aEventSource);
			return;
			}
		iIo->SetTimeOut(this,KPhoneBookTimeForExtraRxData);
		iState=EATPhoneBookCommandExtendedRead;
		return;
	
	case EATPhoneBookCommandWaitForAlternativeWriteComplete:
		__ASSERT_ALWAYS(aEventSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		iIo->SetTimeOut(this,KPhoneBookReadTimeout*KOneSecondPause);
		AddStdExpectStrings();
		iState=EATPhoneBookCommandWaitForAlternativeReadComplete;
		return;
		
	case EATPhoneBookCommandExtendedRead:
		__ASSERT_ALWAYS(aEventSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		Complete(ret,aEventSource);
		return;

	case EATPhoneBookCommandIdle:
	default:
		return;
		}
	}

void CATPhoneBookCommands::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
/**
 * This function is called if an error occurs. It cancels the relevant timer and 
 * completes the request before setting the state to Idle.
 */
	{
	if (iState!=EATPhoneBookCommandIdle)
		{
		iIo->WriteAndTimerCancel(this);
		iTelObject->ReqCompleted(iReqHandle,aStatus);
		iState=EATPhoneBookCommandIdle;
		}
	}

void CATPhoneBookCommands::StartStorageSelect()
/**
 * This function finds out which phone book memory to set and then transmits the 
 * set command ("AT+CPBS=xx") to the relevant phone book.
 */
	{
	LOGTEXT(_L8("Starting AT+CPBS= Command"));
	
	CMobilePhonebookStore* phoneBook=REINTERPRET_CAST(CMobilePhonebookStore*,iTelObject);
	if (iPhoneGlobals->iPhoneStatus.iLastAccessedPhoneBook==phoneBook->StorageType())
		{
		StartThisCommand();
		}
	else
		{
		TBuf8<KGenericBufferSize> buf;
		TStorageType storageType(phoneBook->StorageType());
		buf.Format(KPhoneBookStorageSet,&storageType);
		WriteExpectingResults(buf,3);
		__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
		iState=EATPhoneBookStorageSelectWaitForWriteComplete;
		}
	}

void CATPhoneBookCommands::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This function is used to prematurely stop the state machine.  This would usually 
 * occur following a client cancel request.
 */
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("Cancelling phone book command"));
	
	switch (iState)
		{
	case EATPhoneBookStorageSelectWaitForWriteComplete:
	case EATPhoneBookCommandWaitForWriteComplete:
	case EATPhoneBookCommandWaitForAlternativeWriteComplete:
		Complete(KErrCancel,EWriteCompletion);
		return;
	
	case EATPhoneBookStorageSelectWaitForReadComplete:
	case EATPhoneBookCommandWaitForReadComplete:
	case EATPhoneBookCommandWaitForAlternativeReadComplete:
		Complete(KErrCancel,EReadCompletion);
		return;

	case EATPhoneBookCommandExtendedRead:
		Complete(KErrCancel,ETimeOutCompletion);
		return;

	case EATPhoneBookCommandIdle:
	default:
		return;
		}
	}

void CATPhoneBookCommands::Complete(TInt aError,TEventSource aSource)
/**
 * This function completes a client request. Sets the state machine to Idle.
 */
	{
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	if (aSource==EWriteCompletion)
		iIo->Read();
	iState=EATPhoneBookCommandIdle;
	CATCommands::Complete(aError,aSource);
	iTelObject->ReqCompleted(iReqHandle,aError);
	}

//
// CATPhoneBookRead definitions
//

CATPhoneBookRead::CATPhoneBookRead(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,CATPhoneBookInfo *aInfo)
	: CATPhoneBookCommands(aIo,aTelObject,aInit,aPhoneGlobals),
	  iInfo(aInfo)
/**
 * C++ constructor
 */
	{}

CATPhoneBookRead::~CATPhoneBookRead()
/** 
 * Destructor.
 */
	{
	delete iPbBuffer;
	}

CATPhoneBookRead* CATPhoneBookRead::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,CATPhoneBookInfo *aInfo)
/**
 * Standard 2 phase constructor.
 * This method creats an instance of the CATPhoneBookRead class. 
 */
	{
	CATPhoneBookRead* r=new(ELeave) CATPhoneBookRead(aIo,aTelObject,aInit,aPhoneGlobals,aInfo);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

void CATPhoneBookRead::ConstructL()
/**
 * Standard 2nd phase constructor.
 * Creates phonebook buffer object used to populate client buffer
 */
	{
	CATCommands::ConstructL();
	iPbBuffer = new(ELeave) CPhoneBookBuffer();
	}

void CATPhoneBookRead::Start(TTsyReqHandle aTsyReqHandle,TAny* aParams)
/**
 * Start function. Its calls the StartStorageSelect() function to select 
 * the phone book memory.
 */
	{
	iReqHandle=aTsyReqHandle;
	iRead = static_cast<RMobilePhoneBookStore::TPBIndexAndNumEntries*>(aParams);
	StartStorageSelect();
	}

void CATPhoneBookRead::StartThisCommand()
/**
 * This function starts the "AT+CPBR=minimum index, maximum index" command, i.e, 
 * Read one or a number of entries in the selected phone book. It constructs the command
 * using the starting index and the number of entries that are to be read. It
 * then sets the state machine going.
 */ 
	{
	LOGTEXT(_L8("Starting AT+CPBR=minIndex,maxIndex Command (Read one or more entries)"));
	
	// Construct the AT+CPBR=a,z command. The starting index is not necessarily 1
 	TBuf8<KGenericBufferSize> buf(KPhoneBookReadAll);
	buf.AppendNum(iRead->iIndex); // starting location in the phonebook from which to read

	if (iRead->iNumSlots>1)
		{
		// If more than one entry to be read also put finishing index
		buf.Append(KCommaChar);
		buf.AppendNum(iRead->iIndex + iRead->iNumSlots -1); // last location from which to read
		}

	iNumReadEntries=0;
	WriteExpectingResults(buf,3);
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iCPBRResponse=iIo->AddExpectString(this,KPhoneBookReadResponse);
	iState=EATPhoneBookCommandWaitForAlternativeWriteComplete;
	}

void CATPhoneBookRead::ParseResponseL()
/**
 * This function parses a line of response to the "AT+CBPR=minimum index, maximum index"
 * command and converts the phonebook data into the TLV format. Note that the minimum 
 * index of the phone book memory need not necessarily 
 * start at 1 (e.g the phonebook could use range [100 to 150].
 */
	{
	ParseBufferLC(EFalse, ':');

	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++; //Assume discarded CPBR string
	if (entry==NULL)
		User::Leave(KErrGeneral);
	TInt ret(KErrNone);
	TBuf16<KGenericBufferSize> buffer;

	// new entry so append the new entry tag first
	if(iPbBuffer->AddNewEntryTag() == KErrNone)
		{	
		// convert index into TLV format and append it to the supplied buffer
		entry=iter++;
		if(!entry)
			User::Leave(KErrGeneral);
		TInt index=CATParamListEntry::EntryValL(entry);

		if (iInfo->Completed()) 
			{
			LOGTEXT(_L8("CATPhoneBookRead::ParseResponseL Mapping phone index to client index"));
			iInfo->MapPhoneIndexToClientIndex(index);
			}

		ret = iPbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex, static_cast<TUint16>(index));

		if(ret == KErrNone) // only continue if the previous field successfully appended
			{		
			// convert number into TLV format and append it to the supplied buffer
			entry=iter++;
			if (!entry)
				User::Leave(KErrGeneral);
			buffer.Copy(entry->iResultPtr);
// If the number has a leading "+" then remove it.  Note: in this case, we assume that the TON will be international.
			if((buffer.Length()>0)&&(buffer[0]=='+'))
				buffer.Delete(0,1);

			TPtrC16 numberPtr(buffer);
			ret=iPbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, numberPtr);

			if(ret == KErrNone)	// only continue if the previous field successfully appended
				{
				// convert type into TLV format and append it to the supplied buffer
				entry=iter++;
				TInt type=CATParamListEntry::EntryValL(entry);
				ret=iPbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, static_cast<TUint8>(type));

				if(ret == KErrNone) // only continue if the previous field successfully appended
					{
					// convert text into TLV format and append it to the supplied buffer
					entry=iter++;
					if (!entry)
						User::Leave(KErrGeneral);
					buffer.Copy(entry->iResultPtr);
					TPtrC16 textPtr(buffer);
					ret=iPbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, textPtr);
					if(ret==KErrNone)
						iNumReadEntries++;
					}
				}
			}	
		} // end if AddNewEntryTag
	
	// one of the fields could not be appended so remove previous fields from the buffer
	if(ret != KErrNone)
		{
		(void)iPbBuffer->RemovePartialEntry();
		}
	CleanupStack::PopAndDestroy();		// parsed buffer
	}

void CATPhoneBookRead::EventSignal(TEventSource aEventSource)
/**
 * State machine. Replaces CATPhoneBookCommands implementation to use second command 
 * state.
 */
	{
	if ((aEventSource!=ETimeOutCompletion) && (iState==EATPhoneBookCommandWaitForAlternativeReadComplete))
		{
		__ASSERT_ALWAYS(aEventSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		// may be a entry, OK, or ERROR
		if (iIo->FoundChatString()==iCPBRResponse)
			{
			TRAPD(ret,ParseResponseL());
			if (ret!=KErrNone)
				{
				Complete(ret,EReadCompletion);
				return;
				}
			// wait for another one
			iIo->SetTimeOut(this,KPhoneBookReadTimeout*KOneSecondPause);
			} 
		else
			{
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if((ret==KErrNone)&&(iNumReadEntries==0))
				{
// No entries read, so buffer will be empty and KErrNotFound must be returned.
				ret=KErrNotFound;
				}
			Complete(ret,EReadCompletion); 
			}
		} 
	else
		CATPhoneBookCommands::EventSignal(aEventSource);
	}


void CATPhoneBookRead::CompleteWithIOError(TEventSource,TInt aError)
/**
 * This function is called if an error occurs. It cancels the relevant timer and 
 * completes the request.
 */
	{
	if (iState!=EATPhoneBookCommandIdle)
		{
		iIo->WriteAndTimerCancel(this);
		iCPBRResponse=0;
		iState=EATPhoneBookCommandIdle;
		iTelObject->ReqCompleted(iReqHandle,aError);
		}
	}


//
// CATPhoneBookWrite definitions
//

CATPhoneBookWrite* CATPhoneBookWrite::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
/**
 * Standard 2 phase constructor.
 * This method creats an instance of the CATPhoneBookWrite class. 
 */
	{
	CATPhoneBookWrite* r=new(ELeave) CATPhoneBookWrite(aIo,aTelObject,aInit,aPhoneGlobals);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CATPhoneBookWrite::CATPhoneBookWrite(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CATPhoneBookCommands(aIo,aTelObject,aInit,aPhoneGlobals)
/**
 * C++ constructor
 */
	{}

void CATPhoneBookWrite::ConstructL()
/**
 * Standard 2nd phase constructor.
 * Creates phonebook buffer object used to extract data from client buffer
 */
	{
	CATCommands::ConstructL();
	iPbBuffer = new(ELeave) CPhoneBookBuffer();
	}

CATPhoneBookWrite::~CATPhoneBookWrite()
/**
 * Destructor.
 */
	{
	delete iPbBuffer;
	}

void CATPhoneBookWrite::Start(TTsyReqHandle aTsyReqHandle,TAny* aParams)
/**
 * Start function. Its calls the StartStorageSelect() function to select 
 * the phone book memory.
 */
	{
	iReqHandle=aTsyReqHandle;
	iIndex = *(static_cast<TInt*>(aParams)); // aParams will contain an index 
	StartStorageSelect();
	}

void CATPhoneBookWrite::StartThisCommand()
/**
 * This function starts the Write command. It decodes the phonebook data supplied by the
 * client from the TLV format into the AT+CPBW= format, builds the command and 
 * starts the state machine to write the entry to the phone. It can only write 
 * one entry at a time.
 */
	{
	LOGTEXT(_L8("Starting AT+CPBW= Command"));

	TBuf8<KGenericBufferSize> atbuf;     // used for 8-bit AT command characters
	TBuf8<KGenericBufferSize> name;
	TBuf8<KGenericBufferSize> number;
	
	TBuf16<KGenericBufferSize> des16Buf;	  // used for 16-bit name field
	TPtrC16 des16Ptr(des16Buf);

	TUint8 tonNpi(0);

	TUint8 aTagValue(0);
	CPhoneBookBuffer::TPhBkTagType aDataType; 

	TInt ret=KErrNone;
	iPbBuffer->StartRead();
	while ((ret=iPbBuffer->GetTagAndType(aTagValue, aDataType))==KErrNone)
		{
		if (aTagValue==RMobilePhoneBookStore::ETagPBTonNpi)
			{
			(void)iPbBuffer->GetValue(tonNpi);			
			}
		else if (aTagValue==RMobilePhoneBookStore::ETagPBText)
			{
			(void)iPbBuffer->GetValue(des16Ptr);			

			if (des16Ptr.Length() > KGenericBufferSize)
				{
				Complete(KErrOverflow, EWriteCompletion);
				return;
				}

			name.Copy(des16Ptr);
			}
		else if (aTagValue==RMobilePhoneBookStore::ETagPBNumber)
			{
			(void)iPbBuffer->GetValue(des16Ptr);			

			if (des16Ptr.Length() > KGenericBufferSize)
				{
				Complete(KErrOverflow, EWriteCompletion);
				return;
				}

			number.Copy(des16Ptr);
			}
		else
			{
			// An unsupported field type - just skip this value
			iPbBuffer->SkipValue(aDataType);
			}
		}

	if(ret==KErrNotFound) // This is to ensure the TLV conversion worked
		{
// If the ton is international, but the number doesn't start with a '+' then prepend one.
		if((tonNpi==145)&&(number.Length()>0)&&(number[0]!='+'))
			atbuf.Format(KPhoneBookWriteWithIndexPrependedPlus,iIndex,&number,tonNpi,&name);
		else
			atbuf.Format(KPhoneBookWriteWithIndex,iIndex,&number,tonNpi,&name);
		}

	WriteExpectingResults(atbuf,3);
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iState=EATPhoneBookCommandWaitForWriteComplete;
	}


TUint CATPhoneBookWrite::NumberTypefromMMToGSM(TUint aTypeOfNumberMM)
/** Maping from MM Number Type To GSM Number Type
 * This method maps the MM way of representing a type of telefon number 
 * to the GSM standard.
 */
	{
	switch (aTypeOfNumberMM)
		{
	case 0:		// EUnknownNumber (MM)
		return 129; // Nationality unknown (GSM)
	case 1:		// EInternationalNumber (MM)
		return 145; // International Number (GSM)
	case 2:		// ENationalNumber (MM)
		return 161; // National Number (GSM)
	default:
        return 129; // Nationality unknown (GSM)
		}
	}


void CATPhoneBookWrite::ParseResponseL()
	{
	}


//
// CATPhoneBookDelete definitions
//
CATPhoneBookDelete* CATPhoneBookDelete::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
/**
 * Standard 2 phase constructor.
 * This method creats an instance of the CATPhoneBookDelete class. 
 */
	{	
	CATPhoneBookDelete* r=new(ELeave) CATPhoneBookDelete(aIo,aTelObject,aInit,aPhoneGlobals);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CATPhoneBookDelete::CATPhoneBookDelete(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CATPhoneBookCommands(aIo,aTelObject,aInit,aPhoneGlobals)
/**
 * C++ constructor
 */
	{}

CATPhoneBookDelete::~CATPhoneBookDelete()
	{}

void CATPhoneBookDelete::Start(TTsyReqHandle aTsyReqHandle,TAny* aParams)
/**
 * Start function. Its calls the StartStorageSelect() function to select 
 * the phone book memory.
 */
	{
	iReqHandle=aTsyReqHandle;
	iIndex=*(TInt*)aParams;

	StartStorageSelect();
	}

void CATPhoneBookDelete::StartThisCommand()
/**
 * Start of the Delete command. This function constructs and then sends the Delete 
 * command (the Delete command is the same as the write command with just the index to 
 * delete specified e.g "AT+CPBW=2" command will delete the entry in index location 2).
 */
	{
	LOGTEXT(_L8("Starting AT+CPBW= Command"));
	
	TBuf8<KGenericBufferSize> buf;
	
	buf.Format(KPhoneBookDelete,iIndex); 
	WriteExpectingResults(buf,3);
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iState=EATPhoneBookCommandWaitForWriteComplete;
	}

void CATPhoneBookDelete::StartAlternativeDelete()
/**
 * Alternative Delete command. The same principles as the normal delete apply.
 */
	{
	LOGTEXT(_L8("Starting alternative AT+CPBW= Command"));
	
	TBuf8<KGenericBufferSize> buf;

	buf.Format(KPhoneBookAlternativeDelete,iIndex);
	WriteExpectingResults(buf,3);
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iState=EATPhoneBookCommandWaitForAlternativeWriteComplete;
	}


void CATPhoneBookDelete::EventSignal(TEventSource aEventSource)
/** 
 * State machine for the Delete command.
 */
	{
	if (iState==EATPhoneBookCommandWaitForReadComplete &&
		aEventSource!=ETimeOutCompletion)
		{
		__ASSERT_ALWAYS(aEventSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		TInt ret(ValidateExpectString());
		RemoveStdExpectStrings();
		if (ret==KErrGeneral)
			{
			StartAlternativeDelete();
			return;
			}
		if (ret!=KErrNone)
			{
			Complete(ret,aEventSource);
			return;
			}
		iIo->SetTimeOut(this,KTimeForExtraRxData);
		iState=EATPhoneBookCommandExtendedRead;
		}
	else
		CATPhoneBookCommands::EventSignal(aEventSource);
	}

void CATPhoneBookDelete::ParseResponseL()
/**
 *
 */
	{
	}


