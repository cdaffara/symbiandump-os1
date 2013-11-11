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
// Gets the default ServiceCentreAddress (SCA) stored in the ME (Mobile Equipment). AT+CSCA?
// 
//

#include "NOTIFY.H"
#include "mSMSMESS.H"
#include "mSMSCSCA.H"
#include "mSLOGGER.H"
#include "ATIO.H"

//
// Macros
//
// This macro is used to help keep un-intereting details outside of the main code
// in this file. The macro uses logging functions and always prefixes writes to the 
// log with the name of the class which is implemented in this file. This macro must
// only be used by code in this file.
#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATSmsGetSCAddress::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif


CATSmsGetSCAddress* CATSmsGetSCAddress::NewL (CATIO* aIo, CTelObject* aTelObject,
												CATInit* aInit, CPhoneGlobals* aGlobals)
	{
	CATSmsGetSCAddress* self = new (ELeave) CATSmsGetSCAddress(aIo, aTelObject, aInit, aGlobals);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATSmsGetSCAddress::CATSmsGetSCAddress(CATIO* aIo, CTelObject* aTelObject,
										 CATInit* aInit, CPhoneGlobals* aGlobals)
:CATSmsCommands(aIo, aTelObject, aInit, aGlobals)
	{}

void CATSmsGetSCAddress::Start(TTsyReqHandle aTsyReqHandle, TAny* aParam)
	{
	iReqHandle = aTsyReqHandle;

	if (aParam!=NULL)
		{
		TTsySmsp* info = static_cast<TTsySmsp*>(aParam);
		iSmspInfo.iBufSize = info->iBufSize;
		iSmspInfo.iBufPtr = info->iBufPtr;
		}

	TInt ret=CATSmsCommands::RequestATCommand(EGetSCAFromPhone);
	if(ret!=KErrNone)
		Complete(ret);
	}

void CATSmsGetSCAddress::EventSignal(TEventSource aSource)
	{
	LOCAL_LOGTEXT("EventSignal","");

	//
	// Allow base class to handle events for us
	LOCAL_LOGTEXT("EventSignal","Giving base class a chance to handle the event");
	CATSmsCommands::EventSignal(aSource);

	//
	// Check if base class has completed our request
	if(CATSmsCommands::RequestATCompleted()==EGetSCAFromPhone)
		{
		LOCAL_LOGTEXT("EventSignal","Base class has completed request to get SCA");
		LOGTEXT2(_L8("completion code: %d"),iRequestError);

		if(iRequestError!=KErrNone)
			{
			Complete(iRequestError);
			}
		else
			{
			//
			// Copy SCA from base class to our client
			TRAPD(ret,CreateSmspListL());
			Complete(ret);
			}
		}
	}

void CATSmsGetSCAddress::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempts to halt the process
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));		
	LOCAL_LOGTEXT("Stop","Cancelling command");
	
	CATSmsCommands::RequestATCommandCancel();
	}

void CATSmsGetSCAddress::Complete(TInt aError)
	{
	Complete(aError,EReadCompletion);
	}

void CATSmsGetSCAddress::CreateSmspListL()
	{
	// Create SMSP list and return the one entry
	CMobilePhoneSmspList* list=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(list);
	list->SetMaxNumberEntries(1);

	RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
	smspEntry.iIndex=0;		
	smspEntry.iValidParams=RMobileSmsMessaging::KSCAIncluded;
	smspEntry.iServiceCentre=CATSmsCommands::iRequestSCA;

	list->AddEntryL(smspEntry);

	*(iSmspInfo.iBufPtr) = list->StoreLC();	 // Stream the list to optimise the size of it.
	*(iSmspInfo.iBufSize) = (*(iSmspInfo.iBufPtr))->Size(); // Store the size of the streamed list to be passed back to the client.   

	CleanupStack::Pop();					// pop the CBufBase allocated by StoreLC
	CleanupStack::PopAndDestroy(list);		// pop AND destroy the list.
	}


void CATSmsGetSCAddress::Complete(TInt aError,TEventSource aSource)
	{
	LOCAL_LOGTEXT("Complete","Enter function");
	LOGTEXT2(_L8("Error Code : %d"), aError);

	CATCommands::Complete(aError,aSource);
	if (iReqHandle != 0)
		{
		if (aError==KErrCancel)
			{
			CMobileSmsMessaging* smsMsg = static_cast<CMobileSmsMessaging*>(iTelObject);
			// Always returns KErrNone
			(void)smsMsg->CompleteSmspListCancel(iReqHandle);
			}
		else
			iTelObject->ReqCompleted(iReqHandle, aError);
		}
	if (aSource==EWriteCompletion)
		iIo->Read();
	}

void CATSmsGetSCAddress::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	iIo->WriteAndTimerCancel(this);
	iTelObject->ReqCompleted(iReqHandle, aStatus);
	}
