//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
//

// Create a communications database 

#include <e32test.h>
#include <f32file.h>
#include <commdb.h>
#include <cdblen.h>
#include <cdbpreftable.h>
#include <faxdefn.h>
#include <d32comm.h>
#include <cdbtemp.h>


_LIT(KEditorMess,"Database Editor");
LOCAL_C RTest test(KEditorMess);
LOCAL_C CCommsDatabase* theDb;


//
// The Dial Out ISP table will have 3 records, plus
// two extra records that are read only
//


LOCAL_C CreateDialInISPTable()
{
	TUint32 id;

	CCommsDbTableView* theView;

	theView = theDb->OpenTableLC(TPtrC(DIAL_IN_ISP) );

	//
	//	Record #1
	//
	theView->InsertRecord( id );
	theView->WriteTextL( TPtrC(COMMDB_NAME), _L("Record #1") ) ;
	//Just write the columns that can't be null
	theView->WriteTextL( TPtrC(ISP_IF_NAME), _L("PPP") ) ;
	theView->WriteBoolL( TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), ETrue ) ;

	theView->PutRecordChanges();

	//
	//	Record #2
	//
	theView->InsertRecord( id );
	theView->WriteTextL( TPtrC(COMMDB_NAME), _L("Record #2") ) ;
	//Just write the columns that can't be null
	theView->WriteTextL( TPtrC(ISP_IF_NAME), _L("PPP") ) ;
	theView->WriteBoolL( TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), ETrue ) ;
	theView->PutRecordChanges();


	//
	//	Record #3
	//
	theView->InsertRecord( id );
	theView->WriteTextL( TPtrC(COMMDB_NAME), _L("Record #3") ) ;
	//Just write the columns that can't be null
	theView->WriteTextL( TPtrC(ISP_IF_NAME), _L("SLIP") ) ;
	theView->WriteBoolL( TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP6_IP_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP6_IP_DNS_ADDR_FROM_SERVER), ETrue ) ;
	theView->PutRecordChanges();

	//
	//	Record #4 ( Read Only )
	//
	theView->InsertRecord( id );
	theView->WriteTextL( TPtrC(COMMDB_NAME), _L("Record #4 (Read Only)") ) ;
	//Just write the columns that can't be null
	theView->WriteTextL( TPtrC(ISP_IF_NAME), _L("SLIP") ) ;
	theView->WriteBoolL( TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP6_IP_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP6_IP_DNS_ADDR_FROM_SERVER), ETrue ) ;
	theView->PutRecordChanges(EFalse, ETrue);

	//
	//	Record #5 ( Read Only )
	//
	theView->InsertRecord( id );
	theView->WriteTextL( TPtrC(COMMDB_NAME), _L("Record #5 (Read Only)") ) ;
	//Just write the columns that can't be null
	theView->WriteTextL( TPtrC(ISP_IF_NAME), _L("PPP") ) ;
	theView->WriteBoolL( TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue ) ;
	theView->WriteBoolL( TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), ETrue ) ;
	theView->PutRecordChanges(EFalse, ETrue);

	CleanupStack::PopAndDestroy( theView );
}



//
// The Dial Out ISP table will have 3 records
//

