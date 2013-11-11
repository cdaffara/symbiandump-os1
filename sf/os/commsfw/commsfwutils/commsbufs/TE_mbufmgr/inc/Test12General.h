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
// Test12General.cpp
// Contains declarations of test step 12 
// 
//


#if (!defined __TEST12GENERAL_H__)
#define __TEST12GENERAL_H__
class RMBufChain;
class RMBufPktQ;
NONSHARABLE_CLASS(CTest12General) : public CTestStepCTMbufmgr
{
public:
	CTest12General();
	~CTest12General();

	virtual enum TVerdict doTestStepL( void );
private:

	void LeaveIfErrorL(TInt aErr, RMBufChain& aChain, const TDesC& aMsg);
	void LeaveIfDifferentL(TInt aLHS, TInt aRHS, RMBufChain& aChain, const TDesC& aMsg);
	void Test_QueueStats(RMBufPktQ &aQueue, const TText *aName);
	typedef struct Test_str //This will be header used in test
	{
		TUint8 iHdr[20];
	} Test_str;

};
#endif //(__TEST12GENERAL_H__)
