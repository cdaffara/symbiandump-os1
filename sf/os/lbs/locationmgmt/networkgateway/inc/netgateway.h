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
// Definition of the main objects in the LBS Network Gateway.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LBSNETGATEWAY_H_
#define LBSNETGATEWAY_H_

#include <e32base.h>
#include <e32property.h>
#include <lbs/lbsnetclasstypes.h>
#include "lbscommoninternaldatatypes.h"
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsadmin.h>
#include "lbsprocesssupervisor.h"
#include "agpschannel.h"
#include "netrequestchannel.h"
#include "netlocationchannel.h"
#include "netregstatusmonitor.h"
#include "LbsLogger.h"
#include "LbsExternalLocateLogEvent.h"
#if defined(_DEBUG)
#include "LbsOomListener.h"
#endif
#include "rlbslocmonitorsession.h"
#include "rlbslocmonitorareapositioner.h"
#include "cpsyrequestchannel.h"

class CNetworkRegistrationStatusMonitor;
class CRejectPrivacyRequestCallback;
class CNetObserver;

const TLbsNetSessionId KInvalidNetSessionId = TLbsNetSessionId(TUid::Uid(0x00000001), 0xACACACAC);

class CProtocolModuleInfo : public CBase
	{
	public:
		static CProtocolModuleInfo* NewL(TLbsProtocolModuleId aModuleId, CNetObserver* aObs, CLbsNetworkProtocolBase* aNetworkProtocol, TBool aHome, TBool aRoaming);	
		~CProtocolModuleInfo();
		
		TLbsProtocolModuleId ModuleUid();
		void SetObserver(CNetObserver* aObserver);
		CNetObserver* Observer();
		void SetProtocolModule(CLbsNetworkProtocolBase* aProtocolModule);
		CLbsNetworkProtocolBase* ProtocolModule();
		void SetHome();
		TBool Home();
		void SetRoaming();
		TBool Roaming();

	private:
		CProtocolModuleInfo(TLbsProtocolModuleId aModuleId, CNetObserver* aObs, CLbsNetworkProtocolBase* aNetworkProtocol, TBool aHome, TBool aRoaming);
		void ConstructL();

	private:
		TLbsProtocolModuleId iModuleId;
		CNetObserver* iObs;
		CLbsNetworkProtocolBase* iNetworkProtocol;
		TBool iHome;
    	TBool iRoaming;
	};


/** The main class in the Network Gateway. 

This class acts as the junction point where messages to/from
the network module are translated into messsages sent to/from
the various components of the LBS sub-system.

It contains a number of active objects to service the asynchronous
requests coming from the other LBS components via either
publish & subscribe (RProperty) or a message queue (RMsgQueue).

It also implements the observer interface from the Network Protocol
plugin module in order to receive messages from the network.

*/
class CNetworkGateway : public CBase, 
						public MLbsNetworkProtocolObserver2,
						public MNetworkLocationObserver,
						public MAgpsObserver,
						public MNetworkRequestObserver,
						public MLbsProcessCloseDown,
						public MLbsProcessReset,
						public MNetworkRegistrationStatusObserver,
						public MPsyRequestObserver
	{
	friend class CNetObserver;
	
public:
	static CNetworkGateway* NewL();	
	~CNetworkGateway();

	void SetReceivingProtocolModule(TLbsProtocolModuleId aId);

protected:
	// Process messages coming in from other LBS sub-components
	
	//from MNetworkLocationObserver (NLM)
	void ProcessNetworkLocationMessage(const TLbsNetLocMsgBase& aMessage);
	
	// from MPsyRequestObserver
	void ProcessPsyMessage(const TLbsNetInternalMsgBase& aMessage);
	
	//from MNetworkRequestObserver
	void ProcessNetRequestMessage(const TLbsNetInternalMsgBase& aMessage);
	
	//from MAgpsObserver (from the AGPS manager)
	void OnAssistanceDataRequest(TLbsAsistanceDataGroupInt aDataItemMask);
	void OnSelfLocationRequest(const TLbsNetSessionIdInt& aSessionId, const TLbsNetPosRequestOptionsAssistanceInt& aOptions);
	void OnSelfLocationCancel(const TLbsNetSessionIdInt& aSessionId, TInt aReason);
	void OnSystemStatusAdvice(TBool aTracking);
	
	// from MNetworkRegistrationStatusObserver
	void OnNetworkRegistrationStatusChange(
			RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus aStatus);
	
protected:
	// Process messages coming in from the network
	// (see MLbsNetworkProtocolObserver2)
	void GetCurrentCapabilities(TLbsNetPosCapabilities& aCapabilities) const;

	void ProcessStatusUpdate(TLbsNetProtocolServiceMask aActiveServiceMask);
	
	void ProcessPrivacyRequest(const TLbsNetSessionId& aSessionId,
							   TBool aEmergency, 
							   const TLbsNetPosRequestPrivacy& aPrivacy,
							   const TLbsExternalRequestInfo& aRequestInfo);
	
	void ProcessLocationRequest(const TLbsNetSessionId& aSessionId,
								TBool aEmergency,
								TLbsNetProtocolService aService, 
								const TLbsNetPosRequestQuality& aQuality,
								const TLbsNetPosRequestMethod& aMethod);

	void ProcessSessionComplete(const TLbsNetSessionId& aSessionId,
								TInt  aReason);

	void ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
							   const RLbsAssistanceDataBuilderSet& aData,
							   TInt aReason);
							   
	void ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
								const RLbsAssistanceDataBuilderSet& aData,
								TInt aReason,
								const TLbsNetSessionIdArray& aSessionIdArray);
	
	void ProcessLocationUpdate(const TLbsNetSessionId& aSessionId,
								const TPositionInfoBase& aPosInfo);

