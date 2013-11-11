// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ETELBEHAVIOUR_H__
#define __ETELBEHAVIOUR_H__

#include <e32base.h>
#include "etel.h"
#include "etelmm.h"
#include "etelpckt.h"

class CEtelBehaviourBase : public CBase
	{
public:
	virtual ~CEtelBehaviourBase();

	virtual TInt RTelServerLoadPhoneModule(const TDesC& aFileName) = 0;
	virtual TInt RTelServerEnumeratePhones(TInt& aNoOfPhones) = 0;
	virtual TInt RTelServerGetTsyName(const TInt aIndexOfPhone, TDes& aTsyName) = 0;
	virtual TInt RTelServerGetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aInfo) = 0;
	virtual TInt RTelServerUnloadPhoneModule(const TDesC& aFileName) = 0;
	virtual TInt RPhoneOpen(RTelServer& aSession,const TDesC& aName) = 0;
	virtual void RPhoneCancelAsyncRequest(TInt aReqToCancel) = 0;
	virtual TInt RPhoneGetCaps(RPhone::TCaps& aCaps) = 0;
	virtual TInt RMobilePhoneGetCurrentMode(RMobilePhone::TMobilePhoneNetworkMode& aNetworkMode) = 0;
	virtual TInt RMobilePhoneGetMultimodeCaps(TUint32& aCaps) = 0;
	virtual void RMobilePhoneGetSignalStrength(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar) = 0;
	virtual void RMobilePhoneNotifySignalStrengthChange(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar) = 0;
	virtual TInt RPacketServiceOpen(RPhone& aPhone) = 0;
	virtual void RPacketServiceCancelAsyncRequest(TInt aReqToCancel) = 0;
	virtual TInt RPacketServiceGetStatus(RPacketService::TStatus& aPacketStatus) = 0;
	virtual void RPacketServiceGetMSClass(TRequestStatus& aStatus, RPacketService::TMSClass& aCurrentClass, RPacketService::TMSClass& aMaxClass) = 0;
	virtual void RPacketServiceGetNtwkRegStatus(TRequestStatus& aStatus, RPacketService::TRegistrationStatus& aRegistrationStatus) = 0;
	virtual TInt RPacketServiceGetAttachMode(RPacketService::TAttachMode& aMode) = 0;

protected:
	CEtelBehaviourBase();
	void CompleteCurrentRPhoneAsyncRequest(TInt aError);
	void CompleteCurrentRPacketServiceAsyncRequest(TInt aError);

protected:
	TInt iCurrentRPhoneAsyncRequest;
	TRequestStatus* iCurrentRPhoneRequestStatus;
	TInt iCurrentRPacketServiceAsyncRequest;
	TRequestStatus* iCurrentRPacketServiceRequestStatus;
	};

//
// Utility classes for timing - borrowed from MTimer
//
class MTimerObserver
	{
public:
	virtual void TimerComplete(TInt aStatus) = 0;
	};

class COneShotTimer : public CTimer
	{
public:
	static COneShotTimer* NewL(MTimerObserver* aObserver);

private:
	COneShotTimer(MTimerObserver* aObserver);
	void ConstructL();
	virtual void RunL();

private:
	MTimerObserver *iObserver;
	};

