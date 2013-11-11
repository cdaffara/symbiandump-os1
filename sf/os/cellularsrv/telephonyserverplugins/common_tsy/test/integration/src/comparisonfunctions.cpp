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

#include "comparisonfunctions.h"

TCmpRCallTCallInfo::TCmpRCallTCallInfo
	(RCall::TCallInfo& aExpected, RCall::TCallInfo& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep)
	: TCmpBase<RCall::TCallInfo>(aExpected, aValue, aTestStep)
	{
	}

TBool TCmpRCallTCallInfo::IsEqual(TLogError aLogError)
/**
 * Check that all the parameters in RCall::TCallInfo are identical.
 */
	{
	if (iExpected.iCallName == iValue.iCallName &&
		iExpected.iLineName == iValue.iLineName &&
		iExpected.iHookStatus == iValue.iHookStatus &&
		iExpected.iDuration == iValue.iDuration)
		{
		return ETrue;
		}
	else
		{
		if(aLogError == ELogError)
			{
			_LIT(KAssertRCallErrorEqualsTextA,"FAILURE");
			_LIT(KAssertRCallErrorEqualsTextB," Got callname=%S,linename=%S,hookstatus=%d,duration=%d");
			_LIT(KAssertRCallErrorEqualsTextC," Expected callname=%S,linename=%S,hookstatus=%d,duration=%d");
			ERR_PRINTF1(KAssertRCallErrorEqualsTextA);
			ERR_PRINTF5(KAssertRCallErrorEqualsTextB, &iExpected.iCallName, &iExpected.iLineName, iExpected.iHookStatus, iExpected.iDuration.Int() );
			ERR_PRINTF5(KAssertRCallErrorEqualsTextC, &iValue.iCallName, &iValue.iLineName, iValue.iHookStatus, iValue.iDuration.Int() );	
			}
		return EFalse;
		}
	}
	

TCmpRCallTCallInfoNoHookValidName::TCmpRCallTCallInfoNoHookValidName
	(RCall::TCallInfo& aExpected, RCall::TCallInfo& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep)
	: TCmpBase<RCall::TCallInfo>(aExpected, aValue, aTestStep)
	{
	}
	
TBool TCmpRCallTCallInfoNoHookValidName::IsEqual(TLogError aLogError)
/**
 * Check that the call name and line name are not empty,
 * call status is correct and duration is > 0.
 */
	{
	if(iExpected.iCallName.Length() > 0 &&
			iExpected.iLineName.Length() > 0 &&
			iExpected.iStatus == iValue.iStatus &&
			iExpected.iDuration.Int() > 0)
		{
		return ETrue;
		}
	else
		{
		if(aLogError == ELogError)
			{
			_LIT(KAssertRCallErrorEqualsTextA,"FAILURE");
			_LIT(KAssertRCallErrorEqualsTextB," Got callname=%S,linename=%S,status=%d,duration=%d");
			_LIT(KAssertRCallErrorEqualsTextC," Expected callname=%S,linename=%S,status=%d,duration=%d");
			ERR_PRINTF1(KAssertRCallErrorEqualsTextA);
			ERR_PRINTF5(KAssertRCallErrorEqualsTextB, &iExpected.iCallName, &iExpected.iLineName, iExpected.iStatus, iExpected.iDuration.Int() );
			ERR_PRINTF5(KAssertRCallErrorEqualsTextC, &iValue.iCallName, &iValue.iLineName, iValue.iStatus, iValue.iDuration.Int() );	
			}
		return EFalse;
		}
	}
	
TCmpRMobileCallTMobileCallCaps::TCmpRMobileCallTMobileCallCaps
	(RMobileCall::TMobileCallCapsV1& aExpected, RMobileCall::TMobileCallCapsV1& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep)
	: TCmpBase<RMobileCall::TMobileCallCapsV1>(aExpected, aValue, aTestStep)
	{
	}
	
TBool TCmpRMobileCallTMobileCallCaps::IsEqual(TLogError aLogError)
/**
 * Check that the Call Control Caps and Call Event Caps flags are are as expected,
 */
	{
	if((  (iValue.iCallControlCaps & iExpected.iCallControlCaps) == iExpected.iCallControlCaps) &&
		( (iValue.iCallEventCaps & iExpected.iCallEventCaps) == iExpected.iCallEventCaps))
		{
		return ETrue;
		}
	else
		{
		if(aLogError == ELogError)
			{
			_LIT(KAssertRCallErrorEqualsTextA,"FAILURE");
			_LIT(KAssertRCallErrorEqualsTextB," Got CallControlCaps=0x%x,CallEventCaps=0x%x");
			_LIT(KAssertRCallErrorEqualsTextC," Expected CallControlCaps=0x%x,CallEventCaps=0x%x");
			ERR_PRINTF1(KAssertRCallErrorEqualsTextA);
			ERR_PRINTF3(KAssertRCallErrorEqualsTextB, iValue.iCallControlCaps, iValue.iCallEventCaps);
			ERR_PRINTF3(KAssertRCallErrorEqualsTextC, iExpected.iCallControlCaps, iExpected.iCallEventCaps);	
			}
		return EFalse;
		}
			
			
		
	}	
	

