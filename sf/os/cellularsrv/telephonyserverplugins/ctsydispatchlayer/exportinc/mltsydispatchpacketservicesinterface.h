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
// This file contains all the interfaces classes that can be implemented by
// the Licensee LTSY relating to PacketServices related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHPACKETSERVICESINTERFACE_H_
#define MLTSYDISPATCHPACKETSERVICESINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <etelpckt.h>
#include <etelqos.h>




class MLtsyDispatchPacketServicesPacketAttach : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesPacketAttachApiId = KDispatchPacketServicesFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketAttach
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesPacketAttachComp()
	 *
	 * Implementation of this interface should issue an attach request to the LTSY.
	 * 	
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandlePacketAttachReqL() = 0;

	}; // class MLtsyDispatchPacketServicesPacketAttach



class MLtsyDispatchPacketServicesGetPacketAttachMode : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesGetPacketAttachModeApiId = KDispatchPacketServicesFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketGetAttachMode
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesGetPacketAttachModeComp()
	 *
	 * Implementation of this interface will return the attach mode. However if the call is
	 * being made during a packet service open process it will skip the get attach mode call.
	 * 		 	
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetPacketAttachModeReqL() = 0;

	}; // class MLtsyDispatchPacketServicesGetPacketAttachMode



class MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId = KDispatchPacketServicesFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketGetNtwkRegStatus
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesGetPacketNetworkRegistrationStatusComp()
	 *
	 * Implementation of this interface should retrieve the packet network registration status 
	 * from the baseband.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetPacketNetworkRegistrationStatusReqL() = 0;

	}; // class MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus



class MLtsyDispatchPacketServicesPacketDetach : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesPacketDetachApiId = KDispatchPacketServicesFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketDetach
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesPacketDetachComp()
	 * 	 
	 * Implementation of this interface should issue a detach request to the LTSY.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandlePacketDetachReqL() = 0;

	}; // class MLtsyDispatchPacketServicesPacketDetach



class MLtsyDispatchPacketServicesSetPdpContextConfig : public MLtsyDispatchInterface
	{	
public:

	static const TInt KLtsyDispatchPacketServicesSetPdpContextConfigApiId = KDispatchPacketServicesFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketContextSetConfig
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextConfigComp()
	 * Implementation of this interface should pass LTSY the parameters it needs to set the 
	 * context configuration 
	 *
	 * @param aContextId the context name
	 * @param aAccessPointName the access name which identifies the GGSN to be used
 	 * @param aPdpType the protocol type
 	 * @param aPdpAddress the PDP address for this context
 	 * @param aPcoBuffer the pco buffer
 	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetPdpContextConfigReqL(const TDesC& aContextId,
			const TDesC8& aAccessPointName,
			const RPacketContext::TProtocolType aPdpType,
			const TDesC8& aPdpAddress,
			const TDesC8& aPcoBuffer) = 0;

	}; // class MLtsyDispatchPacketServicesSetPdpContextConfig



class MLtsyDispatchPacketServicesModifyActivePdpContext : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesModifyActivePdpContextApiId = KDispatchPacketServicesFuncUnitId + 6;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketContextModifyActiveContext
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesModifyActivePdpContextComp()
	 * Implementation of this interface should signal to the Licensee TSY that it can action the modification
	 * of an existing context, the changes being indicated via appropriate RPacketContext::SetConfig and 
	 * RPacketContext::SetProfileParameters calls
	 *
	 * @param aContextName the context name.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleModifyActivePdpContextReqL(const TDesC& aContextName) = 0;
	}; // class MLtsyDispatchPacketServicesModifyActivePdpContext



class MLtsyDispatchPacketServicesInitialisePdpContext : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesInitialisePdpContextApiId = KDispatchPacketServicesFuncUnitId + 7;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketContextInitialiseContext
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesInitialisePdpContextComp()
	 * Implementation of this interface should initialise either primary or secondary contexts
	 *
	 *
     * @param aPrimaryContextName Primary context name in the form of a character string, 
	 * the maximum length of the descriptor should not exceed KMaxInfoName.
     * @param aSecondaryContextName Optional secondary context name in the form of a character
     * string, the maximum length of the descriptor should not exceed KMaxInfoName.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleInitialisePdpContextReqL(const TDesC& aPrimaryContextName, const TDesC& aSecondaryContextName) = 0;
	}; // class MLtsyDispatchPacketServicesInitialisePdpContext



class MLtsyDispatchPacketServicesDeletePdpContext : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesDeletePdpContextApiId = KDispatchPacketServicesFuncUnitId + 8;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketContextDelete
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesDeletePdpContextComp()
	 *
	 * Implementation of this interface should delete the context given.      
	 * 
	 * @param aContextName The name of the context to delete, 
	 * the maximum length of the descriptor should not exceed KMaxInfoName.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleDeletePdpContextReqL(const TDesC& aContextName) = 0;

	}; // class MLtsyDispatchPacketServicesDeletePdpContext



