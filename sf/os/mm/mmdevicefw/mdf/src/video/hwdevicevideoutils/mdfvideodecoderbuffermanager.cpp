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
//

#include <fbs.h>
#include <mmf/server/mmfdatabuffer.h>
#include "mdfvideodecoderbuffermanager.h"

#define MDFVIDEODECODERBUFFERMANAGER_DEBUG 1

#if defined (SYMBIAN_MDFVIDEODECODERBUFFERMANAGER_DEBUG)
#define DEBUG_PRINT( x ) ( RDebug::Print( x ) )
#define DEBUG_PRINT2( x,y ) ( RDebug::Print( x,y ) )
#define DEBUG_PRINT3( x,y,z ) ( RDebug::Print( x,y,z ) )

#else
#define DEBUG_PRINT( x )
#define DEBUG_PRINT2( x,y ) 
#define DEBUG_PRINT3( x,y,z ) 
#endif // defined (SYMBIAN_MDFVIDEODECODERBUFFERMANAGER_DEBUG)

CVideoDataBuffer::CVideoDataBuffer()
	{
	}
	
TUint CVideoDataBuffer::Length()
	{
	return iSize;
	}

CVideoDataBuffer* CVideoDataBuffer::NewL(TUint aSize)
	{
	CVideoDataBuffer* self = new (ELeave) CVideoDataBuffer;
	CleanupStack::PushL(self);
	self->ConstructL(aSize);
	CleanupStack::Pop(self);
	return self;
	}
	
void CVideoDataBuffer::ConstructL(TUint aBufferSize)
	{
	iSize = aBufferSize;
	CMMFDescriptorBuffer* buffer = CMMFDescriptorBuffer::NewL(aBufferSize);
	TDes8& des = buffer->Data();
	TUint8* ptr = const_cast<TUint8*>(des.Ptr());
	iVideoInputBuffer.iData.Set(ptr, des.Length(), des.MaxLength());
	iMmfBuffer = buffer;
	}

CVideoDataBuffer::operator CMMFBuffer*()
	{
	return iMmfBuffer;
	}
	
CVideoDataBuffer::operator TVideoInputBuffer&()
	{
	return iVideoInputBuffer;
	}
	


TBool CVideoDataBuffer::operator== (const CMMFBuffer* aBuffer) const
	{
	return (aBuffer == iMmfBuffer); 	
	}

TBool CVideoDataBuffer::operator== (const TVideoInputBuffer& aVideoInputBuffer) const
	{
	// compare the data pointers
	return iVideoInputBuffer.iData == aVideoInputBuffer.iData;
	}

CVideoDataBuffer::~CVideoDataBuffer()
	{
	delete iMmfBuffer;
	}


CVideoFrameBuffer::CVideoFrameBuffer(TSize aFrameSize, 
											TUncompressedVideoFormat aFormat, 
											TDisplayMode aDisplayMode) 
	: iRawPtr(0,0),
	iFrameSize(aFrameSize),
	iVideoFormat(aFormat),
	iDisplayMode(aDisplayMode)
	{
	
	}

