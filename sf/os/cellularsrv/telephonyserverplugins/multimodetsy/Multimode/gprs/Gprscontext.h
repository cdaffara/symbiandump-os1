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

#ifndef __GPRSCONTEXT_H
#define __GPRSCONTEXT_H

#include "gprs.h"

class CATGprsContextActivate;
class CATGprsContextDeactivate;
class CATGprsContextDelete;
class CATGPRSSetConfig;
class CATGPRSGetConfig;
class CATIO;
class CATInit;
class CPhoneGlobals;


class CGprsQoS;
class CGprsQoSEntry;
class CGprsContext :public CSubSessionExtBase
/**
This class implements the RPacketContext API.
CGprsContext inherits from CSubSessionExtBase. It is intended for CGprs usage.
@internalComponent
*/
{
public:
	static	CGprsContext* NewL(CGprs* aParent, CATIO* aIo, CATInit*	aInit, CPhoneGlobals* aPhoneGlobals, const TName& aName, TInt aCid, RPacketContext::TContextConfigGPRS* aContextConfigGPRS);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

	void ContextInfo(RPacketService::TContextInfo* aInfo);
	RPacketService::TContextInfo* ContextInfo();
	void SetContextInfo(const RPacketService::TContextInfo*	aContextInfo);
	TDes* ContextName();
	RPacketContext::TContextConfigGPRS*	Config();
	void SetConfig(const RPacketContext::TContextConfigGPRS* aContextInfo);
	TInt Cid();
	CGprs* Parent();

	void GenerateNewProfileName(TDes& aName);
	void RemoveQoSProfileIfFound(CGprsQoS* aProfile);
protected:
	void	ConstructL(const TName& aName, RPacketContext::TContextConfigGPRS* aContextConfigGPRS);
	CGprsContext(CGprs* aParent, CATIO* iIo, CATInit*	iInit, CPhoneGlobals* iPhoneGlobals, TInt aCid);
	virtual ~CGprsContext();

private: 
	TInt SetConfig(const TTsyReqHandle aTsyReqHandle,const TDesC8* aConfig);
	TInt SetConfigCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetConfig(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig);
	TInt GetConfigCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyConfigChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig);
	TInt NotifyConfigChangedCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Activate(const TTsyReqHandle aTsyReqHandle);
	TInt ActivateCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Deactivate(const TTsyReqHandle aTsyReqHandle);
	TInt DeactivateCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Delete(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt LoanCommPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort* aDataPort);
	TInt LoanCommPortCancel(const TTsyReqHandle aTsyReqHandle);
	TInt RecoverCommPort(const TTsyReqHandle aTsyReqHandle);
	TInt RecoverCommPortCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus);
	TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus);
	TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetDataVolumeTransferred(const TTsyReqHandle aTsyReqHandle,RPacketContext::TDataVolume* aVolume); 
	TInt NotifyDataTransferred(const TTsyReqHandle aTsyReqHandle, RPacketContext::TDataVolume* aVolume, RPacketContext::TNotifyDataTransferredRequest* aData); 
	TInt NotifyDataTransferredCancel(const TTsyReqHandle aTsyReqHandle); 
	TInt GetLastErrorCause(const TTsyReqHandle aTsyReqHandle,TInt* aError);  
	
private:
	TBool ShouldNotProcessIpcCall(TInt aIpc);
	TBool ShouldNotProcessIpcCallWhenComportIsLoaned(TInt aIpc);
private:
	TDblQue<CGprsQoSEntry>					iProfiles;				//< List of profiles
	TInt									iGprsQoSProfileCount; // 
	RPacketService::TContextInfo			iInfo;						//< Internal state. Context information.
	RPacketContext::TContextConfigGPRS		iConfigGPRS;					//< Internal state. Context configuration.
	TInt									iCid;						//< Internal state. Context number.
	CATGprsContextActivate*					iATGprsContextActivate;		//< Pointer to context activation AT command object.
	CATGprsContextDeactivate*				iATGprsContextDeactivate;	//< Pointer to context deactivation AT command object.
	CATGprsContextDelete*					iATGprsContextDelete;	    //< Pointer to context deletion AT command object.
	CATGPRSSetConfig*						iATGPRSSetConfig;			//< Pointer to context configuration AT command object.
	CATGPRSGetConfig*						iATGPRSGetConfig;			//< Pointer to context configuration AT command object.
	CGprs*									iParent;					//< Pointer to context parent.
	CATIO*									iIo;						//< Pointer to communication object.
	CATInit*								iInit;						//< Pointer to AT phone init.
	CPhoneGlobals*							iPhoneGlobals;				//< Pointer to global wide phone states.
};



class CGprsQoS;
class CGprsQoSEntry : public CBase
/**
This class contains a CGprsQoS pointer. It is intended  to 
be part of CGprsContext's linked list of qos profiles
CGprsQoSEntry inherits from CBase. This class is only intended  to be part of the TDblQue list 
that CGprsContext has.
@internalComponent
*/
	{
	friend class CGprsContext;
public:
	CGprsQoSEntry(CGprsQoS* aProfile);
	~CGprsQoSEntry();
	void Deque();
public:
	CGprsQoS* iProfile;							//< Pointer to a qos profile
private:
	TDblQueLink iLink;								//< Link object.
	};

#endif
