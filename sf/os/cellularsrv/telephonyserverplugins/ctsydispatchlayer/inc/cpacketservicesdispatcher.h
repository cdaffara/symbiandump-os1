// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CPACKETSERVICESDISPATCHER_H_
#define __CPACKETSERVICESDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>
#include <etelpckt.h>
#include <etelqos.h>
#include <ctsy/serviceapi/cmmutility.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>
#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"

const TInt KPrimaryAndSecondaryDNSLength = 16;
const TUint KPasswordUsernameHeaderLength = 7;
const TUint KIPCRequestCode = 1;
const TUint KRequestID = 0;
const TUint KIDLength = 3;
const TUint KMaxUserAndPassLength = 80;
const TUint KMaxPdpAddressLength = 50;
const TUint KPrimaryAndSecondaryDNSID = 0xC023;
const TUint KUsenamePasswordDNSID = 0x8021;
const TUint KIndicatePrimaryAddress = 0x81;
const TUint KIndicateSecondaryAddress = 0x83;
const TUint KPacketLengthMSB = 0x0;
const TUint KDnsAddressLength = 6;
_LIT(KEmptyInfoName, "");

const TInt KHeaderGap = 8; 	// Offsets for decoding PCO buffer
const TInt KIPv4Len = 4;		// Offsets for decoding PCO buffer
const TInt KLengthLocation = 4;// Offsets for decoding PCO buffer
const TInt KMinPCOSize = 3;    // Minimum PCO buffer size as set in 3G TS 24.008 spec

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;

class MLtsyDispatchPacketServicesPacketAttach;
class MLtsyDispatchPacketServicesGetPacketAttachMode;
class MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus;
class MLtsyDispatchPacketServicesPacketDetach;
class MLtsyDispatchPacketServicesSetPdpContextConfig;
class MLtsyDispatchPacketServicesModifyActivePdpContext;
class MLtsyDispatchPacketServicesInitialisePdpContext;
class MLtsyDispatchPacketServicesDeletePdpContext;
class MLtsyDispatchPacketServicesSetPacketAttachMode;
class MLtsyDispatchPacketServicesNotifyPacketStatusChange;
class MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams;
class MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params;
class MLtsyDispatchPacketServicesActivatePdpContext;
class MLtsyDispatchPacketServicesSetPdpContextQosGprs;
class MLtsyDispatchPacketServicesSetPdpContextQosR99R4;
class MLtsyDispatchPacketServicesSetPdpContextQosR5;
class MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest;
class MLtsyDispatchPacketServicesDeactivatePdpContext;
class MLtsyDispatchPacketServicesAddPacketFilter;
class MLtsyDispatchPacketServicesGetStatus;
class MLtsyDispatchPacketServicesGetStaticCapabilities;
class MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists;
class MLtsyDispatchPacketServicesGetMaxNoActiveServices;
class MLtsyDispatchPacketServicesInitialiseMbmsContext;
class MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus;
class MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList;
class MLtsyDispatchPacketServicesUpdateMbmsSessionList;
class MLtsyDispatchPacketServicesRemovePacketFilter;

// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to PacketServices related requests to the Licensee LTSY.
 */
class CPacketServicesDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CPacketServicesDispatcher();

	static CPacketServicesDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);

	static CPacketServicesDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);

	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY

	TInt DispatchPacketAttachL();
	TInt DispatchGetPacketAttachModeL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetStatusL();
	TInt DispatchGetPacketNetworkRegistrationStatusL();
	TInt DispatchPacketDetachL();
	TInt DispatchSetPdpContextConfigL(const CMmDataPackage* aDataPackage);
	TInt DispatchModifyActivePdpContextL(const CMmDataPackage* aDataPackage);
	TInt DispatchInitialisePdpContextL(const CMmDataPackage* aDataPackage);
	TInt DispatchDeletePdpContextL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetPacketAttachModeL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetDefaultPdpContextParamsL(const CMmDataPackage* aDataPackage);
	TInt DispatchActivatePdpContextL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetPdpContextQosL(const CMmDataPackage* aDataPackage);
	TInt DispatchRejectNetworkInitiatedContextActivationRequestL();
	TInt DispatchDeactivatePdpContextL(const CMmDataPackage* aDataPackage);
	TInt DispatchAddPacketFilterL(const CMmDataPackage* aDataPackage);
	TInt DispatchRemovePacketFilterL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetStaticCapabilitiesL();
	TInt DispatchGetMaxNoMonitoredServiceListsL();
	TInt DispatchGetMaxNoActiveServicesL();
	TInt DispatchInitialiseMbmsContextL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetMbmsNetworkServiceStatusL(const CMmDataPackage* aDataPackage);
	TInt DispatchUpdateMbmsMonitorServiceListL(const CMmDataPackage* aDataPackage);
	TInt DispatchUpdateMbmsSessionListL(const CMmDataPackage* aDataPackage);

	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.

	void CallbackNotifyPacketAttachModeChange(TInt aError, RPacketService::TPreferredBearer aBearer);
	void CallbackNotifyQosProfileChange(TInt aError, const TDesC& aContextName, const RPacketQoS::TQoSGPRSNegotiated& aQoSGPRSNegotiated);
	void CallbackNotifyQosProfileChange(TInt aError, const TDesC& aContextName, const RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated);
	void CallbackNotifyQosProfileChange(TInt aError, const TDesC& aContextName, const RPacketQoS::TQoSR5Negotiated& aQoSR5Negotiated);
	void CallbackNotifyRatTransferCapsChange(TInt aError, TDynamicTransferCapsFlags aCaps);
	void CallbackNotifyPacketMsClassChange(TInt aError, RPacketService::TMSClass aMSClass);
	void CallbackNotifyPacketDynamicCapsChange(TInt aError, TBool aGprsIsSupportedOnCell, TBool aCsServicesIsSupportedOnCell);
	void CallbackNotifyPdpContextAdded(TInt aError, const TDesC& aContextName);
	void CallbackNotifyNetworkInitiatedContextActivationRequest(TInt aError, const RPacketContext::TContextConfigGPRS& aContextConfigGPRS);
	void CallbackNotifyNetworkInitiatedContextActivationRequest(TInt aError, const RPacketContext::TContextConfigR99_R4& aContextConfigR99_R4);
	void CallbackNotifyNetworkInitiatedContextActivationRequest(TInt aError, const RPacketContext::TContextConfig_R5& aContextConfig_R5);				
	void CallbackNotifyPdpContextConfigChanged(TInt aError, const TDesC& aContextName, const TDesC8& aAccessPointName, const TDesC8& aPdpAddress, TUint aPdpCompression, const TDesC8& aPcoBuffer);
	void CallbackNotifyPdpContextStatusChange(TInt aError, const TDesC& aContextName, const TContextMisc& aContextMisc);
	void CallbackNotifyPdpContextConnectionSpeedChange(TInt aError, const TDesC& aContextName, TUint aConnectionSpeed);
	void CallbackNotifyPacketNetworkRegistrationStatus(TInt aError, const RPacketService::TRegistrationStatus aRegistrationStatus);
	void CallbackNotifyMbmsContextConfigChanged(TInt aError, const TDesC& aContextName, const RPacketMbmsContext::TContextConfigMbmsV1& aContextConfigMbms);
	void CallbackPacketAttach(TInt aError);
	void CallbackGetPacketAttachMode(TInt aError, RPacketService::TAttachMode aAttachMode);
	void CallbackGetPacketNetworkRegistrationStatus(TInt aError, RPacketService::TRegistrationStatus aRegistrationStatus);
	void CallbackPacketDetach(TInt aError);
	void CallbackSetPdpContextConfig(TInt aError, const TDesC& aContextName);
	void CallbackModifyActivePdpContext(TInt aError, const TDesC& aContextName, TInt8 aRejectionCode);
	void CallbackInitialisePdpContext(TInt aError, const TDesC& aContextName, const TDesC& aChannelId);
	void CallbackDeletePdpContext(TInt aError, const TDesC& aContextName);
	void CallbackSetPacketAttachMode(TInt aError);
	void CallbackNotifyPacketStatusChange(TInt aError);
	void CallbackSetDefaultPdpContextParams(TInt aError);
	void CallbackActivatePdpContext(TInt aError, const TDesC& aContextName, const TDesC8& aAccessPointName, RPacketContext::TProtocolType aPdpType);
	void CallbackSetPdpContextQos(TInt aError, const TDesC& aContextName);
	void CallbackRejectNetworkInitiatedContextActivationRequest(TInt aError);
	void CallbackDeactivatePdpContext(TInt aError, const TDesC& aContextName);
	void CallbackGetStatus(TInt aError, RPacketService::TStatus aPacketStatus, TBool aIsResumed);
	void CallbackGetStaticCapabilities(TInt aError, TUint aStaticCapabilities);
	void CallbackGetMaxNoMonitoredServiceLists(TInt aError, TInt aMaxNoMonitoredServiceLists);
	void CallbackGetMaxNoActiveServices(TInt aError, TInt aMaxNoActiveServices);
	void CallbackInitialiseMbmsContext(TInt aError, const TDesC& aContextName, const TDesC& aChannelId);
	void CallbackGetMbmsNetworkServiceStatus(TInt aError, TMbmsNetworkServiceStatus aMbmsNetworkServiceStatus);
	void CallbackNotifyMbmsNetworkServiceStatusChange(TInt aError, TMbmsNetworkServiceStatus aMbmsNetworkServiceStatus);
	void CallbackUpdateMbmsMonitorServiceList(TInt aError);
	void CallbackUpdateMbmsMonitorServiceList(TInt aError, const RArray<TUint>& aFailedServiceIds);
	void CallbackNotifyMbmsServiceAvailabilityChange(TInt aError, const RArray<TUint>& aAvailableServiceIds);
	void CallbackUpdateMbmsSessionList(TInt aError, const TDesC& aContextName);
	void CallbackNotifyConnectionInfoChange(TInt aError, const TDesC& aContextName, const RPacketContext::TConnectionInfoV1& aConnectionInfo);

	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);

