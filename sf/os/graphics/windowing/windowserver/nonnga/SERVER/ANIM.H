// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of classes related to Anim DLL's
// 
//

#ifndef __ANIM_H__
#define __ANIM_H__

#include <e32std.h>
#include <w32adll.h>
#include "server.h"
#include "sprite.h"

class CWsAnimDll : public CWsObject
	{
public:
	static void InitStaticsL();
	static void DeleteStatics();
public:
	CWsAnimDll(CWsClient *owner);
	~CWsAnimDll();
	TInt CreateInstanceL(TUint32 aHandle, TInt aType, TAny *aArgs, TBool aIsWindow);
	void LoadL(const TDesC &aDllName);
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	void Remove(TInt aHandle);
	inline TInt AnimObjectHandle(CWsAnim *aAnimObject) const;
	inline CWsAnim* AnimObjectL(TInt aAnimHandle) const;
	
private:
	TInt doCreateInstanceL(CWsAnim *aInstance, TInt aType, TAny *aArgs, TBool aIsWindow);
private:
	static CObjectConIx *AnimObjectConIx;
private:
	CObjectCon *iInstanceCon;
	CObjectIx *iInstanceIndex;
	CAnimDll *iAnimDll;
	RLibrary iAnimLib;
	
public:
	TSglQueLink iQue;
	};
	
class CWsAnimGc;

class CWsAnim : public CObject, public MAnimGeneralFunctions, public MAnimGeneralFunctionsWindowExtension,
													public MAnimFreeTimerWindowFunctions, public MAnimSpriteFunctions, public MAnimGeneralFunctionsEventExtension
	{
public:
	static void InitStaticsL();
	static void DeleteStatics();
public:
	CWsAnim(CWsAnimDll *aDll);
	~CWsAnim();
	void Connect(CWsClientWindow *aWindow);
	void Connect(CWsSprite *aSprite);
	void ConstructL(CAnim *aAnim, TAny *aArgs, CWsAnimDll *aAnimDll, TBool aIsWindow);
	void Redraw(CFbsBitGc * aGc, const TRegion *aRegion);
	static void UserDeactivateAnimGc();
	inline CAnim *Anim() const;
	static void WindowClosing(CWsAnim *aWsAnim);
	static void CloseAnim(CWsAnim *aWsAnim);
	const TRect& Rect() const;
	TRect BestRect() const;
	void FocusChanged(TBool aNewFocusState);
	void SetMessage(const RMessagePtr2* aMessage);
	TInt CommandReply(TInt aOpcode, TAny* aArgs);
// Functions from MAnimGeneralFunctions
	TDateTime SystemTime() const;
	TBool FlashStateOn() const;				// Return ETrue if in the on part of the flash cycle
	void Panic() const;
	void Panic(TClientPanic aPanic) const;
	const CFbsScreenDevice *ScreenDevice();						// Pointer to screen device
	CFbsBitmap *DuplicateBitmapL(TInt aHandle);
	CFbsFont *DuplicateFontL(TInt aHandle);
	void CloseFont(CFbsFont *);
	TAnimSync Sync() const;					// Return the current sync mode
	void Animate(TDateTime *aDateTime);		// Trigger a call to the DLL's Animate function
	void SetSync(TAnimSync aSyncMode);
	void SetInterval(TInt aInterval);		// Set the regular repeat interval, does not affect the current countdown
	void SetNextInterval(TInt aInterval);	// Sets the interval to the next Animate, after that it will fall back to the usual rate
	const RThread &Client();				// Return a reference to the calling clients thread
	void ReplyBuf(const TDesC8 &aDes);
	void ReplyBuf(const TDesC16 &aDes);
	void GetRawEvents(TBool aGetEvents) const;	// Raw events are passed to CAnim::OfferRawEventL
	void PostRawEvent(const TRawEvent &aRawEvent) const;
	void PostKeyEvent(const TKeyEvent &aRawEvent) const;
	const RMessagePtr2* Message();
	TAny* ExtendedInterface(TInt aInterface);
	TInt RegisterForNotifications(TUint32 aNotifications);

// Functions from MAnimGeneralFunctionsWindowExtension
	TInt Screens() const;
	TInt FocusScreens() const;
	void SetFocusScreen(TInt aScreenNo);
	TInt WindowGroups(TInt aScreen) const;
	TBool WindowGroupInfo(TWindowGroupInfo& aInfo,TInt aScreen,TInt aWindowGroup) const;
	TBool WindowGroupName(TPtrC& aWindowName,TInt aScreen,TInt aWindowGroup) const;
	TInt SetOrdinalPosition(TInt aWindowGroupId,TInt aPos,TInt aOrdinalPriority);
	void WindowConfig(TWindowConfig& aWindowConfig) const;	
// Functions from MAnimWindowFunctions
	void ActivateGc();	// Activate the GC to enable drawing in a command or animate function
	void Invalidate(const TRect &aRect);
	void Parameters(TWindowInfo &aData);
	void VisibleRegion(TRegion& aRegion);
 	TSize WindowSize() const;				// Return the window size
	TBool IsHidden();
	void SetVisible(TBool aState);
	void SetRect(const TRect &aRect);		// Sets the rect that this instance wants to draw to.
// Functions from MAnimFreeTimerWindowFunctions
	void DeactivateGc();					// Deactivate the GC after drawing from our own RunL
	void Update();
// Functions from MAnimSpriteFunctions
	TSpriteMember *GetSpriteMember(TInt aMember) const;	//Returns sprite member data
	void UpdateMember(TInt aMember,const TRect& aRect,TBool aFullUpdate);		//redraw of part of a sprite
	void Activate(TBool aActive);		// Turn sprite on or off
	void SizeChangedL();
	void SetPosition(const TPoint &aPos);			//Change the positon of the sprite
	TBool SpriteCanBeSeen() const;
// Functions from MAnimGeneralFunctionsEventExtension
	void PostKeyEvent(const TKeyEvent& aRawEvent,TInt aRepeats) const; //Repeated key events 
public:
	inline CWsAnim * Next();
private:
	inline CWindowAnim* WindowAnim() {return STATIC_CAST(CWindowAnim*,iAnim);}
private:
	static CWsAnimGc *WsAnimGc;
private:
	CWsClient *iClient;
	TTimeIntervalMicroSeconds iInterval;
	TAnimSync iAnimSync;
	CWsAnim *iNextWin;	// Linked list of animated objects attached to a window
	CAnim *iAnim;
	CWsAnimDll *iAnimDll;
	CWsClientWindow *iWindow;
	TRect iRect;
	CWsSprite *iSprite;
	const RMessagePtr2* iMessage;
	TWindowInfo::TRegionPair* iRedrawRegionPair;
	friend class CWsAnimDll;
	friend class DllAnimator;
	mutable TBool iAnimAddedInHandler;
	TTime iStartTime;
	TInt iLastFrame;
	TBool iFlashOn;
	};

