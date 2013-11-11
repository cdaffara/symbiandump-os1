// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>

#include "TE_EtelBase.h"
#include "TE_Etelmemberfn.h"
#include "../../DSTD/DSTD.H"

CTestMemberFnSetGet::CTestMemberFnSetGet()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestMemberFnSetGet"));
	}

enum TVerdict CTestMemberFnSetGet::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Set and Get Functions"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RTestPhone phone;
    ret = phone.Open(server,DSTD_PHONE_NAME);

	TRequestStatus reqStatus;
	TBufC8<20> set(DSTD_NARROW);
	TBuf8<20> get;
	TBuf16<20> get2;
	TBuf16<20> get21;
	TBuf16<20> get22;
	TBufC16<20> set2(DSTD_UNICODE);

	//
	// Execute the following tests using extended error codes.  Only TestNarrowSetAndGet()
	// has actually been converted to use the extended errors.  All other should work as
	// normal anyway.
	//
	ret = server.SetExtendedErrorGranularity(RTelServer::EErrorExtended);
	TESTCHECK(ret, KErrNone);

	phone.TestNarrowSetAndGet(reqStatus, set, get);
	User::WaitForRequest(reqStatus);
	TESTCHECK(reqStatus.Int(), KErrLicenseeSpecificOkay);
	TESTCHECKSTR(get, DSTD_NARROW);
	
	phone.TestUnicodeSetAndNarrowGet(reqStatus, get, set2);
	User::WaitForRequest(reqStatus);
	TESTCHECK(reqStatus.Int(), KErrNone);
	TESTCHECKSTR(get, DSTD_NARROW);

	phone.TestNarrowSetAndUnicodeGet(reqStatus, set, get2);
	User::WaitForRequest(reqStatus);
	TESTCHECK(reqStatus.Int(), KErrNone);
	TESTCHECKSTR(get2, DSTD_UNICODE);

	phone.TestUnicodeSetAndGet(reqStatus, set2, get2);
	User::WaitForRequest(reqStatus);
	TESTCHECK(reqStatus.Int(), KErrNone);
	TESTCHECKSTR(get2, DSTD_UNICODE);

	phone.TestDoubleUnicodeGet(reqStatus, get21, get22);
	User::WaitForRequest(reqStatus);
	TESTCHECK(reqStatus.Int(), KErrNone);
	TESTCHECKSTR(get21, DSTD_UNICODE);
	TESTCHECKSTR(get22, DSTD_UNICODE);
	
	ret = server.SetExtendedErrorGranularity(RTelServer::EErrorBasic);
	TESTCHECK(ret, KErrNone);

	phone.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}


CTestPhoneMemberFn::CTestPhoneMemberFn()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestPhoneMemberFn"));
	}

enum TVerdict CTestPhoneMemberFn::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Phone Device Member Functions"));

    RTelServer server;	
	TInt ret = server.Connect();	//open server
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RPhone phone;
    ret = phone.Open(server,DSTD_PHONE_NAME);	//open phone
	RPhone::TLineInfo iLineInfo;
	ret=phone.GetLineInfo(0,iLineInfo);
    TESTCHECK(ret, KErrNone);

	RPhone::TCaps phoneCaps;
	ret=phone.GetCaps(phoneCaps);
	TESTCHECK(ret, KErrNone);
	RPhone::TStatus phoneStatus;
	ret=phone.GetStatus(phoneStatus);
    TESTCHECK(ret, KErrNone);
	TInt lineCount;
    ret=phone.EnumerateLines(lineCount);
	TESTCHECK(ret, KErrNone);
	TESTCHECK(lineCount, DSTD_NUMBER_OF_LINES);
 	RPhone::TLineInfo	lineInfo;
	ret=phone.GetLineInfo(0,lineInfo);
    TESTCHECK(ret, KErrNone);
	RPhone::TModemDetection detection;
	TRequestStatus phoneDetectionStatus;

	phone.NotifyModemDetected(phoneDetectionStatus,detection);//REQUEST 1
	User::WaitForRequest(phoneDetectionStatus);//FINISH REQUEST 1

	phone.NotifyModemDetected(phoneDetectionStatus,detection);//REQUEST 2
	User::WaitForRequest(phoneDetectionStatus);//FINISH REQUEST 2
 	phone.NotifyModemDetectedCancel();

	phone.Close();	//close phone
	ServerClose(server);	//close server

	return TestStepResult();
	}

