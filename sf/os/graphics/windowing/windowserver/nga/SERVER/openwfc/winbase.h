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
// CWsWindowBase and associated classes definitions
// 
//

#ifndef __WINBASE_H__
#define __WINBASE_H__

#include "server.h"

class TWalkWindowTreeBase;
class TResumableWalkWindowTreeBase;
class CWsAnim;
class CWsSprite;
class CWsBlankWindow;

/** The base class that represents a node in the window tree.

Every window and window group is part of a window tree. The root of the tree is an instance of the
CWsRootWindow class.

@see RWindowTreeNode
@internalComponent
@released
*/
class CWsWindowBase : public CWsScreenObject, public MWsWindowTreeNode
	{
public:
	CWsWindowBase(CWsClient *aOwner,WH_HANDLES aType, CScreen* aScreen);
	void ConstructL(CWsWindowBase *aParent);
	virtual void AreaCovered(TRegion &aRegion)=0;
	virtual TPoint Origin() const = 0;
	virtual TRect AbsRect() const = 0;
	virtual TSize Size() const = 0;
	CWsWindowGroup *WinGroup() const;
	inline TUint32 ClientHandle() const;
	inline CWsWindowBase *BaseParent() const;
	inline CWsWindowBase *BaseChild() const;
	inline CWsWindowBase *NextSibling() const;
	CWsWindowBase *GetPrevSibling() const;
	CWsWindowBase *LastSibling() const;
	CWsWindowBase *PrevSiblingMultiParent() const;
	CWsWindowBase *NextSiblingMultiParent() const;
	inline TWinType WinType() const;
	virtual void StatusDump(TDes &aBuf)=0;
	TBool QueueEvent(TInt aEvent, TInt aIntVal = 0) const;
	inline TBool AdvancedPointersEnabled() const;
	inline CWsPointerCursor *PointerCursor() const;
	TEventQueueWalkRet EventPurgeCheck(TWsEvent *aEvent);
	virtual void SetOrdinalPosition(TInt aPos);
	TBool TreeIsObscured() const;
	CEventQueue *EventQueue() const;
	TInt Depth() const;
	void WalkWindowTree(TWalkWindowTreeBase &doIt,TWalkMode aMode);
	void WalkWindowTree(TResumableWalkWindowTreeBase& aWalkClass, TWalkMode aMode, TBool aResume);
	void WalkWindowTreeBackToFront(TWalkWindowTreeBase &doIt, TWalkModeBackToFront aMode);
	void PurgeEvents();
	inline TInt FadeCount() const;
  void AddSprite(CWsSpriteBase * aSprite);
  void RemoveSprite(CWsSpriteBase * aSprite);
	inline TBool HasSprite() const;
	virtual TBool IsDSAHost() const;
	virtual void SendState(MWsWindowTreeObserver& aWindowTreeObserver) const;
	
public:	// from CWsObject
	TBool CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd);
	
public: // from MWsWindowTreeNode
	MWsWindowTreeNode::TType NodeType() const;
	const MWsWindow* Window() const;
	const MWsSprite* Sprite() const;
	const MWsStandardTextCursor* StandardTextCursor() const;
	const MWsWindowGroup* WindowGroup() const;
	const MWsWindowTreeNode* ParentNode() const;	
	
protected:
	TInt OrdinalPosition(TBool aFull) const;
	TBool CheckOrdinalPositionChange(TInt aPos);
	virtual void Shutdown();	// Part of two phase destruction
	void SetPointerCursor(CWsPointerCursor *aCursor);
	void SetPointerCursorByIndex(TInt aIndex);
	void ChangeWindowPosition(TInt aPos,CWsWindowBase* aNewParent);

private:
	void RemoveFromSiblingList();
	virtual TBool IsActivated() const;

public:	
	CWsSpriteBase *iSpriteList; // Start of linked list of sprites attached to the window		
	
protected:
	enum TBaseWinFlags
		{
		EBaseWinAdvancedPointersEnabled = 0x00000001,
		EBaseWinNodeCreated				= 0x00000002,
		};
	
	CWsWindowBase *iParent;
	CWsWindowBase *iSibling;
	CWsWindowBase *iChild;
	/** The handle specified by the client when the object was created.
	
	This is different from CWsObject::iWsOwner which is the handle assigned to the object by the server.
	@see CWsObject::iWsOwner
	*/
	TUint32 iClientHandle;
	TInt iOrdinalPriority;
	TWinType iWinType;
	TInt iFadeCount;
	TUint iBaseWinFlags;

private:
	CWsPointerCursor *iPointerCursor;
	
#if defined(_DEBUG)
public:
	void CheckTree();
	TBool IsClientHandleInUse(TUint32 aHandle);
#endif
	};
	
inline TWinType CWsWindowBase::WinType() const
	{
	return iWinType;
	}

inline CWsPointerCursor *CWsWindowBase::PointerCursor() const
	{
	return iPointerCursor;
	}

inline CWsWindowBase *CWsWindowBase::NextSibling() const
	{
	return iSibling;
	}

inline CWsWindowBase *CWsWindowBase::BaseChild() const
	{
	return iChild;
	}

inline CWsWindowBase *CWsWindowBase::BaseParent() const
	{
	return iParent;
	}

inline TUint32 CWsWindowBase::ClientHandle() const
	{
	return iClientHandle;
	}

inline TInt CWsWindowBase::FadeCount() const
	{
	return iFadeCount;
	}

inline TBool CWsWindowBase::AdvancedPointersEnabled() const
	{
	return iBaseWinFlags&EBaseWinAdvancedPointersEnabled;
	}

inline TBool CWsWindowBase::HasSprite() const
	{
	return (NULL != iSpriteList);
	}

#endif
