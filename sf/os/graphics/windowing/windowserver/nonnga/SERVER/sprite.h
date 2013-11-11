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
class CWsSpriteBase : public CWsScreenObject
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

	TRect Rect() const;
	TPoint Pos() const;

	void SetPos(const TPoint &Pos);
	void Deactivate();
	void TimerExpired();
	void Activate();
	inline TBool IsActive();
	inline TBool IsFlashingEnabled();
	TBool CanBeSeen() const;
	
	CWsSpriteBase * Next() { return iNext; }
	void SetNext(CWsSpriteBase * aNext) { iNext = aNext; }
	void Redraw(CFbsBitGc * aGc, const TRegion& aRegion);
	void ForceRedraw();
	inline const CWsWindow* Win();
	TBool IsActivated() const;
	TBool HasSpriteMember() const { return  iMembers->Count(); }
protected:
	void ConstructL(TUint aFlags, CWsWindow *aWindow);
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	void AppendMemberL(const TCmdSpriteMember &aCmdSpriteMember);
	virtual void CompleteL();
	void SetMember(TInt aIndex);
	void CheckSizesL();
	TBool UpdateMemberL(CWsSpriteMember *aMember, const TCmdSpriteMember &aCmdSpriteMember);
	void QueueDeltaTimer();
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
	TBool iClipSprite;
	TPoint iClipOffset;
	TSize iClipSize;
	CWsSpriteBase * iNext; // linked list per window
	TBool iFloating;
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
inline TBool CWsSpriteBase::IsActive()
	{
	return iNext != NULL;
	}
inline TBool CWsSpriteBase::IsFlashingEnabled()
	{
	return (iFlags&ESpriteFlash);
	}
inline const CWsWindow* CWsSpriteBase::Win()
	{
	return iWin;
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
	void DrawFloatingSprites(CFbsBitGc* aGc,const TRegion& aRegion);
	void AddFloatingSprite(CWsSpriteBase* aSprite);
	void RemoveFloatingSprite(CWsSpriteBase* aSprite);
	void Schedule(CWsSpriteBase* aSprite, TRect* aRect = 0);
	TFlashState CurrentSpriteFlashState(const CWsSpriteBase* aSprite) const;
	TFlashState CurrentCursorFlashState() const;
	TInt SpriteCount() const { return iFloatingSprites.Count(); }
	TTimeIntervalMicroSeconds NextCursorFlashStateChange() const;
	void CalcFloatingSpriteRgn( TRegion& aResultRgn, const TRect& aDefaultRect );
private:
	CWsSpriteManager();
	void ConstructL();
	TTimeIntervalMicroSeconds NextSpriteFlashStateChange(const CWsSpriteBase* aSprite) const;
	TTimeIntervalMicroSeconds CalculateTimeToNextFlash(TTimeIntervalMicroSeconds aTime) const;
private:
	RPointerArray<CWsSpriteBase> iFloatingSprites;
	};
#endif
