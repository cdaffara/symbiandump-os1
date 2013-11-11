// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#ifndef _TE_TESTSMSPDUDB_H_
#define _TE_TESTSMSPDUDB_H_


class CTestSmsPduDbMessage : public CSmsPduDbTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsPduDbPdu : public CSmsPduDbTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsPduDbConcatSegment : public CSmsPduDbTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsPduDbConcat : public CSmsPduDbTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsPduDbParse : public CSmsPduDbTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsPduDbCompile : public CSmsPduDbTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsPduDbTools : public CSmsPduDbTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsServiceCenterTimeStamp : public CSmsPduDbTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSmsValidityPeriod : public CSmsPduDbTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

#endif
