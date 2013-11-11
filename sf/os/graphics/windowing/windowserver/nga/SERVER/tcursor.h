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
// RWsTextCursor and associated classes definitions
// 
//

#ifndef __TCURSOR_H__
#define __TCURSOR_H__

#include "server.h"
#include "cliwin.h"
#include "sprite.h"

class RWsTextCursor;
class CEventQueue;
class CWsRenderStage;
class MWsTextCursor;

class TCursorSprite
	{
public:
	static void SetFocus(RWsTextCursor* aFocus,CWsClientWindow* aWin=NULL);
	static void Hide();
	static void Reveal();
	static void SetCurrentCursor(RWsTextCursor* aFocus, CWsClientWindow* aWin);
	static TBool IsStandardCursorActive();
private:
	static RWsTextCursor *iCurrentCursor;
	static TBool iHidden;
	};

class RWsTextCursor : public MWsWindowTreeNode, public MWsStandardTextCursor
	{
public:
	friend class TCursorSprite;
public:
	void ConstructL(CWsWindowGroup *aOwner);
	void Close();
	void SetL(const TWsWinCmdSetTextCursor &aSet, TBool aClipped);
	void Cancel();
	void WindowDisconnected(CWsWindow *aWin);
	void LostFocus();
	void ReceivedFocus();
	inline CWsWindow* Win() const;
	TRect RectRelativeToScreen() const;
	TRect RectRelativeToWindow() const;
	void Enable();
	void Disable();
	TBool IsStandardCursorActive();
	TBool IsFlashing() const;
	void Draw(const TRegion& aRegion);
	TFlashState CurrentCursorFlashState() const;
	void CreateNode();
	void ReleaseNode();
	void SendState(MWsWindowTreeObserver& aWindowTreeObserver) const;
public:
	enum TInternalFlags
		{
		EHasFocus	= 0x00000001,
		EActiveNode	= 0x00000002
		};
public: // from MWsWindowTreeNode
	MWsWindowTreeNode::TType NodeType() const;
	const MWsWindow* Window() const;
	const MWsSprite* Sprite() const;
	const MWsStandardTextCursor* StandardTextCursor() const;
	const MWsWindowGroup* WindowGroup() const;
	const MWsWindowTreeNode* ParentNode() const;
public: // from MWsStandardTextCursor
	TInt Type() const;
	TRect Rect() const;
	TRect ClipRect() const;
    TUint Flags() const;
	TRgb Color() const;
	TTimeIntervalMicroSeconds32 FlashInterval() const;
private:
	void Clear();
	void doDraw(const TRegion& aRegion);
	void ScheduleReDrawNow();
	void UpdateAttributes(TPoint aPos, TPoint aAbsPos, TSize aSize, TInt aType, TUint aFlags, TRect aClipRect, TRgb aColor, CWsCustomTextCursor* aCustomTextCursor, CWsClientWindow* aWin);
	void NotifyObserver(MWsWindowTreeObserver::TAttributes aAttribute) const;
private:
	CWsWindowGroup *iGroupWin;
	CWsClientWindow *iWin;
	TUint iInternalFlags;
	TInt iType;
	TPoint iPos;
	TPoint iAbsPos;
	TSize iSize;
    TUint iFlags;
	TRgb iColor;
	TRect iClipRect;
	TTime iNextCursorUpdate;
	RWsRegion iDrawRegion;
	CWsCustomTextCursor* iCustomTextCursor;
	};

class CWsPointerCursor : public CWsSpriteBase
	{
public:
	CWsPointerCursor(CWsClient *aOwner);
	~CWsPointerCursor();
	void ConstructL(const TWsClCmdCreatePointerCursor &aParams);
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	void CloseObject();
	void Close();
	void Open();
private:
	TInt iAccessCount;
	};

class CWsCustomTextCursor : public CWsSpriteBase
	{
public:
	CWsCustomTextCursor(CWsClient* aOwner, RWsSession::TCustomTextCursorAlignment aAlignment);
	~CWsCustomTextCursor();
	void ConstructL(TInt aFlags);
	void CommandL(TInt aOpcode, const TAny* aCmdData);
	void CompleteL(CWsWindow* aWin, TBool aFlash, TBool aClipSprite, const TPoint& aClipOffset, const TSize& aClipSize);
	void SetPositionNoRedraw(const TPoint& aPos);
	inline RWsSession::TCustomTextCursorAlignment Alignment() const;
	inline void SetWindow(CWsClientWindow* aWin);
private:
	RWsSession::TCustomTextCursorAlignment iAlignment;
	};

inline CWsWindow* RWsTextCursor::Win() const
	{
	return iWin;
	}

inline RWsSession::TCustomTextCursorAlignment CWsCustomTextCursor::Alignment() const
	{
	return iAlignment;
	}

inline void CWsCustomTextCursor::SetWindow(CWsClientWindow* aWin)
	{
	iWin=aWin;
	}

#endif
