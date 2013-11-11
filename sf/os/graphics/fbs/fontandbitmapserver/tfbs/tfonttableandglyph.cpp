// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <gdi.h>
#include <fbs.h> 
#include <e32test.h>
#include <e32property.h>
#include <graphics/openfontconstants.h>
#include <graphics/openfontrasterizer.h>
#include <graphics/gdi/gdiplatapi.h>
#include "fbsmessage.h"
#include "tfonttableandglyph.h"

//global variable
_LIT16(KTestFontFile, "C:\\DejaVuSans.ttf");
_LIT16(KTestFontFaceName, "DejaVu Sans");

_LIT16(KTestFontFile2, "C:\\DejaVuSerif.ttf");
_LIT16(KTestFontFaceName2, "DejaVu Serif");

static void LoadOpenFontLibraries(CFontStore* aFontStore);

LOCAL_C const TInt KFontHeight = 12;
LOCAL_C const TInt KInvalidGlyphId = 0xffff;

CTFontAndGlyph::CTFontAndGlyph(CTestStep* aStep) :
    CTGraphicsBase(aStep),
    iBmp(NULL),
    iBmp2(NULL),
    iDevice(NULL),
    iDevice2(NULL),
    iFont(NULL),
    iFont2(NULL),
    iFontStore(NULL),
    iFontSpec(KTestFontFaceName, KFontHeight),
    iFontSpec2(KTestFontFaceName2, KFontHeight)
    {
        // a null constructor
    }

CTFontAndGlyph::~CTFontAndGlyph()
    {
    CleanEnv();
    }

