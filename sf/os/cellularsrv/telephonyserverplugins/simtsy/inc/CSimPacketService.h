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
// This file contains the definitions of the Simulator TSY Packet Service class.  
// It defines a Simulated packet-switched connection to a packet (GPRS and CDMA) network.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __CSIMPACKETSERVICE_H__
#define __CSIMPACKETSERVICE_H__

#include <et_phone.h>
#include <etelpckt.h>
#include <testconfigfileparser.h>
#include "csimtimer.h"
#include "utils.h"
#include "CSimPubSubChange.h"

#include "CSimPacketContext.h"
#include "CSimMbmsPacketContext.h"
#include "mbmstypes.h"
#include "pcktlist.h"

const TInt KMaxContextPoolSize = 11;
const TInt KGranularity = 2;
const TInt KMaxMbmsContextSupportedByPhone = 7;
const TInt KMaxMbmsContextSupportedByNetwork = 5;
const TInt KMaxMbmsActiveContexts =10;
const TInt KMaxMbmsMonitorContexts =15;

enum TPacketEvent  //< Emum for the state machine
	{
	EPacketEventNone,
	EPacketEventAttach,
	EPacketEventDetach,
	EPacketEventTimeOut
	};


const RPacketService::TStatus KPacketStateTableTerminator=(RPacketService::TStatus)999;

struct TPacketStateChangeValidity			// Enum for packet state
	{
	RPacketService::TStatus iOldState;
	RPacketService::TStatus iNewState;
	TInt iError;
	};

//< This table is used to ensure that the TSY state changes are as accurate as possibly
//< It makes sure that there is no state irregularities and the TSY does not skip any state.
//< Returns KErrNone for a valid change state request (3rd column).
const struct TPacketStateChangeValidity KPacketStateChangeValidity[]= {
	{ RPacketService::EStatusUnattached,	RPacketService::EStatusAttached,	KErrNone },
	
	{ RPacketService::EStatusAttached,		RPacketService::EStatusActive,	KErrNone },
	{ RPacketService::EStatusAttached,		RPacketService::EStatusUnattached,	KErrNone },

	{ RPacketService::EStatusActive,		RPacketService::EStatusSuspended,	KErrNone },
	{ RPacketService::EStatusActive,		RPacketService::EStatusAttached,	KErrNone },
	{ RPacketService::EStatusActive,		RPacketService::EStatusUnattached,	KErrNone },
	
	{ RPacketService::EStatusSuspended,		RPacketService::EStatusUnattached,	KErrNone },
	{ RPacketService::EStatusSuspended,		RPacketService::EStatusAttached,	KErrNone },
	{ RPacketService::EStatusSuspended,		RPacketService::EStatusActive,		KErrNone },

	{ KPacketStateTableTerminator,			KPacketStateTableTerminator,		KPacketStateTableTerminator}
	};

struct TDefaultGPRSConfigParam  //< Used for holding the default GPRS configuration parameters
	{
	TInt	iProtocolType;
	TPtrC8	iGsnAddress;
	TPtrC8	iProtocolAddress;
	TUint	iPdpCompression;
	TInt	iAnonymousAccess;
	TBool	iUseEdge;
	TProtocolConfigOption iProtocolConfigOption;
	};

struct TDefaultR99_R4ConfigParam	//< Used for holding the default Release99/4 configuration parameters
	{
	TInt	iProtocolType;
	TPtrC8	iGsnAddress;
	TPtrC8	iProtocolAddress;
	TBool	iUseEdge;
	TProtocolConfigOption iProtocolConfigOption;
	};

struct TDefaultR5ConfigParam		//< Used for holding the default Release 5 configuration parameters
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


class TConfigData
	{
public:
	static TInt Find(CArrayFixFlat<TConfigData>* aConfigArray,
					 TTsyReqHandle aTsyReqHandle,
					 TInt& aIndex);
	TTsyReqHandle iTsyReqHandle;
	TDes8* iData;
	};


class CSimPhone;
class CSimIncomingContextManager;

class CSimPacketService : public CSubSessionExtBase, public MTimerCallBack, public MPSSimObserver, public MPubSubChangeCallback
	{
	friend CSimPacketContext::~CSimPacketContext();
	friend CSimMbmsPacketContext::~CSimMbmsPacketContext();

public:
	static CSimPacketService* NewL(CSimPhone* aPhone);
	CSimPacketService(CSimPhone* aPhone);
	void ConstructL();
	~CSimPacketService();

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

	CSimPhone* PhonePtr() const;  //< Returns a pointer to the phone object (iPhone)
	TInt ChangeState(RPacketService::TStatus aNewState); //< Changes the state of the packetservice
	const CTestConfigSection* CfgFile(); //< Returns a pointer to a section of the configuration file

	TInt GetCurrentReleaseMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TPacketReleaseMode* aReleaseMode);
	TInt NotifyReleaseModeChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TPacketReleaseMode* aReleaseMode);

	TInt EnumerateNifs(const TTsyReqHandle aTsyReqHandle, TInt& aCount);
	TInt GetNifInfo(const TTsyReqHandle aTsyReqHandle, TInt aIndex, TDes8* aNifInfoV01);
	TInt EnumerateContextsInNif(const TTsyReqHandle aTsyReqHandle, const TDesC* aExistingContextName, TInt& aCount);
	TInt GetContextNameInNif(const TTsyReqHandle aTsyReqHandle, const TDesC* aExistingContextName, TInt aIndex, TDes* aContextName);

	TInt GetCurrentReleaseModeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyReleaseModeChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt EnumerateNifsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetNifInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt EnumerateContextsInNifCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetContextNameInNifCancel(const TTsyReqHandle aTsyReqHandle);
	
	void DecrementContextCount();
	void DecrementMbmsContextCount();
	
	virtual void SimPSEvent(const CSimPubSub::TPubSubProperty aProperty, TInt aStatus);
	virtual void PubSubCallback(TInt aIndex);