class MLtsyDispatchPacketServicesSetPacketAttachMode : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesSetPacketAttachModeApiId = KDispatchPacketServicesFuncUnitId + 9;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketSetAttachMode
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesSetPacketAttachModeComp()
	 *
	 * Implementation of this interface should set the packet service attach mode.
	 * @param aAttachMode the attach mode due to be set.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetPacketAttachModeReqL(RPacketService::TAttachMode aAttachMode) = 0;

	}; // class MLtsyDispatchPacketServicesSetPacketAttachMode



class MLtsyDispatchPacketServicesNotifyPacketStatusChange : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesNotifyPacketStatusChangeApiId = KDispatchPacketServicesFuncUnitId + 10;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketNotifyStatusChange
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketStatusChangeComp()
	 *
	 * Implementation of this interface should notify when the status of the packet service was changed.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleNotifyPacketStatusChangeReqL() = 0;

	}; // class MLtsyDispatchPacketServicesNotifyPacketStatusChange



class MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId = KDispatchPacketServicesFuncUnitId + 11;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketSetDefaultContextParams
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesSetDefaultPdpContextParamsComp()
	 *
	 * Implementation of this interface should set default configuration for Packet Context using GPRS params.
	 *
	 * @param aContextConfigGprs GPRS configuration input parameters.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetDefaultPdpContextGprsParamsReqL(const RPacketContext::TContextConfigGPRS& aContextConfigGprs) = 0;

	}; // class MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams

class MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId = KDispatchPacketServicesFuncUnitId + 12;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketSetDefaultContextParams
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesSetDefaultPdpContextParamsComp()
	 * 
	 * Implementation of this interface should set default configuration for Packet Context using R99/R4 params.
	 *
	 * @param aContextConfigR99R4 R99/R4 configuration input parameters.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetDefaultPdpContextR99R4ParamsReqL(const RPacketContext::TContextConfigR99_R4& aContextConfigR99R4) = 0;

	}; // class MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params


class MLtsyDispatchPacketServicesActivatePdpContext : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesActivatePdpContextApiId = KDispatchPacketServicesFuncUnitId + 13;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketContextActivate
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesActivatePdpContextComp()
	 *
	 * Implementation of this interface should activate the context given.
	 * 
	 * @param aContextName The context name, the maximum length of the descriptor 
	 * should not exceed KMaxInfoName.	 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleActivatePdpContextReqL(const TDesC& aContextName) = 0;

	}; // class MLtsyDispatchPacketServicesActivatePdpContext



class MLtsyDispatchPacketServicesSetPdpContextQosGprs : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId = KDispatchPacketServicesFuncUnitId + 14;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketQoSSetProfileParams
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextQosComp()
	 * 
	 * Implementation of this interface should set GPRS QoS parametes for the given context.
	 *
	 * @param aContextName The name of the context, the maximum length of the 
     * descriptor should not exceed KMaxInfoName.
	 * @param aQoSConfig The QoS configuration parameters.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetPdpContextQosGprsReqL(const TDesC& aContextName, const RPacketQoS::TQoSGPRSRequested& aQoSConfig) = 0;

	}; // class MLtsyDispatchPacketServicesSetPdpContextQosGprs

class MLtsyDispatchPacketServicesSetPdpContextQosR99R4 : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId = KDispatchPacketServicesFuncUnitId + 15;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketQoSSetProfileParams
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextQosComp()
	 * 
	 * Implementation of this interface should set R99R4 QoS parametes for the given context.
	 * 
	 *
	 * @param aContextName the name of the context.
	 * @param aQoSConfig the QoS configuration parameters.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetPdpContextQosR99R4ReqL(const TDesC& aContextName, const RPacketQoS::TQoSR99_R4Requested& aQoSConfig) = 0;

	}; // class MLtsyDispatchPacketServicesSetPdpContextQosR99R4

class MLtsyDispatchPacketServicesSetPdpContextQosR5 : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId = KDispatchPacketServicesFuncUnitId + 16;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketQoSSetProfileParams
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextQosComp()
	 *
	 * Implementation of this interface should set R5 QoS parametes for the given context.
	 *
	 *
	 * @param aContextName The name of the context, the maximum length of the 
	 * descriptor should not exceed KMaxInfoName.
	 * @param aQoSConfig The QoS configuration parameters.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetPdpContextQosR5ReqL(const TDesC& aContextName, const RPacketQoS::TQoSR5Requested& aQoSConfig) = 0;

	}; // class MLtsyDispatchPacketServicesSetPdpContextQosR5




class MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId = KDispatchPacketServicesFuncUnitId + 17;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketRejectActivationRequest
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesRejectNetworkInitiatedContextActivationRequestComp()
	 *
	 * Implementation of this interface should reject the network initiated context activation request.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleRejectNetworkInitiatedContextActivationRequestReqL() = 0;

	}; // class MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest



class MLtsyDispatchPacketServicesDeactivatePdpContext : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesDeactivatePdpContextApiId = KDispatchPacketServicesFuncUnitId + 18;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketContextDeactivate
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesDeactivatePdpContextComp()
	 * Implementation of this interface should deactivate the context indicated by the context name.
	 *
	 *
	 * @param aContextName The context name that is to be deactivated, 
	 * the maximum length of the descriptor should not exceed KMaxInfoName.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleDeactivatePdpContextReqL(const TDesC& aContextName) = 0;

	}; // class MLtsyDispatchPacketServicesDeactivatePdpContext



class MLtsyDispatchPacketServicesAddPacketFilter : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesAddPacketFilterApiId = KDispatchPacketServicesFuncUnitId + 19;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketContextAddPacketFilter
	 * request from the CTSY.
	 * Implementation of this interface should add a packet filter to the packet service.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextConfigComp()	 
	 * 
	 * @param aContextName The name of the context, the maximum length of the 
	 * descriptor should not exceed KMaxInfoName.
	 * @param aContextFilter An array contains up to eight filters to be used
	 * for configuring the secondary context.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleAddPacketFilterSyncL(const TDesC& aContextName,  CArrayFixFlat<RPacketContext::TPacketFilterV2>& aContextFilter) = 0;

	}; // class MLtsyDispatchPacketServicesAddPacketFilter

class MLtsyDispatchPacketServicesGetStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesGetStatusApiId = KDispatchPacketServicesFuncUnitId + 20;

	/**
	 * The CTSY Dispatcher shall invoke this function during the packet services bootup
	 * as part of EPacketNotifyStatusChange call.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesGetStatusComp()
	 * 
	 * Implemetation of this interface should retrieve the packet service status.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetStatusReqL() = 0;

	}; // class MLtsyDispatchPacketServicesGetStatus

class MLtsyDispatchPacketServicesGetStaticCapabilities : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId = KDispatchPacketServicesFuncUnitId + 21;

	/**
	 * The CTSY Dispatcher shall invoke this function during the packet services bootup
	 * as part of EPacketGetStaticCaps call.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesGetStaticCapabilitiesComp()
	 * 
	 * Implemetation of this interface should retrieve the packet service static capabilities.
	 * 
	 * Calls to RPacketService::GetStaticCaps() will then return the static capabilities returned
	 * in the callback.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RPacketService::GetStaticCaps()
	 */
	virtual TInt HandleGetStaticCapabilitiesReqL() = 0;

	}; // class MLtsyDispatchPacketServicesGetStaticCapabilities

class MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId = KDispatchPacketServicesFuncUnitId + 22;

	/**
	 * The CTSY Dispatcher shall invoke this function during the packet services bootup
	 * as part of EPacketEnumerateMbmsMonitorServiceList call.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesGetMaxNoMonitoredServiceListsComp()
	 * 
	 * Implemetation of this interface should retrieve the maximum number of monitored service lists.
	 * 
	 * Calls to RPacketService::EnumerateMbmsMonitorServiceList() will then return the 
	 * maximum number of monitored service lists in the aMaxAllowed field.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RPacketService::EnumerateMbmsMonitorServiceList()
	 */
	virtual TInt HandleGetMaxNoMonitoredServiceListsReqL() = 0;

	}; // class MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists

class MLtsyDispatchPacketServicesGetMaxNoActiveServices : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId = KDispatchPacketServicesFuncUnitId + 23;

	/**
	 * The CTSY Dispatcher shall invoke this function during the packet services bootup
	 * as part of EPacketEnumerateMbmsActiveServiceList call.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesGetMaxNoActiveServicesComp()
	 * 
	 * Implemetation of this interface should retrieve the maximum number of active services allowed
	 * (hence the maximum number of contexts allowed).
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 */
	virtual TInt HandleGetMaxNoActiveServicesReqL() = 0;

	}; // class MLtsyDispatchPacketServicesGetMaxNoActiveServices

class MLtsyDispatchPacketServicesInitialiseMbmsContext : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesInitialiseMbmsContextApiId = KDispatchPacketServicesFuncUnitId + 24;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECtsyPacketMbmsInitialiseContextReq
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesInitialiseMbmsContextComp()
	 * 
	 * Implementation of this interface should initialise the Mbms Context
	 * (Multimedia Broadcast Multicast Service).
	 *
     * @param aContextName The context name in the form of a character string 
	 * the maximum length of the descriptor should not exceed KMaxInfoName.
	 * 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RPacketMbmsContext::InitialiseContext()
	 */
	virtual TInt HandleInitialiseMbmsContextReqL(const TDesC& aContextName) = 0;

	}; // class MLtsyDispatchPacketServicesInitialiseMbmsContext

class MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId = KDispatchPacketServicesFuncUnitId + 25;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketGetMbmsNetworkServiceStatus
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesGetMbmsNetworkServiceStatusComp()
	 * 
	 * Implementation of this interface should retrieve the current MBMS
	 * (Multimedia Broadcast Multicast Service) network state. 
	 * 
	 * There can be 3 states that MBMS feature availability could return,
	 *  Unknown - when UE is not GMM Attached.
	 *  Supported - When UE is GMM Attached and it is known that cell supports MBMS.
	 *  Not Supported - When UE is GMM Attached and it is know that cell does not support MBMS.
	 * 
	 * The state is only known from GMM Attach response from the network or during a Routing Area update.
	 * A consequence of this is that it is required for the UE to be in GMM Attach mode in order 
	 * for either of these messages to be received. If the UE is not in GMM ATTACH then it is unable 
	 * to determine MBMS network support.
	 *
     * @param aAttemptAttach If a GMM attached is to be performed.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see  RPacketService::GetMbmsNetworkServiceStatus()
	 */
	virtual TInt HandleGetMbmsNetworkServiceStatusReqL(TBool aAttemptAttach) = 0;

	}; // class MLtsyDispatchPacketServicesInitialiseMbmsContext

class MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId = KDispatchPacketServicesFuncUnitId + 26;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketUpdateMbmsMonitorServiceList
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsMonitorServiceListComp()
	 * 
	 * Implementation of this interface should request to update the MBMS (Multimedia Broadcast Multicast Service)
	 * monitor service availability list with the change list entries as per the action mentioned. 
	 * The CTSY stores the monitor service availability list, this list is updated on a successful completion.
	 * 
     * @param aAction The MBMS action (add, remove or remove all).
     * @param aServiceList The current MBMS monitor service availability list as held by the CTSY.
     * @param aChangeList The list of entries to be changed as per the action. (e.g. the action = Add,
     * the change list will contain entries to be added to the MBMS monitor service availability list)
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RPacketService::UpdateMbmsMonitorServiceListL()
	 * @see RPacketService::NotifyMbmsServiceAvailabilityChange()
	 * @see CRetrievePcktMbmsMonitoredServices()
	 */
	virtual TInt HandleUpdateMbmsMonitorServiceListsReqL(TMbmsAction aAction, const CPcktMbmsMonitoredServiceList& aServiceList, const CPcktMbmsMonitoredServiceList& aChangeList) = 0;

	}; // class MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList

class MLtsyDispatchPacketServicesUpdateMbmsSessionList : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId = KDispatchPacketServicesFuncUnitId + 27;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketContextUpdateMbmsSessionList
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsSessionListComp()
	 * 
	 * Implementation of this interface should request to add a new session to be received or remove 
	 * an existing session to/from the MBMS services table.
	 * The CTSY stores the MBMS services table, this table is updated on a successful completion.
	 * 
	 * @param aContextName The MBMS context name in the form of a character string.
     * @param aAction The action to be performed (add, remove or remove all).
     * @param aSessionId The MBMS session identifier which needs to be received or removed.
     * @param aServiceList The MBMS services table as held by the CTSY.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RPacketService::UpdateMbmsSessionList()
	 * @see CRetrievePcktMbmsSessionList()
	 */
	virtual TInt HandleUpdateMbmsSessionListReqL(const TDesC& aContextName, TMbmsAction aAction, TMbmsSessionId aSessionId, const TMbmsSessionIdList& aServiceList) = 0;

	}; // class MLtsyDispatchPacketServicesUpdateMbmsSessionList

class MLtsyDispatchPacketServicesRemovePacketFilter : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPacketServicesRemovePacketFilterApiId = KDispatchPacketServicesFuncUnitId + 28;
	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EPacketContextRemovePacketFilter
	 * request from the CTSY.
	 * Implementation of this interface should remove a packet filter from the packet service.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextConfigComp()	 
	 * 
	 * @param aContextName The name of the context, the maximum length of the 
	 * descriptor should not exceed KMaxInfoName.
	 * @param aFilterId The filter Id to remove.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleRemovePacketFilterSyncL(const TDesC& aContextName,  TInt aFilterId) = 0;

	}; // class MLtsyDispatchPacketServicesRemovePacketFilter

#endif /*MLTSYDISPATCHPACKETSERVICESINTERFACE_H_*/
