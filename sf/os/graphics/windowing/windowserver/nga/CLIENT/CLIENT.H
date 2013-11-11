// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Window server client side local header file
// 
//

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <e32hashtab.h>

#if defined(_DEBUG) && defined(__WINS__)
//#define __AUTO_FLUSH		//Define this for test purposes only, it hard codes auto flushing
#endif

/** Panics the client. This will result in the client thread being destroyed. */ 
GLREF_C void Panic(TW32Panic aPanic); 
GLREF_C void Assert(TW32Assert aPanic); 

class TReadDescriptorType;
class TWriteDescriptorType;

class RWsBuffer
	{
	friend class RWsSession;
public:
	enum bufferSizes
		{
		EDefBufferSize=EClientBufferSize,
		EMinBufferSize=EClientBufferSize,
		EMaxBufferSize=EClientBufferMaxSize
		};
public:
	RWsBuffer(RWsSession *aSession);
	void Close();
	void Destroy();
	TInt Open();
	TInt Connect(TUint32 handle);
	TInt WriteReplyWs(TUint opcode);
	TInt WriteReplyWs(const TAny *pData, TInt length,TUint opcode);
	TInt WriteReplyWs(const TAny *pData, TInt length, const TAny *pData2, TInt length2, TUint opcode);
	void SetBufferSizeL(TInt aBufSize);
	void SetMaxBufferSizeL(TInt aMaxBufSize);
	TInt Flush(const TIpcArgs* aIpcArgs=NULL,TBool aRequestFinish=EFalse);
	TBool SetAutoFlush(TBool aState);
	void Write(TInt aHandle, TUint aOpcode, const TAny *aData, TInt aLength, const TAny *aData2, TInt aLength2, const TIpcArgs* aIpcArgs=NULL);
	void Write(TInt aHandle, TUint aOpcode, const TAny *aData, TInt aLength, const TIpcArgs* aIpcArgs=NULL);
	void Write(TInt handle,TUint opcode);
	void AppendData(const TAny *aData, TInt aLength, TBool aFinished);
	TInt WriteReply(TInt handle,TUint opcode, const TIpcArgs* aIpcArgs=NULL);
	TInt WriteReply(TInt handle,TUint opcode,const TAny *buf, TInt bufLen, const TIpcArgs* aIpcArgs=NULL);
	TInt WriteReply(TInt handle,TUint opcode,const TAny *buf, TInt bufLen,const TAny *buf2, TInt buflen2, const TIpcArgs* aIpcArgs=NULL);
	TInt WriteReplyP(TInt aHandle,TUint aOpcode,const TWriteDescriptorType& aReplyBuffer);
	TInt WriteReplyP(TInt aHandle,TUint aOpcode,const TAny *aData,TInt aLength,const TWriteDescriptorType& aReplyBuffer);
	TInt WriteReplyP(TInt aHandle,TUint aOpcode,const TAny *aData1,TInt aLengthData1,const TAny *aData2,TInt aLengthData2,const TWriteDescriptorType& aReplyBuffer);
	TInt WriteReplyByProvidingRemoteReadAccess(TInt aHandle,TUint aOpcode,const TAny *aData, TInt aLength,const TReadDescriptorType& aRemoteReadBuffer);

	inline TInt BufferSize() const;
	inline TBool IsEmpty() const;
	inline RWsSession* Session() {return iSession;}
	static void FlushAllBuffers(TInt aBitmapHandle);
	void SetCallBack();
	void CancelCallBack();
	void AddToBitmapArray(TInt aBitmapHandle);
	
	void SetWsGraphicManager(CWsGraphic::CManager* aManager);
	static CWsGraphic::CManager* WsGraphicManager();

	void AsyncRequest(TInt aHandle, TUint aOpcode, TRequestStatus& aStatus);

	void EnableWindowSizeCacheL();
	inline TBool WindowSizeCacheEnabled() const;
	inline void MarkWindowSizeCacheDirty(TInt aHandle);
	inline void RefreshWindowSizeCache(TInt aHandle, const TSize& aNewSize);
	inline TInt CachedWindowSize(TInt aHandle, TSize& aSize);
	inline void DestroyWindowSizeCacheEntry(TInt aHandle);	
    
private:
	TInt DoWrite(TInt aHandle, TUint aOpcode, TBool aFlush, const TIpcArgs* aIpcArgs, const TAny* aData1=NULL, TInt aLength1=0, const TAny* aData2=NULL, TInt aLength2=0);
	inline void SetAndLimitMaxBufSize(TInt aMaxBufSize);
	void ReAllocBufferL(TInt aNewSize);
	TBool ReAllocBuffer(TInt aNewSize);
	void GrowBuffer(TInt aRequiredSpace, TInt aMsgSize);

private:
	RWsSession *iSession;
	CWsGraphic::CManager* iManager;
	TBool iAutoFlush;
	TPtr8 iBuf;
	RWsBuffer *iNext;
	TInt iPreviousHandle;
	TInt iBufSize;				// current buffer size, DoWrite can expand this up to iMaxBufize
	TInt iMaxBufSize;			// maximum buffer size, set by SetBufferSizeL or SetMaxBufferSizeL
#if defined(_DEBUG)
	TInt iAppendDataLength;
#endif
private:		//Added here as adding it to RWsSession breaks BC.
	TInt iDirectAcessCount;
	RArray<TInt> iBitmapArray;
	TBool iInvalidBitmapArray;

	class TWindowSizeCacheEntry
	    {
	public:
	    inline TWindowSizeCacheEntry(const TSize& aSize);
	public:
	    TSize iSize;
	    TBool iDirty;
	    };
	
	RHashMap<TInt, TWindowSizeCacheEntry>* iWindowSizeCache;
	};


inline TInt RWsBuffer::BufferSize() const
	{return(iBuf.MaxLength());}

inline TBool RWsBuffer::IsEmpty() const
	{return iBuf.Length()==0;}

inline RWsBuffer::TWindowSizeCacheEntry::TWindowSizeCacheEntry(const TSize& aSize)
    : iSize(aSize), iDirty(EFalse)
    {
    }

inline TBool RWsBuffer::WindowSizeCacheEnabled() const
    {
    return iWindowSizeCache ? ETrue : EFalse;
    }

inline void RWsBuffer::MarkWindowSizeCacheDirty(TInt aHandle)
    {
    __ASSERT_ALWAYS(iWindowSizeCache != NULL, Assert(EW32AssertWindowSizeCacheFailure));
    RWsBuffer::TWindowSizeCacheEntry* entry = iWindowSizeCache->Find(aHandle);
    if (entry != NULL)
        {
        entry->iDirty = ETrue;
        }
    }

inline void RWsBuffer::RefreshWindowSizeCache(TInt aHandle, const TSize& aNewSize)
    {
    __ASSERT_ALWAYS(iWindowSizeCache != NULL, Assert(EW32AssertWindowSizeCacheFailure));
    RWsBuffer::TWindowSizeCacheEntry* entry = iWindowSizeCache->Find(aHandle);
    if (entry == NULL)
        {
        iWindowSizeCache->Insert(aHandle, RWsBuffer::TWindowSizeCacheEntry(aNewSize));
        }
    else
        {
        entry->iSize = aNewSize;
        entry->iDirty = EFalse;
        }
    }

inline TInt RWsBuffer::CachedWindowSize(TInt aHandle, TSize& aSize)
    {
    __ASSERT_ALWAYS(iWindowSizeCache != NULL, Assert(EW32AssertWindowSizeCacheFailure));
    RWsBuffer::TWindowSizeCacheEntry* entry = iWindowSizeCache->Find(aHandle);
    if ( entry && !(entry->iDirty) )
        {
        aSize = entry->iSize;
        return KErrNone;
        }
    else
        {
        // return KErrNotFound when either entry not found for aHandle, or entry is dirty
        return KErrNotFound;
        }
    }

inline void RWsBuffer::DestroyWindowSizeCacheEntry(TInt aHandle)
    {
    __ASSERT_ALWAYS(iWindowSizeCache != NULL, Assert(EW32AssertWindowSizeCacheFailure));
    TInt err = iWindowSizeCache->Remove(aHandle);
    // If there is a window size cache entry, then err == KErrNone.
    // Otherwise, there isn't a size cache entry, and err == KErrNotFound.
    __ASSERT_DEBUG(err == KErrNone || err == KErrNotFound, Assert(EW32AssertWindowSizeCacheFailure));
    }


#endif
