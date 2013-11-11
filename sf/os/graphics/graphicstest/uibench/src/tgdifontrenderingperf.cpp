// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tgdifontrenderingperf.h"

//Number of iterations each test is executed

const TInt KIterations = 5;

//Font file and font file related information

_LIT16(KTestIndicFontFile, "C:\\s60ssb_v500.ttf");
_LIT16(KTestIndicFontFaceName, "Series 60 Sans");

//Test data for Reading and writing simulation usecases

_LIT16(KHindi1,"\x0930\x093C\x094D\x0915\x0915\x094D\x0915\x0930\x094D\x0915\x0915\x094D\x0930\x0915\x094D\x0937\x0915\x094D\x0930\x094D\x092A\x0020\x092E\x094D\x200D\x092E\x093F\x200D");
_LIT16(KHindi2,"\x0930\x093C\x094D\x0915\x0915\x094D\x0915\x0930\x094D\x0915\x0915\x094D\x0930\x0915\x094D\x0937\x0915\x094D\x0930\x094D\x092A\x0020\x092E\x094D\x200D\x092E\x093F\x200D\x0930\x093C\x094D\x0915\x0915\x094D\x0915\x0930\x094D\x0915\x0915\x094D\x0930\x0915\x094D\x0937\x0915\x094D\x0930\x094D\x092A\x0020\x092E\x094D\x200D\x092E\x093F\x200D\x0930\x093C\x094D\x0915\x0915\x094D\x0915\x0930\x094D\x0915\x0915\x094D\x0930\x0915\x094D\x0937\x0915\x094D\x0930\x094D\x092A\x0020\x092E\x094D\x200D\x092E\x093F\x200D\x0930\x093C\x094D\x0915\x0915\x094D\x0915\x0930\x094D\x0915\x0915\x094D\x0930\x0915\x094D\x0937\x0915\x094D\x0930\x094D\x092A\x0020\x092E\x094D\x200D\x092E\x093F\x200D\x0930\x093C\x094D\x0915\x0915\x094D\x0915\x0930\x094D\x0915\x0915\x094D\x0930\x0915\x094D\x0937\x0915\x094D");

_LIT16(KKannada1,"\x0CB0\x0CBC\x0CCD\x0C95\x0C95\x0CCD\x0C95\x0CB0\x0CCD\x0C95\x0C95\x0CCD\x0CB0\x0C95\x0CCD\x0CB\x0C95\x0CCD\x0CB0\x0CCD\x0CAA\x0020\x0CAE\x0CCD\x200D\x0CAE\x0CBF\x200D");
_LIT16(KKannada2,"\x0CB0\x0CBC\x0CCD\x0C95\x0C95\x0CCD\x0C95\x0CB0\x0CCD\x0C95\x0C95\x0CCD\x0CB0\x0C95\x0CCD\x0CB7\x0C95\x0CCD\x0CB0\x0CCD\x0CAA\x0020\x0CAE\x0CCD\x200D\x0CAE\x0CBF\x200D\x0CB0\x0CBC\x0CCD\x0C95\x0C95\x0CCD\x0C95\x0CB0\x0CCD\x0C95\x0C95\x0CCD\x0CB0\x0C95\x0CCD\x0CB7\x0C95\x0CCD\x0CB0\x0CCD\x0CAA\x0020\x0CAE\x0CCD\x200D\x0CAE\x0CBF\x200D\x0CB0\x0CBC\x0CCD\x0C95\x0C95\x0CCD\x0C95\x0CB0\x0CCD\x0C95\x0C95\x0CCD\x0CB0\x0C95\x0CCD\x0CB7\x0C95\x0CCD\x0CB0\x0CCD\x0CAA\x0020\x0CAE\x0CCD\x200D\x0CAE\x0CBF\x200D\x0CB0\x0CBC\x0CCD\x0C95\x0C95\x0CCD\x0C95\x0CB0\x0CCD\x0C95\x0C95\x0CCD\x0CB0\x0C95\x0CCD\x0CB7\x0C95\x0CCD\x0CB0\x0CCD\x0CAA\x0020\x0CAE\x0CCD\x200D\x0CAE\x0CBF\x200D\x0CB0\x0CBC\x0CCD\x0C95\x0C95\x0CCD\x0C95\x0CB0\x0CCD\x0C95\x0C95\x0CCD\x0CB0\x0C95\x0CCD\x0CB7\x0C95\x0CCD");

