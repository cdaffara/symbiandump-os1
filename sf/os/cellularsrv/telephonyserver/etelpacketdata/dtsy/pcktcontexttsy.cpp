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
// PACKETCONTEXTTSY.CPP
// 
//

#include "pckttsy.h"
#include "testdef.h"
#include "etelpckt.h"
#include "dpcktlog.h"
#include <pcktcs.h>

#include "coretsy.h"

/**************************************************************************/
//
//	CGprsContextDGprsTsy
//
/**************************************************************************/

CGprsContextDGprsTsy* CGprsContextDGprsTsy::NewL(CPhoneFactoryDummyBase* aFac)
/**
 * NewL method - Standard 2-phase constructor
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy: NewL() method"));

	CGprsContextDGprsTsy* subsession=new(ELeave) CGprsContextDGprsTsy(aFac);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CGprsContextDGprsTsy::Init()
	{}

CGprsContextDGprsTsy::CGprsContextDGprsTsy(CPhoneFactoryDummyBase* aFac)
	:CSubSessionExtDummyBase(aFac)
	{
	}

void CGprsContextDGprsTsy::ConstructL()
/**
 * ConstructL method
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy: Entered constructor"));
	}

CGprsContextDGprsTsy::~CGprsContextDGprsTsy()
/**
 * Destructor
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy: Entered destructor"));
	}

CTelObject* CGprsContextDGprsTsy::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * Opening object(s) from RPacketContext is not supported.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy: OpenNewObjectByNameL() method - Not supported"));

	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CGprsContextDGprsTsy::OpenNewObjectL(TDes& aNewName)
/**
 * A new QoS opened by a client from an existing context will have its name allocated by 
 * the TSY.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy: OpenNewObjectL() method"));
	aNewName.Append(_L("PACKET_QOS"));
	aNewName.AppendNum(++iGprsQoSObjectCount);

	return CGprsQoSDGprsTsy::NewL(FacPtr());
	}

CTelObject::TReqMode CGprsContextDGprsTsy::ReqModeL(const TInt aIpc)
/**
 * ReqModeL is called from the server's CTelObject::ReqAnalyserL in order to check the 
 * type of request it has. The following are example request types for this dummy TSY
 * All TSYs do not have to have these request types but they have been given
 * "sensible" values in this dummy TSY code.
 */	
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy: ReqModeL() method"));
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
		case EPacketContextSetConfig:
		case EPacketContextActivate:
		case EPacketContextDeactivate:
		case EPacketContextDelete:
		case EPacketContextLoanCommPort:
		case EPacketContextRecoverCommPort:
		case EPacketContextGetConfig:
		case EPacketContextGetStatus:
		case EPacketContextGetDataVolumeTransferred:
		case EPacketContextGetLastErrorCause:
		case EPacketContextGetConnectionSpeed:
		case EPacketContextGetProfileName:
		case EPacketContextInitialiseContext:
		case EPacketContextEnumeratePacketFilters:
		case EPacketContextGetPacketFilterInfo:
		case EPacketContextAddPacketFilter:
		case EPacketContextRemovePacketFilter:
		case EPacketContextModifyActiveContext:
		case EPacketContextGetDNSInfo:
		case EPacketContextCreateNewTFT:
		case EPacketContextDeleteTFT:
		case EPacketAddMediaAuthorization:
		case EPacketRemoveMediaAuthorization:
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextGetConnectionInfo:            		    
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
			break;

		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
		case EPacketContextNotifyConnectionSpeedChange:
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextNotifyConnectionInfoChange:           		    
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
			ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
			break;

		default:
			User::Leave(KErrNotSupported);
			break;
		}

	return ret;
	}

TInt CGprsContextDGprsTsy::RegisterNotification(const TInt aIpc)
/**
 * RegisterNotification is called when the server recognises that this notification
 * is being posted for the first time on this sub-session object.
 * It enables the TSY to "turn on" any regular notification messages that it may 
 * receive from the phone
 */
	{
	switch (aIpc)
		{
		case EPacketContextNotifyConfigChanged:
			LOGTEXT(_L8("CGprsContextDGprsTsy: RegisterNotification - Config Changed"));
			return KErrNone;
		case EPacketContextNotifyStatusChange:
			LOGTEXT(_L8("CGprsContextDGprsTsy: RegisterNotification - Status Change"));
			return KErrNone;
		case EPacketContextNotifyDataTransferred:
			LOGTEXT(_L8("CGprsContextDGprsTsy: RegisterNotification - DataTransferred"));
			return KErrNone;
		case EPacketContextNotifyConnectionSpeedChange:
			LOGTEXT(_L8("CGprsContextDGprsTsy: RegisterNotification - Connection Speed Change"));
			return KErrNone;
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextNotifyConnectionInfoChange:
			LOGTEXT(_L8("CGprsContextDGprsTsy: RegisterNotification - Connection Info Change"));
			return KErrNone;                			
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsContextDGprsTsy: Register error, unknown IPC"));
			return KErrNotSupported;
		}
	}

TInt CGprsContextDGprsTsy::DeregisterNotification(const TInt aIpc)
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
		case EPacketContextNotifyConfigChanged:
			LOGTEXT(_L8("CGprsContextDGprsTsy: DeregisterNotification - Config Changed"));
			return KErrNone;
		case EPacketContextNotifyStatusChange:
			LOGTEXT(_L8("CGprsContextDGprsTsy: DeregisterNotification - Status Change"));
			return KErrNone;
		case EPacketContextNotifyDataTransferred:
			LOGTEXT(_L8("CGprsContextDGprsTsy: DeregisterNotification - DataTransferred"));
			return KErrNone;
		case EPacketContextNotifyConnectionSpeedChange:
			LOGTEXT(_L8("CGprsContextDGprsTsy: DeregisterNotification - Connection Speed Change"));
			return KErrNone;
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
		case EPacketContextNotifyConnectionInfoChange:
			LOGTEXT(_L8("CGprsContextDGprsTsy: DeregisterNotification - Connection Info Change"));
			return KErrNone;						
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsContextDGprsTsy: Deregister error, unknown IPC"));
			return KErrNotSupported;
		}
	}

TInt CGprsContextDGprsTsy::NumberOfSlotsL(const TInt aIpc)
/**
 * NumberOfSlotsL is called by the server when it is registering a new notification.
 * It enables the TSY to tell the server how many buffer slots to allocate for
 * "repost immediately" notifications that may trigger before clients collect them.
 */
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
		case EPacketContextNotifyConfigChanged:
		case EPacketContextNotifyStatusChange:
		case EPacketContextNotifyDataTransferred:
        case EPacketContextNotifyConnectionSpeedChange:
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextNotifyConnectionInfoChange:                        
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
			LOGTEXT(_L8("CGprsDGprsTsy: Registered with 5 slots"));
			numberOfSlots=5;
			break;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsContextDGprsTsy: Number of Slots error, unknown IPC"));
			User::Leave(KErrNotSupported);
			break;
		}  
	return numberOfSlots;
	}


