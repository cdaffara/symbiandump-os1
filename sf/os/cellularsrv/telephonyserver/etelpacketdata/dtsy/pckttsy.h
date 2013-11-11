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

/**@file
   @internalComponent */
#ifndef __PCKTTSY_H
#define __PCKTTSY_H

#include <e32base.h>
#include <et_phone.h>
#include "etelpckt.h"
#include "coretsy.h"
#include "pcktlist.h"

class CListReadAllAttempt : public CBase
	{
public:
	static CListReadAllAttempt* NewL(TClientId* aId, TTsyReqHandle aReqHandle);
	~CListReadAllAttempt();

protected:
	CListReadAllAttempt(TClientId* aId, TTsyReqHandle aReqHandle);

private:
	void ConstructL();

public:
	CBufBase* iListBuf;
	TClientId iClient;
	TTsyReqHandle iReqHandle;
	};

class CGprsQoSDGprsTsy : public CSubSessionExtDummyBase
/**
Dummy Tsy class for QoS functionality.
@internalComponent
*/
	{
public:
	static CGprsQoSDGprsTsy* NewL(CPhoneFactoryDummyBase* aFac);
	CGprsQoSDGprsTsy(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CGprsQoSDGprsTsy();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

private:
	TInt SetProfile(const TTsyReqHandle aTsyReqHandle,const TDesC8* aConfig);
	TInt SetProfileCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetProfile(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig);
	TInt GetProfileCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetProfileCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig);
	TInt GetProfileCapsCancel(const TTsyReqHandle aTsyReqHandle);
	// Notify changed. Useful for a second client with handle to context object to know
	// when the first client has configured the context.
	TInt NotifyProfileChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig);
	TInt NotifyProfileChangedCancel(const TTsyReqHandle aTsyReqHandle);
private:
	TInt iNotifyProfileChanged;
	TInt iSetProfile;
	TInt iGetProfile;
	TInt iGetProfileCaps;
	TTsyReqHandle iTsyAsyncReqHandle;	
	};


class CGprsContextDGprsTsy : public CSubSessionExtDummyBase
/**
Dummy Tsy class for Packet context functionality.
@internalComponent
*/
	{
public:
	static CGprsContextDGprsTsy* NewL(CPhoneFactoryDummyBase* aFac);
	CGprsContextDGprsTsy(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CGprsContextDGprsTsy();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
protected:
	TInt SetConfig(const TTsyReqHandle aTsyReqHandle,const TDesC8* aConfig);
	TInt SetConfigCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetConfig(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig);
	TInt GetConfigCancel(const TTsyReqHandle aTsyReqHandle);

	// Notify changed. Useful for a second client with handle to context object to know
	// when the first client has configured the context. 

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
	TInt GetProfileName(const TTsyReqHandle aTsyReqHandle, TName* aQoSProfile);

	TInt GetConnectionSpeed(const TTsyReqHandle aTsyReqHandle,TUint* aRate); 
	TInt NotifyConnectionSpeedChange(const TTsyReqHandle aTsyReqHandle, TUint* aRate);
	TInt NotifyConnectionSpeedChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetDataVolumeTransferred(const TTsyReqHandle aTsyReqHandle,RPacketContext::TDataVolume* aVolume); 
	TInt GetDataVolumeTransferredCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyDataTransferred(const TTsyReqHandle aTsyReqHandle, RPacketContext::TDataVolume* aVolume, RPacketContext::TNotifyDataTransferredRequest* aData); 
	TInt NotifyDataTransferredCancel(const TTsyReqHandle aTsyReqHandle); 

	TInt GetLastErrorCause(const TTsyReqHandle aTsyReqHandle,TInt* aError);

	TInt InitialiseContext(const TTsyReqHandle aTsyReqHandle, TDes8* aDataChannel);
	TInt InitialiseContextCancel(const TTsyReqHandle aTsyReqHandle);

	TInt EnumeratePacketFilters(const TTsyReqHandle aTsyReqHandle,TInt* aCount);
	TInt EnumeratePacketFiltersCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetPacketFilterInfo(const TTsyReqHandle aTsyReqHandle,TInt* aIndex, TDes8* aPacketFilterInfo);
	TInt GetPacketFilterInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt AddPacketFilter(const TTsyReqHandle aTsyReqHandle, const TDesC8* aPacketFilterInfo);
	TInt AddPacketFilterCancel(const TTsyReqHandle aTsyReqHandle);

	TInt RemovePacketFilter(const TTsyReqHandle aTsyReqHandle,TInt* aId);
	TInt RemovePacketFilterCancel(const TTsyReqHandle aTsyReqHandle);

	TInt ModifyActiveContext(const TTsyReqHandle aTsyReqHandle);
	TInt ModifyActiveContextCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetDnsInfo(const TTsyReqHandle aTsyReqHandle, const TDesC8* aDnsInfo); 
	TInt GetDnsInfoCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt CreateNewTFT(const TTsyReqHandle aTsyReqHandle,const TInt* aTFT);
	TInt CreateNewTFTCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteTFT(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteTFTCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetAddMediaAuthorization(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt GetRemoveMediaAuthorization(const TTsyReqHandle aTsyReqHandle, RPacketContext::TAuthorizationToken* aAuthorizationToken);
	TInt GetAddMediaAuthorizationCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetRemoveMediaAuthorizationCancel(const TTsyReqHandle aTsyReqHandle);

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    TInt GetConnectionInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo);
    TInt GetConnectionInfoCancel(const TTsyReqHandle aTsyReqHandle);
    TInt NotifyConnectionInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo);
    TInt NotifyConnectionInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);
    TInt FillConnectionInfo(TDes8* aInfo);
		
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
protected:
	TTsyReqHandle iTsyAsyncReqHandle;
private:
	TInt iNotifyConfigChanged;
	TInt iNotifyStatusChange;
	TInt iNotifyDataTransferred;
	TInt iSetConfig;
	TInt iGetConfig;
	TInt iActivate;
	TInt iDeactivate;
	TInt iDelete;
	TInt iLoanCommPort;
	TInt iRecoverCommPort;
	TInt iNotifySpeedChange;
	TInt iGetDataVolumeTransferred;
	TInt iInitialiseContext;
	TInt iEnumeratePacketFilters;
	TInt iGetPacketFilterInfo;
	TInt iAddPacketFilter;
	TInt iRemovePacketFilter;
	TInt iModifyActiveContext;
	TInt iGetDNSInfo;
	TInt iAddMediaAuthorization;
	TInt iRemoveMediaAuthorization;
    TInt iGetConnectionInfo;
    TInt iNotifyConnectionInfoChange;
	TInt iGprsQoSObjectCount;
	TInt iCreateNewTFT;
	TInt iDeleteTFT;
	};


