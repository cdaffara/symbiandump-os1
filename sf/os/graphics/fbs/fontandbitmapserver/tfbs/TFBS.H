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

#ifndef __TFBS_H__
#define __TFBS_H__

#include <fbs.h>
#include <gdi.h>
#include <test/fontinjector.h>
#include <test/TGraphicsHarness.h>

_LIT(KTempFilename,"c:\\temp.mbm");
_LIT(KTestBitmapOnZ,"z:\\system\\data\\tfbs.mbm");
_LIT(KTestBitmapOnC,"c:\\tfbs.mbm");
_LIT(KFail1Filename,"c:\\testfailed_bitmap1.mbm");
_LIT(KFail2Filename,"c:\\testfailed_bitmap2.mbm");

// Mostly to test bitmap compression
_LIT(KRomNotCompressed,"z:\\system\\data\\rom.mbm");
_LIT(KRomCompressed,"z:\\system\\data\\romcomp.mbm");
_LIT(KFileNotCompressed,"z:\\system\\data\\file.mbm");
_LIT(KFileCompressed,"z:\\system\\data\\filecomp.mbm");

// Panic Category for FBServ panics
_LIT(KFbsPanicCategory, "FBSERV");

enum TTfbs
	{
	ETfbs,
	ETblank
	};

class CFbsFontEx : public CFbsFont
	{
public:
	CFbsFontEx(): CFbsFont() { iHandle=0; iAddressPointer=NULL; }
	void Reset() { CFbsFont::Reset(); }
	void SetHandle() { iHandle = 1; }
	TInt Duplicate(TInt aHandle) { return(CFbsFont::Duplicate(aHandle)); }
	CBitmapFont* Address() const { return CFbsFont::Address(); }
	TInt FontHandle() const {return iHandle;}
	};

/**
Test utility class whose sole aim is to provide public access to CBitmapFont* from CFbsFont.
 */
class CTFbsFont : public CFbsFont
    {
public:
    static CBitmapFont* FontAddress(CFbsFont* aFont) { return static_cast<CTFbsFont*>(aFont)->Address(); }
    };

class CFbsBitmapEx : public CFbsBitmap
	{
public:
	CBitwiseBitmap* BitmapAddress() { if (!iHandle) return NULL; return iAddressPointer; }
	void LockHeap() const { CFbsBitmap::LockHeap(); }
	void UnlockHeap() const { CFbsBitmap::UnlockHeap(); }
	TInt BitmapHandle() const {return iHandle;}
	};

#ifdef __WINS__
template<class C> class XTCallCounter : public MShellcode
	{
public:
	XTCallCounter(CTGraphicsBase& aTestBase);
private:
	// From MShellcode
	void ExecuteShellcode(TInt aFromFunction);
private:
	CTGraphicsBase& iTestBase;
public:
	XVtableInjector<C> iInjector;
	TInt iVFCallsOutsideFBServ;
	};
#endif

class CTFbs : public CTGraphicsBase
	{
public:
	CTFbs(CTestStep* aStep);
	~CTFbs();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();
	
	void TestConstruction();
	void TestConnection();
	void TestFlushCallBack();
	static TInt FlushCallBack(TAny* aPtr);
	void TestInvalidFiles();
	void TestGetFont();
	void TestFontStore();
	void TestFontNameAlias();
	void TestAddFontFile();
	void TestBufferedFont();
	void TestMultipleFont();
	void TestCreateBitmap();
	void TestLoadBitmap();
	void TestQueryBitmap();
	void TestRomBitmapL();
	void TestMultiThread();
	void TestHeapCompression();
	void ExpandCleanupStackL();
	void DeleteScanLineBuffer();
	void AllocScanLineBuf();
	void TestAllFontsLoaded();
	void TestDefaultLanguageForMetrics();
	void TestDuplicateFontFileEntries();
	//COMMENTED OUT FOR NOW BECAUSE DEF084095 FIX MAKES THIS TEST REDUNDANT
	//void TestShapeHeaderMemoryLeakAtClientDeath();
	void TestBitmapHandleImmutable();
	void TestBitmapBeginEnd();
	void TestSingletonServer();
	void TestNoVFCallsToGlobalObjects();
	void TestFontSessionCacheLookupL();
	void TestInvalidHandlesInIpcCallsL();
private:
	CFbsFontEx* SelectFont();
	CFbsFontEx* SelectOpenTypeFont();
	void DoResizeBitmap(TDisplayMode aDispMode);
	void CheckResourceCount();
	void CheckFontNameAlias(const TDesC& aFontAlias, const TDesC& aFontName, TInt& aSize);
	TInt LoadOpenFontLibraries();
	TBool CheckTypefacesSupport(const TTypefaceSupport& aInfo1, const TTypefaceSupport& aInfo2);
	void LoadFontsL(const TDesC& aFontsDir);
	void SecondThreadPanicInvalidHandleInIpcCallL();
	void CreateSecondThreadAndCheckPanicL(void (CTFbs::*aMethodL)(), TInt aPanicCode, const TDesC& aPanicCategory, const TDesC &aThreadName);
	static TInt MethodDelegatorThreadFunction(TAny* aDelegator);

private:
	TInt iHandle;
	RFbsSession* iFbs;
	CFbsTypefaceStore* iTs;
	RHeap* iHeap;
	CFontStore *iFs;
	TInt iResourceCount;
	TInt iAllocs;
	TFileName iTestBitmapName;
	TBool iFlushCallbackReceived;
#ifdef __WINS__
	XTCallCounter<CFont> iFontCallCounter;
	XTCallCounter<COpenFont> iOpenFontCallCounter;
#endif
    
	};

class CTFbsStep : public CTGraphicsStep
	{
public:
	CTFbsStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTFbsStep,"TFbs");



#endif
