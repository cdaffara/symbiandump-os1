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
//

#include <commsdattypesv1_1.h>
#include <cdblen.h>
#include "PHONE.H"
#include "ATCALL.H"
#include "ATINIT.H"
#include "ATESCAPE.H"
#include "mSLOGGER.H"
#include "CALL.H"
#include "NOTIFY.H"
#include "ATNOCARR.H"
#include "ATIO.H"

#include "Matstd.h"

_LIT8(KCallInitCommand,"AT%S%S%S%S%d\r");
_LIT8(KTrasmitResponse,"Transmit:");
_LIT8(KReceiveResponse,"Receive:");

// 
//	CATCallAlterCommands - generic functionality for Dial, Answer, Connect and Hang Up
//

CATCallAlterCommands::CATCallAlterCommands(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
										: CATCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}

void CATCallAlterCommands::ConstructL()
	{
	CATCommands::ConstructL();
	} 

CATCallAlterCommands::~CATCallAlterCommands()
	{}

void CATCallAlterCommands::Start(TTsyReqHandle /*aTsyReqHandle*/, TAny* /*aParams*/)
	{}

void CATCallAlterCommands::CancelCommand(TTsyReqHandle aTsyReqHandle)
	{
	if (iPhoneGlobals->iPhoneStatus.iInitStatus==EPhoneInitialising)
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

TInt CATCallAlterCommands::ChangeCallStatus(RMobileCall::TMobileCallStatus aCallStatus)
//
//	All instances of classes derived from CATCallAlterCommands must be owned by CCallHayes
//
	{
	return REINTERPRET_CAST(CCallHayes*,iTelObject)->ChangeCallStatus(aCallStatus);
	}

void CATCallAlterCommands::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
//
//	Completes the dial/answer/connect/hangup request with an error
//
	{
	iIo->WriteAndTimerCancel(this);
	REINTERPRET_CAST(CCallHayes*,iTelObject)->SetToIdleAndCompleteReq(iReqHandle,aStatus);
	}

//
// CATCallConnectCommands - base class for dial,connect and answer
//

CATCallConnectCommands::CATCallConnectCommands(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
			: CATCallAlterCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{
	iCallType=ENoneCall;
	}

CATCallConnectCommands::~CATCallConnectCommands()
	{
	iIo->RemoveExpectStrings(this);
	iIo->WriteAndTimerCancel(this);	
	}

TBool CATCallConnectCommands::IsPreConnectInProgress()
//
//	The Call class must know whether the pre-connection initialisation sequence is in
//  progress or not.
//
	{
	if (iPreConnectState==ENotInProgress || iPreConnectState==EATInitCompleted)
		return EFalse;
	return ETrue;
	}

void CATCallConnectCommands::CompleteWithIOError(TEventSource aSource,TInt aStatus)
	{
	iPreConnectState = CATCallConnectCommands::ENotInProgress;
	CATCallAlterCommands::CompleteWithIOError(aSource,aStatus);
	}


TInt CATCallConnectCommands::GetSpeakerControlAndVolumeValues(TDes8& aSpeakerCommand,TDes8& aVolumeCommand)
	{
	TInt ret;
	switch (iCallInfo->iSpeakerControl)
		{
	case RCall::EMonitorSpeakerControlAlwaysOff:
		ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameSpeakerAlwaysOff),aSpeakerCommand);
		break;
	case RCall::EMonitorSpeakerControlAlwaysOn:
		ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameSpeakerAlwaysOn),aSpeakerCommand);
		break;
	case RCall::EMonitorSpeakerControlOnExceptDuringDialling:
		ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameSpeakerOnAfterUntilCarrier),aSpeakerCommand);
		break;
	case RCall::EMonitorSpeakerControlOnUntilCarrier:
	case RCall::EMonitorSpeakerControlUnknown:
	default:
		ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameSpeakerOnUntilCarrier),aSpeakerCommand);
		break;
		}
	switch (iCallInfo->iSpeakerVolume)
		{
	case RCall::EMonitorSpeakerVolumeOff:
	case RCall::EMonitorSpeakerVolumeLow:
		ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameSpeakerVolControlLow),aVolumeCommand);
		break;
	case RCall::EMonitorSpeakerVolumeHigh:
		ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameSpeakerVolControlHigh),aVolumeCommand);
		break;
	case RCall::EMonitorSpeakerVolumeMedium:
	case RCall::EMonitorSpeakerVolumeUnknown:
	default:
		ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameSpeakerVolControlMedium),aVolumeCommand);
		break;
		}
	return ret;
	}

