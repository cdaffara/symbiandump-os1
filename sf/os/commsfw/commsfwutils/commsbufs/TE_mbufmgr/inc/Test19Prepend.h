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
// Test19Prepend.h
// Contains declaration of test step 19
//
//

#if (!defined __TEST19PREPEND_H__)
#define __TEST19PREPEND_H__

NONSHARABLE_CLASS(CTest19Prepend) : public CTestStepCTMbufmgr
{
public:
	CTest19Prepend();
	~CTest19Prepend();

	virtual enum TVerdict doTestStepL( void );
private:
};

#endif //(__TEST09ALIGN_H__)
