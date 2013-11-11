// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//


#ifndef TE_STATUSREPORTSCHEMETRANSFER_H
#define TE_STATUSREPORTSCHEMETRANSFER_H

#include "Te_StatusReportScheme.h"

/**
Base class for all sending test cases
 */
class CTestSchemeSending : public CSmsStatusReportSchemeTestStep
    {
public:
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    };

/**
Base class for all sending of messages with TP-SRR
 */
class CTestSendingTPSRRScheme : public CTestSchemeSending
    {
public:
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    }; 

/**
Base class for all sending of messages with SMSC
 */
class CTestSendingSMSCScheme : public CTestSchemeSending
    {
public:
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    }; 

/**
Base class for all receiving test cases
 */
class CTestSchemeReceiving : public CSmsStatusReportSchemeTestStep
    {
public:
    virtual TVerdict doTestStepPreambleL();
    };

/**
Base class for receiving status reports
 */
class CTestReceivingStatusReports : public CTestSchemeReceiving
    {
public:
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();	
    void ReceiveStatusReportL(TSmsStatus::TSmsStatusValue&);		
    virtual TVerdict doTestStepL();

private:
    void GetExpectedStatus();

protected:
    TSmsStatus::TSmsStatusValue iExpectedStatus;	
    };

/**
Base class for receiving Delivered messages
 */
class CTestReceivingMessages : public CTestSchemeReceiving
    {
public:
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();	

protected:
    TSmsStatusReportScheme iExpectedScheme;
    };

#endif // TE_STATUSREPORTSCHEMETRANSFER_H
