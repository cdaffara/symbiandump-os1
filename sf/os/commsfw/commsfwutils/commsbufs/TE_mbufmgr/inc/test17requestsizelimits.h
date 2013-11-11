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
// Contains declaration of test 17
// 
//


#if (!defined __TEST17REQUESTSIZELIMITS_H__)
#define __TEST17REQUESTSIZELIMITS_H__

NONSHARABLE_CLASS(CTest17RequestSizeLimits) : public CTestStepCTMbufmgr
{
public:
	CTest17RequestSizeLimits();
	~CTest17RequestSizeLimits();
	
	virtual enum TVerdict doTestStepL( void );
	
	void FillDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar);
};

#endif //(__TEST17REQUESTSIZELIMITS_H__)
