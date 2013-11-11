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
// AT Command Base Class
// 
//

#include "PHONE.H"
#include "ATIO.H"
#include "mSLOGGER.H"
#include "ATINIT.H"
#include "ATESCAPE.H"
#include "LINE.H"
#include "NOTIFY.H"
#include "sms_rx_queue.h"			// for CReceiveSmsQueue

// Specific GSM Error String Response
_LIT8(KCmsErrorString,"+CMS ERROR:*");

//
// CATParamListEntry
// A pointer to a meaningful result in the received buffer from the modem
//
CATParamListEntry::CATParamListEntry(const TDesC8& aDes) :
	iResultPtr(aDes)
	{
	}

CATParamListEntry::~CATParamListEntry()
	{}

void CATParamListEntry::Deque()
	{
	iLink.Deque();
	}

void CATParamListEntry::EntryValL(CATParamListEntry* aEntry,TInt& aValue)
	{
	if (aEntry==NULL)
		User::Leave(KErrGeneral);
	TLex8 lex(aEntry->iResultPtr);
	(void)User::LeaveIfError(lex.Val(aValue));
	aEntry->Deque();
	delete aEntry;
	}

TInt CATParamListEntry::EntryValL(CATParamListEntry* aEntry)
	{
	TInt val;
	EntryValL(aEntry,val);
	return val;
	}


//
// Async One Shot to call RelinquishOwnershipComplete(). 
// If RelinquishOwnershipComplete() is called synchronously, it destroys the CATIO object
// before the latter has finished processing.
//
CCompleteRelinquish* CCompleteRelinquish::New(CTelObject* aTelObject)
//
//	Create the Relinquish Complete async one shot
//
	{
	// The below TRAP is used to stop leavescan errors occurring.
	// This is not the best solution, but I am pressed for time ;-(
	CCompleteRelinquish* ptr=NULL;
	TRAP_IGNORE(ptr=new(ELeave)CCompleteRelinquish(aTelObject));
	return ptr;
	}

CCompleteRelinquish::CCompleteRelinquish(CTelObject* aTelObject)
//
// C'tor
//
	:CAsyncOneShot(CActive::EPriorityLow), iTelObject(aTelObject)
	{
	__DECLARE_NAME(_S("CCompleteRelinquish"));
	}


CCompleteRelinquish::~CCompleteRelinquish()
	{
	Cancel();
	}

void CCompleteRelinquish::RunL()
//
// Call RelinquishOwnershipCompleted() or RecoverDataPortAndRelinquishOwnershipCompleted()
// after CATIO has finished its processing
//
	{
	__ASSERT_ALWAYS(iPanicOccurred!=ENoPanicOccurred,Panic(EIllegalPanicOccurredValue));
	if (iPanicOccurred == EPanicOccurredWithoutDataPortLoan)
		{
		REINTERPRET_CAST(CCallBase*,iTelObject)->RelinquishOwnershipCompleted(KErrNone);
		}
	if (iPanicOccurred == EPanicOccurredDuringDataPortLoan)
		{
		REINTERPRET_CAST(CCallBase*,iTelObject)->RecoverDataPortAndRelinquishOwnershipCompleted(KErrNone);
		}
	delete this;
	}
//
// The Command Base Class
//
CATBase::CATBase(CATIO* aIo, CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals)
	: iIo(aIo),iPhoneGlobals(aPhoneGlobals),iComplete(NULL),iTelObject(aTelObject),iCallInfo(NULL)
	{
	iRxResults.SetOffset(_FOFF(CATParamListEntry,iLink));
	}

CATBase::~CATBase()
//
//	Assumes CATIO pointer is still valid
//
	{
	if(!iRxResults.IsEmpty())
		{
		CATParamListEntry* entry;
		TDblQueIter<CATParamListEntry> iter(iRxResults);
		while (entry = iter++,entry!=NULL)
			{
			entry->Deque();
			delete entry;
			}
		}
	}

void CATBase::GenericEventSignal(TEventSource aEventSource, TInt aStatus)
//
//	If an IO error has occurred, complete this ATBase-object with the error. 
//
	{		
	if (aStatus!=KErrNone)
		{
		LOGTEXT2(_L8("EventSignal received error status %d"),aStatus);
		CompleteWithIOError(aEventSource,aStatus);
		}
	else
		{
		EventSignal(aEventSource);
		}
	}

void CATBase::AddStdExpectStrings()
	{
	if (!iOKExpectString)
		iOKExpectString=iIo->AddExpectString(this,KOkString);
	if (!iErrorExpectString)
		iErrorExpectString=iIo->AddExpectString(this,KErrorString);
	}


