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
// cltsydispatchpacketserviceshandler.cpp
//

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

#include <pcktcs.h>

#include "cltsypacketserviceshandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"

// Can be removed in real LTSY implementation
#include MTEST_INCLUDE_MOCKLTSYENGINE_HEADER

CLtsyPacketServicesHandler::CLtsyPacketServicesHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsyPacketServicesHandler::~CLtsyPacketServicesHandler()
	{
	}


CLtsyPacketServicesHandler* CLtsyPacketServicesHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyPacketServicesHandler* self = new (ELeave) CLtsyPacketServicesHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyPacketServicesHandler* CLtsyPacketServicesHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyPacketServicesHandler* self=
			CLtsyPacketServicesHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsyPacketServicesHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	
	} // CLtsyPacketServicesHandler::ConstructL

TBool CLtsyPacketServicesHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_SUPPORT_MOCKLTSYENGINE(KDispatchPacketServicesFuncUnitId, aDispatchApiId);
	
	/* uncomment when implementing for real LTSY
	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		// case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId: return ETrue;
		
		default:
			return EFalse;
		}
	*/
	}

void CLtsyPacketServicesHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_IND_SUPPORT_MOCKLTSYENGINE(KDispatchPacketServicesFuncUnitId, aIdGroup, aIndIdBitMask);
	
	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	/*
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask = KLtsyDispatchPhoneNotifyNetworkModeChangeIndId | 
						KLtsyDispatchPhoneNotifyEgprsInfoChangeIndId | 
						KLtsyDispatchPhoneNotifySignalStrengthChangeIndId;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
	*/
	}


TInt CLtsyPacketServicesHandler::HandlePacketAttachReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesPacketAttachComp()
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandlePacketAttachReqL


TInt CLtsyPacketServicesHandler::HandleGetPacketAttachModeReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesGetPacketAttachModeComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleGetPacketAttachModeReqL


TInt CLtsyPacketServicesHandler::HandleGetPacketNetworkRegistrationStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesGetPacketNetworkRegistrationStatusComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleGetPacketNetworkRegistrationStatusReqL


TInt CLtsyPacketServicesHandler::HandlePacketDetachReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesPacketDetachComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	
	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandlePacketDetachReqL


TInt CLtsyPacketServicesHandler::HandleSetPdpContextConfigReqL(const TDesC& aContextId,
		const TDesC8& aAccessPointName,
		const RPacketContext::TProtocolType aPdpType,
		const TDesC8& aPdpAddress,
		const TDesC8& aPcoBuffer)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextConfigComp()
 *
 * @param contextId The context name, the descriptor should be TBuf<10>
 * @param iAccessPointName The access name which identifies the GGSN to be used
 * @param iPdpType The protocol type
 * @param iPdpAddress The PDP address for this context
 * @param aPcoBuffer The pco buffer
 * 
 * The purpose of the protocol configuration options (PCO) is to transfer external network protocol options 
 * associated with a PDP context activation, and transfer additional (protocol) data 
 * (e.g. configuration parameters, error codes or messages/events) associated with an external protocol 
 * or an application.
 * The protocol configuration options (PCO) is a type 4 information element with a minimum length of 3 
 * octets and a maximum length of 253 octets
 * In order to generate the PCO buffer a TTlvStruct object is being used. The TTlvStruct wraps the buffers  
 * inserted to the PCO and identifies the buffers with given IDs.
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId, 
			&aContextId, &aAccessPointName, aPdpType, &aPdpAddress, &aPcoBuffer);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleSetPdpContextConfigReqL


TInt CLtsyPacketServicesHandler::HandleModifyActivePdpContextReqL(const TDesC& aContextName)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesModifyActivePdpContextComp()
 *
 * @param aContextName Context name for which the modification is intended
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;	

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId, &aContextName);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleModifyActivePdpContextReqL


