// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "pckttsy.h"
#include "testdef.h"
#include "etelpckt.h"
#include <et_clsvr.h>
#include "dpcktlog.h"
#include <pcktcs.h>

#include "coretsy.h"


/**************************************************************************/
//
//	CGprsDGprsTsy
//
/**************************************************************************/

CGprsDGprsTsy* CGprsDGprsTsy::NewL(CPhoneFactoryDummyBase* aFac)
/**
 * NewL method - Standard 2 phase constructor.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy: Entered NewL()"));

	CGprsDGprsTsy* subsession=new(ELeave) CGprsDGprsTsy(aFac);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CGprsDGprsTsy::Init()
	{}

CGprsDGprsTsy::CGprsDGprsTsy(CPhoneFactoryDummyBase* aFac)
	:CSubSessionExtDummyBase(aFac),
	iActiveRequestObjects(_FOFF(CGprsDGprsTsy::CActiveListNode,iNext))

	{
	}

void CGprsDGprsTsy::ConstructL()
/**
 * ConstructL method.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy: Entered constructor"));
	iMutex.CreateGlobal(KNullDesC, EOwnerProcess);
	iGetMbmsMonitoredServices = new CArrayPtrFlat<CListReadAllAttempt>(1);
	iList=CPcktMbmsMonitoredServiceList::NewL();
	}

CGprsDGprsTsy::~CGprsDGprsTsy()
/**
 * Destructor.
 */
	{
	if(iGetMbmsMonitoredServices)
		{
		iGetMbmsMonitoredServices->ResetAndDestroy();
		delete iGetMbmsMonitoredServices;
		}
  	if(iList)
  		{
  		delete iList;
  		}
  	iMutex.Close();
	LOGTEXT(_L8("CGprsDGprsTsy: Entered destructor"));
	}

CTelObject* CGprsDGprsTsy::OpenNewObjectByNameL(const TDesC& aName)
/**
 * 	Only gprs context objects can be opened from gprs object(s).
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy: OpenNewObjectByNameL()"));

	if (aName.Compare(_L("GPRS_CONTEXT"))==KErrNone)
		{
		return reinterpret_cast<CTelObject*>(CGprsContextDGprsTsy::NewL(FacPtr()));
		}
	return NULL;
	}

CTelObject* CGprsDGprsTsy::OpenNewObjectL(TDes& aNewName)
/**
 * A new context opened by a client will have its name allocated by the TSY.
 */
	{
	if(aNewName.Compare(KMBMSContextName) == KErrNone)
		{
		LOGTEXT(_L8("CGprsDGprsTsy: OpenNewObjectL()"));
		aNewName.AppendNum(++iMbmsContextObjectCount);
		return CMbmsContextDMbmsTsy::NewL(this,FacPtr());
		}
	else
		{
		LOGTEXT(_L8("CGprsDGprsTsy: OpenNewObjectL()"));
		aNewName.Append(_L("PACKET_CONTEXT"));
		aNewName.AppendNum(++iGprsContextObjectCount);
		return CGprsContextDGprsTsy::NewL(FacPtr());
		}
	}

CTelObject::TReqMode CGprsDGprsTsy::ReqModeL(const TInt aIpc)
/**
 * ReqModeL is called from the server's CTelObject::ReqAnalyserL in order to check the 
 * type of request it has.
 * The following are example request types for this dummy TSY. All TSYs do not need to 
 * have these request types but they have been given "sensible" values in this dummy TSY code.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy: ReqModeL() method"));

	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
		case EPacketAttach:
		case EPacketDetach:
		case EPacketRejectActivationRequest:
		case EPacketSetMSClass:
		case EPacketSetAttachMode:
		case EPacketSetDefaultContextParams:
		case EPacketGetStatus:
		case EPacketGetContextInfo:
		case EPacketGetNtwkRegStatus:
		case EPacketGetMSClass:
		case EPacketGetStaticCaps:
		case EPacketGetDynamicCaps:
		case EPacketGetAttachMode:
		case EPacketGetDefaultContextParams:
		case EPacketEnumerateContexts:
		case EPacketSetPrefBearer:
		case EPacketGetPrefBearer:
		case EPacketGetCurrentReleaseMode:
		case EPacketEnumerateNifs:
		case EPacketGetNifInfo:
		case EPacketEnumerateContextsInNif:
		case EPacketGetContextNameInNif:
		case EPacketPrepareOpenSecondary:
		case EPacketDeactivateNIF:
		case EPacketGetMbmsNetworkServiceStatus:
		case EPacketUpdateMbmsMonitorServiceList:
		case EPacketGetMbmsMonitoredServicesPhase1:
		case EPacketGetMbmsMonitoredServicesPhase2:
		case EPacketEnumerateMbmsMonitorServiceList:
		case EPacketEnumerateMbmsActiveServiceList:
			break;

		case EPacketNotifyContextAdded:
		case EPacketNotifyContextAddedCancel:
		case EPacketNotifyStatusChange:
		case EPacketNotifyContextActivationRequested:
		case EPacketNotifyChangeOfNtwkRegStatus:
		case EPacketNotifyMSClassChange:
		case EPacketNotifyDynamicCapsChange:
		case EPacketNotifyReleaseModeChange:
		case EPacketNotifyAttachModeChange:
		case EPacketNotifyMbmsNetworkServiceStatusChange:
		case EPacketNotifyMbmsServiceAvailabilityChange:
			ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
			break;
	
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	return ret;
	}

TInt CGprsDGprsTsy::RegisterNotification(const TInt aIpc)
/**
 * RegisterNotification is called when the server recognises that this notification
 * is being posted for the first time on this sub-session object.
 * It enables the TSY to "turn on" any regular notification messages that it may 
 * receive from the phone.
 */
	{
	switch (aIpc)
		{
		case EPacketNotifyContextAdded:
			LOGTEXT(_L8("CGprsDGprsTsy: RegisterNotification - Context added"));
			return KErrNone;
		case EPacketNotifyStatusChange:
			LOGTEXT(_L8("CGprsDGprsTsy: RegisterNotification - Gprs status change"));
			return KErrNone;
		case EPacketNotifyContextActivationRequested:
			LOGTEXT(_L8("CGprsDGprsTsy: RegisterNotification - Context activation requested"));
			return KErrNone;
		case EPacketNotifyChangeOfNtwkRegStatus:
			LOGTEXT(_L8("CGprsDGprsTsy: RegisterNotification - Network registration status change"));
			return KErrNone;
		case EPacketNotifyMSClassChange:
			LOGTEXT(_L8("CGprsDGprsTsy: RegisterNotification - MS class change"));
			return KErrNone;
		case EPacketNotifyDynamicCapsChange:
			LOGTEXT(_L8("CGprsDGprsTsy: RegisterNotification - Dynamic capabilities change"));
			return KErrNone;
		case EPacketNotifyReleaseModeChange:
			LOGTEXT(_L8("CGprsDGprsTsy: RegisterNotification - Network Release mode change"));
			return KErrNone;
		case EPacketNotifyAttachModeChange:
			LOGTEXT(_L8("CGprsDGprsTsy: RegisterNotification - Network attach mode change"));
			return KErrNone;
		case EPacketNotifyMbmsNetworkServiceStatusChange:
			LOGTEXT(_L8("CGprsDGprsTsy: RegisterNotification - Network Mbms Service Status change"));
			return KErrNone;
		case EPacketNotifyMbmsServiceAvailabilityChange:
			LOGTEXT(_L8("CGprsDGprsTsy: RegisterNotification - Network Mbms Service change"));
			return KErrNone;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsDGprsTsy: Register error, unknown IPC"));
			return KErrNotSupported;
		}
	}

TInt CGprsDGprsTsy::DeregisterNotification(const TInt aIpc)
/**
 * DeregisterNotification is called when the server recognises that this notification
 * will not be posted again because the last client to have a handle on this sub-session
 * object has just closed the handle.
 * It enables the TSY to "turn off" any regular notification messages that it may 
 * receive from the phone.
 */
	{
	switch (aIpc)
		{
		case EPacketNotifyContextAdded:
			LOGTEXT(_L8("CGprsDGprsTsy: DeregisterNotification - Context added"));
			return KErrNone;
		case EPacketNotifyStatusChange:
			LOGTEXT(_L8("CGprsDGprsTsy: DeregisterNotification - Gprs status change"));
			return KErrNone;
		case EPacketNotifyContextActivationRequested:
			LOGTEXT(_L8("CGprsDGprsTsy: DeregisterNotification - Context activation requested"));
			return KErrNone;
		case EPacketNotifyChangeOfNtwkRegStatus:
			LOGTEXT(_L8("CGprsDGprsTsy: DeregisterNotification - Network registration status change"));
			return KErrNone;
		case EPacketNotifyMSClassChange:
			LOGTEXT(_L8("CGprsDGprsTsy: DeregisterNotification - MS class change"));
			return KErrNone;
		case EPacketNotifyDynamicCapsChange:
			LOGTEXT(_L8("CGprsDGprsTsy: DeregisterNotification - Dynamic capabilities change"));
			return KErrNone;
		case EPacketNotifyReleaseModeChange:
			LOGTEXT(_L8("CGprsDGprsTsy: DeregisterNotification - Network release mode change"));
			return KErrNone;
		case EPacketNotifyAttachModeChange:
			LOGTEXT(_L8("CGprsDGprsTsy: DeregisterNotification - Network attach mode change"));
			return KErrNone;
		case EPacketNotifyMbmsNetworkServiceStatusChange:
			LOGTEXT(_L8("CGprsDGprsTsy: DeregisterNotification - Network Mbms Service Status change"));
			return KErrNone;
		case EPacketNotifyMbmsServiceAvailabilityChange:
			LOGTEXT(_L8("CGprsDGprsTsy: DeregisterNotification - Network Mbms Service change"));
			return KErrNone;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsDGprsTsy: Deregister error, unknown IPC"));
			return KErrNotSupported;
		}
	}

TInt CGprsDGprsTsy::NumberOfSlotsL(const TInt aIpc)
/**
 * NumberOfSlotsL is called by the server when it is registering a new notification.
 * It enables the TSY to tell the server how many buffer slots to allocate for
 * "repost immediately" notifications that may trigger before clients collect them.
 */
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
		case EPacketNotifyStatusChange:
		case EPacketNotifyDynamicCapsChange:
		case EPacketNotifyMbmsNetworkServiceStatusChange:
			LOGTEXT(_L8("CGprsDGprsTsy: Registered with 10 slots"));
			numberOfSlots=10;
			break;
		case EPacketNotifyContextAdded:
		case EPacketNotifyContextActivationRequested:
		case EPacketNotifyChangeOfNtwkRegStatus:
		case EPacketNotifyMSClassChange:
		case EPacketNotifyReleaseModeChange:
		case EPacketNotifyAttachModeChange:
		case EPacketNotifyMbmsServiceAvailabilityChange:
			LOGTEXT(_L8("CGprsDGprsTsy: Registered with 2 slots"));
			numberOfSlots=2;
			break;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsDGprsTsy: Number of Slots error, unknown IPC"));
			User::Leave(KErrNotSupported);
			break;
		}  
	return numberOfSlots;
	}


