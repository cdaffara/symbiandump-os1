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

#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmocksecuritymesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"
/**
 * Factory function
 */
CMockSecurityMessHandler* CMockSecurityMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockSecurityMessHandler* self = new (ELeave)CMockSecurityMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockSecurityMessHandler::CMockSecurityMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {  
    }

/**
 * 2nd phase contructor
 */
void CMockSecurityMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockSecurityMessHandler::~CMockSecurityMessHandler()
    {

    }

TBool CMockSecurityMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{		
		case KMockLtsyDispatchSecurityBootNotifySecurityReadyIndId:
		case KMockLtsyDispatchSecurityNotifySecurityEventIndId:
		case KMockLtsyDispatchSecurityNotifyLockInfoChangeIndId:
		case MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId:
		case MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId:
		case MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId:
		case MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId:
		case MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId:
		case MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId:
		case MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId:
		case KLtsyDispatchSecurityNotifyCipheringInfoChangeApiId:
		case MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId:
		case MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId:
		case MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId:
		case MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId:
		case MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId:			
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockSecurityMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockSecurityMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	case MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId:
    		{    		
    		RMobilePhone::TMobilePhoneSecurityCode secCode = VA_ARG_ENUM(aList, RMobilePhone::TMobilePhoneSecurityCode);
    		RMobilePhone::TCodeAndUnblockCode& twoCodes = *VA_ARG(aList, RMobilePhone::TCodeAndUnblockCode*);    		
    		TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> data(secCode, twoCodes);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}    	 
    	case MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId:
			  {
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
			  }
    	case MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId:
    		{
     		RMobilePhone::TMobilePhoneSecurityCode secCode = VA_ARG_ENUM(aList, RMobilePhone::TMobilePhoneSecurityCode);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode > data(secCode);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId:
    		{
    		RMobilePhone::TMobilePhoneLock lockType = VA_ARG_ENUM(aList, RMobilePhone::TMobilePhoneLock);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneLock > data(lockType);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId:    	
    		{
    		RMobilePhone::TMobilePhoneSecurityCode secCode = VA_ARG_ENUM(aList, RMobilePhone::TMobilePhoneSecurityCode);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode > data(secCode);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);    		
    		}
    	case MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId:
    		{
    		RMobilePhone::TMobilePhoneLock lock = VA_ARG_ENUM(aList, RMobilePhone::TMobilePhoneLock);
    		RMobilePhone::TMobilePhoneLockSetting setting = VA_ARG_ENUM(aList, RMobilePhone::TMobilePhoneLockSetting);
    		TDesC* password = VA_ARG(aList,TDesC*);
    		TMockLtsyData3<RMobilePhone::TMobilePhoneLock,RMobilePhone::TMobilePhoneLockSetting,TDesC*> data(lock,setting,password);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId:
    		{
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);    		
    		}
       	case MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId:
       		{       		
       		DispatcherSecurity::TSecurityCodeId secCodeID = VA_ARG_ENUM(aList, DispatcherSecurity::TSecurityCodeId);
       		RMobilePhone::TMobilePassword& secCode = *VA_ARG(aList, RMobilePhone::TMobilePassword*);       		
      		TMockLtsyData2<DispatcherSecurity::TSecurityCodeId, RMobilePhone::TMobilePassword> data(secCodeID, secCode);
       		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
       		}
    	case MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId:
    		{
    		DispatcherSecurity::TSecurityCodeId secCodeID = VA_ARG_ENUM(aList, DispatcherSecurity::TSecurityCodeId);
     		TMockLtsyData1<DispatcherSecurity::TSecurityCodeId > data(secCodeID);
       		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);     		
    		}
     	case MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId:
     		{
       		RMobilePhone::TMobilePassword& secCode = *VA_ARG(aList, RMobilePhone::TMobilePassword*);       		
      		TMockLtsyData1<RMobilePhone::TMobilePassword> data(secCode);
       		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);    		
     		}
       	case MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId:
       		{
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);    		       		
       		}
     	case MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId:
     		{
     		DispatcherSecurity::TSecurityCodeId secCodeID = VA_ARG_ENUM(aList, DispatcherSecurity::TSecurityCodeId);
     		TMockLtsyData1<DispatcherSecurity::TSecurityCodeId > data(secCodeID);
     		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
     		}
    	case KMockLtsyDispatchSecurityBootNotifySecurityReadyIndId:
    	case KMockLtsyDispatchSecurityNotifySecurityEventIndId:
    	case KMockLtsyDispatchSecurityNotifyLockInfoChangeIndId:
    	case KLtsyDispatchSecurityNotifyCipheringInfoChangeApiId:
    	default:
            {
    		_LIT(KTempPanic, "Here to remind coder to add code to deserialise data otherwise test passes even though CTSY sends down the incorrect data");
    		User::Panic(KTempPanic, KErrGeneral);    		
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	}
    } // CMockSecurityMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockSecurityMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		
		case KMockLtsyDispatchSecurityBootNotifySecurityReadyIndId:
		    {
			iCompletionCallback.CallbackSecurityBootNotifySecurityReadyInd(aResult);
			}
			break;
		case KMockLtsyDispatchSecurityNotifySecurityEventIndId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneSecurityEvent > ltsyData;
		    ltsyData.DeserialiseL(aData);		    		    		    		   		    
			iCompletionCallback.CallbackSecurityNotifySecurityEventInd(aResult, ltsyData.Data1());
			}
			break;
		case KMockLtsyDispatchSecurityNotifyLockInfoChangeIndId:
		    {
		    TMockLtsyData3Buf<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting, RMobilePhone::TMobilePhoneLock > ltsyData;
		    ltsyData.DeserialiseL(aData);		    		    		    		   		    
			iCompletionCallback.CallbackSecurityNotifyLockInfoChangeInd(aResult, ltsyData.Data1(), ltsyData.Data2(), ltsyData.Data3());
			}
			break;
		case MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId:
		    {
		    TMockLtsyData2Buf<RMobilePhone::TMobilePhoneSecurityCode, TInt > ltsyData;
		    ltsyData.DeserialiseL(aData);		    		    		    		   
			iCompletionCallback.CallbackSecurityGetSecurityCodeInfoComp(aResult, ltsyData.Data1(), ltsyData.Data2());
			}
			break;
		case MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId:
		    {
		    TMockLtsyData2Buf<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting > ltsyData;
		    ltsyData.DeserialiseL(aData);
			iCompletionCallback.CallbackSecurityGetLockInfoComp(aResult, ltsyData.Data1(), ltsyData.Data2() );							    
			}
			break;
		case MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId:
		    {
			iCompletionCallback.CallbackSecurityAbortSecurityCodeComp(aResult);
			}
			break;
		case MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneSecurityCode > ltsyData;
		    ltsyData.DeserialiseL(aData);		    		    		    		   		    
			iCompletionCallback.CallbackSecurityGetCurrentActivePinComp(aResult, ltsyData.Data1());
			}
			break;
		case MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId:
		    {
			iCompletionCallback.CallbackSecuritySetLockSettingComp(aResult);
			}
			break;
		case MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId:
		    {
			iCompletionCallback.CallbackSecurityVerifySecurityCodeComp(aResult);
			}
			break;
		case MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId:
			{
			TMockLtsyData1Buf<TBool> status;
			status.DeserialiseL(aData);
			iCompletionCallback.CallbackSecurityGetPin1DisableSupportedComp(aResult, status.Data1());
			}
			break;		
		case KLtsyDispatchSecurityNotifyCipheringInfoChangeApiId:
		    {
			TMockLtsyData1Buf<RMmCustomAPI::TCipheringInfo> info;
			info.DeserialiseL(aData);
			iCompletionCallback.CallbackSecurityNotifyCipheringInfoChangeInd(aResult, info.Data1());
			}
			break;
		case MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId:
		    {
			iCompletionCallback.CallbackSecurityCheckSecurityCodeComp(aResult);
			}
			break;
		case MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId:
		    {
			iCompletionCallback.CallbackSecurityDisablePhoneLockComp(aResult);
			}
			break;
		case MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId:
		    {
			TMockLtsyData1Buf<TBool> indStatus;
			indStatus.DeserialiseL(aData);
			iCompletionCallback.CallbackSecurityGetCipheringInfoComp(aResult, indStatus.Data1());
			}
			break;
		case MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId:
		    {
			TMockLtsyData1Buf<TBool> isBlocked;
			isBlocked.DeserialiseL(aData);
			iCompletionCallback.CallbackSecurityIsSecurityCodeBlockedComp(aResult, isBlocked.Data1());
			}
			break;
		case MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId:
		    {
			iCompletionCallback.CallbackSecurityCheckSecurityCodeCancelComp(aResult);
			}
			break;			
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockSecurityMessHandler::CompleteL
