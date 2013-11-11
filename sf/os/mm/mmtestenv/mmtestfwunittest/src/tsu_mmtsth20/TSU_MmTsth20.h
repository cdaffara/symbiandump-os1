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
// This file contains definitions of the test step classes for TSU_MMTSTH20
// 
//

#ifndef __TSU_MMTSTH20_H__
#define __TSU_MMTSTH20_H__

// TestFrameworkServer : MM-TSTH-U-2201
class RTestMmTsthU2001 : public RTestStep
	{
public:
	static RTestMmTsthU2001* NewL();
	RTestMmTsthU2001();
	virtual TVerdict DoTestStepL();

	};

// TestFrameworkServer : MM-TSTH-U-2202
class RTestMmTsthU2002 : public RTestStep
	{
public:
	static RTestMmTsthU2002* NewL();
	RTestMmTsthU2002();
	virtual TVerdict DoTestStepL();

	};

#endif // __TSU_MMTSTH20_H__
