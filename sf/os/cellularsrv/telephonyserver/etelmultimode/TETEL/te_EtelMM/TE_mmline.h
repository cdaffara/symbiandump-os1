// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent 
*/

#ifndef _TE_MMLINE_H_
#define _TE_MMLINE_H_

#include <etelmm.h>


class CTestLine : public CEtelMMTestStepBase
	{
public:
	CTestLine() ;
	~CTestLine(){} ;
	virtual enum TVerdict doTestStepL();
	TInt iTestCount;
	};

#endif
