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

#ifndef _TE_MMCALL_H_
#define _TE_MMCALL_H_

#include "TE_EtelMMTestStepBase.h"
#include <etelmm.h>

class CTestCall : public CEtelMMTestStepBase
	{
public:
	CTestCall() ;
	~CTestCall(){} ;
	virtual enum TVerdict doTestStepL();
	void ValidateCallDataCaps(const RMobileCall::TMobileCallDataCapsV1 &aDataCaps);
	void ValidateRLPRange(const RMobileCall::TMobileDataRLPRangesV1 &aRLPRanges);
	void ValidateHscsdParams(const RMobileCall::TMobileHscsdCallParamsV1& aHcsdParams);
	void ValidateHscsdInfo(const RMobileCall::TMobileCallHscsdInfoV1& aHcsdInfo);

	void TestBasicCallFunctionality(RMobileCall& aCall, TRequestStatus &aReqStatus);
	void TestMultimediaCall(RMobileCall& aCall, TRequestStatus &aReqStatus);
	TInt iTestCount;
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> iNumber;

	};

class CTestCallb : public CEtelMMTestStepBase
	{
public:
	CTestCallb() ;
	~CTestCallb(){} ;
	virtual enum TVerdict doTestStepL();
	/*static*/ void ValidateMobileCallInfo(const RMobileCall::TMobileCallInfoV1 &aSyncCallInfo);
	TInt iTestCount;
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> iNumber;

	};

class CTestCallc : public CEtelMMTestStepBase
	{
public:
	CTestCallc() ;
	~CTestCallc(){} ;
	virtual enum TVerdict doTestStepL();
	/*static*/ void ValidateRemotePartyInfoChange(const RMobileCall::TMobileCallRemotePartyInfoV1 &aRemoteParty);
	TInt iTestCount;
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> iNumber;

	};
	
 
class CTestCallISV : public CEtelMMTestStepBase
	{
public:
	CTestCallISV() ;
	~CTestCallISV(){} ;
	virtual enum TVerdict doTestStepL();
	TInt iTestCount;
	};


#endif
