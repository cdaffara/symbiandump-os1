// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file
*/
#include "TeIniWriteStep.h"

//EPOC includes
#include <test/testexecutelog.h>
#include <es_sock.h>
#include <es_ini.h>


_LIT(KFileName,				"c:\\te_ini.ini");


CTestIniWriteStep::CTestIniWriteStep() : iESockIniData(NULL)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KWriteToIniFile);
	}

CTestIniWriteStep::~CTestIniWriteStep()
/**
 * Destructor
 */
	{
	//delete iESockIniData;
	//iESockIniData = NULL;
	}


TVerdict CTestIniWriteStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini iESockIniData section
 */
	{

	INFO_PRINTF1(_L("In Test Step WriteToIniFile"));

	TInt ret;
	TInt numVal1;
	// Commented out in p4 cl 224639,"CCOVER: REMOVED CODE THAT WOULD PANIC. This test needs to be fixed"
	// TInt numVal2;
	// TPtrC ptrVal2;
	TPtrC ptrVal1;
	TBuf<512> tmpBuf;

	iESockIniData = CESockIniData::NewL(KFileName);

	// Write with DataSource2
	INFO_PRINTF1(_L("Write DataSource2 (medium) to WriteData."));
	TRAP(ret, iESockIniData->FindVar(_L("DataSource2"), _L("Number"), numVal1));
	TRAP(ret, iESockIniData->FindVar(_L("DataSource2"), _L("String"), ptrVal1));
	tmpBuf.Copy(ptrVal1);
	// Commented out in p4 cl 224639
	// test(file->WriteVar(_L("WriteData"), _L("Number"), numVal1)==KErrNone); 
 	// test(file->WriteVar(_L("WriteData"), _L("String"), tmpBuf)==KErrNone);
	TRAP(ret, iESockIniData->CommitL());
	if (ret != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to Commit DataSource2. Error: %d"), ret);
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Succeeded to Commit DataSource2 "));
		}
	delete iESockIniData;
	iESockIniData = NULL;

	// Commented out in p4 cl 224639
	// Re-open file and confirm write
	// test.Printf(_L("Reopen and confirm\r\n"));
	// TRAP(ret,file=CESockIniData::NewL(KFilename));
	// test(ret==KErrNone);
	// test(file->FindVar(_L("WriteData"), _L("Number"), numVal2));
	// test(file->FindVar(_L("WriteData"), _L("String"), ptrVal2));
	// test(numVal1==numVal2);
	// test(tmpBuf.Compare(ptrVal2)==0);
	// delete file;


	// Write with DataSource1
	INFO_PRINTF1(_L("Write DataSource1 (shorter) to WriteData."));
	TRAP(ret,iESockIniData = CESockIniData::NewL(KFileName));
	// Commented out in p4 cl 224639
	// test(file->FindVar(_L("DataSource1"), _L("Number"), numVal1));
	// test(file->FindVar(_L("DataSource1"), _L("String"), ptrVal1));
	// tmpBuf.Copy(ptrVal1);
	// test(file->WriteVar(_L("WriteData"), _L("Number"), numVal1)==KErrNone);
	// test(file->WriteVar(_L("WriteData"), _L("String"), tmpBuf)==KErrNone);
	TRAP(ret, iESockIniData->CommitL());
	if (ret != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to Commit DataSource1. Error: %d"), ret);
		SetTestStepResult(EFail);
		return EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Succeeded to Commit DataSource1. "));
		}
	delete iESockIniData;
	iESockIniData = NULL;

	// Commented out in p4 cl 224639
/*  Re-open file and confirm write
	test.Printf(_L("Reopen and confirm\r\n"));
	TRAP(ret,file=CESockIniData::NewL(KFilename));
	test(ret==KErrNone);
	test(file->FindVar(_L("WriteData"), _L("Number"), numVal2));
	test(file->FindVar(_L("WriteData"), _L("String"), ptrVal2));
	test(numVal1==numVal2);
	test(tmpBuf.Compare(ptrVal2)==0);
	delete file;
	
	// Write with DataSource2
	test.Printf(_L("Write DataSource3 (longer) to WriteData\r\n"));
	TRAP(ret,file=CESockIniData::NewL(KFilename));
	test(ret==KErrNone);
	test(file->FindVar(_L("DataSource3"), _L("Number"), numVal1));
	test(file->FindVar(_L("DataSource3"), _L("String"), ptrVal1));
	tmpBuf.Copy(ptrVal1);
//	test(file->WriteVar(_L("WriteData"), _L("Number"), numVal1)==KErrNone);
//	test(file->WriteVar(_L("WriteData"), _L("String"), tmpBuf)==KErrNone);
	TRAP(ret, file->CommitL());
	test(ret==KErrNone);
	delete file;

	// Re-open file and confirm write
	test.Printf(_L("Reopen and confirm\r\n"));
	TRAP(ret,file=CESockIniData::NewL(KFilename));
	test(ret==KErrNone);
	test(file->FindVar(_L("WriteData"), _L("Number"), numVal2));
	test(file->FindVar(_L("WriteData"), _L("String"), ptrVal2));
	test(numVal1==numVal2);
//	test(tmpBuf.Compare(ptrVal2)==0);
	delete file;*/
	
	return TestStepResult();
	}



