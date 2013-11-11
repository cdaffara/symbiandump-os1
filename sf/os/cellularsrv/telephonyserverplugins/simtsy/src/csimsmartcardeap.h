// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __CSIMSMARTCARDEAP_H__
#define __CSIMSMARTCARDEAP_H__

#include <et_phone.h>
#include "etelmm.h"
#include "CSimPhone.h"

class CSimSmartCardEap;

/**
Class to hold one challenge and its response ptrs, as read from
the config.txt file.
*/
class TEapChngResp
	{
public:
	TEapChngResp();

public:
	/** Challenge request data parsed from the config.txt. */
	HBufC8* iChallenge;
	/** Challenge response data parsed from the config.txt. */
	HBufC8* iResp;
	/**
	The authentication status after this challenge has completed.
	*/
	RMobileSmartCardEap::TEapAuthStatus iAuthStatus;
	};

/**
class to hold the parsed data of an EAP procedure from the
config.txt file.
*/
class TEapProcedureData
	{
public:
	TEapProcedureData();

public:
	/**
	holds Application's ID, on which the authentication is to be
	carried out.  Note, there is no checking for this AID's
	existence, config file must simply match client's request data.
	*/
	TBuf8<RMobilePhone::KAIDSize*4> iAID;
	/** holds the Eap method type to be used with this procedure */
	RMobileSmartCardEap::TEapType iEapType;
	/** holds the Eap key to be returned at end of this procedure */
	HBufC8* iEapKey; // MSK
	HBufC8* iEapExtKey; // EMSK
	/** holds the Eap identity to be returned at beginning of this procedure */
	HBufC8* iEapId; // Permanent
	HBufC8* iEapPsId; // Pseudonym
	/** holds a list of challenge requests and responses */
	RArray<TEapChngResp> iChResp;
	};

/**
Holds data to help manage access to the smart card applications with
EAP capability.
*/
class CSimSmartCardEapManager : public CBase
	{
public:
	static CSimSmartCardEapManager* NewL(CSimPhone *aPhone);
	CSimSmartCardEapManager(CSimPhone *aPhone);
	void ConstructL();
	~CSimSmartCardEapManager();

	CTelObject* CreateScEapSubSessionL(RMobilePhone::TAID& aAID, RMobileSmartCardEap::TEapType& aEapType);
	TEapProcedureData* ProcData(const RMobilePhone::TAID& aAID, const RMobileSmartCardEap::TEapType& aEapType);
	void ProcDataUseCompleted(const TEapProcedureData* aProcData);

	void RegisterSubSessionL(CSimSmartCardEap* aToRegister);
	TInt DeRegisterSubSession(const CSimSmartCardEap* aToRegister);

private:
	const CTestConfigSection* CfgFile();
	void ParseEapInfoL();
	void ClearParsedData();
	void AID_EapType_ExistsInConfigL(const RMobilePhone::TAID& aAID, const RMobileSmartCardEap::TEapType& aEapType);

private:
	CSimPhone *iPhone;

	/**
	holds the data parsed from the config file on the EAP procedures
	configured.
	*/
	RArray<TEapProcedureData> iEapProcData;
	/**
	We must use an array of flags to specify which of the procedures in
	iEapProcData has been used up.  The reason is that we cannot delete
	from iEapProcData, doing so will cause a mismatch of pointers
	between the iEapProcData items and the pointers held by the
	CSimSmartCardEap objects.
	*/
	RArray<TBool> iDiscardedProcedure;
	/**
	Holds a record of all EAP sub-session (CSimSmartCardEap) objects.
	*/
	RPointerArray<CSimSmartCardEap> iSubSessionObjs;
	};