class CGprsDGprsTsy : public CSubSessionExtDummyBase
/**
Dummy Tsy class for Packet service functionality.
@internalTechnology
*/
	{
public:
	static CGprsDGprsTsy* NewL(CPhoneFactoryDummyBase* aFac);
	CGprsDGprsTsy(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CGprsDGprsTsy();

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual RHandleBase* GlobalKernelObjectHandle();
	
	// methods for the list of aos which correspond to the outstanding delayed asyncrhonous reqs
	TInt RemoveDelayedReq(const TTsyReqHandle aTsyReqHandle);
	void AddDelayedReq(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject);
	// methods for the list of aos which correspond to the outstanding location services delayed asyncrhonous reqs
	void AddDelayedReq(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject,TInt aPriority);
private:
	void RegisterDelayedReqL(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject);
	void RegisterDelayedReqL(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject,TInt aPriority);


private:
	TInt NotifyContextAdded(const TTsyReqHandle aTsyReqHandle, TDes* aContextId);
	TInt NotifyContextAddedCancel(const TTsyReqHandle aTsyReqHandle);

	TInt Attach(const TTsyReqHandle aTsyReqHandle);	
	TInt AttachCancel(const TTsyReqHandle aTsyReqHandle);	

	TInt Detach(const TTsyReqHandle aTsyReqHandle);	
	TInt DetachCancel(const TTsyReqHandle aTsyReqHandle);	

	TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RPacketService::TStatus* aGprsStatus);
	TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RPacketService::TStatus* aGprsStatus);
	TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyContextActivationRequested(const TTsyReqHandle aTsyReqHandle, TPacketDataConfigBase* aConfig);
	TInt NotifyContextActivationRequestedCancel(const TTsyReqHandle aTsyReqHandle);

	TInt RejectActivationRequest(const TTsyReqHandle aTsyReqHandle);
	TInt RejectActivationRequestCancel(const TTsyReqHandle aTsyReqHandle);

	TInt EnumerateContexts(const TTsyReqHandle aTsyReqHandle,TInt* aCount, TInt* aMaxAllowed);
	TInt GetContextInfo(const TTsyReqHandle aTsyReqHandle,TInt* aIndex,RPacketService::TContextInfo* aInfo);
	TInt GetContextInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetNtwkRegStatus(const TTsyReqHandle aTsyReqHandle,RPacketService::TRegistrationStatus* aRegistrationStatus);
	TInt NotifyChangeOfNtwkRegStatus(const TTsyReqHandle aTsyReqHandle,RPacketService::TRegistrationStatus* aRegistrationStatus);
	TInt NotifyChangeOfNtwkRegStatusCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetMSClass(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aCurrentClass, RPacketService::TMSClass* aMaxClass);
	TInt GetMSClassCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SetMSClass(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aClass);
	TInt SetMSClassCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyMSClassChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aNewClass);
	TInt NotifyMSClassChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetStaticCaps(const TTsyReqHandle aTsyReqHandle, TUint* aCaps, RPacketContext::TProtocolType* aPdpType);

	TInt GetDynamicCaps(const TTsyReqHandle aTsyReqHandle, RPacketService::TDynamicCapsFlags* aCaps);
	TInt NotifyDynamicCapsChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TDynamicCapsFlags* aCaps);
	TInt NotifyDynamicCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode);
	TInt SetAttachModeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode);
	TInt GetAttachModeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyAttachModeChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode);
	TInt NotifyAttachModeChangeCancel(const TTsyReqHandle aTsyReqHandle);	

	TInt SetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, const TDesC8* aPckg);
	TInt SetDefaultContextParamsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg);
	TInt GetDefaultContextParamsCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetPreferredBearer(const TTsyReqHandle aTsyReqHandle, RPacketService::TPreferredBearer* aBearer);
	TInt SetPreferredBearer(const TTsyReqHandle aTsyReqHandle, RPacketService::TPreferredBearer* aBearer);
	TInt SetPreferredBearerCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetPreferredBearerCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetCurrentReleaseMode(const TTsyReqHandle aTsyReqHandle,RPacketService::TPacketReleaseMode* aMode);
	TInt GetCurrentReleaseModeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyReleaseModeChange(const TTsyReqHandle aTsyReqHandle,RPacketService::TPacketReleaseMode* aMode);
	TInt NotifyReleaseModeChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt EnumerateNifs(const TTsyReqHandle aTsyReqHandle, TInt* aCount);
	TInt EnumerateNifsCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetNifInfo(const TTsyReqHandle aTsyReqHandle, TInt* aIndex, TDes8* aNifInfoV2);
	TInt GetNifInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt EnumerateContextsInNif(const TTsyReqHandle aTsyReqHandle, const TDesC* aExistingContextName, TInt* aCount);
	TInt EnumerateContextsInNifCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetContextNameInNif(const TTsyReqHandle aTsyReqHandle, RPacketService::TContextNameInNif* aContextNameInNif, TDes* aContextName);
	TInt GetContextNameInNifCancel(const TTsyReqHandle aTsyReqHandle);

	TInt PrepareOpenSecondary(const TTsyReqHandle aTsyReqHandle, const TDesC* aOriginalContextName);
	
	TInt DeactivateNIF(const TTsyReqHandle aTsyReqHandle, const TDesC* dataPtr);
	TInt DeactivateNIFCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt NotifyMbmsNetworkServiceStatusChange(const TTsyReqHandle aTsyReqHandle, TMbmsNetworkServiceStatus* aStatus);
	TInt NotifyMbmsNetworkServiceStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetMbmsNetworkServiceStatus(const TTsyReqHandle aTsyReqHandle, TBool* aAttemptAttach,TMbmsNetworkServiceStatus* aStatus);
	TInt GetMbmsNetworkServiceStatusCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt NotifyMbmsServiceAvailabilityChange(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyMbmsServiceAvailabilityChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt UpdateMbmsMonitorServiceListL(const TTsyReqHandle aTsyReqHandle,TMbmsAction* aAction, TDes8* aBuffer = NULL);
	TInt UpdateMbmsMonitorServiceListCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetMbmsMonitoredServicesPhase1(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TUint* aBufSize);
	TInt ProcessGetMbmsMonitoredServicesPhase1L(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TUint* aBufSize);
	TInt GetMbmsMonitoredServicesPhase2(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TDes8* aBufSize);
	TInt GetMbmsMonitoredServicesCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt EnumerateMbmsActiveServiceList(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed);
	TInt EnumerateMbmsActiveServiceListCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt EnumerateMbmsMonitorServiceList(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed);
	TInt EnumerateMbmsMonitorServiceListCancel(const TTsyReqHandle aTsyReqHandle);
protected:
	TInt AddMbmsServicesToMonitorList(const TTsyReqHandle aTsyReqHandle);
	TInt RemoveMbmsServicesFromMonitorList(const TTsyReqHandle aTsyReqHandle);
		
private:

	class CActiveListNode : public CBase
		{
		public:
			CActiveListNode(CActive *aActive, const TTsyReqHandle aTsyReqHandle);
			virtual ~CActiveListNode();
		public:
			CActive *iActive;
			const TTsyReqHandle iTsyReqHandle;
			TSglQueLink iNext;
		};


	TInt FindDelayedReq(const TTsyReqHandle aTsyReqHandle, CActiveListNode *& aNode);
	
private: // data
	TInt iNotifyContextAdded;
	TInt iNotifyStatusChange;
	TInt iNotifyContextActivationRequested;
	TInt iNotifyChangeOfNtwkRegStatus;
	TInt iNotifyMSClassChange;
	TInt iNotifyDynamicCapsChange;
	TInt iAttach;
	TInt iDetach;
	TInt iRejectActivationRequest;
	TInt iGetContextInfo;
	TInt iGetMSClass;
	TInt iSetMSClass;
	TInt iSetPrefBearer;
	TInt iGetPrefBearer;
	TInt iSetAttachMode;
	TInt iGetAttachMode;
	TInt iSetDefaultContextParams;
	TInt iGetDefaultContextParams;
	TInt iGetCurrentReleaseMode;
	TInt iNotifyReleaseModeChange;
	TInt iEnumerateNifs;
	TInt iGetNifInfo;
	TInt iEnumerateContextsInNif;
	TInt iGetContextNameInNif;
	TInt iGetPacketFilterInfo;
	TInt iNotifyAttachModeChange;
	
	TTsyReqHandle iTsyAsyncReqHandle;

	TInt iGprsContextObjectCount;
	TInt iDeactivateNIF;
	RMutex iMutex;
	
	TInt iNotifyMbmsNetworkServiceStatusChange;	
	TInt iGetMbmsNetworkServiceStatus;
	TInt iServiceAvailability;
	TInt iServiceCount;
	TInt iRemoveMbmsServices;
	TInt iMbmsContextObjectCount;
	CPcktMbmsMonitoredServiceList* iList;
	CArrayPtrFlat<CListReadAllAttempt>* iGetMbmsMonitoredServices;
	
	TSglQue<CActiveListNode> iActiveRequestObjects;
	};

