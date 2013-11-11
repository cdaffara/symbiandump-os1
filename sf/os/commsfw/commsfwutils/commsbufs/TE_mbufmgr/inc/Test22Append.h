// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test22Append.h
// Contains declaration of test step 22
//
//

#if (!defined __TEST22APPEND_H__)
#define __TEST22APPEND_H__

NONSHARABLE_CLASS(CTest22Append) : public CTestStepCTMbufmgr
{
public:
	CTest22Append();
	~CTest22Append();

	virtual enum TVerdict doTestStepL( void );
private:
};

#endif //(__TEST09ALIGN_H__)
