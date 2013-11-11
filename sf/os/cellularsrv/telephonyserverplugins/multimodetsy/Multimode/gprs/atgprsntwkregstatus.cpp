// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <et_phone.h>		// For CTelObject

#include "atgprsntwkregstatus.h"		// Header file for this source file
#include "atgprsntwkregstatuschange.h"		// for CATGprsNtwkRegStatusChange class 
#include "mSLOGGER.H"				// For LOGTEXT macros
#include "ATIO.H"				// For CATIO class
#include "Matstd.h"			// For AT command strings and timeout values

const TUint KWriteTimeout=10000;		// Is in milli seconds
const TInt KReadTimeout=10;		// Is in seconds

//
// Macro for logging text to the log file using the local class name and a simpler 
// call style.
#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATGprsNtwkRegStatus::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif



/**
 * @file
 * This file implements the CATGprsNtwkRegStatus class which is one of the state machine used by the
 * GPRS AT TSY library. 
 * This state machine uses the "AT+CGREG?" command to find out the phones current network
 * registration status.
 */

CATGprsNtwkRegStatus* CATGprsNtwkRegStatus::NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals,CATGprsNtwkRegStatusChange* aATGprsNtwkRegStatusChange)
/**
 * Symbian standard factory function (also known as 1st stage constructor)
 */
	{
	CATGprsNtwkRegStatus* p=new(ELeave) CATGprsNtwkRegStatus(aIo, aTelObject, aInit, aPhoneGlobals,aATGprsNtwkRegStatusChange);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}

void CATGprsNtwkRegStatus::ConstructL()
/**
 * Symbian standard 2nd stage constructor
 */
	{
	// Allow our base class a chance to do its construction
	CATCommands::ConstructL();
	}

CATGprsNtwkRegStatus::CATGprsNtwkRegStatus(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals,CATGprsNtwkRegStatusChange* aATGprsNtwkRegStatusChange)
	: CATCommands(aIo, aTelObject, aInit, aPhoneGlobals),
	iATGprsNtwkRegStatusChange(aATGprsNtwkRegStatusChange)
/**
 * C++ Constructor
 */
	{
	}

CATGprsNtwkRegStatus::~CATGprsNtwkRegStatus()
/**
 * C++ Destructor
 */
	{
	iIo->RemoveExpectStrings(this);				
	}

void CATGprsNtwkRegStatus::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)
/**
 * This method called when a client wants to start the state machine
 * and get the current network registration from the phone.
 */
	{
	// Save the data in the arguments in our class data for later use
	iReqHandle = aTsyReqHandle;
	iRegStatus = REINTERPRET_CAST(RPacketService::TRegistrationStatus*, aParams);

	//
	// Disable the +CRING watcher to ensure it does not steal the 
	// +CRING modem response from the modem
	iATGprsNtwkRegStatusChange->Disable();
	
	// Start the writing of the 'AT+CGREG?' command to the phone
	iTxBuffer.Format(KGetCGREGCommand);
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,KWriteTimeout);
	iState=EWaitForWrite;
	}

void CATGprsNtwkRegStatus::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This method called when a client wants to stop/cancel this state machine.
 */
	{
	LOCAL_LOGTEXT("Stop","Enter method");
	if(iState!=EIdle && aTsyReqHandle==iReqHandle)
		{
		LOCAL_LOGTEXT("Stop","Cancelling client request");
		Complete(KErrCancel,ETimeOutCompletion);
		}
	}

void CATGprsNtwkRegStatus::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client with an error.
 * @param aSource source of event from communication class.
 * @param aStatus status of event.
 */
	{
	LOCAL_LOGTEXT("CompleteWithIOError","Enter function");
	Complete(aStatus, aSource);
	}