_LIT16(KGujarati1,"\x0AB0\x0ABC\x0ACD\x0A95\x0A95\x0ACD\x0A95\x0AB0\x0ACD\x0A95\x0A95\x0ACD\x0AB0\x0A95\x0ACD\x0AB7\x0A95\x0ACD\x0AB0\x0ACD\x0AAA\x0020\x0AAE\x0ACD\x200D\x0AAE\x0ABF\x200D");
_LIT16(KGujarati2,"\x0AB0\x0ABC\x0ACD\x0A95\x0A95\x0ACD\x0A95\x0AB0\x0ACD\x0A95\x0A95\x0ACD\x0AB0\x0A95\x0ACD\x0AB7\x0A95\x0ACD\x0AB0\x0ACD\x0AAA\x0020\x0AAE\x0ACD\x200D\x0AAE\x0ABF\x200D\x0AB0\x0ABC\x0ACD\x0A95\x0A95\x0ACD\x0A95\x0AB0\x0ACD\x0A95\x0A95\x0ACD\x0AB0\x0A95\x0ACD\x0AB7\x0A95\x0ACD\x0AB0\x0ACD\x0AAA\x0020\x0AAE\x0ACD\x200D\x0AAE\x0ABF\x200D\x0AB0\x0ABC\x0ACD\x0A95\x0A95\x0ACD\x0A95\x0AB0\x0ACD\x0A95\x0A95\x0ACD\x0AB0\x0A95\x0ACD\x0AB7\x0A95\x0ACD\x0AB0\x0ACD\x0AAA\x0020\x0AAE\x0ACD\x200D\x0AAE\x0ABF\x200D\x0AB0\x0ABC\x0ACD\x0A95\x0A95\x0ACD\x0A95\x0AB0\x0ACD\x0A95\x0A95\x0ACD\x0AB0\x0A95\x0ACD\x0AB7\x0A95\x0ACD\x0AB0\x0ACD\x0AAA\x0020\x0AAE\x0ACD\x200D\x0AAE\x0ABF\x200D\x0AB0\x0ABC\x0ACD\x0A95\x0A95\x0ACD\x0A95\x0AB0\x0ACD\x0A95\x0A95\x0ACD\x0AB0\x0A95\x0ACD\x0AB7\x0A95\x0ACD");

//Testcase name used as one of the parameter for ResultsAnalysisAverageByNumberOfIterations() function.

_LIT16(KHindiLang1,"Hindi Reading");
_LIT16(KHindiLang2,"Hindi Writing");

_LIT16(KKannadaLang1,"Kannada Reading");
_LIT16(KKannadaLang2,"Kannada Writing");

_LIT16(KGujaratiLang1,"Gujarati Reading");
_LIT16(KGujaratiLang2,"Gujarati Writing");

// Conversion of the test data into constant pointer variables

TPtrC ptrHindiReading(KHindi1());
TPtrC ptrHindiWriting(KHindi2());
TPtrC ptrKannadaReading(KKannada1);
TPtrC ptrKannadaWriting(KKannada2);
TPtrC ptrGujaratiReading(KGujarati1);
TPtrC ptrGujaratiWriting(KGujarati2);
TInt err;


LOCAL_C void ResetAndDestroyRImplInfoPtrArray(TAny* aPtr)
	{
	RImplInfoPtrArray* array = reinterpret_cast <RImplInfoPtrArray*> (aPtr);
	array->ResetAndDestroy();
	}

LOCAL_C void LoadOpenFontLibrariesL(CFontStore* aFontStore)
	{
	RImplInfoPtrArray implementationArray;
	TCleanupItem cleanup(ResetAndDestroyRImplInfoPtrArray, &implementationArray);
	CleanupStack::PushL(cleanup);
	TInt error;
	TInt ecomerror;
	TInt ecomnotready;
	TUid uid = {KUidOpenFontRasterizerPlunginInterface};

	// Making sure that no race situation arises
	// If ECom is not ready, give it another chance and try again. if it still doesn't work 
	// after the third try, then it just carries on quietly and fails... 
	for (ecomnotready =0; ecomnotready <3; ecomnotready++)
		{
		TRAP(ecomerror,REComSession::ListImplementationsL(uid,implementationArray));
		if (ecomerror != KErrNone)
			{
			break;
			}
		}

	const TInt availCount = implementationArray.Count();
	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = implementationArray[count];
		TUid rasterizerUid = info->ImplementationUid();
 		// Create a rasterizer
 		COpenFontRasterizer* rasterizer= NULL;
 		rasterizer = COpenFontRasterizer::NewL(rasterizerUid);
 		CleanupStack::PushL(rasterizer); 
 		// Install it in the font store.
   		TRAP(error,aFontStore->InstallRasterizerL(rasterizer));
   		if (error)
   			{
   			CleanupStack::PopAndDestroy(rasterizer);
   			}
   		CleanupStack::Pop(rasterizer);	
		}

	CleanupStack::PopAndDestroy(&implementationArray); 
	}