TInt CLtsyPacketServicesHandler::HandleInitialisePdpContextReqL(const TDesC& aPrimaryContextName, const TDesC& aSecondaryContextName)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesInitialisePdpContextComp()
 *
 * @param aPrimaryContextName Primary context name in the form of a character string, the descriptor should be TBuf<10>
 * @param aSecondaryContextName Optional secondary context name in the form of a character string, the descriptor should be TBuf<10>
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	
	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId, &aPrimaryContextName, &aSecondaryContextName);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleInitialisePdpContextReqL


TInt CLtsyPacketServicesHandler::HandleDeletePdpContextReqL(const TDesC& aContextName)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesDeletePdpContextComp()
 *
 * @param aContextName the context name to be deleted, , the descriptor should be TBuf<10>
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId, &aContextName);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleDeletePdpContextReqL


TInt CLtsyPacketServicesHandler::HandleSetPacketAttachModeReqL(RPacketService::TAttachMode aAttachMode)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesSetPacketAttachModeComp()
 *
 * @param aAttachMode the attach mode due to be set.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId, aAttachMode);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleSetPacketAttachModeReqL


TInt CLtsyPacketServicesHandler::HandleNotifyPacketStatusChangeReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketStatusChangeComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesNotifyPacketStatusChange::KLtsyDispatchPacketServicesNotifyPacketStatusChangeApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleNotifyPacketStatusChangeReqL


TInt CLtsyPacketServicesHandler::HandleSetDefaultPdpContextGprsParamsReqL(const RPacketContext::TContextConfigGPRS& aContextConfigGprs)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesSetDefaultPdpContextParamsComp()
 *
 * @param aContextConfigGprs Default GPRS parameters
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId, &aContextConfigGprs);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleSetDefaultPdpContextGprsParamsReqL

TInt CLtsyPacketServicesHandler::HandleSetDefaultPdpContextR99R4ParamsReqL(const RPacketContext::TContextConfigR99_R4& aContextConfigR99R4)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesSetDefaultPdpContextParamsComp()
 *
 * @param aContextConfigR99R4 Default R99/R4 parameters
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId, &aContextConfigR99R4);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleSetDefaultPdpContextR99R4ParamsReqL



TInt CLtsyPacketServicesHandler::HandleActivatePdpContextReqL(const TDesC& aContextName)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesActivatePdpContextComp()
 *
 * @param aContextName The context name.	 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;	

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId, &aContextName);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleActivatePdpContextReqL


TInt CLtsyPacketServicesHandler::HandleSetPdpContextQosGprsReqL(const TDesC& aContextName, const RPacketQoS::TQoSGPRSRequested& aQoSConfig)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextQosComp()
 *
 * @param aContextName the name of the context.
 * @param aQoSConfig the QoS configuration parameters. 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId, &aContextName, &aQoSConfig);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleSetPdpContextQosGprsReqL

TInt CLtsyPacketServicesHandler::HandleSetPdpContextQosR99R4ReqL(const TDesC& aContextName, const RPacketQoS::TQoSR99_R4Requested& aQoSConfig)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextQosComp()
 *
 * @param aContextName The name of the context.
 * @param aQoSConfig The QoS configuration parameters. 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId, &aContextName, &aQoSConfig);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleSetPdpContextQosR99R4ReqL

TInt CLtsyPacketServicesHandler::HandleSetPdpContextQosR5ReqL(const TDesC& aContextName, const RPacketQoS::TQoSR5Requested& aQoSConfig)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextQosComp()
 *
 * @param aContextName The name of the context.
 * @param aQoSConfig The QoS configuration parameters. 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	
	TInt ret = KErrNotSupported;
	
	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId, &aContextName, &aQoSConfig);
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleSetPdpContextQosR5ReqL
	


TInt CLtsyPacketServicesHandler::HandleRejectNetworkInitiatedContextActivationRequestReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesRejectNetworkInitiatedContextActivationRequestComp()
 * 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	
	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleRejectNetworkInitiatedContextActivationRequestReqL


TInt CLtsyPacketServicesHandler::HandleDeactivatePdpContextReqL(const TDesC& aContextName)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesDeactivatePdpContextComp()
 *
 * @param aContextName The context name that is to be deactivated.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId, &aContextName);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleDeactivatePdpContextReqL


TInt CLtsyPacketServicesHandler::HandleAddPacketFilterSyncL(const TDesC& aContextName, CArrayFixFlat<RPacketContext::TPacketFilterV2>& aContextFilter)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextConfigComp
 * 
 * @param aContextName The name of the context.
 * @param aContextFilter An array contains up to eight filters to be used
 * for configuring the secondary context.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId, &aContextName, &aContextFilter);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleAddPacketFilterSyncL


TInt CLtsyPacketServicesHandler::HandleGetStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesGetStatus()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleGetStatusReqL

TInt CLtsyPacketServicesHandler::HandleGetStaticCapabilitiesReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesGetStaticCapabilitiesComp()
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleGetStaticCapabilitiesReqL

TInt CLtsyPacketServicesHandler::HandleGetMaxNoMonitoredServiceListsReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesGetMaxNoMonitoredServiceListsComp()
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleGetMaxNoMonitoredServiceListsReqL

TInt CLtsyPacketServicesHandler::HandleGetMaxNoActiveServicesReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesGetMaxNoActiveServicesComp()
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleGetMaxNoActiveServicesReqL

TInt CLtsyPacketServicesHandler::HandleInitialiseMbmsContextReqL(const TDesC& aContextName)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesInitialiseMbmsContextComp()
 *
 * @param aContextName The context name in the form of a character string 
 * the maximum length of the descriptor should not exceed KMaxInfoName.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPacketServicesFuncUnitId, 
									MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId,
									&aContextName);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleInitialiseMbmsContextReqL

TInt CLtsyPacketServicesHandler::HandleGetMbmsNetworkServiceStatusReqL(TBool aAttemptAttach)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesGetMbmsNetworkServiceStatusComp()
 * 
 * @param aAttemptAttach If a GMM attached is to be performed.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPacketServicesFuncUnitId, 
									MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId,
									aAttemptAttach);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleGetMbmsNetworkServiceStatusReqL

TInt CLtsyPacketServicesHandler::HandleUpdateMbmsMonitorServiceListsReqL(TMbmsAction aAction, const CPcktMbmsMonitoredServiceList& aServiceList, const CPcktMbmsMonitoredServiceList& aChangeList)
/**
 * It is a request call that is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsMonitorServiceListComp()
 * 
 * @param aAction The MBMS action (add, remove or remove all).
 * @param aServiceList The current MBMS monitor service availability list as held by the CTSY.
 * @param aChangeList The list of entries to be changed as per the action. (e.g. the action = Add,
 * the change list will contain entries to be added to the MBMS monitor service availability list)
 *
 * @return KErrNone on success, otherwise another error code indicating the
 * failure.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPacketServicesFuncUnitId, 
									MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId,
									aAction,
									&aServiceList,
									&aChangeList);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleUpdateMbmsMonitorServiceListsReqL


TInt CLtsyPacketServicesHandler::HandleUpdateMbmsSessionListReqL(const TDesC& aContextName, TMbmsAction aAction, TMbmsSessionId aSessionId, const TMbmsSessionIdList& aServiceList)
/**
 * It is a request call that is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsSessionListComp()
 * 
 * @param aContextName The MBMS context name in the form of a character string.
 * @param aAction The action to be performed (add, remove or remove all).
 * @param aSessionId The MBMS session identifier which needs to be received or removed.
 * @param aServiceList The MBMS services table as held by the CTSY.
 *
 * @return KErrNone on success, otherwise another error code indicating the
 * failure.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPacketServicesFuncUnitId,MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId,
									&aContextName,
									aAction,
									aSessionId,
									&aServiceList);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleUpdateMbmsSessionListReqL


TInt CLtsyPacketServicesHandler::HandleRemovePacketFilterSyncL(const TDesC& aContextName, TInt aFilterId)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextConfigComp
 * 
 * @param aContextName The name of the context.
 * @param aFilterId The filter Id to be removed.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	
	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesRemovePacketFilter::KLtsyDispatchPacketServicesRemovePacketFilterApiId, &aContextName, aFilterId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleRemovePacketFilterSyncL


