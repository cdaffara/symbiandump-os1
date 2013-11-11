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

#ifndef _TE_SIMEAP_H_
#define _TE_SIMEAP_H_

#include "te_simeapteststepbase.h"
#include <mmretrieve.h>

class CTestSmartCardEap : public CSimEapTestStepBase
	{
public:
	CTestSmartCardEap();
	~CTestSmartCardEap() {};
	virtual TVerdict doTestStepL();
	};

/**
Active object definition for testing the two-phase retrieval of
response packet from TSY to an authentication challenge request.
*/
class CTestSimEapAuthenticateData : public CActive
	{
public:
	static CTestSimEapAuthenticateData* NewLC(RMobileSmartCardEap& aPhone, TDesC8& aDes);
	void Start();
	TInt ValidateEapResponse(const TDesC8& aExpectedResponse);

private:
	~CTestSimEapAuthenticateData();
	CTestSimEapAuthenticateData(RMobileSmartCardEap& aPhone, TDesC8& aDes);
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


class CTestInitEapWithoutClosing : public CSimEapTestStepBase
	{
public:
	CTestInitEapWithoutClosing();
	~CTestInitEapWithoutClosing() {};
	virtual TVerdict doTestStepL();
	};

class CTestSmartCardAppAuthenticate : public CSimEapTestStepBase
	{
public:
	CTestSmartCardAppAuthenticate();
	~CTestSmartCardAppAuthenticate() {};
	virtual TVerdict doTestStepL();
	};

class CTestGetSmartCardAppEapList : public CSimEapTestStepBase
	{
public:
	CTestGetSmartCardAppEapList();
	~CTestGetSmartCardAppEapList() {};
	virtual TVerdict doTestStepL();
	};

class CTestSmartCardAppAuthenticate2 : public CSimEapTestStepBase
	{
public:
	CTestSmartCardAppAuthenticate2();
	~CTestSmartCardAppAuthenticate2() {};
	virtual TVerdict doTestStepL();
	};

#endif // _TE_SIMEAP_H_