TInt CGprsContextDGprsTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
/**
 * ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
 * for the TSY to process. A request handle, request type and request data are passed 
 * to the TSY
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy: ExtFunc() called"));

	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
		case EPacketContextSetConfig:
			return SetConfig(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextSetConfigCancel:
			return SetConfigCancel(aTsyReqHandle);
		case EPacketContextGetConfig:
			return GetConfig(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextGetConfigCancel:
			return GetConfigCancel(aTsyReqHandle);
		case EPacketContextNotifyConfigChanged:
			return NotifyConfigChanged(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextNotifyConfigChangedCancel:
			return NotifyConfigChangedCancel(aTsyReqHandle);
		case EPacketContextActivate:
			return Activate(aTsyReqHandle);
		case EPacketContextActivateCancel:
			return ActivateCancel(aTsyReqHandle);
		case EPacketContextDeactivate:
			return Deactivate(aTsyReqHandle);
		case EPacketContextDeactivateCancel:
			return DeactivateCancel(aTsyReqHandle);
		case EPacketContextDelete:
			return Delete(aTsyReqHandle);
		case EPacketContextDeleteCancel:
			return DeleteCancel(aTsyReqHandle);
		case EPacketContextLoanCommPort:
			return LoanCommPort(aTsyReqHandle,
			REINTERPRET_CAST(RCall::TCommPort*, dataPtr));
		case EPacketContextLoanCommPortCancel:
			return LoanCommPortCancel(aTsyReqHandle);
		case EPacketContextRecoverCommPort:
			return RecoverCommPort(aTsyReqHandle);
		case EPacketContextRecoverCommPortCancel:
			return RecoverCommPortCancel(aTsyReqHandle);
		case EPacketContextGetStatus:
			return GetStatus(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TContextStatus*, dataPtr));
		case EPacketContextNotifyStatusChange:
			return NotifyStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TContextStatus*, dataPtr));
		case EPacketContextNotifyStatusChangeCancel:
			return NotifyStatusChangeCancel(aTsyReqHandle);
		case EPacketContextGetDataVolumeTransferred:
			return GetDataVolumeTransferred(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TDataVolume*, dataPtr));
		case EPacketContextGetDataVolumeTransferredCancel:
			return GetDataVolumeTransferredCancel(aTsyReqHandle);
		case EPacketContextGetConnectionSpeed:
			return GetConnectionSpeed(aTsyReqHandle,
				REINTERPRET_CAST(TUint*, dataPtr));
		case EPacketContextNotifyConnectionSpeedChange:
			return NotifyConnectionSpeedChange(aTsyReqHandle,
				REINTERPRET_CAST(TUint*, dataPtr));
		case EPacketContextNotifyDataTransferred:
			return NotifyDataTransferred(aTsyReqHandle,
			REINTERPRET_CAST(RPacketContext::TDataVolume*, dataPtr),
			REINTERPRET_CAST(RPacketContext::TNotifyDataTransferredRequest*, dataPtr2));
		case EPacketContextNotifyDataTransferredCancel:
			return NotifyDataTransferredCancel(aTsyReqHandle);
		case EPacketContextGetLastErrorCause:
			return GetLastErrorCause(aTsyReqHandle,
			REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketContextGetProfileName:
			return GetProfileName(aTsyReqHandle,
			REINTERPRET_CAST(TName*, dataPtr));
		case EPacketContextInitialiseContext:
			return InitialiseContext(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextInitialiseContextCancel:
			return InitialiseContextCancel(aTsyReqHandle);
		case EPacketContextEnumeratePacketFilters:
			return EnumeratePacketFilters(aTsyReqHandle,
				REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketContextEnumeratePacketFiltersCancel:
			return EnumeratePacketFiltersCancel(aTsyReqHandle);
		case EPacketContextGetPacketFilterInfo:
			return GetPacketFilterInfo(aTsyReqHandle,
				REINTERPRET_CAST(TInt*, dataPtr),
				aPackage.Des2n());
		case EPacketContextGetPacketFilterInfoCancel:
			return GetPacketFilterInfoCancel(aTsyReqHandle);
		case EPacketContextAddPacketFilter:
			return AddPacketFilter(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextAddPacketFilterCancel:
			return AddPacketFilterCancel(aTsyReqHandle);
		case EPacketContextRemovePacketFilter:
			return RemovePacketFilter(aTsyReqHandle,
				REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketContextRemovePacketFilterCancel:
			return RemovePacketFilterCancel(aTsyReqHandle);
		case EPacketContextModifyActiveContext:
			return ModifyActiveContext(aTsyReqHandle);
		case EPacketContextModifyActiveContextCancel:
			return ModifyActiveContextCancel(aTsyReqHandle);
		case EPacketContextGetDNSInfo:
			return GetDnsInfo(aTsyReqHandle, aPackage.Des1n());
		case EPacketContextGetDNSInfoCancel:
			return GetDnsInfoCancel(aTsyReqHandle);
		case EPacketContextCreateNewTFT:  
			return CreateNewTFT(aTsyReqHandle,
				REINTERPRET_CAST(TInt*, dataPtr));
		case EPacketContextDeleteTFT:
			return DeleteTFT(aTsyReqHandle);
		case EPacketAddMediaAuthorization:
			return GetAddMediaAuthorization(aTsyReqHandle, aPackage.Des1n());
		case EPacketAddMediaAuthorizationCancel:
			return GetAddMediaAuthorizationCancel(aTsyReqHandle);
		case EPacketRemoveMediaAuthorization:
			return GetRemoveMediaAuthorization(aTsyReqHandle, reinterpret_cast<RPacketContext::TAuthorizationToken*>(dataPtr));
		case EPacketRemoveMediaAuthorizationCancel:
			return GetRemoveMediaAuthorizationCancel(aTsyReqHandle);
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextGetConnectionInfo:
            return GetConnectionInfo( aTsyReqHandle, aPackage.Des1n() ); 
        case EPacketContextNotifyConnectionInfoChange:
            return NotifyConnectionInfoChange( aTsyReqHandle, aPackage.Des1n() ); 			
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
		default:
			return KErrNotSupported;
		}
	}

TInt CGprsContextDGprsTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
 * CancelService is called by the server when it is "cleaning-up" any still outstanding
 * asynchronous requests before closing a client's sub-session.
 * This will happen if a client closes its R-class handle without cancelling outstanding
 * asynchronous requests.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy: - CancelService called"));
	switch (aIpc)
		{
		case EPacketContextSetConfig:
			return SetConfigCancel(aTsyReqHandle);
		case EPacketContextGetConfig:
			return GetConfigCancel(aTsyReqHandle);
		case EPacketContextNotifyConfigChanged:
			return NotifyConfigChangedCancel(aTsyReqHandle);
		case EPacketContextActivate:
			return ActivateCancel(aTsyReqHandle);
		case EPacketContextDeactivate:
			return DeactivateCancel(aTsyReqHandle);
		case EPacketContextDelete:
			return DeleteCancel(aTsyReqHandle);
		case EPacketContextLoanCommPort:
			return LoanCommPortCancel(aTsyReqHandle);
		case EPacketContextRecoverCommPort:
			return RecoverCommPortCancel(aTsyReqHandle);
		case EPacketContextNotifyStatusChange:
			return NotifyStatusChangeCancel(aTsyReqHandle);
		case EPacketContextNotifyDataTransferred:
			return NotifyDataTransferredCancel(aTsyReqHandle);
		case EPacketContextNotifyConnectionSpeedChange:
			return NotifyConnectionSpeedChangeCancel(aTsyReqHandle);
		case EPacketContextGetDataVolumeTransferred:
			return GetDataVolumeTransferredCancel(aTsyReqHandle);
		case EPacketContextInitialiseContext:
			return InitialiseContextCancel(aTsyReqHandle);
		case EPacketContextEnumeratePacketFilters:
			return EnumeratePacketFiltersCancel(aTsyReqHandle);
		case EPacketContextGetPacketFilterInfo:
			return GetPacketFilterInfoCancel(aTsyReqHandle);
		case EPacketContextAddPacketFilter:
			return AddPacketFilterCancel(aTsyReqHandle);
		case EPacketContextRemovePacketFilter:
			return RemovePacketFilterCancel(aTsyReqHandle);
		case EPacketContextModifyActiveContext:
			return ModifyActiveContextCancel(aTsyReqHandle);
		case EPacketContextGetDNSInfo:
			return GetDnsInfoCancel(aTsyReqHandle);
		case EPacketContextCreateNewTFT:
			return CreateNewTFTCancel(aTsyReqHandle);
		case EPacketContextDeleteTFT:
			return DeleteTFTCancel(aTsyReqHandle);
		case EPacketAddMediaAuthorization:
			return GetAddMediaAuthorizationCancel(aTsyReqHandle);
		case EPacketRemoveMediaAuthorization:
			return GetRemoveMediaAuthorizationCancel(aTsyReqHandle);
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextGetConnectionInfo:
            return GetConnectionInfoCancel(aTsyReqHandle);
        case EPacketContextNotifyConnectionInfoChange:
            return NotifyConnectionInfoChangeCancel(aTsyReqHandle);            			
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
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

TInt CGprsContextDGprsTsy::GetDnsInfo(const TTsyReqHandle aTsyReqHandle, const TDesC8* aDnsInfo)
	{
/**
 * GetDnsInfo method to extract DNS server names from context
 *
 * @param aStatus TRequestStatus object with which error code is returned
 * @param aDnsInfo packaged TDnsInfoV2 object passed as TDes8
 *
 * GetDNSInfo() is needed when the client uses dynamic IPv4 PDP addresses. 
 * After the context is activated the client needs to know the actual primary 
 * and secondary DNS addresses to use when sending packet data. As the network 
 * may have supplied these addresses there is a need for an API method to get 
 * these addresses from the TSY.
 */
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetDnsInfo called"));
	
	User::After(300000); // wait to have KRequestPending
	if(!iGetDNSInfo++)
		{
		RPacketContext::TTDnsInfoV2Pckg* dnsInfoV2Pckg = (RPacketContext::TTDnsInfoV2Pckg*)aDnsInfo;
		RPacketContext::TDnsInfoV2& dnsInfoV2 = (*dnsInfoV2Pckg)();

		dnsInfoV2.iPrimaryDns = DPCKTTSY_DNS_SERVER1;
		dnsInfoV2.iSecondaryDns = DPCKTTSY_DNS_SERVER2;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetDnsInfoCancel(const TTsyReqHandle aTsyReqHandle) 
/**
 * GetDnsInfoCancel - Cancels an asynchronous GetDnsInfo method.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetDnsInfoCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}


TInt CGprsContextDGprsTsy::SetConfig(const TTsyReqHandle aTsyReqHandle,const TDesC8* aConfig)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::SetConfig called"));

	User::After(300000); // wait to have KRequestPending
	TPckg<TPacketDataConfigBase>* configBasePckg = (TPckg<TPacketDataConfigBase>*)aConfig;
	TPacketDataConfigBase& configBase = (*configBasePckg)();

	// GPRS configuration
	if(configBase.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
		{
		TPckg<RPacketContext::TContextConfigGPRS>* contextConfigGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aConfig;
		RPacketContext::TContextConfigGPRS& contextConfig = (*contextConfigGPRSPckg)();
		
		if ((contextConfig.iAccessPointName != DPCKTTSY_ACCESS_POINT1)	
			||(contextConfig.iAnonymousAccessReqd != DPCKTTSY_ANONYMOUS_ACCESS1)
			||(contextConfig.iPdpAddress != DPCKTTSY_PDP_ADDRESS1)
			||(contextConfig.iPdpCompression != DPCKTTSY_COMPRESSION1)
			||(contextConfig.iPdpType != DPCKTTSY_PDP_TYPE1)
			||(contextConfig.iUseEdge != DPCKTTSY_EGPRS_REQUIRED)
			||(contextConfig.iProtocolConfigOption.iAuthInfo.iUsername != DPCKTTSY_USER_NAME1)
			||(contextConfig.iProtocolConfigOption.iAuthInfo.iPassword != DPCKTTSY_PASSWORD1)
			||(contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol != DPCKTTSY_PROTOCOL_TYPE1_0))
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else 
			{
			if(!iSetConfig++)
				ReqCompleted(aTsyReqHandle,KErrNone);
			else
				iSetConfig = 0; //reset for V2 tests
			iTsyAsyncReqHandle = aTsyReqHandle;
			}
		}
	else if (configBase.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
		{
		TPckg<RPacketContext::TContextConfigR99_R4>* configParam = (TPckg<RPacketContext::TContextConfigR99_R4>*)aConfig;
		RPacketContext::TContextConfigR99_R4& contextConfig = (*configParam)();
		
		if(((contextConfig.iAccessPointName).Compare(DPCKTTSY_ACCESS_POINT1) != KErrNone) ||
			   ((contextConfig.iPdpAddress).Compare(DPCKTTSY_PDP_ADDRESS1) != KErrNone) ||
			   (contextConfig.iPdpType != DPCKTTSY_PDP_TYPE1) ||
			   contextConfig.iUseEdge != DPCKTTSY_EGPRS_REQUIRED)
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			{
			if(!iSetConfig++)
				ReqCompleted(aTsyReqHandle,KErrNone);
			else
				iSetConfig = 0; //reset for V2 tests
			iTsyAsyncReqHandle = aTsyReqHandle;
			}
		}
	else if (configBase.ExtensionId()==TPacketDataConfigBase::TPacketDataConfigBase::KConfigRel5)
		{
		TPckg<RPacketContext::TContextConfig_R5>* configParam = (TPckg<RPacketContext::TContextConfig_R5>*)aConfig;
		RPacketContext::TContextConfig_R5& contextConfig = (*configParam)();
		
		if(((contextConfig.iAccessPointName).Compare(DPCKTTSY_ACCESS_POINT1) != KErrNone) ||
			((contextConfig.iPdpAddress).Compare(DPCKTTSY_PDP_ADDRESS1) != KErrNone) ||
			(contextConfig.iPdpType != DPCKTTSY_PDP_TYPE1) ||
			(contextConfig.iUseEdge != DPCKTTSY_EGPRS_REQUIRED) || 
			(contextConfig.iPdpDataCompression != DPCKTTSY_DATACOMPRESSION_EV42) ||
			(contextConfig.iPdpHeaderCompression != DPCKTTSY_HEADERCOMPRESSION_ERFC1144))
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			{
			if(!iSetConfig++)
				ReqCompleted(aTsyReqHandle,KErrNone);
			else
				iSetConfig = 0; //reset for V2 tests
			iTsyAsyncReqHandle = aTsyReqHandle;
			}
		}
	else if(configBase.ExtensionId()==TPacketDataConfigBase::KConfigMBMS)
		{
		TPckg<RPacketMbmsContext::TContextConfigMbmsV1>* contextConfigMbmsPckg = (TPckg<RPacketMbmsContext::TContextConfigMbmsV1>*)aConfig;
		RPacketMbmsContext::TContextConfigMbmsV1& contextConfig = (*contextConfigMbmsPckg)();
		
		if ((contextConfig.iMbmsAccessBearer != DPCKTTSY_MBMS_ACCESSBEARER_ONE)	
			||(contextConfig.iMbmsServicePriority != DPCKTTSY_MBMS_SERVICEPRIORITY_ONE)
			||(contextConfig.iMbmsServiceMode != DPCKTTSY_MBMS_SERVICEMODE_ONE)
			||(contextConfig.iMbmsSessionFlag != DPCKTTSY_MBMS_SESSION_FLAG_TRUE)
			||(contextConfig.iTmgi.GetMCC() != DPCKTTSY_MBMS_MCC_ONE)
			||(contextConfig.iTmgi.GetMNC() != DPCKTTSY_MBMS_MNC_ONE)
			||(contextConfig.iTmgi.GetServiceId() != DPCKTTSY_MBMS_SERVICEID_ONE))
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			{
			if(!iSetConfig++)
				{
				ReqCompleted(aTsyReqHandle,KErrNone);
				}
			else
				{
				iSetConfig = 0; //reset for V2 tests
				}
			iTsyAsyncReqHandle = aTsyReqHandle;
			}
		}
		
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::SetConfigCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::SetConfigCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetConfig(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetConfig called"));

	User::After(300000); // wait to have KRequestPending
	TPckg<TPacketDataConfigBase>* configBasePckg = (TPckg<TPacketDataConfigBase>*)aConfig;
	TPacketDataConfigBase& configBase = (*configBasePckg)();

	if(!iGetConfig++)
		{
		if(configBase.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
			{
			
			TPckg<RPacketContext::TContextConfigGPRS>* contextConfigGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aConfig;
			RPacketContext::TContextConfigGPRS& contextConfig = (*contextConfigGPRSPckg)();

			contextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT2;
			contextConfig.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS2;
			contextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS2;
			contextConfig.iPdpCompression = DPCKTTSY_COMPRESSION2;
			contextConfig.iPdpType = DPCKTTSY_PDP_TYPE2;
			contextConfig.iUseEdge = DPCKTTSY_EGPRS_NOTREQUIRED;
			contextConfig.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
			contextConfig.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
			contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
			contextConfig.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
			
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (configBase.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
			{
			TPckg<RPacketContext::TContextConfigR99_R4>* contextConfigR99Pckg = (TPckg<RPacketContext::TContextConfigR99_R4>*)aConfig;
			RPacketContext::TContextConfigR99_R4& contextConfig = (*contextConfigR99Pckg)();

			contextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT2;
			contextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS2;
			contextConfig.iPdpType = DPCKTTSY_PDP_TYPE2;
			contextConfig.iUseEdge = DPCKTTSY_EGPRS_NOTREQUIRED;
			contextConfig.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
			contextConfig.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
			contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
			contextConfig.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
			contextConfig.iPFI = DPCKTTSY_PFICONTEXT_SMS;
			
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (configBase.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
			{
			TPckg<RPacketContext::TContextConfig_R5>* contextConfigR5Pckg = (TPckg<RPacketContext::TContextConfig_R5>*)aConfig;
			RPacketContext::TContextConfig_R5& contextConfig = (*contextConfigR5Pckg)();

			contextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT2;
			contextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS2;
			contextConfig.iPdpType = DPCKTTSY_PDP_TYPE2;
			contextConfig.iUseEdge = DPCKTTSY_EGPRS_NOTREQUIRED;
			contextConfig.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
			contextConfig.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
			contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
			contextConfig.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
			contextConfig.iPFI = DPCKTTSY_PFICONTEXT_SMS;
			contextConfig.iPdpDataCompression = DPCKTTSY_DATACOMPRESSION_EV44;
			contextConfig.iPdpHeaderCompression = DPCKTTSY_HEADERCOMPRESSION_ERFC2507;
			
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if(configBase.ExtensionId()==TPacketDataConfigBase::KConfigMBMS)
			{
			
			TPckg<RPacketMbmsContext::TContextConfigMbmsV1>* contextConfigMbmsPckg = (TPckg<RPacketMbmsContext::TContextConfigMbmsV1>*)aConfig;
			RPacketMbmsContext::TContextConfigMbmsV1& contextConfig = (*contextConfigMbmsPckg)();

			contextConfig.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_ONE;	
			contextConfig.iMbmsServicePriority = DPCKTTSY_MBMS_SERVICEPRIORITY_ONE;
			contextConfig.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_ONE;
			contextConfig.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_ONE);
			contextConfig.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_ONE);
			contextConfig.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_ONE);

			ReqCompleted(aTsyReqHandle,KErrNone);
			}	
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetConfigCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetConfigCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::NotifyConfigChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig)
	{
	TPckg<TPacketDataConfigBase>* configBasePckg = (TPckg<TPacketDataConfigBase>*)aConfig;
	TPacketDataConfigBase& configBase = (*configBasePckg)();

	LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyConfigChanged called"));

	User::After(300000); // wait to have KRequestPending
	if (!iNotifyConfigChanged++)
		{
		if(configBase.ExtensionId()==TPacketDataConfigBase::KConfigGPRS)
			{
			TPckg<RPacketContext::TContextConfigGPRS>* contextConfigGPRSPckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aConfig;
			RPacketContext::TContextConfigGPRS& contextConfigV1 = (*contextConfigGPRSPckg)();

			contextConfigV1.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
			contextConfigV1.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
			contextConfigV1.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
			contextConfigV1.iPdpCompression = DPCKTTSY_COMPRESSION1;
			contextConfigV1.iPdpType = DPCKTTSY_PDP_TYPE1;
			contextConfigV1.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
			contextConfigV1.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
			contextConfigV1.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
			contextConfigV1.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
			contextConfigV1.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;

			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (configBase.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4)
			{
			TPckg<RPacketContext::TContextConfigR99_R4>* contextConfigR99Pckg = (TPckg<RPacketContext::TContextConfigR99_R4>*)aConfig;
			RPacketContext::TContextConfigR99_R4& contextConfigV1 = (*contextConfigR99Pckg)();

			contextConfigV1.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
			contextConfigV1.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
			contextConfigV1.iPdpType = DPCKTTSY_PDP_TYPE1;
			contextConfigV1.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
			contextConfigV1.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
			contextConfigV1.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
			contextConfigV1.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
			contextConfigV1.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
			contextConfigV1.iPFI = DPCKTTSY_PFICONTEXT_SIG;

			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (configBase.ExtensionId()==TPacketDataConfigBase::KConfigRel5)
			{
			TPckg<RPacketContext::TContextConfig_R5>* contextConfigR5Pckg = (TPckg<RPacketContext::TContextConfig_R5>*)aConfig;
			RPacketContext::TContextConfig_R5& contextConfigV1 = (*contextConfigR5Pckg)();

			contextConfigV1.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
			contextConfigV1.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
			contextConfigV1.iPdpType = DPCKTTSY_PDP_TYPE1;
			contextConfigV1.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
			contextConfigV1.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
			contextConfigV1.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
			contextConfigV1.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
			contextConfigV1.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
			contextConfigV1.iPFI = DPCKTTSY_PFICONTEXT_SIG;
			contextConfigV1.iPdpDataCompression = DPCKTTSY_DATACOMPRESSION_EV42;
			contextConfigV1.iPdpHeaderCompression = DPCKTTSY_HEADERCOMPRESSION_ERFC1144;

			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if(configBase.ExtensionId()==TPacketDataConfigBase::KConfigMBMS)
			{
			TPckg<RPacketMbmsContext::TContextConfigMbmsV1>* contextConfigMbmsPckg = (TPckg<RPacketMbmsContext::TContextConfigMbmsV1>*)aConfig;
			RPacketMbmsContext::TContextConfigMbmsV1& contextConfig = (*contextConfigMbmsPckg)();

			contextConfig.iMbmsAccessBearer = DPCKTTSY_MBMS_ACCESSBEARER_ONE;	
			contextConfig.iMbmsServicePriority = DPCKTTSY_MBMS_SERVICEPRIORITY_ONE;
			contextConfig.iMbmsServiceMode = DPCKTTSY_MBMS_SERVICEMODE_ONE;
			contextConfig.iTmgi.SetMCC(DPCKTTSY_MBMS_MCC_ONE);
			contextConfig.iTmgi.SetMNC(DPCKTTSY_MBMS_MNC_ONE);
			contextConfig.iTmgi.SetServiceId(DPCKTTSY_MBMS_SERVICEID_ONE);

			ReqCompleted(aTsyReqHandle,KErrNone);
			}	
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::NotifyConfigChangedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyConfigChangedCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::Deactivate(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::Deactivate called"));
	User::After(300000); // wait to have KRequestPending
	if(!iDeactivate++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::DeactivateCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::DeactivateCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}
	
TInt CGprsContextDGprsTsy::Activate(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::Activate called"));
	User::After(300000); // wait to have KRequestPending
	if(!iActivate++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::ActivateCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::ActivateCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::Delete(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::Delete called"));
	User::After(300000); // wait to have KRequestPending
	if(!iDelete++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::DeleteCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::DeleteCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::LoanCommPort(const TTsyReqHandle aTsyReqHandle,RCall::TCommPort* aDataPort)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::LoanCommPort called"));
	User::After(300000); // wait to have KRequestPending
	if(!iLoanCommPort++)
		{
		aDataPort->iCsy = DPCKTTSY_CSY;
		aDataPort->iPort = DPCKTTSY_PORT;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::LoanCommPortCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::LoanCommPortCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::RecoverCommPort(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::RecoverCommPort called"));
	User::After(300000); // wait to have KRequestPending
	if(!iRecoverCommPort++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::RecoverCommPortCancel(const TTsyReqHandle aTsyReqHandle)	
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::RecoverCommPortCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetStatus(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetStatus called"));
	User::After(300000); // wait to have KRequestPending
	*aContextStatus = DPCKTTSY_CONTEXT_STATUS1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RPacketContext::TContextStatus* aContextStatus)
	{
	User::After(300000); // wait to have KRequestPending
	if (!iNotifyStatusChange++)
		{
		LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyStatusChange called"));
		*aContextStatus = DPCKTTSY_CONTEXT_STATUS2;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyStatusChangeCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetProfileName(const TTsyReqHandle aTsyReqHandle, TName* aQosProfile)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetProfileName called"));
	User::After(300000); // wait to have KRequestPending
	*aQosProfile = DPCKTTSY_PROFILE_NAME;
	ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


TInt CGprsContextDGprsTsy::GetDataVolumeTransferred(const TTsyReqHandle aTsyReqHandle, RPacketContext::TDataVolume* aVolume)
/** 
 * GetDataVolumeTransferred method - Returns the volume of data transferred.
 * A synchronous and asynchronous variant of this method is now supported - Change 
 * Request: NDOE-58VF4Q. In the test code, the synchronous method is called first.
 * Note that the synchronous variant is deprecated.
 * In the switch statement, cases 0 and 1 implement the synchronous call, while cases 2 and 
 * 3 implement the asynchronous call.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetDataVolumeTransferred called"));
	
//	User::After(300000); // wait to have KRequestPending
	switch (iGetDataVolumeTransferred++)
		{
		case 0:
		case 1:
			aVolume->iBytesSent = DPCKTTSY_BYTES_SENT1;
			aVolume->iOverflowCounterSent = DPCKTTSY_OVERFLOW_COUNTER_SENT1;
			aVolume->iBytesReceived = DPCKTTSY_BYTES_RCVD1;
			aVolume->iOverflowCounterReceived = DPCKTTSY_OVERFLOW_COUNTER_RCVD1;
	
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;
		case 2:
			aVolume->iBytesSent = DPCKTTSY_BYTES_SENT3;
			aVolume->iOverflowCounterSent = DPCKTTSY_OVERFLOW_COUNTER_SENT3;
			aVolume->iBytesReceived = DPCKTTSY_BYTES_RCVD3;
			aVolume->iOverflowCounterReceived = DPCKTTSY_OVERFLOW_COUNTER_RCVD3;
			ReqCompleted(aTsyReqHandle,KErrNone);
		case 3:
			iTsyAsyncReqHandle = aTsyReqHandle;
			break;
		default:
			LOGTEXT(_L8("CGprsContextDGprsTsy::GetDataVolumeTransferred - Error in switch statement"));
			break;
		} // switch

	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetDataVolumeTransferredCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetDataVolumeTransferredCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetConnectionSpeed(const TTsyReqHandle aTsyReqHandle,TUint* aRate)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetConnectionSpeed called"));
	User::After(300000); // wait to have KRequestPending
	*aRate = DPCKTTSY_CONNECTION_RATE;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::NotifyConnectionSpeedChange(const TTsyReqHandle aTsyReqHandle, TUint* aRate)
	{
	User::After(300000); // wait to have KRequestPending
	if (!iNotifySpeedChange++)
		{
		LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyStatusChange called"));
			*aRate = DPCKTTSY_CONNECTION_RATE;
			ReqCompleted(aTsyReqHandle,KErrNone);
		}
    	iTsyAsyncReqHandle = aTsyReqHandle;
		return KErrNone;
	}

TInt CGprsContextDGprsTsy::NotifyConnectionSpeedChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyConnectionSpeedChangeCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::NotifyDataTransferred(const TTsyReqHandle aTsyReqHandle, RPacketContext::TDataVolume* aVolume, RPacketContext::TNotifyDataTransferredRequest* aData)
	{
	User::After(300000); // wait to have KRequestPending
	if ((aData->iRcvdGranularity != DPCKTTSY_RCVD_GRANULARITY) ||
		(aData->iSentGranularity != DPCKTTSY_SENT_GRANULARITY))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		if (!iNotifyDataTransferred++)
			{
			LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyStatusChange called"));
			aVolume->iBytesSent = DPCKTTSY_BYTES_SENT2;
			aVolume->iOverflowCounterSent = DPCKTTSY_OVERFLOW_COUNTER_SENT2;
			aVolume->iBytesReceived = DPCKTTSY_BYTES_RCVD2;
			aVolume->iOverflowCounterReceived = DPCKTTSY_OVERFLOW_COUNTER_RCVD2;
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		iTsyAsyncReqHandle = aTsyReqHandle;
		}
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::NotifyDataTransferredCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyDataTransferredCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetLastErrorCause(const TTsyReqHandle aTsyReqHandle,TInt* aError)
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetLastErrorCause called"));
	User::After(300000); // wait to have KRequestPending
	*aError = DPCKTTSY_LAST_ERROR_CAUSE;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::InitialiseContext(const TTsyReqHandle aTsyReqHandle, TDes8* aDataChannel)
/**
 * InitialiseContext method - Enables asynchronous resource reservation.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::InitialiseContext called"));

	User::After(300000); // wait to have KRequestPending
	RPacketContext::TDataChannelV2Pckg* dataChannelV2Pckg = (RPacketContext::TDataChannelV2Pckg*)aDataChannel;
	RPacketContext::TDataChannelV2& dataChannelV2 = (*dataChannelV2Pckg)();

	if(!iInitialiseContext++)
		{
		dataChannelV2.iCsy = DPCKTTSY_CSY_ASYNC;
		dataChannelV2.iPort = DPCKTTSY_PORT_ASYNC;
		dataChannelV2.iChannelId = DPCKTTSY_CHANNEL_ID;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;

	return KErrNone;
	}

TInt CGprsContextDGprsTsy::InitialiseContextCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * InitialiseContextCancel - Cancels an asynchronous InitialiseContext() request.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::InitialiseContextCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::EnumeratePacketFilters(const TTsyReqHandle aTsyReqHandle, TInt* aCount)
/** 
 * EnumeratePacketFilters method - Sets the number of packet filters
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::EnumeratePacketFilters called"));
	
	User::After(300000); // wait to have KRequestPending
	if (!iEnumeratePacketFilters++)
		{
		*aCount = DPCKTTSY_NUMBER_OF_PACKET_FILTERS;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	
	iTsyAsyncReqHandle = aTsyReqHandle;
	
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::EnumeratePacketFiltersCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * EnumeratePacketFiltersCancel - Cancels asynchronous EnumeratePacketFilters request.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::EnumeratePacketFiltersCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}
	
TInt CGprsContextDGprsTsy::GetPacketFilterInfo(const TTsyReqHandle aTsyReqHandle,TInt* aIndex, TDes8* aPacketFilterInfo)
/**
 * GetPacketFilterInfo method - Sets Packet filter information.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetPacketFilterInfo called"));
	
	TPckg<TPacketBase>* packetFilterBasePckg = (TPckg<TPacketBase>*)aPacketFilterInfo;
	TPacketBase& packetFilterBase = (*packetFilterBasePckg)();

	if (packetFilterBase.ExtensionId() == KETelExtPcktV2)
		{

		User::After(300000); // wait to have KRequestPending
		RPacketContext::TPacketFilterV2Pckg* packetFilterV2Pckg = (RPacketContext::TPacketFilterV2Pckg*)aPacketFilterInfo;
		RPacketContext::TPacketFilterV2& packetFilterV2 = (*packetFilterV2Pckg)();
		
		if(!iGetPacketFilterInfo++)
			{
			switch (*aIndex)
				{
				case 1:
					packetFilterV2.iId=DPCKTTSY_FILTER_ID1;
					packetFilterV2.iEvaluationPrecedenceIndex=DPCKTTSY_EVALUATION_PRECEDENCE_INDEX1;
					
					packetFilterV2.iSrcAddr[0]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_0;
					packetFilterV2.iSrcAddr[1]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_1;
					packetFilterV2.iSrcAddr[2]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_2;
					packetFilterV2.iSrcAddr[3]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_3;
					
					packetFilterV2.iSrcAddrSubnetMask[0]=DPCKTTSY_DOMAIN_SUBNET_MASK1_0;
					packetFilterV2.iSrcAddrSubnetMask[1]=DPCKTTSY_DOMAIN_SUBNET_MASK1_1;
					packetFilterV2.iSrcAddrSubnetMask[2]=DPCKTTSY_DOMAIN_SUBNET_MASK1_2;
					packetFilterV2.iSrcAddrSubnetMask[3]=DPCKTTSY_DOMAIN_SUBNET_MASK1_3;
					packetFilterV2.iProtocolNumberOrNextHeader=DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER1;
					packetFilterV2.iSrcPortMin=DPCKTTSY_MINIMUM_SOURCE_PORT_NUMBER1;
					packetFilterV2.iSrcPortMax=DPCKTTSY_MAXIMUM_SOURCE_PORT_NUMBER1;
					packetFilterV2.iDestPortMin=DPCKTTSY_MINIMUM_DESTINATION_PORT_NUMBER1;
					packetFilterV2.iDestPortMax=DPCKTTSY_MAXIMUM_DESTINATION_PORT_NUMBER1;
					packetFilterV2.iTOSorTrafficClass=DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1;
					
					ReqCompleted(aTsyReqHandle,KErrNone);
					break;
				case 2:
					packetFilterV2.iId=DPCKTTSY_FILTER_ID2;
					packetFilterV2.iEvaluationPrecedenceIndex=DPCKTTSY_EVALUATION_PRECEDENCE_INDEX2;
					
					packetFilterV2.iSrcAddr[0]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_0;
					packetFilterV2.iSrcAddr[1]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_1;
					packetFilterV2.iSrcAddr[2]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_2;
					packetFilterV2.iSrcAddr[3]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_3;
					
					packetFilterV2.iSrcAddrSubnetMask[0]=DPCKTTSY_DOMAIN_SUBNET_MASK2_0;
					packetFilterV2.iSrcAddrSubnetMask[1]=DPCKTTSY_DOMAIN_SUBNET_MASK2_1;
					packetFilterV2.iSrcAddrSubnetMask[2]=DPCKTTSY_DOMAIN_SUBNET_MASK2_2;
					packetFilterV2.iSrcAddrSubnetMask[3]=DPCKTTSY_DOMAIN_SUBNET_MASK2_3;
					
					packetFilterV2.iProtocolNumberOrNextHeader=DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER2;
					packetFilterV2.iIPSecSPI=DPCKTTSY_SECURITY_PARAMETER_INDEX2;
					packetFilterV2.iTOSorTrafficClass=DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS2;

					ReqCompleted(aTsyReqHandle,KErrNone);
					break;
				default:
					ReqCompleted(aTsyReqHandle,KErrArgument);
					break;
				} //switch
			} // if
		}// First if
	
	if (packetFilterBase.ExtensionId() == KETelExtPcktV3)
		{
		User::After(300000); // wait to have KRequestPending
		RPacketContext::TPacketFilterV3Pckg* packetFilterV3Pckg = (RPacketContext::TPacketFilterV3Pckg*)aPacketFilterInfo;
		RPacketContext::TPacketFilterV3& packetFilterV3 = (*packetFilterV3Pckg)();
		
		RPacketContext::TPacketFilterV3 packetFilterV3a(RPacketContext::EPacketFilterTypeOne);
		RPacketContext::TPacketFilterV3 packetFilterV3b(RPacketContext::EPacketFilterTypeTwo);
		
		RPacketContext::TIPAddress srcAddr;
		RPacketContext::TIPAddress srcAddrSubnetMask;
		switch(*aIndex)
			{
			case 3:
				TInt ii;
				for (ii=0; ii<sizeof(RPacketContext::TIPAddress); ii++)
					{
					srcAddr[ii] = 0;	
					}
				srcAddr[0]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_0;
				srcAddr[1]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_1;
				srcAddr[2]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_2;
				srcAddr[3]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_3;
			
				for (ii=0; ii<sizeof(RPacketContext::TIPAddress); ii++)
					{
					srcAddrSubnetMask[ii] = 0;	
					}						
				srcAddrSubnetMask[0]=DPCKTTSY_DOMAIN_SUBNET_MASK1_0;
				srcAddrSubnetMask[1]=DPCKTTSY_DOMAIN_SUBNET_MASK1_1;
				srcAddrSubnetMask[2]=DPCKTTSY_DOMAIN_SUBNET_MASK1_2;
				srcAddrSubnetMask[3]=DPCKTTSY_DOMAIN_SUBNET_MASK1_3;
							
				packetFilterV3a.SetSrcAddr(srcAddr);
				packetFilterV3a.SetSrcAddrSubnetMask(srcAddrSubnetMask);
				packetFilterV3a.SetProtocolNumberOrNextHeader(DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER1);
				packetFilterV3a.SetSrcPortMin(DPCKTTSY_MINIMUM_SOURCE_PORT_NUMBER1);
				packetFilterV3a.SetSrcPortMax(DPCKTTSY_MAXIMUM_SOURCE_PORT_NUMBER1);
				packetFilterV3a.SetDestPortMin(DPCKTTSY_MINIMUM_DESTINATION_PORT_NUMBER1);
				packetFilterV3a.SetDestPortMax(DPCKTTSY_MAXIMUM_DESTINATION_PORT_NUMBER1);
				packetFilterV3a.SetTOSorTrafficClass(DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1);
				
				packetFilterV3 = packetFilterV3a;
						
				ReqCompleted(aTsyReqHandle,KErrNone);
				break;
			
			case 4:
				srcAddr[0]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_0;
				srcAddr[1]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_1;
				srcAddr[2]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_2;
				srcAddr[3]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS2_3;
					
				srcAddrSubnetMask[0]=DPCKTTSY_DOMAIN_SUBNET_MASK2_0;
				srcAddrSubnetMask[1]=DPCKTTSY_DOMAIN_SUBNET_MASK2_1;
				srcAddrSubnetMask[2]=DPCKTTSY_DOMAIN_SUBNET_MASK2_2;
				srcAddrSubnetMask[3]=DPCKTTSY_DOMAIN_SUBNET_MASK2_3;
				
				packetFilterV3b.SetSrcAddr(srcAddr);
				packetFilterV3b.SetSrcAddrSubnetMask(srcAddrSubnetMask);
				packetFilterV3b.SetProtocolNumberOrNextHeader(DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER2);
				packetFilterV3b.SetSPI(DPCKTTSY_SECURITY_PARAMETER_INDEX2);
				packetFilterV3b.SetTOSorTrafficClass(DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS2);
				
				packetFilterV3 = packetFilterV3b;
						
				ReqCompleted(aTsyReqHandle,KErrNone);
				break;
				
			default:
				ReqCompleted(aTsyReqHandle,KErrArgument);
				break;
			}//Switch
		}// If 
	
	iTsyAsyncReqHandle = aTsyReqHandle;

	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetPacketFilterInfoCancel(const TTsyReqHandle aTsyReqHandle) 
/**
 * GetPacketFilterInfoCancel - Cancels an asynchronous GetPacketFilterInfo method.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetPacketFilterInfoCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::AddPacketFilter(const TTsyReqHandle aTsyReqHandle,const TDesC8* aPacketFilter)
/**
 * AddPacketFilter method - Sets the packet filter information.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::AddPacketFilter called"));
		
	TPckg<TPacketBase>* packetFilterBasePckg = (TPckg<TPacketBase>*)aPacketFilter;
	TPacketBase& packetFilterBase = (*packetFilterBasePckg)();
	
	if (packetFilterBase.ExtensionId() == KETelExtPcktV2)
		{
		User::After(300000); // wait to have KRequestPending
		RPacketContext::TPacketFilterV2Pckg* packetFilterV2Pckg = (RPacketContext::TPacketFilterV2Pckg*)aPacketFilter;
		RPacketContext::TPacketFilterV2& packetFilterV2 = (*packetFilterV2Pckg)();

		if(!iAddPacketFilter++)
			{
			if ((packetFilterV2.iId<0) || (packetFilterV2.iId>8))
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
			else
				ReqCompleted(aTsyReqHandle,KErrNone);
			}
		
		}
	if (packetFilterBase.ExtensionId() == KETelExtPcktV3)
		{
		User::After(300000); // wait to have KRequestPending
		RPacketContext::TPacketFilterV3Pckg* packetFilterV3Pckg = (RPacketContext::TPacketFilterV3Pckg*)aPacketFilter;
		RPacketContext::TPacketFilterV3& packetFilterV3 = (*packetFilterV3Pckg)();
		
		RPacketContext::TIPAddress srcAddr;
		RPacketContext::TIPAddress srcAddrSubnetMask;
		
		TInt ii;
		for (ii=0; ii<sizeof(RPacketContext::TIPAddress); ii++)
			{
			srcAddr[ii] = 0;	
			}
		srcAddr[0]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_0;
		srcAddr[1]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_1;
		srcAddr[2]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_2;
		srcAddr[3]=DPCKTTSY_DOMAIN_SOURCE_ADDRESS1_3;
		
		for (ii=0; ii<sizeof(RPacketContext::TIPAddress); ii++)
			{
			srcAddrSubnetMask[ii] = 0;	
			}					
		srcAddrSubnetMask[0]=DPCKTTSY_DOMAIN_SUBNET_MASK1_0;
		srcAddrSubnetMask[1]=DPCKTTSY_DOMAIN_SUBNET_MASK1_1;
		srcAddrSubnetMask[2]=DPCKTTSY_DOMAIN_SUBNET_MASK1_2;
		srcAddrSubnetMask[3]=DPCKTTSY_DOMAIN_SUBNET_MASK1_3;
				
		switch(iAddPacketFilter++)
			{
			case 2:
				if ((packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddr)) &&
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddrSubnetMask)) &&
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterProtocolNumberOrNextHeader)) && 
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMin)) && 
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMax)) && 
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMin)) &&
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMax)) && 
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterIPSecSPI)) && 
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterTOSorTrafficClass)) && 
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterFlowLabel)))
					{
					if ((packetFilterV3.iId>=0) && (packetFilterV3.iId<8) &&
						(packetFilterV3.iEvaluationPrecedenceIndex>=0) && (packetFilterV3.iEvaluationPrecedenceIndex<255) &&
						!(Mem::Compare(packetFilterV3.SrcAddr(),sizeof(RPacketContext::TIPAddress),srcAddr,sizeof(RPacketContext::TIPAddress))) &&
						!(Mem::Compare(packetFilterV3.SrcAddrSubnetMask(),sizeof(RPacketContext::TIPAddress),srcAddrSubnetMask,sizeof(RPacketContext::TIPAddress))) &&
						(packetFilterV3.ProtocolNumberOrNextHeader() == DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER1) &&
						(packetFilterV3.SrcPortMin() == DPCKTTSY_MINIMUM_SOURCE_PORT_NUMBER1) &&
						(packetFilterV3.SrcPortMax() == DPCKTTSY_MAXIMUM_SOURCE_PORT_NUMBER1) &&
						(packetFilterV3.DestPortMin() == DPCKTTSY_MINIMUM_DESTINATION_PORT_NUMBER1) &&
						(packetFilterV3.DestPortMax() == DPCKTTSY_MAXIMUM_DESTINATION_PORT_NUMBER1) &&
						(packetFilterV3.TOSorTrafficClass() == DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1))
						{
						ReqCompleted(aTsyReqHandle,KErrNone);
						}
					else
						{
						ReqCompleted(aTsyReqHandle,KErrCorrupt);	
						}
					}
				else
					{
					ReqCompleted(aTsyReqHandle,KErrCorrupt);	
					}
				break;
					
			case 3:
				if ((packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddr)) &&
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddrSubnetMask)) &&
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterProtocolNumberOrNextHeader)) && 
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMin)) && 
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMax)) && 
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMin)) &&
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMax)) && 
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterIPSecSPI)) && 
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterTOSorTrafficClass)) && 
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterFlowLabel)))
					{
					if ((packetFilterV3.iId>=0) && (packetFilterV3.iId<8) &&
						(packetFilterV3.iEvaluationPrecedenceIndex>=0) && (packetFilterV3.iEvaluationPrecedenceIndex<255) &&
						!(Mem::Compare(packetFilterV3.SrcAddr(),sizeof(RPacketContext::TIPAddress),srcAddr,sizeof(RPacketContext::TIPAddress))) &&
						!(Mem::Compare(packetFilterV3.SrcAddrSubnetMask(),sizeof(RPacketContext::TIPAddress),srcAddrSubnetMask,sizeof(RPacketContext::TIPAddress))) &&
						(packetFilterV3.ProtocolNumberOrNextHeader() == DPCKTTSY_PROTOCOLNUMBER_OR_NEXTHEADER1) &&
						(packetFilterV3.SPI() == DPCKTTSY_SECURITY_PARAMETER_INDEX2) &&
						(packetFilterV3.TOSorTrafficClass() == DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1))
						{
						ReqCompleted(aTsyReqHandle,KErrNone);
						}
					else
						{
						ReqCompleted(aTsyReqHandle,KErrCorrupt);	
						}
					}
				else
					{
					ReqCompleted(aTsyReqHandle,KErrCorrupt);	
					}
				break;
					
			case 4:
				if ((packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddr)) &&
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcAddrSubnetMask)) &&
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterProtocolNumberOrNextHeader)) && 
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMin)) && 
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterSrcPortMax)) && 
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMin)) &&
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterDestPortMax)) && 
					(!packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterIPSecSPI)) && 
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterTOSorTrafficClass)) && 
					(packetFilterV3.IsPresent(RPacketContext::TPacketFilterV3::EPacketFilterFlowLabel)))
					{
					if ((packetFilterV3.iId>=0) && (packetFilterV3.iId<8) &&
						(packetFilterV3.iEvaluationPrecedenceIndex>=0) && (packetFilterV3.iEvaluationPrecedenceIndex<255) &&
						!(Mem::Compare(packetFilterV3.SrcAddr(),sizeof(RPacketContext::TIPAddress),srcAddr,sizeof(RPacketContext::TIPAddress))) &&
						!(Mem::Compare(packetFilterV3.SrcAddrSubnetMask(),sizeof(RPacketContext::TIPAddress),srcAddrSubnetMask,sizeof(RPacketContext::TIPAddress))) &&
						(packetFilterV3.TOSorTrafficClass() == DPCKTTSY_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS1) &&
						(packetFilterV3.FlowLabel() == DPCKTTSY_FLOW_LABEL))
						{
						ReqCompleted(aTsyReqHandle,KErrNone);
						}
					else
						{
						ReqCompleted(aTsyReqHandle,KErrCorrupt);	
						}
					}
				else
					{
					ReqCompleted(aTsyReqHandle,KErrCorrupt);	
					}
				break;
			
			case 5:
				break;
				
			default:
				ReqCompleted(aTsyReqHandle,KErrArgument);
				break;
			} //switch
		} //if
		
	
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::AddPacketFilterCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * AddPacketFilterInfoCancel - Cancels an asynchronous AddPacketFilterInfo request.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::AddPacketFilterCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::RemovePacketFilter(const TTsyReqHandle aTsyReqHandle, TInt* aId)
/**
 * RemovePacketFilter - removes the packet filter depending on the index argument.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::RemovePacketFilter called"));

	User::After(300000); // wait to have KRequestPending
	if(!iRemovePacketFilter++)
		{
			if ((*aId<0) || (*aId>8))
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
			else
				ReqCompleted(aTsyReqHandle,KErrNone);
		}

	iTsyAsyncReqHandle = aTsyReqHandle;

	return KErrNone;
	}

TInt CGprsContextDGprsTsy::RemovePacketFilterCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * RemovePacketFilterCancel - Cancels an asynchronous RemovePacketFilter method.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::RemovePacketFilterCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::ModifyActiveContext(const TTsyReqHandle aTsyReqHandle)
/**
 * ModifyActiveContext method.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::ModifyActiveContext called"));

	User::After(300000); // wait to have KRequestPending
	if(!iModifyActiveContext++)
		ReqCompleted(aTsyReqHandle,KErrNone);

	iTsyAsyncReqHandle = aTsyReqHandle;

	return KErrNone;
	}

TInt CGprsContextDGprsTsy::ModifyActiveContextCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * ModifyActiveContextCancel - Cancels an asynchronous ModifyActiveContext request.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::ModifyActiveContextCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::CreateNewTFT(const TTsyReqHandle aTsyReqHandle,const TInt* aSize)
/**
 * CreateNewTFT - create new Traffic Flow Template (TFT)
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::CreateNewTFT called"));
	User::After(300000); // wait to have KRequestPending
	if (!iCreateNewTFT++)
		{
		
		if (*aSize != DPCKTTSY_TFT_NO_FILTERS)
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			ReqCompleted(aTsyReqHandle,KErrNone);
		}	
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	
TInt CGprsContextDGprsTsy::CreateNewTFTCancel(const TTsyReqHandle aTsyReqHandle) 
/**
 * CreateNewTFTCancel - Cancels an asynchronous CreateNewTFT request
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::CreateNewTFTCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}
	
TInt CGprsContextDGprsTsy::DeleteTFT(const TTsyReqHandle aTsyReqHandle)
/**
 * DeleteTFT - delete TFT
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::DeleteTFT called"));
	User::After(300000); // wait to have KRequestPending
	if (!iDeleteTFT++)
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}	
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}
	
TInt CGprsContextDGprsTsy::DeleteTFTCancel(const TTsyReqHandle aTsyReqHandle) 
/**
 * CreateNewTFTCancel - Cancels an asynchronous CreateNewTFT request
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::DeleteTFTCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetAddMediaAuthorization(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetAddMediaAuthorization called"));
	User::After(300000); // wait to have KRequestPending
	
	if(!iAddMediaAuthorization++)
		{
		RPacketContext::CTFTMediaAuthorizationV3* iMediaAuthV3 = RPacketContext::CTFTMediaAuthorizationV3::NewL();
		CleanupStack::PushL(iMediaAuthV3);
		iMediaAuthV3->InternalizeL(*aBuffer);
		

		if (!(iMediaAuthV3->iAuthorizationToken).Compare(DPCKTTSY_AUTH_TOKEN) &&
			(iMediaAuthV3->iFlowIds[0].iMediaComponentNumber == DPCKTTSY_COMP_NUMBER1) &&
			(iMediaAuthV3->iFlowIds[0].iIPFlowNumber == DPCKTTSY_IPFLOW_NUMBER1) &&
			(iMediaAuthV3->iFlowIds[1].iMediaComponentNumber == DPCKTTSY_COMP_NUMBER2) &&
			(iMediaAuthV3->iFlowIds[1].iIPFlowNumber == DPCKTTSY_IPFLOW_NUMBER2))
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			CleanupStack::Pop(iMediaAuthV3);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			CleanupStack::Pop(iMediaAuthV3);	
			}
		}
	
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	
 	}

TInt CGprsContextDGprsTsy::GetRemoveMediaAuthorization(const TTsyReqHandle aTsyReqHandle, RPacketContext::TAuthorizationToken* aAuthorizationToken)
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetRemoveMediaAuthorization called"));
	User::After(300000); // wait to have KRequestPending

	if(!iRemoveMediaAuthorization++)
		{
		if (!((*aAuthorizationToken).Compare(DPCKTTSY_AUTH_TOKEN)))
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		}
	
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	
	}

TInt CGprsContextDGprsTsy::GetAddMediaAuthorizationCancel(const TTsyReqHandle aTsyReqHandle) 
/**
 * GetAddMediaAuthorizationCancel - Cancels an asynchronous GetAddMediaAuthorization request
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetAddMediaAuthorizationCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsContextDGprsTsy::GetRemoveMediaAuthorizationCancel(const TTsyReqHandle aTsyReqHandle) 
/**
 * GetRemoveMediaAuthorizationCancel - Cancels an asynchronous GetRemoveMediaAuthorization request
 */
	{
	LOGTEXT(_L8("CGprsDGprsTsy::GetRemoveMediaAuthorizationCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}
	
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
TInt CGprsContextDGprsTsy::GetConnectionInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo)
/** 
 * GetConnectionInfo method - Returns the connection info.
 * A asynchronous variant of this method is supported.
 * In the switch statement, case 0 is completed right away, while case 1 implements the asynchronous call.
 */
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetConnectionInfo called"));
	
	User::After(300000); // wait to have KRequestPending
	switch (iGetConnectionInfo++)
		{
		case 0:
            {
            TInt ret = FillConnectionInfo(aInfo);
            ReqCompleted(aTsyReqHandle,ret);
            break;       
            }
		case 1:
			iTsyAsyncReqHandle = aTsyReqHandle;
			break;
		default:
			LOGTEXT(_L8("CGprsContextDGprsTsy::GetConnectionInfo - Error in switch statement"));
			ReqCompleted(aTsyReqHandle, KErrArgument);
			break;
		} // switch

	return KErrNone;
	}
	
TInt CGprsContextDGprsTsy::GetConnectionInfoCancel(const TTsyReqHandle aTsyReqHandle)
/** 
 * GetConnectionInfoCancel method - Cancel active connection info request.
 */     
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::GetConnectionInfoCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}
	
TInt CGprsContextDGprsTsy::FillConnectionInfo(TDes8* aInfo) 
/** 
 * FillConnectionInfo method - Fill connection information into TConnectionInfoVXX structure.
 */    
	{
	TPckg<TConnectionInfoBase>* infoBasePckg = (TPckg<TConnectionInfoBase>*)aInfo;
	TConnectionInfoBase& infoBase = (*infoBasePckg)();
    
	TInt ret = KErrNone;
    if ( TConnectionInfoBase::KConnectionInfoV1 == infoBase.ExtensionId() )
        {
		TPckg<RPacketContext::TConnectionInfoV1>* infoV1Pckg = (TPckg<RPacketContext::TConnectionInfoV1>*)aInfo;
		RPacketContext::TConnectionInfoV1& infoV1 = (*infoV1Pckg)();	            
            
        infoV1.iValid = RPacketContext::KHSDPACategory | RPacketContext::KHSUPACategory;
        infoV1.iHSDPACategory = DPCKTTSY_HSDPA_CATEGORY;     
        infoV1.iHSUPACategory = DPCKTTSY_HSUPA_CATEGORY;                     
        }
    else
        {
        ret = KErrNotSupported;
        }   
    return ret;    
	}		
	
TInt CGprsContextDGprsTsy::NotifyConnectionInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo)
/** 
 * NotifyConnectionInfoChangeChange method - Notify client when the connection info changes.
 * In the switch statement, case 0 is completed right away, while case 1 implements the asynchronous call.
 */	 	
	{ 
	LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyConnectionInfoChange called"));	      
	User::After(300000); // wait to have KRequestPending
	switch (iNotifyConnectionInfoChange++)
		{
		case 0:
            {
            TInt ret = FillConnectionInfo(aInfo);
            ReqCompleted(aTsyReqHandle,ret);
            break;       
            }
		case 1:
			iTsyAsyncReqHandle = aTsyReqHandle;
			break;
		default:
			LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyConnectionInfoChange - Error in switch statement"));
			ReqCompleted(aTsyReqHandle, KErrArgument);
			break;
		} // switch
		
    return KErrNone;
	}

