// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code
*/

#include <test/graphicsfontutils.h>
#include <graphics/fbsglyphmetricsarray.h> 
#include <graphics/fbsglyphdataiterator.h>
#include "tfbsglyphdatapanic.h"

_LIT(KTypefaceName, "DejaVu Sans Condensed");
const TInt KNumGlyphCodes = 96;

CTFbsGlyphDataPanic::CTFbsGlyphDataPanic(CTestStep* aStep):
    CTGraphicsBase(aStep)
    {
    }

CTFbsGlyphDataPanic::~CTFbsGlyphDataPanic()
    {
    iSgDriver.Close();
    if (iTs)
         {
         iTs->ReleaseFont(iFont);
         delete iTs;
         }
    }

void CTFbsGlyphDataPanic::ConstructL()
    {
    User::LeaveIfNull(iFbs = RFbsSession::GetSession());
    User::LeaveIfNull(iTs = (CFbsTypefaceStore*)CFbsTypefaceStore::NewL(NULL));
    User::LeaveIfError(iTs->GetNearestFontToDesignHeightInPixels((CFont*&)iFont, TFontSpec(KTypefaceName, 15)));
    User::LeaveIfError(iSgDriver.Open());
    
    //initialize the array with ASCII characters from 0020 to 007F
    CCharCodeConverter* converter = CCharCodeConverter::NewLC();
    converter->UseFontL(iFont);
    iGlyphCodes = new(ELeave) TUint[KNumGlyphCodes];
    for (TInt i = 0; i < KNumGlyphCodes; ++i)
        {
        TUint asciiCode = i + 0x20; 
        iGlyphCodes[i] = converter->GlyphCodeL(asciiCode);
        }
    CleanupStack::PopAndDestroy(1); // converter
    }

