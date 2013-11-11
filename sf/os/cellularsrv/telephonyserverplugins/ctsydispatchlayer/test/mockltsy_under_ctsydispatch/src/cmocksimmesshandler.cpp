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

#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>

#include "mmockmesshandlerbase.h"
#include "cmocksimmesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"
/**
 * Factory function
 */
CMockSimMessHandler* CMockSimMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockSimMessHandler* self = new (ELeave)CMockSimMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockSimMessHandler::CMockSimMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {  
    }

/**
 * 2nd phase contructor
 */
void CMockSimMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockSimMessHandler::~CMockSimMessHandler()
    {

    }

TBool CMockSimMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		
		case KMockLtsyDispatchSimRefreshSimFilesIndId:
		case KMockLtsyDispatchSimNotifyIccMessageWaitingIndicatorsChangeIndId:
		case KMockLtsyDispatchSimNotifyApnListChangeIndId:
		case KMockLtsyDispatchSimNotifyApnControlListServiceStatusChangeIndId:
		case MLtsyDispatchSimGetApnControlListServiceStatus::KLtsyDispatchSimGetApnControlListServiceStatusApiId:
		case MLtsyDispatchSimDeleteApnName::KLtsyDispatchSimDeleteApnNameApiId:
		case MLtsyDispatchSimEnumerateApnEntries::KLtsyDispatchSimEnumerateApnEntriesApiId:
		case MLtsyDispatchSimChangeSecurityCode::KLtsyDispatchSimChangeSecurityCodeApiId:
		case MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId:
		case MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId:
		case MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId:
		case MLtsyDispatchSimAppendApnName::KLtsyDispatchSimAppendApnNameApiId:
		case MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId:
		case MLtsyDispatchSimSetIccMessageWaitingIndicators::KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId:
		case MLtsyDispatchSimSetApnControlListServiceStatus::KLtsyDispatchSimSetApnControlListServiceStatusApiId:
		case MLtsyDispatchSimGetApnName::KLtsyDispatchSimGetApnNameApiId:
		case MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId:
		case MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId:
		case MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId:
		case KLtsyDispatchSimNotifySimCardStatusApiId:
		case MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId:
		case MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId:
		case MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId:
		case MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId:
		case MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId:
		case MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId:
		case MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId:
		case MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId:
		case MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId:
		case MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId:
		case MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId:
		case MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId:
		case MLtsyDispatchSimSetSimMessageStatusRead::KLtsyDispatchSimSetSimMessageStatusReadApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockSimMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockSimMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	
    	case MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId:
    		{
    		RMobilePhone::TMobilePhoneServiceTable serviceTable = VA_ARG_ENUM(aList,RMobilePhone::TMobilePhoneServiceTable);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> data(serviceTable);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchSimAppendApnName::KLtsyDispatchSimAppendApnNameApiId:
    		{
    		RMobilePhone::TAPNEntryV3& entry = *VA_ARG(aList, RMobilePhone::TAPNEntryV3*);
    		TMockLtsyData1<RMobilePhone::TAPNEntryV3> data(entry);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchSimChangeSecurityCode::KLtsyDispatchSimChangeSecurityCodeApiId:
    		{
    		RMobilePhone::TMobilePhoneSecurityCode secCode = VA_ARG_ENUM(aList, RMobilePhone::TMobilePhoneSecurityCode);
    		RMobilePhone::TMobilePhonePasswordChangeV1& passwds = *VA_ARG(aList, RMobilePhone::TMobilePhonePasswordChangeV1*);
    		
    		TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode,RMobilePhone::TMobilePhonePasswordChangeV1>data(secCode, passwds);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchSimDeleteApnName::KLtsyDispatchSimDeleteApnNameApiId:
    	case MLtsyDispatchSimGetApnName::KLtsyDispatchSimGetApnNameApiId:
    		{
    		TUint32 index = VA_ARG(aList, TUint32);
    		TMockLtsyData1<TUint32> data(index);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchSimSetIccMessageWaitingIndicators::KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId:
    		{
    		RMobilePhone::TMobilePhoneMessageWaitingV1& indicators
    			= *VA_ARG(aList, RMobilePhone::TMobilePhoneMessageWaitingV1*);
    		
    		TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1> data(indicators);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchSimSetApnControlListServiceStatus::KLtsyDispatchSimSetApnControlListServiceStatusApiId:
    		{
    		RMobilePhone::TAPNControlListServiceStatus status = VA_ARG_ENUM(aList,RMobilePhone::TAPNControlListServiceStatus);
    		TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> data(status);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId:
    		{
    		RMobilePhone::TMobilePhoneFdnSetting fdn = VA_ARG_ENUM(aList,RMobilePhone::TMobilePhoneFdnSetting);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneFdnSetting> data(fdn);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	
    	case MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId:
    		{
    		TInt error = VA_ARG(aList,TInt);
    		TMockLtsyData1<TInt> data(error);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId:
    		{
    		TUint8 serviceType  = VA_ARG_ENUM(aList,TUint8);
    		TUint8 cardReaderNumber = VA_ARG_ENUM(aList,TUint8);
    		TUint8 applicationType = VA_ARG_ENUM(aList,TUint8);
    		TDesC8* commandData = VA_ARG(aList, TDesC8*);
    		TMockLtsyData4<TUint8,TUint8,TUint8,TDesC8*> data(serviceType,cardReaderNumber,applicationType,commandData);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId:
    		{
    		TUint8 cardReaderId = VA_ARG_ENUM(aList,TUint8);
    		TDesC8* commandData = VA_ARG(aList, TDesC8*);
    		TMockLtsyData2<TUint8,TDesC8*> data(cardReaderId,commandData);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId:
    		{
    		TDesC8* path =  VA_ARG(aList, TDesC8*);
    		TUint16 offset = VA_ARG_ENUM(aList,TUint16);
    		TUint16 size = VA_ARG_ENUM(aList,TUint16);
    		TMockLtsyData3<TDesC8*,TUint16,TUint16> data(path,offset,size);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSimSetSimMessageStatusRead::KLtsyDispatchSimSetSimMessageStatusReadApiId:
    		{
    		TTime* timestamp = VA_ARG(aList,TTime*);
    		TInt timezoneOffset= VA_ARG(aList,TInt);
    		TMockLtsyData2<TTime,TInt> data(*timestamp,timezoneOffset);
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
			}

    	case MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId:
    	case MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId:
    	case MLtsyDispatchSimGetApnControlListServiceStatus::KLtsyDispatchSimGetApnControlListServiceStatusApiId:
    	case MLtsyDispatchSimEnumerateApnEntries::KLtsyDispatchSimEnumerateApnEntriesApiId:
    	case MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId:
    	case MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId:
    	case MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId:
    	case MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId:
    	case MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId:
    		{
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
   	
    	case MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId:
    	case MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId:
    		{
    		TDesC* password = VA_ARG(aList, TDesC*);
    		RMmCustomAPI::TLockNumber lockNumber = VA_ARG_ENUM(aList, RMmCustomAPI::TLockNumber);
    		TMockLtsyData2<TDesC*, RMmCustomAPI::TLockNumber> data(password, lockNumber);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
		    }
    	case MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId:
    		{
    		TDesC8* answerToReset = VA_ARG(aList, TDesC8*);
    		TMockLtsyData1<TDesC8*> data(answerToReset);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId:
    		{
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId:
    		{
    		TDesC8* randomParameter = VA_ARG(aList, TDesC8*);
    		TInt rfStateInfo = VA_ARG_ENUM(aList, TInt);
    		TMockLtsyData2<TDesC8*, TInt> data(randomParameter, rfStateInfo);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId:
    		{
    		TDesC8* randomParameter = VA_ARG(aList, TDesC8*);
    		TDesC8* autn = VA_ARG(aList, TDesC8*);
    		TInt rfStateInfo = VA_ARG_ENUM(aList, TInt);
    		TMockLtsyData3<TDesC8*, TDesC8*, TInt> data(randomParameter, autn, rfStateInfo);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case KMockLtsyDispatchSimRefreshSimFilesIndId:
    	case KMockLtsyDispatchSimNotifyIccMessageWaitingIndicatorsChangeIndId:
    	case KMockLtsyDispatchSimNotifyApnListChangeIndId:
    	case KMockLtsyDispatchSimNotifyApnControlListServiceStatusChangeIndId:
    	case KLtsyDispatchSimNotifySimCardStatusApiId:
    	default:
    		{
    		_LIT(KTempPanic, "Here to remind coder to add code to deserialise data otherwise test passes even though CTSY sends down the incorrect data");
    		User::Panic(KTempPanic, KErrGeneral);    		
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	}
    } // CMockSimMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockSimMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		
		case KMockLtsyDispatchSimRefreshSimFilesIndId:
		    {
		    TUint16 refreshFileList = 0;
		    TMockLtsyData1<TUint16> refreshFileListData(refreshFileList);
		    refreshFileListData.DeserialiseL(aData);
		    iCompletionCallback.CallbackSimRefreshSimFilesInd(aResult,refreshFileList);
			}
			break;
		case KMockLtsyDispatchSimNotifyIccMessageWaitingIndicatorsChangeIndId:
		    {
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneMessageWaitingV1> indicatorsData;
			indicatorsData.DeserialiseL(aData);
			iCompletionCallback.CallbackSimNotifyIccMessageWaitingIndicatorsChangeInd(aResult, indicatorsData.Data1());
			}
			break;
		case KMockLtsyDispatchSimNotifyApnListChangeIndId:
		    {
			iCompletionCallback.CallbackSimNotifyApnListChangeInd(aResult); 
			}
			break;
		case KMockLtsyDispatchSimNotifyApnControlListServiceStatusChangeIndId:
		    {
			TMockLtsyData1Buf<RMobilePhone::TAPNControlListServiceStatus> status;
			status.DeserialiseL(aData);
			iCompletionCallback.CallbackSimNotifyApnControlListServiceStatusChangeInd(aResult, status.Data1());
			}
			break;
		case MLtsyDispatchSimGetApnControlListServiceStatus::KLtsyDispatchSimGetApnControlListServiceStatusApiId:
		    {
			TMockLtsyData1Buf<RMobilePhone::TAPNControlListServiceStatus> status;
			status.DeserialiseL(aData);
			iCompletionCallback.CallbackSimGetApnControlListServiceStatusComp(aResult, status.Data1());
			}
			break;
		case MLtsyDispatchSimDeleteApnName::KLtsyDispatchSimDeleteApnNameApiId:
		    {
			iCompletionCallback.CallbackSimDeleteApnNameComp(aResult);
			}
			break;
		case MLtsyDispatchSimEnumerateApnEntries::KLtsyDispatchSimEnumerateApnEntriesApiId:
		    {
		    TMockLtsyData1Buf<TUint32> entriesData;
		    entriesData.DeserialiseL(aData);
			iCompletionCallback.CallbackSimEnumerateApnEntriesComp(aResult, entriesData.Data1());
			}
			break;
		case MLtsyDispatchSimChangeSecurityCode::KLtsyDispatchSimChangeSecurityCodeApiId:
		    {
			iCompletionCallback.CallbackSimChangeSecurityCodeComp(aResult);
			}
			break;
		case MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId:
		    {
			iCompletionCallback.CallbackSimSetFdnSettingComp(aResult);
			}
			break;
		case MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneCspFileV1> cspData;
		    cspData.DeserialiseL(aData);
			iCompletionCallback.CallbackSimGetCustomerServiceProfileComp(aResult, cspData.Data1());
			}
			break;
		case MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId:
		    {
		    TMockLtsyData1Buf<TBuf8<RMobilePhone::KIMSISize> > idData;
		    idData.DeserialiseL(aData);
			iCompletionCallback.CallbackSimGetSubscriberIdComp(aResult, idData.Data1());
			}
			break;
		case MLtsyDispatchSimAppendApnName::KLtsyDispatchSimAppendApnNameApiId:
		    {
			iCompletionCallback.CallbackSimAppendApnNameComp(aResult);
			}
			break;
		case MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId:
		    {
		    MLtsyDispatchSimGetActiveIccApplicationType::TIccType iccType;
		    TMockLtsyData1<MLtsyDispatchSimGetActiveIccApplicationType::TIccType> data(iccType);
		    data.DeserialiseL(aData);
			iCompletionCallback.CallbackSimGetActiveIccApplicationTypeComp(aResult, iccType);
			}
			break;
		case MLtsyDispatchSimSetIccMessageWaitingIndicators::KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId:
		    {
			iCompletionCallback.CallbackSimSetIccMessageWaitingIndicatorsComp(aResult);
			}
			break;
		case MLtsyDispatchSimSetApnControlListServiceStatus::KLtsyDispatchSimSetApnControlListServiceStatusApiId:
		    {
			iCompletionCallback.CallbackSimSetApnControlListServiceStatusComp(aResult);
			}
			break;
		case MLtsyDispatchSimGetApnName::KLtsyDispatchSimGetApnNameApiId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TAPNEntryV3> apnData;
		    apnData.DeserialiseL(aData);
			iCompletionCallback.CallbackSimGetApnNameComp(aResult, apnData.Data1());
			}
			break;
		case MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableData;
			serviceTableData.DeserialiseL(aData);
			iCompletionCallback.CallbackSimGetServiceTableComp(aResult, serviceTableData.Data1());
			}
			break;
		case MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneMessageWaitingV1> indicatorsData;
			indicatorsData.DeserialiseL(aData);
			iCompletionCallback.CallbackSimGetIccMessageWaitingIndicatorsComp(aResult, indicatorsData.Data1());
			}
			break;
		case KLtsyDispatchSimNotifySimCardStatusApiId:
		    {
		    TMockLtsyData1Buf<RMmCustomAPI::TSIMCardStatus> cardStatusData;
		    cardStatusData.DeserialiseL(aData);
			iCompletionCallback.CallbackSimNotifySimCardStatusInd(aResult, cardStatusData.Data1());
			}
			break;
		case MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId:
		    {
			iCompletionCallback.CallbackSimSimLockActivateComp(aResult);
			}
			break;
		case MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId:
		    {
			iCompletionCallback.CallbackSimSimLockDeActivateComp(aResult);
			}
			break;
		case MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId:
		    {
		    TDesC8* answerToReset = NULL;
		    TMockLtsyData1<TDesC8*> data(answerToReset);
		    data.DeserialiseL(aData);
		    iCompletionCallback.CallbackSimGetAnswerToResetComp(aResult, *answerToReset);
		    
		    delete answerToReset;
		    answerToReset = NULL;
			}
			break;
		case MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId:
		    {
		    TUint8 simCardReaderStatus;
		    TMockLtsyData1<TUint8> data(simCardReaderStatus);
		    data.DeserialiseL(aData);
		    iCompletionCallback.CallbackSimGetSimCardReaderStatusComp(aResult, simCardReaderStatus);
		    
			}
			break;
    	case MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId:
		    {
	        TDesC8* signedResponse = NULL;
	        TDesC8* cipheringKey = NULL;
	        TMockLtsyData2<TDesC8*, TDesC8*> data(signedResponse, cipheringKey);
	        data.DeserialiseL(aData);
			iCompletionCallback.CallbackSimGetSimAuthenticationDataComp(aResult, *signedResponse, *cipheringKey);
			
		    delete signedResponse;
		    signedResponse = NULL;
		    delete cipheringKey;
		    cipheringKey = NULL;
		    
			}
			break;
    	case MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId:
		    {
		    TDesC8* response = NULL;
		    TDesC8* cipheringKey = NULL;
	        TDesC8* integrityKey = NULL;
	        TDesC8* auts = NULL;
	        TMockLtsyData4<TDesC8*, TDesC8*, TDesC8*, TDesC8*> data(response, cipheringKey, integrityKey, auts);
	        data.DeserialiseL(aData);
			iCompletionCallback.CallbackSimGetSimAuthenticationDataComp(aResult, *response, *cipheringKey, *integrityKey, *auts);
			
		    delete response;
		    response = NULL;
		    delete cipheringKey;
		    cipheringKey = NULL;
		    delete integrityKey;
		    integrityKey = NULL;
		    delete auts;
		    auts = NULL;
		    
			}
			break;
		case MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId:
		    {
			iCompletionCallback.CallbackSimPowerSimOffComp(aResult);
			}
			break;
		case MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId:
		    {
			iCompletionCallback.CallbackSimPowerSimOnComp(aResult);
			}
			break;
		case MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId:
		    {
		    TDesC8* responseBytes = NULL;
		    
		    TMockLtsyData1<TDesC8*> responseData(responseBytes);
		    responseData.DeserialiseL(aData);
		    
			iCompletionCallback.CallbackSimReadSimFileComp(aResult,*responseBytes);
			
			delete responseBytes;
			responseBytes = NULL;
			}
			break;
		case MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId:
		    {
		    TUint8 serviceType = 0;
		    TUint8 cardReaderNumber = 0;
		    TUint8 applicationType = 0;
		   	TDesC8* responseData = NULL;
		   	
		   	TMockLtsyData4<TUint8,TUint8,TUint8,TDesC8*> apduData(serviceType,cardReaderNumber,applicationType,responseData);
		   	apduData.DeserialiseL(aData);
		    
			iCompletionCallback.CallbackSimSendApduRequestComp(aResult,serviceType,cardReaderNumber,applicationType,*responseData);
			
			delete responseData;
			responseData = NULL;
			}
			break;
		case MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId:
			{
		   	TDesC8* responseData = NULL;
						
			TMockLtsyData1<TDesC8*> data(responseData);
			data.DeserialiseL(aData);
			
			iCompletionCallback.CallbackSimSendApduRequestV2Comp(aResult,*responseData);
			
			delete responseData;
			responseData = NULL;
			}
		case MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId:
		    {
			iCompletionCallback.CallbackSimSimWarmResetComp(aResult);
			}
			break;
		case MLtsyDispatchSimSetSimMessageStatusRead::KLtsyDispatchSimSetSimMessageStatusReadApiId:
		    {
			iCompletionCallback.CallbackSimSetSimMessageStatusReadComp(aResult);
			}
			break;
		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockSimMessHandler::CompleteL
