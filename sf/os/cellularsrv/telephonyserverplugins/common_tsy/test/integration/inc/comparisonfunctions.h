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

#ifndef COMPARISONFUNCTIONS_H
#define COMPARISONFUNCTIONS_H

#include <e32def.h>
#include <e32const.h>
#include <stddef.h>
#include <string.h>
#include <etel.h>
#include <etelpckt.h>
#include <etelmm.h>
#include "cctsyintegrationtestsuitebase.h"

enum TLogError
	{
	ELogError,
	ENoLogError
	};

	
template<class Type>
class TCmpBase
	{
public:
	inline TCmpBase(Type& aExpected, Type& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep);
	Type& GetExpected() { return iExpected; }
	Type& GetValue() { return iValue; }	
	inline CTestExecuteLogger& Logger() {return iTestStep.Logger(); };
		
	virtual TBool IsEqual(TLogError /*aLogError*/) { return (memcmp(&iExpected, &iValue, sizeof(iExpected))==0); }

protected:
	Type& iExpected;
	Type& iValue;
	CCTSYIntegrationTestSuiteStepBase& iTestStep;
	}; // class TCmpBase


template<class Type>
class TCapsCmpBase
	{
public:
	inline TCapsCmpBase(Type& aWantedCaps, Type& aUnwantedCaps, Type& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep);
	Type& GetWantedCaps() { return iUnwantedCaps; }
	Type& GetUnwantedCaps() { return iWantedCaps; }
	Type& GetValue() { return iValue; }
	inline CTestExecuteLogger& Logger() {return iTestStep.Logger(); };
	
	virtual TBool IsEqual(TLogError aLogError);

protected:
	Type& iWantedCaps;
	Type& iUnwantedCaps;
	Type& iValue;
	CCTSYIntegrationTestSuiteStepBase& iTestStep;
	}; // class TCapsCmpBase	


class TCmpRCallTCallInfo : public TCmpBase<RCall::TCallInfo>
	{
public:
	TCmpRCallTCallInfo(RCall::TCallInfo& aExpected, RCall::TCallInfo& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep);
	
	virtual TBool IsEqual(TLogError aLogError);
	}; // class TCmpRCallTCallInfo


class TCmpRCallTCallInfoNoHookValidName : public TCmpBase<RCall::TCallInfo>
	{
public:	
	TCmpRCallTCallInfoNoHookValidName(RCall::TCallInfo& aExpected, RCall::TCallInfo& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep);
	
	virtual TBool IsEqual(TLogError aLogError);
	}; // class TCmpRCallTCallInfoNoHookValidName


class TCmpRMobileCallTMobileCallCaps : public TCmpBase<RMobileCall::TMobileCallCapsV1>
	{
	public:
    TCmpRMobileCallTMobileCallCaps(RMobileCall::TMobileCallCapsV1& aExpected, RMobileCall::TMobileCallCapsV1& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep);
	
    virtual TBool IsEqual(TLogError aLogError);
	}; // class TCmpRMobileCallTMobileCallCaps


class TCmpRMobileCallTMobileCallRemotePartyInfo : public TCmpBase<RMobileCall:: TMobileCallRemotePartyInfoV1>
	{
	public:
	TCmpRMobileCallTMobileCallRemotePartyInfo(RMobileCall::TMobileCallRemotePartyInfoV1& aExpected, RMobileCall::TMobileCallRemotePartyInfoV1& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep);
	
	virtual TBool IsEqual(TLogError aLogError);
	}; // class TCmpRMobileCallTMobileCallRemotePartyInfo


template<class Type>
inline TCmpBase<Type>::TCmpBase(Type& aExpected, Type& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep) 
	: iExpected(aExpected), iValue(aValue), iTestStep(aTestStep)
	{
	}


template<class Type>
inline TCapsCmpBase<Type>::TCapsCmpBase(Type& aWantedCaps, Type& aUnwantedCaps, Type& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep)
	: iWantedCaps(aWantedCaps), iUnwantedCaps(aUnwantedCaps), iValue(aValue), iTestStep(aTestStep)
	{
	}


template<class Type>
TBool TCapsCmpBase<Type>::IsEqual(TLogError aLogError)
	{
	if (((iValue & iWantedCaps) != iWantedCaps) || 
		((iValue & iUnwantedCaps) != 0))
			{
			if(aLogError == ELogError)
				{
				_LIT(KAssertBitsSet, "FAILURE: Wrong bits set : Got 0x%x , required 0x%x, unwanted 0x%x");
				ERR_PRINTF4(KAssertBitsSet, iValue, iWantedCaps, iUnwantedCaps);
				}
			return EFalse;
			}
		else
			{
			return ETrue;
			}
	}


class TCmpTMobilePhoneLockInfoV1NoStatus : public TCmpBase<RMobilePhone::TMobilePhoneLockInfoV1>
	{
public:
	TCmpTMobilePhoneLockInfoV1NoStatus(RMobilePhone::TMobilePhoneLockInfoV1& aExpected, RMobilePhone::TMobilePhoneLockInfoV1& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep);
	
	virtual TBool IsEqual(TLogError aLogError);
	}; // class TCmpTMobilePhoneLockInfoV1NoStatus


class TCmpRPacketContextTContextConfigGPRS : public TCmpBase<RPacketContext::TContextConfigGPRS>
	{
	public:
    TCmpRPacketContextTContextConfigGPRS(RPacketContext::TContextConfigGPRS& aExpected, RPacketContext::TContextConfigGPRS& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep);
	
    virtual TBool IsEqual(TLogError aLogError);
	}; // class TCmpRPacketContextTContextConfigGPRS


class TCmpRPacketContextTContextConfigR99_R4 : public TCmpBase<RPacketContext::TContextConfigR99_R4>
	{
	public:
    TCmpRPacketContextTContextConfigR99_R4(RPacketContext::TContextConfigR99_R4& aExpected, RPacketContext::TContextConfigR99_R4& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep);
	
    virtual TBool IsEqual(TLogError aLogError);
	}; // class TCmpRPacketContextTContextConfigR99_R4


class TCmpRPacketContextTContextConfig_R5 : public TCmpBase<RPacketContext::TContextConfig_R5>
	{
	public:
    TCmpRPacketContextTContextConfig_R5(RPacketContext::TContextConfig_R5& aExpected, RPacketContext::TContextConfig_R5& aValue, CCTSYIntegrationTestSuiteStepBase& aTestStep);
	
    virtual TBool IsEqual(TLogError aLogError);
	}; // class TCmpRPacketContextTContextConfig_R5

#endif // COMPARISONFUNCTIONS_H