CTGDIFontRenderingPerf::CTGDIFontRenderingPerf()
	{
	SetTestStepName(KTGDIFontRenderingPerfName);
	}

CTGDIFontRenderingPerf::~CTGDIFontRenderingPerf()
	{

	}

TVerdict CTGDIFontRenderingPerf::doTestStepPreambleL() 
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	return TestStepResult();
	}
	
/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTGDIFontRenderingPerf::doTestStepL()
	{
  	GCP2IndicPerformanceTestL();
	return TestStepResult();
	}
	
TVerdict CTGDIFontRenderingPerf::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CTGDIFontRenderingPerf::GCP2IndicPerformanceTestL()
{
/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0079
@SYMTestCaseDesc GDI performance testing for Hindi
@SYMTestPriority High
@SYMTestActions  Attempt to simulate Reading of the Hindi text
@SYMTestExpectedResults Time taken by GetCharacterPositon2() API to process the Hindi unicode values when all the values are passed once.
@SYMPREQ 19: Performance testing for Ogmha
*/

	SetTestStepID(_L("GRAPHICS-UI-BENCH-0079"));
	GCP2ReadingTextPerfL(ptrHindiReading,KHindiLang1);
	RecordTestResultL();
		
/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0080
@SYMTestCaseDesc GDI Performace testing for Hindi
@SYMTestPriority High
@SYMTestActions  Attempt to compose Simulate writing Hindi Text.
@SYMTestExpectedResults Time taken by GetCharacterPositon2() API to process the Hindi unicode values when each value is given at once
@SYMPREQ 19: Performance testing for Ogmha
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0080"));
	GCP2TypingTextPerfL(ptrHindiWriting,KHindiLang2);
	RecordTestResultL();

	/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0081
@SYMTestCaseDesc GDI Performace testing for Kannada
@SYMTestPriority High
@SYMTestActions  Attempt to compose Simulate Reading Kannada Text.
@SYMTestExpectedResults Time taken by GetCharacterPositon2() API to process the Kannada unicode values when all values are given at once
@SYMPREQ 19: Performance testing for Ogmha
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0081"));
	GCP2ReadingTextPerfL(ptrKannadaReading,KKannadaLang1);
	RecordTestResultL();

/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0082
@SYMTestCaseDesc GDI Performace testing for Kannada
@SYMTestPriority High
@SYMTestActions  Attempt to compose Simulate writing Kannada Text.
@SYMTestExpectedResults Time taken by GetCharacterPositon2() API to process the Kannada unicode values when single value is given at once
@SYMPREQ 19: Performance testing for Ogmha
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0082"));
	GCP2TypingTextPerfL(ptrKannadaWriting,KKannadaLang2);
	RecordTestResultL();
		
/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0083
@SYMTestCaseDesc  GDI Performace testing for Gujarati
@SYMTestPriority High
@SYMTestActions  Attempt to compose Simulate Reading Gujarati Text.
@SYMTestExpectedResults Time taken by GetCharacterPositon2() API to process the Gujarati unicode values when all values are given at once
@SYMPREQ 19: Performance testing for Ogmha
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0083"));
	GCP2ReadingTextPerfL(ptrGujaratiReading,KGujaratiLang1);
	RecordTestResultL();
	
/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0084
@SYMTestCaseDesc GDI Performace testing for Gujarati
@SYMTestPriority High
@SYMTestActions  Attempt to compose Simulate writing Gujarati Text.
@SYMTestExpectedResults Time taken by GetCharacterPositon2() API to process the Gujarati unicode values when each value is given at once
@SYMPREQ 19: Performance testing for Ogmha
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0084"));
	GCP2TypingTextPerfL(ptrGujaratiWriting,KGujaratiLang2);
	RecordTestResultL();
	
	
	REComSession::FinalClose();	
}