void CATCallConnectCommands::PreConnectEventSignal(TEventSource aSource)
	{
	LOGTEXT2(_L8("CATCallConnectCommands::PreConnectEventSignal with aSource %d"),aSource);
	LOGTEXT2(_L8("CATCallConnectCommands::PreConnectEventSignal with iPreConnectState %d"),iPreConnectState);

	//
	// If a timeout has occured then there is not much we can do.
	// Complete client request with KErrTimedOut
	if(aSource==ETimeOutCompletion)
		{
		LOGTEXT(_L8("Timeout Error during Dial"));
		RemoveStdExpectStrings();
		Complete(KErrTimedOut,aSource);
		return;
		}

	//
	// Process event
	TInt ret(KErrNone);
	switch(iPreConnectState)
		{
	case EATDataClassWaitForWriteComplete:
		iIo->Read();
		StandardWriteCompletionHandler(aSource,4);
		iPreConnectState=EATDataClassReadCompleted;
		break;

	case EATDataClassReadCompleted:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
	
		//
		// Ignore any OK, ERROR or any other response from the modem.
		// This is because some modems, eg. those with out speakers, will ERROR these 
		// configuration commands even though the call can go ahead.
		iPreConnectState=EATSendCallInit;

// Note: No "break;", carry on through to the next state...

	case EATSendCallInit:
		{
		//
		// Assemble values for our dial init string
		// Exmaple string is ATM0L0X3S8=0
		TBuf8<KCommsDbSvrMaxFieldLength> atMonConCommand;
		TBuf8<KCommsDbSvrMaxFieldLength> atVolumeCommand;
		TBuf8<KCommsDbSvrMaxFieldLength> levelOfResponseCommand;
		TBuf8<KCommsDbSvrMaxFieldLength> atPauseCommand;
		ret = GetSpeakerControlAndVolumeValues(atMonConCommand,atVolumeCommand);
		if (iCallInfo->iWaitForDialTone==RCall::EDialToneWait)			// use X4
			{
			ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameCallProgress4),levelOfResponseCommand);
			}
		else	// use X3
			{
			ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameCallProgress3),levelOfResponseCommand);
			}
		ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameDialPauseLength),atPauseCommand);
		
		//
		// Send string to modem
		if (ret==KErrNone)
			{
			iTxBuffer.Format(KCallInitCommand,&atMonConCommand,&atVolumeCommand,&levelOfResponseCommand,&atPauseCommand,iCallInfo->iInterval);
			iIo->Write(this,iTxBuffer);	
			iIo->SetTimeOut(this);
			iPreConnectState=EATCallInitWaitForWriteComplete;
			}
		else
			Complete(ret,aSource);		// An error occurred
		break;
		}


	case EATCallInitWaitForWriteComplete:
		StandardWriteCompletionHandler(aSource,4);
		iPreConnectState=EATCallInitCompleted;
		break;

		
	case EATCallInitCompleted:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		//
		// Ignore any OK, ERROR or any other response from the modem.
		// This is because some modems, eg. those with out speakers, will ERROR these 
		// configuration commands even though the call can go ahead.
		RemoveStdExpectStrings();
		
		//
		// If we are dealing with a data call then we still have the MODEM_DATA_INIT_STRING
		// from CommDB to send. If we are dealing with voice or fax then we have
		// completed the init.
		if(iCallType==EDataCall)
			{
			TBuf8<KCommsDbSvrMaxFieldLength> dataInitCommand;
			ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameDataInitString),dataInitCommand);
			if(ret==KErrNone)
				{
				//
				// If string from CommDB is null then use simple 'AT' string
				if(dataInitCommand.Length()==0)
					dataInitCommand=KAT2Command;
				//
				// Write string to modem
				Write(dataInitCommand,4);
				iPreConnectState=EATWaitForDataInitWrite;
				}
			else
				Complete(ret,aSource);		// An error occurred
			}
		else
			{
			iIo->RemoveExpectStrings(this);
			iOKExpectString=NULL;
			iErrorExpectString=NULL;
			iPreConnectState=EATInitCompleted;
			Complete(ret,aSource);
			}
		break;


	case EATWaitForDataInitWrite:
		__ASSERT_DEBUG(iCallType==EDataCall,Panic(ENotDataCallType));
		StandardWriteCompletionHandler(aSource,4);
		iPreConnectState=EATWaitForDataInitOK;
		break;


	case EATWaitForDataInitOK:
		__ASSERT_DEBUG(iCallType==EDataCall,Panic(ENotDataCallType));
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		//
		// Ignore any OK, ERROR or any other response from the modem.
		// This is because some modems, eg. those with out speakers, will ERROR these 
		// configuration commands even though the call can go ahead.
		RemoveStdExpectStrings();
		iIo->WriteAndTimerCancel(this);	
		Write(KAT2Command,4);
		iPreConnectState=EWaitForATCheckWrite;
		break;


	case EWaitForATCheckWrite:
		//
		// When a voice call starts this is where it starts its PreConnection from
		// this state.
		StandardWriteCompletionHandler(aSource,4);
		iPreConnectState=EATWaitForATCheckOK;
		break;


	case EATWaitForATCheckOK:
		ret=ValidateExpectString();
		RemoveStdExpectStrings();
		if(ret==KErrNone)
			{
			//
			// Init is now completed
			iIo->WriteAndTimerCancel(this);	
			iPhoneGlobals->iPhoneStatus.iMode=RPhone::EModeEstablishingLink;
			iPreConnectState=EATInitCompleted;
			}
		else
			Complete(ret,aSource);		// An error occured
		break;


	case ECancelling:
		if (aSource==EWriteCompletion)
			{
			iIo->SetTimeOut(this);
			AddStdExpectStrings();
			}
		if (aSource==EReadCompletion || aSource==ETimeOutCompletion)
			{
			RemoveStdExpectStrings();
			Complete(KErrCancel,aSource);
			}
		break;

	case ENotInProgress:
	case EATInitCompleted:
		__ASSERT_DEBUG(EFalse,Panic(EUnexpectedState));
		}
	}




