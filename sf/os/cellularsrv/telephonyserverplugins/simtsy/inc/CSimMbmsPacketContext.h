// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the definitions of the Simulator TSY MBMS Packet Context class.
// 
//

/**
 @file
 @internalAll
*/

#ifndef CSIMMBMSPACKETCONTEXT_H_
#define CSIMMBMSPACKETCONTEXT_H_

#include <et_phone.h>
#include <etelpckt.h>
#include <testconfigfileparser.h>
#include "csimtimer.h"
#include "utils.h"
#include "CSimNtras.h"
#include "CSimPubSubChange.h"
#include "CSimPhone.h"
#include "CSimPacketContext.h"
#include "mbmstypes.h"

class CSimPacketService;

const RPacketContext::TContextStatus KMbmsContextStateTableTerminator=(RPacketContext::TContextStatus)999;

struct TMbmsContextStateChangeValidity
	{
	RPacketContext::TContextStatus iOldState;
	RPacketContext::TContextStatus iNewState;
	TInt iError;
	};

//< This table is used to ensure that the TSY state changes are as accurate as possibly
//< It makes sure that there is no state irregularities and the TSY does not skip any state.
//< Returns KErrNone for a valid change state request (3rd column).
const struct TMbmsContextStateChangeValidity	KMbmsContextStateChangeValidity[]= {
	{ RPacketContext::EStatusInactive,		RPacketContext::EStatusActivating,	KErrNone },
	{ RPacketContext::EStatusInactive,		RPacketContext::EStatusDeleted,	KErrNone },

	{ RPacketContext::EStatusActivating,	RPacketContext::EStatusActive,		KErrNone },
	{ RPacketContext::EStatusActivating,	RPacketContext::EStatusInactive,	KErrNone },
	{ RPacketContext::EStatusActivating,	RPacketContext::EStatusDeleted,	KErrNone },
	
	{ RPacketContext::EStatusActive,		RPacketContext::EStatusDeactivating, KErrNone },
	{ RPacketContext::EStatusActive,		RPacketContext::EStatusSuspended,	KErrNone },
	{ RPacketContext::EStatusActive,		RPacketContext::EStatusDeleted,	KErrNone },
	{ RPacketContext::EStatusActive,		RPacketContext::EStatusReceiving ,	KErrNone },

	{ RPacketContext::EStatusDeactivating,	RPacketContext::EStatusInactive,	KErrNone },
	{ RPacketContext::EStatusDeactivating,	RPacketContext::EStatusActive,		KErrNone },
	{ RPacketContext::EStatusDeactivating,	RPacketContext::EStatusDeleted,	KErrNone },

	{ RPacketContext::EStatusSuspended,		RPacketContext::EStatusDeactivating, KErrNone },
	{ RPacketContext::EStatusSuspended,		RPacketContext::EStatusActive,		KErrNone },
	{ RPacketContext::EStatusSuspended,		RPacketContext::EStatusDeleted,	KErrNone },
	{ RPacketContext::EStatusSuspended,		RPacketContext::EStatusReceiving ,	KErrNone },
	
	{ RPacketContext::EStatusDeleted,		RPacketContext::EStatusInactive,	KErrNone },


	{ KMbmsContextStateTableTerminator,			KMbmsContextStateTableTerminator,		KMbmsContextStateTableTerminator}
	};

enum TMbmsContextEvent //< Enum for the state machine
	{
	EMbmsContextEventNone,
	EMbmsContextEventActivate,
	EMbmsContextEventDeactivate,
	EMbmsContextEventDelete
	};	

struct TMbmsConfigParams
	{
	TUint16 iMCC;
	TUint16 iMNC;
	TUint iServiceId;
	TMbmsScope iAccessBearer;
	TMbmsServiceMode iServiceMode;
	TMbmsServicePriority iServicePriority;
	TUint  iActivateCode;
	};

struct TSetConfigValues
	{
	TInt iErrorValue;  // error value after validation of set config params with configuration params
	TUint iIndex;  // index if the validation succeeds.
	};

