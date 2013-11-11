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
//


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpacketservicesdispatcherTraces.h"
#endif

#include "cpacketservicesdispatcher.h"

#include <ctsy/ltsy/mltsydispatchpacketservicesinterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

#include <etelpckt.h>
#include <pcktcs.h>
#include <etelqos.h>
#include <in_sock.h>
#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"
#include <pcktlist.h>

CPacketServicesDispatcher::CPacketServicesDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot)
	{
	} // CPacketServicesDispatcher::CPacketServicesDispatcher


CPacketServicesDispatcher::~CPacketServicesDispatcher()
	{
	} // CPacketServicesDispatcher::~CPacketServicesDispatcher


CPacketServicesDispatcher* CPacketServicesDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CPacketServicesDispatcher* self =
		new (ELeave) CPacketServicesDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CPacketServicesDispatcher::NewLC


CPacketServicesDispatcher* CPacketServicesDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CPacketServicesDispatcher* self =
		CPacketServicesDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CPacketServicesDispatcher::NewL


void CPacketServicesDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;

	// Get the Licensee LTSY interfaces related to PacketServices functionality
	// from the factory


	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId))
		{
		TAny* packetAttachInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId,
		       	packetAttachInterface);
		iLtsyDispatchPacketServicesPacketAttach =
				static_cast<MLtsyDispatchPacketServicesPacketAttach*>(packetAttachInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesPacketAttach, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId))
		{
		TAny* getPacketAttachModeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId,
		       	getPacketAttachModeInterface);
		iLtsyDispatchPacketServicesGetPacketAttachMode =
				static_cast<MLtsyDispatchPacketServicesGetPacketAttachMode*>(getPacketAttachModeInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesGetPacketAttachMode, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId))
		{
		TAny* getPacketNetworkRegistrationStatusInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId,
		       	getPacketNetworkRegistrationStatusInterface);
		iLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus =
				static_cast<MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus*>(getPacketNetworkRegistrationStatusInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId))
		{
		TAny* packetDetachInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId,
		       	packetDetachInterface);
		iLtsyDispatchPacketServicesPacketDetach =
				static_cast<MLtsyDispatchPacketServicesPacketDetach*>(packetDetachInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesPacketDetach, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId))
		{
		TAny* setPdpContextConfigInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId,
		       	setPdpContextConfigInterface);
		iLtsyDispatchPacketServicesSetPdpContextConfig =
				static_cast<MLtsyDispatchPacketServicesSetPdpContextConfig*>(setPdpContextConfigInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesSetPdpContextConfig, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId))
		{
		TAny* modifyActivePdpContextInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId,
		       	modifyActivePdpContextInterface);
		iLtsyDispatchPacketServicesModifyActivePdpContext =
				static_cast<MLtsyDispatchPacketServicesModifyActivePdpContext*>(modifyActivePdpContextInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesModifyActivePdpContext, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId))
		{
		TAny* initialisePdpContextInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId,
		       	initialisePdpContextInterface);
		iLtsyDispatchPacketServicesInitialisePdpContext =
				static_cast<MLtsyDispatchPacketServicesInitialisePdpContext*>(initialisePdpContextInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesInitialisePdpContext, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId))
		{
		TAny* deletePdpContextInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId,
		       	deletePdpContextInterface);
		iLtsyDispatchPacketServicesDeletePdpContext =
				static_cast<MLtsyDispatchPacketServicesDeletePdpContext*>(deletePdpContextInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesDeletePdpContext, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId))
		{
		TAny* setPacketAttachModeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId,
		       	setPacketAttachModeInterface);
		iLtsyDispatchPacketServicesSetPacketAttachMode =
				static_cast<MLtsyDispatchPacketServicesSetPacketAttachMode*>(setPacketAttachModeInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesSetPacketAttachMode, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesNotifyPacketStatusChange::KLtsyDispatchPacketServicesNotifyPacketStatusChangeApiId))
		{
		TAny* notifyPacketStatusChangeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesNotifyPacketStatusChange::KLtsyDispatchPacketServicesNotifyPacketStatusChangeApiId,
		       	notifyPacketStatusChangeInterface);
		iLtsyDispatchPacketServicesNotifyPacketStatusChange =
				static_cast<MLtsyDispatchPacketServicesNotifyPacketStatusChange*>(notifyPacketStatusChangeInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesNotifyPacketStatusChange, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId))
		{
		TAny* setDefaultPdpContextParamsInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId,
		       	setDefaultPdpContextParamsInterface);
		iLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams =
				static_cast<MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams*>(setDefaultPdpContextParamsInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId))
		{
		TAny* setDefaultPdpContextParamsInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId,
		       	setDefaultPdpContextParamsInterface);
		iLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params =
				static_cast<MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params*>(setDefaultPdpContextParamsInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId))
		{
		TAny* activatePdpContextInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId,
		       	activatePdpContextInterface);
		iLtsyDispatchPacketServicesActivatePdpContext =
				static_cast<MLtsyDispatchPacketServicesActivatePdpContext*>(activatePdpContextInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesActivatePdpContext, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId))
		{
		TAny* setPdpContextQosInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId,
		       	setPdpContextQosInterface);
		iLtsyDispatchPacketServicesSetPdpContextQosGprs =
				static_cast<MLtsyDispatchPacketServicesSetPdpContextQosGprs*>(setPdpContextQosInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesSetPdpContextQosGprs, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId))
		{
		TAny* setPdpContextQosInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId,
		       	setPdpContextQosInterface);
		iLtsyDispatchPacketServicesSetPdpContextQosR99R4 =
				static_cast<MLtsyDispatchPacketServicesSetPdpContextQosR99R4*>(setPdpContextQosInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesSetPdpContextQosR99R4, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if (iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId))
		{
		TAny* setPdpContextQosInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId,
		       	setPdpContextQosInterface);
		iLtsyDispatchPacketServicesSetPdpContextQosR5 =
				static_cast<MLtsyDispatchPacketServicesSetPdpContextQosR5*>(setPdpContextQosInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesSetPdpContextQosR5, CtsyDispatcherPanic(EInvalidNullPtr));
  		}


	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId))
		{
		TAny* rejectNetworkInitiatedContextActivationRequestInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId,
		       	rejectNetworkInitiatedContextActivationRequestInterface);
		iLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest =
				static_cast<MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest*>(rejectNetworkInitiatedContextActivationRequestInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId))
		{
		TAny* deactivatePdpContextInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId,
		       	deactivatePdpContextInterface);
		iLtsyDispatchPacketServicesDeactivatePdpContext =
				static_cast<MLtsyDispatchPacketServicesDeactivatePdpContext*>(deactivatePdpContextInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesDeactivatePdpContext, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId))
		{
		TAny* addPacketFilterInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId,
		       	addPacketFilterInterface);
		iLtsyDispatchPacketServicesAddPacketFilter =
				static_cast<MLtsyDispatchPacketServicesAddPacketFilter*>(addPacketFilterInterface);
        __ASSERT_DEBUG(iLtsyDispatchPacketServicesAddPacketFilter, CtsyDispatcherPanic(EInvalidNullPtr));
  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId))
			{
			TAny* addGetStatusInterface = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
					MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId,
					addGetStatusInterface);
			iLtsyDispatchPacketServicesGetStatus =
					static_cast<MLtsyDispatchPacketServicesGetStatus*>(addGetStatusInterface);
	        __ASSERT_DEBUG(iLtsyDispatchPacketServicesGetStatus, CtsyDispatcherPanic(EInvalidNullPtr));
	  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId))
			{
			TAny* getStaticCapabilitiesInterface = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
					MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId,
					getStaticCapabilitiesInterface);
			iLtsyDispatchPacketServicesGetStaticCapabilities =
					static_cast<MLtsyDispatchPacketServicesGetStaticCapabilities*>(getStaticCapabilitiesInterface);
	        __ASSERT_DEBUG(iLtsyDispatchPacketServicesGetStaticCapabilities, CtsyDispatcherPanic(EInvalidNullPtr));
	  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId))
			{
			TAny* getMaxNoMonitoredServiceListsInterface = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
					MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId,
					getMaxNoMonitoredServiceListsInterface);
			iLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists =
					static_cast<MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists*>(getMaxNoMonitoredServiceListsInterface);
	        __ASSERT_DEBUG(iLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists, CtsyDispatcherPanic(EInvalidNullPtr));
	  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId))
			{
			TAny* getMaxNoActiveServicesInterface = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
					MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId,
					getMaxNoActiveServicesInterface);
			iLtsyDispatchPacketServicesGetMaxNoActiveServices =
					static_cast<MLtsyDispatchPacketServicesGetMaxNoActiveServices*>(getMaxNoActiveServicesInterface);
	        __ASSERT_DEBUG(iLtsyDispatchPacketServicesGetMaxNoActiveServices, CtsyDispatcherPanic(EInvalidNullPtr));
	  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId))
			{
			TAny* getMbmsNetworkServiceStatus = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
					MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId,
					getMbmsNetworkServiceStatus);
			iLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus =
					static_cast<MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus*>(getMbmsNetworkServiceStatus);
	        __ASSERT_DEBUG(iLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus, CtsyDispatcherPanic(EInvalidNullPtr));
	  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId))
			{
			TAny* initialiseMbmsContext = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
					MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId,
					initialiseMbmsContext);
			iLtsyDispatchPacketServicesInitialiseMbmsContext =
					static_cast<MLtsyDispatchPacketServicesInitialiseMbmsContext*>(initialiseMbmsContext);
	        __ASSERT_DEBUG(iLtsyDispatchPacketServicesInitialiseMbmsContext, CtsyDispatcherPanic(EInvalidNullPtr));
	  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId))
			{
			TAny* updateMbmsMonitorServiceList = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
					MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId,
					updateMbmsMonitorServiceList);
			iLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList =
					static_cast<MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList*>(updateMbmsMonitorServiceList);
	        __ASSERT_DEBUG(iLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList, CtsyDispatcherPanic(EInvalidNullPtr));
	  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId))
			{
			TAny* updateMbmsSessionList = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
					 MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId,
					 updateMbmsSessionList);
			iLtsyDispatchPacketServicesUpdateMbmsSessionList =
					static_cast<MLtsyDispatchPacketServicesUpdateMbmsSessionList*>(updateMbmsSessionList);
	        __ASSERT_DEBUG(iLtsyDispatchPacketServicesUpdateMbmsSessionList, CtsyDispatcherPanic(EInvalidNullPtr));
	  		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPacketServicesFuncUnitId, MLtsyDispatchPacketServicesRemovePacketFilter::KLtsyDispatchPacketServicesRemovePacketFilterApiId))
			{
			TAny* removePacketFilterInterface = NULL;
			iLtsyFactoryV1.GetDispatchHandler(
			       	MLtsyDispatchPacketServicesRemovePacketFilter::KLtsyDispatchPacketServicesRemovePacketFilterApiId,
			       	removePacketFilterInterface);
			iLtsyDispatchPacketServicesRemovePacketFilter =
					static_cast<MLtsyDispatchPacketServicesRemovePacketFilter*>(removePacketFilterInterface);
	        __ASSERT_DEBUG(iLtsyDispatchPacketServicesRemovePacketFilter, CtsyDispatcherPanic(EInvalidNullPtr));
			}

	} // CPacketServicesDispatcher::ConstructL

void CPacketServicesDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.
 *
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;

	iDispatcherHolder = &aDispatcherHolder;
	} // CPacketServicesDispatcher::SetDispatcherHolder

