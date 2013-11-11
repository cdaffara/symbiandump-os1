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
// This file contains the definitions of the Simulator TSY USIM Release6 class.
// 
//

/**
 @file
 @internalAll
*/

#ifndef CSIMUSIMR6_H_
#define CSIMUSIMR6_H_
#include <etelmm.h>
#include <et_phone.h>
#include "csimtimer.h"
#include "CSimPubSubChange.h"

class CSimPhone;
class CTestConfigSection;

/**
 * Implements the RMobilePhone based functions that constitute the 3GPP Release6
 * functionality provided by the SIM TSY.
 */
class CSimUsimR6 : public CBase, MTimerCallBack
	{
public:
	static CSimUsimR6* NewL(CSimPhone* aPhone);
	CSimUsimR6(CSimPhone* aPhone);
	~CSimUsimR6();
	
	/** Videomail related APIS */
	TInt GetMailboxNumbers(TTsyReqHandle aReqHandle, TDes8* aMailBox);
	TInt GetMailboxNumbersCancel(TTsyReqHandle aReqHandle);
	
	TInt NotifyMailboxNumbersChange(TTsyReqHandle aReqHandle, TDes8* aMailBox);
	TInt NotifyMailboxNumbersChangeCancel(TTsyReqHandle aReqHandle);
	
	TInt GetIccMessageWaitingIndicators(TTsyReqHandle aReqHandle, TDes8* aMsgIndicators);
	TInt GetIccMessageWaitingIndicatorsCancel(TTsyReqHandle aReqHandle);
	
	TInt SetIccMessageWaitingIndicators(TTsyReqHandle aReqHandle,TDes8* aMsgIndicators);
	TInt SetIccMessageWaitingIndicatorsCancel(TTsyReqHandle aReqHandle);
	
	TInt NotifyIccMessageWaitingIndicatorsChange(TTsyReqHandle aReqHandle, TDes8* aMsgIndicators);
	TInt NotifyIccMessageWaitingIndicatorsChangeCancel(TTsyReqHandle aReqHandle);
	
	TInt NotifyMessageWaiting(TTsyReqHandle aReqStatus, TInt* aCount);
	TInt NotifyMessageWaitingCancel(TTsyReqHandle aReqHandle);
	
	/** Preferred Networks */	

	TInt GetPreferredNetworksPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt ProcessGetPreferredNetworksPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt GetPreferredNetworksPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetPreferredNetworksCancel(const TTsyReqHandle aTsyReqHandle);

	TInt ProcessStorePreferredNetworksListL(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);
	TInt StorePreferredNetworksList(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer);

	TInt NotifyStorePreferredNetworksListChange(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyStorePreferredNetworksListChangeCancel(const TTsyReqHandle aTsyReqHandle);

	/** WLAN related APIS */
	TInt NotifyWlanDataChange(const TTsyReqHandle aTsyReqHandle, TDes8* aData);
	TInt NotifyWlanDataChangeCancel(TTsyReqHandle aTsyReqHandle);

	TInt NotifyPreferredWlanSIDListChange(TTsyReqHandle aTsyReqHandle);
	TInt NotifyPreferredWlanSIDListChangeCancel(TTsyReqHandle aTsyReqHandle);
	
	TInt GetWlanData(const TTsyReqHandle aTsyReqHandle,TDes8* aData);
	TInt SetWlanData(const TTsyReqHandle aTsyReqHandle,TDes8* aData);

	TInt GetPreferredWlanSIDsPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt ProcessGetPreferredWlanSIDsPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize);
	TInt GetPreferredWlanSIDsPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetPreferredWlanSIDsCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt StorePreferredWlanSIDList(const TTsyReqHandle aTsyReqHandle,TDes8* aBuffer);	
	TInt ProcessStorePreferredWlanSIDListL(TTsyReqHandle aTsyReqHandle,TDes8* aBuffer);
		
	/** GBA and MBMS related APIS */
	
	TInt SetGbaBootstrapParams(const TTsyReqHandle aTsyReqHandle,TDes8* aPckg1, const RMobilePhone::TAID* aAID);
	TInt SetGbaBootstrapParamsCancel(const TTsyReqHandle aTsyReqHandle);
		
	TInt NotifyAuthenticateDataChange(const TTsyReqHandle aReqstatus,TDes8* aAuthPckg);
	TInt NotifyAuthenticateDataChangeCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetAuthenticationParams(const TTsyReqHandle aTsyReqHandle,TDes8* aPckg1, TDes8* aPckg2);
	TInt GetAuthenticationParamsCancel(const TTsyReqHandle aTsyReqHandle);
	
	TInt GetAuthenticationListPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneAuthenticationIds::TAuthRequestData* aRequest,TInt* aBufSize);
	TInt ProcessGetGbaPhase1L(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TClientId* aClientId,TInt* aBufSize);
	TInt ProcessGetMbmsPhase1L(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TClientId* aClientId,TInt* aBufSize);
	TInt GetAuthenticationListPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId*,TDes8* aBuffer);
	TInt GetAuthenticationListCancel(const TTsyReqHandle aTsyReqHandle);
	
	const CTestConfigSection* CfgFile();