class CAnimFbsFont;

class CWsAnimGc : public CAnimGc
	{
	friend class CWsAnim;
public:
	CWsAnimGc();
//
	void Activate(CWsClientWindow *aWindow, CWsAnim *aWsAnim, const TRegion *aRegion, CFbsBitGc * aGc);
	void Deactivate();
	TBool IsActive() const;
	void UserActivate(CWsClientWindow *aWindow, CWsAnim *aWsAnim);
	void UserDeactivate();
	void AnimDeleted(CWsAnim *aWsAnim);
// From CGraphicsContext
	virtual CGraphicsDevice* Device() const;
	virtual void SetDrawMode(TDrawMode aDrawingMode);
	virtual void SetClippingRect(const TRect& aRect);
	virtual void CancelClippingRect();
	virtual void CancelClippingRegion();
	virtual void UseFont(const CFont *aFont);
	virtual void DiscardFont();
	virtual void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
	virtual void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
	virtual void SetWordJustification(TInt aExcessWidth,TInt aNumGaps);
	virtual void SetCharJustification(TInt aExcessWidth,TInt aNumChars);
	virtual void SetPenColor(const TRgb &aColor);
	virtual void SetPenStyle(TPenStyle aPenStyle);
	virtual void SetPenSize(const TSize& aSize);
	virtual void SetBrushColor(const TRgb &aColor);
	virtual void SetBrushStyle(TBrushStyle aBrushStyle);
	virtual void SetBrushOrigin(const TPoint &aOrigin);
	virtual void UseBrushPattern(const CFbsBitmap *aBitmap);
	virtual void DiscardBrushPattern();
	virtual void DrawArc(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd);
	virtual void DrawLine(const TPoint &aPoint1,const TPoint &aPoint2);
	virtual void DrawLineTo(const TPoint &aPoint);
	virtual void DrawLineBy(const TPoint &aPoint);
	virtual void DrawPolyLine(const CArrayFix<TPoint> *aPointList);
	virtual void DrawPolyLine(const TPoint* aPointList,TInt aNumPoints);
	virtual void DrawPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd);
	virtual void DrawEllipse(const TRect &aRect);
	virtual void DrawRect(const TRect &aRect);
	virtual void DrawRoundRect(const TRect &aRect,const TSize &aCornerSize);
	virtual void Clear(const TRect &aRect);
	virtual void Clear();
	virtual TInt DrawPolygon(const CArrayFix<TPoint> *aPointList,TFillRule aFillRule);
	virtual TInt DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule=EAlternate);
	virtual void DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap *aSource);
	virtual void DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aSource);
	virtual void DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aSource, const TRect &aSourceRect);
	virtual void DrawBitmapMasked(const TRect &aDestRect, const CFbsBitmap *aBitmap, const TRect &aSrcRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask);
	virtual void DrawText(const TDesC &aString,const TPoint &aPosition);
	virtual void DrawText(const TDesC &aString,const TRect &aBox,TInt aBaselineOffset,TTextAlign aHoriz=ELeft,TInt aLeftMrg=0);
	virtual void MoveTo(const TPoint &aPoint);
	virtual void MoveBy(const TPoint &aPoint);
	virtual void Plot(const TPoint &aPoint);
	virtual void SetOrigin(const TPoint &aPoint);
	virtual void BitBlt(const TPoint &,const CFbsBitmap *aBitmap);
	virtual void BitBlt(const TPoint &,const CFbsBitmap *aBitmap,const TRect &);
	virtual void BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	virtual void CopyRect(const TPoint& aOffset,const TRect& aRect);
	virtual void Reset();
