// KernLbxModel.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef KERNLBXMODEL_H
#define KERNLBXMODEL_H

#include <e32hashtab.h>

#include <W32STD.H>

class CKernListBoxData;
class TObjectKernelInfo;
class TThreadKernelInfo;
class CUndertaker;
class CDlgProcessInfo;
class RMemoryAccess;
class CApaWindowGroupName;
class RClogger;
class MKernListBoxDataDelegate;
class RWsSession;

NONSHARABLE_CLASS(CKernListBoxModel) : public CBase, public TSwap, public TKey
	{
public: // External interface
	CKernListBoxModel(RMemoryAccess& aMemAccess);
	~CKernListBoxModel();
	void ConstructL();
	void SetCurrentListL(TInt aCurrentList);
	TInt GetCurrentList() const;
	void RefreshDataL(TInt aIndex=-1); // -1 means refresh all

	void DumpToCloggerL(RClogger& aClogger);
	void CloseAllThreadHandles();
	void SummaryInfoL(TDes& aBuf);
	void Sort(TLinearOrder<CKernListBoxData> aOrder);
	void SetInfoChangedCallback(TInt aIndex, const TCallBack& aCallback);

public:
	TInt Count() const;
	TInt ItemIdIndex(TInt aItemId) const;
	CKernListBoxData* RetrieveDataL(TInt aItemIndex);
	CKernListBoxData* RetrieveDataLC(TInt aItemIndex);
	virtual void ModelBeginUpdateLC();
	virtual void ModelEndUpdateL();
	virtual void RemoveAllDataL();
	virtual void DataUpdatedL(TInt aIndex);
	virtual void RemoveDataL(TInt aItemIndex);
	virtual void Sort();


protected: // From TSwap,TKey
	void Swap(TInt aLeft,TInt aRight) const;
	void WtfSwap(TInt aLeft,TInt aRight); // How on earth can Swap be const??
	TInt Compare(TInt aLeft,TInt aRight) const;

public: // For helpers to use, not for public use by client
	void ThreadDiedL(TInt aHandle);
	virtual void NewKernDataL(TInt aType, TObjectKernelInfo* aInfo); // aInfo ownership transferred at END of function
	RMemoryAccess& MemAccess() { return iMemAccess; }

protected:
	void DoNewKernDataL(TInt aType, TObjectKernelInfo* aInfo, MKernListBoxDataDelegate* aDelegate);

protected:
	RPointerArray<CKernListBoxData> iItemData;

private:
	TInt iCurrentList;
	RArray<TInt> iThreadHandleArray;
	CUndertaker* iUnd;
	TLinearOrder<CKernListBoxData> iSort;
	RWsSession* iWsSession; // Needed to keep the window group data objects valid
	RBuf iTempBuf1, iTempBuf2; // So that calls to FormatL don't need to allocate so much
	TCallBack iInfoChangedCallback;
	CKernListBoxData* iCallbackData;
	RMemoryAccess& iMemAccess;
	friend class CKernListBoxData;
	};

class MKernListBoxDataDelegate
	{
public:
	virtual void DataObjectAboutToDestruct(CKernListBoxData* aData) =0;
	virtual void FormatChangedL(CKernListBoxData* aData, const TDesC& aTitle, const TDesC& aInfo) =0;
	virtual TPtrC NameForSortPurposes() const =0;
	};


NONSHARABLE_CLASS(CKernListBoxData) : public CBase
	{
public:
	CKernListBoxData(CKernListBoxModel* aModel);
	~CKernListBoxData();
	void FormatL(TObjectKernelInfo* aInfo, RBuf& aTempBuf1, RBuf& aTempBuf2);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);

	TInt ExeNameForSid(TUint aSid, RBuf& aName);

public:
	void Open();
	void Close();
	//TInt RefCount() const;
	TInt ItemId() const;
	void SetItemId(TInt aItemId);
	void ConstructL();
	void SetDelegate(MKernListBoxDataDelegate* aOwner);
	MKernListBoxDataDelegate* Delegate();
	const MKernListBoxDataDelegate* Delegate() const;
	CKernListBoxModel& Model();

