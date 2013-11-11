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
// Test GenConn Dialog Server
// 
//

#include <e32test.h>
#include <c32comm.h>
#include <agentdialog.h>
#include <cdblen.h>

GLDEF_D RTest test(_L("Generic Agent Dialog Testcode"));

LOCAL_C void checkError(RGenConAgentDialogServer& aDlgSvr, TInt aError);
LOCAL_C void checkPctError(RGenConAgentDialogServer& aDlgSvr, TInt aError);
LOCAL_C void doExampleL();

_LIT(KIapName,"New IAP");

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	
	CTrapCleanup* cleanup=CTrapCleanup::New();

	test.Title();
	TRAPD(error,doExampleL());
	test.Close();
    
	if (error != KErrNone)
		User::Panic(_L("NETDIAL DIALOG TEST"),error);

	delete cleanup;
	
	__UHEAP_MARKEND;
	
	return KErrNone;
    }

LOCAL_C void doExampleL()
	{
// Start the Comms Server - the Database Server thread runs in this process
 	// When bootstrapping C32 we have to avoid the PhBkSyncServer being started, since
 	// it needs a different CommDB
 	_LIT(KPhbkSyncCMI, "phbsync.cmi");
    TInt ret = StartC32WithCMISuppressions(KPhbkSyncCMI);
	test((ret==KErrNone)||(ret==KErrAlreadyExists));
	test.Printf(_L("Started C32\n"));

	RGenConAgentDialogServer dlgSv;
	test(dlgSv.Connect()==KErrNone);
	test.Printf(_L("Connected Dialog Server\n"));

	TRequestStatus status;

	test.Printf(_L("\nTesting the IAP Selection Dialog\n")); 
	TUint32 iap=0;
	TConnectionPrefs prefs;
	prefs.iRank = 1;
	prefs.iDirection = ECommDbConnectionDirectionOutgoing;
	prefs.iBearerSet = ECommDbBearerCSD;
	dlgSv.IapConnection(iap,prefs,status);
	test.Printf(_L("Requested IAP selection\n"));
	User::WaitForRequest(status);
	ret=status.Int();
	test.Printf(_L("Request IAP selection completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	test.Printf(_L("IAP id = %d\n"),iap);

	
	test.Printf(_L("\nTesting the Modem And Location Selection Dialog\n")); 
	TUint32 modemId=0;
	TUint32 locationId=0;
	dlgSv.ModemAndLocationSelection(modemId,locationId,status);
	test.Printf(_L("Requested Modem And Location selection\n"));
	User::WaitForRequest(status);
	ret=status.Int();
	test.Printf(_L("Request Location selection completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	test.Printf(_L("Modem id = %d\n"),modemId);
	test.Printf(_L("Location id = %d\n"),locationId);
	

/*	test.Printf(_L("\nTesting the ISP selection Dialog\n")); 
	TIspConnectionIds conDetails;
	dlgSv.IspConnection(conDetails, prefs, status);
	test.Printf(_L("Requested ISP selection\n"));
	User::WaitForRequest(status);
	ret=status.Int();
	test.Printf(_L("Request ISP selection completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	test.Printf(_L("ServiceId = %d\n"),conDetails.iServiceId);
	test.Printf(_L("Service table = %S\n"),&conDetails.iServiceTable);
	test.Printf(_L("Modem id = %d\n"),conDetails.iModemId);
	test.Printf(_L("Location id = %d\n"),conDetails.iLocationId);
	test.Printf(_L("ChargeCard id = %d\n"),conDetails.iChargecardId);
*/
	test.Printf(_L("\nTesting the IAP Warning Dialog\n")); 
	TBuf<16> iapName(KIapName);
	TBool response = EFalse;
	dlgSv.WarnNewIapConnection(prefs,KErrBadName,iapName,response,status);
	test.Printf(_L("Requested IAP warning\n"));
	User::WaitForRequest(status);
	ret=status.Int();
	test.Printf(_L("Request IAP warning completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	if (response)
		test.Printf(_L("Attempt new connection\n"));
	else
		test.Printf(_L("Do not attempt new connection\n"));

/*	test.Printf(_L("\nTesting the ISP Warning Dialog\n")); 
	TIspConnectionNames ispNames;
	ispNames.iServiceName = KIspName;
	response = EFalse;
	dlgSv.WarnNewIspConnection(prefs,KErrCommsLineFail,ispNames,response,status);
	test.Printf(_L("Requested ISP warning\n"));
	User::WaitForRequest(status);
	ret=status.Int();
	test.Printf(_L("Request ISP warning completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	if (response)
		test.Printf(_L("Attempt new connection\n"));
	else
		test.Printf(_L("Do not attempt new connection\n"));

*/
	test.Printf(_L("\nTesting the Login Dialog\n")); 
	TBuf<KCommsDbSvrMaxFieldLength> username=_L("LnUser");
	TBuf<KCommsDbSvrMaxFieldLength> password=_L("LnPass");
	dlgSv.Login(username,password,EFalse,status);
	User::WaitForRequest(status);
	ret=status.Int();
	test.Printf(_L("Login completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	test.Printf(_L("Username=%S Password=%S\n"),&username,&password);

	test.Printf(_L("\nTesting the Authentication Dialog\n")); 
	username.Copy(_L("User"));
	password.Copy(_L("Pass"));
	dlgSv.Authenticate(username,password,EFalse,status);
	User::WaitForRequest(status);
	ret=status.Int();
	test.Printf(_L("Authenticate completed with code=%d\n"),ret);
	checkError(dlgSv,ret);
	test.Printf(_L("Username=%S Password=%S\n"),&username,&password);

	test.Printf(_L("\nTesting the Reconnection Dialog\n")); 
	TBool b=ETrue;
	dlgSv.Reconnect(b,status);
	User::WaitForRequest(status);
	ret=status.Int();
	checkError(dlgSv,ret);
	if (b)
		test.Printf(_L("Reconnect Required\n"));
	else
		test.Printf(_L("Reconnect Not Required\n"));
	
	test.Printf(_L("\nTesting the QoS Warning Dialog\n")); 
	TBool dlgResponse=ETrue;
	dlgSv.QoSWarning(dlgResponse,status);
	User::WaitForRequest(status);
	ret=status.Int();
	checkError(dlgSv,ret);
	if (dlgResponse)
		test.Printf(_L("Disconnect\n"));
	else
		test.Printf(_L("Do Not Disconnect\n"));
	
	test.Printf(_L("Press a key to continue\n"));
	test.Getch();
	test.Console()->ClearScreen();

	test.Printf(_L("\nTesting the PCT\n")); 
	dlgSv.OpenPct();
	TRequestStatus destroyStatus;
	dlgSv.DestroyPctNotification(destroyStatus);
	test.Printf(_L("Requested destroy notification\n"));	
	TBuf<256> writeBuffer=_L("Welcome to Ann's ISP");
	dlgSv.WritePct(writeBuffer);
	writeBuffer=_L("Login:");
	dlgSv.WritePct(writeBuffer);
	TBuf<256> readBuffer=_L("UserName");
	dlgSv.ReadPct(readBuffer,status);
	User::WaitForRequest(status);
	ret=status.Int();
	test.Printf(_L("Read from PCT completed with code = %d\n"),ret);
	checkPctError(dlgSv,ret);
	test.Printf(_L("Read from PCT = %S\n"),&readBuffer);
	writeBuffer=_L("Password:");
	dlgSv.WritePct(writeBuffer);
	readBuffer=_L("PassWord");
	dlgSv.ReadPct(readBuffer,status);
	User::WaitForRequest(status);
	ret=status.Int();
	test.Printf(_L("Read from PCT completed with code = %d\n"),ret);
	checkPctError(dlgSv,ret);
	test.Printf(_L("Read from PCT = %S\n"),&readBuffer);
	writeBuffer=_L("Login correct!");
	dlgSv.WritePct(writeBuffer);
	User::After(2*1000000);	
	dlgSv.CancelDestroyPctNotification();
	User::WaitForRequest(destroyStatus);
	test(destroyStatus.Int()==KErrCancel);
	dlgSv.ClosePct();
	dlgSv.Close();
	}

LOCAL_C void checkError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
	{
	if (aError!=KErrNone && aError!=KErrCancel)
		aDlgSvr.Close();

	test((aError==KErrNone)||(aError==KErrCancel));
	}

LOCAL_C void checkPctError(RGenConAgentDialogServer& aDlgSvr, TInt aError)
	{
	if (aError!=KErrNone)
		{
		aDlgSvr.ClosePct();
		TRequestStatus destroyStatus;
		User::WaitForRequest(destroyStatus);
		aDlgSvr.Close();
		}

	test(aError==KErrNone);
	}	
