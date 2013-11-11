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

/**
 @file
*/

#include "Te_StatusReportSchemeTransfer.h"

/**
Create a SMS and get the SMSC and TPSRR operations
*/
TVerdict CTestSchemeSending::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CSmsStatusReportSchemeTestStep::doTestStepPreambleL();

	//Create SMS message from ini file specified as "message"
	CreateSmsL(iDataCodingScheme ,_L("message"));
	
	return TestStepResult();
	}

/**
Delete the SMS 
*/
TVerdict CTestSchemeSending::doTestStepPostambleL()
	{	
	delete iSmsMessage;
	iSmsMessage = NULL;

	CSmsStatusReportSchemeTestStep::doTestStepPostambleL();
		
	return TestStepResult();
	}

/**
Get the TP-SRR operations on the SMS 
*/
TVerdict CTestSendingTPSRRScheme::doTestStepPreambleL()
    {	
    //Call the base class preamble
    CTestSchemeSending::doTestStepPreambleL();

    INFO_PRINTF1(_L("Test sending a message with TP-SRR scheme"));

    return TestStepResult();
    }

/**
SIM TSY verifies the message sent
*/
TVerdict CTestSendingTPSRRScheme::doTestStepL()
    {
    SendSmsMessageL();

    return TestStepResult();
    }

/**
Send the message
*/
TVerdict CTestSendingTPSRRScheme::doTestStepPostambleL()
    {	
    //Call the base class preamble
    CTestSchemeSending::doTestStepPostambleL();

    return TestStepResult();
    }

/**
Get the SMSC operations on the SMS 
*/
TVerdict CTestSendingSMSCScheme::doTestStepPreambleL()
    {	
    //Call the base class preamble
    CTestSchemeSending::doTestStepPreambleL();

    INFO_PRINTF1(_L("Test sending a message with SMSC scheme"));

    return TestStepResult();
    }

/**
SIM TSY verifies the message sent
*/
TVerdict CTestSendingSMSCScheme::doTestStepL()
    {
    SendSmsMessageL();

    return TestStepResult();
    }


/**
Send the message
*/
TVerdict CTestSendingSMSCScheme::doTestStepPostambleL()
    {	
    //Call the base class preamble
    CTestSchemeSending::doTestStepPostambleL();

    return TestStepResult();
    }

/**
Preamble for all receiving test cases
*/
TVerdict CTestSchemeReceiving::doTestStepPreambleL()
	{
	//Call base class preamble
	CSmsStatusReportSchemeTestStep::doTestStepPreambleL();
		
	return TestStepResult();
	}

/**
All receiving of status reports
*/
TVerdict CTestReceivingStatusReports::doTestStepPreambleL()
	{
	//Call base class preamble
	CTestSchemeReceiving::doTestStepPreambleL();

	//Create SMS message from ini file specified as "message"
	CreateSmsL(iDataCodingScheme ,_L("message"));
			
	return TestStepResult();
	}

/**
  Receive a status report and return its status
*/
void CTestReceivingStatusReports::ReceiveStatusReportL(TSmsStatus::TSmsStatusValue& aStatus )
    {
    //Get the expectd status
    TInt intStatus;
    GetIntFromConfig(ConfigSection(), _L("status"), intStatus);
    iExpectedStatus = (TSmsStatus::TSmsStatusValue)intStatus;

    //Get the service center
    TSmsServiceCenterAddress telephoneNumber;
    telephoneNumber.Copy( KTestNumber );

    //Receive status report and return the status	
    aStatus = RecvStatusReportL(telephoneNumber, iSocket);
    }

/**
All receiving status reports tests do the same steps
*/
TVerdict CTestReceivingStatusReports::doTestStepL()
    {
    SendSmsMessageL();
    TSmsStatus::TSmsStatusValue status;
    ReceiveStatusReportL(status);
    TESTL(status == iExpectedStatus);

    return TestStepResult();
    }

/**
Delete the SMS 
*/
TVerdict CTestReceivingStatusReports::doTestStepPostambleL()
    {	
    delete iSmsMessage;
    iSmsMessage = NULL;

    CTestSchemeReceiving::doTestStepPostambleL();

    return TestStepResult();
    }

/**
All receiving of status reports
*/
TVerdict CTestReceivingMessages::doTestStepPreambleL()
    {
    //Call base class preamble
    CTestSchemeReceiving::doTestStepPreambleL();

    //Receive an SMS Message
    WaitForRecvL(iSocket);
    iSmsMessage = RecvSmsL(iSocket);

    return TestStepResult();
    }

/**
Delete the SMS 
*/
TVerdict CTestReceivingMessages::doTestStepPostambleL()
    {	
    delete iSmsMessage;
    iSmsMessage = NULL;

    CTestSchemeReceiving::doTestStepPostambleL();

    return TestStepResult();
    }
