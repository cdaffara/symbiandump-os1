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
//

#ifndef __TSU_MMTSTHSUITE01_H__
#define __TSU_MMTSTHSUITE01_H__

#include <testframework.h>

// base class for the TSU_MMTSTH01 suite.

class  CTSUMmTsthSuite01 : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTSUMmTsthSuite01();
	TPtrC GetVersion() const;
	};


#endif //__TSU_MMTSTHSUITE01_H__
