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
// Contains declaration of test step 10
// 
//


#if (!defined __TEST10COPYL_H__)
#define __TEST10COPYL_H__


NONSHARABLE_CLASS(CTest10Copy) : public CTestStepCTMbufmgr
{
public:
	CTest10Copy();
	~CTest10Copy();

	virtual enum TVerdict doTestStepL( void );
private:
	struct RMBufInfo {
		TInt aOffset;
		TInt aLength;
	};
	struct CScriptInput {   // This str will be obtained 
		RMBufInfo aBuf[50]; // from the script file
		TInt aBufs;
		TInt aChainLen;
		TInt aCopyOffset;
		TInt aCopyLen;
	};
};

#endif //(__TEST10COPYL_H__)