class CPhoneDGprsTsy : public CPhoneDummyBase
/**
Dummy Tsy class for Packet phone functionality.
@internalComponent
*/
	{
public:
	static CPhoneDGprsTsy* NewL(CPhoneFactoryDummyBase* aFac);
	CPhoneDGprsTsy(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CPhoneDGprsTsy();
public:
	virtual void Init();
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	static CPhoneDGprsTsy* This(TAny* aPtr);
	};


class CPhoneFactoryDGprsTsy: public CPhoneFactoryDummyBase
/**
Dummy Tsy class for Packet phone factory functionality.
@internalComponent
*/
	{
public:
	CPhoneFactoryDGprsTsy();
	~CPhoneFactoryDGprsTsy();
	virtual CPhoneBase* NewPhoneL(const TDesC& aName);
	virtual TBool IsSupported(const TInt aMixin);
private:
	};

//
//	CDelayedCompleter
//

class CDelayedCompleter : public CTimer
	{
public:
	static CDelayedCompleter *NewL(const TTsyReqHandle aTsyReqHandle, CGprsDGprsTsy *aGprsObject,
		CTelObject *aTelObject);
	void RunL();

protected:
	CDelayedCompleter(const TTsyReqHandle aTsyReqHandle, CGprsDGprsTsy *aGprsObject,
		CTelObject *aTelObject);
	void ConstructL();

protected:
	const TTsyReqHandle iTsyReqHandle;
	CGprsDGprsTsy *iGprsObject;
	CTelObject *iTelObject;
	};
	
class CMbmsContextDMbmsTsy : public CGprsContextDGprsTsy
	{
public:
	static CMbmsContextDMbmsTsy* NewL(CGprsDGprsTsy *aGprs,CPhoneFactoryDummyBase* aFac);
	CMbmsContextDMbmsTsy(CGprsDGprsTsy *aGprs,CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CMbmsContextDMbmsTsy();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
protected:
	TInt GetMbmsSessionsPhase1(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TUint* aBufSize);
	TInt ProcessGetMbmsSessionsPhase1L(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TUint* aBufSize);
	TInt GetMbmsSessionsPhase2(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TDes8* aBufSize);
	TInt GetMbmsSessionsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt UpdateMbmsSessionList(const TTsyReqHandle aTsyReqHandle,TMbmsAction* aAction, TMbmsSessionId* aSessionId = 0);	
	TInt UpdateMbmsSessionListCancel(const TTsyReqHandle aTsyReqHandle);
protected:
	CGprsDGprsTsy *iGprs;
	CArrayPtrFlat<CListReadAllAttempt>* iMbmsSessionList;
	RPacketMbmsContext::CMbmsSession* iMbmsSession;
protected:
	TUint8 iRemoveAllCount;
	};
#endif
	
