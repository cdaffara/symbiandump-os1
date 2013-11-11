// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Auto test Animated DLL functions
// 
//

#include <e32std.h>
#include <w32std.h>
#include <w32adll.h>
#include <bitstd.h>
#include "AUTODLL.H"
#include "TAnimDraw.h"
#include "eventdd.h"
#include <gdi.h>
#include <fbs.h>

union TAnimArgUnion
	{
	const TAny* any;
	const TBool* Bool;
	const TInt* Int;
	const TPoint* Point;
	const TSize* Size;
	const TRect* Rect;
	const TSpriteMemberInfo* SpriteMemberInfo;
	const TShadowDrawTest* ShadowDrawTest;
	const TDrawTestScreen* DrawTestScreen;
	const TSyncTests* SyncTests;
	const TTimeChangeTest* TimeChangeTest;
	const TAnimRawEvent* AnimRawEvent;
	const TWindowGroupInfoParms* WindowGroupInfoParms;
	const TSetOrdinalParms* SetOrdinalParms;
	const TFrameData* FrameData;
	const TRawEvent* RawEvent;
	};

class CPanicAnim : public CWindowAnim
	{
	enum TSyncState
		{
		ESyncStateNull,
		ESyncStateStarting,
		ESyncStateCounting,
		ESyncLeave,
		ESyncStateTimeChange1,
		ESyncStateTimeChange2,
		ESyncStateTimeChange3
		};
	enum {ESyncCount=8};
public:
	~CPanicAnim();
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Animate(TDateTime *aDateTime);
	virtual void Redraw();
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	virtual void FocusChanged(TBool aState);
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	void StartSyncTest(MAnimGeneralFunctions::TAnimSync aSync);
	void StartTimeChangeTest();
	void DoLeaveInActiveCallback();
	static TInt LeaveInActiveCallback(TAny *aThis);
private:
	TRequestStatus *iStatusPtr;
	TRequestStatus *iStatusPtr2;
	TTime iTime;
	TInt iSyncCount;
	TSyncState iSyncState;
	MAnimGeneralFunctions::TAnimSync iSyncMode;
	TBool iLeaveInRedraw;
	CIdle* iActivePanicIdle;
	};

class CPanicAnim2 : public CWindowAnim
//
// Simply panics in the ConstructL()
//
	{
public:
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Animate(TDateTime *aDateTime);
	virtual void Redraw();
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	virtual void FocusChanged(TBool aState);
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
	};

class CAutoAnim3 : public CWindowAnim
//
// test drawing/redrawing
//
	{
public:
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Animate(TDateTime *aDateTime);
	virtual void Redraw();
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	virtual void FocusChanged(TBool aState);
	void Draw(const TRect &aRect);
	void DrawTestScreenL(const TDrawTestScreen* aParams);
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	TRequestStatus *iStatusPtr;
	CFbsFont *iFont;
	TRect iRect;
	TRect iBaseRect;
	TInt iMode;
	};

class CAutoAnimScrBack : public CWindowAnim
//
// test anim drawing behind a backed up window
//
	{
public:
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Animate(TDateTime *aDateTime);
	virtual void Redraw();
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	virtual void FocusChanged(TBool aState);
	void Draw();
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	TRect iRect;
	TInt iMode;
	};

class CTestSpriteAnim : public CSpriteAnim
	{
public:
	virtual void ConstructL(TAny *aArgs);
	virtual void Animate(TDateTime *aDateTime);
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	TBool CheckMember(const TSpriteMemberInfo& aMemInfo);
	void DrawMemberL(TInt aMember);
	void DrawMemberL();
	};

/**
Used by CAnimTimer below
*/
class MAnimTimerObserver
    {
public:
    virtual void DisplayNextFrameL()=0;
    };

/**
This class is used by CTestFreeTimerAnim to determine when to swap the current frame with the next one.
*/
class CAnimTimer : public CTimer
    {
public:
    ~CAnimTimer();
    static CAnimTimer* NewL(MAnimTimerObserver& aObserver);
    void RunL();
private:
    CAnimTimer(MAnimTimerObserver& aObserver);
    void ConstructL();
private:
    MAnimTimerObserver& iAnim;
    };

class CTestFreeTimerAnim : public CFreeTimerWindowAnim, public MAnimTimerObserver
	{
public:
    //from CBase 
    ~CTestFreeTimerAnim();
	//from CAnim
	void Animate(TDateTime *aDateTime);
	void Command(TInt aOpcode, TAny *aArgs);
	TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	//from CWindowAnim 
    void ConstructL(TAny *aArgs, TBool aHasFocus);
    void Redraw();
    void FocusChanged(TBool aState);
	//from MEventHandler
	TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
    void DisplayNextFrameL();
private:
    CAnimTimer* iTimer;
    TInt iFrameTime;
    TRgb iColour;
    TRect iRect;    
	};

class CEventTestAnimBase : public CWindowAnim
	{
	enum {EEventBufferSize=40};
public:
	virtual ~CEventTestAnimBase();
	//Pure virtual functions from CAnim
	virtual void Animate(TDateTime *aDateTime);
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	//Pure virtual functions from CWindowAnim
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Redraw();
	virtual void FocusChanged(TBool aState);
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
protected:
	void Fail();
	TBool IsIgnorableEvent(const TRawEvent& aRawEvent) const;
	TBool GetNextExpectedEvent(TAnimRawEvent& aRawEvent);
	TBool CompareEvents(const TRawEvent& aExpectedEvent, const TRawEvent& aActualEvent) const;
	TBool HandleRecursiveEvent(const TRawEvent& aRawEvent);

	TInt Error() const;
	TInt EventCount() const;
	void ResetEventCount();
	TInt TotalExpectedEvents() const;
	TInt AddEvent(const TAnimRawEvent* event);
private:
	virtual TInt AddExpectedEvent(const TAnimRawEvent* event)=0;
private:
	CCirBuf<TAnimRawEvent> iExpectedEvents;
	TInt iError;
	TInt iEventCount;
	};

class CEventTestAnim : public CEventTestAnimBase
	{
public:
	~CEventTestAnim();
	//Pure virtual functions from CWindowAnim overridden in CEventTestAnimBase
	void ConstructL(TAny* aArgs,TBool aHasFocus);
	//Pure virtual functions from CAnim overridden in CEventTestAnimBase
	TInt CommandReplyL(TInt aOpcode,TAny* aArgs);
	void Command(TInt aOpcode,TAny* aArgs);
private:
	TInt UnloadDeviceDriver();
	//Pure virtual function from CEventTestAnimBase
	TInt AddExpectedEvent(const TAnimRawEvent* event);
private:
	TBool iDevDriverLoaded;
	REventDD iLdd;
	};

class CEventPostingAnim : public CEventTestAnimBase
	{
public:
	~CEventPostingAnim();
	//Virtual functions from CEventTestAnimBase
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	//Virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	//Pure virtual function from CEventTestAnimBase
	virtual TInt AddExpectedEvent(const TAnimRawEvent* event);
	};

class CRemovableAnim : public CEventTestAnimBase
	{
public:
	~CRemovableAnim();
	//Virtual functions from CEventTestAnimBase
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	//Pure virtual function from CEventTestAnimBase
	virtual TInt AddExpectedEvent(const TAnimRawEvent* event);
private:
	TInt iLifetime;
	};

class CTimerTestAnim : public CWindowAnim
	{
public:
	~CTimerTestAnim();
	//Pure virtual functions from CAnim
	virtual void Animate(TDateTime *aDateTime);
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	//Pure virtual functions from CWindowAnim
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Redraw();
	virtual void FocusChanged(TBool aState);
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	TInt iTimerCount;
	TBool iExpectingTicks;
	TBool iFail;
	};

class CWinFunctionTestAnim : public CWindowAnim
	{
	//enum {EEventBufferSize=40};
public:
	~CWinFunctionTestAnim();
	//Pure virtual functions from CAnim
	virtual void Animate(TDateTime *aDateTime);
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	//Pure virtual functions from CWindowAnim
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Redraw();
	virtual void FocusChanged(TBool aState);
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	MAnimGeneralFunctionsWindowExtension* iWinFunctions;
	};

class CNotificationTestAnim : public CWindowAnim
	{
public:
	CNotificationTestAnim();
	~CNotificationTestAnim();
	//Pure virtual functions from CAnim
	virtual void HandleNotification(const TWsEvent& aEvent);
	virtual void Animate(TDateTime *aDateTime);
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	//Pure virtual functions from CWindowAnim
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Redraw();
	virtual void FocusChanged(TBool aState);
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	TFixedArray<TBool,2> iDSA;
	TBool iHeartbeatState;
	TBool iScreenDeviceChanged;
	TTime iLastAnimTime;
	};

class CTransAnim : public CWindowAnim
//
// Test drawing/redrawing in Transparent window.
//
	{
public:
	//Implementaion for pure virtual functions from CAnim
	void ConstructL(TAny* aArgs, TBool aHasFocus);
	void Animate(TDateTime* aDateTime);
	void Redraw();
	void Command(TInt aOpcode, TAny* aArgs);
	TInt CommandReplyL(TInt aOpcode, TAny* aArgs);
	void FocusChanged(TBool aState);
	TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	CFbsFont* iFont;
	TRect iRect;
	TInt iMode;
	TInt iDraw;
	TRgb iColor;
	TInt iCount;
	};

class CTestHandlerAnim : public CWindowAnim
	{
	public:
		~CTestHandlerAnim();
		void ConstructL(TAny* aArgs, TBool aHasFocus);
		void Redraw();
		void FocusChanged(TBool aState);
		TInt CommandReplyL(TInt aOpcode, TAny* aArgs);
		void Command(TInt aOpcode, TAny* aArgs);
		void Animate(TDateTime* aDateTime);
		TBool OfferRawEvent(const TRawEvent &aRawEvent);
	private:
	};

class CCoverageAnim : public CWindowAnim
/**
 This class adds functional coverage to CWsAnimGc. 
 The actual work is handled by the 'Draw' method which calls
 each method in turn.
 Mostly copied from CAutoAnimScrBack and CAutoAnim3.
 @SYMTestCaseID 
 @SYMPREQ 1841 Add Coverage tests.
 @SYMTestCaseDesc  Functional coverage.
 @SYMTestActions Run functional coverage test on CWsAnimGc.
 */
	{
public:
	~CCoverageAnim();
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Animate(TDateTime *aDateTime);
	virtual void Redraw();
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	virtual void FocusChanged(TBool aState);
	void Draw();
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	TRect iRect; /** rectangle used for all draw ops that require one */
	CFbsFont* iFont;
	};

class CMultiPointerAnim : public CWindowAnim
	{
	enum {EEventBufferSize=40};
public:
	virtual ~CMultiPointerAnim();
	//Pure virtual functions from CAnim
	virtual void Animate(TDateTime *aDateTime);
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	//Pure virtual functions from CWindowAnim
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Redraw();
	virtual void FocusChanged(TBool aState);
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
protected:
	TInt AddEvent(const TAnimRawEvent* aEvent);
	TBool CompareEvents(const TRawEvent& aExpectedEvent, const TRawEvent& aActualEvent);
private:
	CCirBuf<TAnimRawEvent> iExpectedEvents;
	TBuf<255> iErrorDes;
	TInt iEventCount;
	TInt iError;
	};

