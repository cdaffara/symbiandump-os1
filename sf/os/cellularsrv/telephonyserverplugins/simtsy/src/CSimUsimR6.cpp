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
// This file contains the implementation for the Simulator USIM R6 class 'CSimUsimR6'
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimUsimR6Traces.h"
#endif

#include <e32property.h>
#include <testconfigfileparser.h>
#include <sacls.h>
#include <e32math.h>
#include "CSimPhone.h"
#include "CSimPubSubChange.h"
#include "CSimTsyMode.h"
#include <etelmmerr.h>
#include "CSimPubSub.h"
#include "SimTsy.h"
#include "simtsyglobalproperties.h"
#include "CSimUsimR6.h"
#include <et_phone.h>

const TInt KUsimGranularity=2;		// < Granularity for USIM list array
const TInt KUsimRel6DurationTimeoutDefault=1;
const TInt KUsimRel6DurationTimeoutDefault1=2;
const TInt KUsimRel6DurationTimeoutDefault2=3;

CSimUsimR6* CSimUsimR6::NewL(CSimPhone* aPhone)
/**
 * Standard two-phase constructor.
 * @param aPhone				The parent phone object.
 * @return CSimUsimR6	The new network status class.
 */
	{
	CSimUsimR6* self=new(ELeave) CSimUsimR6(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimUsimR6::CSimUsimR6(CSimPhone* aPhone)
: iPhone(aPhone)
/**
* Trivial first phase construction.
* @param aPhone				The parent phone object.
*/
	{
	iMailBoxNumberChangePending = EFalse;
	iIccMessageWaitingPending = EFalse;
	iMessageWaitingPending = EFalse;
	iMailBoxNumberChangePendingV8 = EFalse;
	iIccMessageWaitingPendingV8 = EFalse;
	iNotifyWlanDataChangePending=EFalse;
	iNotifyPreferredWlanSIDListChangePending= EFalse;
	}

void CSimUsimR6::ConstructL()
/**
 * Second phase construction. Create instances of the necessary heap-based objects and read the necessary Release6  information
 * from the configuration file. The config file informations simulates the information given by Network
 *
 * 
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_1, "CSimUsimR6: Entered ConstructL()");
	iVoiceMailIds=new(ELeave) CArrayFixFlat<RMobilePhone::TMobilePhoneVoicemailIdsV8>(KUsimGranularity);
	iMessageWaiting=new(ELeave) CArrayFixFlat<RMobilePhone::TMobilePhoneMessageWaitingV8>(KUsimGranularity);
	
	iWlanData=new(ELeave) CArrayFixFlat<RMobilePhone::TUsimWlanDataV8>(KUsimGranularity);
	iWlanSid=new(ELeave) CArrayFixFlat<RMobilePhone::TWlanSIDV8>(KUsimGranularity);	
	iPreferredNetworks=new(ELeave) CArrayFixFlat<RMobilePhone::TMobilePreferredNetworkEntryV3>(KUsimGranularity);
	
	iGBAAuthInfoList =new(ELeave) CArrayFixFlat<TGBAAuthInfo>(KUsimGranularity);
	iMBMSInfoList = new(ELeave) CArrayFixFlat<TMBMSInfo>(KUsimGranularity);

	iGbaNafList = CMobilePhoneGbaNafIdList::NewL();
	iMbmsMskList = CMobilePhoneMbmsMskIdList::NewL();
	
	iTimer = CSimTimer::NewL(iPhone);
	iWlanTimer = CSimTimer::NewL(iPhone);
	iNotifyAuthTimer = CSimTimer::NewL(iPhone);
	
	iAuthListInfo = new(ELeave)CArrayPtrFlat<CListReadAllAttempt>(1);
	iGetWlanSIDsData = new(ELeave) CArrayPtrFlat<CListReadAllAttempt>(1);
	iGetPreferredNetworks = new(ELeave) CArrayPtrFlat<CListReadAllAttempt>(1);

	iCurrentNotifyMailboxIndex=0;
	iCurrentIccMessageWaitingIndex=0;
	iCurrentMessageWaitingIndex=0;
	iGetIccMsgIdCurrentIndex=0;
	iSetIccMsgIdCurrentIndex=0;
	iNotifyWlanDataChangeIndex=0;
	iNotifyPreferredWlanSIDListChangeIndex=0;
	iNotifyPreferredNetworksChangeIndex=0;
	iMailBoxIndex=0;			
	iIndex=0;
	
	TInt count;
	count=CfgFile()->ItemCount(KUsimMailBoxIdInfo);
	CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	TInt i;
	for(i=0;i<count;i++)
		{
		item=const_cast<CTestConfigItem*>(CfgFile()->Item(KUsimMailBoxIdInfo,i));
		if(!item)
			{
			break;
			}
		TInt voice,data,fax,other,video;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,voice);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element VOICE returned %d (element no. %d) from tag %s.",ret,0,KUsimMailBoxIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,data);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element DATA returned %d (element no. %d) from tag %s.",ret,1,KUsimMailBoxIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,fax);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element FAX returned %d (element no. %d) from tag %s.",ret,2,KUsimMailBoxIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,other);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element OTHER returned %d (element no. %d) from tag %s.",ret,3,KUsimMailBoxIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,video);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element VIDEO returned %d (element no. %d) from tag %s.",ret,4,KUsimMailBoxIdInfo);
			continue;
			}
		RMobilePhone::TMobilePhoneVoicemailIdsV8 mailboxIdInfo;
		mailboxIdInfo.iVoice=voice;
		mailboxIdInfo.iData=data;
		mailboxIdInfo.iFax=fax;
		mailboxIdInfo.iOther=other;
		mailboxIdInfo.iVideo=video;	
		iVoiceMailIds->AppendL(mailboxIdInfo);
		}
	count=CfgFile()->ItemCount(KUsimMessageWaitingIdInfo);
	item=NULL;
	for(i=0;i<count;i++)
		{
		item=const_cast<CTestConfigItem*>(CfgFile()->Item(KUsimMessageWaitingIdInfo,i));
		if(!item)
			{
			break;
			}
		TInt displayStatus,voiceMsgs,auxVoiceMsgs,dataMsgs,faxMsgs,emailMsgs,otherMsgs,videoMsgs;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,displayStatus);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element DISPLAYSTATUS returned %d (element no. %d) from tag %s.",ret,0,KUsimMessageWaitingIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,voiceMsgs);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element VOICEMSGS returned %d (element no. %d) from tag %s.",ret,1,KUsimMessageWaitingIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,auxVoiceMsgs);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element AUXVOICEMSGS returned %d (element no. %d) from tag %s.",ret,2,KUsimMessageWaitingIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,dataMsgs);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element DATAMSGS returned %d (element no. %d) from tag %s.",ret,3,KUsimMessageWaitingIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,faxMsgs);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element FAXMSGS returned %d (element no. %d) from tag %s.",ret,4,KUsimMessageWaitingIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,emailMsgs);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element EMAILMSGS returned %d (element no. %d) from tag %s.",ret,5,KUsimMessageWaitingIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,otherMsgs);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_13, "WARNING - CONFIGURATION FILE PARSING - Reading element OTHERMSGS returned %d (element no. %d) from tag %s.",ret,6,KUsimMessageWaitingIdInfo);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,videoMsgs);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_14, "WARNING - CONFIGURATION FILE PARSING - Reading element LONGNAME returned %d (element no. %d) from tag %s.",ret,7,KUsimMessageWaitingIdInfo);
			continue;
			}
		RMobilePhone::TMobilePhoneMessageWaitingV8 messageWaiting;
		messageWaiting.iDisplayStatus = RMobilePhone::TMobilePhoneIndicatorDisplay(displayStatus);
		messageWaiting.iVoiceMsgs = voiceMsgs;
		messageWaiting.iAuxVoiceMsgs = auxVoiceMsgs;
		messageWaiting.iDataMsgs = dataMsgs;
		messageWaiting.iFaxMsgs = faxMsgs;
		messageWaiting.iEmailMsgs = emailMsgs;
		messageWaiting.iOtherMsgs = otherMsgs;
		messageWaiting.iVideoMsgs = videoMsgs;
		iMessageWaiting->AppendL(messageWaiting);
		}
	count=CfgFile()->ItemCount(KUsimWlanDataInfo);
	item=NULL;
	TUint16 digit=0;
	RMobilePhone::TUsimWlanDataV8 wlanData;
	for(i=0;i<count;i++)
		{
		item=const_cast<CTestConfigItem*>(CfgFile()->Item(KUsimWlanDataInfo,i));
		if(!item)
			{
			break;
			}
		TPtrC8 FieldsUsed,Pseudonym,ReauthenticationId,MasterKey,Counter;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,FieldsUsed);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_15, "WARNING - CONFIGURATION FILE PARSING - Reading element FIELDS USED returned %d (element no. %d) from tag %s.",ret,0,KUsimWlanDataInfo);
			continue;
			}
		else 
			{
			if(AsciiToNum(FieldsUsed, digit)==KErrNone)
				{
				wlanData.iFieldsUsed=digit;
				}
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,Pseudonym);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_16, "WARNING - CONFIGURATION FILE PARSING - Reading element PSEUDONYM returned %d (element no. %d) from tag %s.",ret,1,KUsimWlanDataInfo);
			continue;
			}
		else
			{
			wlanData.iPseudonym.Copy(Pseudonym);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,ReauthenticationId);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_17, "WARNING - CONFIGURATION FILE PARSING - Reading element REAUTHENTICATIONID returned %d (element no. %d) from tag %s.",ret,2,KUsimWlanDataInfo);
			continue;
			}
		else
			{
			wlanData.iReauthenticationId.Copy(ReauthenticationId);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,MasterKey);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_18, "WARNING - CONFIGURATION FILE PARSING - Reading element MASTERKEY returned %d (element no. %d) from tag %s.",ret,3,KUsimWlanDataInfo);
			continue;
			}
		else
			{
			wlanData.iMasterKey.Copy(MasterKey);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,Counter);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_19, "WARNING - CONFIGURATION FILE PARSING - Reading element COUNTER returned %d (element no. %d) from tag %s.",ret,4,KUsimWlanDataInfo);
			continue;
			}
		else
			{
			wlanData.iCounter.Copy(Counter);
			}
		iWlanData->AppendL(wlanData);
		}
	count=CfgFile()->ItemCount(KUsimWlanSidInfo);
	item=NULL;
	RMobilePhone::TWlanSIDV8 wlanSid;
	for(i=0;i<count;i++)
		{
		item=const_cast<CTestConfigItem*>(CfgFile()->Item(KUsimWlanSidInfo,i));
		if(!item)
			{
			break;
			}
		TPtrC8 wlanSidBuf;
		TInt userDef;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,wlanSidBuf);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_20, "WARNING - CONFIGURATION FILE PARSING - Reading element WLAN SID BUFFER returned %d (element no. %d) from tag %s.",ret,0,KUsimWlanSidInfo);
			continue;
			}
		else 
			{
			wlanSid.iWSID.Copy(wlanSidBuf);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,userDef);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_21, "WARNING - CONFIGURATION FILE PARSING - Reading element USER DEFINED returned %d (element no. %d) from tag %s.",ret,1,KUsimWlanSidInfo);
			continue;
			}
		else
			{
			wlanSid.iUserDefined=userDef;
			}
		iWlanSid->AppendL(wlanSid);
		}
	count=CfgFile()->ItemCount(KUsimPreferredNetworksInfo);
	item=NULL;
	RMobilePhone::TMobilePreferredNetworkEntryV3 networkEntry;
	for(i=0;i<count;i++)
		{
		item=const_cast<CTestConfigItem*>(CfgFile()->Item(KUsimPreferredNetworksInfo,i));
		if(!item)
			{
			break;
			}
		TPtrC8 accessId,countryCode,networkId;
		TInt userDef;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,accessId);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_22, "WARNING - CONFIGURATION FILE PARSING - Reading element ACCESS ID returned %d (element no. %d) from tag %s.",ret,0,KUsimPreferredNetworksInfo);
			continue;
			}
		else 
			{
			if(AsciiToNum(accessId, digit)==KErrNone)
				{
				networkEntry.iAccess=digit;
				}
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,userDef);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_23, "WARNING - CONFIGURATION FILE PARSING - Reading element USER DEFINED returned %d (element no. %d) from tag %s.",ret,1,KUsimPreferredNetworksInfo);
			continue;
			}
		else
			{
			networkEntry.iUserDefined=userDef;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,countryCode);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_24, "WARNING - CONFIGURATION FILE PARSING - Reading element COUNTRY CODE returned %d (element no. %d) from tag %s.",ret,1,KUsimPreferredNetworksInfo);
			continue;
			}
		else
			{
			networkEntry.iCountryCode.Copy(countryCode);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,networkId);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_25, "WARNING - CONFIGURATION FILE PARSING - Reading element NETWORK ID returned %d (element no. %d) from tag %s.",ret,1,KUsimPreferredNetworksInfo);
			continue;
			}
		else
			{
			networkEntry.iNetworkId.Copy(networkId);
			}
		
		iPreferredNetworks->AppendL(networkEntry);
		}	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_26, "Starting to Parse GBA Authentication Info");
	count = CfgFile()->ItemCount(KGBAAuthInfo);
	TGBAAuthInfo authInfo;

	TInt index;
	for(index = 0; index < count; index++)
		{
		item = const_cast<CTestConfigItem*>(CfgFile()->Item(KGBAAuthInfo, index));
		if(item == NULL)
			{
			break;
			}
		CleanupStack::PushL(item);
		TInt ret = KErrNone, authErr = KErrNone;
		TPtrC8 AUTN, RAND, appId, RES, AUTS, NAFID,IMPI, KSExtNaf, BtID, KeyLifeTime ;
		TBool OtherApplnBusy, applnActive;

		//Get the AUTN
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, AUTN);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_27, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTN returned %d (element no. %d) from tag %s.",ret,0,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAUTN.Copy(AUTN);
			}
		//Get the RAND
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 1, RAND);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_28, "WARNING - CONFIGURATION FILE PARSING - Reading element RAND returned %d (element no. %d) from tag %s.",ret,1,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iRAND.Copy(RAND);
			}
		//Get the APPID
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 2, appId);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_29, "WARNING - CONFIGURATION FILE PARSING - Reading element APPID returned %d (element no. %d) from tag %s.",ret,2,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAID.Copy(appId);
			}
		//Get the RES
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 3, RES);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_30, "WARNING - CONFIGURATION FILE PARSING - Reading element RES returned %d (element no. %d) from tag %s.",ret,3,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iRES.Copy(RES);
			}
		//Get the AUTS
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 4, AUTS);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_31, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTS returned %d (element no. %d) from tag %s.",ret,4,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAUTS.Copy(AUTS);
			}
		//Get the NAFID
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 5, NAFID);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_32, "WARNING - CONFIGURATION FILE PARSING - Reading element NAFID returned %d (element no. %d) from tag %s.",ret,5,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iNAFID.Copy(NAFID);
			}
		//Get the IMPI
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 6, IMPI);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_33, "WARNING - CONFIGURATION FILE PARSING - Reading element IMPI returned %d (element no. %d) from tag %s.",ret,6,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iIMPI.Copy(IMPI);
			}
		//Get the KsExtNaf
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 7, KSExtNaf);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_34, "WARNING - CONFIGURATION FILE PARSING - Reading element KSEXTNAF returned %d (element no. %d) from tag %s.",ret,7,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iKsExtNaf.Copy(KSExtNaf);
			}
		//Get the BtID
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 8, BtID);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_35, "WARNING - CONFIGURATION FILE PARSING - Reading element BTID returned %d (element no. %d) from tag %s.",ret,8,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iBtid.Copy(BtID);
			}
		//Get the KeyLifeTime
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 9, KeyLifeTime);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_36, "WARNING - CONFIGURATION FILE PARSING - Reading element KEYLIFETIME returned %d (element no. %d) from tag %s.",ret,9,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iKeyLifeTime.Copy(KeyLifeTime);
			}
		//Get the OtherApplnBusy
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 10, OtherApplnBusy);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_37, "WARNING - CONFIGURATION FILE PARSING - Reading element OTHERAPPLNBUSY returned %d (element no. %d) from tag %s.",ret,10,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iOtherAppBusy = OtherApplnBusy;
			}
		//Get the CurrentApplnActive
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 11, applnActive);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_38, "WARNING - CONFIGURATION FILE PARSING - Reading element APPLNACTIVE returned %d (element no. %d) from tag %s.",ret,11,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAppActive = applnActive;
			}
		//Get authentication error
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 12, authErr);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_39, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTHERR returned %d (element no. %d) from tag %s.",ret,12,KGBAAuthInfo);
			continue;
			}
		else
			{
			authInfo.iAuthErr = authErr;
			}
		iGBAAuthInfoList->AppendL(authInfo);
		CleanupStack::Pop(item);
		} // end FOR Loop
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_40, "Starting to Parse GBA 2 Phase Retrieval list information");
	count = CfgFile()->ItemCount(KGBAListInfo);
	RMobilePhone::TGbaNafEntryV8 entry;
	for(index = 0; index < count; index++)
		{
		item = const_cast<CTestConfigItem*>(CfgFile()->Item(KGBAListInfo, index));
		if(item == NULL)
			{
			break;
			}
		CleanupStack::PushL(item);
		TInt ret = KErrNone;
		TPtrC8 NafId, Btid;
		
		//Get the NafID
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, NafId);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_41, "WARNING - CONFIGURATION FILE PARSING - Reading element NAFID returned %d (element no. %d) from tag %s.",ret,0,KGBAListInfo);
			continue;
			}
		else
			{
			entry.iNafId.Copy(NafId);
			}

		//Get the Btid
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 1, Btid);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_42, "WARNING - CONFIGURATION FILE PARSING - Reading element BTID returned %d (element no. %d) from tag %s.",ret,1,KGBAListInfo);
			continue;
			}
		else
			{
			entry.iBtid.Copy(Btid);
			}
		
		iGbaNafList->AddEntryL(entry);
		CleanupStack::Pop(item);
		}	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_43, "Starting to Parse MBMS related config information");
	count = CfgFile()->ItemCount(KMBMSInfo);
	TMBMSInfo tMbmsInfo;
	for(index = 0; index < count; index++)
		{
		item = const_cast<CTestConfigItem*>(CfgFile()->Item(KMBMSInfo, index));
		if(item == NULL)
			{
			break;
			}
		CleanupStack::PushL(item);
		
		TInt ret = KErrNone;
		TPtrC8 iMikey,oMikey,mtk,saltKey,keyDmn, MskIdgrp;
		TPtrC8 MukId,MukIdi,MukIdr,MukTimeStamp,appId,KSIntNaf;
		TBool applnActive;
		TInt authErr;
		
		//Get the input Mikey
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, iMikey);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_44, "WARNING - CONFIGURATION FILE PARSING - Reading element IMIKEY returned %d (element no. %d) from tag %s.",ret,0,KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iInputMikey.Copy(iMikey);
			}
		
		//Get the output Mikey
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 1, oMikey);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_45, "WARNING - CONFIGURATION FILE PARSING - Reading element OMIKEY returned %d (element no. %d) from tag %s.",ret,1,KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iOutputMikey.Copy(oMikey);
			}

		//Get the MTK
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 2, mtk);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_46, "WARNING - CONFIGURATION FILE PARSING - Reading element MTK returned %d (element no. %d) from tag %s.",ret,2,KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMtk.Copy(mtk);
			}

		//Get the SaltKey
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 3, saltKey);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_47, "WARNING - CONFIGURATION FILE PARSING - Reading element SALTKEY returned %d (element no. %d) from tag %s.",ret,3,KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iSaltkey.Copy(saltKey);
			}

		//Get the KeyDomain
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 4, keyDmn );
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_48, "WARNING - CONFIGURATION FILE PARSING - Reading element KEYDMN  returned %d (element no. %d) from tag %s.",ret,4,KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iKeyDmn.Copy(keyDmn );
			}

		//Get the MskIdgrp
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 5, MskIdgrp);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_49, "WARNING - CONFIGURATION FILE PARSING - Reading element MSKIDGRP returned %d (element no. %d) from tag %s.",ret,5,KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMskIdGrp.Copy(MskIdgrp);
			}

		//Get the iMukid
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 6, MukId );
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_50, "WARNING - CONFIGURATION FILE PARSING - Reading element IMUKIDTLV  returned %d (element no. %d) from tag %s.",ret,6,KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMukIdTlv.Copy(MukId );
			}

		//Get the MukIdi
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 7, MukIdi);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_51, "WARNING - CONFIGURATION FILE PARSING - Reading element MUKIDI returned %d (element no. %d) from tag %s.",ret,7,KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMukIdi.Copy(MukIdi);
			}

		//Get the MukIdr 
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 8, MukIdr );
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_52, "WARNING - CONFIGURATION FILE PARSING - Reading element MUKIDR  returned %d (element no. %d) from tag %s.",ret,8,KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMukIdr.Copy(MukIdr );
			}

		//Get the MukTimeStamp
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 9, MukTimeStamp );
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_53, "WARNING - CONFIGURATION FILE PARSING - Reading element IMIKEY returned %d (element no. %d) from tag %s.",ret,9,KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iMukTimestamp.Copy(MukTimeStamp );
			}
		
		//Get the APPID
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 10, appId);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_54, "WARNING - CONFIGURATION FILE PARSING - Reading element APPID returned %d (element no. %d) from tag %s.", ret, 10, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iAID.Copy (appId);
			}

		//Get the KsIntNaf
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 11,KSIntNaf);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_55, "WARNING - CONFIGURATION FILE PARSING - Reading element KSINTNAF returned %d (element no. %d) from tag %s.", ret, 11, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iKsIntNaf.Copy (KSIntNaf);
			}

		//Get the CurrentApplnActive
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 12,applnActive);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_56, "WARNING - CONFIGURATION FILE PARSING - Reading element APPLNACTIVE returned %d (element no. %d) from tag %s.", ret, 12, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iAppActive = applnActive;
			}

		//Get authentication error
		ret = CTestConfig::GetElement (item->Value (), KStdDelimiter, 13,authErr);
		if ( ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_57, "WARNING - CONFIGURATION FILE PARSING - Reading element AUTHERR returned %d (element no. %d) from tag %s.", ret, 13, KMBMSInfo);
			continue;
			}
		else
			{
			tMbmsInfo.iAuthErr = authErr;
			}

		iMBMSInfoList->AppendL(tMbmsInfo);
		CleanupStack::Pop(item);
		}
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_58, "Starting to Parse MBMS 2 Phase Retrieval list information");
	count = CfgFile()->ItemCount(KMBMSListInfo);
	RMobilePhone::TMskEntryV8 mskEntry;
	for(index = 0; index < count; index++)
		{
		item = const_cast<CTestConfigItem*>(CfgFile()->Item(KMBMSListInfo, index));
		if(item == NULL)
			{
			break;
			}
		CleanupStack::PushL(item);
		TInt ret = KErrNone;
		TPtrC8 MskId,KeyDomain,TimeStamp;
		
		//Get the MskId
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 0, MskId);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_59, "WARNING - CONFIGURATION FILE PARSING - Reading element MSKID returned %d (element no. %d) from tag %s.",ret,0,KMBMSListInfo);
			continue;
			}
		else
			{
			mskEntry.iMskId.Copy(MskId);
			}

		//Get the KeyDomainGroup
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 1, KeyDomain);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_60, "WARNING - CONFIGURATION FILE PARSING - Reading element KEYDOMAIN returned %d (element no. %d) from tag %s.",ret,1,KMBMSListInfo);
			continue;
			}
		else
			{
			mskEntry.iKeyDomainId.Copy(KeyDomain);
			}
		
		//Get the TimeStampCounter
		
		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 2, TimeStamp);
		if(ret != KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_CONSTRUCTL_61, "WARNING - CONFIGURATION FILE PARSING - Reading element TIMESTAMPCOUNTER returned %d (element no. %d) from tag %s.",ret,2,KMBMSListInfo);
			continue;
			}
		else
			{
			mskEntry.iTimeStampCounter.Copy(TimeStamp);
			}

		iMbmsMskList->AddEntryL(mskEntry);
		CleanupStack::Pop(item);
		}
	if((iGBAAuthInfoList->Count()!=0) ||
			(iMBMSInfoList->Count() !=0 ) ||
			(iMessageWaiting->Count()!=0))
		{
		iListIndex=0;
		iTimer->Start(KUsimRel6DurationTimeoutDefault2,this,ETimerIdNotifyUSimRel6Event);
		}
	};

CSimUsimR6::~CSimUsimR6()
/**
* Trivial Destructor
* Closes all CObject type objects and destroys all other objects created in the ConstructL()
*
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_DTOR_1, "CSimUsimR6: Entered destructor");
	if(iTimer)
		{
		delete iTimer;
		}
	if(iWlanTimer)
		{
		delete iWlanTimer;
		}		
	if(iNotifyAuthTimer)
		{
		delete iNotifyAuthTimer;
		}
	if (iVoiceMailIds)
		{
		iVoiceMailIds->Delete(0,iVoiceMailIds->Count());
		delete iVoiceMailIds;
		}
	if (iMessageWaiting)
		{
		iMessageWaiting->Delete(0,iMessageWaiting->Count());
		delete iMessageWaiting;
		}
	if (iWlanData)
		{
		iWlanData->Delete(0,iWlanData->Count());
		delete iWlanData;
		}
	if (iWlanSid)
		{
		iWlanSid->Delete(0,iWlanSid->Count());
		delete iWlanSid;
		}
	if (iPreferredNetworks)
		{
		iPreferredNetworks->Delete(0,iPreferredNetworks->Count());
		delete iPreferredNetworks;
		}
	if(iGetWlanSIDsData)
		{
		iGetWlanSIDsData->ResetAndDestroy();
		}
	delete iGetWlanSIDsData;
	if(iGetPreferredNetworks)
		{
		iGetPreferredNetworks->ResetAndDestroy();	
		}
	delete iGetPreferredNetworks;
	if(iGBAAuthInfoList)
		{
		iGBAAuthInfoList->Delete(0,iGBAAuthInfoList->Count());
		delete iGBAAuthInfoList;
		}
	if(iAuthListInfo)
		{
		iAuthListInfo->ResetAndDestroy();
		}
	delete iAuthListInfo;
	if(iMBMSInfoList)
		{
		iMBMSInfoList->Delete(0,iMBMSInfoList->Count());
		delete iMBMSInfoList;	
		}
	if(iGbaNafList)
		{
		delete iGbaNafList;
		}
	if(iMbmsMskList)
		{
		delete iMbmsMskList;
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_DTOR_2, "CSimUsimR6: Leaving destructor");
	}

const CTestConfigSection* CSimUsimR6::CfgFile()
/**
 * Returns a pointer to the current configuration file section.
 *
 * @return CTestConfigSection	A pointer to the current configuration file data section.
 */
	{
	return iPhone->CfgFile();
	}

void CSimUsimR6::TimerCallBack(TInt /**aId*/)
/**
* Timer callback function.  When the timer goes off, it will call back into this
* function for further processing.
*
* @param aId an id identifying which timer callback is being called
*/
	{
	if (iMailBoxNumberChangePending)
		{
		iMailBoxNumberChangePending = EFalse;
		RMobilePhone::TMobilePhoneVoicemailIdsV3* mailbox = reinterpret_cast<RMobilePhone::TMobilePhoneVoicemailIdsV3*>(iMailBoxNumberChange);
		mailbox->iVoice = iVoiceMailIds->At(0).iVoice;
		mailbox->iData= iVoiceMailIds->At(0).iData;
		mailbox->iFax = iVoiceMailIds->At(0).iFax;
		mailbox->iOther = iVoiceMailIds->At(0).iOther;
		iPhone->ReqCompleted (iMailBoxNumberChangeNotificationReqHandle,KErrNone);
		iTimer->Start (KUsimRel6DurationTimeoutDefault2, this,ETimerIdNotifyUSimRel6Event);
		}

	else if (iMailBoxNumberChangePendingV8)
		{
		iMailBoxNumberChangePendingV8 = EFalse;
		RMobilePhone::TMobilePhoneVoicemailIdsV8* mailbox = reinterpret_cast<RMobilePhone::TMobilePhoneVoicemailIdsV8*>(iMailBoxNumberChangeV8);
		mailbox->iVoice = iVoiceMailIds->At(iCurrentNotifyMailboxIndex).iVoice;
		mailbox->iData= iVoiceMailIds->At(iCurrentNotifyMailboxIndex).iData;
		mailbox->iFax = iVoiceMailIds->At(iCurrentNotifyMailboxIndex).iFax;
		mailbox->iOther = iVoiceMailIds->At(iCurrentNotifyMailboxIndex).iOther;
		mailbox->iVideo= iVoiceMailIds->At(iCurrentNotifyMailboxIndex).iVideo;
		iPhone->ReqCompleted (iMailBoxNumberChangeNotificationReqHandleV8,KErrNone);
		iCurrentNotifyMailboxIndex++;
		iTimer->Start (KUsimRel6DurationTimeoutDefault2, this,ETimerIdNotifyUSimRel6Event);
		}

	else if (iIccMessageWaitingPending)
		{
		iIccMessageWaitingPending=EFalse;
		RMobilePhone::TMobilePhoneMessageWaitingV1* messageWaiting = reinterpret_cast<RMobilePhone::TMobilePhoneMessageWaitingV1*>(iIccMsgWaiting);
		messageWaiting->iDisplayStatus = iMessageWaiting->At(0).iDisplayStatus;
		messageWaiting->iVoiceMsgs= iMessageWaiting->At(0).iVoiceMsgs;
		messageWaiting->iAuxVoiceMsgs = iMessageWaiting->At(0).iAuxVoiceMsgs;
		messageWaiting->iDataMsgs = iMessageWaiting->At(0).iDataMsgs;
		messageWaiting->iFaxMsgs= iMessageWaiting->At(0).iFaxMsgs;
		messageWaiting->iEmailMsgs= iMessageWaiting->At(0).iEmailMsgs;
		messageWaiting->iOtherMsgs= iMessageWaiting->At(0).iOtherMsgs;
		iPhone->ReqCompleted (iIccMessageWaitingNotificationReqHandle,KErrNone);
		iTimer->Start (KUsimRel6DurationTimeoutDefault1, this,ETimerIdNotifyUSimRel6Event);
		}

	else if (iIccMessageWaitingPendingV8)
		{
		iIccMessageWaitingPendingV8=EFalse;
		RMobilePhone::TMobilePhoneMessageWaitingV8* messageWaiting = reinterpret_cast<RMobilePhone::TMobilePhoneMessageWaitingV8*>(iIccMsgWaitingV8);
		messageWaiting->iDisplayStatus = iMessageWaiting->At(iCurrentIccMessageWaitingIndex).iDisplayStatus;
		messageWaiting->iVoiceMsgs= iMessageWaiting->At(iCurrentIccMessageWaitingIndex).iVoiceMsgs;
		messageWaiting->iAuxVoiceMsgs = iMessageWaiting->At(iCurrentIccMessageWaitingIndex).iAuxVoiceMsgs;
		messageWaiting->iDataMsgs = iMessageWaiting->At(iCurrentIccMessageWaitingIndex).iDataMsgs;
		messageWaiting->iFaxMsgs= iMessageWaiting->At(iCurrentIccMessageWaitingIndex).iFaxMsgs;
		messageWaiting->iEmailMsgs= iMessageWaiting->At(iCurrentIccMessageWaitingIndex).iEmailMsgs;
		messageWaiting->iOtherMsgs= iMessageWaiting->At(iCurrentIccMessageWaitingIndex).iOtherMsgs;
		messageWaiting->iVideoMsgs= iMessageWaiting->At(iCurrentIccMessageWaitingIndex).iVideoMsgs;
		iPhone->ReqCompleted (iIccMessageWaitingNotificationReqHandleV8, KErrNone);
		iCurrentIccMessageWaitingIndex++;
		iTimer->Start (KUsimRel6DurationTimeoutDefault1, this,ETimerIdNotifyUSimRel6Event);
		}

	else if (iMessageWaitingPending)
		{
		iMessageWaitingPending=EFalse;
		*iMsgWaitingCount=6;
		iPhone->ReqCompleted (iMessageWaitingNotificationReqHandle, KErrNone);
		iTimer->Start (KUsimRel6DurationTimeoutDefault, this,ETimerIdNotifyUSimRel6Event);
		}

	else if (iNotifyPreferredWlanSIDListChangePending)
		{
		iNotifyPreferredWlanSIDListChangePending=EFalse;
		iPhone->ReqCompleted (iNotifyPreferredWlanSIDListReqHandle,KErrNone);
		iNotifyPreferredWlanSIDListChangeIndex++;
		iTimer->Start (KUsimRel6DurationTimeoutDefault1,this, ETimerIdNotifyUSimRel6Event);
		}

	else if ( iNotifyPreferredNetworksChangePending)
		{
		iNotifyPreferredNetworksChangePending=EFalse;
		iPhone->ReqCompleted (iNotifyPreferredNetworksReqHandle,KErrNone);
		iNotifyPreferredNetworksChangeIndex++;
		iTimer->Start (KUsimRel6DurationTimeoutDefault1, this,ETimerIdNotifyUSimRel6Event);
		}

	else if (iNotifyWlanDataChangePending)
		{
		iNotifyWlanDataChangePending=EFalse;
		if (iNotifyWlanDataChange->iFieldsUsed == iWlanData->At(0).iFieldsUsed)
			{
			iNotifyWlanDataChange->iPseudonym = iWlanData->At(0).iPseudonym;
			iNotifyWlanDataChange->iReauthenticationId = iWlanData->At(0).iReauthenticationId;
			}
		else if (iNotifyWlanDataChange->iFieldsUsed == iWlanData->At(1).iFieldsUsed)
			{
			iNotifyWlanDataChange->iPseudonym = iWlanData->At(1).iPseudonym;
			iNotifyWlanDataChange->iReauthenticationId = iWlanData->At(1).iReauthenticationId;
			iNotifyWlanDataChange->iMasterKey = iWlanData->At(1).iMasterKey;
			iNotifyWlanDataChange->iCounter = iWlanData->At(1).iCounter;
			}
		else
			{
			iNotifyWlanDataChange->iFieldsUsed = iWlanData->At(1).iFieldsUsed;
			iNotifyWlanDataChange->iPseudonym = iWlanData->At(1).iPseudonym;
			iNotifyWlanDataChange->iReauthenticationId = iWlanData->At(1).iReauthenticationId;
			iNotifyWlanDataChange->iMasterKey = iWlanData->At(1).iMasterKey;
			iNotifyWlanDataChange->iCounter = iWlanData->At(1).iCounter;	
			}
		iPhone->ReqCompleted (iNotifyWlanDataChangeReqHandle,KErrNone);
		iNotifyWlanDataChangeIndex++;
		}

	else if (iSetGBABootstrapParams.iNotifyPending ||
		  iNotifyAuthDataChange.iNotifyPending ||
		  iGetAuthParams.iNotifyPending)
		{
		if (iSetGBABootstrapParams.iNotifyPending)
			{
			RMobilePhone::TBsfDataV8
					&bsfDataV8 = *(RMobilePhone::TBsfDataV8*)iSetGBABootstrapParams.iNotifyData;

			if ((bsfDataV8.iBtid != iGBAAuthInfoList->At(iListIndex).iBtid)||(bsfDataV8.iKeyLifetime != iGBAAuthInfoList->At(iListIndex).iKeyLifeTime))
				{
				iPhone->ReqCompleted (
						iSetGBABootstrapParams.iNotifyHandle,
						KErrAccessDenied);
				}
			else
				{
				iPhone->ReqCompleted (
						iSetGBABootstrapParams.iNotifyHandle,
						KErrNone);
				}
			iSetGBABootstrapParams.iNotifyPending = EFalse;
			}

		if (iNotifyAuthDataChange.iNotifyPending)
			{
			iNotifyAuthDataChange.iNotifyPending = EFalse;
			RMobilePhone::TAuthInfoV8
					&authInfoV8 = *(RMobilePhone::TAuthInfoV8*) iNotifyAuthDataChange.iNotifyData;

			authInfoV8.iAID = iGBAAuthInfoList->At(iListIndex).iAID;

			if (iGBAAuthInfoList->At(iListIndex).iAppActive)
				{
				authInfoV8.iData = RMobilePhone::EGbaBootstrapParams;
				}
			else
				{
				authInfoV8.iData = RMobilePhone::EGbaNafIdList;
				}
			iPhone->ReqCompleted (iNotifyAuthDataChange.iNotifyHandle, KErrNone);
			}
		if (iGetAuthParams.iNotifyPending)
			{
			RMobilePhone::TMultimodeType
					&tMultimode = *(RMobilePhone::TMultimodeType*) iGetAuthParams.iNotifyData;
			TInt ret = iGBAAuthInfoList->At(iListIndex).iAuthErr;
			TInt ret1 = iMBMSInfoList->At(iListIndex).iAuthErr;
			if (ret == KErrNone || ret1 == KErrNone)
				{
				if (tMultimode.ExtensionId ()== RMobilePhone::KETelMobileGbaBootstrapParamsV8)
					{
					// for GBA
					RMobilePhone::TGbaBootstrapParamsV8
							&tGbaBootstrapV8 = *(RMobilePhone::TGbaBootstrapParamsV8*) iGetAuthParams.iNotifyData;
					tGbaBootstrapV8.iBtid = iGBAAuthInfoList->At(iListIndex).iBtid;
					tGbaBootstrapV8.iKeyLifetime = iGBAAuthInfoList->At(iListIndex).iKeyLifeTime;
					tGbaBootstrapV8.iRand = iGBAAuthInfoList->At(iListIndex).iRAND;
					}
				else
					if ( tMultimode.ExtensionId ()== RMobilePhone::KETelMobileMbmsUserKeyV8)
						{
						// for MBMS
						RMobilePhone::TMbmsUserKeyV8
								&tMbmsUserKeyV8 = *(RMobilePhone::TMbmsUserKeyV8*) iGetAuthParams.iNotifyData;
						tMbmsUserKeyV8.iMukIdi = iMBMSInfoList->At(iListIndex).iMukIdi;
						tMbmsUserKeyV8.iMukIdr = iMBMSInfoList->At(iListIndex).iMukIdr;
						tMbmsUserKeyV8.iTimeStampCounter = iMBMSInfoList->At(iListIndex).iMukTimestamp;
						}
					else
						{
						// handle error scenario.
						(void) tMultimode;
						iPhone->ReqCompleted (
								iGetAuthParams.iNotifyHandle,
								KErrCorrupt);
						}
				iPhone->ReqCompleted (iGetAuthParams.iNotifyHandle, KErrNone);
				}
			else
				{
				if (tMultimode.ExtensionId ()== RMobilePhone::KETelMobileMbmsUserKeyV8 && ret1 != KErrNone)
					{
					if (!(ret1 == KErrAccessDenied))
						{
						// for MBMS
						RMobilePhone::TMbmsUserKeyV8
								&tMbmsUserKeyV8 = *(RMobilePhone::TMbmsUserKeyV8*) iGetAuthParams.iNotifyData;
						tMbmsUserKeyV8.iMukIdi = iMBMSInfoList->At(iListIndex).iMukIdi;
						tMbmsUserKeyV8.iMukIdr = iMBMSInfoList->At(iListIndex).iMukIdr;
						tMbmsUserKeyV8.iTimeStampCounter = iMBMSInfoList->At(iListIndex).iMukTimestamp;
						}
					iPhone->ReqCompleted (iGetAuthParams.iNotifyHandle, ret1);
					}
				else
					{
					iPhone->ReqCompleted (iGetAuthParams.iNotifyHandle, ret);
					}
				}
			iGetAuthParams.iNotifyPending = EFalse;
			}
		iTimer->Start (RandTime (), this);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_TIMERCALLBACK_1, "<<CSimUsimR6::TimerCallBack - Exited ");
		}
	}
			
TInt CSimUsimR6::NotifyMailboxNumbersChange(TTsyReqHandle aReqHandle, TDes8* aMailBox)
/**
 * This function member notifies the client of changes to any of the Mailbox numbers identifiers.
 * @param aReqHandle	The request handle associated with this request.
 * @param aMailBox		This parameter is filled with new mailbox numbers identifiers.
 * @return TInt			Standard error value.
 */
	{
	TPckg<RMobilePhone::TMultimodeType>* paramPckg=(TPckg<RMobilePhone::TMultimodeType>*)aMailBox;
	RMobilePhone::TMultimodeType& paramclass=(*paramPckg)();
	if(paramclass.ExtensionId() == KETelExtMultimodeV3)
		{
		__ASSERT_ALWAYS(!iMailBoxNumberChangePending,SimPanic(ENotificationReqAlreadyOutstanding));
		RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg* voicemailPckg=(RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg*)aMailBox;
		RMobilePhone::TMobilePhoneVoicemailIdsV3& voicemail=(*voicemailPckg)();
		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(voicemail);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
		iMailBoxNumberChangePending=ETrue;
		iMailBoxNumberChangeNotificationReqHandle=aReqHandle;
		iMailBoxNumberChange=&voicemail;
		}
	if(paramclass.ExtensionId() == KEtelExtMultimodeV8)
		{
		__ASSERT_ALWAYS(!iMailBoxNumberChangePendingV8,SimPanic(ENotificationReqAlreadyOutstanding));
		RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg* voicemailV8Pckg=(RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg*)aMailBox;
		RMobilePhone::TMobilePhoneVoicemailIdsV8& voicemailV8=(*voicemailV8Pckg)();
		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(voicemailV8);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aReqHandle, err);
			return KErrNone;
			}
		iMailBoxNumberChangePendingV8=ETrue;
		iMailBoxNumberChangeNotificationReqHandleV8=aReqHandle;
		iMailBoxNumberChangeV8=&voicemailV8;
		}	
	else 
		{	
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;
		}
	return KErrNone;
	}

