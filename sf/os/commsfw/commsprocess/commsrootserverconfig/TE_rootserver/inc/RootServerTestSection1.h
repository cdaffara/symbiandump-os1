// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file for ROOTSERVER test section 1
// 
//


#if (!defined __ROOTSERVERTEST_1_H__)
#define __ROOTSERVERTEST_1_H__


class CRootServerTest1_1 : public CTestStepRootServer
    {
public:
	CRootServerTest1_1(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
    };

class CRootServerTest1_2 : public CTestStepRootServer
    {
public:
	CRootServerTest1_2(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
    };

#endif //(__ROOTSERVERTEST_1_H__)
