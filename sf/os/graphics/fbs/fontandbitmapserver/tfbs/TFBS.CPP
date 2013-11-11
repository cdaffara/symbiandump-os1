// Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TFBS.H"
#include "../sfbs/UTILS.H"
#include "../sfbs/fbshelper.h"
#include <shapeinfo.h>
#include <graphics/openfontrasterizer.h>
#include <graphics/openfontconstants.h>
#include "fbsdefs.h"
#include "fbsmessage.h"

TInt buffer[2];

_LIT(KFBSERVFontFileDir, "\\resource\\fonts\\");
_LIT(KTypefaceName, "DejaVu Sans Condensed");

// TLS Handle for TestFlushCallback test, from tfbsserver.mmp
const TInt KTlsHandle = 0x10273364;


#ifdef __WINS__
template<class C> XTCallCounter<C>::XTCallCounter(CTGraphicsBase& aTestBase)
	: iTestBase(aTestBase), iVFCallsOutsideFBServ(0)
	{}

template<class C> void XTCallCounter<C>::ExecuteShellcode(TInt aFromFunction)
	{
	const TUint32 KFBServId = 0x10003A16;
	const TUint32 KTFbsServerId = 0x10273364;
	TSecureId id = RProcess().SecureId();
	if (id != KFBServId)
		{
		User::LockedInc(iVFCallsOutsideFBServ);
		if (id == KTFbsServerId)
			{
			TBuf<128> name;
			XVtableInjector<C>::GetVirtualFunctionName(aFromFunction, name);
			iTestBase.INFO_PRINTF2(_L("Virtual function call to %S from outside FBServ"), &name);
			}
		}
	}
#endif


void CTFbs::DeleteScanLineBuffer()
	{
	delete iFbs->iScanLineBuffer;
	iFbs->iScanLineBuffer = 0;
	};

