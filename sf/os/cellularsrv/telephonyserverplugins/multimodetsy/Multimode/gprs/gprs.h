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

/**
 @file
 @internalAll 
*/

#ifndef _GPRS_H
#define _GPRS_H

#include <etelpckt.h>
#include <pcktcs.h>
#include <e32base.h>
#include "et_tsy.h"
#include <et_phone.h>

class CATIO;
class CATInit;
class CPhoneGlobals;
class CATGprsClass;
class CATGprsDetach;
class CATGprsAttach;
class CATGprsNtwkRegStatus;
class CATGprsNtwkRegStatusChange;
class CGprsContext;
class CGprsContextEntry;

class CGprs :public CSubSessionExtBase
/**
This class implements the GPRS functionality supported in the RPacket APIs
CGprs inherits from CSubSessionExtBase. It is intended for CPhoneGsm usage.
This class contains a linked list of the contexts that it creates.
@internalComponent
*/
	{
public:
	static	CGprs* NewL(CATIO* iIo, CATInit*	iInit, CPhoneGlobals* iPhoneGlobals);
	virtual TInt					ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode				ReqModeL(const TInt aIpc);
	virtual CTelObject*				OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject*				OpenNewObjectL(TDes& aNewName);
	virtual TInt					CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void					Init();
	virtual TInt					RegisterNotification(const TInt aIpc);
	virtual TInt					DeregisterNotification(const TInt aIpc);
	virtual TInt					NumberOfSlotsL(const TInt aIpc);
	RHandleBase* 					GlobalKernelObjectHandle();
	TDes*							NameOfLastAddedContext();

	RPacketService::TStatus			Status();
	void							SetStatus(RPacketService::TStatus aStatus);

	RPacketService::TRegistrationStatus			RegistrationStatus();
	void							SetRegistrationStatus(RPacketService::TRegistrationStatus aStatus);

	TInt							NumberOfContexts();
	RPacketService::TMSClass		CurrentMSClass();
	void							SetCurrentMSClass(RPacketService::TMSClass aMSClass);
	RPacketService::TMSClass		MaxMSClass();
	void							SetContextConfigGPRS(const RPacketContext::TContextConfigGPRS* aContextConfigGPRS);
	void							ContextConfigGPRS(RPacketContext::TContextConfigGPRS* aContextConfigGPRS);
	TInt							MaxNumberOfContexts();
	CGprsContext*					Context(TInt aCount);
	void							RemoveContext(CGprsContext* aContext);

private:
	CGprs(CATIO* iIo, CATInit*	iInit, CPhoneGlobals* iPhoneGlobals);
	virtual ~CGprs();
	void ConstructL();
private:
	void AppendNewContextL(CGprsContext* aNewContext);
	void GenerateNewContextNameAndCid(TDes& aName, TInt& aCid);

	TInt NotifyContextAdded(const TTsyReqHandle aTsyReqHandle, TDes* aContextId);
	TInt NotifyContextAddedCancel(const TTsyReqHandle aTsyReqHandle);

	TInt Attach(const TTsyReqHandle aTsyReqHandle);	
	TInt AttachCancel(const TTsyReqHandle aTsyReqHandle);	

	TInt Detach(const TTsyReqHandle aTsyReqHandle);	
	TInt DetachCancel(const TTsyReqHandle aTsyReqHandle);	

	TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RPacketService::TStatus* aGprsStatus);
	TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RPacketService::TStatus* aGprsStatus);
	TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyContextActivationRequested(const TTsyReqHandle aTsyReqHandle, RPacketContext::TProtocolType* aPdpType);
	TInt NotifyContextActivationRequestedCancel(const TTsyReqHandle aTsyReqHandle);

	TInt RejectActivationRequest(const TTsyReqHandle aTsyReqHandle);
	TInt RejectActivationRequestCancel(const TTsyReqHandle aTsyReqHandle);

	TInt EnumerateContexts(const TTsyReqHandle aTsyReqHandle,TInt* aCount, TInt* aMaxAllowed);
	TInt GetContextInfo(const TTsyReqHandle aTsyReqHandle,TInt aIndex,RPacketService::TContextInfo* aInfo);
	TInt GetContextInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetNtwkRegStatus(const TTsyReqHandle aTsyReqHandle,RPacketService::TRegistrationStatus* aRegistrationStatus);
	TInt GetNtwkRegStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyChangeOfNtwkRegStatus(const TTsyReqHandle aTsyReqHandle,RPacketService::TRegistrationStatus* aRegistrationStatus);
	TInt NotifyChangeOfNtwkRegStatusCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetMSClass(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aCurrentClass, RPacketService::TMSClass* aMaxClass);
	TInt GetMSClassCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SetMSClass(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aClass);
	TInt SetMSClassCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyMSClassChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aNewClass);
	TInt NotifyMSClassChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetStaticCaps(const TTsyReqHandle aTsyReqHandle, TUint* aCaps,RPacketContext::TProtocolType* aPdpType);

	TInt GetDynamicCaps(const TTsyReqHandle aTsyReqHandle, RPacketService::TDynamicCapsFlags* aCaps);
	TInt NotifyDynamicCapsChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TDynamicCapsFlags* aCaps);
	TInt NotifyDynamicCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode);
	TInt GetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode);

	TInt SetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, const TDesC8* aPckg);
	TInt GetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg);	

private:
	TDblQue<CGprsContextEntry>		iContexts;				//< List of contexts.
	CATIO*							iIo;					//< Pointer to communication object.
	CATGprsAttach*					iATGPRSAttach;			//< Pointer to AT GPRS attach command object.
	CATGprsDetach*					iATGprsDetach;			//< Pointer to AT GPRS detach command object.
	CATInit*						iInit;					//< Pointer to AT phone init object.
	CATGprsClass*					iATGprsClass;			//< Pointer to AT GPRS MS class command object.
	CATGprsNtwkRegStatus* iATGprsNtwkRegStatus; //< Pointer to AT GPRS get current network registration command object. 
	CATGprsNtwkRegStatusChange* iATGprsNtwkRegStatusChange; //< Pointer to AT machine which triggers notifications when phone changes reg status
	CPhoneGlobals*					iPhoneGlobals;			//< Pointer to phone globals that store phone wide parameters.

	// internal GPRS connection state.
	RPacketContext::TContextConfigGPRS iDefContextConfigGPRS;	//< Internal state. Default context settings.
	RMutex iMutex; //< Synchronisation mutex
	};

class CGprsContext;

class CGprsContextEntry : public CBase
/**
This class contains a CGprsContext pointer. It is intended  to 
be part of CGprs's linked list of contexts
CGprs inherits from CBase. This class is only intended  to be part of the TDblQue list 
that CGprs has.
@internalComponent
*/
	{
	friend class CGprs;
public:
	CGprsContextEntry(CGprsContext* aContext);
	~CGprsContextEntry();
	void Deque();
public:
	CGprsContext* iContext;							//< Pointer to a context.
private:
	TDblQueLink iLink;								//< Link object.
	};
#endif
