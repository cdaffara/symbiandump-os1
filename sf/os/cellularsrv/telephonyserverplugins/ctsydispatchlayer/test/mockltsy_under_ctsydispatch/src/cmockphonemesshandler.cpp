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

#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmlist.h>
#include <etelmm.h>
#include <ctsy/serviceapi/cmmutility.h>

#include "mmockmesshandlerbase.h"
#include "cmockphonemesshandler.h"
#include "cmockltsyengine.h"
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "mockltsyindicatorids.h"

/**
 * Factory function
 */
CMockPhoneMessHandler* CMockPhoneMessHandler::NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback )
    {
    CMockPhoneMessHandler* self = new (ELeave)CMockPhoneMessHandler(aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Constructor
 */
CMockPhoneMessHandler::CMockPhoneMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback)
 : iMockLtsyEngine(aEngine), iCompletionCallback(aCallback)
    {  
    }

/**
 * 2nd phase contructor
 */
void CMockPhoneMessHandler::ConstructL()
    {
    }

/**
 * Destructor
 */
CMockPhoneMessHandler::~CMockPhoneMessHandler()
    {

    }

TBool CMockPhoneMessHandler::IsHandlerForApi(TInt aApiId)
	{
	switch (aApiId)
		{
		case MLtsyDispatchPhoneBootNotifyModemStatusReady::KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId:
		case MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId:
		case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId:
		case MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId:
		case KMockLtsyDispatchPhoneNotifyNetworkModeChangeIndId:
		case KMockLtsyDispatchPhoneNotifyEgprsInfoChangeIndId:
		case KMockLtsyDispatchPhoneNotifySignalStrengthChangeIndId:
		case KMockLtsyDispatchPhoneNotifyNetworkRegistrationStatusChangeIndId:
		case KMockLtsyDispatchPhoneNotifyNitzInfoChangeIndId:
		case KMockLtsyDispatchPhoneNotifyBatteryInfoChangeIndId:
		case KMockLtsyDispatchPhoneNotifyCurrentNetworkChangeIndId:
		case KMockLtsyDispatchPhoneNotifyRfStatusChangeIndId:
		case KMockLtsyDispatchPhoneNotifyNspsStatusChangeIndId:
		case KMockLtsyDispatchPhoneNotifyCellInfoChangeIndId:
		case MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId:
		case MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId:
		case MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId:
		case MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId:
		case MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId:
		case MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId:
		case MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId:
		case MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId:
		case MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId:
		case MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId:
		case MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId:
		case MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId:
		case MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId:
		case MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId:
		case KMockLtsyDispatchPhoneNotifyNetworkSelectionSettingChangeIndId:
		case MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId:
		case MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId:
		case MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId:
		case MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId:
		case MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId:
		case MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId:
		case KMockLtsyDispatchPhoneNotifyRauEventIndId:
		case KMockLtsyDispatchPhoneNotifyNetworkConnectionFailureIndId:
		case KMockLtsyDispatchCustomPhoneNotifyDtmfEventIndId:
		case KMockLtsyDispatchPhoneNotifyDtmfEventIndId:
		case MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId:
		case MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId:
		case KMockLtsyDispatchPhoneNotifyHspaStatusChangedIndId:
		case MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId:
		case MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId:
		case MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId:
		case KMockLtsyDispatchPhoneNotifyCellInfoChangedIndId:
		case MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId:
		case MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId:
		case MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId:
		case MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId:
		case MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId:
		case MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId:
		case MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId:
			return iConfig.IsSupported(aApiId);
		default:
			return EFalse;
		}
	} // CMockPhoneMessHandler::IsHandlerForApi

/**
 * Execute a request.
 */
TInt CMockPhoneMessHandler::ExtFuncL(TInt aInterfaceId, VA_LIST& aList)
    {
    switch (aInterfaceId)
    	{
    	
    	case MLtsyDispatchPhoneBootNotifyModemStatusReady::KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId:
    		{
    		// This is the first requst in the TSY boot sequence.
			// It is triggered very early in the loading of the TSY and is called
			// before the test code has created a session with MockSY server and
			// populated the expect queue.
    		// This means if we post this message to the server now it will return KErrCorrupt.
    		// Instead we deliberately do nothing and await a completion that is triggered when the
    		// the test code issues its first
			// iMockLTSY.CompleteL(KLtsyDispatchPhoneBootNotifyModemStatusReadyId,KErrNone);
    		return KErrNone; 
    		}

    	case MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId:
    	case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId:
    	case MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId:
		case MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId:
		case MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId:
    	case MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId:
     	case MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId:
    	case MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId:
		case MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId:
		case MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId:
		case MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId:
		case MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId:
		case MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId:
		case MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId:
        case MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId:
        case MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId:
        case MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId:
        case MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId:
        case MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId:
        case MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId:
        case MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId:
		case MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId:
		case MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId:
		case MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId:
		case MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId:
			{
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
		
    	case MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId:
    		{
    		TBool isManual = VA_ARG(aList,TBool);
    		
    		RMobilePhone::TMobilePhoneNetworkManualSelection& networkManualSelection 
    			= *VA_ARG(aList,RMobilePhone::TMobilePhoneNetworkManualSelection*);
    		
		    TMockLtsyData2<TBool,RMobilePhone::TMobilePhoneNetworkManualSelection> data(isManual,networkManualSelection);
		    
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}

    	case MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId:
    		{
    		RMobilePhone::TMobilePhoneNetworkSelectionV1& phoneNetworkSelection 
    			= *VA_ARG(aList,RMobilePhone::TMobilePhoneNetworkSelectionV1*);
 		
    		TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> data(phoneNetworkSelection);

    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
		
    	case MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId:
    		{
    		TUint16 filesToRegister = VA_ARG_ENUM(aList,TUint16);
    		
    		TMockLtsyData1<TUint16> data(filesToRegister);
    		
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
      		
    	case MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId:
    		{
    		RMmCustomAPI::TNetworkModeCaps mode = VA_ARG_ENUM(aList,RMmCustomAPI::TNetworkModeCaps);
    		
    		TMockLtsyData1<RMmCustomAPI::TNetworkModeCaps> data(mode);
    		
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
      		
    	case MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId:
    		{
    		RMmCustomAPI::TSetAlwaysOnMode mode = VA_ARG_ENUM(aList,RMmCustomAPI::TSetAlwaysOnMode);
    		
    		TMockLtsyData1<RMmCustomAPI::TSetAlwaysOnMode> data(mode);
    		
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
      		
    	case MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId:
    		{
    		RMmCustomAPI::TSetDriveMode mode = VA_ARG_ENUM(aList,RMmCustomAPI::TSetDriveMode);
    		
    		TMockLtsyData1<RMmCustomAPI::TSetDriveMode> data(mode);
    		
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
      		
    	case MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId:
    		{
    		RMmCustomAPI::THSxPAStatus status = VA_ARG_ENUM(aList,RMmCustomAPI::THSxPAStatus);
    		
    		TMockLtsyData1<RMmCustomAPI::THSxPAStatus> data(status);
    		
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    		
    	case MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId:
    		{
    		TInt applicationNumber = VA_ARG(aList,TInt);
    		
    		TMockLtsyData1<TInt> data(applicationNumber);
    		
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	
    	case MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId:
    		{
    		TInt callId = VA_ARG(aList,TInt);
    		TMockLtsyData1<TInt> data(callId);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	case MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId:
    		{
    		RMmCustomAPI::TBandSelection band = VA_ARG_ENUM(aList, RMmCustomAPI::TBandSelection);
    		RMmCustomAPI::TNetworkModeCaps mode = VA_ARG_ENUM(aList, RMmCustomAPI::TNetworkModeCaps);
    		TMockLtsyData2<RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> data(band, mode);
    		return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
      		
    	case KMockLtsyDispatchPhoneNotifyNetworkModeChangeIndId:
    	case KMockLtsyDispatchPhoneNotifyEgprsInfoChangeIndId:
    	case KMockLtsyDispatchPhoneNotifySignalStrengthChangeIndId:
    	case KMockLtsyDispatchPhoneNotifyNetworkRegistrationStatusChangeIndId:
    	case KMockLtsyDispatchPhoneNotifyNitzInfoChangeIndId:
    	case KMockLtsyDispatchPhoneNotifyBatteryInfoChangeIndId:
    	case KMockLtsyDispatchPhoneNotifyCurrentNetworkChangeIndId:
    	case KMockLtsyDispatchPhoneNotifyNetworkSelectionSettingChangeIndId:
    	case KMockLtsyDispatchPhoneNotifyCellInfoChangeIndId:
    	default:
    		{
    		_LIT(KTempPanic, "Here to remind coder to add code to deserialise data otherwise test passes even though CTSY sends down the incorrect data");
    		User::Panic(KTempPanic,KErrGeneral);    		
		    TMockLtsyData0 data;
		    return iMockLtsyEngine.ExecuteCommandL(aInterfaceId, data);
    		}
    	}
    } // CMockPhoneMessHandler::ExtFuncL

/**
 * Complete a request
 */
void CMockPhoneMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	switch(aIpc)
		{
		case MLtsyDispatchPhoneBootNotifyModemStatusReady::KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId:
			{
			TMockLtsyData1Buf<TRfStateInfo> status;
			status.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneBootNotifyModemStatusReadyComp(aResult, status.Data1());
			}
			break;
		case MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneRegistrationStatus> status;
			status.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetNetworkRegistrationStatusComp(aResult, status.Data1());
			}
			break;
		case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId:
			{
			iCompletionCallback.CallbackPhoneBootNotifySimStatusReadyComp(aResult);
			}
			break;
		case MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkInfoV5> status;
			status.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetHomeNetworkComp(aResult, status.Data1());
			}
			break;
		case KMockLtsyDispatchPhoneNotifyNetworkModeChangeIndId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkMode> mode;
		    mode.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyNetworkModeChangeInd(aResult, mode.Data1());
			}
			break;
		case KMockLtsyDispatchPhoneNotifyEgprsInfoChangeIndId:
		    {
		    TMockLtsyData1Buf<TBool> mode;
		    mode.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyEgprsInfoChangeInd(aResult, mode.Data1());
			}
			break;
		case KMockLtsyDispatchPhoneNotifySignalStrengthChangeIndId:
		    {
		    TMockLtsyData2Buf<TInt32, TInt8> sigStrength;
		    sigStrength.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifySignalStrengthChangeInd(aResult, sigStrength.Data1(), sigStrength.Data2());
			}
			break;
		case KMockLtsyDispatchPhoneNotifyNetworkRegistrationStatusChangeIndId:
		    {
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneRegistrationStatus> status;
			status.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyNetworkRegistrationStatusChangeInd(aResult, status.Data1());
			}
			break;
		case KMockLtsyDispatchPhoneNotifyNitzInfoChangeIndId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNITZ> nitz;
		    nitz.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyNitzInfoChangeInd(aResult, nitz.Data1());
			}
			break;
		case KMockLtsyDispatchPhoneNotifyBatteryInfoChangeIndId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneBatteryInfoV1> batteryInfo;
		    batteryInfo.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyBatteryInfoChangeInd(aResult, batteryInfo.Data1());
			}
			break;
		case KMockLtsyDispatchPhoneNotifyCurrentNetworkChangeIndId:
		    {
		    TMockLtsyData2Buf<RMobilePhone::TMobilePhoneNetworkInfoV5,
		    			      RMobilePhone::TMobilePhoneLocationAreaV1> mockDataV5;
		    mockDataV5.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyCurrentNetworkChangeInd(aResult, mockDataV5.Data1(), mockDataV5.Data2());
			}
			break;
		case KMockLtsyDispatchPhoneNotifyRfStatusChangeIndId:
		    {
			TMockLtsyData1Buf<TRfStateInfo> status;
			status.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyRfStatusChangeInd(aResult, status.Data1());
			}
			break;
		case MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneServiceProviderNameV2> spnData;
		    spnData.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetServiceProviderNameComp(aResult, spnData.Data1());
			}
			break;
		case MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneIdentityV1> phoneIdData;
		    phoneIdData.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetPhoneIdComp(aResult, phoneIdData.Data1());
			}
			break;
		case MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId:
		    {
		    TMockLtsyData1Buf<CMobilePhoneNetworkListV2*> detectedNetworkList;
		    detectedNetworkList.DeserialiseL(aData);
		    
		    //will eventually result in the CTSY making a copy of the CMobilePhoneNetworkListV2
			iCompletionCallback.CallbackPhoneGetDetectedNetworksComp(aResult,*detectedNetworkList.Data1());

			//delete the allocated CMobilePhoneNetworkListV2 in from the deserialise
			detectedNetworkList.Close();
			}
			break;
		case MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId:
		    {
			iCompletionCallback.CallbackPhoneGetDetectedNetworksCancelComp(aResult);
			}
			break;
		case MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId:
		    {
			iCompletionCallback.CallbackPhoneSelectNetworkComp(aResult);
			}
			break;
		case MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId:
		    {
			iCompletionCallback.CallbackPhoneSelectNetworkCancelComp(aResult);
			}
			break;
		case MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneSelectionMethod> phoneNetworkSelectionData;
		    phoneNetworkSelectionData.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneSetNetworkSelectionSettingComp(aResult,phoneNetworkSelectionData.Data1());
			}
			break;
		case MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId:
		    {
		    TMockLtsyData2Buf<RMobilePhone::TMobilePhoneNetworkInfoV5,
		    			      RMobilePhone::TMobilePhoneLocationAreaV1> mockDataV5;
		    mockDataV5.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetCurrentNetworkInfoComp(aResult, mockDataV5.Data1(), mockDataV5.Data2());
			}
		    break;
		case MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId:
			{
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkMode> mode;
		    mode.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetNetworkModeComp(aResult, mode.Data1());
			}
			break;
		case MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId:
			{
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNITZ> nitz;
		    nitz.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetNitzInfoComp(aResult, nitz.Data1());	
			}
			break;
		case MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId:
			{
		    TMockLtsyData2Buf<TInt32, TInt8> sigStrength;
		    sigStrength.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetSignalStrengthComp(aResult, sigStrength.Data1(), sigStrength.Data2());
			}
			break;
		case MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId:
			{
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneBatteryInfoV1> batteryInfo;
		    batteryInfo.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetBatteryInfoComp(aResult, batteryInfo.Data1());
			}
			break;
		case MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId:
			{
			iCompletionCallback.CallbackPhoneSimRefreshRegisterComp(aResult);
			}
			break;
		case KMockLtsyDispatchPhoneNotifyNspsStatusChangeIndId:
		    {
			TMockLtsyData1Buf<TBool> data;
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyNspsStatusChangeInd(aResult, data.Data1());
			}
			break;
        case MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId:
			{
			iCompletionCallback.CallbackPhoneNspsWakeupComp(aResult);
			}
			break;
        case MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId:
			{
			iCompletionCallback.CallbackPhoneSetSystemNetworkModeComp(aResult);
			}
			break;
        case MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId:
		    {
		    TMockLtsyData1Buf<TUint32> modes;
		    modes.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetCurrentSystemNetworkModesComp(aResult, modes.Data1());
			}
			break;
        case MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId:
			{
			iCompletionCallback.CallbackPhoneResetNetServerComp(aResult);
			}
			break;
        case MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId:
			{
			iCompletionCallback.CallbackPhoneSetAlwaysOnModeComp(aResult);
			}
			break;
		case MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId:
			{
			RMobilePhone::TMobilePhoneFdnStatus fdnStatus;
			TMockLtsyData1<RMobilePhone::TMobilePhoneFdnStatus> data(fdnStatus);
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetFdnStatusComp(aResult,fdnStatus);
			}
			break;
		case KMockLtsyDispatchPhoneNotifyNetworkSelectionSettingChangeIndId:
			{
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneSelectionMethod> phoneNetworkSelectionData;
		    phoneNetworkSelectionData.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyNetworkSelectionSettingChangeInd(aResult,phoneNetworkSelectionData.Data1());
			}
			break;
        case MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId:
			{
			iCompletionCallback.CallbackPhoneSetDriveModeComp(aResult);
			}
			break;
		case KMockLtsyDispatchPhoneNotifyRauEventIndId:
		    {
			TMockLtsyData1Buf<RMmCustomAPI::TRauEventStatus> data;
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyRauEventInd(aResult, data.Data1());
			}
			break;
		case KMockLtsyDispatchPhoneNotifyCellInfoChangedIndId:
		    {
		    TMockLtsyData1Buf<RMmCustomAPI::TMmCellInfo *> info;
		    info.DeserialiseL(aData);
            if(info.Data1()->iMode == RMmCustomAPI::TMmCellInfo::EUnknown)
                {
                iCompletionCallback.CallbackPhoneNotifyCellInfoChangedInd(aResult);
                }
            else if(info.Data1()->iMode == RMmCustomAPI::TMmCellInfo::EGSM)
                {
                iCompletionCallback.CallbackPhoneNotifyCellInfoChangedInd(aResult, info.Data1()->iGsmCellInfo);
                }
            else if(info.Data1()->iMode == RMmCustomAPI::TMmCellInfo::EWCDMA)
                {
                iCompletionCallback.CallbackPhoneNotifyCellInfoChangedInd(aResult, info.Data1()->iWcdmaCellInfo);
                }
            info.Close();
			}
			break;
		case KMockLtsyDispatchPhoneNotifyNetworkConnectionFailureIndId:
		    {
			iCompletionCallback.CallbackPhoneNotifyNetworkConnectionFailureInd(aResult);
			}
			break;
		case KMockLtsyDispatchCustomPhoneNotifyDtmfEventIndId:
		    {
			TMockLtsyData3Buf<RMmCustomAPI::TDtmfEventType, RMmCustomAPI::TDtmfEvent, TChar> customData;
			customData.DeserialiseL( aData );
			iCompletionCallback.CallbackPhoneNotifyDtmfEventInd(
						aResult, customData.Data1(), customData.Data2(), customData.Data3() );
			}
			break;
		case KMockLtsyDispatchPhoneNotifyDtmfEventIndId:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneDTMFEvent> data;
			data.DeserialiseL( aData );
			iCompletionCallback.CallbackPhoneNotifyDtmfEventInd( aResult, data.Data1() );			
			}
			break;
        case MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId:
		    {
		    TMockLtsyData1Buf<RMmCustomAPI::THSxPAStatus> status;
		    status.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetHspaStatusComp(aResult, status.Data1());
			}
			break;
        case MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId:
			{
			iCompletionCallback.CallbackPhoneSetHspaStatusComp(aResult);
			}
			break;
		case KMockLtsyDispatchPhoneNotifyHspaStatusChangedIndId:
		    {
		    TMockLtsyData1Buf<RMmCustomAPI::THSxPAStatus> status;
		    status.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyHspaStatusChangedInd(aResult, status.Data1());
			}
			break;
        case MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId:
		    {
		    TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkLongName> name;
		    name.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetNetworkProviderNameComp(aResult, name.Data1());
			}
			break;
        case MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId:
		    {
		    TMockLtsyData2Buf<RMmCustomAPI::TOperatorNameType, TBuf<KMaxOperatorNameLength> > name;
		    name.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetOperatorNameComp(aResult, name.Data1(), name.Data2());
			}
			break;
        case MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId:
		    {
		    TMockLtsyData1Buf<RMmCustomAPI::TMmCellInfo *> info;
		    info.DeserialiseL(aData);
            if(info.Data1()->iMode == RMmCustomAPI::TMmCellInfo::EUnknown)
                {
                iCompletionCallback.CallbackPhoneGetCellInfoComp(aResult);
                }
            else if(info.Data1()->iMode == RMmCustomAPI::TMmCellInfo::EGSM)
                {
                iCompletionCallback.CallbackPhoneGetCellInfoComp(aResult, info.Data1()->iGsmCellInfo);
                }
            else if(info.Data1()->iMode == RMmCustomAPI::TMmCellInfo::EWCDMA)
                {
                iCompletionCallback.CallbackPhoneGetCellInfoComp(aResult, info.Data1()->iWcdmaCellInfo);
                }
            info.Close();
			}
			break;
		case MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId:
			{
			iCompletionCallback.CallbackPhoneRegisterCellInfoChangeNotificationComp(aResult);
			}
			break;
		case MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId:
			{
			RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
			TMockLtsyData1<RMobilePhone::TMobilePhoneCellInfoV9> data(cellInfo);
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetPhoneCellInfoComp(aResult,cellInfo);
			}
			break;
		case KMockLtsyDispatchPhoneNotifyCellInfoChangeIndId:
			{
			RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
			TMockLtsyData1<RMobilePhone::TMobilePhoneCellInfoV9> data(cellInfo);
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneNotifyCellInfoChangeInd(aResult,cellInfo);
			}
			break;
		case MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId:
			{
			TInt applicationNumber;
			TBool supported;
			TMockLtsyData2<TInt, TBool> data(applicationNumber, supported);
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetUsimServiceSupportComp(aResult, applicationNumber, supported);
			}
			break;
		case MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId:
			{
			TDesC8* aid = NULL;
			TMockLtsyData1<TDesC8*> data(aid);
			data.DeserialiseL(aData);
			iCompletionCallback.CallbackPhoneGetCurrentActiveUsimApplicationComp(aResult, *aid);
			
			delete aid;
			aid = NULL;
			}
			break;
		case MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId:
			{
			iCompletionCallback.CallbackPhoneTerminateAllCallsComp(aResult);
			}
			break;
		case MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId:
			{
    		RMmCustomAPI::TBandSelection band;
    		RMmCustomAPI::TNetworkModeCaps mode;
    		TMockLtsyData2<RMmCustomAPI::TBandSelection, RMmCustomAPI::TNetworkModeCaps> data(band, mode);
    		data.DeserialiseL(aData);
    		iCompletionCallback.CallbackPhoneGetSystemNetworkBandComp(aResult, band, mode);
			}
			break;
		case MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId:
			{
			iCompletionCallback.CallbackPhoneSetSystemNetworkBandComp(aResult);
			}
			break;

		default:
			{
			// Shouldn't get here. will panic MessageManager()->Complete if allowed to continue
			ASSERT(NULL);
			}
			break;
		}
	} // CMockPhoneMessHandler::CompleteL