TInt CGprsContextDGprsTsy::NotifyConnectionInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
/** 
 * NotifyConnectionInfoChangeCancel method - Cancel active notify connection info request.
 */    
	{
	LOGTEXT(_L8("CGprsContextDGprsTsy::NotifyConnectionInfoChangeCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}	
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	
//
CMbmsContextDMbmsTsy* CMbmsContextDMbmsTsy::NewL(CGprsDGprsTsy *aGprs,CPhoneFactoryDummyBase* aFac)
/**
 * NewL method - Standard 2-phase constructor
 */
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy: NewL() method"));

	CMbmsContextDMbmsTsy* subsession=new(ELeave) CMbmsContextDMbmsTsy(aGprs,aFac);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

CMbmsContextDMbmsTsy::CMbmsContextDMbmsTsy(CGprsDGprsTsy *aGprs,CPhoneFactoryDummyBase* aFac)
	:CGprsContextDGprsTsy(aFac),iGprs(aGprs)
	{
	}

void CMbmsContextDMbmsTsy::ConstructL()
/**
 * ConstructL method
 */
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy: Entered constructor"));
	iMbmsSessionList = new CArrayPtrFlat<CListReadAllAttempt>(1);
	}

CMbmsContextDMbmsTsy::~CMbmsContextDMbmsTsy()
/**
 * Destructor
 */
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy: Entered destructor"));
	if(iMbmsSessionList)
		{
		iMbmsSessionList->ResetAndDestroy();
		delete iMbmsSessionList;
		}
	}

TInt CMbmsContextDMbmsTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
/**
 * ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
 * for the TSY to process. A request handle, request type and request data are passed 
 * to the TSY
 */
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy: ExtFunc() called"));

	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
	case EPacketContextUpdateMbmsSessionList:
		return UpdateMbmsSessionList(aTsyReqHandle,
		reinterpret_cast<TMbmsAction*>(dataPtr),
		reinterpret_cast<TUint*>(dataPtr2));	
	case EPacketGetMbmsSessionListPhase1:
		return GetMbmsSessionsPhase1(aTsyReqHandle, 
		reinterpret_cast<TClientId*>(dataPtr), 
		reinterpret_cast<TUint*>(dataPtr2));
	case EPacketGetMbmsSessionListPhase2:
		return GetMbmsSessionsPhase2(aTsyReqHandle, 
		reinterpret_cast<TClientId*>(dataPtr), aPackage.Des2n());		
	default:
		return CGprsContextDGprsTsy::ExtFunc( aTsyReqHandle,aIpc,aPackage);
		}
	}
