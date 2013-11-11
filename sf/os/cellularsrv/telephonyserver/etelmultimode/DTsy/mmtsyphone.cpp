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

/**
 @file
*/


#include "mmtsy.h"
#include "testdef.h"
#include "ETELMM.H"
#include <et_clsvr.h>
#include "Dmmlog.h"
#include <etelmmerr.h>

//
//	CPhoneDMmTsy
//
CPhoneDMmTsy* CPhoneDMmTsy::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CPhoneDMmTsy* phone=new(ELeave) CPhoneDMmTsy(aFac);
	CleanupStack::PushL(phone);
	phone->ConstructL();
	CleanupStack::Pop();
	return phone;
	}

CPhoneDMmTsy::CPhoneDMmTsy(CPhoneFactoryDummyBase* aFac)
: CPhoneDummyBase(aFac),
  iActiveRequestObjects(_FOFF(CPhoneDMmTsy::CActiveListNode,iNext))
	{}

void CPhoneDMmTsy::ConstructL()
	{
	iPhoneTestCount=1;
	CPhoneDummyBase::ConstructL();
	iGetDetectedNetworks = new CArrayPtrFlat<CListReadAllAttempt>(1);
	iGetCFStatus = new CArrayPtrFlat<CListReadAllAttempt>(1);
	iGetCB = new CArrayPtrFlat<CListReadAllAttempt>(1);
	iGetCWStatus = new CArrayPtrFlat<CListReadAllAttempt>(1);
	iGetCcbsRequests = new CArrayPtrFlat<CListReadAllAttempt>(1);
	iGetPreferredNetworks = new CArrayPtrFlat<CListReadAllAttempt>(1);
	iGetAuthorizationData = new CArrayPtrFlat<CListReadAllAttempt>(1);
	iGetWlanSIDsData = new CArrayPtrFlat<CListReadAllAttempt>(1);
	iAuthReadAll = new CArrayPtrFlat<CListReadAllAttempt>(1);

	iAuthInfo=RMobilePhone::CImsAuthorizationInfoV5::NewL();
	LOGTEXT(_L8("CPhoneDMmTsy created"));
	}

CPhoneDMmTsy::~CPhoneDMmTsy()
	{
	if (iGetDetectedNetworks)
		iGetDetectedNetworks->ResetAndDestroy();
	if (iGetCFStatus)
		iGetCFStatus->ResetAndDestroy();
	if (iGetCB)
		iGetCB->ResetAndDestroy();
	if (iGetCWStatus)
		iGetCWStatus->ResetAndDestroy();
	if (iGetCcbsRequests)
		iGetCcbsRequests->ResetAndDestroy();
	if(iGetPreferredNetworks)
		iGetPreferredNetworks->ResetAndDestroy();
	
	if(iGetAuthorizationData)
		iGetAuthorizationData->ResetAndDestroy();
	
	if(iGetWlanSIDsData)
		iGetWlanSIDsData->ResetAndDestroy();
	
  	if(iAuthReadAll)
		iAuthReadAll->ResetAndDestroy();
  
	delete iGetDetectedNetworks;
	delete iGetCFStatus;
	delete iGetCB;
	delete iGetCWStatus;
	delete iGetCcbsRequests;
	delete iGetPreferredNetworks;
	delete iGetAuthorizationData;
	delete iAuthInfo;
	delete iGetWlanSIDsData;
	delete iAuthReadAll;
	
	LOGTEXT(_L8("CPhoneDMmTsy destroyed"));
	}

void CPhoneDMmTsy::Init()
	{
	// server calls this function once it has created the sub-session
	// it gives the TSY chance to do any initialisation it may need to do for
	// this sub-session
	}

CTelObject* CPhoneDMmTsy::OpenNewObjectByNameL(const TDesC& aName)
	{
	// Server calls this function when a client is opening an object from the phone
	// for the first time.
	// Multiple clients opening handles to the same sub-session object will be dealt with
	// by the server - i.e. by reference counting

	if((aName.Compare(DMMTSY_LINE_VOICE_NAME)==KErrNone) ||
	   (aName.Compare(DMMTSY_LINE_VOICE2_NAME)==KErrNone) ||
	   (aName.Compare(DMMTSY_LINE_DATA_NAME)==KErrNone) ||
	   (aName.Compare(DMMTSY_LINE_FAX_NAME)==KErrNone))

	   return REINTERPRET_CAST(CTelObject*,CLineDMmTsy::NewL(this,FacPtr()));

	if (aName.Compare(KETelSmsMessaging)==KErrNone)
	   return REINTERPRET_CAST(CTelObject*,CSmsMessagingDMmTsy::NewL(this,FacPtr(),aName));

	if (aName.Compare(KETelBroadcastMessaging)==KErrNone)
	   return REINTERPRET_CAST(CTelObject*,CBroadcastMessagingDMmTsy::NewL(this,FacPtr(),aName));

	if (aName.Compare(KETelUssdMessaging)==KErrNone)
	   return REINTERPRET_CAST(CTelObject*,CUssdMessagingDMmTsy::NewL(this,FacPtr(),aName));

	if (aName.Compare(KETelNamStore)==KErrNone)
		return REINTERPRET_CAST(CTelObject*, CNamStoreDMmTsy::NewL(this,FacPtr(),aName));

	if (aName.Compare(KETelOwnNumberStore)==KErrNone)
		return REINTERPRET_CAST(CTelObject*, CONStoreDMmTsy::NewL(this,FacPtr(),aName));

	if (aName.Compare(KETelEmergencyNumberStore)==KErrNone)
		return REINTERPRET_CAST(CTelObject*, CENStoreDMmTsy::NewL(this,FacPtr(),aName));

	if (aName.Compare(KETelIccVoiceMailBox)==KErrNone)
		return REINTERPRET_CAST(CTelObject*, CMobilePhoneBookStoreDMmTsy::NewL(this,FacPtr(),aName));

	if (aName.Compare(KETelConferenceCall) == 0)
		return REINTERPRET_CAST(CTelObject*, CMobileConferenceCallDMmTsy::NewL(this,FacPtr(),aName));
    
    if (aName.Compare(KETelLocationServices) == 0)
		return REINTERPRET_CAST(CTelObject*, CMobileLocationServicesDMmTsy::NewL(this,FacPtr(),aName));
	
	if (aName.Left(SCEAP_SSN_LENGTH).Compare(KETelSmartCardEapSession) == 0)
		{
		TInt len;

		// Extracting AID
		RMobilePhone::TAID tempAID;
		len = CharToSeptNumL(aName[SCEAP_SSN_LENGTH]) * 2;
		TPtrC ptrAid = aName.Mid(SCEAP_SSN_LENGTH + 1, len);
		ConvertTextToBinary(ptrAid, tempAID);

		// Extracting EapType
		RMobileSmartCardEap::TEapType eapType;
		TInt posTypeLength = SCEAP_SSN_LENGTH + len + 1;
		len = CharToSeptNumL(aName[posTypeLength]);
		TPtrC ptrEapT = aName.Mid(posTypeLength + 1, len);
		eapType.Copy(ptrEapT);

		CSmartCardEapDMmTsy* eapSubSessObj = NULL;
		TRAPD(err, eapSubSessObj = CSmartCardEapDMmTsy::NewL(this, tempAID, eapType, FacPtr()));

		if (err != KErrNone || eapSubSessObj == NULL)
			{
			User::Leave((err == KErrNone ? KErrGeneral : err));
			}

		return eapSubSessObj;
		} // End of opening Smart Card EAP sub-session

	TInt tLength;
	TPtrC remain(aName);

	if ((tLength=remain.Find(PHBOOK_DELIMITER))!=KErrNotFound)
		{
		if(aName.Left(tLength) == KETelIccVoiceMailBox
			&&	aName.Right(aName.Length()-tLength-2) == KEtelUSimPhoneBookType) 
			return REINTERPRET_CAST(CTelObject*, 
					CMobilePhoneBookStoreDMmTsy::NewL(this,FacPtr(),aName.Left(tLength),
												  aName.Right(aName.Length()-tLength-2)));
		}

	return NULL;
	}

/**
Returns the numerical value that corresponds to the numeric character
of its parameter (in base 17).

@param aDigit A char between '0' to '9' or between 'A' to 'G'
*/
TInt CPhoneDMmTsy::CharToSeptNumL(const TChar& aChar) const
	{
	TInt ret = 0;

	if((aChar >= '0') && (aChar <= '9'))
		{
		ret = TUint(aChar) - '0';
		}
	else if((aChar >= 'A') && (aChar <= 'G'))
		{
		ret = TUint(aChar) - 'A' + 10;
		}
	else
		{
		User::Leave(KErrArgument);
		}

	return ret;
	}

/**
Converts a buffer containing string of the hexadecimal characters,
representing the binary data, into this binary data!

@param aSrc The buffer containing text representation.
@param aDst Binary data will be written to this buffer.
*/
void CPhoneDMmTsy::ConvertTextToBinary(const TDesC& aSrc, TDes8& aDst)
	{
	aDst.SetLength(aSrc.Length() / 2);

	for (TInt i = 0; i < aSrc.Length(); i += 2)
		{
		TInt val = 0;
		if ((aSrc[i] >= '0') && (aSrc[i] <= '9'))
			{
			val = ((aSrc[i] - '0') << 4);
			}
		else if ((aSrc[i] >= 'A') && (aSrc[i] <= 'F'))
			{
			val = ((aSrc[i] - 'A' + 10) << 4);
			}

		if ((aSrc[i+1] >= '0') && (aSrc[i+1] <= '9'))
			{
			val += (aSrc[i+1] - '0');
			}
		else if ((aSrc[i+1] >= 'A') && (aSrc[i+1] <= 'F'))
			{
			val += (aSrc[i+1] - 'A' + 10);
			}

		aDst[i/2] = (TUint8) val;
		}
	}

CTelObject* CPhoneDMmTsy::OpenNewObjectL(TDes& /*aNewName*/)
	{
	// all objects opened from the phone are opened by name, hence this method
	// is not supported
	User::Leave(KErrNotSupported);
	return NULL;
	}


CTelObject::TReqMode CPhoneDMmTsy::ReqModeL(const TInt aIpc)
	{
	// ReqModeL is called from the server's CTelObject::ReqAnalyserL
	// in order to check the type of request it has.

	// The following are example request types for this dummy TSY
	// All TSYs do not have to have these request types but they have been given
	// "sensible" values in this test code

	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
	// Asynchronous Notification Requests
	case EMobilePhoneNotifyDTMFCapsChange:
	case EMobilePhoneNotifyStopInDTMFString:
	case EMobilePhoneNotifyModeChange:
	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
	case EMobilePhoneNotifyCurrentNetworkChange:
	case EMobilePhoneNotifyCellInfoChange:
	case EMobilePhoneNotifyIccAccessCapsChange:
	case EMobilePhoneNotifyBatteryInfoChange:
	case EMobilePhoneNotifySignalStrengthChange:
	case EMobilePhoneNotifyIndicatorChange:
	case EMobilePhoneNotifyCallServiceCapsChange:
	case EMobilePhoneNotifyCallForwardingStatusChange:
	case EMobilePhoneNotifyCallForwardingActive:
	case EMobilePhoneNotifyNetworkSelectionSettingChange:
	case EMobilePhoneNotifyNITZInfoChange:
	case EMobilePhoneNotifyCallBarringStatusChange:
	case EMobilePhoneNotifyCallWaitingStatusChange:
	case EMobilePhoneNotifyCCBSStatusChange:
	case EMobilePhoneNotifyCCBSRecall:
	case EMobilePhoneNotifyAlternatingCallCapsChange:
	case EMobilePhoneNotifyAlternatingCallModeChange:
	case EMobilePhoneNotifyALSLineChange:
	case EMobilePhoneNotifyCostCapsChange:
	case EMobilePhoneNotifyCostInfoChange:
	case EMobilePhoneNotifySecurityCapsChange:
	case EMobilePhoneNotifyLockInfoChange:
	case EMobilePhoneNotifySecurityCodeInfoChange:
	case EMobilePhoneNotifySecurityEvent:
	case EMobilePhoneNotifyMessageWaiting:
	case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
	case EMobilePhoneNotifyFdnStatusChange:
	case EMobilePhoneNotifyMulticallParamsChange:
	case EMobilePhoneNotifyIncomingCallTypeChange:
	case EMobilePhoneNotifyUUSSettingChange:
	case EMobilePhoneNotifyDefaultPrivacyChange:
	case EMobilePhoneNotifyMultimediaCallPreferenceChange:
	case EMobilePhoneNotifyNetworkSecurityLevelChange:
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
	case EMobilePhoneNotifyMailboxNumbersChange:
	case EMobilePhoneNotifyMmsUpdate:
	case EMobilePhoneNotifyMmsConfig:
	case EMobilePhoneNotifyNetworkInvScanChange:
	case EMobilePhoneNotifyNetworkInvScanEvent:
	case EMobilePhoneNotifyAirTimeDurationChange:
	case EMobilePhoneNotifySendNetworkServiceRequest:
	case EMobilePhoneNotifyAllSendNetworkServiceRequest:
	case EMobilePhoneNotifyDTMFEvent:
	case EMobilePhoneNotifyStorePreferredNetworksListChange:
	case EMobilePhoneNotifyCipheringIndicatorStatus:
	case EMobilePhoneNotifyIdentityServiceStatus:
	case EMobilePhoneNotifyIdentitySuppressionRejected:
	case EMobilePhoneNotifyMulticallIndicatorChange:
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
	case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
	case EMobilePhoneNotifyImsAuthorizationInfoChanged:
	case EMobilePhoneNotifyAPNListChanged:
	case EMobilePhoneNotifyAuthenticateDataChange:
	case EMobilePhoneNotifyWlanDataChange:
	case EMobilePhoneNotifyPreferredWlanSIDListChange:
	

	case EMobilePhoneNotifyAPNControlListServiceStatusChange:
	ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
	
	// Non flow-controlled requests
	case EMobilePhoneGetDTMFCaps:
	case EMobilePhoneStopDTMFTone:
	case EMobilePhoneGetNetworkCaps:
	case EMobilePhoneGetCurrentMode:
	case EMobilePhoneGetNetworkRegistrationStatus:
	case EMobilePhoneGetHomeNetwork:
	case EMobilePhoneGetCurrentNetwork:
	case EMobilePhoneGetCellInfo:
	case EMobilePhoneGetDetectedNetworksPhase1:
	case EMobilePhoneGetDetectedNetworksPhase2:
	case EMobilePhoneGetDetectedNetworksV2Phase1:
	case EMobilePhoneGetDetectedNetworksV2Phase2:
	case EMobilePhoneGetDetectedNetworksV5Phase1:
	case EMobilePhoneGetDetectedNetworksV5Phase2:
	case EMobilePhoneGetDetectedNetworksV8Phase1:
	case EMobilePhoneGetDetectedNetworksV8Phase2:
	case EMobilePhoneGetMultimodeCaps:
	case EMobilePhoneGetPhoneStoreInfo:
	case EMobilePhoneGetIccAccessCaps:
	case EMobilePhoneGetIccIdentity:
	case EMobilePhoneGetCustomerServiceProfile:
	case EMobilePhoneGetServiceTable:
	case EMobilePhoneGetBatteryCaps:
	case EMobilePhoneGetBatteryInfo:
	case EMobilePhoneGetSignalCaps:
	case EMobilePhoneGetSignalStrength:
	case EMobilePhoneGetIndicatorCaps:
	case EMobilePhoneGetIndicator:
	case EMobilePhoneGetIdentityCaps:
	case EMobilePhoneGetPhoneId:
	case EMobilePhoneGetSubscriberId:
	case EMobilePhoneGetCallServiceCaps:
	case EMobilePhoneGetCallForwardingStatusPhase1:
	case EMobilePhoneGetCallForwardingStatusPhase2:
	case EMobilePhoneGetFeatureCode:
	case EMobilePhoneGetNetworkSelectionSetting:
	case EMobilePhoneSetNetworkSelectionSetting:
	case EMobilePhoneSelectNetwork:
	case EMobilePhoneGetNITZInfo:
	case EMobilePhoneGetServiceProviderName:
	case EMobilePhoneGetIdentityServiceStatus:
	case EMobilePhoneGetBarringStatusPhase1:
	case EMobilePhoneGetBarringStatusPhase2:
	case EMobilePhoneSetCallBarringStatus:
	case EMobilePhoneSetCallBarringPassword:
	case EMobilePhoneGetWaitingStatusPhase1:
	case EMobilePhoneGetWaitingStatusPhase2:
	case EMobilePhoneSetCallWaitingStatus:
	case EMobilePhoneGetCCBSStatus:
	case EMobilePhoneDeactivateCCBS:
	case EMobilePhoneAcceptCCBSRecall:
	case EMobilePhoneRefuseCCBSRecall:
	case EMobilePhoneGetCompletionRequestsPhase1:
	case EMobilePhoneGetCompletionRequestsPhase2:
	case EMobilePhoneGetAlternatingCallCaps:
	case EMobilePhoneGetAlternatingCallMode:
	case EMobilePhoneSetAlternatingCallMode:
	case EMobilePhoneGetALSLine:
	case EMobilePhoneSetALSLine:
	case EMobilePhoneGetCostCaps:
	case EMobilePhoneClearCostMeter:
	case EMobilePhoneSetMaxCostMeter:
	case EMobilePhoneSetPuct:
	case EMobilePhoneGetCostInfo:
	case EMobilePhoneGetSecurityCaps:
	case EMobilePhoneGetLockInfo:
	case EMobilePhoneSetLockSetting:
	case EMobilePhoneGetSecurityCodeInfo:
	case EMobilePhoneChangeSecurityCode:
	case EMobilePhoneVerifySecurityCode:
	case EMobilePhoneAbortSecurityCode:
	case EMobilePhoneGetFdnStatus:
	case EMobilePhoneSetFdnSetting:
	case EMobilePhoneGetMulticallParams:
	case EMobilePhoneSetMulticallParams:
	case EMobilePhoneGetIncomingCallType:
	case EMobilePhoneSetIncomingCallType:
	case EMobilePhoneGetUUSSetting:
	case EMobilePhoneSetUUSSetting:
	case EMobilePhoneGetDefaultPrivacy:
	case EMobilePhoneSetDefaultPrivacy:
	case EMobilePhoneGetIccMessageWaitingIndicators:
	case EMobilePhoneSetIccMessageWaitingIndicators:
	case EMobilePhoneGetMultimediaCallPreference:
	case EMobilePhoneSetMultimediaCallPreference:
	case EMobilePhoneGetNetworkSecurityLevel:
	case EMobilePhoneEnumerateUSimApplications:
	case EMobilePhoneGetUSimAppsSelectionMode:
	case EMobilePhoneSetUSimAppsSelectionMode:
	case EMobilePhoneSetUSimApplicationStatus:
	case EMobilePhoneGetUSimApplicationsInfo:
	case EMobilePhoneSetSSPassword:
	case EMobilePhoneGetMailboxNumbers:
	case EMobilePhoneSetMmsUserConnParams:	
	case EMobilePhoneSetMmsUserPreferences:
	case EMobilePhoneGetMmsConfig:
	case EMobilePhoneGetHomeNetworkSearchPeriod:
	case EMobilePhoneGetLastUsedAccessTechnology:
	case EMobilePhoneGetNetworkInvScanSetting:
	case EMobilePhoneGetCallForwardingIndicator:
	case EMobilePhoneGetCompMethodName:
	case EMobilePhoneEnumerateAPNEntries:
	case EMobilePhoneGetAPNname:
	case EMobilePhoneAppendAPNName:
	case EMobilePhoneDeleteAPNName:
	case EMobilePhoneSetAPNControlListServiceStatus:
	case EMobilePhoneGetAPNControlListServiceStatus:
	case EMobilePhoneGetAirTimeDuration:
	case EMobilePhoneClearBlacklist:
	case EMobilePhoneGetPersonalisationCaps:
	case EMobilePhoneGetPersonalisationStatus:
	case EMobilePhoneSetPersonalisationStatus:
	case EMobilePhoneGetNetworkName:
	case EMobilePhoneGetPreferredNetworksPhase1:
	case EMobilePhoneGetPreferredNetworksPhase2:
	case EMobilePhoneStorePreferredNetworksList:
	case EMobilePhoneGetCipheringIndicatorStatus:
	case EMobilePhoneSetIdentityServiceStatus:
	case EMobilePhoneEnumerateSmartCardApplications:
	case EMobilePhoneGetSmartCardApplicationInfo:
	case EMobilePhoneSetSmartCardApplicationStatus:
	case EMobilePhoneGetScFileInfo:
	case EMobilePhoneReadScFile:
	case EMobilePhoneUpdateScFile:
	case EMobilePhoneGetCurrentActiveUSimApplication:
	case EMobilePhoneGetCurrentNetworkNoLocation:		
	case EMobilePhoneAuthorizationInfoPhase1:
	case EMobilePhoneAuthorizationInfoPhase2:
	case EMobilePhoneIMSAuthenticate:
	case EMobilePhoneSmartCardAuthenticate:
	case EETelPhoneSetEmergencyClient:
	case EMobilePhoneGetAuthenticationParams:
	case EMobilePhoneSetGbaBootstrapParams:
	case EMobilePhoneAuthenticationListPhase1:
	case EMobilePhoneAuthenticationListPhase2:
	case EMobilePhoneGetWlanData:
	case EMobilePhoneSetWlanData:
	case EMobilePhoneGetPreferredWlanSIDsPhase1:
	case EMobilePhoneGetPreferredWlanSIDsPhase2:
	case EMobilePhoneStorePreferredWlanSIDList:
		break;

	// Flow controlled requests
	case EMobilePhoneSendNetworkServiceRequest:
	case EMobilePhoneSendDTMFTones:
	case EMobilePhoneReadDTMFTones:
	case EMobilePhoneStartDTMFTone:
	case EMobilePhoneContinueDTMFStringSending:
	case EMobilePhoneSetCallForwardingStatus:
	case EMobilePhoneProgramFeatureCode:
	case EMobilePhoneTerminateAllCalls:
	case EMobilePhoneTerminateActiveCalls:
		ret=KReqModeFlowControlObeyed;
		break;
	default:
		ret=CPhoneBase::ReqModeL(aIpc);
		break;
		}
	return ret;
	}

TInt CPhoneDMmTsy::RegisterNotification(const TInt aIpc)
	{
	// RegisterNotification is called when the server recognises that this notification
	// is being posted for the first time on this sub-session object.

	// It enables the TSY to "turn on" any regular notification messages that it may 
	// receive from the phone

	switch (aIpc)
		{
	case EMobilePhoneNotifyDTMFCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - DTMF Caps Change "));
		return KErrNone;
	case EMobilePhoneNotifyStopInDTMFString:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - DTMF Stop In DTMF String "));
		return KErrNone;
	case EMobilePhoneNotifyModeChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Mode Change "));
		return KErrNone;
	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Registration Status Change "));
		return KErrNone;
	case EMobilePhoneNotifyCurrentNetworkChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Current network Change "));
		return KErrNone;
	case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Current network ISV Change No Location"));
		return KErrNone;
	case EMobilePhoneNotifyCellInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Cell information Change "));
		return KErrNone;		
	case EMobilePhoneNotifyIccAccessCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - SIM Access Caps Change "));
		return KErrNone;
	case EMobilePhoneNotifyBatteryInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Battery Info Change "));
		return KErrNone;
	case EMobilePhoneNotifySignalStrengthChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Signal Strength Change "));
		return KErrNone;
	case EMobilePhoneNotifyIndicatorChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Indicator Change "));
		return KErrNone;
	case EMobilePhoneNotifyCallServiceCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Call Service Caps Change "));
		return KErrNone;
	case EMobilePhoneNotifyCallForwardingStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Call Forwarding Status Change "));
		return KErrNone;
	case EMobilePhoneNotifyCallForwardingActive:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Call Forwarding Active"));
		return KErrNone;
	case EMobilePhoneNotifyNetworkSelectionSettingChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Network Selection Setting Change "));
		return KErrNone;
	case EMobilePhoneNotifyNITZInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Network Time Info Change "));
		return KErrNone;
	case EMobilePhoneNotifyCallBarringStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Call Barring Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyCallWaitingStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Call Waiting Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyCCBSStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify CCBS Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyCCBSRecall:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify CCBS Recall"));
		return KErrNone;
	case EMobilePhoneNotifyAlternatingCallCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Alternating Call Caps Change"));
		return KErrNone;
	case EMobilePhoneNotifyAlternatingCallModeChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Alternating Mode Caps Change"));
		return KErrNone;
	case EMobilePhoneNotifyALSLineChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify ALSLine Change"));
		return KErrNone;
	case EMobilePhoneNotifyCostCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Cost Caps Change"));
		return KErrNone;
	case EMobilePhoneNotifyCostInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Cost Info Change"));
		return KErrNone;
	case EMobilePhoneNotifySecurityCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Security Caps Change"));
		return KErrNone;
	case EMobilePhoneNotifyLockInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Lock Info Change"));
		return KErrNone;
	case EMobilePhoneNotifySecurityCodeInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Security Code Info Change"));
		return KErrNone;
	case EMobilePhoneNotifySecurityEvent:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify On Security Event"));
		return KErrNone;
	case EMobilePhoneNotifyMessageWaiting:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Message Waiting"));
		return KErrNone;
	case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Icc Message Waiting Indicators Change"));
		return KErrNone;
	case EMobilePhoneNotifyFdnStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Fdn Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyMulticallParamsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Multicall Params Change"));
		return KErrNone;
	case EMobilePhoneNotifyIncomingCallTypeChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Incoming Call Type Change"));
		return KErrNone;
	case EMobilePhoneNotifyUUSSettingChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify UUS Setting Change"));
		return KErrNone;
	case EMobilePhoneNotifyDefaultPrivacyChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Privacy Setting Change"));
		return KErrNone;
	case EMobilePhoneNotifyMultimediaCallPreferenceChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Multimedia Call Setting Change"));
		return KErrNone;
	case EMobilePhoneNotifyNetworkSecurityLevelChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Network Security Level Change"));
		return KErrNone;	
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify USim Applications Selection Mode Change"));
		return KErrNone;
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify USim Applications Info Change"));
		return KErrNone;
	case EMobilePhoneNotifyMailboxNumbersChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Mobile Mailbox Numbers functionality"));
		return KErrNone;
	case EMobilePhoneNotifyMmsUpdate:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Mobile MMS Update functionality"));
		return KErrNone;
	case EMobilePhoneNotifyMmsConfig:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Mobile MMS  Config functionality"));
		return KErrNone;
	case EMobilePhoneNotifyNetworkInvScanChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Network Investigation Scan Change functionality"));
		return KErrNone;
	case EMobilePhoneNotifyNetworkInvScanEvent:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Network Investigation Scan Event functionality"));
		return KErrNone;
	case EMobilePhoneNotifyAirTimeDurationChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Air Time Duration Change"));
		return KErrNone;
	case EMobilePhoneNotifySendNetworkServiceRequest:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Send Network Service Request"));
		return KErrNone;
	case EMobilePhoneNotifyAllSendNetworkServiceRequest:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify All Send Network Service Request"));
		return KErrNone;
	case EMobilePhoneNotifyDTMFEvent:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify DTMF Event"));
		return KErrNone;
	case EMobilePhoneNotifyStorePreferredNetworksListChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Stored Networks List Change"));
		return KErrNone;
	case EMobilePhoneNotifyCipheringIndicatorStatus:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Ciphering Indicator Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyIdentityServiceStatus:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Identity Service Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyIdentitySuppressionRejected:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Identity Suppression Rejected"));
		return KErrNone;
	case EMobilePhoneNotifyMulticallIndicatorChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Multicall Indicator Change"));
		return KErrNone;
	case EMobilePhoneNotifyImsAuthorizationInfoChanged:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Ims Authorization Info Changed"));
		return KErrNone;
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Smart Card Application Info Change"));
		return KErrNone;
	case EMobilePhoneNotifyAPNListChanged:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify APN Control List Change"));
		return KErrNone;
	case EMobilePhoneNotifyAPNControlListServiceStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify APN Control List Service Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyAuthenticateDataChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Authentication Data Change"));
		return KErrNone;
	case EMobilePhoneNotifyWlanDataChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Wlan Data Change"));
		return KErrNone;
	case EMobilePhoneNotifyPreferredWlanSIDListChange:
		LOGTEXT(_L8("CPhoneDMmTsy: RegisterNotification - Notify Wlan SIDs List Change"));
		return KErrNone;		
		
	default:
		// Unknown or invalid SMS IPC
		return KErrNotSupported;
		}
	}

TInt CPhoneDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	// DeregisterNotification is called when the server recognises that this notification
	// will not be posted again because the last client to have a handle on this sub-session
	// object has just closed the handle.

	// It enables the TSY to "turn off" any regular notification messages that it may
	// receive from the phone.

	switch (aIpc)
		{
	case EMobilePhoneNotifyDTMFCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - DTMF Caps Change "));
		return KErrNone;
	case EMobilePhoneNotifyStopInDTMFString:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - DTMF Stop In DTMF String "));
		return KErrNone;
	case EMobilePhoneNotifyModeChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Mode Change "));
		return KErrNone;
	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Registration Status Change "));
		return KErrNone;
	case EMobilePhoneNotifyCurrentNetworkChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Current network Change "));
		return KErrNone;
	case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Current network Change No Location"));
		return KErrNone;
	case EMobilePhoneNotifyCellInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Cell information Change "));
		return KErrNone;		
	case EMobilePhoneNotifyIccAccessCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - SIM Access Caps Change "));
		return KErrNone;
	case EMobilePhoneNotifyBatteryInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Battery Info Change "));
		return KErrNone;
	case EMobilePhoneNotifySignalStrengthChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Signal Strength Change "));
		return KErrNone;
	case EMobilePhoneNotifyIndicatorChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Indicator Change "));
		return KErrNone;
	case EMobilePhoneNotifyCallServiceCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Call Service Caps Change "));
		return KErrNone;
	case EMobilePhoneNotifyCallForwardingStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Call Forwarding Status Change "));
		return KErrNone;
	case EMobilePhoneNotifyCallForwardingActive:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Call Forwarding Active"));
		return KErrNone;
	case EMobilePhoneNotifyNetworkSelectionSettingChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Network Selection Setting Change "));
		return KErrNone;
	case EMobilePhoneNotifyNITZInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Network Time Info Change "));
		return KErrNone;
	case EMobilePhoneNotifyCallBarringStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Call Barring Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyCallWaitingStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Call Waiting Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyCCBSStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify CCBS Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyCCBSRecall:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify CCBS Recall"));
		return KErrNone;
	case EMobilePhoneNotifyAlternatingCallCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Alternating Call Caps Change"));
		return KErrNone;
	case EMobilePhoneNotifyAlternatingCallModeChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Alternating Mode Caps Change"));
		return KErrNone;
	case EMobilePhoneNotifyALSLineChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify ALSLine Change"));
		return KErrNone;
	case EMobilePhoneNotifyCostCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Cost Caps Change"));
		return KErrNone;
	case EMobilePhoneNotifyCostInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Cost Info Change"));
		return KErrNone;
	case EMobilePhoneNotifySecurityCapsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Security Caps Change"));
		return KErrNone;
	case EMobilePhoneNotifyLockInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Lock Info Change"));
		return KErrNone;
	case EMobilePhoneNotifySecurityCodeInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Security Code Info Change"));
		return KErrNone;
	case EMobilePhoneNotifySecurityEvent:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify On Security Event"));
		return KErrNone;	
	case EMobilePhoneNotifyMessageWaiting:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Message Waiting"));
		return KErrNone;
	case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Icc Message Waiting Indicators Change"));
		return KErrNone;
	case EMobilePhoneNotifyFdnStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Fdn Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyMulticallParamsChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Multicall Params Change"));
		return KErrNone;
	case EMobilePhoneNotifyIncomingCallTypeChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Incoming Call Type Change"));
		return KErrNone;
	case EMobilePhoneNotifyUUSSettingChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify UUS Setting Change"));
		return KErrNone;
	case EMobilePhoneNotifyDefaultPrivacyChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Privacy Setting Change"));
		return KErrNone;
	case EMobilePhoneNotifyMultimediaCallPreferenceChange:	
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Multimedia Call Setting Change"));
		return KErrNone;
	case EMobilePhoneNotifyNetworkSecurityLevelChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Network Security Level Change"));
		return KErrNone;	
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify USim Applications Selection Mode Change"));
		return KErrNone;
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify USim Applications Info Change"));
		return KErrNone;
	case EMobilePhoneNotifyMailboxNumbersChange:			
		LOGTEXT(_L8("CPhoneDMmTsy:  DeregisterNotification - Notify Mobile Mailbox Numbers functionality"));
		return KErrNone;
	case EMobilePhoneNotifyMmsUpdate:
		LOGTEXT(_L8("CPhoneDMmTsy:  DeregisterNotification - Notify Mobile MMS Update functionality"));
		return KErrNone;
	case EMobilePhoneNotifyMmsConfig:
		LOGTEXT(_L8("CPhoneDMmTsy:  DeregisterNotification - Notify Mobile MMS Config functionality"));
		return KErrNone;
	case EMobilePhoneNotifyNetworkInvScanChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Network Investigation Scan Change functionality"));
		return KErrNone;
	case EMobilePhoneNotifyNetworkInvScanEvent:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Network Investigation Scan Event functionality"));
		return KErrNone;
	case EMobilePhoneNotifyAirTimeDurationChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Air Time Duration Change"));
		return KErrNone;
	case EMobilePhoneNotifySendNetworkServiceRequest:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Send Network Service Request"));
		return KErrNone;
	case EMobilePhoneNotifyAllSendNetworkServiceRequest:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify All Send Network Service Request"));
		return KErrNone;
	case EMobilePhoneNotifyDTMFEvent:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify DTMF Event"));
		return KErrNone;	
	case EMobilePhoneNotifyStorePreferredNetworksListChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Stored Networks List Change"));
		return KErrNone;
	case EMobilePhoneNotifyCipheringIndicatorStatus:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Ciphering IndicatorStatus Change"));
		return KErrNone;
	case EMobilePhoneNotifyIdentityServiceStatus:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Identity Service Status Change"));
		return KErrNone;	
	case EMobilePhoneNotifyIdentitySuppressionRejected:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Identity Suppression Rejected"));
		return KErrNone;	
	case EMobilePhoneNotifyMulticallIndicatorChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify MulticallIndicator Change"));
		return KErrNone;
	case EMobilePhoneNotifyImsAuthorizationInfoChanged:
		LOGTEXT(_L8("CPhoneDMmTsy:DeregisterNotification - Notify Ims Authorization Info Changed"));
		return KErrNone;
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Smart Card Application Info Change"));
		return KErrNone;
	case EMobilePhoneNotifyAPNListChanged:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify APN Control List Change"));
		return KErrNone;
	case EMobilePhoneNotifyAPNControlListServiceStatusChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify APN Control List Service Status Change"));
		return KErrNone;
	case EMobilePhoneNotifyAuthenticateDataChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Authentication Data Change"));
		return KErrNone;
	case EMobilePhoneNotifyWlanDataChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Wlan Data Change"));
		return KErrNone;
	case EMobilePhoneNotifyPreferredWlanSIDListChange:
		LOGTEXT(_L8("CPhoneDMmTsy: DeregisterNotification - Notify Wlan SIDs list Change"));
		return KErrNone;		
	default:
		// Unknown or invalid sms IPC
		return KErrNotSupported;
		}
	}