class CSimSmartCardEap : public CSubSessionExtBase
	{
class CThreadTerminationListener;
friend class CThreadTerminationListener;

public:
	static CSimSmartCardEap* NewL(CSimPhone *aPhone, CSimSmartCardEapManager* aEapMan, RMobilePhone::TAID& aAID, RMobileSmartCardEap::TEapType& aEapType);
	CSimSmartCardEap(CSimPhone *aPhone, RMobilePhone::TAID& aAID, RMobileSmartCardEap::TEapType& aEapType);
	void ConstructL(CSimSmartCardEapManager* aEapMan);
	~CSimSmartCardEap();

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual void Init();

private:
	TInt SimInitialiseEapMethod(const TTsyReqHandle aTsyReqHandle, TThreadId* aThreadId);
	TInt SimInitialiseEapMethodCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SimGetUserIdentity(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapUserIdType* aEapIdType, TDes8* aUserId);
	TInt SimGetUserIdentityCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SimGetAuthenticationStatus(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapAuthStatus* aAuthStatus);
	TInt SimGetAuthenticationStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SimGetEapKey(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapKeyTag* aEapKeyTag, TDes8* aKey);
	TInt SimGetEapKeyCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SimSetAuthenticateDataForPhase1(const TTsyReqHandle aTsyReqHandle, TDes8* aEapAuthData, TInt* aPhase1Size);
	TInt SimGetAuthenticateDataForPhase2(const TTsyReqHandle aTsyReqHandle, TDes8* aEapAuthData, TDes8* aPhase2Resp);
	TInt SimSmartCardEapAuthenticationCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SimReleaseEapMethod(const TTsyReqHandle aTsyReqHandle);
	TInt SimGetEapMethodAccessStatus(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapMethodAccessStatus* aEapState);
	TInt SimNotifyEapMethodAccessStatusChange(const TTsyReqHandle aTsyReqHandle, RMobileSmartCardEap::TEapMethodAccessStatus* aEapState);
	TInt SimNotifyEapMethodAccessStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	void SimCompleteNotifyEapMethodAccessStatusChange();
	virtual RHandleBase* GlobalKernelObjectHandle();
	void ClientHasTerminated(TInt aExitReason);

private:
	/**
	Listener class for client thread termination.  Used to ensure
	iSemaphr is signalled in the event that a client dies unnaturally.
	*/
	class CThreadTerminationListener : public CActive
		{
	public:
		static CThreadTerminationListener* NewL(CSimSmartCardEap* aSubSess, const TThreadId& aId);
		void Start();
		~CThreadTerminationListener();

	private:
		CThreadTerminationListener(CSimSmartCardEap* aPhone);
		void ConstructL(const TThreadId& aId);
		void DoCancel();
		void RunL();

	private:
		/** The owner sub-session object. */
		CSimSmartCardEap* iSubSess;
		RThread iCliThread;
		};

	/**
	Basic structure for holding the notification information for a
	SimCompleteNotifyEapMethodAccessStatusChange() call.

	@see CSimSmartCardEap::iEapAccessNotifyData
	*/
	struct TNotifyData
		{
		TBool iNotifyPending;
		TTsyReqHandle iNotifyHandle;
		TAny* iNotifyData;
		};

private:
	/** Simtsy's RMobilePhone server object. */
	CSimPhone *iPhone;
	RMobilePhone::TAID iAID;
	RMobileSmartCardEap::TEapType iEapType;
	/**
	The procedure data for this sub-session, which is parsed by iEapMan
	and retieved by this object during SimInitialiseEapMethod().
	*/
	TEapProcedureData* iProcedureData;
	/**
	Flag indicating whether the sub-session has already been
	intialised.
	*/
	TBool iSSInitialised;
	/**
	The semaphore used by the client side RMobileSmartCardEap code to
	ensure single access to this sub-session.
	*/
	RSemaphore iSemaphr;
	/**	The access of this sub-session.	*/
	RMobileSmartCardEap::TEapMethodAccessStatus iAccessStatus;
	/**
	The authentication status of the EAP procedure running within this
	sub-session.
	*/
	RMobileSmartCardEap::TEapAuthStatus iAuthStatus;
	/** Used in the SimNotifyEapMethodAccessStatusChange() request. */
	TNotifyData iEapAccessNotifyData;
	/**
	This object calls back into ClientHasTerminated() when the client
	thread dies.
	*/
	CThreadTerminationListener* iCliTerminationListener;
	/**
	The EAP manager object to which this sub-session is linked.  This
	manager object should be one in simtsy and it is the object that
	created this sub-session CSimSmartCardEap object (for CSimPhone).
	*/
	CSimSmartCardEapManager* iEapMan;
	/**
	iProcedureData contains an RArray of EAP challenge/response data,
	parsed from the config.txt.  This integer keeps track of which
	challenge should be used in the next call to
	SimSetAuthenticateDataForPhase1().
	*/
	TInt iCurrentChallenge;
	};

#endif // __CSIMSMARTCARDEAP_H__
