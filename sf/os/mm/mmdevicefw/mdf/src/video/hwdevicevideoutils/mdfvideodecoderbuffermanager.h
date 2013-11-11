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

#ifndef MDFVIDEODECODERBUFFERMANAGER_H
#define MDFVIDEODECODERBUFFERMANAGER_H

#include <e32base.h>
#include <mmf/devvideo/videoplayhwdevice.h>
#include <mmf/devvideo/devvideoplay.h>
#include <mmf/server/mmfdatabuffer.h>

/**

*/
class CVideoDataBuffer : public CBase
	{
public:
	static CVideoDataBuffer* NewL(TUint aSize);
	static CVideoDataBuffer* NewL(CMMFBuffer* aBuffer);

	operator CMMFBuffer*();
	operator TVideoInputBuffer&();

	TBool operator ==(const CMMFBuffer* aBuffer) const;
	TBool operator ==(const TVideoInputBuffer& aVideoInputBuffer) const;
	
	TUint Length();

	~CVideoDataBuffer();
private:
	CVideoDataBuffer();
	
	void ConstructL(TUint aSize);
	void ConstructL(CMMFBuffer* aBuffer);

private:
	TUint iSize;
	CMMFBuffer* iMmfBuffer;
	TVideoInputBuffer iVideoInputBuffer;	
	};
	
/**

*/	
class CVideoFrameBuffer : public CBase
	{
public:
	enum TFrameBufferState
		{
		EEmptied,
		EFilled
		};
public:
	static CVideoFrameBuffer* NewL(TSize aFrameSize, TUncompressedVideoFormat aVideoFormat, TDisplayMode aDisplayMode);

	operator CMMFBuffer&();
	operator CMMFBuffer*();
	operator TVideoPicture&();
	operator TVideoPicture*();

	TBool operator ==(const CMMFBuffer* aBuffer) const;
	TBool operator ==(const TVideoPicture& aVideoPicture) const;
	
	void LockFrameBufferHeap();
	void UnlockFrameBufferHeap();
	void SetLastBuffer(TBool aLastBuffer);
	TBool LastBuffer();

	~CVideoFrameBuffer();
private:
	CVideoFrameBuffer(TSize aFrameSize, TUncompressedVideoFormat aVideoFormat, TDisplayMode aDisplayMode);
	
	void ConstructL();

	CMMFBuffer* iMmfBuffer;
	TVideoPicture iVideoPicture;
	CFbsBitmap* iBitmap;
	
	HBufC8* iRawBuffer;
	TPtr8 iRawPtr;
	
	TInt iFrameBufferSize; // still need to fill this in
	TSize iFrameSize;
	TUncompressedVideoFormat iVideoFormat;
	TDisplayMode iDisplayMode;
	TBool iHeapLocked;	
	TBool iLastBuffer;
	};
	
/**
Buffer manager for video decoder hardware device.
@internalComponent
*/
class CMdfVideoDecoderBufferManager : public CBase 
	{

public:
	static CMdfVideoDecoderBufferManager* NewL();
	~CMdfVideoDecoderBufferManager();

	void Init(const TUncompressedVideoFormat& aFormat);
	
	// data buffers (input)
	CVideoDataBuffer& CreateDataBufferL(TUint aSize);
	TInt DataBuffersAvailable() const;
	CVideoDataBuffer& GetDataBufferL();

	// for convenience, allow the return of the data buffer in all permutations
	void ReturnDataBufferL(TVideoInputBuffer& aBuffer);

	// frame buffer management
	CVideoFrameBuffer& CreateFrameBufferL(TDisplayMode aDisplayMode);
	
	CVideoFrameBuffer& GetEmptyFrameBufferL(TBool aLockHeap);
	CVideoFrameBuffer& GetFilledFrameBufferL(TBool aLockHeap);

	void ReturnFrameBufferL(TVideoPicture& aPicture, CVideoFrameBuffer::TFrameBufferState aState);
	void ReturnFrameBufferL(CMMFBuffer* aBuffer, 
							CVideoFrameBuffer::TFrameBufferState aState,
							TBool aLastBuffer);
	
	TInt EmptyFrameBuffersAvailable() const;
	TInt FilledFrameBuffersAvailable() const;
	
	const TPictureData& LastPictureL() const;
	
	// getters/setters for frame attributes
	void SetFrameSize(const TSize& aSize);
	void SetFrameBufferSize(TInt aSize);	
	const TSize& FrameSize() const;
	TInt FrameBufferSize() const;
	TInt MaxFrameBuffers() const;

private:
	CMdfVideoDecoderBufferManager();
	
	TInt FindFrameBuffer(const CMMFBuffer* aBuffer) const;
	TInt FindFrameBuffer(const TVideoPicture& aVideoPicture) const;
	TInt FindFrameBuffer(const CVideoFrameBuffer& aFrameBuffer) const;
	
	void ConstructL();
private:
	// the format of the incoming video
	TUncompressedVideoFormat iFormat;
	
	// input data buffers
	CVideoDataBuffer* iInputBuffer;
	TBool iInputBufferInUse;
	
	// output frame buffers
	RPointerArray<CVideoFrameBuffer> iEmptyFrameBuffers;
	RPointerArray<CVideoFrameBuffer> iFilledFrameBuffers;

	RPointerArray<CVideoFrameBuffer> iFrameBuffersInUse;

	TSize iFrameSize;
	TInt iFrameBufferSize;
	TInt iMaxFrameBuffers;
	
	// last displayed frame
	TPictureData iLastDisplayedFrame;
	
	};

#endif // MDFVIDEODECODERBUFFERMANAGER_H