TInt CPhoneDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	// NumberOfSlotsL is called by the server when it is registering a new notification.
	// It enables the TSY to tell the server how many buffer slots to allocate for
	// "repost immediately" notifications that may trigger before clients collect them

	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobilePhoneNotifyBatteryInfoChange:
	case EMobilePhoneNotifySignalStrengthChange:
	case EMobilePhoneNotifyIndicatorChange:
		LOGTEXT(_L8("CPhoneDMmTsy: Registered with 10 slots"));
		numberOfSlots=10;
		break;

	case EMobilePhoneNotifyDTMFCapsChange:
	case EMobilePhoneNotifyStopInDTMFString:
	case EMobilePhoneNotifyCallServiceCapsChange:
	case EMobilePhoneNotifyCCBSRecall:
	case EMobilePhoneNotifyDTMFEvent:
		LOGTEXT(_L8("CPhoneDMmTsy: Registered with 5 slots"));
		numberOfSlots=5;
		break;
	
	case EMobilePhoneNotifyModeChange:
	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
	case EMobilePhoneNotifyCurrentNetworkChange:
	case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
	case EMobilePhoneNotifyCellInfoChange:
	case EMobilePhoneNotifyIccAccessCapsChange:
	case EMobilePhoneNotifyCallForwardingStatusChange:
	case EMobilePhoneNotifyCallForwardingActive:
	case EMobilePhoneNotifyNetworkSelectionSettingChange:
	case EMobilePhoneNotifyNITZInfoChange:
	case EMobilePhoneNotifyCallBarringStatusChange:
	case EMobilePhoneNotifyCallWaitingStatusChange:
	case EMobilePhoneNotifyCCBSStatusChange:
	case EMobilePhoneNotifyAlternatingCallCapsChange:
	case EMobilePhoneNotifyAlternatingCallModeChange:
	case EMobilePhoneNotifyALSLineChange:
	case EMobilePhoneNotifyCostCapsChange:
	case EMobilePhoneNotifyCostInfoChange:
	case EMobilePhoneNotifySecurityCapsChange:
	case EMobilePhoneNotifyLockInfoChange:
	case EMobilePhoneNotifySecurityCodeInfoChange:
	case EMobilePhoneNotifySecurityEvent:
	case EMobilePhoneNotifyMessageWaiting:
	case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
	case EMobilePhoneNotifyFdnStatusChange:
	case EMobilePhoneNotifyMulticallParamsChange:
	case EMobilePhoneNotifyIncomingCallTypeChange:
	case EMobilePhoneNotifyUUSSettingChange:
	case EMobilePhoneNotifyDefaultPrivacyChange:
	case EMobilePhoneNotifyMultimediaCallPreferenceChange:
	case EMobilePhoneNotifyNetworkSecurityLevelChange:
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
	case EMobilePhoneNotifyMailboxNumbersChange:
	case EMobilePhoneNotifyMmsUpdate:
	case EMobilePhoneNotifyMmsConfig:
	case EMobilePhoneNotifyNetworkInvScanChange:
	case EMobilePhoneNotifyNetworkInvScanEvent:
	case EMobilePhoneNotifyAirTimeDurationChange:
	case EMobilePhoneNotifySendNetworkServiceRequest:
	case EMobilePhoneNotifyAllSendNetworkServiceRequest:
	case EMobilePhoneNotifyStorePreferredNetworksListChange:
	case EMobilePhoneNotifyCipheringIndicatorStatus:
	case EMobilePhoneNotifyIdentityServiceStatus:
	case EMobilePhoneNotifyIdentitySuppressionRejected:
	case EMobilePhoneNotifyMulticallIndicatorChange:
	case EMobilePhoneNotifyImsAuthorizationInfoChanged:
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
	case EMobilePhoneNotifyAPNListChanged:
	case EMobilePhoneNotifyAPNControlListServiceStatusChange:
	case EMobilePhoneNotifyAuthenticateDataChange:
	case EMobilePhoneNotifyWlanDataChange:
	case EMobilePhoneNotifyPreferredWlanSIDListChange:
		LOGTEXT(_L8("CPhoneDMmTsy: Registered with 2 slots"));
		numberOfSlots=2;
		break;
	default:
		// Unknown or invalid Phone IPC
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CPhoneDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
		 				 const TDataPackage& aPackage)
	{
	// ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
	// for the TSY to process
	// A request handle, request type and request data are passed to the TSY

	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();
	
	// The request data has to extracted from TDataPackage and the TAny* pointers have to
	// be "cast" to the expected request data type
	
	switch (aIpc)
		{
	// Non-Flow controlled requests

	case EMobilePhoneGetDTMFCaps:
		return GetDTMFCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneStopDTMFTone:
		return StopDTMFTone(aTsyReqHandle);

	case EMobilePhoneGetNetworkCaps:
		return GetNetworkCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneGetNetworkRegistrationStatus:
		return GetNetworkRegistrationStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneRegistrationStatus*, dataPtr));

	case EMobilePhoneGetHomeNetwork:
		return GetHomeNetwork(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneGetCurrentMode:
		return GetCurrentMode(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneNetworkMode*, dataPtr));

	case EMobilePhoneGetCurrentNetwork:
		return GetCurrentNetwork(aTsyReqHandle, aPackage.Des1n(),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneLocationAreaV1*, dataPtr2));

	case EMobilePhoneGetCellInfo:
		return GetCellInfo(aTsyReqHandle, aPackage.Des1n());
		
	case EMobilePhoneGetCurrentNetworkNoLocation:
		return GetCurrentNetwork(aTsyReqHandle, aPackage.Des1n(), NULL);

	case EMobilePhoneGetDetectedNetworksPhase1:
		return GetDetectedNetworksPhase1(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr),
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobilePhoneGetDetectedNetworksPhase2:
		return GetDetectedNetworksPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPackage.Des2n());

	case EMobilePhoneGetDetectedNetworksV2Phase1:
		return GetDetectedNetworksV2Phase1(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr),
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobilePhoneGetDetectedNetworksV2Phase2:
		return GetDetectedNetworksV2Phase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPackage.Des2n());

	case EMobilePhoneGetDetectedNetworksV5Phase1:
		return GetDetectedNetworksV5Phase1(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr),
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobilePhoneGetDetectedNetworksV5Phase2:
		return GetDetectedNetworksV5Phase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPackage.Des2n());

	case EMobilePhoneGetDetectedNetworksV8Phase1:
		return GetDetectedNetworksV8Phase1(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr),
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobilePhoneGetDetectedNetworksV8Phase2:
		return GetDetectedNetworksV8Phase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPackage.Des2n());

	case EMobilePhoneGetNetworkSelectionSetting:
		return GetNetworkSelectionSetting(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneSetNetworkSelectionSetting:
		return SetNetworkSelectionSetting(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneSelectNetwork:
		return SelectNetwork(aTsyReqHandle,
			REINTERPRET_CAST(TBool*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneNetworkManualSelection*, dataPtr2));

	case EMobilePhoneGetNITZInfo:
		return GetNetworkTimeInfo(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneNITZ*, dataPtr));

	case EMobilePhoneGetServiceProviderName:
		return GetServiceProviderName(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneServiceProviderNameV2*, dataPtr));

	case EMobilePhoneGetMultimodeCaps:
		return GetMultimodeCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneGetPhoneStoreInfo:
		switch(aPackage.Type())
			{
			// Switch between the alternative implementations of GetPhoneStoreInfo
			case TDataPackage::EPackage1n2u:
				return GetPhoneStoreInfo(aTsyReqHandle,
						aPackage.Des1n(),aPackage.Des2u());
			case TDataPackage::EPackage1n2n:
				return GetPhoneStoreInfo(aTsyReqHandle, aPackage.Des2n(),
						REINTERPRET_CAST(RMobilePhone::TPhoneStoreNameAndIccType*, dataPtr));
			default:
				return NULL;
			}

	case EMobilePhoneGetIccAccessCaps:
		return GetIccAccessCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));
			
	case EMobilePhoneGetIccIdentity:
		return GetIccIdentity(aTsyReqHandle, REINTERPRET_CAST(RMobilePhone::TIccIdentity*, dataPtr));

	case EMobilePhoneGetCustomerServiceProfile:
		return GetCustomerServiceProfile(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneGetServiceTable:
		return GetServiceTable(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneServiceTable*,dataPtr),
			aPackage.Des2n());

	case EMobilePhoneGetBatteryCaps:
		return GetBatteryCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneGetBatteryInfo:
		return GetBatteryInfo(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneBatteryInfoV1*, dataPtr));

	case EMobilePhoneGetSignalCaps:
		return GetSignalCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneGetSignalStrength:
		return GetSignalStrength(aTsyReqHandle,
			REINTERPRET_CAST(TInt32*, dataPtr),
			REINTERPRET_CAST(TInt8*, dataPtr2));

	case EMobilePhoneGetIndicatorCaps:
		return GetIndicatorCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr),
			REINTERPRET_CAST(TUint32*, dataPtr2));

	case EMobilePhoneGetIndicator:
		return GetIndicator(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneGetIdentityCaps:
		return GetIdentityCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneGetPhoneId:
		return GetPhoneId(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneIdentityV1*, dataPtr));

	case EMobilePhoneGetSubscriberId:
		return GetSubscriberId(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneSubscriberId*, dataPtr));

	case EMobilePhoneGetDefaultPrivacy:
		return GetDefaultPrivacy(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhonePrivacy*, dataPtr));

	case EMobilePhoneSetDefaultPrivacy:
		return SetDefaultPrivacy(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhonePrivacy*, dataPtr));

	case EMobilePhoneGetCallServiceCaps:
		return GetCallServiceCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneGetCallForwardingStatusPhase1:
		return GetCallForwardingStatusPhase1(aTsyReqHandle,
			REINTERPRET_CAST(CRetrieveMobilePhoneCFList::TGetCallForwardingRequest*, dataPtr),
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobilePhoneGetCallForwardingStatusPhase2:
		return GetCallForwardingStatusPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPackage.Des2n());

	case EMobilePhoneGetFeatureCode:
		return GetFeatureCodeString(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TNetworkServiceAndAction*, dataPtr),
			aPackage.Des2u());

	case EMobilePhoneGetIdentityServiceStatus:
		return GetIdentityServiceStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TIdServiceAndLocation*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneIdServiceStatus*,dataPtr2));

	case EMobilePhoneGetBarringStatusPhase1:
		return GetCallBarringPhase1(aTsyReqHandle,
			REINTERPRET_CAST(CRetrieveMobilePhoneCBList::TGetCallBarringRequest*, dataPtr),
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobilePhoneGetBarringStatusPhase2:
		return GetCallBarringPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPackage.Des2n());

	case EMobilePhoneSetCallBarringStatus:
		return SetCallBarringStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneCBCondition*,dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneCBChangeV1*,dataPtr2));

	case EMobilePhoneSetCallBarringPassword:
		return SetCallBarringPassword(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhonePasswordChangeV1*,dataPtr));

	case EMobilePhoneSetSSPassword:
		return SetSSPassword(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhonePasswordChangeV2*,dataPtr),
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobilePhoneGetWaitingStatusPhase1:
		return GetCallWaitingPhase1(aTsyReqHandle,
			REINTERPRET_CAST(CRetrieveMobilePhoneCWList::TGetCallWaitingRequest*, dataPtr),
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobilePhoneGetWaitingStatusPhase2:
		return GetCallWaitingPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPackage.Des2n());

	case EMobilePhoneSetCallWaitingStatus:
		return SetCallWaitingStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobileService*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneServiceAction*, dataPtr2));

	case EMobilePhoneGetCCBSStatus:
		return GetCCBSStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneCCBSStatus*,dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobileInfoLocation*,dataPtr2));

	case EMobilePhoneAcceptCCBSRecall:
		return AcceptCCBSRecall(aTsyReqHandle,
			REINTERPRET_CAST(TInt* ,dataPtr),aPackage.Des2u());

	case EMobilePhoneRefuseCCBSRecall:
		return RefuseCCBSRecall(aTsyReqHandle,
			REINTERPRET_CAST(TInt* ,dataPtr));

	case EMobilePhoneDeactivateCCBS:
		return DeactivateCCBS(aTsyReqHandle, 
			REINTERPRET_CAST(TInt* ,dataPtr));

	case EMobilePhoneGetCompletionRequestsPhase1:
		return GetCallCompletionPhase1(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr), REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobilePhoneGetCompletionRequestsPhase2:
		return GetCallCompletionPhase2(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr), aPackage.Des2n());

	case EMobilePhoneGetAlternatingCallCaps:
		return GetAlternatingCallCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*,dataPtr));

	case EMobilePhoneGetAlternatingCallMode:
		return GetAlternatingCallMode(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneAlternatingCallMode*,dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobileService*,dataPtr2));
	
	case EMobilePhoneSetAlternatingCallMode:
		return SetAlternatingCallMode(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneAlternatingCallMode*,dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobileService*,dataPtr2));
	
	case EMobilePhoneGetALSLine:
		return GetALSLine(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneALSLine*,dataPtr));
	
	case EMobilePhoneSetALSLine:
		return SetALSLine(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneALSLine*,dataPtr));

	case EMobilePhoneGetCostCaps:
		return GetCostCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*,dataPtr));

	case EMobilePhoneSetMaxCostMeter:
		return SetMaxCostMeter(aTsyReqHandle,
			REINTERPRET_CAST(TUint*,dataPtr));

	case EMobilePhoneClearCostMeter:
		return ClearCostMeter(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneCostMeters*,dataPtr));

	case EMobilePhoneSetPuct:
		return SetPuct(aTsyReqHandle,aPackage.Des1n());

	case EMobilePhoneGetCostInfo:
		return GetCostInfo(aTsyReqHandle,aPackage.Des1n());

	case EMobilePhoneGetSecurityCaps:
		return GetSecurityCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*,dataPtr));

	case EMobilePhoneGetLockInfo:
		return GetLockInfo(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneLock *, dataPtr),
			aPackage.Des2n());

	case EMobilePhoneSetLockSetting:
		return SetLockSetting(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneLock *, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneLockSetting *, dataPtr2)
			);
	case EMobilePhoneChangeSecurityCode:
		return ChangeSecurityCode(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneSecurityCode *, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobilePhonePasswordChangeV1 *, dataPtr2));

	case EMobilePhoneGetSecurityCodeInfo:
		return GetSecurityCodeInfo(aTsyReqHandle,
			reinterpret_cast<RMobilePhone::TMobilePhoneSecurityCode *> (dataPtr),
			aPackage.Des2n());

	case EMobilePhoneVerifySecurityCode:
		return VerifySecurityCode(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneSecurityCode *,dataPtr),
			REINTERPRET_CAST(RMobilePhone::TCodeAndUnblockCode *, dataPtr2));

	case EMobilePhoneAbortSecurityCode:
		return AbortSecurityCode(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneSecurityCode *, dataPtr));

	case EMobilePhoneGetFdnStatus:
		return GetFdnStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneFdnStatus*, dataPtr));

	case EMobilePhoneSetFdnSetting:
		return SetFdnSetting(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneFdnSetting*, dataPtr));

	case EMobilePhoneGetMulticallParams:
		return GetMulticallParams(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneSetMulticallParams:
		return SetMulticallParams(aTsyReqHandle, REINTERPRET_CAST(TInt*, dataPtr));

	case EMobilePhoneGetIncomingCallType:
		return GetIncomingCallType(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneIncomingCallType*, dataPtr), 
			aPackage.Des2n());

	case EMobilePhoneSetIncomingCallType:
		return SetIncomingCallType(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneIncomingCallType*, dataPtr), 
			aPackage.Des2n());

	case EMobilePhoneGetUUSSetting:
		return GetUUSSetting(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneUUSSetting*, dataPtr));

	case EMobilePhoneSetUUSSetting:
		return SetUUSSetting(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneUUSSetting*, dataPtr));

	case EMobilePhoneGetIccMessageWaitingIndicators:
		return GetIccMessageWaitingIndicators(aTsyReqHandle,aPackage.Des1n());

	case EMobilePhoneSetIccMessageWaitingIndicators:
		return SetIccMessageWaitingIndicators(aTsyReqHandle,aPackage.Des1n());
	
	case EMobilePhoneGetMultimediaCallPreference:
		return GetMultimediaCallPreference(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneMultimediaSettings*, dataPtr));

	case EMobilePhoneSetMultimediaCallPreference:
		return SetMultimediaCallPreference(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneMultimediaSettings*, dataPtr));

	case EMobilePhoneGetNetworkSecurityLevel:
		return GetNetworkSecurityLevel(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneNetworkSecurity*, dataPtr));

	case EMobilePhoneEnumerateUSimApplications:
		return EnumerateUSimApplications(aTsyReqHandle,
			REINTERPRET_CAST(TInt*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TAID*, dataPtr2));

	case EMobilePhoneGetUSimAppsSelectionMode:
		return GetUSimAppsSelectionMode(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TUSimSelectionMode*, dataPtr));

	case EMobilePhoneSetUSimAppsSelectionMode:
		return SetUSimAppsSelectionMode(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TUSimSelectionMode*, dataPtr));

	case EMobilePhoneSetUSimApplicationStatus:
		return SetUSimApplicationStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TAID*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TUSimAppAction*, dataPtr2));

	case EMobilePhoneGetUSimApplicationsInfo:
		return GetUSimApplicationInfo(aTsyReqHandle,
			REINTERPRET_CAST(TInt*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TUSimApplicationInfoV2*, dataPtr2));
			
	case EMobilePhoneEnumerateSmartCardApplications:
		return EnumerateSmartCardApplications(aTsyReqHandle,
			reinterpret_cast<TUint*>(dataPtr));
			
	case EMobilePhoneGetSmartCardApplicationInfo:
		return GetSmartCardApplicationInfo(aTsyReqHandle, reinterpret_cast<TUint*>(dataPtr), aPackage.Des2n());
		
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
		return NotifySmartCardApplicationInfoChange(aTsyReqHandle, reinterpret_cast<RMobilePhone::TSmartCardApplicationEvent*>(dataPtr2), reinterpret_cast<RMobilePhone::TAID*>(dataPtr));
		
	case EMobilePhoneSetSmartCardApplicationStatus:
		return SetSmartCardApplicationStatus(aTsyReqHandle, reinterpret_cast<RMobilePhone::TAID*>(dataPtr), *reinterpret_cast<RMobilePhone::TSmartCardApplicationAction*>(dataPtr2));
		
	case EMobilePhoneGetScFileInfo:
		return GetScFileInfo(aTsyReqHandle, *reinterpret_cast<RMobilePhone::TScFilePath*>(dataPtr), aPackage.Des2n());
		
	case EMobilePhoneReadScFile:
		return ReadScFile(aTsyReqHandle, *reinterpret_cast<RMobilePhone::TScFilePathWithAccessOffsets*>(dataPtr), aPackage.Des2n());
		
	case EMobilePhoneUpdateScFile:
		return UpdateScFile(aTsyReqHandle, *reinterpret_cast<RMobilePhone::TScFilePathWithAccessOffsets*>(dataPtr), aPackage.Des2n());
		
	case EMobilePhoneGetCurrentActiveUSimApplication:
		return GetCurrentActiveUSimApplication(aTsyReqHandle, reinterpret_cast<RMobilePhone::TAID*>(dataPtr));
			
	case EMobilePhoneGetHomeNetworkSearchPeriod:
		return GetHomeNetworkSearchPeriod(aTsyReqHandle,
			REINTERPRET_CAST(TInt*, dataPtr));
			
	case EMobilePhoneGetLastUsedAccessTechnology:
		return GetLastUsedAccessTechnology(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneNetworkAccess*, dataPtr));
			
	case EMobilePhoneGetNetworkInvScanSetting:
		return GetNetworkInvScanSetting(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneInvestigationScan*, dataPtr));
			
	case EMobilePhoneGetCallForwardingIndicator:
		return GetCallForwardingIndicator(aTsyReqHandle, aPackage.Des1n());
		
	case EMobilePhoneGetCompMethodName:
		return GetCompMethodName(aTsyReqHandle, 
			*REINTERPRET_CAST(TUint8*, dataPtr), aPackage.Des2u());
			
	case EMobilePhoneEnumerateAPNEntries:
		return EnumerateAPNEntries(aTsyReqHandle, 
			REINTERPRET_CAST(TUint32*, dataPtr));
	
	case EMobilePhoneGetAPNname:
		return GetAPNname(aTsyReqHandle, 
			*REINTERPRET_CAST(TUint32*, dataPtr), aPackage.Des2n());

	case EMobilePhoneAppendAPNName:
		return AppendAPNName(aTsyReqHandle, aPackage.Des1n());
			
	case EMobilePhoneDeleteAPNName:
		return DeleteAPNName(aTsyReqHandle, 
			*REINTERPRET_CAST(TUint32*, dataPtr));
	
	case EMobilePhoneNotifyAPNListChanged:
		return NotifyAPNListChanged(aTsyReqHandle);
	
	case EMobilePhoneSetAPNControlListServiceStatus:
		return SetAPNControlListServiceStatus(aTsyReqHandle, 
			*REINTERPRET_CAST(RMobilePhone::TAPNControlListServiceStatus*, dataPtr));
			
	case EMobilePhoneGetAPNControlListServiceStatus:
		return GetAPNControlListServiceStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TAPNControlListServiceStatus*, dataPtr));
			
	case EMobilePhoneNotifyAPNControlListServiceStatusChange:
		return NotifyAPNControlListServiceStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TAPNControlListServiceStatus*, dataPtr));
	
	case EMobilePhoneGetAirTimeDuration:
		return GetAirTimeDuration(aTsyReqHandle, 
			REINTERPRET_CAST(TTimeIntervalSeconds*, dataPtr));
			
	case EMobilePhoneClearBlacklist:
		return ClearBlacklist(aTsyReqHandle);
		
	case EMobilePhoneGetPersonalisationCaps:
		return GetPersonalisationCaps(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));
			
	case EMobilePhoneGetPersonalisationStatus:
		return GetPersonalisationStatus(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));
			
	case EMobilePhoneSetPersonalisationStatus:
		return SetPersonalisationStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhonePersonalisation*, dataPtr), aPackage.Des2u()); 
			
	case EMobilePhoneGetNetworkName:
		return GetCurrentNetworkName(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n());	
		
	case EMobilePhoneGetPreferredNetworksPhase1:
		return GetPreferredNetworksPhase1(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), 
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobilePhoneGetPreferredNetworksPhase2:
		return GetPreferredNetworksPhase2(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPackage.Des2n());		
			
	case EMobilePhoneStorePreferredNetworksList:
		return StorePreferredNetworksList(aTsyReqHandle, aPackage.Des1n());
		
	case EMobilePhoneGetCipheringIndicatorStatus:
		return GetCipheringIndicatorStatus(aTsyReqHandle, 
			*REINTERPRET_CAST(RMobilePhone::TMobileCallCipheringIndicator*, dataPtr));
			
	case EMobilePhoneSetIdentityServiceStatus:
		return SetIdentityServiceStatus(aTsyReqHandle, 
			*REINTERPRET_CAST(RMobilePhone::TMobilePhoneIdService*, dataPtr),
			*REINTERPRET_CAST(RMobilePhone::TMobilePhoneIdServiceSetting*, dataPtr2));

	// Flow controlled requests

	case EMobilePhoneSendNetworkServiceRequest:
		return SendNetworkServiceRequest(aTsyReqHandle, aPackage.Des1u());

	case EMobilePhoneSendDTMFTones:
		return SendDTMFTones(aTsyReqHandle, aPackage.Des1u());

	case EMobilePhoneReadDTMFTones:
		return ReadDTMFTones(aTsyReqHandle, aPackage.Des1u());

	case EMobilePhoneStartDTMFTone:
		return StartDTMFTone(aTsyReqHandle, 
			REINTERPRET_CAST(TChar*, dataPtr));

	case EMobilePhoneContinueDTMFStringSending:
		return ContinueDTMFStringSending(aTsyReqHandle,
			REINTERPRET_CAST(TBool*,dataPtr));

	case EMobilePhoneSetCallForwardingStatus:
		return SetCallForwardingStatus(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneCFCondition*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneCFChangeV1*, dataPtr2) );

	case EMobilePhoneProgramFeatureCode:
		return ProgramFeatureCodeString(aTsyReqHandle, 
			aPackage.Des2u(),
			REINTERPRET_CAST(RMobilePhone::TNetworkServiceAndAction*, dataPtr));
		
	case EMobilePhoneTerminateAllCalls:
		return TerminateAllCalls(aTsyReqHandle);
		
    case EMobilePhoneTerminateActiveCalls:
        return TerminateActiveCalls(aTsyReqHandle);
			
	case EMobilePhoneGetMailboxNumbers:
		return GetMailboxNumbers (aTsyReqHandle, aPackage.Des1n());
		
	case EMobilePhoneSetMmsUserConnParams:
		return SetMmsUserConnParams(aTsyReqHandle, aPackage.Des1n());
		
	case EMobilePhoneSetMmsUserPreferences:
		return SetMmsUserPreferences(aTsyReqHandle, aPackage.Des1n());
		
	case EMobilePhoneGetMmsConfig:
		return GetMmsConfig(aTsyReqHandle, 		
			*REINTERPRET_CAST(RMobilePhone::TMmsConnParams*,dataPtr), aPackage.Des2n());			
	//
	// Notification requests
	//

	case EMobilePhoneNotifyModeChange:
		return NotifyModeChange(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneNetworkMode*, dataPtr));

	case EMobilePhoneNotifyDTMFCapsChange:
		return NotifyDTMFCapsChange(aTsyReqHandle, 
			REINTERPRET_CAST(TUint32*, dataPtr));
	
	case EMobilePhoneNotifyStopInDTMFString:
		return NotifyStopInDTMFString(aTsyReqHandle);

	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
		return NotifyNetworkRegistrationStatusChange(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneRegistrationStatus*, dataPtr));

	case EMobilePhoneNotifyCurrentNetworkChange:
		return NotifyCurrentNetworkChange(aTsyReqHandle, aPackage.Des1n(),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneLocationAreaV1*, dataPtr2));

	case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
		return NotifyCurrentNetworkChange(aTsyReqHandle, aPackage.Des1n(), NULL);

	case EMobilePhoneNotifyCellInfoChange:
		return NotifyCellInfoChange(aTsyReqHandle, aPackage.Des1n());
	
	case EMobilePhoneNotifyIccAccessCapsChange:
		return NotifyIccAccessCapsChange(aTsyReqHandle, 
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneNotifyBatteryInfoChange:
		return NotifyBatteryInfoChange(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneBatteryInfoV1*, dataPtr));

	case EMobilePhoneNotifySignalStrengthChange:
		return NotifySignalStrengthChange(aTsyReqHandle,
			REINTERPRET_CAST(TInt32*, dataPtr), REINTERPRET_CAST(TInt8*, dataPtr2));

	case EMobilePhoneNotifyIndicatorChange:
		return NotifyIndicatorChange(aTsyReqHandle, REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneNotifyDefaultPrivacyChange:
		return NotifyDefaultPrivacyChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhonePrivacy*, dataPtr));

	case EMobilePhoneNotifyCallForwardingStatusChange:
		return NotifyCallForwardingStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneCFCondition*, dataPtr));

	case EMobilePhoneNotifyCallForwardingActive:
		return NotifyCallForwardingActive(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobileService*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneCFActive*, dataPtr2));

	case EMobilePhoneNotifyNetworkSelectionSettingChange:
		return NotifyNetworkSelectionSettingChange(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneNotifyNITZInfoChange:
		return NotifyNetworkTimeInfoChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneNITZ*, dataPtr));

	case EMobilePhoneNotifyCallServiceCapsChange:
		return NotifyCallServiceCapsChange(aTsyReqHandle, 
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneNotifyCallBarringStatusChange:
		return NotifyCallBarringStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneCBCondition*,dataPtr));
	
	case EMobilePhoneNotifyCallWaitingStatusChange:
		return NotifyCallWaitingStatusChange(aTsyReqHandle,
			aPackage.Des1n());

	case EMobilePhoneNotifyCCBSStatusChange:
		return NotifyCCBSStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneCCBSStatus*,dataPtr));

	case EMobilePhoneNotifyCCBSRecall:
		return NotifyCCBSRecall(aTsyReqHandle,aPackage.Des1n());

	case EMobilePhoneNotifyAlternatingCallCapsChange:
		return NotifyAlternatingCallCapsChange(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*,dataPtr));

	case EMobilePhoneNotifyAlternatingCallModeChange:
		return NotifyAlternatingCallModeChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneAlternatingCallMode*,dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobileService*,dataPtr2));

	case EMobilePhoneNotifyALSLineChange:
		return NotifyALSLineChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneALSLine*,dataPtr));

	case EMobilePhoneNotifyCostCapsChange:
		return NotifyCostCapsChange(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*,dataPtr));

	case EMobilePhoneNotifyCostInfoChange:
		return NotifyCostInfoChange(aTsyReqHandle,aPackage.Des1n());

	case EMobilePhoneNotifySecurityCapsChange:
		return NotifySecurityCapsChange(aTsyReqHandle,
			REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneNotifyLockInfoChange:
		return NotifyLockInfoChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneLock *, dataPtr),
			aPackage.Des2n());

	case EMobilePhoneNotifySecurityCodeInfoChange:
		return NotifySecurityCodeInfoChange(aTsyReqHandle,
			reinterpret_cast<RMobilePhone::TMobilePhoneSecurityCode*> (dataPtr),
			aPackage.Des2n());

	case EMobilePhoneNotifySecurityEvent:
		return NotifyOnSecurityEvent(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneSecurityEvent*,dataPtr));

	case EMobilePhoneNotifyMessageWaiting:
		return NotifyMessageWaiting(aTsyReqHandle,REINTERPRET_CAST(TInt*,dataPtr));

	case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
		return NotifyIccMessageWaitingIndicatorsChange(aTsyReqHandle,aPackage.Des1n());
	
	case EMobilePhoneNotifyFdnStatusChange:
		return NotifyFdnStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneFdnStatus*,dataPtr));

	case EMobilePhoneNotifyMulticallParamsChange:
		return NotifyMulticallParamsChange(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneNotifyIncomingCallTypeChange:
		return NotifyIncomingCallTypeChange(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneIncomingCallType*, dataPtr),
			aPackage.Des2n());
	
	case EMobilePhoneNotifyUUSSettingChange:
		return NotifyUUSSettingChange(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneUUSSetting*, dataPtr));

	case EMobilePhoneNotifyMultimediaCallPreferenceChange:
		return NotifyMultimediaCallPreferenceChange(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneMultimediaSettings*, dataPtr));

	case EMobilePhoneNotifyNetworkSecurityLevelChange:
		return NotifyNetworkSecurityLevelChange(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneNetworkSecurity*, dataPtr));

	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
		return NotifyUSimAppsSelectionModeChange(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TUSimSelectionMode*, dataPtr));	

	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		return NotifyUSimApplicationsInfoChange(aTsyReqHandle,
			REINTERPRET_CAST(TInt*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TAID*, dataPtr2));	
			
	case EMobilePhoneNotifyMailboxNumbersChange:		
			return NotifyMailboxNumbersChange (aTsyReqHandle, aPackage.Des1n());		

	case EMobilePhoneNotifyMmsUpdate:
		return NotifyMmsUpdate (aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneNotifyMmsConfig:
		return NotifyMmsConfig(aTsyReqHandle, 		
			*REINTERPRET_CAST(RMobilePhone::TMmsConnParams*,dataPtr), aPackage.Des2n());
		
	case EMobilePhoneNotifyNetworkInvScanChange:
		return NotifyNetworkInvScanChange(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneInvestigationScan*, dataPtr));
			
	case EMobilePhoneNotifyNetworkInvScanEvent:
		return NotifyNetworkInvScanEvent(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneInvestigationScanEvent*, dataPtr));
			
	case EMobilePhoneNotifyAirTimeDurationChange:
		return NotifyAirTimeDurationChange(aTsyReqHandle,
			REINTERPRET_CAST(TTimeIntervalSeconds*,dataPtr));

	case EMobilePhoneNotifySendNetworkServiceRequest:
		return NotifySendNetworkServiceRequest(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneNotifySendSSOperation*, dataPtr), aPackage.Des2n());

	case EMobilePhoneNotifyAllSendNetworkServiceRequest:
		return NotifyAllSendNetworkServiceRequest(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneNotifySendSSOperation*, dataPtr), aPackage.Des2n());
	
	case EMobilePhoneNotifyDTMFEvent:
		return NotifyDTMFEvent(aTsyReqHandle, REINTERPRET_CAST(RMobilePhone::TMobilePhoneDTMFEvent*, dataPtr));

	case EMobilePhoneNotifyStorePreferredNetworksListChange:
		return NotifyStorePreferredNetworksListChange(aTsyReqHandle);

	case EMobilePhoneNotifyCipheringIndicatorStatus:
		return NotifyCipheringIndicatorStatus(aTsyReqHandle,
			*REINTERPRET_CAST(RMobilePhone::TMobileCallCipheringIndicator*, dataPtr));

	case EMobilePhoneNotifyIdentityServiceStatus:
		return NotifyIdentityServiceStatus(aTsyReqHandle,
			*REINTERPRET_CAST(RMobilePhone::TMobilePhoneIdService*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneIdServiceStatus*, dataPtr2));

	case EMobilePhoneNotifyIdentitySuppressionRejected:
		return NotifyIdentitySuppressionRejected(aTsyReqHandle);

	case EMobilePhoneNotifyMulticallIndicatorChange:
		return NotifyMulticallIndicatorChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneMulticallIndicator*, dataPtr));

	case EMobilePhoneAuthorizationInfoPhase1:
			return GetAuthorizationInfoPhase1(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));


	case EMobilePhoneAuthorizationInfoPhase2:
		return GetAuthorizationInfoPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr), aPackage.Des2n());
			
	case EMobilePhoneIMSAuthenticate:
		return GetAuthenticationData(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneSmartCardAuthenticate:
		return GetSmartCardAuthenticationData(aTsyReqHandle, aPackage.Des1n(),
		         reinterpret_cast<RMobilePhone::TAID*>(dataPtr2));

	case EMobilePhoneNotifyImsAuthorizationInfoChanged:
		return NotifyImsAuthorizationInfoChanged(aTsyReqHandle);
	
	case EETelPhoneSetEmergencyClient:
		return SetEmergencyClient(aTsyReqHandle,REINTERPRET_CAST(TUint32*, dataPtr));

	case EMobilePhoneNotifyAuthenticateDataChange:
		return NotifyAuthenticateDataChange(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneGetAuthenticationParams:
		return GetAuthenticationParams(aTsyReqHandle, aPackage.Des1n(),aPackage.Des2n());
		
	case EMobilePhoneSetGbaBootstrapParams:
		return SetGbaBootstrapParams(aTsyReqHandle, reinterpret_cast<RMobilePhone::TAID*>(dataPtr2),aPackage.Des1n());
	
	case EMobilePhoneAuthenticationListPhase1:
		return GetAuthenticationListPhase1(aTsyReqHandle,
			REINTERPRET_CAST(CRetrieveMobilePhoneAuthenticationIds::TAuthRequestData*, dataPtr),REINTERPRET_CAST(TInt*, dataPtr2));
			
	case EMobilePhoneAuthenticationListPhase2:
		return GetAuthenticationListPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPackage.Des2n());
	
	case EMobilePhoneNotifyWlanDataChange:
		return NotifyWlanDataChange(aTsyReqHandle,aPackage.Des1n());
	
	case EMobilePhoneNotifyPreferredWlanSIDListChange:
		return NotifyPreferredWlanSIDListChange(aTsyReqHandle);
		
	case EMobilePhoneGetWlanData:
		return GetWlanData(aTsyReqHandle,aPackage.Des1n());
		
	case EMobilePhoneSetWlanData:
		return SetWlanData(aTsyReqHandle,aPackage.Des1n());
	
	case EMobilePhoneGetPreferredWlanSIDsPhase1:
		return GetPreferredWlanSIDsPhase1(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), 
			REINTERPRET_CAST(TInt*, dataPtr2));
			
	case EMobilePhoneGetPreferredWlanSIDsPhase2:
		return GetPreferredWlanSIDsPhase2(aTsyReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPackage.Des2n());		
						
	case EMobilePhoneStorePreferredWlanSIDList:
		return StorePreferredWlanSIDList(aTsyReqHandle, aPackage.Des1n());
		
	default:
		return KErrNotSupported;
		}
	}

