// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CWsRedrawMsgWindow and associated classes definitions
// 
//

#ifndef __REDRAWMSGWINDOW_H__
#define __REDRAWMSGWINDOW_H__

#include "wnredraw.h"
#include "redrawmsgwindow.h"

class CWsFbsFont;

class CWsRedrawMsgWindow : public CWsWindowRedraw
	{
	class CRedrawSegment;
	enum TFlags
		{
		EBackgroundClear=0x0001,		//Clear background when starting redraw
		ENoRepeatRedraw=0x0004,			//Redraw is a response to an error, and should not repeat on another error
		EBeginEndRedraw=0x0080,			//This window is currently in the middle of a redraw, that is between client calls to BeginRedraw and EndRedraw.
		EPendingScheduledDraw=0x0200,   //We have scheduled a redraw but it hasn't happened yet
		EStoringEntireWindow=0x0400,    //If store commands for the entire window or only the viewport
		};
	enum TScope
		{
		EStoreEntireWindow,             //We try to obtain draw commands for the entire window
		EStoreViewport                  //We only try to obtain commands for the on screen viewport
		};
	enum TAtomicityType
		{
		ENoAtomicity,
		ESegment,
		EWindow,
		};
public:
	enum TRedrawSegmentType
		{
		ESegmentTypePendingRedraw,      // A redraw segment still being received
		ESegmentTypeRedraw,             // A segment received between a begin and end redraw
		ESegmentTypeNonRedraw,          // A segment storing commands received outside of begin and end redraw
		};
public:
	static void StaticInitL();
	CWsRedrawMsgWindow(CWsWindow *aWin);
	~CWsRedrawMsgWindow();
	void AddFbsBitmapsL(TInt aHandle, TInt aMaskHandle);
	void AddWsBitmapsL(TInt aHandle, TInt aMaskHandle);
	void AddWsFontL(TInt aHandle);
	void SetScope(TScope aStore);
	
public:	//from CWsWindowRedraw	
	void ConstructL();
	TBool CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd);
	void ClipInvalidRegion(const TRect &aRect);
	TBool NeedsRedraw() const;
	TBool GetRedrawRect(TRect &aRect) const;
	void ClearRedrawStore(TBool aClearPendingRedraw=EFalse);
	TBool DrawCommand(CWsGc* aGc,const TAny *aCmdData);
	void GcAttributeChange(CWsGc* aGc,const TAny *aCmdData);
	void GcDeactivate(CWsGc* aGc);
	void ClientExposing();
	void DrawWindow();
	void PrepareForResizeL(const TSize &aSize, TSize &aOldSize);
	void Moved();
	TBool Contains(const TArray<TGraphicDrawerId>& aDrawers,const TRegion& aRegion) const;
	TBool ReleaseMemory(MWsMemoryRelease::TMemoryReleaseLevel aLevel);
	void VisibleRegionChange();
	TBool ReadyToDraw() const;
	TInt SizeInBytes() const;
	void WindowClosing();

private: //from CWsWindowRedraw	
	void Invalidate(const TRect * aRect = 0);
	TRgb BackColor() const;
	void Scroll(const TRect &aClipRect, const TPoint &aOffset,const TRect &aRect);
	
private:
	void RemoveFromRedrawQueueIfEmpty();
	void HandleNonRedrawCommand(TWsGcOpcodes aOpcode);
	const TRegion * ReadRegion(const TInt aRegionNum);
	void BeginRedraw(const TRect* aRect);
	void DoBeginRedrawL(const TRect* aRect);
	TInt SubtractRectFromSegmentArray(const TRect& aRect);
	void EndRedraw();
	void ValidateRect(const TRect *aRect);
	void StoreDrawCommandL(CWsGc* aGc,const TAny *aCmdData);
	void AppendCommandL(const TAny* aCmdData, const TUint16 aOpcodeFlags = 0);
	void StoreAllGcAttributesL(CWsGc* aGc);
	void DrawCommandsL();
	void DiscardStoredCommands();
	TBool DiscardSegmentsOutsideViewport();
	inline TBool NoBuffer() const;
	void DiscardStoredCommandsIfError(TInt aError);
	inline TBool IsFbsBitmapOperation(TInt aOpCode) const;
	inline TBool IsWsBitmapOperation(TInt aOpCode) const;
	inline TBool IsRemoteReadRequired(TInt aOpCode) const;
	inline TBool IsWsFontOperation(TInt aOpCode) const;
	inline TBool IsDrawWsGraphicOperation(TInt aOpCode) const;
	void ReplaceAndAppendCommandL(TInt aOpcode,const TAny* aCmdData);
	// CRedrawSegment related methods
	inline TInt CurrentCommandBufferWritePos() const;
	inline CBufSeg* CurrentDrawCommandBuffer() const;
	void ExpandCommandBufferL(TInt aLength);
	void CommandBufferWrite(const TDesC8& aDes, TInt aLength);
	void CommandBufferWrite(const TAny* aPtr,TInt aLength);
	void CreateNewSegmentL(const TRect& aRect, TRedrawSegmentType aRegionType);
	inline TBool InRedraw() const;
	void AgeNonRedrawSegments();
	void PromoteAndUpdateAllPendingSegments();
	void PromoteLastPendingSegment();
	void Lock();
	void Unlock();
	void ScheduleUpdateOfSegment(CRedrawSegment* aSegment);
	void ReleaseRedrawSegments();

