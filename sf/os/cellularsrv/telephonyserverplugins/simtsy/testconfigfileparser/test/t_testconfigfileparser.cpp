// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests classes defined in testconfigfileparser.h
// 
//

/**
 @file t_testconfigfileparser.cpp
 @since Hurricane
 @copyright Copyright (c) 2001 Symbian Ltd.  All rights reserved.
*/

#include <autotest.h>
#include <e32base.h>
#include <e32test.h>		//< for RTest
#include "testconfigfileparser.h"
#include <f32file.h>


_LIT(KTestScriptFile, "t_testconfigfileparser.txt");
_LIT(KTestScriptComponent, "testscripts");
_LIT(KTestScriptComponentFull, "\\testscripts\\");
_LIT(KTestScriptOutput, "c:\\output.txt");

CAutoTest* gAutoTest;
RTest gTest(_L("Test Config File Test"));

void TestGetElementL(TInt aTestCase, const TDesC8& aInput, TChar aDel, TInt aIndex, const TDesC8& aExpected, TBool aTrimOutput=ETrue)
	{
	TPtrC8 item(KNullDesC8);
	const TInt err = CTestConfig::GetElement(aInput, aDel, aIndex, item, aTrimOutput);

	const TInt KPrintfMaxLen = 200;
	TBuf<KPrintfMaxLen> temp;

	if (aInput.Length() <= KPrintfMaxLen)
		{
		temp.Copy(aInput);
		gTest.Printf(_L("aInput=\"%S\" "), &temp);
		}
	else
		gTest.Printf(_L("aInput=TOO LONG "));

	gTest.Printf(_L("aIndex=%d "), aIndex);
	gTest.Printf(_L("aTimeOutput=%d "), aTrimOutput);

	if (aExpected.Length() <= KPrintfMaxLen)
		{
		temp.Copy(aExpected);
		gTest.Printf(_L("aExpected=\"%S\" "), &temp);
		}
	else
		gTest.Printf(_L("aExpected=TOO LONG "));

	if (item.Length() <= KPrintfMaxLen)
		{
		temp.Copy(item);
		gTest.Printf(_L("Get=\"%S\" "), &temp);
		}
	else
		gTest.Printf(_L("Get=TOO LONG "));

	gTest.Printf(_L("Err=%d\n"), err);

	GLOBAL_CHECKPOINT_COMPARE(err, KErrNone, _L("Error getting element"));

	temp.Format(_L("Element not expected [test=%d"), aTestCase);
	GLOBAL_CHECKPOINT_COMPARE(!(item == aExpected), EFalse, temp);
	}

void TestGetElementL(const CTestConfig& aFile)
	{
	TInt sectionCount = aFile.Sections().Count();

	while (sectionCount--)
		{
		const CTestConfigSection& section = aFile[sectionCount];

		TInt itemCount = section.Items().Count();

		while (itemCount--)
			{
			const CTestConfigItem& item = section[itemCount];
			TPtrC8 output(KNullDesC8);
			aFile.GetElement(item.Value(), ',', 0, output);
			}
		}
	}

void TestParseFileL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TFindFile findFile(fs);
	_LIT(KWild, "*.txt");

	CDir* dirs=NULL;
	User::LeaveIfError(findFile.FindWildByDir(KWild, KTestScriptComponentFull, dirs));

	TInt count = dirs->Count();

	while (count--)
		{
		const TFileName fileName((*dirs)[count].iName);

		CTestConfig* file = CTestConfig::NewLC(fs, KTestScriptComponent, fileName); 

		file->WriteFileL(KTestScriptOutput);

		CTestConfig* output = CTestConfig::NewLC(fs, KTestScriptComponent, KTestScriptOutput); 

		TBool test = (*output == *file);

		GLOBAL_CHECKPOINT_COMPARE(!test, EFalse, _L("Output != File"));

		TestGetElementL(*file);

		CleanupStack::PopAndDestroy(output);
		CleanupStack::PopAndDestroy(file);
		}

	CleanupStack::PopAndDestroy(&fs);
	}

const TInt del = ';';
_LIT8(KFirst, "First");
_LIT8(KMiddle, "Middle");
_LIT8(KLast, "Last");
_LIT8(KElem1, "First;Middle;Last");
_LIT8(KElem2, "    First;           ;Last");
_LIT8(KElem3, ";   Middle       ;Last");
_LIT8(KElem4, "First;Middle;     ");
_LIT8(KElem5, ";;");