//
// CATVoiceCallConnectCommands - voice call specific for dial
//
CATVoiceCallConnectCommands::CATVoiceCallConnectCommands(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
			: CATCallConnectCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{
	iCallType=EVoiceCall;
	}

CATVoiceCallConnectCommands::~CATVoiceCallConnectCommands()
	{}

void CATVoiceCallConnectCommands::Start(TTsyReqHandle aTsyReqHandle,TAny* /*aParams*/)
	{
	iReqHandle = aTsyReqHandle;
	TPtrC8 dataClassCommand(KAT2Command);
	Write(dataClassCommand,3);
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iPreConnectState=CATCallConnectCommands::EWaitForATCheckWrite;
	}

void CATVoiceCallConnectCommands::AddCommonExpectStrings()
	{
	// Voice call setup is indicated by OK, with no indication of
	// connection failure except a subsequent "NO CARRIER".
	// See GSM 07.07 version 6.3.0 Annex G "Voice call example"
	if (!iOKExpectString)
		{
		iOKExpectString=iIo->AddExpectString(this,KOkString);
		}
	if (!iNoCarrierExpectString)
		{
		iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);
		}
	}

void CATVoiceCallConnectCommands::RemoveCommonExpectStrings()
	{
	iIo->RemoveExpectString(iOKExpectString);
	iOKExpectString=NULL;
	iIo->RemoveExpectString(iNoCarrierExpectString);
	iNoCarrierExpectString=NULL;
	}