private:
	static TInt iNonRedrawAgeLimit;
	static TAtomicityType iAtomicity;
private:
	class CRedrawSegment : public CBase
		{
	public:
		static CRedrawSegment* NewLC(const TRect& aRect, TRedrawSegmentType aNewRegionType);
		~CRedrawSegment();
		void AddFbsBitmapL(TInt aHandle, CWsRedrawMsgWindow* aWindow);
		void AddWsBitmapL(DWsBitmap* bitmap);
		void ReleaseFontsAndBitmaps();
		void AddDrawerL(TGraphicDrawerId aDrawerId);
		TBool ContainsDrawers(const TArray<TGraphicDrawerId>& aDrawers,const TRegion& aRegion) const;
		TInt SizeInBytes() const;
	private:
		CRedrawSegment();
		void ConstructL(const TRect& aRect, TRedrawSegmentType aNewRegionType);
	public:
		TRedrawSegmentType	iRedrawSegmentType;
		CBufSeg* iDrawCommands;
		TInt iCurrentCommandBufferWritePos;
		RWsRegion iRegion;
		RPointerArray<CFbsBitmap> iFbsBitmapArray;
		RPointerArray<DWsBitmap> iWsBitmapArray;
		RPointerArray<CWsFbsFont> iWsFontArray;
		RArray<TGraphicDrawerId> iDrawerArray;
		TTime iCreationTime;
		};
	RWsRegion iInvalid; // Region we haven't been sent a begin redraw for.
	TRect iRedrawRect;
	TRgb iBackColor;
	TInt iFlags;
	// Regions currently in use for drawing this window:
	RPointerArray<CRedrawSegment> iRedrawSegments;
	// The segment we are currently storing to.  This is allowed to be null!
	CRedrawSegment * iCurrentSegment;
	// The last GC we drew with so we can detect changes
	CWsGc* iLastDrawGc;
	// During a scheduled draw, this is the area being drawn to (see comments on ReadRegion)
	RWsRegion iLocalRedrawRegion;
	TInt iMemoryLock;
	TBool iOSBStatus;
	TInt iCount;
	};

inline TBool CWsRedrawMsgWindow::IsRemoteReadRequired(TInt aOpCode) const
	{
	return (aOpCode==EWsGcOpDrawTextPtr || aOpCode==EWsGcOpDrawTextVerticalPtr || 
		aOpCode==EWsGcOpDrawBoxTextPtr || aOpCode==EWsGcOpDrawBoxTextVerticalPtr);
	}

inline TInt CWsRedrawMsgWindow::CurrentCommandBufferWritePos() const
	{
	WS_ASSERT_DEBUG(iCurrentSegment, EWsPanicDrawCommandsInvalidState);
	return iCurrentSegment->iCurrentCommandBufferWritePos;
	}

inline CBufSeg* CWsRedrawMsgWindow::CurrentDrawCommandBuffer() const
	{
	WS_ASSERT_DEBUG(iCurrentSegment, EWsPanicDrawCommandsInvalidState);
	return iCurrentSegment->iDrawCommands;
	}

inline TBool CWsRedrawMsgWindow::InRedraw() const
	{
	return iFlags&EBeginEndRedraw;
	}

#endif