private:
	//< The Packet API methods implemented by the TSY
	//< These will be called from the ExtFunc method above
	TInt NotifyContextAdded(const TTsyReqHandle aTsyReqHandle, TDes* aContextId);
	TInt NotifyContextAddedCancel(const TTsyReqHandle aTsyReqHandle);

	TInt Attach(const TTsyReqHandle aTsyReqHandle);	
	TInt AttachCancel(const TTsyReqHandle aTsyReqHandle);	

	TInt Detach(const TTsyReqHandle aTsyReqHandle);	
	TInt DetachCancel(const TTsyReqHandle aTsyReqHandle);	

	TInt DoDetach();

	TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RPacketService::TStatus* aStatus);
	TInt NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RPacketService::TStatus* aStatus);
	TInt NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyContextActivationRequested(const TTsyReqHandle aTsyReqHandle, TDes8* aContextParameters);
	TInt NotifyContextActivationRequestedCancel(const TTsyReqHandle aTsyReqHandle);

	TInt RejectActivationRequest(const TTsyReqHandle aTsyReqHandle);
	TInt RejectActivationRequestCancel(const TTsyReqHandle aTsyReqHandle);

	TInt EnumerateContexts(const TTsyReqHandle aTsyReqHandle,TInt* aCount, TInt* aMaxAllowed);
	TInt EnumerateContextsCancel(const TTsyReqHandle aTsyReqHandle);
	
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
	TInt GetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode);
	TInt NotifyAttachModeChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode);
	TInt NotifyAttachModeChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, const TDesC8* aPckg);
	TInt GetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg);
	TInt GetGPRSDefaultContextParams(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg);
	
	/* MBMS related APIs starts here*/
	
	TInt GetMbmsNetworkServiceStatus(const TTsyReqHandle aTsyReqHandle, TBool aAttemptAttach,TMbmsNetworkServiceStatus* aStatus);
	TInt GetMbmsNetworkServiceStatusCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyMbmsNetworkServiceStatusChange(const TTsyReqHandle aTsyReqHandle, TMbmsNetworkServiceStatus* aStatus);
	TInt NotifyMbmsNetworkServiceStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt NotifyMbmsServiceAvailabilityChange(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyMbmsServiceAvailabilityChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt UpdateMbmsMonitorServiceListL(const TTsyReqHandle aTsyReqHandle,TMbmsAction* aAction, TDes8* aBuffer=NULL);
	TInt UpdateMbmsMonitorServiceListCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt EnumerateMbmsActiveServiceList(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed);
	TInt EnumerateMbmsActiveServiceListCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt EnumerateMbmsMonitorServiceList(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed);
	TInt EnumerateMbmsMonitorServiceListCancel(const TTsyReqHandle aTsyReqHandle);

	/* MBMS related APIs ends here*/
		
	TInt ActionEvent(TPacketEvent aEvent);	//< The state machine for this class
	virtual void TimerCallBack(TInt aId);	//< Timer Callback for the timeout events
	void TimerCallBackPcktRegStatus();
	void TimerCallBackReleaseMode();
	void TimerCallBackNtwkRegStatus();
	void TimerCallBackDeactivate();
	void TimerCallBackDynamicCaps();
	
	void CompleteNtwkRegStatus(RPacketService::TRegistrationStatus aRegistrationStatus);
	
	TInt GetMbmsMonitoredServicesListPhase1(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TInt* aBufSize);
	TInt ProcessGetMbmsMonitoredServicesListPhase1L(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TInt* aBufSize);
	TInt GetMbmsMonitoredServicesListPhase2(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TDes8* aBufSize);
	TInt GetMbmsMonitoredServicesListCancel(const TTsyReqHandle aTsyReqHandle);
	
	
private: // Data
	CSimPhone* iPhone;		//< Pointer to the phone object (The parent of this class)
	TInt iContextCount;		//< Number of contexts currently opened
	TInt iMbmsContextCount;	//< Number of MBMS contexts currently opened
	TInt iMbmsContextNameInc; //< Used for Appending a number to the mbms context name "MBMS_CONTEXT"
	TInt iPrimaryContextNameInc;	//< Used for Appending a number to the packet name "PRIMARY_PACKET_CONTEXT" 
							//< to give each packet object opened a unique name
	TInt iSecondaryContextNameInc;	//< Used for Appending a number to the packet name "SECONDARY_PACKET_CONTEXT" 
							//< to give each packet object opened a unique name

	RPacketService::TStatus iState;		//< Holds the current state of the packet service
	RPacketService::TAttachMode iAttachMode; //< Attach mode for the packet service
	RPacketService::TMSClass iMSClass;  //< We only support class B
	
	TTsyReqHandle iAttachRequestHandle;		//< The request handle for the attach request
	TInt iAttachPause;		//< duration spent in attaching to network state
	TInt iAttachFail;		//< Fail the attach request with this error

	TTsyReqHandle iDetachRequestHandle; //< The request handle for the detach request
	TInt iDetachPause;		//< duration spent in detaching from the network
	TInt iDetachFail;		//< Fail the detach request with this error

	TPacketEvent iCurrentEvent;	//< Holds the current event being processed (allows the tsy to cancel the request)
	TNotifyData	iNotifyStatusChange;	//< Holds the notification data (a flag, TsyReqHandle and any raw data) 
	TNotifyData iNotifyContextAdded;	//< Registers a client interest in being notified of a context being added to the network

	TNotifyData	iNotifyChangeOfNtwk;	//< Registers a client interest in a change of network notification
	TNotifyData	iNotifyMSClassChange;	//< Registers a client interest in a change of MSClass
	
	TNotifyData iNotifyChangeOfReleaseMode;
	TNotifyData iNotifyChangeOfAttachMode;	//< Registers client interest in change of attach mode
	
	TNotifyData iNotifyMbmsNetworkStatusChange; //<Holds the notification data for the changes in the MBMS network status
	TNotifyData iNotifyMbmsServAvailChange; //<Holds the notification data for the changes in the MBMS network service availability
	
	CSimPubSub::TPubSubProperty iNtwkRegStatusChangeProperty;
	CSimPubSub *iNtwkRegStatusChange;
	
	CSimPubSubChange* iMbmsActivation;
	
	TNotifyData iNotifyDynamicCapsChange;	//< Registers client interest in change of Phone capabilities

	CSimTimer* iPcktRegTimer;		//< Timer for the Packet registration
	CSimTimer* iNtwkRegTimer;		//< Timer for the Network registration
	CSimTimer* iReleaseModeTimer;	//< Timer for the Release mode
	CSimTimer* iDynamicCapsTimer;   //< Timer for the Dynamic caps change simulation
	CSimTimer* iTimer;	//< Pointer to the Timer object for callback
	CSimTimer* iMbmsPcktTimer; //<Timer for the MBMS related Network information
	RMutex iMutex;

	struct TPacketRegStatus
		{
		TInt iDuration;				//< Duration of Registration Status
		RPacketService::TStatus iStatus;	//< Registration Status that will be maintained through period iDuration
		};

	struct TNtwkRegStatus
		{
		TInt iDuration;				//< Duration of Registration Status
		RPacketService::TRegistrationStatus iRegStatus;	//< Registration Status that will be maintained through period iDuration
		};

	struct TPacketReleaseMode
		{
		TInt iDuration;
		TInt iReleaseMode;
		};

	struct TPacketDynamicCaps
		{
		TInt iDuration;				//< Duration of dynamic caps value
		RPacketService::TDynamicCapsFlags iDynamicCaps;	//< Dynamic Caps value that will be maintained through period iDuration
		};
	
	struct TMbmsBroadCastParams
		{
		TInt iDuration;
		TMbmsNetworkServiceStatus iMbmsNetworkStatus;
		};
	
	struct TMbmsMonitorListParams
		{
		TInt iDuration;
		TUint16 iMCC;
		TUint16 iMNC;
		TUint iServiceId;
		TMbmsScope iAccessBearer;
		TMbmsServiceMode iServiceMode;
		TMbmsAvailabilityStatus iAvailStatus;
		TBool iIsEntryValid;
		};

	CArrayFixFlat<TPacketRegStatus>* iPcktRegStatusInfo;	//< List of packet registration changes.
	TNotificationType iNtwkRegStatusNotificationType;
	CArrayFixFlat<TNtwkRegStatus>*	 iNtwkRegStatusInfo;	// list of ntwk registration changes.
	CArrayFixFlat<TPacketReleaseMode>* iPacketReleaseModeArray;
	CArrayFixFlat<TPacketDynamicCaps>* iDynamicCapsArray; //< List of dynamic capabilities changes
	CArrayFixFlat<TInt>* iSecondaryContextCountArray;
	CArrayFixFlat<TMbmsBroadCastParams>* iMbmsBroadcastParamsArray; //List of MBMS Broadcast Service related information
	CArrayFixFlat<TMbmsMonitorListParams>* iMbmsMonitorListParamsArray; // List of MBMS Broadcast Monitoring list parameters
	CArrayPtrFlat<CPcktListReadAllAttempt>* iMbmsServicesList; // List containing the MBMS services for 2P retrieval

	TInt iCurrentMbmsParamsIndex;
	TInt iCurrentMbmsMonitorListIndex;
	TInt iCurrentPacketReleaseMode;
	TInt iPacketReleaseModeIndex;
	TInt iCurrentDynamicCaps;
	TInt iDynamicCapsIndex;
	TInt iPcktRegStatIndex;					//< Current index from the iPcktRegStatInfo array
	TInt iCurrentRegStatus;
	TInt iNtwkRegStatIndex;					//< Current index from the iNtwkRegStatInfo array
	TInt iUpdateMbmsServiceError;
	TBool iPcktTimerBool;
	
	TTsyReqHandle iGetMbmsNetworkReqHandle;
	TTsyReqHandle iEnumerateActiveListReqHandle;
	TTsyReqHandle iEnumerateMonitorListReqHandle;
	TTsyReqHandle iUpdateMbmsServiceReqHandle;

	TDefaultGPRSConfigParam iGPRSDefaultConfig;
	TDefaultR99_R4ConfigParam iR99_R4DefaultConfig;
	TDefaultR5ConfigParam iR5DefaultConfig;

	TBool iSecondaryContext;
	TInt PrepareOpenSecondary(const TTsyReqHandle aTsyReqHandle, const TDesC* aOriginalContextName);
	TInt iCurrentPrimaryContextIndex;

	// this class is to enable a Nif to hold and access its contexts and its primary context's name
	
	// The above statement appears to be totally wrong and misleading.
	// It has nothing to do with NIF's, it is just a way of partitioning PDP. 
	// The 1st in the context is the primary context, the next contexts are the secondary contexts.
	class TNifWrapper
	{
		private:
			RPointerArray<CSimPacketContext> iContexts;	// this contains an array of CSimPacketContexts
			TName iPrimaryContextName;				// the primary context name

		public:
			CSimPacketContext* GetContext(TInt aIndex)
			{
			return iContexts[aIndex];										// returns context at specified context
			}; // returns the context at the specified index
			//TArray<CSimPacketContext*> Array(){ return iContexts.Array();};		// returns the array of contexts
			inline TInt NumberOfContexts() {return iContexts.Count();};			// returns number of contexts
			inline void AddContext(CSimPacketContext* aContext) {iContexts.Append(aContext);};	// adds a context to the array
			inline void Close()
				{
				// deletes contexts of nif
				TInt count = iContexts.Count();
				for(TInt i = 0; i<count; i++)
					{
					iContexts[i]->Close();
					}
				iContexts.Close();
				};
	};
	
	class TMbmsNifWrapper
	{
		private:
			RPointerArray<CSimMbmsPacketContext> iContexts;	// this contains an array of CSimMbmsPacketContexts
			TName iContextName;				// the MBMS packet context name

		public:
			inline CSimMbmsPacketContext* GetContext(TInt aIndex);
			inline TInt NumberOfContexts(); 
			inline void AddContext(CSimMbmsPacketContext* aContext);
			inline void Close();
	};

	RArray<TNifWrapper> iNifWrappers;			// array of nifs; each nif holds a primary context and a number of secondary contexts
	RArray<TMbmsNifWrapper> iMbmsNifWrappers; //array of MBMS nifs; each nif holds a MBMS broadcast contexts;
											// shall be used for multicast contexts in future.
	TConfigData iConfigData;
	TInt iMbmsMonitorListCount;  //count of number of Monitor list
			
	CSimIncomingContextManager* iIncomingContextManager;
		// Holds data to do with the incoming contexts.
		// Note all messages come through RPacketService via SimPSEvent
		// This applies to incoming, and network registration.
	
public:
	inline RPacketService::TMSClass MSClass() const {return iMSClass; } //< Returns the Current MSClass object supported
	};

CSimMbmsPacketContext* CSimPacketService::TMbmsNifWrapper::GetContext(TInt aIndex)
	{
	return iContexts[aIndex];					// returns context at specified index
	}; 

TInt CSimPacketService::TMbmsNifWrapper::NumberOfContexts()
	{// returns number of contexts
	return iContexts.Count();
	}

void CSimPacketService::TMbmsNifWrapper::AddContext(CSimMbmsPacketContext* aContext)
	{// adds a context to the array
	iContexts.Append(aContext);
	}

void CSimPacketService::TMbmsNifWrapper::Close()
	{
	// deletes contexts of nif
	TInt count = iContexts.Count();
	for(TInt i = 0; i<count; i++)
		{
		iContexts[i]->Close();
		}
	iContexts.Close();
	}

#endif  //__CSIMPACKETSERVICE_H__