void CATVoiceCallConnectCommands::Complete(TInt aError,TEventSource aSource)
	{
	iIo->WriteAndTimerCancel(this);	
	iIo->RemoveExpectStrings(this);
	iNoCarrierExpectString=NULL;
	iConnectExpectString=NULL;
	iOKExpectString=NULL;
	iPreConnectState = CATCallConnectCommands::ENotInProgress;
	if (aError==KErrNone)
		{
		ChangeLineStatus(RCall::EStatusConnected);
		aError = ChangeCallStatus(RMobileCall::EStatusConnected);
		}
	if (aError==KErrNone)
		{
		iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeUnknown;
		REINTERPRET_CAST(CCallHayes*,iTelObject)->iWaitForNoCarrier->StartWait();
		REINTERPRET_CAST(CCallHayes*,iTelObject)->StartCallTicker();
		}
	else
		{
		//	SetToIdle() sets call to unowned 
		REINTERPRET_CAST(CCallHayes*,iTelObject)->SetToIdle();
		if (aError!=KErrCancel)
			iPhoneGlobals->iPhoneStatus.iInitStatus=EPhoneNotInitialised;
		}
	CATCommands::Complete(aError,aSource);
	iTelObject->ReqCompleted(iReqHandle, aError);
	if (iCallInfo->iClientPanicOccurred!=ENoPanicOccurred)
		{
		iComplete = CCompleteRelinquish::New(iTelObject);
		iComplete->SetWhichCompletion(iCallInfo->iClientPanicOccurred);
		iComplete->Call();	// calls the AysncOneShot Relinquish completion function
		iCallInfo->iClientPanicOccurred = ENoPanicOccurred;
		}	
	if (aSource!=EReadCompletion && !(iIo->ReadPending()))
						// if Complete() called whilst aSource ==EWriteCompletion
									// iIo->Read won't happen otherwise
		iIo->Read();
	}




//
//
// CATDataCallConnectCommands - data call specific for dial/answer/connect
//

CATDataCallConnectCommands::CATDataCallConnectCommands(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
			: CATCallConnectCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{
	iCallType=EDataCall;
	}

CATDataCallConnectCommands::~CATDataCallConnectCommands()
	{}

void CATDataCallConnectCommands::Start(TTsyReqHandle aTsyReqHandle,TAny* /*aParams*/)
	{
	iReqHandle = aTsyReqHandle;
	TPtrC8 dataClassCommand(KDataClassCommand);
	Write(dataClassCommand,3);
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iPreConnectState=CATCallConnectCommands::EATDataClassWaitForWriteComplete;
	}

TInt CATDataCallConnectCommands::AddCommonExpectStrings()
	{
	TInt ret(KErrNone);
	if (!iConnectExpectString)
		{
		ret=iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameConnect),iConnectString);
		if(ret==KErrNone)
			{	
			AppendWildCardChar(iConnectString);
			iConnectExpectString=iIo->AddExpectString(this,iConnectString);
			}
		}
	if (!iNoCarrierExpectString)
		{
		iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);
		}
	return ret;
	}

void CATDataCallConnectCommands::RemoveCommonExpectStrings()
	{
	iIo->RemoveExpectString(iConnectExpectString);
	iConnectExpectString=NULL;
	iIo->RemoveExpectString(iNoCarrierExpectString);
	iNoCarrierExpectString=NULL;
	}

void CATDataCallConnectCommands::ParseForBearerServiceCapsResponseL()
	{
	LOGTEXT(_L8("Parse the CBST Bearer Service values"));
	iBuffer.Set(iIo->Buffer());
	// Look for "CARRIER" in buffer
	TBuf8<KCommsDbSvrMaxFieldLength> carrierString;
	(void)User::LeaveIfError(iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameCarrier),carrierString));
	}