TInt CGprsDGprsTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
/**
 * ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
 * for the TSY to process.
 * A request handle, request type and request data are passed to the TSY.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy: ExtFunc() method"));

	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
		case EPacketNotifyContextAdded:
			return NotifyContextAdded(aTsyReqHandle, aPackage.Des1u());
		case EPacketGetStatus:
			return GetStatus(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TStatus*, dataPtr));
		case EPacketNotifyStatusChange:
			return NotifyStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TStatus*, dataPtr));
		case EPacketNotifyContextActivationRequested:
			return NotifyContextActivationRequested(aTsyReqHandle,
				REINTERPRET_CAST(TPacketDataConfigBase*, dataPtr));
		case EPacketEnumerateContexts:
			return EnumerateContexts(aTsyReqHandle,
			REINTERPRET_CAST(TInt*, dataPtr),
			REINTERPRET_CAST(TInt*, dataPtr2));
		case EPacketGetContextInfo:
			return GetContextInfo(aTsyReqHandle,
			REINTERPRET_CAST(TInt*, dataPtr),
			REINTERPRET_CAST(RPacketService::TContextInfo*, dataPtr2));
		case EPacketGetNtwkRegStatus:
			return GetNtwkRegStatus(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TRegistrationStatus*, dataPtr));
		case EPacketNotifyChangeOfNtwkRegStatus:
			return NotifyChangeOfNtwkRegStatus(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TRegistrationStatus*, dataPtr));
		case EPacketGetMSClass:
			return GetMSClass(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr),
			REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr2));
		case EPacketSetMSClass:
			return SetMSClass(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr));
		case EPacketNotifyMSClassChange:
			return NotifyMSClassChange(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TMSClass*, dataPtr));
		case EPacketGetStaticCaps:
			return GetStaticCaps(aTsyReqHandle, 
				REINTERPRET_CAST(TUint*, dataPtr),
				REINTERPRET_CAST(RPacketContext::TProtocolType*, dataPtr2));
		case EPacketGetDynamicCaps:
			return GetDynamicCaps(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TDynamicCapsFlags*, dataPtr));
		case EPacketNotifyDynamicCapsChange:
			return NotifyDynamicCapsChange(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TDynamicCapsFlags*, dataPtr));
		case EPacketSetAttachMode:
			return SetAttachMode(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TAttachMode*, dataPtr));
		case EPacketGetAttachMode:
			return GetAttachMode(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TAttachMode*, dataPtr));
		case EPacketNotifyAttachModeChange:
			return NotifyAttachModeChange(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TAttachMode*, dataPtr));
		case EPacketNotifyContextAddedCancel:
			return NotifyContextAddedCancel(aTsyReqHandle);
		case EPacketAttach:
			return Attach(aTsyReqHandle);
		case EPacketAttachCancel:
			return AttachCancel(aTsyReqHandle);
		case EPacketDetach:
			return Detach(aTsyReqHandle);
		case EPacketDetachCancel:
			return DetachCancel(aTsyReqHandle);
		case EPacketNotifyStatusChangeCancel:
			return NotifyStatusChangeCancel(aTsyReqHandle);
		case EPacketNotifyContextActivationRequestedCancel:
			return NotifyContextActivationRequestedCancel(aTsyReqHandle);
		case EPacketRejectActivationRequest:
			return RejectActivationRequest(aTsyReqHandle);
		case EPacketRejectActivationRequestCancel:
			return RejectActivationRequestCancel(aTsyReqHandle);
		case EPacketGetContextInfoCancel:
			return GetContextInfoCancel(aTsyReqHandle);
		case EPacketNotifyChangeOfNtwkRegStatusCancel:
			return NotifyChangeOfNtwkRegStatusCancel(aTsyReqHandle);
		case EPacketGetMSClassCancel:
			return GetMSClassCancel(aTsyReqHandle);
		case EPacketSetMSClassCancel:
			return SetMSClassCancel(aTsyReqHandle);
		case EPacketNotifyMSClassChangeCancel:
			return NotifyMSClassChangeCancel(aTsyReqHandle);
		case EPacketSetPrefBearer:
			return SetPreferredBearer(aTsyReqHandle, 
				REINTERPRET_CAST(RPacketService::TPreferredBearer*, dataPtr));
		case EPacketGetPrefBearer:
			return GetPreferredBearer(aTsyReqHandle,
				REINTERPRET_CAST(RPacketService::TPreferredBearer*, dataPtr));
		case EPacketNotifyDynamicCapsChangeCancel:
			return NotifyDynamicCapsChangeCancel(aTsyReqHandle);
		case EPacketSetDefaultContextParams:
			return SetDefaultContextParams(aTsyReqHandle, aPackage.Des1n());
		case EPacketGetDefaultContextParams:
			return GetDefaultContextParams(aTsyReqHandle, aPackage.Des1n());
		case EPacketGetCurrentReleaseMode:
			return GetCurrentReleaseMode(aTsyReqHandle,
				REINTERPRET_CAST(RPacketService::TPacketReleaseMode*, dataPtr));
		case EPacketNotifyReleaseModeChange:
			return NotifyReleaseModeChange(aTsyReqHandle,
			REINTERPRET_CAST(RPacketService::TPacketReleaseMode*, dataPtr));
		case EPacketEnumerateNifs:
			return EnumerateNifs(aTsyReqHandle,
				REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketEnumerateNifsCancel:
			return EnumerateNifsCancel(aTsyReqHandle);
		case EPacketGetNifInfo:
			return GetNifInfo(aTsyReqHandle,
				REINTERPRET_CAST(TInt*, dataPtr),
				aPackage.Des2n());
		case EPacketGetNifInfoCancel:
			return GetNifInfoCancel(aTsyReqHandle);	
		case EPacketEnumerateContextsInNif:
			return EnumerateContextsInNif(aTsyReqHandle,
				aPackage.Des2u(),
				REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketEnumerateContextsInNifCancel:
			return EnumerateContextsInNifCancel(aTsyReqHandle);
		case EPacketGetContextNameInNif:
			return GetContextNameInNif(aTsyReqHandle,
				REINTERPRET_CAST(RPacketService::TContextNameInNif*, dataPtr),
				aPackage.Des2u());
		case EPacketGetContextNameInNifCancel:
			return GetContextNameInNifCancel(aTsyReqHandle);
		case EPacketPrepareOpenSecondary:
			return PrepareOpenSecondary(aTsyReqHandle, aPackage.Des1u());
		case EPacketDeactivateNIF:
			return DeactivateNIF(aTsyReqHandle, REINTERPRET_CAST(TDesC*,dataPtr));
		case EPacketNotifyMbmsNetworkServiceStatusChange:
			return NotifyMbmsNetworkServiceStatusChange(aTsyReqHandle,
			reinterpret_cast<TMbmsNetworkServiceStatus*>(dataPtr));
		case EPacketGetMbmsNetworkServiceStatus:
			return GetMbmsNetworkServiceStatus(aTsyReqHandle,
			reinterpret_cast<TBool*>(dataPtr),
			reinterpret_cast<TMbmsNetworkServiceStatus*>(dataPtr2));
		case EPacketNotifyMbmsServiceAvailabilityChange:
			return NotifyMbmsServiceAvailabilityChange(aTsyReqHandle);
		case EPacketUpdateMbmsMonitorServiceList:
			if(dataPtr2)
				{
				return UpdateMbmsMonitorServiceListL(aTsyReqHandle,
				reinterpret_cast<TMbmsAction*>(dataPtr),aPackage.Des2n());	
				}
			else
				{
				return UpdateMbmsMonitorServiceListL(aTsyReqHandle,
				reinterpret_cast<TMbmsAction*>(dataPtr));
				}
		case EPacketGetMbmsMonitoredServicesPhase1:
			return GetMbmsMonitoredServicesPhase1(aTsyReqHandle, 
			reinterpret_cast<TClientId*>(dataPtr), 
			reinterpret_cast<TUint*>(dataPtr2));
		case EPacketGetMbmsMonitoredServicesPhase2:
			return GetMbmsMonitoredServicesPhase2(aTsyReqHandle, 
			reinterpret_cast<TClientId*>(dataPtr),aPackage.Des2n());
		case EPacketEnumerateMbmsMonitorServiceList:
			return EnumerateMbmsMonitorServiceList(aTsyReqHandle, 
			reinterpret_cast<TInt*>(dataPtr),
			reinterpret_cast<TInt*>(dataPtr2));
		case EPacketEnumerateMbmsActiveServiceList:
			return EnumerateMbmsActiveServiceList(aTsyReqHandle, 
			reinterpret_cast<TInt*>(dataPtr),
			reinterpret_cast<TInt*>(dataPtr2));
		default:
			return KErrNotSupported;
		}
	}

TInt CGprsDGprsTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
 * CancelService is called by the server when it is "cleaning-up" any still outstanding
 * asynchronous requests before closing a client's sub-session.
 * This will happen if a client closes its R-class handle without cancelling outstanding
 * asynchronous requests.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy: - CancelService called"));
	switch (aIpc)
		{
		case EPacketNotifyContextAdded:
			return NotifyContextAddedCancel(aTsyReqHandle);
		case EPacketAttach:
			return AttachCancel(aTsyReqHandle);
		case EPacketDetach:
			return DetachCancel(aTsyReqHandle);
		case EPacketNotifyStatusChange:
			return NotifyStatusChangeCancel(aTsyReqHandle);
		case EPacketNotifyContextActivationRequested:
			return NotifyContextActivationRequestedCancel(aTsyReqHandle);
		case EPacketRejectActivationRequest:
			return RejectActivationRequestCancel(aTsyReqHandle);
		case EPacketGetContextInfo:
			return GetContextInfoCancel(aTsyReqHandle);
		case EPacketNotifyChangeOfNtwkRegStatus:
			return NotifyChangeOfNtwkRegStatusCancel(aTsyReqHandle);
		case EPacketGetMSClass:
			return GetMSClassCancel(aTsyReqHandle);
		case EPacketSetMSClass:
			return SetMSClassCancel(aTsyReqHandle);
		case EPacketNotifyMSClassChange:
			return NotifyMSClassChangeCancel(aTsyReqHandle);
		case EPacketNotifyDynamicCapsChange:
			return NotifyDynamicCapsChangeCancel(aTsyReqHandle);
		case EPacketSetPrefBearer:
			return SetPreferredBearerCancel(aTsyReqHandle);
		case EPacketGetPrefBearer:
			return GetPreferredBearerCancel(aTsyReqHandle);
		case EPacketSetAttachMode:
			return SetAttachModeCancel(aTsyReqHandle);
		case EPacketGetAttachMode:
			return GetAttachModeCancel(aTsyReqHandle);
		case EPacketNotifyAttachModeChange:
			return NotifyAttachModeChangeCancel(aTsyReqHandle);
		case EPacketSetDefaultContextParams:
			return SetDefaultContextParamsCancel(aTsyReqHandle);
		case EPacketGetDefaultContextParams:
			return GetDefaultContextParamsCancel(aTsyReqHandle);
		case EPacketGetCurrentReleaseMode:
			return GetCurrentReleaseModeCancel(aTsyReqHandle);
		case EPacketNotifyReleaseModeChange:
			return NotifyReleaseModeChangeCancel(aTsyReqHandle);
		case EPacketEnumerateNifs:
			return EnumerateNifsCancel(aTsyReqHandle);
		case EPacketGetNifInfo:
			return GetNifInfoCancel(aTsyReqHandle);
		case EPacketEnumerateContextsInNif:
			return EnumerateContextsInNifCancel(aTsyReqHandle);
		case EPacketGetContextNameInNif:
			return GetContextNameInNifCancel(aTsyReqHandle);
		case EPacketDeactivateNIF:
			return DeactivateNIFCancel(aTsyReqHandle);
		case EPacketNotifyMbmsNetworkServiceStatusChange:
			return NotifyMbmsNetworkServiceStatusChangeCancel(aTsyReqHandle);
		case EPacketGetMbmsNetworkServiceStatus:
			return GetMbmsNetworkServiceStatusCancel(aTsyReqHandle);
		case EPacketNotifyMbmsServiceAvailabilityChange:
			return NotifyMbmsServiceAvailabilityChangeCancel(aTsyReqHandle);
		case EPacketUpdateMbmsMonitorServiceList:
			return UpdateMbmsMonitorServiceListCancel(aTsyReqHandle);
		case EPacketGetMbmsMonitoredServicesPhase1:
		case EPacketGetMbmsMonitoredServicesPhase2:
			return GetMbmsMonitoredServicesCancel(aTsyReqHandle);
		case EPacketEnumerateMbmsMonitorServiceList:
			return EnumerateMbmsMonitorServiceListCancel(aTsyReqHandle);
		case EPacketEnumerateMbmsActiveServiceList:
			return EnumerateMbmsActiveServiceListCancel(aTsyReqHandle);
		default:
			return KErrGeneral; 
		} 
	}


/***********************************************************************************/
//
// The following methods are called from ExtFunc and/or CancelService.
// Each of these will process a TSY request or cancel a TSY request
// Here, example values are returned or checked within this dummy TSY in order to ensure
// that the integrity of the data passed to/from client is maintained
//
/***********************************************************************************/
TInt CGprsDGprsTsy::NotifyContextAdded(const TTsyReqHandle aTsyReqHandle, TDes* aContextId)
	{
	User::After(300000); // wait to have KRequestPending
	if (!iNotifyContextAdded++)
		{
		LOGTEXT(_L8("CGprsDGprsTsy::NotifyContextAdded called"));
		*aContextId = DPCKTTSY_CONTEXT_ID;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyContextAddedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyContextAddedCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::Attach(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::Attach called"));
	User::After(300000); // wait to have KRequestPending
	if(!iAttach++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::AttachCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::AttachCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::Detach(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::Detach called"));
	User::After(300000); // wait to have KRequestPending
	if(!iDetach++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::DetachCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy:: called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetStatus(const TTsyReqHandle aTsyReqHandle, RPacketService::TStatus* aGprsStatus)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::DetachCancel called"));
	User::After(300000); // wait to have KRequestPending
	*aGprsStatus = DPCKTTSY_PACKET_STATUS1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TStatus* aGprsStatus)
	{
	User::After(300000); // wait to have KRequestPending
	if (!iNotifyStatusChange++)
		{
		LOGTEXT(_L8("CGprsDGprsTsy::NotifyStatusChange called"));
		*aGprsStatus = DPCKTTSY_PACKET_STATUS2;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyStatusChangeCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyContextActivationRequested(const TTsyReqHandle aTsyReqHandle, TPacketDataConfigBase* aConfig)
	{
	User::After(300000); // wait to have KRequestPending
	if (!iNotifyContextActivationRequested++)
		{
		LOGTEXT(_L8("CGprsDGprsTsy::NotifyContextActivationRequested called"));
		
		if (aConfig->ExtensionId() == TPacketDataConfigBase::KConfigGPRS)
			{
			RPacketContext::TContextConfigGPRS* aGprsConfig = (RPacketContext::TContextConfigGPRS*) aConfig;
			aGprsConfig->iPdpType = DPCKTTSY_PDP_TYPE1;
			aGprsConfig->iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
			aGprsConfig->iAccessPointName = DPCKTTSY_ACCESS_POINT1;
			aGprsConfig->iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
			aGprsConfig->iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
			aGprsConfig->iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
			aGprsConfig->iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
			aGprsConfig->iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (aConfig->ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
			{
			RPacketContext::TContextConfigR99_R4* aR99Config = (RPacketContext::TContextConfigR99_R4*) aConfig;
			aR99Config->iPdpType = DPCKTTSY_PDP_TYPE1;
			aR99Config->iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
			aR99Config->iAccessPointName = DPCKTTSY_ACCESS_POINT1;
			aR99Config->iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
			aR99Config->iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
			aR99Config->iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
			aR99Config->iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
			aR99Config->iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
			aR99Config->iPFI = DPCKTTSY_PFICONTEXT_SIG;
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (aConfig->ExtensionId()==TPacketDataConfigBase::KConfigRel5)
			{
			RPacketContext::TContextConfig_R5* aR5Config = (RPacketContext::TContextConfig_R5*) aConfig;
			aR5Config->iPdpType = DPCKTTSY_PDP_TYPE1;
			aR5Config->iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
			aR5Config->iAccessPointName = DPCKTTSY_ACCESS_POINT1;
			aR5Config->iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
			aR5Config->iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
			aR5Config->iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
			aR5Config->iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
			aR5Config->iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
			aR5Config->iPFI = DPCKTTSY_PFICONTEXT_SIG;
			aR5Config->iPdpDataCompression = DPCKTTSY_DATACOMPRESSION_EV42;
			aR5Config->iPdpHeaderCompression = DPCKTTSY_HEADERCOMPRESSION_ERFC1144;
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else
			ReqCompleted(aTsyReqHandle, KErrNotSupported);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyContextActivationRequestedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyContextActivationRequestedCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::RejectActivationRequest(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::RejectActivationRequest called"));
	User::After(300000); // wait to have KRequestPending
	if(!iRejectActivationRequest++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::RejectActivationRequestCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::RejectActivationRequestCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::EnumerateContexts(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::EnumerateContexts called"));
	User::After(300000); // wait to have KRequestPending
	*aCount = DPCKTTSY_NUMBER_OF_CONTEXTS;
	*aMaxAllowed = DPCKTTSY_MAX_NUMBER_OF_CONTEXTS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetContextInfo(const TTsyReqHandle aTsyReqHandle, TInt* aIndex,RPacketService::TContextInfo* aInfo)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetContextInfo called"));

	User::After(300000); // wait to have KRequestPending
	switch (*aIndex)
		{
		case 0:
			if(!iGetContextInfo++)
				{
				aInfo->iName=DPCKTTSY_CONTEXT_INFO_NAME0;
				aInfo->iStatus=DPCKTTSY_CONTEXT_INFO_STATUS0;
				ReqCompleted(aTsyReqHandle,KErrNone);
				}
			break;
		case 1:
			aInfo->iName=DPCKTTSY_CONTEXT_INFO_NAME1;
			aInfo->iStatus=DPCKTTSY_CONTEXT_INFO_STATUS1;
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;
		default:
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			break;
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetContextInfoCancel(const TTsyReqHandle aTsyReqHandle) 
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetContextInfoCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetNtwkRegStatus(const TTsyReqHandle aTsyReqHandle, RPacketService::TRegistrationStatus* aRegistrationStatus)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetNtwkRegStatus called"));
	User::After(300000); // wait to have KRequestPending
	*aRegistrationStatus = DPCKTTSY_REGISTRATION_STATUS1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyChangeOfNtwkRegStatus(const TTsyReqHandle aTsyReqHandle, RPacketService::TRegistrationStatus* aRegistrationStatus)
	{
	User::After(300000); // wait to have KRequestPending
	if (!iNotifyChangeOfNtwkRegStatus++)
		{
		LOGTEXT(_L8("CGprsDGprsTsy::NotifyChangeOfNtwkRegStatus called"));
		*aRegistrationStatus = DPCKTTSY_REGISTRATION_STATUS2;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyChangeOfNtwkRegStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyChangeOfNtwkRegStatusCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetMSClass(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aCurrentClass, RPacketService::TMSClass* aMaxClass)
	{
	LOGTEXT(_L8("CGprsDGprsTsy:: GetMSClass called"));
	User::After(300000); // wait to have KRequestPending
	if(!iGetMSClass++)
		{
		*aCurrentClass=DPCKTTSY_CURRENT_MS_CLASS;
		*aMaxClass=DPCKTTSY_MAX_MS_CLASS;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetMSClassCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetMSClass called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::SetMSClass(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aClass)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::SetMSClass called"));
	User::After(300000); // wait to have KRequestPending
	if (*aClass!=DPCKTTSY_SET_MS_CLASS) 
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		if(!iSetMSClass++)
			ReqCompleted(aTsyReqHandle,KErrNone);
		iTsyAsyncReqHandle = aTsyReqHandle;
		}
	return KErrNone;
	}

TInt CGprsDGprsTsy::SetMSClassCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::SetMSClassCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyMSClassChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TMSClass* aNewClass)
	{
	User::After(300000); // wait to have KRequestPending
	if (!iNotifyMSClassChange++)
		{
		LOGTEXT(_L8("CGprsDGprsTsy::NotifyMSClassChange called"));
		*aNewClass = DPCKTTSY_NEW_MS_CLASS;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyMSClassChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyMSClassChangeCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetStaticCaps(const TTsyReqHandle aTsyReqHandle, TUint* aCaps,RPacketContext::TProtocolType* aPdpType)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetStaticCaps called"));

	User::After(300000); // wait to have KRequestPending
	if (*aPdpType!=DPCKTTSY_PDP_TYPE1) 
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		*aCaps = DPCKTTSY_MISC_CAPS;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetDynamicCaps(const TTsyReqHandle aTsyReqHandle, RPacketService::TDynamicCapsFlags* aCaps)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetDynamicCaps called"));
	User::After(300000); // wait to have KRequestPending
	*aCaps = DPCKTTSY_DYNAMIC_CAPS1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyDynamicCapsChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TDynamicCapsFlags* aCaps)
	{
	User::After(300000); // wait to have KRequestPending
	if (!iNotifyDynamicCapsChange++)
		{
		LOGTEXT(_L8("CGprsDGprsTsy::NotifyDynamicCapsChange called"));
		*aCaps = DPCKTTSY_DYNAMIC_CAPS2;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyDynamicCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyDynamicCapsChangeCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::SetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode)
/** 
 * SetAttachMode method - Sets the Attach mode for the context.
 * A synchronous and asynchronous variant of this method is now supported - Change 
 * Request: NDOE-58VF4Q. In the test code, the synchronous method is called first.
 * Note that the synchronous variant is deprecated.
 * In the switch statement, cases 0 and 1 implement the synchronous call, while cases 2 and 
 * 3 implement the asynchronous call.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::SetAttachMode called"));

	User::After(300000); // wait to have KRequestPending
	switch (iSetAttachMode++)
		{
		case 0:
		case 1:
			if (*aMode!=DPCKTTSY_ATTACH_MODE1)
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
			else
				ReqCompleted(aTsyReqHandle,KErrNone);
			break;
		case 2:
			if (*aMode!=DPCKTTSY_ATTACH_MODE2)
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
			else
				ReqCompleted(aTsyReqHandle,KErrNone);
			iTsyAsyncReqHandle = aTsyReqHandle;
			break;
		case 3:
			if (*aMode!=DPCKTTSY_ATTACH_MODE2)
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
			iTsyAsyncReqHandle = aTsyReqHandle;
			break;
		default:
			LOGTEXT(_L8("CGprsDGprsTsy::SetAttachMode - Error in switch statement"));
			break;
		} // switch
		
	return KErrNone;
	}

TInt CGprsDGprsTsy::SetAttachModeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::SetAttachModeCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetAttachMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode)
/** 
 * GetAttachMode method - Gets the Attach mode for the context.
 * A synchronous and asynchronous variant of this method is now supported - Change 
 * Request: NDOE-58VF4Q. In the test code, the synchronous method is called first.
 * Note that the synchronous variant is deprecated.
 * In the switch statement, cases 0 and 1 implement the synchronous call, while cases 2 and 
 * 3 implement the asynchronous call.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetAttachMode called"));

	User::After(300000); // wait to have KRequestPending
	switch (iGetAttachMode++)
		{
		case 0:
		case 1:
			*aMode = DPCKTTSY_ATTACH_MODE2;
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;
		case 2:
			*aMode = DPCKTTSY_ATTACH_MODE1;
			ReqCompleted(aTsyReqHandle,KErrNone);
		case 3:
			iTsyAsyncReqHandle = aTsyReqHandle;
			break;
		default:
			LOGTEXT(_L8("CGprsDGprsTsy::GetAttachMode - Error in switch statement"));
			break;
		} // switch
		
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetAttachModeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetAttachModeCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}
	
TInt CGprsDGprsTsy::NotifyAttachModeChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TAttachMode* aMode)
	{
	User::After(300000); // wait to have KRequestPending
	if (!iNotifyAttachModeChange++)
		{
		LOGTEXT(_L8("CGprsDGprsTsy::NotifyAttachModeChange called"));
		*aMode = DPCKTTSY_ATTACH_MODE2;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyAttachModeChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyAttachModeChange called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::SetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, const TDesC8* aPckg)
/** 
 * SetDefaultContextParams method - Sets the Default context parameters.
 * A synchronous and asynchronous variant of this method is now supported - Change 
 * Request: NDOE-58VF4Q. In the test code, the synchronous method is called first.
 * Note that the synchronous variant is deprecated.
 * In the switch statement, cases 0 and 1 implement the synchronous call, while cases 2 and 
 * 3 implement the asynchronous call.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::SetDefaultContextParams called"));

	User::After(300000); // wait to have KRequestPending
	TPckg<TPacketDataConfigBase>* configBase = (TPckg<TPacketDataConfigBase>*)aPckg;
	TPacketDataConfigBase& configBaseV1 = (*configBase)();

	switch (iSetDefaultContextParams++)
		{
		case 0:
		case 1:
			if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
			{
			TPckg<RPacketContext::TContextConfigGPRS>* defaultContextGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aPckg;
			RPacketContext::TContextConfigGPRS& defaultContextV1 = (*defaultContextGPRSPckg)();

			if ((defaultContextV1.iAccessPointName != DPCKTTSY_ACCESS_POINT1)||
			(defaultContextV1.iAnonymousAccessReqd != DPCKTTSY_ANONYMOUS_ACCESS1)||
			(defaultContextV1.iPdpAddress != DPCKTTSY_PDP_ADDRESS1)||
			(defaultContextV1.iPdpCompression != DPCKTTSY_COMPRESSION1)||
			(defaultContextV1.iPdpType != DPCKTTSY_PDP_TYPE1) ||
			(defaultContextV1.iUseEdge != DPCKTTSY_EGPRS_REQUIRED) ||
			(defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername != DPCKTTSY_USER_NAME1) ||
			(defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword != DPCKTTSY_PASSWORD1) ||
			(defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol != DPCKTTSY_PROTOCOL_TYPE1_0) ||
			(defaultContextV1.iNWIContext != DPCKTTSY_NWICONTEXT_TRUE))
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
			else
				ReqCompleted(aTsyReqHandle,KErrNone);
			break;
			}
		else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
			{
			TPckg<RPacketContext::TContextConfigR99_R4>* contextConfigR99Pckg = (TPckg<RPacketContext::TContextConfigR99_R4>*)aPckg;
			RPacketContext::TContextConfigR99_R4& defaultContextV1 = (*contextConfigR99Pckg)();
			
			if ((defaultContextV1.iAccessPointName != DPCKTTSY_ACCESS_POINT1)||
			(defaultContextV1.iPdpAddress != DPCKTTSY_PDP_ADDRESS1)||
			(defaultContextV1.iPdpType != DPCKTTSY_PDP_TYPE1) ||
			(defaultContextV1.iUseEdge != DPCKTTSY_EGPRS_REQUIRED) ||
			(defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername != DPCKTTSY_USER_NAME1) ||
			(defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword != DPCKTTSY_PASSWORD1) ||
			(defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol != DPCKTTSY_PROTOCOL_TYPE1_0) ||
			(defaultContextV1.iNWIContext != DPCKTTSY_NWICONTEXT_TRUE) ||
			(defaultContextV1.iPFI != DPCKTTSY_PFICONTEXT_SMS))
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
			else
				ReqCompleted(aTsyReqHandle,KErrNone);
			break;
			}
		else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
			{
			TPckg<RPacketContext::TContextConfig_R5>* contextConfigR5Pckg = (TPckg<RPacketContext::TContextConfig_R5>*)aPckg;
			RPacketContext::TContextConfig_R5& defaultContextV1 = (*contextConfigR5Pckg)();
			
			if ((defaultContextV1.iAccessPointName != DPCKTTSY_ACCESS_POINT1)||
			(defaultContextV1.iPdpAddress != DPCKTTSY_PDP_ADDRESS1)||
			(defaultContextV1.iPdpType != DPCKTTSY_PDP_TYPE1) ||
			(defaultContextV1.iUseEdge != DPCKTTSY_EGPRS_REQUIRED) ||
			(defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername != DPCKTTSY_USER_NAME1) ||
			(defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword != DPCKTTSY_PASSWORD1) ||
			(defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol != DPCKTTSY_PROTOCOL_TYPE1_0) ||
			(defaultContextV1.iNWIContext != DPCKTTSY_NWICONTEXT_TRUE) ||
			(defaultContextV1.iPFI != DPCKTTSY_PFICONTEXT_SMS) ||
			(defaultContextV1.iPdpDataCompression != DPCKTTSY_DATACOMPRESSION_EV42) ||
			(defaultContextV1.iPdpHeaderCompression != DPCKTTSY_HEADERCOMPRESSION_ERFC1144))
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
			else
				ReqCompleted(aTsyReqHandle,KErrNone);
			break;
			}
		case 2:
		case 4:
			if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
				{
				TPckg<RPacketContext::TContextConfigGPRS>* defaultContextGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aPckg;
				RPacketContext::TContextConfigGPRS& defaultContextV1 = (*defaultContextGPRSPckg)();
				
				if ((defaultContextV1.iAccessPointName != DPCKTTSY_ACCESS_POINT2)||
				(defaultContextV1.iAnonymousAccessReqd != DPCKTTSY_ANONYMOUS_ACCESS2)||
				(defaultContextV1.iPdpAddress != DPCKTTSY_PDP_ADDRESS2)||
				(defaultContextV1.iPdpCompression != DPCKTTSY_COMPRESSION2)||
				(defaultContextV1.iPdpType != DPCKTTSY_PDP_TYPE2) ||
				(defaultContextV1.iUseEdge != DPCKTTSY_EGPRS_NOTREQUIRED) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername != DPCKTTSY_USER_NAME2) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword != DPCKTTSY_PASSWORD2) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol != DPCKTTSY_PROTOCOL_TYPE2_0) ||
				(defaultContextV1.iNWIContext != DPCKTTSY_NWICONTEXT_TRUE))
					ReqCompleted(aTsyReqHandle,KErrCorrupt);
				else
					ReqCompleted(aTsyReqHandle,KErrNone);
				
				iTsyAsyncReqHandle = aTsyReqHandle;
				break;
				}
			else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
				{
				TPckg<RPacketContext::TContextConfigR99_R4>* contextConfigR99Pckg = (TPckg<RPacketContext::TContextConfigR99_R4>*)aPckg;
				RPacketContext::TContextConfigR99_R4& defaultContextV1 = (*contextConfigR99Pckg)();
				
				if ((defaultContextV1.iAccessPointName != DPCKTTSY_ACCESS_POINT2)||
				(defaultContextV1.iPdpAddress != DPCKTTSY_PDP_ADDRESS2)||
				(defaultContextV1.iPdpType != DPCKTTSY_PDP_TYPE2) ||
				(defaultContextV1.iUseEdge != DPCKTTSY_EGPRS_NOTREQUIRED) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername != DPCKTTSY_USER_NAME1) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword != DPCKTTSY_PASSWORD1) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol != DPCKTTSY_PROTOCOL_TYPE1_0) ||
				(defaultContextV1.iNWIContext != DPCKTTSY_NWICONTEXT_TRUE) ||
				(defaultContextV1.iPFI != DPCKTTSY_PFICONTEXT_SIG))
					ReqCompleted(aTsyReqHandle,KErrCorrupt);
				else
					ReqCompleted(aTsyReqHandle,KErrNone);
				
				iTsyAsyncReqHandle = aTsyReqHandle;
				break;
				}
			else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
				{
				TPckg<RPacketContext::TContextConfig_R5>* contextConfigR5Pckg = (TPckg<RPacketContext::TContextConfig_R5>*)aPckg;
				RPacketContext::TContextConfig_R5& defaultContextV1 = (*contextConfigR5Pckg)();
				
				if ((defaultContextV1.iAccessPointName != DPCKTTSY_ACCESS_POINT2)||
				(defaultContextV1.iPdpAddress != DPCKTTSY_PDP_ADDRESS2)||
				(defaultContextV1.iPdpType != DPCKTTSY_PDP_TYPE2) ||
				(defaultContextV1.iUseEdge != DPCKTTSY_EGPRS_NOTREQUIRED) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername != DPCKTTSY_USER_NAME1) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword != DPCKTTSY_PASSWORD1) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol != DPCKTTSY_PROTOCOL_TYPE1_0) ||
				(defaultContextV1.iNWIContext != DPCKTTSY_NWICONTEXT_TRUE) ||
				(defaultContextV1.iPFI != DPCKTTSY_PFICONTEXT_SIG) ||
				(defaultContextV1.iPdpDataCompression != DPCKTTSY_DATACOMPRESSION_EV44) ||
				(defaultContextV1.iPdpHeaderCompression != DPCKTTSY_HEADERCOMPRESSION_ERFC2507))
					ReqCompleted(aTsyReqHandle,KErrCorrupt);
				else
					ReqCompleted(aTsyReqHandle,KErrNone);
				
				iTsyAsyncReqHandle = aTsyReqHandle;
				break;
				}	
		case 3:
			if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
				{
				TPckg<RPacketContext::TContextConfigGPRS>* defaultContextGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aPckg;
				RPacketContext::TContextConfigGPRS& defaultContextV1 = (*defaultContextGPRSPckg)();

				if ((defaultContextV1.iAccessPointName != DPCKTTSY_ACCESS_POINT2)||
				(defaultContextV1.iAnonymousAccessReqd != DPCKTTSY_ANONYMOUS_ACCESS2)||
				(defaultContextV1.iPdpAddress != DPCKTTSY_PDP_ADDRESS2)||
				(defaultContextV1.iPdpCompression != DPCKTTSY_COMPRESSION2)||
				(defaultContextV1.iPdpType != DPCKTTSY_PDP_TYPE2) ||
				(defaultContextV1.iUseEdge != DPCKTTSY_EGPRS_NOTREQUIRED) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername != DPCKTTSY_USER_NAME2) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword != DPCKTTSY_PASSWORD2) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol != DPCKTTSY_PROTOCOL_TYPE2_0) ||
				(defaultContextV1.iNWIContext != DPCKTTSY_NWICONTEXT_TRUE))
					ReqCompleted(aTsyReqHandle,KErrCorrupt);

				iTsyAsyncReqHandle = aTsyReqHandle;
				break;
				}
			else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
				{
				TPckg<RPacketContext::TContextConfigR99_R4>* contextConfigR99Pckg = (TPckg<RPacketContext::TContextConfigR99_R4>*)aPckg;
				RPacketContext::TContextConfigR99_R4& defaultContextV1 = (*contextConfigR99Pckg)();
			
				if ((defaultContextV1.iAccessPointName != DPCKTTSY_ACCESS_POINT2)||
				(defaultContextV1.iPdpAddress != DPCKTTSY_PDP_ADDRESS2)||
				(defaultContextV1.iPdpType != DPCKTTSY_PDP_TYPE2) ||
				(defaultContextV1.iUseEdge != DPCKTTSY_EGPRS_NOTREQUIRED) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername != DPCKTTSY_USER_NAME1) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword != DPCKTTSY_PASSWORD1) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol != DPCKTTSY_PROTOCOL_TYPE1_0) ||
				(defaultContextV1.iNWIContext != DPCKTTSY_NWICONTEXT_TRUE) ||
				(defaultContextV1.iPFI != DPCKTTSY_PFICONTEXT_SIG))
					ReqCompleted(aTsyReqHandle,KErrCorrupt);

				iTsyAsyncReqHandle = aTsyReqHandle;
				break;
				}
			else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
				{
				TPckg<RPacketContext::TContextConfig_R5>* contextConfigR5Pckg = (TPckg<RPacketContext::TContextConfig_R5>*)aPckg;
				RPacketContext::TContextConfig_R5& defaultContextV1 = (*contextConfigR5Pckg)();
			
				if ((defaultContextV1.iAccessPointName != DPCKTTSY_ACCESS_POINT2)||
				(defaultContextV1.iPdpAddress != DPCKTTSY_PDP_ADDRESS2)||
				(defaultContextV1.iPdpType != DPCKTTSY_PDP_TYPE2) ||
				(defaultContextV1.iUseEdge != DPCKTTSY_EGPRS_NOTREQUIRED) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername != DPCKTTSY_USER_NAME1) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword != DPCKTTSY_PASSWORD1) ||
				(defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol != DPCKTTSY_PROTOCOL_TYPE1_0) ||
				(defaultContextV1.iNWIContext != DPCKTTSY_NWICONTEXT_TRUE) ||
				(defaultContextV1.iPFI != DPCKTTSY_PFICONTEXT_SIG) ||
				(defaultContextV1.iPdpDataCompression != DPCKTTSY_DATACOMPRESSION_EV44) ||
				(defaultContextV1.iPdpHeaderCompression != DPCKTTSY_HEADERCOMPRESSION_ERFC2507))
					ReqCompleted(aTsyReqHandle,KErrCorrupt);

				iTsyAsyncReqHandle = aTsyReqHandle;
				break;
				}
		default:
			LOGTEXT(_L8("CGprsDGprsTsy::SetDefaultContextParams - Error in switch statement"));
			break;
		} // switch
		
	return KErrNone;
	}

TInt CGprsDGprsTsy::SetDefaultContextParamsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::SetDefaultContextParamsCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetDefaultContextParams(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg)
/** 
 * GetDefaultContextParams method - Gets the default context parameters.
 * A synchronous and asynchronous variant of this method is now supported - Change 
 * Request: NDOE-58VF4Q. In the test code, the synchronous method is called first.
 * Note that the synchronous variant is deprecated.
 * In the switch statement, cases 0 and 1 implement the synchronous call, while cases 2 and 
 * 3 implement the asynchronous call.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetDefaultContextParams called"));

	User::After(300000); // wait to have KRequestPending
	TPckg<TPacketDataConfigBase>* configBase = (TPckg<TPacketDataConfigBase>*)aPckg;
	TPacketDataConfigBase& configBaseV1 = (*configBase)();

	switch (iGetDefaultContextParams++)
		{
		case 0:
		case 1:
			if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
				{
				TPckg<RPacketContext::TContextConfigGPRS>* defaultContextGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aPckg;
				RPacketContext::TContextConfigGPRS& defaultContextV1 = (*defaultContextGPRSPckg)();

				defaultContextV1.iAccessPointName = DPCKTTSY_ACCESS_POINT2;
				defaultContextV1.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS2;
				defaultContextV1.iPdpAddress = DPCKTTSY_PDP_ADDRESS2;
				defaultContextV1.iPdpCompression = DPCKTTSY_COMPRESSION2;
				defaultContextV1.iPdpType = DPCKTTSY_PDP_TYPE2;
				defaultContextV1.iUseEdge = DPCKTTSY_EGPRS_NOTREQUIRED;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;				
				ReqCompleted(aTsyReqHandle,KErrNone);
				break;
				}
			else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
				{
				TPckg<RPacketContext::TContextConfigR99_R4>* contextConfigR99Pckg = (TPckg<RPacketContext::TContextConfigR99_R4>*)aPckg;
				RPacketContext::TContextConfigR99_R4& defaultContextV1 = (*contextConfigR99Pckg)();
		
				defaultContextV1.iAccessPointName = DPCKTTSY_ACCESS_POINT2;
				defaultContextV1.iPdpAddress = DPCKTTSY_PDP_ADDRESS2;
				defaultContextV1.iPdpType = DPCKTTSY_PDP_TYPE2;
				defaultContextV1.iUseEdge = DPCKTTSY_EGPRS_NOTREQUIRED;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
				defaultContextV1.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
				defaultContextV1.iPFI = DPCKTTSY_PFICONTEXT_SMS;
				ReqCompleted(aTsyReqHandle,KErrNone);
				break;
				}
			else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
				{
				TPckg<RPacketContext::TContextConfig_R5>* contextConfigR5Pckg = (TPckg<RPacketContext::TContextConfig_R5>*)aPckg;
				RPacketContext::TContextConfig_R5& defaultContextV1 = (*contextConfigR5Pckg)();
		
				defaultContextV1.iAccessPointName = DPCKTTSY_ACCESS_POINT2;
				defaultContextV1.iPdpAddress = DPCKTTSY_PDP_ADDRESS2;
				defaultContextV1.iPdpType = DPCKTTSY_PDP_TYPE2;
				defaultContextV1.iUseEdge = DPCKTTSY_EGPRS_NOTREQUIRED;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
				defaultContextV1.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
				defaultContextV1.iPFI = DPCKTTSY_PFICONTEXT_SMS;
				defaultContextV1.iPdpDataCompression = DPCKTTSY_DATACOMPRESSION_EV44;
				defaultContextV1.iPdpHeaderCompression = DPCKTTSY_HEADERCOMPRESSION_ERFC2507;
				ReqCompleted(aTsyReqHandle,KErrNone);
				break;
				}
		case 2:
		case 4:
			if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
				{
				TPckg<RPacketContext::TContextConfigGPRS>* defaultContextGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aPckg;
				RPacketContext::TContextConfigGPRS& defaultContextV1 = (*defaultContextGPRSPckg)();

				defaultContextV1.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
				defaultContextV1.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
				defaultContextV1.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
				defaultContextV1.iPdpCompression = DPCKTTSY_COMPRESSION1;
				defaultContextV1.iPdpType = DPCKTTSY_PDP_TYPE1;
				defaultContextV1.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
				defaultContextV1.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
				ReqCompleted(aTsyReqHandle,KErrNone);
				}
			else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
				{
				TPckg<RPacketContext::TContextConfigR99_R4>* contextConfigR99Pckg = (TPckg<RPacketContext::TContextConfigR99_R4>*)aPckg;
				RPacketContext::TContextConfigR99_R4& defaultContextV1 = (*contextConfigR99Pckg)();
				
				defaultContextV1.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
				defaultContextV1.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
				defaultContextV1.iPdpType = DPCKTTSY_PDP_TYPE1;
				defaultContextV1.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
				ReqCompleted(aTsyReqHandle,KErrNone);
				}
			else if (configBaseV1.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
				{
				TPckg<RPacketContext::TContextConfig_R5>* contextConfigR5Pckg = (TPckg<RPacketContext::TContextConfig_R5>*)aPckg;
				RPacketContext::TContextConfig_R5& defaultContextV1 = (*contextConfigR5Pckg)();
				
				defaultContextV1.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
				defaultContextV1.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
				defaultContextV1.iPdpType = DPCKTTSY_PDP_TYPE1;
				defaultContextV1.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
				defaultContextV1.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
				defaultContextV1.iPdpDataCompression = DPCKTTSY_DATACOMPRESSION_EV42;
				defaultContextV1.iPdpHeaderCompression = DPCKTTSY_HEADERCOMPRESSION_ERFC1144;
				ReqCompleted(aTsyReqHandle,KErrNone);
				}
		case 3:
			iTsyAsyncReqHandle = aTsyReqHandle;
			break;
		default:
			LOGTEXT(_L8("CGprsDGprsTsy::GetDefaultContextParams - Error in switch statement"));
			break;
		} // switch
		
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetDefaultContextParamsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetDefaultContextParamsCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetPreferredBearer(const TTsyReqHandle aTsyReqHandle, RPacketService::TPreferredBearer* aBearer)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetPreferredBearer called"));
	User::After(300000); // wait to have KRequestPending
	switch(iGetPrefBearer++)
		{
		case 0:
			*aBearer = RPacketService::EBearerPacketSwitched;
			ReqCompleted(aTsyReqHandle, KErrNone);
			break;
		case 1:
			*aBearer = RPacketService::EBearerCircuitSwitched;
			ReqCompleted(aTsyReqHandle, KErrNone);
			break;
		default:
			break;
		}
	iTsyAsyncReqHandle=aTsyReqHandle;
	return KErrNone;
	}
	
TInt CGprsDGprsTsy::GetPreferredBearerCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetPreferredBearerCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::SetPreferredBearer(const TTsyReqHandle aTsyReqHandle, RPacketService::TPreferredBearer* aBearer)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::SetPreferredBearer called"));
	User::After(300000); // wait to have KRequestPending
	if(!iSetPrefBearer++)
		{
		if (*aBearer == RPacketService::EBearerPacketSwitched)
			ReqCompleted(aTsyReqHandle, KErrNone);
		else
			ReqCompleted(aTsyReqHandle, KErrCorrupt);
		}
	iTsyAsyncReqHandle=aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::SetPreferredBearerCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::SetPreferredBearerCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetCurrentReleaseMode(const TTsyReqHandle aTsyReqHandle, RPacketService::TPacketReleaseMode* aMode)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetCurrentReleaseMode called"));
	User::After(300000); // wait to have KRequestPending
	if(!iGetCurrentReleaseMode++)
		{
		*aMode = DPCKTTSY_NETWORK_MODE1;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetCurrentReleaseModeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetCurrentReleaseModeCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyReleaseModeChange(const TTsyReqHandle aTsyReqHandle, RPacketService::TPacketReleaseMode* aMode)
	{
	User::After(300000); // wait to have KRequestPending
	if (!iNotifyReleaseModeChange++)
		{
		LOGTEXT(_L8("CGprsDGprsTsy::NotifyReleaseModeChange called"));
		*aMode = DPCKTTSY_NETWORK_MODE2;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyReleaseModeChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyReleaseModeChangeCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::EnumerateNifs(const TTsyReqHandle aTsyReqHandle, TInt* aCount)
/**
 * EnumerateNifs method - Set the number of NIFs.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::EnumerateNifs called"));

	User::After(300000); // wait to have KRequestPending
	if (!iEnumerateNifs++)
		{
		*aCount = DPCKTTSY_NUMBER_OF_NIFS;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	
	iTsyAsyncReqHandle = aTsyReqHandle;

	return KErrNone;
	}

TInt CGprsDGprsTsy::EnumerateNifsCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * EnumerateNifsCancel - Cancels an asynchronous EnumerateNifs request
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::EnumerateNifsCancel called"));

	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}


TInt CGprsDGprsTsy::GetNifInfo(const TTsyReqHandle aTsyReqHandle,TInt* aIndex, TDes8* aNifInfoV2)
/**
 * GetNifInfo method - sets Nif information depending on the index argument passed in.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetNifInfo called"));

	User::After(300000); // wait to have KRequestPending
	RPacketService::TNifInfoV2Pckg* nifInfoV2Pckg = (RPacketService::TNifInfoV2Pckg*)aNifInfoV2;
	RPacketService::TNifInfoV2& nifInfoV2 = (*nifInfoV2Pckg)();

	if(!iGetNifInfo++)
		{
		switch (*aIndex)
			{
			case 1:
				nifInfoV2.iContextName=DPCKTTSY_NIF_CONTEXT1;
				nifInfoV2.iNumberOfContexts=DPCKTTSY_NUMBER_OF_CONTEXTS_IN_NIF1;
				nifInfoV2.iNifStatus=DPCKTTSY_CONTEXT_INFO_STATUS0;
				nifInfoV2.iPdpAddress=DPCKTTSY_PDP_ADDRESS1;
				nifInfoV2.iContextType=DPCKTTSY_INTERNAL_CONTEXT;
				ReqCompleted(aTsyReqHandle,KErrNone);
				break;
			case 2:
				nifInfoV2.iContextName=DPCKTTSY_NIF_CONTEXT2;
				nifInfoV2.iNumberOfContexts=DPCKTTSY_NUMBER_OF_CONTEXTS_IN_NIF2;
				nifInfoV2.iNifStatus=DPCKTTSY_CONTEXT_INFO_STATUS1;
				nifInfoV2.iPdpAddress=DPCKTTSY_PDP_ADDRESS2;
				nifInfoV2.iContextType=DPCKTTSY_EXTERNAL_CONTEXT;
				ReqCompleted(aTsyReqHandle,KErrNone);
				break;
			default:
				ReqCompleted(aTsyReqHandle,KErrArgument);
				break;
			} // switch
		} // if

	iTsyAsyncReqHandle = aTsyReqHandle;

	return KErrNone;
	}

TInt CGprsDGprsTsy::GetNifInfoCancel(const TTsyReqHandle aTsyReqHandle) 
/** 
 * GetNifInfoCancel - Cancels an asynchronous GetNifInfo request
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetNifInfoCancel called"));

	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::EnumerateContextsInNif(const TTsyReqHandle aTsyReqHandle, const TDesC* aExistingContextName, TInt* aCount)
/**
 * EnumerateContextsInNif - sets the number of NIFs in a context.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::EnumerateContextsInNif called"));

	User::After(300000); // wait to have KRequestPending
	if (!iEnumerateContextsInNif++)
		{
		TInt ret = (*aExistingContextName).Compare(DPCKTTSY_NIF_CONTEXT2);
		if (ret == KErrNone)
			{
			*aCount = DPCKTTSY_NUMBER_OF_CONTEXTS_IN_NIF2;
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}

	iTsyAsyncReqHandle = aTsyReqHandle;

	return KErrNone;
	}

TInt CGprsDGprsTsy::EnumerateContextsInNifCancel(const TTsyReqHandle aTsyReqHandle) 
/**
 * EnumerateContextsInNifCancel - Cancels an asynchronous EnumerateContextsInNif request.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::EnumerateContextsInNifCancel called"));

	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetContextNameInNif(const TTsyReqHandle aTsyReqHandle, RPacketService::TContextNameInNif* aContextNameInNif, TDes* aContextName)
/**
 * GetContextNameInNif - gets a context name within a Nif.
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetContextNameInNif called"));

	User::After(300000); // wait to have KRequestPending
	if (!iGetContextNameInNif++)
		{
		if (!(aContextNameInNif->iExistingContextName).Compare(DPCKTTSY_NIF_CONTEXT1))
			{
			switch (aContextNameInNif->iIndex)
				{
				case 0:
					{
					*aContextName=DPCKTTSY_CONTEXT1_IN_NIF1;
					ReqCompleted(aTsyReqHandle,KErrNone);
					}
					break;
				default:
					ReqCompleted(aTsyReqHandle,KErrArgument);
					break;
				} // switch
			} // if, 'inner block'
		else if (!(aContextNameInNif->iExistingContextName).Compare(DPCKTTSY_NIF_CONTEXT2))
			{
			switch (aContextNameInNif->iIndex)
				{
				case 0:
					{
					*aContextName=DPCKTTSY_CONTEXT1_IN_NIF2;
					ReqCompleted(aTsyReqHandle,KErrNone);
					}
					break;
				case 1:
					{
					*aContextName=DPCKTTSY_CONTEXT2_IN_NIF2;
					ReqCompleted(aTsyReqHandle,KErrNone);
					}
					break;
				default:
					ReqCompleted(aTsyReqHandle,KErrArgument);
					break;
				} // switch
			} // else if
		else
			ReqCompleted(aTsyReqHandle, KErrCorrupt);
		} // if, 'outer block'

	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetContextNameInNifCancel(const TTsyReqHandle aTsyReqHandle) 
/**
 * GetContextNameInNifCancel - Cancels an asynchronous GetContextNameInNif request
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetContextNameInNifCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsDGprsTsy::PrepareOpenSecondary(const TTsyReqHandle aTsyReqHandle, const TDesC* /*aOriginalContextName*/)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::PrepareOpenSecondary called"));

	User::After(300000); // wait to have KRequestPending
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}
	
TInt CGprsDGprsTsy::DeactivateNIF(const TTsyReqHandle aTsyReqHandle, const TDesC* dataPtr)
/**
 * DeactivateNIF - deactivates a NIF
 */	
	{
	LOGTEXT(_L8("CGprsDGprsTsy::DeactivateNIF called"));
	User::After(300000); // wait to have KRequestPending
	if (!iDeactivateNIF++)
		{
		if(!dataPtr->Compare(DPCKTTSY_NIF_CONTEXT1))
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			ReqCompleted(aTsyReqHandle,KErrNone);
		}	
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	
TInt CGprsDGprsTsy::DeactivateNIFCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * DeactivateNIFCancel - cancels an asynchronous DeactivateNIF request
 */	
	{
	LOGTEXT(_L8("CGprsDGprsTsy::DeactivateNIFCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}
	
RHandleBase* CGprsDGprsTsy::GlobalKernelObjectHandle()
/**
* Implements CSubSessionExtBase virtual method, which provides
* handle of the mutex object to etel server that passes it up to clients.
* @return pointer to synchronisation mutex object
*/
	{
		return &iMutex;
	}
//
//	Multimedia Broadcast Multicast Service (MBMS)
//
TInt CGprsDGprsTsy::NotifyMbmsNetworkServiceStatusChange(const TTsyReqHandle aTsyReqHandle, TMbmsNetworkServiceStatus* aStatus)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyMbmsNetworkServiceStatusChange called"));
	iTsyAsyncReqHandle = aTsyReqHandle;
	if (!iNotifyMbmsNetworkServiceStatusChange++)
		{
		*aStatus = DPCKTTSY_MBMS_NETWORKSERVICE_SUPPORTED;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CGprsDGprsTsy::NotifyMbmsNetworkServiceStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyMbmsNetworkServiceStatusChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetMbmsNetworkServiceStatus(const TTsyReqHandle aTsyReqHandle, TBool* aAttemptAttach,TMbmsNetworkServiceStatus* aStatus)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetMbmsNetworkServiceStatus called"));
	iTsyAsyncReqHandle = aTsyReqHandle;
	switch(iGetMbmsNetworkServiceStatus)
		{
	case 0:
		if(!(*aAttemptAttach == DPCKTTSY_MBMS_ATTEMPT_GSMATTACH_TRUE))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			*aStatus = DPCKTTSY_MBMS_NETWORKSERVICE_SUPPORTED;
			AddDelayedReq(aTsyReqHandle, this);
			}
		iGetMbmsNetworkServiceStatus++;
		break;
	case 1:
		if(!(*aAttemptAttach == DPCKTTSY_MBMS_ATTEMPT_GSMATTACH_FALSE))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			*aStatus = DPCKTTSY_MBMS_NETWORKSERVICE_UNKNOWN;
			AddDelayedReq(aTsyReqHandle, this);
			}
		iGetMbmsNetworkServiceStatus = 0; //Reset to 0
		break;
	default:
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetMbmsNetworkServiceStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetMbmsNetworkServiceStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		{	
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}
		
TInt CGprsDGprsTsy::NotifyMbmsServiceAvailabilityChange(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyMbmsServiceAvailabilityChange called"));
	iTsyAsyncReqHandle = aTsyReqHandle;
	if(!iServiceAvailability++)
		{
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}
	
TInt CGprsDGprsTsy::NotifyMbmsServiceAvailabilityChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::NotifyMbmsServiceAvailabilityChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt) ;
		}
	return KErrNone;
	}
	
TInt CGprsDGprsTsy::UpdateMbmsMonitorServiceListL(const TTsyReqHandle aTsyReqHandle,TMbmsAction* aAction, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::UpdateMbmsMonitorServiceListL called"));
	iTsyAsyncReqHandle = aTsyReqHandle;
	TInt error = KErrNone;
	switch(*aAction)
		{
	case DPCKTTSY_MBMS_ACTION_ADD:
		iList->RestoreL(*aBuffer);
		iServiceCount=iList->Enumerate();
		if ( iServiceCount != DPCKTTSY_MBMS_SERVICELIST_COUNT_ADD )
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			return KErrNone;
			}
		error=AddMbmsServicesToMonitorList(aTsyReqHandle);
		if(error == KErrNone)
			{
			AddDelayedReq(aTsyReqHandle,this);
			}
		break;
	case DPCKTTSY_MBMS_ACTION_REMOVE:
		//If the client requests to remove invalid service entries, one or more, TSY 
		//shall delete valid entries and if invalid entries are found then the request 
		// shall complete with KErrMbmsImpreciseServiceEntries.
		iList->RestoreL(*aBuffer);
		iServiceCount=iList->Enumerate();	
		error=RemoveMbmsServicesFromMonitorList(aTsyReqHandle);
		if(error == KErrNone)
			{
			AddDelayedReq(aTsyReqHandle,this);
			}
		break;
	case DPCKTTSY_MBMS_ACTION_REMOVEALL:
		//No need to check the list object. TSY should just delete all the objects present
		// in the monitor list irrespective of the contents present in aBuffer parameter.
		error = KErrNone;
		AddDelayedReq(aTsyReqHandle,this);
		break;
	default:
		error = KErrCorrupt;
		}
	if(error == KErrCorrupt)
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if (error == KErrNotFound)
		{
		ReqCompleted(aTsyReqHandle,KErrNotFound);
		}
	else if (error == KErrMbmsImpreciseServiceEntries)
		{
		ReqCompleted(aTsyReqHandle,KErrMbmsImpreciseServiceEntries);
		}
	return KErrNone;
	};
	
TInt CGprsDGprsTsy::UpdateMbmsMonitorServiceListCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::UpdateMbmsMonitorServiceListCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}
	
TInt CGprsDGprsTsy::GetMbmsMonitoredServicesPhase1(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TUint* aBufSize)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetMbmsMonitoredServicesPhase1 called"));
	iTsyAsyncReqHandle = aTsyReqHandle;
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetMbmsMonitoredServicesPhase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		{
		ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	}	
	
TInt CGprsDGprsTsy::ProcessGetMbmsMonitoredServicesPhase1L(const TTsyReqHandle aTsyReqHandle, 
													 TClientId* aClient, 
													 TUint* aBufSize)
	{
	// retrieve MBMS service list,
	// store each entry.
	// stream the list and then return size of this buffer to client
	CPcktMbmsMonitoredServiceList* list=CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(list);
	RPacketService::TMbmsServiceAvailabilityV1 entry;

	// fill up an example list
	for (TInt index=0; index < DPCKTTSY_MBMS_SERVICELIST_COUNT_ADD; index++)
		{
		switch (index)
			{
		case 0:
			entry.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_ONE;
			entry.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_ONE;
			entry.iMbmsAvailabilityStatus = DPCKTTSY_MBMS_AVAILABILITYSTATUS_ONE;
			entry.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_ONE);
			entry.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_ONE);
			entry.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_ONE);
			break;
			
		case 1:
			entry.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_TWO;
			entry.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_TWO;
			entry.iMbmsAvailabilityStatus = DPCKTTSY_MBMS_AVAILABILITYSTATUS_TWO;
			entry.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_TWO);
			entry.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_TWO);
			entry.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_TWO);
			break;
		
		case 2:
		default:
			entry.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_THREE;
			entry.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_THREE;
			entry.iMbmsAvailabilityStatus = DPCKTTSY_MBMS_AVAILABILITYSTATUS_THREE;
			entry.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_THREE);
			entry.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_THREE);
			entry.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_THREE);
			break;
			}
		// Add the entry into the list, at the next empty location
		list->AddEntryL(entry);
		}
	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC
	
	iGetMbmsMonitoredServices->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
   	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CGprsDGprsTsy::GetMbmsMonitoredServicesPhase2(const TTsyReqHandle aTsyReqHandle,TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetMbmsMonitoredServicesPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the get Mbms monitored services from this client
	for (TInt i=0; i<iGetMbmsMonitoredServices->Count(); ++i)
		{
		read = iGetMbmsMonitoredServices->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetMbmsMonitoredServices->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}
	
TInt CGprsDGprsTsy::GetMbmsMonitoredServicesCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetPreferredWlanSIDsCancel called"));
	// Remove the read all attempt from iGetMbmsMonitoredServices
	RemoveDelayedReq(aTsyReqHandle);
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetMbmsMonitoredServices->Count(); ++i)
		{
		read = iGetMbmsMonitoredServices->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetMbmsMonitoredServices->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CGprsDGprsTsy::AddMbmsServicesToMonitorList(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::AddMbmsServicesToMonitorList is called"));
	TInt error = KErrNone;
	iTsyAsyncReqHandle = aTsyReqHandle;
	RPacketService::TMbmsServiceAvailabilityV1 entry;
	for (TInt i=0; i<iServiceCount; ++i)
		{
		TRAPD(ret,entry=iList->GetEntryL(i));
		if (ret != KErrNone)
			break;
		switch(i)
			{
		case 0:
			if(entry.iMbmsServiceMode != DPCKTTSY_MBMS_SERVICEMODE_ONE ||
			   entry.iMbmsAccessBearer != DPCKTTSY_MBMS_ACCESSBEARER_ONE ||
			   entry.iTmgi.GetMCC() != DPCKTTSY_MBMS_MCC_ONE ||
			   entry.iTmgi.GetMNC() != DPCKTTSY_MBMS_MNC_ONE ||
			   entry.iTmgi.GetServiceId() != DPCKTTSY_MBMS_SERVICEID_ONE)		
				{
				error  = KErrCorrupt;
				}
			break;
		case 1:
			if(entry.iMbmsServiceMode != DPCKTTSY_MBMS_SERVICEMODE_TWO ||
			   entry.iMbmsAccessBearer != DPCKTTSY_MBMS_ACCESSBEARER_TWO ||
			   entry.iTmgi.GetMCC() != DPCKTTSY_MBMS_MCC_TWO ||
			   entry.iTmgi.GetMNC() != DPCKTTSY_MBMS_MNC_TWO ||
			   entry.iTmgi.GetServiceId() != DPCKTTSY_MBMS_SERVICEID_TWO)	
				{
				error  = KErrCorrupt;
				}
			break;
		case 2:
		default:
			if(entry.iMbmsServiceMode != DPCKTTSY_MBMS_SERVICEMODE_THREE ||
			   entry.iMbmsAccessBearer != DPCKTTSY_MBMS_ACCESSBEARER_THREE ||
			   entry.iTmgi.GetMCC() != DPCKTTSY_MBMS_MCC_THREE ||
			   entry.iTmgi.GetMNC() != DPCKTTSY_MBMS_MNC_THREE ||
			   entry.iTmgi.GetServiceId() != DPCKTTSY_MBMS_SERVICEID_THREE)
				{
				error  = KErrCorrupt;
				}
			break;
			}//END OF SWITCH
		}//end of for loop
	return error;
	}
	
TInt CGprsDGprsTsy::RemoveMbmsServicesFromMonitorList(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::RemoveMbmsServicesFromMonitorList is called"));
	TInt error = KErrNone;
	RPacketService::TMbmsServiceAvailabilityV1 entry;
	
	switch(iRemoveMbmsServices)
		{
	case 0:	
		for(TInt i=0;i<iServiceCount;i++)
			{
			TRAPD(ret,entry=iList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			if(entry.iMbmsServiceMode != DPCKTTSY_MBMS_SERVICEMODE_TWO ||
		  		entry.iMbmsAccessBearer != DPCKTTSY_MBMS_ACCESSBEARER_TWO ||
		   		entry.iTmgi.GetMCC() != DPCKTTSY_MBMS_MCC_TWO ||
		   		entry.iTmgi.GetMNC() != DPCKTTSY_MBMS_MNC_TWO ||
		   		entry.iTmgi.GetServiceId() != DPCKTTSY_MBMS_SERVICEID_TWO)	
				{
				error=KErrCorrupt;
				}
			}
	 	iRemoveMbmsServices++;
		break;
		
	case 1:	
		for(TInt i=0;i<iServiceCount;i++)
			{
			TRAPD(ret,entry=iList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			if(entry.iMbmsServiceMode != DPCKTTSY_MBMS_SERVICEMODE_INVALID ||
			   entry.iMbmsAccessBearer != DPCKTTSY_MBMS_ACCESSBEARER_INVALID ||
			   entry.iTmgi.GetMCC() != DPCKTTSY_MBMS_MCC_INVALID ||
			   entry.iTmgi.GetMNC() != DPCKTTSY_MBMS_MNC_INVALID ||
			   entry.iTmgi.GetServiceId() != DPCKTTSY_MBMS_SERVICEID_INVALID)	
				{
				error=KErrCorrupt;
				}
			}
		if(error != KErrCorrupt)
			{
			error=KErrNotFound;
			}
		iRemoveMbmsServices++;
		break;
		
	case 2:	
		for(TInt i=0;i<iServiceCount;i++)
			{
			TRAPD(ret,entry=iList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					if(entry.iMbmsServiceMode != DPCKTTSY_MBMS_SERVICEMODE_ONE ||
			   		   entry.iMbmsAccessBearer != DPCKTTSY_MBMS_ACCESSBEARER_ONE  ||
			  		   entry.iTmgi.GetMCC() != DPCKTTSY_MBMS_MCC_ONE  ||
			   		   entry.iTmgi.GetMNC() != DPCKTTSY_MBMS_MNC_ONE  ||
			   		   entry.iTmgi.GetServiceId() != DPCKTTSY_MBMS_SERVICEID_ONE )	
						{
						error=KErrCorrupt;
						}
					break;	
				
				case 1:
					if(entry.iMbmsServiceMode != DPCKTTSY_MBMS_SERVICEMODE_INVALID ||
			   		   entry.iMbmsAccessBearer != DPCKTTSY_MBMS_ACCESSBEARER_INVALID ||
			  		   entry.iTmgi.GetMCC() != DPCKTTSY_MBMS_MCC_INVALID ||
			   		   entry.iTmgi.GetMNC() != DPCKTTSY_MBMS_MNC_INVALID ||
			   		   entry.iTmgi.GetServiceId() != DPCKTTSY_MBMS_SERVICEID_INVALID)	
						{
						error=KErrCorrupt;
						}
					break;
						
				case 2:
				default:
					if(entry.iMbmsServiceMode != DPCKTTSY_MBMS_SERVICEMODE_THREE ||
			   		   entry.iMbmsAccessBearer != DPCKTTSY_MBMS_ACCESSBEARER_THREE ||
			  		   entry.iTmgi.GetMCC() != DPCKTTSY_MBMS_MCC_THREE ||
			   		   entry.iTmgi.GetMNC() != DPCKTTSY_MBMS_MNC_THREE ||
			   		   entry.iTmgi.GetServiceId() != DPCKTTSY_MBMS_SERVICEID_THREE)	
						{
						error=KErrCorrupt;
						}
					break;				
				}
			}
		if(error != KErrCorrupt)
			{
			error=KErrMbmsImpreciseServiceEntries;
			}
		iRemoveMbmsServices=0;
		break;	
					
	default:
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		break;	
		}
	return error;
	}

TInt CGprsDGprsTsy::EnumerateMbmsMonitorServiceList(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::EnumerateMbmsMonitorServiceList called"));
	iTsyAsyncReqHandle = aTsyReqHandle;
	*aCount = DPCKTTSY_MBMS_MONITORSERVICE_NUMBER_OF_CONTEXTS;
	*aMaxAllowed = DPCKTTSY_MBMS_MONITORSERVICE_MAX_NUMBER_OF_CONTEXTS;
	AddDelayedReq(aTsyReqHandle,this, DPCKTTSY_MBMS_DELAY);
	return KErrNone;
	}

TInt CGprsDGprsTsy::EnumerateMbmsMonitorServiceListCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::EnumerateMbmsMonitorServiceListCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}
	
TInt CGprsDGprsTsy::EnumerateMbmsActiveServiceList(const TTsyReqHandle aTsyReqHandle, TInt* aCount, TInt* aMaxAllowed)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::EnumerateMbmsActiveServiceList called"));
	iTsyAsyncReqHandle = aTsyReqHandle;
	*aCount = DPCKTTSY_MBMS_ACTIVESERVICE_NUMBER_OF_CONTEXTS;
	*aMaxAllowed = DPCKTTSY_MBMS_ACTIVESERVICE_MAX_NUMBER_OF_CONTEXTS;
	AddDelayedReq(aTsyReqHandle,this, DPCKTTSY_MBMS_DELAY);
	return KErrNone;
	}

TInt CGprsDGprsTsy::EnumerateMbmsActiveServiceListCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::EnumerateMbmsActiveServiceListCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}	

//
CListReadAllAttempt* CListReadAllAttempt::NewL(TClientId* aId, TTsyReqHandle aReqHandle)
	{
	CListReadAllAttempt* read=new(ELeave) CListReadAllAttempt(aId, aReqHandle);
	CleanupStack::PushL(read);
	read->ConstructL();
	CleanupStack::Pop();
	return read;
	}

CListReadAllAttempt::CListReadAllAttempt(TClientId* aId, TTsyReqHandle aReqHandle)
	:CBase()
	{
	iClient.iSessionHandle=aId->iSessionHandle;
	iClient.iSubSessionHandle=aId->iSubSessionHandle;
	iReqHandle=aReqHandle;
	iListBuf = NULL;
	}

void CListReadAllAttempt::ConstructL()
	{
	}

CListReadAllAttempt::~CListReadAllAttempt()
	{
	delete iListBuf;
	}

//
// methods for CActiveListNode
//

CGprsDGprsTsy::CActiveListNode::CActiveListNode(CActive *aActive, const TTsyReqHandle aTsyReqHandle) :
	iActive(aActive), iTsyReqHandle(aTsyReqHandle)
	{
	}

CGprsDGprsTsy::CActiveListNode::~CActiveListNode()
	{
	delete iActive;
	}

//
// methods for the list of aos which corresspond to the outstanding delayed asyncrhonous reqs
//

TInt CGprsDGprsTsy::FindDelayedReq(const TTsyReqHandle aTsyReqHandle, CActiveListNode *& aNode)
	{
	TInt err = KErrNotFound;
	TSglQueIter<CActiveListNode> iter(iActiveRequestObjects);
	iter.SetToFirst();

	CActiveListNode *node = aNode = NULL;
	while ((node = iter++) != NULL)
		{
		if (node->iTsyReqHandle == aTsyReqHandle)
			{
			break;
			}
		}
	if (node != NULL)
		{
		aNode = node;
		err = KErrNone;
		}
	return err;
	}
	
TInt CGprsDGprsTsy::RemoveDelayedReq(const TTsyReqHandle aTsyReqHandle)
	{
	CActiveListNode *aNode = NULL;
	TInt err = FindDelayedReq(aTsyReqHandle,aNode);
	if (err == KErrNone)
		{
		iActiveRequestObjects.Remove(*aNode);
		delete aNode;
		}
	return err;
	}

void CGprsDGprsTsy::RegisterDelayedReqL(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject)
	{
	CDelayedCompleter *delayed = CDelayedCompleter::NewL(aTsyReqHandle,this,aTelObject);
	CleanupStack::PushL(delayed);
	CActiveListNode *newNode = new (ELeave) CActiveListNode(delayed,aTsyReqHandle);
	iActiveRequestObjects.AddLast(*newNode);
	delayed->After(DPCKTTSY_STANDARD_DELAY);
	CleanupStack::Pop(); // delayed
	}
	
void CGprsDGprsTsy::RegisterDelayedReqL(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject,TInt aPriority)
	{
	CDelayedCompleter *delayed = CDelayedCompleter::NewL(aTsyReqHandle,this,aTelObject);
	CleanupStack::PushL(delayed);
	CActiveListNode *newNode = new (ELeave) CActiveListNode(delayed,aTsyReqHandle);
	iActiveRequestObjects.AddLast(*newNode);
	delayed->After(aPriority);
	CleanupStack::Pop(); // delayed
	}
	
void CGprsDGprsTsy::AddDelayedReq(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject)
	{
	TRAPD(err, RegisterDelayedReqL(aTsyReqHandle, aTelObject));
	if (err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		}
	}
	
void CGprsDGprsTsy::AddDelayedReq(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject,TInt aPriority)
	{
	TRAPD(err, RegisterDelayedReqL(aTsyReqHandle, aTelObject, aPriority));
	if (err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		}
	}
	
//
//
// CDelayedCompleter
//
//

CDelayedCompleter::CDelayedCompleter(const TTsyReqHandle aTsyReqHandle, CGprsDGprsTsy *aGprsObject,
									CTelObject *aTelObject) :
		CTimer(EPriorityStandard),iTsyReqHandle(aTsyReqHandle), iGprsObject(aGprsObject),
			iTelObject(aTelObject)
	{
	}

CDelayedCompleter* CDelayedCompleter::NewL(const TTsyReqHandle aTsyReqHandle, CGprsDGprsTsy *aGprsObject,
									CTelObject *aTelObject)
	{
	CDelayedCompleter* self = new (ELeave) CDelayedCompleter(aTsyReqHandle,aGprsObject,aTelObject);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CDelayedCompleter::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CDelayedCompleter::RunL()
	{
	iTelObject->ReqCompleted(iTsyReqHandle,KErrNone);
	(void) iGprsObject->RemoveDelayedReq(iTsyReqHandle); 
	}