CTelObject::TReqMode CMbmsContextDMbmsTsy::ReqModeL(const TInt aIpc)
/**
 * ReqModeL is called from the server's CTelObject::ReqAnalyserL in order to check the 
 * type of request it has. The following are example request types for this dummy TSY
 * All TSYs do not have to have these request types but they have been given
 * "sensible" values in this dummy TSY code.
 */	
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy: ReqModeL() method"));
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
	case EPacketContextUpdateMbmsSessionList:
	case EPacketGetMbmsSessionListPhase1:
	case EPacketGetMbmsSessionListPhase2:
		break;
	default:
		CGprsContextDGprsTsy::ReqModeL(aIpc);
		break;
		}
	return ret;
	}

TInt CMbmsContextDMbmsTsy::CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle)
/**
 * CancelService is called by the server when it is "cleaning-up" any still outstanding
 * asynchronous requests before closing a client's sub-session.
 * This will happen if a client closes its R-class handle without cancelling outstanding
 * asynchronous requests.
 */
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy: - CancelService called"));
	switch (aIpc)
		{
	case EPacketContextUpdateMbmsSessionList:
		return UpdateMbmsSessionListCancel(aTsyReqHandle);		
	case EPacketGetMbmsSessionListPhase1:
	case EPacketGetMbmsSessionListPhase2:
		return GetMbmsSessionsCancel(aTsyReqHandle);
	default:
		return CGprsContextDGprsTsy::CancelService(aIpc, aTsyReqHandle);
		}
	}

