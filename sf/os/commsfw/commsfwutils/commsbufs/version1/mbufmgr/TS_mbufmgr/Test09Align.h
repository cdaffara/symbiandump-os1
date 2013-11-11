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
// Contains declaration of test step 09
// 
//

#if (!defined __TEST09ALIGN_H__)
#define __TEST09ALIGN_H__

NONSHARABLE_CLASS(CTest09Align) : public CTestStepCTMbufmgr
{
public:
	CTest09Align();
	~CTest09Align();

	virtual enum TVerdict doTestStepL( void );
private:
	struct RMBufInfo {
		TInt aOffset;
		TInt aLength;
	};
	struct CScriptInput {  // This str. will be obtained 
		RMBufInfo aBuf[50];// from the script file
		TInt aBufsBefore;
		TInt aBufsAfter;
	};
	struct CTestStr		   // This str. will 'simulate' a header
	{					   // that has to be aligned within chain
		TUint32 iSrcAddr;
		TUint32 iDstAddr;
		TUint16 iSrcPort;
		TUint16 iDstPort;
		TUint16 iLength;
		TUint16 iOptions;
		TUint8  iData[48];
	};
};

#endif //(__TEST09ALIGN_H__)