class CEtelBehaviour : public CEtelBehaviourBase, public MTimerObserver
	{
public:
	virtual ~CEtelBehaviour();
	static CEtelBehaviour* NewL();

	virtual TInt RTelServerLoadPhoneModule(const TDesC& aFileName);
	virtual TInt RTelServerEnumeratePhones(TInt& aNoOfPhones);
	virtual TInt RTelServerGetTsyName(const TInt aIndexOfPhone, TDes& aTsyName);
	virtual TInt RTelServerGetPhoneInfo(const TInt aIndex,RTelServer::TPhoneInfo& aInfo);
	virtual TInt RTelServerUnloadPhoneModule(const TDesC& aFileName);
	virtual TInt RPhoneOpen(RTelServer& aSession,const TDesC& aName);
	virtual void RPhoneCancelAsyncRequest(TInt aReqToCancel);
	virtual TInt RPhoneGetCaps(RPhone::TCaps& aCaps);
	virtual TInt RMobilePhoneGetCurrentMode(RMobilePhone::TMobilePhoneNetworkMode& aNetworkMode);
	virtual void RMobilePhoneGetSignalStrength(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar);
	virtual void RMobilePhoneNotifySignalStrengthChange(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar);
	virtual TInt RMobilePhoneGetMultimodeCaps(TUint32& aCaps);
	virtual TInt RPacketServiceOpen(RPhone& aPhone);
	virtual void RPacketServiceCancelAsyncRequest(TInt aReqToCancel);
	virtual TInt RPacketServiceGetStatus(RPacketService::TStatus& aPacketStatus);
	virtual void RPacketServiceGetMSClass(TRequestStatus& aStatus, RPacketService::TMSClass& aCurrentClass, RPacketService::TMSClass& aMaxClass);
	virtual void RPacketServiceGetNtwkRegStatus(TRequestStatus& aStatus, RPacketService::TRegistrationStatus& aRegistrationStatus);
	virtual TInt RPacketServiceGetAttachMode(RPacketService::TAttachMode& aMode);

	virtual void TimerComplete(TInt aStatus); 

protected:
	CEtelBehaviour();
	void ConstructL();

private:
	COneShotTimer* iTimer;
	};

//
//  ETEL behaviour for test case NC-4-9
//
class CTest0409Behaviour : public CEtelBehaviour
	{
public:
	virtual ~CTest0409Behaviour();
	static CTest0409Behaviour* NewL();

	// override function from CEtelBehaviourBase
	virtual TInt RTelServerLoadPhoneModule(const TDesC& aFileName);

private:
	CTest0409Behaviour();
	void ConstructL();
	};


//
//  ETEL behaviour for test case NC-4-11
//
class CTest0411Behaviour : public CEtelBehaviour
	{
public:
	virtual ~CTest0411Behaviour();
	static CTest0411Behaviour* NewL();

	// override function from CEtelBehaviourBase
	virtual TInt RPhoneOpen(RTelServer& aSession,const TDesC& aName);
	virtual TInt RTelServerGetTsyName(const TInt aIndexOfPhone, TDes& aTsyName);

private:
	CTest0411Behaviour();
	void ConstructL();
	};


//
//  ETEL behaviour for test case NC-4-13
//
class CTest0413Behaviour : public CEtelBehaviour
	{
public:
	virtual ~CTest0413Behaviour();
	static CTest0413Behaviour* NewL();

	// override function from CEtelBehaviourBase
	virtual TInt RTelServerGetTsyName(const TInt aIndexOfPhone, TDes& aTsyName);
	virtual TInt RPhoneGetCaps(RPhone::TCaps& aCaps);

private:
	CTest0413Behaviour();
	void ConstructL();
	};


//
//  ETEL behaviour for test case NC-4-15
//
class CTest0415Behaviour : public CEtelBehaviour
	{
public:
	virtual ~CTest0415Behaviour();
	static CTest0415Behaviour* NewL();

	// override function from CEtelBehaviourBase
	virtual TInt RTelServerGetTsyName(const TInt aIndexOfPhone, TDes& aTsyName);
	virtual TInt RPacketServiceOpen(RPhone& aPhone);

private:
	CTest0415Behaviour();
	void ConstructL();
	};


//
//  ETEL behaviour for test case NC-4-17
//
class CTest0417Behaviour : public CEtelBehaviour
	{
public:
	virtual ~CTest0417Behaviour();
	static CTest0417Behaviour* NewL();

	// override function from CEtelBehaviourBase
	virtual TInt RTelServerGetTsyName(const TInt aIndexOfPhone, TDes& aTsyName);
	virtual TInt RPacketServiceGetStatus(RPacketService::TStatus& aPacketStatus);

private:
	CTest0417Behaviour();
	void ConstructL();
	};


//
//  ETEL behaviour for test case NC-4-20
//
class CTest0420Behaviour : public CEtelBehaviour
	{
public:
	virtual ~CTest0420Behaviour();
	static CTest0420Behaviour* NewL();

	// override function from CEtelBehaviourBase
	virtual TInt RTelServerGetTsyName(const TInt aIndexOfPhone, TDes& aTsyName);
	virtual TInt RPacketServiceGetAttachMode(RPacketService::TAttachMode& aMode);

private:
	CTest0420Behaviour();
	void ConstructL();
	};

#endif // __ETELBEHAVIOUR_H__