void TestGetElementL()
	{
	TInt testCase = 0;

	TestGetElementL(++testCase, KElem1, del, 0, KFirst);
	TestGetElementL(++testCase, KElem1, del, 1, KMiddle);
	TestGetElementL(++testCase, KElem1, del, 2, KLast);

	TestGetElementL(++testCase, KElem2, del, 0, KFirst);
	TestGetElementL(++testCase, KElem2, del, 1, KNullDesC8);
	TestGetElementL(++testCase, KElem2, del, 2, KLast);

	TestGetElementL(++testCase, KElem3, del, 0, KNullDesC8);
	TestGetElementL(++testCase, KElem3, del, 1, KMiddle);
	TestGetElementL(++testCase, KElem3, del, 2, KLast);

	TestGetElementL(++testCase, KElem4, del, 0, KFirst);
	TestGetElementL(++testCase, KElem4, del, 1, KMiddle);
	TestGetElementL(++testCase, KElem4, del, 2, KNullDesC8);

	TestGetElementL(++testCase, KElem5, del, 0, KNullDesC8);
	TestGetElementL(++testCase, KElem5, del, 1, KNullDesC8);
	TestGetElementL(++testCase, KElem5, del, 2, KNullDesC8);
	}

void PrepareBuf(TDes8& aOutput, TInt aLeadingSpaces, const TDesC8& aInput, TInt aTrailingSpaces)
	{
	const TInt KSpace = ' ';
	aOutput.Zero();
	aOutput.AppendFill(KSpace, aLeadingSpaces);
	aOutput.Append(aInput);
	aOutput.AppendFill(KSpace, aTrailingSpaces);
	}

void TestGetElementWithoutTrimL()
	{
	TInt testCase = 0;
	TBuf8<32> temp;

	TestGetElementL(++testCase, KElem1, del, 0, KFirst, EFalse);
	TestGetElementL(++testCase, KElem1, del, 1, KMiddle, EFalse);
	TestGetElementL(++testCase, KElem1, del, 2, KLast, EFalse);

	PrepareBuf(temp, 4, KFirst, 0);
	TestGetElementL(++testCase, KElem2, del, 0, temp, EFalse);
	PrepareBuf(temp, 11, KNullDesC8, 0);
	TestGetElementL(++testCase, KElem2, del, 1, temp, EFalse);
	TestGetElementL(++testCase, KElem2, del, 2, KLast, EFalse);

	TestGetElementL(++testCase, KElem3, del, 0, KNullDesC8, EFalse);
	PrepareBuf(temp, 3, KMiddle, 7);
	TestGetElementL(++testCase, KElem3, del, 1, temp, EFalse);
	TestGetElementL(++testCase, KElem3, del, 2, KLast, EFalse);

	TestGetElementL(++testCase, KElem4, del, 0, KFirst, EFalse);
	TestGetElementL(++testCase, KElem4, del, 1, KMiddle, EFalse);
	PrepareBuf(temp, 5, KNullDesC8, 0);
	TestGetElementL(++testCase, KElem4, del, 2, temp, EFalse);

	TestGetElementL(++testCase, KElem5, del, 0, KNullDesC8, EFalse);
	TestGetElementL(++testCase, KElem5, del, 1, KNullDesC8, EFalse);
	TestGetElementL(++testCase, KElem5, del, 2, KNullDesC8, EFalse);

	TPtrC8 output;
	const TInt err = CTestConfig::GetElement(KNullDesC8, 0, 0, output);
	GLOBAL_CHECKPOINT(err != KErrNone);
	}


// Set up the array of test functions, which make the API calls
LOCAL_C const TAutoTestCase Cases[] =
	{
	{TestParseFileL,_S("Parse File")},
	{TestGetElementL,_S("Get Element")},
	{TestGetElementWithoutTrimL,_S("Get Element Without Trim")}
	};

void AutoTestExecuteL()
	{
	_LIT(KDummyTests,"TestConfigFileParser_testreport.txt");
	_LIT8(KComponentInfo,"TestConfigFileParser");
	// Library defined macro
	// Uses a global pointer instance of CAutoTest
	GLOBAL_AUTOTEST_EXECUTE(Cases, KDummyTests, KComponentInfo, gTest);
	}

TInt E32Main()
/**
 * Main
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();

	__UHEAP_MARK;

	gTest.SetLogged(ETrue);
	gTest.Title();
	gTest.Start(_L("Starting tests..."));

	TRAPD(err, AutoTestExecuteL());

	if (err!=KErrNone)
		gTest.Printf(_L("ERROR: Leave %d\n"),err);

	gTest.Printf(_L("Test Complete. Press any key ...\n"));
	gTest.Getch();
	gTest.End();
	gTest.Close();

	__UHEAP_MARKEND;

	delete cleanup;
	return KErrNone;
	}
