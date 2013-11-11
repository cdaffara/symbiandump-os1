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
// Test Wap Status reports
// 
//

/**
    @file
    @test
*/

#include "WapStatusReports.h"

#include <Gsmumsg.h>
#include <simtsy.h>

#include "WapProtSuiteDefs.h"

/**
Preamble for all status report steps
*/
TVerdict CTestWapStatusReports::doTestStepPreambleL()
    {
    CWapProtSuiteStepBase::doTestStepPreambleL();
    
    OpenSocketL(iSocketServer, iSocket);
    
    SetCodingSchemeL();
    INFO_PRINTF1(_L("Coding scheme set is successful ..."));
    SetMessageTypeL();
    INFO_PRINTF1(_L("Message type setup is successful ..."));
    ReadWapPortSettingsL(iWapAddr, KWapPort);
    INFO_PRINTF1(_L("Reading Wap port setting is successful ..."));
    
    //  Bind
    User::LeaveIfError(iSocket.Bind(iWapAddr));
    
    TTimeIntervalMicroSeconds32 InitPause=9000000;  //Required Pause to Allow SMSStack to Complete its Async Init
    User::After(InitPause);                         //call to the TSY and finish its StartUp.

    
    return TestStepResult();
    }

/**
Postamble for all status report steps
*/
TVerdict CTestWapStatusReports::doTestStepPostambleL()
    {
    iSocket.Close();

    CWapProtSuiteStepBase::doTestStepPostambleL();

    return TestStepResult();
    }

/**
Utility for sending a wap message
*/
void CTestWapStatusReports::SendWapMessageL()
    {
    //	Indicating to the protocol that it's a new client
    //INFO_PRINTF1(_L("Socket set option for indicating new client"));
    //User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionNewStyleClient,KWapSmsOptionLevel, 0));
    
    //Get the data 
    TPtrC textData;
    GetStringFromConfig(ConfigSection(),_L("message"), textData);
    
    //Set the buffer
    TBuf8<1000> data;
    data.Copy(textData);
    
    //Send the message
    TRequestStatus status;
    iSocket.SendTo(data, iWapAddr, 0, status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());
    INFO_PRINTF1(_L("Message sent!"));
    }

/**
Utility for getting the expected status
*/
void CTestWapStatusReports::GetExpectedStatus()
    {
    //Get the port number from the ini file
    GetIntFromConfig(ConfigSection(),_L("expectedStatus"), iExpectedStatus);
    }

/**
Utility for receiving status reports
*/
TSmsStatus::TSmsStatusValue CTestWapStatusReports::ReceiveStatusReportL()
    {
    INFO_PRINTF1(_L("waiting for incoming status report...") );
    WaitForRecvL(iStatusReportSocket);
    CSmsMessage* smsMessage = RecvSmsL(iStatusReportSocket);
    
    INFO_PRINTF1(_L("incoming SMS") );
    
    //Check the status report
    CleanupStack::PushL(smsMessage);
    TBool isSR = (smsMessage->Type()==CSmsPDU::ESmsStatusReport);
    
    if (isSR)
        {
        INFO_PRINTF1(_L("Received status report"));
        TSmsServiceCenterAddress telephoneNumber=smsMessage->ToFromAddress();
        }
    else
        {
        INFO_PRINTF1(_L("Received SMS is NOT a Status report!"));
        }
    
    TEST(isSR);
    
    //Get the status report
    CSmsStatusReport& statusReport = static_cast<CSmsStatusReport&>(smsMessage->SmsPDU()); 
    TSmsStatus::TSmsStatusValue status = statusReport.Status();
    
    CleanupStack::PopAndDestroy(smsMessage);
    
    return status;
    }

//-----------------------------------------------------------------------------

/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestRequestingStatusReports::doTestStepPreambleL()
    {	
    //Call the base class preamble
    CTestWapStatusReports::doTestStepPreambleL();
    
    return TestStepResult();
    }

/**
Common test steps for all status reports steps
*/
TVerdict CTestRequestingStatusReports::doTestStepL()
    {
    SendWapMessageL();
    
    return TestStepResult();
    }

//-----------------------------------------------------------------------------
/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestRequestingStatusReportsWithOption::doTestStepPreambleL()
    {	
    //Call the base class preamble
    CTestRequestingStatusReports::doTestStepPreambleL();
    
    INFO_PRINTF1(_L("Socket set option for indicating last PDU status report"));
    User::LeaveIfError(iSocket.SetOpt(KWapSmsStatusReportScheme, KWapSmsOptionLevel, EWapSmsTPSRR));
    
    return TestStepResult();
    }

//-----------------------------------------------------------------------------

/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestReceivingStatusReports::doTestStepPreambleL()
    {	
    //Call the base class preamble
    CTestWapStatusReports::doTestStepPreambleL();
    
    GetExpectedStatus();
    SetupStatusReportSocketL();
    
    return TestStepResult();
    }

/**
Common test steps for all status reports steps
*/
TVerdict CTestReceivingStatusReports::doTestStepL()
    {
    SendWapMessageL();
    TSmsStatus::TSmsStatusValue status;
    status=ReceiveStatusReportL();
    
    TESTL(status == iExpectedStatus);
    
    return TestStepResult();
    }

/**
Common test steps for all status reports steps
*/
TVerdict CTestReceivingStatusReports::doTestStepPostambleL()
    {
    iStatusReportSocket.Close();
    
    //Call the base class preamble
    CTestWapStatusReports::doTestStepPostambleL();
    
    return TestStepResult();
    }

//-----------------------------------------------------------------------------

/**
Common test steps for all status reports steps
*/
TVerdict CTestReceivingStatusReportsSevenBitMultiplePDUs::doTestStepL()
    {
    CTestReceivingStatusReports::doTestStepL();
    
    //We will receive 3 status reports for this message
    TSmsStatus::TSmsStatusValue status;
    status=ReceiveStatusReportL();
    TESTL(status == iExpectedStatus);
    
    status=ReceiveStatusReportL();
    TESTL(status == iExpectedStatus);
    return TestStepResult();
    }
//-----------------------------------------------------------------------------


TVerdict CTestReceivingStatusReportsWithOption::doTestStepPreambleL()
    {	
    //Sets up the socket for receiving status reports
    CTestReceivingStatusReports::doTestStepPreambleL();
    
    //Set the option for last PDU only (TPSRR Scheme)
    INFO_PRINTF1(_L("Socket set option for indicating last PDU status report"));
    User::LeaveIfError(iSocket.SetOpt(KWapSmsStatusReportScheme, KWapSmsOptionLevel, EWapSmsTPSRR));
    
    return TestStepResult();
    }

//-----------------------------------------------------------------------------