CTFbs::CTFbs(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iHandle(0),
	iFbs(NULL),
	iTs(NULL),
	iHeap(NULL),
	iFs(NULL),
	iResourceCount(0),
	iAllocs(0)
#ifdef __WINS__
	, iFontCallCounter(*this), iOpenFontCallCounter(*this)
#endif
	{
	}

void CTFbs::ConstructL()
	{
	ExpandCleanupStackL();
	User::LeaveIfError(Logger().ShareAuto());
	TestConstruction();
	AllocScanLineBuf();
	
	INFO_PRINTF1(_L("FBSERV testing"));
	}
	
CTFbs::~CTFbs()
	{
	delete iTs;
	}

void CTFbs::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFbsStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0555"));
		TestConnection();
		break;
	case 2:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0558"));
		TestInvalidFiles();
		break;
	case 3:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0556"));
		TestFlushCallBack();
		break;
	case 4:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0568"));
		TestMultiThread();
		break;
	case 5:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0559"));
		TestGetFont();
		break;
	case 6:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0560"));
		TestFontStore();
		break;
	case 7:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-PREQ807_1_Load_all_fonts-0001"));
		TestAllFontsLoaded();
		break;
	case 8:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0561"));
		TestFontNameAlias();
		break;
	case 9:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0557"));
		TestAddFontFile();
		break;
	case 10:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0562"));
		TestBufferedFont();
		break;
	case 11:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0563"));
		TestMultipleFont();
		break;
	case 12:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0564"));
		TestCreateBitmap();
		break;
	case 13:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0565"));
		TestLoadBitmap();
		break;
	case 14:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0566"));
		TestQueryBitmap();
		TRAPD(errCode, TestRomBitmapL());
		if(errCode==KErrNone)
			{
			TestHeapCompression();
			}
		break;
	case 15:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0570"));
		TestDefaultLanguageForMetrics();		
		break;
	case 16:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-CTFbs-TestDuplicateFontFileEntries-0001"));
		TestDuplicateFontFileEntries();
		break;
	//COMMENTED OUT FOR NOW BECAUSE DEF084095 FIX MAKES THIS TEST REDUNDANT
	/*case 17:
		TestShapeHeaderMemoryLeakAtClientDeath();
		break;*/
	case 17:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0506"));
		TestBitmapHandleImmutable();
		break;
	case 18:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0507"));
		TestBitmapBeginEnd();
		break;
	case 19:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0508"));
		TestSingletonServer();
		break;
	case 21:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0623"));
		TestFontSessionCacheLookupL();
		break;
	case 22:
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0650"));
		TestInvalidHandlesInIpcCallsL();
		break;
	case 23:
		// this one should always be the last test, since it checks the final virtual function call count
		((CTFbsStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0619"));
		TestNoVFCallsToGlobalObjects();
		break;
	case 24:
		((CTFbsStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTFbsStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	((CTFbsStep*)iStep)->RecordTestResultL();
	}

CFbsFontEx* CTFbs::SelectFont()
	{
	CFbsFontEx* font=NULL;
	TInt ret=iTs->GetNearestFontToDesignHeightInPixels((CFont*&)font, TFontSpec(KTypefaceName, 15));
	TEST(ret==KErrNone);
#ifdef __WINS__
	CBitmapFont* bitmapFont = font->Address();
	iFontCallCounter.iInjector.InjectShellcode(bitmapFont, &iFontCallCounter);
	if (bitmapFont->IsOpenFont())
		{
		iOpenFontCallCounter.iInjector.InjectShellcode(bitmapFont->OpenFont(), &iOpenFontCallCounter);
		}
#endif
	TEST(font->MaxCharWidthInPixels()>0);
	TEST(font->MaxNormalCharWidthInPixels()>0);
	return(font);
	}

CFbsFontEx* CTFbs::SelectOpenTypeFont()
	{
	CFbsFontEx* font=NULL;
	TFontSpec fsp;
	fsp.iTypeface.iName=_L("Series 60 Sans");
	fsp.iHeight=15;
	TInt ret=iTs->GetNearestFontToDesignHeightInPixels((CFont*&)font,fsp);
	TEST(ret==KErrNone);
#ifdef __WINS__
	CBitmapFont* bitmapFont = font->Address();
	iFontCallCounter.iInjector.InjectShellcode(bitmapFont, &iFontCallCounter);
	if (bitmapFont->IsOpenFont())
		{
		iOpenFontCallCounter.iInjector.InjectShellcode(bitmapFont->OpenFont(), &iOpenFontCallCounter);
		}
#endif
	TEST(font->MaxCharWidthInPixels()>0);
	TEST(font->MaxNormalCharWidthInPixels()>0);
	return(font);
	}

void CTFbs::TestConstruction()
	{
	CFbsBitmap bmp;
	iTestBitmapName = KTestBitmapOnZ;
	TInt ret=bmp.Load(iTestBitmapName,ETfbs,NULL);
	if (ret != KErrNone)
		User::Panic(_L("Could not load bitmap"),ret);
	bmp.Reset();

	iFbs = RFbsSession::GetSession();
	TRAP(ret,iTs = (CFbsTypefaceStore*)CFbsTypefaceStore::NewL(NULL));
	TEST(iTs != NULL);
	CFbsFontEx* font = SelectFont();
	iTs->ReleaseFont(font);
	}
	
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0555

	@SYMTestCaseDesc
	Tests the connections to the FbsSession

	@SYMTestActions
	Allocates a 100 RFbsSession objects and
	connects to the server and checks if
	all connections succeeded. Frees the memory
	and checks the heap memory.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestConnection()
	{
	INFO_PRINTF1(_L("class RFbsSession"));
	__UHEAP_MARK;
	const TInt numconnects=100;
	RFbsSession* connections=(RFbsSession*)User::Alloc(sizeof(RFbsSession)*numconnects);
	TEST(connections!=NULL);
	TInt count=0;
	for(;count<numconnects;count++)
		new(&connections[count]) RFbsSession;
	TEST(iFbs->ResourceCount()==0);
	for(count=0;count<numconnects;count++)
		{
		TInt ret=connections[count].Connect();
		if(ret!=KErrNone)
			break;
		}
	INFO_PRINTF2(_L("    %d connections created.\n"),count);
	TEST(iFbs->ResourceCount()==0);
	for(TInt count2=0;count2<count;count2++)
		connections[count2].Disconnect();
	TEST(iFbs->ResourceCount()==0);
	User::Free(connections);
	User::Heap().Check();
	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0556

	@SYMTestCaseDesc
	Tests if the flush callback function
	is called properly.

	@SYMTestActions
	Sets the flush callback, fills the cache with different
	fonts until the cache is full.
	Create one more font which forces an eviction of a font
	in the cache.
	Check call back is called.
	Reset the flush callback, create another new font,
	check that the callback was not called.
	Check for memory and resource leaks.
	
	@SYMTestExpectedResults
	Callback function should be called once at end of loop.
*/
void CTFbs::TestFlushCallBack()
	{
	INFO_PRINTF1(_L("Test Flush CallBack"));

	__UHEAP_MARK;
	CFbsTypefaceStore* tfs=NULL;
	TRAPD(ret,tfs=CFbsTypefaceStore::NewL(NULL));
	TEST(ret==KErrNone);
	TEST(tfs!=NULL);

	UserSvr::DllSetTls(KTlsHandle, this);
	INFO_PRINTF1(_L("Testing Flush CallBack mechanism..."));
	iFbs->SetCallBack(TCallBack(FlushCallBack, NULL));
	CFbsFontEx* font=NULL;

	// Fill up the fontcache so that on the final iteration, the first 
	// font is evicted, thereby destroying it server-side and executing the 
	// callback.
	iFlushCallbackReceived = EFalse;
	TInt fontSize = 10;
	for (TInt fontEntries = 0; fontEntries < KMaxFontCacheEntries+1; fontEntries++)
		{
		ret=tfs->GetNearestFontToDesignHeightInTwips((CFont*&)font, TFontSpec(KTypefaceName, fontSize));
		TEST(ret==KErrNone);
		iResourceCount++;
		CheckResourceCount();
		tfs->ReleaseFont(font);
		fontSize += 15;
		}
	TEST(iFlushCallbackReceived);

	// Now check callback is correctly reset and callback function is not executed
	// when another font is evicted from the cache.
	// Resource count wont increase as although a new font is being created, another
	// will be evicted from the cache.
	iFlushCallbackReceived = EFalse;
	iFbs->ResetCallBack();
	ret=tfs->GetNearestFontToDesignHeightInTwips((CFont*&)font, TFontSpec(KTypefaceName, fontSize));
	TEST(ret==KErrNone);
	tfs->ReleaseFont(font);
	TEST(!iFlushCallbackReceived);
	CheckResourceCount();

	UserSvr::DllFreeTls(KTlsHandle);
	delete tfs;
	iResourceCount = 0;
	CheckResourceCount();
	__UHEAP_MARKEND;
	}

TInt CTFbs::FlushCallBack(TAny* /*aPtr*/)
	{
	CTFbs* tfbs = static_cast<CTFbs*>(UserSvr::DllTls(KTlsHandle));
	tfbs->INFO_PRINTF1(_L("    ...Flush CallBack called successfully"));
	tfbs->iFlushCallbackReceived = ETrue;
	--tfbs->iResourceCount;
	return(0);
	}


#ifdef __WINS__
_LIT(KCTFbsTestFont,"z:\\resource\\fonts\\eon14.gdr");
#else
_LIT(KCTFbsTestFont,"z:\\resource\\fonts\\eon.gdr");
#endif

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0557

	@SYMTestCaseDesc
	Tests adding of a font file
	to the font store

	@SYMTestActions
	Creates a type face store and adds
	font files to it. Checks if adding
	the files succeeded. Removes the files
	from the store. Checks if succeeded.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestAddFontFile()
	{
	INFO_PRINTF1(_L("Test Add File"));
	__UHEAP_MARK;
	CFbsTypefaceStore* tfs=NULL;
	TRAPD(ret,tfs=CFbsTypefaceStore::NewL(NULL));
	TEST(ret==KErrNone);
	TEST(tfs!=NULL);
	TInt id1=0;
	TInt id2=0;
	TInt id3=0;
	TInt id4=0;
	ret=tfs->AddFile(KCTFbsTestFont,id1);
	TEST(ret==KErrNone);
	TEST(id1!=0);
	ret=tfs->AddFile(KCTFbsTestFont,id2);
	TEST(ret==KErrNone);
	TEST(id2==id1);
	ret=tfs->AddFile(KCTFbsTestFont,id3);
	TEST(ret==KErrNone);
	TEST(id3==id2);
	tfs->RemoveFile(id1);
	tfs->RemoveFile(id2);
	tfs->RemoveFile(id3);
	ret=tfs->AddFile(KCTFbsTestFont,id4);
	TEST(ret==KErrNone);
	TEST(id4==id1);
	tfs->RemoveFile(id4);
	delete tfs;
	__UHEAP_MARKEND;
	
	//test the sequence AddFile, GetNearestFont, ReleaseFont, RemoveFile
	CFbsTypefaceStore* tfs1=NULL;
    TRAPD(ret1,tfs1=CFbsTypefaceStore::NewL(NULL));
    TEST(ret1==KErrNone);
    TEST(tfs1!=NULL);
    TInt id5=0;
    ret1=tfs1->AddFile(_L("z:\\resource\\fonts\\DejaVuSerifCondensed.ttf"),id5);
    TEST(ret1==KErrNone);
    TEST(id5!=0);
    
    TInt id6=0;
    ret1=tfs1->AddFile(_L("z:\\resource\\fonts\\DejaVuSansCondensed.ttf"), id6);
    TEST(ret1==KErrNone);
    TEST(id6!=0);    
    
    TFontSpec fsp1;
    fsp1.iTypeface.iName=_L("DejaVu Serif Condensed");
    fsp1.iHeight=15;
    CFont* font1;
    ret1=tfs1->GetNearestFontToDesignHeightInTwips((CFont*&)font1,fsp1);
    TEST(ret1==KErrNone);
    
    TFontSpec fsp2;
    fsp2.iTypeface.iName=_L("DejaVu Serif Condensed");
    fsp2.iHeight=30;
    CFont *font2;
    ret1=tfs1->GetNearestFontToDesignHeightInTwips((CFont*&)font2,fsp2);
    TEST(ret1==KErrNone);

    TFontSpec fsp3;
    fsp3.iTypeface.iName=_L("DejaVu Sans Condensed");
    fsp3.iHeight=20;
    CFont *font3;
    ret1=tfs1->GetNearestFontToDesignHeightInTwips((CFont*&)font3,fsp3);
    TEST(ret1==KErrNone);

    TFontSpec fsp4;
    fsp4.iTypeface.iName=_L("DejaVu Serif Condensed");
    fsp4.iHeight=35;
    CFont *font4;
    ret1=tfs1->GetNearestFontToDesignHeightInTwips((CFont*&)font4,fsp4);
    TEST(ret1==KErrNone);
    
    if (font1)
        tfs1->ReleaseFont(font1);
    if (font2)
        tfs1->ReleaseFont(font2);
    if (font4)
        tfs1->ReleaseFont(font4);
 
    if (id6)
        tfs1->RemoveFile(id6);
    if (id5)
        tfs1->RemoveFile(id5);
    delete tfs1;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0558

	@SYMTestCaseDesc
	Tries to add and load a nonexisting 
	file to the FbsTypeFaceStore and checks
	if it fails.
	
	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestInvalidFiles()
	{
	INFO_PRINTF1(_L("Test Invalid Files"));
	__UHEAP_MARK;
	CFbsTypefaceStore* tfs=NULL;
	TRAPD(ret,tfs=CFbsTypefaceStore::NewL(NULL));
	TEST(ret==KErrNone);
	TEST(tfs!=NULL);
	TInt id=0;
	ret=tfs->AddFile(_L("\\nonexist.gdr"),id);
	TEST(ret!=KErrNone);
	delete tfs;
	CFbsBitmapEx bmp;
	ret=bmp.Load(_L("\\nonexist.mbm"),0);
	TEST(ret!=KErrNone);
	User::Heap().Check();
	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0559

	@SYMTestCaseDesc
	Gets a font and tests all its properties. 

	@SYMTestActions
	Tests the char width, text width, handles, verifies
	the resource count and the heap for memory leaks
	CharWidthInPixels() and TextWidthInPixels() may return 
	different values depending on the hardware used.
	Therefore, when comparing these values for the same character,
	allow a margin of 1 pixel difference.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestGetFont()
	{
	INFO_PRINTF1(_L("Test Get Font"));
	CFbsFontEx* font=SelectFont();
	iResourceCount++;
	TEST(font->Handle()!=0);
	CheckResourceCount();
	TUid uid;
	uid=KCFbsFontUid;
	TEST(font->TypeUid()==uid);
	TInt w=font->TextWidthInPixels(_L(" "));
	TEST(w>0);
	// Now test the same use case with context
	CFont::TMeasureTextInput param;
	param.iStartInputChar = 34;
	TInt wContext=font->TextWidthInPixels(_L("->This text will not be measured<- "),&param);
	TEST(wContext==w);
	// End of first use case with context
		
	TInt x=font->TextCount(_L("          "),10);
	TEST(10/w==x);
	TInt y,z;
	y=font->TextCount(_L("   "),10,z);
	TEST(y==x);
	TInt cwidth=font->CharWidthInPixels('i');
	TInt twidth=font->TextWidthInPixels(_L("i"));
	TInt charTextdiff = cwidth - twidth;
	TEST(1>=charTextdiff && -1<=charTextdiff);
	TInt twidthContext=font->TextWidthInPixels(_L("->This text will not be measured<-i"),&param);
	TEST(twidth==twidthContext);
	cwidth=font->CharWidthInPixels('p');
	twidth=font->TextWidthInPixels(_L("p"));
	charTextdiff = cwidth - twidth;
	TEST(1>=charTextdiff && -1<=charTextdiff);
	twidthContext=font->TextWidthInPixels(_L("->This text will not be measured<-p"),&param);
	TEST(twidth==twidthContext);
	cwidth=font->CharWidthInPixels('W');
	twidth=font->TextWidthInPixels(_L("W"));
	charTextdiff = cwidth - twidth;
	TEST(1>=charTextdiff && -1<=charTextdiff);
	twidthContext=font->TextWidthInPixels(_L("->This text will not be measured<-W"),&param);
	TEST(twidth==twidthContext);
	cwidth=font->CharWidthInPixels(' ');
	twidth=font->TextWidthInPixels(_L(" "));
	charTextdiff = cwidth - twidth;
	TEST(1>=charTextdiff && -1<=charTextdiff);
	SCharWidth chwid;
	font->TextWidthInPixels(_L(" "),chwid);
	TEST(chwid.iMove==w);
	TEST(chwid.iWidth+chwid.iLeftAdjust+chwid.iRightAdjust==chwid.iMove);
	font->TextWidthInPixels(_L("->This text will not be measured<- "),&param,chwid);
	TEST(chwid.iMove==w);
	TEST(chwid.iWidth+chwid.iLeftAdjust+chwid.iRightAdjust==chwid.iMove);

	// Now do all these tests with an OpenType font where the width with context is different from the
	// width without context
	// NOTE:	These tests are performed for WINS and WINSCW only because we are using a TrueType font, and the bounds of 
	// 			the text returned is different for different hardware devices, including different h4's	and hence 
	//			cannot be measured.
	#if defined __WINS__ || defined __WINSCW__
	CFbsFontEx* openFont = SelectOpenTypeFont();
	iResourceCount++;
	TEST(font->Handle()!=0);
	CheckResourceCount();
	uid=KCFbsFontUid;
	TEST(font->TypeUid()==uid);
	cwidth=openFont->CharWidthInPixels('.');
	twidth=openFont->TextWidthInPixels(_L("."));
	charTextdiff = cwidth - twidth;
	TEST(1>=charTextdiff && -1<=charTextdiff);
	param.iStartInputChar = 1;
	twidthContext=openFont->TextWidthInPixels(_L("\x0915."),&param);
	TEST(twidth!=twidthContext);
	cwidth=openFont->CharWidthInPixels(',');
	twidth=openFont->TextWidthInPixels(_L(","));
	charTextdiff = cwidth - twidth;
	TEST(1>=charTextdiff && -1<=charTextdiff);
	twidthContext=openFont->TextWidthInPixels(_L("\x0915,"),&param);
	TEST(cwidth!=twidthContext);
	TEST(twidth!=twidthContext);
	openFont->TextWidthInPixels(_L("\x0915."),&param,chwid);
	TEST(chwid.iMove!=cwidth);
	#endif

	CFbsFontEx* font2=SelectFont(); // Don't increment iResourceCount as the font is already in the cache
	CheckResourceCount();
	iTs->ReleaseFont(font2);
	RFbsSession* fbsalt=RFbsSession::GetSession();
	CFbsFontEx* font3=new CFbsFontEx;
	font3->Duplicate(font->Handle());
	iResourceCount++;
	TEST(font3->Handle());
	CheckResourceCount();
	TEST(fbsalt->ResourceCount()==iResourceCount);
	iTs->ReleaseFont(font);
	iResourceCount--;
	TEST(w==font3->TextWidthInPixels(_L(" ")));
	CheckResourceCount();
	TEST(fbsalt->ResourceCount()==iResourceCount);
	font3->Reset();
	delete font3;
	iResourceCount--;
	CheckResourceCount();
	TEST(fbsalt->ResourceCount()==iResourceCount);
	User::Heap().Check();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0560

	@SYMTestCaseDesc
	Tests the font store.

	@SYMTestActions
	Allocates an array of pointers which is
	used to store the font height. It tests
	the font height and gets the nearest font
	available. The heap is checked for 
	memory leaks.	
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestFontStore()
	{
	INFO_PRINTF1(_L("Test Font Store"));
	CFbsFontEx* font=NULL;
	TFontSpec fs;
	TInt typefaces=iTs->NumTypefaces();
	TTypefaceSupport info;
	for(TInt count=0;count<typefaces;count++)
		{
		iTs->TypefaceSupport(info,count);
		INFO_PRINTF1(_L("    "));
	    TBuf<KMaxTypefaceNameLength> tname;
		tname.Copy(info.iTypeface.iName);
		INFO_PRINTF1(tname);
		INFO_PRINTF1(_L("\n"));
		TInt* heightarray=(TInt*)User::Alloc(info.iNumHeights*sizeof(TInt));
		TInt index=0;
		for(;index<info.iNumHeights;index++)
			heightarray[index]=iTs->FontHeightInTwips(count,index);
		for(index=1;index<info.iNumHeights;index++)
			TEST(heightarray[index]>heightarray[index-1]);
		for(index=0;index<info.iNumHeights;index++)
			heightarray[index]=iTs->FontHeightInPixels(count,index);
		for(index=1;index<info.iNumHeights;index++)
			TEST(heightarray[index]>=heightarray[index-1]);
		delete [] heightarray;
		for(index=0;index<info.iNumHeights;index++)
			{
			TInt height=iTs->FontHeightInTwips(count,index);
			fs.iTypeface=info.iTypeface;
			fs.iHeight=height;
			TInt ret=iTs->GetNearestFontToDesignHeightInTwips((CFont*&)font,fs);
			TEST(ret==KErrNone);
			TEST(font->Handle());
			iTs->ReleaseFont(font);

			height=iTs->FontHeightInPixels(count,index);
			fs.iTypeface=info.iTypeface;
			fs.iHeight=height;
			ret=iTs->GetNearestFontToDesignHeightInPixels((CFont*&)font,fs);
			TEST(ret==KErrNone);
			TEST(font->Handle());
			iTs->ReleaseFont(font);
			}
		}
	iResourceCount = iFbs->ResourceCount(); // Allow for fonts left in the typeface store cache
	User::Heap().Check();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0561

	@SYMTestCaseDesc
	Tests the font name alias

	@SYMTestActions
	Creates three different font name alias with a 
	mixture of caps and small letters. Checks the font
	name aliases using the different names.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestFontNameAlias()
	{
	INFO_PRINTF1(_L("Test Font Name Alias"));

	TInt size = 20;
	_LIT(KFontAlias0,"Font alias 0");
	_LIT(KFontAlias1,"Font alias 1");
	_LIT(KFontAlias2,"FOnT AlIaS 0");//Alias name in mixture of caps & small letters for KFontAlias0

    TBuf<KMaxTypefaceNameLength> defaultName;
    TBuf<KMaxTypefaceNameLength> fontName[3];

	CFbsFontEx* font = NULL;
	TFontSpec fs(KNullDesC,200);
	TInt ret = iTs->GetNearestFontToDesignHeightInTwips((CFont*&)font,fs);
	TEST(ret == KErrNone);
	TEST(font->Handle());
	fs = font->FontSpecInTwips();
	defaultName = fs.iTypeface.iName;
	iTs->ReleaseFont(font);

	TTypefaceSupport info;
	for (TInt index = 0, nameIndex = 0; nameIndex < 3; index++)
		{
		iTs->TypefaceSupport(info,index);
		if (info.iTypeface.iName != defaultName)
			fontName[nameIndex++] = info.iTypeface.iName;
		}

	TRAP(ret,iTs->SetFontNameAliasL(KFontAlias0,fontName[0]));
	TEST(ret == KErrNone);
	CheckFontNameAlias(KFontAlias0,fontName[0],size);
	//Testing if it works even if the alias name passed differs in case
	CheckFontNameAlias(KFontAlias2,fontName[0],size);

	TRAP(ret,iTs->SetFontNameAliasL(KFontAlias1,fontName[1]));
	TEST(ret == KErrNone);
	CheckFontNameAlias(KFontAlias0,fontName[0],size);
	CheckFontNameAlias(KFontAlias1,fontName[1],size);

	TRAP(ret,iTs->SetFontNameAliasL(KFontAlias0,fontName[2]));
	TEST(ret == KErrNone);
	CheckFontNameAlias(KFontAlias0,fontName[2],size);
	CheckFontNameAlias(KFontAlias1,fontName[1],size);

	TRAP(ret,iTs->SetFontNameAliasL(KFontAlias1,KNullDesC));
	TEST(ret == KErrNone);
	CheckFontNameAlias(KFontAlias0,fontName[2],size);
	CheckFontNameAlias(KFontAlias1,defaultName,size);

	TRAP(ret,iTs->SetFontNameAliasL(KFontAlias0,KNullDesC));
	TEST(ret == KErrNone);
	CheckFontNameAlias(KFontAlias0,defaultName,size);
	CheckFontNameAlias(KFontAlias1,defaultName,size);
	iResourceCount--; // Allow for a cache eviction because the last check fills the cache

	CheckResourceCount();
	User::Heap().Check();
	}

void CTFbs::CheckFontNameAlias(const TDesC& aFontAlias, const TDesC& aFontName, TInt& aSize)
	{
	CFbsFontEx* font = NULL;
	TFontSpec fs(aFontAlias,aSize);
	aSize += 20;

	TInt ret = iTs->GetNearestFontToDesignHeightInTwips((CFont*&)font,fs);
	TEST(ret == KErrNone);
	TEST(font->Handle());

	TFontSpec checkFS = font->FontSpecInTwips();
	TEST(checkFS.iTypeface.iName == aFontName);

	iTs->ReleaseFont(font);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0562

	@SYMTestCaseDesc
	Tests the width of a font stored in a buffer
	
	@SYMTestActions
	Gets how much of the specified descriptor can be 
	displayed for a specified font without 
	exceeding the specified width.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestBufferedFont()
	{
	INFO_PRINTF1(_L("Test Buffered Font"));
	CFbsFontEx* font=SelectFont();
	TBuf<1280> largebuf;
	TInt count=0;
	for(;count<1280;count++)
		largebuf.Append(32);
	TInt width,lcount,rcount,lwidth,rwidth;
	width=font->TextWidthInPixels(largebuf);
	for(count=0;count<1280;count+=500)
		{
		lcount=font->TextCount(largebuf.Left(count),100000);
		TEST(lcount==count);
		rcount=font->TextCount(largebuf.Right(1280-count),100000);
		TEST(rcount==1280-count);
		lwidth=font->TextWidthInPixels(largebuf.Left(count));
		rwidth=font->TextWidthInPixels(largebuf.Right(1280-count));
		TEST(rwidth+lwidth==width);
		}
	iTs->ReleaseFont(font);
	CheckResourceCount();
	User::Heap().Check();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0563

	@SYMTestCaseDesc
	Tests having multiple fonts allocated
	and deallocated

	@SYMTestActions
	Gets multiple font. Releases the hold of a 
	typeface store client on a specified font.
	Decrements the access-count for the specified 
	font by one. If this reduces the access-count 
	to zero then the font is no longer needed by any 
	client, and is deleted from the typeface store list.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestMultipleFont()
	{
	INFO_PRINTF1(_L("Test Multiple Font"));
	const TInt numfonts=100;
	CFbsFontEx* fonts[numfonts];
	TInt count=0;
	for(;count<numfonts;count++)
		fonts[count]=SelectFont();
	for(count=0;count<numfonts;count++)
		iTs->ReleaseFont(fonts[count]);
	INFO_PRINTF2(_L("    %d font handles created.\n"),numfonts);
	CheckResourceCount();
	User::Heap().Check();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0564

	@SYMTestCaseDesc
	Tests creation of unique bitmaps with
	no handles already to them. Also tests
	and verifies defects DEF069571 and DEF076347.
	
	@SYMDEF DEF069571 DEF076347
	
	@SYMTestActions
	Test creation of original bitmaps. Destroys 
	existing bitmap and creates a new one. Tests
	creation of bitmap by duplicating an already
	existing bitmap. Tests creation of different 
	sized bitmaps and resizes them.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestCreateBitmap()
	{
	INFO_PRINTF1(_L("Test Create Bitmap"));
	__UHEAP_MARK;

	CFbsBitmapEx bmp1;
	CFbsBitmapEx bmp2;

	// Test creation of original bitmaps (ie handles are unique)
	TInt ret=bmp1.Create(TSize(0,0),EGray2);
	TEST(ret==KErrNone);
	iResourceCount++;
	CheckResourceCount();

	ret=bmp2.Load(iTestBitmapName,ETfbs);
	TEST(ret==KErrNone);
	iResourceCount++;
	TEST(bmp2.Handle()!=bmp1.Handle());
	CheckResourceCount();

	ret=bmp2.Create(TSize(0,0),EGray2); // Destroys existing and creates new
	TEST(ret==KErrNone);
	TEST(bmp2.Handle()!=bmp1.Handle());
	CheckResourceCount();
	bmp2.Reset();
	iResourceCount--;
	CheckResourceCount();

	// Test creation by duplication
	RFbsSession* fbsalt=RFbsSession::GetSession();
	CFbsBitmapEx bmp3;

	ret=bmp3.Duplicate(bmp1.Handle());
	TEST(ret==KErrNone);
	iResourceCount++;
	CheckResourceCount();
	TEST(fbsalt->ResourceCount()==iResourceCount);
	TEST((TInt)bmp1.BitmapAddress());
	CBitwiseBitmap* bmp3add=bmp3.BitmapAddress();
	CBitwiseBitmap* bmp1add=bmp1.BitmapAddress();
	TEST(bmp3add==bmp1add);
	TEST(bmp3.Handle()==bmp1.Handle());
	bmp3.Reset();
	iResourceCount--;
	CheckResourceCount();
	TEST(fbsalt->ResourceCount()==iResourceCount);
	bmp1.Reset();
	iResourceCount--;
	CheckResourceCount();
	TEST(fbsalt->ResourceCount()==iResourceCount);
	
	//Testing the DEF069571
	
	CFbsBitmapEx bmp0;
	//Test creation of large bitmaps (>65536)
	TInt ret0=bmp0.Create(TSize(65536,1),EGray256);
	bmp0.Reset();
	
	//Testcode for DEF076347
	CFbsBitmapEx bmp01;
	TInt ret01=bmp01.Create(TSize(65536,1),EColor16MU);
	TInt ret02=bmp01.SetDisplayMode(EColor16MA);
	bmp01.Reset();
	
	

	// Test creation of different sizes and resizing them
	DoResizeBitmap(EGray2);
	DoResizeBitmap(EGray4);
	DoResizeBitmap(EGray16);
	DoResizeBitmap(EGray256);
	DoResizeBitmap(EColor16);
	DoResizeBitmap(EColor256);
	DoResizeBitmap(EColor4K);
	DoResizeBitmap(EColor64K);
	DoResizeBitmap(EColor16M);

	User::Heap().Check();
	__UHEAP_MARKEND;
	}

void CTFbs::DoResizeBitmap(TDisplayMode aDispMode)
	{
	const TInt numNewSizes = 5;
	const TSize newSizes[numNewSizes] = { TSize(0,0), TSize(1,1), TSize(17,20), TSize(32,32), TSize(32,1025) };
	const TInt numTwipsSizes = 4;
	const TSize twipsSizes[numTwipsSizes] = { TSize(100,100), TSize(1440,1440), TSize(1000000,1000000), TSize(0,0) };

	__UHEAP_MARK;

	CFbsBitmapEx bmp;

	TInt count;
	for(count = 0; count < numNewSizes; count++)
		{
		TSize size = newSizes[count];
		TInt ret = bmp.Create(size,aDispMode);
		TEST(ret == KErrNone);
		TEST(bmp.Handle() != 0);
		TEST(bmp.SizeInPixels() == size);
		TEST(bmp.DisplayMode() == aDispMode);
		}

	for(count = 0; count < numNewSizes; count++)
		{
		TInt ret = bmp.Create(TSize(0,0),aDispMode);
		TSize size = newSizes[count];
		ret = bmp.Resize(size);
		TEST(ret == KErrNone);
		TEST(bmp.SizeInPixels() == size);
		}

	for(count = 0; count < numTwipsSizes; count++)
		{
		TSize size = twipsSizes[count];
		bmp.SetSizeInTwips(size);
		TEST(bmp.SizeInTwips() == size);
		}

	bmp.Reset();
	CheckResourceCount();

	User::Heap().Check();
	DeleteScanLineBuffer();
	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0565

	@SYMTestCaseDesc
	Tests loading of bitmaps

	@SYMTestActions
	Loads a specific bitmap from a multi-bitmap 
	file. Retrieves the bitmap addresses for the
	bitmap and check if they are valid.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestLoadBitmap()
	{
	INFO_PRINTF1(_L("Test Load Bitmap"));
	__UHEAP_MARK;
	CFbsBitmapEx bmp1;
	CFbsBitmapEx bmp2;
	CFbsBitmapEx bmp3;
	CFbsBitmapEx bmp4;
	CFbsBitmapEx bmp5;
	TInt ret=bmp1.Load(iTestBitmapName,ETfbs);
	TEST(ret==KErrNone);
	iResourceCount++;
	TEST(bmp1.Handle()!=0);
	ret=bmp2.Load(iTestBitmapName,ETfbs);
	TEST(ret==KErrNone);
	iResourceCount++;
	CBitwiseBitmap* bmp1add=bmp1.BitmapAddress();
	TEST((TInt)bmp1add);
	CBitwiseBitmap* bmp2add=bmp2.BitmapAddress();
	TEST(bmp1add==bmp2add);
	TEST(bmp1.Handle()==bmp2.Handle());
	ret=bmp3.Load(iTestBitmapName,ETblank);
	TEST(ret==KErrNone);
	iResourceCount++;
	TEST(bmp1.Handle()!=bmp3.Handle());
	bmp2.Reset();
	iResourceCount--;
	ret=bmp4.Load(iTestBitmapName,ETfbs);
	TEST(ret==KErrNone);
	iResourceCount++;
	CBitwiseBitmap* bmp4add=bmp4.BitmapAddress();
	TEST(bmp1add==bmp4add);
	TEST(bmp4.Handle()==bmp1.Handle());
	ret=bmp5.Load(iTestBitmapName,ETblank);
	TEST(ret==KErrNone);
	iResourceCount++;
	CBitwiseBitmap* bmp3add=bmp3.BitmapAddress();
	CBitwiseBitmap* bmp5add=bmp5.BitmapAddress();
	TEST((TInt)bmp3add);
	TEST(bmp3add==bmp5add);
	TEST(bmp5.Handle()==bmp3.Handle());
	CheckResourceCount();
	bmp3.Reset();
	iResourceCount--;
	bmp4.Reset();
	iResourceCount--;
	bmp5.Reset();
	iResourceCount--;
	ret=bmp2.Load(iTestBitmapName,ETfbs);
	TEST(ret==KErrNone);
	iResourceCount++;
	bmp2add=bmp2.BitmapAddress();
	TEST(bmp1add==bmp2add);
	TEST(bmp1.Handle()==bmp2.Handle());
	CheckResourceCount();
	bmp1.Reset();
	iResourceCount--;
	bmp2.Reset();
	iResourceCount--;
	CheckResourceCount();

	ret=bmp1.Load(iTestBitmapName,ETfbs,EFalse);
	TEST(ret==KErrNone);
	ret=bmp2.Load(iTestBitmapName,ETfbs,ETrue);
	TEST(ret==KErrNone);
	ret=bmp3.Load(iTestBitmapName,ETfbs,ETrue);
	TEST(ret==KErrNone);
	ret=bmp4.Load(iTestBitmapName,ETfbs,EFalse);
	TEST(ret==KErrNone);
	ret=bmp5.Load(iTestBitmapName,ETfbs,ETrue);
	TEST(ret==KErrNone);
	bmp1add=bmp1.BitmapAddress();
	bmp2add=bmp2.BitmapAddress();
	bmp3add=bmp3.BitmapAddress();
	bmp4add=bmp4.BitmapAddress();
	bmp5add=bmp5.BitmapAddress();
	TEST(bmp1add!=bmp2add);
	TEST(bmp1add!=bmp3add);
	TEST(bmp1add!=bmp4add);
	TEST(bmp1add!=bmp5add);
	TEST(bmp2add==bmp3add);
	TEST(bmp2add!=bmp4add);
	TEST(bmp2add==bmp5add);
	TEST(bmp3add!=bmp4add);
	TEST(bmp3add==bmp5add);
	TEST(bmp4add!=bmp5add);
	bmp1.Reset();
	bmp2.Reset();
	bmp3.Reset();
	bmp4.Reset();
	bmp5.Reset();
	User::Heap().Check();
	DeleteScanLineBuffer();
	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0566

	@SYMTestCaseDesc
	Checks the properties of a created bitmap.
	
	@SYMTestActions
	Creates a bitmap. Checks the resource count.
	Checks the handle, size, display mode and 
	conversion methods for twips to pixels. Checks
	the heap for memory leaks.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestQueryBitmap()
	{
	INFO_PRINTF1(_L("Test Query Bitmap"));
	__UHEAP_MARK;
	CheckResourceCount();
	CFbsBitmapEx bmp;
	TInt ret=bmp.Create(TSize(100,100),EGray16);
	TEST(ret==KErrNone);
	iResourceCount++;
	CheckResourceCount();
	TEST(bmp.Handle()!=0);
	TEST(bmp.SizeInPixels()==TSize(100,100));
	TEST(bmp.DisplayMode()==EGray16);
	bmp.SetSizeInTwips(TSize(1000,1000));
	TEST(bmp.SizeInTwips()==TSize(1000,1000));
	TEST(bmp.HorizontalPixelsToTwips(10)==100);
	TEST(bmp.VerticalPixelsToTwips(10)==100);
	TEST(bmp.HorizontalTwipsToPixels(100)==10);
	TEST(bmp.VerticalTwipsToPixels(100)==10);
	TEST(bmp.ScanLineLength(100,EGray16)==52);
	bmp.Reset();
	iResourceCount--;
	CheckResourceCount();
	User::Heap().Check();
	DeleteScanLineBuffer();
	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0567

	@SYMTestCaseDesc
	Extended test to check scan line buffer in 
	session allocated correctly for SetRomBitmapL	

	@SYMTestActions
	Loads a specific bitmap from a multi-bitmap 
	file. Reset the bitmap and checks the resource 
	count. Loads new bitmaps, checks the handle
	the bitmap address and data address.

	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestRomBitmapL()
	{
	_LIT(KBmpFile, "z:\\system\\data\\tfbs.rbm");
	INFO_PRINTF1(_L("Test Rom Bitmap"));
	CheckResourceCount();
	CFbsBitmapEx bmpx;
	TInt ret=bmpx.Load(KBmpFile,ETfbs);
	if (ret != KErrNone)
		{
		INFO_PRINTF2(_L("Rom bitmap tfbs.mbm not loaded %d\r\n"),ret);
		return;
		}
	bmpx.Reset();
	CheckResourceCount();
	__UHEAP_MARK;
	CFbsBitmapEx bmp1;
	CFbsBitmapEx bmp2;
	CFbsBitmapEx bmp3;
	CFbsBitmapEx bmp4;
	CFbsBitmapEx bmp5;
	ret=bmp1.Load(KBmpFile,ETfbs);
	TEST(ret==KErrNone);
	TEST(bmp1.Handle()!=0);
	ret=bmp2.Load(KBmpFile,ETfbs);
	TEST(ret==KErrNone);
	CBitwiseBitmap* bmp1add=bmp1.BitmapAddress();
	TEST((TInt)bmp1add);
	CBitwiseBitmap* bmp2add=bmp2.BitmapAddress();
	TEST(bmp1add==bmp2add);
	TEST(bmp1.Handle()==bmp2.Handle());
	CheckResourceCount();

	ret=bmp3.Load(KBmpFile,ETblank);
	TEST(ret==KErrNone);
	TEST(bmp1.Handle()!=bmp3.Handle());
	bmp2.Reset();
	ret=bmp4.Load(KBmpFile,ETfbs);
	TEST(ret==KErrNone);
	CBitwiseBitmap* bmp4add=bmp4.BitmapAddress();
	TEST(bmp1add==bmp4add);
	TEST(bmp4.Handle()==bmp1.Handle());

	ret=bmp5.Load(KBmpFile,ETblank);
	TEST(ret==KErrNone);
	CBitwiseBitmap* bmp3add=bmp3.BitmapAddress();
	CBitwiseBitmap* bmp5add=bmp5.BitmapAddress();
	TEST((TInt)bmp3add);
	TEST(bmp3add==bmp5add);
	TEST(bmp5.Handle()==bmp3.Handle());

	CheckResourceCount();
	bmp3.Reset();
	bmp4.Reset();
	bmp5.Reset();
	ret=bmp2.Load(KBmpFile,ETfbs);
	TEST(ret==KErrNone);
	bmp2add=bmp2.BitmapAddress();
	TEST(bmp1add==bmp2add);
	TEST(bmp1.Handle()==bmp2.Handle());

	CheckResourceCount();
	bmp2.Reset();
	ret=bmp2.Load(iTestBitmapName,ETfbs);
	TEST(ret==KErrNone);
	bmp2add=bmp2.BitmapAddress();
	TSize bmpsize=bmp1.SizeInPixels();
	TInt sllen=CFbsBitmap::ScanLineLength(bmpsize.iWidth,bmp1.DisplayMode());
	sllen*=bmpsize.iHeight;
	bmp1.LockHeap();
	TUint32* bmp1bits=bmp1.DataAddress();
	bmp1.UnlockHeap();
	bmp2.LockHeap();
	TUint32* bmp2bits=bmp2.DataAddress();
	bmp2.UnlockHeap();
	TEST(Mem::Compare((TUint8*)bmp1bits,sllen,(TUint8*)bmp2bits,sllen)==0);
	bmp1.Reset();
	bmp2.Reset();

	ret=bmp1.Load(KBmpFile,ETfbs);
	TEST(ret==KErrNone);
	// Extended test to check scan line buffer in session allocated 
	// correctly for SetRomBitmapL()
	DeleteScanLineBuffer();
	TInt size = 0;
	bmp2.SetRomBitmapL(bmp1.BitmapAddress(),size);
	TEST(iFbs->iScanLineBuffer != 0);
	TEST(Mem::Compare((TUint8*)bmp1.BitmapAddress(),size,(TUint8*)bmp2.BitmapAddress(),size)==0);
	bmp1.Reset();
	bmp2.Reset();

	User::Heap().Check();
	DeleteScanLineBuffer();
	__UHEAP_MARKEND;
	}

TInt AlternateThreadTest(TAny* aAny)
	{
	CTFbs* alt = static_cast<CTFbs*> (aAny);
	__UHEAP_MARK;

	TInt ret=RFbsSession::Connect();
	alt->TEST(ret==KErrNone);
	RFbsSession* fbs=RFbsSession::GetSession();
	alt->TEST(fbs!=NULL);
	alt->TEST(fbs->ResourceCount()==0);

	CFbsFontEx* font=new CFbsFontEx;
	ret=font->Duplicate(buffer[0]);
	alt->TEST(ret==KErrNone);
	alt->TEST(fbs->ResourceCount()==1);
	
	CFbsBitmapEx bmp;
	ret=bmp.Duplicate(buffer[1]);
	alt->TEST(ret==KErrNone);
#if defined(__WINS__)
	alt->TEST(fbs->ResourceCount()==2);
#endif
	
	CFbsFontEx* font2=new CFbsFontEx;
	ret=font2->Duplicate(buffer[0]);
	alt->TEST(ret==KErrNone);
	font->Reset();
	User::Free(font);
	font2->Reset();
	User::Free(font2);
#if defined(__WINS__)
	alt->TEST(fbs->ResourceCount()==1);
#endif
	CFbsBitmapEx bmp2;
	ret=bmp2.Duplicate(buffer[1]);
	alt->TEST(ret==KErrNone);
	bmp.Reset();
	bmp2.Reset();
	alt->TEST(fbs->ResourceCount()==0);
	RFbsSession::Disconnect();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0568

	@SYMTestCaseDesc
	Tests the resource counting of font objects
	during multi threading
	
	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestMultiThread()
	{
	INFO_PRINTF1(_L("Test Multiple Threads"));
	CFbsFontEx* font=SelectFont();
	TEST(font->Handle()!=0);
	iResourceCount++;
	CheckResourceCount();
	
	CFbsBitmapEx bmp;
	TInt ret=bmp.Load(iTestBitmapName,ETfbs);
	TEST(ret==KErrNone);
	iResourceCount++;
	CheckResourceCount();

	buffer[0]=font->Handle();
	buffer[1]=bmp.Handle();
	RThread thrd;
	TRequestStatus stat;
	thrd.Create(_L("AlternateThreadTest"),AlternateThreadTest,KDefaultStackSize,0x2000,0x2000,this);
	thrd.SetPriority(EPriorityMuchMore);
	thrd.Logon(stat);
	thrd.Resume();
	User::WaitForRequest(stat);
	thrd.Close();
	bmp.Reset();
	iResourceCount--;
	iTs->ReleaseFont(font);
	iResourceCount--;
	CheckResourceCount();
	User::Heap().Check();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0569

	@SYMTestCaseDesc
	Tests compression of bitmaps on the heap.

	@SYMTestActions
	Creates a bitmap. Checks the resource count.
	Allocates memory on the heap. Locks the heap and
	stores the bitmaps data address. The heap is unlocked.
	Copies data from the stored data address and compares
	the copied data with the original data. The heap is
	checked for memory leaks.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestHeapCompression()
	{
	INFO_PRINTF1(_L("Test Heap Compression"));
	__UHEAP_MARK;
	CheckResourceCount();
	CFbsBitmapEx bmp1;
	TInt ret=bmp1.Create(TSize(100,100),EGray16);
	TEST(ret==KErrNone);
	iResourceCount++;
	CheckResourceCount();
	CFbsBitmapEx bmp2;
	ret=bmp2.Create(TSize(100,100),EGray16);
	TEST(ret==KErrNone);
	iResourceCount++;
	CheckResourceCount();
	TUint32* data=(TUint32*)User::Alloc(5200);
	TEST(data!=NULL);
	bmp2.LockHeap();
	TUint32* sladd=bmp2.DataAddress();
	bmp2.UnlockHeap();
	TUint32* slptr=sladd;
	for(TInt count=0;count<1300;count++)
		*slptr++=count;
	Mem::Copy(data,sladd,5200);
	bmp1.Reset();
	iResourceCount--;
	CheckResourceCount();
	bmp2.LockHeap();
	TUint32* newdata=bmp2.DataAddress();
	bmp2.UnlockHeap();
	TEST(Mem::Compare((TUint8*)data,5200,(TUint8*)newdata,5200)==0);
	bmp2.Reset();
	iResourceCount--;
	CheckResourceCount();
	delete data;
	User::Heap().Check();
	DeleteScanLineBuffer();
	__UHEAP_MARKEND;
	}

void CTFbs::ExpandCleanupStackL()
	{
	TInt count=0;
	for(;count<10;count++)
		CleanupStack::PushL((TUint32*)0x1);
	CleanupStack::Pop(count);
	}

void CTFbs::CheckResourceCount()
	{
	const TInt serverResourceCount = iFbs->ResourceCount();
	TEST(serverResourceCount == iResourceCount);
	}

//
// DoCreateBitmap(), CreateBitmap(), DoAllocScanLineBufL(), CTFbs::AllocScanLineBuf() are 
// used to show "INC044388  SymbianOS week 12 Fbserv crashes (on HW only)" defect
// (problem with NULL scanline buffer) and to prove its fix.
// The idea of the test:
// A second thread ("CreateBitmap" is the thread name) is created and when resumed it
// creates a bitmap and initializes ComprBitmap pointer to point to it.
// The "CreateBitmap" thread signals the main thread, that the bitmap has been created,
// and waits until receiving "Die" signal and then dies.
// The main thread waits for a signal from the second thread and then duplicates the bitmap and 
// calls GetScanLine() on duplicated bitmap object. If the call fails, the defect is not fixed!

CFbsBitmap* ComprBitmap = NULL;//Shared between threads bitmap pointer.
_LIT(KRamComprBitmap, "z:\\system\\data\\16bit20col.mbm");
RSemaphore  BitmapCreatedSyncObj;//It is used to signal when the bitmap, created by the thread, 
								 //is ready for use
RSemaphore  DieSyncObj;//It is used to signal that "CreateBitmapThread" can die.

//Thread function. It is used by AllocScanLineBuf() test to create a test bitmap and share
//it with the main thread. This function is called from CreateBitmap().
static TInt DoCreateBitmapL(CTFbs* aTest)
	{
	aTest->TEST(::ComprBitmap == NULL);

	ComprBitmap = new (ELeave) CFbsBitmap;

	CleanupStack::PushL(::ComprBitmap);
	User::LeaveIfError(::ComprBitmap->Load(KRamComprBitmap, 0));
	CleanupStack::Pop(::ComprBitmap);

	return KErrNone;
	}

//Thread function. It prepares the cleanup stack and traps the call to DoCreateBitmap().
static TInt CreateBitmap(TAny* aTest)
	{
	CTFbs* test = static_cast <CTFbs*> (aTest);

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	test->TEST(trapCleanup != NULL);
	TInt err = RFbsSession::Connect();
	test->TEST(err == KErrNone);

	TRAP(err, DoCreateBitmapL(test));

	::BitmapCreatedSyncObj.Signal();//Signal the main thread, that the bitmap has been created
	::DieSyncObj.Wait();//Wait "Die" command from the main thread

	delete ::ComprBitmap;
	::ComprBitmap = NULL;
	RFbsSession::Disconnect();
	delete trapCleanup;
	return err;
	}

static void DoAllocScanLineBufL(CTFbs& aTest)
	{
	aTest.TEST(::ComprBitmap == NULL);
	//Create semaphores
	::CleanupClosePushL(::BitmapCreatedSyncObj);
	::CleanupClosePushL(::DieSyncObj);
	User::LeaveIfError(::BitmapCreatedSyncObj.CreateLocal(0));
	User::LeaveIfError(::DieSyncObj.CreateLocal(0));
	//Create "CreateBitmap" thread. The tread will create "ComprBitmap" bitmap.
	RThread createBitmapThread;
	_LIT(KName, "CreateBitmap");
	User::LeaveIfError(createBitmapThread.Create(KName, 
		(TThreadFunction)CreateBitmap, KDefaultStackSize, KMinHeapSize, 0x00100000, &aTest));
	RDebug::Print(_L("CreateBitmap thread started\r\n"));	
	//Request notification when the tread dies.
	TRequestStatus status;
	createBitmapThread.Logon(status);
	//Resume the thread
	createBitmapThread.Resume();
	::BitmapCreatedSyncObj.Wait();//Wait "bitmap created" signal
	//The bitmap "ComprBitmap" should be already created
	aTest.TEST(::ComprBitmap != NULL);
	//Duplicate the bitmap
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt err = bmp->Duplicate(::ComprBitmap->Handle());
	aTest.TEST(err == KErrNone);
	//Call GetScanLine(). It will fail if the defect is not fixed.
	TBuf8<100> buf;
	bmp->GetScanLine(buf, TPoint(0, 0), 10, EColor256);
	//Cleanup
	CleanupStack::PopAndDestroy(bmp);
	//Signal & Wait until "CreateBitmap" thread dies.
	::DieSyncObj.Signal();
	User::WaitForRequest(status);
	aTest.TEST(::ComprBitmap == NULL);
	CleanupStack::PopAndDestroy(&::DieSyncObj);
	CleanupStack::PopAndDestroy(&::BitmapCreatedSyncObj);
	}

//INC044388 - SymbianOS week 12 Fbserv crashes (on HW only)
void CTFbs::AllocScanLineBuf()
	{
	TRAPD(err, ::DoAllocScanLineBufL(*this));
	TEST(err == KErrNone);
	}
// End of INC044388 test.


int CTFbs::LoadOpenFontLibraries()
	{
	RImplInfoPtrArray implementationArray;
	TInt error = KErrNone;
	TUid uid = {KUidOpenFontRasterizerPlunginInterface};

	TRAPD(ecomerror,REComSession::ListImplementationsL(uid,implementationArray));
	TEST(ecomerror==KErrNone);

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
			TRAP(error,iFs->InstallRasterizerL(rasterizer));
			if (error)
				delete rasterizer;
			}
		}
	implementationArray.ResetAndDestroy();
	return error;
	}
	
TBool CTFbs::CheckTypefacesSupport(const TTypefaceSupport& aInfo1, const TTypefaceSupport& aInfo2)
	{
	if ((aInfo1.iTypeface == aInfo2.iTypeface) &&
		(aInfo1.iIsScalable == aInfo2.iIsScalable) &&
		(aInfo1.iMaxHeightInTwips == aInfo2.iMaxHeightInTwips) &&
		(aInfo1.iMinHeightInTwips == aInfo2.iMinHeightInTwips) &&
		(aInfo1.iNumHeights == aInfo2.iNumHeights))
		{
		return ETrue;	
		}
	return EFalse;
	}

void CTFbs::LoadFontsL(const TDesC& aFontsDir)
	{
	TUid id1 = TUid::Uid(0);

	RFs fileSys;
	User::LeaveIfError(fileSys.Connect());

	TFindFile fileFinder(fileSys);
	CDir* foundFileList = NULL;
	
	_LIT(KFBSERVFontFilePattern, "*");
	TInt findFileComplete = fileFinder.FindWildByDir(KFBSERVFontFilePattern,aFontsDir,foundFileList);

	while (!findFileComplete)
		{
		CleanupStack::PushL(foundFileList);

		const TInt foundFileCount = foundFileList->Count();
		for (TInt i = 0; i < foundFileCount; i++)
			{			
			TParse parse;
			if (parse.Set((*foundFileList)[i].iName,&fileFinder.File(),NULL) == KErrNone)
				{
				// Get filename and extension of font proposing to be loaded
				TPtrC fontFilename = parse.NameAndExt();
				INFO_PRINTF2(_L("CTFbs::LoadFontsL to load font filename: %S\r\n"), &fontFilename);
				TRAPD(addFileError,id1=iFs->AddFileL(parse.FullName()));
				if (addFileError != KErrNone)
					{
					// Log error in the "corrupt font" file
					User::Panic(_L("Wrong font file"), addFileError);
					}
				}
			}
		CleanupStack::PopAndDestroy(foundFileList); 
		findFileComplete = fileFinder.FindWild(foundFileList);
		}		
	fileSys.Close();
	}

 /**
   @SYMTestCaseID GRAPHICS-PREQ807_1_Load_all_fonts-0001
  
   @SYMPREQ REQ4723
  
   @SYMTestCaseDesc The test manually scans the font folders and loads all font files available.
   Once all the fonts have been loaded and stored in a CFontStore, the TTypefaces stored in this
   CFontStore object are compared to the TTypefaces stored in an existing CFbsTypefaceStore. 
   
   @SYMTestPriority High 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions \n
   API Calls:\n	
      
   @SYMTestExpectedResults The test expects that all the font files loaded by the test matches the ones 
   that FBserv loaded during the startup initialisation.    
  */
void CTFbs::TestAllFontsLoaded()
	{
	INFO_PRINTF1(_L("Test Load all fonts files"));
	__UHEAP_MARK;

	iHeap=UserHeap::ChunkHeap(NULL,0x10000,0x10000);
	TRAPD(ret,iFs=CFontStore::NewL(iHeap));
	TEST(ret==KErrNone);
	TEST(iFs != NULL);

	ret = LoadOpenFontLibraries();
	TEST(ret==KErrNone);

	TRAP(ret, LoadFontsL(KFBSERVFontFileDir));
	TEST(ret==KErrNone);

	// Check the Typefaces are the same both in the newly CFontStore and existing CFbsTypefaceStore
	TInt fsTypefaceNum = iFs->NumTypefaces();
	TInt tsTypefaceNum = iTs->NumTypefaces();
	TEST(fsTypefaceNum == tsTypefaceNum);	

	TBool equal = EFalse;
	for (TInt i = 0; i < fsTypefaceNum; i++)
		{
		TTypefaceSupport infoFs;
		iFs->TypefaceSupport(infoFs,i);
		for (TInt j = 0; j < tsTypefaceNum; j++)
			{
			TTypefaceSupport infoTs;
			iTs->TypefaceSupport(infoTs,j);
			
			equal = CheckTypefacesSupport(infoFs, infoTs);
			if (equal)
				{
				break;
				}
			}
		// Check if the CFontStore typeface has been found in the CFbsTypefaceStore
		TEST(equal);
		}
		
	delete iFs;
	iHeap->Close();

	REComSession::FinalClose();
	
	User::Heap().Check();
	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0570

	@SYMTestCaseDesc
	Tests default language for metrics.

	@SYMTestActions
	Constructs a TFontSpec object with the specified 
	typeface and height. Gets the font which is the 
	nearest to the given font specification. Checks
	that the maximum font height is correct for a 
	specified language.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbs::TestDefaultLanguageForMetrics()
	{
	static const TInt KRequiredHeight = 24;
	static const TInt KReturnedMaxHeight = 36;
	TInt rc = KErrGeneral;

	// Case 00
	// No languages are set on font spec and typeface store
	//
	TFontSpec fontSpec0(KTypefaceName, KRequiredHeight);
	CFont* font0 = NULL;
	rc = iTs->GetNearestFontToDesignHeightInPixels(font0, fontSpec0);
	TEST(KErrNone == rc);
	TEST(NULL != font0);
	TEST(KReturnedMaxHeight == font0->FontMaxHeight());
	INFO_PRINTF2(_L("FontMaxHeight returns %d"), font0->FontMaxHeight());

	// Case 01
	// Set language on font spec will suppress that on typeface store
	//
	fontSpec0.SetScriptTypeForMetrics(ELangGreek);
	CFont* font1 = NULL;
	rc = iTs->GetNearestFontToDesignHeightInPixels(font1, fontSpec0);
	TEST(KErrNone == rc);
	TEST(NULL != font1);
	TEST(KReturnedMaxHeight == font1->FontMaxHeight());
	INFO_PRINTF2(_L("FontMaxHeight returns %d"), font1->FontMaxHeight());

	// Set language on typeface store
	iTs->SetDefaultLanguageForMetrics(ELangEnglish);

	// Case 10
	// Language on typeface store will suppress that on font spec
	// _if_ its not specified
	//
	TFontSpec fontSpec1(KTypefaceName, KRequiredHeight);
	CFont* font2 = NULL;
	rc = iTs->GetNearestFontToDesignHeightInPixels(font2, fontSpec1);
	TEST(KErrNone == rc);
	TEST(NULL != font2);
	TEST(KReturnedMaxHeight == font2->FontMaxHeight());
	INFO_PRINTF2(_L("FontMaxHeight returns %d"), font2->FontMaxHeight());

	// Case 11 - Negative test of case 10
	//
	fontSpec1.SetScriptTypeForMetrics(ELangRussian);
	CFont* font3 = NULL;
	rc = iTs->GetNearestFontToDesignHeightInPixels(font3, fontSpec1);
	TEST(KErrNone == rc);
	TEST(NULL != font3);
	TEST(KReturnedMaxHeight == font3->FontMaxHeight());
	INFO_PRINTF2(_L("FontMaxHeight returns %d"), font3->FontMaxHeight());

	iTs->ReleaseFont(font0);
	iTs->ReleaseFont(font1);
	iTs->ReleaseFont(font2);
	iTs->ReleaseFont(font3);

	CheckResourceCount();
	User::Heap().Check();
	}
	

//The "bad" versions of these standard fonts have been hacked to give specific results
_LIT(KFBSERVFontFileOpen, 		"Z:\\resource\\fonts\\DejaVuSansCondensed.ttf");

//This file is identical to DejaVuSansCondensed.ttf
_LIT(KFBSERVFontFileBadOpen, 	"Z:\\PlatTest\\Graphics\\TestData\\uniquified_fonts\\xx_dejavusanscondensed.ttf");

 /**
   @SYMTestCaseID GRAPHICS-CTFbs-TestDuplicateFontFileEntries-0001
  
   @SYMDEF  DEF094692
  
   @SYMTestCaseDesc  If Open fonts are loaded that are marginally unique compared to the 
   					 existing fonts then they are not actually added to the typeface system, and may even be discarded.
   					 The fix is actually in FNTSTORE, but I need to test in an environment with full Open font support,
   					 which is not provided in the fontstore test code.
   					 Bi9tmap fon ts are tested in fntstore\tfs\t_fntmem.cpp
   
   @SYMTestPriority Med 
  
   @SYMTestStatus Implemented
   
   @SYMTestActions \n
   		The original versions of the Open fonts are first loaded to ensure that they have actually been loaded.
   		The original versions of the Open fonts are loaded again to demobnstrate they have the same IDs.
   		A version of the open font which is identical except for a different filename should be rejected
   	
   			
   API Calls:	AddFile\n	
      
   @SYMTestExpectedResults The test expects:
   		The original versions should load without changing the number of typefaces
   		The second load should return the same UID for the font
   		The identical fonts should return captured fail codes, and not change the typeface counts
   		The semi-identical font should perform a single increase to UIDs and typefaces
  */
void CTFbs::TestDuplicateFontFileEntries()
	{
	
	INFO_PRINTF1(_L("Test Load semi-duplicate fonts files"));
	TInt numFacesBefore=iTs->NumTypefaces();
	TInt aIdOpen1=0;
	TInt aIdOpen1b=0;
	TInt aIdOpen2=0;
	//make sure the originals of these fonts were safely opened
	//both should simply cause a reference-count increase on the originator files.
	TInt err2=iTs->AddFile(KFBSERVFontFileOpen,aIdOpen1);
	TEST(err2==KErrNone);
	if (err2)
		{
		INFO_PRINTF1(_L("One of the expected fonts was missing. Test abandoned."));
		}
	else
		{
		TInt numFacesAfterLoadOriginals=iTs->NumTypefaces();
		//TEST(numFacesBefore==numFacesAfterLoadOriginals);
		if (numFacesBefore!=numFacesAfterLoadOriginals)
			INFO_PRINTF2(_L("Warning: reinstalling the system fonts added %d typefaces!"), numFacesAfterLoadOriginals-numFacesBefore);
		
		//load the originals again just to prove the UIDs are re-used - only makes sense for open font where UID is generated!
		TInt err2=iTs->AddFile(KFBSERVFontFileOpen,aIdOpen1b);
		TEST(err2==KErrNone);
		TEST(aIdOpen1==aIdOpen1b);
		
		TInt numFacesAfterLoadBadFile3=iTs->NumTypefaces();
		TEST(numFacesBefore==numFacesAfterLoadBadFile3);
		
		//This open font file contains no new fonts, so will ultimately be discarded
		//The pathname is different so it won't be considered identical to its originator
		TInt err4=iTs->AddFile(KFBSERVFontFileBadOpen,aIdOpen2);
		TEST(err4==KErrAlreadyExists && aIdOpen2==0); 
		if (err4!=KErrAlreadyExists)
				INFO_PRINTF2(_L("Unexpected error code was %d"),err4);	 
		
		TInt numFacesAfterLoadBadFile4=iTs->NumTypefaces();
		TEST(numFacesBefore==numFacesAfterLoadBadFile4);
		
		}
	
	if (aIdOpen1)	iTs->RemoveFile(aIdOpen1);
	if (aIdOpen1b)	iTs->RemoveFile(aIdOpen1b);
	if (aIdOpen2)	iTs->RemoveFile(aIdOpen2);
	
	//The added typeface should have been uninstalled
	TInt numFacesAfterUninstall=iTs->NumTypefaces();
	TEST(numFacesAfterUninstall==numFacesBefore);
	
	
	}


/*
COMMENTED OUT FOR NOW BECAUSE DEF084095 FIX MAKES THIS TEST REDUNDANT
//DEF078039: Workwer function: Hindi - Risk of memory leak (RShapeInfo) if client thread dies
TInt TShapeHeaderMemoryLeakTest(TAny* aTest)
	{
	_LIT16(KHindi1,"\x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F\x0020\x0909\x0926\x094D\x0926\x0947\x0936\x094D\x0020\x0915\x094D\x0937\x093F\x092A\x094D\x0930");
	_LIT(KDevFontFace, "Devanagari OT Eval");
	
	CTFbs* test = static_cast<CTFbs*> (aTest);
	
	__UHEAP_MARK;
	
	const TDesC16& Hindi1 = KHindi1;
	RShapeInfo shapeInfo;
	TBool isOpen = EFalse;
	
	// Create a cleanup stack for this thread
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	test->TEST(trapCleanup != NULL);

	// Create a new session with FBSERV
	TInt ret=RFbsSession::Connect();
	test->TEST(ret==KErrNone);
	RFbsSession* fbs=RFbsSession::GetSession();
	test->TEST(fbs!=NULL);

	// Create a FbsFont to use for shaping
	TFontSpec testFontSpec(KDevFontFace,200); 
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	ret = bmp->Create(TSize(100,100),EGray2);
	if (ret == KErrNotSupported)
		return ret;
	else
		User::LeaveIfError(ret);

	CFbsBitmapDevice* device = NULL;
	TRAPD(err,device = CFbsBitmapDevice::NewL(bmp));
	User::LeaveIfError(err);

	CFbsBitGc* gc = NULL;
	User::LeaveIfError(device->CreateContext(gc));

	CFbsFont* font = NULL;
	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips(font,testFontSpec));
	gc->UseFont(font);
	
	// Open shapeInfo by calling GetCharacterPosition2. This makes an IPC call to the 
	// server and shapes Hindi, resulting in memory being allocated for a TShapeHeader object
	CFont::TPositionParam param;
	param.iDirection = CFont::EHorizontal;
	param.iFlags = CFont::TPositionParam::EFLogicalOrder;
	param.iText.Set(Hindi1);
	param.iPosInText = 0;
	param.iPen.iX = param.iPen.iY = 0;
	TBool r = font->GetCharacterPosition2(param, shapeInfo);
	
	// Test is memory was allocated to a TShapeHeader object
	isOpen = shapeInfo.IsOpen();
	test->TEST(isOpen == 1);
	
	// Tidy up and close
	delete bmp;
	delete device;
	delete gc;

	// Disconnect from the server. i.e. end ther session, without calling 
	// RShapeInfo::Close(), to simulate a client death
	RFbsSession::Disconnect();
	
	// Check to see is TShapeHeader has been freed.
	// Since a pointer the the TShapeHeader object is stored with the 
	// COpenFontSessionCache, when the client disconnects, the TShapeHeader 
	// object is freed along with the session cache.
	isOpen = shapeInfo.IsOpen();
	test->TEST(isOpen == 0);
	
	delete trapCleanup;

	__UHEAP_MARKEND;
	return(KErrNone);
	}
*/
/*
COMMENTED OUT FOR NOW BECAUSE DEF084095 FIX MAKES THIS TEST REDUNDANT

//DEF078039: Hindi - Risk of memory leak (RShapeInfo) if client thread dies
void CTFbs::TestShapeHeaderMemoryLeakAtClientDeath()
	{
	INFO_PRINTF1(_L("Test Shape Header Memory Leak At Client Death"));
	RThread shaperThread;
	TRequestStatus stat;
	shaperThread.Create(_L("Shaper Thread"),TShapeHeaderMemoryLeakTest,KDefaultStackSize, KMinHeapSize, 0x00100000,this);
	shaperThread.SetPriority(EPriorityMuchMore);
	shaperThread.Logon(stat);
	shaperThread.Resume();
	User::WaitForRequest(stat);
	shaperThread.Close();
	User::Heap().Check();
	}
// End of DEF078039 test.
*/

/**
@SYMTestCaseID			GRAPHICS-FBSERV-0506

@SYMDEF					INC103815

@SYMTestCaseDesc		Tests that the value returned by CFbsBitmap::Handle()
						does not change after resizing or compression

@SYMTestPriority		High

@SYMTestType			UT

@SYMTestStatus			Implemented

@SYMTestActions			1. Create a bitmap and store its handle number
						2. Resize the bitmap and compare its handle number with the stored value
						3. Compress the bitmap and compare its handle number with the stored value

@SYMTestExpectedResults	All the handle numbers returned should be the same
*/
void CTFbs::TestBitmapHandleImmutable()
	{
	INFO_PRINTF1(_L("Test fix for INC103815"));
	CFbsBitmap *bmp = new CFbsBitmap;
	TEST(bmp != NULL);
	if (bmp == NULL)
		return;
	TEST(bmp->Create(TSize(200, 200), EColor256) == KErrNone);
	TInt handle = bmp->Handle();
	TEST(bmp->Resize(TSize(400, 400)) == KErrNone);
	TEST(handle == bmp->Handle());
	TEST(bmp->Compress() == KErrNone);
	TEST(handle == bmp->Handle());
	delete bmp;
	}


/**
@SYMTestCaseID		GRAPHICS-FBSERV-0507
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMDEF				PDEF102570

@SYMTestCaseDesc
	Tests that the attributes and the data address of a bitmap do not change between
	calls to CFbsBitmap::BeginDataAccess() and CFbsBitmap::EndDataAccess() due to resizing
	or compression by other clients

@SYMTestActions
	1: Create a bitmap and a second CFbsBitmap object that refers to the same bitmap.
	2: Call BeginDataAccess() on the first CFbsBitmap object.
	3: Call Resize() on the second CFbsBitmap object.
	4: Call EndDataAccess() on the first CFbsBitmap object checking its state before and after.
	5: Call BeginDataAccess() on the first CFbsBitmap object.
	6: Call Compress() on the second CFbsBitmap object.
	7: Call EndDataAccess() on the first CFbsBitmap object checking its state before and after.

@SYMTestExpectedResults
	The affected attributes and the data address of the bitmap should change only
	after the calls to EndDataAccess()
*/
void CTFbs::TestBitmapBeginEnd()
	{
	INFO_PRINTF1(_L("Test CFbsBitmap::BeginDataAccess()-EndDataAccess()"));
	CFbsBitmap *bmp1 = new CFbsBitmap;
	CFbsBitmap *bmp2 = new CFbsBitmap;
	TEST(bmp1 != NULL && bmp2 != NULL);
	if (bmp1 == NULL || bmp2 == NULL)
		return;
	TEST(bmp1->Create(TSize(200, 200), EColor256) == KErrNone);
	TEST(bmp2->Duplicate(bmp1->Handle()) == KErrNone);
	TUint32* dataAddress1 = bmp1->DataAddress();
	TUint32* dataAddress2 = bmp2->DataAddress();
	TEST(dataAddress1 == dataAddress2);
	bmp1->BeginDataAccess();
	TEST(bmp2->Resize(TSize(400, 400)) == KErrNone);
	dataAddress2 = bmp2->DataAddress();
	TEST(bmp1->SizeInPixels() == TSize(200, 200));
	TEST(bmp1->DataAddress() == dataAddress1);
	bmp1->EndDataAccess();
	TEST(bmp1->SizeInPixels() == TSize(400, 400));
	dataAddress1 = bmp1->DataAddress();
	TEST(dataAddress1 == dataAddress2);
	bmp1->BeginDataAccess();
	TEST(bmp2->Compress() == KErrNone);
	dataAddress2 = bmp2->DataAddress();
	TEST(bmp1->Header().iCompression == ENoBitmapCompression);
	TEST(bmp1->DataAddress() == dataAddress1);
	bmp1->EndDataAccess();
	TEST(bmp1->Header().iCompression != ENoBitmapCompression);
	dataAddress1 = bmp1->DataAddress();
	TEST(dataAddress1 == dataAddress2);
	delete bmp1;
	delete bmp2;
	}


/**
@SYMTestCaseID		GRAPHICS-FBSERV-0508
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMDEF				DEF104752

@SYMTestCaseDesc
	Tests that trying to launch a second instance of FBServ fails gracefully

@SYMTestActions
	Start a new instance of FBSERV.EXE. Since the Font and Bitmap Server should
	already be running, the created process should panic immediately.

@SYMTestExpectedResults
	Panic FBSERV 5
*/
void CTFbs::TestSingletonServer()
	{
	INFO_PRINTF1(_L("Test singleton FBServ process"));
	_LIT(KFBSERVServerExe, "z:\\sys\\bin\\fbserv.exe");
	RProcess fbs;
	TInt ret = fbs.Create(KFBSERVServerExe, KNullDesC);
#ifdef __WINS__
	// On the emulator it's not possible to load the same EXEXP twice
	// DWin32CodeSeg::DoCreate() returns KErrAlreadyExists in that case
	if (ret == KErrAlreadyExists)
		return;
#endif
	TEST(ret == KErrNone);
	TRequestStatus status;
	fbs.Logon(status);
	TEST(status == KRequestPending);
	fbs.Resume();
	User::WaitForRequest(status);
	TExitType exitType = fbs.ExitType();
	TExitCategoryName exitCategory = fbs.ExitCategory();
	TInt exitReason = fbs.ExitReason();
	TEST(exitType == EExitPanic);
	TEST(exitCategory == KFBSERVPanicCategory);
	TEST(exitReason == EFbsPanicStartupFailed);
	fbs.Close();
	}

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0623
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMDEF             DEF140138

@SYMTestCaseDesc
	A positive test that shows in cases where the font glyph cache is full and the 
	session cache becomes used, client-side CFbsFont calls that query the session cache 
	successfully find the glyph, proving that the session handles used client-side and server-side
	are the same, thereby avoiding any unneccessary IPC calls.

@SYMTestActions
	Try to open a global semaphore called "FBSSessionCacheSemaphore" (this is a temporary measure to
		ensure successful propagation of ou1cimx1#250526).
	Create a large font which consumes a lot of memory.
	Create a CFbsBitGc and a CFbsBitmap target.
	Use the CFbsBitGc to render some text, thereby filling the font glyph cache. 
		Renders enough glyphs to fill up the font glyph cache, so that the session cache is used
		for later characters.
	Call CBitmapFont::CharacterNeedsToBeRasterized() with a NULL session handle.
	Call CBitmapFont::CharacterNeedsToBeRasterized() with the RFbsSession handle.
	Call CFbsFont::GetCharacterData() to retrieve a pointer to the bitmap data. Compare this with the data 
		returned from CBitmapFont::GetCharacterData().

@SYMTestExpectedResults
	No global semaphore called "FBSSessionCacheSemaphore" should exist.
	When using KNullHandle, CharacterNeedsToBeRasterized() with the first character, is expected to pass
		proving that the font cache is working as expected when glyphs can fit into it.
	When using KNullHandle, CharacterNeedsToBeRasterized() with the last character, it expected to fail, 
		proving that the glyph has not been cached in the font glyph cache. (If it had, it would find a 
		match regardless of the	session handle as it looks in the font cache before the session cache).
	When using the correct session handle with CharacterNeedsToBeRasterized() with the last character, 
		should return ETrue, proving that the glyph has been cached, and was found because the correct 
		session handle was supplied, therefore must be in the session cache.    
	The bitmap pointers returned by CFbsFont::GetCharacterData() and CBitmapFont::GetCharacterData() should match, 
		proving the public API CFbsFont::GetCharacterData() is correctly using the data stored in the session cache,
		and not returning the data of a separately-rasterized glyph.
*/
void CTFbs::TestFontSessionCacheLookupL()
	{
	INFO_PRINTF1(_L("Test Font Session-cache Lookup"));

	// Make sure the global semaphore is gone for good
	_LIT(KSessionCacheSemaphoreName, "FBSSessionCacheSemaphore");
	RSemaphore sem;
	TEST(sem.OpenGlobal(KSessionCacheSemaphoreName) == KErrNotFound);

	User::LeaveIfError(RFbsSession::Connect());    
	RFbsSession* fbsSession = RFbsSession::GetSession();
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);

	TFontSpec fs(_L("DejaVu Sans Mono"), 80);
	fs.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	CFbsFont* font = NULL;
	TEST(KErrNone == ts->GetNearestFontToDesignHeightInPixels((CFont*&)font, fs));

	CFbsBitmap* target = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(target);
	User::LeaveIfError(target->Create(TSize(100, 100), EColor16MU));
	CFbsBitmapDevice* bgDevice = CFbsBitmapDevice::NewL(target);
	CleanupStack::PushL(bgDevice);
	CFbsBitGc* bgGc = NULL;
	User::LeaveIfError(bgDevice->CreateContext(bgGc));
	bgGc->UseFont(font);
	CleanupStack::PushL(bgGc);
	_LIT(KTestText, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJLKMNOPQRSTUVWXYZ");

	// Draw the text, forcing the glyphs into the font and session caches.
	bgGc->DrawText(KTestText, TPoint(0, 10));

	CBitmapFont* bmFont = CTFbsFont::FontAddress(font);
	
	// Check that the first character in KTestText has been rasterized, and is in the font cache. 
	TBool glyphIsInFontCache = !bmFont->CharacterNeedsToBeRasterized(KNullHandle, KTestText()[0]);
	TEST(glyphIsInFontCache);
	
	// Check that the last character in KTestText has been rasterized, and is not in the font cache
	// because it did not fit, therefore it should be in the session cache.
	const TChar KTestChar = KTestText()[KTestText().Length() - 1];
	glyphIsInFontCache = !bmFont->CharacterNeedsToBeRasterized(KNullHandle, KTestChar);
	TEST(!glyphIsInFontCache);
	TBool glyphIsInSessionCache = !bmFont->CharacterNeedsToBeRasterized(fbsSession->iHelper->iServerSessionHandle, KTestChar);
	TEST(glyphIsInSessionCache);

	// Test that when calling GetCharacterData() on CFbsFont, it returns the same rasterized data as 
	// the bitmap in the session cache, proving that the client API is using the session cache handle correctly.
	TSize bitmapSize;
	const TUint8* bitmapDataClient;
	const TUint8* bitmapDataSessionCache;
	TOpenFontCharMetrics metrics;
	font->GetCharacterData(KTestChar,metrics,bitmapDataClient,bitmapSize);
	bmFont->GetCharacterData(fbsSession->iHelper->iServerSessionHandle, KTestChar, metrics, bitmapDataSessionCache);
	TEST(bitmapDataClient != NULL && bitmapDataClient == bitmapDataSessionCache);

	bgGc->DiscardFont();
	CleanupStack::PopAndDestroy(4); // bgGc, bgDevice, target, ts
	RFbsSession::Disconnect();
	}
	
/**
@SYMTestCaseID      GRAPHICS-FBSERV-0650
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMDEF             DEF141816

@SYMTestCaseDesc
	A negative test that shows that IPC calls from CFbsBitmap which uses a 
	CFbsFont handle fail and that IPC calls from CFbsFont which use a 
	CFbsBitmap handle fail.

@SYMTestActions
	i. Create a bitmap.
	ii. Create a font.
	iii. Send a bitmap-related IPC message to server with bitmap handle.
	iv. Send a bitmap-related IPC message to server with font handle.
	v. Send a font-related IPC message to server with font handle.
	vi. In a separate thread, send a font-related IPC message to server with
		bitmap handle.
	
@SYMTestExpectedResults
	Steps iii. and v. should return KErrNone.
	Step iv. should return KErrUnknown.
	Step vi. should panic with FBSERV -6.
*/
void CTFbs::TestInvalidHandlesInIpcCallsL()
	{
	INFO_PRINTF1(_L("Test Bitmap and Font IPC calls with invalid handles"));
	CFbsFontEx* font = SelectFont();

	CFbsBitmapEx* bmp = new(ELeave) CFbsBitmapEx();
	TEST(KErrNone == bmp->Create(TSize(1,1),EGray2));

	TInt fontHandle = font->FontHandle();
	TInt bitmapHandle = bmp->BitmapHandle();

	// Send a IPC command to resize a bitmap with a font handle
	TPckgBuf<TBmpHandles> handlebuf;
	TIpcArgs args1(fontHandle, 2, 2, &handlebuf);
	// Send a IPC command to resize a bitmap with the bitmap's handle
	TEST(KErrNone == iFbs->SendCommand(EFbsMessBitmapBgCompress, bitmapHandle));
	// Send a IPC command to resize a bitmap with the font's handle
	TEST(KErrUnknown == iFbs->SendCommand(EFbsMessBitmapBgCompress, fontHandle));

	// Send a IPC command to set the font's height with the font's handle
	TEST(KErrNone == iFbs->SendCommand(EFbsMessSetTwipsHeight, fontHandle, 49));
	// In a second thread, send a IPC command to set the font's height with the 
	// bitmap's handle.  Should panic with FBServ -6.
	_LIT(KTestName, "TestInvalidHandlesInIpcCall");
 	CreateSecondThreadAndCheckPanicL(SecondThreadPanicInvalidHandleInIpcCallL, KErrArgument, KFbsPanicCategory, KTestName);

	iTs->ReleaseFont(font);
	delete bmp;
	}

/**
Second thread function for test GRAPHICS-FBSERV-0650. It should panic with FBSERV -6 if the test is successful.
 */
void CTFbs::SecondThreadPanicInvalidHandleInIpcCallL()
	{
	User::LeaveIfError(RFbsSession::Connect());
	RFbsSession* fbs = RFbsSession::GetSession();
	CFbsBitmapEx* bmp = new(ELeave) CFbsBitmapEx();
	CleanupStack::PushL(bmp);
	User::LeaveIfError(bmp->Create(TSize(1,1), EGray2));
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	CFbsFont* font=NULL;
	User::LeaveIfError(ts->GetNearestFontToDesignHeightInPixels((CFont*&)font, TFontSpec(KTypefaceName, 15)));
	TInt bitmapHandle = bmp->BitmapHandle();

	// Send a IPC command to set twips height of font with a bitmap handle
	(void)fbs->SendCommand(EFbsMessSetTwipsHeight, bitmapHandle, 49);

	ts->ReleaseFont(font);
	CleanupStack::PopAndDestroy(2);
	RFbsSession::Disconnect();
	}


struct TFbsTestDelegator
	{
	CTFbs* iTestBase;
	void (CTFbs::*iMethodPtr)();
	};

/**
Creates a second thread and checks that it panics.
@param aMethodL The leaving method of CTFbs that the second thread is going to execute.
@param aPanicCode The expected panic code.
@param aPanicCategory The expected panic category.
@param aThreadName The name of the new thread.
*/
void CTFbs::CreateSecondThreadAndCheckPanicL(void (CTFbs::*aMethodL)(), TInt aPanicCode, const TDesC& aPanicCategory, const TDesC& aThreadName)
	{
	RThread secondThread;
	TFbsTestDelegator delegator;
	delegator.iTestBase = this;
	delegator.iMethodPtr = aMethodL;
	User::LeaveIfError(secondThread.Create(aThreadName, MethodDelegatorThreadFunction, KDefaultStackSize, 0x1000, 0x1000, &delegator));
	// Launch second thread
	TRequestStatus statusSecondThread;
	secondThread.Logon(statusSecondThread);
	secondThread.Resume();
	// Wait until second thread is finished
	User::WaitForRequest(statusSecondThread);
	// Check results
	if (EExitPanic != secondThread.ExitType())
		{
		ERR_PRINTF2(_L("Expected exit type: EExitPanic, actual exit type: %d"), secondThread.ExitType());
		TEST(EFalse);
		}
	if (aPanicCode != secondThread.ExitReason())
		{
		ERR_PRINTF3(_L("Expected exit reason: %d, actual exit reason: %d"), aPanicCode, secondThread.ExitReason());
        TEST(EFalse);
		}
	TExitCategoryName secondThreadExitCategory = secondThread.ExitCategory();
	if (aPanicCategory != secondThreadExitCategory)
		{
		ERR_PRINTF3(_L("Expected exit category: %S, actual exit category: %S"), &aPanicCategory, &secondThreadExitCategory);
        TEST(EFalse);
		}
	// Dispose of the remains of the second thread
	secondThread.Close();
	}

TInt CTFbs::MethodDelegatorThreadFunction(TAny* aDelegator)
	{
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (!cleanupStack)
	    {
	    return KErrNoMemory;
	    }
	TFbsTestDelegator* delegator = static_cast<TFbsTestDelegator*>(aDelegator);
	TRAPD(err, (delegator->iTestBase->*delegator->iMethodPtr)());
	delete cleanupStack;
	return err;
	}

/**
@SYMTestCaseID		GRAPHICS-FBSERV-0619
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMDEF				EI0014

@SYMTestCaseDesc
	Tests that there are no virtual function calls to objects in FBServ's shared heap from outside
	the FBServ process. Such function calls only work if DLLs are guaranteed to be mapped at the
	same logical address in every process. This is not necessarily true for all memory models.

@SYMTestActions
	Selects a font, intercepts the VF table in the corresponding CBitmapFont and COpenFont objects
	to record virtual function calls from outside the FBServ process, calls the vulnerable functions
	on the created CFbsFont object and releases the font.

@SYMTestExpectedResults
	Number of virtual function calls to CBitmapFont and COpenFont from outside FBServ is 0
*/
void CTFbs::TestNoVFCallsToGlobalObjects()
	{
#ifdef __WINS__
	INFO_PRINTF1(_L("Test no virtual function calls to objects in FBServ's shared heap from outside FBServ process"));
	CFbsFontEx* font = SelectFont();
	(void)font->TypeUid();
	(void)font->HeightInPixels();
	(void)font->AscentInPixels();
	(void)font->DescentInPixels();
	(void)font->CharWidthInPixels('A');
	(void)font->TextWidthInPixels(_L("A"));
	(void)font->BaselineOffsetInPixels();
	(void)font->TextCount(_L("A"), 10);
	TInt excess;
	(void)font->TextCount(_L("A"), 10, excess);
	(void)font->MaxCharWidthInPixels();
	(void)font->MaxNormalCharWidthInPixels();
	(void)font->FontSpecInTwips();
	TOpenFontCharMetrics metrics;
	const TUint8* bitmap;
	TSize size;
	(void)font->GetCharacterData('A', metrics, bitmap, size);
	CFont::TPositionParam param;
	param.iText.Set(_L("A"));
	(void)font->GetCharacterPosition(param);
	(void)font->FontCapitalAscent();
	(void)font->FontMaxAscent();
	(void)font->FontStandardDescent();
	(void)font->FontMaxDescent();
	(void)font->FontLineGap();
	iTs->ReleaseFont(font);
	TEST(iFontCallCounter.iVFCallsOutsideFBServ == 0);
	TEST(iOpenFontCallCounter.iVFCallsOutsideFBServ == 0);
#else
	INFO_PRINTF1(_L("Test no virtual function calls to objects in FBServ's shared heap from outside FBServ process - skipped on target"));
#endif
	}

//--------------
__CONSTRUCT_STEP__(Fbs)
