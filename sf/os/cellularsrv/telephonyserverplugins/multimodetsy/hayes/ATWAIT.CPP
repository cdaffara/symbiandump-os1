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
// Wait for Incoming Call
// 
//

#include "NOTIFY.H"
#include "mSLOGGER.H"
#include "LINE.H"
#include "CALL.H"
#include "ATCALL.H"
#include "ATIO.H"
#include "PHONE.H"

_LIT8(KIncomingExtCallIndication,"+CRING:*");
_LIT8(KIncomingCallIndication,"RING");
_LIT8(KNetworkRegistration,"+CREG:*");

//
// CTimer-derived class to switch line status to idle a fixed period after the last 
// RING arrives. Use this rather than CATIO::SetTimeOut() because that has only one 
// timer which could be cancelled by another concurrently running AT command
//
CSetLineToIdle* CSetLineToIdle::NewL(CATWaitForCall* aWait)
//
//	Create the SetLineToIdle async one shot
//
	{
	CSetLineToIdle* setLineToIdle=new(ELeave) CSetLineToIdle(aWait);
	CleanupStack::PushL(setLineToIdle);
	setLineToIdle->ConstructL();
	CleanupStack::Pop();
	return setLineToIdle;
	}

CSetLineToIdle::CSetLineToIdle(CATWaitForCall* aWait)
//
// C'tor
//
	:CTimer(CActive::EPriorityLow), iWait(aWait)
	{
	CActiveScheduler::Add(this);
	}


CSetLineToIdle::~CSetLineToIdle()
	{
	Cancel();
	}

void CSetLineToIdle::RunL()
	{
	iWait->EndRing();
	}

void CSetLineToIdle::Start()
	{
	if (IsActive())
		Cancel();
	After(KTimeBetweenRings);	
	}

void CSetLineToIdle::Stop()
	{
	Cancel();
	}

//
//  CATWaitForCall. Expects a RING constantly, and upon receiving one checks if either line 
//	wants to be notified. Sets all lines and calls ringing anyway, and a timer to switch
//	them back to idle after a specified time has elapsed since the last RING.
//

CATWaitForCall* CATWaitForCall::NewL(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals)
	{
	CATWaitForCall* waitForCall=new(ELeave) CATWaitForCall(aIo,aTelObject,aPhoneGlobals);
	CleanupStack::PushL(waitForCall);
	waitForCall->ConstructL();
	CleanupStack::Pop();
	return waitForCall;
	}

CATWaitForCall::CATWaitForCall(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals) 
								: CATBase(aIo,aTelObject,aPhoneGlobals)
	{}

void CATWaitForCall::ConstructL()
	{
	iSetLineToIdle = CSetLineToIdle::NewL(this);
	iRingExpectString=NULL;
	iCRingExpectString=NULL;
	iRegExpectString=NULL;
	iTailExpectString=NULL;
	}

CATWaitForCall::~CATWaitForCall()
	{
	iIo->RemoveExpectStrings(this);
	iRingExpectString=NULL;
	iCRingExpectString=NULL;
	iRegExpectString=NULL;
	iTailExpectString=NULL;
	iIo->WriteAndTimerCancel(this);	
	delete iSetLineToIdle;
	}

void CATWaitForCall::StartWait()
	{
	if (iRingExpectString==NULL)
		{
		LOGTEXT(_L8("WaitForCall:\tStarting wait for incoming call"));
		iCRingExpectString=iIo->AddExpectString(this,KIncomingExtCallIndication);
		iRingExpectString=iIo->AddExpectString(this,KIncomingCallIndication);
		iRegExpectString=iIo->AddExpectString(this,KNetworkRegistration);
		iState=EATWaitForUnsolicited;
		}
	}

void CATWaitForCall::EndRing()
//
//	Only called from the RunL of CSetLineToIdle, which occurs a fixed period after the
//	last RING. StopBothLinesRinging() sets the calls on both lines to idle
//
	{
	LOGTEXT(_L8("WaitForCall:\tRinging has stopped"));
	ChangeLineStatus(RCall::EStatusIdle);
	STATIC_CAST(CPhoneHayes*,iTelObject)->StopRinging(); 
	iRingCounter = 0;
	iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject,ERingStopped);
	}

void CATWaitForCall::ResetRingCounter()
	{
	iRingCounter = 0;
	iSetLineToIdle->Stop();
	}

void CATWaitForCall::CompleteWithIOError(TEventSource /*aSource*/,TInt /*aStatus*/)
//
//	CATIO removes expect strings in event of IO error
//
	{
	iRingExpectString=NULL;
	iCRingExpectString=NULL;
	iRegExpectString=NULL;
	iTailExpectString=NULL;
	}

