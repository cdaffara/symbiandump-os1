// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestMMFACLNTSuite 
// 
//

#ifndef TS_MMFACLNT_SUITE_H_
#define TS_MMFACLNT_SUITE_H_

#include <testframework.h>
/**
 *
 * @class CTestMmfAclntSuite
 *
 */
class  CTestMmfAclntSuite : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestMmfAclntSuite();
	void AddTestStepL(CTestMmfAclntStep* ptrTestStep);
	void AddTestStepL(RTestStep * aTestStep);
	TPtrC GetVersion();
	};

#endif /* TS_MMFACLNT_SUITE_H_ */

