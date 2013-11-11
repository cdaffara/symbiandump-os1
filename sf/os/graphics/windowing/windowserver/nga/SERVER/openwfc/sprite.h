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
// CWsSprite and associated classes definitions
// 
//

#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "server.h"
#include "window.h"
#include <graphics/wsgraphicscontext.h>
#include <babitflags.h>

//
// CWsSpriteMember
//
class CWsSpriteMember : public CBase
	{
public:
	CWsSpriteMember();
	~CWsSpriteMember();
	TBool SetL(const TCmdSpriteMember &aCmdSpriteMember);
public:
	CFbsBitmap *iBitmap;
	CFbsBitmap *iMaskBitmap;
	TBool iInvertMask;
	CGraphicsContext::TDrawMode iDrawMode;
	TPoint iOffset;
	TTimeIntervalMicroSeconds32 iInterval;
	};

//
// CWsSpriteBase
//
class CWsSpriteBase : public CWsScreenObject, public MWsWindowTreeNode, public MWsSprite
	{
protected:
	enum {ENoChildPriorityBoost=1,EPointerPriorityBoost=2};
public:
	friend class CWsSpriteTimer;
	friend class CWsAnim;
	friend class TCursorSprite;
public:
	static void InitStaticsL();
	static void DeleteStatics();

	CWsSpriteBase(CWsClient *aOwner, WH_HANDLES aType);
	~CWsSpriteBase();

	//from MWsSprite
	TRect Rect() const;
	TSpriteType SpriteType() const;
	
	TPoint Pos() const;
	void SetPos(const TPoint &Pos);
	void Deactivate();
	void DisconnectGroupWin();
	void TimerExpired();
	void Activate();
	inline TBool IsActive() const;
	inline TBool IsFlashingEnabled() const;
	inline TBool AdvancedPointersEnabled() const;
	TBool CanBeSeen() const;
	CWsSpriteBase * Next() { return iNext; }
	void SetNext(CWsSpriteBase * aNext) { iNext = aNext; }
	void CalcRedrawRegion(const TRegion& aSourceRegion, TRegion& aTarget) const;
	void Redraw(MWsGraphicsContext * aGc, const TRegion& aRegion);
	void ForceRedraw();
	inline const CWsWindow* Win() const;
	inline CWsWindow* Win();
	TBool IsActivated() const;
	inline TBool HasSpriteMember() const { return  iMembers->Count(); }
	void SendState(MWsWindowTreeObserver& aWindowTreeObserver) const;
	
public: // from MWsWindowTreeNode
	MWsWindowTreeNode::TType NodeType() const;
	const MWsWindow* Window() const;
	const MWsSprite* Sprite() const;
	const MWsStandardTextCursor* StandardTextCursor() const;
	const MWsWindowGroup* WindowGroup() const;
	const MWsWindowTreeNode* ParentNode() const;

protected:
	void ConstructL(TUint aFlags, CWsWindow *aWindow);
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	void AppendMemberL(const TCmdSpriteMember &aCmdSpriteMember);
	virtual void CompleteL();
	void SetMember(TInt aIndex);
	void CheckSizesL();
	TBool UpdateMemberL(CWsSpriteMember *aMember, const TCmdSpriteMember &aCmdSpriteMember);
	void QueueDeltaTimer();
	void NotifyExtentChanged() const;
	enum TInternalFlags 
		{
		EFloating, 
		EClipSprite,
		EHasAnimation,
		EDirty
		};
	inline void SetIsFloating(TBool aIsFloating);
	inline TBool IsFloating() const;
	inline void SetClipSprite(TBool aClipSprite);
	inline TBool ClipSprite() const;
public:	
	inline void SetHasAnimation(TBool aValue);
	inline TBool HasAnimation() const;
	inline void SetDirty(TBool aValue);
	inline TBool IsDirty() const;

protected:
	CWsWindow *iWin;
	CWsWindowGroup *iGroupWin;
	TPoint iBasePos;
	TPoint iPos;		// Combined position and offset
	TSize iMaxSize;
	TSize iSize;
    TUint iFlags;
	TRect iClipRect;
	CArrayPtrFlat<CWsSpriteMember> *iMembers;
	TInt iCurIndex;
	TWsDeltaTimerEntry iDeltaTimerEntry;
	TDisplayMode iDisplayMode;
	TPoint iClipOffset;
	TSize iClipSize;
	CWsSpriteBase * iNext; // linked list per window
	TBitFlags iInternalFlags;
//
// Static data
//
	static CWsDeltaTimer *iDeltaTimer;
	};