private:
	void ConstructL();
	void TimerCallBack(TInt aId);
	void TimerCallBackNotifyMailboxNumChg();
	void TimerCallBackMessageWaitingStat();
	void TimerCallBackIccMessageWaitingStat();
	TInt RandTime(); // Function to generate random Time to simulate asynchronous request handling.
	
private:
	TInt iGetIccMsgIdCurrentIndex,iSetIccMsgIdCurrentIndex,iNotifyPreferredNetworksChangeIndex;
	TInt iCurrentIndex,iNotifyWlanDataChangeIndex,iNotifyPreferredWlanSIDListChangeIndex,iIndex;
	
	
	CSimPhone* iPhone;			// < Pointer to the parent phone class.
	CSimTimer* iTimer;			//< Pointer to the Timer object for callback
	CSimTimer* iWlanTimer;  	//< Pointer to the TimerObject for Wlan
	CSimTimer* iNotifyAuthTimer;//< Pointer to the TimerObject for NotifyAuthenticationTimer.
	
	TInt iMailBoxIndex;  				// < Current index for the MailBox entries
	TInt iListIndex; 					// < Current index from the iGBAInfo array or iMBMSInfo array
	TInt iVideoMailIndex; 				// < Current index from the iVideoMailInfo array	
	TInt iCurrentNotifyMailboxIndex; 	//< Current index for the NotifyMailBox
	TInt iCurrentIccMessageWaitingIndex;//< Current index for the ICC MessageWaiting.
	TInt iCurrentMessageWaitingIndex; 	//< Current index for the MessageWaitingIndex.
			
	CArrayFixFlat<RMobilePhone::TMobilePhoneMessageWaitingV8>* iMessageWaiting;	//< Array containing the Message Waiting Indicators
	CArrayFixFlat<RMobilePhone::TMobilePhoneVoicemailIdsV8>* iVoiceMailIds;     //< Array containing MialBox Numbers	
	CArrayFixFlat<RMobilePhone::TUsimWlanDataV8>* iWlanData;					//< Array containing Wlan Data.
	CArrayFixFlat<RMobilePhone::TWlanSIDV8>* iWlanSid;							//< Array containing Wlan Sids
	CArrayFixFlat<RMobilePhone::TMobilePreferredNetworkEntryV3>* iPreferredNetworks; //< Array containing Preferred  Networks' list
	CArrayPtrFlat<CListReadAllAttempt>* iGetWlanSIDsData;					
	CArrayPtrFlat<CListReadAllAttempt>* iGetPreferredNetworks;
	
	TBool iMailBoxNumberChangePending;			// < Current Mail Box number request outstanding for V3 parameter class
	TTsyReqHandle iMailBoxNumberChangeNotificationReqHandle;	// < Mail Box number outstanding request handle  for V3 parameter class
	RMobilePhone::TMobilePhoneVoicemailIdsV3* iMailBoxNumberChange;	// < Mail Box number notification data pointer  for V3 parameter class
	
	TBool iMailBoxNumberChangePendingV8;			// < Current Mail Box number request outstanding for V8 parameter class
	TTsyReqHandle iMailBoxNumberChangeNotificationReqHandleV8;	// < Mail Box number outstanding request handle  for V8 parameter class
	RMobilePhone::TMobilePhoneVoicemailIdsV8* iMailBoxNumberChangeV8;	// < Mail Box number notification data pointer  for V8 parameter class
	
	TBool iMessageWaitingPending;			// < Message waiting  notification request outstanding for the class
	TTsyReqHandle iMessageWaitingNotificationReqHandle;	// < Message waiting outstanding request handle  for the class
	TInt* iMsgWaitingCount;	// < Message waiting  notification data pointer  for the class
	
	
	TBool iIccMessageWaitingPending;			// < Icc Message waiting  notification request outstanding for V1 parameter class
	TTsyReqHandle iIccMessageWaitingNotificationReqHandle;	// < Icc Message waiting  outstanding request handle  for V1 parameter class
	RMobilePhone::TMobilePhoneMessageWaitingV1* iIccMsgWaiting;	// < Icc Message waiting  notification data pointer  for V1 parameter class

	TBool iIccMessageWaitingPendingV8;			// < Icc Message waiting  notification request outstanding for V8 parameter class
	TTsyReqHandle iIccMessageWaitingNotificationReqHandleV8;	// < Icc Message waiting  outstanding request handle  for V8 parameter class
	RMobilePhone::TMobilePhoneMessageWaitingV1* iIccMsgWaitingV8;	// < Icc Message waiting  notification data pointer  for V8 parameter class

	TBool iNotifyWlanDataChangePending;			// < WLAN Data change information for V8 parameter class
	TTsyReqHandle iNotifyWlanDataChangeReqHandle;	// < WLAN Data change information request handle  for V8 parameter class
	RMobilePhone::TUsimWlanDataV8* iNotifyWlanDataChange; // <  WLAN Data change information
	
	TBool iStorePreferredWlanSIDListPending;			// < WLAN SID List information for V8 parameter class
	TTsyReqHandle iStorePreferredWlanSIDListeReqHandle;	// < WLAN SID Listinformation request handle  for V8 parameter class
	
	TBool iNotifyPreferredWlanSIDListChangePending;			// < WLAN SID Listinformation for V8 parameter class
	TTsyReqHandle iNotifyPreferredWlanSIDListReqHandle;	// < WLAN SID List information request handle  for V8 parameter class	
	
	TBool iNotifyPreferredNetworksChangePending;		// < NotifyPreferredNetwork change information
	TTsyReqHandle iNotifyPreferredNetworksReqHandle;   // < Request Handle for NotifyPreferredNetworks.
	
	struct TGBAAuthInfo
		{
		/** holds value of network challenge AUTN */
		TBuf8<RMobilePhone::KAutnLength> iAUTN;
		/** holds value of network challenge RAND */
		TBuf8<RMobilePhone::KRandLength> iRAND;
		/**
		 holds Application's ID, on which the authentication is to be
		 carried out.  Note, there is no checking for this AID's
		 existence, config file must simply match client's request data.
		 */
		TBuf8<RMobilePhone::KAIDSize> iAID;
		/** holds result RES of AUTHENTICATE command when it's successfull */
		TBuf8<RMobilePhone::KResLength> iRES;
		/** holds value of Authentication Token when  AUTHENTICATE fails */
		TBuf8<RMobilePhone::KAutsLength> iAUTS;
		/** holds value of session key NAFID */
		TBuf8<RMobilePhone::KNafIdLength> iNAFID;
		/** holds value of session key IMPI */
		TBuf8<RMobilePhone::KImpiLength> iIMPI;
		/** holds value of Ks_ext_Naf key */
		TBuf8<RMobilePhone::KKsExtNafLength> iKsExtNaf;
		/** holds the Btid information */
		TBuf8<RMobilePhone::KBtidLength> iBtid;
		/** holds the keylifetime information */
		TBuf8<RMobilePhone::KKeyLifetimeLength> iKeyLifeTime;
		/** holds whether the other application is busy or not */
		TBool iOtherAppBusy;
		/** holds whether the current application is active or not */
		TBool iAppActive;
		/** holds the error code that the AUTHENTICATE request should pass/fail with */
		TInt iAuthErr;
		};

	struct TMBMSInfo
		{
		/** holds the input Mikey from the Network */
		TBuf8<RMobilePhone::KMikeyLength> iInputMikey ;  
		/** holds the output Mikey containing the verification message */
		TBuf8<RMobilePhone::KMikeyLength> iOutputMikey;  
		/**holds the output parameter MBMS traffic key */
		TBuf8<RMobilePhone::KMtkLength> iMtk;  
		/** holds the output parameter random or pseudo random string used to 
		 * protect against some offline pre computation attacks on the 
		 * underlying security protocol. */
		TBuf8<RMobilePhone::KSaltLength> iSaltkey; 
		/** MSK is identified by its Key Domain ID and MSK ID 
		 * holds the Keydomain ID*/		
		TBuf8<RMobilePhone::KKeyDomainIdLength> iKeyDmn;  
		/** holds the Key group part */
		TBuf8<RMobilePhone::KKeyGroupLength> iMskIdGrp;  
		/** holds the MUK ID TLV contains MUK IDr and MUK IDi */
		TBuf8<RMobilePhone::KMukIdTlvLength> iMukIdTlv; 
		/** holds the IDi part of MBMS User Key */
		TBuf8<RMobilePhone::KMukIdiLength>  iMukIdi;  
		/** holds the IDr part of MBMS User Key */
		TBuf8<RMobilePhone::KMukIdrLength>  iMukIdr; 
		/** holds the timestamp for MIKEY reply protection in MSK delivery. */
		TBuf8<RMobilePhone::KTimeStampCounterLength> iMukTimestamp; 
		/**
		 holds Application's ID, on which the authentication is to be
		 carried out.  Note, there is no checking for this AID's
		 existence, config file must simply match client's request data.
		 */
		TBuf8<RMobilePhone::KAIDSize> iAID;
		/** holds value of Ks_Int_Naf key 
		 * The length used for External NAF is being used here also*/
		TBuf8<RMobilePhone::KKsExtNafLength> iKsIntNaf;
		/** holds whether the current application is active or not */
		TBool iAppActive;
		/** holds the error code that the AUTHENTICATE request should pass/fail with */
		TInt iAuthErr;
		};
		
	enum TTypeOfAuth
		{
		/** Indicates GBA Authentication in progress*/
		EGba =1,
		/** Indicates MBMS Authentication in progress*/
		EMbms
		};
		
	TInt iNotifyAuthDataIndex; // Current index of the array
	CArrayFixFlat<TGBAAuthInfo>* iGBAAuthInfoList; // < Array containing all the GBA config information needed
	CArrayFixFlat<TMBMSInfo>* iMBMSInfoList; // < Array containing all the MBMS config information needed
	
	TNotifyData iSetGBABootstrapParams; //  data struct to be used for SetGBABootstrapParams API.
	TNotifyData iNotifyAuthDataChange; // data struct to be used for NotifyAuthenticateChange API.
	TNotifyData iGetAuthParams; // data struct to be used for GetAuthenticationParams API.
	
	CArrayPtrFlat<CListReadAllAttempt>*  iAuthListInfo; // array to hold the GBA information
	CMobilePhoneGbaNafIdList* iGbaNafList; //pointer to specialised list class for GBA
	CArrayPtrFlat<CListReadAllAttempt>* iMbmsListInfo; // array to hold the MBMS information
	CMobilePhoneMbmsMskIdList* iMbmsMskList; //pointer to specialised list class for MBMS

	TTypeOfAuth	iAuthType; // type of authentication in 2 phase retrieval
	};
	
#endif /**CSIMUSIMR6_H_*/
