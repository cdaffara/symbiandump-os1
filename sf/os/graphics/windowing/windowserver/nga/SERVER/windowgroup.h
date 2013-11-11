// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// CWsWindow and associated classes definitions
// 
//

#ifndef __WINDOWGROUP_H__
#define __WINDOWGROUP_H__

#include "server.h"
#include "tcursor.h"

class CWsWindowGroup : public CWsWindowBase, public MWsWindowGroup
	{
private:
	enum {EMaxIdentifierCount=10000};	// Could go as high as the highest signed int, but this would be impractical to test
	enum TGroupWindowFlags {
		EGroupFlagDisableKeyClick=0x01,
		EGroupFlagReceivesFocus=  0x02,
		EGroupFlagAutoForeground= 0x04,
		EGroupFlagHandlesDeviceChange= 0x08,
		EGroupFlagScreenDeviceDeleted= 0x10,
		EGroupFlagMsgQueueActive= 0x20,
		EGroupFlagMsgQueueNew=	  0x40,
		EGroupFlagMessageSignalled= 0x80,
		};
public:
	static CWsWindowGroup* NewL(CWsClient* aOwner, CScreen* aScreen, 
		const TWsClCmdCreateWindowGroup& aCmd);
	~CWsWindowGroup();
private:
	CWsWindowGroup(CWsClient* aOwner,CScreen* aScreen);
	void ConstructL(const TWsClCmdCreateWindowGroup &aCmd);
public: // virtual functions from CWsObject
	void CommandL(TInt aOpcode, const TAny *aCmdData);
public: // virtual functions from CWsWindowBase
	TPoint Origin() const;
	TRect AbsRect() const;
	TSize Size() const;
	void SendState(MWsWindowTreeObserver& aWindowTreeObserver) const;
	void SendStateWindowGroupChain(MWsWindowTreeObserver& aWindowTreeObserver) const;
	
public: // virtual functions from MWsWindowGroup
	TInt Identifier() const;
	TPtrC Name() const;
	TBool IsFocusable() const;
	TInt OrdinalPriority() const;
    const MWsClient * Client() const;

public:
	CWsTopClientWindow *Child() const;
	inline CWsWindowGroup *PrevSibling() const;
	inline CWsWindowGroup *NextSibling() const;
	CWsRootWindow *Parent() const;
	virtual void AreaCovered(TRegion &aRegion);
	void UpdateOrdinalPriority(TBool aDoAdjust);
	void LostFocus();
	void ReceivedFocus();
	inline RWsTextCursor *TextCursor();
	inline TBool ReceivesFocus() const;
	static TInt NumWindowGroups(TBool aAllPriorities, TInt aPriority);
	static TInt SendWindowGroupListAndChainL(TBool aAllPriorities, TInt aPriority, TInt aCount);
	static TBool SendEventToAllGroups(TBool aAllPriorities,TBool aOnePerClient,const TWsClCmdSendEventToWindowGroup& aData);
	static void SendMessageToAllGroupsL(CWsClient& aSender,TBool aAllPriorities,const TWsClCmdSendMessageToWindowGroup& aData);
	static void GetFocusWindowGroupL();
	static void ReleasePendedMessagesToAllGroups(CWsClient * aClient);
	virtual void SetOrdinalPosition(TInt pos);
	TBool SetOrdinalPosition(TInt pos,CWsWindowGroup *aClosingWindow);
	void SetOrdinalPriority(TInt aPos, TInt aPriority);
	void QueueMessageL(TUid aUid, TInt aDataLength, CWsClient& aSender);
	void AddPriorityKeyL(TUint aKeycode, TUint aModifierMask, TUint aModifiers);
	void RemovePriorityKey(TUint aKeycode, TUint aModifierMask, TUint aModifiers);
	void RemoveAllPriorityKeys();
	TBool CheckForPriorityKey(const TKeyEvent &aKeyEvent);
	inline CWsPointerCursor *GroupPointerCursor() const;
	static CWsWindowGroup *WindowGroupFromIdentifier(TInt aIdentifier);
	static CWsWindowGroup *WindowGroupFromIdentifierL(TInt aIdentifier);
	static CWsWindowGroup *FindWindowGroupL(CWsClient* aClient,TInt aIdentifier,TInt aOffset,const TPtrC *aMatch,const TThreadId *aThreadId);
	inline HBufC *GroupName();
	inline void SetNextDefaultOwningWindow(CWsWindowGroup *aNext);
	inline CWsWindowGroup **NextDefaultOwningWindowPtr();
	void SetScreenDeviceValidState(const DWsScreenDevice *aDevice);
	void SetScreenChangeEventStateL(TBool aEnabled);
	TBool ScreenDeviceValid() const;
	TBool CanReceiveFocus() const;
	static void SetScreenDeviceValidStates(const DWsScreenDevice *aDevice);
	static void SetScreenDeviceValidStates(CScreen* aScreen);
	static void NewOrientation(TInt aMode,CFbsBitGc::TGraphicsOrientation aRotation, CWsRootWindow* aRootWindow);
	inline DWsScreenDevice *Device();
	void FetchMessageL();
	static inline void SetFocusGainPreprocessing(TBool aDo);
	TBool IsChained(TInt& aParentId);
	inline TInt NumWindowGroupsOnMyScreen(TInt aPriority);
	static TInt NumWindowGroupsOnScreen(const CWsWindowGroup* aGroupWin,TBool aAllPriorities,TInt aPriority);
	inline void SetScreenDeviceDeleted();
	inline TBool ScreenDeviceDeleted() const;
	static TInt SendWindowGroupListL(TInt aScreenNumber, TBool aAllPriorities, TInt aPriority, TInt aCount);
	static void GetFocusWindowGroupL(TInt aScreenNumber);
	void SetScreenDevice(DWsScreenDevice *aDevice);
	static void SetEventQueueTestState(TBool aEventQueState);
	TBool HasVisibleTranslucentChild();
	void  ReleasePendedMessage();	
private:
	void SwitchToOwningWindow(CWsWindowGroup *aClosingWindow);
	void MoveChainedWindows(TDblQueIter<CWsWindowGroup>& aIter,TBool aForward,TInt aPos,CWsWindowGroup* aClosingWindow);
	TBool DoSetOrdinalPosition1(TInt aPos,CWsWindowGroup *aClosingWindow);
	void DoSetOrdinalPosition2(TInt aPos,CWsWindowGroup *aClosingWindow);
	TBool SignalMessageReady();
	static void AdvanceIdentifierCount();
	void StatusDump(TDes &aBuf);
	void PurgeCapturedKeys();
	inline void UpdateKeyClickState();
	void ResetFocus(CWsWindowGroup *aClosingWindow);
	static TInt GetWindowGroupListL(TInt aScreenNo,TBool aAllPriorities,TInt aPriority,TInt aCount,CArrayFixFlat<TInt>* aList);	
	inline CWsWindowGroup* BeforeInChain();
	static void GetWindowGroupListAndChainL(TInt aScreen,TBool aAllPriorities,TInt aPriority
																,RArray<RWsSession::TWindowGroupChainInfo>& list,TInt& aCountLeft);
	TBool CheckCapability(TInt& aOrdinalPriority);
	void DeleteQueue(TDblQue<CWsWindowGroup>* aQueue);
	TInt NumClientWindowGroups();
	void DisconnectFloatingSprites();
private:
	RWsTextCursor iTextCursor;
	TInt iFlags;
	TInt iOrdinalPriorityBase;
	TInt iOrdinalPriorityAdjust;
	TPriorityKey *iPriorityKeys;
	TInt iOwningWindowGroup;
	HBufC *iGroupName;
	TInt iIdentifier;
	CWsWindowGroup *iNextDefaultOwningWindow;
	CArrayVarSeg<TWsMessage> *iMessageArray;
	static TInt iIdentifierCount;
	DWsScreenDevice *iScreenDevice;
	static RPointerArray< TDblQue<CWsWindowGroup> > iChains;
	TDblQue<CWsWindowGroup>* iQueue;
	TDblQueLink iChainLink;
	TUint32 iChildSID;
	static TBool iFocusGainPreProcess;		//'REMOVEFADINGONFOCUSGAIN' flag in INI file
	// used for eventqueue testing
#if defined(_DEBUG)
	static TInt iSkipCount;
#endif
	static TBool iEventQueueTest;
	friend class TWsPointer;
	};

