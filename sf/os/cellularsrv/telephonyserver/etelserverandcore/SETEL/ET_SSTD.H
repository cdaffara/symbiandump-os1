// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**@file
   @internalAll */
#if !defined(__ET_SSTD_H__)
#define __ET_SSTD_H__

#if !defined(__ETEL_H__)
#include "ETEL.H"
#endif

#if !defined(__ET_PHONE_H__)
#include "ET_PHONE.H"
#endif

#if !defined(__ET_CLSVR_H__)
#include <et_clsvr.h>
#endif

#if !defined(__ETELEXT_H__)
#include "ETELEXT.H"
#endif

#if !defined(__ET_EMERGENCYCLIENT_H__)
#include "et_emergencyclient.h"
#endif

#include<cfshared.h>
#include<cfmsgs.h>
#include<cfutil.h>
using namespace CommsFW;

/**
@internalComponent
*/
_LIT(KEtelDLLName,"ETEL.DLL");

//
// Asychronously Destroy Dummy Session
//
class CDestroyDummySubSession : public CAsyncOneShot
/**
@internalComponent
*/
	{
public:
	static CDestroyDummySubSession* NewL(CTelServer* aTelServer,CTelObject* aTelObject);
	~CDestroyDummySubSession();

protected:
	CDestroyDummySubSession(CTelServer* aTelServer,CTelObject* aTelObject);
	virtual void RunL();
public:
	CTelObject* iTelObject;
	CTelServer* iTelServer;
	
	TBool iOpen;	
	};

//
// Asychronously Stop the Scheduler
//
class CTelSchedulerStop : public CAsyncOneShot
/**
@internalComponent
*/
	{
public:
	static CTelSchedulerStop* NewL();
	~CTelSchedulerStop();
protected:
	CTelSchedulerStop();
	virtual void RunL();
	};

//
// ETel Server Definition
//
class CTelChannelHandler;
class CPhoneManager;

class CTelServer : public CPolicyServer
/**
@internalComponent
*/
	{
public:
	enum {EPriority=1000};
public:
	static CTelServer* NewL(RCFChannel::TMsgQueues aRxQueues, RCFChannel::TMsgQueues aTxQueues);
	void ConstructL(RCFChannel::TMsgQueues aRxQueues, RCFChannel::TMsgQueues aTxQueues);
	~CTelServer();

	virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	void Dec();
	void Inc();
	TInt Count() const;
	TBool IsPriorityClient(const CTelSession* aSession) const;
	TInt SetPriorityClient(CTelSession* aSession);
	TInt RemovePriorityClient(CTelSession* aSession);
	void CFMessageShutdown(const CommsFW::TCFShutdownMsg& aMessage);
	void SetEmergencyClientSession(CTelSession* aEmergencyClientSession);
	TInt EmergencyClientSessionsCount();
	TInt RemoveEmergencyClient(CTelSession* aSession);
	TBool IsEmergencyClient(const CTelSession* aSession) const;
private:
	CTelServer(TInt aPriority);
	virtual TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);
private:
	TInt iSessionCount;
	TBool iShutGracefully;
	CTelSchedulerStop* iSch;
	CPhoneManager* iPhoneManager;
	CTelSession* iPriorityClientSession;
	CTelChannelHandler* iChannelHandler;
	static const TUint iRangeCount;
	static const TInt iRanges[];
 	static const TUint8 iElementsIndex[];
 	static const TPolicyElement iElements[];
	static const TPolicy iPolicy;
	TInt32 iPriorityClientSid;
	TInt32 iLocationServicesNetworkGatewaySid;
	RPointerArray<CTelSession> iEmergencyClientSessions;
	
	TSecureId iSatEngineSid;
	};

enum TTelObjectOpenSource
	{
	ETelObjectOpenSourceSession,
	ETelObjectOpenSourceSubSession,
	ETelObjectOpenByNameSourceSession,
	ETelObjectOpenByNameSourceSubSession
	};

class CTelChannelHandler:public CCFModuleChannelHandler
	/** Main Comms Channel traffic handler for Etel.
	Listens for incoming messages and deliverst them to ETel
	as well as sending messages on behalf of ETel.
	@internalComponent */
	{
public:
  	static CTelChannelHandler* NewL(RCFChannel::TMsgQueues aRxQueues, 
               RCFChannel::TMsgQueues aTxQueues, CTelServer* aServer);
private:
	CTelChannelHandler(CTelServer* aServer);
	virtual void CFMessageShutdown(const CommsFW::TCFShutdownMsg& aMessage);
private:
	CTelServer* iServer;
	};

