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
// Contains declaration of test 15
// 
//

#if (!defined __TEST15CONCURRENCY_H__)
#define __TEST15CONCURRENCY_H__

NONSHARABLE_CLASS(CTest15Concurrency) : public CTestStepCTMbufmgr
{
public:
	CTest15Concurrency();
	~CTest15Concurrency();
	virtual enum TVerdict doTestStepL( void );
private:
	static TInt fHighPriorityThread(TAny*); //High priority thread in this test

	TInt iMainThreadTries;
	TInt iMainThreadFails;
	TInt iHighThreadTries;
	TInt iHighThreadFails;
};	
#endif //(__TEST15CONCURRENCY_H__)
