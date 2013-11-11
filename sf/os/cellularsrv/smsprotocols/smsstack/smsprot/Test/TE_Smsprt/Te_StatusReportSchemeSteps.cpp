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

#include "Te_StatusReportSchemeSteps.h"


/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestSendingTPSRRSchemeInAllPDUs::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestSendingTPSRRScheme::doTestStepPreambleL();
		
	SetAllTPSRRsL();
	
	return TestStepResult();
	}


/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestSendingTPSRRSchemeInLastPDU::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestSendingTPSRRScheme::doTestStepPreambleL();
		
	CSmsTPSRROperations& TPSRROperations = static_cast<CSmsTPSRROperations&>(iSmsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));
	TPSRROperations.SetSchemeL();
	
	//For a seven bit coding, use the last segment method provided
	if(iDataCodingScheme == ESevenBit)
		{
		TPSRROperations.SetLastSegmentStatusReportL(ETrue);	
		}		//for an eight bit coding, use the specified index
	else
		{
		TPSRROperations.SetSegmentStatusReportL(iNumberOfPDUs-1,ETrue);	
		}
		
	//both methods should result in the same formated PDU
	return TestStepResult();
	}


/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestReceivingLastTPSRR::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestReceivingStatusReports::doTestStepPreambleL();
		
	SetLastTPSRRL();
	
	//both methods should result in the same formated PDU
	return TestStepResult();
	}


/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestReceivingTPSRRAllPDU::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestReceivingStatusReports::doTestStepPreambleL();
	
	SetAllTPSRRsL();
	
	//both methods should result in the same formated PDU
	return TestStepResult();
	}


/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestReceivingTPSRRSomePDU::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestReceivingStatusReports::doTestStepPreambleL();
	
	TInt firstIndex,secondIndex,thirdIndex;
	GetIntFromConfig(ConfigSection(), _L("firstIndex"), firstIndex);
	GetIntFromConfig(ConfigSection(), _L("secondIndex"), secondIndex);
	GetIntFromConfig(ConfigSection(), _L("thirdIndex"), thirdIndex);
	
	SetSomeTPSRRL(firstIndex,secondIndex,thirdIndex);
	
	//both methods should result in the same formated PDU
	return TestStepResult();
	}


/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestSendingSMSCSchemeSinglePDUNoBitSet::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestSendingSMSCScheme::doTestStepPreambleL();
		
	CSmsSMSCCtrlParameterOperations& smscOperations = static_cast<CSmsSMSCCtrlParameterOperations&>(iSmsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));
	smscOperations.SetSchemeL();
	smscOperations.SetStatusReportL(0, 0x00);	
	
	return TestStepResult();
	}


/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestSendingSMSCSchemeSinglePDUSomeBitsSet::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestSendingSMSCScheme::doTestStepPreambleL();
	
	CSmsSMSCCtrlParameterOperations& smscOperations = static_cast<CSmsSMSCCtrlParameterOperations&>(iSmsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));
	smscOperations.SetSchemeL();
		
	for(TInt ii=1; ii<16; ii++)
		{
		smscOperations.SetStatusReportL(0, ii);
		}
		
	for(TInt ii=16; ii<32; ii++)
		{
		TRAPD(err,smscOperations.SetStatusReportL(0, ii));
		TEST(err == KErrNotSupported);	
		}
	
	smscOperations.SetStatusReportL(0, ESmsSMSCControlParametersMask);
	return TestStepResult();
	}


/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestSendingSMSCSchemeMultiplePDUsSomeBitsSet::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestSendingSMSCScheme::doTestStepPreambleL();
	
	CSmsSMSCCtrlParameterOperations& smscOperations = static_cast<CSmsSMSCCtrlParameterOperations&>(iSmsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));
	smscOperations.SetSchemeL();
		
	if(iDataCodingScheme == ESevenBit)
		{
		smscOperations.SetStatusReportL(0, ESmsStatusReportPermanentError);
		smscOperations.SetStatusReportL(iNumberOfPDUs-1, ESmsStatusReportTemporaryError);
		
		}	//for an eight bit coding, use the specified index
	else
		{
		for (TInt ii=0; ii<iNumberOfPDUs; ii++)
			{
			smscOperations.SetStatusReportL(ii, 0x00);	
			}
		}
	
 	return TestStepResult();
	}


/**
Setup the SMS for SMSC operations on the SMS 
*/
TVerdict CTestReceivingLastSMSC::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestReceivingStatusReports::doTestStepPreambleL();
		
	SetLastSMSCL();
	
	//both methods should result in the same formated PDU
	return TestStepResult();
	}


/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestReceivingSMSCAllPDU::doTestStepPreambleL()
{	
	//Call the base class preamble
	CTestReceivingStatusReports::doTestStepPreambleL();
		
	SetAllSMSCL();
	
	//both methods should result in the same formated PDU
	return TestStepResult();
}


/**
Setup the SMS for TP-SRR operations on the SMS 
*/
TVerdict CTestReceivingSMSCSomePDU::doTestStepPreambleL()
{	
	//Call the base class preamble
	CTestReceivingStatusReports::doTestStepPreambleL();
	
	TInt firstIndex,secondIndex,thirdIndex;
	GetIntFromConfig(ConfigSection(), _L("firstIndex"), firstIndex);
	GetIntFromConfig(ConfigSection(), _L("secondIndex"), secondIndex);
	GetIntFromConfig(ConfigSection(), _L("thirdIndex"), thirdIndex);
	
	SetSomeSMSCL(firstIndex,secondIndex,thirdIndex);
	
	//both methods should result in the same formated PDU
	return TestStepResult();
}


/**
Setup the SMS for SMSC operations on the SMS 
*/
TVerdict CTestReceivingSMSCMultiplePDU::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestReceivingMessages::doTestStepPreambleL();
		
	//both methods should result in the same formated PDU
	return TestStepResult();
	}

/**
Setup the SMS for SMSC operations on the SMS 
*/
TVerdict CTestReceivingSMSCMultiplePDU::doTestStepL()
	{	
	//Get the expected selective status from the config file
	TInt expectedStatus1, expectedStatus2, expectedStatus3;
	GetHexFromConfig(ConfigSection(), _L("expectedStatus1"), expectedStatus1);
	GetHexFromConfig(ConfigSection(), _L("expectedStatus2"), expectedStatus2);
	GetHexFromConfig(ConfigSection(), _L("expectedStatus3"), expectedStatus3);
	
	//Check the scheme is SMSC
	TSmsStatusReportScheme scheme;
	scheme = iSmsMessage->Scheme();
	TESTL(scheme == EControlParametersScheme);
	
	//Check the selective status is as expected
	TUint8 selectiveStatus;
	TInt  ret;

	CSmsSMSCCtrlParameterOperations& smscOperations = static_cast<CSmsSMSCCtrlParameterOperations&>(iSmsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));
		
	ret = smscOperations.GetStatusReport(0, selectiveStatus);
	TEST(ret == KErrNone);	
	TEST(selectiveStatus == expectedStatus1);	
			
	ret = smscOperations.GetStatusReport(1, selectiveStatus);	
	TEST(ret == KErrNone);	
	TEST(selectiveStatus == expectedStatus2);	
	
	ret = smscOperations.GetStatusReport(2, selectiveStatus);	
	TEST(ret == KErrNone);	
	TEST(selectiveStatus == expectedStatus3);
			
	//both methods should result in the same formated PDU
	return TestStepResult();
	}
	

/**
Setup the SMS for SMSC operations on the SMS 
*/
TVerdict CTestReceivingSMSCSinglePDU::doTestStepPreambleL()
	{	
	//Call the base class preamble
	CTestReceivingMessages::doTestStepPreambleL();
		
	//both methods should result in the same formated PDU
	return TestStepResult();
	}

/**
Setup the SMS for SMSC operations on the SMS 
*/
TVerdict CTestReceivingSMSCSinglePDU::doTestStepL()
	{	
	//Get the expected selective status from the config file
	TInt expectedStatus;
	GetHexFromConfig(ConfigSection(), _L("expectedStatus"), expectedStatus);
			
	//Check the scheme is SMSC
	TSmsStatusReportScheme scheme;
	scheme = iSmsMessage->Scheme();
	TESTL(scheme == EControlParametersScheme);
	
	//Check the selective status is as expected
	TUint8 selectiveStatus;
	TInt  ret;

	CSmsSMSCCtrlParameterOperations& smscOperations = static_cast<CSmsSMSCCtrlParameterOperations&>(iSmsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));
	
	ret = smscOperations.GetStatusReport(0, selectiveStatus);	
	TEST(ret == KErrNone);
	TEST(selectiveStatus == expectedStatus);	
	
	//both methods should result in the same formated PDU
	return TestStepResult();
	}


TVerdict CTestReceivingDeliversWithTPSRR::doTestStepPreambleL()
{
	//Call the base class preamble
	CTestReceivingMessages::doTestStepPreambleL();
		
	//Get the expected selective status from the config file
	TPtrC expectedSchemeStr;
	GetStringFromConfig(ConfigSection(), _L("expectedScheme"), expectedSchemeStr);

	if(expectedSchemeStr.Compare(_L("TPSRR"))==0)
		iExpectedScheme = ETPSRRScheme;
	else
		iExpectedScheme = EDefaultScheme;
	
	return TestStepResult();
}

/**
Test the scheme of the deliver message
*/
TVerdict CTestReceivingDeliversWithTPSRR::doTestStepL()
{	
	TSmsStatusReportScheme scheme = iSmsMessage->Scheme();
	TESTL(scheme==iExpectedScheme);

	return TestStepResult();
}


/**
Test the scheme of the deliver message
*/
TVerdict CTestDeliversWithTPSRRScheme::doTestStepL()
{
	CSmsTPSRROperations& TPSRROperations = static_cast<CSmsTPSRROperations&>(iSmsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));	
	TSmsStatusReportScheme scheme = TPSRROperations.GetScheme();
	TESTL(scheme==iExpectedScheme);

	//Get the expected selective status from the config file
	TInt expectedStatus;
	GetIntFromConfig(ConfigSection(), _L("expectedStatus"), expectedStatus);

	TInt TPSRRIndex=0;
	TInt InvalidIndex=0;
	TInt NoTPSRRIndex=0;
	GetIntFromConfig(ConfigSection(), _L("TPSRRIndex"), TPSRRIndex);
	GetIntFromConfig(ConfigSection(), _L("InvalidIndex"), InvalidIndex);
	GetIntFromConfig(ConfigSection(), _L("NoTPSRRIndex"), NoTPSRRIndex);
		
	//Get the status
	TInt status=TPSRROperations.GetStatusReport(TPSRRIndex);
	TESTL(status==expectedStatus);
	
	//Get status for an invalid segment
	status=TPSRROperations.GetStatusReport(InvalidIndex);
	TESTL(status==KErrNotFound);

	//Get status for a segment not set
	status=TPSRROperations.GetStatusReport(NoTPSRRIndex);
	TESTL(status==KErrNone);

	return TestStepResult();
	
}

//-----------------------------------------------------------------------------
