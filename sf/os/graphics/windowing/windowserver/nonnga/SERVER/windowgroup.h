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
// CWsWindow and associated classes definitions
// 
//

#ifndef __WINDOWGROUP_H__
#define __WINDOWGROUP_H__

#include "server.h"
#include "tcursor.h"

class CWsWindowGroup : public CWsWindowBase
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
public: // from CWsObject
	virtual void CommandL(TInt aOpcode, const TAny *aCmdData);
public: // from CWsWindowBase
	virtual TPoint Origin() const;
	virtual TRect AbsRect() const;
	virtual TSize Size() const;
public:
	CWsClientWindow *Child() const;
	inline CWsWindowGroup *PrevSibling() const;
	inline CWsWindowGroup *NextSibling() const;
	CWsRootWindow *Parent() const;
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
	virtual void SetOrdinalPosition(TInt pos);
	TBool SetOrdinalPosition(TInt pos,CWsWindowGroup *aClosingWindow);
	void SetOrdinalPriority(TInt aPos, TInt aPriority);
	void QueueMessageL(TUid aUid, TInt aDataLength, CWsClient& aSender);
	void AddPriorityKeyL(TUint aKeycode, TUint aModifierMask, TUint aModifiers);
	void RemovePriorityKey(TUint aKeycode, TUint aModifierMask, TUint aModifiers);
	void RemoveAllPriorityKeys();
	TBool CheckForPriorityKey(const TKeyData &aKey, TInt aScanCode);
	inline CWsPointerCursor *GroupPointerCursor() const;
	static CWsWindowGroup *WindowGroupFromIdentifier(TInt aIdentifier);
	static CWsWindowGroup *WindowGroupFromIdentifierL(TInt aIdentifier);
	static CWsWindowGroup *FindWindowGroupL(CWsClient* aClient,TInt aIdentifier,TInt aOffset,const TPtrC *aMatch,const TThreadId *aThreadId);
	inline TInt Identifier() const;
	inline HBufC *GroupName();
	inline TInt OrdinalPriority();
	inline void SetNextDefaultOwningWindow(CWsWindowGroup *aNext);
	inline CWsWindowGroup **NextDefaultOwningWindowPtr();
	void SetScreenDeviceValidState(const DWsScreenDevice *aDevice);
	void SetScreenChangeEventStateL(TBool aEnabled);
	TBool ScreenDeviceValid() const;
	TBool CanReceiveFocus() const;
	static void SetScreenDeviceValidStates(const DWsScreenDevice *aDevice);
	static TBool SetScreenDeviceValidStates(const TBool aScreenSizeChanged, const TBool aSwapWidthAndHeight, CScreen* aScreen);
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
	TBool HasVisibleTranslucentChild();
private:
	void SwitchToOwningWindow(CWsWindowGroup *aClosingWindow);
	void MoveChainedWindows(TDblQueIter<CWsWindowGroup>& aIter,TBool aForward,TInt aPos,CWsWindowGroup* aClosingWindow);
	TBool DoSetOrdinalPosition1(TInt aPos,CWsWindowGroup *aClosingWindow);
	void DoSetOrdinalPosition2(TInt aPos,CWsWindowGroup *aClosingWindow);
	void SignalMessageReady();
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
// data
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
	friend class WsPointer;
	};

inline CWsClientWindow *CWsWindowGroup::Child() const
	{
	return (CWsClientWindow *)iChild;
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

inline TInt CWsWindowGroup::Identifier() const
	{
	return iIdentifier;
	}

inline HBufC *CWsWindowGroup::GroupName()
	{
	return iGroupName;
	}

inline TInt CWsWindowGroup::OrdinalPriority()
	{
	return iOrdinalPriorityBase;
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
