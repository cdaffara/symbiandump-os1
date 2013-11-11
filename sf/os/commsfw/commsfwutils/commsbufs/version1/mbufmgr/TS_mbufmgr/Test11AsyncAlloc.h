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
// Test11AsyncAlloc.cpp
// Contains declarations of test step 11 
// 
//


#if (!defined __TEST11ASYNCALLOC_H__)
#define __TEST11ASYNCALLOC_H__


NONSHARABLE_CLASS(CTest11AsyncAlloc) : public CTestStepCTMbufmgr
{
public:
	CTest11AsyncAlloc();
	~CTest11AsyncAlloc();

	virtual enum TVerdict doTestStepL( void );
private:

};



#endif //(__TEST11ASYNCALLOC_H__)
