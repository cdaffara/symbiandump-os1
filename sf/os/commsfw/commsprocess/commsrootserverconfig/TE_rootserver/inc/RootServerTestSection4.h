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
// This is the header file for ROOTSERVER test section 4
// 
//


#if (!defined __ROOTSERVERTEST_4_H__)
#define __ROOTSERVERTEST_4_H__


class CRootServerTest4_1 : public CTestStepRootServer
{
public:
	CRootServerTest4_1(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );
};

#endif //(__ROOTSERVERTEST_4_H__)
