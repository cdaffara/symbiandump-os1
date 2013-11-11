// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CLtsyPacketServicesHandler

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>
#include <pcktcs.h>
#include <ctsy/ltsy/ltsylogger.h>

#include "commengine.h"
#include "tsyconfg.h"
#include "cltsypacketserviceshandler.h"
#include "ltsymacros.h"
#include "tsyconfg.h"
#include "mslogger.h"
#include "atgprsattach.h"
#include "atgprsdetach.h"
#include "atgprsntwkregstatus.h"
#include "atgprsconfig.h"
#include "initialisepdpcontext.h"
#include "atgprscontextactivate.h"
#include "activatecontextandgetaddr.h"
#include "atgprscontextdeactivate.h"
#include "atgprscontextdelete.h"
#include "atgprsqosprofile.h"
#include "asynchelperrequest.h"
#include "atgprsntwkregstatuschange.h"

CLtsyPacketServicesHandler::CLtsyPacketServicesHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback,
	CGlobalPhonemanager* aGloblePhone)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback),
	iGloblePhone(aGloblePhone)
	{
	}
	
CLtsyPacketServicesHandler::~CLtsyPacketServicesHandler()
	{
	if(iAsyncHelperRequest)
		{
		delete iAsyncHelperRequest;
		iAsyncHelperRequest = NULL;
		}
	if(iNtwkRegStatusChange)
		{
		delete iNtwkRegStatusChange;
		iNtwkRegStatusChange = NULL;
		}
	}

CLtsyPacketServicesHandler* CLtsyPacketServicesHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback,
	CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsyPacketServicesHandler* self = new (ELeave) CLtsyPacketServicesHandler(aCtsyDispatcherCallback,aGloblePhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CLtsyPacketServicesHandler* CLtsyPacketServicesHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback,
		CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsyPacketServicesHandler* self=
			CLtsyPacketServicesHandler::NewLC(aCtsyDispatcherCallback,aGloblePhone);
	CleanupStack::Pop(self);
	return self;
	}

void CLtsyPacketServicesHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	iAsyncHelperRequest = CAsyncHelperRequest::NewL((*iGloblePhone), iCtsyDispatcherCallback);
	iInitialisePdpContext = CInitialisePdpContext::NewL(*iGloblePhone, iCtsyDispatcherCallback);
	iNtwkRegStatusChange = CATGprsNtwkRegStatusChange::NewL((*iGloblePhone), iCtsyDispatcherCallback);	
	} // CLtsyPacketServicesHandler::ConstructL

TBool CLtsyPacketServicesHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{
	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		case KLtsyDispatchPacketServicesPacketAttachApiId: 
			return ETrue;
		case KLtsyDispatchPacketServicesGetPacketAttachModeApiId: 
			return ETrue;	
		case KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId: 
			return ETrue;			
		case KLtsyDispatchPacketServicesPacketDetachApiId: 
			return ETrue;
		case KLtsyDispatchPacketServicesSetPdpContextConfigApiId: 
			return ETrue;			
		case KLtsyDispatchPacketServicesModifyActivePdpContextApiId: 
			return EFalse;			
		case KLtsyDispatchPacketServicesInitialisePdpContextApiId: 
			return ETrue;			
		case KLtsyDispatchPacketServicesDeletePdpContextApiId: 
			return ETrue;
		case KLtsyDispatchPacketServicesSetPacketAttachModeApiId: 
			return EFalse;	
		case KLtsyDispatchPacketServicesNotifyPacketStatusChangeApiId: 
			return ETrue;			
		case KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId: 
			return ETrue;			
		case KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId: 
			return ETrue;			
		case KLtsyDispatchPacketServicesActivatePdpContextApiId: 
			return ETrue;
		case KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId: 
			return ETrue;
		case KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId: 
			return ETrue;			
		case KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId: 
			return ETrue;			
		case KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId: 
			return ETrue;			
		case KLtsyDispatchPacketServicesDeactivatePdpContextApiId: 
			return ETrue;
		case KLtsyDispatchPacketServicesAddPacketFilterApiId: 
			return EFalse;
		case KLtsyDispatchPacketServicesGetStatusApiId: 
			return ETrue;			
		case KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId: 
			return EFalse;
		case KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId: 
			return EFalse;			
		case KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId: 
			return EFalse;
		case KLtsyDispatchPacketServicesInitialiseMbmsContextApiId: 
			return EFalse;			
		case KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId: 
			return EFalse;			
		case KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId: 
			return EFalse;			
		case KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId: 
			return EFalse;			
		case KLtsyDispatchPacketServicesRemovePacketFilterApiId: 
			return EFalse;			
		default:
			return EFalse;
		}
	}

void CLtsyPacketServicesHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
/**
 * Returns the set of callback indicators that are supported.
 * 
 * @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
 * @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{
	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask =   KLtsyDispatchPacketServicesNotifyQosProfileChangedIndId |
						  KLtsyDispatchPacketServicesNotifyRatTransferCapsChangeIndId |
						  KLtsyDispatchPacketServicesNotifyPacketDynamicCapsChangeIndId |
						  KLtsyDispatchPacketServicesNotifyPdpContextAddedIndId |
						  KLtsyDispatchPacketServicesNotifyNetworkInitiatedContextActivationRequestIndId |
						  KLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId |
						  KLtsyDispatchPacketServicesNotifyPdpContextStatusChangeIndId |
						  KLtsyDispatchPacketServicesNotifyPacketNetworkRegistrationStatusIndId;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
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
    LOGTEXTREL(_L8("pkt:attach"));

	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(KErrNone == ret)
		{
		CATGprsAttach* ATGprsAttach = CATGprsAttach::NewL(*iGloblePhone, iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(ATGprsAttach);
		ATGprsAttach->StartRequest();
		}
	else
	    {
	    LOGTEXTREL2(_L8("pkt:err(%d) in attach"), ret);
	    }
	
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
	TInt ret = KErrNone;
	iAsyncHelperRequest->SetRequestId(MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId);
	
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
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(KErrNone == ret)
		{
		CATGprsNtwkRegStatus* ATGprsNtwkRegStatus = CATGprsNtwkRegStatus::NewL(*iGloblePhone,
				                                          iCtsyDispatcherCallback,
				                                          *iNtwkRegStatusChange);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(ATGprsNtwkRegStatus);
		ATGprsNtwkRegStatus->StartRequest();
		}
	else
	    {
	    LOGTEXTREL2(_L8("pkt:err(%d) in GetPacketNetworkRegistrationStatus"), ret);
	    }
	
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
    LOGTEXTREL(_L8("pkt:detach"));

    TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(KErrNone == ret)
		{
		CATGprsDetach* ATGprsDetach = CATGprsDetach::NewL(*iGloblePhone, iCtsyDispatcherCallback);
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(ATGprsDetach);
		ATGprsDetach->StartRequest();
		}
	else
	    {
        LOGTEXTREL2(_L8("pkt:err(%d) in detach"), ret);
	    }
	
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

    LOGTEXTREL2(_L8("pkt:setconfig,APN>%S<"), &aAccessPointName);
	TInt ret = KErrNotSupported;	
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(KErrNone == ret)
		{
		TInt tmp = GetContext(aContextId);
		if(tmp >= 0)
			{
			iGloblePhone->iContextList[tmp]->iDefContextConfigGPRS.iAccessPointName = aAccessPointName;
			iGloblePhone->iContextList[tmp]->iDefContextConfigGPRS.iPdpType = aPdpType;
			iGloblePhone->iContextList[tmp]->iDefContextConfigGPRS.iPdpAddress = aPdpAddress;
			iGloblePhone->iContextList[tmp]->iDefContextConfigGPRS.iProtocolConfigOption.iMiscBuffer = aPcoBuffer;
			
						
			CATGPRSSetConfig* ATGPRSSetConfig = CATGPRSSetConfig::NewL(*iGloblePhone, iCtsyDispatcherCallback);
			iGloblePhone->iEventSignalActive = ETrue;
			ATGPRSSetConfig->SetContext(iGloblePhone->iContextList[tmp]->iContextID);
			iGloblePhone->SetActiveRequest(ATGPRSSetConfig);
			ATGPRSSetConfig->StartRequest();
			}
		else
		    {
	        LOGTEXTREL2(_L8("pkt:err(%d) in setconfig, not found"), tmp);
		    }
		}
	else
	    {
        LOGTEXTREL2(_L8("pkt:err(%d) in setconfig"), ret);
	    }
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleSetPdpContextConfigReqL


TInt CLtsyPacketServicesHandler::HandleModifyActivePdpContextReqL(const TDesC& /*aContextName*/)
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

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleModifyActivePdpContextReqL


TInt CLtsyPacketServicesHandler::HandleInitialisePdpContextReqL(const TDesC& aPrimaryContextName, const TDesC& /*aSecondaryContextName*/)
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
	LOGTEXT2(_L8("aPrimaryContextName >%S<"), &aPrimaryContextName);
	LOGTEXTREL2(_L8("pkt:initctxt>%S<"), &aPrimaryContextName);
	TInt ret = KErrNone;
	TInt cid = 0;
	TInt contextCount = iGloblePhone->iContextList.Count();
	if(contextCount > 0)
		{
		for(TInt i = 0;i < contextCount;i++)
			{
			if(iGloblePhone->iContextList[i]->iInfo.iName == aPrimaryContextName)
				{
		        LOGTEXTREL2(_L8("pkt:err(%d) in initctxt, already existed"), ret);
				return KErrNotSupported;
				}
			}
		}

	cid = GenerateNewContextCid(aPrimaryContextName);
	LOGTEXT2(_L8("newcid=%d"),cid);
	if(cid > MaxNumberOfContexts())
	    {
        LOGTEXTREL2(_L8("pkt:err(%d) in initctxt, context number overflow"), ret);
		return KErrNotSupported;
	    }
	
	TContextInfomation  *tmpContext = new TContextInfomation;
	tmpContext->iInfo.iName = aPrimaryContextName;
	tmpContext->iContextID = cid;
	iGloblePhone->iContextList.Append(tmpContext);
	LOGTEXT2(_L8("iGloblePhone->iContextList[0]->iContextID=%d"),iGloblePhone->iContextList[0]->iContextID);

	iInitialisePdpContext->SetContext(cid);
	iInitialisePdpContext->StartRequest();
	
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
	LOGTEXTREL2(_L8("pkt:delete aContextName:\t%S"),&aContextName);
	
	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(KErrNone == ret)
		{
		TInt tmp = -1;
		tmp = GetContext(aContextName);
		if(tmp >= 0)
			{		
			CATGprsContextDelete* ATGprsContextDelete = CATGprsContextDelete::NewL(*iGloblePhone, iCtsyDispatcherCallback);
			iGloblePhone->iEventSignalActive = ETrue;
			ATGprsContextDelete->SetContext(iGloblePhone->iContextList[tmp]->iContextID);
			iGloblePhone->SetActiveRequest(ATGprsContextDelete);
			ATGprsContextDelete->StartRequest();
			}
		else
			{
            LOGTEXTREL2(_L8("pkt:err(%d) in deletectxt, not found"), tmp);
			ret = KErrNotFound;
			}
		}
	else
	    {
        LOGTEXTREL2(_L8("pkt:err(%d) in deletectxt"), ret);
	    }
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleDeletePdpContextReqL


TInt CLtsyPacketServicesHandler::HandleSetPacketAttachModeReqL(RPacketService::TAttachMode /*aAttachMode*/)
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
	TInt ret = 0;
	SetDefaultContextConfigGPRS(&aContextConfigGprs);
	iAsyncHelperRequest->SetRequestId(KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId);
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleSetDefaultPdpContextGprsParamsReqL

TInt CLtsyPacketServicesHandler::HandleSetDefaultPdpContextR99R4ParamsReqL(const RPacketContext::TContextConfigR99_R4& /*aContextConfigR99R4*/)
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

    LOGTEXTREL(_L8("pkt:activatectxt"));
	TInt ret = KErrNotSupported;	
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(KErrNone == ret)
		{
		TInt tmp = GetContext(aContextName);
		if(tmp >= 0)
			{		
			CActivatContextAndGetAddr* ActivatContextAndGetAddr = CActivatContextAndGetAddr::NewL(*iGloblePhone, iCtsyDispatcherCallback);
			iGloblePhone->iEventSignalActive = ETrue;
			ActivatContextAndGetAddr->SetContext(iGloblePhone->iContextList[tmp]->iContextID);
			iGloblePhone->SetActiveRequest(ActivatContextAndGetAddr);
			ActivatContextAndGetAddr->StartRequest();
			}
		}
	else
	    {
        LOGTEXTREL2(_L8("pkt:err(%d) in activatectxt"), ret);
	    }
	
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
	
    LOGTEXTREL(_L8("pkt:setctxtQos"));
	TInt ret = KErrNotSupported;
	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(KErrNone == ret)
		{
		TInt tmp = GetContext(aContextName);
		if(tmp >= 0)
			{		
			CATGprsSetProfileReq* ATGprsSetProfileReq = CATGprsSetProfileReq::NewL(*iGloblePhone, iCtsyDispatcherCallback);
			iGloblePhone->iEventSignalActive = ETrue;
			iGloblePhone->iContextList[tmp]->iQoSConfig = aQoSConfig;
			ATGprsSetProfileReq->SetContext(iGloblePhone->iContextList[tmp]->iContextID);
			iGloblePhone->SetActiveRequest(ATGprsSetProfileReq);
			ATGprsSetProfileReq->StartRequest();
			}
		}
	else
	    {
        LOGTEXTREL2(_L8("pkt:err(%d) in setctxtQos"), ret);
	    }
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleSetPdpContextQosGprsReqL

TInt CLtsyPacketServicesHandler::HandleSetPdpContextQosR99R4ReqL(const TDesC& aContextName, const RPacketQoS::TQoSR99_R4Requested& /*aQoSConfig*/)
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
    
    RPacketQoS::TQoSGPRSRequested qosGprsReq;
    qosGprsReq.iReqPrecedence = RPacketQoS::EPriorityMediumPrecedence;
    qosGprsReq.iMinPrecedence = RPacketQoS::EPriorityMediumPrecedence;
    qosGprsReq.iReqDelay = RPacketQoS::EUnspecifiedDelayClass;
    qosGprsReq.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
    qosGprsReq.iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
    qosGprsReq.iMinReliability = RPacketQoS::EUnspecifiedReliabilityClass;
    qosGprsReq.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
    qosGprsReq.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
    qosGprsReq.iReqMeanThroughput = RPacketQoS::EMeanThroughputBestEffort;
    qosGprsReq.iMinMeanThroughput = RPacketQoS::EMeanThroughputBestEffort;

    return HandleSetPdpContextQosGprsReqL(aContextName, qosGprsReq);
    } // CLtsyPacketServicesHandler::HandleSetPdpContextQosR99R4ReqL

TInt CLtsyPacketServicesHandler::HandleSetPdpContextQosR5ReqL(const TDesC& aContextName, const RPacketQoS::TQoSR5Requested& /*aQoSConfig*/)
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
    
    RPacketQoS::TQoSGPRSRequested qosGprsReq;
    qosGprsReq.iReqPrecedence = RPacketQoS::EPriorityMediumPrecedence;
    qosGprsReq.iMinPrecedence = RPacketQoS::EPriorityMediumPrecedence;
    qosGprsReq.iReqDelay = RPacketQoS::EUnspecifiedDelayClass;
    qosGprsReq.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;
    qosGprsReq.iReqReliability = RPacketQoS::EUnspecifiedReliabilityClass;
    qosGprsReq.iMinReliability = RPacketQoS::EUnspecifiedReliabilityClass;
    qosGprsReq.iReqPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
    qosGprsReq.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
    qosGprsReq.iReqMeanThroughput = RPacketQoS::EMeanThroughputBestEffort;
    qosGprsReq.iMinMeanThroughput = RPacketQoS::EMeanThroughputBestEffort;

    return HandleSetPdpContextQosGprsReqL(aContextName, qosGprsReq);
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
    LOGTEXTREL2(_L8("pkt:deact ctxt>%S<"),&aContextName);
	TInt ret = KErrNotSupported;

	ret = iGloblePhone->CheckGlobalPhoneStatus();
	if(KErrNone == ret)
		{
		TInt tmp = GetContext(aContextName);
		if(tmp >= 0)
			{	
			CATGprsContextDeactivate* ATGprsContextDeactivate = CATGprsContextDeactivate::NewL(*iGloblePhone, iCtsyDispatcherCallback);
			iGloblePhone->iEventSignalActive = ETrue;
			ATGprsContextDeactivate->SetContext(iGloblePhone->iContextList[tmp]->iContextID);
			iGloblePhone->SetActiveRequest(ATGprsContextDeactivate);
			ATGprsContextDeactivate->StartRequest();
			ret = KErrNone;
			}
		else
		    {
	        LOGTEXTREL2(_L8("pkt:err(%d) in deact ctxt, not found"), tmp);
		    ret = KErrNotFound;
		    }
		}
	else
	    {
        LOGTEXTREL2(_L8("pkt:err(%d) in deact ctxt"), ret);
	    }
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleDeactivatePdpContextReqL

TInt CLtsyPacketServicesHandler::HandleAddPacketFilterSyncL(const TDesC& /*aContextName*/,  CArrayFixFlat<RPacketContext::TPacketFilterV2>& /*aContextFilter*/)
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
	TInt ret = 0;
	iAsyncHelperRequest->SetRequestId(KLtsyDispatchPacketServicesGetStatusApiId);
	
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
	TInt ret = 0;
	iAsyncHelperRequest->SetRequestId(KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId);
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

	TInt ret = 0;
	iAsyncHelperRequest->SetRequestId(KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId);
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
	TInt ret = 0;
	iAsyncHelperRequest->SetRequestId(KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId);
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleGetMaxNoActiveServicesReqL

TInt CLtsyPacketServicesHandler::HandleInitialiseMbmsContextReqL(const TDesC& /*aContextName*/)
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

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleInitialiseMbmsContextReqL

TInt CLtsyPacketServicesHandler::HandleGetMbmsNetworkServiceStatusReqL(TBool /*aAttemptAttach*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPacketServicesGetMbmsNetworkServiceStatusComp()
 * 
 * @param aAttemptAttach If a GMM attached is to be perfomed.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleGetMbmsNetworkServiceStatusReqL

TInt CLtsyPacketServicesHandler::HandleUpdateMbmsMonitorServiceListsReqL(TMbmsAction /*aAction*/, const CPcktMbmsMonitoredServiceList& /*aServiceList*/, const CPcktMbmsMonitoredServiceList& /*aChangeList*/)
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

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleUpdateMbmsMonitorServiceListsReqL


TInt CLtsyPacketServicesHandler::HandleUpdateMbmsSessionListReqL(const TDesC& /*aContextName*/, TMbmsAction /*aAction*/, TMbmsSessionId /*aSessionId*/, const TMbmsSessionIdList& /*aServiceList*/)
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

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleUpdateMbmsSessionListReqL


TInt CLtsyPacketServicesHandler::HandleRemovePacketFilterSyncL(const TDesC& /*aContextName*/, TInt /*aFilterId*/)
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

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPacketServicesHandler::HandleRemovePacketFilterSyncL

TInt CLtsyPacketServicesHandler::MaxNumberOfContexts()
/**
 * This function returns the maximum number of contexts that the phone supports.
 */
    {
    return iGloblePhone->iPhoneStatus.iGprsMaxNumContexts;
    }
TInt CLtsyPacketServicesHandler::GetContext(const TDesC& aName)
/**
 * This function returns a specific context
 * @param aCount context to Retrieve in linked list.
 */
    {
    TInt index = -1;
    TInt count = iGloblePhone->iContextList.Count();
    for(TInt i = 0; i < count; i++)
    	{
    	if(0 ==(iGloblePhone->iContextList[i]->iInfo.iName.Compare(aName)))
    		{
    		index = i;
    		}
    	}
    
    LOGTEXT3(_L8("[Ltsy Pkt] iContext->ContextName() >%S<, index=%d"), &aName, index);
    return index;
    }

TInt CLtsyPacketServicesHandler::GenerateNewContextCid(const TDesC& /*aName*/)
/**
 * This function generates a unique context name and the first not taken cid number.
 * There is a 1 to 1 mapping between this Cid and the one in the phone. 
 * In order to do this we need to check all existing Cids, since they may not
 * be in conical order and there can be holes in the list(3,1,4,6,5) we need 
 * to loop around and find the lowest that we can use to create a name and a new
 * Cid.
 * @param aName New name that is generated.
 * @param aCid New context id.
 */
    {
    TInt lowestAvailableCid = 1;
    TInt count = iGloblePhone->iContextList.Count();
    if(count != 0 )
        {
        // There can be gaps between the Cids that the tsy has, so get the
        // Cid number between them, if any. This nested for loop will find 
        // the lowest available cid there is.
        for(TInt k = 0; k < count;k++)
            {
            for(TInt i = 0; i < count;i++)
                {
                if(iGloblePhone->iContextList[i]->iContextID == lowestAvailableCid)
                    ++lowestAvailableCid;
                }
            }
        }
    return lowestAvailableCid;
    }

void CLtsyPacketServicesHandler::SetDefaultContextConfigGPRS(const RPacketContext::TContextConfigGPRS* aContextConfig)
/**
 * This function sets the default context configuration.
 *
 * @param aContextConfigV01 Set default context configuration.
 */
    {
    iDefContextConfigGPRS.iPdpType = aContextConfig->iPdpType;
    iDefContextConfigGPRS.iAccessPointName.Copy(aContextConfig->iAccessPointName);
    iDefContextConfigGPRS.iPdpAddress.Copy(aContextConfig->iPdpAddress);
    iDefContextConfigGPRS.iPdpCompression = aContextConfig->iPdpCompression;
    iDefContextConfigGPRS.iAnonymousAccessReqd = aContextConfig->iAnonymousAccessReqd;
    }
//
// End of file