class CTestAnimDll : public CAnimDll
	{
public:
	CAnim *CreateInstanceL(TInt aType);
private:
	};

/*#if defined(__WINS__)
#pragma data_seg(".E32_UID")
__WINS_UID(0, KWservAnimDllUidValue, 0)
#pragma data_seg()
#endif*/

EXPORT_C CAnimDll *CreateCAnimDllL()
	{
	return(new(ELeave) CTestAnimDll());
	}


// Panic Tests //
void CPanicAnim::ConstructL(TAny *, TBool )
	{
	iWindowFunctions->SetRect(TRect(0,0,1,1));
	iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
	if (iFunctions->Sync()!=MAnimGeneralFunctions::ESyncSecond)
		User::Leave(KErrGeneral);
// Check the anim time is not ahead of and not more than 2 secs behind the system time
	TTime now;
	now.UniversalTime();
	TDateTime dt=now.DateTime();
	TDateTime adt=iFunctions->SystemTime();
	if (adt.Day()!=dt.Day())
		User::Leave(KErrGeneral);

	iFunctions->SetSync(MAnimGeneralFunctions::ESyncNone);
	if (iFunctions->Sync()!=MAnimGeneralFunctions::ESyncNone)
		User::Leave(KErrGeneral);
	}

CPanicAnim::~CPanicAnim()
	{
	delete iActivePanicIdle;
	}

void CPanicAnim::Animate(TDateTime *aDateTime)
	{
	switch(iSyncState)
		{
		case ESyncStateNull:
			break;
		case ESyncStateStarting:
			iTime=iFunctions->SystemTime();
			iSyncState=ESyncStateCounting;
			iSyncCount=0;
			break;
		case ESyncStateCounting:
			if (aDateTime)
				{
				TTimeIntervalSeconds interval;
				TTime(*aDateTime).SecondsFrom(iTime, interval);
				iSyncCount=interval.Int();
				}
			else
				iSyncCount++;
			if (iSyncCount==ESyncCount)
				{
				TTimeIntervalMicroSeconds minInterval(0);
				TTimeIntervalMicroSeconds maxInterval(0);
				iSyncState=ESyncStateNull;
				switch(iSyncMode)
					{
					case MAnimGeneralFunctions::ESyncSecond:
						minInterval=iSyncCount*1000000-100000;
						maxInterval=iSyncCount*1000000+100000;
						break;
					default:;
					}
				TTime time(iFunctions->SystemTime());
				TTimeIntervalMicroSeconds interval=time.MicroSecondsFrom(iTime);
				TInt ret=KErrNone;
				if (interval<minInterval || interval>maxInterval)
					ret=KErrGeneral;
				iFunctions->Client().RequestComplete(iStatusPtr,ret);
				}
			break;
		case ESyncLeave:
			User::Leave(KErrGeneral);	// Test: Not allowed to leave, will cause a panic
			break;
		case ESyncStateTimeChange1:
			if (!aDateTime)
				{
				iFunctions->Client().RequestComplete(iStatusPtr,KErrNone);
				iSyncState=ESyncStateTimeChange2;
				}
			break;
		case ESyncStateTimeChange2:
			if (!aDateTime)
				{
				iSyncState=ESyncStateTimeChange3;
				break;
				}
		case ESyncStateTimeChange3:
			iFunctions->Client().RequestComplete(iStatusPtr2,aDateTime ? KErrNone : KErrGeneral);
			iSyncState=ESyncStateNull;
			break;
		}
	}

void CPanicAnim::Redraw()
	{
	if (iLeaveInRedraw)
		User::Leave(KErrGeneral);	//Test: Not allowed to leave, will cause a panic
	}

void CPanicAnim::Command(TInt aOpcode, TAny *)
	{
	switch(aOpcode)
		{
		case EADllPanicCallPanic:
			iFunctions->Panic();
			break;
		case EADllPanicSetInterval:
			iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
			iFunctions->SetInterval(1);
			break;
		case EADllPanicSetNextInterval:
			iFunctions->SetSync(MAnimGeneralFunctions::ESyncMinute);
			iFunctions->SetNextInterval(1);
			break;
		case EADllPanicDoubleActivateGC:
			iWindowFunctions->ActivateGc();
			iWindowFunctions->ActivateGc();
			break;
		case EADllPanicDrawRectWithNoGc:
			iGc->DrawRect(TRect(0,0,10,10));
			break;
		case EADllPanicDrawTextWithNoFont:
			iWindowFunctions->ActivateGc();
			iGc->DrawText(_L("Panic"),TPoint(20,20));
			break;
		case EADllPanicLeaveInAnimate:
			iFunctions->SetSync(MAnimGeneralFunctions::ESyncFlash);
			iSyncState=ESyncLeave;
			break;
		case EADllPanicLeaveInRedraw:
			iLeaveInRedraw=ETrue;
			iWindowFunctions->Invalidate(TRect(0,0,10,10));
			break;
		case EADllPanicLeave:
			//tests a leaving function in a non-leaving function and should not be moved to CommandReplyL as it leaves
			User::Leave(KErrGeneral);	//LeaveScan: intentional test
			break;
		case EADllPanicSetVisWithGcActive:
			iWindowFunctions->ActivateGc();
			iWindowFunctions->SetVisible(EFalse);
			iWindowFunctions->SetVisible(ETrue);
			break;
		case EADllPanicLeaveInActiveCallback:
			iActivePanicIdle=CIdle::NewL(EPriorityHigh);
			iActivePanicIdle->Start(TCallBack(CPanicAnim::LeaveInActiveCallback,this));
			break;
		case EADllPanicSetClippingRectWithNoGc:
			{
			// currently this test fails because when deactivated iWin is also NULL and is deferenced in function
			TRect rect(20,20,20,20);	
			iGc->SetClippingRect(rect);
			}
			break;
		case EADllPanicCancelClippingRegionWithNoGc:
			// currently this test fails because when deactivated iWin is NULL and function leaves
			iGc->CancelClippingRegion();
			break;
		case EADllPanicCancelClippingRectWithNoGc:
			// currently this test fails because when deactivated iWin is also NULL and is deferenced in function
			iGc->CancelClippingRect();
			break;
		case EADllPanicSetDrawModeWithNoGc:
			iGc->SetDrawMode(CGraphicsContext::EDrawModeAND);
			break;
		case EADllPanicUseFontWithNoGc:
			{
			// currently this test fails because does not check for NULL font
			CFbsFont* font = NULL;
			iGc->UseFont(font);
			}
			break;
		case EADllPanicDiscardFontWithNoGc:
			iGc->DiscardFont();
			break;
		case EADllPanicSetUnderlineStyleWithNoGc:
			iGc->SetUnderlineStyle(EUnderlineOff);
			break;
		case EADllPanicSetStrikeThoughStyleWithNoGc:
			iGc->SetStrikethroughStyle(EStrikethroughOff);
			break;
		case EADllPanicSetWordJustificationWithNoGc:
			iGc->SetWordJustification(0,0);
			break;
		case EADllPanicSetCharJustificationWithNoGc:
			iGc->SetCharJustification(0,0);
			break;
		case EADllPanicSetPenColorWithNoGc:
			{
			TRgb rgbs;
			iGc->SetPenColor(rgbs);
			}
			break;
		case EADllPanicSetPenStyleWithNoGc:
			iGc->SetPenStyle(CGraphicsContext::ESolidPen);
			break;
		case EADllPanicSetPenSizeWithNoGc:
			{
			const TSize size;
			iGc->SetPenSize(size);
			}
			break;
		case EADllPanicSetBrushColorWithNoGc:
			{
			TRgb rgbs;
			iGc->SetBrushColor(rgbs);
			}
			break;
		case EADllPanicSetBrushStyleWithNoGc:
			iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
			break;
		case EADllPanicSetBrushOriginWithNoGc:
			{
			TPoint point;
			iGc->SetBrushOrigin(point);
			}
			break;
		case EADllPanicUseBrushPatterWithNoGc:
			{
			CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
			CleanupStack::PushL(bitmap);
			User::LeaveIfError(bitmap->Create(TSize(16,16),EGray4));
			iGc->UseBrushPattern(bitmap);
			CleanupStack::PopAndDestroy(bitmap);
			}
			break;
		case EADllPanicDiscardBrushPatternWithNoGc:
			iGc->DiscardBrushPattern();
			break;
		case EADllPanicSetFadedWithNoGc:
			iGc->SetFaded(EFalse);
			break;
		case EADllPanicSetFadingParametersWithNoGc:
			iGc->SetFadingParameters(1,1);
			break;
		case EADllPanicDrawArcWithNoGc:
			{
			TRect rect;
			iGc->DrawArc(rect,
				TPoint(rect.Center().iX, rect.iTl.iY),
				TPoint(rect.iBr.iX, rect.Center().iY));
			}
			break;
		case EADllPanicDrawPieWithNoGc:
			{
			TRect rect;				
			iGc->DrawPie(rect, TPoint(rect.Center().iX, rect.iTl.iY), TPoint(rect.iBr.iX, rect.Center().iY));
			}
			break;
		case EADllPanicDrawLineWithNoGc:
			{	
			const TPoint point1;
			const TPoint point2;
			iGc->DrawLine(point1, point2);
			}
			break;
		case EADllPanicDrawLineToWithNoGc:
			{
			const TPoint point;
			iGc->DrawLineTo(point);
			}
			break;
		case EADllPanicDrawLineByWithNoGc:
			{
			const TPoint point;
			iGc->DrawLineBy(point);
			}
			break;
		case EADllPanicDrawEllipseWithNoGc:
			{
			TRect rect(20,20,20,20);
			iGc->DrawEllipse(rect);
			}
			break;
		case EADllPanicDrawRoundedRecWithNoGc:
			{
			TRect rect;
			iGc->DrawRoundRect(rect, TSize(rect.Width()/8, rect.Height()/8));
			}
			break;
		case EADllPanicClearWithRectWithNoGc:
			{
			TRect rect(20,20,20,20);
			iGc->Clear(rect);
			}
			break;
		case EADllPanicClearWithNoGc:
			iGc->Clear();
			break;
		case EADllPanicBitBltWithNoGc:
			{
			CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
			CleanupStack::PushL(bitmap);
			User::LeaveIfError(bitmap->Create(TSize(16,16),EGray4));
			iGc->BitBlt(TPoint(0, 0), bitmap);
			CleanupStack::PopAndDestroy(bitmap);
			}
			break;
		case EADllPanicBitBltWithRectWithNoGc:
			{
			CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
			TRect rect;
			CleanupStack::PushL(bitmap);
			User::LeaveIfError(bitmap->Create(TSize(16,16),EGray4));
			iGc->BitBlt(TPoint(0, 0), bitmap, rect);
			CleanupStack::PopAndDestroy(bitmap);
			}
			break;
		case EADllPanicBitBltMaskedWithNoGc:
			{
			CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
			TRect rect;
			CleanupStack::PushL(bitmap);
			User::LeaveIfError(bitmap->Create(TSize(16,16),EGray4));
			iGc->BitBltMasked(TPoint(0, 0), bitmap, rect, bitmap, EFalse);
			CleanupStack::PopAndDestroy(bitmap);
			}
			break;
		case EADllPanicDrawBitmapMaskedFbsBitmapWithNoGc:
			{
			CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
			TRect rect;
			CleanupStack::PushL(bitmap);
			User::LeaveIfError(bitmap->Create(TSize(16,16),EGray4));
			iGc->DrawBitmapMasked(rect, bitmap, rect, bitmap, ETrue);
			CleanupStack::PopAndDestroy(bitmap);
			}
			break;
		case EADllPanicDrawBitmapMaskedWsBitmapWithNoGc:
			{
			CWsBitmap* bitmap = NULL;
			TRect rect;
			iGc->DrawBitmapMasked(rect, bitmap, rect, bitmap, ETrue);
			}
			break;
		case EADllPanicDrawBitmapPointWithNoGc:
			{
			CWsBitmap* bitmap = NULL;
			TRect rect;
			iGc->DrawBitmap(rect.iTl, bitmap);
			}
			break;
		case EADllPanicDrawBitmapWithNoGc:
			{
			CWsBitmap* bitmap = NULL;
			TRect rect;
			iGc->DrawBitmap(rect, bitmap);
			}
			break;
		case EADllPanicDrawBitmapRectWithNoGc:
			{
			CWsBitmap* bitmap = NULL;
			TRect rect;
			iGc->DrawBitmap(rect, bitmap, TRect(0, 0, 16, 16));
			}
			break;
		case EADllPanicDrawPolyLinePointsWithNoGc:
			{
			TRect rect;
			CArrayFixFlat<TPoint>* polyPoints = new(ELeave) CArrayFixFlat<TPoint>(3); //CArrayFixFlat
			CleanupStack::PushL(polyPoints);
			polyPoints->AppendL(rect.iTl);
			polyPoints->AppendL(rect.Center());
			polyPoints->AppendL(TPoint(rect.iBr.iX, rect.iTl.iY));
			iGc->DrawPolyLine(&polyPoints->At(0), 3);
			CleanupStack::PopAndDestroy(polyPoints);
			}
			break;
		case EADllPanicDrawPolyLineArrayWithNoGc:
			{
			TRect rect;
			CArrayFixFlat<TPoint>* polyPoints = new(ELeave) CArrayFixFlat<TPoint>(3); //CArrayFixFlat
			CleanupStack::PushL(polyPoints);
			polyPoints->AppendL(rect.iTl);
			polyPoints->AppendL(rect.Center());
			polyPoints->AppendL(TPoint(rect.iBr.iX, rect.iTl.iY));
			iGc->DrawPolyLine(polyPoints);
			CleanupStack::PopAndDestroy(polyPoints);
			}
			break;
		case EADllPanicMoveToWithNoGc:
			{
			TPoint point;
			iGc->MoveTo(point);
			}
			break;
		case EADllPanicMoveByWithNoGc:
			{
			TPoint point;
			iGc->MoveBy(point);
			}
			break;
		case EADllPanicPlotWithNoGc:
			{
			TPoint point;
			iGc->Plot(point);
			}
			break;
		case EADllPanicSetOriginWithNoGc:
			{
			TPoint point;
			iGc->SetOrigin(point);
			}
			break;
		case EADllPanicCopyRectWithNoGc:
			{
			TPoint point;
			TRect rect;
			iGc->CopyRect(point, rect);
			}
			break;
		case EADllPanicResetWithNoGc:
			iGc->Reset();
			break;
		case EADllPanicMapColorsWithNoGc:
			{
			TRgb rgbs[2];
			TRect rect;
			iGc->MapColors(rect, rgbs, 1, ETrue);
			}
			break;
		case EADllPanicDrawTextWithRectWithNoFont:
			iWindowFunctions->ActivateGc();
			iGc->DrawText(_L("Panic"),TRect(20,20,20,20),0);
			break;
		case EADllPanicDrawTextVerticalWithNoFont:
			iWindowFunctions->ActivateGc();
			iGc->DrawTextVertical(_L("Panic"),TPoint(20,20), EFalse);
			break;
		case EADllPanicDrawTextVerticalWithRectWithNoFont:
			iWindowFunctions->ActivateGc();
			iGc->DrawTextVertical(_L("Panic"),TRect(20,20,20,20), 0, EFalse);
			break;
		case EADllPanicInvalidFocusScreenTooBig:
			(static_cast<MAnimGeneralFunctionsWindowExtension*>(iFunctions->ExtendedInterface(MAnimGeneralFunctions::EWindowExtensionInterface)))->SetFocusScreen(3000);
			break;
		case EADllPanicInvalidFocusScreenNegative:
			(static_cast<MAnimGeneralFunctionsWindowExtension*>(iFunctions->ExtendedInterface(MAnimGeneralFunctions::EWindowExtensionInterface)))->SetFocusScreen(-1);
			break;
		default:
			iFunctions->Panic();
		}
	}

