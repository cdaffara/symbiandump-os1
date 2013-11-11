// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <w32stdgraphic.h>
#include <imageconversion.h>
#include "testbase.h"
#include "testbase.h"

// Bitmap to load for tests
#define MY_TEST_BITMAP _L("Z:\\WSTEST\\MYTEST.MBM")

// Animation to load for tests
_LIT(KSymBallFile, "Z:\\WSTEST\\symball.gif");

// Executables for different sharing of graphic tests
_LIT(KTestExe1, "TWSGRAPHICSHARETEST.exe");
_LIT(KTestExe2, "TWSGRAPHICSHAREGLOBALTEST.exe");
_LIT(KTestExe3, "TWSGRAPHICUNSHAREGLOBALTEST.exe");
_LIT(KTestExe4, "TWSGRAPHICSHARESECURETEST.exe");
_LIT(KTestExe5, "TWSGRAPHICUNSHARESECURETEST.exe");

// Graphic is shared or not in executeable
_LIT(KShare, " true");
_LIT(KNoShare, " false");

TUid KUidTestAnimation = {0x87654321};
const TInt KDummyGraphicId = 99;
const TInt KMaxLogLength = 256;
const TInt KAnimationRunTime = 5000000; // 5 seconds max time to run a single animation loop

// Animation loader
class CIclLoader: public CActive
	{
public:
	CIclLoader();
	~CIclLoader();
	void ConstructL(const TDesC& aFileName, TBool aUseUID, TBool aReplace);
	const TWsGraphicId GetId();	
	inline TInt FrameCount() const {return iTotalFrames;};
	inline TBool Ok() const {return !iFailed;};
protected:
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	void TestL(TInt aCondition);
	CImageDecoder* iDecoder;
	CWsGraphicBitmapAnimation* iTestAnimation;
	TLogMessageText iTestLog;
	TBool iUseUID;
	TBool iReplace;
	RPointerArray<CWsGraphicBitmapAnimation::CFrame> iFrames;
	TInt iTotalFrames;
	void NextL();
	RFs iFs;
	TBool iFailed;
	};
	
CIclLoader::CIclLoader():
	CActive(CActive::EPriorityLow)
	{
	CActiveScheduler::Add(this);
	}

CIclLoader::~CIclLoader()
	{
	if (iTestAnimation)
		{
		delete iTestAnimation;
		iTestAnimation = NULL;
		}
	if (iDecoder)
		{
		delete iDecoder;
		iDecoder = NULL;
		}
	iFrames.ResetAndDestroy();
	iFs.Close();
	}
	
const TWsGraphicId CIclLoader::GetId()
	{
	if (iTestAnimation)
		{
		return iTestAnimation->Id();
		}
	else
		{
		TWsGraphicId id(KDummyGraphicId);
		return id;
		}
	}
	
void CIclLoader::TestL(TInt aCondition)
	{
	if(!aCondition)
		{
		RWsSession rWs;
		User::LeaveIfError(rWs.Connect());
		TBuf<KMaxLogLength> buf;
		_LIT(Fail,"AUTO Failed in WsGraphics Test : CIclLoader");
		buf.Append(Fail);
		buf.Append(iTestLog);
		rWs.LogMessage(buf);
		rWs.Flush();
		rWs.Close();
		User::Leave(KErrGeneral);
		}
	}

void CIclLoader::ConstructL(const TDesC& aFileName, TBool aUseUID,TBool aReplace)
	{
	iUseUID = aUseUID;
	iReplace = aReplace;
	
	User::LeaveIfError(iFs.Connect());
		
	iDecoder = CImageDecoder::FileNewL(iFs,aFileName);
	if(!iDecoder->IsImageHeaderProcessingComplete()) 
		{
		User::Leave(KErrGeneral);
		}
	NextL();
	}
	
void CIclLoader::NextL()
	{
	// Load a frame from the animation
	if (iDecoder && (iDecoder->FrameCount() > iFrames.Count()))
		{
		const TFrameInfo& info = iDecoder->FrameInfo(iFrames.Count());
		CWsGraphicBitmapAnimation::CFrame* frame = CWsGraphicBitmapAnimation::CFrame::NewL();
		CleanupStack::PushL(frame);
		iFrames.AppendL(frame);
		CleanupStack::Pop(frame);
		frame->SetFrameInfo(info);
		TFrameInfo copiedInfo = frame->FrameInfo();
		TestL(info.iFlags==copiedInfo.iFlags);
		
		TSize bmpSize(info.iFrameCoordsInPixels.Size());
		CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
		frame->SetBitmap(bitmap); //takes ownership
		User::LeaveIfError(bitmap->Create(bmpSize,info.iFrameDisplayMode));
		
		TDisplayMode maskDispMode;
		CFbsBitmap* mask = new(ELeave) CFbsBitmap;
		frame->SetMask(mask); //takes ownership
		if((TFrameInfo::EAlphaChannel|TFrameInfo::ETransparencyPossible) & info.iFlags)
			{
			maskDispMode = EGray256;
			}
		else
			{
			maskDispMode = EGray2;
			}
		
		User::LeaveIfError(mask->Create(info.iFrameCoordsInPixels.Size(),maskDispMode));
		iDecoder->Convert(&iStatus,*bitmap,*mask,iFrames.Count()-1);
	
		SetActive();
		}
		
	// if a frame loaded
	else if(iFrames.Count())
		{
		_LIT_SECURE_ID(KTestSecId,0x12345678);
		
		// The extra code around the NewL is checking that no heap failures occur when 
		// creating the CWsGraphicBitmapAnimation
		TInt failRate = 1;
		const TInt KMaxIteration = 1000;
		for (;failRate < KMaxIteration; failRate++)
		    {
		    __UHEAP_RESET;
		    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		    __UHEAP_MARK;

		    TInt err = KErrGeneral;
			if (iUseUID)
				{// creating animation using UID
				TRAP(err, iTestAnimation = CWsGraphicBitmapAnimation::NewL(KUidTestAnimation,iFrames.Array()););
				}
			else
				{// creating using transient ID allocated by wserv
				TRAP(err, iTestAnimation = CWsGraphicBitmapAnimation::NewL(iFrames.Array()););
				}
			
			TestL((err==KErrNone || err==KErrNoMemory));
			
			if (err != KErrNone)
		        {
		        __UHEAP_MARKEND;
		        TestL(iTestAnimation == NULL);
		        }
		    else
		        {
		        break;
		        }
			}
		__UHEAP_RESET;
		TestL(iTestAnimation != NULL); 
		TestL(failRate > 1); //Ensure the udeb version of euser.dll is available (i.e. that the rom was build with the -D_DEBUG option)
		RDebug::Printf("TWSGraphicTest.CPP: Heapfailure loop completed after %d allocs.", failRate-1);
			
		// if testing that a created animation can be replaced	
		if (iReplace)
			{	
			// replace the animation just created with another
			TWsGraphicId testId = iTestAnimation->Id();
			TInt testInt = testId.Id();  
	    
	   		CWsGraphicBitmapAnimation* testReplacement = CWsGraphicBitmapAnimation::NewL(testId,iFrames.Array());
	
			delete iTestAnimation;

			iTestAnimation = testReplacement;
		
			TestL(iTestAnimation->Id().Id()==testInt); 
			}
			
		delete iDecoder;
		iDecoder = NULL;
		iTotalFrames = iFrames.Count();
		iFrames.ResetAndDestroy();
		
		// test that the animation methods can be used without error
		TestL(iTestAnimation->ShareGlobally()==KErrNone);
		TestL(iTestAnimation->UnShareGlobally()==KErrNone);
		TestL(iTestAnimation->Share(KTestSecId)==KErrNone);
		TestL(iTestAnimation->UnShare(KTestSecId)==KErrNone);
		TestL(iTestAnimation->UnShare(KTestSecId)==KErrNotFound);	
		}
	}

