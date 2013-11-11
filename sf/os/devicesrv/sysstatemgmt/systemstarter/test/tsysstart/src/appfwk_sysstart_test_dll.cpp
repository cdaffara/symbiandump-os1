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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "appfwk_sysstart_test_dll.h"
#include <e32debug.h>
#include <barsread.h>
#include <f32file.h>
#include <s32file.h>

_LIT(KSysStartTestDllPanic, "SysStartTestDll");

void InitTestDllResults(TTestDllResults& aTestDllResults);
void ReadTestDllResultsFileL(const TDesC& aFileName, TTestDllResults& aTestDllResults);
void ReplaceTestDllResultsFileL(const TDesC& aFileName, TTestDllResults& aTestDllResults);

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects no arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn1(const TDesC8& /* aBuffer */)
	{
	RDebug::Print(_L("TestDll::TDllFn1 - entry"));
	
	// read in the results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn1ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update the call count
	myResults.count++;
	
	// replace the result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn1ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn1 - exit"));
 	return KErrNone;
	}

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 1 argument in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn2(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn2 - entry"));
	
	// read in the results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn2ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in argument value
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn2ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn2 - exit"));
 	return KErrNone;
	}

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 2 arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn3(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn3 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn3ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	myResults.arg2 =  reader.ReadInt16();
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn3ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn3 - exit"));
 	return KErrNone;
	}


/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 3 arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn4(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn4 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn4ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	myResults.arg2 =  reader.ReadInt16();	
	myResults.arg3 =  reader.ReadInt16();
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn4ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn4 - exit"));
 	return KErrNone;
	}

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 2 arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn5(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn5 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn5ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	myResults.arg2 =  reader.ReadInt16();	
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn5ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn5 - exit"));
 	return KErrNone;
	}


/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 1 argument in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn6(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn6 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn6ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn6ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn6 - exit"));
 	return KErrNone;
	}

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects no arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn7(const TDesC8& /* aBuffer */)
	{
	RDebug::Print(_L("TestDll::TDllFn7 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn7ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn7ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn7 - exit"));
 	return KErrGeneral;
	}

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 1 argument in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn8(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn8 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn8ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn8ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn8 - exit"));
 	return KErrNone;
	}


/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 2 arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn9(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn9 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn9ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	myResults.arg2 =  reader.ReadInt16();
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn9ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn9 - exit"));
 	return KErrGeneral;
	}


/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 3 arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn10(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn10 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn10ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// return general error for first call, and then none on retry
	TInt rc=KErrGeneral;
	if (myResults.count==2)
		rc=KErrNone;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	myResults.arg2 =  reader.ReadInt16();	
	myResults.arg3 =  reader.ReadInt16();
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn10ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn10 - exit"));
 	return rc;
	}

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 2 arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn11(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn11 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn11ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	myResults.arg2 =  reader.ReadInt16();	
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn11ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn11 - exit"));
 	return KErrGeneral;
	}

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 1 argument in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn12(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn12 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn12ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn12ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn12 - exit"));
 	return KErrNone;
	}

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 2 arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn13(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn13 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn13ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	myResults.arg2 =  reader.ReadInt16();	
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn13ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn13 - exit"));
 	return KErrNone;
	}

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 2 arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn14(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn14 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn14ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	myResults.arg2 =  reader.ReadInt16();	
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn14ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn14 - exit"));
 	return KErrNone;
	}


/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 3 arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn15(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn15 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn15ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	myResults.arg2 =  reader.ReadInt16();
	myResults.arg3 =  reader.ReadInt16();	
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn15ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn15 - exit"));
 	return KErrNone;
	}

/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 3 arguments in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn16(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn16 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn16ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();	
	myResults.arg2 =  reader.ReadInt16();
	myResults.arg3 =  reader.ReadInt16();	
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn16ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn16 - exit"));
 	return KErrNone;
	}


/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 1 argument in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn17(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn17 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn17ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in arguments
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn17ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn17 - exit"));
 	return KErrNone;
	}


/**
  Retrieves the argument values from the buffer passed to it by System 
  Starter, and updates the result file associated with this function, which
  is read later by the test execute test case where the expected values are
  then compared.
  
  Function expects 1 argument in buffer.
  
  @param aBuffer - contains the argument values passed from System Starter
  */
EXPORT_C TInt TDllFn18(const TDesC8& aBuffer)
	{
	RDebug::Print(_L("TestDll::TDllFn18 - entry"));
	
	// read in results
	TTestDllResults myResults;
	TRAPD(readErr, ReadTestDllResultsFileL(KTDllFn18ResultFileName, myResults));
	if (readErr)
		{
		User::Panic(KSysStartTestDllPanic,readErr);
		}
	
	// update call count
	myResults.count++;
	
	// read in results
	TResourceReader reader;
	reader.SetBuffer(&aBuffer);
  
	myResults.arg1 =  reader.ReadInt16();
	
	// replace result file
	TRAPD(writeErr,ReplaceTestDllResultsFileL(KTDllFn18ResultFileName, myResults));
	if (writeErr)
		{
		User::Panic(KSysStartTestDllPanic,writeErr);
		}
	
	RDebug::Print(_L("TestDll::TDllFn18 - exit"));
 	return KErrNone;
	}

/**
  This function reads in the results contained within the specified result file,
  and populates the struct passed in.
  
  If the result file is not found, then it initialises the struct to the default
  values.
  
  The expected format of the result file is:
  count=value1
  arg1=value2
  arg2=value3
  arg3=value4
  '=' is used as the delimiter.
  
  @param aFileName - the file which contains results
  @param aTestDllResults - struct to be populated with results from file
  */
void ReadTestDllResultsFileL(const TDesC& aFileName, TTestDllResults& aTestDllResults)
	{
	RFs myFs;
	User::LeaveIfError(myFs.Connect());
	
	RFileReadStream reader;
		
	reader.PushL();
	
	TInt err = reader.Open(myFs,aFileName,EFileWrite);
	
	if (err==KErrNotFound)
		{
		InitTestDllResults(aTestDllResults);
		}
	else
		{
		if (err==KErrNone)
			{
				TChar delimChar('=');
				TBuf8<255> aDes;
				reader.ReadL(aDes, delimChar);
				aTestDllResults.count = reader.ReadInt16L();
				
				reader.ReadL(aDes, delimChar);
				aTestDllResults.arg1 = reader.ReadInt16L();
				reader.ReadL(aDes, delimChar);
				aTestDllResults.arg2 = reader.ReadInt16L();
				reader.ReadL(aDes, delimChar);
				aTestDllResults.arg3 = reader.ReadInt16L();
				
				// print out results for debug purposes.
				RDebug::Print(_L("ReadTestDllResultsFileL::aTestDllResults.count - %d"), aTestDllResults.count);
				RDebug::Print(_L("ReadTestDllResultsFileL::aTestDllResults.arg1  - %d"), aTestDllResults.arg1);
				RDebug::Print(_L("ReadTestDllResultsFileL::aTestDllResults.arg2  - %d"), aTestDllResults.arg2);
				RDebug::Print(_L("ReadTestDllResultsFileL::aTestDllResults.arg3  - %d"), aTestDllResults.arg3);
			}
		else
			{
			User::Panic(KSysStartTestDllPanic,err);
			}
		}

	reader.Close();
	CleanupStack::PopAndDestroy(); // reader
	
	myFs.Close();
	}

/**
  Takes a result struct to be initialised to its default values.
  
  @param aTestDllResults - struct to be initialised
  */
void InitTestDllResults(TTestDllResults& aTestDllResults)
	{
	aTestDllResults.count=0;
	aTestDllResults.arg1=0;
	aTestDllResults.arg2=0;
	aTestDllResults.arg3=0;
	}

/**
  This function takes the passed in results and writes to the specified
  result file.
  
  The format of the result file is:
  count=value1
  arg1=value2
  arg2=value3
  arg3=value4
  '=' is used as the delimiter.
  
  @param aFileName - the file to be replaced/written
  @param aTestDllResults - struct which contains results to be written
  */
void ReplaceTestDllResultsFileL(const TDesC& aFileName, TTestDllResults& aTestDllResults)
	{
	RFs myFs;
	User::LeaveIfError(myFs.Connect());
	
	RFileWriteStream writer;
	writer.PushL(); // writer on cleanup stack
	
	User::LeaveIfError(writer.Replace(myFs, aFileName, EFileWrite));
	
	// write data to file
	writer << _L("Call count=");
	writer.WriteInt16L(aTestDllResults.count);
	writer << _L("\n");
	writer << _L("arg1=");
	writer.WriteInt16L(aTestDllResults.arg1);
	writer << _L("\n");
	writer << _L("arg2=");
	writer.WriteInt16L(aTestDllResults.arg2);
	writer << _L("\n");
	writer << _L("arg3=");
	writer.WriteInt16L(aTestDllResults.arg3);
	writer << _L("\n");
	
	// print out results for debug purposes
	RDebug::Print(_L("ReplaceTestDllResultsFileL::aTestDllResults.count - %d"), aTestDllResults.count);
	RDebug::Print(_L("ReplaceTestDllResultsFileL::aTestDllResults.arg1  - %d"), aTestDllResults.arg1);
	RDebug::Print(_L("ReplaceTestDllResultsFileL::aTestDllResults.arg2  - %d"), aTestDllResults.arg2);
	RDebug::Print(_L("ReplaceTestDllResultsFileL::aTestDllResults.arg3  - %d"), aTestDllResults.arg3);
				
	writer.CommitL();
	writer.Close();
	CleanupStack::PopAndDestroy(); // writer
	
	myFs.Close();
	}
