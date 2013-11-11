// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <etelmm.h>
#include <et_phone.h>
#include "CSimPhone.h"

class CSimPhone;
class CTestConfigSection;

class CSimSmartCardAuth : public CBase
	{
public:
	static CSimSmartCardAuth* NewL(CSimPhone* aPhone);
	~CSimSmartCardAuth();

	TInt GetScAuthenticationData(const TTsyReqHandle aTsyReqHandle, TDes8* aPckg1, const RMobilePhone::TAID* aAID);
	TInt GetScAuthenticationDataCancel(const TTsyReqHandle aTsyReqHandle);

private:
	CSimSmartCardAuth(CSimPhone *aPhone);
	void ConstructL();

	void ParseAuthInfoL();
	void ParseGBAAuthInfoL();
	void ParseMBMSAuthInfoL();

	const CTestConfigSection* CfgFile();

	/**
	Structure for holding the authentication parameters read from the
	config.txt file.  All buffer sizes are multiplied by 4 since each
	byte needs at most 4 chars to be represented in the binary format
	used by the config.txt (i.e. one byte represented by "\xNN").
	*/
	struct TAuthInfo
		{
		/** holds value of network challenge AUTN */
		TBuf8<RMobilePhone::KAutnLength*4> iAUTN;
		/** holds value of network challenge RAND */
		TBuf8<RMobilePhone::KRandLength*4> iRAND;
		/** holds result RES of AUTHENTICATE command when it's successfull */
		TBuf8<RMobilePhone::KResLength*4> iRES;
		/** holds value of session key Integrity Key */
		TBuf8<RMobilePhone::KIkLength*4> iIK;
		/** holds value of session key Cipher Key */
		TBuf8<RMobilePhone::KCkLength*4> iCK;
		/** holds value of GSM Cipher Key */
		TBuf8<RMobilePhone::KKcLength*4> iKc;
		/** holds value of Authentication Token when  AUTHENTICATE fails */
		TBuf8<RMobilePhone::KAutsLength*4> iAUTS;
		/**
		 holds Application's ID, on which the authentication is to be
		 carried out.  Note, there is no checking for this AID's
		 existence, config file must simply match client's request data.
		 */
		TBuf8<RMobilePhone::KAIDSize*4> iAID;
		/** holds the error code that the AUTHENTICATE request should pass/fail with */
		TInt iAuthErr;
		/* holds the Btid information */

		};
		
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
		/* holds the Btid information */
		TBuf8<RMobilePhone::KBtidLength> iBtid;
		/* holds the keylifetime information */
		TBuf8<RMobilePhone::KKeyLifetimeLength> iKeyLifeTime;
		/* holds whether the other application is busy or not */
		TBool iOtherApplnBusy;
		/* holds whether the current application is active or not */
		TBool iApplnActive;
		/** holds the error code that the AUTHENTICATE request should pass/fail with */
		TInt iAuthErr;
		};

	struct TMBMSInfo
		{
		/* holds the input Mikey from the Network */
		TBuf8<RMobilePhone::KMikeyLength> iInputMikey ;  //255
		/* holds the output Mikey containing the verification message */
		TBuf8<RMobilePhone::KMikeyLength> iOutputMikey;  //255
		/**holds the output parameter MBMS traffic key */
		TBuf8<RMobilePhone::KMtkLength> iMtk;  //16
		/** holds the output parameter random or pseudo random string used to 
		 * protect against some offline pre computation attacks on the 
		 * underlying security protocol. */
		TBuf8<RMobilePhone::KSaltLength> iSaltkey;  //16
		/** MSK is identified by its Key Domain ID and MSK ID 
		 * holds the Keydomain ID*/		
		TBuf8<RMobilePhone::KKeyDomainIdLength> iKeyDmn;   //3
		/** holds the Key group part */
		TBuf8<RMobilePhone::KKeyGroupLength> iMskIdGrp;  //2
		/** holds the MUK ID TLV contains MUK IDr and MUK IDi */
		TBuf8<RMobilePhone::KMukIdTlvLength> iMukIdTlv; //255
		/** holds the IDi part of MBMS User Key */
		TBuf8<RMobilePhone::KMukIdiLength>  iMukIdi;  //16
		/** holds the IDr part of MBMS User Key */
		TBuf8<RMobilePhone::KMukIdrLength>  iMukIdr; //16
		/** holds the timestamp for MIKEY reply protection in MSK delivery. */
		TBuf8<RMobilePhone::KTimeStampCounterLength> iMukTimestamp; //16
		/**
		 holds Application's ID, on which the authentication is to be
		 carried out.  Note, there is no checking for this AID's
		 existence, config file must simply match client's request data.
		 */
		TBuf8<RMobilePhone::KAIDSize> iAID;
		/** holds value of Ks_Int_Naf key 
		 * The length used for External NAF is being used here also*/
		TBuf8<RMobilePhone::KKsExtNafLength> iKsIntNaf;
		/* holds whether the current application is active or not */
		TBool iApplnActive;
		/** holds the error code that the AUTHENTICATE request should pass/fail with */
		TInt iAuthErr;
		};
		
private:
	CSimPhone* iPhone;
	TInt index;

	RArray<TAuthInfo> iAuthInfoList;
	CArrayFixFlat<TGBAAuthInfo> *iGBAAuthInfoList;
	CArrayFixFlat<TMBMSInfo> *iMBMSInfoList;
	};

