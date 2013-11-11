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
// Window server client side local header file
// 
//

#ifndef __CLIENT_H__
#define __CLIENT_H__

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
	void Write(TInt handle,TUint opcode,const TAny *pData, TInt length,const TAny *pData2, TInt length2);
	void Write(TInt handle,TUint opcode,const TAny *buf, TInt bufLen);
	void Write(TInt handle,TUint opcode);
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
private:		//Added here as adding it to RWsSession breaks BC.
	TInt iDirectAcessCount;
	RArray<TInt> iBitmapArray;
	TBool iInvalidBitmapArray;
	};

inline TInt RWsBuffer::BufferSize() const
	{return(iBuf.MaxLength());}

inline TBool RWsBuffer::IsEmpty() const
	{return iBuf.Length()==0;}
#endif
