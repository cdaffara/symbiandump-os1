// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent 
*/

#ifndef _TE_MMPHONE_H_
#define _TE_MMPHONE_H_

#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"
#include "TE_mmcall.h"

class CTestPhone : public CEtelMMTestStepBase
	{
public:
	TInt iTestCount;
	CTestPhone() ;
	~CTestPhone(){} ;
	virtual enum TVerdict doTestStepL();
	/*static*/ void ValidatePhoneStoreInfo(const RMobilePhoneStore::TMobilePhoneStoreInfoV1 &aSyncStoreInfo, TInt aSlot);
	/*static*/ void ValidateCustomerServiceProfile(const RMobilePhone::TMobilePhoneCspFileV1 &aSyncCspFile);
	/*static*/ void ValidateServiceTable(const RMobilePhone::TMobilePhoneServiceTableV1 &aSyncServiceTable);
	/*static*/ void ValidateServiceTable(const RMobilePhone::TMobilePhoneServiceTableV2 &aSyncServiceTable);
	/*static*/ void ValidatePhoneId(const RMobilePhone::TMobilePhoneIdentityV1 &aSyncPhoneId);
	/*static*/ void ValidateHomeNetwork(const RMobilePhone::TMobilePhoneNetworkInfoV1 &aSyncHomeNetwork);
	/*static*/ void ValidateCurrentNetwork(const RMobilePhone::TMobilePhoneNetworkInfoV1 &aSyncCurrentNetwork,
									const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea, TBool aValidateArea = ETrue);

	/*static*/ void TestHSUPAMobileNetworkL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestTMobilePhoneNetworkInfoV8(TRequestStatus &aReqStatus);


	/*static*/ void ValidateCurrentNetworkforHSUPA(const RMobilePhone::TMobilePhoneNetworkInfoV8 &aSyncCurrentNetwork,
			const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea);

	/*static*/ void ValidateHomeNetworkforHSUPA(const RMobilePhone::TMobilePhoneNetworkInfoV8 &aSyncHomeNetwork);

	/*static*/ void ValidateNotifyCurrentNetworkChangeforHSUPA(const RMobilePhone::TMobilePhoneNetworkInfoV8 &aSyncCurrentNetwork,const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea);
	/*static*/ void TestCRetrievePhoneDetectedNetworksL(RMobilePhone& aPhone,TRequestStatus &aReqStatus);

	/*static*/ void ValidateNotifyCurrentNetworkChange(const RMobilePhone::TMobilePhoneNetworkInfoV1 &aSyncCurrentNetwork , const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea, TBool aValidateArea = ETrue);

	/*static*/ void ValidateCurrentNetworkISV(const RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1 &aSyncCurrentNetworkISV,
									const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea);
	/*static*/ void ValidateNotifyCurrentNetworkISVChange(const RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1 &aSyncCurrentNetworkISV,
									const RMobilePhone::TMobilePhoneLocationAreaV1 &aSyncLocArea);
	/*static*/ void ValidateNITZInfo(const RMobilePhone::TMobilePhoneNITZ &aNITZInfo);
	/*static*/ void ValidateCostInfo(const RMobilePhone::TMobilePhoneCostInfoV1 &aInfo);
	/*static*/ void ValidateMulticallParams(const RMobilePhone::TMobilePhoneMulticallSettingsV1 &aSettings, TInt aCount);
	/*static*/ void ValidateSPName(const RMobilePhone::TMobilePhoneServiceProviderNameV2 &aName);

	/*static*/ void TestCapabilityL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestSimAccessL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobilePowerL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileSignalL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileIndicatorL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileIdentityL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileDTMFL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileNetworkL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileNetworkISVL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileCallServiceCapsL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobilePrivacyL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileUserNetworkL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileCallForwardingL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileIdentityServiceL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileCallBarringL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileCallWaitingL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileCallCompletionL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileAlternatingCallL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileAlternateLineServiceL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileCallCostL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileSecurityL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileMessageWaitingL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileFixedDiallingNumbersL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileMulticallSettings(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileIncomingCallTypes(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestMobileUUSSetting(RMobilePhone& aPhone, TRequestStatus &aReqStatus);	
	/*static*/ void TestMultimediaCallSettings(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestNetworkSecurity(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
	/*static*/ void TestUSimHandling(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestMobileMailboxNumbersFunctionality(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestMobileMmsFunctionality(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestMobileAPNControlList(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestMobileAPNControlListServiceStatus(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestMobileAirTime(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestTerminateAllCalls(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestTerminateActiveCalls(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestMobileAutoRedial(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestMobilePersonalisation(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestSmartCardHandling(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestGBAAuthenticationL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestMBMSAuthenticationL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestWlanParametersL(RMobilePhone& aPhone, TRequestStatus &aReqStatus);
		   void TestMobileCellInfoL(RMobilePhone& aPhone, TRequestStatus& aReqStatus);

/*private:*/
		class CTestGetDetectedNetworks : public CActive
			{
		public:
			enum TTestVersion
				{
				ETestV1,
				ETestV2,
				ETestV5,
				ETestV8
				};
			
			static CTestGetDetectedNetworks* NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			~CTestGetDetectedNetworks();
			void SetTestVersion(TTestVersion aVersion);
			void TestRetrieve();
			void TestCancel();
		protected:
			CTestGetDetectedNetworks(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			void ConstructL();
			void RunL();
			void DoCancel();
			void Start();
			CTestCanceller* RetrieveCancelPtr();
			TInt CheckResults();
			TInt CheckResultsV2();
			TInt CheckResultsV5();
			TInt CheckResultsV8();
        protected:
			CRetrieveMobilePhoneDetectedNetworks* iRetrieve;
			CTestCanceller* iCanceller;
			RMobilePhone& iPhone;
			CTestPhone* iTestPhone;
		private:
			TTestVersion iTestVersion;		//Used to determine whether to run the test for V1,
											//V2 or V5 list class and retrieval method.
			};

		class CTestGetCFStatus : public CActive
			{
		public:
			static CTestGetCFStatus* NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			~CTestGetCFStatus();
			void Start();
			void StartSingleServiceGroup();
			CTestCanceller* RetrieveCancelPtr();
		protected:
			CTestGetCFStatus(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			void ConstructL();
			void RunL();
			void DoCancel();
			TInt CheckResults();
			TInt CheckSingleServiceGroupResults();
		protected:
			CRetrieveMobilePhoneCFList* iRetrieve;
			CTestCanceller* iCanceller;
			RMobilePhone& iPhone;
			CTestPhone* iTestPhone;
			TBool iSingleServiceGroupTest;
			};

		class CTestGetCB : public CActive
			{
		public:
			static CTestGetCB* NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			~CTestGetCB();
			void Start();
			CTestCanceller* RetrieveCancelPtr();
		protected:
			CTestGetCB(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			void ConstructL();
			void RunL();
			void DoCancel();
			TInt CheckResults();
		protected:
			CRetrieveMobilePhoneCBList* iRetrieve;
			CTestCanceller* iCanceller;
			RMobilePhone& iPhone;
			CTestPhone* iTestPhone;
			};
		

		class CTestGetCW : public CActive
			{
		public:
			static CTestGetCW* NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			~CTestGetCW();
			void Start();
			CTestCanceller* RetrieveCancelPtr();
		protected:
			CTestGetCW(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			void ConstructL();
			void RunL();
			void DoCancel();
			TInt CheckResults();
		protected:
			CRetrieveMobilePhoneCWList* iRetrieve;
			CTestCanceller* iCanceller;
			RMobilePhone& iPhone;
			CTestPhone* iTestPhone;
			};
		
		class CTestGetCcbs : public CActive
			{
		public:
			static CTestGetCcbs* NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			~CTestGetCcbs();
			void Start();
			CTestCanceller* RetrieveCancelPtr();
		protected:
			CTestGetCcbs(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			void ConstructL();
			void RunL();
			void DoCancel();
			TInt CheckResults();
		protected:
			CRetrieveMobilePhoneCcbsList* iRetrieve;
			CTestCanceller* iCanceller;
			RMobilePhone& iPhone;
			CTestPhone* iTestPhone;
			};
			
		class CTestGetPreferredNetworks : public CActive
			{
		public:
			static CTestGetPreferredNetworks* NewLC(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			~CTestGetPreferredNetworks();
			void Start();
			CTestCanceller* RetrieveCancelPtr();
		protected:
			CTestGetPreferredNetworks(RMobilePhone& aPhone, CTestPhone* aTestPhone);
			void ConstructL();
			void RunL();
			void DoCancel();
			TInt CheckResults();
            TInt CheckPrefNetworksErrorResultsL();
		protected:
			CRetrieveMobilePhonePreferredNetworks* iRetrieve;
			CTestCanceller* iCanceller;
			RMobilePhone& iPhone;
			CTestPhone* iTestPhone;
			};
			
	class CTestRetrieveMobilePhoneAuthenticationIds : public CActive
			{
		public:
			enum TTypeOfAuth
				{
				EGba,
				EMbms
				};
			
			static CTestRetrieveMobilePhoneAuthenticationIds* NewLC(RMobilePhone& aPhone,CTestPhone* aTestPhone);
			~CTestRetrieveMobilePhoneAuthenticationIds();
			void SetAuthType(TTypeOfAuth aVersion);
			void TestRetrieve(RMobilePhone::TAuthData aAuthData,TBool aStartReqd);
			void TestCancel(RMobilePhone::TAuthData aAuthData);
		protected:
			CTestRetrieveMobilePhoneAuthenticationIds(RMobilePhone& aPhone,CTestPhone* aTestPhone);
			void ConstructL();
			void RunL();
			void DoCancel();
			void Start(RMobilePhone::TAuthData aAuthData);
			CTestCanceller* RetrieveCancelPtr();
			TInt CheckGBAResults();
			TInt CheckMBMSResults();
            TInt CheckGBAErrorResults();    
			TInt CheckMBMSErrorResults();
		protected:
			CRetrieveMobilePhoneAuthenticationIds* iRetrieveAuthId;
			CTestCanceller* iCanceller;
			RMobilePhone& iPhone;
			CTestPhone* iTestPhone;
			RMobilePhone::TAuthData iListType;
		private:
			TTypeOfAuth iAuthType;		//Used to determine whether to run the test for Gba or Mbms,
			};
			
	  class CTestRetrieveMobilePhonePreferredWlanSIDs : public CActive
		 	{
		public:
		  	static 	CTestRetrieveMobilePhonePreferredWlanSIDs* NewLC(RMobilePhone& aPhone,CTestPhone* aTestPhone);
		  	~CTestRetrieveMobilePhonePreferredWlanSIDs();
		  	void TestRetrieve(TBool aStartReqd);
		  	void TestCancel();
		protected:
		  	CTestRetrieveMobilePhonePreferredWlanSIDs(RMobilePhone& aPhone,CTestPhone* aTestPhone);
		  	void ConstructL();
		  	void RunL();
		  	void DoCancel();
		  	void Start();
		  	CTestCanceller* RetrieveCancelPtr();
		  	TInt CheckWLANResults();
            TInt CheckWLANErrorResultsL();        
		protected:
		  	CRetrieveMobilePhonePreferredWlanSIDs* iRetrieve;
		  	CTestCanceller* iCanceller;
		  	RMobilePhone& iPhone;
		  	CTestPhone* iTestPhone;
		   	};
		   	
		friend class CTestGetDetectedNetworks;
		friend class CTestGetCFStatus;
		friend class CTestGetCB;
		friend class CTestGetCW;
		friend class CTestGetCcbs;
		friend class CTestGetPreferredNetworks;

	};
	
#endif
