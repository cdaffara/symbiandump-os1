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
// This file contains definitions of the test step classes for TSU_MMTSTH02
// 
//

#ifndef __TSU_MMTSTH02_H__
#define __TSU_MMTSTH02_H__

#include "TSU_MmTsthStep02.h"
  
// Script : MM-TSTH-U-0201

class RTestMmTsthU0201 : public RTestStep // NB not derived from RTSUMmTsthStep02
	{
public:
	static RTestMmTsthU0201* NewL();
	RTestMmTsthU0201();
	virtual TVerdict DoTestStepL();

	};

// Script : MM-TSTH-U-0202

class RTestMmTsthU0202 : public RTSUMmTsthStep02
	{
public:
	static RTestMmTsthU0202* NewL();
	RTestMmTsthU0202();
	virtual TVerdict DoTestStepL();

	};

// Script : MM-TSTH-U-0203

class RTestMmTsthU0203 : public RTSUMmTsthStep02
	{
public:
	static RTestMmTsthU0203* NewL();
	RTestMmTsthU0203();
	virtual TVerdict DoTestStepL();

	};

#endif // __TSU_MMTSTH02_H__