public: // Things for the dialog to use
	virtual TBool SupportsCommand(TInt aCommand);
	void KillL();
	void InfoForDialogL(RBuf& aTitle, RBuf& aText, TBool aRefresh); // Pass in false to use cached values if available
	TAny* GetHandleL();
	
protected:
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);

public:
	TInt iType;
	TObjectKernelInfo* iInfo; // Either a TObjectKernelInfo* or a SOpenFile* etc
private:
	TInt iItemId;
	MKernListBoxDataDelegate* iDelegate;
	TInt iRefCount;
	CKernListBoxModel* iModel; // This is necessary so we can figure out who the parent of a process is, but it does mean a particualar instance can only be used by one model at once (which is not an issue for us)
	};

NONSHARABLE_CLASS(COpenFilesListBoxData) : public CKernListBoxData
	{
public:
	COpenFilesListBoxData(CKernListBoxModel* aModel);
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	};
	
NONSHARABLE_CLASS(CThreadsListBoxData) : public CKernListBoxData
	{
public:
	CThreadsListBoxData(CKernListBoxModel* aModel);
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	virtual TBool SupportsCommand(TInt aCommand);
	};

NONSHARABLE_CLASS(CFeatRegListBoxData) : public CKernListBoxData
	{
public:
	CFeatRegListBoxData(CKernListBoxModel* aModel);
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	};
	
NONSHARABLE_CLASS(CServerListBoxData) : public CKernListBoxData
	{
public:
	CServerListBoxData(CKernListBoxModel* aModel);
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	virtual TBool SupportsCommand(TInt aCommand);
	};

NONSHARABLE_CLASS(CHalListBoxData) : public CKernListBoxData
	{
public:
	CHalListBoxData(CKernListBoxModel* aModel);
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	};

NONSHARABLE_CLASS(CWindowGroupListBoxData) : public CKernListBoxData
	{
public:
	CWindowGroupListBoxData(CKernListBoxModel* aModel);
	~CWindowGroupListBoxData();
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	};

NONSHARABLE_CLASS(CMsgQueListBoxData) : public CKernListBoxData
	{
public:
	CMsgQueListBoxData(CKernListBoxModel* aModel);
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	};

NONSHARABLE_CLASS(CMutexListBoxData) : public CKernListBoxData
	{
public:
	CMutexListBoxData(CKernListBoxModel* aModel);
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	};

NONSHARABLE_CLASS(CSemaphoreListBoxData) : public CKernListBoxData
	{
public:
	CSemaphoreListBoxData(CKernListBoxModel* aModel);
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	};

NONSHARABLE_CLASS(CTimerListBoxData) : public CKernListBoxData
	{
public:
	CTimerListBoxData(CKernListBoxModel* aModel);
	virtual void DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId);
	virtual void DumpToCloggerL(RClogger& aClogger, TInt i, TInt count);
	virtual void DoInfoForDialogL(RBuf& aTitle, RBuf& aText, TDes* aTemp);
	};

NONSHARABLE_CLASS(CUndertaker) : public CActive
	{
public:
	CUndertaker(CKernListBoxModel& aModel);
	~CUndertaker();
	void RunL();
	void DoCancel();
	void Register();

	CKernListBoxModel& iModel;
	RUndertaker iUnd;
	TInt iHandle;
	};

struct SHalInfo
	{
	TInt iProperties;
	TInt iValue;
	TInt iAttribute;
	};

struct SWgInfo
	{
	TInt iHandle;
	CApaWindowGroupName* iName;
	RWsSession* iSession; // Doesn't really belong here (as it will be the same for all objects) but it means the data objects don't have to have a handle to it
	};

struct SDataType
	{
	TBuf<256> iMime; // 256 is KMaxDataTypeLength
	TFileName iRecog;
	TFileName iApp;
	TUid iAppUid;
	};

struct SOpenFile
	{
	TFileName iName;
	TInt iNumThreads;
	TFixedArray<TThreadId, 10> iThreadIds;
	};

struct SFeature
	{
	TInt iFeature;
	TInt iErr;
	TUint32 iInfo;
	};

_LIT(Klf, "\n");
_LIT(Klflf, "\n\n");

#endif