void CTFontAndGlyph::RunTestCaseL(TInt aCurTestCase)
    {
    ((CTFontAndGlyphStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
    
    switch(aCurTestCase)
        {
    case 1:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4079"));
        INFO_PRINTF1(_L("GetHintedGlyphById"));
        GetHintedGlyphById(); 
        break;
        
    case 2:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4080"));
        INFO_PRINTF1(_L("GetUnHintedGlyphById"));
        GetUnHintedGlyphById();
        break;
               
    case 3:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4081"));
        INFO_PRINTF1(_L("GetHintedGlyphByWrongId"));
        GetHintedGlyphByWrongId();
        break;   
        
    case 4:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4082"));
        INFO_PRINTF1(_L("GetGlyphWithNoMem"));
        GetGlyphWithNoMem();
        break; 
        
    case 5:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4083"));
        INFO_PRINTF1(_L("GetFontTable"));
        GetFontTable();
        break;         
        
        
    case 6:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4084"));
        INFO_PRINTF1(_L("GetFontTableByWrongTag"));
        GetFontTableByWrongTag();
        break;        

    case 7:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4085"));
        INFO_PRINTF1(_L("GetFontTableWithNoMem"));
        GetFontTableWithNoMem();
        break;    
        
    case 8:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4089"));
        INFO_PRINTF1(_L("GlyphOutlineIteratorPanics"));
        GlyphOutlineIteratorPanics();
        break;   
        
    case 9:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4090"));
        INFO_PRINTF1(_L("WDPAndSMPSafeTest"));
        SMPAndWDPSafeTest();
        break;
        
    case 10:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4087"));
        INFO_PRINTF1(_L("TestFbsFontHandleIsZeroForFont"));
        TestFbsFontHandleIsZeroForFont();
        break;  

    case 11:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(_L("TI18N-GDI-CIT-4088"));
        INFO_PRINTF1(_L("TestFbsFontHandleIsZeroForGlyph"));
        TestFbsFontHandleIsZeroForGlyph();
        break;   
        
    case 12:
        ((CTFontAndGlyphStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
        ((CTFontAndGlyphStep*)iStep)->CloseTMSGraphicsStep();
        TestComplete();
        break;
    default:
        INFO_PRINTF1(_L("Invalid test case id found."));
        }    
        ((CTFontAndGlyphStep*)iStep)->RecordTestResultL();
    }


/**
    @SYMTestCaseID    TI18N-GDI-CIT-4083
    @SYMTestCaseDesc    Get font table. 
    @SYMTestActions    Get font table using the new RFontTable class and test
    the return value.
    @SYMTestExpectedResults  Open() returns KErrNone. TableContent returns the
    table content. 
*/
void CTFontAndGlyph::GetFontTable()
    {
    __UHEAP_MARK;
    RFontTable fontTable;
    TInt err = fontTable.Open(*iFont, 0x68656164);  
    TEST(err == KErrNone);
    TUint32* tablePtr = (TUint32*)fontTable.TableContent();     
    TEST(tablePtr != NULL);
    TEST(tablePtr[3] == 0xF53C0F5F);   //magic number is correct
    RFontTable fontTableInCache;
    err = fontTableInCache.Open(*iFont, 0x68656164);  
    TEST(err == KErrNone);
    TUint32* tablePtr2 = (TUint32*)fontTableInCache.TableContent();  
    TEST(tablePtr2[3] == 0xF53C0F5F);
    TEST(tablePtr == tablePtr2);
    
    RFontTable fontTable2;
    err = fontTable2.Open(*iFont2, 0x68656164);  
    TEST(err == KErrNone);
    tablePtr = (TUint32*)fontTable2.TableContent();     
    TEST(tablePtr != NULL);
    TEST(tablePtr[3] == 0xF53C0F5F);   //magic number is correct
    RFontTable fontTableInCache2;
    err = fontTableInCache2.Open(*iFont2, 0x68656164);  
    TEST(err == KErrNone);
    tablePtr2 = (TUint32*)fontTableInCache2.TableContent();  
    TEST(tablePtr2[3] == 0xF53C0F5F);
    TEST(tablePtr == tablePtr2);
    
    fontTableInCache.Close();
    fontTable.Close();
    fontTableInCache2.Close();
    fontTable2.Close();
    __UHEAP_MARKEND;
    }


/**
    @SYMTestCaseID    TI18N-GDI-CIT-4084
    @SYMTestCaseDesc    Get font table, providing an invalid tag. 
    @SYMTestActions    Get font table, providing an invalid tag (0). 
    Test the return value.
    @SYMTestExpectedResults    Open() returns KErrNotFound.
*/
void CTFontAndGlyph::GetFontTableByWrongTag()
    {
    __UHEAP_MARK;
    RFontTable fontTableForWrongID;
    TInt err = fontTableForWrongID.Open(*iFont, 0);  
    fontTableForWrongID.Close(); 
    TEST(KErrNotFound == err); 
    __UHEAP_MARKEND;
    }


/**
    @SYMTestCaseID    TI18N-GDI-CIT-4085
    @SYMTestCaseDesc    Get font table, when system is OOM. 
    @SYMTestActions    Get font table, simulate OOM conditions using debug macros,
     and test return value.
    @SYMTestExpectedResults    Open() returns KErrNoMemory. TableContent() and 
    TableLength() return NULL/0.
*/
void CTFontAndGlyph::GetFontTableWithNoMem()
    {
#ifdef _DEBUG 
    __UHEAP_MARK;
    RFontTable aFontTableForMem;
    RFbsSession *fbsSession = RFbsSession::GetSession();
    fbsSession->SendCommand(EFbsMessSetHeapFail, RFbsSession::EHeapFailTypeHeapMemory, 1);
    TInt err = aFontTableForMem.Open(*iFont, 0x68656164); /* 'head' in ascii (hex) */
    TEST(KErrNoMemory == err);
    TEST(0 == aFontTableForMem.TableLength());
    TEST(0 == aFontTableForMem.TableContent());
    aFontTableForMem.Close();
    fbsSession->SendCommand(EFbsMessSetHeapReset, RFbsSession::EHeapFailTypeHeapMemory);
    __UHEAP_MARKEND;
#else
    INFO_PRINTF1(_L("Skipping test GetFontTableWithNoMem in release mode")); 
#endif
    }


/**
    @SYMTestCaseID    TI18N-GDI-CIT-4082
    @SYMTestCaseDesc    Get glyph outline when system is out of memory.
    @SYMTestActions    Get  glyph outline. Simulate OOM faliures using debug
    macros to check if the program logic is correct.
    @SYMTestExpectedResults    Open() always returns KErrNoMemory.
*/
void CTFontAndGlyph::GetGlyphWithNoMem()
    {
#ifdef _DEBUG 
    __UHEAP_MARK;
    RFbsSession *fbsSession = RFbsSession::GetSession();
    
    TUint glyphIndex[] = {4, 4, KInvalidGlyphId};  
    __UHEAP_FAILNEXT(1);
    RGlyphOutlineIterator it1;    
    TInt err = it1.Open(*iFont, glyphIndex, sizeof(glyphIndex)/sizeof(glyphIndex[0]), ETrue);
    TEST(err == KErrNoMemory);
    it1.Close(); 
    
    __UHEAP_FAILNEXT(2);
    RGlyphOutlineIterator it2;
    err = it2.Open(*iFont, glyphIndex, sizeof(glyphIndex)/sizeof(glyphIndex[0]));
    TEST(err == KErrNoMemory);
    TEST(KErrNotFound == it2.Next());
    it2.Close(); 
    
    __UHEAP_FAILNEXT(3);
    RGlyphOutlineIterator it3;
    err = it3.Open(*iFont, glyphIndex, sizeof(glyphIndex)/sizeof(glyphIndex[0]), ETrue);
    TEST(err == KErrNoMemory);
    TEST(KErrNotFound == it2.Next());
    it3.Close(); 
    
    RGlyphOutlineIterator it4;  
    fbsSession = RFbsSession::GetSession();
    fbsSession->SendCommand(EFbsMessSetHeapFail, RFbsSession::EHeapFailTypeServerMemory, 1);
    err = it4.Open(*iFont, glyphIndex, sizeof(glyphIndex)/sizeof(glyphIndex[0])); 
    TEST(KErrNoMemory == err);
    TEST(KErrNotFound == it2.Next());
    fbsSession->SendCommand(EFbsMessSetHeapReset, RFbsSession::EHeapFailTypeServerMemory);
    it4.Close();
    
    __UHEAP_FAILNEXT(4);
    RGlyphOutlineIterator it5;
    err = it5.Open(*iFont, glyphIndex, sizeof(glyphIndex)/sizeof(glyphIndex[0]));
    TEST(err == KErrNoMemory);
    TEST(KErrNotFound == it2.Next());
    it5.Close();
    __UHEAP_MARKEND;
#else
    INFO_PRINTF1(_L("Skipping test GetGlyphWithNoMem in release mode"));
#endif
    }


/**
    @SYMTestCaseID    TI18N-GDI-CIT-4079
    @SYMTestCaseDesc    Get hinted glyph outline
    @SYMTestActions    Get hinted glyph outline for glyph 4 and 65536. 
    Pass NULL for the 'aCodes' argument, and 0 for the 'aCount' to test 
    param validation. Check return values in each situation.
    @SYMTestExpectedResults    Open() returns KErrNone. Outline() returns a 
    valid pointer to the string representing the outline. OutlineLength() returns
    the corresponding string length.
*/
void CTFontAndGlyph::GetHintedGlyphById()
    {
    __UHEAP_MARK;
    RGlyphOutlineIterator glyphIterator1;
    TUint glyphIndex[] = {4, 4, KInvalidGlyphId};  //two 4 to ensure the 2nd glyph is in cache.
    TInt err = glyphIterator1.Open(*iFont, glyphIndex, sizeof(glyphIndex)/sizeof(glyphIndex[0]), ETrue);
    TEST(err == KErrNone);  
    TInt len1 = glyphIterator1.OutlineLength();   
    const TUint8* ptr1 = glyphIterator1.Outline();
    TInt end = glyphIterator1.Next();
    const TUint8* ptr2 = glyphIterator1.Outline();
    TInt len2= glyphIterator1.OutlineLength(); 
    TEST(len1 > 0);  
    TEST(ptr1 != NULL);
    TEST(ptr1 == ptr2);  
    TEST(len1 == len2); 
        
    RGlyphOutlineIterator glyphIterator1b;  // for font file 2
    TUint glyphIndex_2[] = {0x41, 0x41, KInvalidGlyphId}; 
    err = glyphIterator1b.Open(*iFont2, glyphIndex_2, 
            sizeof(glyphIndex_2)/sizeof(glyphIndex_2[0]), ETrue);
    TEST(err == KErrNone);  
    len1 = glyphIterator1b.OutlineLength();   
    ptr1 = glyphIterator1b.Outline();
    end = glyphIterator1b.Next();
    ptr2 = glyphIterator1b.Outline();
    len2= glyphIterator1b.OutlineLength(); 
    TEST(len1 > 0);  
    TEST(ptr1 != NULL);
    TEST(ptr1 == ptr2);  
    TEST(len1 == len2); 
    
    end = glyphIterator1.Next();    
    len1 = glyphIterator1.OutlineLength(); 
    ptr1 = glyphIterator1.Outline();
    // for the invalid glyph id, len1<0 indicates an error.
    TEST(NULL == ptr1);
    TEST(len1 < 0);
    end = glyphIterator1.Next();
    TEST(KErrNotFound == end);
    // calling Outline() or OutlineLength() causes panic if Next() 
    // returns KErrNotFound
    glyphIterator1.Close();

    end = glyphIterator1b.Next();    
    len1 = glyphIterator1b.OutlineLength(); 
    ptr1 = glyphIterator1b.Outline();
    // for the invalid glyph id, len1<0 indicates an error.
    TEST(NULL == ptr1);
    TEST(len1 < 0);
    end = glyphIterator1b.Next();
    TEST(KErrNotFound == end);
    // calling Outline() or OutlineLength() causes panic if Next() 
    // returns KErrNotFound
    glyphIterator1b.Close();
    
    RGlyphOutlineIterator glyphIterator3;
    err = glyphIterator3.Open(*iFont, glyphIndex, 0, ETrue);
    TEST(KErrArgument == err);
    TEST(KErrNotFound == glyphIterator3.Next());
    glyphIterator3.Close();  
    
    RGlyphOutlineIterator glyphIterator4;
    err = glyphIterator4.Open(*iFont, NULL, sizeof(glyphIndex)/sizeof(glyphIndex[0]), ETrue);
    TEST(KErrArgument == err);
    TEST(KErrNotFound == glyphIterator4.Next());
    glyphIterator4.Close(); 
    __UHEAP_MARKEND;
    }


/**
    @SYMTestCaseID    TI18N-GDI-CIT-4081
    @SYMTestCaseDesc    Get hinted glyph outline, passing in invalid IDs.
    @SYMTestActions    Get hinted glyph outline, passing in invalid IDs. Check 
    the return values.
    @SYMTestExpectedResults    Open() returns KErrNone. Outline() returns NULL.
    OutlineLength() returns KErrGeneral.
*/
void CTFontAndGlyph::GetHintedGlyphByWrongId()
    {
    __UHEAP_MARK;
    RGlyphOutlineIterator glyphIterator;
    TUint glyphIndex[] = {KInvalidGlyphId, KInvalidGlyphId};  
    TInt err = glyphIterator.Open(*iFont, glyphIndex, sizeof(glyphIndex)/sizeof(glyphIndex[0]), ETrue);
    TEST(err == KErrNone);  
    const TUint8* ptr = glyphIterator.Outline();
    TInt len = glyphIterator.OutlineLength();
    TEST(KErrGeneral == len);
    TEST(ptr == NULL);
    TInt ret = glyphIterator.Next();
    TEST(KErrNone == ret);
    glyphIterator.Close();
    __UHEAP_MARKEND;
    }


/**
    @SYMTestCaseID    TI18N-GDI-CIT-4080
    @SYMTestCaseDesc    Get unhinted glyph outline
    @SYMTestActions    Get unhinted glyph outline for glyph 4 and 5. 
    @SYMTestExpectedResults    Open() returns KErrNone. Outline() and 
    OutlineLength() returns the outline string and corresponding length.
*/
void CTFontAndGlyph::GetUnHintedGlyphById()
    {
    __UHEAP_MARK;
    RGlyphOutlineIterator glyphIterator2;
    TUint glyphIndex[] = {4, 4, 5};  //two '4' to ensure the 2nd is in cache.
    TInt err = glyphIterator2.Open(*iFont, glyphIndex, sizeof(glyphIndex)/sizeof(glyphIndex[0]), EFalse);
    TEST(err == KErrNone);   
    const TUint8* ptr1 = glyphIterator2.Outline();
    TInt len1 = glyphIterator2.OutlineLength();     
    err = glyphIterator2.Next();
    TEST(err == KErrNone);
    const TUint8* ptr2 = glyphIterator2.Outline();
    TInt len2= glyphIterator2.OutlineLength(); 
    err = glyphIterator2.Next();
    TEST(err == KErrNone);
    TEST(len1 > 0);  
    TEST(ptr1 == ptr2); 
    TEST(ptr1 != NULL);
    TEST(len1 == len2);      
    
    RGlyphOutlineIterator glyphIterator2b;  // for the 2nd font file
    TUint glyphIndex2[] = {0x41, 0x41, 0x42}; 
    err = glyphIterator2b.Open(*iFont2, glyphIndex2, 
            sizeof(glyphIndex2)/sizeof(glyphIndex2[0]), EFalse);
    TEST(err == KErrNone);   
    ptr1 = glyphIterator2b.Outline();
    len1 = glyphIterator2b.OutlineLength();     
    err = glyphIterator2b.Next();
    TEST(err == KErrNone);
    ptr2 = glyphIterator2b.Outline();
    len2= glyphIterator2b.OutlineLength(); 
    err = glyphIterator2b.Next();
    TEST(err == KErrNone);
    TEST(len1 > 0);  
    TEST(ptr1 == ptr2); 
    TEST(ptr1 != NULL);
    TEST(len1 == len2);
    
    glyphIterator2.Close();
    glyphIterator2b.Close();
    __UHEAP_MARKEND;
    }

/**
    @SYMTestCaseID    TI18N-GDI-CIT-4087
    @SYMTestCaseDesc    Pass invalid font handle to FBS when getting font table.
    @SYMTestActions    Pass invalid font handle to FBS when getting font table.
        And check the return value.
    @SYMTestExpectedResults    Open returns KErrNotFound.
*/
void CTFontAndGlyph::TestFbsFontHandleIsZeroForFont() 
    {
    __UHEAP_MARK;
    CFbsTestForFont* fbsFont = reinterpret_cast<CFbsTestForFont*> (iFont);
    TEST(NULL != fbsFont);
    fbsFont->SetHandle(0);
    TInt handle = fbsFont->Handle();
    CFbsTestForFont tempFsbFont;
    tempFsbFont.Duplicate(handle);
    tempFsbFont.SetHandle(0);    
    RFontTable ft1;
    TInt err = ft1.Open(tempFsbFont, 0x11667730);
    TEST(err == KErrNotFound);
    ft1.Close(); 
    __UHEAP_MARKEND;
    }

/**
    @SYMTestCaseID    TI18N-GDI-CIT-4088
    @SYMTestCaseDesc    Pass invalid font handle to FBS when getting glyph outline.
    @SYMTestActions    Pass invalid font handle to FBS when getting glyph outline.
        And check the return value.
    @SYMTestExpectedResults    Open() returns KErrNotFound.
*/
void CTFontAndGlyph::TestFbsFontHandleIsZeroForGlyph() 
    {
    __UHEAP_MARK;
    CFbsTestForFont* fbsFont = reinterpret_cast<CFbsTestForFont*> (iFont);
    TEST(NULL != fbsFont);
    TInt handle = fbsFont->Handle();
    CFbsTestForFont tempFsbFont;
    tempFsbFont.Duplicate(handle);
    tempFsbFont.SetHandle(0);     
    RGlyphOutlineIterator glyphIterator1;
    TUint glyphIndex[] = {1, 1, KInvalidGlyphId}; 
    TInt err = glyphIterator1.Open(tempFsbFont, glyphIndex, sizeof(glyphIndex)/sizeof(glyphIndex[0]), ETrue);
    TEST(err == KErrNotFound);
    glyphIterator1.Close();    
    __UHEAP_MARKEND;
    }

static TInt OutlinePanicThread(TAny* /* ptr */)
    {   
    CTrapCleanup* trapCleanup=CTrapCleanup::New();
    RGlyphOutlineIterator it;
    it.Outline();
    delete trapCleanup;
    return 0;
    }

static TInt OutlineLengthPanicThread(TAny* /* ptr */)
    {   
    CTrapCleanup* trapCleanup=CTrapCleanup::New();
    RGlyphOutlineIterator it;
    it.OutlineLength();
    delete trapCleanup;
    return 0;
    }

void CTFontAndGlyph::OutlinePanics(TInt (*aThreadFun)(TAny*))
    {   
    __UHEAP_MARK;
    RThread thread;
    TThreadFunction fun(aThreadFun);
    TAny* ptr = NULL;
    thread.Create(_L("RGlyphOutlineIterator"), fun, 0x1000, 0x20000, 0x20000, ptr);
    
    TRequestStatus status;
    thread.Logon(status);
    thread.Resume();
    User::WaitForRequest(status);
    
    TInt err = thread.ExitReason();
    TEST(err == 0);
    TExitCategoryName name = thread.ExitCategory();
    TEST(name == _L("GDI"));    
    thread.Close();
    __UHEAP_MARKEND;
    }


void CTFontAndGlyph::OutlineLengthPanics(TInt (*aThreadFun)(TAny*))
    {   
    __UHEAP_MARK;
    RThread thread;
    TThreadFunction fun(aThreadFun);
    TAny* ptr = NULL;
    thread.Create(_L("RGlyphOutlineLengthIterator"), fun, 0x1000, 0x20000, 0x20000, ptr);
    
    TRequestStatus status;
    thread.Logon(status);
    thread.Resume();  
    User::WaitForRequest(status);
    
    TInt err = thread.ExitReason();
    TEST(err == 0);
    TExitCategoryName name = thread.ExitCategory();
    TEST(name == _L("GDI"));
    thread.Close();
    __UHEAP_MARKEND;
    }

/**
    @SYMTestCaseID    TI18N-GDI-CIT-4089
    @SYMTestCaseDesc    Try to get glyph outline's length and content when 
        the glyph is not found in font file.
    @SYMTestActions    Try to get glyph outline's length and content when 
        the glyph is not found in font file. This should cause a client panic.
    @SYMTestExpectedResults    Both cases exit with ExitCategory 'GDI' and 
    reason code 0.
*/
void CTFontAndGlyph::GlyphOutlineIteratorPanics()
    {
    __UHEAP_MARK;
    OutlinePanics(OutlinePanicThread);
    OutlineLengthPanics(OutlineLengthPanicThread);
    __UHEAP_MARKEND;
    }

void CTFontAndGlyph::ConstructL()
    {    
    INFO_PRINTF1(_L("Calling CTFontAndGlyph::ConstructL()"));
    iFontStore = CFontStore::NewL(&User::Heap());
    LoadOpenFontLibraries(iFontStore);
    iFontStore->iKPixelWidthInTwips = 11860;
    iBmp = new (ELeave) CFbsBitmap;
    User::LeaveIfError(iBmp->Create(TSize(100,100),EGray2));
    iDevice = CFbsBitmapDevice::NewL(iBmp);
    iUid = iFontStore->AddFileL(KTestFontFile);
    TInt err = iDevice->GetNearestFontToMaxHeightInTwips(iFont,iFontSpec,0);
    if (KErrNone != err)
        {
        iFontStore->RemoveFile(iUid);
        User::Leave(err);
        }   
    
    iBmp2 = new (ELeave) CFbsBitmap;  
    User::LeaveIfError(iBmp2->Create(TSize(100,100),EGray2));
    iDevice2 = CFbsBitmapDevice::NewL(iBmp2);
    iUid2 = iFontStore->AddFileL(KTestFontFile2);
    err = iDevice2->GetNearestFontToMaxHeightInTwips(iFont2,iFontSpec2,0);    
    if (KErrNone != err)
        {
        iFontStore->RemoveFile(iUid);
        iFontStore->RemoveFile(iUid2);
        User::Leave(err);
        }
    }

void CTFontAndGlyph::CleanEnv()
    {
    iDevice->ReleaseFont(iFont); 
    delete iDevice;
    delete iBmp;
    iFontStore->RemoveFile(iUid);
    
    iDevice2->ReleaseFont(iFont2); 
    delete iDevice2;
    delete iBmp2;
    iFontStore->RemoveFile(iUid2);
    
    delete iFontStore;
    iDevice = NULL;
    iBmp = NULL;
    iFontStore = NULL;
    
    iDevice2 = NULL;
    iBmp2 = NULL;
    
    REComSession::FinalClose();
    }

//--------------
CTFontAndGlyphStep::CTFontAndGlyphStep()
    {
    SetTestStepName(KTFontTableAndGlyphStep);
    }

CTGraphicsBase* CTFontAndGlyphStep::CreateTestL()
    {
    return new (ELeave) CTFontAndGlyph(this);
    }

static void LoadOpenFontLibraries(CFontStore* aFontStore)
    {

    RImplInfoPtrArray implementationArray;
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
        if (!ecomerror)
            {
            break;
            }
        else
            {
            ecomerror = 0;  
            User::After(0);
            }
        }

    const TInt availCount = implementationArray.Count();
    for (TInt count=0;count<availCount;++count)
        {
        const CImplementationInformation* info = implementationArray[count];
        TUid rasterizerUid = info->ImplementationUid();
        // Create a rasterizer
        COpenFontRasterizer* rasterizer=0;
        TRAP(error,rasterizer = COpenFontRasterizer::NewL(rasterizerUid));
        if (!error)
            {
            // Install it in the font store.
            TRAP(error,aFontStore->InstallRasterizerL(rasterizer));
            if (error)
                {
                RDebug::Printf("tfonttableandglyph: failed to load rasterizer."); 
                delete rasterizer;
                }
            }
        }
    
    implementationArray.ResetAndDestroy();
    }

/**
Worker function that implements the threads executed by SMPAndWDPSafeTest.
@leave If a leave occurs, either the test failed to be set-up successfully,
    or the test itself failed.
*/
static void DoSMPAndWDPSafeTestFunL()
    {
    User::LeaveIfError(RFbsSession::Connect());
    CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bmp);
    User::LeaveIfError(bmp->Create(TSize(100,100), EGray2));
    CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL(bmp);
    CleanupStack::PushL(bmpDevice);
    CFont* font = NULL;
    TFontSpec fontSpec(KTestFontFaceName, KFontHeight);
    User::LeaveIfError(bmpDevice->GetNearestFontToMaxHeightInTwips(font, fontSpec, 0));

    for(TInt i = 0; i < 100; i++)
        {
        RFontTable fontTable;
        CleanupClosePushL(fontTable);
        User::LeaveIfError(fontTable.Open(*font, 0x68656164));
        TUint32* ptr = (TUint32*)fontTable.TableContent();
        if (!ptr)
            {
            User::Leave(KErrGeneral);
            }
        CleanupStack::PopAndDestroy(1); // fontTable;
        }
    bmpDevice->ReleaseFont(font);
    CleanupStack::PopAndDestroy(2); // bmpDevice, bmp
    RFbsSession::Disconnect();
    }

static TInt SMPAndWDPSafeTestFun(TAny* /*aParam*/)
    {
    CTrapCleanup* trapCleanup = CTrapCleanup::New();
    if (!trapCleanup)
        {
        return KErrNoMemory;
        }
    TRAPD(err, DoSMPAndWDPSafeTestFunL());
    delete trapCleanup;
    return err;
    }

/**
    @SYMTestCaseID    TI18N-GDI-CIT-4090
    @SYMTestCaseDesc    Test if the new code is SMP and WDP safe. 
    @SYMTestActions    Create two threads to access the new interfaces concurrently.
    @SYMTestExpectedResults    The threads run to the end correctly. All the
    Open() operations succeed.
*/
void CTFontAndGlyph::SMPAndWDPSafeTest()
    {
    RThread thread1;
    RThread thread2;
    TThreadFunction fun(SMPAndWDPSafeTestFun);
    
    TInt ret1 = thread1.Create(_L("SMPAndWDPSafeThread1"), fun, 0x2000, 0x20000, 0x80000, iFontStore);
	TEST(KErrNone == ret1);
    TInt ret2 = thread2.Create(_L("SMPAndWDPSafeThread2"), fun, 0x2000, 0x20000, 0x80000, iFontStore);
	TEST(KErrNone == ret2);
    
    TRequestStatus status1;
    TRequestStatus status2;
    thread1.Logon(status1);
    thread2.Logon(status2);
    thread1.Resume(); 
    thread2.Resume(); 
    User::WaitForRequest(status1);
    User::WaitForRequest(status2);
    
    TEST(EExitKill == thread1.ExitType());
    TEST(KErrNone == thread1.ExitReason());
    TEST(EExitKill == thread2.ExitType());
    TEST(KErrNone == thread2.ExitReason());
    
    thread1.Close();
    thread2.Close();
    }
