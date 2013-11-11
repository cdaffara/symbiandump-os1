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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __TDSCSTORE_STEP_BASE_H__)
#define __TDSCSTORE_STEP_BASE_H__

#include <test/testexecutestepbase.h>

/**
CAppfwkDSCStoreTestStepBase provides additional utility methods to be used.
*/
class CAppfwkDscStoreTestStepBase : public CTestStep
	{
public:
	CAppfwkDscStoreTestStepBase();	
	~CAppfwkDscStoreTestStepBase();
	 	
public:
	void SetEnumFileL(const TDesC& aFileName);
	TBool GetEnumFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
	void SetTEFParameterL(const TDesC& aKeyName, const TDesC& aValue);
	TBool GetTEFParameterL(const TDesC& aKeyName,  TPtrC& aValue);	
	
private:
	CIniData*		iEnumData;
	CIniData*		iTEFParams;
	};

_LIT(KAppfwkDscStoreTestStepBase,"CAppfwkDscStoreTestStepBase");

#endif