CTelObject* CMbmsContextDMbmsTsy::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * Opening object(s) from RPacketContext is not supported.
 */
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy: OpenNewObjectByNameL() method - Not supported"));

	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CMbmsContextDMbmsTsy::OpenNewObjectL(TDes& /*aNewName*/)
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy: OpenNewObjectL() method"));
	return NULL;
	}
	
TInt CMbmsContextDMbmsTsy::UpdateMbmsSessionList(const TTsyReqHandle aTsyReqHandle,TMbmsAction* aAction, TMbmsSessionId* aSessionId)
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy::UpdateMbmsSessionList called"));
	iTsyAsyncReqHandle = aTsyReqHandle;
	switch(*aAction)
		{
	case DPCKTTSY_MBMS_ACTION_ADD:
		if(*aSessionId != DPCKTTSY_MBMS_SESSIONID_ONE)
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			iGprs->AddDelayedReq(aTsyReqHandle, this);
			}
		break;
	case DPCKTTSY_MBMS_ACTION_REMOVE:
		if((*aSessionId != DPCKTTSY_MBMS_SESSIONID_ONE) && (*aSessionId != DPCKTTSY_MBMS_SESSIONID_INVALID))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		if(*aSessionId == DPCKTTSY_MBMS_SESSIONID_INVALID)
			{
			ReqCompleted(aTsyReqHandle, KErrNotFound);
			}
		else
			{
			iGprs->AddDelayedReq(aTsyReqHandle, this);
			}
		break;
	case DPCKTTSY_MBMS_ACTION_REMOVEALL:
		switch(iRemoveAllCount)
			{
		case 0:
			iGprs->AddDelayedReq(aTsyReqHandle, this);
			iRemoveAllCount++;
			break;
		default:
		case 1:
			ReqCompleted(aTsyReqHandle,KErrMbmsImpreciseServiceEntries);
			iRemoveAllCount = 0;
			break;
			}
		break;
		}
	return KErrNone;	
	}
	