// Extra command defined in CBitmapContext
	virtual void SetFaded(TBool aFaded);
	virtual void SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap);

	virtual TInt SetClippingRegion(const TRegion &aRegion);	
	virtual TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp, const TRect& aSrcRect, const CFbsBitmap* aAlphaBmp,	const TPoint& aAlphaPt);	
	virtual void MapColors(const TRect &aRect,const TRgb *aColors,TInt aNumPairs,TBool aMapForwards);
	virtual void DrawTextVertical(const TDesC& aText,const TPoint& aPos,TBool aUp);
	virtual void DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
	
private:
	void SetDrawingRegions(const TRegion *aRegion);
	void CloseCurrentFont();
	virtual void DrawBitmapMasked(const TRect& /*aDestRect*/,const CWsBitmap* /*aBitmap*/,const TRect& /*aSrcRect*/,const CWsBitmap* /*aMaskBitmap*/, TBool /*aInvertMask*/);
	TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CWsBitmap* aSrcBmp,	const TRect& aSrcRect, const CWsBitmap* aAlphaBmp,	const TPoint& aAlphaPt);
private:
	CFbsBitGc* iGc;
	TBool iUserActive;
	const TRegion *iBaseRegion;	// Pointer to base clipping region
	TBool iUseDrawingRegion; // If ETrue use iDrawingRegion instead of iBaseRegion
	RWsRegion iDrawingRegion;	// Intersection of iBaseRegion and extra clipping
	CWsClientWindow *iWin;
	RWsRegion iAnimateRegion;
	CWsAnim *iOwningWsAnim;
	TRect iClippingRect;
	CAnimFbsFont *iFont;
	};

//
// inlines			//
//

inline CAnim *CWsAnim::Anim() const
	{return(iAnim);}
inline TInt CWsAnimDll::AnimObjectHandle(CWsAnim *aAnimObject) const
	{return(iInstanceIndex->At(aAnimObject));}
inline CWsAnim* CWsAnimDll::AnimObjectL(TInt aAnimHandle) const
	{return STATIC_CAST(CWsAnim*,iInstanceIndex->AtL(aAnimHandle));}
inline CWsAnim* CWsAnim::Next()
	{return iNextWin;}
#endif