CVideoFrameBuffer* CVideoFrameBuffer::NewL(TSize aFrameSize, 
													TUncompressedVideoFormat aFormat,		
													TDisplayMode aDisplayMode)
	{
	CVideoFrameBuffer* self = new (ELeave) CVideoFrameBuffer(aFrameSize, aFormat, aDisplayMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CVideoFrameBuffer::SetLastBuffer(TBool aLastBuffer)
	{
	iLastBuffer = aLastBuffer;
	}
	
TBool CVideoFrameBuffer::LastBuffer()
	{
	return iLastBuffer;
	}
	
void CVideoFrameBuffer::ConstructL()
	{
	iFrameBufferSize = CFbsBitmap::ScanLineLength(iFrameSize.iWidth, iDisplayMode) *
														iFrameSize.iHeight;
	Mem::FillZ(&iVideoPicture, sizeof(TVideoPicture));
	if(iVideoFormat.iDataFormat == ERgbFbsBitmap) 
		{
		iBitmap = new(ELeave)CFbsBitmap;
		User::LeaveIfError(iBitmap->Create(iFrameSize, iDisplayMode));				
		DEBUG_PRINT2(_L("BufferManager: Created bitmap at 0x%08x"), iBitmap);
		
		iVideoPicture.iData.iRgbBitmap = iBitmap;
		iBitmap->LockHeap();
		TRAPD(err, iMmfBuffer = CMMFPtrBuffer::NewL(TPtr8(reinterpret_cast<TUint8*>(iBitmap->DataAddress()),
												iFrameBufferSize,
												iFrameBufferSize)) );
		iBitmap->UnlockHeap();
		User::LeaveIfError(err);
		}
	else if((iVideoFormat.iDataFormat == ERgbRawData) || (iVideoFormat.iDataFormat == EYuvRawData))
		{
		iRawBuffer  = HBufC8::NewMaxL(iFrameBufferSize);
		iRawPtr.Set(iRawBuffer->Des());
		iMmfBuffer = CMMFPtrBuffer::NewL(iRawPtr);
		iVideoPicture.iData.iRawData = &iRawPtr;
		}
	iVideoPicture.iData.iDataFormat = iVideoFormat.iDataFormat;
	iVideoPicture.iData.iDataSize = iFrameSize;
	}

CVideoFrameBuffer::operator CMMFBuffer*()
	{
	return iMmfBuffer;
	}
	
CVideoFrameBuffer::operator TVideoPicture&()
	{
	return iVideoPicture;
	}

CVideoFrameBuffer::operator CMMFBuffer&()
	{
	return *iMmfBuffer;
	}
	
CVideoFrameBuffer::operator TVideoPicture*()
	{
	return &iVideoPicture;
	}
	
void CVideoFrameBuffer::LockFrameBufferHeap()
	{
	iBitmap->LockHeap();
	iHeapLocked = ETrue;
	}

void CVideoFrameBuffer::UnlockFrameBufferHeap()
	{
	if (iHeapLocked)
		{
		iBitmap->UnlockHeap();
		iHeapLocked = EFalse;
		}
	}


TBool CVideoFrameBuffer::operator== (const CMMFBuffer* aBuffer) const
	{
	return (aBuffer == iMmfBuffer); 	
	}

TBool CVideoFrameBuffer::operator== (const TVideoPicture& aVideoInputBuffer) const
	{
	// compare the data pointers - cheat, it is the same pointer for the bitmap and 
	// raw data
	return iVideoPicture.iData.iRawData == aVideoInputBuffer.iData.iRawData;
	}

CVideoFrameBuffer::~CVideoFrameBuffer()
	{
	UnlockFrameBufferHeap();
	delete iMmfBuffer;
	delete iBitmap;
	delete iRawBuffer;
	}


/** 
 Constructs a new instance of CMDFVideoDecoderBufferManager.
 @return    "CMDFVideoDecoderBufferManager*"
            A pointer to the newly constructed CMDFVideoDecoderBufferManager
 */
CMdfVideoDecoderBufferManager* CMdfVideoDecoderBufferManager::NewL()
	{
	CMdfVideoDecoderBufferManager* s = new(ELeave) CMdfVideoDecoderBufferManager;
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return (static_cast<CMdfVideoDecoderBufferManager*>(s));
	}

CMdfVideoDecoderBufferManager::CMdfVideoDecoderBufferManager()
	{
	}
void CMdfVideoDecoderBufferManager::ConstructL()
	{
	RFbsSession::Connect();
	}

/**
 Default destructor
 */
CMdfVideoDecoderBufferManager::~CMdfVideoDecoderBufferManager()
	{
	// get rid of input buffers
	delete iInputBuffer;
	// cleanup frame buffer arrays	
	iEmptyFrameBuffers.ResetAndDestroy();
	iFilledFrameBuffers.ResetAndDestroy();
	iFrameBuffersInUse.ResetAndDestroy();
	RFbsSession::Disconnect();
	
	DEBUG_PRINT(_L("BufferManager: Closing down"));
	}

/** 
 Initializes the decoder buffer manager.
 @param		"const TUncompressedVideoFormat& aFormat"
 			The image format to decode into
 */
void CMdfVideoDecoderBufferManager::Init(const TUncompressedVideoFormat& aFormat)
	{
	DEBUG_PRINT(_L("BufferManager: Initializing"));

	iFormat = aFormat;
	// The actual frame size will come from the VOL headers.
	// Set an initial value for iFrameBufferSize so we can get
	// the VOL headers themselves.
	// Also ensure we allow one free output buffer.
	iFrameBufferSize = 1024; // 1k
	iMaxFrameBuffers = 1;
	
	iInputBufferInUse = EFalse;
	}

/** 
 Creates a new empty input buffer.
 @param		"TUint aBufferSize"
 			The requested buffer size.
 @return	"TVideoInputBuffer&"
 			An empty input buffer.
 @leave		May leave with KErrInUse if no input buffer is available, or with KErrNoMemory
 			if a buffer cannot be created or adjusted. 			 			
 */
CVideoDataBuffer& CMdfVideoDecoderBufferManager::CreateDataBufferL(TUint aBufferSize)
	{
	// We use one input buffer and one overflow buffer.

	// If the app requests another buffer before this one has been processed, then leave.
	if(iInputBufferInUse) 
		{
		User::Leave(KErrInUse);
		}
		
	if(aBufferSize < 1)
		{
		User::Leave(KErrArgument);
		}
	
	// Create or adjust the input buffer
	
	if(!iInputBuffer) 
		{
		iInputBuffer = CVideoDataBuffer::NewL(aBufferSize);
		}	
	else if(iInputBuffer->Length() != aBufferSize)
		{
		// NB don't use ReAllocL() as it will panic if the buffer is shrinking
		// Use temp variable		
		CVideoDataBuffer* tempBuffer = CVideoDataBuffer::NewL(aBufferSize);

		// Coverity reports a leave_without_push error here because
		// CVideoDataBuffer contains a frame buffer member which could contain
		// a CFbsBitmap member which can panic during its destructor. The panic
		// has a trapped leave and for some reason coverity does not recognise it.
		// Mark as false positive
		// coverity[leave_without_push : FALSE]
		delete iInputBuffer;
		iInputBuffer = NULL;
		iInputBuffer = tempBuffer;		
		}
	return *iInputBuffer;	
	}

void CMdfVideoDecoderBufferManager::ReturnDataBufferL(TVideoInputBuffer& aInputBuffer)
	{
	if (!iInputBuffer)
		{
		User::Leave(KErrNotFound);
		}	
	// if this buffer isn't the input buffer, leave
	if(! (*iInputBuffer == aInputBuffer) )
		{
		User::Leave(KErrNotFound);	
		}
	// if the buffer is not in use, leave
	if(!iInputBufferInUse)
		{
		User::Leave(KErrInUse);	
		}
	
	// finished with input buffer - mark it unused
	iInputBufferInUse = EFalse;
	}
	
CVideoDataBuffer& CMdfVideoDecoderBufferManager::GetDataBufferL()
	{
	if (!iInputBuffer)
		{
		User::Leave(KErrNotFound);
		}
	if (iInputBufferInUse)
		{
		User::Leave(KErrInUse);
		}
	iInputBufferInUse = ETrue;
	return *iInputBuffer;
	}

/** 
 Gets the number of available input buffers.
 @return	"TInt"
 			The number of available input buffers.
 */
TInt CMdfVideoDecoderBufferManager::DataBuffersAvailable() const
	{
	// this plugin has one input buffer. if it is in use (i.e. the client hasn't been
	// notified with MdvppNewBuffers()) then return 0.
	return ((iInputBufferInUse) ? 0 : 1);
	}

/** 
 Sets the frame size.
 @param		"const TSize& aSize"
 			The frame size.
 */
void CMdfVideoDecoderBufferManager::SetFrameSize(const TSize& aSize)
	{
	iFrameSize = aSize;
	}

/** 
 Gets the frame size.
 @return	"const TSize&"
 			The frame size.
 */
const TSize& CMdfVideoDecoderBufferManager::FrameSize() const
	{
	return iFrameSize;
	}

/** 
 Sets the frame buffer size.
 @param		"TInt aSize"
 			The frame buffer size.
 */
void CMdfVideoDecoderBufferManager::SetFrameBufferSize(TInt aSize)
	{
	iFrameBufferSize = aSize;
	}

/** 
 Gets the frame buffer size.
 @return	"TInt"
 			The frame buffer size.
 */
TInt CMdfVideoDecoderBufferManager::FrameBufferSize() const
	{
	return iFrameBufferSize;
	}

/** 
 Gets the maximum number of frame buffers.
 @return	"TInt"
 			The maximum number of frame buffers.
 */
TInt CMdfVideoDecoderBufferManager::MaxFrameBuffers() const
	{
	return iMaxFrameBuffers;
	}

/** 
 Gets the number of available pictures (full output buffers).
 @return	"TInt"
 			The number of available pictures.
 */
TInt CMdfVideoDecoderBufferManager::FilledFrameBuffersAvailable() const
	{
	return ((iFilledFrameBuffers.Count() > 0) ? 1 : 0);
	}

/** 
 Gets the number of available frame buffers.
 @return	"TInt"
 			The number of available frame buffers.
 */
TInt CMdfVideoDecoderBufferManager::EmptyFrameBuffersAvailable() const
	{
	return ((iEmptyFrameBuffers.Count() < iMaxFrameBuffers) ? 1 : 0);
	}
	
	
/** 
 Gets the first available framebuffer
 @return	"TVideoPicture&"
 			A picture.
 @leave		May leave with KErrNotReady if no picture is available. 			
 */
CVideoFrameBuffer& CMdfVideoDecoderBufferManager::GetEmptyFrameBufferL(TBool aLockHeap)
	{
	DEBUG_PRINT2(_L("Get Empty Frame Buffer avail: %d"),iEmptyFrameBuffers.Count());

	if(iEmptyFrameBuffers.Count() == 0) 
		{
		User::Leave(KErrNotReady);
		}
	CVideoFrameBuffer* frameBuf = iEmptyFrameBuffers[0];
	iFrameBuffersInUse.AppendL(frameBuf);
	iEmptyFrameBuffers.Remove(0);
	if (aLockHeap)
		{
		frameBuf->LockFrameBufferHeap();
		}
	return *frameBuf;
	}



/** 
 Gets the first available picture (full frame buffer).
 @return	"TVideoPicture&"
 			A picture.
 @leave		May leave with KErrNotReady if no picture is available. 			
 */
CVideoFrameBuffer& CMdfVideoDecoderBufferManager::GetFilledFrameBufferL(TBool aLockHeap)
	{
	DEBUG_PRINT2(_L("Get Filled Frame Buffer avail: %d"),iFilledFrameBuffers.Count());

	if(iFilledFrameBuffers.Count() == 0) 
		{		
		User::Leave(KErrNotReady);
		}
	CVideoFrameBuffer* frameBuf = iFilledFrameBuffers[0];
	iFilledFrameBuffers.Remove(0);
	iFrameBuffersInUse.AppendL(frameBuf);
	
	if (aLockHeap)
		{
		frameBuf->LockFrameBufferHeap();
		}
	return *frameBuf;
	}

/** 
 Gets the last displayed picture.
 Used by Direct Screen Access to redraw.
 @return	"TPictureData&"
 			The last displayed picture.
 @leave		May leave with KErrNotReady if no picture is available. 			
 */
const TPictureData& CMdfVideoDecoderBufferManager::LastPictureL() const
	{
	// it's a union, so we can check either iRgbBitmap or iRawData
	if(!iLastDisplayedFrame.iRgbBitmap) 
		{
		User::Leave(KErrNotReady);
		}
	return iLastDisplayedFrame;
	}

/** 
 Gets a new empty frame buffer.
 @param		aDisplayMode
 			The display mode of the bitmap to be created.
 @return	"TVideoPicture&"
 			A frame buffer.
 @leave		May leave with KErrNotReady if no frame buffer is available, or if there
 			is no memory to create a new one. 
 */
CVideoFrameBuffer& CMdfVideoDecoderBufferManager::CreateFrameBufferL(TDisplayMode aDisplayMode) 
	{
	CVideoFrameBuffer* frameBuffer = CVideoFrameBuffer::NewL(iFrameSize, iFormat, aDisplayMode);
	CleanupStack::PushL(frameBuffer);
	iEmptyFrameBuffers.AppendL(frameBuffer);
	CleanupStack::Pop(frameBuffer);
	
	DEBUG_PRINT2(_L("BufferManager: Created frame buffer; %d buffers in use"), iEmptyFrameBuffers.Count());
	return *frameBuffer;
	}

/** 
 Returns a used picture to the buffer manager for disposal.
 @param		"TVideoPicture* aFrameBuffer"
 			A used picture.
 @param		"TBool aSaveLast"
 			Save this picture as the last displayed (for redraw).
 @leave		May leave with KErrNotFound if the picture does not exist. 			
 */
void CMdfVideoDecoderBufferManager::ReturnFrameBufferL(TVideoPicture& aFrameBuffer, CVideoFrameBuffer::TFrameBufferState aState)
	{
	DEBUG_PRINT(_L("Return Frame Buffer from TVideoPicture"));

	TInt bufIndex;
	User::LeaveIfError(bufIndex = FindFrameBuffer(aFrameBuffer));
	// remove from the list of avaible buffers
	CVideoFrameBuffer* frameBuf = iFrameBuffersInUse[bufIndex];
	frameBuf->UnlockFrameBufferHeap();
	if (aState == CVideoFrameBuffer::EEmptied)
		{
		DEBUG_PRINT(_L("Added to empty pool"));
		iEmptyFrameBuffers.AppendL(frameBuf);
		}
	else
		{
		DEBUG_PRINT(_L("Added to filled pool"));
		iFilledFrameBuffers.AppendL(frameBuf);
		}
	iFrameBuffersInUse.Remove(bufIndex);
	
	}

void CMdfVideoDecoderBufferManager::ReturnFrameBufferL(CMMFBuffer* aBuffer, 
									CVideoFrameBuffer::TFrameBufferState aState,
									TBool aLastBuffer)
	{
	DEBUG_PRINT(_L("Return Frame buffer from CMMFBuffer"));
	TInt bufIndex;
	User::LeaveIfError(bufIndex = FindFrameBuffer(aBuffer));
	// remove from the list of avaible buffers
	CVideoFrameBuffer* frameBuf = iFrameBuffersInUse[bufIndex];
	frameBuf->SetLastBuffer(aLastBuffer);
	frameBuf->UnlockFrameBufferHeap();
	if (aState == CVideoFrameBuffer::EEmptied)
		{
		DEBUG_PRINT(_L("Added to empty pool"));
		iEmptyFrameBuffers.AppendL(frameBuf);
		}
	else
		{
		DEBUG_PRINT(_L("Added to filled pool"));
		iFilledFrameBuffers.AppendL(frameBuf);
		}
	iFrameBuffersInUse.Remove(bufIndex);
	
	}

// private methods

TInt CMdfVideoDecoderBufferManager::FindFrameBuffer(const TVideoPicture& aFrameBuffer) const
	{
	TInt theCount = iFrameBuffersInUse.Count();
	TInt found = KErrNotFound;
	for(TInt i = 0; i < theCount; i++) 
		{
		if(*iFrameBuffersInUse[i] == aFrameBuffer) 
			{
			found = i;
			break;
			}
		}
	return found;
	}


TInt CMdfVideoDecoderBufferManager::FindFrameBuffer(const CMMFBuffer* aBuffer) const
	{
	TInt theCount = iFrameBuffersInUse.Count();
	TInt found = KErrNotFound;
	for(TInt i = 0; i < theCount; i++) 
		{
		if(*iFrameBuffersInUse[i] == aBuffer) 
			{
			found = i;
			break;
			}
		}
	return found;
	}

// end
