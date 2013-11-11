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
// AT Command Base Class Header
// 
//

/**
 @file
 @internalAll 
*/


#ifndef __ATCALL_H__
#define	__ATCALL_H__

#include <cdblen.h>
#include "ATBASE.H"
/**
@internalTechnology
*/
const TInt KDTRLowPeriod=500;	// milliseconds
const TInt KDTRHighSettle=200;
/**
@internalTechnology
*/
const TText8* const KBearerResponseStrings[]={
	_S8("LAP-M"),
	_S8("LAPM"),
	_S8("ALT "),
	_S8("ALT-CELLULAR"),
	_S8("V.42bis"),
	_S8("CLASS 5"),
	NULL
	};
/**
@internalTechnology
*/
const TText8* const KBearerSpeedResponseStrings[]={
	_S8("57600"),
	_S8("33600"),
	_S8("31200"),
	_S8("19200"),
	_S8("14400"),
	_S8("12000"),
	_S8("9600"),
	_S8("7200"),
	_S8("4800"),
	_S8("2400"),
	_S8("1200"),
	_S8("75TX"),
	_S8("1200TX"),
	_S8("300"),
	NULL
	};

class CATInit;
class CATCallAlterCommands : public CATCommands
/**
@internalTechnology
*/
	{
public:
	void CancelCommand(TTsyReqHandle aTsyReqHandle);
protected:
	CATCallAlterCommands(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	~CATCallAlterCommands();
	TInt ChangeCallStatus(RMobileCall::TMobileCallStatus aCallStatus);
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle)=0;
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void Complete(TInt aError,TEventSource aSource)=0;
	};

class CATCallConnectCommands : public CATCallAlterCommands 
/**
@internalTechnology
*/
	{
public:
	TBool IsPreConnectInProgress();
protected:
	CATCallConnectCommands(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATCallConnectCommands();
	TInt GetSpeakerControlAndVolumeValues(TDes8& aSpeakerCommand,TDes8& aVolumeCommand);
	void PreConnectEventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
protected:
	enum {
		ENotInProgress,							//0
		EATDataClassWaitForWriteComplete,		//1
		EATDataClassReadCompleted,				//2
		EATSendCallInit,						//3
		EATCallInitWaitForWriteComplete,		//4
		EATCallInitCompleted,					//5
		EATWaitForDataInitWrite,				//6
		EATWaitForDataInitOK,					//7
		EWaitForATCheckWrite,					//8
		EATWaitForATCheckOK,					//9
		EATInitCompleted,						//10
		ECancelling 							//11
		} iPreConnectState;

	enum 
		{
		ENoneCall,
		EDataCall,
		EVoiceCall,
		EFaxCall
		} iCallType;
	
	CCommChatString* iConnectExpectString;
	CCommChatString* iNoCarrierExpectString;
private:
	TUint iRetryCounter;
	};

class CATVoiceCallConnectCommands : public CATCallConnectCommands
/**
@internalComponent
*/
	{
protected:
	CATVoiceCallConnectCommands(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATVoiceCallConnectCommands();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	void AddCommonExpectStrings();
	void RemoveCommonExpectStrings();
	virtual void Complete(TInt aError,TEventSource aSource);
	};

class CATDataCallConnectCommands : public CATCallConnectCommands
/**
@internalTechnology
*/
	{
protected:
	CATDataCallConnectCommands(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATDataCallConnectCommands();
	TInt ParseForBearerCapsResponse();
	TInt ParseForBearerSpeedResponse();
	void ParseForBearerServiceCapsResponseL();  // new function for CBST parsing

	TInt AddCommonExpectStrings();
	void RemoveCommonExpectStrings();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Complete(TInt aError,TEventSource aSource);
private:
	TBuf8<KCommsDbSvrMaxFieldLength> iConnectString;
	TInt SetBearerSpeed(TLex8& aLexString);
	TInt ConfigurePortForConnection();
	TAny* iBearerServiceType;
	};

class CATFaxCallConnectCommands : public CATCallConnectCommands
/**
@internalComponent
*/
	{
protected:
	CATFaxCallConnectCommands(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATFaxCallConnectCommands();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void Complete(TInt aError,TEventSource aSource);
	virtual void CompleteSuccessfully()=0;
	};

#endif