void CTFbsGlyphDataPanic::RunTestCaseL(TInt /*aCurTestCase*/)
    {
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
    TInt testCase = -1;
    _LIT(KCaseNumber, "CaseNumber");
    TEST(iStep->GetIntFromConfig(iStep->ConfigSection(), KCaseNumber, testCase));
    
    switch(testCase)
        {
    case 1:
        TestGlyphDataIteratorNotInitializedNext();
        break;
    case 2:
        TestGlyphDataIteratorNotInitializedImage();
        break;
    case 3:
        TestGlyphDataIteratorNotInitializedMetrics();
        break;
    case 4:
        TestGlyphDataIteratorNotInitializedGlyphCode();
        break;
    case 5:
        TestGlyphDataIteratorClosedNext();
        break;
    case 6:
        TestGlyphDataIteratorClosedImage();
        break;
    case 7:
        TestGlyphDataIteratorClosedMetrics();
        break;
    case 8:
        TestGlyphDataIteratorClosedGlyphCode();
        break;
    case 9:
        TestGlyphDataIteratorDiscardUsedFont();
        break;
    case 10:
        TestGlyphMetricsArrayIndexNegative();
        break;
    case 11:
    	TestGlyphMetricsArrayIndexTooHigh();
        break;
    case 12:
    	TestGlyphDataIteratorOpenFailed();
    	break;
    default:
        break;
        }
    TEST(EFalse); // shouldn't reach this point as the test will panic at the end of each test case function
    
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
    ((CTFbsGlyphDataPanicStep*)iStep)->CloseTMSGraphicsStep();
    TestComplete();     
    ((CTFbsGlyphDataPanicStep*)iStep)->RecordTestResultL();
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0651
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that calling Next() on an unopened iterator will panic.

@SYMTestActions
	Call Next() on an RFbsGlyphDataIterator instance without opening it.

@SYMTestExpectedResults
	The call to Next() should panic with FBSCLI 31.
*/
void CTFbsGlyphDataPanic::TestGlyphDataIteratorNotInitializedNext()
    {
    INFO_PRINTF1(_L("Ensure that Next() operation applied on null iterator will panic"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0651"));
    RFbsGlyphDataIterator iter;
    iter.Next(); //expect to panic here
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0652
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that Image() function applied on an unopened iterator will panic.

@SYMTestActions
	Call Image() on an RFbsGlyphDataIterator instance without opening it.

@SYMTestExpectedResults
	The call to Image() should panic with FBSCLI 31.
*/
void CTFbsGlyphDataPanic::TestGlyphDataIteratorNotInitializedImage()
    {
    INFO_PRINTF1(_L("Ensure that Image() operation applied on null iterator will panic"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0652"));
    RFbsGlyphDataIterator iter;
    iter.Image(); //expect to panic here
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0653
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that Metrics() function applied on null iterator will panic.

@SYMTestActions
	Call Metrics() on an RFbsGlyphDataIterator instance without opening it.

@SYMTestExpectedResults
	The call to Metrics() should panic with FBSCLI 31.
*/
void CTFbsGlyphDataPanic::TestGlyphDataIteratorNotInitializedMetrics()
    {
    INFO_PRINTF1(_L("Ensure that Metrics() operation applied on null iterator will panic"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0653"));
    RFbsGlyphDataIterator iter;
    iter.Metrics(); //expect to panic here
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0654
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that calling GlyphCode() on an unopened iterator will panic.

@SYMTestActions
	Call GlyphCode() on an RFbsGlyphDataIterator instance without opening it.

@SYMTestExpectedResults
	The call to GlyphCode() should panic with FBSCLI 31.
*/
void CTFbsGlyphDataPanic::TestGlyphDataIteratorNotInitializedGlyphCode()
    {
    INFO_PRINTF1(_L("Ensure that GlyphCode() operation applied on null iterator will panic"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0654"));
    RFbsGlyphDataIterator iter;
    iter.GlyphCode(); //expect to panic here
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0655
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that calling Next() on an iterator which has been closed will panic.

@SYMTestActions
	i. Open the glyph data iterator with 1 glyph code.
	ii. Call Next() to attempt to move the iterator past the end of the array of glyph codes.
	iii. Call Close() and repeat ii.

@SYMTestExpectedResults
	The call to Next() should fail with KErrNotFound, signifying the iterator is still open.
	The final call to Next() should panic with FBSCLI 31.
*/
void CTFbsGlyphDataPanic::TestGlyphDataIteratorClosedNext()
    {
    INFO_PRINTF1(_L("Ensure that Next() operation applied on iterator which has been closed will panic"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0655"));
    RFbsGlyphDataIterator iter;
    TESTNOERROR(iter.Open(*iFont, iGlyphCodes, 1));
    TInt iterErr = iter.Next();
    TESTE(iterErr == KErrNotFound, iterErr);
    iter.Close();
    iter.Next(); //expect to panic here
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0656
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that calling Image() on an iterator which has been closed will panic.

@SYMTestActions
	i. Open the glyph data iterator.
	ii. Close the glyph data iterator.
	iii. Call Image() on an RFbsGlyphDataIterator instance which has been closed.

@SYMTestExpectedResults
	The call to Image() should panic with FBSCLI 31.
*/
void CTFbsGlyphDataPanic::TestGlyphDataIteratorClosedImage()
    {
    INFO_PRINTF1(_L("Ensure that Image() operation applied on iterator which has been closed will panic"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0656"));
    RFbsGlyphDataIterator iter;
    OpenAndCloseIterator(iter);
    iter.Image(); //expect to panic here
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0657
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that calling Metrics() on an iterator which has been closed will panic.

@SYMTestActions
	i. Open the glyph data iterator.
	ii. Close the glyph data iterator.
	iii. Call Metrics() on an RFbsGlyphDataIterator instance which has been closed.

@SYMTestExpectedResults
	The call to Metrics() should panic with FBSCLI 31.
*/
void CTFbsGlyphDataPanic::TestGlyphDataIteratorClosedMetrics()
    {
    INFO_PRINTF1(_L("Ensure that Metrics()operation applied on iterator which has been closed will panic"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0657"));
    RFbsGlyphDataIterator iter;
    OpenAndCloseIterator(iter);
    iter.Metrics(); //expect to panic here
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0658
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      In development
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that calling GlyphCode() on an iterator which has been closed will panic.

@SYMTestActions
	i. Open the glyph data iterator.
	ii. Close the glyph data iterator.
	iii. Call GlyphCode() on an RFbsGlyphDataIterator instance which has been closed.

@SYMTestExpectedResults
	The call to GlyphCode() should panic with FBSCLI 31.
*/
void CTFbsGlyphDataPanic::TestGlyphDataIteratorClosedGlyphCode()
    {
    INFO_PRINTF1(_L("Ensure that GlyphCode() operation applied on iterator which has been closed will panic"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0658"));
    RFbsGlyphDataIterator iter;
    OpenAndCloseIterator(iter);
    iter.GlyphCode(); //expect to panic here
    }

// the function opens and then closes an iterator
void CTFbsGlyphDataPanic::OpenAndCloseIterator(RFbsGlyphDataIterator& aIterator) 
    {
    TESTNOERROR(aIterator.Open(*iFont, iGlyphCodes, 1));
    aIterator.Close();
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0647
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that attempting to use an invalid Font handle will cause a panic when iterating through an RFbsGlyphDataIterator. 

@SYMTestActions
	i. Open the glyph data iterator.
	ii. Release the font associated with the iterator.
	iii. Attempt to use invalid Font handle by iterating through the iterator.

@SYMTestExpectedResults
	The call to Next() after the associated Font has been released should cause panic FBSERV -8 when the invalid Font handle is passed to FBServ.
*/
void CTFbsGlyphDataPanic::TestGlyphDataIteratorDiscardUsedFont()
    {
    INFO_PRINTF1(_L("Ensure that attempting to use an invalid Font handle will cause a panic when iterating through an RFbsGlyphDataIterator"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0647"));
    
    RFbsGlyphDataIterator iter;
    TESTNOERROR(iter.Open(*iFont, iGlyphCodes, KNumGlyphCodes));
    iTs->ReleaseFont(iFont);
    
    for (TInt i = 0; i < KNumGlyphCodes; i++)
        {
        //should panic when attempt is made to use invalid Font handle
        iter.Next();
        }

    iter.Close(); //shouldn't reach this line. If for some reason the code doesn't panic test should fail.
    TEST(EFalse);  
    }
    

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0648
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that accessing the glyph metrics entry with negative index will cause a panic.

@SYMTestActions
	i. Retrieve glyph metrics array.
	ii. Try to access glyph metrics entry with a negative index.

@SYMTestExpectedResults
	Should panic with FBSCLI 32.
*/
void CTFbsGlyphDataPanic::TestGlyphMetricsArrayIndexNegative()
    {
    INFO_PRINTF1(_L("Ensure that accessing the glyph metrics entry with negative index will cause a panic"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0648"));
    RFbsGlyphMetricsArray glyphMetricsArray;

    // Retrieve list of metrics
    TESTNOERROR(glyphMetricsArray.Get(*iFont, iGlyphCodes, 2));
    TEST(2 == glyphMetricsArray.Count());
    glyphMetricsArray[-1];
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0649
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that accessing the glyph metrics entry with an index greater than number
	of elements in array will cause a panic.

@SYMTestActions
	i. Retrieve a glyph metrics array with total number of elements equals 2 
	ii. Try to access glyph metrics entry with index equals 2.

@SYMTestExpectedResults
	Should panic with FBSCLI 32.
*/
void CTFbsGlyphDataPanic::TestGlyphMetricsArrayIndexTooHigh()
    {
    INFO_PRINTF1(_L("Ensure that accessing the glyph metrics entry with index greater than number of elemnets in array will cause a panic"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0649"));
    RFbsGlyphMetricsArray glyphMetricsArray;

    // Retrieve list of metrics
    TESTNOERROR(glyphMetricsArray.Get(*iFont, iGlyphCodes, 2));
    TEST(2 == glyphMetricsArray.Count());
    glyphMetricsArray[2]; //expect to panic here as index varies 0..1
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0667
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
	To ensure that if RFbsGlyphDataIterator::Open() fails, it is still in the closed state
	and cannot be used.

@SYMTestActions
	i. Open an RFbsGlyphDataIterator on a font.  
	ii. Attempt to open a second RFbsGlyphDataIterator on the same font.
	ii. Attempt to use the properties of the iterator.

@SYMTestExpectedResults
	The call to GlyphCode() should panic with FBSCLI 31 since the iterator should be in a closed state.
*/
void CTFbsGlyphDataPanic::TestGlyphDataIteratorOpenFailed()
	{
    INFO_PRINTF1(_L("Ensure that if RFbsGlyphDataIterator::Open() fails, it is still in the closed state and cannot be used"));
    ((CTFbsGlyphDataPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0667"));
   
    CFbsFont* bitmapFont = NULL;
    TInt err = iTs->GetNearestFontToDesignHeightInPixels((CFont*&)bitmapFont, TFontSpec(_L("Digital"), 14));
    TESTNOERROR(err);
    TEST(!bitmapFont->IsOpenFont());
    
    RFbsGlyphDataIterator iter;
    err = iter.Open(*bitmapFont, iGlyphCodes, 2);
    TEST(err == KErrNotSupported);
        
    iter.GlyphCode(); // Should panic here as iter should be closed.
    TEST(EFalse);     //shouldn't reach this line. If for some reason the code doesn't panic test should fail.
    
	}
//--------------
__CONSTRUCT_STEP__(FbsGlyphDataPanic)

