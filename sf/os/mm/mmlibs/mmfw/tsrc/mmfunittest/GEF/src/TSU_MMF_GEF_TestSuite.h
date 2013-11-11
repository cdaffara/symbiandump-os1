// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests for ECam
// 
//

#ifndef TSU_MMF_GEF_TESTSUITE_H
#define TSU_MMF_GEF_TESTSUITE_H

#include <testframework.h>

class  CTestSuite_MMF_GEF : public CTestSuite
{
public:
	virtual ~CTestSuite_MMF_GEF();
	void 	InitialiseL();
	TPtrC 	GetVersion();
};

#endif // TSU_MMF_GEF_TESTSUITE_H

