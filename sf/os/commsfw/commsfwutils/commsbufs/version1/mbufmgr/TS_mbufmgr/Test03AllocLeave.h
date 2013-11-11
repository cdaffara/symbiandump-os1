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
// Contains declaration of test step 03 
// 
//

#if (!defined __TEST03ALLOCLEAVE_H__)
#define __TEST03ALLOCLEAVE_H__

NONSHARABLE_CLASS(CTest03AllocLeave) : public CTestStepCTMbufmgr
{
public:
	CTest03AllocLeave();
	~CTest03AllocLeave();

	virtual enum TVerdict doTestStepL( void );
private:

};

#endif //(__TEST03ALLOCLEAVE_H__)