/*
This function is simulation of the Reading a document 
@param aPtr is the Testdata ie. unicode values for each language which are passed as input to GetCharacterPosition2()
@param aTestCaseName is the name of the test case which is used as one of the parameter to ResultsAnalysisAverageByNumberOfIterations() 
*/
void CTGDIFontRenderingPerf::GCP2ReadingTextPerfL(TPtrC aPtr,const TDesC& aTestCaseName )
	{
	//Setting up the required Font
	CFontStore* fontStore = CFontStore::NewL(&User::Heap());
	CleanupStack::PushL(fontStore);
	LoadOpenFontLibrariesL(fontStore);
	//Adding any required font files
	TUid fontFileUid = fontStore->AddFileL(KTestIndicFontFile);
	TFontSpec testFontSpec(KTestIndicFontFaceName,15); 
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	CFbsFont* font = NULL;
	ts->GetNearestFontToDesignHeightInTwips((CFont* &)font,testFontSpec);
	iTestText.SetLength(aPtr.Length());
	for(TInt i = 0; i < aPtr.Length(); i++)
	{
		iTestText[i]=aPtr[i];		
	}
	// Do the testing
	iParam.iDirection = CFont::EHorizontal;
	iParam.iFlags = CFont::TPositionParam::EFLogicalOrder;
	RShapeInfo  shapeInfo;
	iParam.iText.Set(iTestText);
	iProfiler->InitResults();
	for(TInt j = 0; j<KIterations ;j++)
		{
		iParam.iPosInText = 0;	
		
		while (iParam.iPosInText < (iTestText.Length()))
			{
			iParam.iPen.iX = iParam.iPen.iY = 0;
			font->GetCharacterPosition2(iParam,shapeInfo);
			}
		shapeInfo.Close();
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysisAverageByNumberOfIterations(aTestCaseName,0,0,0,KIterations);
	//Tidying Up
	CleanupStack::PopAndDestroy(ts);
	fontStore->RemoveFile(fontFileUid);
	CleanupStack::PopAndDestroy(fontStore);
	
	}
	
/*
This function is simulation of the Typing 
@param aPtr is the Testdata ie. unicode values for each language which are passed as input to GetCharacterPosition2()
@param aTestCaseName is the name of the test case which is used as one of the parameter to ResultsAnalysisAverageByNumberOfIterations() 
*/
	
void CTGDIFontRenderingPerf::GCP2TypingTextPerfL(TPtrC aPtr,const TDesC& aTestCaseName)
	{
	//Setting up the required Font
	CFontStore* fontStore = CFontStore::NewL(&User::Heap());
	CleanupStack::PushL(fontStore);
	LoadOpenFontLibrariesL(fontStore);
	//Adding any required font files
	TUid fontFileUid = fontStore->AddFileL(KTestIndicFontFile);
	TFontSpec testFontSpec(KTestIndicFontFaceName,15); 
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	CFbsFont* font = NULL;
	ts->GetNearestFontToDesignHeightInTwips((CFont* &)font,testFontSpec);
	iTestText1.SetLength(aPtr.Length());
	for(TInt i = 0; i < aPtr.Length(); i++)
	{
		iTestText1[i]=aPtr[i];
	}
	iParam.iDirection = CFont::EHorizontal;
	iParam.iFlags = CFont::TPositionParam::EFLogicalOrder;
	iProfiler->InitResults();	
	for(TInt j = 0; j < KIterations ;j++)
	{
	TBuf<130> testText3(0);
	for (TInt length = 0; length < (aPtr.Length()); length++)
		{
		testText3.Append(iTestText1[length]);
		iParam.iText.Set(testText3);
		iParam.iPosInText = 0;
		iParam.iPen.iX = iParam.iPen.iY = 0;
		TInt k = testText3.Length();
		iParam.iPen.iX = iParam.iPen.iY = 0;
		TInt prev_pos_inText = iParam.iPosInText;
		for (TInt m =0 ; m<5; m++)
			{
			iParam.iPosInText = prev_pos_inText;
			RShapeInfo  shapeInfo;
			while (iParam.iPosInText <(testText3.Length()))
				{
				iParam.iPen.iX = iParam.iPen.iY = 0;
				font->GetCharacterPosition2(iParam,shapeInfo);
				}
			shapeInfo.Close();
			}
		}
	iProfiler->MarkResultSetL();
	}
	iProfiler->ResultsAnalysisAverageByNumberOfIterations(aTestCaseName,0,0,0,KIterations);
	CleanupStack::PopAndDestroy(ts);
	fontStore->RemoveFile(fontFileUid);
	CleanupStack::PopAndDestroy(fontStore);
	}

	



