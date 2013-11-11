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
// This file contains definitions of the test step classes for TSU_MMTSTH03
// 
//

#ifndef __TSU_MMTSTH03_H__
#define __TSU_MMTSTH03_H__

 
// FileName : MM-TSTH-U-0301
class RTestMmTsthU0301 : public RTSUMmTsthStep03
	{
public:
	static RTestMmTsthU0301* NewL();
	RTestMmTsthU0301();
	virtual TVerdict OpenL();
	virtual void Close();
	virtual TVerdict DoTestStepL();

	};

// FileName : MM-TSTH-U-0302
class RTestMmTsthU0302 : public RTSUMmTsthStep03
	{
public:
	static RTestMmTsthU0302* NewL();
	RTestMmTsthU0302();
	virtual TVerdict DoTestStepL();

	};

// FileName : MM-TSTH-U-0303
class RTestMmTsthU0303 : public RTSUMmTsthStep03
	{
public:
	static RTestMmTsthU0303* NewL();
	RTestMmTsthU0303();
	virtual TVerdict DoTestStepL();

	};

// FileName : MM-TSTH-U-0304
class RTestMmTsthU0304 : public RTSUMmTsthStep03
	{
public:
	static RTestMmTsthU0304* NewL();
	RTestMmTsthU0304();
	virtual TVerdict DoTestStepL();

	};

// FileName : MM-TSTH-U-0305
class RTestMmTsthU0305 : public RTSUMmTsthStep03
	{
public:
	static RTestMmTsthU0305* NewL();
	RTestMmTsthU0305();
	virtual TVerdict DoTestStepL();

	};

// FileName : MM-TSTH-U-0311
class RTestMmTsthU0311 : public RTSUMmTsthStep03
	{
public:
	static RTestMmTsthU0311* NewL();
	RTestMmTsthU0311();
	virtual TVerdict DoTestStepL();

	};

#endif //__TSU_MMTSTH02_H__
