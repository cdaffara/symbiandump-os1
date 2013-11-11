/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/**
    @file
    @test
*/

#ifndef WAPPROTSTATUSREPORTS_H
#define WAPPROTSTATUSREPORTS_H

#include "WapProtSuiteStepBase.h"

/** 
Base class for testing WAP status reports
*/
class CTestWapStatusReports : public CWapProtSuiteStepBase
	{
public:
	//TEF virtual functions
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	
	void SendWapMessageL();
    TSmsStatus::TSmsStatusValue ReceiveStatusReportL();	
    	
protected:
	void GetExpectedStatus();
	
protected:
	TInt iExpectedStatus;
	};

/**
Base class for sending WAP messages, requesting status reports
*/
class CTestRequestingStatusReports : public CTestWapStatusReports
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	};

/**
Class sets the request for only the last segment to receive status reports
*/
class CTestRequestingStatusReportsWithOption : public CTestRequestingStatusReports
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};

/** 
Class for receiving status reports 
*/
class CTestReceivingStatusReports : public CTestWapStatusReports
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

/** 
Class for receiving status reports for multiple 7 bit messages
*/
class CTestReceivingStatusReportsSevenBitMultiplePDUs : public CTestReceivingStatusReports
	{
public:
	virtual TVerdict doTestStepL();
	};

/**
Step receives a status report for a request for only the last segment
*/
class CTestReceivingStatusReportsWithOption : public CTestReceivingStatusReports
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};

#endif //WAPPROTSTATUSREPORTS_H
