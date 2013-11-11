// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SimConstants.h"
#include "Te_SimSatTest.h"

CSimSatTest::CSimSatTest() 
	{ 
	SetTestStepName(_L("SatTest"));
	}

TVerdict CSimSatTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginSatTest"));

 	CreateConfigFileL(_L("c:\\config4.txt")); 
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	// local variables used throughout the sat tests
	TRequestStatus reqStatusPCmd;
	TRequestStatus reqStatusRequired;
	TRequestStatus reqStatusAllowed;
	

	// Refresh
	RSat::TRefreshV1 refresh;
	RSat::TRefreshV1Pckg refreshPckg(refresh);
	
	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrNone);

	TESTL(refresh.iType==SIMTSY_REFRESH_TYPE1);
	TESTL(refresh.iFileList.Length()==1);
 	TESTL(refresh.iFileList[0]==SIMTSY_REFRESH_FILE1);
	INFO_PRINTF1(_L("SAT Refresh test 1 passed"));

	// Refresh Cancel
	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);
	iSat.CancelAsyncRequest(ESatNotifyRefreshPCmd);
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrCancel);
	INFO_PRINTF1(_L("SAT Refresh Cancel test passed"));

	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrNone);

	TESTL(refresh.iType==SIMTSY_REFRESH_TYPE2);
	TESTL(refresh.iFileList.Length()==1);
 	TESTL(refresh.iFileList[0]==SIMTSY_REFRESH_FILE2);
	INFO_PRINTF1(_L("SAT Refresh test 2 passed"));

	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrNone);

	TESTL(refresh.iType==SIMTSY_REFRESH_TYPE3);
	TESTL(refresh.iFileList.Length()==2);
 	TESTL(refresh.iFileList[0]==SIMTSY_REFRESH_FILE1);
 	TESTL(refresh.iFileList[1]==SIMTSY_REFRESH_FILE2);
	INFO_PRINTF1(_L("SAT Refresh test 3 passed"));

	// Refresh Required successful test
	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);

	iSat.NotifyRefreshRequired(reqStatusRequired);
	User::WaitForRequest(reqStatusRequired);
	TESTL(reqStatusRequired.Int()==KErrNone);
	
	// Refresh Required Response
	RSat::TRefreshRspV1* refreshAllowedRsp = new (ELeave) RSat::TRefreshRspV1;
	CleanupStack::PushL(refreshAllowedRsp);
	RSat::TRefreshRspV1Pckg* refreshAllowedRspPckg = new (ELeave) RSat::TRefreshRspV1Pckg(*refreshAllowedRsp);
	CleanupStack::PushL(refreshAllowedRspPckg);

	refreshAllowedRsp->iGeneralResult=SIMTSY_REFRESH_RESULT_SUCCESS;
	refreshAllowedRsp->iInfoType=SIMTSY_REFRESH_RSP_NO_INFO;
	refreshAllowedRsp->iAdditionalInfo=SIMTSY_REFRESH_NULL_BUF;

	iSat.RefreshAllowed(reqStatusAllowed, *refreshAllowedRspPckg);
	User::WaitForRequest(reqStatusAllowed);
	TESTL(reqStatusAllowed.Int()==KErrNone);

	// Wait for completion of outstanding PCmd now permission given
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrNone);

	TESTL(refresh.iType==SIMTSY_REFRESH_TYPE1);
	TESTL(refresh.iFileList.Length()==1);
 	TESTL(refresh.iFileList[0]==SIMTSY_REFRESH_FILE1);
	INFO_PRINTF1(_L("SAT NotifyRefreshRequired test 1 passed"));

	// Refresh Required unsuccessful test
	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);

	iSat.NotifyRefreshRequired(reqStatusRequired);
	User::WaitForRequest(reqStatusRequired);
	TESTL(reqStatusRequired.Int()==KErrNone);

	// Refresh Required Response
	refreshAllowedRsp->iGeneralResult=SIMTSY_REFRESH_RESULT_FAIL;

	iSat.RefreshAllowed(reqStatusAllowed, *refreshAllowedRspPckg);
	User::WaitForRequest(reqStatusAllowed);
	TESTL(reqStatusAllowed.Int()==KErrNone);

	// Cancel ESatNotifyRefreshPCmd due to failing RefreshAllowed
	iSat.CancelAsyncRequest(ESatNotifyRefreshPCmd);
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrCancel);
	INFO_PRINTF1(_L("SAT NotifyRefreshPCmd Cancel test passed"));

	// Refresh Required second successful test
	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);

	iSat.NotifyRefreshRequired(reqStatusRequired);
	User::WaitForRequest(reqStatusRequired);
	TESTL(reqStatusRequired.Int()==KErrNone);

	// Refresh Required Response
	refreshAllowedRsp->iGeneralResult=SIMTSY_REFRESH_RESULT_SUCCESS;

	iSat.RefreshAllowed(reqStatusAllowed, *refreshAllowedRspPckg);
	User::WaitForRequest(reqStatusAllowed);
	TESTL(reqStatusAllowed.Int()==KErrNone);

	// Wait for completion of outstanding PCmd now permission given
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrNone);

	TESTL(refresh.iType==SIMTSY_REFRESH_TYPE2);
	TESTL(refresh.iFileList.Length()==1);
 	TESTL(refresh.iFileList[0]==SIMTSY_REFRESH_FILE2);
	INFO_PRINTF1(_L("SAT NotifyRefreshRequired test 2 passed"));

	// Refresh Required third successful test
	iSat.NotifyRefreshRequired(reqStatusRequired);
	User::WaitForRequest(reqStatusRequired);
	TESTL(reqStatusRequired.Int()==KErrNone);

	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);

	// Refresh Required Response
	iSat.RefreshAllowed(reqStatusAllowed, *refreshAllowedRspPckg);
	User::WaitForRequest(reqStatusAllowed);
	TESTL(reqStatusAllowed.Int()==KErrNone);

	// Wait for completion of outstanding PCmd now permission given
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrNone);

	TESTL(refresh.iType==SIMTSY_REFRESH_TYPE3);
	TESTL(refresh.iFileList.Length()==2);
 	TESTL(refresh.iFileList[0]==SIMTSY_REFRESH_FILE1);
 	TESTL(refresh.iFileList[1]==SIMTSY_REFRESH_FILE2);
	INFO_PRINTF1(_L("SAT NotifyRefreshRequired test 3 passed"));

	// Refresh Required Cancel test
	iSat.NotifyRefreshRequired(reqStatusRequired);
	iSat.CancelAsyncRequest(ESatNotifyRefreshRequired);
	User::WaitForRequest(reqStatusRequired);
	TESTL(reqStatusRequired.Int()==KErrCancel);
	INFO_PRINTF1(_L("SAT NotifyRefreshRequired Cancel test passed"));

	// RefreshAllowed Cancel test
	iSat.RefreshAllowed(reqStatusAllowed, *refreshAllowedRspPckg);
	iSat.CancelAsyncRequest(ESatRefreshAllowed);
	User::WaitForRequest(reqStatusAllowed);
	// Test for either KErrNone or KErrCancel since we expect the request to be responded to immediately
	TESTL(reqStatusAllowed.Int()==KErrNone || reqStatusAllowed.Int()==KErrCancel);
	INFO_PRINTF1(_L("SAT RefreshAllowed Cancel test passed"));

	// Refresh Required test successful repeated with parameterised NotifyRefreshRequired
	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);

	// Refresh
	RSat::TRefreshV1 refreshRequired;
	RSat::TRefreshV1Pckg refreshRequiredPckg(refreshRequired);

	iSat.NotifyRefreshRequired(reqStatusRequired, refreshRequiredPckg);
	User::WaitForRequest(reqStatusRequired);
	TESTL(reqStatusRequired.Int()==KErrNone);

 	TESTL(refreshRequired.iType==SIMTSY_REFRESH_TYPE1);
	TESTL(refreshRequired.iFileList.Length()==1);
 	TESTL(refreshRequired.iFileList[0]==SIMTSY_REFRESH_FILE1);	

	refreshAllowedRsp->iGeneralResult=SIMTSY_REFRESH_RESULT_SUCCESS;
	refreshAllowedRsp->iInfoType=SIMTSY_REFRESH_RSP_NO_INFO;
	refreshAllowedRsp->iAdditionalInfo=SIMTSY_REFRESH_NULL_BUF;

	iSat.RefreshAllowed(reqStatusAllowed, *refreshAllowedRspPckg);

	User::WaitForRequest(reqStatusAllowed);
	TESTL(reqStatusAllowed.Int()==KErrNone);
	
	// Wait for completion of outstanding PCmd now permission given
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrNone);

	TESTL(refresh.iType==SIMTSY_REFRESH_TYPE1);
	TESTL(refresh.iFileList.Length()==1);
 	TESTL(refresh.iFileList[0]==SIMTSY_REFRESH_FILE1);
  
	INFO_PRINTF1(_L("SAT NotifyRefreshRequiredParam test 1 passed"));

	// Refresh Required unsuccessful test
	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);

	iSat.NotifyRefreshRequired(reqStatusRequired, refreshRequiredPckg);
	User::WaitForRequest(reqStatusRequired);
	TESTL(reqStatusRequired.Int()==KErrNone);

	// Refresh Required Response
	refreshAllowedRsp->iGeneralResult=SIMTSY_REFRESH_RESULT_FAIL;

	iSat.RefreshAllowed(reqStatusAllowed, *refreshAllowedRspPckg);
	User::WaitForRequest(reqStatusAllowed);
	TESTL(reqStatusAllowed.Int()==KErrNone);

 	// Cancel ESatNotifyRefreshPCmd due to failing RefreshAllowed
	iSat.CancelAsyncRequest(ESatNotifyRefreshPCmd);
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrCancel);
	INFO_PRINTF1(_L("SAT NotifyRefreshPCmd Cancel test passed"));

	// Refresh Required second successful test
	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);

	iSat.NotifyRefreshRequired(reqStatusRequired, refreshRequiredPckg);
	User::WaitForRequest(reqStatusRequired);
	TESTL(reqStatusRequired.Int()==KErrNone);

 	TESTL(refreshRequired.iType==SIMTSY_REFRESH_TYPE2);
	TESTL(refreshRequired.iFileList.Length()==1);
 	TESTL(refreshRequired.iFileList[0]==SIMTSY_REFRESH_FILE2);

	// Refresh Required Response
	refreshAllowedRsp->iGeneralResult=SIMTSY_REFRESH_RESULT_SUCCESS;

	iSat.RefreshAllowed(reqStatusAllowed, *refreshAllowedRspPckg);
	User::WaitForRequest(reqStatusAllowed);
	TESTL(reqStatusAllowed.Int()==KErrNone);

	// Wait for completion of outstanding PCmd now permission given
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrNone);

	TESTL(refresh.iType==SIMTSY_REFRESH_TYPE2);
	TESTL(refresh.iFileList.Length()==1);
 	TESTL(refresh.iFileList[0]==SIMTSY_REFRESH_FILE2);
 	 	
	INFO_PRINTF1(_L("SAT NotifyRefreshRequiredParam test 2 passed"));

	// Refresh Required third successful test
	iSat.NotifyRefreshRequired(reqStatusRequired, refreshRequiredPckg);
	User::WaitForRequest(reqStatusRequired);
	TESTL(reqStatusRequired.Int()==KErrNone);

  	TESTL(refreshRequired.iType==SIMTSY_REFRESH_TYPE3);
	TESTL(refreshRequired.iFileList.Length()==2);
 	TESTL(refreshRequired.iFileList[0]==SIMTSY_REFRESH_FILE1);
  	TESTL(refreshRequired.iFileList[1]==SIMTSY_REFRESH_FILE2);

	iSat.NotifyRefreshPCmd(reqStatusPCmd,refreshPckg);

	// Refresh Required Response
	iSat.RefreshAllowed(reqStatusAllowed, *refreshAllowedRspPckg);
	User::WaitForRequest(reqStatusAllowed);
	TESTL(reqStatusAllowed.Int()==KErrNone);

	// Wait for completion of outstanding PCmd now permission given
	User::WaitForRequest(reqStatusPCmd);
	TESTL(reqStatusPCmd.Int()==KErrNone);

  	TESTL(refreshRequired.iType==SIMTSY_REFRESH_TYPE3);
	TESTL(refreshRequired.iFileList.Length()==2);
 	TESTL(refreshRequired.iFileList[0]==SIMTSY_REFRESH_FILE1);
  	TESTL(refreshRequired.iFileList[1]==SIMTSY_REFRESH_FILE2);
 	 	
	INFO_PRINTF1(_L("SAT NotifyRefreshRequiredParam test 3 passed"));

	// Refresh Required Cancel test
	iSat.NotifyRefreshRequired(reqStatusRequired, refreshRequiredPckg);
	iSat.CancelAsyncRequest(ESatNotifyRefreshRequiredParam);
	User::WaitForRequest(reqStatusRequired);
	TESTL(reqStatusRequired.Int()==KErrNone || reqStatusRequired.Int()==KErrCancel);
	INFO_PRINTF1(_L("SAT NotifyRefreshRequired Cancel test passed"));

	// RefreshAllowed Cancel test
	iSat.RefreshAllowed(reqStatusAllowed, *refreshAllowedRspPckg);
	iSat.CancelAsyncRequest(ESatRefreshAllowed);
	User::WaitForRequest(reqStatusAllowed);
	// Test for either KErrNone or KErrCancel since we expect the request to be responded to immediately
	TESTL(reqStatusAllowed.Int()==KErrNone || reqStatusAllowed.Int()==KErrCancel);
	INFO_PRINTF1(_L("SAT RefreshAllowed Cancel test passed"));

	CleanupStack::PopAndDestroy(2);

	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}
	
	
CSimSatRetMMPCmdTest::CSimSatRetMMPCmdTest() 
	{ 
	SetTestStepName(_L("SatTestRetMMPCmd"));
	}


