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

#ifndef _TE_TESTWAPSMS_H_
#define _TE_TESTWAPSMS_H_


class CTestTxRx7DCS : public CWapSmsTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestTxRx8DCS : public CWapSmsTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestTxRxRingTone : public CWapSmsTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestTxRxWap : public CWapSmsTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestWapAndApplicationPortBinding : public CWapSmsTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSendAndRecvViaAppllicationPortAndWap : public CWapSmsTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestUnSupportedAck : public CWapSmsTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestConsecutiveTx : public CWapSmsTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTestSinglePDUWapMessageWithConCatRef : public CWapSmsTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};

#endif