void CIclLoader::RunL()
	{
	if (iStatus == KErrNone)
		{
		NextL();
		}
	else
		{
		TestL(EFalse); // kill the test
		}
	}

TInt CIclLoader::RunError(TInt aError)
	{
	RDebug::Printf("CIclLoader::RunError, aError %d", aError);
	iFailed = ETrue;
	return KErrNone;
	}

void CIclLoader::DoCancel()
	{
	if(iDecoder)
		{
		iDecoder->Cancel();
		}
	}

// Class for testing CWsGraphics
class CActiveWait;
class CRedrawAO;
class CWsGraphicBase : public CBase
	{
public:
	CWsGraphicBase();
	CWsGraphicBase(TInt aScreenNumber);
	~CWsGraphicBase();
	void ConstructL();
	void DoTestL(TInt aTestNo);
	void RedrawMe(TRect aRedrawRect, TInt aFrame);

	enum TTestCases
		{
		ETestCreateGraphicUID,
		ETestCreateGraphicID,
		ETestUpdateGraphic,
		ETestDeleteGraphic,
		ETestDrawInvalideBitmapID,
		ETestDrawGraphic,
		ETestDrawGraphicID,
		ETestDrawGraphicCompare,
		ETestDrawGraphicSessionHandle,
		ETestDrawAnimatedGraphicUID,
		ETestDrawAnimatedGraphicID,
		ETestCreateMsgGraphicMsgBuf,
		ETestDrawReplaceGraphicID,
		ETestDrawInvalidAnimationID,
		ETestDrawSharedGraphic,
		// additional cases to be added here, before ETestMaxNumberOfTests
		ETestMaxNumberOfTests
		};

private :
	void PrepGc();
	void RetireGc();
	void RunAnimation(TInt aFrameCount);
	void LaunchNewProcessL(const TDesC& aExecutable, TBool aShare);
	inline void TestForIdenticalBitmaps(){Test(iScreen->RectCompare(iPosition1,iPosition2));};
	inline void TestForDifferentBitmaps(){Test(!iScreen->RectCompare(iPosition1,iPosition2));};
	void Test(TInt aCondition);
	void DoTestCreateGraphicUidL();	
	void DoTestCreateGraphicIdL();
	void DoTestUpdateGraphicL();
	void DoTestDrawSharedGraphicL();
	void DoTestDeleteGraphicL();
	void DoTestDrawGraphicL();
	void DoTestDrawGraphicIDL();
	void DoTestDrawGraphicCompareL();
	void DoTestDrawAnimatedGraphicUIDL();
	void DoTestDrawAnimatedGraphicIDL();
	void DoTestDrawGraphicSessionHandleL();
	void DoTestCreateMsgGraphicMsgBufL();
	void DoTestDrawReplaceGraphicIDL();
	void DoTestDrawInvalidBitmapIDL();
	void DoTestDrawInvalidAnimationIDL();
private :
	TInt iScreenNumber;
	CWindowGc *iGc;
	RWsSession iWs;
	RWindowGroup *iGroupWin;
	CWsScreenDevice *iScreen;
	RWindow *iWin;	
	TLogMessageText iTestLog;
	TRect iPosition1;
	TRect iPosition2;
	CActiveWait* iTimer;
	CRedrawAO* iRedrawListener;
	TWsGraphicId iAnimId;
	TWsGraphicAnimation iAnimData;
	};

// 
// class CRedrawAO
// request & listen for redraw events from wserv
// if a redraw event is received, notify the observing class
// 
class CRedrawAO : public CActive
	{
public:
	static CRedrawAO* NewL(RWsSession* aWs);
	~CRedrawAO();
	// from CActive:
	void RunL();
	void DoCancel();	
	TInt RunError(TInt aError);
	void RequestRedraw();
	inline void SetFrameCount(TInt aCount){iFrameCount = aCount;};
	inline TInt GetFrameCount() const {return iFrameCount;};
private:
	CRedrawAO(RWsSession* aWs);
	void ConstructL();
private:
	RWsSession* iWs;
	TInt iFrameCount;
	};