//
// ETel Session Definition
//
class CTelSession : public CSession2
/**
@internalComponent
*/
	{
public:
	CTelSession(CPhoneManager* aPhoneManager);
	virtual ~CTelSession();
	virtual void ServiceL(const RMessage2& aMessage);

	CTelObject* CObjectFromHandle(TUint aHandle) const;
	CTelServer* TelServer() const;
	TBool IsExpectingExtendedError() const;
	TBool IsUnicodeReq(TInt aReq) const;
	RHeap* EmergencyClientHeap(TInt aReq) const;
	TBool IsEmergencyClientReq(TInt aReq) const;
private:
	inline CTelServer* Server() const;
	void CreateL();

	TInt GetModuleName(TDes& aName) const;
	void LoadPhoneModule();
	void ClosePhoneModule();
	void EnumeratePhones();
	void IsSupportedByTsy();
	void NewTelObject(TTelObjectOpenSource aSource);
	void PhoneInfo(TInt aIndex);
	void GetTsyNameByPhone(const TInt aIndexOfPhone);
	void SetPriorityClient();
	void UnsetPriorityClient();
	void GetTsyVersionNumber() const;

	TInt Write(TUint aIndexOfMsgParam,const TDesC8& aDes,TInt aOffset=0) const;
	TInt Read(TUint aIndexOfMsgParam,TDes8& aDes,TInt aOffset=0) const;
	TInt Write(TUint aIndexOfMsgParam,const TDesC16& aDes,TInt aOffset=0) const;
	TInt Read(TUint aIndexOfMsgParam,TDes16& aDes,TInt aOffset=0) const;

	TPtrC StripOutNextName(TPtrC& aRemainingName,const TFullName& aFullName);
	void CheckAndAppendNewName(TDes& aName);
	void GenerateName(TDes& aName);
	void SetExtendedErrorGranularity();
	TInt SetEmergencyClient(const RMessage2& aMessage);
	TInt PreallocateEmergencyHeap(TUint32 aEmergencyRequest);
	void UnsetEmergencyClient();
	void SetPriorityClientV2();
	
private: // data
	CPhoneManager* iPhoneManager;
	CObjectIx* iTsyModulesIx;
	CObjectIx* iObjectIx;
	TUint32 iNameIndex;
	RTelServer::TErrorGranularity iErrorGranularity;

	RMessage2 iMessage;
	RHeap* iEmergencyHeap;
	};

class CTelScheduler : public CActiveScheduler
/**
@internalComponent
*/
	{
public:
	static CTelScheduler* New();
	void Error(TInt anError) const;
	};

class CDuplicatePhoneInfo : public CBase
	{
public:
	static CDuplicatePhoneInfo* NewLC(const TDesC* aTsyName,const TDesC* aOriginalName,const TDesC* aNewName);
	~CDuplicatePhoneInfo();
protected:
	CDuplicatePhoneInfo();
	void ConstructL(const TDesC* aTsyName,const TDesC* aOriginalName,const TDesC* aNewName);
public:
	HBufC* iTsyName;
	HBufC* iOriginalName;	
	HBufC* iNewName;
	};

//
// Phone Manager Definition
//
class CPhoneManager : public CBase
/**
@internalComponent
*/
	{
public:
	static CPhoneManager* NewL();
	CPhoneManager();
	void ConstructL();
	~CPhoneManager();

	CPhoneFactoryBase* LoadPhoneModuleL(const TDesC& aFileName);
	CPhoneFactoryBase* OpenPhoneFactoryFromTsyL(const TDesC& aName);
	CPhoneBase* OpenPhoneFromFactoryL(CPhoneFactoryBase* aParent,const TDesC& aName);
	CTelObject* OpenSubSessionObjectL(CTelObject* aParent, TDes& aNewName);
	CTelObject* OpenSubSessionObjectByNameL(CTelObject* aParent,const TDesC& aName/*,TTelObjectOpenSource aSource*/);
	CTelObject* OpenSubSessionObjectByNameAndTypeL(CTelObject* aParent,const TDesC& aName, const TDesC& aType);
	TInt EnumeratePhones() const;
	TInt GetPhoneInfo(TUint aIndex,RTelServer::TPhoneInfo& aInfo) const;
	TInt GetTsyName(const TInt aIndexOfPhone,TDes& aTsyName) const;
	CPhoneFactoryBase* PhoneFactoryObjectFromPhoneName(const TDesC& aPhoneName) const;
	CPhoneFactoryBase* PhoneFactoryObjectFromPhoneIndex(TUint aIndex) const;
//	TBool DuplicatePhoneName() const;
	TBool DuplicatePhoneName(CPhoneFactoryBase* aPhoneFac) const;
	TInt RenameDuplicatePhoneName() const;

	void StoreDuplicateNameL(const TDesC& aTsyName,const TDesC& aOriginalName,const TDesC& aNewName) const;
	TInt ConvertPhoneNameToOriginal(TDes& aName) const;
	TInt ConvertPhoneNameFromOriginal(const TDesC& aTsyName, TDes& aPhoneName) const;
	void RemoveDuplicatePhoneInfo(const TDesC& aTsyName);
private:
	CObjectConIx* iContainerIx;
	CObjectCon* iTsyModulesCon;
	CObjectCon* iObjectCon;
	CArrayPtrFlat<CDuplicatePhoneInfo>* iDuplicatePhones;
	};




/**
@internalComponent
*/
IMPORT_C TInt EtelServerThread(TAny* aArg);
GLREF_C void PanicClient(TEtelPanic aPanic,const RMessage2& aMessage);
GLREF_C void Fault(TEtelFault aFault);

#endif