void CATGprsNtwkRegStatus::Complete(TInt aError,TEventSource aSource)
/**
 * This Function completes the command from the client.
 * @param aError an error code to relay to client.
 */
	{
	LOGTEXT2(_L8("CATGprsNtwkRegStatus::Complete called error: %d"), aError);

	// Cleanup our use of the CATIO class
	RemoveStdExpectStrings();
	iIo->WriteAndTimerCancel(this);	
	iIo->RemoveExpectStrings(this);

	//
	// Enable the +CRING watcher so it can get back to doing what it does
	// best (ie. catching the unsolicted +CRING commands from the modem).
	iATGprsNtwkRegStatusChange->Enable();

	// Allow our base class to do its thing and then complete the client request
	CATCommands::Complete(aError,aSource);		
	iTelObject->ReqCompleted(iReqHandle, aError);

	iState=EIdle;
	}


void CATGprsNtwkRegStatus::EventSignal(TEventSource aSource)
/**
 * Main body of the state machine.
 *
 * This method with iState==EWaitForWrite when our 
 * 'AT+CGREG?' has been successfully sent to the phone/modem.
 * 
 * This method with iState==EWaitForRead when received a response from the 
 * phone/modem.
 */
	{
	LOCAL_LOGTEXT("EventSignal","Enter method");
	TInt ret(KErrNone);

	if (aSource==ETimeOutCompletion)
		{
		//
		// Catch timeout events
		LOCAL_LOGTEXT("EventSignal","Timeout event");
		Complete(KErrTimedOut,aSource);
		}
	else
		{
		//
		// Process non-timeout events
		switch(iState)
			{
		case EWaitForWrite:
			LOCAL_LOGTEXT("EventSignal","EWaitForWrite event");
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource,KReadTimeout);
			iState=EWaitForRead;
			break;

		case EWaitForRead:
			LOCAL_LOGTEXT("EventSignal","EWaitForRead event");
			ret = ValidateExpectString();
			RemoveStdExpectStrings();
			
			// If modem returned ERROR then assume the modem does not suport this command
			if(ret!=KErrNone)		
				{
				Complete(KErrNotSupported,aSource);
				}
			else
				{
				TRAP(ret,ParseResponseL());
				Complete(ret,aSource);
				}
			break;
	
		case EIdle:		// All states must be included in the switch statement to 
			break;			// prevent an ARM compilation warning
			}			
		}
	}


void CATGprsNtwkRegStatus::ParseResponseL()
/**
 * This method parses the modems response to the 'AT+CGREG?' command.
 * An example response is '+CGREG: 0,1' where second value denotes the
 * current registration status.
 */
	{
	ParseBufferLC();
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++;
	
	if((entry==NULL)||(entry->iResultPtr!=KCGREGResponseString))
		{
		LOCAL_LOGTEXT("ParseResponseL","Failed to find +CGREG:");
		User::Leave(KErrNotSupported);
		}
	
	// Skip over first parameter
	entry=iter++;
	if(!entry)
		{
		LOCAL_LOGTEXT("ParseResponseL","Failed to find 1st parameter");
		User::Leave(KErrNotFound);
		}		

	// Read in second parameter
	entry=iter++;
	if(!entry)
		{
		LOCAL_LOGTEXT("ParseResponseL","Failed to find 2nd parameter");
		User::Leave(KErrNotFound);
		}		
	TInt etsiVal;
	CATParamListEntry::EntryValL(entry,etsiVal);

	// Convert scond value to equivalent EtelMM enum
	switch(etsiVal)
		{
	case 0:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		*iRegStatus=RPacketService::ENotRegisteredNotSearching;
		break;
	case 1:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		*iRegStatus=RPacketService::ERegisteredOnHomeNetwork;
		break;
	case 2:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		*iRegStatus=RPacketService::ENotRegisteredSearching;
		break;
	case 3:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		*iRegStatus=RPacketService::ERegistrationDenied;
		break;
	case 4:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		*iRegStatus=RPacketService::EUnknown;
		break;
	case 5:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		*iRegStatus=RPacketService::ERegisteredRoaming;
		break;
	default:
		*iRegStatus=RPacketService::EUnknown;
		break;
		}
		
	CleanupStack::PopAndDestroy();	// Pop and destroy the object pushed by ParseBufferLC()
	}

