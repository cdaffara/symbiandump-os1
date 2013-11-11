// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// source\server\MmfBtSwCodecDatapath.cpp
// 
//

#include "mmfBtSwCodecDataPath.h"

CMMFSwCodecDataPath::~CMMFSwCodecDataPath()
	{
	}

/*
 * CycleAudioBufferL
 *
 * Sets up a usable buffer for passing to MMF
 *	
 * This method has been written such that it must allocate a new buffer before
 * replacing the existing one. The purpose of this is to force creation of a 
 * new buffer. Simply deleting and then re-allocing may result in the same 
 * address being used.
 * 
 * Only cycles if there is enough memory
 *
 */
#ifdef __CYCLE_MMF_DATABUFFERS__
CMMFDataBuffer* CMMFSwCodecDataPath::CycleAudioBuffer(CMMFDataBuffer* aBuffer)
	{
	CMMFDataBuffer* buffer = NULL;
	TUint bufferSize = aBuffer->Data().MaxLength();

#ifdef __USE_MMF_TRANSFERBUFFERS__
	TRAPD(err, buffer = CreateTransferBufferL(bufferSize, static_cast<CMMFTransferBuffer*>(aBuffer)));
#else
	TRAPD(err,buffer = CMMFDataBuffer::NewL(bufferSize));

	if (err == KErrNone)
		{
		delete aBuffer;
		}
#endif
	if (err != KErrNone)
		{//there was a problem creating buffer eg OOM so use same buffer
		buffer = aBuffer;
		}

	return buffer;

	}
#endif

/*
 * DoCleanupRHandleBase
 *
 * This method will initially Close the handle and then delete it.
 *
 */
#ifdef __USE_MMF_TRANSFERBUFFERS__
inline static void DoCleanupRHandleBase(TAny* aRHandleBase)
	{
	ASSERT(aRHandleBase);
	RHandleBase* rHandleBase = STATIC_CAST(RHandleBase*, aRHandleBase);
	TRAPD(error, rHandleBase->Close());
	delete aRHandleBase;
	}

CMMFTransferBuffer* CMMFSwCodecDataPath::CreateTransferBufferL(TUint aBufferSize, CMMFTransferBuffer* aOldBuffer)
	{
	CMMFTransferBuffer* buffer = NULL;

	RTransferBuffer* transBuffer = new  (ELeave) RTransferBuffer;
	
	TCleanupItem bufferCleanupItem(DoCleanupRHandleBase, transBuffer); //closes and deletes.
	CleanupStack::PushL(bufferCleanupItem);

	RTransferWindow* transWindow = new (ELeave) RTransferWindow;
	
	TCleanupItem windowCleanupItem(DoCleanupRHandleBase, transWindow); //closes and deletes.
	CleanupStack::PushL(windowCleanupItem);

	User::LeaveIfError(transBuffer->Create(aBufferSize));
	User::LeaveIfError(transWindow->Create(aBufferSize));
	User::LeaveIfError(transWindow->MapInBuffer(*transBuffer));

	buffer = CMMFTransferBuffer::NewL(*transWindow);

	delete aOldBuffer; //closes RTransferWindow
	delete iTransferWindow;

	if(iTransferBuffer)
		{
		iTransferBuffer->Close();
		}
	delete iTransferBuffer;

	iTransferBuffer = transBuffer;
	iTransferWindow = transWindow;

	CleanupStack::Pop(transWindow); 
	CleanupStack::Pop(transBuffer); 

	return buffer;
	}
#endif


#ifdef __USE_MMF_PTRBUFFERS__
CMMFPtrBuffer* CMMFSwCodecDataPath::CreatePtrBufferL(TUint aBufferSize)
	{
	CMMFPtrBuffer* buffer = NULL;
	if (iPtrBufferMemoryBlock)
		delete iPtrBufferMemoryBlock;//incase already exisits
	iPtrBufferMemoryBlock = HBufC8::NewL(aBufferSize);
	TPtr8 ptrMemoryBlock(iPtrBufferMemoryBlock->Des());
	buffer = CMMFPtrBuffer::NewL(ptrMemoryBlock);
	return buffer;
	}
#endif  // __USE_MMF_PTRBUFFERS__

	
