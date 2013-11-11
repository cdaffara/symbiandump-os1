// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @deprecated Emulator Deprecated
*/

#ifndef __DISPLAY_CHAN_H__
#define __DISPLAY_CHAN_H__

#include "nk_priv.h"
#include <dispchannel.h>
#include <videodriver.h>
#include "displayhandler.h"

struct TBufferAddressA;
class TScreenBuffer;
/**
Logical Channel kernel side class for Display
*/


class DDisplayChannel : public DLogicalChannel
	{
public:
	// constants
	enum { KDisplayChMajorVersionNumber = 1 };
	enum { KDisplayChMinorVersionNumber = 2 };
	enum { KDisplayChBuildVersionNumber = 1 };
public:
	// Duplicate of RDisplayChannel structure, as the header file cannot be used.
	class TCaps
    {
      public:
          TVersion iVersion;
    };

	class TRequest
		{
	public:
		inline TRequest(void) : iThread(0), iStatus(0) {}
		TBool SetStatus(TThreadMessage& aMsg);
		void Complete(TInt aResult);
	public:
		DThread* iThread;
		TRequestStatus* iStatus;
		};

	class TBufferInfo
		{
	public:
		TAny* iAddress;
		DChunk* iChunk;
		TRequest iRequest;
		};

	enum TDisplayPanic
		{
		EDisplayPanicNullThreadOnSet = 1,
		EDisplayPanicInUse = 2,
		EDisplayPanicThreadAlreadySet = 3,
		EDisplayPanicNullThreadOnComplete = 4,
		EDisplayPanicThreadOpenFailed = 5,
		EDisplayPanicWaitForPostMissed = 6,
		EDisplayPanicNullArgument = 7,
		EDisplayPanicNotYetImplemented = 8,
		EDisplayPanicInvalidDimensions = 9,
		EDisplayPanicInvalidRotation = 10,
		EDisplayPanicInvalidBitDepth = 11,
		EDisplayPanicInvalidStride = 12,
		EDisplayPanicInvalidWindowHandle = 13,
		EDisplayPanicInvalidFrameBuffers = 14,
		EDisplayPanicInvalidRefreshRate = 15,
		EDisplayPanicInvalidOffset = 16,
		EDisplayPanicNoLegacyBuffer = 17
		};

public:
	DDisplayChannel(void);
	~DDisplayChannel(void);

	// DLogicalChannel implementation
	virtual TInt DoCreate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);
	virtual void HandleMsg(TMessageBase* aMsg);

	// Function used by the UI code to set up the buffers.
	virtual TInt Initialize(RDisplayChannel::TDisplayInfo& aInfo, 
							RDisplayChannel::TDisplayRotation aRotation,
							HWND aHwnd, RPointerArray<TAny>& aFrameBuffers, 
							RPointerArray<TBufferAddressA>& aChunks,
							TScreenBuffer& aDsaBuffer,
							TSize aResolution, 
							TSize aTwips,
							const RDisplayChannel::TPixelFormat aPixelFormatArray[],
							const TInt aPixelFormatArraySize,
							const RDisplayChannel::TBufferFormat& aBufferFormat);
	virtual void SetLegacyBuffer(void *aAddress);

private:
	DDisplayChannel(const DDisplayChannel&);

	// Handlers for the three classes of channel message sent
	TInt DoControl(TInt aFunction);
	void DoRequest(TInt aFunction);
	TInt DoCancel(TInt aRequestMask);
	TInt NumberOfResolutions();
	TInt SafePut(TAny* aDst, TAny* aSrc, TInt aBytes);

	void ValidateSpecificInfo(TInt aBytesPerPixel, RDisplayChannel::TOrientationSpecificInfo& aInfo);
	static void Panic(TDisplayPanic aPanic);
	void ClientPanic(RDisplayChannel::TPanic aPanic);

	inline TBool IsCompositionBuffer(RDisplayChannel::TBufferId aId);
	static inline TBool IsLegacyBuffer(RDisplayChannel::TBufferId aId);
	static inline TBool IsUserBuffer(RDisplayChannel::TBufferId aId);
	inline TBool IsValidBuffer(RDisplayChannel::TBufferId aId);
    inline RDisplayChannel::TBufferId NextCompositionBuffer(RDisplayChannel::TBufferId aId);

	static inline TBool IsFlipped(RDisplayChannel::TDisplayRotation aRotation);

	static void VSyncTimerFn(TAny* aDisplayChannel);
	void DoVSyncTimer(void);
	static void VSyncDfcFn(TAny* aDisplayChannel);
	void DoVSync(void);

	TInt PostCompositionBuffer(TAny* aRegion, RDisplayChannel::TPostCount* aPostCount);
	TInt PostLegacyBuffer(TAny* aRegion, RDisplayChannel::TPostCount* aPostCount);
	TInt GetCompositionBuffer(TAny** aAddress);
	TInt PostUserBuffer(RDisplayChannel::TBufferId* aBufferId, TAny* aRegion, RDisplayChannel::TPostCount* aPostCount);
	TInt WaitForPost(RDisplayChannel::TPostCount* aPostCount);
	TInt WaitForDisplayConnect();
	TInt SetRotation(RDisplayChannel::TDisplayRotation* aNewRotation, TBool* aIsBufferPreserved);
	TInt RegisterUserBuffer(TInt aChunkHandle, TInt aOffset, RDisplayChannel::TBufferId* aBufferId);
	TInt DeregisterUserBuffer(RDisplayChannel::TBufferId* aBufferId);
	TInt GetResolutions();
	TInt SetResolution(TSize* aSize);
	TInt GetResolution(TSize* aSize);
	TInt GetTwips(TSize* aSize);
	TInt GetIndexForSize(const TSize& aSize,TInt& aSpinnerOut);
	TInt GetPixelFormats();
	TInt SetBufferFormat(RDisplayChannel::TBufferFormat* aBufferFormat);
	TInt NextPlaneOffset(RDisplayChannel::TBufferFormat* aBufferFormat, 
			            RDisplayChannel::TBufferFormatContext* aContext);
	TInt NextLineOffset(RDisplayChannel::TBufferFormat* aBufferFormat, 
			            RDisplayChannel::TBufferFormatContext* aContext);
	TInt ValidateBufferFormat(const RDisplayChannel::TBufferFormat& aBufferFormat,
						 	  const RDisplayChannel::TResolution& aResolution);