TInt CATBase::ValidateExpectString()
/**
 * New version ValidateExpectStringL which returns an error code
 * as opposed to leaving. 
 * Use of this new code should hopefully remove alot of 
 * unesseccary TRAP harness in the TSY.
 */
 	{
	TInt ret(KErrNone);

	if(iIo->FoundChatString()!=iOKExpectString)
		{
		if(iIo->FoundChatString()==iErrorExpectString)
			{
			LOGTEXT(_L8("Modem returned ERROR in response to command"));
			ret=KErrGeneral;
			}
		else
			{
			LOGTEXT(_L8("Modem returned unexpected response to command"));
			ret=KErrUnknown;
			}
		}

	return ret;
	}

void CATBase::RemoveStdExpectStrings()
	{
	LOGTEXT(_L8("RemoveStdExpectStrings()"));
	iIo->RemoveExpectString(iOKExpectString);
	iOKExpectString=NULL;
	iIo->RemoveExpectString(iErrorExpectString);
	iErrorExpectString=NULL;
	}

void CATBase::RemoveUnsolicitedStrings()
	{
	LOGTEXT(_L8("RemoveUnsolicitedStrings()"));

	_LIT8(KNetworkRegistration,"+CREG:");						// Network registration
	_LIT8(KIncomingCallIndication,"RING");						// Normal Ring
	_LIT8(KIncomingExtCallIndication,"+CRING:");				// Extended format call
		_LIT8(KIncomingExtNTCallParameter,"REL");				// Non-Transparent parameter
		_LIT8(KIncomingExtAlternatingCallParameter,"ALT");		// Unsupported Alternating parameter
		_LIT8(KIncomingExtVoiceRelCallParameter,"VOICE/REL");	// NT Voice parameter

	if(!iRxResults.IsEmpty())
		{
		TInt aQueueSteps = NULL;
		CATParamListEntry* entry;
		TDblQueIter<CATParamListEntry> iter(iRxResults);
		// Step along the queue looking for an unsolicited strings
		while (entry = iter++,entry!=NULL)	
			{
			TPtrC8 aResult(entry->iResultPtr);
			// If a match is found, we dequeue it and all its expected parameters
			if (aResult==KIncomingCallIndication) // 0 Params
				{
				// Remove the "RING"
				entry->Deque(); delete entry;
				LOGTEXT2(_L8("Unsolicited >%S< Removed"),&aResult);
				}
	        else if (aResult==KIncomingExtCallIndication) // 1/2 Params 			    
				{
				// Remove the "+CRING:"
				entry->Deque(); delete entry; entry = iter++;
				LOGTEXT2(_L8("Unsolicited >%S< Removed"),&aResult);
				// We know we should be getting at least one and maybee two parameters
				TPtrC8 aFirstParameterResult(entry->iResultPtr);
				if (aFirstParameterResult==KIncomingExtNTCallParameter
				 || aFirstParameterResult==KIncomingExtVoiceRelCallParameter
				 || aFirstParameterResult==KIncomingExtAlternatingCallParameter)
					{
					// We have an "ALT" or a "REL" so we have two parameters
					entry->Deque(); delete entry; entry = iter++;
					TPtrC8 aSecondParameterResult(entry->iResultPtr);
					entry->Deque(); delete entry;	// If recognised parameter - remove
					LOGTEXT3(_L8("Unsolicited Parameter >%S %S< Removed"),&aFirstParameterResult,&aSecondParameterResult); aSecondParameterResult==KIncomingCallIndication; // Quick fix to remove unused parameters make warning
					}
				else
					{
					// We only have one parameter
					entry->Deque(); delete entry;	// If recognised parameter - remove
					LOGTEXT2(_L8("Unsolicited Parameter >%S< Removed"),&aFirstParameterResult);
					}
				}
			else if (aResult==KNetworkRegistration) // 2 Params
				{
				// Remove the "+CREG:"
				entry->Deque(); delete entry; entry = iter++;
				LOGTEXT2(_L8("Unsolicited >%S< Removed"),&aResult);
				// Remove both parameters
				TPtrC8 aFirstParameterResult(entry->iResultPtr);
				entry->Deque(); delete entry; entry = iter++;
				TPtrC8 aSecondParameterResult(entry->iResultPtr);
				entry->Deque(); delete entry;
				LOGTEXT3(_L8("Unsolicited Parameters >%S,%S< Removed"),&aFirstParameterResult,&aSecondParameterResult); aFirstParameterResult==KIncomingCallIndication; aSecondParameterResult==KIncomingCallIndication; // Quick fix to remove unused parameters make warning
				}

			// Increment the queue step counter 
			else aQueueSteps++;
			}
		// Now we have to retrace the list, ignoring dequeued entries
		for (TInt aLoop=NULL;aLoop<aQueueSteps;aLoop++) 
			iter--;
		}
	}

