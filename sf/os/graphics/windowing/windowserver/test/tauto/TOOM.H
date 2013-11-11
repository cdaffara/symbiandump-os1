// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TOOM_H__
#define __TOOM_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"


typedef TBuf<0x40> TOomTestName;

class CTOom;

const TInt KNumCapKeyRequest = 20;


class COomFailBase : public CBase
	{
public:
	COomFailBase(CTOom *aTest);
	virtual void ConstructL();
	virtual void PreFail();
	virtual void ClearUpL();
	virtual void Flush();
	virtual TInt Fail()=0;
	virtual TOomTestName TestName()=0;
protected:
	RWsSession iWs;
	CWsScreenDevice* iDummyScreen;
	CTOom *iTest;
	};


class COomConnect : public COomFailBase
	{
public:
	COomConnect(CTOom *aTest);
	TInt Fail();
	void ClearUpL();
	TOomTestName TestName();
	};

class COomSetup : public COomFailBase
	{
public:
	COomSetup(CTOom *aTest);
	~COomSetup();
	void ConstructL();
protected:
	RWindowGroup iWinGroup;
	};

class COomWindowGroup : public COomSetup
	{
public:
	COomWindowGroup(CTOom *aTest);
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	RWindowGroup iFailWinGroup;
	};

class COomWindow : public COomSetup
	{
public:
	COomWindow(CTOom *aTest);
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	RWindow iFailWin;
	};

class COomBackedUpWindow : public COomSetup
	{
public:
	COomBackedUpWindow(CTOom *aTest);
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	RBackedUpWindow iFailWin;
	};

class COomBlankWindow : public COomSetup
	{
public:
	COomBlankWindow(CTOom *aTest);
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	RBlankWindow iFailWin;
	};

class COomGc : public COomSetup
	{
public:
	COomGc(CTOom *aTest);
	~COomGc();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	CWsScreenDevice *iScrDev;
	CWindowGc *iFailGc;
	};

class COomPolygon : public COomSetup
	{
public:
	COomPolygon(CTOom *aTest);
	~COomPolygon();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	CWsScreenDevice *iScrDev;
	RBackedUpWindow iWin;
	CWindowGc *iGc;
	CArrayFixSeg<TPoint> iPnts;
	};

class COomScreenDevice : public COomSetup
	{
public:
	COomScreenDevice(CTOom *aTest);
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	CWsScreenDevice *iScrDev;
	};

class COomBackupResize : public COomSetup
	{
public:
	COomBackupResize(CTOom *aTest);
	~COomBackupResize();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	RBackedUpWindow iFailWin;
	};

class COomPointerBuffer : public COomSetup
	{
public:
	COomPointerBuffer(CTOom *aTest);
	~COomPointerBuffer();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	RBackedUpWindow iFailWin;
	};

class COomPriorityKey : public COomSetup
	{
public:
	COomPriorityKey(CTOom *aTest);
	~COomPriorityKey();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
	};

class COomCaptureKeyBase : public COomSetup
	{
public:
	COomCaptureKeyBase(CTOom *aTest);
	~COomCaptureKeyBase();
	void ConstructL();
protected:
	TInt32 iCapKey[KNumCapKeyRequest];
	TInt iIndex;	
	};

class COomCaptureKey : public COomCaptureKeyBase
	{
public:
	COomCaptureKey(CTOom *aTest);
	~COomCaptureKey();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
	};

class COomCaptureKeyUpDown : public COomCaptureKeyBase
	{
public:
	COomCaptureKeyUpDown(CTOom *aTest);
	~COomCaptureKeyUpDown();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
	};

class COomLongKeyCapture : public COomCaptureKeyBase
	{
public:
	COomLongKeyCapture(CTOom *aTest);
	~COomLongKeyCapture();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
	};

class COomHotKey : public COomSetup
	{
public:
	COomHotKey(CTOom *aTest);
	~COomHotKey();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
	};

class COomGroupName : public COomSetup
	{
public:
	COomGroupName(CTOom *aTest);
	~COomGroupName();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
	};

class COomMessageSend: public COomSetup
	{
public:
	COomMessageSend(CTOom *aTest);
	~COomMessageSend();
	void ConstructL();
	void PreFail();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	RWindowGroup iWinGroup2;
	};

class COomMessageFetch: public COomSetup
	{
public:
	COomMessageFetch(CTOom *aTest);
	~COomMessageFetch();
	void PreFail();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	RWindowGroup iWinGroup2;
	};

class COomCopyScreen : public COomSetup
	{
public:
	COomCopyScreen(CTOom *aTest);
	~COomCopyScreen();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	CWsScreenDevice *iScrDev;
	CFbsBitmap *iBitmap;
	};

class COomRequestEvents : public COomSetup
	{
public:
	COomRequestEvents(CTOom *aTest);
	~COomRequestEvents();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
	};

class COomSpriteBase : public COomSetup
	{
public:
	COomSpriteBase(CTOom *aTest);
	~COomSpriteBase();
	void ConstructL();
protected:
	RWindow iWin;
	CFbsBitmap *iBitmap;
	CFbsBitmap *iMask;
	CFbsBitmap *iBitmap2;
	CFbsBitmap *iMask2;
	};

class COomSprite : public COomSpriteBase
	{
public:
	COomSprite(CTOom *aTest);
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	RWsSprite iSprite;
	};

class COomPointerCursor : public COomSpriteBase
	{
public:
	COomPointerCursor(CTOom *aTest);
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	RWsPointerCursor iPointerCursor;
	};

class COomCustomTextCursor : public COomSetup
	{
public:
	COomCustomTextCursor(CTOom *aTest);
	~COomCustomTextCursor();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	CArrayFixFlat<TSpriteMember>* iMemberArray;
	CFbsBitmap* iBitmap;
	};

class COomTranspWindow : public COomSetup
	{
public:
	COomTranspWindow(CTOom *aTest);
	~COomTranspWindow();
	void ConstructL();
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
	CTransWindow* CreateTransWinL(CTWinBase* aParent,TRgb aColor,TRect aPos, TDisplayMode* aDisplayMode);
private:
	TInt SetTransparencyTesting();

	CBlankWindow* iBackgroundWin;
	CTransWindow* iFirst;
	CTransWindow* iSecond;
	
	CFbsBitmap* iFbsBitmap;
	CWsBitmap* iWsBitmap;
	CFbsBitmapDevice* iFbsBitmapDevice; 
	CFbsBitGc* iFbsBitmapGc;
	CFbsBitmapDevice* iWsBitmapDevice; 
	CFbsBitGc* iWsBitmapGc;
	TBool iTransparencyEnabled;
	};


class CWindowWithOneLine : public CTWin
	{
public:
	static CWindowWithOneLine* NewL(CTWinBase& aParent, const TRect& aExtent);
	void Draw();
	};
	
class COomObscuredWindow : public COomSetup
	{
public:
	COomObscuredWindow(CTOom *aTest);
	void ClearUpL();
	TInt Fail();
	TOomTestName TestName();
private:
	void DoFailL();
	};

class CTOom : public CTWsGraphicsBase
	{
public:
	CTOom(CTestStep* aStep);
	~CTOom();
	const TDesC& TestName() const;
	void ConstructL();
	void DoOomTestL(COomFailBase *aOomTest);
	inline TInt ScreenNumber(){ return iTest->ScreenNumber();}

protected:
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
	TInt iOldCount;
	RBlankWindow iShieldWin;
	TSize iWinSize;
	TInt iState;
	};

class CTOomStep : public CTGraphicsStep
	{
public:
	CTOomStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTOomStep,"TOom");


#endif