TInt CSimUsimR6::NotifyMailboxNumbersChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a previous request from an client to be notified of a change in mailbox numbers identifiers..
* @param aTsyReqHandle request handle
*/
	{
	if(iMailBoxNumberChangePending && iMailBoxNumberChangeNotificationReqHandle == aTsyReqHandle)
		{
		iMailBoxNumberChangePending=EFalse;
		iPhone->ReqCompleted(iMailBoxNumberChangeNotificationReqHandle,KErrCancel);
		}
	else if(iMailBoxNumberChangePendingV8 && iMailBoxNumberChangeNotificationReqHandleV8 == aTsyReqHandle)
		{
		iMailBoxNumberChangePendingV8=EFalse;
		iPhone->ReqCompleted(iMailBoxNumberChangeNotificationReqHandleV8,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimUsimR6::NotifyMessageWaiting(TTsyReqHandle aReqHandle, TInt* aCount)
/**
 * This function member allows a client to be notified when the phone receives 
 * a message waiting indication from the network.
 * @param aReqHandle	The request handle associated with this request.
 * @param aCount		This parameter is filled with the number of voice mail messages waiting in the network. 
 * @return TInt			Standard error value.
 */	
	{
	__ASSERT_ALWAYS(!iMessageWaitingPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iMessageWaitingPending=ETrue;
	iMessageWaitingNotificationReqHandle=aReqHandle;
	iMsgWaitingCount=aCount;
	return KErrNone;		
	};

TInt CSimUsimR6::NotifyMessageWaitingCancel(const TTsyReqHandle aReqHandle)
	{
	if(iMessageWaitingPending && iMessageWaitingNotificationReqHandle == aReqHandle)
		{
		iMessageWaitingPending=EFalse;
		iPhone->ReqCompleted(iMessageWaitingNotificationReqHandle,KErrCancel);
		}
	return KErrNone;
	};
		
TInt CSimUsimR6::NotifyIccMessageWaitingIndicatorsChange(TTsyReqHandle aReqHandle, TDes8* aMsgIndicators)
/**
 * This function member notifies the client if the message waiting indicators 
 * on the current ICC change.
 * @param aReqHandle	The request handle associated with this request.
 * @param aMsgIndicators  This parameter is filled with the message waiting indicators infromation. 
 * @return TInt			Standard error value.
 */	
	{
	TPckg<RMobilePhone::TMultimodeType>* paramPckg=(TPckg<RMobilePhone::TMultimodeType>*)aMsgIndicators;
	RMobilePhone::TMultimodeType& paramclass=(*paramPckg) ();
	if ( paramclass.ExtensionId ()== KETelExtMultimodeV1)
		{
		__ASSERT_ALWAYS(!iIccMessageWaitingPending,SimPanic(ENotificationReqAlreadyOutstanding));
		RMobilePhone::TMobilePhoneMessageWaitingV1Pckg* messagewaitingPckg=(RMobilePhone::TMobilePhoneMessageWaitingV1Pckg*)aMsgIndicators;
		RMobilePhone::TMobilePhoneMessageWaitingV1& messagewaiting=(*messagewaitingPckg) ();
		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion (messagewaiting);
		if ( err != KErrNone)
			{
			iPhone->ReqCompleted (aReqHandle, err);
			return KErrNone;
			}
		iIccMessageWaitingPending=ETrue;
		iIccMessageWaitingNotificationReqHandle=aReqHandle;
		iIccMsgWaiting=&messagewaiting;
		}
	if (paramclass.ExtensionId ()== KEtelExtMultimodeV8)
		{
		__ASSERT_ALWAYS(!iIccMessageWaitingPendingV8,SimPanic(ENotificationReqAlreadyOutstanding));
		RMobilePhone::TMobilePhoneMessageWaitingV8Pckg* messagewaitingPckg=(RMobilePhone::TMobilePhoneMessageWaitingV8Pckg*)aMsgIndicators;
		RMobilePhone::TMobilePhoneMessageWaitingV8& messagewaiting=(*messagewaitingPckg) ();
		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion (messagewaiting);
		if ( err != KErrNone)
			{
			iPhone->ReqCompleted (aReqHandle, err);
			return KErrNone;
			}
		iIccMessageWaitingPendingV8=ETrue;
		iIccMessageWaitingNotificationReqHandleV8=aReqHandle;
		iIccMsgWaitingV8=&messagewaiting;
		}
	else
		{
		iPhone->ReqCompleted (aReqHandle, KErrNotSupported);
		return KErrNone;
		}
	return KErrNone;
	}

TInt CSimUsimR6::NotifyIccMessageWaitingIndicatorsChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel a previous request from an client to be notified of a change in the message waiting indicators 
* on the current ICC.
* @param aTsyReqHandle request handle
*/
	{
	if ( iIccMessageWaitingPending && iIccMessageWaitingNotificationReqHandle == aTsyReqHandle)
		{
		iIccMessageWaitingPending=EFalse;
		iPhone->ReqCompleted (iIccMessageWaitingNotificationReqHandle, KErrCancel);
		}
	else
		if ( iIccMessageWaitingPendingV8 && iIccMessageWaitingNotificationReqHandleV8 == aTsyReqHandle)
			{
			iIccMessageWaitingPendingV8=EFalse;
			iPhone->ReqCompleted (iIccMessageWaitingNotificationReqHandleV8, KErrCancel);
			}
	return KErrNone;
	}

TInt CSimUsimR6::GetMailboxNumbers(TTsyReqHandle aReqHandle,TDes8* aMailBox)
/**
 * Retrieves the Mailbox numbers identifier information from the EF_MBI file in the USIM.
 * @param aReqHandle	The request handle associated with this request.
 * @param aMailBox		This will be populated with the
 *						mailbox identifier information to be returned.
 * @return TInt			Standard error value.
 */
	{
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg* voicemailPckg=(RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg*)aMailBox;
	RMobilePhone::TMultimodeType& packagetype = (*voicemailPckg)();
	
	if(packagetype.ExtensionId() == KETelExtMultimodeV3 || packagetype.ExtensionId() == KEtelExtMultimodeV8)
		{	
		RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg* voicemailPckg=(RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg*)aMailBox;
		RMobilePhone::TMobilePhoneVoicemailIdsV3& mailbox=(*voicemailPckg)();		
		mailbox.iVoice = iVoiceMailIds->At(iCurrentIndex).iVoice;
		mailbox.iData = iVoiceMailIds->At(iCurrentIndex).iData;
		mailbox.iFax = iVoiceMailIds->At(iCurrentIndex).iFax;
		mailbox.iOther = iVoiceMailIds->At(iCurrentIndex).iOther;
		}
	if(packagetype.ExtensionId() == KEtelExtMultimodeV8)
		{	
		RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg* voicemailV8Pckg=(RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg*)aMailBox;
		RMobilePhone::TMobilePhoneVoicemailIdsV8& voicemailV8=(*voicemailV8Pckg)();		
		voicemailV8.iVideo = iVoiceMailIds->At(iCurrentIndex).iVideo;
		}
	iCurrentIndex++;
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimUsimR6::GetIccMessageWaitingIndicators(TTsyReqHandle aReqHandle,TDes8* aMsgIndicators)
/**
 * This function member retrieves the set of message waiting indicators from the 
 * current ICC. 
 * @param aReqHandle	        The request handle associated with this request.
 * @param aMsgIndicators		The first parameter package.This will be populated with the
 *						        message waiting indicators  information to be returned.
 * @return TInt		        	Standard error value.
 */
	{
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg* messagewaitPckg=(RMobilePhone::TMobilePhoneMessageWaitingV1Pckg*)aMsgIndicators;
	RMobilePhone::TMultimodeType& packagetype = (*messagewaitPckg)();
	
	if(packagetype.ExtensionId() == KETelExtMultimodeV1 || packagetype.ExtensionId() == KEtelExtMultimodeV8)
		{	
		RMobilePhone::TMobilePhoneMessageWaitingV1Pckg* messagewaitingPckg=(RMobilePhone::TMobilePhoneMessageWaitingV1Pckg*)aMsgIndicators;
		RMobilePhone::TMobilePhoneMessageWaitingV1& messagewaiting=(*messagewaitingPckg)();		
		messagewaiting.iDisplayStatus=iMessageWaiting->At(iGetIccMsgIdCurrentIndex).iDisplayStatus;
		messagewaiting.iVoiceMsgs=iMessageWaiting->At(iGetIccMsgIdCurrentIndex).iVoiceMsgs;
		messagewaiting.iAuxVoiceMsgs=iMessageWaiting->At(iGetIccMsgIdCurrentIndex).iAuxVoiceMsgs;
		messagewaiting.iDataMsgs =iMessageWaiting->At(iGetIccMsgIdCurrentIndex).iDataMsgs;
		messagewaiting.iFaxMsgs=iMessageWaiting->At(iGetIccMsgIdCurrentIndex).iFaxMsgs;
		messagewaiting.iEmailMsgs=iMessageWaiting->At(iGetIccMsgIdCurrentIndex).iEmailMsgs;
		messagewaiting.iOtherMsgs=iMessageWaiting->At(iGetIccMsgIdCurrentIndex).iOtherMsgs;
		}
	if(packagetype.ExtensionId() == KEtelExtMultimodeV8)
		{	
		RMobilePhone::TMobilePhoneMessageWaitingV8Pckg* messagewaitingV8Pckg=(RMobilePhone::TMobilePhoneMessageWaitingV8Pckg*)aMsgIndicators;
		RMobilePhone::TMobilePhoneMessageWaitingV8& messagewaitingV8=(*messagewaitingV8Pckg)();		
		messagewaitingV8.iVideoMsgs=iMessageWaiting->At(iGetIccMsgIdCurrentIndex).iVideoMsgs;
		iGetIccMsgIdCurrentIndex++;
		}
	else
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);
		return KErrNone;		
		}
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimUsimR6::SetIccMessageWaitingIndicators(TTsyReqHandle aReqHandle,TDes8* aMsgIndicators)
/**
 * Sending information about the set of message waiting indicators from the 
 * current ICC.
 * @param aReqHandle	The request handle associated with this request.
 * @param aMsgIndicators		The first parameter package.This contains the
 *						message waiting indicators  information
 * @return TInt			Standard error value.
 */
	{
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg* messagewaitPckg=(RMobilePhone::TMobilePhoneMessageWaitingV1Pckg*)aMsgIndicators;
	RMobilePhone::TMultimodeType& packagetype = (*messagewaitPckg)();
	
	if(packagetype.ExtensionId() == KETelExtMultimodeV1)
		{	
		RMobilePhone::TMobilePhoneMessageWaitingV1Pckg* messagewaitingPckg=(RMobilePhone::TMobilePhoneMessageWaitingV1Pckg*)aMsgIndicators;
		RMobilePhone::TMobilePhoneMessageWaitingV1& messagewaiting=(*messagewaitingPckg)();		

		if ((messagewaiting.iVoiceMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iVoiceMsgs) ||
			(messagewaiting.iAuxVoiceMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iAuxVoiceMsgs) ||
			(messagewaiting.iDataMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iDataMsgs) ||
			(messagewaiting.iFaxMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iFaxMsgs) ||
			(messagewaiting.iEmailMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iEmailMsgs)  ||
			(messagewaiting.iOtherMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iOtherMsgs))
			{
			iPhone->ReqCompleted(aReqHandle,KErrCorrupt); //KErrCorrupt
			}
		else
			{	
			iPhone->ReqCompleted(aReqHandle,KErrNone);
			}
		}
		
	if(packagetype.ExtensionId() == KEtelExtMultimodeV8)
		{	
		RMobilePhone::TMobilePhoneMessageWaitingV8Pckg* messagewaitingV8Pckg=(RMobilePhone::TMobilePhoneMessageWaitingV8Pckg*)aMsgIndicators;
		RMobilePhone::TMobilePhoneMessageWaitingV8& messagewaitingV8=(*messagewaitingV8Pckg)();		

		if ((messagewaitingV8.iVoiceMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iVoiceMsgs) ||
			(messagewaitingV8.iAuxVoiceMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iAuxVoiceMsgs) ||
			(messagewaitingV8.iDataMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iDataMsgs) ||
			(messagewaitingV8.iFaxMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iFaxMsgs) ||
			(messagewaitingV8.iEmailMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iEmailMsgs)  ||
			(messagewaitingV8.iOtherMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iOtherMsgs) ||
			(messagewaitingV8.iVideoMsgs != iMessageWaiting->At(iSetIccMsgIdCurrentIndex).iVideoMsgs))
			{
			iPhone->ReqCompleted(aReqHandle,KErrCorrupt); //KErrCorrupt
			}
		else
			{	
			iSetIccMsgIdCurrentIndex++;
			iPhone->ReqCompleted(aReqHandle,KErrNone);
			}
		}
	else
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotSupported);		
		}
	return KErrNone;	
	}

//
// Wireless LAN (WLAN)
//

TInt CSimUsimR6::NotifyWlanDataChange(const TTsyReqHandle aTsyReqHandle,TDes8* aData)
/**
 * Notification request posted by the client is completed by the TSY when there is a change in 
 * the Wlan Data. This API upon completion returns the current Wlan Data to the client.
 * @param aReqHandle	The request handle associated with this request.
 * @param aData			This contains the current Wlan data upon completion.
 * @return TInt			Standard error value.
 */
	{
	__ASSERT_ALWAYS(!iNotifyWlanDataChangePending,SimPanic(ENotificationReqAlreadyOutstanding));
	RMobilePhone::TUsimWlanDataV8Pckg *WlanDataV8Pckg = reinterpret_cast<RMobilePhone::TUsimWlanDataV8Pckg*> (aData);
	RMobilePhone::TUsimWlanDataV8 &WlanDataV8 = (*WlanDataV8Pckg)();
	iNotifyWlanDataChangeReqHandle=aTsyReqHandle;
	iNotifyWlanDataChange=&WlanDataV8;
	if(iIndex%2==0)
		{
		iNotifyWlanDataChangePending=ETrue;
		iWlanTimer->Start(KUsimRel6DurationTimeoutDefault,this,ETimerIdNotifyWlanDataEvent);
		}
	iIndex++;
	return KErrNone;	
	}

TInt CSimUsimR6::NotifyWlanDataChangeCancel(TTsyReqHandle aTsyReqHandle)
/**
 * This API is used by the TSY to cancel any pending NotifyWlanDataChange request. 
 * @param aReqHandle	The request handle associated with this request.
 * @return TInt			Standard error value.
 */
	{
	if(iNotifyWlanDataChangeReqHandle==aTsyReqHandle)
		{
		iNotifyWlanDataChangePending=EFalse;
		iPhone->ReqCompleted(iNotifyWlanDataChangeReqHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimUsimR6::SetWlanData(const TTsyReqHandle aTsyReqHandle,TDes8* aData)
/**
 * This API is used by the TSY to receive the Wlan data sent by the client.
 * @param aReqHandle	The request handle associated with this request.
 * @param aData			This contains the Wlan Data.
 * @return TInt			Standard error value.
 */
	{
	RMobilePhone::TUsimWlanDataV8Pckg *WlanDataV8Pckg = reinterpret_cast<RMobilePhone::TUsimWlanDataV8Pckg*> (aData);
	RMobilePhone::TUsimWlanDataV8 &WlanDataV8 = (*WlanDataV8Pckg)();
	
	if(WlanDataV8.iPseudonym==SIMTSY_PHONE_WLAN_ACESS_DENIED)
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrAccessDenied);
		}
	else if(WlanDataV8.iFieldsUsed == iWlanData->At(0).iFieldsUsed)
		{
		if((WlanDataV8.iPseudonym != iWlanData->At(0).iPseudonym) ||
			(WlanDataV8.iReauthenticationId != iWlanData->At(0).iReauthenticationId))
			{
			iPhone->ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	else if(WlanDataV8.iFieldsUsed == iWlanData->At(1).iFieldsUsed)
		{	
		if((WlanDataV8.iPseudonym != iWlanData->At(1).iPseudonym) ||
			(WlanDataV8.iReauthenticationId != iWlanData->At(1).iReauthenticationId) ||
			(WlanDataV8.iMasterKey != iWlanData->At(1).iMasterKey) ||
			(WlanDataV8.iCounter != iWlanData->At(1).iCounter))
			{
			iPhone->ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
			}		
		}
	else
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;	
	}

TInt CSimUsimR6::GetWlanData(const TTsyReqHandle aTsyReqHandle,TDes8* aData)
/**
 * This API is used by the TSY to send the Wlan data information requested by the client
 * @param aReqHandle	The request handle associated with this request.
 * @param aData			This contains the current Wlan data upon completion.
 * @return TInt			Standard error value.
 */
	{
	RMobilePhone::TUsimWlanDataV8Pckg *WlanDataV8Pckg = reinterpret_cast<RMobilePhone::TUsimWlanDataV8Pckg*> (aData);
	RMobilePhone::TUsimWlanDataV8 &WlanDataV8 = (*WlanDataV8Pckg)();

	if(WlanDataV8.iPseudonym==SIMTSY_PHONE_WLAN_ACESS_DENIED)
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrAccessDenied);
		}
	else if(WlanDataV8.iCounter == iWlanData->At(2).iCounter)
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrNotSupported);
		}
	else if(WlanDataV8.iFieldsUsed ==iWlanData->At(0).iFieldsUsed )
		{
		WlanDataV8.iPseudonym = iWlanData->At(0).iPseudonym;
		WlanDataV8.iReauthenticationId = iWlanData->At(0).iReauthenticationId;
		iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else if(WlanDataV8.iFieldsUsed ==iWlanData->At(1).iFieldsUsed )
		{
		WlanDataV8.iPseudonym = iWlanData->At(1).iPseudonym;
		WlanDataV8.iReauthenticationId = iWlanData->At(1).iReauthenticationId;
		WlanDataV8.iMasterKey = iWlanData->At(1).iMasterKey;
		WlanDataV8.iCounter = iWlanData->At(1).iCounter ;
		iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else
		{
		WlanDataV8.iFieldsUsed =iWlanData->At(1).iFieldsUsed; 
		WlanDataV8.iPseudonym = iWlanData->At(1).iPseudonym;
		WlanDataV8.iReauthenticationId = iWlanData->At(1).iReauthenticationId;
		WlanDataV8.iMasterKey = iWlanData->At(1).iMasterKey;
		WlanDataV8.iCounter = iWlanData->At(1).iCounter ;
		iPhone->ReqCompleted(aTsyReqHandle,KErrNone);	
		}
	return KErrNone;	
	}

TInt CSimUsimR6::StorePreferredWlanSIDList(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
/**
 * This API is used by the TSY to store the Preferred Wlan SID List sent by the Client.
 * @param aReqHandle	The request handle associated with this request.
 * @param aBuffer		This contains the Preferred Wlan SID List.
 * @return TInt			Standard error value.
 */
	{
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessStorePreferredWlanSIDListL(aTsyReqHandle, aBuffer););
	if (leaveCode != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	}

TInt CSimUsimR6::ProcessStorePreferredWlanSIDListL(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
/**
 * This API is used by the TSY to process the Preferred Wlan SID List sent by the Client and verify it.
 * @param aReqHandle	The request handle associated with this request.
 * @param aBuffer		This contains the Preferred Wlan SID List.
 * @return TInt			Standard error value.
 */
	{
	CMobilePhoneStoredWlanSIDList* list=CMobilePhoneStoredWlanSIDList::NewL();
	CleanupStack::PushL(list);
	list->RestoreL(*aBuffer);
	RMobilePhone::TWlanSIDV8 entry;
	if (list->Enumerate() > SIMTSY_PHONE_WLAN_SIDLIST_COUNT)
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrNoMemory);
		CleanupStack::PopAndDestroy(); // list
		return KErrNone;
		}
	TInt error=KErrCorrupt;
	TInt ret;
	for (TInt i=0; i<SIMTSY_PHONE_WLAN_SIDLIST_COUNT; ++i)
		{
		TRAP(ret,entry=list->GetEntryL(i));
		if (ret != KErrNone)
			{
			break;
			}
		switch(i)
			{
		case 0:
			if((entry.iWSID == iWlanSid->At(0).iWSID||entry.iWSID != iWlanSid->At(1).iWSID ||entry.iWSID != iWlanSid->At(2).iWSID)
			   && (entry.iUserDefined == iWlanSid->At(0).iUserDefined || entry.iUserDefined == iWlanSid->At(1).iUserDefined || 
			   entry.iUserDefined == iWlanSid->At(2).iUserDefined))
				{
				error  = KErrNone;
				}
			break;
		case 1:
			if((entry.iWSID == iWlanSid->At(0).iWSID||entry.iWSID != iWlanSid->At(1).iWSID ||entry.iWSID != iWlanSid->At(2).iWSID)
			   && (entry.iUserDefined == iWlanSid->At(0).iUserDefined || entry.iUserDefined == iWlanSid->At(1).iUserDefined ||
			    entry.iUserDefined == iWlanSid->At(2).iUserDefined))
				{
				error  = KErrNone;
				}
			break;
		case 2:
		default:
			if((entry.iWSID == iWlanSid->At(0).iWSID||entry.iWSID != iWlanSid->At(1).iWSID ||entry.iWSID != iWlanSid->At(2).iWSID)
			   && (entry.iUserDefined == iWlanSid->At(0).iUserDefined || entry.iUserDefined == iWlanSid->At(1).iUserDefined ||
			    entry.iUserDefined == iWlanSid->At(2).iUserDefined))
				{
				error  = KErrNone;
				}
			break;
			}
		}
	if(error == KErrNone)	
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrNone);	
		}
	else
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	CleanupStack::PopAndDestroy(); 
	return KErrNone;
	}

TInt CSimUsimR6::NotifyPreferredWlanSIDListChange(TTsyReqHandle aTsyReqHandle)
/**
 * This API is used by the TSY to complete the Preferred Wlan SID List change
 * notification request posted by the client.
 * 
 * @param aReqHandle	The request handle associated with this request.
 * @return TInt			Standard error value.
 */
	{
	iNotifyPreferredWlanSIDListChangePending=ETrue;
	iNotifyPreferredWlanSIDListReqHandle=aTsyReqHandle;
	return KErrNone;
	}
	
TInt CSimUsimR6::NotifyPreferredWlanSIDListChangeCancel(TTsyReqHandle /**aTsyReqHandle*/)
/**
 * This API is used by TSY to cancel any NotifyPreferredWlanSIDListChange outstanding requests 
 * @return TInt			Standard error value.
 */
	{
	iNotifyPreferredWlanSIDListChangePending=EFalse;
	iPhone->ReqCompleted(iNotifyPreferredWlanSIDListReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSimUsimR6::GetPreferredWlanSIDsPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize)
/** 
 * Tsy uses this API to retrieve the Preferred Wlan SIDs of each line from phone, 
 * store each Preferred Wlan SID as a list entry,
 * stream the list and then return size of this buffer to client.
 * @param aReqHandle  Handle to the request.
 * @param aClient     Pointer to the client.
 * @param aBufSize    Size of the buffer the client has to allocate for the 2nd pahase.
 */
	{
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetPreferredWlanSIDsPhase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	}	
	
TInt CSimUsimR6::ProcessGetPreferredWlanSIDsPhase1L(const TTsyReqHandle aTsyReqHandle, 
													 RMobilePhone::TClientId* aClient, 
													 TInt* aBufSize)
/**
* Retrieve stored Wlan specific IDs from USIM.
* Store each entry,stream the list and then return size of this buffer to client
* @param aReqHandle  Handle to the request.
* @param aClient     Pointer to the client.
* @param aBufSize    Size of the buffer the client has to allocate for the 2nd pahase.
*/	
	{
	CMobilePhoneStoredWlanSIDList* list=CMobilePhoneStoredWlanSIDList::NewL();
	CleanupStack::PushL(list);
	RMobilePhone::TWlanSIDV8 entry;
	// fill up an example list
	for (TInt index=0; index < SIMTSY_PHONE_WLAN_SIDLIST_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iWSID = iWlanSid->At(0).iWSID;
		    entry.iUserDefined = iWlanSid->At(0).iUserDefined;
			break;
		case 1:
			entry.iWSID = iWlanSid->At(1).iWSID;
		    entry.iUserDefined = iWlanSid->At(1).iUserDefined;
			break;
		case 2:
		default:
			entry.iWSID = iWlanSid->At(2).iWSID ;
		    entry.iUserDefined = iWlanSid->At(2).iUserDefined;
			break;
			}
		// Add the entry into the list, at the next empty location
		list->AddEntryL(entry);
		}
	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(*aClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC
	
	iGetWlanSIDsData->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CSimUsimR6::GetPreferredWlanSIDsPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
/**
* 2nd phase retrieval of the the Preferred Wlan SIDs list.
* @param aReqHandle   Handle to the request.
* @param aClient      Pointer to the client.
* @param aBuf Buffer  Containing the Preferred Wlan SIDs list.
*/
	{
	CListReadAllAttempt* read=NULL;
	// Find the get preferred wlan SIDs from this client
	for (TInt i=0; i<iGetWlanSIDsData->Count(); ++i)
		{
		read = iGetWlanSIDsData->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetWlanSIDsData->Delete(i);
			iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CSimUsimR6::GetPreferredWlanSIDsCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel the request to retrieve the Preferred Wlan SIDs List.
* @param aTsyReqHandle handle to the request
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_GETPREFERREDWLANSIDSCANCEL_1, "CSimPhone::GetPreferredNetworksCancel");
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	// Remove the read all attempt from iGetWlanSIDsData
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetWlanSIDsData->Count(); ++i)
		{
		read = iGetWlanSIDsData->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetWlanSIDsData->Delete(i);
			break;
			}
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_GETPREFERREDWLANSIDSCANCEL_2, "CSimPhone::GetPreferredWlanSIDsCancel");
	return KErrNone;
	}
	
TInt CSimUsimR6::GetPreferredNetworksPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize)
/** 
 * Tsy uses this API to retrieve the Preferred Networks of each line from phone, 
 * store each Preferred Networks as a list entry,
 * stream the list and then return size of this buffer to client.
 * @param aReqHandle  Handle to the request.
 * @param aClient     Pointer to the client.
 * @param aBufSize    Size of the buffer the client has to allocate for the 2nd pahase.
 */
	{
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetPreferredNetworksPhase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	}	
	
