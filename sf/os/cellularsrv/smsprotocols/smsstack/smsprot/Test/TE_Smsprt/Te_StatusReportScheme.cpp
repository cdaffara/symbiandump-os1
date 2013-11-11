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

#include "Te_StatusReportScheme.h"

#include <gsmuelem.h>

CSmsStatusReportSchemeTestStep::CSmsStatusReportSchemeTestStep()
    {

    }

CSmsStatusReportSchemeTestStep::~CSmsStatusReportSchemeTestStep()
    {

    }

TVerdict CSmsStatusReportSchemeTestStep::doTestStepPreambleL()
    {
    //Call base class preamble
    CSmsBaseTestStep::doTestStepPreambleL();

    iSmsStackTestUtils->OpenSmsSocketL(iSocketServer,iSocket,ESmsAddrRecvAny);

    SetCodingScheme();
    GetNumberOfPDUs();

    return TestStepResult() ;
    }

TVerdict CSmsStatusReportSchemeTestStep::doTestStepPostambleL()
    {
    iSocket.Close();

    CSmsBaseTestStep::doTestStepPostambleL();

    return TestStepResult() ;
    }

void CSmsStatusReportSchemeTestStep::OpenSocketServerL()
	{
	INFO_PRINTF1(_L("Connecting to SocketServer ..."));
	TInt ret=iSocketServer.Connect(KSocketMessageSlots);
	TEST(ret == KErrNone);

	iSmsStackTestUtils->OpenSmsSocketL(iSocketServer,iSocket,ESmsAddrRecvAny);
	}


void CSmsStatusReportSchemeTestStep::CreateSmsL(TCodingScheme aCodingScheme ,TPtrC aConfigSectionEntry)
	{
	TSmsDataCodingScheme::TSmsAlphabet alphabet;

	if (aCodingScheme == ESevenBit)
		{	
		//8 bit coding scheme
		alphabet = TSmsDataCodingScheme::ESmsAlphabet7Bit;
		}

	else 
		{
		alphabet = TSmsDataCodingScheme::ESmsAlphabet8Bit;
		}		
	
	TPtrC messageText;
	GetStringFromConfig(ConfigSection(), aConfigSectionEntry, messageText);

	iSmsMessage=CreateSmsMessageL(messageText,alphabet);
	} 

void CSmsStatusReportSchemeTestStep::TriggerReceiveL()
    {
    CreateSmsL(EEightBit,_L("trigger"));
    SendSmsMessageL();
    }

void CSmsStatusReportSchemeTestStep::SendSmsMessageL()
    {
    SendSmsL(iSmsMessage,iSocket);
    }

void CSmsStatusReportSchemeTestStep::SetCodingScheme()
    {
    TInt codingScheme;
    GetIntFromConfig(ConfigSection(), _L("messageCoding"), codingScheme);
    iDataCodingScheme = (TCodingScheme)codingScheme;	

    }

void CSmsStatusReportSchemeTestStep::GetNumberOfPDUs()
    {
    GetIntFromConfig(ConfigSection(), _L("numberOfPDUs"), iNumberOfPDUs);
    }

/**
Sets the TP-SRR bit in every PDU
 */
void CSmsStatusReportSchemeTestStep::SetAllTPSRRsL()
    {	
    CSmsTPSRROperations& TPSRROperations = static_cast<CSmsTPSRROperations&>(iSmsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));
    TPSRROperations.SetSchemeL();

    //Set TPSRR for all PDUs
    for(TInt i=0; i< iNumberOfPDUs; ++i)
        {
        TPSRROperations.SetSegmentStatusReportL(i,ETrue);
        }
    }

/**
Set TPSRR bit for the last segment of message
*/
void CSmsStatusReportSchemeTestStep::SetLastTPSRRL()
    {
    CSmsTPSRROperations& TPSRROperations = static_cast<CSmsTPSRROperations&>(iSmsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));
    TPSRROperations.SetSchemeL();
    TPSRROperations.SetLastSegmentStatusReportL(ETrue);	
    }

/**
Set TPSRR bit for some segments
*/
void CSmsStatusReportSchemeTestStep::SetSomeTPSRRL(TInt aFirstIndex, TInt aSecondIndex, TInt aThirdIndex)
    {
    CSmsTPSRROperations& TPSRROperations = static_cast<CSmsTPSRROperations&>(iSmsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));
    TPSRROperations.SetSchemeL();

    if(aFirstIndex)
        {
        TPSRROperations.SetSegmentStatusReportL(0,ETrue);		
        }
    else
        {
        TPSRROperations.SetSegmentStatusReportL(0,EFalse);	
        }

    if(aSecondIndex)
        {
        TPSRROperations.SetSegmentStatusReportL(1,ETrue);	
        }
    else
        {
        TPSRROperations.SetSegmentStatusReportL(1,EFalse);	
        }
    if(aThirdIndex)
        {
        TPSRROperations.SetSegmentStatusReportL(2,ETrue);	
        }
    else
        {
        TPSRROperations.SetSegmentStatusReportL(2,EFalse);	
        }

    }

/**
Set the last segment SMSC value to 0x01
*/
void CSmsStatusReportSchemeTestStep::SetLastSMSCL()
    {
    CSmsSMSCCtrlParameterOperations& SMSCOperations = static_cast<CSmsSMSCCtrlParameterOperations&>(iSmsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));
    SMSCOperations.SetSchemeL();
    SMSCOperations.SetStatusReportL(iNumberOfPDUs-1, 0x01);	
    }

/**
Set all segments SMSC value to 0x01
*/
void CSmsStatusReportSchemeTestStep::SetAllSMSCL()
    {
    CSmsSMSCCtrlParameterOperations& SMSCOperations = static_cast<CSmsSMSCCtrlParameterOperations&>(iSmsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));
    SMSCOperations.SetSchemeL();

    for(TInt i = 0; i < iNumberOfPDUs; ++i)
        {
        SMSCOperations.SetStatusReportL(i, 0x01);		
        }
    }

/**
Set SMSC bit for some segments
*/
void CSmsStatusReportSchemeTestStep::SetSomeSMSCL(TInt aFirstIndex, TInt aSecondIndex, TInt aThirdIndex)
    {
    CSmsSMSCCtrlParameterOperations& SMSCOperations = static_cast<CSmsSMSCCtrlParameterOperations&>(iSmsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));
    SMSCOperations.SetSchemeL();

    if(aFirstIndex)
        {
        SMSCOperations.SetStatusReportL(0,0x01);		
        }
    else
        {
        SMSCOperations.SetStatusReportL(0,0x00);	
        }

    if(aSecondIndex)
        {
        SMSCOperations.SetStatusReportL(1,0x01);	
        }
    else
        {
        SMSCOperations.SetStatusReportL(1,0x00);	
        }
    
    if(aThirdIndex)
        {
        SMSCOperations.SetStatusReportL(2,0x01);	
        }
    else
        {
        SMSCOperations.SetStatusReportL(2,0x00);	
        }
    }
