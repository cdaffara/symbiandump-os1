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
// Base classes definitions used for building window server test code
// 
//

#ifndef __TESTBASE_H__
#define __TESTBASE_H__

enum TTlibActivePriorities
	{
	ETlibRedrawActivePriority=-10,
	ETlibWsEventActivePriority=0,
	};

const TInt KDefaultScreen = 0;

//enum {ENullWsHandle=0xFFFFFFFF};	// Events delivered to this handle are thrown away
const TUint32 ENullWsHandle=0xFFFFFFFF;	// Events delivered to this handle are thrown away

typedef TBuf<KMaxFullName> TWindowTitle;
typedef TBuf<0x100> TWinCommand;

const TUid KUidWServSecurityTesting={0x10205152};
enum TWServSecTest
	{
	EWServSecTestBufferSecurity
	};

class CTClient;	// Forward reference
class CTWindowGroup;
class CTBaseWin;
class CTWin;

typedef CTClient *(*TCreateClientFunc)();

IMPORT_C TBool CheckBlankWindow(TRect aArea,TRgb aColor,const CWsScreenDevice* aScreen);

class CTWinBase : public CBase
	{
protected:
public:
	IMPORT_C CTWinBase(TInt aType);
	IMPORT_C virtual TPoint Position() const;
	IMPORT_C virtual void PointerEnter(const TTime &aTime);
	IMPORT_C virtual void PointerExit(const TTime &aTime);
	IMPORT_C virtual void PointerBufferReady(const TTime &aTime);
	IMPORT_C virtual void PointerL(const TPointerEvent &aPointer,const TTime &aTime);
	IMPORT_C virtual void SwitchOn(const TTime &aTime);
	IMPORT_C virtual void ModifiersChanged(const TModifiersChangedEvent &aModifiersChanged ,const TTime &aTime);
	IMPORT_C virtual void ErrorMessage(const TWsErrorMessage &aErrorMessage, const TTime &aTime);
	IMPORT_C virtual void SetVisible(TBool aState);
	IMPORT_C CTWinBase *Parent() const;
	IMPORT_C CTWinBase *NextSibling() const;
	IMPORT_C CTWinBase *PrevSibling() const;
	IMPORT_C CTBaseWin *Child() const;
	IMPORT_C virtual void AdjustOrdinal(TInt adjust);
	IMPORT_C virtual void AdjustShadow(TInt aAdjust);
	IMPORT_C virtual TInt SubType();
// Pure virtual definitions
	virtual void WinKeyL(const TKeyEvent &aKey,const TTime &aTime)=0;
	virtual TSize Size() const=0;
	virtual RWindowTreeNode *WinTreeNode()=0;
	virtual const RWindowTreeNode *WinTreeNode() const=0;
	IMPORT_C virtual CTClient *Client() const;
	virtual CTWindowGroup *Group() const=0;
	CTWindowGroup *iOwnerWin;
public:
	TInt iType;
	__DECLARE_TEST;
	};

class CTBaseWin : public CTWinBase
	{
public:
	IMPORT_C CTBaseWin(TInt aType);
	IMPORT_C ~CTBaseWin();
	IMPORT_C virtual void ConstructL(CTWinBase &parent);
	IMPORT_C virtual void ConstructExtLD(CTWinBase &aParent, const TPoint &aPos, const TSize &aSize);
	virtual TInt ConstructWin(const CTWinBase &aWs)=0;
	IMPORT_C virtual void InitWin();
	IMPORT_C virtual void PointerL(const TPointerEvent &aPointer,const TTime &aTime);
	IMPORT_C virtual void DragDropL(const TPointerEvent &aPointer,const TTime &aTime);
	IMPORT_C virtual void RelinquishFocus();
	IMPORT_C void Activate();
	IMPORT_C virtual void AssignGC(CWindowGc &aGc);
	IMPORT_C CTBaseWin *Next();
	IMPORT_C CTBaseWin *Prev();
	IMPORT_C virtual void SetExtL(const TPoint &aPos, const TSize &aSize);
	IMPORT_C virtual void SetSizeL(const TSize &aSize);
	IMPORT_C virtual void SetPos(const TPoint &aPos);
	IMPORT_C void SetInitialPos(const TPoint &aPos);
	IMPORT_C virtual void SetDefaultExtL();
	IMPORT_C virtual void SetFullScreenExtL();
	IMPORT_C virtual void AdjustSizeL(TInt xMove,TInt yMove,TInt resize);
	IMPORT_C virtual TSize Size() const;
	IMPORT_C virtual void Draw();
	IMPORT_C void DrawBorder();
//	void FillWindow(TInt inset);
	IMPORT_C virtual TPoint Position() const;
	IMPORT_C static void Delete(CTBaseWin *aWin);
	IMPORT_C void AdjustOrdinal(TInt aAdjust);
	IMPORT_C virtual void AdjustShadow(TInt aAdjust);
	IMPORT_C void SetVisible(TBool aState);
	IMPORT_C RWindowTreeNode *WinTreeNode();
	IMPORT_C const RWindowTreeNode *WinTreeNode() const;
	IMPORT_C virtual void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	IMPORT_C virtual void KeyUpL(const TKeyEvent &aKey,const TTime &aTime);
	IMPORT_C virtual void KeyDownL(const TKeyEvent &aKey,const TTime &aTime);
	IMPORT_C virtual void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc);
	IMPORT_C virtual void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc, TDisplayMode *aMode);
	IMPORT_C void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc,TBool aVisible);
	enum 
		{
		ENoTransparency=0x100
		};
	IMPORT_C virtual void SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc, TDisplayMode *aMode,TBool aVisible,TInt aTransparency=ENoTransparency);
	IMPORT_C void SetDragRect(const TRect &aRect);
	IMPORT_C virtual void Resized(const TSize &aSize);
	IMPORT_C virtual void FocusChanged(TBool aState);
	IMPORT_C virtual CTWindowGroup *Group() const;
	virtual RWindowBase *BaseWin()=0;
	virtual const RWindowBase *BaseWin() const=0;
	inline CWindowGc *Gc();
protected:
	TSize iSize;
	CWindowGc *iGc;		   	// Not owned by the window, just using it
	TPoint iPos;
	TInt iShadow;
	TRect iDragRect;
	TBool iDragging;
	TPoint iDragPos;
	CFbsFont *iFont;
	__DECLARE_TEST;
	};

class CTDrawableWin : public CTBaseWin
	{
public:
	IMPORT_C CTDrawableWin(TInt aType);
	virtual RDrawableWindow *DrawableWin()=0;
	virtual const RDrawableWindow *DrawableWin() const=0;
	};

class CTWin : public CTDrawableWin
	{
public:
	IMPORT_C CTWin();
	IMPORT_C ~CTWin();
	inline RWindow *Win() const;
	IMPORT_C virtual TInt ConstructWin(const CTWinBase &aWs);
	IMPORT_C void Invalidate();
	IMPORT_C void Invalidate(const TRect &rect);
	IMPORT_C virtual void Redraw();
	IMPORT_C virtual void Redraw(const TRect &aRect);
	IMPORT_C void DrawNow();
	IMPORT_C void SetExt(const TPoint &aPos, const TSize &aSize);
	IMPORT_C void SetSize(const TSize &aSize);
	IMPORT_C void SetDefaultExt();
	IMPORT_C void AdjustSize(TInt xMove,TInt yMove,TInt resize);
	IMPORT_C virtual RWindowBase *BaseWin();
	IMPORT_C virtual const RWindowBase *BaseWin() const;
	IMPORT_C virtual RDrawableWindow *DrawableWin();
	IMPORT_C virtual const RDrawableWindow *DrawableWin() const;
protected:
	RWindow iWin;
	__DECLARE_TEST;
	};

class CTBackedUpWin : public CTDrawableWin
	{
public:
	IMPORT_C CTBackedUpWin(TDisplayMode aDisplayMode);
	IMPORT_C ~CTBackedUpWin();
	IMPORT_C virtual TInt ConstructWin(const CTWinBase &aWs);
	IMPORT_C virtual RWindowBase *BaseWin();
	IMPORT_C virtual const RWindowBase *BaseWin() const;
	IMPORT_C virtual RDrawableWindow *DrawableWin();
	IMPORT_C virtual const RDrawableWindow *DrawableWin() const;
	IMPORT_C RBackedUpWindow *BackedUpWin();
	IMPORT_C const RBackedUpWindow *BackedUpWin() const;
protected:
	RBackedUpWindow iWin;
	TDisplayMode iDisplayMode;
	__DECLARE_TEST;
	};

class CTTitledWindow : public CTWin
	{
public:
	IMPORT_C CTTitledWindow();
	IMPORT_C ~CTTitledWindow();
	IMPORT_C void ConstructL(CTWinBase &parent);
	IMPORT_C void Draw();
	IMPORT_C void FocusChanged(TBool aState);
	IMPORT_C void SetTitle(const TWindowTitle &aTitle);
	IMPORT_C void SetColor(TRgb aRgb);
	inline TRgb Color();
private:
	IMPORT_C void Resized(const TSize &aSize);
protected:
	TWindowTitle iTitle;
	TInt iTitleHeight;
	TBool iFocus;
	TRgb iWinColor;
	TRgb iPenColor;
	};

class CTBlankWindow : public CTBaseWin
	{
public:
	IMPORT_C CTBlankWindow();
	IMPORT_C ~CTBlankWindow();
	IMPORT_C void ConstructL(CTWinBase &aParent);
	IMPORT_C void SetColor(const TRgb &aRgb);
	IMPORT_C virtual TInt ConstructWin(const class CTWinBase&);
	IMPORT_C virtual const RWindowBase *BaseWin()const;
	IMPORT_C virtual RWindowBase *BaseWin();
	inline const RBlankWindow *BlankWin()const;
	inline RBlankWindow *BlankWin();
	IMPORT_C void SetExt(const TPoint &aPos, const TSize &aSize);
	IMPORT_C void SetSize(const TSize &aSize);
protected:
	RBlankWindow iWin;
	};

class CTWindowGroup : public CTWinBase
	{
public:
	IMPORT_C CTWindowGroup(CTClient *aClient);
	IMPORT_C ~CTWindowGroup();
	IMPORT_C virtual void ConstructL();
	IMPORT_C void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	IMPORT_C virtual TSize Size() const;
	IMPORT_C RWindowTreeNode *WinTreeNode();
	IMPORT_C const RWindowTreeNode *WinTreeNode() const;
	inline RWindowGroup *GroupWin();
	IMPORT_C virtual void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	IMPORT_C virtual void KeyUpL(const TKeyEvent &aKey,const TTime &aTime);
	IMPORT_C virtual void KeyDownL(const TKeyEvent &aKey,const TTime &aTime);
	IMPORT_C virtual void PasswordL(const TTime &aTime);
	IMPORT_C virtual void MessageReady(const TWsEvent &aTime);
	IMPORT_C virtual void ScreenDeviceChanged();
	IMPORT_C virtual void UserEvent(TInt aEventType);
	IMPORT_C void SetCurrentWindow(CTBaseWin *aWindow);
	IMPORT_C void SetCurrentWindow(CTBaseWin *aWindow,TBool aLocked);
	IMPORT_C CTBaseWin *CurWin(void) const;
	IMPORT_C void FocusLost();
	IMPORT_C void FocusGained();
	IMPORT_C TBool HasFocus(CTBaseWin *aWin) const;
	IMPORT_C virtual CTWindowGroup *Group() const;
	IMPORT_C virtual CTClient *Client() const;
	IMPORT_C void ClearCurrentWindow();
protected:
	CTClient *iClient;
	CTBaseWin *iCurWin;
	RWindowGroup iGroupWin;
	TBool iLocked;
	TBool iFocus;
	};

inline RWindowGroup *CTWindowGroup::GroupWin()
	{return(&iGroupWin);}

class CTEventBase : public CActive
	{
public:
	IMPORT_C CTEventBase(RWsSession *aWs, TInt aPriority);
	IMPORT_C ~CTEventBase();
	IMPORT_C void Construct();
	void SetCancelFunction(const TCallBack &aCallBack);
	virtual void Request()=0;
	inline TInt Count() {return iCount;}
protected:
	virtual void doRunL()=0;
	IMPORT_C void RunL();
	IMPORT_C void CancelHandler();
protected:
	TBool iCancelRequested;
	TCallBack iCancelCallBack;
	RWsSession *iWs;
	TInt iCount;
	};

class CTRedraw : public CTEventBase
	{
public:
	IMPORT_C CTRedraw(RWsSession *aWs);
	IMPORT_C ~CTRedraw();
	IMPORT_C void Request();
protected:
	IMPORT_C void DoCancel();
	IMPORT_C void doRunL();
	};

class TlibWsEvent : public TWsEvent
	{
public:
	IMPORT_C CTWindowGroup *WindowGroup();
	IMPORT_C CTBaseWin *BaseWin();
	};

class CTEvent : public CTEventBase
	{
public:
	IMPORT_C CTEvent(RWsSession *aWs);
	IMPORT_C ~CTEvent();
	IMPORT_C void Request();
	IMPORT_C virtual void LogEvent(const TWsEvent &aEvent);
protected:
	IMPORT_C void DoCancel();
	IMPORT_C void doRunL();
	};

class CTClient : public CBase
	{
public:
	IMPORT_C CTClient();
	IMPORT_C ~CTClient();
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void ConstructEventHandlerL();
	IMPORT_C void DestroyWindows();
	IMPORT_C TWindowTitle *Title();
	IMPORT_C void ResetFocus();
	IMPORT_C TBool QueueRead();
	IMPORT_C void CancelRead();
	IMPORT_C void CancelRedrawRead();
	IMPORT_C void SetCancelFunction(const TCallBack &aCallBack);
	IMPORT_C void SetRedrawCancelFunction(const TCallBack &aCallBack);
	IMPORT_C void RequestRedraw();
	IMPORT_C void LogMessage(const TLogMessageText &aMessage);
	inline void Flush() {iWs.Flush();}
	inline void SetScreenNumber(TInt aScreenNumber);
	IMPORT_C TBool IsEventWaiting();
	IMPORT_C TBool WaitUntilEventPending();
	IMPORT_C TBool WaitUntilRedrawPending();
	inline CTEvent* EventHandler();
	inline CTRedraw* RedrawHandler();
	IMPORT_C TInt WaitForRedrawsToFinish();
	IMPORT_C TInt WaitForAllEventProcessingToFinish();
private:
	TBool WaitUntilEventPending(const TRequestStatus& aStatus);
	TInt WaitForEventsToFinish(TBool aAll);
protected:
	CTEvent *iEventHandler;
	CTRedraw *iRedrawEventHandler;
public:
	CWindowGc *iGc;
	CTWindowGroup *iGroup;
	RWsSession iWs;
	CWsScreenDevice *iScreen;
protected:
	TWindowTitle iTitle;
	TInt iScreenNumber;
	};

class CTAutoKey;
class CTDialog : public CTTitledWindow
	{
public:
	enum
		{
		EDialogWaitForButtonUp=0x0001,
		EDialogDisplayAtBottom=0x0002,
		EDialogDisplayAtLeft=0x0008,
		};
public:
	IMPORT_C CTDialog();
	IMPORT_C ~CTDialog();
	IMPORT_C void ConstructLD(CTWinBase &aParent, CWindowGc &aGc);
	IMPORT_C void Draw();
	IMPORT_C void WinKeyL(const TKeyEvent &aKey,const TTime &aTime);
	IMPORT_C void PointerL(const TPointerEvent &aPointer,const TTime &aTime);
	IMPORT_C virtual void ButtonPressL(TInt aButton);
	IMPORT_C void SetLine1(const TDesC &aLine1);
	IMPORT_C void SetLine2(const TDesC &aLine2);
	IMPORT_C void SetNumButtons(TInt aNum);
	IMPORT_C void SetButtonText(TInt aNum,const TDesC &aButton);
	IMPORT_C void SetWindowSize();
	IMPORT_C void RelinquishFocus();
	IMPORT_C TInt Display();
	IMPORT_C void SetFlags(TUint aFlags);
protected:
	void SetResult(TInt aButton);
	inline void SetTakeFocus() {iTakeFocus=ETrue;}
private:
	void SetMaxWid(TInt &aMax, TInt aWid);
	TRect ButtonRect(TInt aIndex) const;
private:
	TBool iIsActive;
	TBool iActivated;
	TBool iWinActive;
	TBool iTakeFocus;
	TInt iNumButtons;
	TInt iButWid;
	CTBaseWin *iOldFocus;
	TInt *iResultPtr;
	TBuf<0x40> iLine1;
	TBuf<0x40> iLine2;
	TBuf<0x20> iButton[3];
	TInt iButtonClickOn;
	TUint iFlags;
	CTAutoKey* iAutoKey;
	};

class CInfoDialog : public CTDialog
	{		  
private:
	enum
		{
		eSeconds=1000000,
		eTimes=6,
		};
public:
	IMPORT_C CInfoDialog(CTWindowGroup *aGroupWin,CWindowGc *aGc);
	IMPORT_C void ConstructLD();
	IMPORT_C void TimerResults();
	//Virtual function from CTDialog
	IMPORT_C void ButtonPressL(TInt aButton);
private:
	void AppendProfileTime(TDes &aDes, TInt aNum);
	void AppendProfileCount(TDes &aDes, TInt aNum);
	void AddComma(TDes &aDes);
private:
	CTWindowGroup *iGroupWin;
	CWindowGc *iGc;
	};

class CEventWindow : public CTWin
	{
public:
	IMPORT_C CEventWindow(TInt aLogSize);
	IMPORT_C ~CEventWindow();
	IMPORT_C void ConstructL(CTWinBase &parent);
	IMPORT_C void SetUpL(const TPoint &pos, CTWinBase *parent, CWindowGc &aGc);
	IMPORT_C void Draw();
	IMPORT_C void WinKeyL(const TKeyEvent &,const TTime &aTime);
	IMPORT_C void LogEvent(const TWsEvent &aEvent);
private:
	void DrawLine(TInt aLine, const TDesC &aText);
	void LogEvent(TInt aLogNum, const TWsEvent &aEvent);
private:
	TInt iLineHeight;
	TInt iCount;
	TInt iLogSize;
	TInt iNumLogged;
	TWsEvent *iLoggedEvents;
	};

class CBlankWindow : public CTWin
	{
public:
	IMPORT_C CBlankWindow(TRgb aCol);
	IMPORT_C CBlankWindow();
	IMPORT_C void ConstructL(CTWinBase &parent);
	IMPORT_C void SetColor(TRgb aColor);
	IMPORT_C void RealDraw(TBool aRealDraw);
	IMPORT_C virtual void Draw();
	IMPORT_C void DrawNow();
	IMPORT_C void DrawNow(TRect& aRect);
	IMPORT_C TBool Check(const CTClient& aClient);
protected:
	TRgb iCol;
	TBool iRealDraw;
	};

struct TSpriteCreateParams;

class CTSpriteBase : public CBase
	{
public:
	typedef void (*TSpriteDrawFunction)(CBitmapContext *aGc,TInt aIndex, const TSize &aSize, TBool aDoMask, TAny *aParam);
protected:
	virtual RWsSpriteBase &SpriteBase()=0;
	void CreateBitmapL(CFbsBitmap *&aBitmap, CFbsBitmapDevice *&aBitmapDevice, TSpriteCreateParams *aParams, TBool aDoMask);
public:
	IMPORT_C void UpdateL(TInt aIndex, TSpriteCreateParams *aParams);
	IMPORT_C void RedrawL(CFbsBitmap *&aBitmap, CFbsBitmap *&aMaskBitmap, TSpriteCreateParams *aParams);
	};

