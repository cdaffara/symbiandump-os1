// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cmockcallmesshandler.h"
#include "MmTsy_Csd_Defs.h"
#include "cmmmessagerouter.h"
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include "cmockltsyengine.h"
#include "tmockltsydata.h"

/**
Factory function
*/
CMockCallMessHandler* CMockCallMessHandler::NewL(CMmMessageRouter* aMessageRouter)
    {
    CMockCallMessHandler* self = new(ELeave)CMockCallMessHandler(aMessageRouter);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }

/**
Constructor
*/
CMockCallMessHandler::CMockCallMessHandler(CMmMessageRouter* aMessageRouter)
	: iMessageRouter(aMessageRouter)
    {
    }

/**
2nd phase contructor
*/
void CMockCallMessHandler::ConstructL()
    {
    }


/**
Destructor
*/
CMockCallMessHandler::~CMockCallMessHandler()
    {
    }

/**
Execute a request
*/
TInt CMockCallMessHandler::ExtFuncL(TInt aIpc,const CMmDataPackage* aMmDataPackage)
    {
 	const CCallDataPackage* callDataPackage = static_cast<const CCallDataPackage*>(aMmDataPackage);

	switch (aIpc)
    	{
		case EEtelCallDial:
		case EMobileCallDialNoFdnCheck:
			{
			TInt callId;
			RMobilePhone::TMobileService service;
			callDataPackage->GetCallIdAndMode(callId, service);
			// extract MmData
			//RMobileCall::TCallParams callParams;
			TPckg<RMobileCall::TCallParams>* callParamsPckg;
			RMobileCall::TMobileCallInfoV8* callInfo;
			aMmDataPackage->UnPackData(&callParamsPckg, &callInfo);
			RMobileCall::TCallParams& callParams = (*callParamsPckg)();
			
			if (callParams.ExtensionId() == RMobileCall::KETelMobileCallParamsV1)
				{
				RMobileCall::TMobileCallParamsV1Pckg* callParamsPckg;
				aMmDataPackage->UnPackData(&callParamsPckg, &callInfo);
				TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV8> 
					data(callId, service, (*callParamsPckg)(), *callInfo);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (callParams.ExtensionId() == RMobileCall::KETelMobileCallParamsV2)
				{
				RMobileCall::TMobileCallParamsV2Pckg* callParamsPckg;
				aMmDataPackage->UnPackData(&callParamsPckg, &callInfo);
				TMockLtsyCallData2<RMobileCall::TMobileCallParamsV2, RMobileCall::TMobileCallInfoV8> 
					data(callId, service, (*callParamsPckg)(), *callInfo);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (callParams.ExtensionId() == RMobileCall::KETelMobileCallParamsV7)
				{
				RMobileCall::TMobileCallParamsV7Pckg* callParamsPckg;
				aMmDataPackage->UnPackData(&callParamsPckg, &callInfo);
				TMockLtsyCallData2<RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8> 
					data(callId, service, (*callParamsPckg)(), *callInfo);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (callParams.ExtensionId() == RMobileCall::KETelMobileDataCallParamsV1)
				{
				RMobileCall::TMobileDataCallParamsV1Pckg* callParamsPckg;
				aMmDataPackage->UnPackData(&callParamsPckg, &callInfo);
				TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV1, RMobileCall::TMobileCallInfoV8> 
					data(callId, service, (*callParamsPckg)(), *callInfo);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (callParams.ExtensionId() == RMobileCall::KETelMobileDataCallParamsV2)
				{
				RMobileCall::TMobileDataCallParamsV2Pckg* callParamsPckg2;
				aMmDataPackage->UnPackData(&callParamsPckg2, &callInfo);
				TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV2, RMobileCall::TMobileCallInfoV8> 
					data(callId, service, (*callParamsPckg2)(), *callInfo);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (callParams.ExtensionId() == RMobileCall::KETelMobileDataCallParamsV8)
				{
				RMobileCall::TMobileDataCallParamsV8Pckg* callParamsPckg8;
				aMmDataPackage->UnPackData(&callParamsPckg8, &callInfo);
				TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV8, RMobileCall::TMobileCallInfoV8> 
					data(callId, service, (*callParamsPckg8)(), *callInfo);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (callParams.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV1)
				{
				RMobileCall::TMobileHscsdCallParamsV1Pckg* callParamsPckg;
				aMmDataPackage->UnPackData(&callParamsPckg, &callInfo);
				TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV1, RMobileCall::TMobileCallInfoV8> 
					data(callId, service, (*callParamsPckg)(), *callInfo);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (callParams.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV2)
				{
				RMobileCall::TMobileHscsdCallParamsV2Pckg* callParamsPckg2;
				aMmDataPackage->UnPackData(&callParamsPckg2, &callInfo);
				TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV2, RMobileCall::TMobileCallInfoV8> 
					data(callId, service, (*callParamsPckg2)(), *callInfo);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (callParams.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV7)
				{
				RMobileCall::TMobileHscsdCallParamsV7Pckg* callParamsPckg7;
				aMmDataPackage->UnPackData(&callParamsPckg7, &callInfo);
				TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV7, RMobileCall::TMobileCallInfoV8> 
					data(callId, service, (*callParamsPckg7)(), *callInfo);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
			else if (callParams.ExtensionId() == RMobileCall::KETelMobileHscsdCallParamsV8)
				{
				RMobileCall::TMobileHscsdCallParamsV8Pckg* callParamsPckg8;
				aMmDataPackage->UnPackData(&callParamsPckg8, &callInfo);
				TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV8, RMobileCall::TMobileCallInfoV8> 
					data(callId, service, (*callParamsPckg8)(), *callInfo);
				return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
				}
						
			else        // default
			    {
                TMockLtsyCallData0 data(callId, service);    
                return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
			    }
			}
		
    	case EEtelCallHangUp:
    		{
			TInt callId;
			RMobilePhone::TMobileService service;
			callDataPackage->GetCallIdAndMode(callId, service);
    		TInt hangUpCause;
    		TBool autoStChangeDisable;
			aMmDataPackage->UnPackData(hangUpCause, autoStChangeDisable);
  			TMockLtsyCallData2<TInt, TBool> data(callId, service, hangUpCause, autoStChangeDisable);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
 			}
    	case EMobileCallDialEmergencyCall:
			{
			RMobileENStore::TEmergencyNumber* number;
			aMmDataPackage->UnPackData(&number);
  			TMockLtsyData1<RMobileENStore::TEmergencyNumber> data(*number);
            return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
			}
    	case EEtelCallLoanDataPort:
    	case EEtelCallRecoverDataPort:
			{
			TInt callId;
			RMobilePhone::TMobileService service;
			callDataPackage->GetCallIdAndMode(callId, service);
			RCall::TCommPort* commPort;
			aMmDataPackage->UnPackData(&commPort);
						
			TMockLtsyCallData1<RCall::TCommPort> data(callId, service, *commPort);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
			}
        case EMobileCallSetDynamicHscsdParams:
        	{
			TInt callId;
			RMobilePhone::TMobileService service;
			callDataPackage->GetCallIdAndMode(callId, service);
        	RMobileCall::TMobileHscsdCallParamsV1 hscsdParams;
		   	aMmDataPackage->UnPackData(hscsdParams);
        
        	TMockLtsyCallData1<RMobileCall::TMobileHscsdCallParamsV1> hscsdData(callId, service, hscsdParams);
           	return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, hscsdData);
        	}
    	case EMobileCallAnswerISV:
 		case EMobileCallTransfer:
		case EMobileCallResume:
    	case EMobileCallSwap:
 		case EMobileCallHold:	
        case EEtelCallAnswer:
        case EMobileCallGoOneToOne:
    	case EMobileConferenceCallAddCall:
            {
			TInt callId;
			RMobilePhone::TMobileService service;
			callDataPackage->GetCallIdAndMode(callId, service);
            TMockLtsyCallData0 data(callId, service);    
            return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
            }
    	case EMobileConferenceCallCreateConference:
    	default:
    		{
		    TMockLtsyData0 data;
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	}
    }

/**
Complete a request
*/
void CMockCallMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	CCallDataPackage dataPackage;
	switch (aIpc)
		{
        case EEtelLineNotifyIncomingCall:
		case EEtelLineNotifyCallAdded:
			{
			TMockLtsyCallData1Buf<RMobileCall::TMobileCallInfoV1> callInfoData;
			callInfoData.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(callInfoData.CallId(),callInfoData.CallMode());
			dataPackage.PackData(callInfoData.Data1Ptr());
			}
		break;
		
		case EEtelCallNotifyStatusChange:
		    {
			TMockLtsyCallData1Buf<RCall::TStatus> callStatusData;
			callStatusData.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(callStatusData.CallId(),callStatusData.CallMode());
			dataPackage.PackData(callStatusData.Data1Ptr());
		    }
		break;
		    
		case EMobileCallNotifyMobileCallStatusChange:
			{
			TMockLtsyCallData1Buf<RMobileCall::TMobileCallStatus> callStatusData;
			callStatusData.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(callStatusData.CallId(),callStatusData.CallMode());
			dataPackage.PackData(callStatusData.Data1Ptr());
			}
		break;
		
		case EMobileCallNotifyCallEvent:
		    {
			TMockLtsyCallData1Buf<RMobileCall::TMobileCallEvent> callEv;
			callEv.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(callEv.CallId(),callEv.CallMode());
			dataPackage.PackData(callEv.Data1Ptr());
		    }
		    break;
		    
		case EMobileCallGetMobileCallInfo:
		    {
			TMockLtsyCallData1Buf<RMobileCall::TMobileCallInfoV1> data;            
			data.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(data.CallId(), data.CallMode());
			dataPackage.PackData(data.Data1Ptr());
		    }
		    break;
		case EEtelCallGetBearerServiceInfo:
			{
			TMockLtsyCallData1Buf<RCall::TBearerService> bearerData;
			bearerData.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(bearerData.CallId(), bearerData.CallMode());
			dataPackage.PackData(bearerData.Data1Ptr());
			break;	
			}
		case EMobileCallNotifyHscsdInfoChange:
			{
			TMockLtsyCallData2Buf<RMobileCall::TMobileCallHscsdInfoV8, RMobileCall::TMobileCallDataCapsV1> hscsdData;
			hscsdData.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(hscsdData.CallId(), hscsdData.CallMode());
			dataPackage.PackData(hscsdData.Data1Ptr(), hscsdData.Data2Ptr());
			break;
			}
		case EMobileCallNotifyMobileDataCallCapsChange:
			{
			TMockLtsyCallData1Buf<RMobileCall::TMobileCallDataCapsV1> capsData;
			capsData.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(capsData.CallId(), capsData.CallMode());
			dataPackage.PackData(capsData.Data1Ptr());
			break;
			}
		case EMobileCallNotifyPrivacyConfirmation:
			{
			TMockLtsyCallData1Buf<RMobilePhone::TMobilePhonePrivacy> privacyData;
			privacyData.DeserialiseL(aData);
			dataPackage.PackData(privacyData.Data1Ptr());
			}
			break;
		case EMobileConferenceCallNotifyConferenceEvent:
			{
			TMockLtsyCallData1Buf<TInt> ghostOperationData;
			ghostOperationData.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(ghostOperationData.CallId(), ghostOperationData.CallMode());
			dataPackage.PackData(ghostOperationData.Data1Ptr());
			break;
			}
		case EMobileCallReceiveUUI:
			{
			TMockLtsyCallData1Buf<RMobileCall::TMobileCallUUI> receiveUUI;
			receiveUUI.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(receiveUUI.CallId(), receiveUUI.CallMode());
			dataPackage.PackData(receiveUUI.Data1Ptr());
			break;
			}
		// no data params
		case EMobileCallDialISV:
		case EMobileCallAnswerISV:
 		case EMobileCallTransfer:
		case EMobileCallResume:
		case EMobileCallSwap:
 		case EMobileCallHold:	
		case EEtelCallAnswer:
		case EEtelCallDial:
		case EEtelCallHangUp:
		case EMobileCallDialEmergencyCall:
		case EMobileCallGoOneToOne:
		case EMobileCallDialNoFdnCheck:
		case EMobileCallSetDynamicHscsdParams:
			{
			TMockLtsyCallData0 callStatusData;
			callStatusData.DeserialiseL(aData);
			dataPackage.SetCallIdAndMode(callStatusData.CallId(),callStatusData.CallMode());
			}
			break;
		case EMobileConferenceCallCreateConference:
		case EMobileConferenceCallAddCall:
		case EMobileConferenceCallNotifyConferenceStatusChange:
		default:
			break;
		}
	iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
	}


/**
Log utils
*/
void CMockCallMessHandler::LogErrorData(TInt aIpc, const MMockSyEngineData& aData, const TDesC8& aExpectedData)
	{
	switch (aIpc)
    	{
		case EEtelCallDial:
			{
			LogEtelCallDialErrorData(aData,aExpectedData);
			}
			break;
    	default:
    		{
    		}
    	}
    	
	}

/**
Log EEtelCallDial ipc data
*/
void CMockCallMessHandler::LogEtelCallDialErrorData(const MMockSyEngineData& aData, const TDesC8& aExpectedData)
	{
	RMobileCall::TMobileCallParamsV1 expectedCallMode;
	RMobileCall::TMobileCallInfoV1 expectedCallInfo;
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> expectedData(expectedCallMode, expectedCallInfo);
	TRAPD(err, expectedData.DeserialiseL(aExpectedData));
	
	const TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1>& receivedData = 
		static_cast<const TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> &>(aData);
		
	TBuf<KMaxLogLineSize> logBuffer;
	if (err == KErrNone)
		{
		logBuffer.Format(_L("Received id=%d mode=%d, Expected id=%d mode=%d, callParam OK=%d callInfo OK=%d"), 
			receivedData.CallId(), receivedData.CallMode(), expectedData.CallId(), expectedData.CallMode(), 
			TComparator<RMobileCall::TMobileCallParamsV1>::IsEqual(expectedData.Data1(), receivedData.Data1()),
			TComparator<RMobileCall::TMobileCallInfoV1>::IsEqual(expectedData.Data2(), receivedData.Data2()));
		iMessageRouter->MockLtsyEngine()->LogData(logBuffer);
		}
	}
