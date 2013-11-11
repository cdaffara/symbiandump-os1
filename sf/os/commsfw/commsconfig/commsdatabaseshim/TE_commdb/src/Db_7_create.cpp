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

// Create a totaly empty database

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



LOCAL_C void MainL() 
    {
	RFs fs;
	test(fs.Connect()==KErrNone);
	
	TUint dummy;
	
	test.Printf(_L("Creating CommDb Unit test database  #7") );

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
	//and close it
	delete theDb;

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