TInt CPhoneDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	// CancelService is called by the server when it is "cleaning-up" any still outstanding
	// asynchronous requests before closing a client's sub-session.
	// This will happen if a client closes its R-class handle without cancelling outstanding
	// asynchronous requests.

	switch (aIpc)
		{
	case EMobilePhoneGetHomeNetwork:
		return GetHomeNetworkCancel(aTsyReqHandle);
	case EMobilePhoneNotifyModeChange:
		return NotifyModeChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyDTMFCapsChange:
		return NotifyDTMFCapsChangeCancel(aTsyReqHandle);
	case EMobilePhoneSendDTMFTones:
		return SendDTMFTonesCancel(aTsyReqHandle);
	case EMobilePhoneReadDTMFTones:
		return ReadDTMFTonesCancel(aTsyReqHandle);
	case EMobilePhoneNotifyStopInDTMFString:
		return NotifyStopInDTMFStringCancel(aTsyReqHandle);
	case EMobilePhoneGetNetworkRegistrationStatus:
		return GetNetworkRegistrationStatusCancel(aTsyReqHandle);
	case EMobilePhoneNotifyNetworkRegistrationStatusChange:
		return NotifyNetworkRegistrationStatusChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetCurrentNetwork:
		return GetCurrentNetworkCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCurrentNetworkChange:
		return NotifyCurrentNetworkChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetCurrentNetworkNoLocation:
		return GetCurrentNetworkCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
		return NotifyCurrentNetworkChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetCellInfo:
		return GetCellInfoCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCellInfoChange:
		return NotifyCellInfoChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetDetectedNetworksPhase1:
	case EMobilePhoneGetDetectedNetworksPhase2:
		return GetDetectedNetworksCancel(aTsyReqHandle);
	case EMobilePhoneGetDetectedNetworksV2Phase1:
	case EMobilePhoneGetDetectedNetworksV2Phase2:
		return GetDetectedNetworksV2Cancel(aTsyReqHandle);
	case EMobilePhoneGetDetectedNetworksV5Phase1:
	case EMobilePhoneGetDetectedNetworksV5Phase2:
		return GetDetectedNetworksV5Cancel(aTsyReqHandle);	
	case EMobilePhoneGetDetectedNetworksV8Phase1:
	case EMobilePhoneGetDetectedNetworksV8Phase2:
		return GetDetectedNetworksV8Cancel(aTsyReqHandle);
	case EMobilePhoneGetPhoneStoreInfo:
		return GetPhoneStoreInfoCancel(aTsyReqHandle);
	case EMobilePhoneNotifyIccAccessCapsChange:
		return NotifyIccAccessCapsChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetCustomerServiceProfile:
		return GetCustomerServiceProfileCancel(aTsyReqHandle);
	case EMobilePhoneGetServiceTable:
		return GetServiceTableCancel(aTsyReqHandle);
	case EMobilePhoneGetBatteryInfo:
		return GetBatteryInfoCancel(aTsyReqHandle);
	case EMobilePhoneNotifyBatteryInfoChange:
		return NotifyBatteryInfoChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetSignalStrength:
		return GetSignalStrengthCancel(aTsyReqHandle);
	case EMobilePhoneNotifySignalStrengthChange:
		return NotifySignalStrengthChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetIndicator:
		return GetIndicatorCancel(aTsyReqHandle);
	case EMobilePhoneNotifyIndicatorChange:
		return NotifyIndicatorChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetPhoneId:
		return GetPhoneIdCancel(aTsyReqHandle);
	case EMobilePhoneGetSubscriberId:
		return GetSubscriberIdCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCallServiceCapsChange:
		return NotifyCallServiceCapsChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetCallForwardingStatusPhase1:
	case EMobilePhoneGetCallForwardingStatusPhase2:
		return GetCallForwardingStatusCancel(aTsyReqHandle);
	case EMobilePhoneSetCallForwardingStatus:
		return SetCallForwardingStatusCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCallForwardingStatusChange:
		return NotifyCallForwardingStatusChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCallForwardingActive:
		return NotifyCallForwardingActiveCancel(aTsyReqHandle);
	case EMobilePhoneProgramFeatureCode:
		return ProgramFeatureCodeStringCancel(aTsyReqHandle);
	case EMobilePhoneGetFeatureCode:
		return GetFeatureCodeStringCancel(aTsyReqHandle);
	case EMobilePhoneSendNetworkServiceRequest:
		return SendNetworkServiceRequestCancel(aTsyReqHandle);
	case EMobilePhoneNotifyNetworkSelectionSettingChange:
		return NotifyNetworkSelectionSettingChangeCancel(aTsyReqHandle);
	case EMobilePhoneSelectNetwork:
		return SelectNetworkCancel(aTsyReqHandle);
	case EMobilePhoneNotifyNITZInfoChange:
		return NotifyNetworkTimeInfoChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetServiceProviderName:
		return GetServiceProviderNameCancel(aTsyReqHandle);
	case EMobilePhoneGetIdentityServiceStatus:
		return GetIdentityServiceStatusCancel(aTsyReqHandle);
	case EMobilePhoneGetBarringStatusPhase1:
	case EMobilePhoneGetBarringStatusPhase2:
		return GetCallBarringCancel(aTsyReqHandle);
	case EMobilePhoneSetCallBarringStatus:
		return SetCallBarringStatusCancel(aTsyReqHandle);
	case EMobilePhoneSetCallBarringPassword:
		return SetCallBarringPasswordCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCallBarringStatusChange:
		return NotifyCallBarringStatusChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetWaitingStatusPhase1:
	case EMobilePhoneGetWaitingStatusPhase2:
		return GetCallWaitingCancel(aTsyReqHandle);
	case EMobilePhoneSetCallWaitingStatus:
		return SetCallWaitingStatusCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCallWaitingStatusChange:
		return NotifyCallWaitingStatusChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetCCBSStatus:
		return GetCCBSStatusCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCCBSStatusChange:
		return NotifyCCBSStatusChangeCancel(aTsyReqHandle);
	case EMobilePhoneDeactivateCCBS:
		return DeactivateCCBSCancel(aTsyReqHandle);
	case EMobilePhoneAcceptCCBSRecall:
		return AcceptCCBSRecallCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCCBSRecall:
		return NotifyCCBSRecallCancel(aTsyReqHandle);
	case EMobilePhoneGetCompletionRequestsPhase1:
	case EMobilePhoneGetCompletionRequestsPhase2:
		return GetCallCompletionCancel(aTsyReqHandle);
	case EMobilePhoneNotifyAlternatingCallCapsChange:
		return NotifyAlternatingCallCapsChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyAlternatingCallModeChange:
		return NotifyAlternatingCallModeChangeCancel(aTsyReqHandle);
	case EMobilePhoneSetALSLine:
		return SetALSLineCancel(aTsyReqHandle);
	case EMobilePhoneNotifyALSLineChange:
		return NotifyALSLineChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCostCapsChange:
		return NotifyCostCapsChangeCancel(aTsyReqHandle);
	case EMobilePhoneClearCostMeter:
		return ClearCostMeterCancel(aTsyReqHandle);
	case EMobilePhoneSetMaxCostMeter:
		return SetMaxCostMeterCancel(aTsyReqHandle);
	case EMobilePhoneSetPuct:
		return SetPuctCancel(aTsyReqHandle);
	case EMobilePhoneGetCostInfo:
		return GetCostInfoCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCostInfoChange:
		return NotifyCostInfoChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifySecurityCapsChange:
		return NotifySecurityCapsChangeCancel(aTsyReqHandle);
	 case EMobilePhoneGetLockInfo:
		return GetLockInfoCancel(aTsyReqHandle);
	case EMobilePhoneNotifyLockInfoChange:
		return NotifyLockInfoChangeCancel(aTsyReqHandle);
	case EMobilePhoneSetLockSetting:
		return SetLockSettingCancel(aTsyReqHandle);
	case EMobilePhoneChangeSecurityCode:
		return ChangeSecurityCodeCancel(aTsyReqHandle);
	case EMobilePhoneGetSecurityCodeInfo:
		return GetSecurityCodeInfoCancel(aTsyReqHandle);
	case EMobilePhoneNotifySecurityCodeInfoChange:
		return NotifySecurityCodeInfoChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifySecurityEvent:
		return NotifyOnSecurityEventCancel(aTsyReqHandle);
	case EMobilePhoneVerifySecurityCode:
		return VerifySecurityCodeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyMessageWaiting:
		return NotifyMessageWaitingCancel(aTsyReqHandle);
	case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
		return NotifyIccMessageWaitingIndicatorsChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetIccMessageWaitingIndicators:
		return GetIccMessageWaitingIndicatorsCancel(aTsyReqHandle);
	case EMobilePhoneSetIccMessageWaitingIndicators:
		return SetIccMessageWaitingIndicatorsCancel(aTsyReqHandle);
	case EMobilePhoneGetFdnStatus:
		return GetFdnStatusCancel(aTsyReqHandle);
	case EMobilePhoneSetFdnSetting:
		return SetFdnSettingCancel(aTsyReqHandle);
	case EMobilePhoneNotifyFdnStatusChange:
		return NotifyFdnStatusChangeCancel(aTsyReqHandle);
	case EMobilePhoneSetNetworkSelectionSetting:
		return SetNetworkSelectionSettingCancel(aTsyReqHandle);
	case EMobilePhoneSetAlternatingCallMode:
		return SetAlternatingCallModeCancel(aTsyReqHandle);
	case EMobilePhoneGetMulticallParams:
		return GetMulticallParamsCancel(aTsyReqHandle);
	case EMobilePhoneSetMulticallParams:
		return SetMulticallParamsCancel(aTsyReqHandle);
	case EMobilePhoneNotifyMulticallParamsChange:
		return NotifyMulticallParamsChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetIncomingCallType:
		return GetIncomingCallTypeCancel(aTsyReqHandle);
	case EMobilePhoneSetIncomingCallType:
		return SetIncomingCallTypeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyIncomingCallTypeChange:
		return NotifyIncomingCallTypeChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetUUSSetting:
		return GetUUSSettingCancel(aTsyReqHandle);
	case EMobilePhoneSetUUSSetting:
		return SetUUSSettingCancel(aTsyReqHandle);
	case EMobilePhoneNotifyUUSSettingChange:
		return NotifyUUSSettingChangeCancel(aTsyReqHandle);
	case EMobilePhoneSetDefaultPrivacy:
		return SetDefaultPrivacyCancel(aTsyReqHandle);
	case EMobilePhoneNotifyDefaultPrivacyChange:
		return NotifyDefaultPrivacyChangeCancel(aTsyReqHandle);
	case EMobilePhoneSetMultimediaCallPreference:
		return SetMultimediaCallPreferenceCancel(aTsyReqHandle);
	case EMobilePhoneNotifyMultimediaCallPreferenceChange:
		return NotifyMultimediaCallPreferenceChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetNetworkSecurityLevel:
		return GetNetworkSecurityLevelCancel(aTsyReqHandle);
	case EMobilePhoneNotifyNetworkSecurityLevelChange:
		return NotifyNetworkSecurityLevelChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
		return NotifyUSimAppsSelectionModeChangeCancel(aTsyReqHandle);
	case EMobilePhoneSetUSimApplicationStatus:
		return SetUSimApplicationStatusCancel(aTsyReqHandle);
	case EMobilePhoneGetUSimApplicationsInfo:
		return GetUSimApplicationInfoCancel(aTsyReqHandle);
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		return NotifyUSimApplicationsInfoChangeCancel(aTsyReqHandle);
	case EMobilePhoneEnumerateUSimApplications:
		return EnumerateUSimApplicationsCancel(aTsyReqHandle);
	case EMobilePhoneEnumerateSmartCardApplications:
		return EnumerateSmartCardApplicationsCancel(aTsyReqHandle);
	case EMobilePhoneGetSmartCardApplicationInfo:
		return GetSmartCardApplicationInfoCancel(aTsyReqHandle);
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
		return NotifySmartCardApplicationInfoChangeCancel(aTsyReqHandle);
	case EMobilePhoneSetSmartCardApplicationStatus:
		return SetSmartCardApplicationStatusCancel(aTsyReqHandle);
	case EMobilePhoneGetScFileInfo:
		return GetScFileInfoCancel(aTsyReqHandle);
	case EMobilePhoneReadScFile:
		return ReadScFileCancel(aTsyReqHandle);
	case EMobilePhoneUpdateScFile:
		return UpdateScFileCancel(aTsyReqHandle);
	case EMobilePhoneGetCurrentActiveUSimApplication:
		return GetCurrentActiveUSimApplicationCancel(aTsyReqHandle);
	case EMobilePhoneSetSSPassword:
		return SetSSPasswordCancel(aTsyReqHandle);
	case EMobilePhoneNotifyMailboxNumbersChange:
		return NotifyMailboxNumbersChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetMailboxNumbers:
		return GetMailboxNumbersCancel(aTsyReqHandle);
	case EMobilePhoneNotifyMmsUpdate:
		return NotifyMmsUpdateCancel(aTsyReqHandle);
	case EMobilePhoneNotifyMmsConfig:
		return SetMmsUserConnParamsCancel(aTsyReqHandle);
	case EMobilePhoneSetMmsUserConnParams:
		return SetMmsUserConnParamsCancel(aTsyReqHandle);
	case EMobilePhoneSetMmsUserPreferences:
		return SetMmsUserPreferencesCancel(aTsyReqHandle);
	case EMobilePhoneGetMmsConfig:
		return GetMmsConfigCancel(aTsyReqHandle);
	case EMobilePhoneGetHomeNetworkSearchPeriod:
		return GetHomeNetworkSearchPeriodCancel(aTsyReqHandle);
	case EMobilePhoneGetLastUsedAccessTechnology:
		return GetLastUsedAccessTechnologyCancel(aTsyReqHandle);
	case EMobilePhoneNotifyNetworkInvScanChange:
		return NotifyNetworkInvScanChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyNetworkInvScanEvent:
		return NotifyNetworkInvScanEventCancel(aTsyReqHandle);
	case EMobilePhoneGetCallForwardingIndicator:
		return GetCallForwardingIndicatorCancel(aTsyReqHandle);
	case EMobilePhoneGetCompMethodName:
		return GetCompMethodNameCancel(aTsyReqHandle);
	case EMobilePhoneEnumerateAPNEntries:
		return EnumerateAPNEntriesCancel(aTsyReqHandle);
	case EMobilePhoneGetAPNname:
		return GetAPNnameCancel(aTsyReqHandle);
	case EMobilePhoneAppendAPNName:
		return AppendAPNNameCancel(aTsyReqHandle);
	case EMobilePhoneDeleteAPNName:
		return DeleteAPNNameCancel(aTsyReqHandle);
	case EMobilePhoneNotifyAPNListChanged:
		return NotifyAPNListChangedCancel(aTsyReqHandle);
	case EMobilePhoneSetAPNControlListServiceStatus:
		return SetAPNControlListServiceStatusCancel(aTsyReqHandle);
	case EMobilePhoneGetAPNControlListServiceStatus:
		return GetAPNControlListServiceStatusCancel(aTsyReqHandle);
	case EMobilePhoneNotifyAPNControlListServiceStatusChange:
		return NotifyAPNControlListServiceStatusChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyAirTimeDurationChange:
		return NotifyAirTimeDurationChangeCancel(aTsyReqHandle);
	case EMobilePhoneTerminateAllCalls:
		return TerminateAllCallsCancel(aTsyReqHandle);
    case EMobilePhoneTerminateActiveCalls:
        return TerminateActiveCallsCancel(aTsyReqHandle);
	case EMobilePhoneNotifySendNetworkServiceRequest:
		return NotifySendNetworkServiceRequestCancel(aTsyReqHandle);
	case EMobilePhoneNotifyAllSendNetworkServiceRequest:
		return NotifyAllSendNetworkServiceRequestCancel(aTsyReqHandle);
	case EMobilePhoneClearBlacklist:
		return ClearBlacklistCancel(aTsyReqHandle);
	case EMobilePhoneNotifyDTMFEvent:
		return NotifyDTMFEventCancel(aTsyReqHandle);
	case EMobilePhoneGetPersonalisationCaps:
		return GetPersonalisationCapsCancel(aTsyReqHandle);
	case EMobilePhoneGetPersonalisationStatus:
		return GetPersonalisationStatusCancel(aTsyReqHandle);
	case EMobilePhoneSetPersonalisationStatus:
		return SetPersonalisationStatusCancel(aTsyReqHandle);
	case EMobilePhoneGetNetworkName:
		return GetCurrentNetworkNameCancel(aTsyReqHandle);
	case EMobilePhoneGetPreferredNetworksPhase1:
	case EMobilePhoneGetPreferredNetworksPhase2:
		return GetPreferredNetworksCancel(aTsyReqHandle);
	case EMobilePhoneStorePreferredNetworksList:
		return StorePreferredNetworksListCancel(aTsyReqHandle);
	case EMobilePhoneNotifyStorePreferredNetworksListChange:
		return NotifyStorePreferredNetworksListChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetCipheringIndicatorStatus:
		return GetCipheringIndicatorStatusCancel(aTsyReqHandle);
	case EMobilePhoneNotifyCipheringIndicatorStatus:
		return NotifyCipheringIndicatorStatusCancel(aTsyReqHandle);
	case EMobilePhoneSetIdentityServiceStatus:
		return SetIdentityServiceStatusCancel(aTsyReqHandle);
	case EMobilePhoneNotifyIdentityServiceStatus:
		return NotifyIdentityServiceStatusCancel(aTsyReqHandle);
	case EMobilePhoneNotifyIdentitySuppressionRejected:
		return NotifyIdentitySuppressionRejectedCancel(aTsyReqHandle);
	case EMobilePhoneNotifyMulticallIndicatorChange:
		return NotifyMulticallIndicatorChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyImsAuthorizationInfoChanged:
		return NotifyImsAuthorizationInfoChangedCancel(aTsyReqHandle);
	case EMobilePhoneAuthorizationInfoPhase1:
	case EMobilePhoneAuthorizationInfoPhase2:
		return GetAuthorizationInfoCancel(aTsyReqHandle);
	case EMobilePhoneIMSAuthenticate:
		return GetAuthenticationDataCancel(aTsyReqHandle);
	case EMobilePhoneSmartCardAuthenticate:
		return GetSmartCardAuthenticationDataCancel(aTsyReqHandle);
	case EMobilePhoneGetIccIdentity:
		return GetIccIdentityCancel(aTsyReqHandle);
	case EMobilePhoneNotifyAuthenticateDataChange:
		return NotifyAuthenticateDataChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetAuthenticationParams:
		return GetAuthenticationParamsCancel(aTsyReqHandle);
	case EMobilePhoneSetGbaBootstrapParams:
		return SetGbaBootstrapParamsCancel(aTsyReqHandle);
	case EMobilePhoneAuthenticationListPhase1:
	case EMobilePhoneAuthenticationListPhase2:
		return GetAuthenticationListCancel(aTsyReqHandle);
	case EMobilePhoneNotifyWlanDataChange:
		return NotifyWlanDataChangeCancel(aTsyReqHandle);
	case EMobilePhoneNotifyPreferredWlanSIDListChange:
		return NotifyPreferredWlanSIDListChangeCancel(aTsyReqHandle);
	case EMobilePhoneGetWlanData:
		return GetWlanDataCancel(aTsyReqHandle);
	case EMobilePhoneSetWlanData:
		return SetWlanDataCancel(aTsyReqHandle);
	case EMobilePhoneGetPreferredWlanSIDsPhase1:
	case EMobilePhoneGetPreferredWlanSIDsPhase2:
		return GetPreferredWlanSIDsCancel(aTsyReqHandle);
	case EMobilePhoneStorePreferredWlanSIDList:
		return StorePreferredWlanSIDListCancel(aTsyReqHandle);
	
	default:
		return CPhoneBase::CancelService(aIpc,aTsyReqHandle);
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

TInt CPhoneDMmTsy::GetMultimodeCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetMultimodeCaps called"));
	*aCaps=DMMTSY_PHONE_MULTIMODE_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetPhoneStoreInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aGetInfo,
		TDes* aName)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPhoneStoreInfo called"));

	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg *infoPckg = REINTERPRET_CAST(RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg *,aGetInfo);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 &info = (*infoPckg)();

	if (aName->Compare(DMMTSY_PHONE_STORE_NAME1) == 0)
		{
		info.iName=DMMTSY_PHONE_STORE_NAME1;
		info.iType=DMMTSY_PHONE_STORE_TYPE1;
		info.iTotalEntries=DMMTSY_PHONE_STORE_TOTAL_ENTRIES1;
		info.iUsedEntries=DMMTSY_PHONE_STORE_USED_ENTRIES1;
		info.iCaps=DMMTSY_PHONE_STORE_CAPS1;
		AddDelayedReq(aTsyReqHandle,this);
		}
	else if (aName->Compare(DMMTSY_PHONE_STORE_NAME2) == 0)
		{
		info.iName=DMMTSY_PHONE_STORE_NAME2;
		info.iType=DMMTSY_PHONE_STORE_TYPE2;
		info.iTotalEntries=DMMTSY_PHONE_STORE_TOTAL_ENTRIES2;
		info.iUsedEntries=DMMTSY_PHONE_STORE_USED_ENTRIES2;
		info.iCaps=DMMTSY_PHONE_STORE_CAPS2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetPhoneStoreInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aGetInfo, 
		RMobilePhone::TPhoneStoreNameAndIccType* aTable)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPhoneStoreInfo called using IccType"));

	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg *infoPckg = REINTERPRET_CAST(RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg *,aGetInfo);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 &info = (*infoPckg)();

	if ((aTable->iStoreName.Compare(DMMTSY_PHONE_STORE_NAME1) == 0) &&
				(aTable->iMode.Compare(DMMTSY_PHONEBOOK_TYPE) == 0))
		{
		info.iName=DMMTSY_PHONE_STORE_NAME1;
		info.iType=DMMTSY_PHONE_STORE_TYPE1;
		info.iTotalEntries=DMMTSY_PHONE_STORE_TOTAL_ENTRIES1;
		info.iUsedEntries=DMMTSY_PHONE_STORE_USED_ENTRIES1;
		info.iCaps=DMMTSY_PHONE_STORE_CAPS1;
		AddDelayedReq(aTsyReqHandle,this);
		}
	else if ((aTable->iStoreName.Compare(DMMTSY_PHONE_STORE_NAME2) == 0) &&
					(aTable->iMode.Compare(DMMTSY_PHONEBOOK_TYPE) == 0))
		{
		info.iName=DMMTSY_PHONE_STORE_NAME2;
		info.iType=DMMTSY_PHONE_STORE_TYPE2;
		info.iTotalEntries=DMMTSY_PHONE_STORE_TOTAL_ENTRIES2;
		info.iUsedEntries=DMMTSY_PHONE_STORE_USED_ENTRIES2;
		info.iCaps=DMMTSY_PHONE_STORE_CAPS2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetPhoneStoreInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPhoneStoreInfoCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetIccAccessCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIccAccessCaps called"));
	*aCaps=DMMTSY_PHONE_SIM_ACCESS_CAPS1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyIccAccessCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	if (!iNotifyIccAccessCaps++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyIccAccessCapsChange called"));
		*aCaps=DMMTSY_PHONE_SIM_ACCESS_CAPS2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyIccAccessCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyIccAccessCapsChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetIccIdentity(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TIccIdentity* aIdentity)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIccIdentity called"));
	*aIdentity=DMMTSY_PHONE_ICC_ID;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetIccIdentityCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIccIdentity called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCustomerServiceProfile(const TTsyReqHandle aTsyReqHandle, TDes8* aFile)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCustomerServiceProfile called"));
	RMobilePhone::TMobilePhoneCspFileV1Pckg* cspV1Pckg = (RMobilePhone::TMobilePhoneCspFileV1Pckg*)aFile;
	RMobilePhone::TMobilePhoneCspFileV1& cspV1 = (*cspV1Pckg)();

	cspV1.iCallOfferingServices=DMMTSY_PHONE_CSP_BYTE1;
	cspV1.iCallRestrictionServices=DMMTSY_PHONE_CSP_BYTE2;
	cspV1.iOtherSuppServices=DMMTSY_PHONE_CSP_BYTE3;
	cspV1.iCallCompletionServices=DMMTSY_PHONE_CSP_BYTE4;
	cspV1.iTeleservices=DMMTSY_PHONE_CSP_BYTE5;
	cspV1.iCphsTeleservices=DMMTSY_PHONE_CSP_BYTE6;
	cspV1.iCphsFeatures=DMMTSY_PHONE_CSP_BYTE7;
	cspV1.iNumberIdentServices=DMMTSY_PHONE_CSP_BYTE8;
	cspV1.iPhase2PlusServices=DMMTSY_PHONE_CSP_BYTE9;
	cspV1.iValueAddedServices=DMMTSY_PHONE_CSP_BYTE10;

	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCustomerServiceProfileCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCustomerServiceProfileCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetServiceTable(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneServiceTable* aTable, TDes8* aFile)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetServiceTable called"));
	RMobilePhone::TMobilePhoneServiceTableV1Pckg* sstFilePckg = (RMobilePhone::TMobilePhoneServiceTableV1Pckg*) aFile;
	RMobilePhone::TMobilePhoneServiceTableV1 &sstFile = (*sstFilePckg)();
		
	if (*aTable!=DMMTSY_PHONE_SERVICE_TABLE)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		{
		if(sstFile.ExtensionId()==KEtelExtMultimodeV8)
			{
			LOGTEXT(_L8("CPhoneDMmTsy::GetServiceTable is using V8table"));
			RMobilePhone::TMobilePhoneServiceTableV8Pckg* sstV8Pckg = (RMobilePhone::TMobilePhoneServiceTableV8Pckg*)aFile;
			RMobilePhone::TMobilePhoneServiceTableV8& sstV8 = (*sstV8Pckg)();

			sstV8.iServices1To8=DMMTSY_PHONE_SST_BYTE1;
			sstV8.iServices9To16=DMMTSY_PHONE_SST_BYTE2;
			sstV8.iServices17To24=DMMTSY_PHONE_SST_BYTE3;
			sstV8.iServices25To32=DMMTSY_PHONE_SST_BYTE4;
			sstV8.iServices33To40=DMMTSY_PHONE_SST_BYTE5;
			sstV8.iServices41To48=DMMTSY_PHONE_SST_BYTE6;
			sstV8.iServices49To56=DMMTSY_PHONE_SST_BYTE7;
			sstV8.iAID=DMMTSY_PHONE_USIM_AID;			
			sstV8.iServices57To64=DMMTSY_PHONE_SST_BYTE8;
			sstV8.iServices65To72=DMMTSY_PHONE_SST_BYTE9;
			}
			
		if(sstFile.ExtensionId()==KETelExtMultimodeV2)
			{
			LOGTEXT(_L8("CPhoneDMmTsy::GetServiceTable is using V2table"));
			RMobilePhone::TMobilePhoneServiceTableV2Pckg* sstV2Pckg = (RMobilePhone::TMobilePhoneServiceTableV2Pckg*)aFile;
			RMobilePhone::TMobilePhoneServiceTableV2& sstV2 = (*sstV2Pckg)();

			sstV2.iServices1To8=DMMTSY_PHONE_SST_BYTE1;
			sstV2.iServices9To16=DMMTSY_PHONE_SST_BYTE2;
			sstV2.iServices17To24=DMMTSY_PHONE_SST_BYTE3;
			sstV2.iServices25To32=DMMTSY_PHONE_SST_BYTE4;
			sstV2.iServices33To40=DMMTSY_PHONE_SST_BYTE5;
			sstV2.iServices41To48=DMMTSY_PHONE_SST_BYTE6;
			sstV2.iServices49To56=DMMTSY_PHONE_SST_BYTE7;
			sstV2.iAID=DMMTSY_PHONE_USIM_AID;
			}
		else if(sstFile.ExtensionId()==KETelExtMultimodeV1)
			{
			LOGTEXT(_L8("CPhoneDMmTsy::GetServiceTable is using V1table"));

			sstFile.iServices1To8=DMMTSY_PHONE_SST_BYTE1;
			sstFile.iServices9To16=DMMTSY_PHONE_SST_BYTE2;
			sstFile.iServices17To24=DMMTSY_PHONE_SST_BYTE3;
			sstFile.iServices25To32=DMMTSY_PHONE_SST_BYTE4;
			sstFile.iServices33To40=DMMTSY_PHONE_SST_BYTE5;
			sstFile.iServices41To48=DMMTSY_PHONE_SST_BYTE6;
			sstFile.iServices49To56=DMMTSY_PHONE_SST_BYTE7;
			}

		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetServiceTableCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetServiceTableCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetBatteryCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetBatteryCaps called"));
	*aCaps=DMMTSY_PHONE_BATTERY_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetBatteryInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneBatteryInfoV1* aInfo)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetBatteryInfo called"));
	aInfo->iStatus=DMMTSY_PHONE_BATTERY_STATUS1;
	aInfo->iChargeLevel=DMMTSY_PHONE_BATTERY_CHARGE1;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetBatteryInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetBatteryInfoCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyBatteryInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneBatteryInfoV1* aInfo)
	{
	if (!iNotifyBatteryInfoChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyBatteryInfoChange called"));
		aInfo->iStatus=DMMTSY_PHONE_BATTERY_STATUS2;
		aInfo->iChargeLevel=DMMTSY_PHONE_BATTERY_CHARGE2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyBatteryInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyBatteryInfoChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetSignalCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSignalCaps called"));
	*aCaps=DMMTSY_PHONE_SIGNAL_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetSignalStrength(const TTsyReqHandle aTsyReqHandle, TInt32* aSignalStrength, TInt8* aBar)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSignalStrength called"));
	*aSignalStrength=DMMTSY_PHONE_SIGNAL_STRENGTH1;
	*aBar=DMMTSY_PHONE_SIGNAL_BAR1;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetSignalStrengthCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSignalStrengthCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifySignalStrengthChange(const TTsyReqHandle aTsyReqHandle, TInt32* aSignalStrength, TInt8* aBar)
	{
	if (!iNotifySignalStrengthChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifySignalStrengthChange called"));
		*aSignalStrength=DMMTSY_PHONE_SIGNAL_STRENGTH2;
		*aBar=DMMTSY_PHONE_SIGNAL_BAR2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifySignalStrengthChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifySignalStrengthChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetIndicatorCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aActionCaps, TUint32* aIndCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIndicatorCaps called"));
	*aActionCaps=DMMTSY_PHONE_ACTION_INDICATOR_CAPS;
	*aIndCaps=DMMTSY_PHONE_INDICATOR_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetIndicator(const TTsyReqHandle aTsyReqHandle, TUint32* aIndicator)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIndicator called"));
	*aIndicator=DMMTSY_PHONE_INDICATOR1;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetIndicatorCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIndicatorCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyIndicatorChange(const TTsyReqHandle aTsyReqHandle, TUint32* aIndicator)
	{
	if (!iNotifyIndicatorChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyIndicatorChange called"));
		*aIndicator=DMMTSY_PHONE_INDICATOR2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyIndicatorChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyIndicatorChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetIdentityCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIdentityCaps called"));
	*aCaps=DMMTSY_PHONE_IDENTITY_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetPhoneId(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIdentityV1* aId)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPhoneId called"));
	aId->iManufacturer=DMMTSY_PHONE_MANUFACTURER;
	aId->iModel=DMMTSY_PHONE_MODEL;
	aId->iRevision=DMMTSY_PHONE_REVISION;
	aId->iSerialNumber=DMMTSY_PHONE_SERIAL_NUMBER;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetPhoneIdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPhoneIdCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetSubscriberId(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSubscriberId* aId)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSubscriberId called"));
	*aId=DMMTSY_PHONE_SUBSCRIBER_ID;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetSubscriberIdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSubscriberIdCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetDTMFCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDTMFCaps called"));
	*aCaps=DMMTSY_PHONE_DTMF_CAPS1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyDTMFCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	if (!iNotifyDTMFCapsChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyDTMFCapsChange called"));
		*aCaps=DMMTSY_PHONE_DTMF_CAPS2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyDTMFCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyDTMFCapsChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyStopInDTMFString(const TTsyReqHandle aTsyReqHandle)
	{
	if (!iNotifyStopInDTMFString++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyStopInDTMFString called"));
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyStopInDTMFStringCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyStopInDTMFStringCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::ContinueDTMFStringSending(const TTsyReqHandle aTsyReqHandle, TBool *aContinue)
	{
	TInt err = KErrNone;
	if (!(*aContinue))
		err = KErrCorrupt;
	ReqCompleted(aTsyReqHandle,err);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SendDTMFTones(const TTsyReqHandle aTsyReqHandle, TDesC* aTones)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SendDTMFTones called"));
	if (aTones->Length()!=DMMTSY_PHONE_SEND_TONES_LENGTH)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else if (aTones->Compare(DMMTSY_PHONE_SEND_TONES)!=KErrNone)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SendDTMFTonesCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SendDTMFTonesCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::ReadDTMFTones(const TTsyReqHandle aTsyReqHandle, TDes* aTones)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ReadDTMFTones called"));
	aTones->Copy(DMMTSY_PHONE_READ_TONES);
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::ReadDTMFTonesCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ReadDTMFTonesCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::StartDTMFTone(const TTsyReqHandle aTsyReqHandle, TChar* aTone)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::StartDTMFTone called"));
	if (*aTone != DMMTSY_PHONE_SINGLE_TONE)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CPhoneDMmTsy::StopDTMFTone(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::StopDTMFTone called"));
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetNetworkCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetNetworkCaps called"));
	*aCaps=DMMTSY_PHONE_NETWORK_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCurrentMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkMode* aMode)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCurrentMode called"));
	*aMode=DMMTSY_PHONE_NETWORK_MODE1;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyModeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkMode* aMode)
	{
	if (!iNotifyModeChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyModeChange called"));
		*aMode=DMMTSY_PHONE_NETWORK_MODE2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyModeChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyModeChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetHomeNetwork(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkInfoPckg)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetHomeNetwork called"));
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* networkInfoV1Pckg = (RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*)aNetworkInfoPckg;
	RMobilePhone::TMobilePhoneNetworkInfoV1& networkInfoV1 = (*networkInfoV1Pckg)();
	
	networkInfoV1.iMode=DMMTSY_PHONE_HOME_NETWORK_MODE;
	networkInfoV1.iStatus=DMMTSY_PHONE_HOME_NETWORK_STATUS;
	networkInfoV1.iBandInfo=DMMTSY_PHONE_HOME_NETWORK_BAND;
	networkInfoV1.iCountryCode=DMMTSY_PHONE_HOME_NETWORK_MCC;
	networkInfoV1.iNetworkId=DMMTSY_PHONE_HOME_NETWORK_ID;
	networkInfoV1.iCdmaSID=DMMTSY_PHONE_HOME_NETWORK_SID;
	networkInfoV1.iAnalogSID=DMMTSY_PHONE_HOME_NETWORK_SID;
	networkInfoV1.iDisplayTag=DMMTSY_PHONE_HOME_NETWORK_TAG;
	networkInfoV1.iShortName=DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME;
	networkInfoV1.iLongName=DMMTSY_PHONE_HOME_NETWORK_LONG_NAME;

	if(networkInfoV1.ExtensionId() == KEtelExtMultimodeV8)
	{
		RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* networkInfoV8Pckg = (RMobilePhone::TMobilePhoneNetworkInfoV8Pckg*)aNetworkInfoPckg;
		RMobilePhone::TMobilePhoneNetworkInfoV8& networkInfoV8 = (*networkInfoV8Pckg)();
		networkInfoV8.iAccess=DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH;
		networkInfoV8.iHsdpaAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY;
		networkInfoV8.iEgprsAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY;
		networkInfoV8.iHsupaAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_HSUPA_AVAILABILITY;
	}

	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetHomeNetworkCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetHomeNetworkCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCurrentNetwork(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkInfoPckg, RMobilePhone::TMobilePhoneLocationAreaV1* aArea)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCurrentNetwork called"));
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* networkInfoV1Pckg = (RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*)aNetworkInfoPckg;
	RMobilePhone::TMobilePhoneNetworkInfoV1& networkInfoV1 = (*networkInfoV1Pckg)();
	
	if(networkInfoV1.ExtensionId() == KETelExt3rdPartyV1)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::GetCurrentNetworkISV called"));
		RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1Pckg* networkInfoISVPckg = (RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1Pckg*)aNetworkInfoPckg;
		RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1& networkInfoISV = (*networkInfoISVPckg)();

		networkInfoISV.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE_ISV;
		networkInfoISV.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS_ISV;
		networkInfoISV.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND_ISV;
		networkInfoISV.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC_ISV;
		networkInfoISV.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID_ISV;
		networkInfoISV.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG_ISV;
		networkInfoISV.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME_ISV;
		networkInfoISV.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME_ISV;
		networkInfoISV.iCdmaSID=DMMTSY_PHONE_CURRENT_NETWORK_CDMASID_ISV;
		networkInfoISV.iAccess=DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_UTRAN_ISV;
		
		if(aArea)
			{
			aArea->iAreaKnown=DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN_ISV;
			aArea->iLocationAreaCode=DMMTSY_PHONE_CURRENT_NETWORK_LAC_ISV;
			aArea->iCellId=DMMTSY_PHONE_CURRENT_NETWORK_CELLID_ISV;
			
			}	
		
		}
	else
		{
		if(networkInfoV1.ExtensionId() == KEtelExtMultimodeV8)
			{
			RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* networkInfoV8Pckg = (RMobilePhone::TMobilePhoneNetworkInfoV8Pckg*)aNetworkInfoPckg;
			RMobilePhone::TMobilePhoneNetworkInfoV8& networkInfoV8 = (*networkInfoV8Pckg)();
			networkInfoV8.iAccess = DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH1;
			
			networkInfoV8.iHsdpaAvailableIndicator = DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY1;
			networkInfoV8.iEgprsAvailableIndicator = DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY1;
			networkInfoV8.iHsupaAvailableIndicator = DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY1;
			}
			

		else if(networkInfoV1.ExtensionId() == KEtelExtMultimodeV5)
			{
			RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* networkInfoV5Pckg = (RMobilePhone::TMobilePhoneNetworkInfoV5Pckg*)aNetworkInfoPckg;
			RMobilePhone::TMobilePhoneNetworkInfoV5& networkInfoV5 = (*networkInfoV5Pckg)();
			networkInfoV5.iAccess = DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_GSM;
			
			networkInfoV5.iHsdpaAvailableIndicator = DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY;
			networkInfoV5.iEgprsAvailableIndicator = DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY;
			}
			
		else if(networkInfoV1.ExtensionId() == KETelExtMultimodeV2)
			{
			RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* networkInfoV2Pckg = (RMobilePhone::TMobilePhoneNetworkInfoV2Pckg*)aNetworkInfoPckg;
			RMobilePhone::TMobilePhoneNetworkInfoV2& networkInfoV2 = (*networkInfoV2Pckg)();
			networkInfoV2.iAccess = DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_GSM;
			}
		
		networkInfoV1.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE1;
		networkInfoV1.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS1;
		networkInfoV1.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND1;
		networkInfoV1.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC1;
		networkInfoV1.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID1;
		networkInfoV1.iCdmaSID=DMMTSY_PHONE_CURRENT_NETWORK_SID;
		networkInfoV1.iAnalogSID=DMMTSY_PHONE_CURRENT_NETWORK_SID;
		networkInfoV1.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG1;
		networkInfoV1.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1;
		networkInfoV1.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1;

		if(aArea)
			{
			aArea->iAreaKnown=DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN;
			aArea->iLocationAreaCode=DMMTSY_PHONE_CURRENT_NETWORK_LAC1;
			aArea->iCellId=DMMTSY_PHONE_CURRENT_NETWORK_CELLID1;
			}	
		}

	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCurrentNetworkCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCurrentNetworkCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCurrentNetworkChange(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkInfoPckg, RMobilePhone::TMobilePhoneLocationAreaV1* aArea)
	{
	if (!iNotifyCurrentNetworkChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCurrentNetworkChange called"));

		RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* networkInfoPckg = (RMobilePhone::TMobilePhoneNetworkInfoV1Pckg*)aNetworkInfoPckg;
		RMobilePhone::TMobilePhoneNetworkInfoV1& networkInfo = (*networkInfoPckg)();
		
		if(networkInfo.ExtensionId() == KETelExt3rdPartyV1)
			{
			LOGTEXT(_L8("CPhoneDMmTsy::GetCurrentNetworkISV called"));
			RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1Pckg* networkInfoISVPckg = (RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1Pckg*)aNetworkInfoPckg;
			RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1& networkInfoISV = (*networkInfoISVPckg)();

			networkInfoISV.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE_ISV2;
			networkInfoISV.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS_ISV2;
			networkInfoISV.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND_ISV2;
			networkInfoISV.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC_ISV2;
			networkInfoISV.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID_ISV2;
			networkInfoISV.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG_ISV2;
			networkInfoISV.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME_ISV2;
			networkInfoISV.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME_ISV2;
			networkInfoISV.iCdmaSID=DMMTSY_PHONE_CURRENT_NETWORK_CDMASID_ISV2;
			networkInfoISV.iAccess=DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_UTRAN_ISV2;
					
			aArea->iAreaKnown=DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN_ISV2;
			aArea->iLocationAreaCode=DMMTSY_PHONE_CURRENT_NETWORK_LAC_ISV2;
			aArea->iCellId=DMMTSY_PHONE_CURRENT_NETWORK_CELLID_ISV2;		
			
			}
		else
			{
			if(networkInfo.ExtensionId() == KETelExtMultimodeV2)
				{
				RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* networkInfoV2Pckg = (RMobilePhone::TMobilePhoneNetworkInfoV2Pckg*)aNetworkInfoPckg;
				RMobilePhone::TMobilePhoneNetworkInfoV2& networkInfoV2 = (*networkInfoV2Pckg)();
				networkInfoV2.iAccess = DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_GSM;
				//reset for V1
				iNotifyCurrentNetworkChange = 0;
				}		

			if(networkInfo.ExtensionId() == KEtelExtMultimodeV8)
				{
				RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* networkInfoV8Pckg = (RMobilePhone::TMobilePhoneNetworkInfoV8Pckg*)aNetworkInfoPckg;
				RMobilePhone::TMobilePhoneNetworkInfoV8& networkInfoV8 = (*networkInfoV8Pckg)();
				networkInfoV8.iAccess = DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH2;
				//reset for V8
				networkInfoV8.iHsdpaAvailableIndicator= DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY2;
				networkInfoV8.iEgprsAvailableIndicator= DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY2;
				networkInfoV8.iHsupaAvailableIndicator= DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY2;
				iNotifyCurrentNetworkChange = 0;
				}		

			networkInfo.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE2;
			networkInfo.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS2;
			networkInfo.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND2;
			networkInfo.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC2;
			networkInfo.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID2;
			networkInfo.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG2;
			networkInfo.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2;
			networkInfo.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2;

			if(aArea)
				{
				aArea->iAreaKnown=DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN;
				aArea->iLocationAreaCode=DMMTSY_PHONE_CURRENT_NETWORK_LAC2;
				aArea->iCellId=DMMTSY_PHONE_CURRENT_NETWORK_CELLID2;
				}
				

			}
		//reset for etel3rd
		iNotifyCurrentNetworkChange = 0;

		AddDelayedReq(aTsyReqHandle,this);	
		}
	else
		{
		//reset for multiple completion test
		iNotifyCurrentNetworkChange = 0;
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCurrentNetworkChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCurrentNetworkChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCellInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aCellInfoPckg)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCellInfo called"));
	RMobilePhone::TMobilePhoneCellInfoV9Pckg* cellInfoPckg = (RMobilePhone::TMobilePhoneCellInfoV9Pckg*)aCellInfoPckg;
	RMobilePhone::TMobilePhoneCellInfoV9& cellInfo = (*cellInfoPckg)();
	
	cellInfo.iMode = DMMTSY_PHONE_CELL_INFO_MODE;
	cellInfo.iCellId = DMMTSY_PHONE_CELL_INFO_CELLID;
	cellInfo.iTimingAdvance = DMMTSY_PHONE_CELL_INFO_TIMING_ADVANCE;	
	cellInfo.iStatus = DMMTSY_PHONE_CELL_INFO_STATUS;
	
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCellInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCellInfoCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCellInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCellInfoPckg)
	{
	if (!iNotifyCellInfoChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCellInfoChange called"));

		RMobilePhone::TMobilePhoneCellInfoV9Pckg* cellInfoPckg = (RMobilePhone::TMobilePhoneCellInfoV9Pckg*)aCellInfoPckg;
		RMobilePhone::TMobilePhoneCellInfoV9& cellInfo = (*cellInfoPckg)();
	
		cellInfo.iMode = DMMTSY_PHONE_CELL_INFO_MODE2;
		cellInfo.iCellId = DMMTSY_PHONE_CELL_INFO_CELLID2;
		cellInfo.iTimingAdvance = DMMTSY_PHONE_CELL_INFO_TIMING_ADVANCE2;
		cellInfo.iStatus = DMMTSY_PHONE_CELL_INFO_STATUS2;
		
		AddDelayedReq(aTsyReqHandle,this);	
		}
	else
		{
		//reset for multiple completion test
		iNotifyCellInfoChange = 0;
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCellInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCellInfoChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetNetworkRegistrationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneRegistrationStatus* aStatus)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetNetworkRegistrationStatus called"));
	*aStatus=DMMTSY_PHONE_REGISTRATION_STATUS1;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetNetworkRegistrationStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetNetworkRegistrationStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyNetworkRegistrationStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneRegistrationStatus* aStatus)
	{
	if(!iNotifyNetworkRegistrationStatusChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkRegistrationStatusChange called"));
		*aStatus=DMMTSY_PHONE_REGISTRATION_STATUS2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyNetworkRegistrationStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkRegistrationStatusChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetDetectedNetworksPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksPhase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetDetectedNetworksPhase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	}

TInt CPhoneDMmTsy::ProcessGetDetectedNetworksPhase1L(const TTsyReqHandle aTsyReqHandle, 
													 RMobilePhone::TClientId* aClient, 
													 TInt* aBufSize)
	{
	// retrieve detected networks from phone, 
	// store each network as a list entry,
	// stream the list and then return size of this buffer to client
	CMobilePhoneNetworkList* list=CMobilePhoneNetworkList::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TMobilePhoneNetworkInfoV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_NETWORK_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE1;
			entry.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS1;
			entry.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND1;
			entry.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC1;
			entry.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID1;
			entry.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG1;
			entry.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1;
			entry.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1;
			break;
		case 1:
			entry.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE2;
			entry.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS2;
			entry.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND2;
			entry.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC2;
			entry.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID2;
			entry.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG2;
			entry.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2;
			entry.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2;
			break;
		case 2:
		default:
			entry.iMode=DMMTSY_PHONE_HOME_NETWORK_MODE;
			entry.iStatus=DMMTSY_PHONE_HOME_NETWORK_STATUS;
			entry.iBandInfo=DMMTSY_PHONE_HOME_NETWORK_BAND;
			entry.iCountryCode=DMMTSY_PHONE_HOME_NETWORK_MCC;
			entry.iNetworkId=DMMTSY_PHONE_HOME_NETWORK_ID;
			entry.iDisplayTag=DMMTSY_PHONE_HOME_NETWORK_TAG;
			entry.iShortName=DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME;
			entry.iLongName=DMMTSY_PHONE_HOME_NETWORK_LONG_NAME;
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
	
	iGetDetectedNetworks->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}



TInt CPhoneDMmTsy::GetDetectedNetworksPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the get detected network attempt from this client
	for (TInt i=0; i<iGetDetectedNetworks->Count(); ++i)
		{
		read = iGetDetectedNetworks->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetDetectedNetworks->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CPhoneDMmTsy::GetDetectedNetworksCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	// Remove the read all attempt from iGetDetectedNetworks
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetDetectedNetworks->Count(); ++i)
		{
		read = iGetDetectedNetworks->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetDetectedNetworks->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetDetectedNetworksV2Phase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksV2Phase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetDetectedNetworksV2Phase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	}

TInt CPhoneDMmTsy::ProcessGetDetectedNetworksV2Phase1L(const TTsyReqHandle aTsyReqHandle, 
													 RMobilePhone::TClientId* aClient, 
													 TInt* aBufSize)
	{
	// Retrieve detected networks from phone, 
	// Store each network as a list entry of type TMobilePhoneNetworkInfoV2,
	// Stream the list and then return size of this buffer to client
	CMobilePhoneNetworkListV2* list=CMobilePhoneNetworkListV2::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TMobilePhoneNetworkInfoV2 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_NETWORK_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE1;
			entry.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS1;
			entry.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND1;
			entry.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC1;
			entry.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID1;
			entry.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG1;
			entry.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1;
			entry.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1;
			entry.iAccess=DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH1;
			break;
		case 1:
			entry.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE2;
			entry.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS2;
			entry.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND2;
			entry.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC2;
			entry.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID2;
			entry.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG2;
			entry.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2;
			entry.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2;
			entry.iAccess=DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH2;
			break;
		case 2:
			entry.iMode=DMMTSY_PHONE_HOME_NETWORK_MODE;
			entry.iStatus=DMMTSY_PHONE_HOME_NETWORK_STATUS;
			entry.iBandInfo=DMMTSY_PHONE_HOME_NETWORK_BAND;
			entry.iCountryCode=DMMTSY_PHONE_HOME_NETWORK_MCC;
			entry.iNetworkId=DMMTSY_PHONE_HOME_NETWORK_ID;
			entry.iDisplayTag=DMMTSY_PHONE_HOME_NETWORK_TAG;
			entry.iShortName=DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME;
			entry.iLongName=DMMTSY_PHONE_HOME_NETWORK_LONG_NAME;
			entry.iAccess=DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH;
			break;
		case 3:
		default:
			entry.iMode=DMMTSY_PHONE_HOME_NETWORK_MODE;
			entry.iStatus=DMMTSY_PHONE_HOME_NETWORK_STATUS;
			entry.iBandInfo=DMMTSY_PHONE_HOME_NETWORK_BAND;
			entry.iCountryCode=DMMTSY_PHONE_HOME_NETWORK_MCC;
			entry.iNetworkId=DMMTSY_PHONE_HOME_NETWORK_ID;
			entry.iDisplayTag=DMMTSY_PHONE_HOME_NETWORK_TAG;
			entry.iShortName=DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME;
			entry.iLongName=DMMTSY_PHONE_HOME_NETWORK_LONG_NAME;
			entry.iAccess=DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH3;
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
	
	iGetDetectedNetworks->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}



TInt CPhoneDMmTsy::GetDetectedNetworksV2Phase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksV2Phase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the get detected network attempt from this client
	for (TInt i=0; i<iGetDetectedNetworks->Count(); ++i)
		{
		read = iGetDetectedNetworks->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetDetectedNetworks->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CPhoneDMmTsy::GetDetectedNetworksV2Cancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksV2Cancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	// Remove the read all attempt from iGetDetectedNetworks
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetDetectedNetworks->Count(); ++i)
		{
		read = iGetDetectedNetworks->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetDetectedNetworks->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

//
// GetDetectedNetworksV8 //
//
TInt CPhoneDMmTsy::GetDetectedNetworksV8Phase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksV8Phase1 called"));
	TInt ret=KErrNone;
	TRAPD(leaveCode, ret=ProcessGetDetectedNetworksV8Phase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	}

TInt CPhoneDMmTsy::ProcessGetDetectedNetworksV8Phase1L(const TTsyReqHandle aTsyReqHandle, 
					RMobilePhone::TClientId* aClient, TInt* aBufSize)
	{
	// Retrieve detected networks from phone, 
	// Store each network as a list entry of type TMobilePhoneNetworkInfoV8,
	// Stream the list and then return size of this buffer to client
	CMobilePhoneNetworkListV8* list=CMobilePhoneNetworkListV8::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TMobilePhoneNetworkInfoV8 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_NETWORK_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE1;
			entry.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS1;
			entry.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND1;
			entry.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC1;
			entry.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID1;
			entry.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG1;
			entry.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1;
			entry.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1;
			entry.iAccess=DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH1;
			entry.iHsdpaAvailableIndicator=DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY1;
			entry.iEgprsAvailableIndicator=DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY1;
			entry.iHsupaAvailableIndicator=DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY1;
			break;
		case 1:
			entry.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE2;
			entry.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS2;
			entry.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND2;
			entry.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC2;
			entry.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID2;
			entry.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG2;
			entry.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2;
			entry.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2;
			entry.iAccess=DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH2;
			entry.iHsdpaAvailableIndicator=DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY2;
			entry.iEgprsAvailableIndicator=DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY2;
			entry.iHsupaAvailableIndicator=DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY2;
			break;
		case 2:
			entry.iMode=DMMTSY_PHONE_HOME_NETWORK_MODE;
			entry.iStatus=DMMTSY_PHONE_HOME_NETWORK_STATUS;
			entry.iBandInfo=DMMTSY_PHONE_HOME_NETWORK_BAND;
			entry.iCountryCode=DMMTSY_PHONE_HOME_NETWORK_MCC;
			entry.iNetworkId=DMMTSY_PHONE_HOME_NETWORK_ID;
			entry.iDisplayTag=DMMTSY_PHONE_HOME_NETWORK_TAG;
			entry.iShortName=DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME;
			entry.iLongName=DMMTSY_PHONE_HOME_NETWORK_LONG_NAME;
			entry.iAccess=DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH;
			entry.iHsdpaAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY;
			entry.iEgprsAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY;
			entry.iHsupaAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_HSUPA_AVAILABILITY;
			break;
		case 3:
		default:
			entry.iMode=DMMTSY_PHONE_HOME_NETWORK_MODE;
			entry.iStatus=DMMTSY_PHONE_HOME_NETWORK_STATUS;
			entry.iBandInfo=DMMTSY_PHONE_HOME_NETWORK_BAND;
			entry.iCountryCode=DMMTSY_PHONE_HOME_NETWORK_MCC;
			entry.iNetworkId=DMMTSY_PHONE_HOME_NETWORK_ID;
			entry.iDisplayTag=DMMTSY_PHONE_HOME_NETWORK_TAG;
			entry.iShortName=DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME;
			entry.iLongName=DMMTSY_PHONE_HOME_NETWORK_LONG_NAME;
			entry.iAccess=DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH3;
			entry.iHsdpaAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY;
			entry.iEgprsAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY;
			entry.iHsupaAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_HSUPA_AVAILABILITY;
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
	
	iGetDetectedNetworks->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetDetectedNetworksV8Phase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksV8Phase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the get detected network attempt from this client
	for (TInt i=0; i<iGetDetectedNetworks->Count(); ++i)
		{
		read = iGetDetectedNetworks->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetDetectedNetworks->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CPhoneDMmTsy::GetDetectedNetworksV8Cancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksV8Cancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	// Remove the read all attempt from iGetDetectedNetworks
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetDetectedNetworks->Count(); ++i)
		{
		read = iGetDetectedNetworks->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetDetectedNetworks->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


//
// GetDetectedNetworksV5 //
//
TInt CPhoneDMmTsy::GetDetectedNetworksV5Phase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksV5Phase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetDetectedNetworksV5Phase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	}

TInt CPhoneDMmTsy::ProcessGetDetectedNetworksV5Phase1L(const TTsyReqHandle aTsyReqHandle, 
													 RMobilePhone::TClientId* aClient, 
													 TInt* aBufSize)
	{
	// Retrieve detected networks from phone, 
	// Store each network as a list entry of type TMobilePhoneNetworkInfoV2,
	// Stream the list and then return size of this buffer to client
	CMobilePhoneNetworkListV5* list=CMobilePhoneNetworkListV5::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TMobilePhoneNetworkInfoV5 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_NETWORK_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE1;
			entry.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS1;
			entry.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND1;
			entry.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC1;
			entry.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID1;
			entry.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG1;
			entry.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1;
			entry.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1;
			entry.iAccess=DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH1;
			entry.iHsdpaAvailableIndicator=DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY1;
			entry.iEgprsAvailableIndicator=DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY1;
			break;
		case 1:
			entry.iMode=DMMTSY_PHONE_CURRENT_NETWORK_MODE2;
			entry.iStatus=DMMTSY_PHONE_CURRENT_NETWORK_STATUS2;
			entry.iBandInfo=DMMTSY_PHONE_CURRENT_NETWORK_BAND2;
			entry.iCountryCode=DMMTSY_PHONE_CURRENT_NETWORK_MCC2;
			entry.iNetworkId=DMMTSY_PHONE_CURRENT_NETWORK_ID2;
			entry.iDisplayTag=DMMTSY_PHONE_CURRENT_NETWORK_TAG2;
			entry.iShortName=DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2;
			entry.iLongName=DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2;
			entry.iAccess=DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH2;
			entry.iHsdpaAvailableIndicator=DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY2;
			entry.iEgprsAvailableIndicator=DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY2;
			break;
		case 2:
			entry.iMode=DMMTSY_PHONE_HOME_NETWORK_MODE;
			entry.iStatus=DMMTSY_PHONE_HOME_NETWORK_STATUS;
			entry.iBandInfo=DMMTSY_PHONE_HOME_NETWORK_BAND;
			entry.iCountryCode=DMMTSY_PHONE_HOME_NETWORK_MCC;
			entry.iNetworkId=DMMTSY_PHONE_HOME_NETWORK_ID;
			entry.iDisplayTag=DMMTSY_PHONE_HOME_NETWORK_TAG;
			entry.iShortName=DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME;
			entry.iLongName=DMMTSY_PHONE_HOME_NETWORK_LONG_NAME;
			entry.iAccess=DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH;
			entry.iHsdpaAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY;
			entry.iEgprsAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY;
			break;
		case 3:
		default:
			entry.iMode=DMMTSY_PHONE_HOME_NETWORK_MODE;
			entry.iStatus=DMMTSY_PHONE_HOME_NETWORK_STATUS;
			entry.iBandInfo=DMMTSY_PHONE_HOME_NETWORK_BAND;
			entry.iCountryCode=DMMTSY_PHONE_HOME_NETWORK_MCC;
			entry.iNetworkId=DMMTSY_PHONE_HOME_NETWORK_ID;
			entry.iDisplayTag=DMMTSY_PHONE_HOME_NETWORK_TAG;
			entry.iShortName=DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME;
			entry.iLongName=DMMTSY_PHONE_HOME_NETWORK_LONG_NAME;
			entry.iAccess=DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH3;
			entry.iHsdpaAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY;
			entry.iEgprsAvailableIndicator=DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY;
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
	
	iGetDetectedNetworks->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetDetectedNetworksV5Phase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksV5Phase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the get detected network attempt from this client
	for (TInt i=0; i<iGetDetectedNetworks->Count(); ++i)
		{
		read = iGetDetectedNetworks->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetDetectedNetworks->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CPhoneDMmTsy::GetDetectedNetworksV5Cancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDetectedNetworksV5Cancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	// Remove the read all attempt from iGetDetectedNetworks
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetDetectedNetworks->Count(); ++i)
		{
		read = iGetDetectedNetworks->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetDetectedNetworks->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

//

TInt CPhoneDMmTsy::GetNetworkSelectionSetting(const TTsyReqHandle aTsyReqHandle, TDes8* aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetNetworkSelectionSetting called"));
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg* selectionPckg = (RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg*)aSetting;
	RMobilePhone::TMobilePhoneNetworkSelectionV1& selection = (*selectionPckg)();

	selection.iMethod = DMMTSY_PHONE_NETWORK_SELECTION_METHOD1;
	selection.iBandClass = DMMTSY_PHONE_NETWORK_SELECTION_BAND1;
	selection.iOperationMode = DMMTSY_PHONE_NETWORK_SELECTION_OPERATION1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetNetworkSelectionSetting(const TTsyReqHandle aTsyReqHandle, TDes8* aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetNetworkSelectionSetting called"));
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg* selectionPckg = (RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg*)aSetting;
	RMobilePhone::TMobilePhoneNetworkSelectionV1& selection = (*selectionPckg)();

	if ((selection.iMethod != DMMTSY_PHONE_NETWORK_SELECTION_METHOD2) ||
	    (selection.iBandClass != DMMTSY_PHONE_NETWORK_SELECTION_BAND2) ||
	    (selection.iOperationMode != DMMTSY_PHONE_NETWORK_SELECTION_OPERATION2))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetNetworkSelectionSettingCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetNetworkSelectionSettingCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyNetworkSelectionSettingChange(const TTsyReqHandle aTsyReqHandle, TDes8* aSetting)
	{
	if(!iNotifyNetworkSelectionSettingChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkSelectionSettingChange called"));

		RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg* selectionPckg = (RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg*)aSetting;
		RMobilePhone::TMobilePhoneNetworkSelectionV1& selection = (*selectionPckg)();
		
		selection.iMethod = DMMTSY_PHONE_NETWORK_SELECTION_METHOD1;
		selection.iBandClass = DMMTSY_PHONE_NETWORK_SELECTION_BAND1;
		selection.iOperationMode = DMMTSY_PHONE_NETWORK_SELECTION_OPERATION1;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyNetworkSelectionSettingChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkSelectionSettingChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SelectNetwork(const TTsyReqHandle aTsyReqHandle, 
								 TBool* aIsManual, 
								 RMobilePhone::TMobilePhoneNetworkManualSelection* aManualSelection)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SelectNetwork called"));
	
	if (*aIsManual==DMMTSY_PHONE_MANUAL_SELECTION)
		{
		if ((aManualSelection->iCountry != DMMTSY_PHONE_COUNTRY_ID) ||
		    (aManualSelection->iNetwork != DMMTSY_PHONE_NETWORK_ID))

			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			AddDelayedReq(aTsyReqHandle,this);
		}
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	
	return KErrNone;
	}

TInt CPhoneDMmTsy::SelectNetworkCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SelectNetworkCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetNetworkTimeInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNITZ* aDateTime)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetNetworkTimeInfo called"));

	aDateTime->iNitzFieldsUsed = DMMTSY_PHONE_NWTINFO_NITZ_USED;
	aDateTime->Set(DMMTSY_PHONE_NWTINFO_YEAR,DMMTSY_PHONE_NWTINFO_MONTH,DMMTSY_PHONE_NWTINFO_DAY,
		DMMTSY_PHONE_NWTINFO_HOUR, DMMTSY_PHONE_NWTINFO_MINUTE, DMMTSY_PHONE_NWTINFO_SECOND,
		DMMTSY_PHONE_NWTINFO_MICRO);
	aDateTime->iTimeZone = DMMTSY_PHONE_NWTINFO_TIMEZONE;
	aDateTime->iLongNetworkId = DMMTSY_PHONE_NWTINFO_LONG_ID;
	aDateTime->iShortNetworkId = DMMTSY_PHONE_NWTINFO_SHORT_ID;
	aDateTime->iDST = DMMTSY_PHONE_NWTINFO_DST;

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyNetworkTimeInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNITZ* aDateTime)
	{
	if(!iNotifyNetworkTimeInfoChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkTimeInfoChange called"));

		aDateTime->iNitzFieldsUsed = DMMTSY_PHONE_NWTINFO_NITZ_USED;
		aDateTime->Set(DMMTSY_PHONE_NWTINFO_YEAR,DMMTSY_PHONE_NWTINFO_MONTH,DMMTSY_PHONE_NWTINFO_DAY,
			DMMTSY_PHONE_NWTINFO_HOUR, DMMTSY_PHONE_NWTINFO_MINUTE, DMMTSY_PHONE_NWTINFO_SECOND,
			DMMTSY_PHONE_NWTINFO_MICRO);
		aDateTime->iTimeZone = DMMTSY_PHONE_NWTINFO_TIMEZONE;
		aDateTime->iLongNetworkId = DMMTSY_PHONE_NWTINFO_LONG_ID;
		aDateTime->iShortNetworkId = DMMTSY_PHONE_NWTINFO_SHORT_ID;
		aDateTime->iDST = DMMTSY_PHONE_NWTINFO_DST;

		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyNetworkTimeInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkTimeInfoChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


TInt CPhoneDMmTsy::GetServiceProviderName(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneServiceProviderNameV2* aName)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetServiceProviderName called"));

	aName->iDisplayReq = DMMTSY_PHONE_DISPREQ;
	aName->iSPName = DMMTSY_PHONE_SPN_NAME;
	aName->iPLMNField = DMMTSY_PHONE_PLMN_FIELD;

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetServiceProviderNameCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetServiceProviderNameCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetDefaultPrivacy(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePrivacy* aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetDefaultPrivacy called"));

	*aSetting = DMMTSY_PHONE_PRIVACY_SETTING1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetDefaultPrivacy(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePrivacy* aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetDefaultPrivacy called"));

	if (*aSetting != DMMTSY_PHONE_PRIVACY_SETTING1)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetDefaultPrivacyCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetDefaultPrivacyCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyDefaultPrivacyChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePrivacy* aSetting)
	{
	if(!iNotifyDefaultPrivacyChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyDefaultPrivacyChange called"));

		*aSetting = DMMTSY_PHONE_PRIVACY_SETTING2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyDefaultPrivacyChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyDefaultPrivacyChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCallServiceCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallServiceCaps called"));
	*aCaps=DMMTSY_PHONE_CALL_SERVICE_CAPS1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCallServiceCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	if (!iNotifyCallServiceCaps++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCallServiceCapsChange called"));
		*aCaps=DMMTSY_PHONE_CALL_SERVICE_CAPS2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCallServiceCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCallServiceCapsChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::ProgramFeatureCodeString(const TTsyReqHandle aTsyReqHandle, 
											TDesC* aFCString, RMobilePhone::TNetworkServiceAndAction *aFeatureCode)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ProgramFeatureCodeString called"));
	if ((aFCString->Compare(DMMTSY_PHONE_FEATURE_CODE) != 0) ||
	    (aFeatureCode->iService != DMMTSY_PHONE_NETWORK_SERVICE) || 
		(aFeatureCode->iAction != DMMTSY_PHONE_NETWORK_SERVICE_ACTION))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CPhoneDMmTsy::ProgramFeatureCodeStringCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ProgramFeatureCodeStringCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetFeatureCodeString(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TNetworkServiceAndAction *aFeatureCode, TDes *aFCString)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetFeatureCodeString called"));
	if	(aFeatureCode->iService!=DMMTSY_PHONE_NETWORK_SERVICE2 || aFeatureCode->iAction!=DMMTSY_PHONE_NETWORK_SERVICE_ACTION2)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		*aFCString=DMMTSY_PHONE_FEATURE_CODE2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetFeatureCodeStringCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetFeatureCodeStringCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SendNetworkServiceRequest(const TTsyReqHandle aTsyReqHandle, TDesC* aString)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SendNetworkServiceRequest called"));
	if (aString->Compare(DMMTSY_PHONE_NETWORK_SERVICE_REQUEST)!=0)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SendNetworkServiceRequestCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SendNetworkServiceRequestCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCallForwardingStatusPhase1(const TTsyReqHandle aTsyReqHandle, 
												 CRetrieveMobilePhoneCFList::TGetCallForwardingRequest* aReqData, 
												 TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallForwardingStatusPhase1 called"));
	TInt ret=KErrNone;

	if ( ( (aReqData->iCondition==DMMTSY_PHONE_CF_CONDITION_CFU) &&
		   (aReqData->iLocation==DMMTSY_PHONE_CF_LOCATION) &&
		   (aReqData->iServiceGroup==DMMTSY_PHONE_CF_SERVICE_GROUP_ALL) ) ||
		 ( (aReqData->iCondition==DMMTSY_PHONE_CF_CONDITION_CFU) &&
		   (aReqData->iServiceGroup==DMMTSY_PHONE_CF_SERVICE_GROUP_FAX) &&
    	   (aReqData->iLocation==DMMTSY_PHONE_CF_CACHELOCATION) ) )		
		{
		TInt leaveCode=KErrNone;
		TRAP(leaveCode, ret=ProcessGetCallForwardingStatusPhase1L(aTsyReqHandle, aReqData, aBufSize););
		if (leaveCode != KErrNone)
			ReqCompleted(aTsyReqHandle,leaveCode);
		}
		else
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return ret;
	}

TInt CPhoneDMmTsy::ProcessGetCallForwardingStatusPhase1L(const TTsyReqHandle aTsyReqHandle, 
														 CRetrieveMobilePhoneCFList::TGetCallForwardingRequest* aReqData, 
														 TInt* aBufSize)
	{
	// retrieve call forwarding status of each line from phone, 
	// store each CF status response as a list entry,
	// stream the list and then return size of this buffer to client

	CMobilePhoneCFList* list=CMobilePhoneCFList::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;
	RMobilePhone::TMobileService servicegroup = aReqData->iServiceGroup;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_CF_ENTRY_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iCondition=DMMTSY_PHONE_CF_CONDITION_CFU;
			entry.iServiceGroup=DMMTSY_PHONE_CF_SERVICE_GROUP_VOICE;
			entry.iStatus=DMMTSY_PHONE_CF_NOT_ACTIVE;
			entry.iNumber.iTelNumber=DMMTSY_PHONE_TEL_NUMBER;
			entry.iTimeout=DMMTSY_PHONE_CF_TIMEOUT;
			break;
		case 1:
			entry.iCondition=DMMTSY_PHONE_CF_CONDITION_CFU;
			entry.iServiceGroup=DMMTSY_PHONE_CF_SERVICE_GROUP_DATA;
			entry.iStatus=DMMTSY_PHONE_CF_ACTIVE;
			entry.iNumber.iTelNumber=DMMTSY_PHONE_TEL_NUMBER;
			entry.iTimeout=DMMTSY_PHONE_CF_TIMEOUT;
			break;
		case 2:
			entry.iCondition=DMMTSY_PHONE_CF_CONDITION_CFU;
			entry.iServiceGroup=DMMTSY_PHONE_CF_SERVICE_GROUP_FAX;
			entry.iStatus=DMMTSY_PHONE_CF_ACTIVE;
			entry.iNumber.iTelNumber=DMMTSY_PHONE_TEL_NUMBER;
			entry.iTimeout=DMMTSY_PHONE_CF_TIMEOUT;
			break;
		case 3:
		default:
			entry.iCondition=DMMTSY_PHONE_CF_CONDITION_CFU;
			entry.iServiceGroup=DMMTSY_PHONE_CF_SERVICE_GROUP_AUXVOICE;
			entry.iStatus=DMMTSY_PHONE_CF_UNKNOWN;
			entry.iTimeout=-1;
			break;
			}
			// Add the entry into the list, at the next empty location
		if( (servicegroup == DMMTSY_PHONE_CF_SERVICE_GROUP_ALL) || (servicegroup == entry.iServiceGroup) )
		list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(&(aReqData->iClient),aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC

	iGetCFStatus->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}


TInt CPhoneDMmTsy::GetCallForwardingStatusPhase2(const TTsyReqHandle aTsyReqHandle, 
												 RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallForwardingStatusPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the get detected network attempt from this client
	for (TInt i=0; i<iGetCFStatus->Count(); ++i)
		{
		read = iGetCFStatus->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetCFStatus->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CPhoneDMmTsy::GetCallForwardingStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallForwardingStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	// Remove the read all attempt from iGetCFStatus
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetCFStatus->Count(); ++i)
		{
		read = iGetCFStatus->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetCFStatus->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCallForwardingStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCFCondition* aCondition)
	{
	if(!iNotifyCallForwardingStatusChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCallForwardingStatusChange called"));
		*aCondition=DMMTSY_PHONE_CF_CONDITION_ALL;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCallForwardingStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCallForwardingStatusChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetCallForwardingStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCFCondition* aCondition, RMobilePhone::TMobilePhoneCFChangeV1* aInfo)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetCallForwardingStatus called"));
	if ((*aCondition!=DMMTSY_PHONE_CF_CONDITION_CFNRy) ||
		(aInfo->iServiceGroup!=DMMTSY_PHONE_CF_SERVICE_GROUP_VOICE) ||
		(aInfo->iAction!=DMMTSY_PHONE_CF_SETTING) ||
		(aInfo->iNumber.iTelNumber!=DMMTSY_PHONE_TEL_NUMBER) ||
		(aInfo->iTimeout!=DMMTSY_PHONE_CF_TIMEOUT))

		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CPhoneDMmTsy::SetCallForwardingStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetCallForwardingStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCallForwardingActive(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobileService* aServiceGroup, RMobilePhone::TMobilePhoneCFActive* aActive)
	{
	if(!iNotifyCallForwardingActive++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCallForwardingActive called"));
		*aActive=DMMTSY_PHONE_CF_ACTIVE_TYPE;
		*aServiceGroup=DMMTSY_PHONE_CF_SERVICE_GROUP_FAX;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCallForwardingActiveCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCallForwardingActiveCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetIdentityServiceStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TIdServiceAndLocation *aInfo,
											RMobilePhone::TMobilePhoneIdServiceStatus* aStatus)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIdentityServiceStatus called"));
	if (aInfo->iLocation != DMMTSY_PHONE_ID_LOCATION || aInfo->iService != DMMTSY_PHONE_ID_SERVICE)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		*aStatus = DMMTSY_PHONE_ID_STATUS;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetIdentityServiceStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIdentityServiceStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCallBarringPhase1(const TTsyReqHandle aTsyReqHandle, 
												 CRetrieveMobilePhoneCBList::TGetCallBarringRequest* aReqData, 
												 TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallBarringPhase1 called"));
	TInt ret=KErrNone;

	if ((aReqData->iCondition!=DMMTSY_PHONE_CB_CONDITION) ||
		(aReqData->iLocation!=DMMTSY_PHONE_CB_LOCATION))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		TInt leaveCode=KErrNone;
		TRAP(leaveCode, ret=ProcessGetCallBarringPhase1L(aTsyReqHandle, aReqData, aBufSize));
		if (leaveCode != KErrNone)
			ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	}

TInt CPhoneDMmTsy::ProcessGetCallBarringPhase1L(const TTsyReqHandle aTsyReqHandle, 
														 CRetrieveMobilePhoneCBList::TGetCallBarringRequest* aReqData,
														 TInt* aBufSize)
	{
	// retrieve call forwarding status of each line from phone, 
	// store each CB status response as a list entry,
	// stream the list and then return size of this buffer to client

	CMobilePhoneCBList* list=CMobilePhoneCBList::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TMobilePhoneCBInfoEntryV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_CALL_BARRING_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iCondition = DMMTSY_PHONE_CB_CONDITION0;
			entry.iServiceGroup = DMMTSY_PHONE_CB_SG0;
			entry.iStatus = DMMTSY_PHONE_CB_STATUS0;
			break;
		case 1:
			entry.iCondition = DMMTSY_PHONE_CB_CONDITION1;
			entry.iServiceGroup = DMMTSY_PHONE_CB_SG1;
			entry.iStatus = DMMTSY_PHONE_CB_STATUS1;
			break;
		case 2:
			entry.iCondition = DMMTSY_PHONE_CB_CONDITION2;
			entry.iServiceGroup = DMMTSY_PHONE_CB_SG2;
			entry.iStatus = DMMTSY_PHONE_CB_STATUS2;
			break;
		case 3:
		default:
			entry.iCondition = DMMTSY_PHONE_CB_CONDITION3;
			entry.iServiceGroup = DMMTSY_PHONE_CB_SG3;
			entry.iStatus = DMMTSY_PHONE_CB_STATUS3;
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(&(aReqData->iClient),aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC
	
	iGetCB->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}


TInt CPhoneDMmTsy::GetCallBarringPhase2(const TTsyReqHandle aTsyReqHandle, 
												 RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallBarringPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the get call barring attempt from this client
	for (TInt i=0; i<iGetCB->Count(); ++i)
		{
		read = iGetCB->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetCB->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}
	
TInt CPhoneDMmTsy::GetCallBarringCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallBarringCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	// Remove the read all attempt from iGetCB
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetCB->Count(); ++i)
		{
		read = iGetCB->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetCB->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetCallBarringStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCBCondition *aCondition, RMobilePhone::TMobilePhoneCBChangeV1* aInfo)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetCallBarringStatus called"));
	if (*aCondition != DMMTSY_PHONE_CB_CONDITION ||  aInfo->iAction != DMMTSY_PHONE_CB_ACTION ||
		(aInfo->iPassword.Compare(DMMTSY_PHONE_CB_PASSWORD) != 0))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetCallBarringStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetCallBarringStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCallBarringStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCBCondition* aCondition)
	{
	if (!iNotifyCallBarringStatusChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCallBarringStatusChange called"));
		*aCondition = DMMTSY_PHONE_CB_CONDITION;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCallBarringStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCallBarringStatusChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::SetCallBarringPassword(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePasswordChangeV1* aPassword)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetCallBarringPassword called"));
	if ((aPassword->iOldPassword.Compare(DMMTSY_PHONE_CB_PASSWORD) != 0) ||
		(aPassword->iNewPassword.Compare(DMMTSY_PHONE_CB_NEW_PASSWORD) != 0))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetCallBarringPasswordCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetCallBarringPasswordCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetSSPassword(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePasswordChangeV2* aPassword, TInt* aService)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetSSPassword called"));
	if ((aPassword->iOldPassword.Compare(DMMTSY_PHONE_CB_PASSWORD) != 0) ||
		(aPassword->iNewPassword.Compare(DMMTSY_PHONE_CB_NEW_PASSWORD) != 0) ||
		(aPassword->iVerifiedPassword.Compare(DMMTSY_PHONE_CB_VERIFIED_PASSWORD) != 0) &&
		aService != DMMTSY_PHONE_CB_ALL_SERVICES )
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetSSPasswordCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetSSPasswordCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}



TInt CPhoneDMmTsy::GetCallWaitingPhase1(const TTsyReqHandle aTsyReqHandle, 
												 CRetrieveMobilePhoneCWList::TGetCallWaitingRequest* aReqData, 
												 TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallWaitingPhase1 called"));
	TInt ret=KErrNone;

	if (aReqData->iLocation!=DMMTSY_PHONE_CW_LOCATION)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		TInt leaveCode=KErrNone;
		TRAP(leaveCode, ret=ProcessGetCallWaitingPhase1L(aTsyReqHandle, aReqData, aBufSize));
		if (leaveCode != KErrNone)
			ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	}

TInt CPhoneDMmTsy::ProcessGetCallWaitingPhase1L(const TTsyReqHandle aTsyReqHandle, 
														 CRetrieveMobilePhoneCWList::TGetCallWaitingRequest* aReqData, 
														 TInt* aBufSize)
	{
	// retrieve call forwarding status of each line from phone,
	// store each CW status response as a list entry,
	// stream the list and then return size of this buffer to client

	CMobilePhoneCWList* list=CMobilePhoneCWList::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TMobilePhoneCWInfoEntryV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_CW_ENTRY_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iServiceGroup = DMMTSY_PHONE_CW_SG0;
			entry.iStatus = DMMTSY_PHONE_CW_STATUS0;
			break;
		case 1:
			entry.iServiceGroup = DMMTSY_PHONE_CW_SG1;
			entry.iStatus = DMMTSY_PHONE_CW_STATUS1;
			break;
		case 2:
			entry.iServiceGroup = DMMTSY_PHONE_CW_SG2;
			entry.iStatus = DMMTSY_PHONE_CW_STATUS2;
			break;
		case 3:
		default:
			entry.iServiceGroup = DMMTSY_PHONE_CW_SG3;
			entry.iStatus = DMMTSY_PHONE_CW_STATUS3;
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(&(aReqData->iClient),aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC
	
	iGetCWStatus->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}


TInt CPhoneDMmTsy::GetCallWaitingPhase2(const TTsyReqHandle aTsyReqHandle, 
												 RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallWaitingPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the get call waiting attempt from this client
	for (TInt i=0; i<iGetCWStatus->Count(); ++i)
		{
		read = iGetCWStatus->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetCWStatus->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CPhoneDMmTsy::GetCallWaitingCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallWaitingCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	// Remove the read all attempt from iGetCWStatus
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetCWStatus->Count(); ++i)
		{
		read = iGetCWStatus->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetCWStatus->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


TInt CPhoneDMmTsy::SetCallWaitingStatus(const TTsyReqHandle aTsyReqHandle,
										RMobilePhone::TMobileService* aServiceGroup,
										RMobilePhone::TMobilePhoneServiceAction *aAction)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetCallWaitingStatus called"));
	if ((*aAction != DMMTSY_PHONE_CW_SET_STATUS_ACTION) ||
		(*aServiceGroup != DMMTSY_PHONE_CW_SG0))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetCallWaitingStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetCallWaitingStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCallWaitingStatusChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCWStatus)
	{
	if (!iNotifyCallWaitingStatusChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCallWaitingStatusChange called"));
		RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg *cwInfoPckg = STATIC_CAST(RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg*,aCWStatus);
		RMobilePhone::TMobilePhoneCWInfoEntryV1 &cwInfo = (*cwInfoPckg)();
		cwInfo.iServiceGroup = DMMTSY_PHONE_CW_SG0;
		cwInfo.iStatus = DMMTSY_PHONE_CW_STATUS0;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCallWaitingStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCallWaitingStatusChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCCBSStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCCBSStatus* aCcbsStatus, RMobilePhone::TMobileInfoLocation* aLocation)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCCBSStatus called"));
	if (*aLocation != DMMTSY_PHONE_CCC_LOCATION)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		*aCcbsStatus = DMMTSY_PHONE_CCC_STATUS;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCCBSStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCCBSStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCCBSStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCCBSStatus* aCcbsStatus)
	{
	if (!iNotifyCCBSStatusChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCCBSStatusChange called"));
		*aCcbsStatus = DMMTSY_PHONE_CCC_STATUS;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCCBSStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCCBSStatusChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCCBSRecall(const TTsyReqHandle aTsyReqHandle, TDes8* aCCBSEntry)
	{
//	_asm{int 3};
	if (!iNotifyCCBSRecall++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCCBSRecall called"));
		
		RMobilePhone::TMobilePhoneCCBSEntryV1Pckg* ccbsPckg = STATIC_CAST(RMobilePhone::TMobilePhoneCCBSEntryV1Pckg*,aCCBSEntry);
		RMobilePhone::TMobilePhoneCCBSEntryV1& ccbs = (*ccbsPckg)();

		ccbs.iCcbsIndex=DMMTSY_PHONE_CCC_INDEX1;
		ccbs.iServiceGroup=DMMTSY_PHONE_CCC_SERVICE_GROUP0;
		ccbs.iDestination.iNumberPlan=DMMTSY_PHONE_CCC_DESTINATION_NP0;
		ccbs.iDestination.iTelNumber= DMMTSY_PHONE_CCC_DESTINATION_NUM0;
		ccbs.iDestination.iTypeOfNumber = DMMTSY_PHONE_CCC_DESTINATION_TON0;
		
		if(ccbs.ExtensionId()==KETelExtMultimodeV2)
			{
			RMobilePhone::TMobilePhoneCCBSEntryV2Pckg* ccbsPckgV2 = STATIC_CAST(RMobilePhone::TMobilePhoneCCBSEntryV2Pckg*,aCCBSEntry);
			RMobilePhone::TMobilePhoneCCBSEntryV2& ccbs2 = (*ccbsPckgV2)();
			
			// Set that the network has informed us that the CCBS request can proceed
			ccbs2.iEvent = DMMTSY_PHONE_CCC_DESTINATION_CCBS_RECEIVED;
			//reset for multiple complete V1
			iNotifyCCBSRecall = 0;
			}

		AddDelayedReq(aTsyReqHandle,this);
		}
	else
		{
			// reset for the V2 test 
			iNotifyCCBSRecall=0;
		}
	return KErrNone;	
	}

TInt CPhoneDMmTsy::NotifyCCBSRecallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCCBSRecallCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::AcceptCCBSRecall(const TTsyReqHandle aTsyReqHandle, TInt* aIndex, TDes* aCallName)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::AcceptCCBSRecall called"));
	if(*aIndex!=DMMTSY_PHONE_CCC_INDEX1)
		return KErrCorrupt;
	*aCallName = DMMTSY_PHONE_CCC_CALLNAME0;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::AcceptCCBSRecallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::AcceptCCBSRecallCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::RefuseCCBSRecall(const TTsyReqHandle aTsyReqHandle, TInt* aIndex)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::RefuseCCBSRecall called"));
	if(*aIndex!=DMMTSY_PHONE_CCC_INDEX1)
		return KErrCorrupt;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}


TInt CPhoneDMmTsy::DeactivateCCBS(const TTsyReqHandle aTsyReqHandle, TInt* aIndex)
	{
	if (*aIndex== -1)		//-1 means deactivate all
		return KErrEtelCallNotActive;
	if(*aIndex!=DMMTSY_PHONE_CCC_INDEX1) 
		return KErrCorrupt;
		
	LOGTEXT(_L8("CPhoneDMmTsy::DeactivateAllCCBS called"));
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::DeactivateCCBSCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::DeactivateAllCCBSCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCallCompletionPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aReqData, TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallCompletionPhase1 called"));
	TInt ret = KErrNone;

	TRAPD(leaveCode, ret=ProcessGetCallCompletionPhase1L(aTsyReqHandle, aReqData, aBufSize));
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;	
	}

TInt CPhoneDMmTsy::ProcessGetCallCompletionPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClientId, TInt* aBufSize)
	{
	CMobilePhoneCcbsList* list=CMobilePhoneCcbsList::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TMobilePhoneCCBSEntryV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_PHONE_CCC_ENTRY_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iServiceGroup = DMMTSY_PHONE_CCC_SERVICE_GROUP0;
			entry.iCcbsIndex = DMMTSY_PHONE_CCC_INDEX0;
			entry.iDestination.iTypeOfNumber = DMMTSY_PHONE_CCC_DESTINATION_TON0;
			entry.iDestination.iNumberPlan = DMMTSY_PHONE_CCC_DESTINATION_NP0;
			entry.iDestination.iTelNumber = DMMTSY_PHONE_CCC_DESTINATION_NUM0;
			break;
		case 1:
			entry.iServiceGroup = DMMTSY_PHONE_CCC_SERVICE_GROUP1;
			entry.iCcbsIndex = DMMTSY_PHONE_CCC_INDEX1;
			entry.iDestination.iTypeOfNumber = DMMTSY_PHONE_CCC_DESTINATION_TON1;
			entry.iDestination.iNumberPlan = DMMTSY_PHONE_CCC_DESTINATION_NP1;
			entry.iDestination.iTelNumber = DMMTSY_PHONE_CCC_DESTINATION_NUM1;
			break;
		case 2:
			entry.iServiceGroup = DMMTSY_PHONE_CCC_SERVICE_GROUP2;
			entry.iCcbsIndex = DMMTSY_PHONE_CCC_INDEX2;
			entry.iDestination.iTypeOfNumber = DMMTSY_PHONE_CCC_DESTINATION_TON2;
			entry.iDestination.iNumberPlan = DMMTSY_PHONE_CCC_DESTINATION_NP2;
			entry.iDestination.iTelNumber = DMMTSY_PHONE_CCC_DESTINATION_NUM2;
			break;
		case 3:
			entry.iServiceGroup = DMMTSY_PHONE_CCC_SERVICE_GROUP3;
			entry.iCcbsIndex = DMMTSY_PHONE_CCC_INDEX3;
			entry.iDestination.iTypeOfNumber = DMMTSY_PHONE_CCC_DESTINATION_TON3;
			entry.iDestination.iNumberPlan = DMMTSY_PHONE_CCC_DESTINATION_NP3;
			entry.iDestination.iTelNumber = DMMTSY_PHONE_CCC_DESTINATION_NUM3;
		default:
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aClientId,aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC
	
	iGetCcbsRequests->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCallCompletionPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallCompletionPhase2 called"));
	CListReadAllAttempt* read=NULL;
	
	for (TInt i=0; i<iGetCcbsRequests->Count(); ++i)
		{
		read = iGetCcbsRequests->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetCcbsRequests->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}

TInt CPhoneDMmTsy::GetCallCompletionCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallWaitingCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	// Remove the read all attempt from iGetCcbsRequests
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetCcbsRequests->Count(); ++i)
		{
		read = iGetCcbsRequests->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetCcbsRequests->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetAlternatingCallCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAlternatingCallCaps called"));

	*aCaps = DMMTSY_PHONE_ALTERNATING_CALL_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyAlternatingCallCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	if (!iNotifyAlternatingCallCapsChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyAlternatingCallCapsChange called"));
		*aCaps = DMMTSY_PHONE_ALTERNATING_CALL_CAPS;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyAlternatingCallCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyAlternatingCallCapsChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetAlternatingCallMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneAlternatingCallMode* aMode, RMobilePhone::TMobileService* aFirstService)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAlternatingCallMode called"));
	*aMode = DMMTSY_PHONE_ALTERNATING_CALL_MODE;
	*aFirstService = DMMTSY_PHONE_ALTERNATING_CALL_SERVICE;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetAlternatingCallMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneAlternatingCallMode* aMode, RMobilePhone::TMobileService* aFirstService)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetAlternatingCallMode called"));
	if (*aMode != DMMTSY_PHONE_ALTERNATING_CALL_MODE || *aFirstService != DMMTSY_PHONE_ALTERNATING_CALL_SERVICE)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetAlternatingCallModeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetAlternatingCallModeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyAlternatingCallModeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneAlternatingCallMode* aMode,RMobilePhone::TMobileService* aFirstService)
	{
	if (!iNotifyAlternatingCallModeChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyAlternatingCallModeChange called"));
		*aMode = DMMTSY_PHONE_ALTERNATING_CALL_MODE;
		*aFirstService = DMMTSY_PHONE_ALTERNATING_CALL_SERVICE;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyAlternatingCallModeChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyAlternatingCallModeChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetALSLine(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneALSLine* aALSLine)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetALSLine called"));
	*aALSLine = DMMTSY_PHONE_ALS_LINE;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetALSLine(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneALSLine* aALSLine)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetALSLine called"));
	if (*aALSLine != DMMTSY_PHONE_ALS_LINE)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetALSLineCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetALSLineCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


TInt CPhoneDMmTsy::NotifyALSLineChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneALSLine* aALSLine)
	{
	if (!iNotifyALSLineChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyALSLineChange called"));
		*aALSLine = DMMTSY_PHONE_ALS_LINE;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyALSLineChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyALSLineChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCostCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCostCaps called"));
	*aCaps = DMMTSY_PHONE_COST_CAPS;	
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCostCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	if (!iNotifyCostCapsChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCostCapsChange called"));
		*aCaps = DMMTSY_PHONE_COST_CAPS;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCostCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCostCapsChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::ClearCostMeter(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneCostMeters* aMeters)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ClearCostMeter called"));
	if (*aMeters != DMMTSY_PHONE_COST_METER)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::ClearCostMeterCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ClearCostMeterCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetMaxCostMeter(const TTsyReqHandle aTsyReqHandle, TUint *aUnits)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetMaxCostMeter called"));
	if (*aUnits != DMMTSY_PHONE_COST_UNITS)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetMaxCostMeterCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetMaxCostMeterCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetPuct(const TTsyReqHandle aTsyReqHandle, TDes8* aPuct)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetPuct called"));
	RMobilePhone::TMobilePhonePuctV1Pckg* puctPckg = STATIC_CAST(RMobilePhone::TMobilePhonePuctV1Pckg*,aPuct);
	RMobilePhone::TMobilePhonePuctV1& puct = (*puctPckg)();
	
	if ((puct.iCurrencyName.Compare(DMMTSY_PHONE_COST_PUCT_CNAME) != 0) || 
		(puct.iPricePerUnit != DMMTSY_PHONE_COST_PUCT_PPU))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CPhoneDMmTsy::SetPuctCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetPuctCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCostInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aCostInfo)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCostInfo called"));
	RMobilePhone::TMobilePhoneCostInfoV1Pckg* ciPckg = STATIC_CAST(RMobilePhone::TMobilePhoneCostInfoV1Pckg*,aCostInfo);
	RMobilePhone::TMobilePhoneCostInfoV1& ci = (*ciPckg)();

	ci.iService = DMMTSY_PHONE_COST_SERVICE;
	ci.iCCM = DMMTSY_PHONE_COST_CCM;
	ci.iACM = DMMTSY_PHONE_COST_ACM;
	ci.iACMmax = DMMTSY_PHONE_COST_ACMMAX;
	ci.iPuct.iCurrencyName = DMMTSY_PHONE_COST_PUCT_CNAME;
	ci.iPuct.iPricePerUnit = DMMTSY_PHONE_COST_PUCT_PPU;

	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetCostInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCostInfoCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::NotifyCostInfoChange(const TTsyReqHandle aTsyReqHandle, TDes8* aCostInfo)
	{
	if (!iNotifyCostInfoChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyCostInfoChange called"));
		RMobilePhone::TMobilePhoneCostInfoV1Pckg* ciPckg = STATIC_CAST(RMobilePhone::TMobilePhoneCostInfoV1Pckg*,aCostInfo);
		RMobilePhone::TMobilePhoneCostInfoV1& ci = (*ciPckg)();
	
		ci.iService = DMMTSY_PHONE_COST_SERVICE;
		ci.iCCM = DMMTSY_PHONE_COST_CCM;
		ci.iACM = DMMTSY_PHONE_COST_ACM;
		ci.iACMmax = DMMTSY_PHONE_COST_ACMMAX;
		ci.iPuct.iCurrencyName = DMMTSY_PHONE_COST_PUCT_CNAME;
		ci.iPuct.iPricePerUnit = DMMTSY_PHONE_COST_PUCT_PPU;

		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCostInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCostInfoChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetSecurityCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSecurityCaps called"));
	*aCaps = DMMTSY_PHONE_SECURITY_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifySecurityCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps)
	{
	if (!iNotifySecurityCapsChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifySecurityCapsChange called"));
		*aCaps = DMMTSY_PHONE_SECURITY_CAPS;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifySecurityCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifySecurityCapsChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetLockInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneLock *aLock, TDes8* aLockInfo)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetLockInfo called"));
	if (*aLock != DMMTSY_PHONE_SECURITY_LOCK)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		RMobilePhone::TMobilePhoneLockInfoV1Pckg* liPckg = STATIC_CAST(RMobilePhone::TMobilePhoneLockInfoV1Pckg*,aLockInfo);
		RMobilePhone::TMobilePhoneLockInfoV1& li = (*liPckg)();
		li.iStatus = DMMTSY_PHONE_SECURITY_STATUS;
		li.iSetting = DMMTSY_PHONE_SECURITY_SETTING;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetLockInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetLockInfoCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::NotifyLockInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneLock* aLock, TDes8* aLockInfo)
	{
	if (!iNotifyLockInfoChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyLockInfoChange called"));
		RMobilePhone::TMobilePhoneLockInfoV1Pckg* liPckg = STATIC_CAST(RMobilePhone::TMobilePhoneLockInfoV1Pckg*,aLockInfo);
		RMobilePhone::TMobilePhoneLockInfoV1& li = (*liPckg)();
		li.iStatus = DMMTSY_PHONE_SECURITY_STATUS;
		li.iSetting = DMMTSY_PHONE_SECURITY_SETTING;
		*aLock = DMMTSY_PHONE_SECURITY_LOCK;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyLockInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyLockInfoChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

 
TInt CPhoneDMmTsy::SetLockSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneLock *aLock, RMobilePhone::TMobilePhoneLockSetting *aLockSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetLockSetting called"));
	if (*aLock != DMMTSY_PHONE_SECURITY_SC_LOCK || 
		*aLockSetting != DMMTSY_PHONE_SECURITY_SC_SETTING)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CPhoneDMmTsy::SetLockSettingCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetLockSettingCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::ChangeSecurityCode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode *aType, RMobilePhone::TMobilePhonePasswordChangeV1 *aChange)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ChangeSecurityCode called"));
	if ((*aType != DMMTSY_PHONE_SECURITY_CODE) || 
		(aChange->iOldPassword.Compare(DMMTSY_PHONE_SECURITY_OLD_PASS) != 0) ||
		(aChange->iNewPassword.Compare(DMMTSY_PHONE_SECURITY_NEW_PASS) != 0)) 
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
		
	return KErrNone;
	}

TInt CPhoneDMmTsy::ChangeSecurityCodeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ChangeSecurityCodeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetSecurityCodeInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode *aSecurityCode, TDes8* aSecurityCodeInfo)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSecurityCodeInfo called"));
	if (*aSecurityCode != DMMTSY_PHONE_SECURITY_CODE)
		ReqCompleted(aTsyReqHandle,KErrNotFound);
	else
		{
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg* infoPckg = STATIC_CAST(RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg*,aSecurityCodeInfo);
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5& info = (*infoPckg)();
		info.iRemainingEntryAttempts = DMMTSY_PHONE_SECURITY_REMAINING_ATTEMPTS;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetSecurityCodeInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSecurityCodeInfoCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::NotifySecurityCodeInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aSecurityCode,TDes8* aSecurityCodeInfo)
	{
	if (!iNotifySecurityCodeInfoChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::SecurityCodeInfoChange called"));
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg* infoPckg = STATIC_CAST(RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg*,aSecurityCodeInfo);
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5& info = (*infoPckg)();
		info.iRemainingEntryAttempts = DMMTSY_PHONE_SECURITY_REMAINING_ATTEMPTS;
		*aSecurityCode = DMMTSY_PHONE_SECURITY_CODE;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifySecurityCodeInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifySecurityCodeInfoChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}
	
TInt CPhoneDMmTsy::NotifyOnSecurityEvent(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityEvent* aEvent)
	{
	if (!iNotifyOnSecurityEvent++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::Notify On Security Event called"));
		*aEvent = DMMTSY_PHONE_SECURITY_EVENT;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyOnSecurityEventCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyOnSecurityEventCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::VerifySecurityCode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode *aType, RMobilePhone::TCodeAndUnblockCode* aData)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::Verify Security Code called"));

	if ((*aType != DMMTSY_PHONE_SECURITY_CODE) ||
		(aData->iCode.Compare(DMMTSY_PHONE_SECURITY_OLD_PASS) != 0) ||
		(aData->iUnblockCode.Compare(DMMTSY_PHONE_SECURITY_NEW_PASS) != 0))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CPhoneDMmTsy::VerifySecurityCodeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::VerifySecurityCodeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::AbortSecurityCode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode *aType)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::AbortSecurityCode called"));
	if (*aType != DMMTSY_PHONE_SECURITY_CODE)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


TInt CPhoneDMmTsy::NotifyMessageWaiting(const TTsyReqHandle aTsyReqHandle, TInt* aCount)
	{
	if (!iNotifyMessageWaiting++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyMessageWaiting called"));
		*aCount = DMMTSY_PHONE_MW_COUNT;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyMessageWaitingCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyMessageWaitingCanel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetIccMessageWaitingIndicators(const TTsyReqHandle aTsyReqHandle, TDes8* aMsgIndicators)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIccMessageWaitingIndicators called"));
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg* indPckg = STATIC_CAST(RMobilePhone::TMobilePhoneMessageWaitingV1Pckg*,aMsgIndicators);
	RMobilePhone::TMobilePhoneMessageWaitingV1& indicators = (*indPckg)();
	indicators.iDisplayStatus=DMMTSY_PHONE_DISPLAYSTATUS; 
	indicators.iVoiceMsgs=DMMTSY_PHONE_VOICEMSGS;
	indicators.iAuxVoiceMsgs=DMMTSY_PHONE_AUXVOICEMSGS;
	indicators.iDataMsgs=DMMTSY_PHONE_DATAMSGS;
	indicators.iFaxMsgs=DMMTSY_PHONE_FAXMSGS;
	indicators.iEmailMsgs=DMMTSY_PHONE_EMAILMSGS;
	indicators.iOtherMsgs=DMMTSY_PHONE_OTHERMSGS;	
	
	if(indicators.ExtensionId() == KEtelExtMultimodeV8 )
		{
		RMobilePhone::TMobilePhoneMessageWaitingV8Pckg* indV8Pckg = STATIC_CAST(RMobilePhone::TMobilePhoneMessageWaitingV8Pckg*,aMsgIndicators);
		RMobilePhone::TMobilePhoneMessageWaitingV8& indicatorsV8 = (*indV8Pckg)();
		indicatorsV8.iVideoMsgs=DMMTSY_PHONE_VIDEOMSGS1;	
		}
	
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetIccMessageWaitingIndicatorsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIccMessageWaitingIndicators called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::SetIccMessageWaitingIndicators(const TTsyReqHandle aTsyReqHandle, TDes8* aMsgIndicators)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetIccMessageWaitingIndicators called"));
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg* indPckg = STATIC_CAST(RMobilePhone::TMobilePhoneMessageWaitingV1Pckg*,aMsgIndicators);
	RMobilePhone::TMobilePhoneMessageWaitingV1& indicators = (*indPckg)();
	if ((indicators.iDisplayStatus!=DMMTSY_PHONE_DISPLAYSTATUS) || 
			(indicators.iVoiceMsgs != DMMTSY_PHONE_VOICEMSGS2) ||
			(indicators.iAuxVoiceMsgs != DMMTSY_PHONE_AUXVOICEMSGS2) ||
			(indicators.iDataMsgs != DMMTSY_PHONE_DATAMSGS2) ||
			(indicators.iFaxMsgs != DMMTSY_PHONE_FAXMSGS2) ||
			(indicators.iEmailMsgs != DMMTSY_PHONE_EMAILMSGS2) ||
			(indicators.iOtherMsgs != DMMTSY_PHONE_OTHERMSGS2))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	
	if(indicators.ExtensionId() == KEtelExtMultimodeV8 ) 
 		{
 		RMobilePhone::TMobilePhoneMessageWaitingV8Pckg* indV8Pckg = STATIC_CAST(RMobilePhone::TMobilePhoneMessageWaitingV8Pckg*,aMsgIndicators);
		RMobilePhone::TMobilePhoneMessageWaitingV8& indicatorsV8 = (*indV8Pckg)();
		if (indicatorsV8.iVideoMsgs != DMMTSY_PHONE_VIDEOMSGS2)
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		}

	AddDelayedReq(aTsyReqHandle,this);			
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetIccMessageWaitingIndicatorsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetIccMessageWaitingIndicators called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::NotifyIccMessageWaitingIndicatorsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aMsgIndicators)
	{
	if (!iNotifyIccMessageWaitingChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyIccMessageWaitingIndicatorsChange called"));
		RMobilePhone::TMobilePhoneMessageWaitingV1Pckg* indPckg = STATIC_CAST(RMobilePhone::TMobilePhoneMessageWaitingV1Pckg*,aMsgIndicators);
		RMobilePhone::TMobilePhoneMessageWaitingV1& indicators = (*indPckg)();
		
		indicators.iDisplayStatus=DMMTSY_PHONE_DISPLAYSTATUS; 
		indicators.iVoiceMsgs=DMMTSY_PHONE_VOICEMSGS2;
		indicators.iAuxVoiceMsgs=DMMTSY_PHONE_AUXVOICEMSGS2;
		indicators.iDataMsgs=DMMTSY_PHONE_DATAMSGS2;
		indicators.iFaxMsgs=DMMTSY_PHONE_FAXMSGS2;
		indicators.iEmailMsgs=DMMTSY_PHONE_EMAILMSGS2;
		indicators.iOtherMsgs=DMMTSY_PHONE_OTHERMSGS2;
		
		if(indicators.ExtensionId() == KEtelExtMultimodeV8)
			{
			RMobilePhone::TMobilePhoneMessageWaitingV8Pckg* indV8Pckg = STATIC_CAST(RMobilePhone::TMobilePhoneMessageWaitingV8Pckg*,aMsgIndicators);
			RMobilePhone::TMobilePhoneMessageWaitingV8& indicatorsV8 = (*indV8Pckg)();
			indicatorsV8.iVideoMsgs=DMMTSY_PHONE_VIDEOMSGS2;
			iNotifyIccMessageWaitingChange = 0;	
			} 
		AddDelayedReq(aTsyReqHandle,this);
		}
	else
		{
		//reset for multiple complete test
		iNotifyIccMessageWaitingChange = 0;		
		}	
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyIccMessageWaitingIndicatorsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyIccMessageWaitingIndicatorsChange called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetFdnStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneFdnStatus* aFdnStatus)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetFdnStatus called"));
	*aFdnStatus = DMMTSY_PHONE_FDN_STATUS;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetFdnStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetFdnStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::SetFdnSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneFdnSetting *aFdnSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetFdnSetting called"));
	if (*aFdnSetting != DMMTSY_PHONE_FDN_SETTING)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetFdnSettingCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetFdnSettingCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::NotifyFdnStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneFdnStatus* aFdnStatus)
	{
	if (!iNotifyFdnStatusChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyFdnStatusChange called"));
		*aFdnStatus = DMMTSY_PHONE_FDN_STATUS;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyFdnStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyFdnStatusChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetMulticallParams(const TTsyReqHandle aTsyReqHandle, TDes8* aMulticallParams)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetMulticallParams called"));

	RMobilePhone::TMobilePhoneMulticallSettingsV1Pckg *multicallParamsPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneMulticallSettingsV1Pckg *, aMulticallParams);
	RMobilePhone::TMobilePhoneMulticallSettingsV1     &multicallParams     = (*multicallParamsPckg)();

	multicallParams.iUserMaxBearers              = DMMTSY_PHONE_USER_MAX_BEARERS1;
	multicallParams.iServiceProviderMaxBearers  = DMMTSY_PHONE_PROVIDER_MAX_BEARERS1;
	multicallParams.iNetworkSupportedMaxBearers = DMMTSY_PHONE_NETWORK_MAX_BEARERS1;
	multicallParams.iUESupportedMaxBearers      = DMMTSY_PHONE_UE_MAX_BEARERS1;

	AddDelayedReq(aTsyReqHandle, this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetMulticallParams(const TTsyReqHandle aTsyReqHandle, TInt* aUserMaxBearers)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetMulticallParams called"));

	if (*aUserMaxBearers != DMMTSY_PHONE_USER_MAX_BEARERS1)
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetMulticallParamsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetMulticallParamsCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetMulticallParamsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetMulticallParamsCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyMulticallParamsChange(const TTsyReqHandle aTsyReqHandle, TDes8* aMulticallParams)
	{
	if (!iNotifyMulticallParamsChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyMulticallParamsChange called"));

		RMobilePhone::TMobilePhoneMulticallSettingsV1Pckg *multicallParamsPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneMulticallSettingsV1Pckg *, aMulticallParams);
		RMobilePhone::TMobilePhoneMulticallSettingsV1     &multicallParams     = (*multicallParamsPckg)();

		multicallParams.iUserMaxBearers              = DMMTSY_PHONE_USER_MAX_BEARERS2;
		multicallParams.iServiceProviderMaxBearers  = DMMTSY_PHONE_PROVIDER_MAX_BEARERS2;
		multicallParams.iNetworkSupportedMaxBearers = DMMTSY_PHONE_NETWORK_MAX_BEARERS2;
		multicallParams.iUESupportedMaxBearers      = DMMTSY_PHONE_UE_MAX_BEARERS2;		

		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyMulticallParamsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyMulticallParamsChangeCancel called"));
	
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetIncomingCallType(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIncomingCallType* aCallType, TDes8* aDataParams)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIncomingCallType called"));

	RMobileCall::TMobileDataCallParamsV1Pckg *dataParamsPckgV1 = REINTERPRET_CAST(RMobileCall::TMobileDataCallParamsV1Pckg*, aDataParams);
	RMobileCall::TMobileDataCallParamsV1     &dataParamsV1     = (*dataParamsPckgV1)();

	dataParamsV1.iService = DMMTSY_DATA_CALL_SERVICE;
	dataParamsV1.iSpeed = DMMTSY_DATA_CALL_SPEED;
	dataParamsV1.iProtocol = DMMTSY_DATA_CALL_PROTOCOL;
	dataParamsV1.iQoS = DMMTSY_DATA_CALL_QOS;
	dataParamsV1.iRLPVersion = DMMTSY_DATA_CALL_RLP;
	dataParamsV1.iV42bisReq = DMMTSY_DATA_CALL_V42BIS;
 	dataParamsV1.iUseEdge = DMMTSY_DATA_CALL_EGPRS_REQUIRED;

	*aCallType = DMMTSY_PHONE_INCOMING_CALL_TYPES;

	AddDelayedReq(aTsyReqHandle, this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetIncomingCallTypeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetIncomingCallTypeCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;

	}

TInt CPhoneDMmTsy::SetIncomingCallType(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIncomingCallType* aCallType, TDes8* aDataParams)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetIncomingCallType called"));

	RMobileCall::TMobileDataCallParamsV1Pckg *dataParamsPckgV1 = REINTERPRET_CAST(RMobileCall::TMobileDataCallParamsV1Pckg*, aDataParams);
	RMobileCall::TMobileDataCallParamsV1     &dataParamsV1     = (*dataParamsPckgV1)();

	if ((*aCallType != DMMTSY_PHONE_INCOMING_CALL_TYPES) ||
		(dataParamsV1.iService != DMMTSY_DATA_CALL_SERVICE) ||
		(dataParamsV1.iSpeed != DMMTSY_DATA_CALL_SPEED) ||
		(dataParamsV1.iProtocol != DMMTSY_DATA_CALL_PROTOCOL) ||
		(dataParamsV1.iQoS != DMMTSY_DATA_CALL_QOS) || 
		(dataParamsV1.iRLPVersion != DMMTSY_DATA_CALL_RLP) ||
		(dataParamsV1.iV42bisReq != DMMTSY_DATA_CALL_V42BIS) ||
 		(dataParamsV1.iUseEdge != DMMTSY_DATA_CALL_EGPRS_REQUIRED))

		ReqCompleted(aTsyReqHandle, KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetIncomingCallTypeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetIncomingCallTypeCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;

	}

TInt CPhoneDMmTsy::NotifyIncomingCallTypeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIncomingCallType* aCallType, TDes8* aDataParams)
	{
	if (!iNotifyIncomingCallTypeChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyIncomingCallTypeChange called"));

		RMobileCall::TMobileDataCallParamsV1Pckg *dataParamsPckg = REINTERPRET_CAST(RMobileCall::TMobileDataCallParamsV1Pckg*, aDataParams);
		RMobileCall::TMobileDataCallParamsV1     &dataParams     = (*dataParamsPckg)();

		if(dataParams.ExtensionId() == RMobileCall::KETelMobileDataCallParamsV2)
			{
			RMobileCall::TMobileDataCallParamsV2Pckg *dataParamsPckgV2 = REINTERPRET_CAST(RMobileCall::TMobileDataCallParamsV2Pckg*, aDataParams);
			RMobileCall::TMobileDataCallParamsV2     &dataParamsV2     = (*dataParamsPckgV2)();
			dataParamsV2.iBearerMode = DMMTSY_PHONE_MULTICALLBEARERMODE_NOTSUPPORTED;
			//reset for V1
			iNotifyIncomingCallTypeChange = 0;
			}
		dataParams.iService = DMMTSY_DATA_CALL_SERVICE1;
		dataParams.iSpeed = DMMTSY_DATA_CALL_SPEED1;
		dataParams.iProtocol = DMMTSY_DATA_CALL_PROTOCOL1;
		dataParams.iQoS = DMMTSY_DATA_CALL_QOS1;
		dataParams.iRLPVersion = DMMTSY_DATA_CALL_RLP1;
		dataParams.iV42bisReq = DMMTSY_DATA_CALL_V42BIS1;
 		dataParams.iUseEdge = DMMTSY_DATA_CALL_EGPRS_NOTREQUIRED;

		*aCallType = DMMTSY_PHONE_INCOMING_CALL_TYPES1;
		
		AddDelayedReq(aTsyReqHandle,this);
		}
	else
		{
		//reset for multiple complete test
		iNotifyIncomingCallTypeChange = 0;
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyIncomingCallTypeChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyIncomingCallTypeChangeCancel called"));
	
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetUUSSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneUUSSetting* aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetUUSSetting called"));

	*aSetting = DMMTSY_PHONE_UUS_SETTING1;

	AddDelayedReq(aTsyReqHandle, this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetUUSSettingCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetUUSSettingCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetUUSSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneUUSSetting* aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetUUSSetting called"));

	if(*aSetting != DMMTSY_PHONE_UUS_SETTING2)
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetUUSSettingCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetUUSSettingCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyUUSSettingChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneUUSSetting* aSetting)
	{
	if (!iNotifyUUSSettingChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyUUSSettingChange called"));

		*aSetting = DMMTSY_PHONE_UUS_SETTING2;

		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyUUSSettingChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyUUSSettingChangeCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetMultimediaCallPreference(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneMultimediaSettings* aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetMultimediaCallPreference called"));

	*aSetting = DMMTSY_PHONE_MM_SETTING;

	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetMultimediaCallPreference(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneMultimediaSettings* aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetMultimediaCallPreference called"));

	if(*aSetting != DMMTSY_PHONE_MM_SETTING)
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetMultimediaCallPreferenceCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetMultimediaCallPreferenceCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyMultimediaCallPreferenceChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneMultimediaSettings* aSetting)
	{
	if (!iNotifyMmCallPrefChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyMultimediaCallPreferenceChange called"));

		*aSetting = DMMTSY_PHONE_MM_SETTING_2;

		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyMultimediaCallPreferenceChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyMultimediaCallPreferenceChangeCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetNetworkSecurityLevel(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkSecurity* aSecurity)
	{

	LOGTEXT(_L8("CPhoneDMmTsy::GetNetworkSecurityLevel called"));

	*aSecurity = DMMTSY_PHONE_NET_SECURITY;

	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetNetworkSecurityLevelCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetNetworkSecurityLevelCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyNetworkSecurityLevelChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkSecurity* aSecurity)
	{

	if (!iNotifyNetworkSecurityChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkSecurityLevelChange called"));
	
		*aSecurity = DMMTSY_PHONE_NET_SECURITY;

		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyNetworkSecurityLevelChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkSecurityLevelChangeCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::EnumerateUSimApplications(const TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::EnumerateUSimApplications called"));

	*aCount = DMMTSY_PHONE_USIM_COUNT;
	aActiveAID->Copy(DMMTSY_PHONE_USIM_AID);
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::EnumerateUSimApplicationsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::EnumerateUSimApplicationsCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetUSimApplicationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TUSimAppAction* aAction)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetUSimApplicationStatus called"));

	if ((*aAction != DMMTSY_PHONE_USIM_ACTIVATE) ||
		(aAID->Compare(DMMTSY_PHONE_USIM_AID) !=0 ))
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetUSimApplicationStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetUSimApplicationStatusCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}


TInt CPhoneDMmTsy::GetUSimApplicationInfo(const TTsyReqHandle aTsyReqHandle, TInt* aIndex, RMobilePhone::TUSimApplicationInfoV2* aInfo)
	{

	LOGTEXT(_L8("CPhoneDMmTsy::GetUSimApplicationInfo called"));

	if(*aIndex != DMMTSY_PHONE_USIM_APPINDEX)
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
	else
		{
		aInfo->iLabel.Copy(DMMTSY_PHONE_USIM_APPLABEL);
		aInfo->iAID.Copy(DMMTSY_PHONE_USIM_AID);
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetUSimApplicationInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetUSimApplicationInfoCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyUSimApplicationsInfoChange(const TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID)
	{
	if (!iNotifyUSimApplicationsInfoChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyUSimApplicationsInfoChange called"));
		
		aActiveAID->Copy(DMMTSY_PHONE_USIM_AID);
		*aCount = DMMTSY_PHONE_USIM_COUNT;

		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyUSimApplicationsInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyUSimApplicationsInfoChangeCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetUSimAppsSelectionMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aMode)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetUSimAppsSelectionMode called"));

	*aMode = DMMTSY_PHONE_USIM_SELMODE;

	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetUSimAppsSelectionMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aMode)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetUSimAppsSelectionMode called"));

	if(*aMode != DMMTSY_PHONE_USIM_SELMODE)
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyUSimAppsSelectionModeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aMode)
	{
	if (!iNotifyUSimAppsSelectionModeChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyUSimAppsSelectionModeChange called"));

		*aMode = DMMTSY_PHONE_USIM_SELMODE;

		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyUSimAppsSelectionModeChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyUSimAppsSelectionModeChangeCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetMailboxNumbers(const TTsyReqHandle aTsyReqHandle, TDes8 *aMailBox)
	{
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg *mailboxPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg*, aMailBox);
	RMobilePhone::TMobilePhoneVoicemailIdsV3 &mailbox     = (*mailboxPckg)();
		
	LOGTEXT(_L8("CPhoneDMmTsy::GetMailboxNumbers called"));
	mailbox.iVoice = DMMTSY_PHONE_MAILBOXNUMBER_VOICE;
	mailbox.iData = DMMTSY_PHONE_MAILBOXNUMBER_DATA;
	mailbox.iFax = DMMTSY_PHONE_MAILBOXNUMBER_FAX;
	mailbox.iOther = DMMTSY_PHONE_MAILBOXNUMBER_OTHER;
		
	if(mailbox.ExtensionId() == KEtelExtMultimodeV8 )
		{
		RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg *mailboxV8Pckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg*, aMailBox);
		RMobilePhone::TMobilePhoneVoicemailIdsV8 &mailboxV8     = (*mailboxV8Pckg)();
		LOGTEXT(_L8("CPhoneDMmTsy::GetMailboxNumbers V8 class is called"));
		mailboxV8.iVideo = DMMTSY_PHONE_MAILBOXNUMBER_VIDEO;
		}
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;	
	}

	
TInt CPhoneDMmTsy::GetMailboxNumbersCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetMailboxNumbersCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyMailboxNumbersChange(const TTsyReqHandle aTsyReqHandle, TDes8 *aMailBox)
	
	{
	RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg *mailboxPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg*, aMailBox);
	RMobilePhone::TMobilePhoneVoicemailIdsV3 &mailbox     = (*mailboxPckg)();
	
	if (!iNotifyMailBoxNumbersChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyMailboxNumbersChange called"));
		mailbox.iVoice = DMMTSY_PHONE_MAILBOXNUMBER_VOICE;
		mailbox.iData = DMMTSY_PHONE_MAILBOXNUMBER_DATA;
		mailbox.iFax = DMMTSY_PHONE_MAILBOXNUMBER_FAX;
		mailbox.iOther = DMMTSY_PHONE_MAILBOXNUMBER_OTHER;
		
		if(mailbox.ExtensionId() == KEtelExtMultimodeV8)
			{
			RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg *mailboxPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneVoicemailIdsV8Pckg*, aMailBox);
			RMobilePhone::TMobilePhoneVoicemailIdsV8 &mailbox     = (*mailboxPckg)();
			mailbox.iVideo = DMMTSY_PHONE_MAILBOXNUMBER_VIDEO;
			iNotifyMailBoxNumbersChange=0;
			}
		AddDelayedReq(aTsyReqHandle,this);
		}
	else
		{
		iNotifyMailBoxNumbersChange=0;		
		}

	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyMailboxNumbersChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyMailboxNumbersChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyMmsUpdate(const TTsyReqHandle aTsyReqHandle, TDes8 *aNotification)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyMmsUpdate called"));

	RMobilePhone::TMmsNotificationV3Pckg *mmsPckg = REINTERPRET_CAST(RMobilePhone::TMmsNotificationV3Pckg *, aNotification);
	RMobilePhone::TMmsNotificationV3 &mms = (*mmsPckg)();

	if (!iNotifyMmsUpdate++)
		{
		mms.iSpace = DMMTSY_PHONE_MMS_UPDATE_SPACE;
		mms.iNotification = DMMTSY_PHONE_MMS_UPDATE_NOTIFICATION;
		mms.iMmsStatus = DMMTSY_PHONE_MMS_UPDATE_STATUS;
		mms.iImplementation = DMMTSY_PHONE_MMS_UPDATE_IMPLEMENTATION;
		mms.iNotify = DMMTSY_PHONE_MMS_UPDATE_NOTIFY;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetAirTimeDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAirTimeDuration called"));
	*aTime=DMMTSY_AIR_TIME_DURATION1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyAirTimeDurationChange(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime)
	{
	if (!iNotifyAirTimeChangeChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyAirTimeDurationChange called"));
		*aTime=DMMTSY_AIR_TIME_DURATION2;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyAirTimeDurationChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyAirTimeDurationChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::TerminateAllCalls(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::TerminateAllCalls called"));
	if (!iTerminateAllCalls++)
		{
		//Just return KErrNone
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::TerminateAllCallsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::TerminateAllCallsCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::TerminateActiveCalls(const TTsyReqHandle aTsyReqHandle)
    {
    LOGTEXT(_L8("CPhoneDMmTsy::TerminateActiveCalls called"));
    if (!iTerminateActiveCalls++)
        {
        //Just return KErrNone
        ReqCompleted(aTsyReqHandle,KErrNone);
        }
    return KErrNone;
    }
    
TInt CPhoneDMmTsy::TerminateActiveCallsCancel(const TTsyReqHandle aTsyReqHandle)
    {
    LOGTEXT(_L8("CPhoneDMmTsy::TerminateActiveCallsCancel called"));
    RemoveDelayedReq(aTsyReqHandle);
    ReqCompleted(aTsyReqHandle,KErrCancel);
    return KErrNone;
    }

 TInt CPhoneDMmTsy::NotifySendNetworkServiceRequest(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNotifySendSSOperation* aOperation, TDes8* aRequestComplete)
 	{
 	LOGTEXT(_L8("CPhoneDMmTsy::NotifySendNetworkServiceRequest called"));
	if (!iNotifySendNetworkServiceRequest++)
		{
		RMobilePhone::TMobilePhoneSendSSRequestV3Pckg* ssRequestPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneSendSSRequestV3Pckg*,aRequestComplete);
		RMobilePhone::TMobilePhoneSendSSRequestV3& ssRequest = (*ssRequestPckg)();
			
		if(*aOperation!=DMMTSY_PHONE_NOTIFY_SERVICEREQUEST || 
			ssRequest.ExtensionId()!=KETelExtMultimodeV3)
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			{	
			ssRequest.iOpCode = DMMTSY_PHONE_NOTIFY_OPCODE;
			ssRequest.iAdditionalInfo = DMMTSY_PHONE_NOTIFY_ADDINFO;			
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}	
	return KErrNone;
 	}
 
 TInt CPhoneDMmTsy::NotifySendNetworkServiceRequestCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifySendNetworkServiceRequestCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
 TInt CPhoneDMmTsy::NotifyAllSendNetworkServiceRequest(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNotifySendSSOperation* aOperation, TDes8* aRequestComplete)
 	{
 	LOGTEXT(_L8("CPhoneDMmTsy::NotifyAllSendNetworkServiceRequest called"));
	
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg* ssRequestPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneSendSSRequestV3Pckg*,aRequestComplete);
	RMobilePhone::TMobilePhoneSendSSRequestV3& ssRequest = (*ssRequestPckg)();
			
	if(ssRequest.ExtensionId()!=KETelExtMultimodeV3)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		//Return the error for some Invoke operation, like received from network.
		*aOperation = DMMTSY_PHONE_NOTIFY_SERVICEREQUEST_ERROR;
		//Return result problem code (return Mistyped Parameter)
		ssRequest.iOpCode = DMMTSY_PHONE_NOTIFY_RETURN_ERROR_PROBLEM_CODE;
		ssRequest.iAdditionalInfo = DMMTSY_PHONE_NOTIFY_ADDINFO;
		AddDelayedReq(aTsyReqHandle, this);
		}				
	
	return KErrNone;
 	}
 	
 TInt CPhoneDMmTsy::NotifyAllSendNetworkServiceRequestCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyAllSendNetworkServiceRequestCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

 TInt CPhoneDMmTsy::ClearBlacklist(const TTsyReqHandle aTsyReqHandle)
 	{
	LOGTEXT(_L8("CPhoneDMmTsy::ClearBlacklist called"));
	if (!iClearBlacklist++)
		{
		//Just return KErrNone
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone; 	
 	}
 	
TInt CPhoneDMmTsy::ClearBlacklistCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ClearBlacklistCancel called"));
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyDTMFEvent(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TMobilePhoneDTMFEvent* aEvent)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyDTMFEvent called"));
	if (!iNotifyDTMFEvent++)
		{
		*aEvent = DMMTSY_PHONE_NOTIFY_DTMFEVENT_ON;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone; 	
	}
	
TInt CPhoneDMmTsy::NotifyDTMFEventCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyDTMFEventCancel called"));
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetPersonalisationCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps) 
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPersonalisationCaps called"));
	*aCaps = (DMMTSY_PHONE_PERS_CAPS_SERV_PROV | DMMTSY_PHONE_PERS_CAPS_NETWORK);
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetPersonalisationCapsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPersonalisationCapsCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetPersonalisationStatus(const TTsyReqHandle aTsyReqHandle, TUint32* aPersSchemes) 
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPersonalisationStatus called"));
	*aPersSchemes = (DMMTSY_PHONE_PERS_GET_NETWORK_SUB | DMMTSY_PHONE_PERS_GET_CORPORATE);
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetPersonalisationStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPersonalisationStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetPersonalisationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhonePersonalisation* aPersSchemes, TDes* aPersCode) 
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetPersonalisationStatus called"));
	if((*aPersSchemes == DMMTSY_PHONE_PERS_SET_USIM) &&
	   (*aPersCode == DMMTSY_PHONE_PERS_SET_CODE))
		AddDelayedReq(aTsyReqHandle,this);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);			
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::SetPersonalisationStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetPersonalisationStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCurrentNetworkName(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkName, TDes8* aLocInfo)	
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCurrentNetworkName called"));

	RMobilePhone::TMobilePhoneNetworkNameV3Pckg* networkNamePckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneNetworkNameV3Pckg*,aNetworkName);
	RMobilePhone::TMobilePhoneNetworkNameV3& networkName = (*networkNamePckg)();
	
	RMobilePhone::TMobilePhoneOPlmnV3Pckg* plmnPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneOPlmnV3Pckg*,aLocInfo);
	RMobilePhone::TMobilePhoneOPlmnV3& plmn = (*plmnPckg)();
	
	if(networkName.ExtensionId() != KETelExtMultimodeV3 ||
	   plmn.ExtensionId() != KETelExtMultimodeV3)
	   		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
	 else
	 	{
	 	//Fill TMobilePhoneNetworkNameV3
	 	networkName.iLongName = DMMTSY_PHONE_HOME_NETWORK_LONG_NAME;
		networkName.iShortName = DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME;
		networkName.iOtherNames = DMMTSY_PHONE_HOME_NETWORK_OTHER_NAMES;
		//Fill TMobilePhoneOPlmnV3
		plmn.iCountryCode = DMMTSY_PHONE_COUNTRY_ID;
		plmn.iNetworkId = DMMTSY_PHONE_NETWORK_ID;
		plmn.iPNNid = DMMTSY_PHONE_HOME_NETWORK_NAME_LOC_ID;		
		plmn.iFirstLocationAreaCode = DMMTSY_PHONE_HOME_NETWORK_FIRST_ID;
		plmn.iLastLocationAreaCode = DMMTSY_PHONE_HOME_NETWORK_LAST_ID;
		AddDelayedReq(aTsyReqHandle,this);
	 	}
	return KErrNone;	
	}
	
TInt CPhoneDMmTsy::GetCurrentNetworkNameCancel(const TTsyReqHandle aTsyReqHandle)	
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCurrentNetworkNameCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetPreferredNetworksPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPreferredNetworksPhase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetPreferredNetworksPhase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	}	
	
TInt CPhoneDMmTsy::ProcessGetPreferredNetworksPhase1L(const TTsyReqHandle aTsyReqHandle, 
													 RMobilePhone::TClientId* aClient, 
													 TInt* aBufSize)
	{
	// retrieve stored networks from USIM,
	// store each entry.
	// stream the list and then return size of this buffer to client
	CMobilePhoneStoredNetworkList* list=CMobilePhoneStoredNetworkList::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TMobilePreferredNetworkEntryV3 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_PHONE_NTWK_LIST_ENTRIES; index++)
		{
		switch (index)
			{
		case 0:
			entry.iAccess = DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM;
			entry.iUserDefined = DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE;
			entry.iCountryCode = DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID1;
			entry.iNetworkId = DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1;
			break;
		case 1:
			entry.iAccess = DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSMC;
			entry.iUserDefined = DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE;
			entry.iCountryCode = DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2;
			entry.iNetworkId = DMMTSY_PHONE_NTWK_LIST_NETWORK_ID2;
			break;
		case 2:
		default:
			entry.iAccess = DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_UTRAN;
			entry.iUserDefined = DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE;
			entry.iCountryCode = DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2;
			entry.iNetworkId = DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1;
			break;
		case 3:
			entry.iAccess = DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_WLAN;
			entry.iUserDefined = DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE;
			entry.iCountryCode = DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2;
			entry.iNetworkId = DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1;
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
	
	iGetPreferredNetworks->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}



TInt CPhoneDMmTsy::GetPreferredNetworksPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPreferredNetworksPhase2 called"));
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
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}
	
TInt CPhoneDMmTsy::GetPreferredNetworksCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPreferredNetworksPhaseCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	// Remove the read all attempt from iGetPreferredNetworks
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
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyMmsUpdateCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyMmsUpdateCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetMmsUserConnParams(const TTsyReqHandle aTsyReqHandle, TDes8* aConnectivity)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetMmsUserConnParams called"));
	if(aConnectivity->Compare(DMMTSY_PHONE_MMS_USER_CONN_PARAMS) == 0 )
		AddDelayedReq(aTsyReqHandle,this);		
	else
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
	return KErrNone;
	} 
	
TInt CPhoneDMmTsy::SetMmsUserConnParamsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetMmsUserConnParamsCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetMmsUserPreferences(const TTsyReqHandle aTsyReqHandle, TDes8* aPreferences)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetMmsUserPreferences called"));
	if(aPreferences->Compare(DMMTSY_PHONE_MMS_USER_PREF_PARAMS) == 0 )
		AddDelayedReq(aTsyReqHandle,this);		
	else
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetMmsUserPreferencesCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetMmsUserPreferencesCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetMmsConfig(const TTsyReqHandle aTsyReqHandle,  RMobilePhone::TMmsConnParams aType, TDes8* aConnectivity)

	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetMmsConfig called"));
	if (aType == DMMTSY_PHONE_MMS_USER_CONN_PARAMS_TYPE )
		{
		*aConnectivity = DMMTSY_PHONE_MMS_USER_CONN_PARAMS;
		AddDelayedReq(aTsyReqHandle,this);
		} 
	else
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetMmsConfigCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetMmsConfigCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}	
	
TInt CPhoneDMmTsy::NotifyMmsConfig(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMmsConnParams aType, TDes8* aConnectivity)
	
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyMmsConfig called"));
	if(!iNotifyMmsConfig++)
		{
		if (aType == DMMTSY_PHONE_MMS_USER_CONN_PARAMS_TYPE )
			{
			*aConnectivity = DMMTSY_PHONE_MMS_USER_CONN_PARAMS;
			AddDelayedReq(aTsyReqHandle,this);
			} 
		else
			ReqCompleted(aTsyReqHandle, KErrCorrupt);
		}
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyMmsConfigCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyMmsConfigCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetHomeNetworkSearchPeriod(const TTsyReqHandle aTsyReqHandle, TInt* aSearchIntv)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetHomeNetworkSearchPeriod called"));
	*aSearchIntv = DMMTSY_PHONE_HOME_NETWORK_SEARCHINTV;
	AddDelayedReq(aTsyReqHandle,this);	
	return KErrNone;
	
	}
	
TInt CPhoneDMmTsy::GetHomeNetworkSearchPeriodCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetHomeNetworkSearchPeriodCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetLastUsedAccessTechnology(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkAccess* aAccessTech)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetLastUsedAccessTechnology called"));
	*aAccessTech = DMMTSY_PHONE_LAST_USED_ACCESS_TECH;
	AddDelayedReq(aTsyReqHandle,this);	
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetLastUsedAccessTechnologyCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetLastUsedAccessTechnologyCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetNetworkInvScanSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneInvestigationScan* aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetNetworkInvScanSetting called"));
	*aSetting = DMMTSY_PHONE_NETWORK_INV_SCAN;
	AddDelayedReq(aTsyReqHandle,this);	
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyNetworkInvScanChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneInvestigationScan* aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkInvScanChange called"));
	if(!iNotifyNetworkInvScanChange++)
		{
		*aSetting = DMMTSY_PHONE_NETWORK_INV_SCAN;
		AddDelayedReq(aTsyReqHandle,this);	
		}
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyNetworkInvScanChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkInvScanChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}
	
TInt CPhoneDMmTsy::NotifyNetworkInvScanEvent(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneInvestigationScanEvent* aEvent)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkInvScanEvent called"));
	if(!iNotifyNetworkInvScanEvent++)
		{
		*aEvent = DMMTSY_PHONE_NETWORK_INV_SCAN_EVENT;
		AddDelayedReq(aTsyReqHandle,this);	
		}
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyNetworkInvScanEventCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyNetworkInvScanEventCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}
	
TInt CPhoneDMmTsy::GetCallForwardingIndicator(const TTsyReqHandle aTsyReqHandle, TDes8* aIndicator)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallForwardingIndicator called"));
	
	RMobilePhone::TMobilePhoneCFUIndicatorV3Pckg* indicatorPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneCFUIndicatorV3Pckg*, aIndicator);
	RMobilePhone::TMobilePhoneCFUIndicatorV3& indicator = (*indicatorPckg)();
	if( indicator.ExtensionId() == KETelExtMultimodeV3 )
		{
		indicator.iIndicator = DMMTSY_PHONE_CFU_INDICATOR;
		}
	else if(indicator.ExtensionId() == KEtelExtMultimodeV6 )
		{
		RMobilePhone::TMobilePhoneCFUIndicatorV6Pckg* indicatorPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneCFUIndicatorV6Pckg*, aIndicator);
		RMobilePhone::TMobilePhoneCFUIndicatorV6& indicator = (*indicatorPckg)();
		indicator.iIndicator = DMMTSY_PHONE_CFU_INDICATOR1;
		indicator.iCFNumber.iTypeOfNumber = DMMTSY_PHONE_CFU_TON;
		indicator.iCFNumber.iNumberPlan = DMMTSY_PHONE_CFU_NPI;
		indicator.iCFNumber.iTelNumber = DMMTSY_PHONE_CFU_TELNUMBER;
		indicator.iMultipleSubscriberProfileID = DMMTSY_PHONE_CFU_MSP;
		}
	AddDelayedReq(aTsyReqHandle,this);	
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetCallForwardingIndicatorCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCallForwardingIndicatorCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetCompMethodName(const TTsyReqHandle aTsyReqHandle, TUint aCmi, TDes* aAlphaTag)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCompMethodName called"));
	if(aCmi != DMMTSY_PHONE_CB_COMP_METHOD_ID)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		*aAlphaTag = DMMTSY_PHONE_CB_ALPHA_TAG;
		AddDelayedReq(aTsyReqHandle,this);	
		}
	return KErrNone;
	}
TInt CPhoneDMmTsy::GetCompMethodNameCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCompMethodNameCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::EnumerateAPNEntries(const TTsyReqHandle aTsyReqHandle, TUint32* aIndex)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::EnumerateAPNEntries called"));
	*aIndex = DMMTSY_PHONE_APN_ENUMERATE;
	AddDelayedReq(aTsyReqHandle,this);	
	return KErrNone;
	}

TInt CPhoneDMmTsy::EnumerateAPNEntriesCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::EnumerateAPNEntriesCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetAPNname(const TTsyReqHandle aTsyReqHandle, TUint32 aIndex, TDes8* aAPN)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAPNname called"));
	
	RMobilePhone::TAPNEntryV3Pckg* entryPckg = REINTERPRET_CAST(RMobilePhone::TAPNEntryV3Pckg*, aAPN);
	RMobilePhone::TAPNEntryV3& entry = (*entryPckg)();
	
	switch(aIndex)
		{
	case 0:
		{
		entry.iApn.Append(DMMTSY_PHONE_APN_ENTRY1);
		AddDelayedReq(aTsyReqHandle,this);
		}
		break;
	case 1:
		{
		entry.iApn.Append(DMMTSY_PHONE_APN_ENTRY2);
		AddDelayedReq(aTsyReqHandle,this);	
		}
		break;
	case 2:
		{
		entry.iApn.Append(DMMTSY_PHONE_APN_ENTRY3);
		AddDelayedReq(aTsyReqHandle,this);	
		}
		break;
	default:
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		break;
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetAPNnameCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAPNnameCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::AppendAPNName(const TTsyReqHandle aTsyReqHandle, TDes8* aAPN)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::AppendAPNName called"));
	
	RMobilePhone::TAPNEntryV3Pckg* entryPckg = REINTERPRET_CAST(RMobilePhone::TAPNEntryV3Pckg*, aAPN);
	RMobilePhone::TAPNEntryV3& entry = (*entryPckg)();
	
	if ((entry.iApn.Length() > RMobilePhone::KMaxApnName) ||
		(entry.iApn != DMMTSY_PHONE_APN_ENTRY_TO_APPEND))
		{
		// Checking that the entry passed in is equal to DMMTSY_PHONE_APN_ENTRY_TO_APPEND
		// confirms that the APN entry has made it from the client to the TSY properly,
		// i.e the packaging etc. works.
		ReqCompleted(aTsyReqHandle, KErrBadName);
		}
	else 
		{
		AddDelayedReq(aTsyReqHandle, this);
		}
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::AppendAPNNameCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::AppendAPNNameCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::DeleteAPNName(const TTsyReqHandle aTsyReqHandle, TUint32 aIndex)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::DeleteAPNName called"));
	if (aIndex > (DMMTSY_PHONE_APN_ENUMERATE-1))
		{
		ReqCompleted(aTsyReqHandle, KErrTooBig);
		}
	else
		{
		AddDelayedReq(aTsyReqHandle, this);
		}
	return KErrNone; 
	}
	
TInt CPhoneDMmTsy::DeleteAPNNameCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::DeleteAPNNameCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);	
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyAPNListChanged(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyAPNListChanged called"));
	AddDelayedReq(aTsyReqHandle, this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyAPNListChangedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyAPNListChangedCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);	
	return KErrNone;	
	}

TInt CPhoneDMmTsy::SetAPNControlListServiceStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAPNControlListServiceStatus aAPNControlListServiceStatus)
	{
	if ((aAPNControlListServiceStatus != DMMTSY_PHONE_APN_CONTROL_LIST_SERVICE_ENABLED) &&
		(aAPNControlListServiceStatus != DMMTSY_PHONE_APN_CONTROL_LIST_SERVICE_DISABLED))
		{
		ReqCompleted(aTsyReqHandle, KErrArgument);
		}
	else
		{
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::SetAPNControlListServiceStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetAPNControlListServiceStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);	
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetAPNControlListServiceStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAPNControlListServiceStatus* aAPNControlListServiceStatus)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAPNControlListServiceStatus called"));
	*aAPNControlListServiceStatus = DMMTSY_PHONE_APN_CONTROL_LIST_SERVICE_DISABLED;
	AddDelayedReq(aTsyReqHandle, this);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetAPNControlListServiceStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAPNControlListServiceStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);	
	return KErrNone;	
	}

TInt CPhoneDMmTsy::NotifyAPNControlListServiceStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAPNControlListServiceStatus* aAPNControlListServiceStatus)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyAPNControlListServiceStatusChange called"));
	*aAPNControlListServiceStatus = DMMTSY_PHONE_APN_CONTROL_LIST_SERVICE_ENABLED;
	AddDelayedReq(aTsyReqHandle, this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyAPNControlListServiceStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyAPNControlListServiceStatusChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::StorePreferredNetworksList(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::StorePreferredNetworksList called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessStorePreferredNetworksListL(aTsyReqHandle, aBuffer););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	};

TInt CPhoneDMmTsy::ProcessStorePreferredNetworksListL(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	CMobilePhoneStoredNetworkList* list=CMobilePhoneStoredNetworkList::NewL();
	CleanupStack::PushL(list);
	
	list->RestoreL(*aBuffer);

	RMobilePhone::TMobilePreferredNetworkEntryV3 entry;

	if (list->Enumerate() != DMMTSY_PHONE_NTWK_LIST_ENTRIES)
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		CleanupStack::PopAndDestroy(); // list
		return KErrNone;
		}

	TInt error=KErrNone;
	TInt ret;
	for (TInt i=0; i<DMMTSY_PHONE_NTWK_LIST_ENTRIES; ++i)
		{
		TRAP(ret,entry=list->GetEntryL(i));
		if (ret != KErrNone)
			break;
			switch(i)
			{
			case 0:
				{
				if(entry.iAccess != DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_WLAN ||
				entry.iUserDefined != DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE ||
				entry.iCountryCode != DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2 ||
				entry.iNetworkId != DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1)
					error  = KErrCorrupt;
				}
				
				break;
			case 1:
				{
				if(entry.iAccess != DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_UTRAN ||
				entry.iUserDefined != DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE ||
				entry.iCountryCode != DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2 ||
				entry.iNetworkId != DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1)
					error  = KErrCorrupt;
				}
				
				break;
			case 2:
				{
				if(entry.iAccess != DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSMC ||
				entry.iUserDefined != DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE ||
				entry.iCountryCode != DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2 ||
				entry.iNetworkId != DMMTSY_PHONE_NTWK_LIST_NETWORK_ID2)
					error  = KErrCorrupt;
				}
				break;
			case 3:
			default:
				{
				if(entry.iAccess != DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM ||
				entry.iUserDefined != DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE ||
				entry.iCountryCode != DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID1 ||
				entry.iNetworkId != DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1)
					error  = KErrCorrupt;
				}
				break;
			}
		}
	if(error==KErrNone)	
		AddDelayedReq(aTsyReqHandle,this);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	CleanupStack::PopAndDestroy(); // list
	return KErrNone;
	}

TInt CPhoneDMmTsy::StorePreferredNetworksListCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmspStoreDMmTsy::StorePreferredNetworksListCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyStorePreferredNetworksListChange(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyStorePreferredNetworksListChange called"));
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyStorePreferredNetworksListChangeCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyStorePreferredNetworksListChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetCipheringIndicatorStatus(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobileCallCipheringIndicator& aIndicatorStatus)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCipheringIndicatorStatus called"));
	aIndicatorStatus = DMMTSY_PHONE_NET_CIPHER_IND;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetCipheringIndicatorStatusCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCipheringIndicatorStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyCipheringIndicatorStatus(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobileCallCipheringIndicator& aIndicatorStatus)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCipheringIndicatorStatus called"));
	if (!iNotifyCipheringIndicatorStatus++)
		{
		aIndicatorStatus = DMMTSY_PHONE_NET_CIPHER_IND;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyCipheringIndicatorStatusCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyCipheringIndicatorStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetIdentityServiceStatus(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIdService aService, RMobilePhone::TMobilePhoneIdServiceSetting aSetting)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetIdentityServiceStatus called"));
	if(aService != DMMTSY_PHONE_ID_SERVICE || aSetting != DMMTSY_PHONE_ID_SETTING)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		AddDelayedReq(aTsyReqHandle,this);

	return KErrNone;
	}

TInt CPhoneDMmTsy::SetIdentityServiceStatusCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetIdentityServiceStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt  CPhoneDMmTsy::NotifyIdentityServiceStatus(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneIdService aService, RMobilePhone::TMobilePhoneIdServiceStatus* aStatus)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyIdentityServiceStatus called"));
	if (!iNotifyIdentityServiceStatus++)
		{
		if(aService!=DMMTSY_PHONE_ID_SERVICE)
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			{
			*aStatus = DMMTSY_PHONE_ID_STATUS;
			AddDelayedReq(aTsyReqHandle,this);
			}
		}
	return KErrNone;
	}
TInt CPhoneDMmTsy::NotifyIdentityServiceStatusCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyIdentityServiceStatusCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt  CPhoneDMmTsy::NotifyIdentitySuppressionRejected(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyIdentitySuppressionRejected called"));
	if (!iNotifyIdentitySuppressionRejected++)
		{
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}
TInt CPhoneDMmTsy::NotifyIdentitySuppressionRejectedCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyIdentitySuppressionRejectedCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyMulticallIndicatorChange(TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneMulticallIndicator* aMulticallIndicator)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyMulticallIndicatorChange called"));
	if (!iNotifyMulticallIndicatorChange++)
		{
		*aMulticallIndicator = DMMTSY_PHONE_MULTICALL_IND;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyMulticallIndicatorChangeCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyMulticallIndicatorChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::EnumerateSmartCardApplications(const TTsyReqHandle aTsyReqHandle,TUint* aCount)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::EnumerateSmartCardApplications called"));

	*aCount = DMMTSY_PHONE_SC_COUNT;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CPhoneDMmTsy::EnumerateSmartCardApplicationsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::EnumerateSmartCardApplicationsCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetSmartCardApplicationInfo(const TTsyReqHandle aTsyReqHandle,TUint* aAppIndex, TDes8* aInfoBuffer)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSmartCardApplicationInfo called"));

	if(*aAppIndex == DMMTSY_PHONE_SC_APP_INDEX1)
		{
		RMobilePhone::TSmartCardApplicationInfoV5Pckg *ScAppInfoPckg = REINTERPRET_CAST(RMobilePhone::TSmartCardApplicationInfoV5Pckg *, aInfoBuffer);
		RMobilePhone::TSmartCardApplicationInfoV5 &ScAppInfo = (*ScAppInfoPckg)();
		
		if(ScAppInfo.ExtensionId() != KEtelExtMultimodeV5)
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ScAppInfo.iAID = DMMTSY_PHONE_SC_AID;
			ScAppInfo.iLabel = DMMTSY_PHONE_SC_LABEL;
			}			
		}
	else
		{
		return DMMTSY_PHONE_SC_OUTOFRANGE;	
		}	
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;		
	}
	
TInt CPhoneDMmTsy::GetSmartCardApplicationInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSmartCardApplicationInfoCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifySmartCardApplicationInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TSmartCardApplicationEvent* aEvent, RMobilePhone::TAID* aAID)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifySmartCardApplicationInfoChange called"));
	if(!iNotifySmartCardApplicationInfoChange++)
		{
		*aEvent = DMMTSY_PHONE_SC_EVENT_ACTIVATE;
		*aAID = DMMTSY_PHONE_SC_AID;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifySmartCardApplicationInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifySmartCardApplicationInfoChange called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::SetSmartCardApplicationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TSmartCardApplicationAction aAction)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetSmartCardApplicationStatus called"));

	if(*aAID != DMMTSY_PHONE_SC_AID2 || aAction != DMMTSY_PHONE_SC_ACTION_TERMINATE)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else		
		AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;		
	}
	
TInt CPhoneDMmTsy::SetSmartCardApplicationStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetSmartCardApplicationStatusCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::GetScFileInfo(const TTsyReqHandle aTsyReqHandle, const RMobilePhone::TScFilePath& aFilePathId, TDes8* aInfo)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetScFileInfo called"));

	if(aFilePathId.iAID != DMMTSY_PHONE_SC_AID || aFilePathId.iPath != DMMTSY_PHONE_SC_PATH_REF)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else		
		{
		RMobilePhone::TScFileInfoV5Pckg *scFileInfoPckg = REINTERPRET_CAST(RMobilePhone::TScFileInfoV5Pckg*, aInfo);
		RMobilePhone::TScFileInfoV5 &scFileInfo = (*scFileInfoPckg)();
		
		if(scFileInfo.ExtensionId() != KEtelExtMultimodeV5)
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			scFileInfo.iFileID = DMMTSY_PHONE_SC_FILEID;
			scFileInfo.iNumberRecords = DMMTSY_PHONE_SC_NORECORDS;
			scFileInfo.iRecordLength = DMMTSY_PHONE_SC_RECORD_SIZE;
			scFileInfo.iType = DMMTSY_PHONE_SC_TYPE;
			scFileInfo.iTotalLength = DMMTSY_PHONE_SC_FILESIZE;
			}	
		AddDelayedReq(aTsyReqHandle,this);
		}		
	return KErrNone;		
	}
	
TInt CPhoneDMmTsy::GetScFileInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetScFileInfoCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::ReadScFile(const TTsyReqHandle aTsyReqHandle, const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ReadScFile called"));
	
	if(aFilePathOffsets.iAID != DMMTSY_PHONE_SC_AID || aFilePathOffsets.iPath != DMMTSY_PHONE_SC_PATH_REF || aFilePathOffsets.iType != DMMTSY_PHONE_SC_TYPE || aFilePathOffsets.iCount != DMMTSY_PHONE_SC_NORECORDS || aFilePathOffsets.iStartOffset != DMMTSY_PHONE_SC_START_OFFSET)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else		
		{
		aReadBuffer->Zero();
		aReadBuffer->Append(DMMTSY_PHONE_SC_RECORD0);
		aReadBuffer->Append(DMMTSY_PHONE_SC_RECORD1);
		aReadBuffer->Append(DMMTSY_PHONE_SC_RECORD2);
		
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::ReadScFileCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ReadScFileCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}
	
TInt CPhoneDMmTsy::UpdateScFile(const TTsyReqHandle aTsyReqHandle, const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aWriteBuffer)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::UpdateScFile called"));
	
	TInt startoffset = 0;
	TPtr8 record0(aWriteBuffer->MidTPtr(startoffset,DMMTSY_PHONE_SC_RECORD_SIZE));
	startoffset += DMMTSY_PHONE_SC_RECORD_SIZE;
	TPtr8 record1(aWriteBuffer->MidTPtr(startoffset,DMMTSY_PHONE_SC_RECORD_SIZE));
	startoffset += DMMTSY_PHONE_SC_RECORD_SIZE;
	TPtr8 record2(aWriteBuffer->MidTPtr(startoffset,DMMTSY_PHONE_SC_RECORD_SIZE));
		
	if(aFilePathOffsets.iAID != DMMTSY_PHONE_SC_AID || aFilePathOffsets.iPath != DMMTSY_PHONE_SC_PATH_REF || aFilePathOffsets.iType != DMMTSY_PHONE_SC_TYPE || aFilePathOffsets.iCount != DMMTSY_PHONE_SC_NORECORDS || aFilePathOffsets.iStartOffset != DMMTSY_PHONE_SC_START_OFFSET)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else if(record0 != DMMTSY_PHONE_SC_RECORD0 || record1 != DMMTSY_PHONE_SC_RECORD1 || record2 != DMMTSY_PHONE_SC_RECORD2)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else	
		AddDelayedReq(aTsyReqHandle,this);
		
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::UpdateScFileCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::UpdateScFileCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}
	
TInt CPhoneDMmTsy::GetCurrentActiveUSimApplication(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCurrentActiveUSimApplication called"));

	*aAID = DMMTSY_PHONE_SC_AID;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;		
	}
	
TInt CPhoneDMmTsy::GetCurrentActiveUSimApplicationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetCurrentActiveUSimApplicationCancel called"));

	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}
	
CPhoneDMmTsy* CPhoneDMmTsy::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CPhoneDMmTsy*,aPtr);
	}

TInt CPhoneDMmTsy::GetAuthorizationInfoPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAuthorizationInfoPhase1 called"));
	TInt ret(KErrNone);
	TInt leaveCode(KErrNone);
	TRAP(leaveCode, ret=ProcessGetAuthorizationInfoPhase1L(aTsyReqHandle, aId, aBufSize););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	};

TInt CPhoneDMmTsy::ProcessGetAuthorizationInfoPhase1L(const TTsyReqHandle aTsyReqHandle,
											 RMobilePhone::TClientId* aClient,
											 TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAuthorizationInfoPhase1 called"));	

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	HBufC8* iBuff=NULL;
	iAuthInfo->ExternalizeL(iBuff);	
	CleanupStack::PushL(iBuff);
		
	CBufFlat* buf=CBufFlat::NewL(iBuff->Length());
	CleanupStack::PushL(buf);
	buf->InsertL(0,*iBuff);	
	
	read->iListBuf = buf;
	CleanupStack::Pop(buf);	
	CleanupStack::PopAndDestroy(iBuff);
	
	iGetAuthorizationData->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}
TInt CPhoneDMmTsy::GetAuthorizationInfoPhase2(TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAuthorizationInfoPhase2 called"));	
	CListReadAllAttempt* read=NULL;
	// Find the get detected network attempt from this client
	for (TInt i=0; i<iGetAuthorizationData->Count(); ++i)
		{
		read = iGetAuthorizationData->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuffer->Copy(bufPtr);
			delete read;
			iGetAuthorizationData->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrBadHandle);
	return KErrNone;
	}


TInt CPhoneDMmTsy::GetAuthorizationInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAuthorizationInfoCancel called"));

	// Remove the read all attempt from iGetAuthorizationData
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetAuthorizationData->Count(); ++i)
		{
		read = iGetAuthorizationData->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetAuthorizationData->Delete(i);
			break;
			}
		}
		
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetAuthenticationData(const TTsyReqHandle aTsyReqHandle,TDes8* aAuthenticationData)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::AuthentificationData called"));
	TInt ret(KErrNone);
	User::After(300000); // wait to have KRequestPending
	RMobilePhone::TImsAuthenticateDataV5Pckg* authenticateDataPckgd=(RMobilePhone::TImsAuthenticateDataV5Pckg*)aAuthenticationData;
	RMobilePhone::TImsAuthenticateDataV5& authenticateData=(*authenticateDataPckgd)();
	if(authenticateData.iAUTN!=DMMTSY_PHONE_AUTN_IN || authenticateData.iRAND!=DMMTSY_PHONE_RAND_IN)
		{
		ret=KErrCorrupt;
		ReqCompleted(aTsyReqHandle,ret);
		}
	else
		{
		authenticateData.iAUTN=DMMTSY_PHONE_AUTN_OUT;
		authenticateData.iRAND=DMMTSY_PHONE_RAND_OUT;
		authenticateData.iRES=DMMTSY_PHONE_RES_OUT;
		authenticateData.iIK=DMMTSY_PHONE_IK_OUT;
		authenticateData.iCK=DMMTSY_PHONE_CK_OUT;
		authenticateData.iAUTS=DMMTSY_PHONE_AUTS_OUT;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetAuthenticationDataCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::AuthentificationDataCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyImsAuthorizationInfoChanged(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyImsAuthorizationInfoChanged called"));
	if (!iNotifyImsAuthorizationInfoChange++)
		{
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	
	}
TInt CPhoneDMmTsy::NotifyImsAuthorizationInfoChangedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyImsAuthorizationInfoChangedCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetSmartCardAuthenticationData(const TTsyReqHandle aTsyReqHandle, TDes8* aSmartCardAuthData, RMobilePhone::TAID* aAID)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSmartCardAuthenticationData called"));

	RMobilePhone::TSmartCardAuthenticateDataV6Pckg *smartCardAuthenticateDataPckg =
	  reinterpret_cast<RMobilePhone::TSmartCardAuthenticateDataV6Pckg*>(aSmartCardAuthData);

	RMobilePhone::TSmartCardAuthenticateDataV6 &smartCardAuthenticateData =
	  (*smartCardAuthenticateDataPckg)();

	if (aAID != NULL)
		{
		if (*aAID != DMMTSY_PHONE_GBA_AID() && 
			*aAID != DMMTSY_PHONE_EAPAPP_AID() &&
			*aAID != DMMTSY_PHONE_MBMS_AID())
			{
			ReqCompleted(aTsyReqHandle, KErrCorrupt);
			return KErrNone;
			}
		}
	// USIM authentication
	if(smartCardAuthenticateData.ExtensionId() == KEtelExtMultimodeV6)
		{
		if(smartCardAuthenticateData.iAUTN != DMMTSY_PHONE_SC_AUTN_IN ||
		  smartCardAuthenticateData.iRAND != DMMTSY_PHONE_SC_RAND_IN)
			{
			ReqCompleted(aTsyReqHandle, KErrCorrupt);
			}
		else
			{
			smartCardAuthenticateData.iAUTN=DMMTSY_PHONE_SC_AUTN_OUT;
			smartCardAuthenticateData.iRAND=DMMTSY_PHONE_SC_RAND_OUT;
			smartCardAuthenticateData.iRES=DMMTSY_PHONE_SC_RES_OUT;
			smartCardAuthenticateData.iIK=DMMTSY_PHONE_SC_IK_OUT;
			smartCardAuthenticateData.iCK=DMMTSY_PHONE_SC_CK_OUT;
			smartCardAuthenticateData.iAUTS=DMMTSY_PHONE_SC_AUTS_OUT;
			smartCardAuthenticateData.iKc=DMMTSY_PHONE_SC_KC_OUT;
			AddDelayedReq(aTsyReqHandle,this);
			}
		}
	else if(smartCardAuthenticateData.ExtensionId() == RMobilePhone::KETelMobileGbaBootstrapAuthDataV8)
		{
		RMobilePhone::TGbaBootstrapAuthDataV8Pckg *gbaBootstrapAuthenticateDataPckg =
		  STATIC_CAST(RMobilePhone::TGbaBootstrapAuthDataV8Pckg*,aSmartCardAuthData);

		RMobilePhone::TGbaBootstrapAuthDataV8 &gbaBootstrapAuthData =
		  (*gbaBootstrapAuthenticateDataPckg)();
		
		 if(gbaBootstrapAuthData.iAUTN != DMMTSY_PHONE_GBA_AUTN_IN ||
				gbaBootstrapAuthData.iRAND != DMMTSY_PHONE_GBA_RAND_IN)
			{
			iGbaBootstrapAuthDataFail++;
			switch(iGbaBootstrapAuthDataFail)
				{
			case 1:
				gbaBootstrapAuthData.iRES=DMMTSY_PHONE_GBA_FAIL_RES_OUT;
				gbaBootstrapAuthData.iAUTS=DMMTSY_PHONE_GBA_FAIL_AUTS_OUT;
				ReqCompleted(aTsyReqHandle, KErrMMEtelAuthVerificationFailure);
				break;
			
			case 2:
				gbaBootstrapAuthData.iRES=DMMTSY_PHONE_GBA_FAIL_RES_OUT;
				gbaBootstrapAuthData.iAUTS=DMMTSY_PHONE_GBA_SUCC_AUTS_OUT;
				ReqCompleted(aTsyReqHandle, KErrMMEtelAuthMacInvalid);
				break;
				
			case 3:
			default:
				gbaBootstrapAuthData.iRES=DMMTSY_PHONE_GBA_FAIL_RES_OUT;
				gbaBootstrapAuthData.iAUTS=DMMTSY_PHONE_GBA_SUCC_AUTS_OUT;
				ReqCompleted(aTsyReqHandle, KErrInUse);
				iGbaBootstrapAuthDataFail=0;
				break;
				}
			}
		else
			{
			gbaBootstrapAuthData.iRES=DMMTSY_PHONE_GBA_SUCC_RES_OUT;
			gbaBootstrapAuthData.iAUTS=DMMTSY_PHONE_GBA_SUCC_AUTS_OUT;
			AddDelayedReq(aTsyReqHandle,this);
			}
		}
	else if(smartCardAuthenticateData.ExtensionId() == RMobilePhone::KETelMobileGbaNafAuthDataV8)
		{
		RMobilePhone::TGbaNafAuthDataV8Pckg *gbaNafAuthenticateDataPckg =
		  STATIC_CAST(RMobilePhone::TGbaNafAuthDataV8Pckg*,aSmartCardAuthData);

		RMobilePhone::TGbaNafAuthDataV8 &gbaNafAuthData=
		  (*gbaNafAuthenticateDataPckg)();
		if(gbaNafAuthData.iNafId != DMMTSY_PHONE_GBA_VALID_NAFID ||
				gbaNafAuthData.iImpi != DMMTSY_PHONE_GBA_VALID_IMPI)
			{
			iGbaNafAuthDataFail++;
			switch(iGbaNafAuthDataFail)
				{	
			case 1:
				ReqCompleted(aTsyReqHandle, KErrMMEtelAuthKeyNotFound);
				break;
				
			case 2:
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
				break;	
					
			case 3:
			default:
				ReqCompleted(aTsyReqHandle, KErrInUse);
				iGbaNafAuthDataFail=0;
				break;	
				}
			}
		else
			{
			gbaNafAuthData.iKsExtNaf=DMMTSY_PHONE_GBA_KS_EXTNAF;
			AddDelayedReq(aTsyReqHandle,this);
			}
		}
	else if(smartCardAuthenticateData.ExtensionId() == RMobilePhone::KETelMobileMbmsMskUpdateDataV8)
		{
		RMobilePhone::TMbmsMskUpdateDataV8Pckg *mbmsMskUpdateAuthenticateDataPckg =
		  STATIC_CAST(RMobilePhone::TMbmsMskUpdateDataV8Pckg*,aSmartCardAuthData);

		RMobilePhone::TMbmsMskUpdateDataV8 &mbmsMskUpdateAuthData=
		  (*mbmsMskUpdateAuthenticateDataPckg)();
		 if(mbmsMskUpdateAuthData.iInputMikey == DMMTSY_PHONE_MBMS_INVALID_IMIKEY)
			{
			iMbmsMskUpdateAuthDataFail++;
			switch(iMbmsMskUpdateAuthDataFail)
				{
			case 1:
				ReqCompleted(aTsyReqHandle, KErrMMEtelAuthKeyNotFound);
				break;
					
			case 2:
				ReqCompleted(aTsyReqHandle, KErrMMEtelAuthKeyInvalid);
				break;
					
			case 3:
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
				break;
					
			case 4:
			default:
				ReqCompleted(aTsyReqHandle, KErrInUse);
				iMbmsMskUpdateAuthDataFail=0;
				break;
				}
			} 
		else if(mbmsMskUpdateAuthData.iInputMikey == DMMTSY_PHONE_MBMS_VALID_IMIKEY_WO_VBIT)
			{
			AddDelayedReq(aTsyReqHandle,this);
			}
		else if (mbmsMskUpdateAuthData.iInputMikey == DMMTSY_PHONE_MBMS_VALID_IMIKEY_W_VBIT)
			{
			mbmsMskUpdateAuthData.iOutputMikey=DMMTSY_PHONE_MBMS_OMIKEY_FOR_W_VBIT;
			AddDelayedReq(aTsyReqHandle,this);
			}
		}
		
	else if(smartCardAuthenticateData.ExtensionId() == RMobilePhone::KETelMobileMbmsMtkGenerateDataV8)
		{
		RMobilePhone::TMbmsMtkGenerateDataV8Pckg *mbmsMtkGenerateAuthenticateDataPckg =
		  STATIC_CAST(RMobilePhone::TMbmsMtkGenerateDataV8Pckg*,aSmartCardAuthData);

		RMobilePhone::TMbmsMtkGenerateDataV8 &mbmsMtkGenerateAuthData=
		  (*mbmsMtkGenerateAuthenticateDataPckg)();
		  
		if(mbmsMtkGenerateAuthData.iMikey!= DMMTSY_PHONE_MBMS_VALID_IMIKEY)
			{
			iMbmsMtkGenerateAuthDataFail++;
			switch(iMbmsMtkGenerateAuthDataFail)
				{
			case 1:
				ReqCompleted(aTsyReqHandle, KErrMMEtelAuthKeyNotFound);
				break;
						
			case 2:
				ReqCompleted(aTsyReqHandle, KErrMMEtelAuthKeyInvalid);
				break;
						
			case 3:
				ReqCompleted(aTsyReqHandle, KErrMMEtelAuthMacInvalid);
				break;
						
			case 4:
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
				break;
						
			case 5:
			default:
				ReqCompleted(aTsyReqHandle, KErrInUse);
				iMbmsMtkGenerateAuthDataFail=0;
				break;
				}
			}
		else
			{
			mbmsMtkGenerateAuthData.iMtk = DMMTSY_PHONE_MBMS_VALID_MTK;
			mbmsMtkGenerateAuthData.iSaltKey = DMMTSY_PHONE_MBMS_VALID_SALTKEY;
			AddDelayedReq(aTsyReqHandle,this);
			}
		}
	else if(smartCardAuthenticateData.ExtensionId() == RMobilePhone::KETelMobileMbmsMskDeleteDataV8)
		{
		RMobilePhone::TMbmsMskDeleteDataV8Pckg *mbmsMskDeleteAuthenticateDataPckg =
		  STATIC_CAST(RMobilePhone::TMbmsMskDeleteDataV8Pckg*,aSmartCardAuthData);

		RMobilePhone::TMbmsMskDeleteDataV8 &mbmsMskDeleteAuthData=
		  (*mbmsMskDeleteAuthenticateDataPckg)();
		  
		if((mbmsMskDeleteAuthData.iKeyDomainId!= DMMTSY_PHONE_MBMS_VALID_KEYDMN) ||
		   (mbmsMskDeleteAuthData.iMskIdKeyGroup != DMMTSY_PHONE_MBMS_VALID_MSKIDGRP))	
			{
			iMbmsMskDeleteAuthDataFail++;
			switch(iMbmsMskDeleteAuthDataFail)
				{
			case 1:
				ReqCompleted(aTsyReqHandle, KErrNotFound);
				break;
					
			case 2:
			default:
				ReqCompleted(aTsyReqHandle, KErrInUse);
				iMbmsMskDeleteAuthDataFail=0;
				break;
				}	
			}
		else
			{
			AddDelayedReq(aTsyReqHandle,this);	
			}
		}
	else if(smartCardAuthenticateData.ExtensionId() == RMobilePhone::KETelMobileMbmsMukDeleteDataV8)
		{
		RMobilePhone::TMbmsMukDeleteDataV8Pckg *mbmsMukDeleteAuthenticateDataPckg =
		  STATIC_CAST(RMobilePhone::TMbmsMukDeleteDataV8Pckg*,aSmartCardAuthData);

		RMobilePhone::TMbmsMukDeleteDataV8 &mbmsMukDeleteAuthData=
		  (*mbmsMukDeleteAuthenticateDataPckg)();
		  
		if(mbmsMukDeleteAuthData.iMukIdTlv!= DMMTSY_PHONE_MBMS_VALID_MUKID) 
			{
			iMbmsMukDeleteAuthDataFail++;
			switch(iMbmsMukDeleteAuthDataFail)
				{
			case 1:
				ReqCompleted(aTsyReqHandle, KErrNotFound);
				break;
					
			case 2:
			default:
				ReqCompleted(aTsyReqHandle, KErrInUse);
				iMbmsMukDeleteAuthDataFail=0;
				break;
				}	
			}
		else
			{
			AddDelayedReq(aTsyReqHandle,this);	
			}
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetSmartCardAuthenticationDataCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetSmartCardAuthenticationDataCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle, KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::SetEmergencyClient(const TTsyReqHandle aTsyReqHandle, TUint32* aEmergencyRequest)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetEmergencyClient called"));
	
	switch (*aEmergencyRequest)
		{
	case RPhone::EEmergencyLCSRequest:
	case RPhone::EEmergencyCSVoiceCallRequest:
	case RPhone::EEmergencyLCSRequest | RPhone::EEmergencyCSVoiceCallRequest:
		ReqCompleted(aTsyReqHandle, KErrNone);
		break;
	default:
		return KErrCorrupt;
		}
	
	return KErrNone;
	}

//
// First Ordinal Functions
//
extern "C"
	{
	IMPORT_C CPhoneFactoryBase* LibEntry();	// Force "Proper Name" export
	}

EXPORT_C CPhoneFactoryBase* LibEntry()
	{
	CPhoneFactoryDMmTsy* factory = NULL;
	TRAP_IGNORE(factory = CPhoneFactoryDMmTsy::NewL());
	return factory;
	}

//
// DMmTsy Phone Factory Functions
//

CPhoneFactoryDMmTsy* CPhoneFactoryDMmTsy::NewL()
	{
	CPhoneFactoryDMmTsy *This = new (ELeave) CPhoneFactoryDMmTsy;
	CleanupStack::PushL(This);
	This->ConstructL();
	CleanupStack::Pop();
	return This;
	}

CPhoneFactoryDMmTsy::CPhoneFactoryDMmTsy()
	{
	iVersion=TVersion(	KTsyEtelMajorVersionNumber,
						KTsyEtelMinorVersionNumber,
						KTsyEtelBuildVersionNumber);
	}

CPhoneFactoryDMmTsy::~CPhoneFactoryDMmTsy()
	{
	LOGDESTROY();
	}

CPhoneBase * CPhoneFactoryDMmTsy::NewPhoneL(const TDesC& aName)
	{
	if (aName.Compare(DMMTSY_PHONE_NAME)==KErrNone)
		return CPhoneDMmTsy::NewL(this);
	else if (aName.Compare(DMMTSY_PHONE_NAMEAUTH1)==KErrNone)
		{
		CPhoneDMmTsy* phone=CPhoneDMmTsy::NewL(this);
		CleanupStack::PushL(phone);
		// it van be initialised in different ways by using name of the phone -
		// that's why different names of phones shall be used
		phone->iAuthInfo->iIMPI=DMMTSY_PHONE1_IMPI;
		phone->iAuthInfo->iIMPUArray.Append(DMMTSY_PHONE1_IMPU0);
		phone->iAuthInfo->iHomeNetworkDomainName=DMMTSY_PHONE1_HNDN;
		phone->iAuthInfo->iAuthenticationDataSource=DMMTSY_PHONE1_DATASOURCE;
		CleanupStack::Pop(phone);
		return phone;
		}
	else if (aName.Compare(DMMTSY_PHONE_NAMEAUTH2)==KErrNone)
		{
		CPhoneDMmTsy* phone=CPhoneDMmTsy::NewL(this);
		CleanupStack::PushL(phone);
		// it van be initialised in different ways by using name of the phone -
		// that's why different names of phones shall be used
		phone->iAuthInfo->iIMPI=DMMTSY_PHONE2_IMPI;
		phone->iAuthInfo->iIMPUArray.Append(DMMTSY_PHONE2_IMPU1);
		phone->iAuthInfo->iIMPUArray.Append(DMMTSY_PHONE2_IMPU2);
		phone->iAuthInfo->iIMPUArray.Append(DMMTSY_PHONE2_IMPU3);
		phone->iAuthInfo->iIMPUArray.Append(DMMTSY_PHONE2_IMPU4);
		phone->iAuthInfo->iHomeNetworkDomainName=DMMTSY_PHONE2_HNDN;
		phone->iAuthInfo->iAuthenticationDataSource=DMMTSY_PHONE2_DATASOURCE;
		CleanupStack::Pop(phone);
		return phone;
		}
	else if (aName.Compare(DMMTSY_PHONE_NAMEAUTH3)==KErrNone)
		{
		CPhoneDMmTsy* phone=CPhoneDMmTsy::NewL(this);
		CleanupStack::PushL(phone);
		// it van be initialised in different ways by using name of the phone -
		// that's why different names of phones shall be used
		phone->iAuthInfo->iIMPI=DMMTSY_PHONE3_IMPI;
		phone->iAuthInfo->iHomeNetworkDomainName=DMMTSY_PHONE3_HNDN;
		phone->iAuthInfo->iAuthenticationDataSource=DMMTSY_PHONE3_DATASOURCE;
		CleanupStack::Pop(phone);
		return phone;
		}
	return NULL;
	}

TBool CPhoneFactoryDMmTsy::IsSupported(const TInt /*aMixin*/)
	{
	return EFalse;
	}

//
// methods for CActiveListNode
//

CPhoneDMmTsy::CActiveListNode::CActiveListNode(CActive *aActive, const TTsyReqHandle aTsyReqHandle) :
	iActive(aActive), iTsyReqHandle(aTsyReqHandle)
	{

	}

CPhoneDMmTsy::CActiveListNode::~CActiveListNode()
	{
	delete iActive;
	}

//
// methods for the list of aos which corresspond to the outstanding delayed asyncrhonous reqs
//

TInt CPhoneDMmTsy::FindDelayedReq(const TTsyReqHandle aTsyReqHandle, CActiveListNode *& aNode)
	{
	TInt err = KErrNotFound;
	TSglQueIter<CActiveListNode> iter(iActiveRequestObjects);
	iter.SetToFirst();

	CActiveListNode *node = aNode = NULL;
	while ((node = iter++) != NULL)
		if (node->iTsyReqHandle == aTsyReqHandle)
			break;

	if (node != NULL)
		{
		aNode = node;
		err = KErrNone;
		}

	return err;
	}

TInt CPhoneDMmTsy::RemoveDelayedReq(const TTsyReqHandle aTsyReqHandle)
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

void CPhoneDMmTsy::RegisterDelayedReqL(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject)
	{
	CDelayedCompleter *delayed = CDelayedCompleter::NewL(aTsyReqHandle,this,aTelObject);
	CleanupStack::PushL(delayed);
	CActiveListNode *newNode = new (ELeave) CActiveListNode(delayed,aTsyReqHandle);
	iActiveRequestObjects.AddLast(*newNode);
	delayed->After(100000);
	CleanupStack::Pop(); // delayed
	}
void CPhoneDMmTsy::RegisterDelayedReqL(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject,TInt aPriority)
	{
	CDelayedCompleter *delayed = CDelayedCompleter::NewL(aTsyReqHandle,this,aTelObject);
	CleanupStack::PushL(delayed);
	CActiveListNode *newNode = new (ELeave) CActiveListNode(delayed,aTsyReqHandle);
	iActiveRequestObjects.AddLast(*newNode);
	delayed->After(aPriority);
	CleanupStack::Pop(); // delayed
	}
void CPhoneDMmTsy::AddDelayedReq(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject)
	{
	TRAPD(err, RegisterDelayedReqL(aTsyReqHandle, aTelObject));

	if (err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		}
	}
void CPhoneDMmTsy::AddDelayedReq(const TTsyReqHandle aTsyReqHandle, CTelObject *aTelObject,TInt aPriority)
	{
	TRAPD(err, RegisterDelayedReqL(aTsyReqHandle, aTelObject, aPriority));


	if (err != KErrNone)
		{
		ReqCompleted(aTsyReqHandle, err);
		}
	}

TInt CPhoneDMmTsy::NotifyAuthenticateDataChange(const TTsyReqHandle aTsyReqHandle,TDes8* aAuthInfo)
	{
	if(!iNotifyAuthenticationDataChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyAuthenticationDataChange called"));
		RMobilePhone::TAuthInfoV8Pckg *authInfoV8Pckg = STATIC_CAST(RMobilePhone::TAuthInfoV8Pckg*,aAuthInfo);
		RMobilePhone::TAuthInfoV8 &authInfoV8 = (*authInfoV8Pckg)();
		authInfoV8.iAID = DMMTSY_PHONE_GBA_AID;
		authInfoV8.iData = DMMTSY_PHONE_AUTH_DATA_GBA_PARAMS;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyAuthenticateDataChangeCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyAuthenticationDataChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetGbaBootstrapParams(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TAID* aAID,TDes8* aParams)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetGbaBootstrapParams called"));
	RMobilePhone::TBsfDataV8Pckg *bsfDataV8Pckg = STATIC_CAST(RMobilePhone::TBsfDataV8Pckg*,aParams);
	RMobilePhone::TBsfDataV8 &bsfDataV8 = (*bsfDataV8Pckg)();
	if((bsfDataV8.iBtid != DMMTSY_PHONE_BSF_VALID_BTID) ||
		(bsfDataV8.iKeyLifetime != DMMTSY_PHONE_BSF_VALID_KEYLIFETIME) ||
		(*aAID != DMMTSY_PHONE_GBA_AID )) 
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;	
	}

TInt CPhoneDMmTsy::SetGbaBootstrapParamsCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetGbaBootstrapParamsCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}	

TInt CPhoneDMmTsy::GetAuthenticationParams(const TTsyReqHandle aTsyReqHandle,TDes8* aAuthInfo,TDes8* aParams)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAuthenticationParams called"));
	RMobilePhone::TAuthInfoV8Pckg *authInfoV8Pckg = STATIC_CAST(RMobilePhone::TAuthInfoV8Pckg*,aAuthInfo);
	RMobilePhone::TAuthInfoV8 &authInfoV8 = (*authInfoV8Pckg)();
	if(authInfoV8.iData == DMMTSY_PHONE_AUTH_DATA_GBA_PARAMS && authInfoV8.iAID == DMMTSY_PHONE_GBA_AID)
		{
		RMobilePhone::TGbaBootstrapParamsV8Pckg *gbaBootstrapV8Pckg = STATIC_CAST(RMobilePhone::TGbaBootstrapParamsV8Pckg*,aParams);
		RMobilePhone::TGbaBootstrapParamsV8 &gbaBootstrapV8 = (*gbaBootstrapV8Pckg)();
		gbaBootstrapV8.iRand = DMMTSY_PHONE_GBA_RAND_IN;
		gbaBootstrapV8.iBtid = DMMTSY_PHONE_BSF_VALID_BTID;
		gbaBootstrapV8.iKeyLifetime = DMMTSY_PHONE_BSF_VALID_KEYLIFETIME;
		AddDelayedReq(aTsyReqHandle,this);
		}
	else if(authInfoV8.iData == DMMTSY_PHONE_AUTH_DATA_MBMS_MUK && authInfoV8.iAID == DMMTSY_PHONE_MBMS_AID)
		{
		RMobilePhone::TMbmsUserKeyV8Pckg *mbmsUserKeyV8Pckg = STATIC_CAST(RMobilePhone::TMbmsUserKeyV8Pckg*,aParams);
		RMobilePhone::TMbmsUserKeyV8 &mbmsUserKeyV8 = (*mbmsUserKeyV8Pckg)();
		mbmsUserKeyV8.iMukIdi = DMMTSY_PHONE_MBMS_VALID_MUK_IDI;
		mbmsUserKeyV8.iMukIdr = DMMTSY_PHONE_MBMS_VALID_MUK_IDR;
		mbmsUserKeyV8.iTimeStampCounter = DMMTSY_PHONE_MBMS_VALID_MUK_ITIMESTM;
		AddDelayedReq(aTsyReqHandle,this);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetAuthenticationParamsCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAuthenticationParamsCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetAuthenticationListPhase1(const TTsyReqHandle aTsyReqHandle,CRetrieveMobilePhoneAuthenticationIds::TAuthRequestData* aRequest,TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetAuthenticationListPhase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	
	if (((aRequest->iAuthInfo.iData) == DMMTSY_PHONE_AUTH_DATA_NAF_LIST) ||
		((aRequest->iAuthInfo.iData) == DMMTSY_PHONE_AUTH_DATA_MBMS_MSK))
		{
		leaveCode = KErrNone;
		}
	else
	 	{
	 	leaveCode = KErrCorrupt;	
	 	}  

	if (leaveCode == KErrNone)
		{
		if (aRequest->iAuthInfo.iData==DMMTSY_PHONE_AUTH_DATA_NAF_LIST)
			{
			TRAP(leaveCode, ret=ProcessGetGbaPhase1L(aTsyReqHandle, &aRequest->iClient,aBufSize););
			}
		else if(aRequest->iAuthInfo.iData==DMMTSY_PHONE_AUTH_DATA_MBMS_MSK)
			{
			TRAP(leaveCode, ret=ProcessGetMbmsPhase1L(aTsyReqHandle, &aRequest->iClient,aBufSize););
			}
		}

	if (leaveCode != KErrNone)
		{
		ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	};

TInt CPhoneDMmTsy::ProcessGetGbaPhase1L(const TTsyReqHandle aTsyReqHandle,
											   RMobilePhone::TClientId* aId,TInt* aBufSize)
	{
	// read the list, store its content and then return size of this buffer to client
	CMobilePhoneGbaNafIdList* list=CMobilePhoneGbaNafIdList::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TGbaNafEntryV8 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_PHONE_GBA_NAFLIST_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iNafId = DMMTSY_PHONE_GBA_NAFLIST_NAFID_ONE;
			entry.iBtid = DMMTSY_PHONE_GBA_NAFLIST_BTID_ONE;
			break;
		case 1:
			entry.iNafId = DMMTSY_PHONE_GBA_NAFLIST_NAFID_TWO;
			entry.iBtid = DMMTSY_PHONE_GBA_NAFLIST_BTID_TWO;
			break;
		case 2:
			entry.iNafId = DMMTSY_PHONE_GBA_NAFLIST_NAFID_THREE;
			entry.iBtid = DMMTSY_PHONE_GBA_NAFLIST_BTID_THREE;
			break;
		case 3:
		default:
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}
	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aId, aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iAuthReadAll->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt

	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();

	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CPhoneDMmTsy::ProcessGetMbmsPhase1L(const TTsyReqHandle aTsyReqHandle,
											   RMobilePhone::TClientId* aId,TInt* aBufSize)
	{
	// read the list, store its content and then return size of this buffer to client
	CMobilePhoneMbmsMskIdList* list=CMobilePhoneMbmsMskIdList::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TMskEntryV8 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_PHONE_MBMS_LIST_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iKeyDomainId = DMMTSY_PHONE_MBMSLIST_KEYDMN_ONE;
			entry.iMskId = DMMTSY_PHONE_MBMSLIST_MSKID_ONE;
			entry.iTimeStampCounter = DMMTSY_PHONE_MBMSLIST_TIMESTM_ONE;
			break;
		case 1:
			entry.iKeyDomainId = DMMTSY_PHONE_MBMSLIST_KEYDMN_TWO;
			entry.iMskId = DMMTSY_PHONE_MBMSLIST_MSKID_TWO;
			entry.iTimeStampCounter = DMMTSY_PHONE_MBMSLIST_TIMESTM_TWO;
			break;
		case 2:
			entry.iKeyDomainId = DMMTSY_PHONE_MBMSLIST_KEYDMN_THREE;
			entry.iMskId = DMMTSY_PHONE_MBMSLIST_MSKID_THREE;
			entry.iTimeStampCounter = DMMTSY_PHONE_MBMSLIST_TIMESTM_THREE;
			break;
		case 3:
		default:
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}
	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aId, aTsyReqHandle);
	CleanupStack::PushL(read);

	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC

	iAuthReadAll->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt

	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();

	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetAuthenticationListPhase2(TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ReadAllPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the read attempt from this client
	for (TInt i=0; i<iAuthReadAll->Count(); ++i)
		{
		read = iAuthReadAll->At(i);
		if ((read->iClient.iSessionHandle==aId->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aId->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuffer->Copy(bufPtr);
			delete read;
			iAuthReadAll->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}
	
TInt CPhoneDMmTsy::GetAuthenticationListCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::ReadAllCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	// Remove the read all attempt from iAuthReadAll
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iAuthReadAll->Count(); ++i)
		{
		read = iAuthReadAll->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iAuthReadAll->Delete(i);
			break;
			}
		}

	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

//
// Wlan
//
TInt CPhoneDMmTsy::NotifyWlanDataChange(const TTsyReqHandle aTsyReqHandle,TDes8* aData)
	{
	if(!iNotifyWlanDataChange++)
		{
		LOGTEXT(_L8("CPhoneDMmTsy::NotifyWlanDataChange called"));
		RMobilePhone::TUsimWlanDataV8Pckg *WlanDataV8Pckg = STATIC_CAST(RMobilePhone::TUsimWlanDataV8Pckg*,aData);
		RMobilePhone::TUsimWlanDataV8 &WlanDataV8 = (*WlanDataV8Pckg)();
		WlanDataV8.iFieldsUsed = DMMTSY_PHONE_WLAN_LIST_DATA_FLAGS;
		WlanDataV8.iPseudonym = DMMTSY_PHONE_WLAN_VALID_PSEUDONYM;
		WlanDataV8.iReauthenticationId = DMMTSY_PHONE_WLAN_VALID_REAUTHID;
		WlanDataV8.iMasterKey = DMMTSY_PHONE_WLAN_VALID_MASTERKEY;
		WlanDataV8.iCounter = DMMTSY_PHONE_WLAN_VALID_COUNTER;
		AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CPhoneDMmTsy::NotifyWlanDataChangeCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyWlanDataChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::SetWlanData(const TTsyReqHandle aTsyReqHandle,TDes8* aData)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetWlanData called"));
	RMobilePhone::TUsimWlanDataV8Pckg *WlanDataV8Pckg = STATIC_CAST(RMobilePhone::TUsimWlanDataV8Pckg*,aData);
	RMobilePhone::TUsimWlanDataV8 &WlanDataV8 = (*WlanDataV8Pckg)();
	if((WlanDataV8.iFieldsUsed != DMMTSY_PHONE_WLAN_LIST_DATA_FLAGS) ||
	(WlanDataV8.iPseudonym != DMMTSY_PHONE_WLAN_VALID_PSEUDONYM) ||
	(WlanDataV8.iReauthenticationId != DMMTSY_PHONE_WLAN_VALID_REAUTHID) ||
	(WlanDataV8.iMasterKey != DMMTSY_PHONE_WLAN_VALID_MASTERKEY) ||
	(WlanDataV8.iCounter != DMMTSY_PHONE_WLAN_VALID_COUNTER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		AddDelayedReq(aTsyReqHandle,this);
		}
		
	return KErrNone;	
	}

TInt CPhoneDMmTsy::SetWlanDataCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::SetWlanDataCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}	

TInt CPhoneDMmTsy::GetWlanData(const TTsyReqHandle aTsyReqHandle,TDes8* aData)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetWlanData called"));
	RMobilePhone::TUsimWlanDataV8Pckg *WlanDataV8Pckg = STATIC_CAST(RMobilePhone::TUsimWlanDataV8Pckg*,aData);
	RMobilePhone::TUsimWlanDataV8 &WlanDataV8 = (*WlanDataV8Pckg)();
	WlanDataV8.iFieldsUsed = DMMTSY_PHONE_WLAN_LIST_DATA_FLAGS;
	WlanDataV8.iPseudonym = DMMTSY_PHONE_WLAN_VALID_PSEUDONYM;
	WlanDataV8.iReauthenticationId = DMMTSY_PHONE_WLAN_VALID_REAUTHID;
	WlanDataV8.iMasterKey = DMMTSY_PHONE_WLAN_VALID_MASTERKEY;
	WlanDataV8.iCounter = DMMTSY_PHONE_WLAN_VALID_COUNTER;
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;	
	}

TInt CPhoneDMmTsy::GetWlanDataCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetWlanDataCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::StorePreferredWlanSIDList(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::StorePreferredWlanSIDList called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessStorePreferredWlanSIDListL(aTsyReqHandle, aBuffer););
	if (leaveCode != KErrNone)
		{
		ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	};

TInt CPhoneDMmTsy::ProcessStorePreferredWlanSIDListL(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	CMobilePhoneStoredWlanSIDList* list=CMobilePhoneStoredWlanSIDList::NewL();
	CleanupStack::PushL(list);
	
	list->RestoreL(*aBuffer);

	RMobilePhone::TWlanSIDV8 entry;

	if (list->Enumerate() != DMMTSY_PHONE_WLAN_SIDLIST_COUNT)
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		CleanupStack::PopAndDestroy(); // list
		return KErrNone;
		}

	TInt error=KErrNone;
	TInt ret;
	for (TInt i=0; i<DMMTSY_PHONE_WLAN_SIDLIST_COUNT; ++i)
		{
		TRAP(ret,entry=list->GetEntryL(i));
		if (ret != KErrNone)
			break;
		switch(i)
			{
		case 0:
			{
			if(entry.iWSID != DMMTSY_PHONE_WLAN_WSID_ONE ||
			   entry.iUserDefined != DMMTSY_PHONE_WLAN_USERDEFD_ONE)
				{
				error  = KErrCorrupt;
				}
			}
			break;
		case 1:
			{
			if(entry.iWSID != DMMTSY_PHONE_WLAN_WSID_TWO ||
			   entry.iUserDefined != DMMTSY_PHONE_WLAN_USERDEFD_TWO)
				{
				error  = KErrCorrupt;
				}
			}
			break;
		case 2:
		default:
			{
			if(entry.iWSID != DMMTSY_PHONE_WLAN_WSID_THREE ||
			   entry.iUserDefined != DMMTSY_PHONE_WLAN_USERDEFD_THREE)
				{
				error  = KErrCorrupt;
				}
			}
			break;
			}
		}
	if(error==KErrNone)	
		{
		AddDelayedReq(aTsyReqHandle,this);	
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	CleanupStack::PopAndDestroy(); 
	return KErrNone;
	}

TInt CPhoneDMmTsy::StorePreferredWlanSIDListCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::StorePreferredWlanSIDListCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyPreferredWlanSIDListChange(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyPreferredWlanSIDListChange called"));
	AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}
	
TInt CPhoneDMmTsy::NotifyPreferredWlanSIDListChangeCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::NotifyPreferredWlanSIDListChangeCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetPreferredWlanSIDsPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPreferredWlanSIDsPhase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetPreferredWlanSIDsPhase1L(aTsyReqHandle, aClient, aBufSize););
	if (leaveCode != KErrNone)
		{
		ReqCompleted(aTsyReqHandle,leaveCode);
		}
	return ret;
	}	
	
TInt CPhoneDMmTsy::ProcessGetPreferredWlanSIDsPhase1L(const TTsyReqHandle aTsyReqHandle, 
													 RMobilePhone::TClientId* aClient, 
													 TInt* aBufSize)
	{
	// retrieve stored Wlan specific IDs from USIM,
	// store each entry.
	// stream the list and then return size of this buffer to client
	CMobilePhoneStoredWlanSIDList* list=CMobilePhoneStoredWlanSIDList::NewL();
	CleanupStack::PushL(list);

	RMobilePhone::TWlanSIDV8 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_PHONE_WLAN_SIDLIST_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iWSID = DMMTSY_PHONE_WLAN_WSID_ONE;
		    entry.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_ONE;
			break;
			
		case 1:
			entry.iWSID = DMMTSY_PHONE_WLAN_WSID_TWO;
		    entry.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_TWO;
			break;
		
		case 2:
		default:
			entry.iWSID = DMMTSY_PHONE_WLAN_WSID_THREE;
		    entry.iUserDefined = DMMTSY_PHONE_WLAN_USERDEFD_THREE;
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
	
	iGetWlanSIDsData->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CPhoneDMmTsy::GetPreferredWlanSIDsPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPreferredNetworksPhase2 called"));
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
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}
	
TInt CPhoneDMmTsy::GetPreferredWlanSIDsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CPhoneDMmTsy::GetPreferredWlanSIDsCancel called"));
	RemoveDelayedReq(aTsyReqHandle);
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
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

/*******************************************************************/
//
// CDelayedCompleter
//
/*******************************************************************/



CDelayedCompleter::CDelayedCompleter(const TTsyReqHandle aTsyReqHandle, CPhoneDMmTsy *aPhoneObject,
									CTelObject *aTelObject) :
		CTimer(EPriorityStandard),iTsyReqHandle(aTsyReqHandle), iPhoneObject(aPhoneObject),
			iTelObject(aTelObject)
	{

	}

CDelayedCompleter* CDelayedCompleter::NewL(const TTsyReqHandle aTsyReqHandle, CPhoneDMmTsy *aPhoneObject,
									CTelObject *aTelObject)
	{
	CDelayedCompleter* This = new (ELeave) CDelayedCompleter(aTsyReqHandle,aPhoneObject,aTelObject);
	CleanupStack::PushL(This);
	This->ConstructL();
	CleanupStack::Pop();
	return This;
	}

void CDelayedCompleter::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CDelayedCompleter::RunL()
	{
	iTelObject->ReqCompleted(iTsyReqHandle,KErrNone);
	(void) iPhoneObject->RemoveDelayedReq(iTsyReqHandle); // we get deleted here
	}