void CPanicAnim::DoLeaveInActiveCallback()
	{
	iWindowFunctions->ActivateGc();
	User::Leave(KErrUnknown);
	}

TInt CPanicAnim::LeaveInActiveCallback(TAny *aThis)
	{
	CPanicAnim* panicAnim=(CPanicAnim*)aThis;
	panicAnim->DoLeaveInActiveCallback();
	return(KErrNone);
	}

void CPanicAnim::StartSyncTest(MAnimGeneralFunctions::TAnimSync aSync)
	{
	iFunctions->SetSync(aSync);
	iSyncMode=aSync;
	iSyncState=ESyncStateStarting;
	}

void CPanicAnim::StartTimeChangeTest()
	{
	iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
	iSyncMode=MAnimGeneralFunctions::ESyncSecond;
	iSyncState=ESyncStateTimeChange1;
	}

TInt CPanicAnim::CommandReplyL(TInt aOpcode,TAny* aParams)
	{
	TAnimArgUnion pData;
	pData.any=aParams;
	switch(aOpcode)
		{
		case EADllPanicBadFont:
			iFunctions->DuplicateFontL(123);
			break;
		case EADllPanicBadBitmap:
			iFunctions->DuplicateBitmapL(123);
			break;
		case EADllPanicCallPanic:
			iFunctions->Panic();
			break;
		case EADllReadRemoteDescriptor:
			{
			TInt ret=0;
			const RMessagePtr2& message=*iFunctions->Message();
			TInt bufLength = message.GetDesLength(KIpcSlot);
			TUint16* heapCell=static_cast<TUint16*>(User::AllocL(bufLength*2));
			TPtr buf(heapCell,bufLength*2,bufLength*2);
			const TInt theError=message.Read(KIpcSlot,buf);
			if(theError == KErrNone)
				{
				for (TInt index=0;index<bufLength;index++)
					{
					ret+=buf[index];
					}
				}
			User::Free(heapCell);
			return(ret);
			}
		case EADllTestWindowSize:
			return(*pData.Size==iWindowFunctions->WindowSize());
		case EADllSyncTests:
			iStatusPtr=pData.SyncTests->status;
			StartSyncTest(pData.SyncTests->syncMode);
			break;
		case EADllTimeChangedTest:
			iStatusPtr=pData.TimeChangeTest->status;
			iStatusPtr2=pData.TimeChangeTest->status2;
			StartTimeChangeTest();
			break;
		case EADllTestSetVisible:
			if (iWindowFunctions->IsHidden())
				goto setVisFailed;
			iWindowFunctions->SetVisible(EFalse);
			if (!iWindowFunctions->IsHidden())
				goto setVisFailed;
			iWindowFunctions->SetVisible(ETrue);
			if (iWindowFunctions->IsHidden())
setVisFailed:	return(EFalse);
			return(ETrue);
		case EADllPanicSetVisWithGcActiveReply:
			iWindowFunctions->ActivateGc();
			iWindowFunctions->SetVisible(EFalse);
			iWindowFunctions->SetVisible(ETrue);
			break;
		case EADllPanicSetClippingRegionWithNoGc:
			{
			// currently this test fails because when deactivated iWin is also NULL and is deferenced in function
			RRegion region;
			TBool err = iGc->SetClippingRegion(region);
			}
			break;
		case EADllPanicDrawPolygonPointsWithNoGc:
			{	
			TRect rect(20,20,20,20);
			CArrayFixFlat<TPoint>* polyPoints = new(ELeave) CArrayFixFlat<TPoint>(3); //CArrayFixFlat
			CleanupStack::PushL(polyPoints);
			polyPoints->AppendL(rect.iTl);
			polyPoints->AppendL(rect.Center());
			polyPoints->AppendL(TPoint(rect.iBr.iX, rect.iTl.iY));
			iGc->DrawPolygon(polyPoints, CGraphicsContext::EWinding);
			CleanupStack::PopAndDestroy(polyPoints);
			}
			break;
		case EADllPanicDrawPolygonArrayWithNoGc:
			{	
			TRect rect(20,20,20,20);
			CArrayFixFlat<TPoint>* polyPoints = new(ELeave) CArrayFixFlat<TPoint>(3); //CArrayFixFlat
			CleanupStack::PushL(polyPoints);
			polyPoints->AppendL(rect.iTl);
			polyPoints->AppendL(rect.Center());
			polyPoints->AppendL(TPoint(rect.iBr.iX, rect.iTl.iY));
			iGc->DrawPolygon(&polyPoints->At(0), 3, CGraphicsContext::EAlternate);
			CleanupStack::PopAndDestroy(polyPoints);
			}
			break;
		case EADllPanicAlphaBlendingBitmapsFbsBitmapWithNoGc:
			{
			TRect rect(20,20,20,20);
			CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
			CleanupStack::PushL(bitmap);
			User::LeaveIfError(bitmap->Create(TSize(16,16),EGray4));
			iGc->AlphaBlendBitmaps(TPoint(0, 0), bitmap, rect, bitmap, TPoint(0,0));
			CleanupStack::PopAndDestroy(bitmap);
			}
			break;
		case EADllPanicDevice:
			{
			CGraphicsDevice *device = iGc->Device();
			}
			break;
		default:
			iFunctions->Panic();
		}
	return(KErrNone);
	}

void CPanicAnim::FocusChanged(TBool )
	{
	}

TBool CPanicAnim::OfferRawEvent(const TRawEvent &/*aRawEvent*/)
	{
	return EFalse;
	}


// CPanicAnim2 simply panics in the construct //
void CPanicAnim2::ConstructL(TAny *, TBool )
	{
	iFunctions->Panic();
	}

void CPanicAnim2::Animate(TDateTime *)
	{}

void CPanicAnim2::Redraw()
	{}

void CPanicAnim2::Command(TInt , TAny *)
	{}

TInt CPanicAnim2::CommandReplyL(TInt , TAny *)
	{return(0);}

void CPanicAnim2::FocusChanged(TBool )
	{}

TBool CPanicAnim2::OfferRawEvent(const TRawEvent &/*aRawEvent*/)
	{
	return EFalse;
	}


// CAutoAnim3 test drawing/redrawing //
void CAutoAnim3::ConstructL(TAny *aParam, TBool )
	{
	iRect=*((TRect *)aParam);
	iWindowFunctions->SetRect(iRect);
	iMode=0;
	iWindowFunctions->Invalidate(iRect);
	iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
	}

void CAutoAnim3::Animate(TDateTime *)
	{
	iWindowFunctions->ActivateGc();
	if (iMode==1)
		{
		Draw(iRect);
		if (iRect.Width()>20 && iRect.Height()>20)
			iRect.Shrink(TSize(20,20));
		else
			iRect=iBaseRect;
		Draw(iRect);
		}
	else if (iMode==2)
		{
		Draw(iRect);
		iFunctions->Client().RequestComplete(iStatusPtr,KErrNone);
		iFunctions->SetSync(MAnimGeneralFunctions::ESyncNone);
		}
	else if (iMode==3)
		{
		// flip between two sizes
		if (iRect.iTl.iX == 10)
			iRect.SetRect(40,40,80,80);
		else
			iRect.SetRect(10,10,110,110);
		Draw(iRect);
		}
	}

void CAutoAnim3::Draw(const TRect &aRect)
	{
	switch(iMode)
		{
		case 0:
			iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
			iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
			iGc->SetBrushColor(TRgb::Gray256(85));
			iGc->SetPenColor(TRgb::Gray256(170));
			iGc->DrawEllipse(aRect);
			iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
			break;
		case 1:	// Shrinking box
			iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
			iGc->SetPenColor(TRgb::Gray256(255));
			iGc->DrawRect(aRect);
			break;
		case 2:
		case 3:
			iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
			iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
			iGc->SetBrushColor(TRgb::Gray256(85));
			iGc->SetPenColor(TRgb::Gray256(170));
			iGc->DrawEllipse(aRect);
			iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
			break;
		default:
			break;
		}
	}

void CAutoAnim3::Redraw()
	{
	Draw(iRect);
	}

void CAutoAnim3::Command(TInt aOpcode,TAny* aParam)
	{
	TAnimArgUnion pData;
	pData.any=aParam;
	switch(aOpcode)
		{
		case EADllSetRect:
			iWindowFunctions->ActivateGc();
			Draw(iRect);
			iRect=*pData.Rect;
			iWindowFunctions->SetRect(iRect);
			Draw(iRect);
			{		//Do more extensive testing
			TWindowInfo data;
			iWindowFunctions->Parameters(data);
			}
			break;
		case EADllStartAnimate:
			iWindowFunctions->ActivateGc();
			Draw(iRect);
			iMode=1;
			iBaseRect=*pData.Rect;
			iRect=iBaseRect;
			iWindowFunctions->SetRect(iRect);
			iFunctions->SetSync(MAnimGeneralFunctions::ESyncFlash);
			Draw(iRect);
			break;
		case EADllCancelAnimate:
			iFunctions->SetSync(MAnimGeneralFunctions::ESyncNone);
			iWindowFunctions->ActivateGc();
			Draw(iRect);
			iMode=-1;
			break;
		case EADllSetVisible:
			iWindowFunctions->SetVisible(*pData.Bool);
			break;
		case EADllSetShadowDrawMode:
			iMode=2;
			break;
		case EADllDrawTestScreen:
			DrawTestScreenL(pData.DrawTestScreen);		//Should move to CommandReplyL as it leaves
			break;
		case EADllSetMode:
			iMode = *pData.Int;
			break;	
		}
	}

TInt CAutoAnim3::CommandReplyL(TInt aOpcode, TAny *aParam)
	{
	TAnimArgUnion pData;
	pData.any = aParam;
	TInt ret=0;
	switch(aOpcode)
		{
		case EADllShadowAnimTest:
			iFunctions->SetSync(MAnimGeneralFunctions::ESyncFlash);
			iStatusPtr=((TShadowDrawTest *)aParam)->status;
			break;
		case EADllParameterRectValueTest:
			{
			TWindowInfo wininf;
			iWindowFunctions->Parameters(wininf);
			TRect comp = *(pData.Rect);
			ret=(comp==wininf.iScreenPos);
			}
			break;
		case EADllDoSetSync:
			iFunctions->SetSync(static_cast<MAnimGeneralFunctions::TAnimSync>(*pData.Int));
			break;
		case EADllSetInterval:					
			iFunctions->SetInterval(*pData.Int);
			break;
		}
	return(ret);
	}

void CAutoAnim3::FocusChanged(TBool )
	{}

//void CAutoAnim3::DrawTestScreen(TDrawTestScreen *aParams)
//Function now at bottom of file

TBool CAutoAnim3::OfferRawEvent(const TRawEvent &/*aRawEvent*/)
	{
	return EFalse;
	}


// CAutoAnimScrBack test drawing/redrawing //
void CAutoAnimScrBack::ConstructL(TAny *aParam, TBool )
	{
	iRect=*((TRect *)aParam);
	iWindowFunctions->SetRect(iRect);
	iMode=1;
	iFunctions->SetSync(MAnimGeneralFunctions::ESyncFlash);
	}

void CAutoAnimScrBack::Animate(TDateTime *)
	{
	iWindowFunctions->ActivateGc();
	Draw();
	iFunctions->SetSync(MAnimGeneralFunctions::ESyncNone);
	}

void CAutoAnimScrBack::Draw()
	{
	iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	if (iMode==2)
		iGc->SetPenSize(TSize(3,3));
	iGc->SetPenColor(TRgb::Gray4(iMode==1 ? 2 : 3));
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(TRgb::Gray4(iMode==1 ? 1 : 0));
	iGc->DrawRect(iRect);
	}

void CAutoAnimScrBack::Redraw()
	{
	Draw();
	}

void CAutoAnimScrBack::Command(TInt aOpcode,TAny* aParam)
	{
	TAnimArgUnion pData;
	pData.any=aParam;
	switch(aOpcode)
		{
		case EADllDrawOtherRect:
			{
			TRect oldRect(iRect);
			if (iMode==2)
				oldRect.Grow(1,1);	// To allow for 3x3 pen size
			iMode=2;
			iRect=*pData.Rect;
			TRect fullRect(iRect);
			fullRect.Grow(1,1);	// To allow for 3x3 pen size
			iWindowFunctions->Invalidate(oldRect);
			iWindowFunctions->SetRect(fullRect);
			iWindowFunctions->Invalidate(fullRect);
			}
//			iFunctions->ActivateGc();
//			Draw();
			break;
		}
	}

TInt CAutoAnimScrBack::CommandReplyL(TInt aOpcode, TAny *)
	{
	switch(aOpcode)
		{
		case EADllIsHidden:
			return iWindowFunctions->IsHidden();
		}
	return(0);
	}

void CAutoAnimScrBack::FocusChanged(TBool )
	{}

TBool CAutoAnimScrBack::OfferRawEvent(const TRawEvent &/*aRawEvent*/)
	{
	return EFalse;
	}


// CTestSpriteAnim test sprite access //
void CTestSpriteAnim::ConstructL(TAny* /*aParam*/)
	{
	iSpriteFunctions->SizeChangedL();
	iSpriteFunctions->Activate(ETrue);
	}

void CTestSpriteAnim::Animate(TDateTime*)
	{
	}

void CTestSpriteAnim::Command(TInt aOpcode,TAny* aParam)
	{
	TAnimArgUnion pData;
	pData.any=aParam;
	switch (aOpcode)
		{
	case EADllSizeChanged:
		iSpriteFunctions->Activate(EFalse);
		iSpriteFunctions->SizeChangedL();		//Should move to CommandReplyL as it leaves
	case EADllActivate:
		iSpriteFunctions->Activate(ETrue);
		break;
	case EADllDeactivate:
		iSpriteFunctions->Activate(EFalse);
		break;
	case EADllSetPos:
		iSpriteFunctions->SetPosition(*pData.Point);
		break;
	case EADllDraw1:
	case EADllDraw2:
	case EADllDraw3:
		DrawMemberL(aOpcode-EADllDraw1);		//Should move to CommandReplyL as it leaves
		break;
	case EADllIncDraw:
		DrawMemberL();		//Should move to CommandReplyL as it leaves
		break;
	default:;
		}
	}

TInt CTestSpriteAnim::CommandReplyL(TInt aOpcode,TAny* aParam)
	{
	TAnimArgUnion pData;
	pData.any=aParam;
	switch (aOpcode)
		{
	case EADllCheckMember:
		return CheckMember(*pData.SpriteMemberInfo);
	default:;
		}
	return(KErrNone);
	}

TBool CTestSpriteAnim::OfferRawEvent(const TRawEvent& /*aRawEvent*/)
	{
	return EFalse;
	}

TBool CTestSpriteAnim::CheckMember(const TSpriteMemberInfo& aMemInfo)
	{
	TSpriteMember spriteMember=*iSpriteFunctions->GetSpriteMember(aMemInfo.iMember);
	if (spriteMember.iBitmap->Handle()!=aMemInfo.iBitmapHandle)
		return EFalse;
	if (aMemInfo.iMaskBitmapHandle==0)
		{
		if (spriteMember.iMaskBitmap!=NULL)
			return EFalse;
		}
	else
		{
		if (spriteMember.iMaskBitmap->Handle()!=aMemInfo.iMaskBitmapHandle)
			return EFalse;
		}
	if (spriteMember.iInvertMask!=aMemInfo.iInvertMask)
		return EFalse;
	if (spriteMember.iDrawMode!=aMemInfo.iDrawMode)
		return EFalse;
	if (spriteMember.iOffset!=aMemInfo.iOffset)
		return EFalse;
	if (spriteMember.iInterval!=aMemInfo.iInterval)
		return EFalse;
	return ETrue;
	}

void CTestSpriteAnim::DrawMemberL(TInt aMember)
	{
	CFbsBitmap *bitmap=iSpriteFunctions->GetSpriteMember(aMember)->iBitmap;
	CFbsBitmapDevice *device=CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(device);
	CFbsBitGc *gc;
	User::LeaveIfError(device->CreateContext(gc));
	CleanupStack::PushL(gc);
	//gc->SetBrushColor(TRgb::Gray4((aMember+1)%3));
	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->SetPenSize(TSize());
	TSize size=bitmap->SizeInPixels();
	size.SetSize(size.iWidth/2,size.iHeight/2);
	TPoint point=size.AsPoint();
	gc->SetBrushColor(TRgb::Gray4(0));
	//TRect rect=bitmap->SizeInPixels();
	gc->DrawRect(TRect(TPoint(),size));
	gc->SetBrushColor(TRgb::Gray4(1));
	gc->DrawRect(TRect(TPoint(point.iX,0),size));
	gc->SetBrushColor(TRgb::Gray4(2));
	gc->DrawRect(TRect(TPoint(0,point.iY),size));
	gc->SetBrushColor(TRgb::Gray4(3));
	gc->DrawRect(TRect(point,size));
	iSpriteFunctions->UpdateMember(aMember,TRect(bitmap->SizeInPixels()),ETrue);
	CleanupStack::PopAndDestroy(2);
	}