TInt CMbmsContextDMbmsTsy::UpdateMbmsSessionListCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy::UpdateMbmsSessionListCancel called"));
	iGprs->RemoveDelayedReq(aTsyReqHandle);
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

TInt CMbmsContextDMbmsTsy::GetMbmsSessionsPhase1(const TTsyReqHandle aTsyReqHandle, TClientId* aClient, TUint* aBufSize)
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy::GetMbmsSessionsPhase1 called"));
	iTsyAsyncReqHandle = aTsyReqHandle;
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetMbmsSessionsPhase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		{
		ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	}	

TInt CMbmsContextDMbmsTsy::ProcessGetMbmsSessionsPhase1L(const TTsyReqHandle aTsyReqHandle, 
													 TClientId* aClient, 
													 TUint* aBufSize)

	{
	// Retrieve MBMS session list,
	// Store the sessions and then return the size of the buffer to the client
	LOGTEXT(_L8("CMbmsContextDMbmsTsy::ProcessGetMbmsSessionsPhase1L called"));	
	RPacketMbmsContext::CMbmsSession* mbmsSession=RPacketMbmsContext::CMbmsSession::NewL();
	CleanupStack::PushL(mbmsSession);
	mbmsSession->iSessionIdList.Append(DPCKTTSY_MBMS_CONTEXT_SESSION_ID_ONE);
	mbmsSession->iSessionIdList.Append(DPCKTTSY_MBMS_CONTEXT_SESSION_ID_TWO);
	mbmsSession->iSessionIdList.Append(DPCKTTSY_MBMS_CONTEXT_SESSION_ID_THREE);
	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	HBufC8* iBuff=NULL;
	mbmsSession->ExternalizeL(iBuff);	
	CleanupStack::PushL(iBuff);
		
	CBufFlat* buf=CBufFlat::NewL(iBuff->Length());
	CleanupStack::PushL(buf);
	buf->InsertL(0,*iBuff);	
	
	read->iListBuf = buf;
	CleanupStack::Pop(buf);	
	CleanupStack::PopAndDestroy(iBuff);
	
	iMbmsSessionList->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	CleanupStack::PopAndDestroy(mbmsSession);
	
	// Complete first phase of list retrieval
	iGprs->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CMbmsContextDMbmsTsy::GetMbmsSessionsPhase2(const TTsyReqHandle aTsyReqHandle,TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy::GetMbmsSessionsPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the get Mbms monitored services from this client
	for (TInt i=0; i<iMbmsSessionList->Count(); ++i)
		{
		read = iMbmsSessionList->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iMbmsSessionList->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	ReqCompleted(aTsyReqHandle,KErrBadHandle);
	return KErrNone;
	}
	
TInt CMbmsContextDMbmsTsy::GetMbmsSessionsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMbmsContextDMbmsTsy::GetPreferredWlanSIDsCancel called"));
	// Remove the MBMS sessions from iMbmsSessionList
	iGprs->RemoveDelayedReq(aTsyReqHandle);
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iMbmsSessionList->Count(); ++i)
		{
		read = iMbmsSessionList->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iMbmsSessionList->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


