// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file for MM Autorization Data
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETELMMAUTHORIZATION_H__)
#define __ETELMMAUTHORIZATION_H__

#include "TE_EtelMMTestStepBase.h"
#include "mmretrieve.h"
#include "TE_mmcanceller.h"

NONSHARABLE_CLASS(CEtelMMTestAuthorizationData) : public CEtelMMTestStepBase
{
public:
	CEtelMMTestAuthorizationData();
	~CEtelMMTestAuthorizationData();
	void Test(TBool aCondition);

	virtual enum TVerdict doTestStepL( void );
};


NONSHARABLE_CLASS(CTestAuthorizationData) : public CActive
		{
public:
		static CTestAuthorizationData* NewLC(RMobilePhone& aPhone,RMobilePhone::CImsAuthorizationInfoV5& aAuthInfo);
		~CTestAuthorizationData();
		void Start();		
		CAsyncRetrieveAuthorizationInfo* iRetrieve;
	protected:
		CTestAuthorizationData(RMobilePhone& aPhone,RMobilePhone::CImsAuthorizationInfoV5& aAuthInfo);
		void ConstructL();
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
	protected:
		RMobilePhone::CImsAuthorizationInfoV5& iAuthInfo;	
	public:
		CTestCanceller* iCanceller;
	protected:
		RMobilePhone& iPhone;
};


//
#endif //(__ETELMMAUTHORIZATION_H__)
