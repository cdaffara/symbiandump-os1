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
// This is the header file for ROOTSERVER test section 2
// 
//


#if (!defined __ROOTSERVERTEST_2_H__)
#define __ROOTSERVERTEST_2_H__


class CRootServerTest2_1 : public CTestStepRootServer
{
public:
	CRootServerTest2_1(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest2_2 : public CTestStepRootServer
{
public:
	CRootServerTest2_2(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

class CRootServerTest2_3 : public CTestStepRootServer
{
public:
	CRootServerTest2_3(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

#endif //(__ROOTSERVERTEST_2_H__)