class CTSprite : public CTSpriteBase
	{
public:
	IMPORT_C CTSprite(RWsSession &aWs);
	IMPORT_C ~CTSprite();
	IMPORT_C void ConstructL(RWindowTreeNode &aWindow, const TPoint &aPos, TInt aCount, TSpriteCreateParams *aParams, TInt aFlags);
	IMPORT_C RWsSpriteBase &SpriteBase();
	IMPORT_C RWsSprite &Sprite();
private:
	RWsSprite iSprite;
	};

class CTPointerCursor : public CTSpriteBase
	{
public:
	IMPORT_C CTPointerCursor(RWsSession &aWs);
	IMPORT_C ~CTPointerCursor();
	IMPORT_C void ConstructL(TInt aFlags);
	IMPORT_C void ConstructL(TInt aCount, TSpriteCreateParams *aParams, TInt aFlags, CFbsBitmap *&aBitmap, CFbsBitmap *&aMaskBitmap);
	IMPORT_C void ConstructL(TInt aCount, TSpriteCreateParams *aParams, TInt aFlags);
	IMPORT_C RWsSpriteBase &SpriteBase();
	IMPORT_C RWsPointerCursor &PointerCursor();
private:
	RWsPointerCursor iCursor;
	};

struct TSpriteCreateParams
	{
public:
	IMPORT_C TSpriteCreateParams(TSize aSize,TPoint aOffset,CTSpriteBase::TSpriteDrawFunction aDrawFunc,TAny *aDrawFuncParam=NULL, TBool aMask=ETrue,CGraphicsContext::TDrawMode aDrawMode=CGraphicsContext::EDrawModePEN);
	IMPORT_C TSpriteCreateParams();
public:
	TSize iSize;
	TPoint iOffset;
	CTSpriteBase::TSpriteDrawFunction iDrawFunc;
	TAny *iDrawFuncParam;
	TBool iMask;
	CGraphicsContext::TDrawMode iDrawMode;
	TTimeIntervalMicroSeconds32 iInterval;
	};

class CTUser
	{
public:
	IMPORT_C static void Splat(CTClient *aClient, const TRect &aRect, const TRgb &aRgb);
	};

IMPORT_C void DisplayDialog(const TWindowTitle &aTitle, const TDesC &aLine1, const TDesC &aLine2, const RWindowGroup *aGroup=NULL);
IMPORT_C void DisplayDialog(TInt aScreenNumber,const TWindowTitle &aTitle, const TDesC &aLine1, const TDesC &aLine2, const RWindowGroup *aGroup=NULL);
IMPORT_C void DisplayDialog(CTClient *aClient, const TWindowTitle &aTitle, const TDesC &aLine1, const TDesC &aLine2);
IMPORT_C TInt TestLibStartUp(TCreateClientFunc aFunc);
IMPORT_C TInt TestLibStartUp(TCreateClientFunc aFunc,TInt aScreenNumber);


inline TRgb CTTitledWindow::Color()
	{return(iWinColor);}
inline CWindowGc *CTBaseWin::Gc()
	{return(iGc);}
inline RWindow *CTWin::Win() const
	{return((RWindow *)BaseWin());}
inline const RBlankWindow *CTBlankWindow::BlankWin()const
	{return(&iWin);}
inline RBlankWindow *CTBlankWindow::BlankWin()
	{return(&iWin);}
inline void CTClient::SetScreenNumber(TInt aScreenNumber)
	{iScreenNumber=aScreenNumber;}
inline CTEvent* CTClient::EventHandler()
	{return iEventHandler;}
inline CTRedraw* CTClient::RedrawHandler()
	{return iRedrawEventHandler;}
#endif
