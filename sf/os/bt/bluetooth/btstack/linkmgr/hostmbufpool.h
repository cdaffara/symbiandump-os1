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


#ifndef HOSTMBUFPOOL
#define HOSTMBUFPOOL

#include <e32base.h>
#include <bttypes.h>
#include <es_mbuf.h>
#include <bluetooth/hcicommandqueueclient.h>

#ifdef HOSTCONTROLLER_TO_HOST_FLOW_CONTROL

class MHCICommandQueue;


NONSHARABLE_CLASS(CHostMBufPool) 
	: public CActive
	, private MHCICommandQueueClient
	{

	NONSHARABLE_STRUCT(TPoolBuffer)
		{
		RMBufChain		iMBufChain;
		THCIConnHandle	iCurrentHandle;
		TSglQueLink		iLink;
		};
	
public:
	static CHostMBufPool* NewL(MHCICommandQueue& aCommandQueue);
	~CHostMBufPool();
	
	RMBufChain TakeBufferL(THCIConnHandle aConnHandle);
	
	void InvalidateByConnH(THCIConnHandle aConnHandle);
	
private:
	CHostMBufPool(MHCICommandQueue& aCommandQueue);
	void ConstructL();
	
	void DeletePool(TSglQue<TPoolBuffer>& aPool);
	void DeleteBuffer(TPoolBuffer*& aBuffer);
	void AllocNewBuffer(TPoolBuffer& aBuffer);
	TPoolBuffer* CreatePoolBufferL();
	void HostNumberOfCompletedPacketsL(THCIConnHandle aConnH, TUint16 aNumPackets);
	void TryToAllocQueuedBuffer();
	
	inline void AddToBufferPool(TPoolBuffer& aBuffer);
	inline void RemoveFromBufferPool(TPoolBuffer& aBuffer);
	
	void Error(TInt aError);
	
private: // from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
private: // from MHCICommandQueueClient
	void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);
	
private:
	MHCICommandQueue& iCmdQ;
	
	TSglQue<TPoolBuffer> iBufferPool;
	TSglQue<TPoolBuffer> iWaitingAllocPool;
	TPoolBuffer* iBufferBeingAllocd;
	
	// We batch up completed packets notifications for a connection handle so that they are 
	// only sent every 'x' packets OR if we get a packet for a different connection handle.
	TInt iCurrAckHandle; // current handle of packets being batched for completion
	TUint iCurrCompletedPackets; // number of packets already completed for current handle
	
	RMBufAsyncRequest iMBufRequester;
	};
	
inline void CHostMBufPool::AddToBufferPool(TPoolBuffer& aBuffer)
	{
	iBufferPool.AddLast(aBuffer);
	}
	
inline void CHostMBufPool::RemoveFromBufferPool(TPoolBuffer& aBuffer)
	{
	iBufferPool.Remove(aBuffer);
	}
	
#endif //HOSTCONTROLLER_TO_HOST_FLOW_CONTROL

#endif // HOSTMBUFPOOL
