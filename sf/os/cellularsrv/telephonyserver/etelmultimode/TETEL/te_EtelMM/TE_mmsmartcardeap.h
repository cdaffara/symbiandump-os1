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
 @internalComponent
*/

#ifndef _TE_MMSCEAP_H_
#define _TE_MMSCEAP_H_

#include "TE_EtelMMTestStepBase.h"
#include <etelmm.h>
#include "mmretrieve.h"

/**
Test all available functionality of RMobileSmartCardEap API.
*/
class CTestSmartCardEap : public CEtelMMTestStepBase
	{
public:
	CTestSmartCardEap() ;
	virtual enum TVerdict doTestStepL();
	void TestScEapL(TRequestStatus& aReqStatus, RMobileSmartCardEap& aOwnerScEap,
	                RMobileSmartCardEap& aNonownerScEap, RMobileSmartCardEap& aUnInitScEap);
	};

/**
Active object definition for testing the two-phase retrieval of
response packet from TSY to an authentication challenge request.
*/
class CTestEapAuthenticateData : public CActive
	{
public:
	static CTestEapAuthenticateData* NewLC(RMobileSmartCardEap& aPhone, TDesC8& aDes);
	void Start();
	TInt ValidateEapResponse();

private:
	~CTestEapAuthenticateData();
	CTestEapAuthenticateData(RMobileSmartCardEap& aPhone, TDesC8& aDes);
	void ConstructL(TDesC8& aDes);
	void DoCancel();
	void RunL();

private:
	RMobileSmartCardEap& iPhone;
	CAsyncSmartCardEapAuthentication* iTwoPhaseRetrieve;
	TDesC8& iDes;
	/**
	Descriptor to retrieved data, used in the ValidateEapResponse()
	check.
	*/
	TDesC8* iAuthResp;
	};

/**
Class for testing of recovery after bad usage of RMobileSmartCardEap.
*/
class CTestInitEapWithoutClosing : public CEtelMMTestStepBase
	{
public:
	CTestInitEapWithoutClosing() ;
	virtual enum TVerdict doTestStepL();
	};

#endif // _TE_MMSCEAP_H_