TInt CPacketServicesDispatcher::DispatchPacketAttachL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 *
 * @see RPacketService::Attach()
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	if (iLtsyDispatchPacketServicesPacketAttach)
		{
		ret = iLtsyDispatchPacketServicesPacketAttach->HandlePacketAttachReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchPacketAttachL

TInt CPacketServicesDispatcher::DispatchGetPacketAttachModeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketGetAttachMode
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle method in Licensee LTSY
	if (iLtsyDispatchPacketServicesGetPacketAttachMode)
		{

		// Indicator to whether this call made during the open call
		TBool initPhase;
		aDataPackage->UnPackData(initPhase);

		if (!initPhase)
			{
			ret = iLtsyDispatchPacketServicesGetPacketAttachMode->HandleGetPacketAttachModeReqL();
			}
		}
	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchGetPacketAttachModeL

TInt CPacketServicesDispatcher::DispatchGetPacketNetworkRegistrationStatusL()
/**
 * Pass EPacketGetNtwkRegStatus request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	if (iLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus)
		{
		ret = iLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus->HandleGetPacketNetworkRegistrationStatusReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchGetPacketNetworkRegistrationStatusL

TInt CPacketServicesDispatcher::DispatchPacketDetachL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 *
 * @see RPacketService::Detach()
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Unpack data for this request inside the if statement if required,
	if (iLtsyDispatchPacketServicesPacketDetach)
		{
		ret = iLtsyDispatchPacketServicesPacketDetach->HandlePacketDetachReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchPacketDetachL

TInt CPacketServicesDispatcher::DispatchSetPdpContextConfigL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketContextSetConfig
 * and pass request on to Licensee LTSY.
 * 
 * The following function uses a PCO buffer: 
 * The purpose of the protocol configuration options (PCO) is to transfer external network protocol options 
 * associated with a PDP context activation, and transfer additional (protocol) data 
 * (e.g. configuration parameters, error codes or messages/events) associated with an external protocol 
 * or an application.
 * The protocol configuration options (PCO) is a type 4 information element with a minimum length of 3 
 * octets and a maximum length of 253 octets
 * In order to generate the PCO buffer a TTlvStruct object is being used. The TTlvStruct wraps the buffers  
 * inserted to the PCO and identifies the buffers with given IDs.
 * 
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesSetPdpContextConfig)
		{
		TInfoName* contextId = NULL;
		TPacketDataConfigBase* generalConfig = NULL;

		aDataPackage->UnPackData(&generalConfig, &contextId);

		__ASSERT_DEBUG(generalConfig, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(contextId, CtsyDispatcherPanic(EInvalidNullPtr));
		
		switch(generalConfig->ExtensionId())
			{
			case TPacketDataConfigBase::KConfigGPRS:
				{
				RPacketContext::TContextConfigGPRS* configGPRS = static_cast<RPacketContext::TContextConfigGPRS*>(generalConfig);

				// Protocol Config Option handling
				RPacketContext::TMiscProtocolBuffer pcoBuffer;
				pcoBuffer.Zero();
				TPtr8 pcoPtr (const_cast<TUint8*> (pcoBuffer.Ptr()), RPacketContext::KMiscProtocolBufferLength);

				TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> tlv(pcoPtr,0);

				TBuf8<KMaxUserAndPassLength> usernameAndPasswordBuffer(KMaxUserAndPassLength);
				
				// Generates the username, password and requestID buffer as described in 3GPP specification 3G TS 24.008
				ConvertUsernameAndPasswordToPCOBuffer(configGPRS->iProtocolConfigOption.iAuthInfo.iUsername, configGPRS->iProtocolConfigOption.iAuthInfo.iPassword, configGPRS->iProtocolConfigOption.iId, usernameAndPasswordBuffer);

				TPtr8 usernameAndPasswordParamDataPtr(const_cast<TUint8*>(usernameAndPasswordBuffer.Ptr()),
						usernameAndPasswordBuffer.Length(), usernameAndPasswordBuffer.Length());

				TBuf8<KMaxPdpAddressLength> primaryAndSecondaryDNSBuffer(KPrimaryAndSecondaryDNSLength + 1);
				
				// Generates the primary and secondary DNS buffers as described in 3GPP specification 3G TS 24.008
				ConvertDNSToPCOBuffer(configGPRS->iProtocolConfigOption.iDnsAddresses.iPrimaryDns, configGPRS->iProtocolConfigOption.iDnsAddresses.iSecondaryDns, configGPRS->iProtocolConfigOption.iId+1, primaryAndSecondaryDNSBuffer);

				// Setting the configurations buffers
				TPtr8 primaryAndSecondaryDNSParamDataPtr(const_cast<TUint8*>(primaryAndSecondaryDNSBuffer.Ptr()),
						primaryAndSecondaryDNSBuffer.Length(), primaryAndSecondaryDNSBuffer.Length());

				// Set up the TTlvStruct to include the pco details
				RPacketContext::TPcoId primaryAndSecondaryDNSID(KPrimaryAndSecondaryDNSID);
				RPacketContext::TPcoId usenameAndPasswordID(KUsenamePasswordDNSID);

				if (tlv.AppendItemL(usenameAndPasswordID, usernameAndPasswordParamDataPtr) !=  KErrOverflow)
					{
					if (tlv.AppendItemL(primaryAndSecondaryDNSID, primaryAndSecondaryDNSParamDataPtr) != KErrOverflow)
						{
						pcoBuffer.SetLength(primaryAndSecondaryDNSParamDataPtr.Length() + usernameAndPasswordParamDataPtr.Length() + 2 * KIDLength);
						pcoBuffer.Append(configGPRS->iProtocolConfigOption.iMiscBuffer);
						ret = iLtsyDispatchPacketServicesSetPdpContextConfig->HandleSetPdpContextConfigReqL(*contextId, configGPRS->iAccessPointName,
								configGPRS->iPdpType, configGPRS->iPdpAddress, pcoBuffer);
						}
					else
						{
						ret = KErrNotSupported;
						}
					}
				else
					{
					ret = KErrNotSupported;
					}
				}
				break;
			case TPacketDataConfigBase::KConfigRel99Rel4:
				{
				RPacketContext::TContextConfigR99_R4* contextConfigR99R4 = static_cast<RPacketContext::TContextConfigR99_R4*>(generalConfig);

				// Protocol Config Option handling
				RPacketContext::TMiscProtocolBuffer pcoBuffer;
				pcoBuffer.Zero();
				TPtr8 pcoPtr (const_cast<TUint8*> (pcoBuffer.Ptr()), RPacketContext::KMiscProtocolBufferLength);

				TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> tlv(pcoPtr,0);

				TBuf8<KMaxUserAndPassLength> usernameAndPasswordBuffer(KMaxUserAndPassLength);

				// Generates the username, password and requestID buffer as described in 3GPP specification 3G TS 24.008
				ConvertUsernameAndPasswordToPCOBuffer(contextConfigR99R4->iProtocolConfigOption.iAuthInfo.iUsername, contextConfigR99R4->iProtocolConfigOption.iAuthInfo.iPassword, contextConfigR99R4->iProtocolConfigOption.iId, usernameAndPasswordBuffer);

				TPtr8 usernameAndPasswordParamDataPtr(const_cast<TUint8*>(usernameAndPasswordBuffer.Ptr()),
						usernameAndPasswordBuffer.Length(), usernameAndPasswordBuffer.Length());
				
				TBuf8<KMaxPdpAddressLength> primaryAndSecondaryDNSBuffer(KPrimaryAndSecondaryDNSLength + 1);
				
				// Generates the primary and secondary DNS buffers as described in 3GPP specification 3G TS 24.008
				ConvertDNSToPCOBuffer(contextConfigR99R4->iProtocolConfigOption.iDnsAddresses.iPrimaryDns, contextConfigR99R4->iProtocolConfigOption.iDnsAddresses.iSecondaryDns, contextConfigR99R4->iProtocolConfigOption.iId+1, primaryAndSecondaryDNSBuffer);

				// Setting the configurations buffers
				TPtr8 primaryAndSecondaryDNSParamDataPtr(const_cast<TUint8*>(primaryAndSecondaryDNSBuffer.Ptr()),
						primaryAndSecondaryDNSBuffer.Length(), primaryAndSecondaryDNSBuffer.Length());

				// Set up the TTlvStruct to include the pco details
				RPacketContext::TPcoId usenameAndPasswordID(KUsenamePasswordDNSID);
				RPacketContext::TPcoId primaryAndSecondaryDNSID(KPrimaryAndSecondaryDNSID);

				if (tlv.AppendItemL(usenameAndPasswordID, usernameAndPasswordParamDataPtr) !=  KErrOverflow)
					{
					if (tlv.AppendItemL(primaryAndSecondaryDNSID, primaryAndSecondaryDNSParamDataPtr) != KErrOverflow)
						{
						pcoBuffer.SetLength(primaryAndSecondaryDNSParamDataPtr.Length() + usernameAndPasswordParamDataPtr.Length() + 2 * KIDLength);
						pcoBuffer.Append(contextConfigR99R4->iProtocolConfigOption.iMiscBuffer);
						ret = iLtsyDispatchPacketServicesSetPdpContextConfig->HandleSetPdpContextConfigReqL(*contextId, contextConfigR99R4->iAccessPointName,
								contextConfigR99R4->iPdpType, contextConfigR99R4->iPdpAddress, pcoBuffer);
						}
					else
						{
						ret = KErrNotSupported;
						}
					}
				else
					{
					ret = KErrNotSupported;
					}
				}
				break;
			case TPacketDataConfigBase::KConfigRel5:
				{
				RPacketContext::TContextConfig_R5* contextConfigR95 = static_cast<RPacketContext::TContextConfig_R5*>(generalConfig);

				// Protocol Config Option handling
				RPacketContext::TMiscProtocolBuffer pcoBuffer;
				pcoBuffer.Zero();
				TPtr8 pcoPtr (const_cast<TUint8*> (pcoBuffer.Ptr()), RPacketContext::KMiscProtocolBufferLength);

				TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> tlv(pcoPtr,0);

				TBuf8<KMaxUserAndPassLength> usernameAndPasswordBuffer(KMaxUserAndPassLength);
				
				// Generates the username, password and requestID buffer as described in 3GPP specification 3G TS 24.008
				ConvertUsernameAndPasswordToPCOBuffer(contextConfigR95->iProtocolConfigOption.iAuthInfo.iUsername, contextConfigR95->iProtocolConfigOption.iAuthInfo.iPassword, contextConfigR95->iProtocolConfigOption.iId, usernameAndPasswordBuffer);

				TPtr8 usernameAndPasswordParamDataPtr(const_cast<TUint8*>(usernameAndPasswordBuffer.Ptr()),
						usernameAndPasswordBuffer.Length(), usernameAndPasswordBuffer.Length());

				TBuf8<KMaxPdpAddressLength> primaryAndSecondaryDNSBuffer(KPrimaryAndSecondaryDNSLength + 1);

				// Generates the primary and secondary DNS buffers as described in 3GPP specification 3G TS 24.008
				ConvertDNSToPCOBuffer(contextConfigR95->iProtocolConfigOption.iDnsAddresses.iPrimaryDns, contextConfigR95->iProtocolConfigOption.iDnsAddresses.iSecondaryDns, contextConfigR95->iProtocolConfigOption.iId+1, primaryAndSecondaryDNSBuffer);

				// Setting the configurations buffers
				TPtr8 primaryAndSecondaryDNSParamDataPtr(const_cast<TUint8*>(primaryAndSecondaryDNSBuffer.Ptr()),
						primaryAndSecondaryDNSBuffer.Length(), primaryAndSecondaryDNSBuffer.Length());

				// Set up the TTlvStruct to include the pco details
				RPacketContext::TPcoId primaryAndSecondaryDNSID(KPrimaryAndSecondaryDNSID);
				RPacketContext::TPcoId usenameAndPasswordID(KUsenamePasswordDNSID);

				if (tlv.AppendItemL(usenameAndPasswordID, usernameAndPasswordParamDataPtr) !=  KErrOverflow)
					{
					if (tlv.AppendItemL(primaryAndSecondaryDNSID, primaryAndSecondaryDNSParamDataPtr) != KErrOverflow)
						{
						pcoBuffer.SetLength(primaryAndSecondaryDNSParamDataPtr.Length() + usernameAndPasswordParamDataPtr.Length() + 2 * KIDLength);
						pcoBuffer.Append(contextConfigR95->iProtocolConfigOption.iMiscBuffer);
						ret = iLtsyDispatchPacketServicesSetPdpContextConfig->HandleSetPdpContextConfigReqL(*contextId, contextConfigR95->iAccessPointName, contextConfigR95->iPdpType, contextConfigR95->iPdpAddress, pcoBuffer);
						}
					else
						{
						ret = KErrNotSupported;
						}
					}
				else
					{
					ret = KErrNotSupported;
					}
				}
				break;
			default:
				ret = KErrNotSupported;
				break;
			}
		}
	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchSetPdpContextConfigL

TInt CPacketServicesDispatcher::DispatchModifyActivePdpContextL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketContextModifyActiveContext
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPacketServicesModifyActivePdpContext)
		{
		TInfoName* contextName = NULL;
		aDataPackage->UnPackData( &contextName );
		
		__ASSERT_DEBUG(contextName, CtsyDispatcherPanic(EInvalidNullPtr));

		ret = iLtsyDispatchPacketServicesModifyActivePdpContext->HandleModifyActivePdpContextReqL(*contextName);
		}
		
	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchModifyActivePdpContextL

TInt CPacketServicesDispatcher::DispatchInitialisePdpContextL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketContextInitialiseContext
 * and pass request on to Licensee LTSY.
 *
 * @param aDataPackage Package primary and secondary context names
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 *
 * @see RPacketContext::InitialiseContext()
 *
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesInitialisePdpContext)
		{
	
		TInfoName* contextName = NULL;
		TInfoName* hostCID = NULL;

		aDataPackage->UnPackData( &contextName, &hostCID );
		
		__ASSERT_DEBUG(contextName, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(hostCID, CtsyDispatcherPanic(EInvalidNullPtr));	

		if (hostCID->Length() == 0) // this is not a secondary context request
			{
			ret = iLtsyDispatchPacketServicesInitialisePdpContext->HandleInitialisePdpContextReqL(*contextName, KEmptyInfoName);
			}
		else // secondary context request also needs primary context name
			{
			ret = iLtsyDispatchPacketServicesInitialisePdpContext->HandleInitialisePdpContextReqL(*hostCID, *contextName);
			}

		}
	
	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchInitialisePdpContextL

TInt CPacketServicesDispatcher::DispatchDeletePdpContextL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketContextDelete
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 *
 * @see RPacketContext::Delete()
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPacketServicesDeletePdpContext)
		{
		// Unpack the deleted context name.
		TInfoName* contextName = NULL;
		aDataPackage->UnPackData(&contextName);

		__ASSERT_DEBUG(contextName, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchPacketServicesDeletePdpContext->HandleDeletePdpContextReqL(*contextName);
		}
		
	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchDeletePdpContextL

TInt CPacketServicesDispatcher::DispatchSetPacketAttachModeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketSetAttachMode
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPacketServicesSetPacketAttachMode)
		{
		RPacketService::TAttachMode attachMode;
		aDataPackage->UnPackData(attachMode);

		ret = iLtsyDispatchPacketServicesSetPacketAttachMode->HandleSetPacketAttachModeReqL(attachMode);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchSetPacketAttachModeL

TInt CPacketServicesDispatcher::DispatchGetStatusL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	if (iLtsyDispatchPacketServicesGetStatus)
		{
		ret = iLtsyDispatchPacketServicesGetStatus->HandleGetStatusReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchGetStatusL


TInt CPacketServicesDispatcher::DispatchSetDefaultPdpContextParamsL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketSetDefaultContextParams
 * and pass request on to Licensee LTSY.
 *
 * @param aDataPackage Packaged configuration parameters, either GPRS or R99
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	TPacketDataConfigBase* arg1 = NULL;
	aDataPackage->UnPackData( &arg1 );

	__ASSERT_DEBUG(arg1, CtsyDispatcherPanic(EInvalidNullPtr));
	
	switch(arg1->ExtensionId())
		{
		case TPacketDataConfigBase::KConfigGPRS:
       		{
       		if(iLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams)
       			{
           		const RPacketContext::TContextConfigGPRS& contextConfigGprs = *static_cast<RPacketContext::TContextConfigGPRS*>(arg1);
           		ret = iLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams->HandleSetDefaultPdpContextGprsParamsReqL(contextConfigGprs);
       			}
       		}
       		break;

		case TPacketDataConfigBase::KConfigRel99Rel4:
       		{
       		if(iLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params)
       			{
           		const RPacketContext::TContextConfigR99_R4& contextConfigR99R4 = *static_cast<RPacketContext::TContextConfigR99_R4*>(arg1);
           		ret = iLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params->HandleSetDefaultPdpContextR99R4ParamsReqL(contextConfigR99R4);
       			}
       		}
       		break;

		case TPacketDataConfigBase::KConfigRel5:
		case TPacketDataConfigBase::KConfigCDMA:
		default:
       		break;
		}
		
	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchSetDefaultPdpContextParamsL

TInt CPacketServicesDispatcher::DispatchActivatePdpContextL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketContextActivate
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 *
 * @see RPacketContext::Activate()
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesActivatePdpContext)
		{
		// Unpack the activated context name.
		TInfoName* contextName = NULL;
		aDataPackage->UnPackData(&contextName);

		__ASSERT_DEBUG(contextName, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchPacketServicesActivatePdpContext->HandleActivatePdpContextReqL(*contextName);
		}
		
	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchActivatePdpContextL

TInt CPacketServicesDispatcher::DispatchSetPdpContextQosL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketQoSSetProfileParams
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 *
 * @see RPacketQoS::SetProfileParameters()
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	TContextParams*	contextParams = NULL;
	TDes8** qosProfile = NULL;

	aDataPackage->UnPackData(&contextParams, &qosProfile);
	
	__ASSERT_DEBUG(contextParams, CtsyDispatcherPanic(EInvalidNullPtr));
	__ASSERT_DEBUG(qosProfile, CtsyDispatcherPanic(EInvalidNullPtr));

	TPckg<TPacketDataConfigBase>& profilePckg = *static_cast<TPckg<TPacketDataConfigBase>*>(*qosProfile);

	TPacketDataConfigBase& profile = profilePckg();	
	
	// Get what client has requested
	switch(profile.ExtensionId())
		{
		case TPacketDataConfigBase::KConfigGPRS:
			{
			if(iLtsyDispatchPacketServicesSetPdpContextQosGprs)
				{
		        TPckg<RPacketQoS::TQoSGPRSRequested>& gprsProfilePckg = *static_cast<TPckg<RPacketQoS::TQoSGPRSRequested>*>(*qosProfile);
		        RPacketQoS::TQoSGPRSRequested& gprsProfile = gprsProfilePckg();
		        ret = iLtsyDispatchPacketServicesSetPdpContextQosGprs->HandleSetPdpContextQosGprsReqL(contextParams->iContextName, gprsProfile);
				}
        	}
			break;
		case TPacketDataConfigBase::KConfigRel99Rel4:
			{
			if(iLtsyDispatchPacketServicesSetPdpContextQosR99R4)
				{
				TPckg<RPacketQoS::TQoSR99_R4Requested>& r99r4ProfilePckg = *static_cast<TPckg<RPacketQoS::TQoSR99_R4Requested>*>(*qosProfile);
				RPacketQoS::TQoSR99_R4Requested& r99r4Profile = r99r4ProfilePckg();
				ret = iLtsyDispatchPacketServicesSetPdpContextQosR99R4->HandleSetPdpContextQosR99R4ReqL(contextParams->iContextName, r99r4Profile);
				}
			}
			break;
		case TPacketDataConfigBase::KConfigRel5:
			{
			if(iLtsyDispatchPacketServicesSetPdpContextQosR5)
				{
				TPckg<RPacketQoS::TQoSR5Requested>& r5ProfilePckg = *static_cast<TPckg<RPacketQoS::TQoSR5Requested>*>(*qosProfile);
		        RPacketQoS::TQoSR5Requested& r5Profile = r5ProfilePckg();
		        ret = iLtsyDispatchPacketServicesSetPdpContextQosR5->HandleSetPdpContextQosR5ReqL(contextParams->iContextName, r5Profile);
				}
			}
			break;
		default:
       		ret = KErrNotSupported;
       		break;
		}
		
	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchSetPdpContextQosL

TInt CPacketServicesDispatcher::DispatchRejectNetworkInitiatedContextActivationRequestL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 *
 * @see RPacketService::RejectActivationRequest()
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	if (iLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest)
		{
		ret = iLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest->HandleRejectNetworkInitiatedContextActivationRequestReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchRejectNetworkInitiatedContextActivationRequestL

TInt CPacketServicesDispatcher::DispatchDeactivatePdpContextL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketContextDeactivate
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 *
 * @see RPacketContext::Deactivate()
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesDeactivatePdpContext)
		{
		TInfoName* contextName = NULL;
		aDataPackage->UnPackData( &contextName );
			
		__ASSERT_DEBUG(contextName, CtsyDispatcherPanic(EInvalidNullPtr));

		ret = iLtsyDispatchPacketServicesDeactivatePdpContext->HandleDeactivatePdpContextReqL(*contextName);
		}
		
	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchDeactivatePdpContextL

TInt CPacketServicesDispatcher::DispatchAddPacketFilterL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketContextAddPacketFilter
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 * 
 * @see RPacketContext::AddPacketFilter()
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPacketServicesAddPacketFilter)
		{
		TInfoName* secondaryContext = NULL;					
		CArrayFixFlat<RPacketContext::TPacketFilterV2>*  contextFilterArray = NULL;
					
		aDataPackage->UnPackData( &secondaryContext, &contextFilterArray);
		__ASSERT_DEBUG(secondaryContext, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(contextFilterArray, CtsyDispatcherPanic(EInvalidNullPtr));
			
		ret = iLtsyDispatchPacketServicesAddPacketFilter->HandleAddPacketFilterSyncL(*secondaryContext, *contextFilterArray);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchAddPacketFilterL

TInt CPacketServicesDispatcher::DispatchRemovePacketFilterL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketContextRemovePacketFilter
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 * 
 * @see RPacketContext::RemovePacketFilter()
 */
	{
	TSYLOGENTRYEXIT;	
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPacketServicesRemovePacketFilter)
		{
		TInfoName* secondaryContext = NULL;
		TInt* filterId = NULL;				

		aDataPackage->UnPackData( &secondaryContext, &filterId);
		__ASSERT_DEBUG(secondaryContext, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(filterId, CtsyDispatcherPanic(EInvalidNullPtr));
		ret = iLtsyDispatchPacketServicesRemovePacketFilter->HandleRemovePacketFilterSyncL(*secondaryContext, *filterId);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchAddPacketFilterL

TInt CPacketServicesDispatcher::DispatchGetStaticCapabilitiesL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesGetStaticCapabilities)
		{
		ret = iLtsyDispatchPacketServicesGetStaticCapabilities->HandleGetStaticCapabilitiesReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchGetStaticCapabilitiesL

TInt CPacketServicesDispatcher::DispatchGetMaxNoMonitoredServiceListsL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists)
		{
		ret = iLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists->HandleGetMaxNoMonitoredServiceListsReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchGetMaxNoMonitoredServiceListsL

TInt CPacketServicesDispatcher::DispatchGetMaxNoActiveServicesL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesGetMaxNoActiveServices)
		{
		ret = iLtsyDispatchPacketServicesGetMaxNoActiveServices->HandleGetMaxNoActiveServicesReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchGetMaxNoActiveServicesL

TInt CPacketServicesDispatcher::DispatchInitialiseMbmsContextL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECtsyPacketMbmsInitialiseContextReq
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesInitialiseMbmsContext)	
		{
		
		TInfoName* contextName = NULL;
		aDataPackage->UnPackData(&contextName);
		__ASSERT_DEBUG(contextName, CtsyDispatcherPanic(EInvalidNullPtr));
				
		ret = iLtsyDispatchPacketServicesInitialiseMbmsContext->HandleInitialiseMbmsContextReqL(*contextName);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchInitialiseMbmsContextL

TInt CPacketServicesDispatcher::DispatchGetMbmsNetworkServiceStatusL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketGetMbmsNetworkServiceStatus
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus)	
		{
		TBool attemptAttach;
		aDataPackage->UnPackData(attemptAttach);
		
		ret = iLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus->HandleGetMbmsNetworkServiceStatusReqL(attemptAttach);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchGetMbmsNetworkServiceStatusL


TInt CPacketServicesDispatcher::DispatchUpdateMbmsMonitorServiceListL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketUpdateMbmsMonitorServiceList
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList)	
		{
		TMmMbmsMonitorServiceListParams* mbmsMonitorServiceListParams = NULL;
		
		aDataPackage->UnPackData(&mbmsMonitorServiceListParams);
		__ASSERT_DEBUG(mbmsMonitorServiceListParams, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(mbmsMonitorServiceListParams->iServiceList, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(mbmsMonitorServiceListParams->iListOfChanges, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList->HandleUpdateMbmsMonitorServiceListsReqL(	mbmsMonitorServiceListParams->iActionType,
																												*(mbmsMonitorServiceListParams->iServiceList),
																												*(mbmsMonitorServiceListParams->iListOfChanges));
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchUpdateMbmsMonitorServiceListL

TInt CPacketServicesDispatcher::DispatchUpdateMbmsSessionListL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EPacketContextUpdateMbmsSessionList
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPacketServicesUpdateMbmsSessionList)	
		{
		
		TInfoName* contextName = NULL;
		TMmMbmsActiveServiceListParams* mbmsActiveServiceListParams = NULL;
		aDataPackage->UnPackData(&contextName, &mbmsActiveServiceListParams);
		
		__ASSERT_DEBUG(contextName, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(mbmsActiveServiceListParams, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(mbmsActiveServiceListParams->iServiceList, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchPacketServicesUpdateMbmsSessionList->HandleUpdateMbmsSessionListReqL(*contextName, 
																								mbmsActiveServiceListParams->iActionType,
																								mbmsActiveServiceListParams->iChangedItem,
																								mbmsActiveServiceListParams->iServiceList->iSessionIdList);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPacketServicesDispatcher::DispatchUpdateMbmsSessionListL

//
// Callback handlers follow
//

void CPacketServicesDispatcher::CallbackNotifyPacketAttachModeChange(TInt aError, RPacketService::TPreferredBearer aBearer)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketAttachModeChangeInd()
 *
 * @param aError The error code.
 * @param aBearer The new bearer.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aBearer=%d"), aError, aBearer);
	
    // Pack the data to return to the Common TSY
    CMmPacketDataPackage dataPackage;
    dataPackage.PackData(&aBearer);
    iMessageManagerCallback.Complete(EPacketNotifyAttachModeChange, &dataPackage, aError);	
	} // CPacketServicesDispatcher::CallbackNotifyPacketAttachModeChange

void CPacketServicesDispatcher::CallbackNotifyQosProfileChange(TInt aError, const TDesC& aContextName,
		const RPacketQoS::TQoSGPRSNegotiated& aQoSGPRSNegotiated)
/**
 * Callback function to be used by the Licensee LTSY to notify change in the GPRS QoS profile (IPCEPacketQoSNotifyProfileChanged).
 *
 * @param aError The error code.
 * @param aContextName context name to be returned to the CTSY Dispatcher.
 * @param aQoSGPRSNegotiated context negotiated GPRS QOS profile to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	
	CMmPacketDataPackage dataPackage;
	
	RPacketQoS::TQoSR99_R4Negotiated tempR99_R4Negotiated;
	TInt ret = EtelPcktR97NegToEtelPcktR99R4NegQoS(aQoSGPRSNegotiated, tempR99_R4Negotiated);
	RPacketQoS::TQoSR5Negotiated tempR5Negotiated;
	ret = EtelPcktR99R4NegToEtelPcktR5NegQoS(tempR99_R4Negotiated, tempR5Negotiated);
	dataPackage.SetProfileChanged(
		const_cast<RPacketQoS::TQoSGPRSNegotiated*>(&aQoSGPRSNegotiated),
		const_cast<RPacketQoS::TQoSR99_R4Negotiated*>(&tempR99_R4Negotiated),
		const_cast<RPacketQoS::TQoSR5Negotiated*>(&tempR5Negotiated));
	
	//CTSY unpacks as a TInfoName thus safe.
	dataPackage.PackData(const_cast<TDesC*>(&aContextName));
	iMessageManagerCallback.Complete(EPacketQoSNotifyProfileChanged, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyQosProfileChangedGPRS

void CPacketServicesDispatcher::CallbackNotifyQosProfileChange(TInt aError,
		const TDesC& aContextName, const RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated)
/**
 * Callback function to be used by the Licensee LTSY to notify change in the R99R4 QoS profile (IPCEPacketQoSNotifyProfileChanged).
 *
 * @param aError The error code.
 * @param aContextName context name to be returned to the CTSY Dispatcher.
 * @param aQoSR99_R4Negotiated context negotiated R99 QOS profile to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));

	RPacketQoS::TQoSGPRSNegotiated  tempGPRSNegotiated;
	TInt ret = EtelPcktR99R4NegToEtelPcktR97NegQoS(aQoSR99_R4Negotiated, tempGPRSNegotiated);
	
	RPacketQoS::TQoSR5Negotiated tempR5Negotiated;
	ret = EtelPcktR99R4NegToEtelPcktR5NegQoS(aQoSR99_R4Negotiated, tempR5Negotiated);

	CMmPacketDataPackage dataPackage;
	dataPackage.SetProfileChanged(
			const_cast<RPacketQoS::TQoSGPRSNegotiated*>(&tempGPRSNegotiated),
			const_cast<RPacketQoS::TQoSR99_R4Negotiated*>(&aQoSR99_R4Negotiated),
			const_cast<RPacketQoS::TQoSR5Negotiated*>(&tempR5Negotiated));
	
	//CTSY unpacks as a TInfoName thus safe.
	dataPackage.PackData(const_cast<TDesC*>(&aContextName));
	iMessageManagerCallback.Complete(EPacketQoSNotifyProfileChanged, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyQosProfileChangedR99

void CPacketServicesDispatcher::CallbackNotifyQosProfileChange(TInt aError,
		const TDesC& aContextName, const RPacketQoS::TQoSR5Negotiated& aQoSR5Negotiated)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyQosProfileChangedInd()
 *
 * @param aError The error code.
 * @param aContextName Context name to be returned to the CTSY Dispatcher.
 * @param aQoSR5Negotiated Context negotiated R5 QOS profile to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	
	const RPacketQoS::TQoSR99_R4Negotiated* tempR99_R4Negotiated = &aQoSR5Negotiated;

	RPacketQoS::TQoSGPRSNegotiated tempGPRSNegotiated;
	EtelPcktR99R4NegToEtelPcktR97NegQoS(*tempR99_R4Negotiated, tempGPRSNegotiated);

	CMmPacketDataPackage dataPackage;

	dataPackage.SetProfileChanged(
		const_cast<RPacketQoS::TQoSGPRSNegotiated*>(&tempGPRSNegotiated),
		const_cast<RPacketQoS::TQoSR99_R4Negotiated*>(tempR99_R4Negotiated),
		const_cast<RPacketQoS::TQoSR5Negotiated*>(&aQoSR5Negotiated));
	
	//CTSY unpacks as a TInfoName thus safe.
	dataPackage.PackData(const_cast<TDesC*>(&aContextName));
	iMessageManagerCallback.Complete(EPacketQoSNotifyProfileChanged, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyQosProfileChangedR5

void CPacketServicesDispatcher::CallbackNotifyRatTransferCapsChange(TInt aError, TDynamicTransferCapsFlags aCaps)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyRatTransferCapsChangeInd()
 *
 * @param aError The error code.
 * @param aCaps The capabilities flag.
 *
 * @see RPacketService::NotifyDynamicCapsChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCaps=%d"), aError, aCaps);

	// Pack the data to return to the Common TSY
	CMmPacketDataPackage dataPackage;
	dataPackage.PackData(&aCaps);
	iMessageManagerCallback.Complete(EPacketNotifyTransferCapsIPC, &dataPackage, aError);
	} // CPacketServicesDispatcher::CallbackNotifyRatTransferCapsChange

void CPacketServicesDispatcher::CallbackNotifyPacketMsClassChange(TInt aError, RPacketService::TMSClass aMSClass)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketMsClassChangeInd()
 *
 * @param aError The error code.
 * @param aMSClass MS class to be returned to the CTSY.
 *
 * @see RPacketService::NotifyMSClassChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMSClass=%d"), aError, aMSClass);
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aMSClass);
	iMessageManagerCallback.Complete(EPacketNotifyMSClassChange, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyPacketMsClassChange

void CPacketServicesDispatcher::CallbackNotifyPacketDynamicCapsChange(TInt aError, TBool aGprsIsSupportedOnCell, TBool aCsServicesIsSupportedOnCell)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketDynamicCapsChangeInd()
 *
 * @param aError The error code.
 * @param aGprsIsSupportedOnCell indication whether GPRS is supported and should be returned to the CTSY Dispatcher.
 * @param aCsServicesIsSupportedOnCell indication whether CS services are supported and should be returned to the CTSY Dispatcher.
 *
 * @see RPacketService::NotifyDynamicCapsChange()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aGprsIsSupportedOnCell=%d, aCsServicesIsSupportedOnCell=%d"), aError, aGprsIsSupportedOnCell, aCsServicesIsSupportedOnCell);
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aGprsIsSupportedOnCell, &aCsServicesIsSupportedOnCell);
	iMessageManagerCallback.Complete(EPacketNotifyDynamicCapsChange, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyPacketDynamicCapsChange

void CPacketServicesDispatcher::CallbackNotifyPdpContextAdded(TInt aError, const TDesC& aContextName)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextAddedInd()
 *
 * @param aError The error code.
 * @param aContextName PDP context name to be returned.
 *
 * @see RPacketService::NotifyContextAdded()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));

	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName.Left(KMaxInfoName));
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName, &aError);
	iMessageManagerCallback.Complete(EPacketNotifyContextAdded, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyPdpContextAdded

void CPacketServicesDispatcher::CallbackNotifyNetworkInitiatedContextActivationRequest(TInt aError,
		const RPacketContext::TContextConfigGPRS& aContextConfigGPRS)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd()
 *
 * @param aError The error code.
 * @param aContextConfigGPRS The GPRS context configuration to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPdpType=%d"), aError, aContextConfigGPRS.iPdpType);

	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RPacketContext::TContextConfigGPRS*> (&aContextConfigGPRS));
	iMessageManagerCallback.Complete(EPacketNotifyContextActivationRequested, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyNetworkInitiatedContextActivationRequest

void CPacketServicesDispatcher::CallbackNotifyNetworkInitiatedContextActivationRequest(TInt aError,
		const RPacketContext::TContextConfigR99_R4& aContextConfigR99_R4)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd()
 *
 * @param aError The error code.
 * @param aContextConfigR99_R4 The release 99 or release 4 context configuration to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPdpType=%d"), aError, aContextConfigR99_R4.iPdpType);

	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RPacketContext::TContextConfigR99_R4*> (&aContextConfigR99_R4));
	iMessageManagerCallback.Complete(EPacketNotifyContextActivationRequested, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyNetworkInitiatedContextActivationRequest

void CPacketServicesDispatcher::CallbackNotifyNetworkInitiatedContextActivationRequest(TInt aError,
		const RPacketContext::TContextConfig_R5& aContextConfig_R5)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd()
 *
 * @param aError The error code.
 * @param aContextConfig_R5 The release 5 context configuration to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPdpType=%d"), aError, aContextConfig_R5.iPdpType);

	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RPacketContext::TContextConfig_R5*> (&aContextConfig_R5));
	iMessageManagerCallback.Complete(EPacketNotifyContextActivationRequested, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyNetworkInitiatedContextActivationRequest


void CPacketServicesDispatcher::CallbackNotifyPdpContextConfigChanged(TInt aError,
		const TDesC& aContextName, const TDesC8& aAccessPointName,
		const TDesC8& aPdpAddress, TUint aPdpCompression, const TDesC8& aPcoBuffer)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextConfigChangedInd()
 *
 * @param aError The error code.
 *
 * @param aContextName The name of the context which the change in configuration refers to.
 *
 * @param aAccessPointName The name of the access point. This should be a descriptor of maximum length
 * RPacketContext::KGSNNameLength such as a RPacketContext::TGSNAddress
 *
 * @param aPdpAddress The protocol address. This should be a descriptor of maximum length
 * RPacketContext::KMaxPDPAddressLength such as a RPacketContext::TProtocolAddress
 *
 * @param aPdpCompression The PDP compression level.
 *
 * @param aPcoBuffer The PCO buffer. This should be a descriptor of maximum length
 * RPacketContext::KMiscProtocolBufferLength such as a RPacketContext::TMiscProtocolBuffer.
 *
 * @see RPacketContext::NotifyConfigChanged()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aPdpCompression=%d"), aError, &aContextName, aPdpCompression);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	__ASSERT_DEBUG(aAccessPointName.Length() <= RPacketContext::KGSNNameLength, CtsyDispatcherPanic(EBadLength));
	__ASSERT_DEBUG(aPdpAddress.Length() <= RPacketContext::KMaxPDPAddressLength, CtsyDispatcherPanic(EBadLength));
	__ASSERT_DEBUG(aPcoBuffer.Length() >= KHeaderGap + KIPv4Len + KLengthLocation + KMinPCOSize, CtsyDispatcherPanic(EBadLength));

	RPacketContext::TContextConfigGPRS contextConfig;

	// Decode the PCO buffer and set the profile
	DecodePCO(aPcoBuffer, contextConfig.iProtocolConfigOption.iAuthInfo.iUsername,
			contextConfig.iProtocolConfigOption.iAuthInfo.iPassword,
			contextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns,
			contextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns,
			contextConfig.iProtocolConfigOption.iMiscBuffer);

	// Re-constuct the profile
	contextConfig.iAccessPointName.Copy(aAccessPointName.Left(RPacketContext::KGSNNameLength));
	contextConfig.iPdpAddress.Copy(aPdpAddress.Left(RPacketContext::KMaxPDPAddressLength));
	contextConfig.iPdpCompression = aPdpCompression;

	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName.Left(KMaxInfoName));
	
	CMmPacketDataPackage dataPackage;
	dataPackage.PackData(&contextName, &contextConfig);
	iMessageManagerCallback.Complete(EPacketContextNotifyConfigChanged, &dataPackage, aError);
	} // CPacketServicesDispatcher::CallbackNotifyPdpContextConfigChanged

void CPacketServicesDispatcher::CallbackNotifyPdpContextStatusChange(TInt aError, const TDesC& aContextName, const TContextMisc& aContextMisc)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyPdpContextStatusChangeInd()
 *
 * @param aError The error code to be returned.
 * @param aContextName context name.
 * @param aContextMisc context miscellaneous information 
 * 
 * @see TContextMisc.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));

	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName.Left(KMaxInfoName));
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName, const_cast<TContextMisc*>(&aContextMisc));
	iMessageManagerCallback.Complete(EPacketContextNotifyStatusChange, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyPdpContextStatusChange

void CPacketServicesDispatcher::CallbackNotifyPdpContextConnectionSpeedChange(TInt aError, const TDesC& aContextName, TUint aConnectionSpeed)
/**
 * Callback function to be used by the Licensee LTSY to notify context connection speed change (IPC EPacketContextNotifyConnectionSpeedChange).
 *
 * @param aError The error code to be returned.
 * @param aContextName context name.
 * @param aConnectionSpeed context connection speed in bits per second.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aConnectionSpeed=%d"), aError, &aContextName, aConnectionSpeed);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	
	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName.Left(KMaxInfoName));
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName, &aConnectionSpeed);
	iMessageManagerCallback.Complete(EPacketContextNotifyConnectionSpeedChange, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyPdpContextConnectionSpeedChange

void CPacketServicesDispatcher::CallbackNotifyPacketNetworkRegistrationStatus(TInt aError, const RPacketService::TRegistrationStatus aRegistrationStatus)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyPacketNetworkRegistrationStatusInd()
 *
 *
 * @param aError The error code.
 * @param aRegistrationStatus network registration status.
 *
 * @see RPacketService::NotifyChangeOfNtwkRegStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRegistrationStatus=%d"), aError, aRegistrationStatus);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RPacketService::TRegistrationStatus*>(&aRegistrationStatus));
	iMessageManagerCallback.Complete(EPacketNotifyChangeOfNtwkRegStatus, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyPacketNetworkRegistrationStatus


void CPacketServicesDispatcher::CallbackNotifyMbmsContextConfigChanged(TInt aError, const TDesC& aContextName, const RPacketMbmsContext::TContextConfigMbmsV1& aContextConfigMbms)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyMbmsContextConfigChangedInd()
 *
 *
 * @param aError The error code to be returned.
 * @param aContextName The context name.
 * @param aContextConfigMbms The context configuration data.
 *
 * @see RPacketMbmsContext::NotifyConfigChanged()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, sId=%d, mcc=%d, mnc=%d, ab=%d, sp=%d, sm=%d, sf=%d"),
			aError, 
			&aContextName, 
			aContextConfigMbms.iTmgi.GetServiceId(),  
			aContextConfigMbms.iTmgi.GetMCC(),
			aContextConfigMbms.iTmgi.GetMNC(),
			aContextConfigMbms.iMbmsAccessBearer,
			aContextConfigMbms.iMbmsServicePriority,
			aContextConfigMbms.iMbmsServiceMode,
			aContextConfigMbms.iMbmsSessionFlag);
	
	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
		
	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName.Left(KMaxInfoName));
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName, &const_cast<RPacketMbmsContext::TContextConfigMbmsV1&>(aContextConfigMbms));
	iMessageManagerCallback.Complete(ECtsyPacketMbmsContextNotifyConfigChanged, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackNotifyMbmsContextConfigChanged

void CPacketServicesDispatcher::CallbackPacketAttach(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesPacketAttachComp()
 *
 * @param aError The error code.
 *
 * @see RPacketService::Attach()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	iMessageManagerCallback.Complete(EPacketAttach, aError);
	} // CPacketServicesDispatcher::CallbackPacketAttach

void CPacketServicesDispatcher::CallbackGetPacketAttachMode(TInt aError, RPacketService::TAttachMode aAttachMode)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesGetPacketAttachModeComp()
 *
 * @param aError The error code.
 * @param aAttachMode The packet attach mode.
 *
 * @see RPacketService::GetAttachMode()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAttachMode=%d"), aError, aAttachMode);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aAttachMode);

	iMessageManagerCallback.Complete(EPacketGetAttachMode, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackGetPacketAttachMode

void CPacketServicesDispatcher::CallbackGetPacketNetworkRegistrationStatus(TInt aError,
		RPacketService::TRegistrationStatus aRegistrationStatus)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesGetPacketNetworkRegistrationStatusComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aRegistrationStatus The current network registration status.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRegistrationStatus=%d"), aError, aRegistrationStatus);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aRegistrationStatus);

	iMessageManagerCallback.Complete(EPacketGetNtwkRegStatus, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackGetPacketNetworkRegistrationStatus

void CPacketServicesDispatcher::CallbackPacketDetach(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesPacketDetachComp()
 *
 * @param aError The error code.
 *
 * @see RPacketService::Detach()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	iMessageManagerCallback.Complete(EPacketDetach, aError);

	} // CPacketServicesDispatcher::CallbackPacketDetach

void CPacketServicesDispatcher::CallbackSetPdpContextConfig(TInt aError, const TDesC& aContextName)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextConfigComp()
 *
 * @param aError The error code.
 * @param callbackContextId the context name, the maximum length of the descriptor should not exceed KMaxInfoName.
 *
 * @see RPacketContext::SetConfig()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	
	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName);

	iMessageManagerCallback.Complete(EPacketContextSetConfig, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackSetPdpContextConfig

void CPacketServicesDispatcher::CallbackModifyActivePdpContext(TInt aError, const TDesC& aContextName, TInt8 aRejectionCode)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesModifyActivePdpContextComp()
 *
 * @param aError The error code.
 * @param aContextName The context name which has been modified.
 * @param aRejectionCode The reason code for any failure of modification.
 *
 * @see RPacketContext::ModifyActiveContext()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aRejectionCode=%d"), aError, &aContextName, aRejectionCode);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	typedef TBuf8<1> TRejectionCode;
	TRejectionCode rejectionCode;
	rejectionCode.Append(aRejectionCode);
	
	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName);
	
	dataPackage.PackData(&contextName, &rejectionCode);

	iMessageManagerCallback.Complete(EPacketContextModifyActiveContext, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackModifyActivePdpContext

void CPacketServicesDispatcher::CallbackInitialisePdpContext(TInt aError, const TDesC& aContextName, const TDesC& aChannelId)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesInitialisePdpContextComp()
 *
 * @param aError The error code.
 * @param aContextName The name of the current context, the descriptor should be TBuf<10>
 * @param aChannelId The current channelId descriptor, the descriptor should be a TName
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aChannelId=%S"), aError, &aContextName, &aChannelId);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	__ASSERT_DEBUG(aChannelId.Length() <= KMaxName, CtsyDispatcherPanic(EBadLength));

	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName, &const_cast<TDesC&>(aChannelId));

	iMessageManagerCallback.Complete(EPacketContextInitialiseContext, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackInitialisePdpContext

void CPacketServicesDispatcher::CallbackDeletePdpContext(TInt aError, const TDesC& aContextName)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesDeletePdpContextComp()
 *
 * @param aError The error code.
 * @param aContextName The context name.
 *
 * @see RPacketContext::Delete()
 */
	{
    TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

    __ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
    
    //CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
    TInfoName contextName(aContextName);
    
    // Pack the data to return to the Common TSY
    CMmDataPackage dataPackage;
    dataPackage.PackData(&contextName);

    iMessageManagerCallback.Complete(EPacketContextDelete, &dataPackage, aError);	
	} // CPacketServicesDispatcher::CallbackDeletePdpContext

void CPacketServicesDispatcher::CallbackSetPacketAttachMode(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesSetPacketAttachModeComp()
 *
 * @param aError The error code.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	iMessageManagerCallback.Complete(EPacketSetAttachMode, aError);

	} // CPacketServicesDispatcher::CallbackSetPacketAttachMode

void CPacketServicesDispatcher::CallbackSetDefaultPdpContextParams(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesSetDefaultPdpContextParamsComp()
 *
 * @param aError The error code.
 *
 * @see RPacketContext::SetDefaultContextParams()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EPacketSetDefaultContextParams, aError);
	} // CPacketServicesDispatcher::CallbackSetDefaultPdpContextParams

void CPacketServicesDispatcher::CallbackActivatePdpContext(TInt aError,
		const TDesC& aContextName,
		const TDesC8& aAccessPointName,
		RPacketContext::TProtocolType aPdpType)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesActivatePdpContextComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aContextName The name of the context that has been activated.
 * @param aAccessPointName The name of the access point that has been activated.
 * This should be a descriptor of maximum length RPacketContext::KGSNNameLength such as RPacketContext::TGSNAddress
 * @param aPdpType The protocol used to connect to the packet data gateway.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aAccessPointName=%S, aPdpType=%d"), aError, &aContextName, &aAccessPointName, aPdpType);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	__ASSERT_DEBUG(aAccessPointName.Length() <= RPacketContext::KGSNNameLength, CtsyDispatcherPanic(EBadLength));

	RPacketContext::TContextConfigGPRS contextConfig;
	contextConfig.iAccessPointName.Copy(aAccessPointName);
	contextConfig.iPdpType = aPdpType;
	const RPacketContext::TContextConfigGPRS* contextConfigPtr = &contextConfig;

	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName.Left(KMaxInfoName));

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName, &contextConfigPtr);

	iMessageManagerCallback.Complete(EPacketContextActivate, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackActivatePdpContext

void CPacketServicesDispatcher::CallbackSetPdpContextQos(TInt aError, const TDesC& aContextName)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesSetPdpContextQosComp()
 *
 * @param aError The error code.
 * @param aContextName The context's name, the maximum length of the descriptor should not exceed KMaxInfoName.
 *
 * @see RPacketContext::SetProfileParameters()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	//CTSY unpacks as a TInfoName so safe to pack the TDesC& here.
	dataPackage.PackData(const_cast<TDesC*>(&aContextName));

	iMessageManagerCallback.Complete(EPacketQoSSetProfileParams, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackSetPdpContextQos

void CPacketServicesDispatcher::CallbackRejectNetworkInitiatedContextActivationRequest(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesRejectNetworkInitiatedContextActivationRequestComp()
 *
 * @param aError The error code.
 *
 * @see RPacketContext::RejectActivationRequest()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	iMessageManagerCallback.Complete(EPacketRejectActivationRequest, aError);

	} // CPacketServicesDispatcher::CallbackRejectNetworkInitiatedContextActivationRequest

void CPacketServicesDispatcher::CallbackDeactivatePdpContext(TInt aError, const TDesC& aContextName)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesDeactivatePdpContextComp()
 *
 * @param aError The error code.
 * @param aContextName The context name that has been deactivated,
 * The maximum length of the descriptor should not exceed KMaxInfoName.
 *
 * @see RPacketContext::Deactivate()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	
	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName.Left(KMaxInfoName));
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName);

	iMessageManagerCallback.Complete(EPacketContextDeactivate, &dataPackage, aError);
	} // CPacketServicesDispatcher::CallbackDeactivatePdpContext
	
void CPacketServicesDispatcher::CallbackGetStatus(TInt aError, RPacketService::TStatus aPacketStatus, TBool aIsResumed)
/**
 * Callback function to be used by the request to complete the packet status.
 *
 * @param aError The error code.
 * @param aPacketStatus The packet status as returned from LTSY
 * @param aIsResumed Flag indicating whether packet service has been resumed from suspended state.
 * 
 * @see RPacketService::GetStatus()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPacketStatus=%d, aIsResumed=%d"), aError, aPacketStatus, aIsResumed);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;

	dataPackage.PackData(&aPacketStatus, &aIsResumed);

	iMessageManagerCallback.Complete(EPacketNotifyStatusChange, &dataPackage, aError);

	} // CPacketServicesDispatcher::CallbackGetStatus


void CPacketServicesDispatcher::CallbackGetStaticCapabilities(TInt aError, TUint aStaticCapabilities)
/**
 * Callback function to be used by the request to complete the packet static capabilities.
 * CCtsyDispatcherCallback::CallbackPacketServicesGetStaticCapabilitiesComp()
 *
 * @param aError The error code.
 * @param aStaticCapabilities A bit field containing the static capabilities to be returned to the CTSY.
 * This can be created from a bit-wise OR (|) of values from RPacketService::TStaticMiscCaps.
 * 
 * @see RPacketService::TStaticMiscCaps
 * @see RPacketService::GetStaticCaps()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStaticCapabilities=%d"), aError, aStaticCapabilities);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	
	dataPackage.PackData(&aStaticCapabilities);
	
	iMessageManagerCallback.Complete(EPacketGetStaticCaps, &dataPackage, aError);
	
	} // CPacketServicesDispatcher::CallbackGetStaticCapabilities
	
void CPacketServicesDispatcher::CallbackGetMaxNoMonitoredServiceLists(TInt aError, TInt aMaxNoMonitoredServiceLists)
/**
 * Callback function to be used by the request to complete the maximum number of monitored service lists
 * CCtsyDispatcherCallback::CallbackPacketServicesGetMaxNoMonitoredServiceListsComp()
 *
 * @param aError The error code.
 * @param aMaxNoMonitoredServiceLists The maximum number of monitored service lists.
 * 
 * @see RPacketService::EnumerateMbmsMonitorServiceList()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMaxNoMonitoredServiceLists=%d"), aError, aMaxNoMonitoredServiceLists);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	
	dataPackage.PackData(&aMaxNoMonitoredServiceLists);
	
	iMessageManagerCallback.Complete(EPacketEnumerateMbmsMonitorServiceList, &dataPackage, aError);
	
	} // CPacketServicesDispatcher::CallbackGetMaxNoMonitoredServiceLists

void CPacketServicesDispatcher::CallbackGetMaxNoActiveServices(TInt aError, TInt aMaxNoActiveServices)
/**
 * Callback function to be used by the request to complete the maximum number of active services.
 * CCtsyDispatcherCallback::CallbackPacketServicesGetMaxNoActiveServicesComp()
 *
 * @param aError The error code.
 * @param aMaxNoActiveServices The maximum number of active services allowed (hence the maximum number of contexts allowed).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMaxNoActiveServices=%d"), aError, aMaxNoActiveServices);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	
	dataPackage.PackData(&aMaxNoActiveServices);
	
	iMessageManagerCallback.Complete(EPacketEnumerateMbmsActiveServiceList, &dataPackage, aError);
	
	} // CPacketServicesDispatcher::CallbackGetMaxNoActiveServices

void CPacketServicesDispatcher::CallbackInitialiseMbmsContext(TInt aError, const TDesC& aContextName, const TDesC& aChannelId)
/**
 * Callback function to be used by the request to complete the maximum number of active services.
 * CCtsyDispatcherCallback::CallbackPacketServicesInitialiseMbmsContextComp()
 *
 * @param aError The error code.
 * @param aContextName The name of the current context, the descriptor should be a maximum length of KMaxInfoName.
 * @param aChannelId The current channelId descriptor, the descriptor should be a maximum length of KMaxName.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, aChannelId=%S"), aError, &aContextName, &aChannelId);

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	__ASSERT_DEBUG(aChannelId.Length() <= KMaxName, CtsyDispatcherPanic(EBadLength));
		
	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName.Left(KMaxInfoName));
	
	RPacketContext::TDataChannelV2 dataChannelV2;
	dataChannelV2.iChannelId = aChannelId.Left(KMaxName);
	dataChannelV2.iPort = aContextName.Left(KMaxInfoName);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName,&dataChannelV2);
	
	iMessageManagerCallback.Complete(ECtsyPacketMbmsInitialiseContextComp, &dataPackage, aError);
	
	} // CPacketServicesDispatcher::CallbackInitialiseMbmsContext

void CPacketServicesDispatcher::CallbackGetMbmsNetworkServiceStatus(TInt aError, TMbmsNetworkServiceStatus aMbmsNetworkServiceStatus)
/**
 * Callback function to be used by the request to complete the MBMS network service status.
 * CCtsyDispatcherCallback::CallbackPacketServicesGetMbmsNetworkServiceStatusComp()
 *
 * @param aError The error code.
 * @param aMbmsNetworkServiceStatus The network service status.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMbmsNetworkServiceStatus=%d"), aError, aMbmsNetworkServiceStatus);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aMbmsNetworkServiceStatus);
	
	iMessageManagerCallback.Complete(EPacketGetMbmsNetworkServiceStatus, &dataPackage, aError);
	
	} // CPacketServicesDispatcher::CallbackGetMbmsNetworkServiceStatus

void CPacketServicesDispatcher::CallbackNotifyMbmsNetworkServiceStatusChange(TInt aError, TMbmsNetworkServiceStatus aMbmsNetworkServiceStatus)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyMbmsNetworkServiceStatusChangeInd()
 * 
 * @param aError The error code.
 * @param aMbmsNetworkServiceStatus The network service status.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aMbmsNetworkServiceStatus=%d"), aError, aMbmsNetworkServiceStatus);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aMbmsNetworkServiceStatus);
	
	iMessageManagerCallback.Complete(EPacketNotifyMbmsNetworkServiceStatusChange, &dataPackage, aError);
	
	} // CPacketServicesDispatcher::CallbackNotifyMbmsNetworkServiceStatusChange


void CPacketServicesDispatcher::CallbackUpdateMbmsMonitorServiceList(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsMonitorServiceListComp()
 * 
 * @param aError The error code.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(EPacketUpdateMbmsMonitorServiceList, KErrNone);
	
	(void)aError; // Suppress compiler warning about unused argument in UREL
	} // CPacketServicesDispatcher::CallbackUpdateMbmsMonitorServiceList

void CPacketServicesDispatcher::CallbackUpdateMbmsMonitorServiceList(TInt aError, const RArray<TUint>& aFailedServiceIds)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsMonitorServiceListComp()
 * 
 * @param aFailedServiceIds A list containing the service Id which cannot be updated via the action (e.g. Id of
 * a service that could not be added)
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aFailedServiceIds.Count()=%d"), aError, aFailedServiceIds.Count());
	
	CPcktMbmsMonitoredServiceList* list = NULL;

	//if the list creation leaves then list will be NULL, which the CTSY can handle.
	TRAPD(err,list = CreateServiceListL(aFailedServiceIds));
	__ASSERT_DEBUG(err == KErrNone, CtsyDispatcherPanic(ELeaveTrapped));
	
	CMmDataPackage dataPackage;
	
	if(err == KErrNone)
		{
		//no real problem packing if list is NULL, mainly so err does not cause a UREL build warning as its not used.
		dataPackage.PackData(list);
		}
	
	iMessageManagerCallback.Complete(EPacketUpdateMbmsMonitorServiceList, &dataPackage, KErrMbmsImpreciseServiceEntries);

	(void)aError; // Suppress compiler warning about unused argument in UREL
	delete list;
	list = NULL;
	} // CPacketServicesDispatcher::CallbackUpdateMbmsMonitorServiceList

void CPacketServicesDispatcher::CallbackNotifyMbmsServiceAvailabilityChange(TInt aError, const RArray<TUint>& aAvailableServiceIds)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyMbmsServiceAvailabilityChangeInd()
 * 
 * @param aError The error code.
 * @param aAvailableServiceIds The list of Service Ids which are currently available.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAvailableServiceIds.Count()=%d"), aError,  aAvailableServiceIds.Count());
	
	if(aError == KErrNone)
		{
		CPcktMbmsMonitoredServiceList* list = NULL;

		//if the list creation leaves then list will be NULL, which the CTSY can handle.
		TRAPD(err,list = CreateServiceListL(aAvailableServiceIds));

		CMmDataPackage dataPackage;
		dataPackage.PackData(list);
		
		iMessageManagerCallback.Complete(EPacketNotifyMbmsServiceAvailabilityChange, &dataPackage, err);
		
		delete list;
		list = NULL;
		
		}
	else
		{
		CMmDataPackage dataPackage;
		iMessageManagerCallback.Complete(EPacketNotifyMbmsServiceAvailabilityChange, &dataPackage, aError);
		}
	
	} //CPacketServicesDispatcher::CallbackNotifyMbmsServiceAvailabilityChange

void CPacketServicesDispatcher::CallbackUpdateMbmsSessionList(TInt aError, const TDesC& aContextName)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesUpdateMbmsSessionListComp()
 * 
 * @param aError The error code.
 * @param aContextName The name of the MBMS context, the descriptor should be a maximum length of KMaxInfoName.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S"), aError, &aContextName);
	
	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));
	
	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName.Left(KMaxInfoName));
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName);
			
	iMessageManagerCallback.Complete(EPacketContextUpdateMbmsSessionList, &dataPackage, aError);
	} //CPacketServicesDispatcher::CallbackUpdateMbmsSessionList

void CPacketServicesDispatcher::CallbackNotifyConnectionInfoChange(TInt aError, const TDesC& aContextName, const RPacketContext::TConnectionInfoV1& aConnectionInfo)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPacketServicesNotifyConnectionInfoChangeInd()
 * 
 * @param aError The error code to be returned.
 * @param aContextName The context name, the maximum length of the descriptor should not exceed KMaxInfoName.
 * @param aConnectionInfo The details of the connection, including HSDPA Category and HSUPA Category
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aContextName=%S, iHSDPACategory=%d, iHSUPACategory=%d"), 
	    aError, &aContextName, aConnectionInfo.iHSDPACategory, aConnectionInfo.iHSUPACategory);	

	__ASSERT_DEBUG(aContextName.Length() <= KMaxInfoName, CtsyDispatcherPanic(EBadLength));	

	//CTSY unpacks as a TInfoName* so we have to copy the TDesC& here to be safe.
	TInfoName contextName(aContextName.Left(KMaxInfoName));	

	CMmDataPackage dataPackage;
	dataPackage.PackData(&contextName, &const_cast<RPacketContext::TConnectionInfoV1&>(aConnectionInfo));
			
	iMessageManagerCallback.Complete(EPacketContextNotifyConnectionInfoChange, &dataPackage, aError);	
	}

void CPacketServicesDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
/**
 * Part of the MDispatcherCallback interface. Used to complete requests handled
 * synchronously by the Licensee LTSY asynchronously back to the Common TSY.
 *
 * @param aIpcDataPackage Package encapsulating the request.
 *
 * @see MDispatcherCallback::CallbackSync
 */
	{
	TSYLOGENTRYEXIT;

	switch (aIpcDataPackage.iIpc)
		{

	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPACKETSERVICESDISPATCHER_CALLBACKSYNC_1, "WARNING: CPacketServicesDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;
		} // switch (aIpcDataPackage.iIpc)

	} // CPacketServicesDispatcher::CallbackSync

/**
* Perform conversion from R99 negotiated QoS to R97 negotiated QoS according to 3GPP 24.008-750
*
* @param aQoSR99_R4Negotiated - Etel R99_R4 negotiated QoS
* @param aQoSGPRSNegotiated - Etel GPRS negotiated QoS
* @return Etel error code
**/
TInt CPacketServicesDispatcher::EtelPcktR99R4NegToEtelPcktR97NegQoS(const RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
                                           RPacketQoS::TQoSGPRSNegotiated& aQoSGPRSNegotiated)
    {

    RPacketQoS::TQoSGPRSRequested generatedGPRSQoS;

    /* Traffic class/handling priority to delay */
    if ((aQoSR99_R4Negotiated.iTrafficClass == RPacketQoS::ETrafficClassConversational) || (aQoSR99_R4Negotiated.iTrafficClass == RPacketQoS::ETrafficClassStreaming))
        {
        aQoSGPRSNegotiated.iDelay = RPacketQoS::EDelayClass1;
        }
    else if ((aQoSR99_R4Negotiated.iTrafficClass == RPacketQoS::ETrafficClassInteractive) && (aQoSR99_R4Negotiated.iTrafficHandlingPriority == RPacketQoS::ETrafficPriority1))
        {
        aQoSGPRSNegotiated.iDelay = RPacketQoS::EDelayClass1;
        }
    else if ((aQoSR99_R4Negotiated.iTrafficClass == RPacketQoS::ETrafficClassInteractive) && (aQoSR99_R4Negotiated.iTrafficHandlingPriority == RPacketQoS::ETrafficPriority2))
        {
        aQoSGPRSNegotiated.iDelay = RPacketQoS::EDelayClass2;
        }
    else if ((aQoSR99_R4Negotiated.iTrafficClass == RPacketQoS::ETrafficClassInteractive) && (aQoSR99_R4Negotiated.iTrafficHandlingPriority == RPacketQoS::ETrafficPriority3))
        {
        aQoSGPRSNegotiated.iDelay = RPacketQoS::EDelayClass3;
        }
    else if (aQoSR99_R4Negotiated.iTrafficClass == RPacketQoS::ETrafficClassBackground)
        {
        aQoSGPRSNegotiated.iDelay = RPacketQoS::EDelayClass4;
        }
    else
        {
        aQoSGPRSNegotiated.iDelay = RPacketQoS::EUnspecifiedDelayClass;
        }

    /* Error Ratio/max BER to reliability */
    if (aQoSR99_R4Negotiated.iSDUErrorRatio == RPacketQoS::ESDUErrorRatioOnePerHundredThousand) // Assumption this means < 0.00001
        {
        aQoSGPRSNegotiated.iReliability = RPacketQoS::EReliabilityClass2;
        }
    else if (aQoSR99_R4Negotiated.iSDUErrorRatio == RPacketQoS::ESDUErrorRatioOnePerTenThousand) // Assumption x between 0.00001 and 0.0005 best representation ESDUErrorRatioOnePerTenThousand
        {
        aQoSGPRSNegotiated.iReliability = RPacketQoS::EReliabilityClass3;
        }
    else if ((aQoSR99_R4Negotiated.iSDUErrorRatio == RPacketQoS::ESDUErrorRatioOnePerThousand) && (aQoSR99_R4Negotiated.iBER == RPacketQoS::EBEROnePerHundredThousand)) // Assumption > 0.0005 SDU & <= 0.0002 BER
        {
        aQoSGPRSNegotiated.iReliability = RPacketQoS::EReliabilityClass4;
        }
    else if ((aQoSR99_R4Negotiated.iSDUErrorRatio == RPacketQoS::ESDUErrorRatioOnePerThousand) && (aQoSR99_R4Negotiated.iBER == RPacketQoS::EBEROnePerThousand)) // Assumption > 0.0005 & > 0.0002 BER
        {
        aQoSGPRSNegotiated.iReliability = RPacketQoS::EReliabilityClass5;
        }
    else
        {
        aQoSGPRSNegotiated.iReliability = RPacketQoS::EUnspecifiedReliabilityClass;
        }

    /* Max rate to peak throughput, using maximum between uplink and downlink */
    TInt tempMaxBitRate = Max(aQoSR99_R4Negotiated.iMaxRate.iDownlinkRate, aQoSR99_R4Negotiated.iMaxRate.iUplinkRate);

    if (tempMaxBitRate < 16)
        {
        aQoSGPRSNegotiated.iPeakThroughput = RPacketQoS::EPeakThroughput1000;
        }
    else if ((tempMaxBitRate >= 16) && (tempMaxBitRate < 32))
        {
        aQoSGPRSNegotiated.iPeakThroughput = RPacketQoS::EPeakThroughput2000;
        }
    else if ((tempMaxBitRate >= 32) && (tempMaxBitRate < 64))
        {
        aQoSGPRSNegotiated.iPeakThroughput = RPacketQoS::EPeakThroughput4000;
        }
    else if ((tempMaxBitRate >= 64) && (tempMaxBitRate < 128))
        {
        aQoSGPRSNegotiated.iPeakThroughput = RPacketQoS::EPeakThroughput8000;
        }
    else if ((tempMaxBitRate >= 128) && (tempMaxBitRate < 256))
        {
        aQoSGPRSNegotiated.iPeakThroughput = RPacketQoS::EPeakThroughput16000;
        }
    else if ((tempMaxBitRate >= 256) && (tempMaxBitRate < 512))
        {
        aQoSGPRSNegotiated.iPeakThroughput = RPacketQoS::EPeakThroughput32000;
        }
    else if ((tempMaxBitRate >= 512) && (tempMaxBitRate < 1024))
        {
        aQoSGPRSNegotiated.iPeakThroughput = RPacketQoS::EPeakThroughput64000;
        }
    else if ((tempMaxBitRate >= 1024) && (tempMaxBitRate < 2048))
        {
        aQoSGPRSNegotiated.iPeakThroughput = RPacketQoS::EPeakThroughput128000;
        }        
    else 
        {
        aQoSGPRSNegotiated.iPeakThroughput = RPacketQoS::EPeakThroughput256000;
        }
  
    /* Traffic handling priority to precedence */
    switch (aQoSR99_R4Negotiated.iTrafficHandlingPriority)
    	{
	case RPacketQoS::ETrafficPriority1:
		aQoSGPRSNegotiated.iPrecedence = RPacketQoS::EPriorityHighPrecedence;
		break;
	case RPacketQoS::ETrafficPriority2:
		aQoSGPRSNegotiated.iPrecedence = RPacketQoS::EPriorityMediumPrecedence;
		break;
	case RPacketQoS::ETrafficPriority3:
		aQoSGPRSNegotiated.iPrecedence = RPacketQoS::EPriorityLowPrecedence;
		break;
	default:
		aQoSGPRSNegotiated.iPrecedence = RPacketQoS::EUnspecifiedPrecedence;
		break;
    	}
    
    /* Mean throughput always at best effort */
    aQoSGPRSNegotiated.iMeanThroughput = RPacketQoS::EMeanThroughputBestEffort;

    /* Delivery order to reordering required */
    if (aQoSR99_R4Negotiated.iDeliveryOrderReqd == RPacketQoS::EDeliveryOrderRequired)
        {
        /* Not clear what corresponding value to set */
        }
    else if (aQoSR99_R4Negotiated.iDeliveryOrderReqd == RPacketQoS::EDeliveryOrderNotRequired)
        {
        /* Not clear what corresponding value to set */
        }
    else
        {
        /* Not clear what corresponding value to set */
        }

    return KErrNone;
    }

/*
* Perform conversion from R97 negotiated QoS to R99 negotiated QoS according to 3GPP 24.008-750
*
* @param aQoSGPRSNegotiated - Etel GPRS negotiated QoS
* @param aQoSR99_R4Negotiated - Etel R99_R4 negotiated QoS
*
* @return Etel error code
*/
TInt CPacketServicesDispatcher::EtelPcktR97NegToEtelPcktR99R4NegQoS(const RPacketQoS::TQoSGPRSNegotiated& aQoSGPRSNegotiated,
											   RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated)
	{

	RPacketQoS::TQoSGPRSRequested generatedGPRSQoS;

    /* Traffic class/handling delay to traffic*/
	if ((aQoSGPRSNegotiated.iDelay == RPacketQoS::EDelayClass1) || (aQoSGPRSNegotiated.iDelay == RPacketQoS::EDelayClass2) || (aQoSGPRSNegotiated.iDelay == RPacketQoS::EDelayClass3))
		{
		aQoSR99_R4Negotiated.iTrafficClass = RPacketQoS::ETrafficClassInteractive;
		}
	else if (aQoSGPRSNegotiated.iDelay == RPacketQoS::EDelayClass4)
		{
		aQoSR99_R4Negotiated.iTrafficClass = RPacketQoS::ETrafficClassBackground;
		}
	else
		{
		aQoSR99_R4Negotiated.iTrafficClass = RPacketQoS::ETrafficClassUnspecified;
		}

	/* Traffic handling priority class/traffic handling priority*/
	switch (aQoSGPRSNegotiated.iDelay)
		{
	case RPacketQoS::EDelayClass1:
		aQoSR99_R4Negotiated.iTrafficHandlingPriority = RPacketQoS::ETrafficPriority1;
		break;
	case RPacketQoS::EDelayClass2:
		aQoSR99_R4Negotiated.iTrafficHandlingPriority = RPacketQoS::ETrafficPriority2;
		break;
	case RPacketQoS::EDelayClass3:
		aQoSR99_R4Negotiated.iTrafficHandlingPriority = RPacketQoS::ETrafficPriority3;
		break;
	default:	
		aQoSR99_R4Negotiated.iTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
		break;		
		}
	
	/* Reliability calss to SDU error ratio*/
	if ((aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass1) || (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass2))
		{
		aQoSR99_R4Negotiated.iSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundredThousand;
		}
	else if (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass3)
		{
		aQoSR99_R4Negotiated.iSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTenThousand;
		}
	else if ((aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass4) || (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass5))
		{
		aQoSR99_R4Negotiated.iSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand;
		}
	else
		{
		aQoSR99_R4Negotiated.iSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
		}


	/* Reliability calss to Residual bit error ratio*/
    if  ((aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass1) || (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass2)
    	|| (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass3) || (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass4))
    	{
    	aQoSR99_R4Negotiated.iBER = RPacketQoS::EBEROnePerHundredThousand;
    	}
    else if (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass5)
        {
        aQoSR99_R4Negotiated.iBER = RPacketQoS::EBERFourPerThousand;
        }
    else
    	{
    	aQoSR99_R4Negotiated.iBER = RPacketQoS::EBERUnspecified;
    	}

    /* Reliability class to delivery of erroneous SDUs*/
    if  ((aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass1) || (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass2)
    	|| (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass3) || (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass4))
    	{
    	aQoSR99_R4Negotiated.iDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
    	}
    else if (aQoSGPRSNegotiated.iReliability == RPacketQoS::EReliabilityClass5)
        {
        aQoSR99_R4Negotiated.iDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryRequired;
        }
    else
    	{
    	aQoSR99_R4Negotiated.iDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
    	}

    /* Peak throughput class to Max bitrate*/
    switch (aQoSGPRSNegotiated.iPeakThroughput)
    	{
	case RPacketQoS::EPeakThroughput1000:		
		aQoSR99_R4Negotiated.iMaxRate.iDownlinkRate = 8;
		break;
	case RPacketQoS::EPeakThroughput2000:
		aQoSR99_R4Negotiated.iMaxRate.iDownlinkRate = 16;
		break;
	case RPacketQoS::EPeakThroughput4000:
		aQoSR99_R4Negotiated.iMaxRate.iDownlinkRate = 32;
		break;
	case RPacketQoS::EPeakThroughput8000:
		aQoSR99_R4Negotiated.iMaxRate.iDownlinkRate = 64;
		break;
	case RPacketQoS::EPeakThroughput16000:
		aQoSR99_R4Negotiated.iMaxRate.iDownlinkRate = 128;
		break;
	case RPacketQoS::EPeakThroughput32000:
		aQoSR99_R4Negotiated.iMaxRate.iDownlinkRate = 256;
		break;		
	case RPacketQoS::EPeakThroughput64000:
		aQoSR99_R4Negotiated.iMaxRate.iDownlinkRate = 512;
		break;
	case RPacketQoS::EPeakThroughput128000:
		aQoSR99_R4Negotiated.iMaxRate.iDownlinkRate = 1024;
		break;
	case RPacketQoS::EPeakThroughput256000:
		aQoSR99_R4Negotiated.iMaxRate.iDownlinkRate = 2048;
		break;
	case RPacketQoS::EUnspecifiedPeakThroughput:
	default:
		break;
    	}

    /*Precendence class to allocation/retention priority*/
    if (aQoSGPRSNegotiated.iPrecedence == RPacketQoS::EPriorityHighPrecedence)
    	{
    	/**/
    	}
    else if (aQoSGPRSNegotiated.iPrecedence == RPacketQoS::EPriorityMediumPrecedence)
    	{
    	/**/
    	}
    else if (aQoSGPRSNegotiated.iPrecedence == RPacketQoS::EPriorityMediumPrecedence)
    	{
    	/**/
    	}
    else
    	{
    	/**/
    	}

	return KErrNone;
	}

/*
* Perform conversion from R99 negotiated QoS to R5 negotiated QoS according to 3GPP 24.008-750
*
* @param aQoSR99_R4Negotiated - Etel R99_R4 negotiated QoS
* @param aQoSR5Negotiated - Etel R5 negotiated QoS
*
* @return Etel error code
*/
TInt CPacketServicesDispatcher::EtelPcktR99R4NegToEtelPcktR5NegQoS(const RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
											   RPacketQoS::TQoSR5Negotiated& aQoSR5Negotiated)
	{
	aQoSR5Negotiated.iTrafficClass = aQoSR99_R4Negotiated.iTrafficClass;
	aQoSR5Negotiated.iDeliveryOrderReqd = aQoSR99_R4Negotiated.iDeliveryOrderReqd;	//< Negotiated value for sequential SDU delivery
	aQoSR5Negotiated.iDeliverErroneousSDU = aQoSR99_R4Negotiated.iDeliverErroneousSDU;	//< Negotiated value for erroneous SDU delivery
	aQoSR5Negotiated.iMaxSDUSize = aQoSR99_R4Negotiated.iMaxSDUSize; //< Negotiated maximum SDU size (in octets)
	aQoSR5Negotiated.iMaxRate = aQoSR99_R4Negotiated.iMaxRate;		//< Negotiated maximum bit rates on the uplink and downlink
	aQoSR5Negotiated.iBER = aQoSR99_R4Negotiated.iBER;				//< Negotiated target BER
	aQoSR5Negotiated.iSDUErrorRatio = aQoSR99_R4Negotiated.iSDUErrorRatio;	//< Negotiated target SDU error ratio
	aQoSR5Negotiated.iTrafficHandlingPriority = aQoSR99_R4Negotiated.iTrafficHandlingPriority;	//< Negotiated traffic handling priority
	aQoSR5Negotiated.iTransferDelay = aQoSR99_R4Negotiated.iTransferDelay;	//< Negotiated transfer delay (in milliseconds)
	aQoSR5Negotiated.iGuaranteedRate = aQoSR99_R4Negotiated.iGuaranteedRate;

	return KErrNone;
	}

void CPacketServicesDispatcher::ConvertDNSToPCOBuffer
	(const TDes8& aPrimaryDNS, const TDes8& aSecondaryDNS, const TUint8 aRequestID, TDes8& aFormatPcoBufferAddr)
	{

    // Handle Primary DNS address
    TInetAddr netPrimaryDNS;
    TBuf<32> netPrimaryDNSConv;
    netPrimaryDNSConv.Copy(aPrimaryDNS);
    // coverity[check_return]
    netPrimaryDNS.Input(netPrimaryDNSConv);
    TUint32 primDNSaddr =  netPrimaryDNS.Address();

    // Handle Secondary DNS address
    TInetAddr netSecondaryDNS;
    TBuf<32> netSecondaryDNSConv;
    netSecondaryDNSConv.Copy(aSecondaryDNS);
    // coverity[check_return]
    netSecondaryDNS.Input(netSecondaryDNSConv);
    TUint32 secondaryDNSaddr =  netSecondaryDNS.Address();

	aFormatPcoBufferAddr[0] = KPrimaryAndSecondaryDNSLength;		// Length
	aFormatPcoBufferAddr[1] = KIPCRequestCode; 					// IPC PRequest Code
	aFormatPcoBufferAddr[2] = aRequestID;  					// Request ID (CHAP challenge)
	aFormatPcoBufferAddr[3] = KPacketLengthMSB;					// Packet length MSB (always zero)
	aFormatPcoBufferAddr[4] = KPrimaryAndSecondaryDNSLength;     // Packet length LSB
	aFormatPcoBufferAddr[5] = KIndicatePrimaryAddress; 			// Indicate Primary address
	aFormatPcoBufferAddr[6]	= KDnsAddressLength;				// Length of address (inc header)
	aFormatPcoBufferAddr[7] = (primDNSaddr >> 24) & 0xff; 		// Primary DNS
	aFormatPcoBufferAddr[8] = (primDNSaddr >> 16) & 0xff; 		// Primary DNS
	aFormatPcoBufferAddr[9] = (primDNSaddr >> 8) & 0xff; 		// Primary DNS
	aFormatPcoBufferAddr[10] =  primDNSaddr & 0xff; 		    // Primary DNS
	aFormatPcoBufferAddr[11] = KIndicateSecondaryAddress;		// Indicate Secondary address
	aFormatPcoBufferAddr[12] = KDnsAddressLength;				// Length of address (inc header)
	aFormatPcoBufferAddr[13] = (secondaryDNSaddr >> 24) & 0xff; // Secondary DNS
	aFormatPcoBufferAddr[14] = (secondaryDNSaddr >> 16) & 0xff; // Secondary DNS
	aFormatPcoBufferAddr[15] = (secondaryDNSaddr >> 8) & 0xff; 	// Secondary DNS
	aFormatPcoBufferAddr[16] =  secondaryDNSaddr & 0xff; 		// Secondary DNS
	aFormatPcoBufferAddr.SetLength(KPrimaryAndSecondaryDNSLength + 1);
	}

void CPacketServicesDispatcher::ConvertUsernameAndPasswordToPCOBuffer(const TDes8& aUsername, const TDes8& aPassword, TUint8 aRequestID, TDes8& aFormatedBuffer)
	{
	TInt usernameLen = aUsername.Length();
	TInt passwordLen = aPassword.Length();
	TInt dataLen = usernameLen + passwordLen + KPasswordUsernameHeaderLength;
	TInt i = 0;

	aFormatedBuffer[i++] = dataLen; 				// Length
	aFormatedBuffer[i++] = KIPCRequestCode; 		// IPC PRequest Code
	aFormatedBuffer[i++] = aRequestID; 				// Request ID (CHAP challenge)
	aFormatedBuffer[i++] = KPacketLengthMSB; 		// Packet length MSB (always zero)
	aFormatedBuffer[i++] = dataLen; 				// Packet length LSB
	aFormatedBuffer[i++] = usernameLen; 			// Length of username
	aFormatedBuffer.SetLength(i);
	aFormatedBuffer.Append(aUsername); 				// Username
	aFormatedBuffer.SetLength(i + usernameLen + 1);
	aFormatedBuffer[i + usernameLen] = passwordLen; // Length of password
	aFormatedBuffer.Append(aPassword);				// Password
	}

void CPacketServicesDispatcher::DecodePCO(const RPacketContext::TMiscProtocolBuffer& aPcoBuffer, RPacketContext::TAuthData& aUsername, RPacketContext::TAuthData& aPassword, RPacketContext::TProtocolAddress& aPrimaryDNS, RPacketContext::TProtocolAddress& aSecondaryDNS, RPacketContext::TMiscProtocolBuffer& aMiscBuffer)
	{
	TUint currIndex = KHeaderGap;

	// Skip Headers
	TUint usernameLen = aPcoBuffer[currIndex++];

	// Get the username
	aUsername = aPcoBuffer.Mid(currIndex, usernameLen);
	currIndex += usernameLen;

	TUint passwordLen = aPcoBuffer[currIndex++];
	aPassword = aPcoBuffer.Mid(currIndex, passwordLen);

	currIndex += passwordLen;
	currIndex += KHeaderGap;

	// Skip primary DNS length
	currIndex += 2;

	// Get the primary DNS
	TUint32 uintDnsAddr = 0;
	uintDnsAddr = aPcoBuffer[currIndex++];
	uintDnsAddr <<= 8;
	uintDnsAddr |= aPcoBuffer[currIndex++];
	uintDnsAddr <<= 8;
	uintDnsAddr |= aPcoBuffer[currIndex++];
	uintDnsAddr <<= 8;
	uintDnsAddr |= aPcoBuffer[currIndex++];

	TInetAddr dnsInetAddr;
	dnsInetAddr.SetAddress(uintDnsAddr);
	TBuf<32> netPrimaryDNSAddrLogOutput;
	dnsInetAddr.Output(netPrimaryDNSAddrLogOutput);
	aPrimaryDNS.Copy(netPrimaryDNSAddrLogOutput);

	// Skip the Secondary indicator
	currIndex += 2;

	uintDnsAddr = 0;
	uintDnsAddr = aPcoBuffer[currIndex++];
	uintDnsAddr <<= 8;
	uintDnsAddr |= aPcoBuffer[currIndex++];
	uintDnsAddr <<= 8;
	uintDnsAddr |= aPcoBuffer[currIndex++];
	uintDnsAddr <<= 8;
	uintDnsAddr |= aPcoBuffer[currIndex++];

	dnsInetAddr.SetAddress(uintDnsAddr);
	TBuf<32> netSecondaryDNSAddrLogOutput;
	dnsInetAddr.Output(netSecondaryDNSAddrLogOutput);
	aSecondaryDNS.Copy(netSecondaryDNSAddrLogOutput);

	aMiscBuffer = aPcoBuffer.Mid(currIndex);
	}

CPcktMbmsMonitoredServiceList* CPacketServicesDispatcher::CreateServiceListL(const RArray<TUint>& aServiceIds) const
	{
	CPcktMbmsMonitoredServiceList* updateMbmsMonitorServiceList = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(updateMbmsMonitorServiceList);

	const TInt serviceIdCount = aServiceIds.Count();
	RPacketService::TMbmsServiceAvailabilityV1 entry;

	for(TInt i = 0; i < serviceIdCount; ++i)
		{
		entry.iTmgi.SetServiceId(aServiceIds[i]);
		updateMbmsMonitorServiceList->AddEntryL(entry);
		}
	
	CleanupStack::Pop(updateMbmsMonitorServiceList);
	return updateMbmsMonitorServiceList;
	}