inline CWsTopClientWindow *CWsWindowGroup::Child() const
	{
	return (CWsTopClientWindow *)iChild;
	}

inline CWsWindowGroup *CWsWindowGroup::NextSibling() const
	{
	return (CWsWindowGroup *)iSibling;
	}

inline CWsWindowGroup *CWsWindowGroup::PrevSibling() const
	{
	return (CWsWindowGroup *)GetPrevSibling();
	}

inline CWsRootWindow *CWsWindowGroup::Parent() const
	{
	return (CWsRootWindow *)iParent;
	}

inline TBool CWsWindowGroup::ReceivesFocus() const
	{
	return(iFlags&EGroupFlagReceivesFocus);
	}

inline RWsTextCursor *CWsWindowGroup::TextCursor()
	{
	return &iTextCursor;
	}

inline HBufC *CWsWindowGroup::GroupName()
	{
	return iGroupName;
	}

inline void CWsWindowGroup::SetNextDefaultOwningWindow(CWsWindowGroup *aNext)
	{
	iNextDefaultOwningWindow=aNext;
	}

inline CWsWindowGroup **CWsWindowGroup::NextDefaultOwningWindowPtr()
	{
	return &iNextDefaultOwningWindow;
	}

/**
@return A pointer to the DWsScreenDevice used by this window-group, or 
		NULL when there is no DWsScreenDevice used by the window-group.
*/
inline DWsScreenDevice *CWsWindowGroup::Device()
	{
	return iScreenDevice;
	}

inline void CWsWindowGroup::SetFocusGainPreprocessing(TBool aDo)
	{
	iFocusGainPreProcess=aDo;
	}

inline void CWsWindowGroup::SetScreenDeviceDeleted()
	{
	iFlags|=EGroupFlagScreenDeviceDeleted;
	iScreenDevice = NULL; //  PDEF100409
	}

inline TBool CWsWindowGroup::ScreenDeviceDeleted() const
	{
	return (iFlags&EGroupFlagScreenDeviceDeleted);
	}

#endif