TCmpRMobileCallTMobileCallRemotePartyInfo::TCmpRMobileCallTMobileCallRemotePartyInfo
	(RMobileCall::TMobileCallRemotePartyInfoV1& aExpected, RMobileCall::TMobileCallRemotePartyInfoV1& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep)
	: TCmpBase<RMobileCall::TMobileCallRemotePartyInfoV1>(aExpected, aValue, aTestStep)
	{
	}

	
TBool TCmpRMobileCallTMobileCallRemotePartyInfo::IsEqual(TLogError aLogError)
/**
 * Check that the Direction and Remote Number flags are are as expected,
 */
	{
	if(( iValue.iDirection == iExpected.iDirection) &&
		(iValue.iRemoteNumber.iTelNumber == iExpected.iRemoteNumber.iTelNumber) )
		{
		return ETrue;
		}
	else
		{
		if(aLogError == ELogError)
			{
			_LIT(KAssertRCallErrorEqualsTextA,"FAILURE");
			_LIT(KAssertRCallErrorEqualsTextB," Got Direction=%X,Remote Number=%X");
			_LIT(KAssertRCallErrorEqualsTextC," Expected Direction=%X,Remote Number=%X");
			ERR_PRINTF1(KAssertRCallErrorEqualsTextA);
			ERR_PRINTF3(KAssertRCallErrorEqualsTextB, &iValue.iDirection, &iValue.iRemoteNumber.iTelNumber);
			ERR_PRINTF3(KAssertRCallErrorEqualsTextC,&iExpected.iDirection, &iExpected.iRemoteNumber.iTelNumber);	
			}
		return EFalse;
		}
	}	
	


TCmpTMobilePhoneLockInfoV1NoStatus::TCmpTMobilePhoneLockInfoV1NoStatus
(RMobilePhone::TMobilePhoneLockInfoV1& aExpected, RMobilePhone::TMobilePhoneLockInfoV1& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep)
: TCmpBase<RMobilePhone::TMobilePhoneLockInfoV1>(aExpected, aValue, aTestStep)
{
}

TBool TCmpTMobilePhoneLockInfoV1NoStatus::IsEqual(TLogError aLogError)
/**
* Check that the required parameters in RMobilePhone::TMobilePhoneLockInfoV1 are identical.
*/
{
if (iExpected.iSetting == iValue.iSetting
	 /* && iExpected.iStatus == iValue.iStatus*/)
	{
	return ETrue;
	}
else
	{
	if(aLogError == ELogError)
		{
		_LIT(KAssertRMobilePhoneErrorEqualsTextA,"FAILURE");
		_LIT(KAssertRMobilePhoneErrorEqualsTextB," Got iSetting=%d, iStatus=%d");
		_LIT(KAssertRMobilePhoneErrorEqualsTextC," Expected iSetting=%d, iStatus=%d");
		ERR_PRINTF1(KAssertRMobilePhoneErrorEqualsTextA);
		ERR_PRINTF3(KAssertRMobilePhoneErrorEqualsTextB, iExpected.iSetting,iExpected.iStatus);
		ERR_PRINTF3(KAssertRMobilePhoneErrorEqualsTextC, iValue.iSetting, iValue.iStatus);	
		}
	return EFalse;
	}
}


TCmpRPacketContextTContextConfigGPRS::TCmpRPacketContextTContextConfigGPRS
	(RPacketContext::TContextConfigGPRS& aExpected, RPacketContext::TContextConfigGPRS& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep)
	: TCmpBase<RPacketContext::TContextConfigGPRS>(aExpected, aValue, aTestStep)
	{
	}

TBool TCmpRPacketContextTContextConfigGPRS::IsEqual(TLogError aLogError)
/**
* Check that the required parameters in RPacketContext::TContextConfigGPRS are identical.
*/
	{
	if(	iExpected.iAccessPointName == iValue.iAccessPointName &&
		iExpected.iProtocolConfigOption.iAuthInfo.iUsername == iValue.iProtocolConfigOption.iAuthInfo.iUsername &&
		iExpected.iProtocolConfigOption.iAuthInfo.iPassword == iValue.iProtocolConfigOption.iAuthInfo.iPassword)
		{
		return ETrue;
		}

	else
		{
		if(aLogError == ELogError)
			{
			TBuf<255> apn, username, password;
			apn.Copy(iValue.iAccessPointName);
			username.Copy(iValue.iProtocolConfigOption.iAuthInfo.iUsername);
			password.Copy(iValue.iProtocolConfigOption.iAuthInfo.iPassword);
			_LIT(KAssertRPacketContextErrorEqualsTextA,"FAILURE");
			_LIT(KAssertRPacketContextErrorEqualsTextB," Got apn=%S,username=%S,password=%S");
			_LIT(KAssertRPacketContextErrorEqualsTextC," Expected apn=%S,username=%S,password=%S");
			ERR_PRINTF1(KAssertRPacketContextErrorEqualsTextA);
			ERR_PRINTF4(KAssertRPacketContextErrorEqualsTextB, 
				&apn,
				&username,
				&password);
			apn.SetLength(0);
			username.SetLength(0);
			password.SetLength(0);
			apn.Copy(iExpected.iAccessPointName);
			username.Copy(iExpected.iProtocolConfigOption.iAuthInfo.iUsername);
			password.Copy(iExpected.iProtocolConfigOption.iAuthInfo.iPassword);
			ERR_PRINTF4(KAssertRPacketContextErrorEqualsTextC,
				&apn,
				&username,
				&password);	
			}
		return EFalse;
		}
	}