LOCAL_C CreateDialOutISPTable()
{
	TUint32 id;

	CCommsDbTableView* theView;

	theView = theDb->OpenTableLC(TPtrC(DIAL_OUT_ISP) );

	//
	// Record #1
	//
	theView->InsertRecord( id );

	theView->WriteTextL( TPtrC(COMMDB_NAME), _L("Record #1") ) ;
	theView->WriteTextL( TPtrC(ISP_DESCRIPTION), _L("Isp #1") );
	theView->WriteUintL( TPtrC(ISP_TYPE),EIspTypeWAPOnly);
	theView->WriteBoolL( TPtrC(ISP_DISPLAY_PCT), ETrue);

	//Must write to these columns as they cannot hold null values
	theView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	theView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	theView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
	theView->WriteTextL(TPtrC(ISP_IF_NAME), _L("PPP") );
	theView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	theView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	theView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	theView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	theView->WriteTextL(TPtrC(ISP_IF_CALLBACK_INFO), _L8("Call me back") );
	theView->PutRecordChanges();


	//
	// Record #2
	//
	theView->InsertRecord( id );

	theView->WriteTextL( TPtrC(COMMDB_NAME), _L("Record #2") ) ;
	theView->WriteUintL( TPtrC(ISP_TYPE),EIspTypeInternetAndWAP);

	//Must write to these columns as they cannot hold null values
	theView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	theView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	theView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
	theView->WriteTextL(TPtrC(ISP_IF_NAME), _L("PPP") );
	theView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	theView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	theView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	theView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	theView->WriteTextL(TPtrC(ISP_IF_CALLBACK_INFO), _L8("Call me back tomorrow") );
	
	theView->PutRecordChanges();
	
	//
	// Record #3
	//
	theView->InsertRecord( id );

	theView->WriteTextL( TPtrC(COMMDB_NAME), _L("Record #3") ) ;
	theView->WriteUintL( TPtrC(ISP_TYPE),EIspTypeInternetAndWAP);

	//Must write to these columns as they cannot hold null values
	theView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	theView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	theView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
	theView->WriteTextL(TPtrC(ISP_IF_NAME), _L("PPP") );
	theView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	theView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	theView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	theView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	theView->WriteTextL(TPtrC(ISP_IF_CALLBACK_INFO), _L8("Call me back") );

	theView->PutRecordChanges();

	CleanupStack::PopAndDestroy( theView );

}


LOCAL_C void MainL() 
    {
	RFs fs;
	test(fs.Connect()==KErrNone);
	
	TUint dummy;
	
	test.Printf(_L("Creating CommDb Unit test database  #1") );
	
	_LIT(KSvDbFileName,"C:\\private\\100012a5\\DBS[10004e1d]CDBV3.DAT");

	if(fs.Att(KSvDbFileName,dummy)==KErrNone)
		{
		_LIT(KDbaseMess,"\nExisting Database File detected\n - Overwrite? [y/n]\n\n");
		test.Printf(KDbaseMess);
		TKeyCode key;
		key=test.Getch();
		if((key=='y')||(key=='Y'))
			{
			// Instantiate Database Server
			RDbs rDbs;
			User::LeaveIfError(rDbs.Connect());
			CleanupClosePushL(rDbs);
			_LIT(KCommDb,"c:cdbv3.dat"); 
			const TUid KCommsDbFileUid = {0x10004e1d};
			test(rDbs.DeleteDatabase(KCommDb, KCommsDbFileUid)==KErrNone);
			CleanupStack::PopAndDestroy(&rDbs);
			}
		else
			User::Leave(KErrNone);
		}
	fs.Close();

	//Create a blank database
	theDb=CCommsDatabase::NewL(EFalse);
	CleanupStack::PushL(theDb);
	test(theDb->BeginTransaction()==KErrNone);


	CreateDialOutISPTable();
	CreateDialInISPTable();


	theDb->CommitTransaction();
	CleanupStack::PopAndDestroy();
	}

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	
	test.Title();
	TRAPD(error,MainL());

	if (error)
		{
		_LIT(KFailMess,"DbApp failed with error = %d");
		test.Printf(KFailMess, error);
		_LIT(KAnyKeyMess," [Press any key]");
		test.Printf(KAnyKeyMess);
		test.Getch();
		}
	else
		{
		_LIT(KCompleteMess,"DbApp Complete OK");
		test.Printf(KCompleteMess);
		}
	_LIT(KPanicMess,"DbApp");
	__ASSERT_ALWAYS(!error,test.Panic(KPanicMess,error));
	test.Close();
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }
