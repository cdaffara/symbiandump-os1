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
// This file contains definitions of the test step classes for TSU_MMTSTH11
// 
//

#ifndef __TSU_MMTSTH11_H__
#define __TSU_MMTSTH11_H__

 
// TestStep : MM-TSTH-U-1101
class RTestMmTsthU1101 : public RTSUMmTsthStep11
	{
public:
	static RTestMmTsthU1101* NewL();
	RTestMmTsthU1101();
	virtual TVerdict OpenL();
	virtual void Close();
	virtual TVerdict DoTestStepL();

	};

// TestStep : MM-TSTH-U-1102
class RTestMmTsthU1102 : public RTSUMmTsthStep11
	{
public:
	static RTestMmTsthU1102* NewL();
	RTestMmTsthU1102();
	virtual TVerdict DoTestStepL();

	};

// TestStep : MM-TSTH-U-1103
class RTestMmTsthU1103 : public RTSUMmTsthStep11
	{
public:
	static RTestMmTsthU1103* NewL();
	RTestMmTsthU1103();
	virtual TVerdict DoTestStepL();

	};

// TestStep : MM-TSTH-U-1104
class RTestMmTsthU1104 : public RTSUMmTsthStep11
	{
public:
	static RTestMmTsthU1104* NewL();
	RTestMmTsthU1104();
	virtual TVerdict DoTestStepL();

	};

// TestStep : MM-TSTH-U-1105
class RTestMmTsthU1105 : public RTSUMmTsthStep11
	{
public:
	static RTestMmTsthU1105* NewL();
	RTestMmTsthU1105();
	virtual TVerdict DoTestStepL();

	};

// TestStep : MM-TSTH-U-1111
class RTestMmTsthU1111 : public RTSUMmTsthStep11
	{
public:
	static RTestMmTsthU1111* NewL();
	RTestMmTsthU1111();
	virtual TVerdict DoTestStepL();

	};

#endif // __TSU_MMTSTH11_H__