TVerdict CSimSatRetMMPCmdTest::doTestStepL()
	{
	INFO_PRINTF1(_L("RETRIEVE MULTIMEDIA PCMD TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8501L();	// For testing the TEL-SIMTSY-SAT-8501 
	testSAT8502L();	// For testing the TEL-SIMTSY-SAT-8502 
	testSAT8503L();	// For testing the TEL-SIMTSY-SAT-8503 
	testSAT8504L();	// For testing the TEL-SIMTSY-SAT-8504 
	testSAT8505L();	// For testing the TEL-SIMTSY-SAT-8505 
	testSAT8506L();	// For testing the TEL-SIMTSY-SAT-8506 
	testSAT8507L();	// For testing the TEL-SIMTSY-SAT-8507
	testSAT8508L();	// For testing the TEL-SIMTSY-SAT-8508
	
	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("RETRIEVE MULTIMEDIA PCMD TESTING - END"));

	return TestStepResult();
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8501
 *
 *	SYMREQ REQ7329
 *
 *	SYMTestCaseDesc UICC issues RETRIEVE MULTIMEDIA Proactive command to
 *	ME successfully
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i) 	Client requests notification for RETRIEVE MM using RSat::NotifyRetrieveMultimediaMsgPCmd() API.
 *		(ii)	UICC issues RETRIEVE MULTIMEDIA MESSAGE command to the ME.
 *		(iii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ()
 *
 *	SYMTestExpectedResults 
 *	ME is notified of the request.
 *	Upon completion of the proactive command, details are returned in a 
 *	packaged version of TRetrieveMultimediaMessageV6 in the command 
 *	parameter. 
 *	ME responds with KSuccess in the Terminal response command for the 
 *	TRequestStatus variable
 *
 * 	SYMTestType CIT
 */

TInt CSimSatRetMMPCmdTest::testSAT8501L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8501"));
	
	TRequestStatus reqStatus;

	RSat::TRetrieveMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TRetrieveMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TRetrieveMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) RSat::TRetrieveMultimediaMessageV6Pckg
											(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyRetrieveMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	TESTL(tMMMessageV6->iDestination == SIMTSY_DEVICEID_ME);
	TESTL(tMMMessageV6->iAlphaId.iStatus == SIMTSY_ALPHAID_STATUS);
	TESTL(tMMMessageV6->iAlphaId.iAlphaId.Compare(SIMTSY_ALPHAID_BUF) == 0);
	TESTL(tMMMessageV6->iIconId.iQualifier == SIMTSY_ICONID_QLFR);
	TESTL(tMMMessageV6->iIconId.iIdentifier == SIMTSY_ICONID_ID);
	TESTL(tMMMessageV6->iMultimediaMessageRef.Compare(SIMTSY_MMMSG_REF_FILES) ==0);
	TESTL(tMMMessageV6->iReceptionFile.Compare(SIMTSY_RCPTION_FILES) ==0);
	TESTL(tMMMessageV6->iMultimediaContentId.Compare(SIMTSY_MMCONTENT_ID_DATA) ==0);
	TESTL(tMMMessageV6->iMultimediaMessageId.Compare(SIMTSY_MMMSG_ID_DATA) ==0);
	TESTL(tMMMessageV6->iTextAttribute.iStatus == SIMTSY_TXT_ATTR_STATUS);
	TESTL(tMMMessageV6->iTextAttribute.iTextAttributeData.Compare(SIMTSY_TXT_ATTR_DATA) ==0);
	
	RSat::TRetrieveMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TRetrieveMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TRetrieveMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave) 
		RSat::TRetrieveMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_RET_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ERetrieveMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()== KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8501 - Successful"));
	return KErrNone;

	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8502
 *
 *	SYMREQ REQ7329
 *
 *	SYMTestCaseDesc Retrieve  Multimedia command rejected by the ME as ME is busy on a MMS transaction.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *	(i)	Client requests notification for RETRIEVE MULTIMEDIA MESSAGE using NotifyRetrieveMultimediaMsgPCmd()
 *		API.
 *	(ii)	UICC issues RETRIEVE MULTIMEDIA MESSAGE command to the ME.
 *	(iii)	ME is unable to execute the command as its busy processing some other MMS transaction.
 *	(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *	ME is notified of the request.
 *	Upon completion of the proactive command, details are returned in a packaged version of 
 *	TRetrieveMultimediaMessageV6 in the command parameter. 
 *	ME responds with KMMSTemporaryProblem in the Terminal response as the TPCmdResult with the 
 *	additional information ‘KNoSpecificMeProblem’.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatRetMMPCmdTest::testSAT8502L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8502"));
	
	TRequestStatus reqStatus;

	RSat::TRetrieveMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TRetrieveMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TRetrieveMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) RSat::TRetrieveMultimediaMessageV6Pckg
										(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyRetrieveMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	TESTL(tMMMessageV6->iDestination == SIMTSY_DEVICEID_ME);
	TESTL(tMMMessageV6->iAlphaId.iStatus == SIMTSY_ALPHAID_STATUS);
	TESTL(tMMMessageV6->iAlphaId.iAlphaId.Compare(SIMTSY_ALPHAID_BUF) == 0);
	TESTL(tMMMessageV6->iIconId.iQualifier == SIMTSY_ICONID_QLFR);
	TESTL(tMMMessageV6->iIconId.iIdentifier == SIMTSY_ICONID_ID);
	TESTL(tMMMessageV6->iMultimediaMessageRef.Compare(SIMTSY_MMMSG_REF_FILES) ==0);
	TESTL(tMMMessageV6->iReceptionFile.Compare(SIMTSY_RCPTION_FILES) ==0);
	TESTL(tMMMessageV6->iMultimediaContentId.Compare(SIMTSY_MMCONTENT_ID_DATA) ==0);
	TESTL(tMMMessageV6->iMultimediaMessageId.Compare(SIMTSY_MMMSG_ID_DATA_2) ==0);
	TESTL(tMMMessageV6->iTextAttribute.iStatus == SIMTSY_TXT_ATTR_STATUS);
	TESTL(tMMMessageV6->iTextAttribute.iTextAttributeData.Compare(SIMTSY_TXT_ATTR_DATA) ==0);
	
	RSat::TRetrieveMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TRetrieveMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TRetrieveMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = 
		new (ELeave) RSat::TRetrieveMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	_LIT(KText,"KNoSpecificMeProblem");
	TBuf<254> buf(KText);
	tMMMessageRspV6->iGeneralResult = RSat::KMMSTemporaryProblem;
	tMMMessageRspV6->iInfoType = RSat::KMeProblem;
	tMMMessageRspV6->iAdditionalInfo = buf;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_RET_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ERetrieveMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8502 - Successful"));
	return KErrNone;

	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8503
 *	
 *	SYMREQ REQ REQ7329
 *	
 *	SYMTestCaseDesc 	RETRIEVE MULTIMEDIA MESSAGE command accepted 
 *	on the second attempt after the first attempt being replied as 
 *	ME busy on MMS transaction.
 *	
 *	SYMTestPriority High
 *
 * 	SYMTestActions 
 *	(i)	Client requests notification for RETRIEVE MULTIMEDIA MESSAGE 
 *		using NotifyRetrieveMultimediaMsgPCmd () API.
 *	(ii)	UICC issues RETRIEVE MULTIMEDIA MESSAGE Command to the ME.
 *	(iii)	ME is unable to execute the command as its busy processing 
 *		some other MMS transaction.
 *	(iv)	ME sends Terminal Response to the UICC using 
 *		RSat::TerminalRsp ().
 *	(v)	Client requests notification for RETRIEVE MULTIMEDIA MESSAGE 
 *		using NotifyRetrieveMultimediaMsgPCmd () API again.
 *	(vi)	UICC makes another attempt by resending the proactive command.
 *		This time Clients responds to execute the command successfully.
 *	(vii)	ME sends Terminal Response to the UICC using 
 *		RSat::TerminalRsp ().
 *	
 *	SYMTestExpectedResults 
 *	Client is notified of the request.
 *	ME responds with KMMSTemporaryProblem in the response parameter. 
 *	Upon completion of the proactive command in the second attempt, 
 *	details are returned in a packaged version of 
 *	TRetrieveMultimediaMessageV6 in the command parameter. 
 *	For the second attempt, Client responds with KSuccess in the 
 *	response parameter
 *
 * 	SYMTestType CIT
 */
TInt CSimSatRetMMPCmdTest::testSAT8503L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8503"));
	
	TRequestStatus reqStatus;

	RSat::TRetrieveMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TRetrieveMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TRetrieveMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) RSat::TRetrieveMultimediaMessageV6Pckg
			(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyRetrieveMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	RSat::TRetrieveMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TRetrieveMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TRetrieveMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 =
	       new (ELeave) RSat::TRetrieveMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	reqStatus = KErrNone;
	
	_LIT(KText,"KNoSpecificMeProblem");
	TBuf<254> buf(KText);
	tMMMessageRspV6->iGeneralResult = RSat::KMMSTemporaryProblem;
	tMMMessageRspV6->iInfoType = RSat::KMeProblem;
	tMMMessageRspV6->iAdditionalInfo = buf;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_RET_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ERetrieveMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	iSat.NotifyRetrieveMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_RET_MM_PCMD_NUMBER);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8503 - Successful"));
	return KErrNone;

	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8504
 *
 *	SYMREQ REQ7329
 *
 *	SYMTestCaseDesc ME rejects  Retrieve  Multimedia command as the ME is unable to process the MMS transaction
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for RETRIEVE MULTIMEDIA MESSAGE using 
 *			NotifyRetrieveMultimediaMsgPCmd () API.
 *		(ii)	UICC issues RETRIEVE MULTIMEDIA MESSAGE Command to the ME.
 *		(iii)	Client is unable to process the MMS transaction.
 *		(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *	Client is notified of the request.
 *	Upon completion of the proactive command details are returned in a packaged version of 
 *	TRetrieveMultimediaMessageV6 in the command parameter. 
 *	ME responds with KMMSError  in the Terminal response as the TPCmdResult with the 
 *	additional information ‘KNoSpecificMeProblem’.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatRetMMPCmdTest::testSAT8504L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8504"));
	TRequestStatus reqStatus;

	RSat::TRetrieveMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TRetrieveMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TRetrieveMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TRetrieveMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyRetrieveMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	RSat::TRetrieveMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TRetrieveMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TRetrieveMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave) 
		RSat::TRetrieveMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	_LIT(KText,"KNoSpecificMeProblem");
	TBuf<254> buf(KText);
	tMMMessageRspV6->iGeneralResult = RSat::KMMSError;
	tMMMessageRspV6->iInfoType = RSat::KMeProblem;
	tMMMessageRspV6->iAdditionalInfo = buf;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_RET_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ERetrieveMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8504 - Successful"));
	return KErrNone;

	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8505
 *
 *	SYMREQ REQ7329
 *
 *	SYMTestCaseDesc Cancel notification of RETRIEVE MULTIMEDIA MESSAGE request
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Register to be notified of RETRIEVE MULTIMEDIA MESSAGE 
 *			command using NotifyRetrieveMultimediaMsgPCmd () API.
 *		(ii)	Cancel the request by calling CancelAsyncRequest (ESatNotifyRetrieveMultimediaMessagePCmd).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */

TInt CSimSatRetMMPCmdTest::testSAT8505L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8505"));
	
	TRequestStatus reqStatus;

	RSat::TRetrieveMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TRetrieveMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TRetrieveMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TRetrieveMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyRetrieveMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	iSat.CancelAsyncRequest(ESatNotifyRetrieveMultimediaMsgPCmd);
	User::WaitForRequest(reqStatus);
	
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));

	CleanupStack::PopAndDestroy(2);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8505 - Successful"));
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8506
 *
 *	SYMREQ REQ7329
 *
 *	SYMTestCaseDesc Cancel the terminal response for the request placed for notification of 
 *			RETRIEVE MULTIMEDIA MESSAGE request
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Register to be notified of RETRIEVE MULTIMEDIA MESSAGE 
 *			command using NotifyRetrieveMultimediaMsgPCmd () API.
 *		(ii)	Call the Terminal response() API with the TPCmd as ERetrieveMultimediaMsg
 *		(iii)	Cancel the request by calling CancelAsyncRequest (ESatTerminalRsp).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatRetMMPCmdTest::testSAT8506L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8506"));

	TRequestStatus reqStatus;

	RSat::TRetrieveMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TRetrieveMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TRetrieveMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TRetrieveMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyRetrieveMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	RSat::TRetrieveMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TRetrieveMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TRetrieveMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
	       	RSat::TRetrieveMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_RET_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ERetrieveMultimediaMsg,*(tMMMessageRspPckgV6));
	iSat.CancelAsyncRequest(ESatTerminalRsp);
	User::WaitForRequest(reqStatus);
	
	TESTL((reqStatus.Int() == KErrNone) || (reqStatus.Int() ==  KErrCancel));
	
	CleanupStack::PopAndDestroy(4);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8506 - Successful"));
	return KErrNone;

	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8507
 *
 *	SYMREQ REQ7329 and REQ7334
 *
 *	SYMTestCaseDesc To verify the event of the envelope command for MMS TRANSFER STATUS envelope command 
 *			after the RETRIEVE MULTIMEDIA MESSAGE proactive command.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *	(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference.
 *	(ii)	Call the API RSat::ClientSatProfileIndication using the TSatProfileV6 with TSatProfileByte22 
 *		set to KCapsRetrieveMultimediaMessage and TSatProfileByte25 set to KCapsMmsTransferStatusEvent.
 *	(iii)	Client requests notification for RETRIEVE MULTIMEDIA MESSAGE using 
 *		NotifyRetrieveMultimediaMsgPCmd () API.
 *	(iv)	UICC issues RETRIEVE MULTIMEDIA MESSAGE Command to the ME.
 *	(v)	ME is able to process the MMS transaction and the response is sent using RSat::TerminalRsp ()
 *		for NotifyRetrieveMultimediaMsgPCmd.
 *	(vi)	Call the API RSat::MMSTransferStatus with the parameter as object of TMMTransferStatusV6 
 *		to store the MM1_retrieve.RES message on to the UICC.
 *
 *	SYMTestExpectedResults 
 *		GetMeSideSatProfile shall return the RSat::TSatProfileV6 with TSatProfileByte22 set to 
 *		KCapsRetrieveMultimediaMessage and TSatProfileByte25 set to KCapsMmsTransferStatusEvent.
 *		The ClientSideSatProfile shall return with KErrNone.
 *		The envelope command MMS TRANSFER STATUS is passed on from ME to UICC successfully.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatRetMMPCmdTest::testSAT8507L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8507"));
	
	TRequestStatus reqStatus;

	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(profileV6->iSatProfileByte22 & (RSat::KCapsRetrieveMultimediaMessage));
	TESTL(profileV6->iSatProfileByte25 & (RSat::KCapsMmsTransferStatusEvent));
	
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TRetrieveMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TRetrieveMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TRetrieveMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TRetrieveMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyRetrieveMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TRetrieveMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TRetrieveMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TRetrieveMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
	       	RSat::TRetrieveMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_RET_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ERetrieveMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	RSat::TMmsTransferStatusV6 *tMMSTransferStsV6 = new (ELeave) RSat::TMmsTransferStatusV6;
	CleanupStack::PushL(tMMSTransferStsV6);
	
	RSat::TMmsTransferStatusV6Pckg *tMMSTransferStsV6Pckg = new (ELeave)
	       	RSat::TMmsTransferStatusV6Pckg(*tMMSTransferStsV6);
	CleanupStack::PushL(tMMSTransferStsV6Pckg);
	
	_LIT(KText,"TransferFilePath");
	TBuf<255> trfFileBuf(KText);
	_LIT(KText1,"MMMessageBuf");
	TBuf<255> trfFileMMMsgBuf(KText1);
	_LIT(KText2,"MMMessageIdentifier");
	TBuf<255> trfFileMMMsgId(KText2);
	
	tMMSTransferStsV6->iDeviceId = RSat::KME;
	tMMSTransferStsV6->iMMSTransferFile = trfFileBuf;
	tMMSTransferStsV6->iMultimediaMessageStatus = trfFileMMMsgBuf;
	tMMSTransferStsV6->iMultimediaMessageId = trfFileMMMsgId ;
	
	iSat.MmsTransferStatus(reqStatus,*tMMSTransferStsV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	CleanupStack::PopAndDestroy(8);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8507 - Successful"));
	return KErrNone;

	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8508
 *
 *	SYMREQ REQ7329 and REQ7334
 *
 *	SYMTestCaseDesc To verify the event of cancellation of the envelope command for MMS TRANSFER STATUS 
 *			envelope command after the RETRIEVE MULTIMEDIA MESSAGE Proactive command is placed.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for RETRIEVE MULTIMEDIA MESSAGE 
 *			using NotifyRetrieveMultimediaMsgPCmd () API.
 *		(ii)	UICC issues RETRIEVE MULTIMEDIA MESSAGE Command to the ME.
 *		(iii)	ME is able to process the MMS transaction and the response is sent 
 *			using RSat::TerminalRsp  for NotifyRetrieveMultimediaMsgPCmd.
 *		(iv)	Call the API RSat::MMSTransferStatus with the parameter as object of TMMTransferStatusV6.
 *		(v)	Cancel the request for the envelope command API placed using 
 *			RSat::CancelAsyncRequest(ESatMMTransferStatus).
 *		(vi)	ME is able to send the envelope command MULTIMEDIA TRANSFER STATUS 
 *		and the response is sent using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */

TInt CSimSatRetMMPCmdTest::testSAT8508L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8508"));
	
	TRequestStatus reqStatus;

	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(profileV6->iSatProfileByte22 & (RSat::KCapsRetrieveMultimediaMessage));
	TESTL(profileV6->iSatProfileByte25 & (RSat::KCapsMmsTransferStatusEvent));
	
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TRetrieveMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TRetrieveMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TRetrieveMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TRetrieveMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyRetrieveMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TRetrieveMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TRetrieveMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TRetrieveMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
	       	RSat::TRetrieveMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_RET_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ERetrieveMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	RSat::TMmsTransferStatusV6 *tMMSTransferStsV6 = new (ELeave) RSat::TMmsTransferStatusV6;
	CleanupStack::PushL(tMMSTransferStsV6);
	
	RSat::TMmsTransferStatusV6Pckg *tMMSTransferStsV6Pckg = new (ELeave) 
		RSat::TMmsTransferStatusV6Pckg(*tMMSTransferStsV6);
	CleanupStack::PushL(tMMSTransferStsV6Pckg);
	
	_LIT(KText,"TransferFilePath");
	TBuf<255> trfFileBuf(KText);
	_LIT(KText1,"MMMessageBuf");
	TBuf<255> trfFileMMMsgBuf(KText1);
	_LIT(KText2,"MMMessageIdentifier");
	TBuf<255> trfFileMMMsgId(KText2);
	
	tMMSTransferStsV6->iDeviceId = RSat::KME;
	tMMSTransferStsV6->iMMSTransferFile = trfFileBuf;
	tMMSTransferStsV6->iMultimediaMessageStatus = trfFileMMMsgBuf;
	tMMSTransferStsV6->iMultimediaMessageId = trfFileMMMsgId ;
	
	iSat.MmsTransferStatus(reqStatus,*tMMSTransferStsV6Pckg);
	iSat.CancelAsyncRequest(ESatMmsTransferStatus);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));
	
	CleanupStack::PopAndDestroy(8);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8508 - Successful"));
	return KErrNone;
	}


CSimSatSubMMPCmdTest::CSimSatSubMMPCmdTest() 
	{ 
	SetTestStepName(_L("SatTestSubMMPCmd"));
	}

TVerdict CSimSatSubMMPCmdTest::doTestStepL()
	{
	INFO_PRINTF1(_L("SUBMIT MULTIMEDIA PCMD TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8509L();	// For testing the TEL-SIMTSY-SAT-8509
	testSAT8510L();	// For testing the TEL-SIMTSY-SAT-8510 
	testSAT8511L();	// For testing the TEL-SIMTSY-SAT-8511 
	testSAT8512L();	// For testing the TEL-SIMTSY-SAT-8512 
	testSAT8513L();	// For testing the TEL-SIMTSY-SAT-8513 
	testSAT8514L();	// For testing the TEL-SIMTSY-SAT-8514 
	testSAT8515L();	// For testing the TEL-SIMTSY-SAT-8515
			
	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("SUBMIT MULTIMEDIA PCMD TESTING - END"));

	return TestStepResult();
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8509
 *
 * 	SYMREQ REQ7330
 *
 *	SYMTestCaseDesc UICC issues SUBMIT MULTIMEDIA MESSAGE command to ME successfully.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for SUBMIT MULTIMEDIA MESSAGE
 *			using RSat::NotifySubmitMultimediaMsgPCmd () API. 
 *		(ii)	UICC issues SUBMIT MULTIMEDIA MESSAGE command to the ME.
 *		(iii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 * 	SYMTestExpectedResults 
 * 	Client is notified of the request.
 * 	Upon completion of the proactive command details are returned in a packaged version of 
 * 	TSubmitMultimediaMessageV6 in the command parameter. 
 * 	ME responds with KSuccess as the response parameter
 *
 * 	SYMTestType CIT
 */

TInt CSimSatSubMMPCmdTest::testSAT8509L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8509"));

	TRequestStatus reqStatus;

	RSat::TSubmitMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TSubmitMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TSubmitMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TSubmitMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifySubmitMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	TESTL(tMMMessageV6->iDestination == SIMTSY_DEVICEID_ME);
	TESTL(tMMMessageV6->iAlphaId.iStatus == SIMTSY_ALPHAID_STATUS);
	TESTL(tMMMessageV6->iAlphaId.iAlphaId.Compare(SIMTSY_ALPHAID_BUF) == 0);
	TESTL(tMMMessageV6->iIconId.iQualifier == SIMTSY_ICONID_QLFR);
	TESTL(tMMMessageV6->iIconId.iIdentifier == SIMTSY_ICONID_ID);
	TESTL(tMMMessageV6->iSubmissionFile.Compare(SIMTSY_SUBMISSION_FILES) ==0);
	TESTL(tMMMessageV6->iMultimediaMessageId.Compare(SIMTSY_MMMSG_ID_DATA) ==0);
	TESTL(tMMMessageV6->iTextAttribute.iStatus == SIMTSY_TXT_ATTR_STATUS);
	TESTL(tMMMessageV6->iTextAttribute.iTextAttributeData.Compare(SIMTSY_TXT_ATTR_DATA) ==0);
	
	RSat::TSubmitMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TSubmitMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TSubmitMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave) 
		RSat::TSubmitMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_SUB_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESubmitMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8509 - Successful"));
	return KErrNone;

	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8510
 *
 *	SYMREQ REQ7330
 *
 *	SYMTestCaseDesc Submit  Multimedia command rejected by the client as the ME is busy on a MMS transaction
 *	
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for SUBMIT MULTIMEDIA MESSAGE using 
 *			NotifySubmitMultimediaMsgPCmd () API.
 *		(ii)	UICC issues SUBMIT MULTIMEDIA MESSAGE command to the ME.
 *		(iii)	ME is unable to execute the command as its busy processing some other MMS transaction.
 *		(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request.
 *		Upon completion of the proactive command details are returned in a packaged version of 
 *		TSubmitMultimediaMessageV6 in the command parameter. 
 *		ME responds with KMMSTemporaryProblem in the Terminal response as the TPCmdResult 
 *		with the additional information ‘KNoSpecificMeProblem’.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatSubMMPCmdTest::testSAT8510L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8510"));
	
	TRequestStatus reqStatus;

	RSat::TSubmitMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TSubmitMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TSubmitMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	    RSat::TSubmitMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifySubmitMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	TESTL(tMMMessageV6->iDestination == SIMTSY_DEVICEID_ME);
	TESTL(tMMMessageV6->iAlphaId.iStatus == SIMTSY_ALPHAID_STATUS);
	TESTL(tMMMessageV6->iAlphaId.iAlphaId.Compare(SIMTSY_ALPHAID_BUF) == 0);
	TESTL(tMMMessageV6->iIconId.iQualifier == SIMTSY_ICONID_QLFR);
	TESTL(tMMMessageV6->iIconId.iIdentifier == SIMTSY_ICONID_ID);
	TESTL(tMMMessageV6->iSubmissionFile.Compare(SIMTSY_SUBMISSION_FILES) ==0);
	TESTL(tMMMessageV6->iMultimediaMessageId.Compare(SIMTSY_MMMSG_ID_DATA) ==0);
	TESTL(tMMMessageV6->iTextAttribute.iStatus == SIMTSY_TXT_ATTR_STATUS);
	TESTL(tMMMessageV6->iTextAttribute.iTextAttributeData.Compare(SIMTSY_TXT_ATTR_DATA) ==0);
	
	RSat::TSubmitMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TSubmitMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TSubmitMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave) 
		RSat::TSubmitMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	_LIT(KText,"KNoSpecificMeProblem");
	TBuf<254> buf(KText);
	tMMMessageRspV6->iGeneralResult = RSat::KMMSTemporaryProblem;
	tMMMessageRspV6->iInfoType = RSat::KMeProblem;
	tMMMessageRspV6->iAdditionalInfo = buf;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_SUB_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESubmitMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8510 - Successful"));
	return KErrNone;

	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8511
 *
 *	SYMREQ REQ7330
 *
 *	SYMTestCaseDesc Submit  Multimedia command accepted on the second attempt
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for SUBMIT MULTIMEDIA MESSAGE using
 *			NotifySubmitMultimediaMsgPCmd () API.
 *		(ii)	UICC issues SUBMIT MULTIMEDIA MESSAGE Command to the ME.
 *		(iii)	ME is unable to execute the command as its busy processing some other MMS transaction.
 *		(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *		(v)	Client requests notification for SUBMIT MULTIMEDIA MESSAGE using 
 *			NotifySubmitMultimediaMsgPCmd () API again.
 *		(vi)	UICC makes another attempt by resending the proactive command.  
 *			This time ME responds to execute the command successfully.
 *		(vii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request.
 *		Upon completion of the proactive command details are returned in a packaged 
 *		version of TSubmitMultimediaMessageV6 in the command parameter. 
 *		ME responds with KMMSTemporaryProblem in the Terminal response as the TPCmdResult 
 *		with the additional information ‘KNoSpecificMeProblem’.
 *		For the second attempt, ME responds with KSuccess in the response parameter
 *
 * 	SYMTestType CIT
 */

TInt CSimSatSubMMPCmdTest::testSAT8511L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8511"));
	
	TRequestStatus reqStatus;

	RSat::TSubmitMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TSubmitMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TSubmitMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TSubmitMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifySubmitMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	RSat::TSubmitMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TSubmitMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TSubmitMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave) 
		RSat::TSubmitMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	_LIT(KText,"KNoSpecificMeProblem");
	TBuf<254> buf(KText);
	tMMMessageRspV6->iGeneralResult = RSat::KMMSTemporaryProblem;
	tMMMessageRspV6->iInfoType = RSat::KMeProblem;
	tMMMessageRspV6->iAdditionalInfo = buf;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_SUB_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESubmitMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	iSat.NotifySubmitMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_SUB_MM_PCMD_NUMBER);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8511 - Successful"));
	return KErrNone;

	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8512
 *
 *	SYMREQ REQ7330
 *
 *	SYMTestCaseDesc Client rejects  Submit  Multimedia command as the ME is unable to process the MMS transaction
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for SUBMIT MULTIMEDIA MESSAGE using 
 *			NotifySubmitMultimediaMsgPCmd () API.
 *		(ii)	UICC issues Submit Command to the ME.
 *		(iii)	ME is unable to process the MMS transaction.
 *		(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request.
 *		Upon completion of the proactive command details are returned in a packaged version of 
 *		TSubmitMultimediaMessageV6 in the command parameter. 
 *		ME responds with KMMSError  in the Terminal response as the TPCmdResult with the additional 
 *		information ‘KNoSpecificMeProblem’.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatSubMMPCmdTest::testSAT8512L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8512"));

	TRequestStatus reqStatus;

	RSat::TSubmitMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TSubmitMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TSubmitMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TSubmitMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifySubmitMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TSubmitMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TSubmitMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TSubmitMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave) 
		RSat::TSubmitMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	_LIT(KText,"KNoSpecificMeProblem");
	TBuf<254> buf(KText);
	tMMMessageRspV6->iGeneralResult = RSat::KMMSError;
	tMMMessageRspV6->iInfoType = RSat::KMeProblem;
	tMMMessageRspV6->iAdditionalInfo = buf;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_SUB_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESubmitMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8512 - Successful"));
	return KErrNone;

	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8513
 *
 *	SYMREQ REQ7330
 *
 *	SYMTestCaseDesc Cancel notification of Submit  Multimedia request
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Register to be notified of SUBMIT MULTIMEDIA MESSAGE command using 
 *			NotifySubmitMultimediaMsgPCmd () API.
 *		(ii)	Cancel the request by calling CancelAsyncRequest(ESatNotifySubmitMultimediaMessagePCmd)
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */
TInt CSimSatSubMMPCmdTest::testSAT8513L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8513"));

	TRequestStatus reqStatus;

	RSat::TSubmitMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TSubmitMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TSubmitMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TSubmitMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifySubmitMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	iSat.CancelAsyncRequest(ESatNotifySubmitMultimediaMsgPCmd);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));

	CleanupStack::PopAndDestroy(2);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8513 - Successful"));
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8514
 *
 *	SYMREQ REQ7330 and REQ7334
 *
 *	SYMTestCaseDesc To verify the event of the envelope command for MMS TRANSFER STATUS envelope command 
 *			after the SUBMIT MULTIMEDIA MESSAGE proactive command.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference.
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the TSatProfileV6 with TSatProfileByte22 \
 *			set to KCapsSubmitMultimediaMessage and TSatProfileByte25 set to KCapsMmsTransferStatusEvent.
 *		(iii)	Client requests notification for RETRIEVE MULTIMEDIA MESSAGE using 
 *			NotifySubmtiMultimediaMsgPCmd () API.
 *		(iv)	UICC issues SUBMIT MULTIMEDIA MESSAGE Command to the ME.
 *		(v)	ME is able to process the MMS transaction and the response is sent using 
 *			RSat::TerminalRsp () for NotifySubmitMultimediaMsgPCmd.
 *		(vi)	Call the API RSat::MMSTransferStatus with the parameter as object of TMMTransferStatusV6 to 
 *			store the MM1_retrieve.RES message.
 *
 *	SYMTestExpectedResults 
 *	GetMeSideSatProfile shall return TSatProfileV6 with TSatProfileByte22 set to KCapsSubmitMultimediaMessage 
 *	and TSatProfileByte25 set to KCapsMmsTransferStatusEvent.
 *	ClientSatProfileIndication shall return KErrNone.
 *	The envelope command is passed on from ME to UICC successfully.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatSubMMPCmdTest::testSAT8514L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8514"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(profileV6->iSatProfileByte22 & (RSat::KCapsSubmitMultimediaMessage));
	TESTL(profileV6->iSatProfileByte25 & (RSat::KCapsMmsTransferStatusEvent));
	
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
	
	RSat::TSubmitMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TSubmitMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TSubmitMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TSubmitMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifySubmitMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	RSat::TSubmitMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TSubmitMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TSubmitMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
	       RSat::TSubmitMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_SUB_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESubmitMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
		
	RSat::TMmsTransferStatusV6 *tMMSTransferStsV6 = new (ELeave) RSat::TMmsTransferStatusV6;
	CleanupStack::PushL(tMMSTransferStsV6);
	
	RSat::TMmsTransferStatusV6Pckg *tMMSTransferStsV6Pckg = new (ELeave) 
		RSat::TMmsTransferStatusV6Pckg(*tMMSTransferStsV6);
	CleanupStack::PushL(tMMSTransferStsV6Pckg);
	
	_LIT(KText,"TransferFilePath");
	TBuf<255> trfFileBuf(KText);
	_LIT(KText1,"MMMessageBuf");
	TBuf<255> trfFileMMMsgBuf(KText1);
	_LIT(KText2,"MMMessageIdentifier");
	TBuf<255> trfFileMMMsgId(KText2);
	
	tMMSTransferStsV6->iDeviceId = RSat::KME;
	tMMSTransferStsV6->iMMSTransferFile = trfFileBuf;
	tMMSTransferStsV6->iMultimediaMessageStatus = trfFileMMMsgBuf;
	tMMSTransferStsV6->iMultimediaMessageId = trfFileMMMsgId ;
	
	iSat.MmsTransferStatus(reqStatus,*tMMSTransferStsV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	CleanupStack::PopAndDestroy(8);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8514 - Successful"));
	return KErrNone;

	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8515
 *
 *	SYMREQ REQ7330 and REQ7334
 *
 *	SYMTestCaseDesc To verify the event of cancellation of the envelope command for MMS TRANSFER STATUS
 *			envelope command after the SUBMIT MULTIMEDIA MESSAGE Proactive command is placed.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for SUBMIT MULTIMEDIA MESSAGE using 
 *			NotifySubmitMultimediaMsgPCmd () API.
 *		(ii)	UICC issues SUBMIT MULTIMEDIA MESSAGE Command to the ME.
 *		(iii)	ME is able to process the MMS transaction and the response is sent using 
 *			RSat::TerminalRsp () for NotifySubmitMultimediaMsgPCmd.
 *		(iv)	Call the API RSat::MMSTransferStatus with the parameter as object of TMMTransferStatusV6.
 *		(v)	Cancel the request for the envelope command API placed using 
 *			RSat::CancelAsyncRequest(ESatMMTransferStatus).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 *
 * 	SYMTestType CIT
 */


TInt CSimSatSubMMPCmdTest::testSAT8515L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8515"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(profileV6->iSatProfileByte22 & (RSat::KCapsSubmitMultimediaMessage));
	TESTL(profileV6->iSatProfileByte25 & (RSat::KCapsMmsTransferStatusEvent));
	
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
	
	RSat::TSubmitMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TSubmitMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TSubmitMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TSubmitMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifySubmitMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	RSat::TSubmitMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TSubmitMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TSubmitMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
		RSat::TSubmitMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_SUB_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESubmitMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
		
	RSat::TMmsTransferStatusV6 *tMMSTransferStsV6 = new (ELeave) RSat::TMmsTransferStatusV6;
	CleanupStack::PushL(tMMSTransferStsV6);
	
	RSat::TMmsTransferStatusV6Pckg *tMMSTransferStsV6Pckg = new (ELeave) 
		RSat::TMmsTransferStatusV6Pckg(*tMMSTransferStsV6);
	CleanupStack::PushL(tMMSTransferStsV6Pckg);
	
	_LIT(KText,"TransferFilePath");
	TBuf<255> trfFileBuf(KText);
	_LIT(KText1,"MMMessageBuf");
	TBuf<255> trfFileMMMsgBuf(KText1);
	_LIT(KText2,"MMMessageIdentifier");
	TBuf<255> trfFileMMMsgId(KText2);
	
	tMMSTransferStsV6->iDeviceId = RSat::KME;
	tMMSTransferStsV6->iMMSTransferFile = trfFileBuf;
	tMMSTransferStsV6->iMultimediaMessageStatus = trfFileMMMsgBuf;
	tMMSTransferStsV6->iMultimediaMessageId = trfFileMMMsgId ;
	
	iSat.MmsTransferStatus(reqStatus,*tMMSTransferStsV6Pckg);
	iSat.CancelAsyncRequest(ESatMmsTransferStatus);
	User::WaitForRequest(reqStatus);
	
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));
	
	CleanupStack::PopAndDestroy(8);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8515 - Successful"));
	return KErrNone;
	}


CSimSatDispMMPCmdTest::CSimSatDispMMPCmdTest() 
	{ 
	SetTestStepName(_L("SatTestDispMMPCmd"));
	}


TVerdict CSimSatDispMMPCmdTest::doTestStepL()
	{
	INFO_PRINTF1(_L("DISPLAY MULTIMEDIA PCMD TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8516L();	// For testing the TEL-SIMTSY-SAT-8516 
	testSAT8517L();	// For testing the TEL-SIMTSY-SAT-8517 
	testSAT8518L();	// For testing the TEL-SIMTSY-SAT-8518 
	testSAT8519L();	// For testing the TEL-SIMTSY-SAT-8519 
	testSAT8520L();	// For testing the TEL-SIMTSY-SAT-8520 
	testSAT8521L();	// For testing the TEL-SIMTSY-SAT-8521 
	testSAT8522L();	// For testing the TEL-SIMTSY-SAT-8522
	testSAT8523L();	// For testing the TEL-SIMTSY-SAT-8523
	testSAT8524L();	// For testing the TEL-SIMTSY-SAT-8524	
	testSAT8525L();	// For testing the TEL-SIMTSY-SAT-8525
	testSAT8526L();	// For testing the TEL-SIMTSY-SAT-8526

	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("DISPLAY MULTIMEDIA PCMD TESTING - END"));

	return TestStepResult();
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8516
 *
 *	SYMREQ REQ7331
 *
 *	SYMTestCaseDesc UICC issues normal priority DISPLAY MULTIMEDIA MESSAGE command to ME
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for DISPLAY MULTIMEDIA MESSAGE using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.  
 *		(ii)	UICC issues normal priority DISPLAY MULTIMEDIA MESSAGE command to the 
 *			ME requesting for “No Immediate Response” from client.
 *		(iii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request. 
 *		Upon completion of the proactive command details are returned in a packaged version of 
 *		TDisplayMultimediaMessageV6 with priority as ENormalPriority and ImmediateRsp as ENoImmediateRsp
 *		and TClearScreenTrigger as EUserClear.
 *		ME responds with KSuccess in the Terminal response.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatDispMMPCmdTest::testSAT8516L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8516"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tMMMessageV6->iDestination == RSat::KME);
	TESTL(tMMMessageV6->iDisplayPriority == RSat::ENormalPriority);
	TESTL(tMMMessageV6->iClearScreenTrigger == RSat::EUserClear);
	TESTL(tMMMessageV6->iSubmissionFile.Compare(SIMTSY_SUBMISSION_FILES)==0);
	TESTL(tMMMessageV6->iMultimediaMessageId.Compare(SIMTSY_MMMSG_ID_DATA_2)==0);
	TESTL(tMMMessageV6->iImmediateRsp == RSat::ENoImmediateRsp);

	RSat::TDisplayMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave) 
		RSat::TDisplayMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8516 - Successful"));
	return KErrNone;
	
	}
	

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8517
 *
 *	SYMREQ REQ7331
 *
 *	SYMTestCaseDesc UICC issues high priority DISPLAY MULTIMEDIA MESSAGE command to ME
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for DISPLAY MULTIMEDIA MESSAGE using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.
 *		(ii)	UICC issues high priority DISPLAY MULTIMEDIA MESSAGE command to the ME requesting 
 *			for “Immediate Response” and “clearing the display after some delay”.
 *		(iii)	Client responds indicating that the proactive command was successful using RSat::TerminalRsp()
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request. 
 *		Upon completion of the proactive command details are returned in a packaged version of 
 *		TDisplayMultimediaMessageV6 with priority as EHighPriority, ImmediateRsp as EImmediateRsp and 
 *		TClearScreenTrigger as EClearAfterDelay.
 *		Client responds with KSuccess in the Terminal response
 *
 * 	SYMTestType CIT
 */


TInt CSimSatDispMMPCmdTest::testSAT8517L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8517"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tMMMessageV6->iDestination == RSat::KME);
	TESTL(tMMMessageV6->iDisplayPriority == RSat::EHighPriority);
	TESTL(tMMMessageV6->iClearScreenTrigger == RSat::EClearAfterDelay);
	TESTL(tMMMessageV6->iSubmissionFile.Compare(SIMTSY_SUBMISSION_FILES)==0);
	TESTL(tMMMessageV6->iMultimediaMessageId.Compare(SIMTSY_MMMSG_ID_DATA_2)==0);
	TESTL(tMMMessageV6->iImmediateRsp == RSat::EImmediateRsp);

	RSat::TDisplayMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
	       	RSat::TDisplayMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8517 - Successful"));
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8518
 *
 *	SYMREQ REQ7331
 *
 *	SYMTestCaseDesc UICC issues DISPLAY MULTIMEDIA MESSAGE command to ME and the user has indicated to
 *	end the proactive UICC application session.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for DISPLAY MULTIMEDIA MESSAGE using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.  
 *		(ii)	UICC issues Display MultiMedia Message command to the ME requesting for 
 *			“Immediate Response” from the client.
 *		(iii)	Client responds indicating that the proactive command was successful using RSat::TerminalRsp()
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request. 
 *		ME responds with the Terminal Response with TPcmdresult as KPSessionTerminatedByUser
 *
 * 	SYMTestType CIT
 */

TInt CSimSatDispMMPCmdTest::testSAT8518L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8518"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	RSat::TDisplayMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave) 
		RSat::TDisplayMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KPSessionTerminatedByUser;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8518 - Successful"));
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8519
 *
 *	SYMREQ REQ7331
 *
 *	SYMTestCaseDesc UICC issues DISPLAY MULTIMEDIA MESSAGE command to ME and the user has indicated to go 
 *			backward in the proactive UICC application session.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for DISPLAY MULTIMEDIA MESSAGE using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.  
 *		(ii)	Client responds indicating that the proactive command was successful using RSat::TerminalRsp()
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request. 
 *		ME responds with the Terminal Response with TPcmdresult as KBackwardModeRequestedByUser.
 *
 * 	SYMTestType CIT
 */
TInt CSimSatDispMMPCmdTest::testSAT8519L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8519"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	RSat::TDisplayMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
	       	RSat::TDisplayMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KBackwardModeRequestedByUser;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8519 - Successful"));
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8520
 *
 *	SYMREQ  REQ7331
 *
 *	SYMTestCaseDesc UICC issues DISPLAY MULTIMEDIA MESSAGE command to ME with the indication of 
 *			clearing the screen with user action and there is no response from the user.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for DISPLAY MULTIMEDIA MESSAGE using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.  
 *		(ii)	Client responds indicating that the proactive command using RSat::TerminalRsp()
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request. 
 *		ME responds with the Terminal Response with TPcmdresult as KNoResponseFromUser
 *
 * 	SYMTestType CIT
 */

TInt CSimSatDispMMPCmdTest::testSAT8520L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8520"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	RSat::TDisplayMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
	       	RSat::TDisplayMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KNoResponseFromUser;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8520 - Successful"));
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8521
 *
 *	SYMREQ REQ7331
 *
 *	SYMTestCaseDesc UICC issues DISPLAY MULTIMEDIA MESSAGE command to ME and ME is not able to process the command.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for DISPLAY MULTIMEDIA MESSAGE using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.  
 *		(ii)	Client responds indicating that the proactive command using RSat::TerminalRsp()
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request. 
 *		Client responds with KMMSError in the Terminal response with the additional information 
 *		as KNoSpecificMeProblem.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatDispMMPCmdTest::testSAT8521L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8521"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	RSat::TDisplayMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave) 
		RSat::TDisplayMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	_LIT(KText,"KNoSpecificMeProblem");
	TBuf<254> buf(KText);
	tMMMessageRspV6->iGeneralResult = RSat::KMMSError;
	tMMMessageRspV6->iInfoType = RSat::KMeProblem;
	tMMMessageRspV6->iAdditionalInfo = buf;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8521 - Successful"));
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8522
 *
 *	SYMREQ REQ7331
 *
 *	SYMTestCaseDesc UICC issues normal priority DISPLAY MULTIMEDIA MESSAGE to ME while a normal priority 
 *			DISPLAY MULTIMEDIA MESSAGE is being displayed
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for DISPLAY MULTIMEDIA MESSAGE using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.  
 *		(ii)	Client responds indicating that the proactive command using RSat::TerminalRsp()
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request. 
 *		Client responds with KMeUnableToProcessCmd in the Terminal response with the 
 *		additional information as KScreenBusy.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatDispMMPCmdTest::testSAT8522L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8522"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	TESTL(tMMMessageV6->iDisplayPriority == RSat::ENormalPriority);
	
	RSat::TDisplayMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
	       	RSat::TDisplayMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	// Attempt to send second command when the first message is being displayed
	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tMMMessageV6->iDisplayPriority == RSat::ENormalPriority);
	
	_LIT(KText,"KScreenBusy");
	TBuf<254> buf(KText);
	tMMMessageRspV6->iGeneralResult = RSat::KMeUnableToProcessCmd;
	tMMMessageRspV6->iInfoType = RSat::KMeProblem;
	tMMMessageRspV6->iAdditionalInfo = buf;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
			
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8522 - Successful"));
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8523
 *
 *	SYMREQ REQ7331
 *
 *	SYMTestCaseDesc UICC issues High priority DISPLAY MULTIMEDIA MESSAGE to ME while a normal priority 
 *			DISPLAY MULTIMEDIA MESSAGE is being displayed
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for DISPLAY MULTIMEDIA MESSAGE using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.  
 *		(ii)	Client responds indicating that the proactive command using RSat::TerminalRsp()
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request. 
 *		Client responds with KSuccess  in the Terminal response.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatDispMMPCmdTest::testSAT8523L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8523"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	TESTL(tMMMessageV6->iDisplayPriority == RSat::ENormalPriority);

	RSat::TDisplayMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave) 
		RSat::TDisplayMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	// Attempt to send second command when the first message is being displayed
	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tMMMessageV6->iDisplayPriority == RSat::EHighPriority);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
			
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8523 - Successful"));
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8524
 *
 *	SYMREQ REQ7331
 *
 *	SYMTestCaseDesc UICC issues Normal priority DISPLAY MULTIMEDIA MESSAGE to ME while a High priority
 *			DISPLAY MULTIMEDIA MESSAGE is being displayed
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for DISPLAY MULTIMEDIA MESSAGE using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.  
 *		(ii)	Client responds indicating that the proactive command using RSat::TerminalRsp()
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request. 
 *		Client responds with KMeUnableToProcessCmd in the Terminal response with the 
 *		additional information as KScreenBusy.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatDispMMPCmdTest::testSAT8524L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8524"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tMMMessageV6->iDisplayPriority == RSat::EHighPriority);

	RSat::TDisplayMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
	       	RSat::TDisplayMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	// Attempt to send second command when the first message is being displayed
	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tMMMessageV6->iDisplayPriority == RSat::ENormalPriority);
	
	_LIT(KText,"KScreenBusy");
	TBuf<254> buf(KText);
	tMMMessageRspV6->iGeneralResult = RSat::KMeUnableToProcessCmd;
	tMMMessageRspV6->iInfoType = RSat::KMeProblem;
	tMMMessageRspV6->iAdditionalInfo = buf;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
			
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8524 - Successful"));
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8525
 *
 *	SYMREQ REQ7331
 *
 *	SYMTestCaseDesc UICC issues High priority DISPLAY MULTIMEDIA MESSAGE to ME while a High priority
 *			DISPLAY MULTIMEDIA MESSAGE is being displayed
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for DISPLAY MULTIMEDIA MESSAGE using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.  
 *		(ii)	Client responds indicating that the proactive command using RSat::TerminalRsp()
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request. 
 *		Client responds with KMeUnableToProcessCmd in the Terminal response with the 
 *		additional information as KScreenBusy.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatDispMMPCmdTest::testSAT8525L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8525"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave) 
		RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tMMMessageV6->iDisplayPriority == RSat::EHighPriority);

	RSat::TDisplayMultimediaMessageRspV6 *tMMMessageRspV6 = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(tMMMessageRspV6);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMMessageRspPckgV6 = new (ELeave)
	       	RSat::TDisplayMultimediaMessageRspV6Pckg(*tMMMessageRspV6);
	CleanupStack::PushL(tMMMessageRspPckgV6);
	
	tMMMessageRspV6->iGeneralResult = RSat::KSuccess;
	tMMMessageRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	// Attempt to send second command when the first message is being displayed
	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tMMMessageV6->iDisplayPriority == RSat::EHighPriority);
	
	_LIT(KText,"KScreenBusy");
	TBuf<254> buf(KText);
	tMMMessageRspV6->iGeneralResult = RSat::KMeUnableToProcessCmd;
	tMMMessageRspV6->iInfoType = RSat::KMeProblem;
	tMMMessageRspV6->iAdditionalInfo = buf;
	tMMMessageRspV6->SetPCmdNumber(SIMTSY_DISP_MM_PCMD_NUMBER);
			
	iSat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*(tMMMessageRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8525 - Successful"));
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8526
 *
 *	SYMREQ REQ7331
 *
 *	SYMTestCaseDesc Cancel notification of DISPLAY MULTIMEDIA MESSAGE request
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Register to be notified of DISPLAY MULTIMEDIA MESSAGE command using 
 *			RSat::NotifyDisplayMultimediaMsgPCmd () API.
 *		(ii)	Cancel the request by calling CancelAsyncRequest (ESatNotifyDisplayMultimediaMessagePCmd)
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */

TInt CSimSatDispMMPCmdTest::testSAT8526L()
	{

	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8526"));

	TRequestStatus reqStatus;

	RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TDisplayMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TDisplayMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyDisplayMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	iSat.CancelAsyncRequest(ESatNotifyDisplayMultimediaMsgPCmd);
	User::WaitForRequest(reqStatus);
	
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));

	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8526 - Successful"));
	return KErrNone;
	}

CSimSatSetFrmsPCmdTest::CSimSatSetFrmsPCmdTest()
	{
	SetTestStepName(_L("SatTestSetFrmsPCmd"));
	}