TInt CSimUsimR6::ProcessGetPreferredNetworksPhase1L(const TTsyReqHandle aTsyReqHandle, 
													 RMobilePhone::TClientId* aClient, 
													 TInt* aBufSize)
/**
* Retrieve stored Preferred Networks from USIM.
* Store each entry,stream the list and then return size of this buffer to client
* @param aReqHandle  Handle to the request.
* @param aClient     Pointer to the client.
* @param aBufSize    Size of the buffer the client has to allocate for the 2nd pahase.
*/
	{
	CMobilePhoneStoredNetworkList* list=CMobilePhoneStoredNetworkList::NewL();
	CleanupStack::PushL(list);
	RMobilePhone::TMobilePreferredNetworkEntryV3 entry;
	// fill up an example list
	for (TInt index=0; index < SIMTSY_PHONE_NTWK_LIST_ENTRIES; index++)
		{
		switch (index)
			{
		case 0:
			entry.iAccess = iPreferredNetworks->At(0).iAccess;
			entry.iUserDefined = iPreferredNetworks->At(0).iUserDefined;
			entry.iCountryCode = iPreferredNetworks->At(0).iCountryCode;
			entry.iNetworkId = iPreferredNetworks->At(0).iNetworkId;
			break;
		case 1:
			entry.iAccess = iPreferredNetworks->At(1).iAccess;
			entry.iUserDefined = iPreferredNetworks->At(1).iUserDefined;
			entry.iCountryCode = iPreferredNetworks->At(1).iCountryCode;
			entry.iNetworkId = iPreferredNetworks->At(1).iNetworkId;
			break;
		case 2:
		default:
			entry.iAccess = iPreferredNetworks->At(2).iAccess;
			entry.iUserDefined = iPreferredNetworks->At(2).iUserDefined;
			entry.iCountryCode = iPreferredNetworks->At(2).iCountryCode;
			entry.iNetworkId = iPreferredNetworks->At(2).iNetworkId;
			break;
		case 3:
			entry.iAccess = iPreferredNetworks->At(3).iAccess;
			entry.iUserDefined = iPreferredNetworks->At(3).iUserDefined;
			entry.iCountryCode = iPreferredNetworks->At(3).iCountryCode;
			entry.iNetworkId = iPreferredNetworks->At(3).iNetworkId;
			break;
			}
		// Add the entry into the list, at the next empty location
		list->AddEntryL(entry);
		}
	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(*aClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC
	
	iGetPreferredNetworks->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CSimUsimR6::GetPreferredNetworksPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
/**
* 2nd phase retrieval of the the Preferred Networks list.
* @param aReqHandle   Handle to the request.
* @param aClient      Pointer to the client.
* @param aBuf Buffer  Containing the Preferred Networks list.
*/
	{
	CListReadAllAttempt* read=NULL;
	// Find the get detected network attempt from this client
	for (TInt i=0; i<iGetPreferredNetworks->Count(); ++i)
		{
		read = iGetPreferredNetworks->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetPreferredNetworks->Delete(i);
			iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CSimUsimR6::GetPreferredNetworksCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Cancel the request to retrieve the Preferred Networks List.
* @param aTsyReqHandle handle to the request
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_GETPREFERREDNETWORKSCANCEL_1, "CSimPhone::GetPreferredNetworksCancel");
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetPreferredNetworks->Count(); ++i)
		{
		read = iGetPreferredNetworks->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetPreferredNetworks->Delete(i);
			break;
			}
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMUSIMR6_GETPREFERREDNETWORKSCANCEL_2, "CSimPhone::GetPreferredNetworksCancel");
	return KErrNone;
	}
		
