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
// This file contains definitions of the test step classes for TSU_MMTSTH01
// 
//

#ifndef __TSU_MMTSTH01_H__
#define __TSU_MMTSTH01_H__

#include "TSU_MmTsthStep01.h"
 
// Parseline : MM-TSTH-U-0101

class RTestMmTsthU0101 : public RTSUMmTsthStep01
	{
public:
	static RTestMmTsthU0101* NewL();
	RTestMmTsthU0101();
	virtual TVerdict OpenL();
	virtual void Close();
	virtual TVerdict DoTestStepL();
	};

// Parseline : MM-TSTH-U-0102

class RTestMmTsthU0102 : public RTSUMmTsthStep01
	{
public:
	static RTestMmTsthU0102* NewL();
	RTestMmTsthU0102();
	virtual TVerdict DoTestStepL();
	};

// Parseline : MM-TSTH-U-0103

class RTestMmTsthU0103 : public RTSUMmTsthStep01
	{
public:
	static RTestMmTsthU0103* NewL();
	RTestMmTsthU0103();
	virtual TVerdict DoTestStepL();
	};

// Parseline : MM-TSTH-U-0104

class RTestMmTsthU0104 : public RTSUMmTsthStep01
	{
public:
	static RTestMmTsthU0104* NewL();
	RTestMmTsthU0104();
	virtual TVerdict DoTestStepL();
	};

// Parseline : MM-TSTH-U-0111

class RTestMmTsthU0111 : public RTSUMmTsthStep01
	{
public:
	static RTestMmTsthU0111* NewL();
	RTestMmTsthU0111();
	virtual TVerdict DoTestStepL();
	};


// Dummy test steps

class RTestStepDummy : public RTestStep
	{
public:
	static RTestStepDummy* NewL();
	RTestStepDummy();
	virtual TVerdict DoTestStepL();
	};

// Dummy panic test step

class RPanicTestStepDummy : public RTestStep
	{
public:
	static RPanicTestStepDummy* NewL();
	RPanicTestStepDummy();
	virtual TVerdict DoTestStepL();
	};

#endif // __TSU_MMTSTH01_H__
