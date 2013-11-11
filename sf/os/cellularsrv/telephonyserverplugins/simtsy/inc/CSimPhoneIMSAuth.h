// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SIMPHONEIMSAUTH_H_
#define __SIMPHONEIMSAUTH_H_

/**
 * @file
 *
 * Contains the class definitions required for the Phone IMS Auth
 * @internalAll
 */

#include <etelmm.h>
#include <et_phone.h>
#include "csimtimer.h"
#include "CSimSmsStore.h"//ugly but needed for listReadAllAttempt
//#include "CSimPhone.h"

//Constants

/**
 * The main Phone IMS Auth class which simulates IMS Authorization &
 * Authentication features
 */
class CSimPhone;
class CTestConfigSection;
class CTestConfigItem;
class CListReadAllAttempt;
class CSimPhoneIMSAuth : public CBase, public MTimerCallBack
	{
public:
	static CSimPhoneIMSAuth* NewL(CSimPhone* aPhone);
	CSimPhoneIMSAuth(CSimPhone* aPhone);
	~CSimPhoneIMSAuth();
	const CTestConfigSection* CfgFile();

	TInt GetAuthorizationInfoPhase1(const TTsyReqHandle aTsyReqHandle, 
			RMobilePhone::TClientId* aClient,TInt* aBufSize);
	TInt ProcessGetAuthorizationInfoPhase1L(const TTsyReqHandle aTsyReqHandle,
			RMobilePhone::TClientId* aClientId, TInt* aBufSize);
	TInt GetAuthorizationInfoPhase2(const TTsyReqHandle aTsyReqHandle, 
			RMobilePhone::TClientId* aClientId,TDes8* aBuffer);
	TInt GetAuthorizationInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyImsAuthorizationInfoChanged(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyImsAuthorizationInfoChangedCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetAuthenticationData(const TTsyReqHandle aTsyReqHandle,TDes8* aAuthenticationData);
	TInt GetAuthenticationDataCancel(const TTsyReqHandle aTsyReqHandle);

protected:
	void ConstructL();
	
private:
	struct TAuthorizationInfo
		{
		/**Internet Multimedia Private Identity stored on the ISIM/USIM*/
		RMobilePhone::TIMPI iIMPI;
		/**Internet Multimedia Public Identity(ies) stored on the ISIM/USIM*/
		RArray<RMobilePhone::TIMPU> iIMPUArray;
		/**Home Network Domain Name stored on the ISIM/USIM*/
		RMobilePhone::THNDN iHomeNetworkDomainName;
		/**indicates whether authorization data has been retrieved from the ISIM*/
		RMobilePhone::TAuthorizationDataSource iAuthenticationDataSource;
		/**The delay after calling NotifyChange after which the next Authorization info is set*/
		TInt iInfoChangeDelay;
		};

	struct TAuthenticationInfo
		{
		/** holds value of network challenge AUTN */
		TBuf8<RMobilePhone::KAutnLength> iAUTN;
		/** holds value of network challenge RAND */
		TBuf8<RMobilePhone::KRandLength> iRAND;
		/** holds result RES of AUTHENTICATE command when it's successfull*/
		TBuf8<RMobilePhone::KResLength> iRES;
		/** holds value of session key Integrity Key */
		TBuf8<RMobilePhone::KIkLength> iIK;
		/** holds value of session key Cipher Key */
		TBuf8<RMobilePhone::KCkLength> iCK;
		/** holds value of Authentication Token when  AUTHENTICATE fails*/
		TBuf8<RMobilePhone::KAutsLength> iAUTS;
		/** holds the error code that the AUTHENTICATE request should pass/fail with*/
		TInt iAuthenticationErr;
		};

	void TimerCallBack(TInt aId);
	void ParseAuthorizationInfoL();
	void ParseAuthenticationInfoL();

private:
	CSimPhone* iPhone;
	CSimTimer* iTimer;
	CArrayPtrFlat<CListReadAllAttempt>* iGetAuthorizationData;
	TBool iTimerStarted;

	TBool iAuthInfoChangeNotifyPending;				// < Authorization Info change notification outstanding
	TTsyReqHandle iAuthInfoChangeNotifyReqHandle;	// < Authorization Info change notification request handle


	RArray<TAuthorizationInfo> iAuthorizationInfoList;
	TInt iCurrentAuthorizationInfo;
	RArray<TAuthenticationInfo> iAuthenticationInfoList;
	};

#endif