class CSimMbmsPacketContext : public CSimPacketContext
	{
public:
	static CSimMbmsPacketContext* NewL(CSimPhone* aPhone, CSimPacketService* aPacketService, const TDesC& aContextName);
	CSimMbmsPacketContext(CSimPhone* aPhone, CSimPacketService* aPacketService, const TName& aContextName);
	void ConstructL();
	~CSimMbmsPacketContext();

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	const CTestConfigSection* CfgFile();
	void Init();
	
	TInt ReActivate(CSimPacketService* aPacketService, const TName& aContextName);
	
public:
	TName ContextName() const;
	inline TInt ContextType() const {return iContextType;  }
	inline RPacketContext::TContextStatus ContextStatus() const {return iState; }
	
private:
	TName iContextName;						//< Current MBMS context name
	TInt iContextType;						//< Current MBMS context's type
	TBool iIsActive;						//< Boolean that holds whether the context is active or not.
	TBool iDeleted;							//< Boolean that holds whether the context is deleted or not.
	TMbmsContextEvent iCurrentEvent;		//< holds the current event taking place.
	TUint iMbmsContextConfigParamsIndex; 	// Index for the array  iMbmsContextConfigParams
	TSetConfigValues iSetConfigValues;		// values after validation of set config params
	
	CSimPhone* iPhone;
	CSimPacketService* iPacketService;		//< Pointer to the parent class
	CSimTimer* iMbmsSetConfigTimer;			//< pointer to the timer object for Set Config operation
	CSimTimer* iMbmsContextTimer;			//< pointer to the timer object for activation and deactivation of the MBMS context
	CSimTimer* iMbmsUpdateSessionTimer;		//< pointer to the timer object for Update Session List
	CSimTimer* iMbmsContextInitTimer;		//< pointer to the timer object for MBMS Context Initialisation
	CSimTimer* iMbmsGetSessionTimer;		//< pointer to the timer object for MBMS Get Session
	
	TTsyReqHandle iActivateRequestHandle;	//< Request handle used for activating a context
	TTsyReqHandle iDeactivateRequestHandle;	//< Request handle used for deactivating a context
	TTsyReqHandle iDeleteRequestHandle;		//< Request handle used to complete a delete request
	TTsyReqHandle iSetConfigRequestHandle;  //< Request handle used to complete a SetConfig request
	TTsyReqHandle iUpdateSessionHandle;		//< Request handle used to complete a UpdateSession request
	TTsyReqHandle iGetMbmsSessionHandle;	//< Request handle used to complete a Get MBMS Session request
	TNotifyData	iNotifyConfigMBMS;			//< Holds the MBMS context config changenotification data (a flag, TsyReqHandle and any raw data)
	TNotifyData	iNotifyStatusChange;		//< Holds the notification data (a flag, TsyReqHandle and any raw data)
	CArrayFixFlat<TUint>* iSessionIdList;   //< List of Session Ids maintained.
	CArrayFixFlat<TMbmsConfigParams>* iMbmsContextConfigParams; // List of MBMS Config Parameters
	
	CArrayPtrFlat<CPcktListReadAllAttempt>* iMbmsSessionList;
	RPacketMbmsContext::CMbmsSession* iMbmsSession;
	TInt iLastError;
	TUint iNumOfSessionId;					//< holds the number of session ids maintained. 
			
	virtual void TimerCallBack(TInt aId);
	
	TInt RandTime(); // Utility function that returns a random time.
	RPacketService::TStatus ConvertToPacketServiceStatus(RPacketContext::TContextStatus aNewState);
	TInt ActionEvent(TMbmsContextEvent aEvent);
	TInt ChangeState(RPacketContext::TContextStatus aNewState); //< Changes the state of the packetcontext

	TInt GetLastErrorCause(const TTsyReqHandle aTsyReqHandle,TInt* aError);
	
	TInt SetConfig(const TTsyReqHandle aTsyReqHandle,const TDesC8* aConfig);
	TInt SetConfigCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetConfig(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig);
	TInt GetConfigCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt Activate(const TTsyReqHandle aTsyReqHandle);
	TInt ActivateCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt Deactivate(const TTsyReqHandle aTsyReqHandle);
	TInt DeactivateCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt Delete(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt NotifyConfigChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig);
	TInt NotifyConfigChangedCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus);
	TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus);
	TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetMbmsSessionsPhase1(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TInt* aBufSize);
	TInt ProcessGetMbmsSessionsPhase1L(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TInt* aBufSize);
	TInt GetMbmsSessionsPhase2(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TDes8* aBufSize);
	TInt GetMbmsSessionsCancel(const TTsyReqHandle aTsyReqHandle);

	TInt UpdateMbmsSessionList(const TTsyReqHandle aTsyReqHandle,TMbmsAction* aAction, TUint* aSessionId);
	TInt UpdateMbmsSessionListCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt InitialiseContext(const TTsyReqHandle aTsyReqHandle, TDes8* aDataChannelV2Pckg); // This method supersedes the LoanCommPort() and RecoverCommPort() methods.
	TInt InitialiseContextCancel(const TTsyReqHandle aTsyReqHandle); // This method supersedes the LoanCommPort() and RecoverCommPort() methods.
	TInt DoInitialiseContext();
	
	};

#endif /*CSIMMBMSPACKETCONTEXT_H_*/