void CATBase::CleanupRxResults(TAny *aCATBase)	// for TCleanupOperation
	{
	CATBase* p= reinterpret_cast<CATBase*>(aCATBase);
	TDblQueIter<CATParamListEntry> iter(p->iRxResults);
	CATParamListEntry* entry=iter++;
	while (entry!=NULL)
		{
		entry->Deque();
		delete entry;
		entry=iter++;
		}
	}

void CATBase::RxResultsPushLC()
	{
	TCleanupItem cleanup(CleanupRxResults,this);
	CleanupStack::PushL(cleanup);
	}

void CATBase::ParseBufferLC(TBool aReportLists, TUint aSeparatorChar)
//
//  Parses buffer.  Treats aSeparatorChar like a comma
//
	{
	LOGTEXT(_L8("Parse the Buffer List"));

	if (iIo->CurrentLine()==KOkString)
		iIo->ClearCurrentLine();	// remove trailing "OK"

	iBuffer.Set(iIo->Buffer());
	iIo->ClearBuffer();

	ParseLC(aReportLists, aSeparatorChar);
	}

void CATBase::ParseLineLC(TBool aReportLists, TUint aSeparatorChar)
//
//  Parses current line.  Treats aSeparatorChar like a comma
//
	{
	LOGTEXT(_L8("Parse the current line"));

	iBuffer.Set(iIo->CurrentLine());
	iIo->ClearCurrentLine();
	ParseLC(aReportLists, aSeparatorChar);
	}

void CATBase::AddParamL(const TDesC8 &aPtr)
	{
	CATParamListEntry* aParamListEntry = new (ELeave) CATParamListEntry(aPtr);
	iRxResults.AddLast(*aParamListEntry);
	}

void CATBase::ParseLC(TBool aReportLists, TUint aSeparatorChar)
// 
// Does the actual parsing of iBuffer, set up from ParseLineLC or ParseBufferLC
//
	{
_LIT8(KOpenBracket,  "(");
_LIT8(KCloseBracket, ")");

#ifdef __LOGDEB__
	TPtrC8 buf(iBuffer);
	if (buf.Length()>180) 
		{
		buf.Set(buf.Left(180));
		LOGTEXT2(_L8("buffer to parse >%S< and so on ..."),&buf);
		}
	else
		LOGTEXT2(_L8("buffer to parse >%S<"),&buf);
#endif

	RxResultsPushLC();
	TLex8 yyLex(iBuffer);

	//	Move cursor past any spaces or open brackets
	yyLex.SkipSpace();
	TChar peek=yyLex.Peek();
	if (peek=='(' || peek=='[' || peek=='{')
		yyLex.Inc();

	if (aReportLists && peek=='(')
		{
		AddParamL(KOpenBracket);
		LOGTEXT2(_L8("list separator >%S<"),&iRxResults.Last()->iResultPtr);
		}
	do
		{
		// Skip all space and opening brackets
		for (;;)
			{
			yyLex.SkipSpace();
			if (yyLex.Eos())
				return;
			peek = yyLex.Peek();
			if (peek!='(')
				break;
			if (aReportLists)
				{
				AddParamL(KOpenBracket);
				LOGTEXT2(_L8("list separator >%S<"),&iRxResults.Last()->iResultPtr);
				}
			yyLex.Inc();
			}

		if (peek=='"')		// start of quoted string
			{
			yyLex.Inc();	// step over the quote
			yyLex.Mark();
			while (!yyLex.Eos())
				{
				peek=yyLex.Peek();
				if (peek=='"')
					break;
				yyLex.Inc();
				}

			AddParamL(yyLex.MarkedToken());
			LOGTEXT2(_L8("quoted parameter >%S<"),&iRxResults.Last()->iResultPtr);
			if (yyLex.Eos())
				return;

			// Skip all space and closing brackets
			for (;;)
				{
				yyLex.Inc();
				yyLex.SkipSpace();
				if (yyLex.Eos())
					return;
				peek = yyLex.Peek();
				if (peek!=')')
					break;
				if (aReportLists)
					{
					AddParamL(KCloseBracket);
					LOGTEXT2(_L8("list separator >%S<"),&iRxResults.Last()->iResultPtr);
					}
				}

			// Skip any following separator
			if (peek==',' || peek==aSeparatorChar)
				{
				yyLex.Inc();
				yyLex.SkipSpace();
				if (yyLex.Eos())
					return;
				peek=yyLex.Peek();
				}
			}
		else // if (peek!=',' && peek !='(' && peek!='"' && peek!=aSeparatorChar)		
			{
			yyLex.Mark();
			while (peek!=',' && !peek.IsSpace() && peek!=')' && peek!=']' && peek!='}' && peek!=aSeparatorChar)
				{
				yyLex.Inc();
				if (yyLex.Eos())
					break;
				peek=yyLex.Peek();
				if ((peek==':')&&(aSeparatorChar!=':'))
					{
					yyLex.Inc();
					break;
					}
				}
			
			AddParamL(yyLex.MarkedToken());
			LOGTEXT2(_L8("normal parameter >%S<"),&iRxResults.Last()->iResultPtr);
			if (yyLex.Eos())
				return;
			// skip any whitespace and closing brackets
			for (;;)
				{
				yyLex.SkipSpace();
				if (yyLex.Eos())
					return;
				peek = yyLex.Peek();
				if (peek!=')')
					break;
				if (aReportLists)
					{
					AddParamL(KCloseBracket);
					LOGTEXT2(_L8("list separator >%S<"),&iRxResults.Last()->iResultPtr);
					}
				yyLex.Inc();
				}
			// Skip any following separator
			if (peek==',' || peek==aSeparatorChar)
				{
				yyLex.Inc();
				yyLex.SkipSpace();
				if (yyLex.Eos())
					return;
				peek=yyLex.Peek();
				}
			}
		} while (peek!=')'&& peek!=']'&& peek!='}');
	if (aReportLists && peek==')')
		{
		AddParamL(KCloseBracket);
		LOGTEXT2(_L8("list separator >%S<"),&iRxResults.Last()->iResultPtr);
		}

	}

void CATBase::ChangeCallStatus(RMobileCall::TMobileCallStatus aCallStatus)
//
//	This is called from a phone-based command as it is overloaded in CATCallAlterCommands.
//
	{
	if (iCallInfo->iMobileStatus != aCallStatus)
		{
		iCallInfo->iMobileStatus = aCallStatus;
		if (aCallStatus == RMobileCall::EStatusIdle)
			{
			iCallInfo->iHookStatus = RCall::EHookStatusOn;
			iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject,EBecomeIdle);
			}
		else if (aCallStatus != RMobileCall::EStatusUnknown && aCallStatus != RMobileCall::EStatusRinging)
			{
			iCallInfo->iHookStatus = RCall::EHookStatusOff;
			}
		}
	}

void CATBase::ChangeLineStatus(RCall::TStatus aLineStatus)
	{
	iPhoneGlobals->iPhoneStatus.iLineStatus = aLineStatus;
	}

void CATBase::SetToNotInitialised()
	{
	iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeUnknown;
	iPhoneGlobals->iPhoneStatus.iDataAndFaxFlags = RPhone::KCapsUnknown;
	iPhoneGlobals->iPhoneStatus.iInitStatus = EPhoneNotInitialised;
	}

void CATBase::StandardWriteCompletionHandler(TEventSource aSource,TInt aTimeOut)
	{
	__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
	iIo->SetTimeOut(this,aTimeOut * KOneSecondPause);
	AddStdExpectStrings();
	}

void CATBase::Write(const TDesC8& aCommand,TInt aTimeOut)
	{
	iTxBuffer.Format(KStringFormatString,&aCommand);
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,aTimeOut * KOneSecondPause);
	}

void CATBase::Write(const TDesC8& aCommand,TInt aTimeOut,TInt aValue)
	{
	iTxBuffer.Format(KStringAndIntegerFormatString,&aCommand,aValue);
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,aTimeOut * KOneSecondPause);
	}

void CATBase::Write(const TInt aTimeOut)
{
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,aTimeOut * KOneSecondPause);
}

void CATBase::WriteExpectingResults(const TDesC8& aCommand,TInt aTimeOut)
	{						   
	iIo->ClearBuffer();
	Write(aCommand,aTimeOut);
	}

void CATBase::AppendWildCardChar(TDes8& aString)
//
// A utility function to append a '*' to aString if there's not one already there
//
	{
	_LIT8(KAsterisk,"*");
	if(aString.Right(1)!=KAsterisk)
		aString.Append(KAsterisk);
	}

void CATBase::AddCmsErrorExpectString()
//
// Add the "+CMS ERROR:" expect string
//
	{
	iCmsExpectString=iIo->AddExpectString(this,KCmsErrorString);
	LOGTEXT(_L8("Added \"+CMS ERROR:\" string."));
	}

void CATBase::RemoveCmsErrorExpectString()
//
// Remove the "+CMS ERROR:" expect string
//
	{
	iIo->RemoveExpectString(iCmsExpectString);
	iCmsExpectString=NULL;
	LOGTEXT(_L8("Removed \"+CMS ERROR:\" string."));
	}

CTelObject* CATBase::Owner()
	{
	return iTelObject;
	}

TCallInfoTSY* CATBase::CallInfo()
	{
	return iCallInfo;
	}

//
// CATCommands class
//
CATCommands::CATCommands(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
										: CATBase(aIo,aTelObject,aPhoneGlobals), iInit(aInit)
	{}

void CATCommands::ConstructL()
	{
	iATSetToOnlineCommandMode=CATSetToOnlineCommandMode::NewL(iIo,iTelObject,iPhoneGlobals);
	}

CATCommands::~CATCommands()
	{
	delete iATSetToOnlineCommandMode;
	}

void CATCommands::ExecuteCommand(TTsyReqHandle aTsyReqHandle, TAny* aParams)
//
//	Ensures that phone is initialised and not in on-line data mode
//
	{
	if (iPhoneGlobals->iPhoneStatus.iInitStatus==EPhoneNotInitialised)
		{
		iInit->StartInit(this,aTsyReqHandle,aParams);
		}
	else if (iPhoneGlobals->iPhoneStatus.iMode==RPhone::EModeOnlineData)
		{
		iATSetToOnlineCommandMode->StartEscapeSequence(this,aTsyReqHandle,aParams);
		}
	else
		{
		if (iPhoneGlobals->iEventSignalActive)
			{
			// Make sure request only completed if we have a valid request handle
			// Signal that request could not be executed
			if (aTsyReqHandle)
				iTelObject->ReqCompleted(aTsyReqHandle, KErrInUse);
			}
		else
			{
			iPhoneGlobals->iEventSignalActive = ETrue;
			Start(aTsyReqHandle, aParams);
			}
		}	
	}

void CATCommands::ExecuteCommand(TTsyReqHandle aTsyReqHandle, TAny* aParams,TCallInfoTSY* aCallInfo)
//
//	Overloaded function for CCallHayes-originated commands, so that the status can be
//	changed	for CCallHayes->iCallInfo 
//
	{
	iCallInfo = aCallInfo;
	ExecuteCommand(aTsyReqHandle,aParams);
	}

void CATCommands::CancelCommand(TTsyReqHandle aTsyReqHandle)
	{
	if(iInit->CheckActiveReqHandle(aTsyReqHandle))
		{
		iInit->StopInit(aTsyReqHandle);
		}
	else if (iPhoneGlobals->iPhoneStatus.iMode==RPhone::EModeOnlineData)
		{
		iATSetToOnlineCommandMode->StopEscapeSequence(aTsyReqHandle);
		}
	else
		{
		Stop(aTsyReqHandle);
		}
	}

void CATCommands::Complete(TInt aError,TEventSource /*aSource*/)
//
// Should be called by all Complete()'s before ReqCompleted()
//
	{
	LOGTEXT(_L8("CATCommands::Complete called"));

 	if (aError==KErrTimedOut)
		{
		LOGTEXT(_L8("CATCommands::Complete KErrTimedOut error, setting EPhoneNotIntialised"));
	 	iPhoneGlobals->iPhoneStatus.iInitStatus = EPhoneNotInitialised;
		}

	// Clear the flow control flag to show no AT commands are writing to the 
	// serial port.
	iPhoneGlobals->iEventSignalActive = EFalse;

	// Allow the CReceiveSmsQueue object to read PDUs from the phones memory, if needed
	if(iPhoneGlobals->iReceiveSmsQueuePtr)
		iPhoneGlobals->iReceiveSmsQueuePtr->ReadPDUFromPhone();

	// Check the flow control flag, as the previous lines may have started off a
	// AT command. 
	// If the flow control is clear then allow the Check ForChangeOfNetwork to have
	// a go at updating its status and writing to the serial port.
	if (!(iPhoneGlobals->iEventSignalActive))
		iPhoneGlobals->CheckForChangeOfNetwork();
	}