TInt CATDataCallConnectCommands::ParseForBearerCapsResponse()
	{
	LOGTEXT(_L8("Parse the Buffer List for Bearer Service values"));
	iBuffer.Set(iIo->Buffer());
	iCallInfo->iBearerService.iBearerCaps = 0;
	for (TInt i=0;KBearerResponseStrings[i];i++)
		{
		TPtrC8 ptr(KBearerResponseStrings[i]);
		TInt aPos=iBuffer.FindF(ptr);
		if (aPos!=KErrNotFound)
			{
			switch (i)
				{
				case 0:
				case 1:
					iCallInfo->iBearerService.iBearerCaps |= RCall::KBearerCapsProtocolLAPM;
					break;
				case 2:
					iCallInfo->iBearerService.iBearerCaps |= RCall::KBearerCapsProtocolALT_CELLULAR;
					break;
				case 3:
					iCallInfo->iBearerService.iBearerCaps |= RCall::KBearerCapsProtocolALT;
					break;
				case 4:
					iCallInfo->iBearerService.iBearerCaps |= RCall::KBearerCapsCompressionV42bis;
					break;
				case 5:
					iCallInfo->iBearerService.iBearerCaps |= RCall::KBearerCapsCompressionMNP5;
					break;
				default:
					iCallInfo->iBearerService.iBearerCaps = RCall::KBearerCapsProtocolUnknown;
					iCallInfo->iBearerService.iBearerCaps = RCall::KBearerCapsCompressionUnknown;
					return KErrUnknown;
				}
			}
		}
	if (!(iCallInfo->iBearerService.iBearerCaps & (RCall::KBearerCapsProtocolLAPM | RCall::KBearerCapsProtocolALT_CELLULAR | RCall::KBearerCapsProtocolALT)))
		{
		iCallInfo->iBearerService.iBearerCaps |= RCall::KBearerCapsProtocolNone;
		}
	if (!(iCallInfo->iBearerService.iBearerCaps & (RCall::KBearerCapsCompressionV42bis | RCall::KBearerCapsCompressionMNP5)))
		{
		iCallInfo->iBearerService.iBearerCaps |= RCall::KBearerCapsCompressionNone;
		}

	return KErrNone;
	}

TInt CATDataCallConnectCommands::ParseForBearerSpeedResponse()
	{
	LOGTEXT(_L8("Parse the Buffer List for Bearer speed values"));

	iBuffer.Set(iIo->Buffer());
	TLex8 lexString(iBuffer);
	// Look for "CARRIER" in buffer
	TBuf8<KCommsDbSvrMaxFieldLength> carrierString;
	TInt ret=iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameCarrier),carrierString);
	if(ret!=KErrNone)
		return ret;
	TInt aPos=iBuffer.FindF(carrierString);
	if (aPos!=KErrNotFound)
		{
		lexString.Inc(aPos+1);
		lexString.SkipCharacters();
		lexString.SkipSpace();
		lexString.Mark();                     // remember where we are
        lexString.SkipCharacters();           // move to end of character token
        if (lexString.TokenLength() != 0)    // if valid potential token
			{
            TPtrC8 token = lexString.MarkedToken();        // extract token
            if (token.CompareF(KTrasmitResponse) == 0 || token.CompareF(KReceiveResponse) == 0)    // and test
				{
				ret=SetBearerSpeed(lexString);
				if(ret!=KErrNone)
					return ret;
				}
			}
		else
			{
			ret=SetBearerSpeed(lexString);
			if(ret!=KErrNone)
				return ret;
			}
		}
	else
		{
		TBuf8<KCommsDbSvrMaxFieldLength> connectString;
		ret=iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameConnect),connectString);
		if(ret!=KErrNone)
			return ret;
		TInt aPos=iBuffer.FindF(connectString);
		lexString.Inc(aPos+1);
		lexString.SkipCharacters();
		lexString.SkipSpace();
		if (lexString.Peek().IsDigit())
			{
			ret=SetBearerSpeed(lexString);
			if(ret!=KErrNone)
				return ret;
			}
		}
	return KErrNone;
	}

TInt CATDataCallConnectCommands::SetBearerSpeed(TLex8& aLexString)
	{
	TPtrC8 token;
	aLexString.SkipSpaceAndMark();

	while (aLexString.Peek().IsDigit())
		{
		aLexString.Inc();
		}
	if (TUint(aLexString.Peek())==84)
		{
		aLexString.Inc();
		if (TUint(aLexString.Peek())==88)
			{
			// We have TX
			aLexString.Inc();
			}
		}
	token.Set(aLexString.MarkedToken());
	TInt i;
	for(i=0;KBearerSpeedResponseStrings[i];i++)
		{
		TPtrC8 ptr(KBearerSpeedResponseStrings[i]);
		if(token.MatchF(ptr)==KErrNone)
			break;
		}
	switch (i)
		{
	case 0:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData57600;
		break;
	case 1:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData33600;
		break;
	case 2:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData31200;
		break;
	case 3:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData19200;
		break;
	case 4:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData14400;
		break;
	case 5:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData12000;
		break;
	case 6:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData9600;
		break;
	case 7:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData7200;
		break;
	case 8:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData4800;
		break;
	case 9:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData2400;
		break;
	case 10:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData1200;
		break;
	case 11:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData75_1200;
		break;
	case 12:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData1200_75;
		break;
	case 13:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerData300;
		break;
	case 14:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerDataUnknown;
		break;
	default:
		iCallInfo->iBearerService.iBearerSpeed = RCall::EBearerDataUnknown;
		return KErrUnknown;
		}
	return KErrNone;
	}

