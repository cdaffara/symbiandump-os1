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

#include <ctsy/ltsy/mltsydispatchsupplementaryservicesinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmocksupplementaryservicesmesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"
/**
 * Factory function
 */
CMockSupplementaryServicesMessHandler* CMockSupplementaryServicesMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockSupplementaryServicesMessHandler* self = new (ELeave)CMockSupplementaryServicesMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockSupplementaryServicesMessHandler::CMockSupplementaryServicesMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {  
    }

/**
 * 2nd phase contructor
 */
void CMockSupplementaryServicesMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockSupplementaryServicesMessHandler::~CMockSupplementaryServicesMessHandler()
    {

    }

TBool CMockSupplementaryServicesMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		
		case KMockLtsyDispatchSupplementaryServicesNotifyCallBarringStatusChangeIndId:
		case KMockLtsyDispatchSupplementaryServicesReceiveUssdMessageIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyCallWaitingStatusChangeIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyCallForwardingStatusChangeIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyCallForwardingActiveIndId:
		case KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventForwardModeIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventCallWaitingIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventHoldModeIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventConfrenceIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventCugIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventClirSuppressionIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventEctCallStateIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyAdditionalInfoIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyRequestCompleteIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifySendNetworkServiceRequestIndId:
		case MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId:
		case MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId:
		case MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId:
		case MLtsyDispatchSupplementaryServicesGetCallBarringStatus::KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId:
		case MLtsyDispatchSupplementaryServicesSetCallBarringStatus::KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId:
		case MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId:
		case MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId:
		case MLtsyDispatchSupplementaryServicesSetSsPassword::KLtsyDispatchSupplementaryServicesSetSsPasswordApiId:
		case MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId:
		case MLtsyDispatchSupplementaryServicesSendUssdMessage::KLtsyDispatchSupplementaryServicesSendUssdMessageApiId:
		case MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId:
		case MLtsyDispatchSupplementaryServicesSendUssdRelease::KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId:
		case MLtsyDispatchSupplementaryServicesClearBlacklist::KLtsyDispatchSupplementaryServicesClearBlacklistApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockSupplementaryServicesMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockSupplementaryServicesMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	
    	
    	case MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId:
    		{
    		RMobilePhone::TMobilePhoneCFCondition condition = VA_ARG_ENUM(aList,RMobilePhone::TMobilePhoneCFCondition);
    		RMobilePhone::TMobileService serviceGroup = VA_ARG_ENUM(aList,RMobilePhone::TMobileService);
    		RMobilePhone::TMobilePhoneServiceAction action = VA_ARG_ENUM(aList,RMobilePhone::TMobilePhoneServiceAction);
    		TBuf<RMobilePhone::KMaxMobileTelNumberSize> telNumber = *VA_ARG(aList,TDesC*);
    		TInt timeout = VA_ARG(aList,TInt);
    		
    		TMockLtsyData5<RMobilePhone::TMobilePhoneCFCondition, 
    		               RMobilePhone::TMobileService,
    		               RMobilePhone::TMobilePhoneServiceAction,
    		               TInt,
    		               TBuf<RMobilePhone::KMaxMobileTelNumberSize> > data(condition,
    		            		   serviceGroup,
    		            		   action,
    		            		   timeout,
    		            		   telNumber);
		    
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId:
    		{
    		RMobilePhone::TMobilePhoneCFCondition condition = VA_ARG_ENUM(aList,RMobilePhone::TMobilePhoneCFCondition);
    		RMobilePhone::TMobileService service = VA_ARG_ENUM(aList,RMobilePhone::TMobileService);
    		
		    TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition,RMobilePhone::TMobileService> data(condition,service);
		    
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId:
    	case MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId:
    		{
    		TDesC *request 
    			= VA_ARG(aList,TDesC*);
    		TName serviceStringBuf(*request);
		    TMockLtsyData1<TName> data(serviceStringBuf);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSupplementaryServicesSetSsPassword::KLtsyDispatchSupplementaryServicesSetSsPasswordApiId:
    		{
    		RMobilePhone::TMobilePhonePasswordChangeV2 password;
    		TDesC *oldPassword 
    			= VA_ARG(aList,TDesC*);
    		TDesC *newPassword 
    			= VA_ARG(aList,TDesC*);
		    password.iOldPassword = *oldPassword;
    		password.iNewPassword = *newPassword;
    		password.iVerifiedPassword = password.iNewPassword;
    		MLtsyDispatchSupplementaryServicesSetSsPassword::TServiceType service = VA_ARG_ENUM(aList,MLtsyDispatchSupplementaryServicesSetSsPassword::TServiceType);
    		TUint16 uintService = (TUint16) service;
		    TMockLtsyData2<RMobilePhone::TMobilePhonePasswordChangeV2,TUint16> data(password,uintService);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSupplementaryServicesSendUssdMessage::KLtsyDispatchSupplementaryServicesSendUssdMessageApiId:
    	case MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId:
    		{
    		RMobileUssdMessaging::TGsmUssdMessageData msg = *VA_ARG(aList,TDesC8 *);
    		RMobileUssdMessaging::TMobileUssdAttributesV1 attr;
    		attr.iDcs = VA_ARG_ENUM(aList,TUint8);
    		attr.iFormat = VA_ARG_ENUM(aList,RMobileUssdMessaging::TMobileUssdDataFormat);
    		attr.iType = VA_ARG_ENUM(aList,RMobileUssdMessaging::TMobileUssdMessageType);
    		attr.iFlags = RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageType | RMobileUssdMessaging::KUssdMessageDcs;
		    TMockLtsyData2<RMobileUssdMessaging::TGsmUssdMessageData,RMobileUssdMessaging::TMobileUssdAttributesV1> data(msg,attr);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSupplementaryServicesSendUssdRelease::KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId:
    		{
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSupplementaryServicesClearBlacklist::KLtsyDispatchSupplementaryServicesClearBlacklistApiId:
    		{
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSupplementaryServicesGetCallBarringStatus::KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId:
           {
           RMobilePhone::TMobilePhoneCBCondition condition = VA_ARG_ENUM(aList,RMobilePhone::TMobilePhoneCBCondition);
           TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> data(condition);
           return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
           }
    	case MLtsyDispatchSupplementaryServicesSetCallBarringStatus::KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId:
          {
          RMobilePhone::TMobilePhoneCBCondition condition = VA_ARG_ENUM(aList,RMobilePhone::TMobilePhoneCBCondition);
          RMobilePhone::TMobilePhoneCBChangeV1 info;
          info.iAction = VA_ARG_ENUM(aList,RMobilePhone::TMobilePhoneServiceAction);
          info.iServiceGroup = VA_ARG_ENUM(aList,RMobilePhone::TMobileService);
          TDesC *password = VA_ARG(aList,TDesC*);
		  info.iPassword = *password;
          TMockLtsyData2<RMobilePhone::TMobilePhoneCBCondition,RMobilePhone::TMobilePhoneCBChangeV1> data(condition,info);
		  return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
          }
    	case MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId:
        {
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
        }
    	case MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId:
          {
          RMobilePhone::TMobileService service = VA_ARG_ENUM(aList,RMobilePhone::TMobileService);
          RMobilePhone::TMobilePhoneServiceAction action = VA_ARG_ENUM(aList,RMobilePhone::TMobilePhoneServiceAction);
          TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneServiceAction> data(service, action);
		  return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
          }
    	case KMockLtsyDispatchSupplementaryServicesNotifyCallBarringStatusChangeIndId:
    	case KMockLtsyDispatchSupplementaryServicesReceiveUssdMessageIndId:
    	case KMockLtsyDispatchSupplementaryServicesNotifyCallWaitingStatusChangeIndId:
    	case KMockLtsyDispatchSupplementaryServicesNotifyCallForwardingStatusChangeIndId:
    	case KMockLtsyDispatchSupplementaryServicesNotifyCallForwardingActiveIndId:
        case KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventForwardModeIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventCallWaitingIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventHoldModeIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventConfrenceIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventCugIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventClirSuppressionIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventEctCallStateIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyAdditionalInfoIndId:
		case KMockLtsyDispatchSupplementaryServicesNotifyRequestCompleteIndId:
    	case KMockLtsyDispatchSupplementaryServicesNotifySendNetworkServiceRequestIndId:
    	default:
    		{
    		_LIT(KTempPanic, "Here to remind coder to add code to deserialise data otherwise test passes even though CTSY sends down the incorrect data");
    		User::Panic(KTempPanic, KErrGeneral);
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	}
    } // CMockSupplementaryServicesMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockSupplementaryServicesMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		
		case KMockLtsyDispatchSupplementaryServicesNotifyCallBarringStatusChangeIndId:
		    {
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneCBCondition> condition;
			condition.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyCallBarringStatusChangeInd(aResult,condition.Data1());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesReceiveUssdMessageIndId:
		    {
		    TDesC8* message = NULL;
		    RMobileUssdMessaging::TMobileUssdAttributesV1 ussdAttributes;
		    TMockLtsyData2<TDesC8*, RMobileUssdMessaging::TMobileUssdAttributesV1> data(message,ussdAttributes); 
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesReceiveUssdMessageInd(aResult, *message,
					ussdAttributes.iDcs,ussdAttributes.iType,ussdAttributes.iFormat);
			
			delete message;
			message = NULL;
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyCallWaitingStatusChangeIndId:
		    {
			TMockLtsyData2Buf<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneCWStatus> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyCallWaitingStatusChangeInd(aResult, info.Data1(), info.Data2());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyCallForwardingStatusChangeIndId:
		    {
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneCFCondition> condition;
			condition.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyCallForwardingStatusChangeInd(aResult,condition.Data1());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyCallForwardingActiveIndId:
		    {
		    TMockLtsyData2Buf<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneCFActive > callForwarding;
		    callForwarding.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyCallForwardingActiveInd(aResult,callForwarding.Data1(),callForwarding.Data2());
			}
			break;
		case MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId:
		    {
			iCompletionCallback.CallbackSupplementaryServicesSendNetworkServiceRequestNoFdnCheckComp(aResult);
			}
			break;
		case MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId:
		    {
		    CMobilePhoneCWList* cwList = CMobilePhoneCWList::NewL();
		    CleanupStack::PushL(cwList);
		    cwList->RestoreL(aData);
			iCompletionCallback.CallbackSupplementaryServicesGetCallWaitingStatusComp(aResult, *cwList);
			CleanupStack::PopAndDestroy(cwList);
			}
			break;
		case MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId:
		    {
			iCompletionCallback.CallbackSupplementaryServicesSendNetworkServiceRequestComp(aResult);
			}
			break;
		case MLtsyDispatchSupplementaryServicesGetCallBarringStatus::KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId:
		    {
		    CMobilePhoneCBList* cbList = CMobilePhoneCBList::NewL();
		    CleanupStack::PushL(cbList);
		    cbList->RestoreL(aData);
			iCompletionCallback.CallbackSupplementaryServicesGetCallBarringStatusComp(aResult, *cbList);
			CleanupStack::PopAndDestroy(cbList);
			}
			break;
		case MLtsyDispatchSupplementaryServicesSetCallBarringStatus::KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId:
		    {
			iCompletionCallback.CallbackSupplementaryServicesSetCallBarringStatusComp(aResult);
			}
			break;
		case MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId:
		    {
			iCompletionCallback.CallbackSupplementaryServicesSetCallForwardingStatusComp(aResult);
			}
			break;
		case MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId:
		    {
			iCompletionCallback.CallbackSupplementaryServicesSetCallWaitingStatusComp(aResult);
			}
			break;
		case MLtsyDispatchSupplementaryServicesSetSsPassword::KLtsyDispatchSupplementaryServicesSetSsPasswordApiId:
		    {
			iCompletionCallback.CallbackSupplementaryServicesSetSsPasswordComp(aResult);
			}
			break;
		case MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId:
		    {
		    CMobilePhoneCFList* cfList = CMobilePhoneCFList::NewL();
		    CleanupStack::PushL(cfList);
		    cfList->RestoreL(aData);
			iCompletionCallback.CallbackSupplementaryServicesGetCallForwardingStatusComp(aResult, *cfList);
			CleanupStack::PopAndDestroy(cfList);
			}
			break;
		case MLtsyDispatchSupplementaryServicesSendUssdMessage::KLtsyDispatchSupplementaryServicesSendUssdMessageApiId:
		    {
			iCompletionCallback.CallbackSupplementaryServicesSendUssdMessageComp(aResult);
			}
			break;
		case MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId:
		    {
			iCompletionCallback.CallbackSupplementaryServicesSendUssdMessageNoFdnCheckComp(aResult);
			}
			break;
		case MLtsyDispatchSupplementaryServicesSendUssdRelease::KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId:
		    {
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneSendSSRequestV3> request;
			request.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesSendUssdReleaseComp(aResult,
					request.Data1().iOpCode,request.Data1().iAdditionalInfo);
			}
			break;
		case MLtsyDispatchSupplementaryServicesClearBlacklist::KLtsyDispatchSupplementaryServicesClearBlacklistApiId:
		    {
			iCompletionCallback.CallbackSupplementaryServicesClearBlacklistComp(aResult);
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId:
		    {
			TMockLtsyData2Buf<TInt, TUint8> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesGetDiagnosticOctetsInd(aResult, info.Data1(), info.Data2());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyAdditionalInfoIndId:
		    {
			TMockLtsyData1Buf<RMmCustomAPI::TSsAdditionalInfo> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyAdditionalInfoInd(aResult, info.Data1().iOperationCode, info.Data1().iAdditionalInfo);
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyRequestCompleteIndId:
		    {
			TMockLtsyData1Buf<TInt> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyRequestCompleteInd(aResult, info.Data1());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventForwardModeIndId:
		    {
			TMockLtsyData2Buf<RMmCustomAPI::TSsTypeAndMode, RMmCustomAPI::TSsForwMode> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyNetworkEventForwardModeInd(aResult, info.Data1().iSsType, 
					info.Data1().iSsMode, info.Data2());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventCallWaitingIndId:
		    {
			TMockLtsyData2Buf<RMmCustomAPI::TSsMode, TBool> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyNetworkEventCallWaitingInd(aResult,
					info.Data1(), info.Data2());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventHoldModeIndId:
		    {
			TMockLtsyData2Buf<RMmCustomAPI::TSsTypeAndMode, RMmCustomAPI::TSsHoldMode> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyNetworkEventHoldModeInd(aResult, info.Data1().iSsType, 
					info.Data1().iSsMode, info.Data2());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventConfrenceIndId:
		    {
			TMockLtsyData2Buf<RMmCustomAPI::TSsTypeAndMode, TBool> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyNetworkEventConfrenceInd(aResult, info.Data1().iSsType, 
					info.Data1().iSsMode, info.Data2());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventCugIndId:
		    {
			TMockLtsyData2Buf<RMmCustomAPI::TSsTypeAndMode, TUint16> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyNetworkEventCugInd(aResult, info.Data1().iSsType, 
					info.Data1().iSsMode, info.Data2());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventClirSuppressionIndId:
		    {
			TMockLtsyData2Buf<RMmCustomAPI::TSsMode, TBool> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyNetworkEventClirSuppressionInd(aResult, info.Data1(), 
					info.Data2());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventEctCallStateIndId:
		    {
			TMockLtsyData4Buf<RMmCustomAPI::TSsTypeAndMode, RMmCustomAPI::TSsEctState, RMmCustomAPI::TSsChoice, TBuf<RMmCustomAPI::KRemoteAddressSize> > info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyNetworkEventEctCallStateInd(aResult, info.Data1().iSsType, 
					info.Data1().iSsMode, info.Data2(), info.Data3(), info.Data4());
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventIndId:
		    {
			TMockLtsyData1Buf<RMmCustomAPI::TSsTypeAndMode> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifyNetworkEventInd(aResult, info.Data1().iSsType,
					info.Data1().iSsMode);
			}
			break;
		case KMockLtsyDispatchSupplementaryServicesNotifySendNetworkServiceRequestIndId:
			{
			RMobilePhone::TMobilePhoneNotifySendSSOperation operationCode; 
			TDesC* additionalInfo = NULL;
			TMockLtsyData2<RMobilePhone::TMobilePhoneNotifySendSSOperation, TDesC*> data(operationCode,additionalInfo);
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackSupplementaryServicesNotifySendNetworkServiceRequestInd(aResult,operationCode,*additionalInfo);
			
			delete additionalInfo;
			additionalInfo = NULL;
			}
			break;
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockSupplementaryServicesMessHandler::CompleteL
