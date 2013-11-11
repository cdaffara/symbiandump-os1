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
// This file contains definitions of the test step classes for TSU_MMTSTH10
// 
//

#ifndef __TSU_MMTSTH10_H__
#define __TSU_MMTSTH10_H__

 
// Log : MM-TSTH-U-1001

class RTestMmTsthU1001 : public RTestStep
	{
public:
	static RTestMmTsthU1001* NewL();
	RTestMmTsthU1001();
	virtual TVerdict DoTestStepL();

	};

// Log : MM-TSTH-U-1011

class RTestMmTsthU1011 : public RTestStep
	{
public:
	static RTestMmTsthU1011* NewL();
	RTestMmTsthU1011();
	virtual TVerdict DoTestStepL();

	};

// Log : MM-TSTH-U-1012

class RTestMmTsthU1012 : public RTestStep
	{
public:
	static RTestMmTsthU1012* NewL();
	RTestMmTsthU1012();
	virtual TVerdict DoTestStepL();

	};

// Log : MM-TSTH-U-1013

class RTestMmTsthU1013 : public RTestStep
	{
public:
	static RTestMmTsthU1013* NewL();
	RTestMmTsthU1013();
	virtual TVerdict DoTestStepL();

	};

#endif // __TSU_MMTSTH10_H__
