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
// This file contains definitions of the test step classes for TSU_MMTSTH12
// 
//

#ifndef __TSU_MMTSTH12_H__
#define __TSU_MMTSTH12_H__

 
// TestSuite : MM-TSTH-U-1201
class RTestMmTsthU1201 : public RTSUMmTsthStep12
	{
public:
	static RTestMmTsthU1201* NewL();
	RTestMmTsthU1201();
	virtual TVerdict OpenL();
	virtual void Close();
	virtual TVerdict DoTestStepL();

	};

// TestSuite : MM-TSTH-U-1202
class RTestMmTsthU1202 : public RTSUMmTsthStep12
	{
public:
	static RTestMmTsthU1202* NewL();
	RTestMmTsthU1202();
	virtual TVerdict DoTestStepL();

	};

// TestSuite : MM-TSTH-U-1203
class RTestMmTsthU1203 : public RTSUMmTsthStep12
	{
public:
	static RTestMmTsthU1203* NewL();
	RTestMmTsthU1203();
	virtual TVerdict DoTestStepL();

	};

// TestSuite : MM-TSTH-U-1221
class RTestMmTsthU1221 : public RTSUMmTsthStep12
	{
public:
	static RTestMmTsthU1221* NewL();
	RTestMmTsthU1221();
	virtual TVerdict DoTestStepL();

	};

// TestSuite : MM-TSTH-U-1222
class RTestMmTsthU1222 : public RTSUMmTsthStep12
	{
public:
	static RTestMmTsthU1222* NewL();
	RTestMmTsthU1222();
	virtual TVerdict DoTestStepL();

	};
#endif // __TSU_MMTSTH12_H__
