// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmockphonemesshandler.h"
#include "MmTsy_Csd_Defs.h" 
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmmessagerouter.h"
#include "cmockltsyengine.h"
#include "tmockltsydata.h"
#include <mmlist.h>
#include "etelmm.h"

/**
Factory function
*/
CMockPhoneMessHandler* CMockPhoneMessHandler::NewL(CMmMessageRouter* aMessageRouter )
    {
    CMockPhoneMessHandler* self = new(ELeave)CMockPhoneMessHandler(aMessageRouter);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/**
Constructor
*/
CMockPhoneMessHandler::CMockPhoneMessHandler(CMmMessageRouter* aMessageRouter)
 :iMessageRouter(aMessageRouter)
    {  
    }

/**
2nd phase contructor
*/
void CMockPhoneMessHandler::ConstructL()
    {
    }

/**
Destructor
*/
CMockPhoneMessHandler::~CMockPhoneMessHandler()
    {
	iAuthorizationInfos.ResetAndDestroy();
	iCBLists.ResetAndDestroy();
	iCFLists.ResetAndDestroy();
	iCWLists.ResetAndDestroy();
	iVoicemailIds.ResetAndDestroy();
	iNetworkInfoV1List.ResetAndDestroy();
	iNetworkInfoV2List.ResetAndDestroy();
	iNetworkInfoV5List.ResetAndDestroy();
    }

/**
Execute a request
*/
TInt CMockPhoneMessHandler::ExtFuncL(TInt aIpc,const CMmDataPackage* aMmDataPackage)
    {
    switch (aIpc)
    	{
    	case EMobilePhoneGetAPNname: 
    		{
    		TUint32 index;
    		aMmDataPackage->UnPackData(index);
			TMockLtsyData1<TUint32> data(index);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);    		
    		} 
    		
    	case EMobilePhoneAppendAPNName:
    		{
    		RMobilePhone::TAPNEntryV3 apnEntry;
    		aMmDataPackage->UnPackData(apnEntry);    		   		
			TMockLtsyData1<RMobilePhone::TAPNEntryV3> data( apnEntry);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);     		
    		}  
    		
    	case EMobilePhoneDeleteAPNName:
    		{
    		TUint32 index;
    		aMmDataPackage->UnPackData(index);    		 		
			TMockLtsyData1<TUint32> data( index);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);  
    		}
	 
		case EMmTsyUpdateLifeTimeIPC:
    		{
    		TUint32 callDuration;
    		aMmDataPackage->UnPackData(callDuration);    		 		
			TMockLtsyData1<TUint32> data(callDuration);
			// ignore callDuration param value, when doing check in mocksy engine:
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data, EFalse);  
    		}      		
		case EMobilePhoneSetAPNControlListServiceStatus:
    		{
    		RMobilePhone::TAPNControlListServiceStatus apnControlListServiceStatus;
    		aMmDataPackage->UnPackData(apnControlListServiceStatus);    		 		
			TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> data(apnControlListServiceStatus);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);  
    		}
    	
    	case EMobilePhoneGetServiceTable:
    		{
     		RMobilePhone::TMobilePhoneServiceTable serviceTable;
   			aMmDataPackage->UnPackData(serviceTable);
			TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> data(serviceTable);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    		
    	case EMobilePhoneGetLockInfo:
    		{
    		RMobilePhone::TMobilePhoneLock lockType;
    		aMmDataPackage->UnPackData(lockType);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneLock> data(lockType);
       		return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
			    		
    	case EMobilePhoneSetLockSetting:
    		{
    		TLockAndSetting* lockAndSetting;
    		RMobilePhone::TMobilePassword* password;
    		aMmDataPackage->UnPackData(&lockAndSetting, &password);
    		if ( (*lockAndSetting).iLock && (*lockAndSetting).iSetting )
    			{
	    		TLockSettingPassword lockSettingPassword;
	    		lockSettingPassword.iLock = *((*lockAndSetting).iLock);
	    		lockSettingPassword.iSetting = *((*lockAndSetting).iSetting);
	    		if ( password )
	    			{
	    			lockSettingPassword.iPassword = *password;
	    			}
	    		else
	    			{
	    			lockSettingPassword.iPassword = KNullDesC();
	    			}
	    		TMockLtsyData1<TLockSettingPassword> data(lockSettingPassword);
	       		return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    			}
    		else
    			{
    			return KErrCorrupt;
    			}
    		}
    	case EMobilePhoneNotifySecurityEvent:
    		{
    		RMobilePhone::TMobilePhoneSecurityEvent event;
    		aMmDataPackage->UnPackData(event);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityEvent> data(event);
    		return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneNotifyLockInfoChange:
    		{
    		RMobilePhone::TMobilePhoneLock lock;
			RMobilePhone::TMobilePhoneLockInfoV1 lockInfoV1;
			aMmDataPackage->UnPackData(lockInfoV1,lock);
			TMockLtsyData2<RMobilePhone::TMobilePhoneLockInfoV1, RMobilePhone::TMobilePhoneLock> data(lockInfoV1, lock);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}	
    	case EMobilePhoneAbortSecurityCode:
		case EMobilePhoneGetSecurityCodeInfo:
			{
			RMobilePhone::TMobilePhoneSecurityCode secCode;
			aMmDataPackage->UnPackData(secCode);
			TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> data(secCode);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
			}
    	case EMobilePhoneSetALSLine:
    		{
    		RMobilePhone::TMobilePhoneALSLine alsLine;
    		aMmDataPackage->UnPackData(alsLine);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> data(alsLine);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneGetMailboxNumbers:
    		{
    		RMobilePhone::TMobilePhoneVoicemailIdsV3* voicemailIds;
    		aMmDataPackage->UnPackData(&voicemailIds);
		    TMockLtsyData1<RMobilePhone::TMobilePhoneVoicemailIdsV3> data(*voicemailIds);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneIMSAuthenticate:
    		{
    		RMobilePhone::TImsAuthenticateDataV5* authData;
    		aMmDataPackage->UnPackData(&authData);
		    TMockLtsyData1<RMobilePhone::TImsAuthenticateDataV5> data(*authData);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneSetIccMessageWaitingIndicators:
    		{
    		RMobilePhone::TMobilePhoneMessageWaitingV1* indicatorsData;
    		aMmDataPackage->UnPackData(&indicatorsData);
		    TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1> data(*indicatorsData);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneSendDTMFTones:
    		{
    		TDesC* tones;
    		aMmDataPackage->UnPackData(&tones);
    		TInfoName bufTones;
    		if (tones)
	    		{
	    		bufTones.Copy((*tones).Left(KMaxInfoName));	
	    		}
		    TMockLtsyData1<TInfoName> data(bufTones);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneStartDTMFTone:
    		{
    		TChar* tone;
    		aMmDataPackage->UnPackData(&tone);
		    TMockLtsyData1<TChar> data(*tone);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneContinueDTMFStringSending:
    		{
    		const HBufC16* tones;
    		aMmDataPackage->UnPackData(&tones);
    		TInfoName bufTones;
    		if (tones)
	    		{
	    		bufTones.Copy(*tones);	
	    		}
		    TMockLtsyData1<TInfoName> data(bufTones);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneSetCallWaitingStatus:
    		{
			RMobilePhone::TMobileService serviceGroup;
    		RMobilePhone::TMobilePhoneServiceAction action;
    		aMmDataPackage->UnPackData(serviceGroup, action);
		    TMockLtsyData2<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneServiceAction>
		                  data(serviceGroup, action);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneVerifySecurityCode:
    		{
    		RMobilePhone::TMobilePhoneSecurityCode secCode;
    		RMobilePhone::TCodeAndUnblockCode twoCodes;
    		aMmDataPackage->UnPackData(secCode, twoCodes);
    		TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> data(secCode,twoCodes);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
		case EMobilePhoneNotifyCallBarringStatusChange:
    		{
    		RMobilePhone::TMobilePhoneCBCondition condition;
    		aMmDataPackage->UnPackData(condition);
		    TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> data(condition);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneGetCallForwardingStatusPhase1:
    		{
			RMobilePhone::TMobilePhoneCFCondition condition;
			RMobilePhone::TMobileService serviceGroup;
    		aMmDataPackage->UnPackData(condition, serviceGroup);
    		TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobileService>
    		                                          data(condition, serviceGroup);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
    		}
    	case EMobilePhoneSetCallForwardingStatus:
    		{
			RMobilePhone::TMobilePhoneCFCondition condition;
			RMobilePhone::TMobilePhoneCFChangeV1* cfInfo( NULL );
    		aMmDataPackage->UnPackData(condition, cfInfo);
    		if ( cfInfo )
	    		{
	    		TMockLtsyData2<RMobilePhone::TMobilePhoneCFCondition, RMobilePhone::TMobilePhoneCFChangeV1>
	    		                                          data(condition, *cfInfo);
	 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
	    		}
	    	else
	    		{
	    		return KErrCorrupt;
	    		}
    		}
    	case EMobilePhoneSetSSPassword:
    		{
    		RMobilePhone::TMobilePhonePasswordChangeV2 pwdChange;
    		TUint16 service;
    		aMmDataPackage->UnPackData(pwdChange, service);
    		TMockLtsyData2<RMobilePhone::TMobilePhonePasswordChangeV2, TUint16> data(pwdChange, service);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneSendNetworkServiceRequest:
    	case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
    		{
    		const TDesC* serviceString;
    		aMmDataPackage->UnPackData(serviceString);
    		TInfoName serviceStringBuf(*serviceString);
		    TMockLtsyData1<TInfoName> data(serviceStringBuf);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
    		}
    	case ECtsyPhoneSendNetworkServiceRequestIndReq:
    		{
    		RMobilePhone::TMobilePhoneNotifySendSSOperation op;
    		aMmDataPackage->UnPackData(op);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneNotifySendSSOperation> data(op);
    		return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}    		    		
    	case EMobilePhoneGetIdentityServiceStatus:
    		{
    		RMobilePhone::TMobilePhoneIdService service;
    		aMmDataPackage->UnPackData(service);
			TMockLtsyData1<RMobilePhone::TMobilePhoneIdService> data(service);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
    		}
    	case EMobilePhoneSetNetworkSelectionSetting:
    		{
			RMobilePhone::TMobilePhoneNetworkSelectionV1 setting;
    		aMmDataPackage->UnPackData(setting);
    		TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> data(setting);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneSelectNetwork:
    		{
			TBool isManual;
			RMobilePhone::TMobilePhoneNetworkManualSelection manualSelection;
    		aMmDataPackage->UnPackData(isManual, manualSelection);
			TMockLtsyData2<TBool, RMobilePhone::TMobilePhoneNetworkManualSelection> 
				data(isManual, manualSelection);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
    		}
    	case EMobilePhoneGetPhoneStoreInfo:
    		{
    		RMobilePhone::TMobileName storeName;
    		aMmDataPackage->UnPackData(storeName);
			TMockLtsyData1<RMobilePhone::TMobileName> data(storeName);
 			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);    		
    		}
		case EMobilePhoneGetBarringStatusPhase1:
			{
			RMobilePhone::TMobilePhoneCBCondition condition;
			aMmDataPackage->UnPackData(condition);
			TMockLtsyData1<RMobilePhone::TMobilePhoneCBCondition> data(condition);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
			}
		case EMobilePhoneSetCallBarringStatus:
			{
			RMobilePhone::TMobilePhoneCBCondition condition;
			RMobilePhone::TMobilePhoneCBChangeV1* info = NULL;
			aMmDataPackage->UnPackData(condition, info);
			TMockLtsyData2<RMobilePhone::TMobilePhoneCBCondition, 
						   RMobilePhone::TMobilePhoneCBChangeV1> data(condition, *info);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc,data);
			}
		case ECtsyPhoneTerminateAllCallsReq:		
			{
			TInt callId(0);
			aMmDataPackage->UnPackData(callId);
			TMockLtsyData1< TInt > data(callId);
			return iMessageRouter->MockLtsyEngine()->ExecuteCommandL(aIpc, data);
			}
        case ECtsyPhoneTerminateActiveCallsReq: 
		case ECtsyPhoneCellInfoReq:
    	case ECtsyPhoneCellInfoIndReq:			
    	case EMobilePhoneSelectNetworkCancel:
    	case EMobilePhoneGetFdnStatus:
    	case EMobilePhoneEnumerateAPNEntries:
    	case EMobilePhoneGetAPNControlListServiceStatus:			    		
    	case EMobilePhoneAuthorizationInfoPhase1:
    	case EMobilePhoneGetIccMessageWaitingIndicators:
    	case EMobilePhoneStopDTMFTone:
    	case EMobilePhoneSendDTMFTonesCancel:
    	case EMobilePhoneGetWaitingStatusPhase1:
    	case EMobilePhoneGetServiceProviderName:
    	case EMobilePhoneGetCustomerServiceProfile:
    	case EMobilePhoneGetSubscriberId:
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
void CMockPhoneMessHandler::CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult)
	{
	CMmDataPackage dataPackage;
	switch (aIpc)
		{
		case EMobilePhoneGetAPNname:
			{
			TMockLtsyData1Buf<RMobilePhone::TAPNEntryV3> apnEntry;
			apnEntry.DeserialiseL(aData);
			dataPackage.PackData(apnEntry.Data1Ptr());
			}
			break;
		
		case EMobilePhoneNotifyIccAccessCapsChange:
		case EMobilePhoneAppendAPNName:
		case EMobilePhoneDeleteAPNName:
		case EMobilePhoneNotifyAPNListChanged:
		case EMobilePhoneSetAPNControlListServiceStatus:	 
  		case EMmTsyUpdateLifeTimeIPC:
			{
			// no parameter is requred
			}
			break;
			
		case EMobilePhoneEnumerateAPNEntries:
			{	
			TMockLtsyData1Buf<TUint32> index;
			index.DeserialiseL(aData);
			dataPackage.PackData(index.Data1Ptr());		
			}
			break;
			
		case EMobilePhoneGetAPNControlListServiceStatus:
		case EMobilePhoneNotifyAPNControlListServiceStatusChange:
			{
			TMockLtsyData1Buf<RMobilePhone::TAPNControlListServiceStatus> status;
			status.DeserialiseL(aData);
			dataPackage.PackData(status.Data1Ptr());
			}
			break;	
		
		case EMobilePhoneGetHomeNetwork:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkInfoV8> idData;
			idData.DeserialiseL(aData);
			
			TInt id = idData.Data1Ptr()->ExtensionId();
			
		    if ( KETelExtMultimodeV1 == id )
				{
				TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkInfoV1> homeNetworkDataV1;
				homeNetworkDataV1.DeserialiseL(aData);
				dataPackage.PackData(homeNetworkDataV1.Data1Ptr());
				}
			else if ( KETelExtMultimodeV2 == id )
				{
				TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkInfoV2> homeNetworkDataV2;
				homeNetworkDataV2.DeserialiseL(aData);
				dataPackage.PackData(homeNetworkDataV2.Data1Ptr());
				}
    		else if ( KEtelExtMultimodeV5 == id )
    			{
				TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkInfoV5> homeNetworkDataV5;
				homeNetworkDataV5.DeserialiseL(aData);
				dataPackage.PackData(homeNetworkDataV5.Data1Ptr());
    			}  
    		else
    			{
				dataPackage.PackData(idData.Data1Ptr());
    			} 
			}
			break;
		
		case EMmTsyPhoneGetPin1DisableSupportedIPC:
			{
			TMockLtsyData1Buf<TBool> pin1DisableSupportData;
			pin1DisableSupportData.DeserialiseL(aData);
			dataPackage.PackData(pin1DisableSupportData.Data1Ptr());
			}
			break;
			
		case EMobilePhoneGetALSLine:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneALSLine> alsLineData;
			alsLineData.DeserialiseL(aData);
			dataPackage.PackData(alsLineData.Data1Ptr());
			}
			break;
			
		case EMobilePhoneGetServiceTable:
			{
			RMobilePhone::TMobilePhoneServiceTableV1 table;
			TPckg<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableData(table);
			serviceTableData.Copy(aData);
			dataPackage.PackData(&table);
			}
			break;
		case EMobilePhoneGetSubscriberId:
			{
			TBuf8<RMobilePhone::KIMSISize> subscriberId;
			subscriberId.Copy(aData);
			dataPackage.PackData(&subscriberId);
			}
			break;
		case EMobilePhoneGetPhoneId:
			{
			//CTSY does not expect the full TMobilePhoneIdentityV1 but just a TBuf8<KSerialNumberLength> containing serial id.
			TMockLtsyData1Buf<TBuf8<KSerialNumberLength> > serialIdData;
			serialIdData.DeserialiseL(aData);
			dataPackage.PackData(serialIdData.Data1Ptr()); //<<<how to know how to pack up data to send to CTSY??
			}
			break;
		case EMobilePhoneGetCurrentActiveUSimApplication:
			{
			TMockLtsyData1Buf<RMobilePhone::TAID > aid;
			aid.DeserialiseL(aData);
			dataPackage.PackData(aid.Data1Ptr());
			}
			break;						
		case EMobilePhoneGetNetworkRegistrationStatus:
			// no params
			break;
			
		case EMobilePhoneNotifyBatteryInfoChange:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneBatteryInfoV1> serialBatInfo;
			serialBatInfo.DeserialiseL(aData);
			dataPackage.PackData(serialBatInfo.Data1Ptr());
			}			
			break;
		case EMobilePhoneGetPhoneStoreInfo:
			{
			TMockLtsyData1Buf<RMobilePhoneStore::TMobilePhoneStoreInfoV1> phoneStoreInfo;
			phoneStoreInfo.DeserialiseL(aData);
			dataPackage.PackData(phoneStoreInfo.Data1Ptr());
			}							
			break;
		case EMobilePhoneNotifySignalStrengthChange:
		    {
		    TInt32 signalStrength;
		    TInt8  signalBar;		    	    			
 			TMockLtsyData2<TInt32, TInt8> signalStrengthData(signalStrength, signalBar);
 			signalStrengthData.DeserialiseL(aData);
 			dataPackage.PackData(signalStrengthData.Data2Ptr(),signalStrengthData.Data1Ptr());		  						
		}
		    break;
		case EMobilePhoneGetLockInfo:
			{
			TMockLtsyData2Buf<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> lockInfoData;
			lockInfoData.DeserialiseL(aData);
			dataPackage.PackData(lockInfoData.Data1Ptr(), lockInfoData.Data2Ptr());
			}
			break;
			
		case EMobilePhoneNotifyLockInfoChange:
			{
			TMockLtsyData2Buf<RMobilePhone::TMobilePhoneLockInfoV1, RMobilePhone::TMobilePhoneLock> lockInfoData;
			lockInfoData.DeserialiseL(aData);
			dataPackage.PackData(lockInfoData.Data1Ptr(), lockInfoData.Data2Ptr());
			}
			break;

		case EMobilePhoneNotifyNetworkSecurityLevelChange:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkSecurity> netSecurity;
			netSecurity.DeserialiseL(aData);
			dataPackage.PackData(netSecurity.Data1Ptr());
			}			
			break;
			
		case EMobilePhoneNotifySecurityCapsChange:
		    {
			TMockLtsyData1Buf<TUint32> caps;
			caps.DeserialiseL(aData);
			dataPackage.PackData(caps.Data1Ptr());
		    }
		    break;
		    
		case EMobilePhoneNotifySecurityEvent:
		    {
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneSecurityEvent> ev;
			ev.DeserialiseL(aData);
			dataPackage.PackData(ev.Data1Ptr());
		    }
		    break;	

		case EMobilePhoneGetCipheringIndicatorStatus:
		    {
			TMockLtsyData1Buf<RMobilePhone::TMobileCallCipheringIndicator> indi;
			indi.DeserialiseL(aData);
			dataPackage.PackData(indi.Data1Ptr());
		    }
		    break;	
		case EMobilePhoneSetALSLine:
			break;
		case EMobilePhoneAuthorizationInfoPhase1:
		    {
			RMobilePhone::CImsAuthorizationInfoV5* info = RMobilePhone::CImsAuthorizationInfoV5::NewL();
			CleanupStack::PushL(info);
			iAuthorizationInfos.Append(info);
			CleanupStack::Pop(info);
			info->InternalizeL(aData);
			dataPackage.PackData(&info);
		    }
			break;
		case EMmTsyBootGetRFStatusIPC:
			{
			TMockLtsyData1Buf<TRfStateInfo> indi;
			indi.DeserialiseL(aData);
			dataPackage.PackData(indi.Data1Ptr());
			}
			break;

		case EMobilePhoneGetMailboxNumbers:
		    {
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneVoicemailIdsV3> voicemailIdsData;
			voicemailIdsData.DeserialiseL(aData);
			RMobilePhone::TMobilePhoneVoicemailIdsV3* voicemailIds = 
			           new (ELeave) RMobilePhone::TMobilePhoneVoicemailIdsV3(voicemailIdsData.Data1());
			CleanupStack::PushL(voicemailIds);
			iVoicemailIds.Append(voicemailIds);
			CleanupStack::Pop(voicemailIds);
			dataPackage.PackData(&voicemailIds);
		    }
			break;

		case EMobilePhoneIMSAuthenticate:
		    {
			TMockLtsyData1Buf<RMobilePhone::TImsAuthenticateDataV5> authenticateData;
			authenticateData.DeserialiseL(aData);
			// compare with special constant _L8("17796") for recognize null pointer packing test case
			if (authenticateData.Data1Ptr()->iAUTS.Compare(_L8("17796"))==0)
				{
				RMobilePhone::TImsAuthenticateDataV5* nullPtr(NULL);
				dataPackage.PackData(nullPtr);
				}
			else
				{
				dataPackage.PackData(authenticateData.Data1Ptr());
				}
		    }
			break;

		case EMobilePhoneSetLockSetting:
		    {
			TMockLtsyData2Buf<RMobilePhone::TMobilePhoneLockStatus, RMobilePhone::TMobilePhoneLockSetting> lockSettingData;
			lockSettingData.DeserialiseL(aData);
			dataPackage.PackData(lockSettingData.Data1Ptr(), lockSettingData.Data2Ptr());
		    }
			break;
		case EMobilePhoneGetSecurityCodeInfo:
			{
			TMockLtsyData2Buf<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData;
			codeInfoData.DeserialiseL(aData);
			dataPackage.PackData(codeInfoData.Data1Ptr(), codeInfoData.Data2Ptr());
			}
			break;

		case EMobilePhoneNotifySecurityCodeInfoChange:
			{
			TMockLtsyData2Buf<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TMobilePhoneSecurityCodeInfoV5> codeInfoData;
			codeInfoData.DeserialiseL(aData);
			dataPackage.PackData(codeInfoData.Data1Ptr(), codeInfoData.Data2Ptr());
			}
			break;
		
		case EMobilePhoneGetIccMessageWaitingIndicators:
		case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneMessageWaitingV1> indicatorsData;
			indicatorsData.DeserialiseL(aData);
			dataPackage.PackData(indicatorsData.Data1Ptr());
			}
			break;
		case EMobilePhoneSetIccMessageWaitingIndicators:
			break;
		case EMobilePhoneNotifyStopInDTMFString:
			{
			TMockLtsyData1Buf<TInfoName> tones;
			tones.DeserialiseL(aData);
			TBuf<KMaxInfoName> tonesBuf;

			_LIT(KNull, "null");
			if ( tones.Data1() != KNull() )
				{
				tonesBuf = tones.Data1();
				}
			dataPackage.PackData(&tonesBuf);
			}
			break;
		case EMobilePhoneStopDTMFTone:
		case EMobilePhoneSendDTMFTones:
		case EMobilePhoneSendDTMFTonesCancel:
			break;
		case EMobilePhoneNotifyDTMFEvent:
			{
	   		TMockLtsyData1Buf< RMobilePhone::TMobilePhoneDTMFEvent > toneEvent;
			toneEvent.DeserialiseL(aData);
			dataPackage.PackData(toneEvent.Data1Ptr());	
			}			
			break;		
		case EMobilePhoneGetWaitingStatusPhase1:
		    {
			TMockLtsyData1Buf<CMobilePhoneCWList*> completeData;
			completeData.DeserialiseL(aData);
			if (completeData.Data1())
				{
				completeData.PushL();
				iCWLists.AppendL(completeData.Data1());
				CleanupStack::Pop(&completeData);
				}
			dataPackage.PackData(completeData.Data1());
		    }
			break;
    	case EMobilePhoneGetCallForwardingStatusPhase1:
		    {
			TMockLtsyData1Buf<CMobilePhoneCFList*> completeData;
			completeData.DeserialiseL(aData);
			if (completeData.Data1())
				{
				completeData.PushL();
				iCFLists.AppendL(completeData.Data1());
				CleanupStack::Pop(&completeData);
				}
			dataPackage.PackData(completeData.Data1());
		    }
			break;

		case EMmTsySimGetICCType:
    		{
			TMockLtsyData1Buf<TInt> icc;
			icc.DeserialiseL(aData);
			dataPackage.PackData(icc.Data1Ptr());
		    }
    		break;

		case EMobilePhoneGetFdnStatus:
		    {
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneFdnStatus> status;
			status.DeserialiseL(aData);
			dataPackage.PackData(status.Data1Ptr());
		    }
		    break;
			
		case EMobilePhoneNotifyNetworkRegistrationStatusChange:
			{
			TMockLtsyData2Buf<RMobilePhone::TMobilePhoneRegistrationStatus, RMobilePhone::TMobilePhoneNetworkStatus> data;
			data.DeserialiseL(aData);
			dataPackage.PackData(data.Data1Ptr(), data.Data2Ptr());
			}
			break;

		case EMobilePhoneSetCallWaitingStatus:
			break;
		case EMobilePhoneNotifyCallWaitingStatusChange:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneCWInfoEntryV1> cwInfoEntry;
			cwInfoEntry.DeserialiseL(aData);
			dataPackage.PackData(cwInfoEntry.Data1Ptr());
			}
			break;
			
		case EMobilePhoneSetFdnSetting:
		    break;
		    
		case EMobilePhoneNotifyFdnStatusChange:
		    break;
		
		case EMobilePhoneGetDetectedNetworksV2Phase1:
			{
			TMockLtsyData1Buf<CMobilePhoneNetworkListV2*> status;
			status.DeserialiseL(aData);
			dataPackage.PackData(status.Data1Ptr());
			iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
			status.Close();
			return;
			}
		case EMobilePhoneNotifyNITZInfoChange:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNITZ> status;
			status.DeserialiseL(aData);
			dataPackage.PackData(status.Data1Ptr());
			}
			break;
		case ECtsyPhoneCellInfoComp:				
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneCellInfoV9> mockData;
			mockData.DeserialiseL(aData);
			dataPackage.PackData(mockData.Data1Ptr());
			}
			break;
		case ECtsyPhoneCellInfoIndComp:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneCellInfoV9> mockData;
			mockData.DeserialiseL(aData);
			dataPackage.PackData(mockData.Data1Ptr());
			}
			break;
		case EMobilePhoneNotifyCurrentNetworkChange:
			{
			TMockLtsyData2Buf<RMobilePhone::TMobilePhoneNetworkInfoV8,
			                  RMobilePhone::TMobilePhoneLocationAreaV1> mockDataV8;
			mockDataV8.DeserialiseL(aData);
			
			TInt id = mockDataV8.Data1Ptr()->ExtensionId();
			
		    if ( KETelExtMultimodeV1 == id )
				{
				TMockLtsyData2Buf<RMobilePhone::TMobilePhoneNetworkInfoV1,
                                  RMobilePhone::TMobilePhoneLocationAreaV1> mockData;
				mockData.DeserialiseL(aData);
				dataPackage.PackData(mockData.Data1Ptr(), mockData.Data2Ptr());
				}
			else if ( KETelExtMultimodeV2 == id )
				{
				TMockLtsyData2Buf<RMobilePhone::TMobilePhoneNetworkInfoV2,
                                  RMobilePhone::TMobilePhoneLocationAreaV1> mockData;
				mockData.DeserialiseL(aData);
				dataPackage.PackData(mockData.Data1Ptr(), mockData.Data2Ptr());
				}
    		else if( KEtelExtMultimodeV5 == id)
    			{	
    			TMockLtsyData2Buf<RMobilePhone::TMobilePhoneNetworkInfoV5,
                                  RMobilePhone::TMobilePhoneLocationAreaV1> mockData;
				mockData.DeserialiseL(aData);
				dataPackage.PackData(mockData.Data1Ptr(), mockData.Data2Ptr());
    			}
    		else
    			{
    			dataPackage.PackData(mockDataV8.Data1Ptr(), mockDataV8.Data2Ptr());
    			} 
			}
			break;

		case EMobilePhoneNotifyCallBarringStatusChange:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneCBCondition> conditionData;
			conditionData.DeserialiseL(aData);
			dataPackage.PackData(conditionData.Data1Ptr());
			}
			break;
			
		case EMobilePhoneGetBarringStatusPhase1:
			{
			CMobilePhoneCBList* cbList(NULL);
			if ( KNullDesC8() != aData )
				{
				cbList = CMobilePhoneCBList::NewL();
				CleanupStack::PushL(cbList);
				iCBLists.Append(cbList);
				CleanupStack::Pop(cbList);
				cbList->RestoreL(aData);
				}
			dataPackage.PackData(cbList);
			}
			break;
			
		case EMobilePhoneNotifyCallForwardingActive:
			{
			TMockLtsyData2Buf<RMobilePhone::TMobileService, RMobilePhone::TMobilePhoneCFActive> data;
			data.DeserialiseL(aData);
			dataPackage.PackData(data.Data1Ptr(), data.Data2Ptr());
			}
			break;
		case EMobilePhoneNotifyCallForwardingStatusChange:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneCFCondition> data;
			data.DeserialiseL(aData);
			dataPackage.PackData(data.Data1Ptr());
			}
			break;
    	case EMobilePhoneGetIdentityServiceStatus:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneIdServiceStatus> data;
			data.DeserialiseL(aData);
			dataPackage.PackData(data.Data1Ptr());
			}
			break;
		case EMobilePhoneSetCallForwardingStatus:
			break;
		case EMobilePhoneSendNetworkServiceRequest:
		case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
			break;
		case ECtsyPhoneSendNetworkServiceRequestInd:
			{
			TMockLtsyData1Buf<RMobilePhone::TMobilePhoneSendSSRequestV3> data;
			data.DeserialiseL(aData);
			dataPackage.PackData(data.Data1Ptr());			
			}			
			break;
    	case EMobilePhoneNotifyModeChange:
    		{
    		TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkMode> status;
			status.DeserialiseL(aData);
			dataPackage.PackData(status.Data1Ptr());
    		}
    		break;
		case EMobilePhoneGetServiceProviderName:
			{
    		TMockLtsyData1Buf<RMobilePhone::TMobilePhoneServiceProviderNameV2> status;
			status.DeserialiseL(aData);
			dataPackage.PackData(status.Data1Ptr());
			}
			break;
		case EMobilePhoneNotifyNetworkSelectionSettingChange:
			{
    		TMockLtsyData1Buf<RMobilePhone::TMobilePhoneNetworkSelectionV1> status;
			status.DeserialiseL(aData);
			dataPackage.PackData(status.Data1Ptr());
			}
			break;
		case EMobilePhoneGetCustomerServiceProfile:
			{
			RMobilePhone::TMobilePhoneCspFileV1 cspFile;
			TPckg<RMobilePhone::TMobilePhoneCspFileV1> mockData(cspFile);
			mockData.Copy(aData);
			dataPackage.PackData(&cspFile);
			}
			break;
			
		case EEtelPhoneNotifyModemDetected:
			{
    		TMockLtsyData1Buf<RPhone::TStatus> status;
			status.DeserialiseL(aData);
			dataPackage.PackData(status.Data1Ptr());
			}
			break;
		case EMmTsySimRefreshNowIPC:
			{
    		TMockLtsyData1Buf<TUint16> files;
			files.DeserialiseL(aData);
			dataPackage.PackData(files.Data1Ptr());
			}
			break;
		case EETelLineCapsChangeNotification:
			break;

		case EMobilePhoneNotifyCallServiceCapsChange:
			{
			TUint32 caps;
			TPckg<TUint32> mockData(caps);
			mockData.Copy(aData);
			dataPackage.PackData(&caps);
			}
			break;
		case ECtsyPhoneTerminateAllCallsComp:
		case ECtsyPhoneTerminateActiveCallsComp:
			{
			// no parameter is required
			}
			break;
		default:
			{
			// shouldnt get here. will panic MessageManager()->Complete if allowed to continue
			}
			break;
		}
	iMessageRouter->MessageManagerCallback()->Complete(aIpc, &dataPackage, aResult);
	}

//  End of File  
    
