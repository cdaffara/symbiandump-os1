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

#include <ctsy/ltsy/mltsydispatchpacketservicesinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>
#include <in_sock.h>

#include "mmockmesshandlerbase.h"
#include "cmockpacketservicesmesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"
/**
 * Factory function
 */
CMockPacketServicesMessHandler* CMockPacketServicesMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockPacketServicesMessHandler* self = new (ELeave)CMockPacketServicesMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockPacketServicesMessHandler::CMockPacketServicesMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {
    }

/**
 * 2nd phase contructor
 */
void CMockPacketServicesMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockPacketServicesMessHandler::~CMockPacketServicesMessHandler()
    {

    }

TBool CMockPacketServicesMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		case KMockLtsyDispatchPacketServicesNotifyPacketAttachModeChangeIndId:
		case KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId:
		case KMockLtsyDispatchPacketServicesNotifyRatTransferCapsChangeIndId:
		case KMockLtsyDispatchPacketServicesNotifyPacketMsClassChangeIndId:
		case KMockLtsyDispatchPacketServicesNotifyPacketDynamicCapsChangeIndId:
		case KMockLtsyDispatchPacketServicesNotifyPdpContextAddedIndId:
		case KMockLtsyDispatchPacketServicesNotifyNetworkInitiatedContextActivationRequestIndId:
		case KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId:
		case KMockLtsyDispatchPacketServicesNotifyPdpContextStatusChangeIndId:
		case KMockLtsyDispatchPacketServicesNotifyPdpContextConnectionSpeedChangeIndId:
		case KMockLtsyDispatchPacketServicesNotifyPacketNetworkRegistrationStatusIndId:
		case KMockLtsyDispatchPacketServicesNotifyMbmsContextConfigChangedIndId:
		case KMockLtsyDispatchPacketServicesNotifyMbmsNetworkServiceStatusChangeIndId:
		case KMockLtsyDispatchPacketServicesNotifyMbmsServiceAvailabilityChangeIndId:
		case KMockLtsyDispatchPacketServicesNotifyConnectionInfoChangeIndId:
		case MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId:
		case MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId:
		case MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId:
		case MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId:
		case MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId:
		case MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId:
		case MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId:
		case MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId:
		case MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId:
		case MLtsyDispatchPacketServicesNotifyPacketStatusChange::KLtsyDispatchPacketServicesNotifyPacketStatusChangeApiId:
		case MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId:
		case MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId:
		case MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId:
		case MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId:
		case MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId:
		case MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId:
		case MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId:
		case MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId:
		case MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId:
		case MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId:
		case MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId:
		case MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId:
		case MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId:
		case MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId:
		case MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId:
		case MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId:
		case MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId:
		case MLtsyDispatchPacketServicesRemovePacketFilter::KLtsyDispatchPacketServicesRemovePacketFilterApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockPacketServicesMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockPacketServicesMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	case MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId:
    	case MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId:
    	case MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId:
    	case MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId:
    		{
	    	TMockLtsyData0 data;
	    	return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}    		
    	case MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId:
    		{
    		const TDesC& contextId = *(VA_ARG(aList, const TDesC*));
    		TInfoName contextIdBuf;
    		contextIdBuf.Copy(contextId.Left(KMaxInfoName));	
	    	TMockLtsyData1<TInfoName> data(contextIdBuf);
	    	return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}    		
    	case MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId:
    		{
	    	TMockLtsyData0 data;
	    	return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}    		
    	case MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId:
    		{

    		const TDesC& contextId = *(VA_ARG(aList, const TDesC*));
    		TInfoName contextIdBuf;
    		contextIdBuf.Copy(contextId.Left(KMaxInfoName));
    		
    		const TDesC8& accessPointName = *(VA_ARG(aList, const TDesC8*));
    		RPacketContext::TGSNAddress accessPointNameBuf;
    		accessPointNameBuf.Copy(accessPointName.Left(RPacketContext::KGSNNameLength));

    		RPacketContext::TProtocolType pdpType = VA_ARG_ENUM(aList, RPacketContext::TProtocolType);
    		
    		const TDesC8& pdpAddress = *(VA_ARG(aList, const TDesC8*));
    		RPacketContext::TProtocolAddress pdpAddressBuf;
    		pdpAddressBuf.Copy(pdpAddress.Left(RPacketContext::KMaxPDPAddressLength));
    		
    		const TDesC8& pcoBuffer = *(VA_ARG(aList, const TDesC8*));
    		RPacketContext::TMiscProtocolBuffer pcoBufferBuf;
    		pcoBufferBuf.Copy(pcoBuffer.Left(RPacketContext::KMiscProtocolBufferLength));    		    					
    				
			TMockLtsyData5<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType, 
			RPacketContext::TProtocolAddress,RPacketContext::TMiscProtocolBuffer> data(contextIdBuf, 
					accessPointNameBuf, pdpType, pdpAddressBuf, pcoBufferBuf);
	    	return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
    	case MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId:
    		{
    		
    		const TDesC& deleteContextName = *(VA_ARG(aList, const TDesC*));
    		TInfoName deleteContextNameBuf;
    		deleteContextNameBuf.Copy(deleteContextName.Left(KMaxInfoName));    		    		
    		TMockLtsyData1<TInfoName> data(deleteContextNameBuf);
	    	return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
      	case MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId:
    		{
	    	TMockLtsyData0 data;
	    	return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
    	case MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId:
    		{
    		
    		const TDesC& primaryContext = *(VA_ARG(aList, const TDesC*));
    		const TDesC& secondaryContext = *(VA_ARG(aList, const TDesC*));    		
    		TInfoName primaryContextBuf;
    		TInfoName secondaryContextBuf;    		
    		primaryContextBuf.Copy(primaryContext.Left(KMaxInfoName));
    		secondaryContextBuf.Copy(secondaryContext.Left(KMaxInfoName));
    		
    		TMockLtsyData2<TInfoName, TInfoName> data(primaryContextBuf, secondaryContextBuf);
  			return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);  			
    		}
    		 
    	case MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId:
    		{
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
    	case MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId:
    		{
		    RPacketService::TAttachMode attachMode =  VA_ARG_ENUM(aList, RPacketService::TAttachMode);
		    TMockLtsyData1<RPacketService::TAttachMode> data(attachMode);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
    	case MLtsyDispatchPacketServicesNotifyPacketStatusChange::KLtsyDispatchPacketServicesNotifyPacketStatusChangeApiId:
    		{
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
    	case MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId:
    		{
    		RPacketContext::TContextConfigGPRS& dataConfigGPRS = *VA_ARG(aList, RPacketContext::TContextConfigGPRS*);
    		TMockLtsyData1<RPacketContext::TContextConfigGPRS> data(dataConfigGPRS);
  			return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
    	case MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId:
    		{
    		RPacketContext::TContextConfigR99_R4& dataConfigR99R4 = *VA_ARG(aList, RPacketContext::TContextConfigR99_R4*);
    		TMockLtsyData1<RPacketContext::TContextConfigR99_R4> data(dataConfigR99R4);
  			return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
    	case MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId:
    		{
    		const TDesC& contextName = *(VA_ARG(aList, const TDesC*));
    		TInfoName contextNameBuf;
    		contextNameBuf.Copy(contextName.Left(KMaxInfoName));
    		
    		TMockLtsyData1<TInfoName> data(contextNameBuf);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
    	case MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId:
    		{    	
    		const TDesC& contextName = *(VA_ARG(aList, const TDesC*));
    		TInfoName contextNameBuf;
    		contextNameBuf.Copy(contextName.Left(KMaxInfoName));
    		
    		RPacketQoS::TQoSGPRSRequested& qoSConfig = *VA_ARG(aList, RPacketQoS::TQoSGPRSRequested*);
    		TMockLtsyData2<TInfoName, RPacketQoS::TQoSGPRSRequested> data(contextNameBuf, qoSConfig);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
			}
    		 
    	case MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId:
    		{
    		const TDesC& contextName = *(VA_ARG(aList, const TDesC*));
    		TInfoName contextNameBuf;
    		contextNameBuf.Copy(contextName.Left(KMaxInfoName));
    		RPacketQoS::TQoSR99_R4Requested& qoSConfig = *VA_ARG(aList, RPacketQoS::TQoSR99_R4Requested*);
    		TMockLtsyData2<TInfoName, RPacketQoS::TQoSR99_R4Requested> data(contextNameBuf, qoSConfig);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
			}
    		 
    	case MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId:
    		{
    		const TDesC& contextName = *(VA_ARG(aList, const TDesC*));
    		TInfoName contextNameBuf;
    		contextNameBuf.Copy(contextName.Left(KMaxInfoName));
    		RPacketQoS::TQoSR5Requested& qoSConfig = *VA_ARG(aList, RPacketQoS::TQoSR5Requested*);
    		TMockLtsyData2<TInfoName, RPacketQoS::TQoSR5Requested> data(contextNameBuf, qoSConfig);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
			}
    		 
    	case MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
    	case MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId:
    		{
    		const TDesC& contextName = *(VA_ARG(aList, const TDesC*));
    		TInfoName contextNameBuf;
			contextNameBuf.Copy(contextName.Left(KMaxInfoName));
    		TMockLtsyData1<TInfoName> data(contextNameBuf);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		 
    	case MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId:
    		{
    		TMockLtsyData0 data;
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	
    	case MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId:
    		{
    		TDesC* contextName = VA_ARG(aList, TDesC*);
    		TMockLtsyData1<TDesC*> data(contextName);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId:
    		{
    		TBool attemptAttach = VA_ARG_ENUM(aList, TBool);
    		TMockLtsyData1<TBool> data(attemptAttach);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	
    	case MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId:
    		{
    		TMbmsAction action = VA_ARG_ENUM(aList, TMbmsAction);
    		CPcktMbmsMonitoredServiceList* serviceList = VA_ARG(aList, CPcktMbmsMonitoredServiceList*);
    		CPcktMbmsMonitoredServiceList* changeList = VA_ARG(aList, CPcktMbmsMonitoredServiceList*);
    		TMockLtsyData3<TMbmsAction,CPcktMbmsMonitoredServiceList*,CPcktMbmsMonitoredServiceList*> data(action,serviceList,changeList);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	
    	case MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId:
    		{
    		TDesC* contextName = VA_ARG(aList, TDesC*);
    		TMbmsAction action = VA_ARG_ENUM(aList, TMbmsAction);
    		TMbmsSessionId sessionId = VA_ARG_ENUM(aList, TMbmsSessionId);
    		TMbmsSessionIdList& sessionIdList = *VA_ARG(aList, TMbmsSessionIdList*);
    		TMockLtsyData4<TDesC*,TMbmsAction,TMbmsSessionId,TMbmsSessionIdList> data(contextName,action,sessionId,sessionIdList);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	
    	case MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId:
    		{
    		const TDesC& contextName = *(VA_ARG(aList, const TDesC*));
    		TInfoName contextNameBuf(contextName.Left(KMaxInfoName));   		
    		CArrayFixFlat<RPacketContext::TPacketFilterV2>* contextFilter = VA_ARG(aList, CArrayFixFlat<RPacketContext::TPacketFilterV2>*);
    		
    		TMockLtsyData2<TInfoName, CArrayFixFlat<RPacketContext::TPacketFilterV2>* > data(contextNameBuf, contextFilter);    		
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);    		
    		}
    		
    	case MLtsyDispatchPacketServicesRemovePacketFilter::KLtsyDispatchPacketServicesRemovePacketFilterApiId:
    		{
    		const TDesC& contextName = *(VA_ARG(aList, const TDesC*));
    		TInfoName contextNameBuf(contextName.Left(KMaxInfoName));    		
    		TInt filterId = VA_ARG(aList, TInt);
    		
    		TMockLtsyData2<TInfoName, TInt > data(contextNameBuf, filterId);    		
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);    		
    		}

    	case KMockLtsyDispatchPacketServicesNotifyPacketAttachModeChangeIndId:
    	case KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId:
    	case KMockLtsyDispatchPacketServicesNotifyRatTransferCapsChangeIndId:
    	case KMockLtsyDispatchPacketServicesNotifyPacketMsClassChangeIndId:
    	case KMockLtsyDispatchPacketServicesNotifyPacketDynamicCapsChangeIndId:
    	case KMockLtsyDispatchPacketServicesNotifyPdpContextAddedIndId:
    	case KMockLtsyDispatchPacketServicesNotifyNetworkInitiatedContextActivationRequestIndId:
    	case KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId:
    	case KMockLtsyDispatchPacketServicesNotifyPdpContextStatusChangeIndId:
    	case KMockLtsyDispatchPacketServicesNotifyPdpContextConnectionSpeedChangeIndId:
    	case KMockLtsyDispatchPacketServicesNotifyPacketNetworkRegistrationStatusIndId:
    	case KMockLtsyDispatchPacketServicesNotifyMbmsContextConfigChangedIndId:
    	case KMockLtsyDispatchPacketServicesNotifyMbmsNetworkServiceStatusChangeIndId:
    	case KMockLtsyDispatchPacketServicesNotifyMbmsServiceAvailabilityChangeIndId:
    	case KMockLtsyDispatchPacketServicesNotifyConnectionInfoChangeIndId:
    	default:
    		{
    		_LIT(KTempPanic, "Here to remind coder to add code to deserialise data otherwise test passes even though CTSY sends down the incorrect data");
    		User::Panic(KTempPanic, KErrGeneral);
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	}
    } // CMockPacketServicesMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockPacketServicesMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{

		case KMockLtsyDispatchPacketServicesNotifyPacketAttachModeChangeIndId:
		    {		    
		    TMockLtsyData1Buf<RPacketService::TPreferredBearer> data;
		    data.DeserialiseL(aData);		    		   
			iCompletionCallback.CallbackPacketServicesNotifyPacketAttachModeChangeInd(aResult, data.Data1());
			}
			break;
		case KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId:
		    {

		    TMockLtsyData2Buf<TInfoName, TPacketDataConfigBase> data;
		    data.DeserialiseL(aData);

		    TInfoName contextName = data.Data1();
		    TPacketDataConfigBase profile = data.Data2();

		    switch(profile.ExtensionId())
	    		{
		    	case TPacketDataConfigBase::KConfigGPRS:
	    			{
	    			TMockLtsyData2Buf<TInfoName, RPacketQoS::TQoSGPRSNegotiated> gprsData;
	    					    gprsData.DeserialiseL(aData);
	    			RPacketQoS::TQoSGPRSNegotiated gprsProfile = gprsData.Data2();
	    			iCompletionCallback.CallbackPacketServicesNotifyQosProfileChangedGPRSInd(aResult, contextName, gprsProfile);
	    			}
	    			break;
	    		case TPacketDataConfigBase::KConfigRel99Rel4:
	    			{
	    			TMockLtsyData2Buf<TInfoName, RPacketQoS::TQoSR99_R4Negotiated> r99r4Data;
	    			r99r4Data.DeserialiseL(aData);
	    		    RPacketQoS::TQoSR99_R4Negotiated r99r4Profile = r99r4Data.Data2();
	    			iCompletionCallback.CallbackPacketServicesNotifyQosProfileChangedR99Ind(aResult, contextName, r99r4Profile);
	    			}
	    			break;
	    		case TPacketDataConfigBase::KConfigRel5:
	    			{
	    			TMockLtsyData2Buf<TInfoName, RPacketQoS::TQoSR5Negotiated> r5Data;
	    			r5Data.DeserialiseL(aData);
	    		    RPacketQoS::TQoSR5Negotiated r5Profile = r5Data.Data2();
	    			iCompletionCallback.CallbackPacketServicesNotifyQosProfileChangedR5Ind(aResult, contextName, r5Profile);
	    			}
	    			break;
		    	}

			}
			break;
		case KMockLtsyDispatchPacketServicesNotifyRatTransferCapsChangeIndId:
		    {
		    TMockLtsyData1Buf<TDynamicTransferCapsFlags> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesNotifyRatTransferCapsChangeInd(aResult, data.Data1());
			}
			break;
		case KMockLtsyDispatchPacketServicesNotifyPacketMsClassChangeIndId:
		    {
		    TMockLtsyData1Buf<RPacketService::TMSClass> data;
		    data.DeserialiseL(aData);
		    RPacketService::TMSClass tMSClass = data.Data1();
			iCompletionCallback.CallbackPacketServicesNotifyPacketMsClassChangeInd(aResult, tMSClass);
			}
			break;
		case KMockLtsyDispatchPacketServicesNotifyPacketDynamicCapsChangeIndId:
		    {
		    TMockLtsyData2Buf<TBool,TBool> data;
		    data.DeserialiseL(aData);
		    TBool gprsIsSupportedOnCell = data.Data1();
		    TBool csServicesIsSupportedOnCell = data.Data2();
			iCompletionCallback.CallbackPacketServicesNotifyPacketDynamicCapsChangeInd(aResult, gprsIsSupportedOnCell, csServicesIsSupportedOnCell);
			}
			break;
		case KMockLtsyDispatchPacketServicesNotifyPdpContextAddedIndId:
		    {
			TMockLtsyData1Buf<TInfoName> data;
    		data.DeserialiseL(aData);
    		TInfoName contextName = data.Data1();
			iCompletionCallback.CallbackPacketServicesNotifyPdpContextAddedInd(aResult, contextName);
			}
			break;
		case KMockLtsyDispatchPacketServicesNotifyNetworkInitiatedContextActivationRequestIndId:
		    {
			TMockLtsyData1Buf<TPacketDataConfigBase> data;
			data.DeserialiseL(aData);
			TPacketDataConfigBase profile = data.Data1();

			switch(profile.ExtensionId())
				{
				case TPacketDataConfigBase::KConfigGPRS:
					{
					TMockLtsyData1Buf<RPacketContext::TContextConfigGPRS> gprsData;
					gprsData.DeserialiseL(aData);
					RPacketContext::TContextConfigGPRS gprsConfig = gprsData.Data1();
					iCompletionCallback.CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd(aResult, gprsConfig);
					}
					break;
				case TPacketDataConfigBase::KConfigRel99Rel4:
					{
					TMockLtsyData1Buf<RPacketContext::TContextConfigR99_R4> r99r4Data;
					r99r4Data.DeserialiseL(aData);
					RPacketContext::TContextConfigR99_R4 r99r4Config = r99r4Data.Data1();
					iCompletionCallback.CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd(aResult, r99r4Config);
					}
					break;
				case TPacketDataConfigBase::KConfigRel5:
					{
					TMockLtsyData1Buf<RPacketContext::TContextConfig_R5> r5Data;
					r5Data.DeserialiseL(aData);
					RPacketContext::TContextConfig_R5 r5Config = r5Data.Data1();
					iCompletionCallback.CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd(aResult, r5Config);
					}
					break;
				}
		    }
			break;
		case KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId:
		    {
			TMockLtsyData5Buf<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolAddress, TUint, RPacketContext::TMiscProtocolBuffer> data;
		    data.DeserialiseL(aData);
		    iCompletionCallback.CallbackPacketServicesNotifyPdpContextConfigChangedInd(aResult, data.Data1(), data.Data2(), data.Data3(), data.Data4(), data.Data5());
			}
			break;
		case KMockLtsyDispatchPacketServicesNotifyPdpContextStatusChangeIndId:
		    {
		    TMockLtsyData2Buf<TInfoName, TContextMisc> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesNotifyPdpContextStatusChangeInd(aResult, data.Data1(), data.Data2());
			}
			break;
		case KMockLtsyDispatchPacketServicesNotifyPdpContextConnectionSpeedChangeIndId:
		    {
		    TMockLtsyData2Buf<TInfoName, TUint> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesNotifyPdpContextConnectionSpeedChangeInd(aResult, data.Data1(), data.Data2());
			}
			break;
		case KMockLtsyDispatchPacketServicesNotifyPacketNetworkRegistrationStatusIndId:
		    {
		    TMockLtsyData1Buf<RPacketService::TRegistrationStatus> data;
    		data.DeserialiseL(aData);
    		RPacketService::TRegistrationStatus registrationStatus = data.Data1();
			iCompletionCallback.CallbackPacketServicesNotifyPacketNetworkRegistrationStatusInd(aResult, registrationStatus);
			}
			break;
		case MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId:
		    {
			iCompletionCallback.CallbackPacketServicesPacketAttachComp(aResult);
			}
			break;
		case MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId:
		    {
		    TMockLtsyData1Buf<RPacketService::TAttachMode> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesGetPacketAttachModeComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId:
		    {
			TMockLtsyData1Buf<RPacketService::TRegistrationStatus> data;
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesGetPacketNetworkRegistrationStatusComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId:
		    {
			iCompletionCallback.CallbackPacketServicesPacketDetachComp(aResult);
			}
			break;
		case MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId:
		    {
		    TMockLtsyData1Buf<TInfoName> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesSetPdpContextConfigComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId:
		    {
		    TMockLtsyData2Buf<TInfoName, TInt8> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesModifyActivePdpContextComp(aResult, data.Data1(), data.Data2());
			}
			break;
		case MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId:
		    {
		    TDesC* contextName = NULL;
		    TDesC* channelId = NULL;
		    TMockLtsyData2<TDesC*, TDesC*> data(contextName, channelId);
    		data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesInitialisePdpContextComp(aResult, *contextName, *channelId);
			
			delete contextName;
			contextName = NULL;
			
			delete channelId;
			channelId = NULL;
		    }
			break;
		case MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId:
		    {
            TMockLtsyData1Buf<TInfoName> data;
            data.DeserialiseL(aData);
            iCompletionCallback.CallbackPacketServicesDeletePdpContextComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId:
		    {
			iCompletionCallback.CallbackPacketServicesSetPacketAttachModeComp(aResult);
			}
			break;		
		case MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId:
		    {
			iCompletionCallback.CallbackPacketServicesSetDefaultPdpContextParamsComp(aResult);
			}
			break;
		case MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId:
		    {
			iCompletionCallback.CallbackPacketServicesSetDefaultPdpContextParamsComp(aResult);
			}
		    break;
		case MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId:
		    {
		    TMockLtsyData3Buf<TInfoName, RPacketContext::TGSNAddress, RPacketContext::TProtocolType> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesActivatePdpContextComp(aResult, data.Data1(), data.Data2(), data.Data3());
			}
			break;
		case MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId:
		    {
		    TMockLtsyData1Buf<TInfoName> data;
		    data.DeserialiseL(aData);

			iCompletionCallback.CallbackPacketServicesSetPdpContextQosComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId:
		    {
		    TMockLtsyData1Buf<TInfoName> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesSetPdpContextQosComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId:
		    {
		    TMockLtsyData1Buf<TInfoName> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesSetPdpContextQosComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId:
		    {
			iCompletionCallback.CallbackPacketServicesRejectNetworkInitiatedContextActivationRequestComp(aResult);
			}
			break;
		case MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId:
		    {
		    TMockLtsyData1Buf<TInfoName> data;
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesDeactivatePdpContextComp(aResult, data.Data1());
			}
			break;
		case MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId:
			{
			TMockLtsyData2Buf<RPacketService::TStatus, TBool> data;
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackPacketServicesGetStatusComp(aResult, data.Data1(), data.Data2());
			}
			break;
    	case MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId:
    		{
    		TUint staticCapabilities = 0;
    		TMockLtsyData1<TUint> data(staticCapabilities);
    		data.DeserialiseL(aData);
    		iCompletionCallback.CallbackPacketServicesGetStaticCapabilitiesComp(aResult,staticCapabilities);
    		}
    		break;
    	case MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId:
    		{
    		TInt maxNoMonitoredServiceLists = 0;
    		TMockLtsyData1<TInt> data(maxNoMonitoredServiceLists);
    		data.DeserialiseL(aData);
    		iCompletionCallback.CallbackPacketServicesGetMaxNoMonitoredServiceListsComp(aResult,maxNoMonitoredServiceLists);
    		}
    		break;
    	case MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId:
    		{
    		TInt maxNoActiveServices = 0;
    		TMockLtsyData1<TInt> data(maxNoActiveServices);
    		data.DeserialiseL(aData);
    		iCompletionCallback.CallbackPacketServicesGetMaxNoActiveServicesComp(aResult,maxNoActiveServices);
    		}
    		break;
    	case MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId:
    		{
    		TDesC* contextName = NULL;
    		TDesC* channelId = NULL;
    		TMockLtsyData2<TDesC*,TDesC*> data(contextName, channelId);
    		data.DeserialiseL(aData);    		
    		iCompletionCallback.CallbackPacketServicesInitialiseMbmsContextComp(aResult,*contextName,*channelId);
    		
    		delete contextName;
    		contextName = NULL;
    		
    		delete channelId;
    		channelId = NULL;
    		}
    		break;
    	case KMockLtsyDispatchPacketServicesNotifyMbmsContextConfigChangedIndId:
    		{
    		TDesC* contextName = NULL;
    		RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	
    		TMockLtsyData2<TDesC*, RPacketMbmsContext::TContextConfigMbmsV1> data(contextName,contextConfig);
    		data.DeserialiseL(aData);
    		iCompletionCallback.CallbackPacketServicesNotifyMbmsContextConfigChangedInd(aResult,*contextName,contextConfig);
    		
    		delete contextName;
    		contextName = NULL;
    		}
    		break;
    	case MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId:
    		{
    		TMbmsNetworkServiceStatus mbmsNetworkServiceStatus;
    		TMockLtsyData1<TMbmsNetworkServiceStatus> data(mbmsNetworkServiceStatus);
    		data.DeserialiseL(aData);
    		iCompletionCallback.CallbackPacketServicesGetMbmsNetworkServiceStatusComp(aResult,mbmsNetworkServiceStatus);
    		}
    		break;
    	case KMockLtsyDispatchPacketServicesNotifyMbmsNetworkServiceStatusChangeIndId:
    		{
      		TMbmsNetworkServiceStatus mbmsNetworkServiceStatus;
        	TMockLtsyData1<TMbmsNetworkServiceStatus> data(mbmsNetworkServiceStatus);
        	data.DeserialiseL(aData);
        	iCompletionCallback.CallbackPacketServicesNotifyMbmsNetworkServiceStatusChangeInd(aResult,mbmsNetworkServiceStatus);
    		}
    		break;
    	case MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId:
    		{
    		if(aResult == KErrNone)
    			{
    			iCompletionCallback.CallbackPacketServicesUpdateMbmsMonitorServiceListComp(aResult);
    			}
    		else
    			{
        		RArray<TUint> serviceIds;
        		CleanupClosePushL(serviceIds);
        		
       			TMockLtsyData1<RArray<TUint> > data(serviceIds);
       			data.DeserialiseL(aData);
        		    			
       			iCompletionCallback.CallbackPacketServicesUpdateMbmsMonitorServiceListComp(aResult, serviceIds);

       			CleanupStack::PopAndDestroy(&serviceIds);
    			}
    		}
    		break;
    	case KMockLtsyDispatchPacketServicesNotifyMbmsServiceAvailabilityChangeIndId:
    		{
    		RArray<TUint> serviceIds;
    		CleanupClosePushL(serviceIds);
    		
   			TMockLtsyData1<RArray<TUint> > data(serviceIds);
   			data.DeserialiseL(aData);
    		    			
   			iCompletionCallback.CallbackPacketServicesNotifyMbmsServiceAvailabilityChangeInd(aResult, serviceIds);

   			CleanupStack::PopAndDestroy(&serviceIds);
    		}
    		break;
    	case KMockLtsyDispatchPacketServicesNotifyConnectionInfoChangeIndId:
    		{
    		TDesC* contextName = NULL;
    		RPacketContext::TConnectionInfoV1 connectionInfo;	
    		TMockLtsyData2<TDesC*, RPacketContext::TConnectionInfoV1> data(contextName,connectionInfo);
    		data.DeserialiseL(aData);
    		iCompletionCallback.CallbackPacketServicesNotifyConnectionInfoChangeInd(aResult,*contextName,connectionInfo);
    		
    		delete contextName;
    		contextName = NULL;    		
    		}
    		break;
    	case MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId:
    		{
    		TDesC* contextName = NULL;
    		TMockLtsyData1<TDesC*> data(contextName);
    		data.DeserialiseL(aData);
    		
    		iCompletionCallback.CallbackPacketServicesUpdateMbmsSessionListComp(aResult, *contextName);
    		
    		delete contextName;
    		contextName = NULL;
    		}
    		break;
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockPacketServicesMessHandler::CompleteL