void CATWaitForCall::EventSignal(TEventSource aSource)
//
//	Completes Incoming Call Notification for every ring.
//	This is different from the previous TSY, but is needed for the Nokia 7110 which does
//  not supply multiple RING notifications.
//
	{
	__ASSERT_ALWAYS(aSource!=EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteNotExpected));
	if (aSource!=EReadCompletion)
		return;
	switch (iState)
		{
	case EATWaitForUnsolicited:
		if (iIo->FoundChatString()==iCRingExpectString)
			{
			LOGTEXT(_L8("CATWaitForCall::EventSignal +CRING detected"));
			LOGTEXT(_L8("CATWaitForCall::EventSignal Will only process +CRING if phone is initialised..."));
			if (iPhoneGlobals->iPhoneStatus.iInitStatus==EPhoneInitialised)
				{
				LOGTEXT(_L8("CATWaitForCall::EventSignal Processing +CRING"));
				TInt index=KErrNotFound;
				TRAPD(ret,index=ParseIncomingCallIndicationL());
				if (ret==KErrNone)
					{
					iRingCounter+=1;
					if (iRingCounter==1)
						{
						LOGTEXT(_L8("CATWaitForCall::EventSignal This is 1st +CRING"));
						// Only do this once
						ChangeLineStatus(RCall::EStatusRinging);
						STATIC_CAST(CPhoneHayes*,iTelObject)->SetCallRinging(index); 
						}
					iSetLineToIdle->Start();
					}
				}
			}
		else if (iIo->FoundChatString()==iRingExpectString)
			{
			LOGTEXT(_L8("WaitForCall:\tRING detected"));
			if (iPhoneGlobals->iPhoneStatus.iInitStatus==EPhoneInitialised)
				{
				iRingCounter+=1;
				if (iRingCounter==1)
					{
					// Only do this once
					ChangeLineStatus(RCall::EStatusRinging);
					STATIC_CAST(CPhoneHayes*,iTelObject)->SetAmbiguousDataFaxCallRinging(); 
					}
				iSetLineToIdle->Start();
				}
			}
		else if (iIo->FoundChatString()==iRegExpectString)
			{
			LOGTEXT(_L8("WaitForCall:\t+CREG detected"));
			TRAPD(ret,ParseNetworkRegistrationL());
			if (ret!=KErrNone)
				{
				LOGTEXT(_L8("WaitForCall:\tBad +CREG response?"));
				}
			}
		else
			{
			LOGTEXT(_L8("WaitForCall:\tUnexpected string"));
			iTelObject->ReqCompleted(iReqHandle,KErrGeneral);
			iReqHandle = NULL;
			break;
			}
		break;

	default:
		break;
		}
	}

// Compare with +CMTI handling in GSMSNOTI.CPP
// which has a similar problem
/**
*	Parse incoming call indication string and return a value corresponding to the call type.
*
* @note		modified by Dmitry Lyokhin 24.04.02
* 
* @return	KFaxLineIndex	for fax call
*       	KVoiceLineIndex	for voice call
*       	KDataLineIndex	for data call or if call modifier is not recognized
*
* @note		This function can leave with code KErrUnknown 
*			if 'RING' token not found 
*/
TInt CATWaitForCall::ParseIncomingCallIndicationL()
	{
	//-- possible incoming call indication strings that can be parsed correctly
	//-- string					corresponds to

	//	+CRING: VOICE		   -> voice call
	//	+CRING: FAX			   -> fax call
	//	+CRING: DATA		   -> data call
	//	+CRING: VOICE/xxx	   -> voice call
	//	 RING				   -> data call	

	//-- alternating mode calls
	//	+CRING: ALT FAX/VOICE  -> fax call
	//	+CRING: ALT DATA/VOICE -> data call
	//	+CRING: ALT VOICE/FAX  -> fax call
	//	+CRING: ALT VOICE/DATA -> data call


	iBuffer.Set(iIo->CurrentLine());

	TInt	index = KErrNotFound;
	TLex8	Lex1(iBuffer);
	
	Lex1.SkipSpace();
	
	//-- the first token must be '+CRING' or 'RING'
	if( Lex1.NextToken().Find(_L8("RING")) < 0 )
	{	//-- the token not found, leave
		LOGTEXT(_L8("CATWaitForCall::ParseIncomingCallIndicationL *RING not found"));		
		User::Leave(KErrUnknown); 
    }
	
	Lex1.SkipSpaceAndMark();
	Lex1.SkipCharacters();

	//-- try to find 'ALT' token that indicates the alternatng mode call
	if( Lex1.MarkedToken().Find(_L8("ALT")) >=0 ) 
	{//-- Alternating mode call, skip the token "ALT"
	}
	else  Lex1.UnGetToMark();

	Lex1.SkipSpace();

	TPtrC8 CallStr=Lex1.RemainderFromMark();
	
	_LIT8(KDataToken,	"DATA");
	_LIT8(KFaxToken,	"FAX");
	_LIT8(KVoiceToken,	"VOICE");

	   if(CallStr.Find(KDataToken) >= 0)		index=KDataLineIndex;	//-- Data call detected
	   else
		 if(CallStr.Find(KFaxToken) >= 0)		index=KFaxLineIndex;	//-- Fax call detected
		 else
		   if(CallStr.Find(KVoiceToken) >= 0)	index=KVoiceLineIndex;	//-- Voice call detected
			else 
			{//-- unmatched call type, assume data call by default
				LOGTEXT(_L8("CATWaitForCall::ParseIncomingCallIndicationL +CRING has unmatched type!, assuming data"));
				index=KDataLineIndex;	
				//User::Leave(KErrUnknown); 		
			}

	iIo->ClearCurrentLine();
	LOGTEXT2(_L8("+CRING for line %d"),index);
	
	return index;
	}