private:
	// Constants
	enum {KDfcPriority = 6};

	enum {KBufferNotSet = -1};

	// Buffer index points. The legacy buffer is first, followed by the user
	// buffers, followed by a variable number of composition buffers.
	enum { KLegacyBuffer = 0};
	enum { KFirstUserBuffer = KLegacyBuffer + 1};
	enum { KFirstCompositionBuffer = KFirstUserBuffer + RDisplayChannel::TDisplayInfo::KMaxUserBuffers};
	enum { KMaxBufferSizeHeightAndWidth = KMaxTInt16 };

private:
	TInt iScreenNumber;
	TThreadMessage* iMsg;
	TInt iVSyncTicks;					// Number of nanokernel ticks between frames
	TUint iNumCompositionBuffers;		// Number of composition buffers
	TUint iTotalBuffers;				// KFirstCompositionBuffer + iNumCompositionBuffers
	RDisplayChannel::TDisplayInfo iChannelInfo;
	RDisplayChannel::TPostCount iPostCount;				// Count of Post... calls
	RDisplayChannel::TPostCount iLastPostCount;			// Value of iPostCount when last buffer was actually posted
	TBufferInfo* iBuffer;				// Allocated to contain iTotalBuffers
	TBufferAddressA* iChunks;
	DThread* iClient;

	// Posting support
	NTimer iVSync;						// Emulated VSync signal using a timer
	TDfc iVSyncDfc;						// DFC queued on DfcQue0 when iVSync triggers
	RDisplayChannel::TBufferId iPostedBuffer;			// Index of buffer to be posted on next frame
	RDisplayChannel::TDisplayRotation iCurrentRotation;	// Rotation of buffer being posted
	RDisplayChannel::TDisplayRotation iNewRotation;	// Rotation of buffer being posted
	TSize iCurrentResolution;			// Display resolution (normal rotation) to be posted
	TSize iNewResolution;			    // Display resolution (normal rotation) for buffer being posted
	TSize iCurrentTwips;
	TInt iPostedRectCount;				// Number of rectangles defined in region
	RECT iPostedRect[RDisplayChannel::TDisplayInfo::KMaxRectangles];	//

	// Paint support
	HWND iHwnd;							// Window to be painted
	RDisplayChannel::TBufferId iDisplayBuffer;			// Index of buffer to be painted
	RDisplayChannel::TDisplayRotation iDisplayRotation;	// Rotation of buffer to be painted
	TSize iDisplayResolution;			// Display resolution (normal rotation) on screen

	// GetCompositionBuffer support
	TUint iGetBuffer;					// Index of next composition buffer

	// WaitForPost support
	RDisplayChannel::TPostCount iWaitForPost;			// Post count being awaited
	TRequest iWaitForPostRequest;		// Request to complete when post count reached
	
	// WaitForDisplayConnect support 
	TRequest iWaitForDisplayConnect;		// Request to complete when display connection state changes
	
	TInt iDisplayStateSpinner;
	
	TVersion iVersion;
	const RDisplayChannel::TPixelFormat* iPixelFormatArray;
	TInt iPixelFormatArraySize;
	RDisplayChannel::TBufferFormat iInitialBufferFormat;
	RDisplayChannel::TBufferFormat iCurrentBufferFormat;
	RDisplayChannel::TBufferFormat iDisplayBufferFormat;
	RDisplayChannel::TBufferFormat iNewBufferFormat;
	};

inline TBool DDisplayChannel::IsCompositionBuffer(RDisplayChannel::TBufferId aId)
	{ return (aId >= KFirstCompositionBuffer && aId < iTotalBuffers); }
inline TBool DDisplayChannel::IsLegacyBuffer(RDisplayChannel::TBufferId aId)
	{ return (aId == KLegacyBuffer); }
inline TBool DDisplayChannel::IsUserBuffer(RDisplayChannel::TBufferId aId)
	{ return (aId >= KFirstUserBuffer && aId < (KFirstUserBuffer + RDisplayChannel::TDisplayInfo::KMaxUserBuffers)); }
inline TBool DDisplayChannel::IsValidBuffer(RDisplayChannel::TBufferId aId)
	{ return (aId >= 0 && aId < iTotalBuffers); }
inline RDisplayChannel::TBufferId DDisplayChannel::NextCompositionBuffer(RDisplayChannel::TBufferId aId)
	{ TUint r = (aId + 1); return (r >= iTotalBuffers) ? KFirstCompositionBuffer : r; }

inline TBool DDisplayChannel::IsFlipped(RDisplayChannel::TDisplayRotation aRotation)
	{ return !(aRotation & (RDisplayChannel::ERotationNormal | RDisplayChannel::ERotation180)); }

#endif

