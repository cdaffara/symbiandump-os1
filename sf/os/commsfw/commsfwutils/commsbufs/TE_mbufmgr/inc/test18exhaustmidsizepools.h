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
// Contains declaration of test 18
// 
//


#if (!defined __TEST18EXHAUSTMIDSIZEPOOLS_H__)
#define __TEST18EXHAUSTMIDSIZEPOOLS_H__

NONSHARABLE_CLASS(CTest18ExhaustMidSizePools) : public CTestStepCTMbufmgr
{
public:
	CTest18ExhaustMidSizePools();
	~CTest18ExhaustMidSizePools();
	
	virtual enum TVerdict doTestStepL( void );
	
	void FillDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar);
};

#endif /*__TEST18EXHAUSTMIDSIZEPOOLS_H__*/
