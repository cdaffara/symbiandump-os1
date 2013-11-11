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
// Contains declaration of test step 21
// 
//

#ifndef __TEST21COMMSBUFS_H__
#define __TEST21COMMSBUFS_H__

NONSHARABLE_CLASS(CTest21CommsBufs) : public CTestStepCTMbufmgr
{
public:
	CTest21CommsBufs();
	~CTest21CommsBufs();

	virtual enum TVerdict doTestStepL( void );
private:

};

#endif // __TEST21COMMSBUFS_H__