/** Sprite flags.

These can be combined with each other and TSpriteFlags using a bit-wise OR operation.

@internalComponent
@released
@see RWsPointerCursor::Construct()
@see RWsSprite::Construct() */
enum TSystemSpriteFlags
	{
	ESpriteNonSystemFlags=0x0000FFFF,
	ESpriteSystemFlags=	  0xFFFF0000,
	ESpritePointer=0x10000,
	ESpriteOOM=0x20000,
	ESpriteDisabled=0x40000,
	ESpriteActive=0x80000,
	ESpriteDirty=0x100000
	};

//
// CWsSpriteBase inlines
//
inline TBool CWsSpriteBase::IsActive() const
	{
	return (iFlags&ESpriteActive);
	}
inline TBool CWsSpriteBase::IsFlashingEnabled() const
	{
	return (iFlags&ESpriteFlash);
	}
inline const CWsWindow* CWsSpriteBase::Win() const
	{
	return iWin;
	}
inline CWsWindow* CWsSpriteBase::Win()
	{
	return iWin;
	}
/*
@return ETrue if this sprite belongs to window which has Advanced Pointer Support enabled.
*/
inline TBool CWsSpriteBase::AdvancedPointersEnabled() const
	{
	return iWin ? iWin->AdvancedPointersEnabled() : EFalse;
	}

inline void CWsSpriteBase::SetIsFloating(TBool aIsFloating) 
	{ 
	iInternalFlags.Assign(EFloating, aIsFloating); }	

inline TBool CWsSpriteBase::IsFloating() const 
	{
	return iInternalFlags.IsSet(EFloating);
	}

inline void CWsSpriteBase::SetClipSprite(TBool aClipSprite) 
	{ 
	iInternalFlags.Assign(EClipSprite, aClipSprite);
	}

inline TBool CWsSpriteBase::ClipSprite() const 
	{ 
	return iInternalFlags.IsSet(EClipSprite);
	}

inline void CWsSpriteBase::SetHasAnimation(TBool aValue) 
	{
	iInternalFlags.Assign(EHasAnimation, aValue);
	}

inline TBool CWsSpriteBase::HasAnimation() const
	{
	return iInternalFlags.IsSet(EHasAnimation);
	}

inline void CWsSpriteBase::SetDirty(TBool aValue)
	{
	iInternalFlags.Assign(EDirty, aValue);
	}

inline TBool CWsSpriteBase::IsDirty() const	
	{ 
	return iInternalFlags.IsSet(EDirty);
	}	
	

//
// CWsSprite
//
class CWsSprite : public CWsSpriteBase
	{
public:
	CWsSprite(CWsClient *aOwner);
	~CWsSprite();
	void ConstructL(const TWsClCmdCreateSprite &aParams);
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	void CompleteL();
	void Update(TInt aMember,TRect aRect,TBool aFullUpdate);
public:
	CWsAnim *iAnim;	// The AnimDLL on this sprite if there is one.
	};

//
// CWsSpriteManager
//
class CWsSpriteManager : public CBase
	{
public:
	~CWsSpriteManager();
	static CWsSpriteManager* NewL();
	void DrawFloatingSprites(MWsGraphicsContext* aGc,const TRegion& aRegion);
	void AddFloatingSprite(CWsSpriteBase* aSprite);
	void RemoveFloatingSprite(CWsSpriteBase* aSprite);
	void Schedule(CWsSpriteBase* aSprite, TRect* aRect = 0);
	TFlashState CurrentSpriteFlashState(const CWsSpriteBase* aSprite) const;
	TInt SpriteCount() const { return iFloatingSprites.Count(); }
	TTimeIntervalMicroSeconds NextCursorFlashStateChange() const;
	void CalcFloatingSpriteRgn( TRegion& aResultRgn, const TRect& aDefaultRect );
	void SendState(MWsWindowTreeObserver& aWindowTreeObserver) const;
private:
	CWsSpriteManager();
	void ConstructL();
	TTimeIntervalMicroSeconds NextSpriteFlashStateChange(const CWsSpriteBase* aSprite) const;
	TTimeIntervalMicroSeconds CalculateTimeToNextFlash(TTimeIntervalMicroSeconds aTime) const;
private:
	RPointerArray<CWsSpriteBase> iFloatingSprites;
	};
#endif