CRedrawAO* CRedrawAO::NewL(RWsSession* aWs)
	{
	CRedrawAO* self = new (ELeave) CRedrawAO(aWs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

CRedrawAO::CRedrawAO(RWsSession* aWs):
CActive(CActive::EPriorityHigh), iWs(aWs)
	{
	CActiveScheduler::Add(this);
	}

CRedrawAO::~CRedrawAO()
	{
	// cleanup 
	Cancel();
	}

void CRedrawAO::ConstructL()
	{
	// nothing to construct 
	}

void CRedrawAO::RunL()
	{
	// leave if status is not ok. RunError will process this result
	User::LeaveIfError(iStatus.Int());
	TWsRedrawEvent redraw;
	iWs->GetRedraw(redraw);
	TUint redrawHandle = redraw.Handle();
	if (redrawHandle == ENullWsHandle)
		{
		User::Leave(KErrBadHandle); // sanity check the client handle isn't a dummy 
		}
	else if (redrawHandle)
		{
		--iFrameCount;
		(reinterpret_cast<CWsGraphicBase *>(redrawHandle))->RedrawMe(redraw.Rect(), iFrameCount); // handle the redraw signal
		}
	
	if (iFrameCount > 0) 
		{
		RequestRedraw();
		}
	}

TInt CRedrawAO::RunError(TInt aError)
	{
	if (aError != KErrBadHandle)
		{
		RequestRedraw();
		}
	return KErrNone;
	}

void CRedrawAO::DoCancel()
	{
	// kill all outstanding asynch. wserv requests
	iWs->RedrawReadyCancel();
	iFrameCount = KErrNone;	
	}

void CRedrawAO::RequestRedraw()
	{
	if (!IsActive())
		{
		iWs->RedrawReady(&iStatus);
		SetActive();
		}
	}

//

class CActiveWait : public CActive
	{
public:
	static CActiveWait* NewL();
	~CActiveWait();
	void Wait(TInt aDelay);
	// From CActive:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
protected:
	CActiveWait();
	void ConstructL();
protected:
	RTimer iTimer;
	TTime iFromTime;
	};

CActiveWait* CActiveWait::NewL()
	{
	CActiveWait* self = new (ELeave) CActiveWait;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CActiveWait::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
	}
	
CActiveWait::CActiveWait() : CActive(CActive::EPriorityStandard)
	{
	iFromTime.HomeTime();
	}

CActiveWait::~CActiveWait()
	{
	Cancel();
	iTimer.Close();
	}

void CActiveWait::DoCancel()
	{
	iTimer.Cancel();
	CActiveScheduler::Stop();
	}

void CActiveWait::RunL()
	{
	CActiveScheduler::Stop();
	}
	
TInt CActiveWait::RunError(TInt aError)
	{
	return aError; // exists so a break point can be placed on it.
	}

void CActiveWait::Wait(TInt aDelay)


	{
	iTimer.After(iStatus, aDelay);
	SetActive();
	CActiveScheduler::Start();
	}
	
//

CWsGraphicBase::CWsGraphicBase(TInt aScreenNumber) : iScreenNumber(aScreenNumber), iAnimId(KDummyGraphicId)
	{
	}
	
CWsGraphicBase::~CWsGraphicBase() 
	{
	iWin->Close();
	delete iWin;
	delete iScreen;
	delete iGc;
	delete iGroupWin;
	iWs.Close();
	if (iTimer)
		{
		delete iTimer;
		iTimer = NULL;
		}
	if (iRedrawListener)
		{
		delete iRedrawListener;
		iRedrawListener = NULL;
		}
	}
	
void CWsGraphicBase::ConstructL()
	{
	User::LeaveIfError(iWs.Connect());
	iScreen=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScreen->Construct(iScreenNumber));

	TSize screenSize = iScreen->SizeInPixels();
	iPosition1.SetRect(0,0,screenSize.iWidth/2,screenSize.iHeight);
	iPosition2.SetRect(screenSize.iWidth/2,0,screenSize.iWidth,screenSize.iHeight);

	iTimer = CActiveWait::NewL();
	iRedrawListener = CRedrawAO::NewL(&iWs);

	iGc=new(ELeave) CWindowGc(iScreen);
	User::LeaveIfError(iGc->Construct());
	iGroupWin=new(ELeave) RWindowGroup(iWs);
	iGroupWin->Construct(1);
		
	iWin=new(ELeave) RWindow(iWs);
	iWin->Construct(*iGroupWin, (TUint32)this);
	iWin->EnableRedrawStore(EFalse);	// disable the redraw store for these tests
	iWin->SetRequiredDisplayMode(EColor256);
	iWin->SetExtent(TPoint(0,0),iScreen->SizeInPixels());
	iWin->Activate();
	iWin->BeginRedraw();
	iWin->EndRedraw();
	iWs.Flush();
	}
	
// To test whether sharing of graphics works a new process has to be launched.
// The argument is set whether the graphic should be shared or not.	
void CWsGraphicBase::LaunchNewProcessL(const TDesC& aExecutable, TBool aShare)
	{
	TBuf<128> args;
	RProcess pr;
	TRequestStatus status;
	
	if (aShare)
		{
		args.Append(KShare);
		}
	else
		{
		args.Append(KNoShare);
		}
		
	User::LeaveIfError(pr.Create(aExecutable,args));
	pr.Logon(status);
	pr.Resume();
	User::WaitForRequest(status);
	pr.Close();
	
	if (status != KErrNone)
		{
		User::Leave(status.Int());
		}
	}

//
// CWsGraphicBase::PrepGc
// activate a gc & clear the two rects
//
void CWsGraphicBase::PrepGc()
	{
	iGc->Activate(*iWin);
	iWin->Invalidate();
	iWin->BeginRedraw();
	iGc->Clear(iPosition1);
	iGc->Clear(iPosition2);
	iWs.Flush();
	}

//
// CWsGraphicBase::RetireGc
// deactivate a gc & flush any outstanding RWindow requests
void CWsGraphicBase::RetireGc()
	{
	iGc->Deactivate();
	iWin->EndRedraw();
	iWs.Flush();
	}

//
// CWsGraphicBase::RedrawMe
// called from the redraw listener AO, triggered by a redraw event
// Invalidates the area requiring a redraw & 
// initiates a redraw of the CWsGraphicBitmapAnimation's window
// 
void CWsGraphicBase::RedrawMe(TRect aRedrawRect, TInt aFrame)
	{
	// do draw with next frame
	if (iAnimData.IsPlaying())
		{
		iGc->Activate(*iWin);
		iWin->Invalidate(aRedrawRect);
		iWin->BeginRedraw();
		iWs.Flush();
		iGc->DrawWsGraphic(iAnimId,iPosition1,iAnimData.Pckg());
		iGc->Deactivate();
		iWin->EndRedraw();
		iWs.Flush();
		
		// check for last frame
		if (aFrame == 0)
			{
			iTimer->Cancel();
			}
		}
	}
			
/**
   @SYMTestCaseID GRAPHICS-WSERV-0001
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Create Globally and Locally Shared Graphic Bitmaps from UIDs.
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions First test that TWsGraphicIds can be created from UIDs. Then create CWsGraphicBitmap objects through 
   	CWsGraphicBitmap::NewL, passing a UID from a TWsGraphicId. Two different objects are created
		1.	Globally shared available to all applications
		2.	Locally shared available to selected clients
   
   @SYMTestExpectedResults The CWsGraphicBitmap objects are created and no errors are reported.
 */	
void CWsGraphicBase::DoTestCreateGraphicUidL()
	{
	iTestLog.Append(_L("CreateGraphicUid"));
	
	_LIT_SECURE_ID(KTestSecId,0x12345678);
			
	// Test the creation of TWsGraphicIds from UIDs
	TUid uid1 = {0x10000001};
	TUid uid2 = {0x10000002};
	 
	TWsGraphicId twsGraphicId1(uid1);
	Test(twsGraphicId1.Uid()==uid1);
	
	TWsGraphicId twsGraphicId2(uid2);
	Test(twsGraphicId2.Uid()==uid2);
	
	TWsGraphicId twsGraphicId3(twsGraphicId2);
	Test(twsGraphicId3.Uid()==uid2);
	
	TWsGraphicId twsGraphicId4(1);
	twsGraphicId4.Set(uid1);
	Test(twsGraphicId4.Uid()==uid1);		
	
	// Create globally shared CWsGraphicBitmap		
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;		
	
	TSize screenSize = iScreen->SizeInPixels();
	bitmap1.Create(screenSize,iScreen->DisplayMode());
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
	CWsGraphicBitmap* bTest = CWsGraphicBitmap::NewL(twsGraphicId1.Uid(), &bitmap1,&mask1);		
	Test(bTest->IsActive());
	
	TWsGraphicId tid1 = bTest->Id();
	Test(tid1.Uid()==uid1);
	
	Test(bTest->ShareGlobally()==KErrNone);
	
	// Create local shared CWsGraphicBitmap		
	CFbsBitmap bitmap2;
	CFbsBitmap mask2;		
	
	bitmap2.Create(screenSize,iScreen->DisplayMode());
	mask2.Create(bitmap2.SizeInPixels(),iScreen->DisplayMode());
	
	CWsGraphicBitmap* bTest2 = CWsGraphicBitmap::NewL(twsGraphicId2.Uid(), &bitmap2,&mask2);		
 
	TWsGraphicId tid2 = bTest2->Id();
	Test(tid2.Uid()==uid2);
	
	Test(bTest2->Share(KTestSecId)==KErrNone);
	
	// Test the unsharing of the CWsGraphicBitmaps
	Test(bTest->UnShareGlobally()==KErrNone);
	Test(bTest2->UnShare(KTestSecId)==KErrNone);
	Test(bTest2->UnShare(KTestSecId)==KErrNotFound);
	
	delete bTest;	
	delete bTest2;
	}
	
/**
   @SYMTestCaseID GRAPHICS-WSERV-0002
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Create Globally and Locally Shared Graphic Bitmaps.
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions First test that TWsGraphicIds can be created from IDs. Then create CWsGraphicBitmap objects through 
   	CWsGraphicBitmap::NewL. Two different objects are created
		1.	Globally shared available to all applications
		2.	Locally shared available to selected clients
   
   @SYMTestExpectedResults The CWsGraphicBitmap objects are created and no errors are reported.
 */
void CWsGraphicBase::DoTestCreateGraphicIdL()
	{
	iTestLog.Append(_L("CreateGraphicId"));
 		
	_LIT_SECURE_ID(KTestSecId,0x12345678);
	
	// Test creating TWsGraphicIds from ids first
	TUid uid1 = {0x10000001};
	 
	TWsGraphicId twsGraphicId1(uid1);
	
	twsGraphicId1.Set(9);
	Test(twsGraphicId1.Id()==9);
	
	TWsGraphicId twsGraphicId2(twsGraphicId1);
	Test(twsGraphicId2.Id()==9);
	
	TWsGraphicId twsGraphicId3(7);
	Test(twsGraphicId3.Id()==7);
	
	// Create globally shared CWsGraphicBitmap
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;

	TSize screenSize = iScreen->SizeInPixels();
	bitmap1.Create(screenSize,iScreen->DisplayMode());
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
	CWsGraphicBitmap* bTest = CWsGraphicBitmap::NewL(&bitmap1,&mask1);
	Test(bTest->IsActive());
	
	TWsGraphicId tid1 = bTest->Id();
	
	Test(bTest->ShareGlobally()==KErrNone);
	
	// Create local shared CWsGraphicBitmap	
	CFbsBitmap bitmap2;
	CFbsBitmap mask2;
	
	bitmap2.Create(screenSize,iScreen->DisplayMode());
	mask2.Create(bitmap2.SizeInPixels(),iScreen->DisplayMode());
 		
	CWsGraphicBitmap* bTest2 = CWsGraphicBitmap::NewL(&bitmap2,&mask2);
	
	TWsGraphicId tid2 = bTest2->Id();
	
	Test(bTest2->Share(KTestSecId)==KErrNone);
	
	// Test the unsharing of the CWsGraphicBitmaps
	Test(bTest->UnShareGlobally()==KErrNone);
	Test(bTest2->UnShare(KTestSecId)==KErrNone);
	Test(bTest2->UnShare(KTestSecId)==KErrNotFound);
 	
	delete bTest2;	
	delete bTest;		
	}
	
	/**
   @SYMTestCaseID GRAPHICS-WSERV-0003
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Update an existing graphic bitmap with new data.
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test calls CWsGraphicBitmap::NewL method with new data passed to the CWsGraphicBitmap object. 
   		
   
   @SYMTestExpectedResults The CWsGraphicBitmap object is updated with no errors reported.
 */
void CWsGraphicBase::DoTestUpdateGraphicL()
	{
	iTestLog.Append(_L("UpdateGraphic"));
 		
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;
	CFbsBitmap bitmap2;
	CFbsBitmap mask2;
	
	TSize screenSize = iScreen->SizeInPixels();
	bitmap1.Create(screenSize,iScreen->DisplayMode());
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
 	
	CWsGraphic* bTest = CWsGraphicBitmap::NewL(&bitmap1,&mask1);

	bitmap2.Create(screenSize,iScreen->DisplayMode());
	mask2.Create(bitmap2.SizeInPixels(),iScreen->DisplayMode());
 	
	TWsGraphicId tid1 = bTest->Id();
	TInt testInt = tid1.Id(); 
    
   	CWsGraphic* testReplacement = CWsGraphicBitmap::NewL(tid1, &bitmap2,&mask2);

	delete bTest;
	bTest = testReplacement;
	
	Test(bTest->Id().Id()==testInt); 
	
	delete bTest;		
	}
	

	
	/**
   @SYMTestCaseID GRAPHICS-WSERV-0004
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Try to delete an existing graphic.
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app calls CWsGraphic::Destroy() method, 
   
   @SYMTestExpectedResults The CWsGraphicBitmap object is removed from the Window Server with no 
   		errors reported
 */
void CWsGraphicBase::DoTestDeleteGraphicL()
	{
	iTestLog.Append(_L("DeleteGraphic"));
 		
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;
	
	TSize screenSize = iScreen->SizeInPixels();
	bitmap1.Create(screenSize,iScreen->DisplayMode());
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
	CWsGraphic* bTest = CWsGraphicBitmap::NewL(&bitmap1,&mask1);
	
	bTest->Destroy();
	
	Test(!bTest->IsActive());
 
	delete bTest; 
	}
	
		/**
   @SYMTestCaseID GRAPHICS-WSERV-0005
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Check a bitmap is not drawn if the bitmap and mask it uses are invalid
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app creates a valid and invalid bitmap and attempts to draw them
   
   @SYMTestExpectedResults The valid bitmap is drawn but the invalid bitmap is not drawn
 */	
void CWsGraphicBase::DoTestDrawInvalidBitmapIDL()
	{
	iTestLog.Append(_L("DrawInvalidBitmapID"));
 		
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;
	CFbsBitmap *bitmap2 = NULL;
	CFbsBitmap *mask2 = NULL;
	
	User::LeaveIfError(bitmap1.Load(MY_TEST_BITMAP,0));
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
	// valid bitmap
	CWsGraphicBitmap* bTest = CWsGraphicBitmap::NewL(&bitmap1,&mask1);
	
	// invalid bitmap
	CWsGraphicBitmap* bTest2 = CWsGraphicBitmap::NewL(bitmap2,mask2);
 	
	PrepGc();
	iGc->DrawWsGraphic(bTest->Id(),iPosition1);
	iGc->DrawWsGraphic(bTest2->Id(),iPosition2);
	RetireGc();
	
	// compare the graphic in both positions, should only be graphic in position 1
	TestForDifferentBitmaps();
	
	delete bTest2;
	delete bTest;
	}
	
		/**
   @SYMTestCaseID GRAPHICS-WSERV-0006
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Draw a graphic within a rectangle on the screen, then try to draw with a non-existant graphic
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app calls CWindowGC::DrawWsGraphic() method using the TWGraphicId object, to draw within a rectangle on the screen
   		 
   @SYMTestExpectedResults The graphic is drawn on the screen with no errors reported. Drawing with a non-existant graphic causes
   		nothing to be drawn and no error reported
 */
void CWsGraphicBase::DoTestDrawGraphicL()
	{
	iTestLog.Append(_L("DrawGraphic"));
	
	_LIT8(KTestData,"HelloWorld");
	
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;

	User::LeaveIfError(bitmap1.Load(MY_TEST_BITMAP,0));
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
	CWsGraphicBitmap* bTest = CWsGraphicBitmap::NewL(&bitmap1,&mask1);

	PrepGc();	
	iGc->DrawWsGraphic(bTest->Id(),iPosition1,KTestData);
	TWsGraphicId twsGraphicId1(KDummyGraphicId); // create unrecognised wsbitmap id & attempt to draw it
	iGc->DrawWsGraphic(twsGraphicId1,iPosition2,KTestData);
	RetireGc();

	// compare the graphic in both positions, should only be graphic in position 1
	TestForDifferentBitmaps();
	
	delete bTest;  	
	}
	
			/**
   @SYMTestCaseID GRAPHICS-WSERV-0007
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Draw a graphic using a transient ID within a rectangle on the screen
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app calls CWindowGC::DrawWsGraphic() using a TWsGraphic object, to draw within
   		a rectangle on the screen
   
   @SYMTestExpectedResults The graphic is drawn.
 */
void CWsGraphicBase::DoTestDrawGraphicIDL()
	{
	iTestLog.Append(_L("DrawGraphicID"));
		
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;

	User::LeaveIfError(bitmap1.Load(MY_TEST_BITMAP,0));
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
	CWsGraphicBitmap* bTest = CWsGraphicBitmap::NewL(&bitmap1,&mask1);
	 			
	PrepGc();
	iGc->DrawWsGraphic(bTest->Id(),iPosition1);
	RetireGc();
	
	// compare the graphic in both positions, should only be graphic in position 1
	TestForDifferentBitmaps();
 	
	delete bTest;
	}
	

	
			/**
   @SYMTestCaseID GRAPHICS-WSERV-0008
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Draw a graphic in two different rectangles on the screen and compare them
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app calls CWindowGC::DrawWsGraphic() using the TWsGraphic object, to draw a known bitmap
   		rectangle on the screen twice in different places. The bitmaps are then compared.
   
   @SYMTestExpectedResults The two bitmaps are identical
 */
void CWsGraphicBase::DoTestDrawGraphicCompareL()
	{
	iTestLog.Append(_L("DrawGraphicCompare"));

	_LIT8(KTestData,"HelloWorld");
	
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;
	
	CFbsBitmap bitmap2;
	CFbsBitmap mask2;
	
	User::LeaveIfError(bitmap1.Load(MY_TEST_BITMAP,0));
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
	User::LeaveIfError(bitmap2.Load(MY_TEST_BITMAP,0));
	mask2.Create(bitmap2.SizeInPixels(),iScreen->DisplayMode());

	CWsGraphicBitmap* bTest = CWsGraphicBitmap::NewL(&bitmap1,&mask1);
	CWsGraphicBitmap* bTest2 = CWsGraphicBitmap::NewL(&bitmap2,&mask2);
	
	PrepGc();
	//draw the graphic in the two different rectangles	
	iGc->DrawWsGraphic(bTest->Id(),iPosition1,KTestData);
	iGc->DrawWsGraphic(bTest2->Id(),iPosition2,KTestData);
	RetireGc();

	// compare the graphic in both positions. Contents of each rect should be identical
	TestForIdenticalBitmaps();
			
	delete bTest2;
	delete bTest;
	}
	
		/**
   @SYMTestCaseID GRAPHICS-WSERV-0009
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Draw a global and local graphic in two different rectangles on the screen and compare them
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app calls CWindowGC::DrawGraphic() using the TWsGraphic object, to draw a known 
   		global and local bitmap rectangle on the screen twice in different places. The bitmaps are then compared.
   
   @SYMTestExpectedResults The two bitmaps are identical
 */
void CWsGraphicBase::DoTestDrawGraphicSessionHandleL()
	{
	iTestLog.Append(_L("DrawGraphicSessionHandle"));
	_LIT_SECURE_ID(KTestSecId,0x12345678);
	
	// test TWsGraphicControlState first
	_LIT8(KTestData,"HelloWorld");

	CFbsBitmap bitmap1;
	CFbsBitmap mask1;
	
	CFbsBitmap bitmap2;
	CFbsBitmap mask2;
	
	User::LeaveIfError(bitmap1.Load(MY_TEST_BITMAP,0));
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
	User::LeaveIfError(bitmap2.Load(MY_TEST_BITMAP,0));
	mask2.Create(bitmap2.SizeInPixels(),iScreen->DisplayMode());
	
	CWsGraphicBitmap* bTest = CWsGraphicBitmap::NewL(&bitmap1,&mask1);
	CWsGraphicBitmap* bTest2 = CWsGraphicBitmap::NewL(&bitmap2,&mask2);
	
	Test(bTest->ShareGlobally()==KErrNone);
	Test(bTest2->Share(KTestSecId)==KErrNone);
	
	PrepGc();
	iGc->DrawWsGraphic(bTest->Id(),iPosition1,KTestData);
	iGc->DrawWsGraphic(bTest2->Id(),iPosition2,KTestData);	
	RetireGc();

	// compare the graphic in both positions. Contents of each rect should be identical
	TestForIdenticalBitmaps();
 		
	delete bTest2;
	delete bTest;
	}

	/**
   @SYMTestCaseID GRAPHICS-WSERV-0010
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Check an animation can be constructed using a UID, manipulated and then drawn
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app creates CWsGraphicBitmapAnimation object via a UID and then draws the object to the screen
   
   @SYMTestExpectedResults The object is drawn
 */	
void CWsGraphicBase::DoTestDrawAnimatedGraphicUIDL()
	{
	iTestLog.Append(_L("DrawAnimatedGraphicUID"));

	// test TWsGraphicAnimation first
	iAnimData.Play(EFalse);

	// load the animation via a UID
	CIclLoader*	iclLoader;		
	iclLoader = new(ELeave) CIclLoader();
	iclLoader->ConstructL(KSymBallFile,ETrue,EFalse); // this is actually an asynchronous operation, so we give it a chance to execute below

	while (iclLoader->Ok() && iclLoader->GetId().Id() == KDummyGraphicId)
		{
		iTimer->Wait(1000);
		}
	
	Test(iclLoader->Ok()); // fail test if iclLoder experienced problems
	
	iAnimId = iclLoader->GetId();
	
	// animation is ready to be drawn. draw to the 1st position only	
	PrepGc();
	iGc->DrawWsGraphic(iAnimId,iPosition1,iAnimData.Pckg());	
	RetireGc();
	
	// run the animation
	RunAnimation(iclLoader->FrameCount());

	// compare the graphic in both positions. 
	TestForDifferentBitmaps();
 	
	iTimer->Cancel();
	iclLoader->Cancel();
	delete iclLoader;
	}

	/**
   @SYMTestCaseID GRAPHICS-WSERV-0011
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Check an animation can be constructed using an ID, manipulated and then drawn
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app creates CWsGraphicBitmapAnimation object via an ID and then draws the object to the screen
   
   @SYMTestExpectedResults The object is drawn
 */	
void CWsGraphicBase::DoTestDrawAnimatedGraphicIDL()
	{
	iTestLog.Append(_L("DrawAnimatedGraphicID"));	
	iAnimData.Play(ETrue);
	iAnimData.Play(ETrue);
	Test(iAnimData.Loops());
	iAnimData.Pause();
	Test(!iAnimData.IsPlaying());
	Test(iAnimData.IsPaused());
	iAnimData.Pause();
	Test(iAnimData.Loops());
	iAnimData.Play(EFalse);
	Test(!iAnimData.Loops());
	Test(!iAnimData.IsStopping());
	Test(!iAnimData.IsStopped());		
 		
	// load the animation via an ID
	CIclLoader*	iclLoader;
	iclLoader = new(ELeave) CIclLoader();
	iclLoader->ConstructL(KSymBallFile,EFalse,EFalse);
	
	while (iclLoader->GetId().Id() == KDummyGraphicId)
		{
		iTimer->Wait(1000);
		}
	iAnimId = iclLoader->GetId();
	
	PrepGc();
	iGc->DrawWsGraphic(iAnimId,iPosition1,iAnimData.Pckg());	
	RetireGc();

	// run the animation
	RunAnimation(iclLoader->FrameCount());

	// compare the graphic in both positions
	TestForDifferentBitmaps();
	
	iAnimData.Stop(ETrue);
	Test(iAnimData.IsStopped());
	iAnimData.Pause();
	iAnimData.Play(EFalse);
	iAnimData.Stop(EFalse);
	Test(!iAnimData.IsStopped());

	iTimer->Cancel();	
	iclLoader->Cancel();
	delete iclLoader;
	}

	/**
   @SYMTestCaseID GRAPHICS-WSERV-0012
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Check an animation can be constructed and then replaced, manipulated and then drawn
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app creates CWsGraphicBitmapAnimation object, the replaces it, and then draws the object 
   		to the screen
   
   @SYMTestExpectedResults The object is drawn
 */		
void CWsGraphicBase::DoTestDrawReplaceGraphicIDL()
	{
	// test TWsGraphicControlStateTimed first
	iTestLog.Append(_L("DrawAnimatedGraphicID"));	
	_LIT8(KTestData,"HelloWorld");
	iAnimData.Stop(ETrue);
			
	// load and replace the animation 	
	CIclLoader*	iclLoader;
	iclLoader = new(ELeave) CIclLoader();
	iclLoader->ConstructL(KSymBallFile,false,true);
	
	while (iclLoader->GetId().Id() == KDummyGraphicId)
		{
		iTimer->Wait(1000);
		}
	iAnimId = iclLoader->GetId();

	// draw the animation in two positions
	PrepGc();
	iGc->DrawWsGraphic(iAnimId,iPosition1,KTestData);	
	RetireGc();
	
	// run the animation
	RunAnimation(iclLoader->FrameCount());

	// compare the graphic in both positions
	// Expect identical, as the command buffer used in position1 animation is invalid, therefore never drawn
	TestForIdenticalBitmaps();
	
	iTimer->Cancel();
	iclLoader->Cancel();
	delete iclLoader;			
	}

	/**
   @SYMTestCaseID GRAPHICS-WSERV-0013
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Check the creation and manipulation of an RWsGraphicMsgBuf object
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions Creates and manipulates an RWsGraphicMsgBuf object
   
   @SYMTestExpectedResults RWsGraphicMsgBuf functions correctly
 */		
void CWsGraphicBase::DoTestCreateMsgGraphicMsgBufL()
	{
	iTestLog.Append(_L("CreateMsgGraphicMsgBuf"));
	
	_LIT(KNebraska,"Nebraska");
	_LIT8(KTesting,"Testing");
	RWsGraphicMsgBuf msgBuf;
	msgBuf.CleanupClosePushL();
	msgBuf.Append(TUid::Uid(0x12345670),KTesting());
	msgBuf.Append(TUid::Uid(0x12345671),KNebraska());
	msgBuf.Append(TUid::Uid(0x12345670),KTesting());
	
	Test(TUid::Uid(0x12345670)==msgBuf.TypeId(0));

	msgBuf.Remove(0);
	const TInt count = msgBuf.Count();
	Test(count == 2);
			
	iAnimData.Play(ETrue);
	msgBuf.Append(iAnimData);
	Test(msgBuf.Count() == 3);
	
	CleanupStack::Pop();
	
	// load the animation via a UID
	CIclLoader*	iclLoader;		
	iclLoader = new(ELeave) CIclLoader();
	iclLoader->ConstructL(KSymBallFile,true,false);

	while (iclLoader->GetId().Id() == KDummyGraphicId)
		{
		iTimer->Wait(1000);
		}
	iAnimId = iclLoader->GetId();

	PrepGc();
	iGc->DrawWsGraphic(iAnimId,iPosition1,msgBuf.Pckg());	
	RetireGc();
 	
	// run the animation
	RunAnimation(iclLoader->FrameCount());

	// compare the graphic in both positions
	TestForDifferentBitmaps();
 	
	iTimer->Cancel();
	iclLoader->Cancel();
	delete iclLoader;
	msgBuf.Close();
	}

	
	
	/**
   @SYMTestCaseID GRAPHICS-WSERV-0014
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Check an animation is not drawn if the command buffer it uses is invalid
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app creates CWsGraphicBitmapAnimation object then draws the animation using
   		a valid and invalid command buffer
   
   @SYMTestExpectedResults The animation is drawn while using the valid command buffer but not drawn
   		when the command buffer is invalid
 */		
void CWsGraphicBase::DoTestDrawInvalidAnimationIDL()
	{
	// test TWsGraphicControlStateTimed first, a valid command buffer
	iTestLog.Append(_L("DrawInvalidAnimationID"));	
	iAnimData.Play(ETrue);
	
	// invalid command buffer
	_LIT8(KTestData2,"12345678");
			
	// load and replace the animation 	
	CIclLoader*	iclLoader;
	iclLoader = new(ELeave) CIclLoader();
	iclLoader->ConstructL(KSymBallFile,false,false);
	
	while (iclLoader->GetId().Id() == KDummyGraphicId)
		{
		iTimer->Wait(1000);
		}
	iAnimId = iclLoader->GetId();

	PrepGc();
	iGc->DrawWsGraphic(iAnimId,iPosition1,iAnimData.Pckg());	
	iGc->DrawWsGraphic(iAnimId,iPosition2,KTestData2);
	RetireGc();
	
	// run the animation
	RunAnimation(iclLoader->FrameCount());

	// compare the graphic in both positions
	TestForDifferentBitmaps();

	iAnimData.Stop(ETrue);	
	iTimer->Cancel();
	iclLoader->Cancel();
	delete iclLoader;			
	}
	
	/**
   @SYMTestCaseID GRAPHICS-WSERV-0015
  
   @SYMPREQ	PREQ1246

   @SYMTestCaseDesc Check the sharing of graphics with other clients.
   
   @SYMTestPriority High
   
   @SYMTestStatus Implemented
   
   @SYMTestActions The test app creates CWsGraphicBitmap object an then tests the sharing of the object with
   			different clients
   
   @SYMTestExpectedResults The CWsGraphicBitmap object is shared correctly
 */
void CWsGraphicBase::DoTestDrawSharedGraphicL()
	{
	iTestLog.Append(_L("DrawSharedGraphic"));
	_LIT_SECURE_ID(KTestSecId,0x10003a4b);

	TUid uid1 = {0x12000021};
	TWsGraphicId twsGraphicId1(uid1);
	
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;
	
	User::LeaveIfError(bitmap1.Load(MY_TEST_BITMAP,0));
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
	CWsGraphicBitmap* bTest = CWsGraphicBitmap::NewL(twsGraphicId1.Uid(), &bitmap1,&mask1);
	
	// Try to draw the graphic in an client. Should fail as graphic is not shared 
	TRAPD(err, LaunchNewProcessL(KTestExe1, EFalse));
	Test(err == KErrNone);

	// Share the graphic globally and try to draw the graphic in an client. Should pass
	Test(bTest->ShareGlobally()==KErrNone);
	TRAP(err, LaunchNewProcessL(KTestExe2, ETrue));
	Test(err == KErrNone);
	
	// Unshare the graphic globally and try to draw the graphic in an client. Should fail
	Test(bTest->UnShareGlobally()==KErrNone);
	TRAP(err, LaunchNewProcessL(KTestExe3, EFalse));
	Test(err == KErrNone);
	
	// Share the graphic to a client and try to draw the graphic in the client. Should pass
	Test(bTest->Share(KTestSecId)==KErrNone);		
	TRAP(err, LaunchNewProcessL(KTestExe4, ETrue));
	Test(err == KErrNone);
	
	// Unshare the graphic to a client and try to draw the graphic in the client. Should fail
	Test(bTest->UnShare(KTestSecId)==KErrNone);
	TRAP(err, LaunchNewProcessL(KTestExe5, EFalse));
	Test(err == KErrNone);
			
	delete bTest;	
	}

	
void CWsGraphicBase::DoTestL(TInt aTestNo)
	{
	switch (aTestNo)
		{
		case ETestCreateGraphicUID:
			DoTestCreateGraphicUidL();
			break;
		case ETestCreateGraphicID:
			DoTestCreateGraphicIdL();
			break;
		case ETestUpdateGraphic:
			DoTestUpdateGraphicL();
			break;
		case ETestDeleteGraphic:
			DoTestDeleteGraphicL();
			break;
		case ETestDrawInvalideBitmapID:
			DoTestDrawInvalidBitmapIDL();
			break;
		case ETestDrawGraphic:
			DoTestDrawGraphicL();
			break;
		case ETestDrawGraphicID:
			DoTestDrawGraphicIDL();
			break;
		case ETestDrawGraphicCompare:
			DoTestDrawGraphicCompareL();
			break;
		case ETestDrawGraphicSessionHandle:
			DoTestDrawGraphicSessionHandleL();
			break;	
#ifdef _DEBUG
      // These tests require debug-only API to simulate OOM. Running
      // the tests in non-debug environments invalidates the tests.
		case ETestDrawAnimatedGraphicUID:
			DoTestDrawAnimatedGraphicUIDL();
			break;
		case ETestDrawAnimatedGraphicID:
			DoTestDrawAnimatedGraphicIDL();
			break;
		case ETestCreateMsgGraphicMsgBuf:
			DoTestCreateMsgGraphicMsgBufL();
			break;
		case ETestDrawReplaceGraphicID:
			DoTestDrawReplaceGraphicIDL();
			break;
		case ETestDrawInvalidAnimationID:
			DoTestDrawInvalidAnimationIDL();
			break;
#endif
		case ETestDrawSharedGraphic:
			DoTestDrawSharedGraphicL();
			break;
		}
	RDebug::Print(iTestLog);
	iTestLog.Delete(0,256);	
	}
	
// writes out to WSERV.log if error
void CWsGraphicBase::Test(TInt aCondition)
	{
	if(!aCondition)
		{
		TBuf<KMaxLogLength> buf;
		_LIT(Fail,"AUTO Failed in WsGraphics Test : ");
		buf.Append(Fail);
		buf.Append(iTestLog);
		RDebug::Print(buf);
		RProcess().Terminate(KErrGeneral);
		}
	}
	
//
// CWsGraphicBase::RunAnimation
// Redraw event listener is launched & the 
// animation is given a total of KAnimationRunTime (25 seconds) to run a single loop
// 
void CWsGraphicBase::RunAnimation(TInt aFrameCount)
	{
	--aFrameCount; // account for the fact the initial frame is already displayed
	iRedrawListener->SetFrameCount(aFrameCount);
	iRedrawListener->RequestRedraw();
	iTimer->Wait(KAnimationRunTime);
	if (iAnimData.IsPlaying())
		{
		iAnimData.Stop(ETrue);
		}
	iRedrawListener->Cancel();
	iAnimData.Stop(EFalse);
	Test(iRedrawListener->GetFrameCount() == 0); // ensure the animation ran through until last frame
	}

void MainL()
	{
	TInt testCount = KErrNone;

    //Read the argument from the command line for current screen number
	TBuf<256> commandLine;
	User::CommandLine(commandLine);
	TLex lex(commandLine);
	lex.NextToken();
	lex.SkipSpace();
	TInt screenNumber=(TInt)lex.Get();

	CActiveScheduler* activeScheduler=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);
	CleanupStack::PushL(activeScheduler);

    CWsGraphicBase testBase(screenNumber);
    testBase.ConstructL();
    
    // run through all the tests   
    while (testCount < CWsGraphicBase::ETestMaxNumberOfTests)
    	{
    	testBase.DoTestL(testCount);
    	testCount++;
    	}
    
	CleanupStack::PopAndDestroy(activeScheduler);
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanUpStack=CTrapCleanup::New();
	if(cleanUpStack==NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err, MainL());
	delete cleanUpStack;
	return(err);
	}