void CTestSpriteAnim::DrawMemberL()
	{
	CFbsBitmap *bitmap=iSpriteFunctions->GetSpriteMember(0)->iBitmap;
	CFbsBitmapDevice *device=CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(device);
	CFbsBitGc *gc;
	User::LeaveIfError(device->CreateContext(gc));
	CleanupStack::PushL(gc);
	gc->SetPenSize(TSize(3,3));
	gc->SetPenColor(TRgb::Gray4(1));
	TPoint corner=bitmap->SizeInPixels().AsPoint();
	gc->DrawLine(TPoint(),corner);
	gc->DrawLine(TPoint(0,corner.iY),TPoint(corner.iX,0));
	iSpriteFunctions->UpdateMember(0,TRect(bitmap->SizeInPixels()),EFalse);
	CleanupStack::PopAndDestroy(2);
	}


// CTestFreeTimerAnim test drawing off own timer //
CTestFreeTimerAnim::~CTestFreeTimerAnim()
    {
    iTimer->Cancel();
    delete iTimer;
    }

void CTestFreeTimerAnim::ConstructL(TAny* /*aParam*/, TBool )
	{
    User::LeaveIfNull(iFunctions);
    User::LeaveIfNull(iWindowFunctions);
    
    //Configure free-timer anim (these values should be the default ones)
    iFunctions->SetSync( MAnimGeneralFunctions::ESyncNone );
    iFunctions->SetInterval(0);
    
    iFrameTime = 100000; //default frametime 0.1s
    iTimer = CAnimTimer::NewL(*this);
    iRect = iWindowFunctions->WindowSize();	
	}

#define MAX(a,b)    (((a)>(b))?(a):(b))

void CTestFreeTimerAnim::Animate(TDateTime*)
	{
    TInt max = MAX(iRect.Width(), iRect.Height());
    if(max <= 0)
        {
        iRect = iWindowFunctions->WindowSize();
        }
    else
        {
        iRect.Shrink(1,1);
        }
    
    //change colour
    switch(iColour.Value())
        {
        case 0x000000:  //black
        case 0xffffff:  //white
            iColour = KRgbRed;
            break;
        case 0x0000ff:  //red
            iColour = KRgbGreen;
            break;
        case 0x00ff00:  //green
            iColour = KRgbBlue;
            break;
        case 0xff0000:  //blue
            iColour = KRgbBlack;
            break;
        }	
	}

void CTestFreeTimerAnim::Redraw()
	{
    //draw something in the animated colour
    iGc->SetBrushColor(iColour);
    iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iGc->SetPenColor(KRgbBlack);
    iGc->DrawRect(iRect);	
	}

void CTestFreeTimerAnim::Command(TInt aOpcode, TAny* aArgs)
	{
    switch(aOpcode)
        {
        case EStartAnimation:
            DisplayNextFrameL();
            break;
        case EStopAnimation:
            iTimer->Cancel();
            break;
        case ESetFrameTime:
            ASSERT(aArgs);        
            iFrameTime = *reinterpret_cast<TInt*>(aArgs);
            break;
        default:
            ASSERT(0);
        }
    }

TInt CTestFreeTimerAnim::CommandReplyL(TInt aOpcode, TAny* aArgs)
	{    
    switch(aOpcode)
        {
        case EStartAnimation:
            DisplayNextFrameL();
            break;
        case EStopAnimation:
            iTimer->Cancel();
            break;
        case ESetFrameTime:
            ASSERT(aArgs); 
            iFrameTime = *reinterpret_cast<TInt*>(aArgs);
            break;
        default:
            ASSERT(0);
        }
    return(KErrNone);
    }

void CTestFreeTimerAnim::FocusChanged(TBool /*aState*/)
	{
	}

TBool CTestFreeTimerAnim::OfferRawEvent(const TRawEvent& /*aRawEvent*/)
	{
	return EFalse;
	}


void CTestFreeTimerAnim::DisplayNextFrameL()
    {    
    //Reschedule timer callback
    iTimer->Cancel();
    iTimer->After(iFrameTime);
    
    Animate(NULL); //wserv don't animate free-timer animations
    
    //Schedule wserv redraw
    MAnimFreeTimerWindowFunctions* windowFunctions = WindowFunctions();
    ASSERT(windowFunctions);
    windowFunctions->ActivateGc();
    windowFunctions->DeactivateGc();
    windowFunctions->Update();
    }

//
// CAnimTimer class
//

/**
 Constructs a new bitmap animation timer object,and adds the specified active object to the current active scheduler.

@param aObserver a pointer to MAnimTimerObserver
*/
CAnimTimer::CAnimTimer(MAnimTimerObserver& aObserver)
    :CTimer(EPriorityStandard),
    iAnim(aObserver)
    {
    CActiveScheduler::Add(this);
    }

/**
 Frees resources owned by the object prior to deletion.
*/ 
CAnimTimer::~CAnimTimer()
    {
    }

/**
 Creates a new bitmap animation timer object.

@param aObserver A pointer to the MAnimTimerObserver
@return A pointer to the new bitmap animation timer object.
*/
CAnimTimer* CAnimTimer::NewL(MAnimTimerObserver& aObserver)
    {
    CAnimTimer* timer=new(ELeave) CAnimTimer(aObserver);
    CleanupStack::PushL(timer);
    timer->ConstructL();
    CleanupStack::Pop();
    return timer;
    }

/**
 Completes construction of the bitmap animation timer object and
 constructs a new asynchronous timer.
*/
void CAnimTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

/**
 Handles an active object's request completion event.
 Invokes the function to draw the next frame
*/
void CAnimTimer::RunL()
    {
    iAnim.DisplayNextFrameL();
    }

// CEventTestAnimBase base class for event test anims //
CEventTestAnimBase::~CEventTestAnimBase()
	{
	iFunctions->GetRawEvents(EFalse);
	}

void CEventTestAnimBase::ConstructL(TAny* /*aParam*/, TBool )
	{
	iExpectedEvents.SetLengthL(EEventBufferSize);
	}

void CEventTestAnimBase::Animate(TDateTime*)
	{
	}

void CEventTestAnimBase::Redraw()
	{
	}

void CEventTestAnimBase::Command(TInt /*aOpcode*/, TAny* /*aParam*/)
	{
	/*switch (aOpcode)
		{
	default:;
		}*/
	}

void CEventTestAnimBase::FocusChanged(TBool /*aState*/)
	{
	}

// Base class common client server interface
TInt CEventTestAnimBase::CommandReplyL(TInt aOpcode,TAny* aParam)
	{
	TAnimArgUnion pData;
	pData.any=aParam;
	switch (aOpcode)
		{
	case EADllAddExpectedEvent:
		return AddExpectedEvent(pData.AnimRawEvent);
	case EADllNumberOfEventsReceived:
		return iEventCount;
	case EADllErrorCode:
		return iError;
	case EADllReset:
		iExpectedEvents.Reset();
		iError = 0;
		iEventCount = 0;
		return KErrNone;
	default:;
		}
	return(KErrNone);
	}

TBool CEventTestAnimBase::HandleRecursiveEvent(const TRawEvent& aRawEvent)
	{
	if (aRawEvent.ScanCode() == 60)
		{
		TRawEvent rawEvent;
		rawEvent.Set(TRawEvent::EKeyUp, 34);
		iFunctions->PostRawEvent(rawEvent);
		rawEvent.Set(TRawEvent::EKeyUp, 35);
		iFunctions->PostRawEvent(rawEvent);
		return ETrue;
		}
	return EFalse;
	}

TBool CEventTestAnimBase::OfferRawEvent(const TRawEvent& aRawEvent)
	{
	// Do not consume the following types of events
	if (IsIgnorableEvent(aRawEvent))
		{
		return EFalse;
		}

	// Increment event count if not in an error state
	if (!Error())
		{
		iEventCount++;
		}
	
	// For anim which generates recursive events 
	if (aRawEvent.Type() == TRawEvent::EKeyDown && aRawEvent.ScanCode() == 60)
		{
		return HandleRecursiveEvent(aRawEvent);
		}

	TAnimRawEvent event;

	if (!GetNextExpectedEvent(event))
		{
		Fail();
		return EFalse;
		}

	// Fail if wrong event type
	if (!CompareEvents(event.iRawEvent, aRawEvent))
		{
		Fail();
		}

	return event.iEat;
	}

TBool CEventTestAnimBase::GetNextExpectedEvent(TAnimRawEvent& aRawEvent)
	{
	// Get next expected event
	if (iExpectedEvents.Remove(&aRawEvent)<1)
		{
		Fail();
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

TBool CEventTestAnimBase::IsIgnorableEvent(const TRawEvent& aRawEvent) const
	{
	TRawEvent::TType aType=aRawEvent.Type();
	TBool result = (aType==TRawEvent::EActive || aType==TRawEvent::EInactive || aType==TRawEvent::ERedraw
			|| (iExpectedEvents.Count()==0 && aType==TRawEvent::EPointerMove));

	return result;
	}

TBool CEventTestAnimBase::CompareEvents(const TRawEvent& aExpectedEvent, const TRawEvent& aActualEvent) const
	{
	TRawEvent::TType eType=aExpectedEvent.Type();
	TRawEvent::TType aType=aActualEvent.Type();
	if (eType!=aType)
		{
		return EFalse;
		}
	else
		{
		switch (aType)
			{
		case TRawEvent::EUpdateModifiers:
			{
			TInt eMod=aExpectedEvent.Modifiers(),aMod=aActualEvent.Modifiers();
			if (eMod!=aMod)
				{
				return EFalse;
				}
			break;
			}
		case TRawEvent::EKeyDown:
		case TRawEvent::EKeyUp:
			{
			TInt eScan=aExpectedEvent.ScanCode(),aScan=aActualEvent.ScanCode();
			if (eScan!=aScan)
				{
				return EFalse;
				}
			break;
			}
		case TRawEvent::EPointerMove:
		case TRawEvent::EPointerSwitchOn:
		case TRawEvent::EButton1Down:
		case TRawEvent::EButton1Up:
		case TRawEvent::EButton2Down:
		case TRawEvent::EButton2Up:
		case TRawEvent::EButton3Down:
		case TRawEvent::EButton3Up:
			{
			TPoint ePoint=aExpectedEvent.Pos(),aPoint=aActualEvent.Pos();
			if (ePoint!=aPoint)
				{
				return EFalse;
				}
			break;
			}
		default:;
			}
		}
	return ETrue;
	}

void CEventTestAnimBase::Fail()
	{
	if (iError==0)
		{
		iError=iEventCount;
		}
	}

inline TInt CEventTestAnimBase::Error() const
	{
	return iError;
	}

inline TInt CEventTestAnimBase::EventCount() const
	{
	return iEventCount;
	}

inline void CEventTestAnimBase::ResetEventCount()
	{
	iEventCount = 0;
	}

inline TInt CEventTestAnimBase::TotalExpectedEvents() const
	{
	return iExpectedEvents.Count();
	}

inline TInt CEventTestAnimBase::AddEvent(const TAnimRawEvent* aEvent)
	{
	return iExpectedEvents.Add(aEvent);
	}

// CEventTestAnim test drawing off own timer //
CEventTestAnim::~CEventTestAnim()
	{
	UnloadDeviceDriver();
	}

void CEventTestAnim::ConstructL(TAny* aParam, TBool aBool)
	{
	CEventTestAnimBase::ConstructL(aParam, aBool);
	iFunctions->GetRawEvents(ETrue);
	iDevDriverLoaded=EFalse;
	}

TInt CEventTestAnim::UnloadDeviceDriver()
	{
	TInt err=KErrNone;
	iLdd.Close();
	if (iDevDriverLoaded)
		{
		err=User::FreeLogicalDevice(REventDD::BinaryName());
		if (err==KErrNone)
			iDevDriverLoaded=EFalse;
		}
	return err;
	}

TInt CEventTestAnim::AddExpectedEvent(const TAnimRawEvent* aEvent)
	{
	return (AddEvent(aEvent) ? KErrNone : KErrOverflow);
	}

TInt CEventTestAnim::CommandReplyL(TInt aOpcode,TAny* aParam)
	{
	switch (aOpcode)
		{
	case EADllNoEventsAndReset:
		{
		TInt events=EventCount();
		ResetEventCount();
		return events;
		}
	case EADllLoadDeviceDriver:
		{
		TInt err=User::LoadLogicalDevice(REventDD::BinaryName());
		if (err==KErrNone || err==KErrAlreadyExists)
			{
			TInt err2=iLdd.Open();
			if (err==KErrNone)
				{
				if (err2==KErrNone)
					iDevDriverLoaded=ETrue;
				else 		//Only unload if we loaded it
					User::FreeLogicalDevice(REventDD::BinaryName());
				}
			err=err2;
			}
		return err;
		}
	case EADllUnloadDeviceDriver:
		return UnloadDeviceDriver();
	case EADllSendEvent:
		{
		TAnimRawEvent event;
		TInt events=*static_cast<TInt*>(aParam);
		TInt err=KErrNone;
		TInt ii;
		for (ii=events;ii>0;--ii)
			{
			event.iRawEvent.Set(ii/2==0 ? TRawEvent::ECaseOpen : TRawEvent::ECaseClose);
			//TInt err=UserSvr::AddEvent(event.iRawEvent);
			//Need to do the above line here, but don't have enough capability
			//so call a device driver to do it instead
			err=iLdd.SendEvent(event.iRawEvent);
			if (err!=KErrNone)
				return err;
			event.iEat=ETrue;
			AddExpectedEvent(&event);
			}
		return KErrNone;
		}
	default:;
		return CEventTestAnimBase::CommandReplyL(aOpcode, aParam);
		}
	}

void CEventTestAnim::Command(TInt aOpcode,TAny* /*aParam*/)
	{
	switch (aOpcode)
		{
	case EADllAfterEvent:
		if (TotalExpectedEvents()>0)
			Fail();
		break;
	default:;
		}
	}

// CEventPostingAnim test posting of anim events //
CEventPostingAnim::~CEventPostingAnim()
	{
	}

void CEventPostingAnim::ConstructL(TAny* aParam, TBool aBool)
	{
	CEventTestAnimBase::ConstructL(aParam, aBool);
	iFunctions->GetRawEvents(ETrue);
	}

TInt CEventPostingAnim::AddExpectedEvent(const TAnimRawEvent* aEvent)
	{
	return (AddEvent(aEvent) ? KErrNone : KErrOverflow);
	}

TBool CEventPostingAnim::OfferRawEvent(const TRawEvent &aRawEvent)
	{
	// Call PostRawEvent or PostKeyEvent according to type of the event
	TBool ret = CEventTestAnimBase::OfferRawEvent(aRawEvent);
	if (ret)
		{
		if (aRawEvent.Type() == TRawEvent::EKeyDown && aRawEvent.ScanCode() == 45)
			{
			TKeyEvent event;
			event.iCode='J';
			event.iScanCode=0;
			event.iModifiers=0;
			
			// This is to check the normal PostKeyEvent API
			iFunctions->PostKeyEvent(event);
			
			// This is to check the newly added API
			iFunctions->EventExtension()->PostKeyEvent(event, 2);
			}
		else
			{
			iFunctions->PostRawEvent(aRawEvent);	
			}
		}
	return ret;
	}

// CTimerTestAnim check that timer events stop //
CTimerTestAnim::~CTimerTestAnim()
	{
	}

void CTimerTestAnim::ConstructL(TAny* /*aParam*/, TBool )
	{
	iWindowFunctions->SetRect(TRect(0,0,1,1));
	iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
	iExpectingTicks=ETrue;
	iFail=EFalse;
	}

void CTimerTestAnim::Animate(TDateTime*)
	{
	++iTimerCount;
	if (!iExpectingTicks)
		iFail=iTimerCount;
	}

void CTimerTestAnim::Redraw()
	{
	}

void CTimerTestAnim::Command(TInt aOpcode, TAny* /*aParam*/)
	{
	switch (aOpcode)
		{
	case EADllResetCount:
		iTimerCount=0;
		break;
	case EADllNoTimer:
		iExpectingTicks=EFalse;
		iFunctions->SetSync(MAnimGeneralFunctions::ESyncNone);
		break;
	case EADllTimerStarted:
		iExpectingTicks=ETrue;
		iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
		break;
	default:;
		}
	}

TInt CTimerTestAnim::CommandReplyL(TInt aOpcode, TAny* /*aParam*/)
	{
	switch (aOpcode)
		{
	case EADllCurrentCount:
		return iTimerCount;
	case EADllFailed:
		return iFail;
	default:;
		}
	return(KErrNone);
	}

void CTimerTestAnim::FocusChanged(TBool /*aState*/)
	{
	}

TBool CTimerTestAnim::OfferRawEvent(const TRawEvent& /*aRawEvent*/)
	{
	return EFalse;
	}


// CWinFunctionTestAnim test drawing off own timer //
CWinFunctionTestAnim::~CWinFunctionTestAnim()
	{}

void CWinFunctionTestAnim::ConstructL(TAny* /*aParam*/,TBool)
	{
	iWinFunctions=static_cast<MAnimGeneralFunctionsWindowExtension*>
										(iFunctions->ExtendedInterface(MAnimGeneralFunctions::EWindowExtensionInterface));
	}

void CWinFunctionTestAnim::Animate(TDateTime*)
	{}

void CWinFunctionTestAnim::Redraw()
	{}

void CWinFunctionTestAnim::Command(TInt /*aOpcode*/,TAny* /*aParam*/)
	{
	/*switch (aOpcode)
		{
	default:;
		}*/
	}

TInt CWinFunctionTestAnim::CommandReplyL(TInt aOpcode,TAny* aParam)
	{
	TAnimArgUnion pData;
	pData.any=aParam;
	switch (aOpcode)
		{
	case EADllScreen:
		return iWinFunctions->Screens();
	case EADllFocusScreen:
		return iWinFunctions->FocusScreens();
	case EADllWindowGroups:
		return iWinFunctions->WindowGroups(*pData.Int);
	case EADllWindowGroupInfo:
		{
		const TWindowGroupInfoParms& params=*pData.WindowGroupInfoParms;
		TPckgBuf<MAnimGeneralFunctionsWindowExtension::TWindowGroupInfo> info;
		TBool ret=iWinFunctions->WindowGroupInfo(info(),params.iScreen,params.iOrdinalPosition);
		/*const TInt theError=*/(*iFunctions->Message()).Write(KIpcSlot,info);
		return ret;
		}
	case EADllWindowGroupName:
		{
		const TWindowGroupInfoParms& params=*pData.WindowGroupInfoParms;
		TPtrC name;
		TBool ret=iWinFunctions->WindowGroupName(name,params.iScreen,params.iOrdinalPosition);
		/*const TInt theError=*/(*iFunctions->Message()).Write(KIpcSlot,name);
		return ret;
		}
	case EADllSetOrdinalPosition:
		{
		const TSetOrdinalParms& params=*pData.SetOrdinalParms;
		return iWinFunctions->SetOrdinalPosition(params.iIdentifier,params.iOrdinalPosition,params.iOrdinalPriority);
		}
	case EADllSetFocusScreen:
		{
		const TInt& params=*pData.Int;
 		iWinFunctions->SetFocusScreen(params);
 		break;
		}
	default:;
		}
	return(KErrNone);
	}

void CWinFunctionTestAnim::FocusChanged(TBool /*aState*/)
	{}

TBool CWinFunctionTestAnim::OfferRawEvent(const TRawEvent& /*aRawEvent*/)
	{
	return EFalse;
	}


// CNotificationTestAnim
CNotificationTestAnim::CNotificationTestAnim() : iHeartbeatState(EFalse) , iScreenDeviceChanged(ETrue)
	{
	}

CNotificationTestAnim::~CNotificationTestAnim()
	{
//	Removed for test case GRAPHICS-WSERV-0139 for DEF12220
//	iFunctions->RegisterForNotifications(EFalse);
	}

void CNotificationTestAnim::ConstructL(TAny* /*aParam*/, TBool )
	{
	iWindowFunctions->SetRect(TRect(0,0,1,1));
	iFunctions->RegisterForNotifications(EDirectScreenAccess|EHeartbeatTimer|EScreenDeviceChange);
	iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
	iLastAnimTime.UniversalTime();
	}

void CNotificationTestAnim::HandleNotification(const TWsEvent& aEvent)
	{
	switch (aEvent.Type())
		{
	case EEventDirectScreenAccessBegin:
		{
		TInt screenNum = *(aEvent.Int());
		iDSA[screenNum] = ETrue;
		}
		break;
	case EEventDirectScreenAccessEnd:
		{
		TInt screenNum = *(aEvent.Int());
		iDSA[screenNum] = EFalse;
		}
		break;
	case EEventHeartbeatTimerStateChange:
		{
		iHeartbeatState=*aEvent.Int();
		}
		break;
	case EEventScreenDeviceChanged:
		{
		iScreenDeviceChanged=!iScreenDeviceChanged;
		}
		break;
	default:
		break;
		}
	}

void CNotificationTestAnim::Animate(TDateTime*)
	{
	iLastAnimTime.UniversalTime();
	}

void CNotificationTestAnim::Redraw()
	{
	}

void CNotificationTestAnim::Command(TInt /*aOpcode*/, TAny* /*aParam*/)
	{
	}

TInt CNotificationTestAnim::CommandReplyL(TInt aOpcode,TAny* aParam)
	{
	TAnimArgUnion pData;
	pData.any=aParam;
	switch (aOpcode)
		{
	case EADllQueryDSA:
		{
		TInt screenNum = *pData.Int;
		return iDSA[screenNum];
		}
	case EADllQueryHeartbeatState:
		{
		return iHeartbeatState;
		}
	case EADllIsAnimating:
		{
		// Check if the anim DLL is currently animating by comparing
		// the current time with the last anim time - if this is greater than a second
		// then we are not animating
		TTime timeNow;
		timeNow.UniversalTime();
		TTimeIntervalMicroSeconds microSecs = timeNow.MicroSecondsFrom(iLastAnimTime);
		if(microSecs > TTimeIntervalMicroSeconds(1200000)) // 1 sec plus 0.2 secs to cope with any latency
			{
			return EFalse;
			}
		else
			{
			return ETrue;
			}
		}
	default:;
		}
	return(KErrNone);
	}

void CNotificationTestAnim::FocusChanged(TBool /*aState*/)
	{
	}

TBool CNotificationTestAnim::OfferRawEvent(const TRawEvent& /*aRawEvent*/)
	{
	return EFalse;
	}

// CTransAnim test drawing//
void CTransAnim::ConstructL(TAny* aParam, TBool)
	{
	iRect=*static_cast<TRect*>(aParam);//Assigns the animation area
	iWindowFunctions->SetRect(iRect);
	iFunctions->SetSync(MAnimGeneralFunctions::ESyncFlash);
	iFont=NULL;
	iColor=TRgb(0,0,0);
	}

void CTransAnim::Animate(TDateTime*)
	{
	iWindowFunctions->ActivateGc();
	if(iMode==1)		//Draw line mode
		{
		DoDraw(iDraw,*iGc,iColor,iRect);
		}
	else if(iMode==2)	//Draw text mode
		{
		DoDraw(iDraw,*iGc,iColor,iRect,iFont,iCount);
		}
	}

void CTransAnim::Redraw()
	{
	DoDraw(iDraw,*iGc,iColor,iRect,iFont,iCount);
	}

void CTransAnim::Command(TInt aOpcode,TAny* aParam)
	{
	TAnimArgUnion pData;
	pData.any=aParam;
	switch(aOpcode)
		{
		case EADllDrawNow:	//Used to draw a single frame
			iWindowFunctions->ActivateGc();
			iMode=1;
			iDraw=36;
			iColor=TRgb(0,0,0);
			DoDraw(iDraw,*iGc,iColor,iRect);
			break;
		case EADllNextFrame:	//Used to draw a continous frame
			iMode=2;
			iColor=pData.FrameData->color;
			iDraw=pData.FrameData->draw;
			iCount=pData.FrameData->text;
			break;
		case EADllStartAnimText:	//Used to start text animate
			iWindowFunctions->ActivateGc();
			iMode=2;
			iDraw=37;
			iFont=iFunctions->DuplicateFontL(pData.FrameData->font);
			iColor=pData.FrameData->color;
			iCount=pData.FrameData->text;
			if(pData.FrameData->sync==EStateFlash)
				iFunctions->SetSync(MAnimGeneralFunctions::ESyncFlash);
			else if(pData.FrameData->sync==EStateSecond)
				iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
			DoDraw(iDraw,*iGc,iColor,iRect,iFont,iCount);
			break;
		case EADllEndAnimText:		//Used to end text animate
			iFunctions->SetSync(MAnimGeneralFunctions::ESyncNone);
			iWindowFunctions->ActivateGc();
			iCount=0;
			iMode=0;
			iFunctions->CloseFont(iFont);
			iFont=NULL;
			break;
		}
	}

TInt CTransAnim::CommandReplyL(TInt /*aOpcode*/, TAny* /*aParam*/)
	{
	return(0);
	}

void CTransAnim::FocusChanged(TBool /*aState*/)
	{}

TBool CTransAnim::OfferRawEvent(const TRawEvent& /*aRawEvent*/)
	{
	return EFalse;
	}

// an anim dll that doesn't remove itself from the event handler list
CTestHandlerAnim::~CTestHandlerAnim()
	{
	}

void CTestHandlerAnim::ConstructL(TAny* /*aArgs*/, TBool /*aHasFocus*/)
	{
	iFunctions->GetRawEvents(ETrue);
	}

void CTestHandlerAnim::Redraw()
	{
	}

void CTestHandlerAnim::FocusChanged(TBool /*aState*/)
	{
	}

TInt CTestHandlerAnim::CommandReplyL(TInt /*aOpcode*/, TAny* /*aArgs*/)
	{
	return KErrNone;
	}

void CTestHandlerAnim::Command(TInt /*aOpcode*/, TAny* /*aArgs*/)
	{
	}

void CTestHandlerAnim::Animate(TDateTime* /*aDateTime*/)
	{
	}

TBool CTestHandlerAnim::OfferRawEvent(const TRawEvent &/*aRawEvent*/)
	{
	return EFalse;
	}

// CRemovableAnim Test removal of anim during event handling //
CRemovableAnim::~CRemovableAnim()
	{
	}

TInt CRemovableAnim::AddExpectedEvent(const TAnimRawEvent* aEvent)
	{
	if (TotalExpectedEvents() + EventCount() < iLifetime)
		{
		return (AddEvent(aEvent) ? KErrNone : KErrOverflow);
		}
	else
		{
		return KErrNone;
		}
	}

TInt CRemovableAnim::CommandReplyL(TInt aOpcode,TAny* aParam)
	{
	TAnimArgUnion pData;
	pData.any=aParam;
	switch (aOpcode)
		{
	case EADllSetEventHandlerLifetime:
		iLifetime = *pData.Int;
		return iLifetime;
	case EADllEventHandlerLifetime:
		return iLifetime;
	case EADllAddToEventHandlers:
		iFunctions->GetRawEvents(ETrue);
		return KErrNone;
	case EADllRemoveFromEventHandlers:
		iFunctions->GetRawEvents(EFalse);
		return KErrNone;
	case EADllTestPassed:
		{
		// If any unexpected events are received, then Error() > 0
		// If fewer than the number of expected events are received, then TotalExpectedEvents() > 0
		TBool passed = (Error() == 0) && (TotalExpectedEvents() == 0);
		return passed;
		}
	default:
		return CEventTestAnimBase::CommandReplyL(aOpcode, aParam);
		}
	}


TBool CRemovableAnim::OfferRawEvent(const TRawEvent& aRawEvent)
	{
	TBool eat = CEventTestAnimBase::OfferRawEvent(aRawEvent);

	// Remove anim from event handlers if lifetime has been exceeded
	if (EventCount() >= iLifetime)
		{
		iFunctions->GetRawEvents(EFalse);
		}
	return eat;
	}

// CMultiPointerAnim for Testing multipointerevents for anims //
CMultiPointerAnim::~CMultiPointerAnim()
	{
	iFunctions->GetRawEvents(EFalse);
	}

// Important thing here is to call GetRawEvents() which Switches animation raw event handling on
void CMultiPointerAnim::ConstructL(TAny */*aArgs*/, TBool /*aHasFocus*/)
	{
	iExpectedEvents.SetLengthL(EEventBufferSize);
	iFunctions->GetRawEvents(ETrue);
	}

void CMultiPointerAnim::Animate(TDateTime */*aDateTime*/)
	{
	}

void CMultiPointerAnim::Command(TInt /*aOpcode*/, TAny */*aArgs*/)
	{
	}

TInt CMultiPointerAnim::CommandReplyL(TInt aOpcode, TAny *aArgs)
	{
	TAnimArgUnion pData;
	pData.any=aArgs;
	switch (aOpcode)
		{
	case EADllAddExpectedMultiPtrEvent:
		return AddEvent(pData.AnimRawEvent);
	case EADllMultiPtrEventError:
		return iError;
	case EADllMultiPtrEventErrorDesc:
		if (iError != KErrNone)
			{
			// returns the error description which gets displayed in logs if test failed
			const RMessagePtr2& message=*iFunctions->Message();
			const TInt error=message.Write(KIpcSlot, iErrorDes);
			if (error != KErrNone)
				{
				return error;
				}
			}
		break;
	case EADllMultiPtrEventReset:
		iExpectedEvents.Reset();
		iError = 0;
		iEventCount = 0;
		break;
	default:;
		}
	return(KErrNone);
	}

void CMultiPointerAnim::Redraw()
	{
	}

void CMultiPointerAnim::FocusChanged(TBool /*aState*/)
	{
	}

TBool CMultiPointerAnim::OfferRawEvent(const TRawEvent &aRawEvent)
	{
	// Ignore following raw events
	TRawEvent::TType aType=aRawEvent.Type();
	if (aType==TRawEvent::EActive || aType==TRawEvent::EInactive || aType==TRawEvent::ERedraw
			|| aType==TRawEvent::EPointerSwitchOn || aType==TRawEvent::EUpdateModifiers)
		{
		return EFalse;
		}
	
	// If it has already failed then do not test other events becasue the error value 
	// and its descriptor will be over written
	if (iError)
		{
		return EFalse;;
		}
		
	TAnimRawEvent expectedEvent; 
	if (iExpectedEvents.Remove(&expectedEvent)<1)
		{
		return EFalse;
		}
	
	// Increment event count
	iEventCount++;
	
	// Call compare events function passing in both the events
	if (!CompareEvents(expectedEvent.iRawEvent, aRawEvent))
		{
		iError = iEventCount;
		}
	
	return expectedEvent.iEat;
	}

TInt CMultiPointerAnim::AddEvent(const TAnimRawEvent* aEvent)
	{
	return iExpectedEvents.Add(aEvent);
	}

TBool CMultiPointerAnim::CompareEvents(const TRawEvent& aExpectedEvent, const TRawEvent& aActualEvent)
	{
	// Check pointer type
	if (aExpectedEvent.Type() != aActualEvent.Type())
		{
		_LIT(KEventType, "Actual Event type = %d Expected Event Type = %d ");
		iErrorDes.Format(KEventType, aActualEvent.Type(), aExpectedEvent.Type());
		return EFalse;
		}
	
	// Here for EOutOfRange we cannot query Pos or Pos3D...
	if (aExpectedEvent.Type() != TRawEvent::EPointer3DOutOfRange)
		{
		// Check 3d position
		TPoint3D expected3DPos = aExpectedEvent.Pos3D();
		TPoint3D actual3DPos = aActualEvent.Pos3D();
		if (expected3DPos.iX != actual3DPos.iX)
			{
			_LIT(KXPosition, "Actual X coordinate = %d Expected X coordinate = %d ");
			iErrorDes.Format(KXPosition, actual3DPos.iX, expected3DPos.iX);
			return EFalse;
			}
		if (expected3DPos.iY != actual3DPos.iY)
			{
			_LIT(KYPosition, "Actual Y coordinate = %d Expected Y coordinate = %d ");
			iErrorDes.Format(KYPosition, actual3DPos.iY, expected3DPos.iY);
			return EFalse;
			}
		if (expected3DPos.iZ != actual3DPos.iZ)
			{
			_LIT(KZPosition, "Actual Z coordinate = %d Expected Z coordinate = %d ");
			iErrorDes.Format(KZPosition, actual3DPos.iZ, expected3DPos.iZ);
			return EFalse;
			}
		}

	// Check pointer number
	if (aExpectedEvent.PointerNumber() != aActualEvent.PointerNumber())
		{
		_LIT(KPointerNumber, "Actual Pointer number = %d Expected Pointer number = %d ");
		iErrorDes.Copy(KPointerNumber);
		iErrorDes.Format(KPointerNumber, aActualEvent.PointerNumber(), aExpectedEvent.PointerNumber());
		return EFalse;
		}
	return ETrue;
	}

// CCoverageAnim test drawing/redrawing //
CCoverageAnim::~CCoverageAnim()
	{
	if(iFont && iFunctions)
		iFunctions->CloseFont(iFont);
	} 

/**
 Initializes members.
 */
void CCoverageAnim::ConstructL(TAny *aParam, TBool )
	{
	iRect=*static_cast<TRect*>(aParam);
	iWindowFunctions->SetRect(iRect);
	iWindowFunctions->Invalidate(iRect);
	
	iFunctions->SetSync(MAnimGeneralFunctions::ESyncDay);
	if (iFunctions->Sync() != MAnimGeneralFunctions::ESyncDay)
		User::Leave(KErrGeneral);	 

	iFunctions->SetSync(MAnimGeneralFunctions::ESyncMinute);
	if (iFunctions->Sync() != MAnimGeneralFunctions::ESyncMinute)
		User::Leave(KErrGeneral);	 

	iFunctions->SetSync(MAnimGeneralFunctions::ESyncSecond);
	if (iFunctions->Sync() != MAnimGeneralFunctions::ESyncSecond)
		User::Leave(KErrGeneral);	 

	iFunctions->SetSync(MAnimGeneralFunctions::ESyncFlash);
	if (iFunctions->Sync() != MAnimGeneralFunctions::ESyncFlash)
		User::Leave(KErrGeneral);	 
		
	iFunctions->SetSync(MAnimGeneralFunctions::ESyncNone);
	if (iFunctions->Sync() != MAnimGeneralFunctions::ESyncNone)
		User::Leave(KErrGeneral);	 
		
	iFunctions->SetInterval(1);
	iFunctions->SetInterval(0);
	iFunctions->SetNextInterval(0);
		
	User::LeaveIfNull(iFunctions->ExtendedInterface(MAnimGeneralFunctions::ENumberOfExtendedInterfaces));
	User::LeaveIfNull(iFunctions->ExtendedInterface(MAnimGeneralFunctions::EWindowExtensionInterface));
	User::LeaveIfNull(iFunctions->ExtendedInterface(MAnimGeneralFunctions::EEventExtentionInterface));
	User::LeaveIfError(iFunctions->ExtendedInterface(MAnimGeneralFunctions::EInterfaceCount) != NULL);

	#if !defined(SYMBIAN_GRAPHICS_GCE)
		// Deprecated in NGA but needs to be tested in NONNGA
	User::LeaveIfNull((TAny*)iFunctions->ScreenDevice());
	#endif

	iFunctions->Client();
	}

/**
 Copied from CAutoAnimScrBack.
 */
void CCoverageAnim::Animate(TDateTime *)
	{
	iWindowFunctions->ActivateGc();
	Draw();
	}

/**
 All methods of the graphic-context are executed one by one.
 */
void CCoverageAnim::Draw()
	{
	iGc->SetClippingRect(iRect);
	iGc->Clear();
	iGc->Clear(iRect);

	iGc->SetDrawMode(CGraphicsContext::EDrawModeAND);
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iGc->SetBrushColor(TRgb::Gray256(85));
	iGc->SetBrushOrigin(TPoint(0,0));
	iGc->SetPenColor(TRgb::Gray256(170));

	//primitive method calls
	iGc->SetFaded(ETrue);
	iGc->SetFadingParameters(1, 1);

	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iGc->SetStrikethroughStyle(EStrikethroughOff);
	iGc->SetUnderlineStyle(EUnderlineOff);
	iGc->SetWordJustification(2, 1);

	if(iFont)
		iGc->UseFont(iFont);
	
	iGc->DrawArc(iRect,
			TPoint(iRect.Center().iX, iRect.iTl.iY),
			TPoint(iRect.iBr.iX, iRect.Center().iY));
	iGc->DrawLine(iRect.iTl,iRect.Center());
	iGc->DrawLineTo(TPoint(iRect.iBr.iX, iRect.iTl.iY));

	iGc->DrawLineBy(TPoint(iRect.iTl.iX, iRect.iBr.iY));
	iGc->MoveBy(iRect.iTl + TPoint(1,1));
	iGc->MoveTo(iRect.iTl + TPoint(0,0)); 
	iGc->SetPenSize(TSize(10,10));
	iGc->Plot(iRect.iTl + TPoint(2,2));
	iGc->SetPenSize(TSize(1,1));

	CArrayFixFlat<TPoint>* polyPoints = new(ELeave) CArrayFixFlat<TPoint>(3); //CArrayFixFlat
	CleanupStack::PushL(polyPoints);
	polyPoints->AppendL(iRect.iTl);
	polyPoints->AppendL(iRect.Center());
	polyPoints->AppendL(TPoint(iRect.iBr.iX, iRect.iTl.iY));
	
	iGc->DrawPolyLine(polyPoints);
	iGc->DrawPolyLine(&polyPoints->At(0), 3);
	iGc->DrawPolygon(polyPoints, CGraphicsContext::EWinding);
	iGc->DrawPolygon(&polyPoints->At(0), 3, CGraphicsContext::EAlternate);

	iGc->DrawPie(iRect, TPoint(iRect.Center().iX, iRect.iTl.iY), TPoint(iRect.iBr.iX, iRect.Center().iY));
	iGc->DrawEllipse(iRect);
	iGc->DrawRect(iRect);
	iGc->DrawRoundRect(iRect, TSize(iRect.Width()/8, iRect.Height()/8));

	CleanupStack::PopAndDestroy(polyPoints);

	iGc->CopyRect(TPoint(10, 10), iRect);

	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(16,16),EGray4));

	iGc->UseBrushPattern(bitmap);
	iGc->DrawBitmap(iRect.iTl, bitmap);
	iGc->DrawBitmap(iRect, bitmap);
	iGc->DrawBitmap(iRect, bitmap, TRect(0, 0, 16, 16));
	iGc->DrawBitmapMasked(iRect, bitmap, TRect(0, 0, 16, 16), bitmap, ETrue);

	iGc->BitBlt(TPoint(0, 0), bitmap);
	iGc->BitBlt(TPoint(0, 0), bitmap, iRect);
	iGc->BitBltMasked(TPoint(0, 0), bitmap, iRect, bitmap, ETrue);
	iGc->AlphaBlendBitmaps(TPoint(0, 0), bitmap, iRect, bitmap, TPoint(0,0));
	
	CleanupStack::PopAndDestroy(bitmap);

	iGc->SetCharJustification(1,1);
	iGc->SetClippingRect(iRect);
	if(iFont)
		{
		_LIT(KHelloWorld,"Hello World");
		iGc->DrawText(*&KHelloWorld, iRect.iTl);
		iGc->DrawText(*&KHelloWorld, iRect, 0, CGraphicsContext::ELeft, 0);
		iGc->DrawTextVertical(*&KHelloWorld, iRect.iBr, ETrue);
		iGc->DrawTextVertical(*&KHelloWorld, iRect, 0, ETrue, CGraphicsContext::ELeft, 0);
		}
	
	TRgb rgbs[2];
	iGc->MapColors(iRect, rgbs, 1, ETrue);
	iGc->DiscardBrushPattern();

	if(iFont)
		iGc->DiscardFont();
	
	iFunctions->CloseFont(0);
	
	TDateTime dt = iFunctions->SystemTime();
	}
	
/**
 Copied from CAutoAnim3.
 */
void CCoverageAnim::Redraw()
	{
	Draw();
	}

/**
 Copied from CAutoAnim3.
 */
void CCoverageAnim::Command(TInt ,TAny*)
	{
	}

/**
 Copied from CAutoAnim3.
 */
TInt CCoverageAnim::CommandReplyL(TInt aOpcode , TAny* aParam)
	{
	TAnimArgUnion pData;
	pData.any=aParam;
	switch(aOpcode)
		{
		case EADllDrawNow:
			iFunctions->SetSync(MAnimGeneralFunctions::ESyncFlash);
			break;
		case EADllSetFont:
			if(iFont)
				{
				iFunctions->CloseFont(iFont);
				iFont = NULL;
				}
			iFont = iFunctions->DuplicateFontL(*pData.Int);
			break;
		} 
	return(KErrNone);
	}

/**
 Copied from CAutoAnim3.
 */
void CCoverageAnim::FocusChanged(TBool )
	{}

/**
 Copied from CAutoAnim3.
 */
TBool CCoverageAnim::OfferRawEvent(const TRawEvent &/*aRawEvent*/)
	{
	return EFalse;
	}


// DLL code //

CAnim *CTestAnimDll::CreateInstanceL(TInt aType)
	{
	CAnim *anim=NULL;
	switch(aType)
		{
		case EAnimTypeTest1:
			anim=new(ELeave) CPanicAnim();
			break;
		case EAnimTypeTest2:
			anim=new(ELeave) CPanicAnim2();
			break;
		case EAnimTypeTest3:
			anim=new(ELeave) CAutoAnim3();
			break;
		case EAnimTypeScrBackTest:
			anim=new(ELeave) CAutoAnimScrBack();
			break;
		case EAnimTypeSprite:
			anim=new(ELeave) CTestSpriteAnim();
			break;
		case EAnimTypeFreeTimer:
			anim=new(ELeave) CTestFreeTimerAnim();
			break;
		case EAnimTypeEventTest:
			anim=new(ELeave) CEventTestAnim();
			break;
		case EAnimTypeNotificationTest:
			anim=new(ELeave) CNotificationTestAnim();
			break;
		case EAnimTypeTimer:
			anim=new(ELeave) CTimerTestAnim();
			break;
		case EAnimTypeWindowFunctions:
			anim=new(ELeave) CWinFunctionTestAnim();
			break;
		case EAnimTypeTrans:
			anim=new(ELeave) CTransAnim();
			break;
		case EAnimTypeEventHandler:
			anim = new (ELeave) CTestHandlerAnim();
			break;
		case EAnimTypeRemovableAnim:
			anim=new(ELeave) CRemovableAnim();
			break;
		case EAnimTypeEventPostingTest:
			anim=new(ELeave) CEventPostingAnim();
			break;
		case EAnimTypeCoverage:
			anim=new(ELeave) CCoverageAnim();
			break;
		case EAnimTypeMultiPointer:
			anim=new(ELeave) CMultiPointerAnim();
			break;
		}
	return(anim);
	}

// Dummy E32Dll needed by E32 to build //

void CAutoAnim3::DrawTestScreenL(const TDrawTestScreen* aParams)
//Moved here as the include messes up the debugging of the rest of the file.
	{
	CFbsBitmap *aBitmap=iFunctions->DuplicateBitmapL(aParams->bitmap);
	CleanupStack::PushL(aBitmap);
	CFbsBitmap *aMaskBitmap=iFunctions->DuplicateBitmapL(aParams->maskBitmap);
	CleanupStack::PushL(aMaskBitmap);
	CFbsFont *aFont=iFont=iFunctions->DuplicateFontL(aParams->font);
	TSize size(iWindowFunctions->WindowSize());
	iWindowFunctions->ActivateGc();
	CBitmapContext *gc=iGc;
	TBool aExtraDrawBitMap=ETrue;
//
#include "DLLDRAW.H"
//
	iFunctions->CloseFont(iFont);
	iFont=NULL;
	CleanupStack::PopAndDestroy(2);
	}