protected:
	// Process the close-down request from LbsRoot
	void OnProcessCloseDown();
	
	// Process a (test-only) request to reset the internal state
	// of data buffers, etc. E.g. The assistance data properties.
	void OnProcessReset();
	void ResetAssistanceDataBufferL();
	
		
private:

	CNetworkGateway();
	void ConstructL();

	// Values representing sessions that require module info to be stored. 
	// Keep exactly in this order as we use these values to index into arrays.
	enum _TSessionTypes 
		{
		ESelfLocation =0,
		ENetworkLocation,
		EPSYLocation,
		EX3p,					// last of the SET Initiated
		EMtlrHome,
		EMtlrRoaming,
		EInvalidSession
		};
	typedef TUint32 TSessionTypes;
	
	// Determine the original PM for a session using the given session id.
	CLbsNetworkProtocolBase* FindOriginalPMFromID(const TLbsNetSessionId& aId, TSessionTypes& aType);
	
	void LoadProtocolModuleL(TLbsProtocolModuleId aModuleId);

	CLbsNetworkProtocolBase* DefaultPm();	
	TBool ValidPm(TUid aPmUid);

	CProtocolModuleInfo* ModuleInfo(TLbsProtocolModuleId aUid);
	void DeleteLoadedModules();

	CLbsAdmin::TGpsMode GetAdminGpsMode() const;
	
	TBool IsProtocolModulePermitted(TUid aProtocolUid) const;
	
	void ConvertToTLbsNetPosRequestQualityInt(
			const TLbsNetPosRequestQuality& aSource,
			TLbsNetPosRequestQualityInt& aDest) const;
	void ConvertToTLbsNetPosRequestQuality(
			const TLbsNetPosRequestQualityInt& aSource,
			TLbsNetPosRequestQuality& aDest) const;
	void ConvertToTLbsNetPosRequestOptions(
			const TLbsNetPosRequestOptionsInt& aSource,
			TLbsNetPosRequestOptions& aDest) const;
	void ConvertToTLbsNetPosRequestOptionsAssistance(
			const TLbsNetPosRequestOptionsAssistanceInt& aSource,
			TLbsNetPosRequestOptionsAssistance& aDest) const;
	void ConvertToTLbsNetPosRequestOptionsTechnology(
				const TLbsNetPosRequestOptionsTechnologyInt& aSource,
				TLbsNetPosRequestOptionsTechnology& aDest) const;	
private:
	CLbsAdmin* iAdmin;
	CNetworkLocationChannel* iNetworkLocationChannel;
	CAgpsChannel* iAgpsChannel;
	CNetworkRequestChannel* iNetworkRequestChannel;	
    CPsyRequestChannel* iPSYChannel;
	CLbsCloseDownRequestDetector* iCloseDownRequestDetector;
	CNetworkRegistrationStatusMonitor* iNetworkRegistrationStatusMonitor;
	
	// Logging
	RLbsLogger		  			iLogger;
	CLbsExternalLocateLogEvent*	iExternalLogEvent;
	TBool						iSeenLocationUpdate;
	TBool						iCancelled; // Required as reason still comes into SessionComplete as KErrNone

	// Monitoring home/roaming status
	TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray iHomePmUidArray;
	TInt iHomePmCount;
	
	TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray iRoamingPmUidArray;
	TInt iRoamingPmCount;
	
	CProtocolModuleInfo* iModules[KLbsAdminMaxProtocolModuleIds*2];
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus iRegistrationStatus;
	
	TInt iDefaultHomePmIndex;
	TInt iDefaultRoamingPmIndex;
	
	TUint iPmCount;
	TLbsAdminProtocolModuleLoadingInfo iLoadInfo;

	TLbsProtocolModuleId iHomePmUid;
	TLbsProtocolModuleId iRoamingPmUid;
	
	/** A member used to make callbacks asynchronously */
	CRejectPrivacyRequestCallback* iRejectPrivacyRequest;

    // For all types of session, we store the module that is being used.
    CLbsNetworkProtocolBase* iModuleForSession[EInvalidSession];
    
	TLbsProtocolModuleId iReceivingModuleId;
	#if defined(_DEBUG)
		CLbsOomListener*	iOomListener;
    #endif	
	
    // Session with the location monitor
    RLbsLocMonitorSession   iLocMonSession;     
    // Subsession with the location monitor 
    RLbsLocMonitorAreaPositioner  iLocMonSubSession;  
	};


#endif // LBSNETGATEWAY_H_
