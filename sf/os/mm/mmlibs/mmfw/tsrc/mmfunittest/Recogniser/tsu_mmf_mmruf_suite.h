// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU_MMF_MMRUF_SUITE_H
#define TSU_MMF_MMRUF_SUITE_H

#include <apgcli.h>
#include <testframework.h>
#include <e32base.h>
#include "definitions.h"



/*
**
*/
class CTSU_MMF_MMRUF : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTSU_MMF_MMRUF();
	virtual TPtrC GetVersion() const;
	};

#endif