private:

	CPacketServicesDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);

	TInt EtelPcktR99R4NegToEtelPcktR97NegQoS(const RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
	                                           RPacketQoS::TQoSGPRSNegotiated& aQoSGPRSNegotiated);

	TInt EtelPcktR97NegToEtelPcktR99R4NegQoS(const RPacketQoS::TQoSGPRSNegotiated& aQoSGPRSNegotiated,
			   RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated);

	TInt EtelPcktR99R4NegToEtelPcktR5NegQoS(const RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
												   RPacketQoS::TQoSR5Negotiated& aQoSR5Negotiated);

	void ConvertDNSToPCOBuffer (const TDes8& aPrimaryDNS, const TDes8& aSecondaryDNS, const TUint8 aRequestID, TDes8& aFormatPcoBufferAddr);
	void ConvertUsernameAndPasswordToPCOBuffer(const TDes8& aUsername, const TDes8& aPassword, const TUint8 aRequestID, TDes8& aFormatedBuffer);

	void DecodePCO(const RPacketContext::TMiscProtocolBuffer& aPcoBuffer, RPacketContext::TAuthData& aUsername, RPacketContext::TAuthData& aPassword, RPacketContext::TProtocolAddress& aPrimaryDNS, RPacketContext::TProtocolAddress& aSecondaryDNS, RPacketContext::TMiscProtocolBuffer& aMiscBuffer);

	CPcktMbmsMonitoredServiceList* CreateServiceListL(const RArray<TUint>& aServiceIds) const;

	
	void ConstructL();
	
	

private:	// Not owned

	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;

	// Interfaces in the Licensee LTSY, not owned by this object

    MLtsyDispatchPacketServicesPacketAttach* iLtsyDispatchPacketServicesPacketAttach;
    MLtsyDispatchPacketServicesGetPacketAttachMode* iLtsyDispatchPacketServicesGetPacketAttachMode;
    MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus* iLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus;
    MLtsyDispatchPacketServicesPacketDetach* iLtsyDispatchPacketServicesPacketDetach;
    MLtsyDispatchPacketServicesSetPdpContextConfig* iLtsyDispatchPacketServicesSetPdpContextConfig;
    MLtsyDispatchPacketServicesModifyActivePdpContext* iLtsyDispatchPacketServicesModifyActivePdpContext;
    MLtsyDispatchPacketServicesInitialisePdpContext* iLtsyDispatchPacketServicesInitialisePdpContext;
    MLtsyDispatchPacketServicesDeletePdpContext* iLtsyDispatchPacketServicesDeletePdpContext;
    MLtsyDispatchPacketServicesSetPacketAttachMode* iLtsyDispatchPacketServicesSetPacketAttachMode;
    MLtsyDispatchPacketServicesNotifyPacketStatusChange* iLtsyDispatchPacketServicesNotifyPacketStatusChange;
    MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams* iLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams;
    MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params* iLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params;
    MLtsyDispatchPacketServicesActivatePdpContext* iLtsyDispatchPacketServicesActivatePdpContext;
    MLtsyDispatchPacketServicesSetPdpContextQosGprs* iLtsyDispatchPacketServicesSetPdpContextQosGprs;
    MLtsyDispatchPacketServicesSetPdpContextQosR99R4* iLtsyDispatchPacketServicesSetPdpContextQosR99R4;
    MLtsyDispatchPacketServicesSetPdpContextQosR5* iLtsyDispatchPacketServicesSetPdpContextQosR5;
    MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest* iLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest;
    MLtsyDispatchPacketServicesDeactivatePdpContext* iLtsyDispatchPacketServicesDeactivatePdpContext;
    MLtsyDispatchPacketServicesAddPacketFilter* iLtsyDispatchPacketServicesAddPacketFilter;
    MLtsyDispatchPacketServicesGetStatus* iLtsyDispatchPacketServicesGetStatus;
    MLtsyDispatchPacketServicesGetStaticCapabilities* iLtsyDispatchPacketServicesGetStaticCapabilities;
    MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists* iLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists;
    MLtsyDispatchPacketServicesGetMaxNoActiveServices* iLtsyDispatchPacketServicesGetMaxNoActiveServices;
    MLtsyDispatchPacketServicesInitialiseMbmsContext* iLtsyDispatchPacketServicesInitialiseMbmsContext;
    MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus* iLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus;
    MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList* iLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList;
    MLtsyDispatchPacketServicesUpdateMbmsSessionList* iLtsyDispatchPacketServicesUpdateMbmsSessionList;
	MLtsyDispatchPacketServicesRemovePacketFilter* iLtsyDispatchPacketServicesRemovePacketFilter;

	}; // class CPacketServicesDispatcher

#endif // __CPACKETSERVICESDISPATCHER_H_

