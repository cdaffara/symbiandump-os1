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
// Contains declaration of CTestSuiteCTMbufmgr that is
// container class for all Mbufmgr test steps
// 
//

#if (!defined __TESTSUITECTMBUFMGR_H__)
#define __TESTSUITECTMBUFMGR_H__

#include <networking/testsuite.h>
#include <es_mbman.h>

NONSHARABLE_CLASS(CTestSuiteCTMbufmgr) : public CTestSuite
{
public:
	void InitialiseL( void );
	~CTestSuiteCTMbufmgr();
	void AddTestStepL( CTestStepCTMbufmgr * ptrTestStep );
	TPtrC GetVersion( void );

	CActiveScheduler* as;
	CMBufManager* MBMngr;
private:
};

#endif /* __TESTSUITECTMBUFMGR_H__ */