CTestLineMemberFn::CTestLineMemberFn()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestLineMemberFn"));
	}

enum TVerdict CTestLineMemberFn::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Line Member Functions"));

    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RPhone phone;
    ret = phone.Open(server,DSTD_PHONE_NAME);
	TESTCHECK(ret, KErrNone);

	RLine line;
    ret = line.Open(phone,DSTD_LINE_NAME);
	TESTCHECK(ret, KErrNone);

	RLine::TCaps lineCaps;
	ret=line.GetCaps(lineCaps);
	TESTCHECK(ret, KErrNone);
	RCall::TStatus lineStatus;
	ret=line.GetStatus(lineStatus);
	TESTCHECK(ret, KErrNone);
	RCall::THookStatus hookStatus;
	ret=line.GetHookStatus(hookStatus);
	TESTCHECK(ret, KErrNone);
	TInt callCount;
	ret=line.EnumerateCall(callCount);
	TESTCHECK(ret, KErrNone);
	RLine::TCallInfo callInfo;
	ret=line.GetCallInfo(0,callInfo);
	TESTCHECK(ret, KErrNone);

	TRequestStatus notifyIncomingCallStatus;
	TName name;
	line.NotifyIncomingCall(notifyIncomingCallStatus,name);	//Request 1
	User::WaitForRequest(notifyIncomingCallStatus);	//Request 1 completed
	line.NotifyIncomingCall(notifyIncomingCallStatus,name);	//Request 2
	User::WaitForRequest(notifyIncomingCallStatus);	//Request 2 completed
	line.NotifyIncomingCallCancel();

	TRequestStatus notifyHookChangeStatus;
	RCall::THookStatus hookStatusAsyn;
	line.NotifyHookChange(notifyHookChangeStatus,hookStatusAsyn);//Request 3
	User::WaitForRequest(notifyHookChangeStatus);//Request 3 completed
	line.NotifyHookChange(notifyHookChangeStatus,hookStatusAsyn);//Request 4
	User::WaitForRequest(notifyHookChangeStatus);//Request 4 completed
	line.NotifyHookChangeCancel();

	TRequestStatus notifyStatusChangeStatus;
	RCall::TStatus lineStatusAsyn;
	line.NotifyStatusChange(notifyStatusChangeStatus,lineStatusAsyn);//Request 5
	User::WaitForRequest(notifyStatusChangeStatus);//Request 5 completed
	line.NotifyStatusChange(notifyStatusChangeStatus,lineStatusAsyn);//Request 6
	User::WaitForRequest(notifyStatusChangeStatus);//Request 6 completed
	line.NotifyStatusChangeCancel();

	line.Close();
	phone.Close();
	server.UnloadPhoneModule(DSTD_MODULE_NAME);
	ServerClose(server);

	return TestStepResult();
	}

CTestCallMemberFn::CTestCallMemberFn()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCallMemberFn"));
	}

enum TVerdict CTestCallMemberFn::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Call Member Functions"));

    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
    ret = call.OpenNewCall(server,DSTD_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);

	RCall::TStatus callStatus;
	TRequestStatus callRequestStatus;
	ret=call.GetStatus(callStatus);
	TESTCHECK(ret, KErrNone);
	RCall::TCaps callCaps;
	ret=call.GetCaps(callCaps);
	TESTCHECK(ret, KErrNone);
   	RCall::TBearerService callBearerService;
   	ret=call.GetBearerServiceInfo(callBearerService);
	TESTCHECK(ret, KErrNone);

	RCall::THookStatus hookStatus;
	call.NotifyHookChange(callRequestStatus,hookStatus);//Request 1
	User::WaitForRequest(callRequestStatus);//Completed Request 1
	call.NotifyHookChange(callRequestStatus,hookStatus);//Request 2
	User::WaitForRequest(callRequestStatus);//Completed Request 2
	call.NotifyHookChangeCancel();

    call.NotifyStatusChange(callRequestStatus,callStatus);//Request 3
	User::WaitForRequest(callRequestStatus);//Completed Request 3
    call.NotifyStatusChange( callRequestStatus,callStatus);//Request 4
	User::WaitForRequest(callRequestStatus);//Completed Request 4
	call.NotifyStatusChangeCancel();