void CATDataCallConnectCommands::Complete(TInt aError,TEventSource aSource)
	{
	iIo->WriteAndTimerCancel(this);	
	iIo->RemoveExpectStrings(this);
	iNoCarrierExpectString=NULL;
	iConnectExpectString=NULL;
	iOKExpectString=NULL;
	iPreConnectState = CATCallConnectCommands::ENotInProgress;
	if (aError==KErrNone)
		{
		ChangeLineStatus(RCall::EStatusConnected);
		aError = ChangeCallStatus(RMobileCall::EStatusConnected);
		}
	if (aError==KErrNone)
		{
		iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeOnlineData;
		REINTERPRET_CAST(CCallHayes*,iTelObject)->iWaitForNoCarrier->StartWait();
		REINTERPRET_CAST(CCallHayes*,iTelObject)->StartCallTicker();
		}
	else
		{
		//	SetToIdle() sets call to unowned 
		REINTERPRET_CAST(CCallHayes*,iTelObject)->SetToIdle();
		if (aError!=KErrCancel)
			iPhoneGlobals->iPhoneStatus.iInitStatus=EPhoneNotInitialised;
		}
	CATCommands::Complete(aError,aSource);
	iTelObject->ReqCompleted(iReqHandle, aError);
	if (iCallInfo->iClientPanicOccurred!=ENoPanicOccurred)
		{
		iComplete = CCompleteRelinquish::New(iTelObject);
		iComplete->SetWhichCompletion(iCallInfo->iClientPanicOccurred);
		iComplete->Call();	// calls the AysncOneShot Relinquish completion function
		iCallInfo->iClientPanicOccurred = ENoPanicOccurred;
		}	
	if (aSource!=EReadCompletion && !(iIo->ReadPending()))
						// if Complete() called whilst aSource ==EWriteCompletion
									// iIo->Read won't happen otherwise
		iIo->Read();
	}

//
// CATFaxCallConnectCommands - fax call specific for dial/answer/connect
//

CATFaxCallConnectCommands::CATFaxCallConnectCommands(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
			: CATCallConnectCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{
	iCallType=EFaxCall;
	}

CATFaxCallConnectCommands::~CATFaxCallConnectCommands()
	{}

void CATFaxCallConnectCommands::Start(TTsyReqHandle aTsyReqHandle, TAny* /*aParams*/)
	{
	iReqHandle=aTsyReqHandle;
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iPreConnectState=CATCallConnectCommands::EATSendCallInit;
	EventSignal(EReadCompletion);	// EReadCompletion is a dummy enum here
	}

void CATFaxCallConnectCommands::Stop(TTsyReqHandle aTsyReqHandle)
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	if (iPreConnectState!=CATCallConnectCommands::EATInitCompleted)
		{
		iPreConnectState=ECancelling;
		AddStdExpectStrings();
		}

	}

void CATFaxCallConnectCommands::Complete(TInt aError,TEventSource aSource)
//
// Doesn't call CATCommands::Complete() 
//
	{
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	if (aError)
		{
		REINTERPRET_CAST(CCallHayes*,iTelObject)->SetToIdleAndCompleteReq(iReqHandle,aError);
		if (aError!=KErrCancel)
			iPhoneGlobals->iPhoneStatus.iInitStatus=EPhoneNotInitialised;
		if (iCallInfo->iClientPanicOccurred!=ENoPanicOccurred)
			{
			iComplete = CCompleteRelinquish::New(iTelObject);
			iComplete->SetWhichCompletion(iCallInfo->iClientPanicOccurred);
			iComplete->Call();	
			iCallInfo->iClientPanicOccurred = ENoPanicOccurred;
			}	
		if (aSource!=EReadCompletion && !(iIo->ReadPending()))
			iIo->Read();
		}
	else
		CompleteSuccessfully();
	}