TInt CSimUsimR6::StorePreferredNetworksList(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
/**
 * This API is used by the TSY to store the Preferred Networks List sent by the Client.
 * @param aReqHandle	The request handle associated with this request.
 * @param aBuffer		This contains the Preferred Networks List.
 * @return TInt			Standard error value.
 */
	{
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessStorePreferredNetworksListL(aTsyReqHandle, aBuffer););
	if (leaveCode != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	};

TInt CSimUsimR6::ProcessStorePreferredNetworksListL(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
/**
 * This API is used by the TSY to process the Preferred Networks List sent by the Client and verify it.
 * @param aReqHandle	The request handle associated with this request.
 * @param aBuffer		This contains the Preferred Networks List.
 * @return TInt			Standard error value.
 */
	{
	CMobilePhoneStoredNetworkList* list=CMobilePhoneStoredNetworkList::NewL();
	CleanupStack::PushL(list);
	
	list->RestoreL(*aBuffer);

	RMobilePhone::TMobilePreferredNetworkEntryV3 entry;

	if (list->Enumerate() > SIMTSY_PHONE_NTWK_LIST_ENTRIES)
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrNoMemory);
		CleanupStack::PopAndDestroy(); // list
		return KErrNone;
		}
	TInt error=KErrCorrupt;
	TInt ret;
	for (TInt i=0; i<SIMTSY_PHONE_NTWK_LIST_ENTRIES; ++i)
		{
		TRAP(ret,entry=list->GetEntryL(i));
		if (ret != KErrNone)
			{
			break;
			}
			switch(i)
				{
			case 0:
				if((entry.iAccess == iPreferredNetworks->At(0).iAccess ||
				entry.iAccess == iPreferredNetworks->At(1).iAccess ||
				entry.iAccess == iPreferredNetworks->At(2).iAccess ||
				entry.iAccess == iPreferredNetworks->At(3).iAccess ) && 
				(entry.iUserDefined == iPreferredNetworks->At(0).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(1).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(2).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(3).iUserDefined) && 
			    (entry.iCountryCode == iPreferredNetworks->At(0).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(1).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(2).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(3).iCountryCode ) &&
				(entry.iNetworkId == iPreferredNetworks->At(0).iNetworkId ||
				entry.iNetworkId == iPreferredNetworks->At(1).iNetworkId || 
				entry.iNetworkId == iPreferredNetworks->At(2).iNetworkId || 
				entry.iNetworkId == iPreferredNetworks->At(3).iNetworkId ))
					{
					error  = KErrNone;
					}
				break;
			case 1:
				if((entry.iAccess == iPreferredNetworks->At(0).iAccess ||
				entry.iAccess == iPreferredNetworks->At(1).iAccess ||
				entry.iAccess == iPreferredNetworks->At(2).iAccess ||
				entry.iAccess == iPreferredNetworks->At(3).iAccess ) && 
				(entry.iUserDefined == iPreferredNetworks->At(0).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(1).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(2).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(3).iUserDefined) && 
			    (entry.iCountryCode == iPreferredNetworks->At(0).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(1).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(2).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(3).iCountryCode ) &&
				(entry.iNetworkId == iPreferredNetworks->At(0).iNetworkId ||
				entry.iNetworkId == iPreferredNetworks->At(1).iNetworkId || 
				entry.iNetworkId == iPreferredNetworks->At(2).iNetworkId || 
				entry.iNetworkId == iPreferredNetworks->At(3).iNetworkId ))
					{
					error  = KErrNone;
					}
				break;
			case 2:
				if((entry.iAccess == iPreferredNetworks->At(0).iAccess ||
				entry.iAccess == iPreferredNetworks->At(1).iAccess ||
				entry.iAccess == iPreferredNetworks->At(2).iAccess ||
				entry.iAccess == iPreferredNetworks->At(3).iAccess ) && 
				(entry.iUserDefined == iPreferredNetworks->At(0).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(1).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(2).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(3).iUserDefined) && 
			    (entry.iCountryCode == iPreferredNetworks->At(0).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(1).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(2).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(3).iCountryCode ) &&
				(entry.iNetworkId == iPreferredNetworks->At(0).iNetworkId ||
				entry.iNetworkId == iPreferredNetworks->At(1).iNetworkId || 
				entry.iNetworkId == iPreferredNetworks->At(2).iNetworkId || 
				entry.iNetworkId == iPreferredNetworks->At(3).iNetworkId ))
					{
					error  = KErrNone;
					}
				break;
			case 3:
			default:
				if((entry.iAccess == iPreferredNetworks->At(0).iAccess ||
				entry.iAccess == iPreferredNetworks->At(1).iAccess ||
				entry.iAccess == iPreferredNetworks->At(2).iAccess ||
				entry.iAccess == iPreferredNetworks->At(3).iAccess ) && 
				(entry.iUserDefined == iPreferredNetworks->At(0).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(1).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(2).iUserDefined ||
				entry.iUserDefined == iPreferredNetworks->At(3).iUserDefined) && 
			    (entry.iCountryCode == iPreferredNetworks->At(0).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(1).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(2).iCountryCode  ||
				entry.iCountryCode == iPreferredNetworks->At(3).iCountryCode ) &&
				(entry.iNetworkId == iPreferredNetworks->At(0).iNetworkId ||
				entry.iNetworkId == iPreferredNetworks->At(1).iNetworkId || 
				entry.iNetworkId == iPreferredNetworks->At(2).iNetworkId || 
				entry.iNetworkId == iPreferredNetworks->At(3).iNetworkId ))
					{
					error  = KErrNone;
					}
				break;
			}
		}
	if(error==KErrNone)	
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	CleanupStack::PopAndDestroy(); // list
	return KErrNone;
	}
	