//	User::WaitForRequest(callRequestStatus); //Request already completed
	TInt dum = (callRequestStatus==KErrCancel || callRequestStatus==KErrNone);
	TESTCHECK(dum, TRUE);

	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	TRequestStatus dialStatus;
    call.Dial(dialStatus,callParamsPckg,dialString); //Request 5
	call.DialCancel();
	User::WaitForRequest(dialStatus);//Completed request 5

    ret=call.Dial(callParamsPckg,dialString);
	TESTCHECK(ret, KErrNone);
	call.DialCancel();

    ret=call.Connect(callParamsPckg);
	TESTCHECK(ret, KErrNone);
	TRequestStatus connectStatus;
    call.Connect(connectStatus,callParamsPckg);//Request 6
	User::WaitForRequest(connectStatus);//Completed request 6
    call.ConnectCancel();

	ret=call.AnswerIncomingCall(callParamsPckg);
	TESTCHECK(ret, KErrNone);
	TRequestStatus answerStatus;
    call.AnswerIncomingCall(answerStatus,callParamsPckg);//Request 7
	User::WaitForRequest(answerStatus);//Completed request 7
    call.AnswerIncomingCallCancel();

	ret=call.HangUp(); // call becomes Unowned
	TESTCHECK(ret, KErrNone);

	RCall::TCommPort port;
	ret=call.LoanDataPort(port);
	TESTCHECK(ret, KErrNone);
	ret=call.RecoverDataPort();
	TESTCHECK(ret, KErrNone);
    ret=call.Dial(callParamsPckg,DACQ_PHONE_NUMBER_TO_DIAL);
	TESTCHECK(ret, KErrNone);
	ret=call.TransferOwnership();
	TESTCHECK(ret, KErrEtelNoClientInterestedInThisCall);

	TRequestStatus acquireStatus;
	call.AcquireOwnership(acquireStatus);//request 8
	User::WaitForRequest(acquireStatus);//completed request 8
	call.AcquireOwnership(acquireStatus);//request 9
	User::WaitForRequest(acquireStatus);//Completed request 9
	call.AcquireOwnershipCancel();
	RCall::TOwnershipStatus ownerStatus;
	ret=call.GetOwnershipStatus(ownerStatus);
	TESTCHECK(ret, KErrNone);

	TDummyString8 params;
	ret=call.GetCallParams(params);
	TESTCHECK(ret, KErrNone);

	RCall::TFaxSessionSettings getSettings;
	ret=call.GetFaxSettings(getSettings);
	TESTCHECK(ret, KErrNone);
	TESTCHECKSTR(getSettings.iFaxId, DACQ_MYFAX);

	RCall::TFaxSessionSettings setSettings;
	setSettings.iFaxId=DACQ_MYFAX;
	ret=call.SetFaxSettings(setSettings);
	TESTCHECK(ret, KErrNone);
	
	//test sending file handle to server
	RFs fs;
	ret = fs.Connect();
	TESTCHECK(ret, KErrNone);
	CleanupClosePushL(fs);
	fs.ShareProtected();
	TDriveUnit driveUnit(EDriveC);
	//create private path for this process;
	fs.CreatePrivatePath(driveUnit);
	TPath path;
	TFileName fname;
	fs.PrivatePath(path);	
	
	TDriveName drive=driveUnit.Name();
	fname.Insert(0, drive);
		
	_LIT(KHeaderFile, "TestHeaderFile.txt");
	fname.Append(path);
	fname.Append(KHeaderFile);
	
	RFile file;	
	CleanupClosePushL(fs);
	
	file.Replace(fs, fname, EFileWrite);
	file.Write(DACQ_MFAXFILE);
	
	ret = call.AdoptFaxSharedHeaderFile(file);
	TESTCHECK(ret, KErrNone);
	
	CleanupStack::PopAndDestroy();//file
	CleanupStack::PopAndDestroy();//fs	

	// Test the retrieval of the reference count
 	// Simulate two clients opening an handle on the same call objects
 	// from two different servers.
 
 	RCall callA;
	TName name;
 	ret = callA.OpenNewCall(server,DSTD_CALL_FULLNAME,name);
	TESTCHECK(ret, KErrNone);
 
 	TInt countA=0;
 	countA=callA.ReferenceCount();
	TESTCHECK(countA, 1);
 
 	RTelServer server2;
	
	ret = server2.Connect();
	TESTCHECK(ret, KErrNone);
	ret = server2.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
 
 	TName callname = DSTD_CALL_FULLNAME;
 	callname.Append(name);
 	RCall callB;
 	ret = callB.OpenExistingCall(server2,callname);
	TESTCHECK(ret, KErrNone);
 
 	countA=callA.ReferenceCount();
	TESTCHECK(countA, 2);
 
 	TInt countB=0;
 	countB=callB.ReferenceCount();
	TESTCHECK(countB, 2);
 
 	INFO_PRINTF1(_L("Test Reference Count getter successful"));
 
	call.Close();
	callA.Close();
	ServerClose(server);
	callB.Close();
	ServerClose(server2);
	return TestStepResult();
	}

