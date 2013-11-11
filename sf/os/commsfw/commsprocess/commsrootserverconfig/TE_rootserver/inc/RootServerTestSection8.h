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
// // Root Server Test Section 8 - configure Root Server
// 
//


#if (!defined __ROOTSERVERTEST_8_H__)
#define __ROOTSERVERTEST_8_H__

class CRootServerTest8_1 : public CTestStepRootServer
{
public:
	CRootServerTest8_1(CTE_RootServerServer *aSuite);

	virtual TVerdict doTestStepL( void );

private:
	void TestConvertPriorityValidL(const TDesC8& aPriorityName, TThreadPriority aExpectedPriority, TVerdict& aVerdict);
	void TestConvertPriorityInvalidL(const TDesC8& aPriorityName, TVerdict& aVerdict);
};

#endif //(__ROOTSERVERTEST_8_H__)