TInt CSimUsimR6::NotifyStorePreferredNetworksListChange(TTsyReqHandle aTsyReqHandle)
/**
 * This API is used by the TSY to complete the Preferred Networks List change
 * notification request posted by the client.
 * 
 * @param aReqHandle	The request handle associated with this request.
 * @return TInt			Standard error value.
 */
	{
	iNotifyPreferredNetworksChangePending=ETrue;
	iNotifyPreferredNetworksReqHandle=aTsyReqHandle;
	return KErrNone;
	}
	
TInt CSimUsimR6::NotifyStorePreferredNetworksListChangeCancel(TTsyReqHandle /**aTsyReqHandle*/)
/**
 * This API is used by TSY to cancel any NotifyPreferredNetworksListChange outstanding requests 

 * @return TInt			Standard error value.
 */
	{
	iNotifyPreferredNetworksChangePending=EFalse;
	iPhone->ReqCompleted(iNotifyPreferredNetworksReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CSimUsimR6::SetGbaBootstrapParams(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg, const RMobilePhone::TAID* aAID)
	{
	
	RMobilePhone::TBsfDataV8Pckg *bsfDataV8Pckg = reinterpret_cast<RMobilePhone::TBsfDataV8Pckg*> (aPckg);
	RMobilePhone::TBsfDataV8 &bsfDataV8 = (*bsfDataV8Pckg) ();
	if((aAID == NULL) || (*aAID == iGBAAuthInfoList->At(iListIndex).iAID))
		{
		iSetGBABootstrapParams.iNotifyPending = ETrue;
		iSetGBABootstrapParams.iNotifyHandle = aTsyReqHandle;
		iSetGBABootstrapParams.iNotifyData = &bsfDataV8;
		}
	return KErrNone;
	}

TInt CSimUsimR6::SetGbaBootstrapParamsCancel(const TTsyReqHandle /**aTsyReqHandle*/)
	{
	if(iSetGBABootstrapParams.iNotifyPending)
		{
		iSetGBABootstrapParams.iNotifyPending=EFalse;
		iPhone->ReqCompleted(iSetGBABootstrapParams.iNotifyHandle,KErrCancel);
		}
	else
		{
		iPhone->ReqCompleted(iSetGBABootstrapParams.iNotifyHandle,KErrNone);
		}
	return KErrNone;
	}


TInt CSimUsimR6::NotifyAuthenticateDataChange(TTsyReqHandle aTsyReqHandle, TDes8* aPckg)
	{
	RMobilePhone::TAuthInfoV8Pckg *authInfV8Pckg = reinterpret_cast<RMobilePhone::TAuthInfoV8Pckg*> (aPckg);
	RMobilePhone::TAuthInfoV8 &authInfoV8 = (*authInfV8Pckg)();
	iNotifyAuthDataChange.iNotifyPending = ETrue;
	iNotifyAuthDataChange.iNotifyHandle = aTsyReqHandle;
	iNotifyAuthDataChange.iNotifyData = &authInfoV8;
	return KErrNone;
	}

TInt CSimUsimR6::NotifyAuthenticateDataChangeCancel(const TTsyReqHandle /**aTsyReqHandle*/)
	{
	if(iNotifyAuthDataChange.iNotifyPending)
		{
		iNotifyAuthDataChange.iNotifyPending=EFalse;
		iPhone->ReqCompleted(iNotifyAuthDataChange.iNotifyHandle,KErrCancel);
		}
	else
		{
		iPhone->ReqCompleted(iNotifyAuthDataChange.iNotifyHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CSimUsimR6::GetAuthenticationParams(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg, TDes8* aPckg2)
	{
	RMobilePhone::TAuthInfoV8Pckg *authInfoV8Pckg = reinterpret_cast<RMobilePhone::TAuthInfoV8Pckg*> (aPckg);
	RMobilePhone::TAuthInfoV8 &authInfoV8 = (*authInfoV8Pckg)();
	if ( authInfoV8.iAID == iGBAAuthInfoList->At(iListIndex).iAID ||
			authInfoV8.iAID == iMBMSInfoList->At(iListIndex).iAID )
		{
		iGetAuthParams.iNotifyPending = ETrue;
		iGetAuthParams.iNotifyHandle = aTsyReqHandle;
		if ( authInfoV8.iData == RMobilePhone::EGbaBootstrapParams)
			{
			RMobilePhone::TGbaBootstrapParamsV8Pckg
					*gbaBootstrapV8Pckg = reinterpret_cast<RMobilePhone::TGbaBootstrapParamsV8Pckg*> (aPckg2);
			RMobilePhone::TGbaBootstrapParamsV8 &gbaBootstrapV8 = (*gbaBootstrapV8Pckg) ();
			iGetAuthParams.iNotifyData = &gbaBootstrapV8;
			return KErrNone;
			}
		if ( authInfoV8.iData == RMobilePhone::EMbmsMuk)
			{
			RMobilePhone::TMbmsUserKeyV8Pckg
					*mbmsUserKeyV8Pckg = reinterpret_cast<RMobilePhone::TMbmsUserKeyV8Pckg*> (aPckg2);
			RMobilePhone::TMbmsUserKeyV8 &mbmsUserKeyV8 = (*mbmsUserKeyV8Pckg) ();

			iGetAuthParams.iNotifyData = &mbmsUserKeyV8;
			return KErrNone;
			}
		iGetAuthParams.iNotifyData = NULL;
		}
	else
		{
		iGetAuthParams.iNotifyPending = ETrue;
		iGetAuthParams.iNotifyHandle = aTsyReqHandle;
		iGetAuthParams.iNotifyData = NULL;
		}
	return KErrNone;
	}

TInt CSimUsimR6::GetAuthenticationParamsCancel(const TTsyReqHandle /**aTsyReqHandle*/)
	{
	if(iGetAuthParams.iNotifyPending)
		{
		iGetAuthParams.iNotifyPending=EFalse;
		iPhone->ReqCompleted(iGetAuthParams.iNotifyHandle,KErrCancel);
		}
	else
		{
		iPhone->ReqCompleted(iGetAuthParams.iNotifyHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CSimUsimR6::GetAuthenticationListPhase1(const TTsyReqHandle aTsyReqHandle,
		CRetrieveMobilePhoneAuthenticationIds::TAuthRequestData* aRequest,TInt* aBufSize)
	/**
	1st phase retrieval of the the call waiting status list
	@param aReqHandle Handle to the request
	@param aReqData information about the request
	@param aBufSize Size of the buffer the client has to allocate for the 2nd pahase
  	*/
	{
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	if (aRequest->iAuthInfo.iData==RMobilePhone::EGbaNafIdList)
		{
		iAuthType = EGba;
		TRAP(leaveCode, ret=ProcessGetGbaPhase1L(aTsyReqHandle,& (aRequest->iClient),aBufSize));
		}
	else if(aRequest->iAuthInfo.iData==RMobilePhone::EMbmsMskIdList)
		{
		iAuthType = EMbms;
		TRAP(leaveCode, ret=ProcessGetMbmsPhase1L(aTsyReqHandle,& (aRequest->iClient),aBufSize));
		}
	else
		{
		leaveCode = KErrCorrupt;
		}
	if (leaveCode != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	};

TInt CSimUsimR6::ProcessGetGbaPhase1L(const TTsyReqHandle aTsyReqHandle,
		RMobilePhone::TClientId* aClientId,TInt* aBufSize)
/** Retrieve call waiting status of each line from phone, 
 store each CB status response as a list entry,
 stream the list and then return size of this buffer to client
 @param aReqHandle Handle to the request
 @param aReqData information about the request
 @param aBufSize Size of the buffer the client has to allocate for the 2nd pahase
 */
	{
	// read the list, store its content and then return size of this buffer to client
	CMobilePhoneGbaNafIdList* list=CMobilePhoneGbaNafIdList::NewL();
	CleanupStack::PushL(list);
	TInt maxList = iGbaNafList->Enumerate();
	if(maxList)
		{
		for (TInt i=0;i<maxList;i++)
			{
			list->AddEntryL(iGbaNafList->GetEntryL(i));
			}
		}
	else
		{
		CleanupStack::PopAndDestroy(list);
		*aBufSize = 10;
		iPhone->ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNotFound;
		}
	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(*(aClientId), aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iAuthListInfo->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt

	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();

	CleanupStack::PopAndDestroy(list);
	
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimUsimR6::ProcessGetMbmsPhase1L(const TTsyReqHandle aTsyReqHandle,
		RMobilePhone::TClientId* aClientId,TInt* aBufSize)
	{
	CMobilePhoneMbmsMskIdList* list=CMobilePhoneMbmsMskIdList::NewL();
	CleanupStack::PushL(list);
	TInt maxList;
	if(iAuthType == EGba)
		{
		maxList = iGbaNafList->Enumerate();
		}
	else
		{
		maxList = iMbmsMskList->Enumerate();
		}
	if(maxList)
		{
		for (TInt i=0;i<maxList;i++)
			{
			list->AddEntryL(iMbmsMskList->GetEntryL(i));
			}
		}
	else
		{
		CleanupStack::PopAndDestroy(list);
		*aBufSize = 10;
		iPhone->ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNotFound;
		}
	
	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(*(aClientId), aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iAuthListInfo->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt

	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();

	CleanupStack::PopAndDestroy(list);
	
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimUsimR6::GetAuthenticationListPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId,TDes8* aBuf)
/**
 *	 2nd phase retrieval of the the call waiting status list
 @param aReqHandle Handle to the request
 @param aClient Ponter to the client
 @param aBuf Buffer containiong the call waiting status list
 */
	{
	CListReadAllAttempt* read=NULL;
	TBool tPosScen = ETrue;
	
	// Find the Authentication information for the client
	if ( iAuthType == EGba)
		{
		for (TInt i=0; i<iAuthListInfo->Count (); ++i)
			{
			read = iAuthListInfo->At (i);
			if ( (read->iClient.iSessionHandle==aId->iSessionHandle)&&(read->iClient.iSubSessionHandle==aId->iSubSessionHandle))
				{
				if ( tPosScen)
					{
					TPtr8 bufPtr((read->iListBuf)->Ptr (0));
					// Copy the streamed list to the client
					aBuf->Copy (bufPtr);
					}
				delete read;
				iAuthListInfo->Delete (i);
				User::After (300000);
				iPhone->ReqCompleted (aTsyReqHandle, KErrNone);
				return KErrNone;
				}
			}
		}
	else // authtype = EMbms
		{
		for (TInt i=0; i<iAuthListInfo->Count (); ++i)
			{
			read = iAuthListInfo->At (i);
			if ( (read->iClient.iSessionHandle==aId->iSessionHandle)&&(read->iClient.iSubSessionHandle==aId->iSubSessionHandle))
				{
				if ( tPosScen)
					{
					TPtr8 bufPtr((read->iListBuf)->Ptr (0));
					// Copy the streamed list to the client
					aBuf->Copy (bufPtr);
					}
				delete read;
				iAuthListInfo->Delete (i);
				User::After (300000);
				iPhone->ReqCompleted (aTsyReqHandle, KErrNone);
				return KErrNone;
				}
			}
		}
	// Should handle error case of not finding the matching client from read phase 1
	return KErrNotFound;
	}

TInt CSimUsimR6::GetAuthenticationListCancel(const TTsyReqHandle aTsyReqHandle)
	{
	// Remove the read all attempt from iAuthReadAll
	
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iAuthListInfo->Count(); ++i)
		{
		read = iAuthListInfo->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iAuthListInfo->Delete(i);
			break;
			}
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CSimUsimR6::RandTime()
/** Function that generates time(in seconds) at random for the SimTSY 
 * Maximum of ten seconds; zero seconds also included just to simulate the 
 * synchronous call scenario 
 */
	{
	TInt ranT= Math::Random()%10; 
	return(ranT);
	}