CTestFaxMemberFn::CTestFaxMemberFn()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestFaxMemberFn"));
	}

enum TVerdict CTestFaxMemberFn::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Fax Member Functions"));

    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
    ret = call.OpenNewCall(server,DSTD_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	RFax fax;
	ret=fax.Open(call);
	TESTCHECK(ret, KErrNone);

	TRequestStatus readFaxStatus;
	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;

	fax.Read(readFaxStatus,buf);
	User::WaitForRequest(readFaxStatus);
	// Check result ....
	TESTCHECKSTR(buf, DACQ_FAX_BUF_DATA);

	TRequestStatus writeFaxStatus;
	TAcqFaxBuf writeBuf=DACQ_FAX_BUF_DATA;
	fax.Write(writeFaxStatus,writeBuf);
	User::WaitForRequest(writeFaxStatus);

	TRequestStatus waitForEndOfPageStatus;
	fax.WaitForEndOfPage(waitForEndOfPageStatus);
	User::WaitForRequest(waitForEndOfPageStatus);

	RFax::TProgress progress;
	ret=fax.GetProgress(progress);

	//
	// Call returns KErrNotFound with no phone available or KErrNone when a phone is
	// present.  If the watches are not present, then the phone will not be available.
	//
	TESTCHECKCONDITION(ret==KErrNotFound  ||  ret==KErrNone);

/*	TRequestStatus progressNotificationStatus;
	fax.ProgressNotification(progressNotificationStatus,progress);
	User::WaitForRequest(progressNotificationStatus);

	// Cancels
	fax.ProgressNotification(progressNotificationStatus,progress);
	ret=fax.ProgressNotificationCancel();
	TESTCHECK(ret, KErrNone);
*/	fax.Read(readFaxStatus,buf);
	ret=fax.TerminateFaxSession();
	TESTCHECK(ret, KErrNone);
	fax.Write(writeFaxStatus,writeBuf);
	ret=fax.TerminateFaxSession();
	TESTCHECK(ret, KErrNone);
	fax.WaitForEndOfPage(waitForEndOfPageStatus);
	ret=fax.TerminateFaxSession();
	TESTCHECK(ret, KErrNone);

	fax.Close();
	call.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

    ServerClose(server);

	return TestStepResult();
    }