void CATWaitForCall::ParseNetworkRegistrationL()
	{
	// +CREG:    <n>, <stat> [, <lac>, <ci>  ] 		-- solicited
	// +CREG:         <stat> [, <lac>, <ci>  ]		-- unsolicited
	//        number, number [, "hex", "hex" ]

	RMobilePhone::TMobilePhoneLocationAreaV1& locationInfo = iPhoneGlobals->iPhoneStatus.iLocationArea;
	locationInfo.iAreaKnown= EFalse;
	locationInfo.iLocationAreaCode=0;
	locationInfo.iCellId=0;

	ParseLineLC();
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);

	entry=iter++;
	if (entry==NULL)
		User::Leave(KErrGeneral);
	// should be +CREG:

	entry=iter++;
	if (entry==NULL)
		User::Leave(KErrGeneral);
	TLex8 lex(entry->iResultPtr);
	TInt number;
	(void)User::LeaveIfError(lex.Val(number));

	entry=iter++;
	if (entry!=NULL)
		{ // is this <status> in a solicited +CREG?
		lex.Assign(entry->iResultPtr);
		if (lex.Val(number)==KErrNone)
			entry=iter++; // yes - <n> has been replaced by <status>
		}

	// optional <lac> and <ci>
	if (entry != NULL)
		{
		lex.Assign(entry->iResultPtr);
		(void)User::LeaveIfError(lex.Val(locationInfo.iLocationAreaCode,EHex));
		locationInfo.iAreaKnown = ETrue;
		entry=iter++;
		}
	if (entry!=NULL)
		{
		lex.Assign(entry->iResultPtr);
		(void)User::LeaveIfError(lex.Val(locationInfo.iCellId,EHex));
		locationInfo.iAreaKnown = ETrue;
		}
   
	

	if (number<0 || number > RMobilePhone::ERegisteredRoaming)
		User::Leave(KErrGeneral);

	RMobilePhone::TMobilePhoneRegistrationStatus regstatus;
	regstatus = MappingRegistrationStatusFromETSIToMM(&number); //Mapp the ETSI value number to MM enum for network registration status.

	RMobilePhone::TMobilePhoneRegistrationStatus newstatus = regstatus;// change number to be of type MobilePhone...
	RMobilePhone::TMobilePhoneRegistrationStatus oldstatus = iPhoneGlobals->iPhoneStatus.iRegistrationStatus;

	if (oldstatus != newstatus)
		{
		LOGTEXT3(_L8("MmTsy:CATWaitForCall:\tRegistrationStatus changed %d->%d"), oldstatus, newstatus);
		iPhoneGlobals->iPhoneStatus.iRegistrationStatus = newstatus;

		iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject,ERegistrationStatusChanged);

		if (   (newstatus == RMobilePhone::ERegisteredOnHomeNetwork || newstatus == RMobilePhone::ERegisteredRoaming)
			|| (oldstatus == RMobilePhone::ERegisteredOnHomeNetwork || oldstatus == RMobilePhone::ERegisteredRoaming)
			)
			{
			// interesting transition - need new operator details
			LOGTEXT(_L8("MmTsy:CATWaitForCall:\tCurrent Network has changed"));
			iPhoneGlobals->iPhoneStatus.iNetworkChanged=ETrue;
			if (iPhoneGlobals->iPhoneStatus.iInitStatus == EPhoneInitialised &&
				!iPhoneGlobals->iEventSignalActive)
				{
				// no current activity - fire up +COPS stuff immediately
				LOGTEXT(_L8("MmTsy:CATWaitForCall:\tNo activity - Checking current network immediately"));
				iPhoneGlobals->CheckForChangeOfNetwork();
				}
			}
		}

	CleanupStack::PopAndDestroy();
	}

RMobilePhone::TMobilePhoneRegistrationStatus CATWaitForCall::MappingRegistrationStatusFromETSIToMM(TInt *aNumber) 
	// This function is mapping the ETSI standard format for the registartion status of the phone to multimode representation.
	// TINT *number is the ETSI value for the registration value to be converted.

	{ 
	RMobilePhone::TMobilePhoneRegistrationStatus regstatus = RMobilePhone::ERegistrationUnknown;
	switch (*aNumber)
		{
		case 0:
			regstatus = RMobilePhone::ENotRegisteredNoService;
			break;
		case 1:
			regstatus = RMobilePhone::ERegisteredOnHomeNetwork;
			break;
		case 2:
			regstatus = RMobilePhone::ENotRegisteredSearching;
			break;
		case 3:
			regstatus = RMobilePhone::ERegistrationDenied;
			break;
		case 4:
			regstatus = RMobilePhone::ERegistrationUnknown;
			break;
		case 5:
			regstatus = RMobilePhone::ERegisteredRoaming;
			break;
		default:
			break;
		}
	return regstatus;
	}