TVerdict CSimSatSetFrmsPCmdTest::doTestStepL()
	{
	INFO_PRINTF1(_L("SET FRAMES PCMD TESTING - START"));
	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8527L();	// For testing the TEL-SIMTSY-SAT-8527
	testSAT8528L();	// For testing the TEL-SIMTSY-SAT-8528
	testSAT8529L();  // For testing the TEL-SIMTSY-SAT-8529
	testSAT8530L();  // For testing the TEL-SIMTSY-SAT-8530
	
	
	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("SET FRAMES PCMD TESTING - END"));

	return TestStepResult();
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8527
 *
 *	SYMREQ REQ7332
 *
 *	SYMTestCaseDesc UICC issues the SET FRAMES proactive command successfully.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Request notification for SET FRAMES with RSat::NotifySetFramesPCmd ().
 *		(ii)	UICC issues SET FRAMES Command to the ME.  
 *		(iii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request.
 *		Upon completion of the  proactive command details are returned in a packaged version of 
 *		TSetFramesRspV6 containing the correct Frames Layout parameters.
 *		ME responds with KSuccess in the Terminal response.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatSetFrmsPCmdTest::testSAT8527L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8527"));

	TRequestStatus reqStatus;

	RSat::TSetFramesV6 *tSetFramesV6 = new (ELeave) RSat::TSetFramesV6;
	CleanupStack::PushL(tSetFramesV6);

	RSat::TSetFramesV6Pckg *tSetFramesV6Pckg = new (ELeave) RSat::TSetFramesV6Pckg(*tSetFramesV6);
	CleanupStack::PushL(tSetFramesV6Pckg);
	
	iSat.NotifySetFramesPCmd(reqStatus,*tSetFramesV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tSetFramesV6->iDestination == RSat::KME);
	TESTL(tSetFramesV6->iFrameId == SIMTSY_FRAME_IDENTIFIER);
	TESTL(tSetFramesV6->iFrameLayout.iFramesLayout == RSat::EFrameHorizontal);
	TESTL(tSetFramesV6->iFrameLayout.iFrameLayoutBuf.Compare(SIMTSY_FRAME_LAYOUT_BUFFER) == 0);
	TESTL(tSetFramesV6->iDefaultFrameId == SIMTSY_DEF_FRAME_IDENTIFIER);
	TESTL(tSetFramesV6->iFramesSeparator == RSat::EFrameSeparatorDrawn);
	
	RSat::TSetFramesRspV6 *tSetFramesRspV6 = new (ELeave) RSat::TSetFramesRspV6;
	CleanupStack::PushL(tSetFramesRspV6);
	
	RSat::TSetFramesRspV6Pckg *tSetFramesRspV6Pckg = new (ELeave) RSat::TSetFramesRspV6Pckg(*tSetFramesRspV6);
	CleanupStack::PushL(tSetFramesRspV6Pckg);
		
	tSetFramesRspV6->iGeneralResult = RSat::KSuccess;
	tSetFramesRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tSetFramesRspV6->SetPCmdNumber(SIMTSY_SET_FRMS_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESetFrames,*(tSetFramesRspV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8527- Successful"));

	return KErrNone;
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8528
 *
 *	SYMREQ REQ7332
 *
 *	SYMTestCaseDesc Client rejects SET FRAMES command as invalid frame parameters provided 
 *			(requested sizes of the frames are not allowed).
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Request notification for SET FRAMES with RSat::NotifySetFramesPCmd ().
 *		(ii)	The notification completes when UICC issues the SET FRAMES command to the ME.  
 *		(iii)	ME is unable to process the proactive command because of the requested size of the
 *			frames are not allowed and responds indicating that the proactive command was 
 *			unsuccessful using RSat::TerminalRsp() 
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request.
 *		Upon completion of the  proactive command details are returned in a packaged version of 
 *		TSetFramesRspV6 containing  invalid Frames Layout parameters.
 *		ME responds with KFramesError in the Terminal response with the additional information of 
 *		KRequestSizeNotIdentified.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatSetFrmsPCmdTest::testSAT8528L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8528"));

	TRequestStatus reqStatus;

	RSat::TSetFramesV6 *tSetFramesV6 = new (ELeave) RSat::TSetFramesV6;
	CleanupStack::PushL(tSetFramesV6);

	RSat::TSetFramesV6Pckg *tSetFramesV6Pckg = new (ELeave) RSat::TSetFramesV6Pckg(*tSetFramesV6);
	CleanupStack::PushL(tSetFramesV6Pckg);
	
	iSat.NotifySetFramesPCmd(reqStatus,*tSetFramesV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tSetFramesV6->iDestination == RSat::KME);
	TESTL(tSetFramesV6->iFrameId == SIMTSY_FRAME_IDENTIFIER);
	TESTL(tSetFramesV6->iFrameLayout.iFramesLayout == RSat::EFrameHorizontal);
	TESTL(tSetFramesV6->iFrameLayout.iFrameLayoutBuf.Compare(SIMTSY_FRAME_LAYOUT_BUFFER) == 0);
	TESTL(tSetFramesV6->iDefaultFrameId == SIMTSY_DEF_FRAME_IDENTIFIER);
	TESTL(tSetFramesV6->iFramesSeparator == RSat::EFrameSeparatorDrawn);
	
	RSat::TSetFramesRspV6 *tSetFramesRspV6 = new (ELeave) RSat::TSetFramesRspV6;
	CleanupStack::PushL(tSetFramesRspV6);
	
	RSat::TSetFramesRspV6Pckg *tSetFramesRspV6Pckg = new (ELeave) RSat::TSetFramesRspV6Pckg(*tSetFramesRspV6);
	CleanupStack::PushL(tSetFramesRspV6Pckg);
		
	_LIT(KText,"KRequestSizeNotSupported"); 
	TBuf<254> buf(KText);
	tSetFramesRspV6->iGeneralResult = RSat::KFramesError;
	tSetFramesRspV6->iInfoType = RSat::KMeProblem;
	tSetFramesRspV6->iAdditionalInfo = buf;
	tSetFramesRspV6->SetPCmdNumber(SIMTSY_SET_FRMS_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESetFrames,*(tSetFramesRspV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8528- Successful"));

	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8529
 *
 *	SYMREQ REQ7332
 *
 *	SYMTestCaseDesc Client rejects SET FRAMES command as invalid frame parameters provided 
 *			(number of frames beyond the terminal’s capabilities).
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Request notification for SET FRAMES with RSat::NotifySetFramesPCmd ().
 *		(ii)	The notification completes when UICC issues the SET FRAMES command to the ME.  
 *		(iii)	ME  is unable to process the proactive command because the number of frames requested 
 *			are beyond the terminal’s capabilities and responds indicating that the proactive command 
 *			was unsuccessful using RSat::TerminalRsp() 
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request.
 *		Upon completion of the  proactive command details are returned in a packaged version of 
 *		TSetFramesRspV6 containing  invalid Frames Layout parameters.
 *		ME responds with KFramesError in the Terminal response with the additional information of 
 *		‘KCmdBeyondMeCapabilities’.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatSetFrmsPCmdTest::testSAT8529L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8529"));

	TRequestStatus reqStatus;

	RSat::TSetFramesV6 *tSetFramesV6 = new (ELeave) RSat::TSetFramesV6;
	CleanupStack::PushL(tSetFramesV6);

	RSat::TSetFramesV6Pckg *tSetFramesV6Pckg = new (ELeave) RSat::TSetFramesV6Pckg(*tSetFramesV6);
	CleanupStack::PushL(tSetFramesV6Pckg);
	
	iSat.NotifySetFramesPCmd(reqStatus,*tSetFramesV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TSetFramesRspV6 *tSetFramesRspV6 = new (ELeave) RSat::TSetFramesRspV6;
	CleanupStack::PushL(tSetFramesRspV6);
	
	RSat::TSetFramesRspV6Pckg *tSetFramesRspV6Pckg = new (ELeave) RSat::TSetFramesRspV6Pckg(*tSetFramesRspV6);
	CleanupStack::PushL(tSetFramesRspV6Pckg);
		
	_LIT(KText,"KNumberOfFramesExceeded"); 
	TBuf<254> buf(KText);
	tSetFramesRspV6->iGeneralResult = RSat::KFramesError;
	tSetFramesRspV6->iInfoType = RSat::KMeProblem;
	tSetFramesRspV6->iAdditionalInfo = buf;
	tSetFramesRspV6->SetPCmdNumber(SIMTSY_SET_FRMS_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESetFrames,*(tSetFramesRspV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8529- Successful"));

	return KErrNone;
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8530
 *
 *	SYMREQ REQ7332
 *
 *	SYMTestCaseDesc Cancel notification of SET FRAMES request
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Request notification for SET FRAMES with RSat::NotifySetFramesPCmd ().
 *		(ii)	Cancel the request by calling CancelAsyncRequest (ESatNotifySetFramesPCmd).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatSetFrmsPCmdTest::testSAT8530L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8530"));

	TRequestStatus reqStatus;

	RSat::TSetFramesV6 *tSetFramesV6 = new (ELeave) RSat::TSetFramesV6;
	CleanupStack::PushL(tSetFramesV6);

	RSat::TSetFramesV6Pckg *tSetFramesV6Pckg = new (ELeave) RSat::TSetFramesV6Pckg(*tSetFramesV6);
	CleanupStack::PushL(tSetFramesV6Pckg);
	
	iSat.NotifySetFramesPCmd(reqStatus,*tSetFramesV6Pckg);
	iSat.CancelAsyncRequest(ESatNotifySetFramesPCmd);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));

	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8530- Successful"));

	return KErrNone;
	}


CSimSatGetFrmsPCmdTest::CSimSatGetFrmsPCmdTest() 
	{ 
	SetTestStepName(_L("SatTestGetFrmsPCmd"));
	}

TVerdict CSimSatGetFrmsPCmdTest::doTestStepL()
	{
	INFO_PRINTF1(_L("GET FRAMES PCMD TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8531L();	// For testing the TEL-SIMTSY-SAT-8531
	testSAT8532L();	// For testing the TEL-SIMTSY-SAT-8532
	testSAT8533L();	// For testing the TEL-SIMTSY-SAT-8533

	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("GET FRAMES PCMD TESTING - END"));

	return TestStepResult();
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8531
 *
 *	SYMREQ REQ7333
 *
 *	SYMTestCaseDesc UICC issues GET FRAMES STATUS command to the client
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Request notification for GET FRAMES STATUS with  RSat::NotifyGetFramesStatusPCmd () 
 *		(ii)	UICC sends the GET FRAMES STATUS to the ME.
 *		(iii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request.
 *		Upon completion of the  proactive command details are returned in a packaged version 
 *		of TGetFramesStatusRspV6().
 *		ME responds with KSuccess in the Terminal response.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatGetFrmsPCmdTest::testSAT8531L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8531"));

	TRequestStatus reqStatus;

	RSat::TGetFramesStatusV6 *tGetFramesStsV6 = new (ELeave) RSat::TGetFramesStatusV6;
	CleanupStack::PushL(tGetFramesStsV6);

	RSat::TGetFramesStatusV6Pckg *tGetFramesStsV6Pckg = new (ELeave) RSat::TGetFramesStatusV6Pckg(*tGetFramesStsV6);
	CleanupStack::PushL(tGetFramesStsV6Pckg);
	
	iSat.NotifyGetFramesStatusPCmd(reqStatus,*tGetFramesStsV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tGetFramesStsV6->iDestination == RSat::KNetwork);
	
	RSat::TGetFramesStatusRspV6 *tGetFramesStsRspV6 = new (ELeave) RSat::TGetFramesStatusRspV6;
	CleanupStack::PushL(tGetFramesStsRspV6);
	
	RSat::TGetFramesStatusRspV6Pckg *tGetFramesStsRspV6Pckg = new (ELeave) 
		RSat::TGetFramesStatusRspV6Pckg(*tGetFramesStsRspV6);
	CleanupStack::PushL(tGetFramesStsRspV6Pckg);
		
	_LIT8(KFrameIdBuf,"AE0240BC");
	TBuf8<255> buf(KFrameIdBuf); 
	tGetFramesStsRspV6->iGeneralResult = RSat::KSuccess;
	tGetFramesStsRspV6->iInfoType = RSat::KNoAdditionalInfo;
	tGetFramesStsRspV6->iFramesInformation.iFrameId = 0x25;
	tGetFramesStsRspV6->iFramesInformation.iFrameList = buf;
	tGetFramesStsRspV6->SetPCmdNumber(SIMTSY_GET_FRMS_STS_PCMD_NUMBER);
	
	iSat.TerminalRsp(reqStatus,RSat::EGetFramesStatus,*(tGetFramesStsRspV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8531- Successful"));

	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8532
 *
 *	SYMREQ REQ7333
 *
 *	SYMTestCaseDesc UICC issues GET FRAMES STATUS command to the client when there is no frame available.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Request notification for GET FRAMES STATUS with  RSat::NotifyGetFramesStatusPCmd () 
 *		(ii)	UICC sends the GET FRAMES STATUS to the ME.
 *		(iii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Client is notified of the request.
 *		Upon completion of the  proactive command details are returned in a packaged version of 
 *		TGetFramesStatusRspV6 ().
 *		ME responds with KFramesError in the Terminal response with the additional information 
 *		'KNoFramesIdentified'.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatGetFrmsPCmdTest::testSAT8532L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8532"));

	TRequestStatus reqStatus;

	RSat::TGetFramesStatusV6 *tGetFramesStsV6 = new (ELeave) RSat::TGetFramesStatusV6;
	CleanupStack::PushL(tGetFramesStsV6);

	RSat::TGetFramesStatusV6Pckg *tGetFramesStsV6Pckg = new (ELeave) RSat::TGetFramesStatusV6Pckg(*tGetFramesStsV6);
	CleanupStack::PushL(tGetFramesStsV6Pckg);
	
	iSat.NotifyGetFramesStatusPCmd(reqStatus,*tGetFramesStsV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tGetFramesStsV6->iDestination == RSat::KNetwork);
	
	RSat::TGetFramesStatusRspV6 *tGetFramesStsRspV6 = new (ELeave) RSat::TGetFramesStatusRspV6;
	CleanupStack::PushL(tGetFramesStsRspV6);
	
	RSat::TGetFramesStatusRspV6Pckg *tGetFramesStsRspV6Pckg = new (ELeave) 
		RSat::TGetFramesStatusRspV6Pckg(*tGetFramesStsRspV6);
	CleanupStack::PushL(tGetFramesStsRspV6Pckg);
		
	_LIT8(KFrameIdBuf,"AE0240BC");
	TBuf8<255> buf(KFrameIdBuf);
	tGetFramesStsRspV6->iGeneralResult = RSat::KFramesError;
	tGetFramesStsRspV6->iInfoType = RSat::KNoFrameIdentified;
	tGetFramesStsRspV6->iFramesInformation.iFrameId = 0x25;
	tGetFramesStsRspV6->iFramesInformation.iFrameList = buf;
	tGetFramesStsRspV6->SetPCmdNumber(SIMTSY_GET_FRMS_STS_PCMD_NUMBER);
	
	iSat.TerminalRsp(reqStatus,RSat::EGetFramesStatus,*(tGetFramesStsRspV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8532- Successful"));

	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8533
 *
 *	SYMREQ REQ7333
 *
 *	SYMTestCaseDesc Cancel notification of GET FRAMES STATUS request.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Register to be notified of GET FRAMES STATUS command using RSat::NotifyGetFramesStatusPCmd ().
 *		(ii)	Cancel the request by calling CancelAsyncRequest (ESatNotifyGetFramesStatusPCmd).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatGetFrmsPCmdTest::testSAT8533L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8533"));

	TRequestStatus reqStatus;

	RSat::TGetFramesStatusV6 *tGetFramesStsV6 = new (ELeave) RSat::TGetFramesStatusV6;
	CleanupStack::PushL(tGetFramesStsV6);

	RSat::TGetFramesStatusV6Pckg *tGetFramesStsV6Pckg = new (ELeave) RSat::TGetFramesStatusV6Pckg(*tGetFramesStsV6);
	CleanupStack::PushL(tGetFramesStsV6Pckg);
	
	iSat.NotifyGetFramesStatusPCmd(reqStatus,*tGetFramesStsV6Pckg);
	iSat.CancelAsyncRequest(ESatNotifyGetFramesStatusPCmd);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int() ==KErrCancel));
	
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8533- Successful"));

	return KErrNone;
	}

CSimSatMmsNotificationDownload::CSimSatMmsNotificationDownload() 
	{ 
	SetTestStepName(_L("SatTestGetFrmsPCmd"));
	}

TVerdict CSimSatMmsNotificationDownload::doTestStepL()
	{
	INFO_PRINTF1(_L("MM NOTIFICATION DOWNLOAD TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8534L();	// For testing the TEL-SIMTSY-SAT-8534
	testSAT8535L();	// For testing the TEL-SIMTSY-SAT-8535
	testSAT8536L();	// For testing the TEL-SIMTSY-SAT-8536
	testSAT8537L();	// For testing the TEL-SIMTSY-SAT-8537
	testSAT8538L();	// For testing the TEL-SIMTSY-SAT-8538

	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("MM NOTIFICATION DOWNLOAD TESTING - END"));

	return TestStepResult();
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8534
 *
 *	SYMREQ REQ7335
 *
 *	SYMTestCaseDesc ME issues a MMS NOTIFICATION DOWNLOAD envelope command to the UICC successfully.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the TSatProfileV6 with TSatProfileByte23 
 *			set to KCapsMmsNotificationDownload.
 *		(iii)	Issue a request to the UICC to trigger the download of a MMS notification command using 
 *			RSat::MmsNotificationDownload (). 
 *		(iv)	The ME shall wait for an acknowledgement from the UICC.
 *		(v)	It completes when the UICC informs the TSY that the download is complete. 
 *
 *	SYMTestExpectedResults 
 *		Client shall supply the command parameters in a packaged version of TMMSNotificationDownloadV6. 
 *		Device Source and Destination identities shall be KNetwork and KUICC respectively.
 *		UICC responds with ‘90 00’ as MMS status and indicates MMS notification download is 
 *		successfully transferred to the UICC.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatMmsNotificationDownload::testSAT8534L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8534"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(profileV6->iSatProfileByte23 & (RSat::KCapsMmsNotificationDownload));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TMmsNotificationDownloadV6 *mmsNotificationDownloadV6 = new (ELeave) RSat::TMmsNotificationDownloadV6;
	CleanupStack::PushL(mmsNotificationDownloadV6);
	
	RSat::TMmsNotificationDownloadV6Pckg *mmsNotificationDownloadV6Pckg = new (ELeave) 
		RSat::TMmsNotificationDownloadV6Pckg(*mmsNotificationDownloadV6);
	CleanupStack::PushL(mmsNotificationDownloadV6Pckg);

	_LIT(KText,"ABCDE1");
	TBuf<255> mmsNotification(KText);
	
	mmsNotificationDownloadV6->iDeviceId = RSat::KUICC;
	mmsNotificationDownloadV6->iMMSNotification = mmsNotification;
	mmsNotificationDownloadV6->iLastEnvelope = ETrue;
	
	iSat.MmsNotificationDownload(reqStatus,*(mmsNotificationDownloadV6Pckg));
	User::WaitForRequest(reqStatus);
	
	TEST(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8534- Successful"));
	
	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8535
 *
 *	SYMREQ REQ7335
 *
 *	SYMTestCaseDesc MMS NOTIFICATION DOWNLOAD envelope command not transferred to the UICC successfully
 *			at the first attempt and successful at the second attempt.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Issue a request to the UICC to trigger the download of a MMS notification command 
 *			using RSat::MmsNotificationDownload ().
 *		(ii)	The ME shall wait for an acknowledgement from the UICC.
 *		(iii)	It completes when the UICC informs the TSY that the download is complete. 
 *
 *	SYMTestExpectedResults 
 *		Client must supply the command parameters in a packaged version of TMMSNotificationDownloadV6. 
 *		Device Source and Destination identities shall be KNetwork and KUICC respectively.
 *		UICC responds with ‘93 00’as MMS status and indicates MMS notification download has not been 
 *		successfully transferred to the UICC.
 *		ME retries the same command as a second attempt.
 *		UICC responds with ‘90 00’ as MMS status and indicates MMS notification download is 
 *		successfully transferred to the UICC.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatMmsNotificationDownload::testSAT8535L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8535"));

	TRequestStatus reqStatus;
	
	RSat::TMmsNotificationDownloadV6 *mmsNotificationDownloadV6 = new (ELeave) RSat::TMmsNotificationDownloadV6;
	CleanupStack::PushL(mmsNotificationDownloadV6);
	
	RSat::TMmsNotificationDownloadV6Pckg *mmsNotificationDownloadV6Pckg = new (ELeave) 
		RSat::TMmsNotificationDownloadV6Pckg(*mmsNotificationDownloadV6);
	CleanupStack::PushL(mmsNotificationDownloadV6Pckg);

	_LIT(KText,"ABCDE2");
	TBuf<255> mmsNotification(KText);
	
	mmsNotificationDownloadV6->iDeviceId = RSat::KUICC;
	mmsNotificationDownloadV6->iMMSNotification = mmsNotification;
	mmsNotificationDownloadV6->iLastEnvelope = EFalse;
	
	iSat.MmsNotificationDownload(reqStatus,*(mmsNotificationDownloadV6Pckg));
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotReady);
	
	/* Retrying the MMS NOTIFICATION DOWNLOAD COMMAND */
	mmsNotificationDownloadV6->iLastEnvelope = ETrue;
	iSat.MmsNotificationDownload(reqStatus,*(mmsNotificationDownloadV6Pckg));
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8535- Successful"));
	
	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8536
 *
 *	SYMREQ REQ7335
 *
 *	SYMTestCaseDesc MMS NOTIFICATION DOWNLOAD envelope command responded with ‘6F XX’ error
 *			 at the first attempt and ME not reissuing the command.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Issue a request to the UICC to trigger the download of a MMS notification command
 *			using RSat::MmsNotificationDownload ().
 *		(ii)	The ME shall wait for an acknowledgement from the UICC.
 *		(iii)	It completes when the UICC informs the TSY that the download is complete.
 *		 
 *	SYMTestExpectedResults 
 *		Client must supply the command parameters in a packaged version of TMMSNotificationDownloadV6.
 *		Device Source and Destination identities shall be KNetwork and KUICC respectively.
 *		UICC responds with ‘6F XX’as MMS status and indicates MMS notification download has 
 *		not been successfully transferred to the UICC.
 *		ME shall not retry the same command.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatMmsNotificationDownload::testSAT8536L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8536"));

	TRequestStatus reqStatus;
	
	RSat::TMmsNotificationDownloadV6 *mmsNotificationDownloadV6 = new (ELeave) RSat::TMmsNotificationDownloadV6;
	CleanupStack::PushL(mmsNotificationDownloadV6);
	
	RSat::TMmsNotificationDownloadV6Pckg *mmsNotificationDownloadV6Pckg = new (ELeave)
	       	RSat::TMmsNotificationDownloadV6Pckg(*mmsNotificationDownloadV6);
	CleanupStack::PushL(mmsNotificationDownloadV6Pckg);

	_LIT(KText,"ABCDE3");
	TBuf<255> mmsNotification(KText);
	
	mmsNotificationDownloadV6->iDeviceId = RSat::KUICC;
	mmsNotificationDownloadV6->iMMSNotification = mmsNotification;
	mmsNotificationDownloadV6->iLastEnvelope = ETrue;
	
	iSat.MmsNotificationDownload(reqStatus,*(mmsNotificationDownloadV6Pckg));
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotReady);
	
	CleanupStack::PopAndDestroy(2);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8536- Successful"));
	
	return KErrNone;
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8537
 *
 *	SYMREQ REQ7335
 *
 *	SYMTestCaseDesc MMS NOTIFICATION DOWNLOAD envelope command not transferred to the UICC successfully 
 *			as the USIM service table does not support the envelope command.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Issue a request to the UICC to trigger the download of a MMS notification command 
 *			using RSat::MmsNotificationDownload (). 
 *		(ii)	ME receives a MMS Notification message and sends an error message to the network.
 *
 *	SYMTestExpectedResults 
 *		Client must supply the command parameters in a packaged version of TMMSNotificationDownloadV6.  
 *		As MMS transfer mode not recognised by UICC , ME sends error message to the network.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatMmsNotificationDownload::testSAT8537L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8537"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TEST((profileV6->iSatProfileByte23 & RSat::KCapsMmsNotificationDownload) > 0);
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TMmsNotificationDownloadV6 *mmsNotificationDownloadV6 = new (ELeave) RSat::TMmsNotificationDownloadV6;
	CleanupStack::PushL(mmsNotificationDownloadV6);
	
	RSat::TMmsNotificationDownloadV6Pckg *mmsNotificationDownloadV6Pckg = new (ELeave) 
		RSat::TMmsNotificationDownloadV6Pckg(*mmsNotificationDownloadV6);
	CleanupStack::PushL(mmsNotificationDownloadV6Pckg);

	_LIT(KText,"ABCDE4");
	TBuf<255> mmsNotification(KText);
	
	mmsNotificationDownloadV6->iDeviceId = RSat::KUICC;
	mmsNotificationDownloadV6->iMMSNotification = mmsNotification;
	mmsNotificationDownloadV6->iLastEnvelope = ETrue;
	
	iSat.MmsNotificationDownload(reqStatus,*(mmsNotificationDownloadV6Pckg));
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotSupported);
	
	CleanupStack::PopAndDestroy(4);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8537- Successful"));
	
	return KErrNone;
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8538
 *
 *	SYMREQ REQ7335
 *
 *	SYMTestCaseDesc Cancellation of MMS NOTIFICATION DOWNLOAD request
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Register to be notified of MMS Notification download command using
 *			RSat::MmsNotificationDownload ().
 *		(ii)	Cancel the request by invoking the API CancelAsyncRequest(ESatMmsNotificationDownload).
 *
 *	SYMTestExpectedResults 
*		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */

TInt CSimSatMmsNotificationDownload::testSAT8538L()
	{
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8538"));

	TRequestStatus reqStatus;
	
	RSat::TMmsNotificationDownloadV6 *mmsNotificationDownloadV6 = new (ELeave) RSat::TMmsNotificationDownloadV6;
	CleanupStack::PushL(mmsNotificationDownloadV6);
	
	RSat::TMmsNotificationDownloadV6Pckg *mmsNotificationDownloadV6Pckg = new (ELeave) RSat::TMmsNotificationDownloadV6Pckg(*mmsNotificationDownloadV6);
	CleanupStack::PushL(mmsNotificationDownloadV6Pckg);

	_LIT(KText,"ABCDE5");
	TBuf<255> mmsNotification(KText);
	
	mmsNotificationDownloadV6->iDeviceId = RSat::KUICC;
	mmsNotificationDownloadV6->iMMSNotification = mmsNotification;
	mmsNotificationDownloadV6->iLastEnvelope = ETrue;
	
	iSat.MmsNotificationDownload(reqStatus,*(mmsNotificationDownloadV6Pckg));
	iSat.CancelAsyncRequest(ESatMmsNotificationDownload);
	User::WaitForRequest(reqStatus);
	
	TEST((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));
	
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8538- Successful"));

	return KErrNone;
	}


CSimSatUssdDataDownload::CSimSatUssdDataDownload() 
	{ 
	SetTestStepName(_L("SatTestUssdDataDownload"));
	}

TVerdict CSimSatUssdDataDownload::doTestStepL()
	{
	INFO_PRINTF1(_L("USSD DATA DOWNLOAD TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer, KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8539L();	// For testing the TEL-SIMTSY-SAT-8539
	testSAT8540L();	// For testing the TEL-SIMTSY-SAT-8540
	
	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("USSD DATA DOWNLOAD TESTING - END"));

	return TestStepResult();
	
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8539
 *
 *	SYMREQ REQ7336
 *
 *	SYMTestCaseDesc ME issues a USSD Data Download command to the UICC.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the TSatProfileV6 with TSatProfileByte23
 *			set to KCapsUssdDataDownload.
 *		(iii)	Issue a request to the UICC to trigger the download of a USSD data using
 *			RSat::NotifySendUssdPCmd (). 
 *		(iv)	The ME shall wait for an acknowledgement from the UICC.
 *		(v)	It completes when the UICC informs the TSY that the download is complete
 *
 *	SYMTestExpectedResults 
 *		Client must supply the command parameters in a packaged version of TUssdDataDownloadV6. 
 *		Device identities shall be KNetwork and KUICC.
 *		UICC responds with ‘90 00’ as MMS status and indicates USSD Data Download is successfully 
 *		transferred to the UICC.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatUssdDataDownload::testSAT8539L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8539"));
	
	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(profileV6->iSatProfileByte23 & (RSat::KCapsUssdDataDownload));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TUssdDataDownloadV6 *ussdDataV6 = new(ELeave)RSat::TUssdDataDownloadV6();
	CleanupStack::PushL(ussdDataV6);
	
	RSat::TUssdDataDownloadV6Pckg *ussdDataV6Pckg = new(ELeave)RSat::TUssdDataDownloadV6Pckg(*ussdDataV6);
	CleanupStack::PushL(ussdDataV6Pckg);
	
	ussdDataV6->iDeviceId = RSat::KNetwork;
	ussdDataV6->iUssdString.iDcs = SIMTSY_USSD_DCS;
	ussdDataV6->iUssdString.iUssdString = SIMTSY_USSD_STR;
	
	// Test posting of the UssdDataDownload request 
	iSat.UssdDataDownload(reqStatus, *(ussdDataV6Pckg));
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	
	// Testing the cancellation of the UssdDataDownload Request 
	iSat.UssdDataDownload(reqStatus, *(ussdDataV6Pckg));
	iSat.CancelAsyncRequest(ESatUssdDataDownload);
	User::WaitForRequest(reqStatus);
	TEST((reqStatus.Int() == KErrNone) || (reqStatus.Int() == KErrCancel ));
	
	
	CleanupStack::PopAndDestroy(4, profileV6);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8539- Successful"));

	return KErrNone;
	}



/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8540
 *
 *	SYMREQ REQ7336
 *
 *	SYMTestCaseDesc USSD Data Download envelope command not transferred to the UICC successfully 
 *			as the USIM service table does not support the envelope command.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Issue a request to the UICC to trigger the USSD Data Download command 
 *			using RSat:: NotifySendUssdPCmd (). 
 *		(ii)	ME receives a USSD DATA DOWNLOAD and sends a FACILITY message to the network .
 *
 *	SYMTestExpectedResults 
 *		Client must supply the command parameters in a packaged version of TUssdDataDownloadV6.  
 *		As USSD Data Download not recognised by UICC, ME sends FACILITY message to the network.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatUssdDataDownload::testSAT8540L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8542"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TEST((profileV6->iSatProfileByte23 & RSat::KCapsUssdDataDownload) > 0);
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TUssdDataDownloadV6 *ussdDataV6 = new(ELeave)RSat::TUssdDataDownloadV6();
	CleanupStack::PushL(ussdDataV6);
	
	RSat::TUssdDataDownloadV6Pckg *ussdDataV6Pckg = new(ELeave)RSat::TUssdDataDownloadV6Pckg(*ussdDataV6);
	CleanupStack::PushL(ussdDataV6Pckg);

	ussdDataV6->iDeviceId = RSat::KNetwork;
	ussdDataV6->iUssdString.iDcs = SIMTSY_USSD_DCS;
	ussdDataV6->iUssdString.iUssdString = SIMTSY_NOTFN_DNLD;
	
	iSat.UssdDataDownload(reqStatus,*(ussdDataV6Pckg));
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNotSupported);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8542- Successful"));

	return KErrNone;

	}



CSimSatProvideLocalInfo::CSimSatProvideLocalInfo() 
	{ 
	SetTestStepName(_L("SatTestProvideLocalInfo"));
	}

TVerdict CSimSatProvideLocalInfo::doTestStepL()
	{
	INFO_PRINTF1(_L("PROVIDE LOCAL INFORMATION TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8544L();	// For testing the TEL-SIMTSY-SAT-8544
	testSAT8545L();	// For testing the TEL-SIMTSY-SAT-8545
	testSAT8546L();	// For testing the TEL-SIMTSY-SAT-8546
	testSAT8547L();	// For testing the TEL-SIMTSY-SAT-8547
	testSAT8548L();	// For testing the TEL-SIMTSY-SAT-8548
	testSAT8549L();	// For testing the TEL-SIMTSY-SAT-8549
	
	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("PROVIDE LOCAL INFORMATION TESTING - END"));

	return TestStepResult();
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8544
 *
 *	SYMREQ REQ7337
 *
 *	SYMTestCaseDesc Extended Support for PROVIDE LOCAL INFORMATION that includes additional 3GPP R6 parameters
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication with RSat::TSatProfileV6Pckg with 
 *			(a)	KCapsProvideLocalInfoMEID set for the TSatProfileByte23.
 *			(b)	KCapsProvideLocalInfoUTRAN and KCapsProvideLocalInfoBatteryState 
 *				set for the TSatProfileByte22.
 *			(c)	KCapsProvideLocalInfoIMEISV and KCapsProvideLocalInfoSearchModeChange 
 *				set for the TSatProfileByte18.
 *		(iii)	The client notifies the ME for PROVIDE LOCAL INFORMATION using the RSat::NotifyLocalInfoPCmd()
 *		(iv)	UICC issues the proactive command PROVIDE LOCAL INFORMATION to ME.
 *		(v)	The ME responds back with the RSat::TerminalRsp with the TLocalInfoRspV3 as the response 
 *			parameter.
 *
 *	SYMTestExpectedResults 
 *		The TSatProfileV6Pckg from the step (i) shall have 
 *			(i)	KCapsProvideLocalInfoIMEISV and KCapsProvideLocalInfoSearchModeChange
 *				 in the TSatProfileByte18 attribute.
 *			(ii)	KCapsProvideLocalInfoUTRAN and KCapsProvideLocalInfoBatteryState in the 
 *				TSatProfileByte22 attribute.
 *			(iii)	KCapsProvideLocalInfoMEID in the TSatProfileByte23 attribute.
 *		ClientSatProfileIndication shall return KErrNone.
 *		ME shall respond with KSuccess  in the Terminal response
 *
 * 	SYMTestType CIT
 */

TInt CSimSatProvideLocalInfo::testSAT8544L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8544"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL((profileV6->iSatProfileByte23 & (RSat::KCapsProvideLocalInfoMEID)));
	TESTL(profileV6->iSatProfileByte22 & (RSat::KCapsProvideLocalInfoBatteryState));
	TESTL((profileV6->iSatProfileByte18 & (RSat::KCapsProvideLocalInfoIMEISV)) ||
			(profileV6->iSatProfileByte18 & (RSat::KCapsProvideLocalInfoSearchModeChange)));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TLocalInfoV6 *tlocalInfoV6 = new(ELeave) RSat::TLocalInfoV6();
	CleanupStack::PushL(tlocalInfoV6);
	
	RSat::TLocalInfoV6Pckg *tLclInfoV6Pckg = new(ELeave)RSat::TLocalInfoV6Pckg(*tlocalInfoV6);
	CleanupStack::PushL(tLclInfoV6Pckg);
	
	iSat.NotifyLocalInfoPCmd(reqStatus,*(tLclInfoV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tlocalInfoV6->iDevideId == RSat::KKeypad);
	TESTL(tlocalInfoV6->iInfoType == RSat::KProvideLocalInfoChargeStateOfBattery);
	TESTL(tlocalInfoV6->iUtranMeasurementQualifier 
			== RSat::EUtranMeasurementQualifierIntraFrequency);
	
	RSat::TLocalInfoRspV3 *tLocalInfoRsp = new (ELeave) RSat::TLocalInfoRspV3;
	CleanupStack::PushL(tLocalInfoRsp);
	
	RSat::TLocalInfoRspV3Pckg *tLocalInfoRspPckgV6 = new (ELeave) RSat::TLocalInfoRspV3Pckg(*tLocalInfoRsp);
	CleanupStack::PushL(tLocalInfoRspPckgV6);
	
	tLocalInfoRsp->iGeneralResult = RSat::KSuccess;
	tLocalInfoRsp->iInfoType = RSat::KNoAdditionalInfo;
	tLocalInfoRsp->SetPCmdNumber(SIMTSY_PRV_LCL_INFO_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ELocalInfo,*(tLocalInfoRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(6);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8544- Successful"));
				
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8545
 *
 *	SYMREQ REQ7337
 *
 *	SYMTestCaseDesc Extended Support for PROVIDE LOCAL INFORMATION that includes additional 3GPP R6 parameters 
 *			when the terminal has no service currently available.
 *
 *	SYMTestPriority High
 *		
 *	SYMTestActions 
 *		(i)	The client notifies the ME for PROVIDE LOCAL INFORMATION using the RSat::NotifyLocalInfoPCmd()
 *		(ii)	UICC issues the proactive command PROVIDE LOCAL INFORMATION to ME.
 *		(iii)	The ME responds back with the RSat::TerminalRsp with the TLocalInfoRspV3 as the response 
 *			parameter.
 *
 *	SYMTestExpectedResults 
 *		ME shall respond with KMeUnableToProcessCmd in the Terminal response with KNoService 
 *		as the additional information.
 * 	SYMTestType CIT
 */

TInt CSimSatProvideLocalInfo::testSAT8545L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8545"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(!(profileV6->iSatProfileByte23 & (RSat::KCapsProvideLocalInfoMEID)));
	TESTL(!(profileV6->iSatProfileByte22 & (RSat::KCapsProvideLocalInfoBatteryState)));
	TESTL(!((profileV6->iSatProfileByte18 & (RSat::KCapsProvideLocalInfoIMEISV)) ||
			(profileV6->iSatProfileByte18 & (RSat::KCapsProvideLocalInfoSearchModeChange))));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TLocalInfoV6 *tlocalInfoV6 = new(ELeave) RSat::TLocalInfoV6();
	CleanupStack::PushL(tlocalInfoV6);
	
	RSat::TLocalInfoV6Pckg *tLclInfoV6Pckg = new(ELeave)RSat::TLocalInfoV6Pckg(*tlocalInfoV6);
	CleanupStack::PushL(tLclInfoV6Pckg);
	
	iSat.NotifyLocalInfoPCmd(reqStatus,*(tLclInfoV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNotSupported);
	
	RSat::TLocalInfoRspV3 *tLocalInfoRsp = new (ELeave) RSat::TLocalInfoRspV3;
	CleanupStack::PushL(tLocalInfoRsp);
	
	RSat::TLocalInfoRspV3Pckg *tLocalInfoRspPckgV6 = new (ELeave) RSat::TLocalInfoRspV3Pckg(*tLocalInfoRsp);
	CleanupStack::PushL(tLocalInfoRspPckgV6);
	
	_LIT(KText,"KNoService");
	TBuf<254> buf(KText);
	tLocalInfoRsp->iGeneralResult = RSat::KMeUnableToProcessCmd;
	tLocalInfoRsp->iInfoType = RSat::KMeProblem;
	tLocalInfoRsp->iAdditionalInfo = buf;
	tLocalInfoRsp->SetPCmdNumber(SIMTSY_PRV_LCL_INFO_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::ELocalInfo,*(tLocalInfoRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(6);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8545- Successful"));
	
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8546
 *
 *	SYMREQ REQ7337
 *
 *	SYMTestCaseDesc Cancellation of the PROVIDE LOCAL INFORMATION proactive command with additional 3GPP 
 *			R6 parameters
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for PROVIDE LOCAL INFORMATION using the RSat::NotifyLocalInfoPCmd().
 *		(ii)	Cancel the placed request using the API RSat::CancelAsyncRequest(ESatNotifyLocalInfoPCmd).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */

TInt CSimSatProvideLocalInfo::testSAT8546L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8546"));

	TRequestStatus reqStatus;
		
	RSat::TLocalInfoV6 *tlocalInfoV6 = new(ELeave) RSat::TLocalInfoV6();
	CleanupStack::PushL(tlocalInfoV6);
	
	RSat::TLocalInfoV6Pckg *tLclInfoV6Pckg = new(ELeave)RSat::TLocalInfoV6Pckg(*tlocalInfoV6);
	CleanupStack::PushL(tLclInfoV6Pckg);
	
	iSat.NotifyLocalInfoPCmd(reqStatus,*(tLclInfoV6Pckg));
	iSat.CancelAsyncRequest(ESatNotifyLocalInfoPCmd);
	User::WaitForRequest(reqStatus);
	
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));
	
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8546- Successful"));
	
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8547
 *
 *	SYMREQ REQ7338
 *
 *	SYMTestCaseDesc Extended Support for PROVIDE LOCAL INFORMATION that includes network measurement
 *			 results for UTRAN
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference.
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the reference received in the above step.
 *		(iii)	The client notifies the ME for the PROVIDE LOCAL INFORMATION using the 
 *			RSat::NotifyLocalInfoPCmd ().
 *		(iv)	The ME responds back with the RSat::TerminalRsp with the TLocalInfoRspV3 
 *			as the response parameter.
 *
 *	SYMTestExpectedResults 
 *		The Terminal Response shall contain the class RSat::TLocalInfoRspV3 .
 *		ME returns KSuccess in the terminal response.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatProvideLocalInfo::testSAT8547L()
	{
		
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8547"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL((profileV6->iSatProfileByte23 & (RSat::KCapsProvideLocalInfoMEID)));
	TESTL((profileV6->iSatProfileByte22 & (RSat::KCapsProvideLocalInfoUTRAN)) || 
			(profileV6->iSatProfileByte22 & (RSat::KCapsProvideLocalInfoBatteryState)));
	TESTL((profileV6->iSatProfileByte18 & (RSat::KCapsProvideLocalInfoIMEISV)) ||
			(profileV6->iSatProfileByte18 & (RSat::KCapsProvideLocalInfoSearchModeChange)));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TLocalInfoV6 *tlocalInfoV6 = new(ELeave) RSat::TLocalInfoV6();
	CleanupStack::PushL(tlocalInfoV6);
	
	RSat::TLocalInfoV6Pckg *tLclInfoV6Pckg = new(ELeave)RSat::TLocalInfoV6Pckg(*tlocalInfoV6);
	CleanupStack::PushL(tLclInfoV6Pckg);
	
	iSat.NotifyLocalInfoPCmd(reqStatus,*(tLclInfoV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone); 
	
	TESTL(tlocalInfoV6->iDevideId == RSat::KKeypad);
	TESTL(tlocalInfoV6->iInfoType == RSat::KProvideLocalInfoChargeStateOfBattery);
	TESTL(tlocalInfoV6->iUtranMeasurementQualifier 
			== RSat::EUtranMeasurementQualifierInterFrequency);
	
	RSat::TLocalInfoRspV3 *tLocalInfoRsp = new (ELeave) RSat::TLocalInfoRspV3;
	CleanupStack::PushL(tLocalInfoRsp);
	
	RSat::TLocalInfoRspV3Pckg *tLocalInfoRspPckgV6 = new (ELeave) RSat::TLocalInfoRspV3Pckg(*tLocalInfoRsp);
	CleanupStack::PushL(tLocalInfoRspPckgV6);
	
	tLocalInfoRsp->iGeneralResult = RSat::KSuccess;
	tLocalInfoRsp->iInfoType = RSat::KNoAdditionalInfo;
	tLocalInfoRsp->SetPCmdNumber(SIMTSY_PRV_LCL_INFO_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ELocalInfo,*(tLocalInfoRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(6);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8547- Successful"));
				
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8548
 *
 *	SYMREQ REQ7338
 *
 *	SYMTestCaseDesc Extended Support for PROVIDE LOCAL INFORMATION when the network is of limited service.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the PROVIDE LOCAL INFORMATION using the 
 *			RSat::NotifyLocalInfoPCmd ().
 *		(ii)	The ME responds back with the RSat::TerminalRsp with the TLocalInfoRspV3 
 *			as the response parameter.
 *
 *	SYMTestExpectedResults 
 *		The Terminal Response shall contain the class RSat::TLocalInfoRspV3. 
 *		The ME shall respond with KSuccessLimitedService in the terminal response.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatProvideLocalInfo::testSAT8548L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8548"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL((profileV6->iSatProfileByte23 & (RSat::KCapsProvideLocalInfoMEID)));
	TESTL(!(profileV6->iSatProfileByte22 & (RSat::KCapsProvideLocalInfoUTRAN)));
	TESTL(((profileV6->iSatProfileByte18 & (RSat::KCapsProvideLocalInfoIMEISV)) ||
			(profileV6->iSatProfileByte18 & (RSat::KCapsProvideLocalInfoSearchModeChange))));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TLocalInfoV6 *tlocalInfoV6 = new(ELeave) RSat::TLocalInfoV6();
	CleanupStack::PushL(tlocalInfoV6);
	
	RSat::TLocalInfoV6Pckg *tLclInfoV6Pckg = new(ELeave)RSat::TLocalInfoV6Pckg(*tlocalInfoV6);
	CleanupStack::PushL(tLclInfoV6Pckg);
	
	iSat.NotifyLocalInfoPCmd(reqStatus,*(tLclInfoV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNotSupported);
	
	RSat::TLocalInfoRspV3 *tLocalInfoRsp = new (ELeave) RSat::TLocalInfoRspV3;
	CleanupStack::PushL(tLocalInfoRsp);
	
	RSat::TLocalInfoRspV3Pckg *tLocalInfoRspPckgV6 = new (ELeave) RSat::TLocalInfoRspV3Pckg(*tLocalInfoRsp);
	CleanupStack::PushL(tLocalInfoRspPckgV6);
	
	tLocalInfoRsp->iGeneralResult = RSat::KSuccessLimitedService ;
	tLocalInfoRsp->iInfoType = RSat::KNoAdditionalInfo;
	tLocalInfoRsp->SetPCmdNumber(SIMTSY_PRV_LCL_INFO_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::ELocalInfo,*(tLocalInfoRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(6);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8548- Successful"));
	
	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8549
 *
 *	SYMREQ REQ7338
 *
 *	SYMTestCaseDesc Extended Support for PROVIDE LOCAL INFORMATION when the 
 *			network is connected to different radio access technology (RAT).
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the PROVIDE LOCAL INFORMATION 
 *			using the RSat::NotifyLocalInfoPCmd ().
 *		(ii)	The UICC responds back with the RSat::TerminalRsp with the
 *			TLocalInfoRspV3 as the response parameter.
 *
 *	SYMTestExpectedResults 
 *		The Terminal Response shall contain the class RSat::TLocalInfoRspV3. 
 *		The ME shall respond with KMeUnableToProcessCmd in the terminal response.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatProvideLocalInfo::testSAT8549L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8549"));

	TRequestStatus reqStatus;
	
	RSat::TLocalInfoV6 *tlocalInfoV6 = new(ELeave) RSat::TLocalInfoV6();
	CleanupStack::PushL(tlocalInfoV6);
	
	RSat::TLocalInfoV6Pckg *tLclInfoV6Pckg = new(ELeave)RSat::TLocalInfoV6Pckg(*tlocalInfoV6);
	CleanupStack::PushL(tLclInfoV6Pckg);
	
	iSat.NotifyLocalInfoPCmd(reqStatus,*(tLclInfoV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNotSupported);
	
	RSat::TLocalInfoRspV3 *tLocalInfoRsp = new (ELeave) RSat::TLocalInfoRspV3;
	CleanupStack::PushL(tLocalInfoRsp);
	
	RSat::TLocalInfoRspV3Pckg *tLocalInfoRspPckgV6 = new (ELeave) RSat::TLocalInfoRspV3Pckg(*tLocalInfoRsp);
	CleanupStack::PushL(tLocalInfoRspPckgV6);
	
	tLocalInfoRsp->iGeneralResult = RSat::KMeUnableToProcessCmd;
	tLocalInfoRsp->iInfoType = RSat::KNoAdditionalInfo;
	tLocalInfoRsp->SetPCmdNumber(SIMTSY_PRV_LCL_INFO_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::ELocalInfo,*(tLocalInfoRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8549 - Successful"));
	
	return KErrNone;
	
	}

CSimSatLnchBrwsr::CSimSatLnchBrwsr() 
	{ 
	SetTestStepName(_L("SatTestLaunchBrowser"));
	}

TVerdict CSimSatLnchBrwsr::doTestStepL()
	{
	INFO_PRINTF1(_L("LAUNCH BROWSER TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8550L();	// For testing the TEL-SIMTSY-SAT-8550
	testSAT8551L();	// For testing the TEL-SIMTSY-SAT-8551
	testSAT8552L();	// For testing the TEL-SIMTSY-SAT-8552
	testSAT8553L();	// For testing the TEL-SIMTSY-SAT-8553
	testSAT8554L();	// For testing the TEL-SIMTSY-SAT-8554
	testSAT8555L();	// For testing the TEL-SIMTSY-SAT-8555
	
	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("LAUNCH BROWSER TESTING - END"));

	return TestStepResult();
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8550
 *
 *	SYMREQ REQ7346
 *
 *	SYMTestCaseDesc R6 support for LAUNCH BROWSER proactive command
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the reference received in the above step.
 *		(iii)	The client notifies the ME for the LAUNCH BROWSER using the RSat::NotifyLaunchBrowserPCmd ().
 *		(iv)	The ME responds back with the RSat::TerminalRsp with the TLaunchBrowserRspV2 as the 
 *			response parameter.
 *
 *	SYMTestExpectedResults 
 *		TSatProfileByte21 for Release 6 packaged in TSatProfileV6 shall have one of the following 
 *		types of browsing: XML, WXML, HTML and CHTML.
 *		Terminal response contains value as KSuccess in the Response package.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatLnchBrwsr::testSAT8550L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8550"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(profileV6->iSatProfileByte21 & (RSat::KCapsXHTML));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TLaunchBrowserV6* tLnchBrwsr = new (ELeave) RSat::TLaunchBrowserV6();
	CleanupStack::PushL(tLnchBrwsr);
	
	RSat::TLaunchBrowserV6Pckg* tLnchBrwsrPckg = new(ELeave) RSat::RSat::TLaunchBrowserV6Pckg(*tLnchBrwsr);
	CleanupStack::PushL(tLnchBrwsrPckg);
	
	iSat.NotifyLaunchBrowserPCmd(reqStatus,*(tLnchBrwsrPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	TESTL(tLnchBrwsr->iFrameId == SIMTSY_LAUNCH_BROWSER_FRM_ID);
	
	RSat::TLaunchBrowserRspV2 *tLnchBrRsp = new (ELeave) RSat::TLaunchBrowserRspV2;
	CleanupStack::PushL(tLnchBrRsp);
	
	RSat::TLaunchBrowserRspV2Pckg *tLnchBrRspPckg = new (ELeave) RSat::TLaunchBrowserRspV2Pckg(*tLnchBrRsp);
	CleanupStack::PushL(tLnchBrRspPckg);
	
	tLnchBrRsp->iGeneralResult = RSat::KSuccess;
	tLnchBrRsp->iInfoType = RSat::KNoAdditionalInfo;
	tLnchBrRsp->SetPCmdNumber(SIMTSY_LNCH_BRWSR_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::ELaunchBrowser,*(tLnchBrRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(6);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8550 - Successful"));
	
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8551
 *
 *	SYMREQ REQ7346
 *
 *	SYMTestCaseDesc LAUNCH BROWSER proactive command without the R6 support.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the reference received in the above step.
 *		(iii)	The client notifies the ME for the LAUNCH BROWSER using the RSat::NotifyLaunchBrowserPCmd ().
 *		(iv)	The ME responds back with the RSat::TerminalRsp with the TLaunchBrowserRspV2 as the 
 *			response parameter.
 *
 *	SYMTestExpectedResults 
 *		TBrowserIdentity in the TSatProfileByte21 for Release 6 packaged in TSatProfileV6, 
 *		with one of the following types of browsing: XML, WXML, HTML and CHTML.
 *		Terminal response contains value as KErrCorrupt in the Response package
 *
 * 	SYMTestType CIT
 */

TInt CSimSatLnchBrwsr::testSAT8551L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8551"));

	TRequestStatus reqStatus;

	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(!(profileV6->iSatProfileByte21 & (RSat::KCapsXHTML)));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TLaunchBrowserV6* tLnchBrwsr = new (ELeave) RSat::TLaunchBrowserV6();
	CleanupStack::PushL(tLnchBrwsr);
	
	RSat::TLaunchBrowserV6Pckg* tLnchBrwsrPckg = new(ELeave) RSat::RSat::TLaunchBrowserV6Pckg(*tLnchBrwsr);
	CleanupStack::PushL(tLnchBrwsrPckg);
	
	iSat.NotifyLaunchBrowserPCmd(reqStatus,*(tLnchBrwsrPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	TESTL(tLnchBrwsr->iFrameId == SIMTSY_LAUNCH_BROWSER_FRM_ID);
	
	RSat::TLaunchBrowserRspV2 *tLnchBrRsp = new (ELeave) RSat::TLaunchBrowserRspV2;
	CleanupStack::PushL(tLnchBrRsp);
	
	RSat::TLaunchBrowserRspV2Pckg *tLnchBrRspPckg = new (ELeave) RSat::TLaunchBrowserRspV2Pckg(*tLnchBrRsp);
	CleanupStack::PushL(tLnchBrRspPckg);
	
	_LIT(KText,"KNoSpecificBrowserError");
	TBuf<254> buf(KText);
	tLnchBrRsp->iGeneralResult = RSat::KLaunchBrowserError;
	tLnchBrRsp->iInfoType = RSat::KMeProblem;
	tLnchBrRsp->iAdditionalInfo = buf;
	tLnchBrRsp->SetPCmdNumber(SIMTSY_LNCH_BRWSR_PCMD_NUMBER);
	
	iSat.TerminalRsp(reqStatus,RSat::ELaunchBrowser,*(tLnchBrRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(6);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8551 - Successful"));
	
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8552
 *
 *	SYMREQ REQ7346
 *
 *	SYMTestCaseDesc LAUNCH BROWSER proactive command the R6 support when the browser is not available.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *	(i)	The client notifies the ME for the LAUNCH BROWSER using the RSat::NotifyLaunchBrowserPCmd ().
 *	(ii)	The ME responds back with the RSat::TerminalRsp with the TLaunchBrowserRspV2 as the response parameter.
 *
 *	SYMTestExpectedResults 
 *		Terminal response contains value as KMeUnableToProcessCmd in the Response package and
 *		 the iAdditionalInfo of TLaunchBrowserRspV2 shall have KBrowserUnavailable.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatLnchBrwsr::testSAT8552L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8552"));

	TRequestStatus reqStatus;

	RSat::TLaunchBrowserV6* tLnchBrwsr = new (ELeave) RSat::TLaunchBrowserV6();
	CleanupStack::PushL(tLnchBrwsr);
	
	RSat::TLaunchBrowserV6Pckg* tLnchBrwsrPckg = new(ELeave) RSat::RSat::TLaunchBrowserV6Pckg(*tLnchBrwsr);
	CleanupStack::PushL(tLnchBrwsrPckg);
	
	iSat.NotifyLaunchBrowserPCmd(reqStatus,*(tLnchBrwsrPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	TESTL(tLnchBrwsr->iFrameId == SIMTSY_LAUNCH_BROWSER_FRM_ID);
	
	RSat::TLaunchBrowserRspV2 *tLnchBrRsp = new (ELeave) RSat::TLaunchBrowserRspV2;
	CleanupStack::PushL(tLnchBrRsp);
	
	RSat::TLaunchBrowserRspV2Pckg *tLnchBrRspPckg = new (ELeave) RSat::TLaunchBrowserRspV2Pckg(*tLnchBrRsp);
	CleanupStack::PushL(tLnchBrRspPckg);
	
	_LIT(KText,"KBrowserUnavailable");
	TBuf<254> buf(KText);
	tLnchBrRsp->iGeneralResult = RSat::KLaunchBrowserError;
	tLnchBrRsp->iInfoType = RSat::KMeProblem;
	tLnchBrRsp->iAdditionalInfo = buf;
	tLnchBrRsp->SetPCmdNumber(SIMTSY_LNCH_BRWSR_PCMD_NUMBER);
	
	iSat.TerminalRsp(reqStatus,RSat::ELaunchBrowser,*(tLnchBrRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8552 - Successful"));
	
	return KErrNone;
	
	}
	

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8553
 *
 *	SYMREQ REQ7346
 *
 *	SYMTestCaseDesc LAUNCH BROWSER proactive command the R6 support when the bearer is not available.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *	(i)	The client notifies the ME for the LAUNCH BROWSER using the RSat::NotifyLaunchBrowserPCmd ().
 *	(ii)	The ME responds back with the RSat::TerminalRsp with the TLaunchBrowserRspV2 as the response parameter.
 *
 *	SYMTestExpectedResults 
 *		Terminal response contains value as KMeUnableToProcessCmd in the Response package and 
 *		the iAdditionalInfo of TLaunchBrowserRspV2 shall have KBearerUnvailable.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatLnchBrwsr::testSAT8553L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8553"));

	TRequestStatus reqStatus;
	
	RSat::TLaunchBrowserV6* tLnchBrwsr = new (ELeave) RSat::TLaunchBrowserV6();
	CleanupStack::PushL(tLnchBrwsr);
	
	
	RSat::TLaunchBrowserV6Pckg* tLnchBrwsrPckg = new(ELeave) RSat::RSat::TLaunchBrowserV6Pckg(*tLnchBrwsr);
	CleanupStack::PushL(tLnchBrwsrPckg);
	
	iSat.NotifyLaunchBrowserPCmd(reqStatus,*(tLnchBrwsrPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	TESTL(tLnchBrwsr->iFrameId == SIMTSY_LAUNCH_BROWSER_FRM_ID);
	
	RSat::TLaunchBrowserRspV2 *tLnchBrRsp = new (ELeave) RSat::TLaunchBrowserRspV2;
	CleanupStack::PushL(tLnchBrRsp);
	
	RSat::TLaunchBrowserRspV2Pckg *tLnchBrRspPckg = new (ELeave) RSat::TLaunchBrowserRspV2Pckg(*tLnchBrRsp);
	CleanupStack::PushL(tLnchBrRspPckg);
	
	_LIT(KText,"KBearerUnvailable");
	TBuf<254> buf(KText);
	tLnchBrRsp->iGeneralResult = RSat::KLaunchBrowserError;
	tLnchBrRsp->iInfoType = RSat::KMeProblem;
	tLnchBrRsp->iAdditionalInfo = buf;
	tLnchBrRsp->SetPCmdNumber(SIMTSY_LNCH_BRWSR_PCMD_NUMBER);
	
	iSat.TerminalRsp(reqStatus,RSat::ELaunchBrowser,*(tLnchBrRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8553 - Successful"));
	
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8554
 *
 *	SYMREQ REQ7346 & REQ7343
 *
 *	SYMTestCaseDesc LAUNCH BROWSER proactive command with the R6 support when error occurs
 *			 when accessing the resource in the URL.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *	(i)	The client notifies the ME for the LAUNCH BROWSER using the RSat::NotifyLaunchBrowserPCmd ().
 *	(ii)	The ME responds back with the RSat::TerminalRsp with the TLaunchBrowserRspV2 as the response parameter.
 *	(iii)	Call the API EventDownload with the TEventList as RSat::KBrowserStatusChange and 
 *		package class as TBrowserStatusV6.
 *
 *	SYMTestExpectedResults 
 *		Terminal response contains value as KMeUnableToProcessCmd in the Response package and 
 *		the iAdditionalInfo of TLaunchBrowserRspV2 shall have KMeUnableToReadProvisioningData.
 *		The TBrowserStatusV6 shall contain the error cause from the network.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatLnchBrwsr::testSAT8554L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8554"));

	TRequestStatus reqStatus;
	
	RSat::TLaunchBrowserV6* tLnchBrwsr = new (ELeave) RSat::TLaunchBrowserV6();
	CleanupStack::PushL(tLnchBrwsr);
	
	RSat::TLaunchBrowserV6Pckg* tLnchBrwsrPckg = new(ELeave) RSat::RSat::TLaunchBrowserV6Pckg(*tLnchBrwsr);
	CleanupStack::PushL(tLnchBrwsrPckg);
	
	iSat.NotifyLaunchBrowserPCmd(reqStatus,*(tLnchBrwsrPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	TESTL(tLnchBrwsr->iFrameId == SIMTSY_LAUNCH_BROWSER_FRM_ID);
	
	RSat::TLaunchBrowserRspV2 *tLnchBrRsp = new (ELeave) RSat::TLaunchBrowserRspV2;
	CleanupStack::PushL(tLnchBrRsp);
	
	RSat::TLaunchBrowserRspV2Pckg *tLnchBrRspPckg = new (ELeave) RSat::TLaunchBrowserRspV2Pckg(*tLnchBrRsp);
	CleanupStack::PushL(tLnchBrRspPckg);
	
	_LIT(KText,"KMeUnableToReadProvisioningData");
	TBuf<254> buf(KText);
	tLnchBrRsp->iGeneralResult = RSat::KLaunchBrowserError;
	tLnchBrRsp->iInfoType = RSat::KMeProblem;
	tLnchBrRsp->iAdditionalInfo = buf;
	tLnchBrRsp->SetPCmdNumber(SIMTSY_LNCH_BRWSR_PCMD_NUMBER);
	
	iSat.TerminalRsp(reqStatus,RSat::ELaunchBrowser,*(tLnchBrRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	RSat::TBrowsingStatusEventV6 *tBrwStsV6 = new(ELeave) RSat::TBrowsingStatusEventV6();
	CleanupStack::PushL(tBrwStsV6);
	
	RSat::TBrowsingStatusEventV6Pckg *tBrwStsV6Pckg = new(ELeave)RSat::TBrowsingStatusEventV6Pckg(*tBrwStsV6);
	CleanupStack::PushL(tBrwStsV6Pckg);
	
	tBrwStsV6->iBrowserStatus = 0x55;  // 0x55 assumed as Error status from the network.
	
	iSat.EventDownload(reqStatus,RSat::KBrowsingStatusChange ,*(tBrwStsV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);

	CleanupStack::PopAndDestroy(6);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8554 - Successful"));
	
	return KErrNone;
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8555
 *
 *	SYMREQ REQ7346
 *
 *	SYMTestCaseDesc Cancel for the LAUNCH BROWSER proactive command
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the LAUNCH BROWSER using the RSat::NotifyLaunchBrowserPCmd ().
 *		(ii)	Issue cancellation of the Proactive command using the CancelAsyncRequest 
 *			(ESatNotifyLaunchBrowserPCmd).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatLnchBrwsr::testSAT8555L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8555"));

	TRequestStatus reqStatus;
	
	RSat::TLaunchBrowserV6* tLnchBrwsr = new (ELeave) RSat::TLaunchBrowserV6();
	CleanupStack::PushL(tLnchBrwsr);
	
	RSat::TLaunchBrowserV6Pckg* tLnchBrwsrPckg = new(ELeave) RSat::RSat::TLaunchBrowserV6Pckg(*tLnchBrwsr);
	CleanupStack::PushL(tLnchBrwsrPckg);
	
	iSat.NotifyLaunchBrowserPCmd(reqStatus,*(tLnchBrwsrPckg));
	iSat.CancelAsyncRequest(ESatNotifyLaunchBrowserPCmd);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int() == KErrNone) || (reqStatus.Int()==KErrCancel));
	
	CleanupStack::PopAndDestroy(2);
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8555 - Successful"));
	
	return KErrNone;
	
	}

CSimSatOpenChnl::CSimSatOpenChnl() 
	{ 
	SetTestStepName(_L("SatTestOpenChnl"));
	}

TVerdict CSimSatOpenChnl::doTestStepL()
	{
	INFO_PRINTF1(_L("OPEN CHANNEL TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8556L();	// For testing the TEL-SIMTSY-SAT-8556
	testSAT8557L();	// For testing the TEL-SIMTSY-SAT-8557
	testSAT8558L();	// For testing the TEL-SIMTSY-SAT-8558
	testSAT8559L();	// For testing the TEL-SIMTSY-SAT-8559
	
	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("OPEN CHANNEL TESTING - END"));

	return TestStepResult();
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8556
 *
 *	SYMREQ REQ7345
 *
 *	SYMTestCaseDesc R6 support for  “Open Channel using UTRAN packet service and HSDPA”
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication with the parameter as a reference of the
 *			object RSat::TSatProfileV6 with the TSatProfileByte22 with KUtranAndHsdpaBearer set.
 *		(iii)	The client notifies the ME for the OPEN CHANNEL using the RSat::NotifyOpenChannelPCmd ()
 *		(iv)	The ME responds back with the RSat::TerminalRsp with the TOpenChannelRspV2 as the 
 *			response parameter.
 *
 *	SYMTestExpectedResults 
 *		Terminal response contains value as KSuccess in the response message.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatOpenChnl::testSAT8556L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8556"));

	TRequestStatus reqStatus;
	
	RSat::TOpenLocalLinksChannelV6 *tOpenChnV6 = new(ELeave) RSat::TOpenLocalLinksChannelV6();
	CleanupStack::PushL(tOpenChnV6);
	
	RSat::TOpenLocalLinksChannelV6Pckg *tOpenChnV6Pckg = new(ELeave)RSat::TOpenLocalLinksChannelV6Pckg(*tOpenChnV6);
	CleanupStack::PushL(tOpenChnV6Pckg);
	
	iSat.NotifyOpenChannelPCmd(reqStatus,*(tOpenChnV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tOpenChnV6->iBearer.iType == RSat::EUtranBearer);
	
	RSat::TOpenChannelRspV2 *tOpenChnRsp = new (ELeave) RSat::TOpenChannelRspV2;
	CleanupStack::PushL(tOpenChnRsp);
	
	RSat::TOpenChannelRspV2Pckg *tOpenChnRspPckg = new (ELeave) RSat::TOpenChannelRspV2Pckg(*tOpenChnRsp);
	CleanupStack::PushL(tOpenChnRspPckg);
	
	tOpenChnRsp->iGeneralResult = RSat::KSuccess;
	tOpenChnRsp->iInfoType = RSat::KNoAdditionalInfo;
	tOpenChnRsp->SetPCmdNumber(SIMTSY_OPEN_CHAN_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::EOpenChannelLocal,*(tOpenChnRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8556 - Successful"));
	
	return KErrNone;
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8557
 *
 *	SYMREQ REQ7345
 *
 *	SYMTestCaseDesc R6 support for “Open Channel using UTRAN packet service and HSDPA” with client
 *			 responding busy on SS transaction or unable to activate a PDP context.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the OPEN CHANNEL using the RSat::NotifyOpenChannelPCmd ()
 *		(ii)	The ME responds back with the RSat::TerminalRsp with the TOpenChannelRspV2 as the 
 *			response parameter.
 *			
 *	SYMTestExpectedResults 
 *		Terminal response contains value as KNetworkUnableToProcessCmd in the response message.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatOpenChnl::testSAT8557L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8557"));

	TRequestStatus reqStatus;
	
	RSat::TOpenLocalLinksChannelV6 *tOpenChnV6 = new(ELeave) RSat::TOpenLocalLinksChannelV6();
	CleanupStack::PushL(tOpenChnV6);
	
	RSat::TOpenLocalLinksChannelV6Pckg *tOpenChnV6Pckg = new(ELeave)RSat::TOpenLocalLinksChannelV6Pckg(*tOpenChnV6);
	CleanupStack::PushL(tOpenChnV6Pckg);
	
	iSat.NotifyOpenChannelPCmd(reqStatus,*(tOpenChnV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tOpenChnV6->iBearer.iType == RSat::EUtranBearer);
	
	RSat::TOpenChannelRspV2 *tOpenChnRsp = new (ELeave) RSat::TOpenChannelRspV2;
	CleanupStack::PushL(tOpenChnRsp);
	
	RSat::TOpenChannelRspV2Pckg *tOpenChnRspPckg = new (ELeave) RSat::TOpenChannelRspV2Pckg(*tOpenChnRsp);
	CleanupStack::PushL(tOpenChnRspPckg);
	
	_LIT(KText,"KNoSpecificMeProblem");
	TBuf<254> buf(KText);
	tOpenChnRsp->iGeneralResult = RSat::KNetworkUnableToProcessCmd ;
	tOpenChnRsp->iInfoType = RSat::KMeProblem;
	tOpenChnRsp->iAdditionalInfo = buf;
	tOpenChnRsp->SetPCmdNumber(SIMTSY_OPEN_CHAN_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::EOpenChannelLocal,*(tOpenChnRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);

	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8557 - Successful"));
	
	return KErrNone;
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8558 
 *
 *	SYMREQ REQ7345
 *
 *	SYMTestCaseDesc R6 support for “Open Channel using UTRAN packet service and HSDPA” with the terminal
 *			 not able to activate a packet data service with the requested parameters.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the OPEN CHANNEL using the RSat::NotifyOpenChannelPCmd ()
 *		(ii)	The ME responds back with the RSat::TerminalRsp with the TOpenChannelRspV2 as the 
 *			response parameter
 *
 *	SYMTestExpectedResults 
 *		Terminal response contains value as KPerformedWithModifications in the response message.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatOpenChnl::testSAT8558L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8558"));

	TRequestStatus reqStatus;
	
	RSat::TOpenLocalLinksChannelV6 *tOpenChnV6 = new(ELeave) RSat::TOpenLocalLinksChannelV6();
	CleanupStack::PushL(tOpenChnV6);
	
	RSat::TOpenLocalLinksChannelV6Pckg *tOpenChnV6Pckg = new(ELeave)RSat::TOpenLocalLinksChannelV6Pckg(*tOpenChnV6);
	CleanupStack::PushL(tOpenChnV6Pckg);
	
	iSat.NotifyOpenChannelPCmd(reqStatus,*(tOpenChnV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tOpenChnV6->iBearer.iType == RSat::EUtranBearer);
	
	RSat::TOpenChannelRspV2 *tOpenChnRsp = new (ELeave) RSat::TOpenChannelRspV2;
	CleanupStack::PushL(tOpenChnRsp);
	
	RSat::TOpenChannelRspV2Pckg *tOpenChnRspPckg = new (ELeave) RSat::TOpenChannelRspV2Pckg(*tOpenChnRsp);
	CleanupStack::PushL(tOpenChnRspPckg);
	
	tOpenChnRsp->iGeneralResult = RSat::KPerformedWithModifications;
	tOpenChnRsp->iInfoType = RSat::KNoAdditionalInfo;
	tOpenChnRsp->SetPCmdNumber(SIMTSY_OPEN_CHAN_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::EOpenChannelLocal,*(tOpenChnRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);


	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8558 - Successful"));
	
	return KErrNone;
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8559
 *
 *	SYMREQ REQ7345
 *
 *	SYMTestCaseDesc Cancellation of the R6 support extended OPEN CHANNEL proactive command.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the OPEN CHANNEL using the RSat::NotifyOpenChannelPCmd ()
 *		(ii)	Issue the cancel request using the API CancelAsyncRequest(ESatNotifyOpenChannelPCmd)
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */

TInt CSimSatOpenChnl::testSAT8559L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8559"));

	TRequestStatus reqStatus;
	
	RSat::TOpenLocalLinksChannelV6 *tOpenChnV6 = new(ELeave) RSat::TOpenLocalLinksChannelV6();
	CleanupStack::PushL(tOpenChnV6);
	
	RSat::TOpenLocalLinksChannelV6Pckg *tOpenChnV6Pckg = new(ELeave)RSat::TOpenLocalLinksChannelV6Pckg(*tOpenChnV6);
	CleanupStack::PushL(tOpenChnV6Pckg);
	
	iSat.NotifyOpenChannelPCmd(reqStatus,*(tOpenChnV6Pckg));
	iSat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);
	User::WaitForRequest(reqStatus);

	TESTL((reqStatus.Int() == KErrNone) || (reqStatus.Int() == KErrCancel));
	
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8559 - Successful"));
	
	return KErrNone;
	
	}


CSimSatPlayTone::CSimSatPlayTone() 
	{ 
	SetTestStepName(_L("SatTestPlayTone"));
	}

TVerdict CSimSatPlayTone::doTestStepL()
	{
	INFO_PRINTF1(_L("PLAY TONE TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8560L();	// For testing the TEL-SIMTSY-SAT-8560
	testSAT8561L();	// For testing the TEL-SIMTSY-SAT-8561
	testSAT8562L();	// For testing the TEL-SIMTSY-SAT-8562
	testSAT8563L();	// For testing the TEL-SIMTSY-SAT-8563
	
	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("PLAY TONE TESTING - END"));

	return TestStepResult();
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8560
 *
 *	SYMREQ REQ7342
 *
 *	SYMTestCaseDesc R6 support for “Play Tone” 
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *	(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference.
 *	(ii)	Call the API RSat::ClientSatProfileIndication using the reference of RSat::TSatProfileV6Pckg.
 *	(iii)	The client notifies the ME for the PLAY TONE using the RSat::NotifyPlayTonePCmd().
 *	(iv)	The ME responds back with the RSat::TerminalRsp with the TPlayToneRspV1 as the response parameter.
 *
 *	SYMTestExpectedResults 
 *		Byte 22 of the Terminal profile will contain the extended play tone and shall contain value
 *		KCapsPlayThemedAndMelodyTone.
 *		The ME sends TERMINAL RESPONSE to the UICC.
 *		Terminal response indicates the tone as been successfully played and will contain value as
 *		KCommandPerformedSuccessfullyTone in the response package 
 *		
 * 	SYMTestType CIT
 */

TInt CSimSatPlayTone::testSAT8560L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8560"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL((profileV6->iSatProfileByte22) & (RSat::KCapsPlayThemedAndMelodyTone));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TPlayToneV6 *tPlayToneV6 = new(ELeave) RSat::TPlayToneV6();
	CleanupStack::PushL(tPlayToneV6);
	
	RSat::TPlayToneV6Pckg *tPlayToneV6Pckg = new(ELeave)RSat::TPlayToneV6Pckg(*tPlayToneV6);
	CleanupStack::PushL(tPlayToneV6Pckg);
	
	iSat.NotifyPlayTonePCmd(reqStatus,*(tPlayToneV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TPlayToneRspV1 *tPlayToneRsp = new (ELeave) RSat::TPlayToneRspV1;
	CleanupStack::PushL(tPlayToneRsp);
	
	RSat::TPlayToneRspV1Pckg *tPlayToneRspPckgV6 = new (ELeave) RSat::TPlayToneRspV1Pckg(*tPlayToneRsp);
	CleanupStack::PushL(tPlayToneRspPckgV6);
	
	tPlayToneRsp->iGeneralResult = RSat::KPlayTonePerformedSuccessfully ;
	tPlayToneRsp->iInfoType = RSat::KNoAdditionalInfo;
	tPlayToneRsp->SetPCmdNumber(SIMTSY_PLAY_TONE_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::EPlayTone,*(tPlayToneRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(6);
	
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8560 - Successful"));
	
	return KErrNone;
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8561
 *
 *	SYMREQ REQ7342
 *
 *	SYMTestCaseDesc R6 support for “Play Tone” when the user needs to terminate the session.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the PLAY TONE using the RSat::NotifyPlayTonePCmd()
 *		(ii)	The ME responds back with the RSat::TerminalRsp with the TPlayToneRspV1 as the
 *			response parameter
 *
 *	SYMTestExpectedResults 
 *		The ME sends TERMINAL RESPONSE to the UICC.
 *		Terminal response indicates that the session has been terminated by the user and will contain 
 *		value as KSessionTerminatedByUser in the response package.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatPlayTone::testSAT8561L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8561"));

	TRequestStatus reqStatus;
	
	RSat::TPlayToneV6 *tPlayToneV6 = new(ELeave) RSat::TPlayToneV6();
	CleanupStack::PushL(tPlayToneV6);
	
	RSat::TPlayToneV6Pckg *tPlayToneV6Pckg = new(ELeave)RSat::TPlayToneV6Pckg(*tPlayToneV6);
	CleanupStack::PushL(tPlayToneV6Pckg);
	
	iSat.NotifyPlayTonePCmd(reqStatus,*(tPlayToneV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TPlayToneRspV1 *tPlayToneRsp = new (ELeave) RSat::TPlayToneRspV1;
	CleanupStack::PushL(tPlayToneRsp);
	
	RSat::TPlayToneRspV1Pckg *tPlayToneRspPckgV6 = new (ELeave) RSat::TPlayToneRspV1Pckg(*tPlayToneRsp);
	CleanupStack::PushL(tPlayToneRspPckgV6);
	
	tPlayToneRsp->iGeneralResult = RSat::KPSessionTerminatedByUser;
	tPlayToneRsp->iInfoType = RSat::KNoAdditionalInfo;
	tPlayToneRsp->SetPCmdNumber(SIMTSY_PLAY_TONE_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::EPlayTone,*(tPlayToneRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8561 - Successful"));
	
	return KErrNone;
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8562
 *
 *	SYMREQ REQ7342
 *
 *	SYMTestCaseDesc R6 support for “Play Tone” when the terminal not supporting the requested tone to play.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the PLAY TONE using the RSat::NotifyPlayTonePCmd()
 *		(ii)	The ME responds back with the RSat::TerminalRsp with the TPlayToneRspV1 as the 
 *			response parameter.
 *			
 *	SYMTestExpectedResults 
 *		The ME sends TERMINAL RESPONSE to the UICC.
 *		Terminal response will contain KCmdBeyondMeCapabilities in the response package.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatPlayTone::testSAT8562L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8562"));

	TRequestStatus reqStatus;
	
	RSat::TPlayToneV6 *tPlayToneV6 = new(ELeave) RSat::TPlayToneV6();
	CleanupStack::PushL(tPlayToneV6);
	
	RSat::TPlayToneV6Pckg *tPlayToneV6Pckg = new(ELeave)RSat::TPlayToneV6Pckg(*tPlayToneV6);
	CleanupStack::PushL(tPlayToneV6Pckg);
	
	iSat.NotifyPlayTonePCmd(reqStatus,*(tPlayToneV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TPlayToneRspV1 *tPlayToneRsp = new (ELeave) RSat::TPlayToneRspV1;
	CleanupStack::PushL(tPlayToneRsp);
	
	RSat::TPlayToneRspV1Pckg *tPlayToneRspPckgV6 = new (ELeave) RSat::TPlayToneRspV1Pckg(*tPlayToneRsp);
	CleanupStack::PushL(tPlayToneRspPckgV6);
	
	tPlayToneRsp->iGeneralResult = RSat::KCmdBeyondMeCapabilities;
	tPlayToneRsp->iInfoType = RSat::KNoAdditionalInfo;
	tPlayToneRsp->SetPCmdNumber(SIMTSY_PLAY_TONE_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::EPlayTone,*(tPlayToneRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8562 - Successful"));
	
	return KErrNone;
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8563
 *
 *	SYMREQ REQ7342
 *
 *	SYMTestCaseDesc Cancellation R6 support for “Play Tone”
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the PLAY TONE using the RSat::NotifyPlayTonePCmd()
 *		(ii)	Cancel the previously placed Notification request with the API 
 *			RSat::CancelAsyncRequest(ESatNotifyPlayTonePCmd).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatPlayTone::testSAT8563L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8563"));

	TRequestStatus reqStatus;
	
	RSat::TPlayToneV6 *tPlayToneV6 = new(ELeave) RSat::TPlayToneV6();
	CleanupStack::PushL(tPlayToneV6);
	
	RSat::TPlayToneV6Pckg *tPlayToneV6Pckg = new(ELeave)RSat::TPlayToneV6Pckg(*tPlayToneV6);
	CleanupStack::PushL(tPlayToneV6Pckg);
	
	iSat.NotifyPlayTonePCmd(reqStatus,*(tPlayToneV6Pckg));
	iSat.CancelAsyncRequest(ESatNotifyPlayTonePCmd);
	User::WaitForRequest(reqStatus);
	
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));
	
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8563 - Successful"));
	
	return KErrNone;
	}

CSimSatSetupCall::CSimSatSetupCall() 
	{ 
	SetTestStepName(_L("SatTestSetupCall"));
	}

TVerdict CSimSatSetupCall::doTestStepL()
	{
	INFO_PRINTF1(_L("SETUP CALL TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8564L();	// For testing the TEL-SIMTSY-SAT-8564
	testSAT8565L();	// For testing the TEL-SIMTSY-SAT-8565
	testSAT8566L();	// For testing the TEL-SIMTSY-SAT-8566
	testSAT8567L();	// For testing the TEL-SIMTSY-SAT-8567
	testSAT8568L();	// For testing the TEL-SIMTSY-SAT-8568
	
	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("SETUP CALL TESTING - END"));

	return TestStepResult();
	
	}


/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8564
 *
 *	SYMREQ REQ7499
 *
 *	SYMTestCaseDesc R6 support for “Set up Call”
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the reference of RSat::TSatProfileV6Pckg.
 *		(iii)	The client notifies the ME for the SET UP CALL using the RSat::NotifySetUpCallPCmd()
 *		(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Byte 22 of the Terminal profile shall contain support for multimedia call with value
 *		 KCapsSetUpCallMultimediaCall.
 *		Terminal response holds value KSuccess, in the Response package indicating the call has
 *		been successfully established.
 *
 * 	SYMTestType CIT
 */

TInt CSimSatSetupCall::testSAT8564L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8564"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL((profileV6->iSatProfileByte22) & (RSat::KCapsSetUpCallMultimediaCall));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TSetUpCallV6 *tSetupCallV6 = new(ELeave) RSat::TSetUpCallV6();
	CleanupStack::PushL(tSetupCallV6);
	
	RSat::TSetUpCallV6Pckg *tSetupCallV6Pckg = new(ELeave)RSat::TSetUpCallV6Pckg(*tSetupCallV6);
	CleanupStack::PushL(tSetupCallV6Pckg);
	
	iSat.NotifySetUpCallPCmd(reqStatus,*(tSetupCallV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TSetUpCallRspV2 *tSetupCallRsp = new (ELeave) RSat::TSetUpCallRspV2;
	CleanupStack::PushL(tSetupCallRsp);
	
	RSat::TSetUpCallRspV2Pckg *tSetupCallRspPckgV6 = new (ELeave) RSat::TSetUpCallRspV2Pckg(*tSetupCallRsp);
	CleanupStack::PushL(tSetupCallRspPckgV6);
	
	tSetupCallRsp->iGeneralResult = RSat::KSuccess;
	tSetupCallRsp->iInfoType = RSat::KNoAdditionalInfo;
	tSetupCallRsp->SetPCmdNumber(SIMTSY_SETUP_CALL_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::ESetUpCall,*(tSetupCallRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(6);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8564 - Successful"));
	
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8565
 *
 *	SYMREQ REQ7499
 *
 *	SYMTestCaseDesc R6 support for “Set up Call” with the ME not having the capabilities of
 *			 setting up a multimedia call
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the SET UP CALL using the RSat::NotifySetUpCallPCmd()
 *		(ii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Terminal response holds value KCmdBeyondMeCapabilities in the Response package indicating 
 *		the terminal is not capable of executing the Proactive command.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatSetupCall::testSAT8565L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8565"));

	TRequestStatus reqStatus;
	
	RSat::TSetUpCallV6 *tSetupCallV6 = new(ELeave) RSat::TSetUpCallV6();
	CleanupStack::PushL(tSetupCallV6);
	
	RSat::TSetUpCallV6Pckg *tSetupCallV6Pckg = new(ELeave)RSat::TSetUpCallV6Pckg(*tSetupCallV6);
	CleanupStack::PushL(tSetupCallV6Pckg);
	
	iSat.NotifySetUpCallPCmd(reqStatus,*(tSetupCallV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TSetUpCallRspV2 *tSetupCallRsp = new (ELeave) RSat::TSetUpCallRspV2;
	CleanupStack::PushL(tSetupCallRsp);
	
	RSat::TSetUpCallRspV2Pckg *tSetupCallRspPckgV6 = new (ELeave) RSat::TSetUpCallRspV2Pckg(*tSetupCallRsp);
	CleanupStack::PushL(tSetupCallRspPckgV6);
	
	tSetupCallRsp->iGeneralResult = RSat::KCmdBeyondMeCapabilities;
	tSetupCallRsp->iInfoType = RSat::KNoAdditionalInfo;
	tSetupCallRsp->SetPCmdNumber(SIMTSY_SETUP_CALL_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::ESetUpCall,*(tSetupCallRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8565 - Successful"));
	
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8566
 *
 *	SYMREQ REQ7499
 *
 *	SYMTestCaseDesc R6 support for “Set up Call” with the ME not able to process the command as
 *			 it is busy on another call
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the SET UP CALL using the RSat::NotifySetUpCallPCmd()
 *		(ii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *		
 *	SYMTestExpectedResults 
 *		Terminal response holds value KMeUnableToProcessCmd in the Response package indicating the
 *		 terminal is not capable of executing the Proactive command.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatSetupCall::testSAT8566L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8566"));

	TRequestStatus reqStatus;
	
	RSat::TSetUpCallV6 *tSetupCallV6 = new(ELeave) RSat::TSetUpCallV6();
	CleanupStack::PushL(tSetupCallV6);
	
	RSat::TSetUpCallV6Pckg *tSetupCallV6Pckg = new(ELeave)RSat::TSetUpCallV6Pckg(*tSetupCallV6);
	CleanupStack::PushL(tSetupCallV6Pckg);
	
	iSat.NotifySetUpCallPCmd(reqStatus,*(tSetupCallV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TSetUpCallRspV2 *tSetupCallRsp = new (ELeave) RSat::TSetUpCallRspV2;
	CleanupStack::PushL(tSetupCallRsp);
	
	RSat::TSetUpCallRspV2Pckg *tSetupCallRspPckgV6 = new (ELeave) RSat::TSetUpCallRspV2Pckg(*tSetupCallRsp);
	CleanupStack::PushL(tSetupCallRspPckgV6);
	
	_LIT(KText,"KMeBusyOnCall");
	TBuf<254> buf(KText);
	tSetupCallRsp->iGeneralResult = RSat::KMeUnableToProcessCmd;
	tSetupCallRsp->iInfoType = RSat::KMeProblem;
	tSetupCallRsp->iAdditionalInfo = buf;
	tSetupCallRsp->SetPCmdNumber(SIMTSY_SETUP_CALL_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::ESetUpCall,*(tSetupCallRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8566 - Successful"));
	
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8567
 *
 *	SYMREQ REQ7499
 *
 *	SYMTestCaseDesc R6 support for “Set up Call” when the terminal is not able to establish a call.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the SET UP CALL using the RSat::NotifySetUpCallPCmd()
 *			 with the iDuration (time to redial) set to a valid duration for the redial.
 *		(ii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Terminal response holds value KNetworkUnableToProcessCmd in the Response package indicating 
 *		the terminal is not able to establish the call.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatSetupCall::testSAT8567L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8567"));

	TRequestStatus reqStatus;
	
	RSat::TSetUpCallV6 *tSetupCallV6 = new(ELeave) RSat::TSetUpCallV6();
	CleanupStack::PushL(tSetupCallV6);
	
	RSat::TSetUpCallV6Pckg *tSetupCallV6Pckg = new(ELeave)RSat::TSetUpCallV6Pckg(*tSetupCallV6);
	CleanupStack::PushL(tSetupCallV6Pckg);
	
	iSat.NotifySetUpCallPCmd(reqStatus,*(tSetupCallV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TSetUpCallRspV2 *tSetupCallRsp = new (ELeave) RSat::TSetUpCallRspV2;
	CleanupStack::PushL(tSetupCallRsp);
	
	RSat::TSetUpCallRspV2Pckg *tSetupCallRspPckgV6 = new (ELeave) RSat::TSetUpCallRspV2Pckg(*tSetupCallRsp);
	CleanupStack::PushL(tSetupCallRspPckgV6);
	
	_LIT(KText,"KMeBusyOnCall");
	TBuf<254> buf(KText);
	tSetupCallRsp->iGeneralResult = RSat::KNetworkUnableToProcessCmd;
	tSetupCallRsp->iInfoType = RSat::KMeProblem;
	tSetupCallRsp->iAdditionalInfo = buf;
	tSetupCallRsp->SetPCmdNumber(SIMTSY_SETUP_CALL_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::ESetUpCall,*(tSetupCallRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8567 - Successful"));
	
	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8568
 *
 *	SYMREQ REQ7499
 *
 *	SYMTestCaseDesc Cancellation of the proactive command “Set up Call” with R6 support.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the SET UP CALL using the RSat::NotifySetUpCallPCmd()
 *		(ii)	Cancel the previously placed Notification request with the API
 *			 RSat::CancelAsyncRequest(ESatNotifyPlayTonePCmd).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */
 
TInt CSimSatSetupCall::testSAT8568L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8568"));

	TRequestStatus reqStatus;
	
	RSat::TSetUpCallV6 *tSetupCallV6 = new(ELeave) RSat::TSetUpCallV6();
	CleanupStack::PushL(tSetupCallV6);
	
	RSat::TSetUpCallV6Pckg *tSetupCallV6Pckg = new(ELeave)RSat::TSetUpCallV6Pckg(*tSetupCallV6);
	CleanupStack::PushL(tSetupCallV6Pckg);
	
	iSat.NotifySetUpCallPCmd(reqStatus,*(tSetupCallV6Pckg));
	iSat.CancelAsyncRequest(ESatNotifySetUpCallPCmd);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));
	
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8568 - Successful"));
	
	return KErrNone;
	
	}

CSimSatRefresh::CSimSatRefresh() 
	{ 
	SetTestStepName(_L("SatTestRefresh"));
	}

TVerdict CSimSatRefresh::doTestStepL()
	{
	INFO_PRINTF1(_L("REFRESH COMMAND TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8569L();	// For testing the TEL-SIMTSY-SAT-8569
	testSAT8570L();	// For testing the TEL-SIMTSY-SAT-8570
	testSAT8571L();	// For testing the TEL-SIMTSY-SAT-8571

	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("REFRESH COMMAND TESTING - END"));

	return TestStepResult();
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8569
 *
 *	SYMREQ REQ7339
 *
 *	SYMTestCaseDesc R6 support for “Refresh” command in the USIM Initialisation and File Change Notification Mode.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the reference of RSat::TSatProfileV6Pckg.
 *		(iii)	The client notifies the ME for the REFRESH using the RSat::NotifyRefreshPCmd () with the 
 *			TRefreshType = ESimInitFileChangeNotification in the TRefreshV6 class parameter.
 *		(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Byte 22 of the Terminal profile shall contain support for multimedia call with value KCapsRefreshGBA
 *		Terminal response contains value as KSuccess, in the Response package.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatRefresh::testSAT8569L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8569"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL((profileV6->iSatProfileByte22) & (RSat::KCapsRefreshGBA));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TRefreshV2 *tRefreshV6 = new(ELeave) RSat::TRefreshV2();
	CleanupStack::PushL(tRefreshV6);
	tRefreshV6->iType = RSat::E3GSessionReset;
	
	RSat::TRefreshV2Pckg *tRefreshV6Pckg = new(ELeave)RSat::TRefreshV2Pckg(*tRefreshV6);
	CleanupStack::PushL(tRefreshV6Pckg);
		
	iSat.NotifyRefreshPCmd(reqStatus,*(tRefreshV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL((tRefreshV6->iType == RSat::ESimInitFileChangeNotification) || 
	    	(tRefreshV6->iType == RSat::EFileChangeNotification) ||
			(tRefreshV6->iType == RSat::E3GSessionReset));
	
	RSat::TRefreshRspV1 *tRefreshRsp = new (ELeave) RSat::TRefreshRspV1;
	CleanupStack::PushL(tRefreshRsp);
	
	RSat::TRefreshRspV1Pckg *tRefreshRspPckgV6 = new (ELeave) RSat::TRefreshRspV1Pckg(*tRefreshRsp);
	CleanupStack::PushL(tRefreshRspPckgV6);
	
	tRefreshRsp->iGeneralResult = RSat::KSuccess;
	tRefreshRsp->iInfoType = RSat::KNoAdditionalInfo;
	tRefreshRsp->SetPCmdNumber(SIMTSY_REFRESH_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::ERefresh,*(tRefreshRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(6);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8569 - Successful"));
	
	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8570
 *
 *	SYMREQ REQ7339
 *
 *	SYMTestCaseDesc R6 support for “Refresh” command in the USIM Application Reset mode.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the REFRESH using the RSat::NotifyRefreshPCmd () 
 *			with the TRefreshType = EUsimApplicationReset in the TRefreshV6 class parameter.
 *		(ii)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		Terminal response contains value as KMeUnableToProcessCmd, in the Response package.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatRefresh::testSAT8570L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8570"));

	TRequestStatus reqStatus;
	
	RSat::TRefreshV2 *tRefreshV6 = new(ELeave) RSat::TRefreshV2();
	CleanupStack::PushL(tRefreshV6);
	tRefreshV6->iType = RSat::EFileChangeNotification;
	
	RSat::TRefreshV2Pckg *tRefreshV6Pckg = new(ELeave)RSat::TRefreshV2Pckg(*tRefreshV6);
	CleanupStack::PushL(tRefreshV6Pckg); 
	
	iSat.NotifyRefreshPCmd(reqStatus,*(tRefreshV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tRefreshV6->iType == RSat::EUsimApplicationReset);
	
	RSat::TRefreshRspV1 *tRefreshRsp = new (ELeave) RSat::TRefreshRspV1;
	CleanupStack::PushL(tRefreshRsp);
	
	RSat::TRefreshRspV1Pckg *tRefreshRspPckgV6 = new (ELeave) RSat::TRefreshRspV1Pckg(*tRefreshRsp);
	CleanupStack::PushL(tRefreshRspPckgV6);
	
	tRefreshRsp->iGeneralResult = RSat::KMeUnableToProcessCmd;
	tRefreshRsp->iInfoType = RSat::KNoAdditionalInfo;
	tRefreshRsp->SetPCmdNumber(SIMTSY_REFRESH_PCMD_NUMBER);

	iSat.TerminalRsp(reqStatus,RSat::ERefresh,*(tRefreshRspPckgV6));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8570 - Successful"));
	
	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8571
 *
 *	SYMREQ  REQ7339
 *
 *	SYMTestCaseDesc Cancellation of the Proactive command “Refresh” command with R6 support.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the REFRESH using the RSat::NotifyRefreshPCmd() 
 *			with the TRefreshType = EUsimApplicationReset in the TRefreshV6 class parameter.
 *		(ii)	Cancel the previously placed Notification request with the API 
 *			RSat::CancelAsyncRequest(ESatNotifyRefreshPCmd).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */
 
TInt CSimSatRefresh::testSAT8571L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8571"));

	TRequestStatus reqStatus;
	
	RSat::TRefreshV2 *tRefreshV6 = new(ELeave) RSat::TRefreshV2();
	CleanupStack::PushL(tRefreshV6);
	tRefreshV6->iType = RSat::E3GSessionReset;
	
	RSat::TRefreshV2Pckg *tRefreshV6Pckg = new(ELeave)RSat::TRefreshV2Pckg(*tRefreshV6);
	CleanupStack::PushL(tRefreshV6Pckg);
	
	iSat.NotifyRefreshPCmd(reqStatus,*(tRefreshV6Pckg));
	iSat.CancelAsyncRequest(ESatNotifyRefreshPCmd);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));
	
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8571 - Successful"));
	
	return KErrNone;
	}


CSimSatEventDnld::CSimSatEventDnld() 
	{ 
	SetTestStepName(_L("SatTestRefresh"));
	}

TVerdict CSimSatEventDnld::doTestStepL()
	{
	INFO_PRINTF1(_L("EVENT DOWNLOAD TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8572L();	// For testing the TEL-SIMTSY-SAT-8572
	testSAT8573L();	// For testing the TEL-SIMTSY-SAT-8573
	testSAT8574L();	// For testing the TEL-SIMTSY-SAT-8574
	testSAT8575L();	// For testing the TEL-SIMTSY-SAT-8575
	testSAT8576L();	// For testing the TEL-SIMTSY-SAT-8576
	testSAT8577L();	// For testing the TEL-SIMTSY-SAT-8577

	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("EVENT DOWNLOAD TESTING - END"));

	return TestStepResult();
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8572
 *
 *	SYMREQ REQ7343
 *
 *	SYMTestCaseDesc EVENT DOWNLOAD Network Search Mode Change
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the reference of RSat::TSatProfileV6Pckg.
 *		(iii)	The client notifies the ME for the EVENT DOWNLOAD using the RSat::EventDownload () with the
 *			eventlist as KNetworkSearchModeChange in the TNetworkSearchModeChangeV6 class parameter.
 *	
 *	SYMTestExpectedResults 
 *		Byte 6 of the Terminal profile shall contain support for multimedia call with value 
 *		KCapsNetworkSearchModeChangeEvent.
 *		The EventDownload request completes successfully with KErrNone.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatEventDnld::testSAT8572L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8572"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL((profileV6->iSatProfileByte6) & (RSat::KCapsNetworkSearchModeChangeEvent));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TNetworkSearchModeChangeEventV6 *tEventDnldV6 = new(ELeave) RSat::TNetworkSearchModeChangeEventV6();
	CleanupStack::PushL(tEventDnldV6);
	
	RSat::TNetworkSearchModeChangeEventV6Pckg *tEventDnldV6Pckg = new(ELeave)
		RSat::TNetworkSearchModeChangeEventV6Pckg(*tEventDnldV6);
	CleanupStack::PushL(tEventDnldV6Pckg);
	
	tEventDnldV6->iNetworkSearchMode = RSat::EAutomatic;
	
	iSat.EventDownload(reqStatus,RSat::KNetworkSearchModeChange ,*(tEventDnldV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
 
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8572 - Successful"));
	
	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8573
 *
 *	SYMREQ REQ7343
 *
 *	SYMTestCaseDesc Cancellation of EVENT DOWNLOAD Network Search Mode Change
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the EVENT DOWNLOAD using the RSat:: EventDownload () with the 
 *			TEventList = KNetworkSearchModeChange in the TNetworkSearchModeChangeV6 class parameter.
 *		(ii)	Cancel the placed request using RSat::CancelAsyncRequest(ESatEventDownload).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */
 
TInt CSimSatEventDnld::testSAT8573L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8573"));

	TRequestStatus reqStatus;
	
	RSat::TNetworkSearchModeChangeEventV6 *tEventDnldV6 = new(ELeave) RSat::TNetworkSearchModeChangeEventV6();
	
	CleanupStack::PushL(tEventDnldV6);
	
	RSat::TNetworkSearchModeChangeEventV6Pckg *tEventDnldV6Pckg = new(ELeave)
		RSat::TNetworkSearchModeChangeEventV6Pckg(*tEventDnldV6);
	CleanupStack::PushL(tEventDnldV6Pckg);
	
	tEventDnldV6->iNetworkSearchMode = RSat::EAutomatic;
	
	iSat.EventDownload(reqStatus,RSat::KNetworkSearchModeChange ,*(tEventDnldV6Pckg));
	iSat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int() == KErrNone) || (reqStatus.Int() == KErrCancel));

	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8573 - Successful"));
	
	return KErrNone;

	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8574
 *
 *	SYMREQ REQ7343
 *
 *	SYMTestCaseDesc EVENT DOWNLOAD Browsing Status Change
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the reference of RSat::TSatProfileV6Pckg.
 *		(iii)	The client notifies the ME for the EVENT DOWNLOAD using the RSat::EventDownload () with the 
 *			eventlist as KBrowsingStatusChange in the TBrowsingStatusChangeV6 class parameter.
 *
 *	SYMTestExpectedResults 
 *		The EventDownload request completes successfully with KErrNone.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatEventDnld::testSAT8574L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8574"));
	
	TRequestStatus reqStatus = KErrNone;

	RSat::TBrowsingStatusEventV6 *tBrwStsV6 = new(ELeave) RSat::TBrowsingStatusEventV6();
	CleanupStack::PushL(tBrwStsV6);
	
	RSat::TBrowsingStatusEventV6Pckg *tBrwStsV6Pckg = new(ELeave)RSat::TBrowsingStatusEventV6Pckg(*tBrwStsV6);
	CleanupStack::PushL(tBrwStsV6Pckg);
	
	tBrwStsV6->iBrowserStatus = 0x02;
	
	iSat.EventDownload(reqStatus,RSat::KBrowsingStatusChange ,*(tBrwStsV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);

	CleanupStack::PopAndDestroy(2);
		
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8574 - Successful"));
	
	return KErrNone;

	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8575
 *
 *	SYMREQ REQ7343
 *
 *	SYMTestCaseDesc Cancellation of the EVENT DOWNLOAD command of Browsing Status Change
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the EVENT DOWNLOAD using the RSat::EventDownload () 
 *			with the TEventList as KBrowsingStatusChange in the TBrowsingStatusChangeV6 class parameter.
 *		(ii)	Cancel the previously placed request using RSat::CancelAsyncRequest(ESatEventDownload).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatEventDnld::testSAT8575L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8575"));

	TRequestStatus reqStatus;
	
	RSat::TBrowsingStatusEventV6 *tBrwStsV6 = new(ELeave) RSat::TBrowsingStatusEventV6();
	CleanupStack::PushL(tBrwStsV6);
	
	RSat::TBrowsingStatusEventV6Pckg *tBrwStsV6Pckg = new(ELeave)RSat::TBrowsingStatusEventV6Pckg(*tBrwStsV6);
	CleanupStack::PushL(tBrwStsV6Pckg);
	
	tBrwStsV6->iBrowserStatus = 0x05;
	
	iSat.EventDownload(reqStatus,RSat::KBrowsingStatusChange ,*(tBrwStsV6Pckg));
	iSat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int() == KErrNone) || (reqStatus.Int() == KErrCancel));

	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8575 - Successful"));
	
	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8576
 *
 *	SYMREQ REQ7343
 *
 *	SYMTestCaseDesc EVENT DOWNLOAD Frames Information Change
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the reference of RSat::TSatProfileV6Pckg.
 *		(iii)	The client notifies the ME for the EVENT DOWNLOAD using the RSat::EventDownload () with the
 *			eventlist as KFramesInformationChange in the TFramesInformationChangeV6 class parameter.
 *
 *	SYMTestExpectedResults 
 *		The EventDownload request completes successfully with KErrNone.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatEventDnld::testSAT8576L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8576"));

	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL((profileV6->iSatProfileByte25) & (RSat::KCapsFrameParametersChangeEvent));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	RSat::TFramesInformationChangedEventV6 *tFrmsChgV6 = new(ELeave) RSat::TFramesInformationChangedEventV6();
	CleanupStack::PushL(tFrmsChgV6);
	
	RSat::TFramesInformationChangedEventV6Pckg *tFrmsChgV6Pckg = new(ELeave)
		RSat::TFramesInformationChangedEventV6Pckg(*tFrmsChgV6);
	CleanupStack::PushL(tFrmsChgV6Pckg);
	
	_LIT8(KFrameIdBuf,"ABC");
	TBuf8<255> buf(KFrameIdBuf);
	tFrmsChgV6->iFramesInformation.iFrameId = 0x25;
	tFrmsChgV6->iFramesInformation.iFrameList = buf;
	
	iSat.EventDownload(reqStatus,RSat::KFramesInformationChange ,*(tFrmsChgV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);

	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8576 - Successful"));
	
	return KErrNone;

	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8577
 *
 *	SYMREQ REQ7343
 *
 *	SYMTestCaseDesc Cancellation of EVENT DOWNLOAD with Frames Information Change.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	The client notifies the ME for the REFRESH using the RSat::EventDownload () with the 
 *			eventlist as KFramesInformationChange in the TFramesInformationChangeV6 class parameter.
 *		(ii)	Cancel the previously placed request using RSat::CancelAsyncRequest(ESatEventDownload). 
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */
 
TInt CSimSatEventDnld::testSAT8577L()
	{	
		
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8577"));

	TRequestStatus reqStatus;
	
	RSat::TFramesInformationChangedEventV6 *tFrmsChgV6 = new(ELeave) RSat::TFramesInformationChangedEventV6();
	CleanupStack::PushL(tFrmsChgV6);
	
	RSat::TFramesInformationChangedEventV6Pckg *tFrmsChgV6Pckg = new(ELeave)
		RSat::TFramesInformationChangedEventV6Pckg(*tFrmsChgV6);
	CleanupStack::PushL(tFrmsChgV6Pckg);
	
	_LIT8(KFrameIdBuf,"ABC");
	TBuf8<255> buf(KFrameIdBuf);
	tFrmsChgV6->iFramesInformation.iFrameId = 0x25;
	tFrmsChgV6->iFramesInformation.iFrameList = buf;
	
	iSat.EventDownload(reqStatus,RSat::KFramesInformationChange ,*(tFrmsChgV6Pckg));
	iSat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int() == KErrNone) || (reqStatus.Int() == KErrCancel));

	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8577 - Successful"));
	
	return KErrNone;

	}


CSimSatSendUssd::CSimSatSendUssd() 
	{ 
	SetTestStepName(_L("SatTestSendUssd"));
	}	

TVerdict CSimSatSendUssd::doTestStepL()
	{
	INFO_PRINTF1(_L("SEND USSD TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8578L();	// For testing the TEL-SIMTSY-SAT-8578
	testSAT8579L();	// For testing the TEL-SIMTSY-SAT-8579
	testSAT8580L();	// For testing the TEL-SIMTSY-SAT-8580
	testSAT8581L();	// For testing the TEL-SIMTSY-SAT-8581
	testSAT8582L();	// For testing the TEL-SIMTSY-SAT-8582

	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("SEND USSD TESTING - END"));

	return TestStepResult();
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8578
 *
 *	SYMREQ REQ7340
 *
 *	SYMTestCaseDesc SEND USSD to support the application mode.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::GetMeSideSatProfile using the parameter RSat::TSatProfileV6Pckg as reference
 *		(ii)	Call the API RSat::ClientSatProfileIndication using the reference of RSat::TSatProfileV6Pckg.
 *
 *	SYMTestExpectedResults 
 *		The TSatProfileV6 shall have the TSatProfileByte4 set to KCapsSendUSSD .
 *		The ClientSatProfileIndication shall return KErrNone.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatSendUssd::testSAT8578L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8578"));
	
	TRequestStatus reqStatus;
	
	RSat::TSatProfileV6 *profileV6 = new (ELeave)RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg	*profileV6Pckg = new (ELeave)RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);

	iSat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(profileV6->iSatProfileByte4 & (RSat::KCapsSendUSSD));
		
	TRequestStatus  trr = KErrNone;
	trr= iSat.ClientSatProfileIndication(*profileV6Pckg);
	TESTL(trr == KErrNone);
		
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8578- Successful"));

	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8579
 *
 *	SYMREQ REQ7340
 *
 *	SYMTestCaseDesc SEND USSD to support the application mode.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::NotifySendUssdPCmd using the TSendUssdV6 as the parameter.
 *
 *	SYMTestExpectedResults 
 *		The NotiftySendUssdPCmd request completes successfully with KErrNone.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatSendUssd::testSAT8579L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8579"));
	
	TRequestStatus reqStatus;
	
	RSat::TSendUssdV6* tSendUssd = new(ELeave) RSat::TSendUssdV6();
	CleanupStack::PushL(tSendUssd);
	
	RSat::TSendUssdV6Pckg* tSendUssdPckg = new(ELeave) RSat::TSendUssdV6Pckg(*tSendUssd);
	CleanupStack::PushL(tSendUssdPckg);
	
	iSat.NotifySendUssdPCmd(reqStatus,*(tSendUssdPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tSendUssd->iTextAttribute.iStatus == RSat::ETextAttributeProvided);
	TESTL(tSendUssd->iTextAttribute.iTextAttributeData.Compare(SIMTSY_TXT_ATTR_DATA) ==0);	
	
	RSat::TSendUssdRspV1 *tSendUssdRsp = new (ELeave) RSat::TSendUssdRspV1;
	CleanupStack::PushL(tSendUssdRsp);
	
	RSat::TSendUssdRspV1Pckg *tSendUssdRspPckg = new (ELeave) RSat::TSendUssdRspV1Pckg(*tSendUssdRsp);
	CleanupStack::PushL(tSendUssdRspPckg);
	
	tSendUssdRsp->iGeneralResult = RSat::KSuccess;
	tSendUssdRsp->iInfoType = RSat::KNoAdditionalInfo;
	tSendUssdRsp->SetPCmdNumber(SIMTSY_SEND_USSD_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESendUssd,*(tSendUssdRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
		
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8579- Successful"));

	return KErrNone;
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8580 
 *
 *	SYMREQ REQ7340
 *
 *	SYMTestCaseDesc SEND USSD command rejected by the ME as ME is busy on a USSD transaction
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for RETRIEVE MULTIMEDIA MESSAGE using NotifySendUssdPCmd () API.
 *		(ii)	UICC issues SEND USSD command to the ME.
 *		(iii)	ME is unable to execute the command as its busy processing some other USSD transaction.
 *		(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		ME is notified of the request.
 *		Upon completion of the proactive command, details are returned in a packaged version of 
 *		 TSendUssdV6in the command parameter. 
 *		ME responds with KMeUnableToProcessCmd in the Terminal response with the additional information 
 *		 KMeBusyOnUssd. 
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatSendUssd::testSAT8580L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8580"));
		
	TRequestStatus reqStatus;
	
	RSat::TSendUssdV6* tSendUssd = new(ELeave) RSat::TSendUssdV6();
	CleanupStack::PushL(tSendUssd);
	
	RSat::TSendUssdV6Pckg* tSendUssdPckg = new(ELeave) RSat::TSendUssdV6Pckg(*tSendUssd);
	CleanupStack::PushL(tSendUssdPckg);
	
	iSat.NotifySendUssdPCmd(reqStatus,*(tSendUssdPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tSendUssd->iTextAttribute.iStatus == RSat::ETextAttributeProvided);
	TESTL(tSendUssd->iTextAttribute.iTextAttributeData.Compare(SIMTSY_TXT_ATTR_DATA) ==0);	
	
	RSat::TSendUssdRspV1 *tSendUssdRsp = new (ELeave) RSat::TSendUssdRspV1;
	CleanupStack::PushL(tSendUssdRsp);
	
	RSat::TSendUssdRspV1Pckg *tSendUssdRspPckg = new (ELeave) RSat::TSendUssdRspV1Pckg(*tSendUssdRsp);
	CleanupStack::PushL(tSendUssdRspPckg);
	
	_LIT(KText,"KMeBusyOnUssd");
	TBuf<254> buf(KText);
	tSendUssdRsp->iGeneralResult = RSat::KMeUnableToProcessCmd;
	tSendUssdRsp->iInfoType = RSat::KMeProblem;
	tSendUssdRsp->iAdditionalInfo = buf;
	tSendUssdRsp->SetPCmdNumber(SIMTSY_SEND_USSD_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESendUssd,*(tSendUssdRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
		
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8580- Successful"));

	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8581
 *
 *	SYMREQ REQ7340
 *
 *	SYMTestCaseDesc SEND USSD command rejected by the ME as ME is busy on a SS transaction
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for RETRIEVE MULTIMEDIA MESSAGE using NotifySendUssdPCmd () API.
 *		(ii)	UICC issues SEND USSD command to the ME.
 *		(iii)	ME is unable to execute the command as its busy processing some other SS transaction.
 *		(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		ME is notified of the request.
 *		Upon completion of the proactive command, details are returned in a packaged version of 
 *		 TSendUssdV6in the command parameter. 
 *		ME responds with KMeUnableToProcessCmd in the Terminal response with the additional information 
 *		 KMeBusyOnSs. 
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatSendUssd::testSAT8581L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8581"));
	
	TRequestStatus reqStatus;
	
	RSat::TSendUssdV6* tSendUssd = new(ELeave) RSat::TSendUssdV6();
	CleanupStack::PushL(tSendUssd);
	
	RSat::TSendUssdV6Pckg* tSendUssdPckg = new(ELeave) RSat::TSendUssdV6Pckg(*tSendUssd);
	CleanupStack::PushL(tSendUssdPckg);
	
	iSat.NotifySendUssdPCmd(reqStatus,*(tSendUssdPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tSendUssd->iTextAttribute.iStatus == RSat::ETextAttributeProvided);
	TESTL(tSendUssd->iTextAttribute.iTextAttributeData.Compare(SIMTSY_TXT_ATTR_DATA) ==0);	
	
	RSat::TSendUssdRspV1 *tSendUssdRsp = new (ELeave) RSat::TSendUssdRspV1;
	CleanupStack::PushL(tSendUssdRsp);
	
	RSat::TSendUssdRspV1Pckg *tSendUssdRspPckg = new (ELeave) RSat::TSendUssdRspV1Pckg(*tSendUssdRsp);
	CleanupStack::PushL(tSendUssdRspPckg);
	
	_LIT(KText,"KMeBusyOnSs");
	TBuf<254> buf(KText);
	tSendUssdRsp->iGeneralResult = RSat::KMeUnableToProcessCmd;
	tSendUssdRsp->iInfoType = RSat::KMeProblem;
	tSendUssdRsp->iAdditionalInfo = buf;
	tSendUssdRsp->SetPCmdNumber(SIMTSY_SEND_USSD_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::ESendUssd,*(tSendUssdRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
		
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8581- Successful"));

	return KErrNone;
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8582
 *
 *	SYMREQ REQ7340
 *
 *	SYMTestCaseDesc Cancellation of SEND USSD command
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::NotifySendUssdPCmd using the TSendUssdV6 as the parameter.
 *		(ii)	Cancel the request using RSat::CancelAsyncRequest(ESatNotifySendUssdPCmd).
 *
 *	SYMTestExpectedResults 
 *		Client shall be able to cancel the request. The cancel API shall respond with KErrCancel or KErrNone.
 * 	SYMTestType CIT
 */
 
TInt CSimSatSendUssd::testSAT8582L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8582"));
	
	TRequestStatus reqStatus;

	RSat::TSendUssdV6* tSendUssd = new(ELeave) RSat::TSendUssdV6();
	CleanupStack::PushL(tSendUssd);
	
	RSat::TSendUssdV6Pckg* tSendUssdPckg = new(ELeave) RSat::TSendUssdV6Pckg(*tSendUssd);
	CleanupStack::PushL(tSendUssdPckg);
	
	iSat.NotifySendUssdPCmd(reqStatus,*(tSendUssdPckg));
	iSat.CancelAsyncRequest(ESatNotifySendUssdPCmd);
	User::WaitForRequest(reqStatus);
	TESTL((reqStatus.Int()==KErrNone) || (reqStatus.Int()==KErrCancel));
	
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8582 - Successful"));

	return KErrNone;
	}

CSimSatGenPCmd::CSimSatGenPCmd() 
	{ 
	SetTestStepName(_L("SatTestGenPCmd"));
	}

TVerdict CSimSatGenPCmd::doTestStepL()
	{
	INFO_PRINTF1(_L("FrameIdentifier and Text attribute TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8583L();	// For testing the TEL-SIMTSY-SAT-8583
	testSAT8584L();	// For testing the TEL-SIMTSY-SAT-8584

	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("SEND USSD TESTING - END"));

	return TestStepResult();
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8583
 *
 *	SYMREQ REQ7344
 *
 *	SYMTestCaseDesc Extended support for the proactive commands for which the frame identifier is being added.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat::NotifyGetInkeyPCmd using the TGetInkeyV6 as the parameter that contains
 *			the Frame identifier and the iExtensionID as RSat::KSatV6.
 *
 *	SYMTestExpectedResults 
 *		The request shall be successfully completed with return status as KErrNone.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatGenPCmd::testSAT8583L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8583"));
	
	TRequestStatus reqStatus;
	
	RSat::TGetInkeyV6* tGetInkey = new(ELeave) RSat::TGetInkeyV6;
	CleanupStack::PushL(tGetInkey);
	
	RSat::TGetInkeyV6Pckg *tGetInkeyPckg = new(ELeave) RSat::TGetInkeyV6Pckg(*tGetInkey);
	CleanupStack::PushL(tGetInkeyPckg);

	iSat.NotifyGetInkeyPCmd(reqStatus,*(tGetInkeyPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tGetInkey->iFrameId == 0x25);

	RSat::TGetInkeyRspV2* tGetInkeyRsp = new (ELeave)RSat::TGetInkeyRspV2;
	CleanupStack::PushL(tGetInkeyRsp);
	
	RSat::TGetInkeyRspV2Pckg *tGetInkeyRspPckg = new(ELeave)RSat::TGetInkeyRspV2Pckg(*tGetInkeyRsp);
	CleanupStack::PushL(tGetInkeyRspPckg);
	
	tGetInkeyRsp->iGeneralResult = RSat::KSuccess;
	tGetInkeyRsp->iInfoType = RSat::KNoAdditionalInfo;
	tGetInkeyRsp->SetPCmdNumber(SIMTSY_GET_INKEY_PCMD_NUMBER);
		
	iSat.TerminalRsp(reqStatus,RSat::EGetInkey,*(tGetInkeyRspPckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(4);
	
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8583- Successful"));
	
	return KErrNone;

	}
	
/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8584
 *
 *	SYMREQ REQ7500
 *
 *	SYMTestCaseDesc Extended support for the proactive commands for which the text attributes are being added.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Call the API RSat:: NotifySendSsPCmd using the TSendSsV6 as the parameter that contains the 
 *			text attributes and the iExtensionID as RSat::KSatV6.
 *
 *	SYMTestExpectedResults 
 *		The request shall be successfully completed with return status as KErrNone.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatGenPCmd::testSAT8584L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8584"));
	
	TRequestStatus reqStatus;
	
	RSat::TSendSsV6* tSendSS = new (ELeave)RSat::TSendSsV6;
	CleanupStack::PushL(tSendSS);
	
	RSat::TSendSsV6Pckg* tSendSSpckg = new (ELeave)RSat::TSendSsV6Pckg(*tSendSS);
	CleanupStack::PushL(tSendSSpckg);
	
	iSat.NotifySendSsPCmd(reqStatus,*(tSendSSpckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tSendSS->ExtensionId() == RSat::KSatV6);
	TESTL(tSendSS->iTextAttribute.iStatus == RSat::ETextAttributeProvided);
	TESTL(tSendSS->iTextAttribute.iTextAttributeData.Compare(SIMTSY_TXT_ATTR_DATA) == 0);
	
	CleanupStack::PopAndDestroy(2);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8584- Successful"));
	
	return KErrNone;

	}


CSimSatElemFile::CSimSatElemFile() 
	{ 
	SetTestStepName(_L("SatTestElemFiles"));
	}

TVerdict CSimSatElemFile::doTestStepL()
	{
	INFO_PRINTF1(_L("ELEMENTARY FILES TESTING - START"));

 	CreateConfigFileL(_L("c:\\config4.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSat.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Sat object"));

	testSAT8585L();	// For testing the TEL-SIMTSY-SAT-8585
	testSAT8586L();	// For testing the TEL-SIMTSY-SAT-8586

	iSat.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	
	INFO_PRINTF1(_L("ELEMENTARY FILES TESTING - END"));

	return TestStepResult();
	
	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8585
 *
 *	SYMREQ REQ7771
 *
 *	SYMTestCaseDesc Elementary File change when a RETRIEVE MULTIMEDIA MESSAGE command is issued by UICC to ME.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for RETRIEVE MULTIMEDIA MESSAGE using 
 *			RSat::NotifyRetrieveMultimediaMsgPCmd () API. 
 *		(ii)	UICC issues RETRIEVE MULTIMEDIA MESSAGE command to the ME.
 *		(iii)	Clients requests notification for REFRESH using the RSat::NotifyRefreshPCmd() API
 *				with the TRefreshV1Pckg class.
 *		(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		The TRefreshV1Pckg shall consists of the EFs EFMML  and  EFMMDF in the Filelist as the modified EFs.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatElemFile::testSAT8585L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8585"));
	
	TRequestStatus reqStatus;
	
	RSat::TRetrieveMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TRetrieveMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TRetrieveMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TRetrieveMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifyRetrieveMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TRefreshV2 *tRefreshV6 = new(ELeave) RSat::TRefreshV2();
	CleanupStack::PushL(tRefreshV6);
	
	RSat::TRefreshV2Pckg *tRefreshV6Pckg = new(ELeave)RSat::TRefreshV2Pckg(*tRefreshV6);
	CleanupStack::PushL(tRefreshV6Pckg);
	
	iSat.NotifyRefreshPCmd(reqStatus,*(tRefreshV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tRefreshV6->iFileList.Length()==2);
 	TESTL(tRefreshV6->iFileList[0]==SIMTSY_MM_ELEM_FILE1); 
 	TESTL(tRefreshV6->iFileList[1]==SIMTSY_MM_ELEM_FILE2);
 	
 	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8585- Successful"));
	
	return KErrNone;

	}

/*
 * 	SYMTestCaseID TEL-SIMTSY-NA-8586
 *
 *	SYMREQ REQ7771
 *
 *	SYMTestCaseDesc Elementary File change when a SUBMIT MULTIMEDIA MESSAGE command is issued by UICC to ME.
 *
 *	SYMTestPriority High
 *
 *	SYMTestActions 
 *		(i)	Client requests notification for SUBMIT MULTIMEDIA MESSAGE using 
 *			RSat::NotifyRetrieveMultimediaMsgPCmd () API. 
 *		(ii)	UICC issues SUBMIT MULTIMEDIA MESSAGE command to the ME.
 *		(iii)	Clients requests notification for REFRESH using the RSat::NotifyRefreshPCmd() API with the 
 *			TRefreshV1Pckg class.
 *		(iv)	ME sends Terminal Response to the UICC using RSat::TerminalRsp ().
 *
 *	SYMTestExpectedResults 
 *		The TRefreshV1Pckg shall consists of the EFs EFMML  and  EFMMDF in the Filelist as the modified EFs.
 *
 * 	SYMTestType CIT
 */
 
TInt CSimSatElemFile::testSAT8586L()
	{
	
	INFO_PRINTF1(_L("Executing the Test Case TEL-SIMTSY-SAT-8586"));
	
	TRequestStatus reqStatus;
	
	RSat::TSubmitMultimediaMessageV6 *tMMMessageV6 = new (ELeave) RSat::TSubmitMultimediaMessageV6;
	CleanupStack::PushL(tMMMessageV6);

	RSat::TSubmitMultimediaMessageV6Pckg *tMMMessagePckg = new (ELeave)
	       	RSat::TSubmitMultimediaMessageV6Pckg(*tMMMessageV6);
	CleanupStack::PushL(tMMMessagePckg);

	iSat.NotifySubmitMultimediaMsgPCmd(reqStatus,*tMMMessagePckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	RSat::TRefreshV2 *tRefreshV6 = new(ELeave) RSat::TRefreshV2();
	CleanupStack::PushL(tRefreshV6);
	
	RSat::TRefreshV2Pckg *tRefreshV6Pckg = new(ELeave)RSat::TRefreshV2Pckg(*tRefreshV6);
	CleanupStack::PushL(tRefreshV6Pckg);
	
	iSat.NotifyRefreshPCmd(reqStatus,*(tRefreshV6Pckg));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(tRefreshV6->iFileList.Length()==2);
 	TESTL(tRefreshV6->iFileList[0]==SIMTSY_MM_ELEM_FILE1); 
 	TESTL(tRefreshV6->iFileList[1]==SIMTSY_MM_ELEM_FILE2);
	
	CleanupStack::PopAndDestroy(4);
	
	INFO_PRINTF1(_L("Execution of the Test Case TEL-SIMTSY-SAT-8586- Successful"));
	
	return KErrNone;
	}

