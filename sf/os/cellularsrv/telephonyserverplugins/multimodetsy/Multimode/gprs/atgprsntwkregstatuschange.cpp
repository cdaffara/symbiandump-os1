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

#include "atgprsntwkregstatuschange.h"		// Header file for this source file
#include "mSLOGGER.H"					// For LOGTEXT macros
#include "ATIO.H"							// For CATIO class
#include "Matstd.h"						// For AT command strings and timeout values
#include "gprs.h"							// for CGprs class
#include "NOTIFY.H"						// for CNotifications

// Macro for logging text to the log file using the local class name and a simpler 
// call style.
#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATGprsNtwkRegStatusChange::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif


CATGprsNtwkRegStatusChange* CATGprsNtwkRegStatusChange::NewL(CATIO* aIo,CTelObject* aTelObject, CPhoneGlobals* aGlobals)
/**
 * Standard class factory function
 */
 	{
	return (new(ELeave) CATGprsNtwkRegStatusChange(aIo,aTelObject,aGlobals));
	// Currently no need for a ConstructL call, maybe we'll need one in the future...
	}

CATGprsNtwkRegStatusChange::CATGprsNtwkRegStatusChange(CATIO* aIo, CTelObject* aTelObject,CPhoneGlobals* aGlobals)
	:CATBase(aIo,aTelObject,aGlobals)
 	{
	Enable();
	}

CATGprsNtwkRegStatusChange::~CATGprsNtwkRegStatusChange()
	{
	Disable();
	}

void CATGprsNtwkRegStatusChange::Enable()
/** 
 * Adds the all important +CGREG:* expect string so that our ::EventSignal is called when 
 * we get an unsolicited network registration status changes.
 */
	{
	LOCAL_LOGTEXT("Enable","Adding +CGREG:* expect string");
	
	// Ensure +CRING is not on the expect string stack before we add it.
	// (Doing things this way means we do not have to maintain an expect string in our
	// member data)
	iIo->RemoveExpectStrings(this);		

	// Add +CRING expect string
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KCGREGMatchString) != NULL, Panic(EGeneral));
	}

void CATGprsNtwkRegStatusChange::Disable()
/** 
 * Removes all expect strings added by this object. 
 * This object will not process +CRING until ::Enable() is called.
 */
	{
	LOCAL_LOGTEXT("Disable","Removing all CATGprsNtwkRegStatusCchange expect strings");
	iIo->RemoveExpectStrings(this);		
	}

void CATGprsNtwkRegStatusChange::EventSignal(TEventSource aSource)
/**
 * Called when a '+CGREG:' is received.
 * Parses the +CGREG:... string which we recieved from the phone and then triggers
 * the EPacketNtwkRegStatusChange notification as appropriate.
 */
	{
	LOCAL_LOGTEXT("EventSignal","Enter function");

	TInt ret(KErrNone);
	RPacketService::TRegistrationStatus regStatus(RPacketService::EUnknown);
		
	__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
	TRAP(ret,ParseResponseL(regStatus));
	if(ret!=KErrNone)
		{
		LOCAL_LOGTEXT("EventSignal","Unable to parse CGREG response");
		}
	else
		{
		//
		// Trigger the notificaiton mechanism
		LOCAL_LOGTEXT("EventSignal","Triggering EPacketNtwkRegStatusChange notification");
		((CGprs*)iTelObject)->SetRegistrationStatus(regStatus);
		iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject, EPacketNtwkRegStatusChange);

		// We are done, but there is no client request to complete
		// we just return and wait until we get our next +CGREG:* command
		}
	}

void CATGprsNtwkRegStatusChange::ParseResponseL(RPacketService::TRegistrationStatus& aRegStatus)
/**
 * Parses the +CGREG unsolicited string from the modem.
 * An example response woulf be '+CGREG: 2' where the value denotes the
 * current registration status.
 * If parsing succeeds then aRegStatus is updated.
 */
	{
	ParseBufferLC();
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++;
	
	// Validate we have +CGREG at the start 
	if((entry==NULL) ||(entry->iResultPtr!=KCGREGResponseString))
		{
		LOCAL_LOGTEXT("ParseResponseL","Failed to find +CGREG:");
		User::Leave(KErrNotSupported);
		}
	
	// Read in value
	entry=iter++;
	if(!entry)
		{
		LOCAL_LOGTEXT("ParseResponseL","Failed to find 1st parameter");
		User::Leave(KErrNotFound);
		}		
	TInt etsiVal;
	CATParamListEntry::EntryValL(entry,etsiVal);

	// Convert value to equivalent EtelMM enum
	switch(etsiVal)
		{
	case 0:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		aRegStatus=RPacketService::ENotRegisteredNotSearching;
		break;
	case 1:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		aRegStatus=RPacketService::ERegisteredOnHomeNetwork;
		break;
	case 2:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		aRegStatus=RPacketService::ENotRegisteredSearching;
		break;
	case 3:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		aRegStatus=RPacketService::ERegistrationDenied;
		break;
	case 4:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		aRegStatus=RPacketService::EUnknown;
		break;
	case 5:		// ETSI value as defined in ETSI 07.07 section 10.1.14
		aRegStatus=RPacketService::ERegisteredRoaming;
		break;
	default:
		aRegStatus=RPacketService::EUnknown;
		break;
		}
	CleanupStack::PopAndDestroy();	// Pop and destroy the object pushed by ParseBufferLC()
	}


void CATGprsNtwkRegStatusChange::CompleteWithIOError(TEventSource /*aSource*/,TInt /*aStatus*/)
	{
	LOCAL_LOGTEXT("CATGprsNtwkRegStatusChange","CompleteWithIOError");
	// CATIO removes expect strings in event of IO error so don't do it here
	// Also, CATErrorHandler::CleanUp completes notification with error
	}