TCmpRPacketContextTContextConfigR99_R4::TCmpRPacketContextTContextConfigR99_R4
	(RPacketContext::TContextConfigR99_R4& aExpected, RPacketContext::TContextConfigR99_R4& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep)
	: TCmpBase<RPacketContext::TContextConfigR99_R4>(aExpected, aValue, aTestStep)
	{
	}

TBool TCmpRPacketContextTContextConfigR99_R4::IsEqual(TLogError aLogError)
/**
* Check that the required parameters in RPacketContext::TContextConfigR99_R4 are identical.
*/
	{
	if(	iExpected.iAccessPointName == iValue.iAccessPointName &&
		iExpected.iProtocolConfigOption.iAuthInfo.iUsername == iValue.iProtocolConfigOption.iAuthInfo.iUsername &&
		iExpected.iProtocolConfigOption.iAuthInfo.iPassword == iValue.iProtocolConfigOption.iAuthInfo.iPassword)
		{
		return ETrue;
		}

	else
		{
		if(aLogError == ELogError)
			{
			TBuf<255> apn, username, password;
			apn.Copy(iValue.iAccessPointName);
			username.Copy(iValue.iProtocolConfigOption.iAuthInfo.iUsername);
			password.Copy(iValue.iProtocolConfigOption.iAuthInfo.iPassword);

			_LIT(KAssertRPacketContextErrorEqualsTextA,"FAILURE");
			_LIT(KAssertRPacketContextErrorEqualsTextB," Got apn=%S,username=%S,password=%S");
			_LIT(KAssertRPacketContextErrorEqualsTextC," Expected apn=%S,username=%S,password=%S");
			ERR_PRINTF1(KAssertRPacketContextErrorEqualsTextA);
			ERR_PRINTF4(KAssertRPacketContextErrorEqualsTextB, 
				&apn,
				&username,
				&password);
			apn.SetLength(0);
			username.SetLength(0);
			password.SetLength(0);
			apn.Copy(iExpected.iAccessPointName);
			username.Copy(iExpected.iProtocolConfigOption.iAuthInfo.iUsername);
			password.Copy(iExpected.iProtocolConfigOption.iAuthInfo.iPassword);
			ERR_PRINTF4(KAssertRPacketContextErrorEqualsTextC,
				&apn,
				&username,
				&password);	

			}
		return EFalse;
		}
	}


TCmpRPacketContextTContextConfig_R5::TCmpRPacketContextTContextConfig_R5
	(RPacketContext::TContextConfig_R5& aExpected, RPacketContext::TContextConfig_R5& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep)
	: TCmpBase<RPacketContext::TContextConfig_R5>(aExpected, aValue, aTestStep)
	{
	}

TBool TCmpRPacketContextTContextConfig_R5::IsEqual(TLogError aLogError)
/**
* Check that the required parameters in RPacketContext::TContextConfig_R5 are identical.
*/
	{
	if(	iExpected.iAccessPointName == iValue.iAccessPointName &&
		iExpected.iProtocolConfigOption.iAuthInfo.iUsername == iValue.iProtocolConfigOption.iAuthInfo.iUsername &&
		iExpected.iProtocolConfigOption.iAuthInfo.iPassword == iValue.iProtocolConfigOption.iAuthInfo.iPassword)
		{
		return ETrue;
		}

	else
		{
		if(aLogError == ELogError)
			{
			TBuf<255> apn, username, password;
			apn.Copy(iValue.iAccessPointName);
			username.Copy(iValue.iProtocolConfigOption.iAuthInfo.iUsername);
			password.Copy(iValue.iProtocolConfigOption.iAuthInfo.iPassword);
			_LIT(KAssertRPacketContextErrorEqualsTextA,"FAILURE");
			_LIT(KAssertRPacketContextErrorEqualsTextB," Got apn=%S,username=%S,password=%S");
			_LIT(KAssertRPacketContextErrorEqualsTextC," Expected apn=%S,username=%S,password=%S");
			ERR_PRINTF1(KAssertRPacketContextErrorEqualsTextA);
			ERR_PRINTF4(KAssertRPacketContextErrorEqualsTextB, 
				&apn,
				&username,
				&password);
			apn.SetLength(0);
			username.SetLength(0);
			password.SetLength(0);
			apn.Copy(iExpected.iAccessPointName);
			username.Copy(iExpected.iProtocolConfigOption.iAuthInfo.iUsername);
			password.Copy(iExpected.iProtocolConfigOption.iAuthInfo.iPassword);
			ERR_PRINTF4(KAssertRPacketContextErrorEqualsTextC,
				&apn,
				&username,
				&password);	
			}
		return EFalse;
		}
	}

