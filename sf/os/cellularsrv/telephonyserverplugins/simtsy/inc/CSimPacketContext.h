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
// This file contains the definitions of the Simulator TSY Packet Context class.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __CSIMPACKETCONTEXT_H__
#define __CSIMPACKETCONTEXT_H__

#include <et_phone.h>
#include <etelpckt.h>
#include <testconfigfileparser.h>
#include "csimtimer.h"
#include "utils.h"
#include "CSimNtras.h"
#include "CSimPubSubChange.h"
#include "CSimPhone.h"
#include <ctsy/serviceapi/cmmutility.h>

const TUint KNotFound=0xfffffff;
const TInt KLimitForErrorCodeForContextGoToInactive=-700;
const TInt KContextGoToInactiveWithNormalErrorCode=-600;
const TInt KContextGoToInactiveWithExtendedErrorCode=-400;
const TInt KContextGoToInactiveWithErrorWhenTryingToGetLastErrorCode = -300;

enum TContextEvent //< Enum for the state machine
	{
	EContextEventNone,
	EContextEventActivate,
	EContextEventDeactivate,
	EContextEventDoDeactivate,
	EContextEventSuspend,
	EContextEventDelete,
	EContextEventNtRas,
	EContextEventTimeOut
	};	

const RPacketContext::TContextStatus KContextStateTableTerminator=(RPacketContext::TContextStatus)999;

struct TContextStateChangeValidity
	{
	RPacketContext::TContextStatus iOldState;
	RPacketContext::TContextStatus iNewState;
	TInt iError;
	};

//< This table is used to ensure that the TSY state changes are as accurate as possibly
//< It makes sure that there is no state irregularities and the TSY does not skip any state.
//< Returns KErrNone for a valid change state request (3rd column).
const struct TContextStateChangeValidity	KContextStateChangeValidity[]= {
	{ RPacketContext::EStatusInactive,		RPacketContext::EStatusActivating,	KErrNone },
	{ RPacketContext::EStatusInactive,		RPacketContext::EStatusDeleted,	KErrNone },

	{ RPacketContext::EStatusActivating,	RPacketContext::EStatusActive,		KErrNone },
	{ RPacketContext::EStatusActivating,	RPacketContext::EStatusInactive,	KErrNone },
	{ RPacketContext::EStatusActivating,	RPacketContext::EStatusDeleted,	KErrNone },
	
	{ RPacketContext::EStatusActive,		RPacketContext::EStatusDeactivating, KErrNone },
	{ RPacketContext::EStatusActive,		RPacketContext::EStatusSuspended,	KErrNone },
	{ RPacketContext::EStatusActive,		RPacketContext::EStatusDeleted,	KErrNone },

	{ RPacketContext::EStatusDeactivating,	RPacketContext::EStatusInactive,	KErrNone },
	{ RPacketContext::EStatusDeactivating,	RPacketContext::EStatusActive,		KErrNone },
	{ RPacketContext::EStatusDeactivating,	RPacketContext::EStatusDeleted,	KErrNone },

	{ RPacketContext::EStatusSuspended,		RPacketContext::EStatusDeactivating, KErrNone },
	{ RPacketContext::EStatusSuspended,		RPacketContext::EStatusActive,		KErrNone },
	{ RPacketContext::EStatusSuspended,		RPacketContext::EStatusDeleted,	KErrNone },

	{ RPacketContext::EStatusDeleted,		RPacketContext::EStatusInactive,	KErrNone },


	{ KContextStateTableTerminator,			KContextStateTableTerminator,		KContextStateTableTerminator}
	};


struct TProtocolConfigOption	// used for authentication parameters 
	{
	TInt	iAuthProtocol;
	TPtrC8	iUsername;
	TPtrC8	iPassword;
	TPtrC8	iChallenge;
	TPtrC8	iResponse;
	TPtrC8	iPrimaryDNS;
	TPtrC8	iSecondaryDNS;
	TUint	iId;
	RPacketContext::TMiscProtocolBuffer iMiscBuffer;
	};

/**
Used for holding valid GPRS/R99/R4/R99/R5 context configuration parameters
*/
struct TContextConfigParam
	{
	TInt	iProtocolType;
	TPtrC8	iGsnAddress;
	TPtrC8	iProtocolAddress;
	TInt	iAnonymousAccess;
	TBool	iUseEdge;
	TProtocolConfigOption iProtocolConfigOption;
	TUint	iPdpHeaderCompression;
	TUint	iPdpDataCompression;
	};

struct TConfigFail		// Used for failing SetConfig, DeleteTFT, CreateTFT, AddPacketFilter call
	{
	TName iContextName;
	TInt32 iNumberOfTimes;
	TInt32 iFailErrorCode;
	};
	
struct TTftConfig
	{
	TName iContextName;
	TInt32 iNumberOfFailures;
	TInt32 iFailErrorCode;
	TInt32 iDelay;
	};

struct TRel99ContextConfig		// Used holding the Rel99 configuration parameters
	{
		TName iContextName;
		TUint iActivatePause;
		TInt iActivateErrorCode;
		TUint iDeactivatePause;
		TUint iDeactivateErrorCode;
		TUint iDeletionPause;
		TUint iDeletionErrorCode;
		TBool iConnectToNtRas;
	};

struct TNetworkRegStatus			// Used for holding the network registration
	{
		TInt iDuration;				
		RPacketService::TRegistrationStatus iRegStatus;
	};

struct TNotifyContextStatusChange		// Used for notifying context status
	{
		TName iContextName;
		TInt  iDuration;
		RPacketContext::TContextStatus iStatus;
	};

struct TPacketFilterV3					// Used for holding packet filter configuration
	{
		TInt iId;							//< Packet filter identifier. Value range: 1 - 8
		TInt iEvaluationPrecedenceIndex;	//< Evaluation Precedence Index. Value range: 255 (lowest) - 0 (highest)
		TUint8 iSrcAddr[RPacketContext::KIPAddressSize];				//< Domain Source address
		TUint8 iSrcAddrSubnetMask[RPacketContext::KIPAddressSize];		//< Domain Subnet mask
		TInt iProtocolNumberOrNextHeader;	//< Protocol number (IPv4) or Next Header (IPv6) field of incoming packets. Value range 0 - 255
		TInt iSrcPortMin;					//< Minimum source port number of incoming packets. Value range 0 - 65 535
		TInt iSrcPortMax;					//< Maximum source port number of incoming packets. Value range 0 - 65 535
		TInt iDestPortMin;					//< Lowest destination port number of incoming packets. Value range 0 - 65 535
		TInt iDestPortMax;					//< Maximum destination port number of incoming packets. Value range 0 - 65 535
		TUint32 iIPSecSPI;					//< Security Parameter Index, 32-bit field.
		TUint16 iTOSorTrafficClass;			//< Type of Service (IPv4) or Traffic Class (IPv6). 1 octet TOS/Traffic class + 1 octet TOS/Traffic class Mask.
		TUint32 iFlowLabel;		
		TBool iAdded;
		RPacketContext::TPacketFilterType iFilterType;
	};

struct TAuthorizationTokenResponse  //< Used for SIMTSY responses for authorization tokens
	{
	RPacketContext::TAuthorizationToken iAuthorizationToken;
	TInt iResponse;
	};

/**
 * This class is used to hold the details of batch read requests between the first and
 * second phases of a "two phase read".  During the first phase, an instance of this class
 * is created containing a unique identifier to the request and the resulting information.
 * Once the second phase operation has identified this instance from a list and completed
 * the client request, this instance will be deleted.
 */
class CPcktListReadAllAttempt : public CBase
	{
public:
	static CPcktListReadAllAttempt* NewL(TClientId& aId, TTsyReqHandle aReqHandle);
	~CPcktListReadAllAttempt();

protected:
	CPcktListReadAllAttempt(TClientId& aId, TTsyReqHandle aReqHandle);

private:
	void ConstructL();

public:
	CBufBase* iListBuf;					// < The result of the read operation.
	TClientId iClient;					// < A unique identifier of the read request.
	TTsyReqHandle iReqHandle;			// < The first phase request handle.
	};


class CSimPacketService;
class CSimPacketQoS;
class CATDialGPRSData;
class CSimPacketContext : public CSubSessionExtBase, public MTimerCallBack, public MNTRasCallBack, public MPubSubChangeCallback
	{
public:
	static CSimPacketContext* NewL(CSimPhone* aPhone, CSimPacketService* aPacketService, const TDesC& aContextName);
	CSimPacketContext(CSimPhone* aPhone, CSimPacketService* aPacketService, const TName& aContextName);
	void ConstructL();
	~CSimPacketContext();

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

	TInt AttemptDeactivateImmediately();
	
	TInt ChangeState(RPacketContext::TContextStatus aNewState); //< Changes the state of the packetcontext
	const CTestConfigSection* CfgFile();
	TInt ReActivate(CSimPacketService* aPacketService, const TName& aContextName);

	void ConstructPacketFiltersL();

	void PubSubCallback(TInt aId);

private:
	TInt GetProfileName(const TTsyReqHandle aTsyReqHandle, TName* aQoSProfile);

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

	IMPORT_C static TInt OpenNewSecondaryContext(CSimPacketService& aPacketService, 
												const TDesC& anExistingContextName,
												TDes& aNewContextName);

	TInt CreateNewTFT(const TTsyReqHandle aTsyReqHandle, const TInt* aSize);
	TInt DoCreateNewTFT();
	TInt DeleteTFT(const TTsyReqHandle aTsyReqHandle);
	TInt EnumeratePacketFilters(const TTsyReqHandle aTsyReqHandle, TInt& aCount);
	TInt GetPacketFilterInfoL(const TTsyReqHandle aTsyReqHandle, TInt aIndex, TDes8* aPacketFilterInfo);
	TInt AddPacketFilter(const TTsyReqHandle aTsyReqHandle, const TDesC8* aPacketFilterInfo);
	TInt RemovePacketFilter(const TTsyReqHandle aTsyReqHandle, TInt aId);
	TInt ModifyActiveContext(const TTsyReqHandle aTsyReqHandle);
	TInt InitialiseContext(const TTsyReqHandle aTsyReqHandle, TDes8* aDataChannelV2Pckg); // This method supersedes the LoanCommPort() and RecoverCommPort() methods.
	TInt DoInitialiseContext();
	TInt GetDnsInfo(const TTsyReqHandle aTsyReqHandle, const TDesC8* aDnsInfo);			// retrieves primary and secondary DNS server names
	TInt GetAddMediaAuthorization(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt GetRemoveMediaAuthorization(const TTsyReqHandle aTsyReqHandle, RPacketContext::TAuthorizationToken* aAuthorizationToken);
	TInt GetAddMediaAuthorizationCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetRemoveMediaAuthorizationCancel(const TTsyReqHandle aTsyReqHandle);

	TInt CreateNewTFTCancel(const TTsyReqHandle aTsyReqHandle);
	TInt DeleteTFTCancel(const TTsyReqHandle aTsyReqHandle);
	TInt EnumeratePacketFiltersCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetPacketFilterInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt AddPacketFilterCancel(const TTsyReqHandle aTsyReqHandle);
	TInt RemovePacketFilterCancel(const TTsyReqHandle aTsyReqHandle);
	TInt ModifyActiveContextCancel(const TTsyReqHandle aTsyReqHandle);
	TInt InitialiseContextCancel(const TTsyReqHandle aTsyReqHandle); // This method supersedes the LoanCommPort() and RecoverCommPort() methods.
	TInt GetDnsInfoCancel(const TTsyReqHandle aTsyReqHandle);
	
	void TimerCallBackNotifyContextStatusChange();

	void CompleteContextStatusChange(RPacketContext::TContextStatus& aStatus);
	CSimPubSubChange *iContextStatusChange;

private:

	CSimPhone* iPhone;

	TBool iDeleted;		//< Used to indicate whether this context is deleted or not
	TBool iIsActive;	//< Used to indicate whether this context is active as only one 
						//< context can be active at any one time.

	CSimPacketService* iPacketService;	//< Pointer to the parent class
	CSimPacketQoS* iPacketqos;			//< Pointer to the qos associated with this context
	CSimNtRas* iNtRas;					//< pointer to the NtRas class that uses ntras to connect to network
	TName iContextName;					//< Current Packet context name
	TName iQoSName;
	CSimTimer* iTimer;					//< pointer to the timer object
	CSimTimer* iNotifyContextStatusChangeTimer;
	CSimTimer* iNotifyContextConfigChangeTimer;
	CSimTimer* iSetConfigTimer;
	CSimTimer* iInitContextTimer;
	CSimTimer* iCreateTftTimer;

	TNotifyData	iNotifyStatusChange;					//< Holds the notification data (a flag, TsyReqHandle and any raw data) 
	CArrayFixFlat<TContextConfigParam>* iContextConfigParams;	//< Pointer to a list of valid Context (GPRS/R99/R5) config parameters

	CArrayFixFlat<TRel99ContextConfig>* iContextConfigsRel99;
	TBool iTFTCreated;
	TInt iNumTFTsCreated;
	TInt iNumFiltersAdded;
	CArrayFixFlat<TPacketFilterV3>* iPacketFilterInfoArray;
	TInt GetContextStatusChangeCount();
	TNotifyContextStatusChange& GetContextStatusChangeAt(TInt aIndex);
	CArrayFixFlat<TNotifyContextStatusChange>* iNotifyContextStatusChangeArray;
	CArrayFixFlat<TAuthorizationTokenResponse>* iAuthorizationTokenResponse; //< Holds tokens and desired responses for SIMTSY to report
	CArrayFixFlat<RPacketContext::CTFTMediaAuthorizationV3>* iMediaAuthorization; //< Contains authorization token and flow Ids for verification

	TInt iContextConfigParamsIndex;						//< index in iContextConfigParams


	TNotifyData	iNotifyConfigGPRS;		//< Holds the GPRS context config change notification data (a flag, TsyReqHandle and any raw data) 
	TNotifyData	iNotifyConfigR99;		//< Holds the R99 context config changenotification data (a flag, TsyReqHandle and any raw data)
	TNotifyData	iNotifyConfigR5;		//< Holds the R5 context config changenotification data (a flag, TsyReqHandle and any raw data)
	
	TTsyReqHandle iActivateRequestHandle;	//< Request handle used for activating a context
	TTsyReqHandle iDeactivateRequestHandle;	//< Request handle used for deactivating a context
	TTsyReqHandle iDeleteRequestHandle;		//< Request handle used to complete a delete request
	TBool iCommPortLoaned;		//< A flag indicating whether the CommPort is loaned.
	TContextEvent iCurrentEvent;			//< holds the current event taking place. 

	TInt iActivatePause;		//< duration spent in activating the context
	TInt iActivateFail;			//< Fail the activate request with this error
	TInt iDeactivatePause;		//< duration spent in deactivating the context
	TInt iDeactivateFail;		//< Fail the deactivate request with this error
	TInt iDeletePause;			//< duration spent in deleting the context
	TInt iDeleteFail;			//< Fail the delete request with this error

	TInt iQoSObjectCount;		//< used to append a number to the QoS object created

	TConfigFail iSetConfigFail;
	TConfigFail iDeleteTftFail;
	TTftConfig iCreateTftConfig;
	TConfigFail iAddPacketFilter;
	
	TInt iSetConfigDelay;
	
	class TNotifyContextConfigChangeData
		{
		public:
		TInt iDelay;
		TInt iNewContextConfigIndex;
		};
	CArrayFixFlat<TNotifyContextConfigChangeData> *iNotifyContextConfigChangeArray;
	
	class TSetConfigData
		{
		public:
		static TInt Find(CArrayFixFlat<TSetConfigData>* aSetConfigArray, TTsyReqHandle aTsyReqHandle, TInt& aIndex);
		TTsyReqHandle iTsyReqHandle;
		TDesC8* iData;
		};
	CArrayFixFlat<TSetConfigData> *iSetConfigData;

	class TCreateTftData
		{
		public:
		TTsyReqHandle iTsyReqHandle;
		TInt iSize;
		};
	TCreateTftData iCreateTftData;

	TInt iSetConfigCallCount;
	TInt iContextType;
	TBool iTFTChangeBool;
	TBool iCommReset;
	TInt iNotifyContextStatusChangeIndex;
	RPacketContext::TContextStatus iCurrentNotifyContextStatusChange;
	TNotificationType iNotifyContextStatusChangeType;

private:
	virtual void NTRasCallBack(TInt aStatus);
	TInt ActionEvent(TContextEvent aEvent,TInt aStatus);
	RPacketService::TStatus ConvertToPacketServiceStatus(RPacketContext::TContextStatus aNewState);

	TInt DoActivate();		//< this is where we try to connect to a network using ntras
	TInt DoDeactivate();	//< deactivates the context by clossing the comm port
	TInt DoDelete();		//< deletes the context

	void GetCommSetupSettingsL();
	void GetContextConfigParamSettingsL(TPtrC8 aTag);
	void GetActivateSettings();
	void GetSetConfigSettings();
	void GetPacketFilterSettings();
	void GetContextConfigRel99SettingsL();
	void GetContextStatusChangeSettingsL();
	void GetContextConfigChangeSettingsL();

public:
	TName ContextName() const;
	inline TInt ContextType() const {return iContextType;  }
	inline RPacketContext::TContextStatus ContextStatus() const {return iState; }
	
protected:
	
	RPacketContext::TContextStatus iState;				//< Holds the current state of the packet context

	class TCommSetupItem
		{
		public:
			TFileName iCsyName;					//< The CSY name used for comm communication
			TCommConfigV01 iConfig;				//< The configuration to be used with the NTRas port.
			TName iPortName;					//< The port to be used for read / write data read from the config file.
			TName iContextName;
			TBool iCommReset;
			TInt iNumInitializeFailures;
			TInt iErrorCode;
			TInt iDelay;
		};

	class TInitContextData
		{
		public:
		TTsyReqHandle iTsyReqHandle;
		TDesC8* iChannel;
		};
	TInitContextData iInitContextData;
	
	CArrayFixFlat<TCommSetupItem>* iCommSetupArray;
	virtual void TimerCallBack(TInt aId);
	TCommSetupItem FindCommSettings();

private:
    TInt iLastError;
    TInt iErrorCodeForGetLastErrorCause;
	};

#endif
