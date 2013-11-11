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
// Tests added for new functionality in 8.0
// 
//

/**
 @file
*/

#include "Te_EtelSatTestStepBase.h"
#include "TE_EtelSatpcmds1e.h"

CTestSatPCmds1e::CTestSatPCmds1e()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSatPCmds1e"));
	}
	
enum TVerdict CTestSatPCmds1e::doTestStepL()
	{
	INFO_PRINTF1(_L("***********************************************"));
	INFO_PRINTF1(_L("RSat Proactive Commands Functionality suite - 4"));
	INFO_PRINTF1(_L("***********************************************"));
	
	TInt ret=phone.Open(iTelServer,DSATTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	ret=sat.Open(phone);
    TEST(ret==KErrNone);
    
	TRequestStatus reqStatus;
	
	//CB DOWNLOAD
	
	RSat::TCbDownloadV3 cbDownload;
	RSat::TCbDownloadV3Pckg cbDownloadPckg(cbDownload);
	
	sat.NotifyCbDownload(reqStatus, cbDownloadPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	
	TEST(cbDownload.iDeviceId==RSat::KEarPiece);
	TEST(cbDownload.iPdu==DSATTSY_CB_PDU);
	
	//Cancel
	sat.NotifyCbDownload(reqStatus, cbDownloadPckg);
	sat.CancelAsyncRequest(ESatNotifyCbDownload);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	
	//SMS PP DOWNLOAD
	
	RSat::TSmsPpDownloadV3 smsPpDownload;
	RSat::TSmsPpDownloadV3Pckg smsPpDownloadPckg(smsPpDownload);
	
	sat.NotifySmsPpDownload(reqStatus, smsPpDownloadPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	
	TEST(smsPpDownload.iDeviceId=RSat::KEarPiece);
	TEST(smsPpDownload.iRpAddress.iTypeOfNumber	== DSATTSY_TON);	
	TEST(smsPpDownload.iRpAddress.iNumberPlan	== DSATTSY_NPI);
	TEST(smsPpDownload.iRpAddress.iTelNumber	== DSATTSY_TEL_NUMBER);	
	TEST(smsPpDownload.iPdu== DSATTSY_SMS_TPDU);
	
	//Cancel
	sat.NotifySmsPpDownload(reqStatus, smsPpDownloadPckg);
	sat.CancelAsyncRequest(ESatNotifySmsPpDownload);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	//TIMER EXPIRATION
	
	RSat::TTimerExpirationV3 timerExpiration;
	RSat::TTimerExpirationV3Pckg timerExpirationPckg(timerExpiration);
	
	sat.NotifyTimerExpiration(reqStatus, timerExpirationPckg);
	
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	
	TEST(timerExpiration.iTimerId == RSat::KTimer1);	
	TEST(timerExpiration.iTimerValue.iTimeUnit == RSat::ESeconds);
	
	//Cancel
	sat.NotifyTimerExpiration(reqStatus, timerExpirationPckg);
	sat.CancelAsyncRequest(ESatNotifyTimerExpiration);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	//LOCAL INFO
	
	RSat::TLocalInfoV3 localInfo;
	RSat::TLocalInfoV3Pckg localInfoPckg(localInfo);
	
	sat.NotifyLocalInfoPCmd(reqStatus, localInfoPckg);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	
	TEST(localInfo.iDevideId == RSat::KKeypad);	
	TEST(localInfo.iInfoType == RSat::KProvideLocalInfo);
	
	//Cancel
	sat.NotifyLocalInfoPCmd(reqStatus, localInfoPckg);
	sat.CancelAsyncRequest(ESatNotifyLocalInfoPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);	
	
	//Response
	RSat::TLocalInfoRspV3 localInfoRsp;
	RSat::TLocalInfoRspV3Pckg localInfoRspPckg(localInfoRsp);	
	localInfoRsp.SetPCmdNumber(DSATTSY_LOCAL_INFO_NUMBER);
	localInfoRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	localInfoRsp.iInfoType=DSATTSY_INFO_TYPE;	
	localInfoRsp.iAdditionalInfo=DSATTSY_NULL_BUF;
	
	sat.NotifyLocalInfoPCmd(reqStatus, localInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	sat.TerminalRsp(reqStatus,RSat::ELocalInfo,localInfoRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Local Infot - passed"),iTestCount++);	

		
	//POLLING INTERVAL
	
	RSat::TPollingIntervalV3 pollingIntv;
	RSat::TPollingIntervalV3Pckg pollingIntvPckg(pollingIntv);
	
	sat.NotifyPollingIntervalPCmd(reqStatus, pollingIntvPckg);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	
	TEST(pollingIntv.iIntv.iTimeUnit == DSATTSY_TIME_UNIT);
	TEST(pollingIntv.iIntv.iNumOfUnits == DSATTSY_NUM_OF_UNITS);
	TEST(pollingIntv.iDestination == RSat::KEarPiece);
	
	//Cancel
	sat.NotifyPollingIntervalPCmd(reqStatus, pollingIntvPckg);
	sat.CancelAsyncRequest(ESatNotifyPollingIntervalPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	//Response	
	RSat::TPollingIntervalRspV3 pollingIntvRsp;
	RSat::TPollingIntervalRspV3Pckg pollingIntvRspPckg(pollingIntvRsp);
	pollingIntvRsp.SetPCmdNumber(DSATTSY_POLLING_INTERVAL_NUMBER);
	pollingIntvRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	pollingIntvRsp.iInfoType=DSATTSY_INFO_TYPE;	
	pollingIntvRsp.iAdditionalInfo=DSATTSY_NULL_BUF;
	
	sat.NotifyPollingIntervalPCmd(reqStatus, pollingIntvPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	
	
	sat.TerminalRsp(reqStatus,RSat::EPollingInterval,pollingIntvRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Polling Interval - passed"),iTestCount++);	
	
	//POLLING OFF
	
	RSat::TPollingOffV3 pollingOff;
	RSat::TPollingOffV3Pckg pollingOffPckg(pollingOff);
	
	sat.NotifyPollingOffPCmd(reqStatus, pollingOffPckg);

	User::WaitForRequest(reqStatus);	
	TEST(reqStatus.Int()==KErrNone);
	TEST(pollingOff.iIntv.iTimeUnit==RSat::ESeconds);
	TEST(pollingOff.iIntv.iNumOfUnits == 20);
	TEST(pollingOff.iDestination == RSat::KNetwork);
	
	//Cancel	
	sat.NotifyPollingOffPCmd(reqStatus, pollingOffPckg);
	sat.CancelAsyncRequest(ESatNotifyPollingOffPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	//Response	
	RSat::TPollingOffRspV3 pollingOffRsp;
	RSat::TPollingOffRspV3Pckg pollingOffRspPckg(pollingOffRsp);	
	pollingOffRsp.SetPCmdNumber(DSATTSY_POLLING_OFF_NUMBER);
	pollingOffRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	pollingOffRsp.iInfoType=DSATTSY_INFO_TYPE;	
	pollingOffRsp.iAdditionalInfo=DSATTSY_NULL_BUF;
	
	sat.NotifyPollingOffPCmd(reqStatus, pollingOffPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	
	
	sat.TerminalRsp(reqStatus,RSat::EPollingOff,pollingOffRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Polling Off - passed"),iTestCount++);	
	
	
	
	//TIMER MANAGEMENT
	
	RSat::TTimerMgmtV3		timerMngt;
	RSat::TTimerMgmtV3Pckg	timerMngtPckg(timerMngt);

	sat.NotifyTimerMgmtPCmd(reqStatus, timerMngtPckg);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(timerMngt.iTimerId == RSat::KTimer1);	
	TEST(timerMngt.iTimerValue.iTimeUnit==RSat::ESeconds);
	TEST(timerMngt.iTimerValue.iNumOfUnits == 10);
	TEST(timerMngt.iOperation ==RSat::EStartTimer);	
	
    //Cancel	
	sat.NotifyTimerMgmtPCmd(reqStatus, timerMngtPckg);
	sat.CancelAsyncRequest(ESatNotifyTimerMgmtPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
    
    //Response
	RSat::TTimerMgmtRspV3 timerMangtRsp;
	RSat::TTimerMgmtRspV3Pckg timerMgmtRspPckg(timerMangtRsp);	
	timerMangtRsp.SetPCmdNumber(DSATTSY_TIMER_MNGT_NUMBER);
	timerMangtRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	timerMangtRsp.iInfoType=DSATTSY_INFO_TYPE;	
	timerMangtRsp.iAdditionalInfo=DSATTSY_NULL_BUF;
	
	sat.NotifyTimerMgmtPCmd(reqStatus, timerMngtPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	

	sat.TerminalRsp(reqStatus,RSat::ETimerMngt,timerMgmtRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Tmer Managt - passed"),iTestCount++);	
    
    
    
    
    sat.Close();
	phone.Close();

	return TestStepResult();
	}
