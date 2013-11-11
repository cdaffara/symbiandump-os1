// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains definition of CTestStepCTMbufmgr which is the base class
// for all the Mbufmgr Test Step classes
//


#if (!defined __TEST20Alloc_H__)
#define __TEST20Alloc_H__

NONSHARABLE_CLASS(CTest20Alloc) : public CTestStepCTMbufmgr
{
public:
	CTest20Alloc();
	~CTest20Alloc();

	virtual enum TVerdict doTestStepL( void );
private:
};

#endif //(__TEST09ALIGN_H__)
