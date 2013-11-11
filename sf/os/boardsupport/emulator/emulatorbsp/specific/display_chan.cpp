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


#include <kernel/kern_priv.h>
#include <kernel/kernel.h>
#include <winsdef.h>
#include <emulator.h>
#include <pixelformats.h>
#include "gui.h"
#include "display_chan.h"

#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#include <windows.h>
#pragma warning( default : 4201 ) // nonstandard extension used : nameless struct/union

#ifndef DD_TRACE
#ifdef DD_TRACE_ENABLED
#define DD_TRACE(X) X
#else
#define DD_TRACE(X)
#endif
#endif



DDisplayChannel::DDisplayChannel() :
		iVSync(&DDisplayChannel::VSyncTimerFn, this),
		iVSyncDfc(&DDisplayChannel::VSyncDfcFn, this, Kern::DfcQue0(), KDfcPriority),
		iPostedBuffer((TUint)KBufferNotSet),
		iGetBuffer((TUint)KFirstCompositionBuffer),
		iDisplayStateSpinner(0),
		iVersion(KDisplayChMajorVersionNumber, 
			     KDisplayChMinorVersionNumber, 
			     KDisplayChBuildVersionNumber)
	{
	DD_TRACE(Kern::Printf("DDisplayChannel Creation");)
	}


DDisplayChannel::~DDisplayChannel()
	{
	DD_TRACE(Kern::Printf("DDisplayChannel Destruction");)
	//clean up.

	NKern::ThreadEnterCS();
	Kern::Free(iBuffer);
	NKern::ThreadLeaveCS();

	}


/**
  Second stage constructor called by the kernel's device driver framework.
  This is called in the context of the user thread (client) which requested the creation of a Logical Channel
  (E.g. through a call to RBusLogicalChannel::DoCreate)
  The thread is in a critical section.

  @param aUnit The unit argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aInfo The info argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aVer The version argument supplied by the client to RBusLogicalChannel::DoCreate

  @return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DDisplayChannel::DoCreate(TInt aUnit, const TDesC8* /*aInfo*/, const TVersion& aVer)
	{
	// Check version
	if (!Kern::QueryVersionSupported(iDevice->iVersion, aVer))
		{
		return KErrNotSupported;
		}

	iScreenNumber = aUnit;	/*aUnit?*/
	SetDfcQ(Kern::DfcQue0());
	iMsgQ.Receive();

	// Ask the GUI code to initialize the rest of the object.
	return Kern::HalFunction(EHalGroupEmulator, EEmulatorHalSetDisplayChannel,
			(TAny*)iScreenNumber, this);
	}


void DDisplayChannel::ValidateSpecificInfo(TInt aBytesPerPixel, RDisplayChannel::TOrientationSpecificInfo& aInfo)
	{
	__ASSERT_DEBUG(aInfo.iWidth > 0 && aInfo.iHeight > 0, Panic(EDisplayPanicInvalidDimensions));
	__ASSERT_DEBUG(aInfo.iOffsetBetweenLines > 0, Panic(EDisplayPanicInvalidStride));
	__ASSERT_DEBUG(aInfo.iWidth * aBytesPerPixel <= (TUint)aInfo.iOffsetBetweenLines, Panic(EDisplayPanicInvalidStride));
	
	//this is just to stop compiler complaining..
	(TAny)aBytesPerPixel;
	(TAny)aInfo;
	}

/**
Finish creating the display channel object.

@param aInfo			Display description object
@param aRotation		Initial rotation
@param aHwnd			Associated window handle
@param aFrameBuffers	Array of one or more pointers to frame buffers
@param aResolution		Initial resolution
@param aTwips           Size in twips
@return	KErrNone if successful, or a system-wide error otherwise.
*/
TInt DDisplayChannel::Initialize(RDisplayChannel::TDisplayInfo& aInfo, 
		                         RDisplayChannel::TDisplayRotation aRotation,
		                         HWND aHwnd, RPointerArray<TAny>& aFrameBuffers, 
		                         RPointerArray<TBufferAddressA>& aChunks,
								 TScreenBuffer& aDsaBuffer,
		                         TSize aResolution, 
		                         TSize aTwips,
		                         const RDisplayChannel::TPixelFormat aPixelFormatArray[],
								 const TInt aPixelFormatArraySize,
								 const RDisplayChannel::TBufferFormat& aBufferFormat)

	{
	__ASSERT_DEBUG(aInfo.iBitsPerPixel >= 12 && aInfo.iBitsPerPixel <= 32, Panic(EDisplayPanicInvalidBitDepth));
	__ASSERT_DEBUG(aInfo.iRefreshRateHz > 0, Panic(EDisplayPanicInvalidRefreshRate));
	__ASSERT_DEBUG(aInfo.iAvailableRotations & aRotation, Panic(EDisplayPanicInvalidRotation));
	__ASSERT_DEBUG(aInfo.iNormal.iWidth == aInfo.iFlipped.iHeight, Panic(EDisplayPanicInvalidDimensions));
	__ASSERT_DEBUG(aInfo.iNormal.iHeight == aInfo.iFlipped.iWidth, Panic(EDisplayPanicInvalidDimensions));
	__ASSERT_DEBUG(aRotation == RDisplayChannel::ERotationNormal ||
					aRotation == RDisplayChannel::ERotation90CW ||
					aRotation == RDisplayChannel::ERotation180 ||
					aRotation == RDisplayChannel::ERotation270CW, Panic(EDisplayPanicInvalidRotation));
	__ASSERT_DEBUG(aFrameBuffers.Count() > 0, Panic(EDisplayPanicInvalidFrameBuffers));
	__ASSERT_DEBUG(aHwnd, Panic(EDisplayPanicInvalidWindowHandle));

	ValidateSpecificInfo((aInfo.iBitsPerPixel > 16) ? 4 : 2, aInfo.iNormal);
	ValidateSpecificInfo((aInfo.iBitsPerPixel > 16) ? 4 : 2, aInfo.iFlipped);

	// aFrameBuffers includes the legacy buffer at index 0.
	iNumCompositionBuffers = aFrameBuffers.Count() - 1;
	iTotalBuffers = KFirstCompositionBuffer + iNumCompositionBuffers;
	iBuffer = (TBufferInfo*)Kern::AllocZ(iTotalBuffers * sizeof(TBufferInfo));
	iChunks = (TBufferAddressA*)Kern::AllocZ(iTotalBuffers * sizeof(TBufferAddressA));
	if (!iBuffer)
		{
		return KErrNoMemory;
		}

	aInfo.iNumCompositionBuffers = iNumCompositionBuffers;
	for (TUint index = 0; index < iNumCompositionBuffers; index++)
		{
		iBuffer[index + KFirstCompositionBuffer].iAddress = aFrameBuffers[index + 1];
		iChunks[index + KFirstCompositionBuffer].iChunk = aChunks[index + 1]->iChunk;
		}
		
	iChannelInfo = aInfo;
	
	iDisplayRotation = iCurrentRotation = aRotation;
	iDisplayResolution = iCurrentResolution = aResolution;
	iCurrentTwips = aTwips;
	iHwnd = aHwnd;
	
	// Ensure the VSync DFC is on the same queue as the message handling.
	iVSyncTicks = NKern::TimerTicks(1000 / aInfo.iRefreshRateHz);
    iVSync.OneShot(iVSyncTicks);

	if (aDsaBuffer.iMemChunks.Count() != 0)
		{	
		iBuffer[KLegacyBuffer].iAddress = aDsaBuffer.iFrameBuffers[0];
		}
	else
		{
		iBuffer[KLegacyBuffer].iAddress = NULL;
		}
    
	iPixelFormatArray = aPixelFormatArray;
	iPixelFormatArraySize = aPixelFormatArraySize;

	// copy the initial buffer format
	iInitialBufferFormat = aBufferFormat;
	iDisplayBufferFormat = iInitialBufferFormat;
	iCurrentBufferFormat = iDisplayBufferFormat;
	return KErrNone;
	}


/**
Set the address of the legacy buffer. 
 
@param aAddress			Pointer to the legacy buffer. 
*/
void DDisplayChannel::SetLegacyBuffer(void *aAddress)
	{ 
	iBuffer[KLegacyBuffer].iAddress = aAddress;
	}



/**
Handle a message on this display for the logical channel.

@param aMsg	The message to process. The iValue member of this distinguishes the
		message type:
		iValue == ECloseMsg, channel close message
		iValue == KMaxTInt, a 'DoCancel' message
		iValue >= 0, a 'DoControl' message with function number equal to iValue
		iValue < 0, a 'DoRequest' message with function number equal to ~iValue
*/
void DDisplayChannel::HandleMsg(TMessageBase* aMsg)
	{
	iMsg = (TThreadMessage*)aMsg;
	TInt id = iMsg->iValue;
	TInt r = KErrNone;

    if (id == (TInt)ECloseMsg)
       {
       iVSync.Cancel();
	   iWaitForPostRequest.Complete(KErrCancel);
	   iWaitForDisplayConnect.Complete(KErrCancel);
       if (iBuffer)
	       	{
    		// Unhook display channel from the GUI code if initialized.
    		if (iBuffer[KLegacyBuffer].iAddress)
	       		{
    		      	Kern::HalFunction(EHalGroupEmulator, EEmulatorHalSetDisplayChannel,
	   					(TAny*)iScreenNumber, NULL);
        	   		PostMessage(iHwnd, WMU_SET_DISPLAY_BUFFER, 0, 0);
		      	    InvalidateRect(iHwnd, NULL, FALSE);
    			}

	       	RDisplayChannel::TBufferId index = iTotalBuffers;

    		while (index-- > 0)
	       		{
		          	iBuffer[index].iRequest.Complete(KErrCancel);

        			if (IsUserBuffer(index))
		          		{
        				// Close chunk
		          		if (iBuffer[index].iChunk)
        					{
		              			NKern::ThreadEnterCS();
            					Kern::ChunkClose(iBuffer[index].iChunk);
            					NKern::ThreadLeaveCS();
        					}
				        }
			     }

            }
			iMsg->Complete(KErrNone, EFalse);
			return;
        }

	if (id < 0)
		{
		DoRequest(~id);
		r = KErrNone;
		}
	else
		{
		r = (id == KMaxTInt) ? DoCancel(iMsg->Int0()) : DoControl(id);
		}

	iMsg->Complete(r, (id != ECloseMsg));
	}


/**
Process synchronous 'control' requests. Parameters are in the first two
arguments of iMsg.
*/
TInt DDisplayChannel::DoControl(TInt aFunction)
	{
	DD_TRACE(Kern::Printf(">DDisplayChannel::DoControl fn=%d\n", aFunction);)

	TInt r = KErrNotSupported;
	TPckgBuf<RDisplayChannel::TDisplayInfo> pkg(iChannelInfo);
	switch(aFunction)
		{
		case RDisplayChannel::ECtrlGetDisplayInfo:
			// a1: TDisplayInfo* info [out]
			Kern::KUDesPut(*(TDes8*)iMsg->Ptr0(), pkg); ///*&iChannelInfo*/);
			r = KErrNone;
			break;

		case RDisplayChannel::ECtrlOpen:
			// Everything is done in Construct
			r = KErrNone;
			break;

		case RDisplayChannel::ECtrlClose:
			// Nothing to do?
			r = KErrNone;
			break;

		case RDisplayChannel::ECtrlPostCompositionBuffer:
			// a1: const TRegionFix<KMaxRectangles>* region [in]
			// a2: TPostCount* post count [out]
			r = PostCompositionBuffer(iMsg->Ptr0(), (RDisplayChannel::TPostCount*)iMsg->Ptr1());
			break;

		case RDisplayChannel::ECtrlPostLegacyBuffer:
			// a1: const TRegionFix<KMaxRectangles>* region [in]
			// a2: TPostCount* post count [out]
			r = PostLegacyBuffer(iMsg->Ptr0(), (RDisplayChannel::TPostCount*)iMsg->Ptr1());
			break;

		case RDisplayChannel::ECtrlRegisterUserBuffer:
			// a1: { TInt Chunk handle, TInt offset }* [in]
			// a2: TInt* buffer ID [out]
			{
			__ASSERT_DEBUG(iMsg->Ptr0(), Panic(EDisplayPanicNullArgument));
			TInt arg[2];
			kumemget(arg, iMsg->Ptr0(), sizeof(arg));
			r = RegisterUserBuffer(arg[0], arg[1], (RDisplayChannel::TBufferId*)iMsg->Ptr1());
			}
			break;

		case RDisplayChannel::ECtrlDeregisterUserBuffer:
			// a1: TBufferId* buffer ID [in]
			r = DeregisterUserBuffer((RDisplayChannel::TBufferId*)iMsg->Ptr0());
			break;

		case RDisplayChannel::ECtrlSetRotation:
			// a1: TDisplayRotation* new rotation [in]
			// a2: TBool* display config has changed [out]
			r = SetRotation((RDisplayChannel::TDisplayRotation*)iMsg->Ptr0(), (TBool*)iMsg->Ptr1());
			break;

		case RDisplayChannel::ECtrlCurrentRotation:
			// a1: TDisplayRotation* current rotation [out]
			r = SafePut(iMsg->Ptr0(), &iCurrentRotation, sizeof(iCurrentRotation));
			break;

		case ECloseMsg:
			r = KErrNone;
			break;

	    case RDisplayChannel::ECtrlGetCompositionBufferInfo:
    	    {
    	    TInt arg[2];
    	    TInt index = *((TInt*)(iMsg->Ptr0()));
            r = Kern::MakeHandleAndOpen(iMsg->Client(), iChunks[index + KFirstCompositionBuffer].iChunk);
            if(r >= 0)
                {
	            arg[0] = r;
	            arg[1] = 0;
				SafePut(iMsg->Ptr1(),&arg,(sizeof(TInt)*2));
                r = KErrNone;
                }
             }
    	    break;
    	 // added to v1.0
	    case RDisplayChannel::ECtrlVersion:
			r = SafePut(iMsg->Ptr0(), &iVersion, sizeof(iVersion));
	    	break;
    	//v1_1
	    case RDisplayChannel::ECtrlNumberOfResolutions:
	    	{
	    	r = NumberOfResolutions();
	    	}
	    	break;
	    case RDisplayChannel::ECtrlGetResolutions:
	    	{
	    	r = GetResolutions();
	    	if(r == KErrCorrupt) //Resolution list changed during GetResolutions
	    		{//second go
	    		r = GetResolutions();
	    		}
	    	}
	    	break;
	    case RDisplayChannel::ECtrlSetResolution:
	    	{
	    	r = SetResolution((TSize*)iMsg->Ptr0());
	    	}
	    	break;
	    case RDisplayChannel::ECtrlGetResolution:
	    	{
	    	r = GetResolution((TSize*)iMsg->Ptr0());
	    	}
	    	break;
	    case RDisplayChannel::ECtrlGetTwips:
	    	{
	    	r = GetTwips((TSize*)iMsg->Ptr0());
	    	}
	    	break;
	    case RDisplayChannel::ECtrlNumberOfPixelFormats:
			r = iPixelFormatArraySize;
	    	break;
	    case RDisplayChannel::ECtrlGetPixelFormats:
	    	r = GetPixelFormats();
	    	break;
	    case RDisplayChannel::ECtrlSetBufferFormat:
	    	r = SetBufferFormat((RDisplayChannel::TBufferFormat*)iMsg->Ptr0());
	    	break;
	    case RDisplayChannel::ECtrlGetBufferFormat:
			r = SafePut(iMsg->Ptr0(), &iCurrentBufferFormat, sizeof(iCurrentBufferFormat));
	    	break;
	    case RDisplayChannel::ECtrlNextPlaneOffset:
	    	// we support, for moment only packed pixel formats
	    	r = NextPlaneOffset((RDisplayChannel::TBufferFormat*)iMsg->Ptr0(), NULL);
	    	break;
	    case RDisplayChannel::ECtrlNextLineOffset:
	    	// use the internal current resolution and current rotation
	    	r = NextLineOffset((RDisplayChannel::TBufferFormat*)iMsg->Ptr0(), NULL);
	    	break;
	    case RDisplayChannel::ECtrlNextPlaneOffsetExtended:
	    	// we support, for moment only packed pixel formats
	    	r = NextPlaneOffset((RDisplayChannel::TBufferFormat*)iMsg->Ptr0(),
	    			            (RDisplayChannel::TBufferFormatContext*)iMsg->Ptr1());
	    	break;
	    case RDisplayChannel::ECtrlNextLineOffsetExtended:
	    	r = NextLineOffset((RDisplayChannel::TBufferFormat*)iMsg->Ptr0(),
	    			           (RDisplayChannel::TBufferFormatContext*)iMsg->Ptr1());
	    	break;
		}
	DD_TRACE(Kern::Printf("<DDisplayChannel::DoControl result=%d\n",r);)
	return r;
	}

TInt DDisplayChannel::GetIndexForSize(const TSize& aSize,TInt& aSpinnerOut)
	{
	TInt numberOfResolutions = 0;
	TInt spinner1 = 0;
	TInt r = Kern::HalFunction(EHalGroupDisplay, EDisplayHalNumberOfResolutions, &numberOfResolutions, &spinner1, iScreenNumber);
	if(r < KErrNone)
		return r;
	TVideoInfoV01 info;
	TPckg<TVideoInfoV01> infoPckg(info);

	for (TInt res=0;res<numberOfResolutions;res++)
		{
		//pass info package to be filled in, also sending the config to read, and the screen within that
		Kern::HalFunction(EHalGroupDisplay, EDisplayHalSpecificScreenInfo, (TAny*)&res, (TAny*)&infoPckg, iScreenNumber);
		if (info.iSizeInPixels.iWidth==aSize.iWidth && info.iSizeInPixels.iHeight==aSize.iHeight)
			{
			r = Kern::HalFunction(EHalGroupDisplay, EDisplayHalNumberOfResolutions, &numberOfResolutions, &aSpinnerOut, iScreenNumber);
			if(r < KErrNone)
				return r;
			if (aSpinnerOut!=spinner1)
				{
				return KErrCorrupt;
				}
			else
				{
				return res;
				}
			}
		}
	return KErrArgument;
	}

/**
Process asynchronous requests. TRequestStatus is at iMsg->Ptr0(), and the other
two parameters are in iMsg->Ptr1() and iMsg->Ptr2().
*/
void DDisplayChannel::DoRequest(TInt aFunction)
	{
	DD_TRACE(Kern::Printf(">DDisplayChannel::DoRequest fn=%d\n", aFunction);)

	TInt r = KErrNotSupported;

	switch(aFunction)
		{
		case RDisplayChannel::EReqGetCompositionBuffer:
			// a1: TAny** buffer address [out]
			r = GetCompositionBuffer(static_cast<TAny**>(iMsg->Ptr1()));
			break;

		case RDisplayChannel::EReqPostUserBuffer:
			// a1: { TBufferId* buffer ID, const TRegionFix<KMaxRectangles>* region }* [in]
			// a2: TPostCount* post count [out]
			{
			TAny* arg[2];
			__ASSERT_DEBUG(iMsg->Ptr1(), Panic(EDisplayPanicNullArgument));
			kumemget(arg, iMsg->Ptr1(), sizeof(arg));
			r = PostUserBuffer(reinterpret_cast<RDisplayChannel::TBufferId*>(arg[0]), arg[1],
					reinterpret_cast<RDisplayChannel::TPostCount*>(iMsg->Ptr2()));
			}
			break;

		case RDisplayChannel::EReqWaitForPost:
			// a1: TInt* post to wait for [in]
			r = WaitForPost(static_cast<RDisplayChannel::TPostCount*>(iMsg->Ptr1()));
			break;
	    	//v1_1
		case RDisplayChannel::EReqWaitForDisplayConnect:
			r = WaitForDisplayConnect();
			break;
		}

	DD_TRACE(Kern::Printf("<DDisplayChannel::DoRequest result=%d\n",r);)

	if (r != KRequestPending)
		{
		TRequestStatus* status = reinterpret_cast<TRequestStatus*>(iMsg->Ptr0());
		Kern::RequestComplete(iMsg->Client(), status, r);
		}
	}


/**
Process asynchronous request cancellations
*/
TInt DDisplayChannel::DoCancel(TInt aRequestMask)
	{
	DD_TRACE(Kern::Printf(">DDisplayChannel::DoCancel mask=%x\n", aRequestMask);)

	if (aRequestMask & (1 << RDisplayChannel::ECtrlCancelGetCompositionBuffer))
		{
		if (IsCompositionBuffer(iGetBuffer))
			{
			iBuffer[iGetBuffer].iRequest.Complete(KErrCancel);
			}
		}

	if (aRequestMask & (1 << RDisplayChannel::ECtrlCancelPostUserBuffer))
		{
		if (IsUserBuffer(iPostedBuffer))
			{
			iBuffer[iPostedBuffer].iRequest.Complete(KErrCancel);
			}
		}

	if (aRequestMask & (1 << RDisplayChannel::ECtrlCancelWaitForPost))
		{
		iWaitForPostRequest.Complete(KErrCancel);
		}
	if (aRequestMask & (1 << RDisplayChannel::ECtrlCancelWaitForDisplayConnect))
		{
		iWaitForDisplayConnect.Complete(KErrCancel);
		}

	DD_TRACE(Kern::Printf("<DDisplayChannel::DoCancel\n",r);)
	return KErrNone;
	}


/**
Get the number of resolutions on this screen
@return Number of resolutions if successful, otherwise a system wide error code.
*/
TInt DDisplayChannel::NumberOfResolutions()
	{
	TInt numberOfResolutions = 0;
	TInt error = Kern::HalFunction(EHalGroupDisplay, EDisplayHalNumberOfResolutions, &numberOfResolutions, NULL, iScreenNumber);
	if (error != KErrNone)
		{
		return error;
		}
	return numberOfResolutions;
	}


/**
Safely write from kernel to user memory.
@param aDst	Pointer to destination, user memory.
@param aSrc	Pointer to source, kernel memory.
@param aBytes	Number of bytes to write.
@return KErrNone if successful, KErrArgument if either pointer is NULL.
*/
TInt DDisplayChannel::SafePut(TAny* aDst, TAny* aSrc, TInt aBytes)
	{
	if (!aDst || !aSrc)
		{
		return KErrArgument;
		}

	kumemput(aDst, aSrc, aBytes);
	return KErrNone;
	}


/**
Driver panic.
@param	aPanic	The cause of the panic.
*/
void DDisplayChannel::Panic(TDisplayPanic aPanic)
	{
	Kern::Fault("DISPLAY", aPanic);
	}


/**
Client panic.
@param	aPanic	The cause of the panic.
*/
void DDisplayChannel::ClientPanic(RDisplayChannel::TPanic aPanic)
	{
	_LIT(KLitDisplayChannel, "DISPLAYCHANNEL");
	Kern::ThreadKill(iClient, EExitPanic, aPanic, KLitDisplayChannel);
	}


/**
VSync is emulated by using a nanokernel timer to call this function at the
frame rate.
*/
void DDisplayChannel::VSyncTimerFn(TAny* aDisplayChannel)
	{
	reinterpret_cast<DDisplayChannel*>(aDisplayChannel)->DoVSyncTimer();
	}

/**
Instance function called on the timer thread. Queues the VSyncDfc to run on its
thread.
*/
void DDisplayChannel::DoVSyncTimer(void)
	{
	iVSyncDfc.Add();
	iVSync.Again(iVSyncTicks);
	}


/**
DFC function to post the current buffer to the display.
@param aDisplayImpl	The display object to be posted.
*/
void DDisplayChannel::VSyncDfcFn(TAny* aDisplayChannel)
	{
	reinterpret_cast<DDisplayChannel*>(aDisplayChannel)->DoVSync();
	}


/**
Post the current buffer to the display. Signal waiting threads under various
conditions. This is run off the same DFC queue as message handling, so there is
no need to protect the fields.
*/
void DDisplayChannel::DoVSync(void)
	{
	if(iWaitForDisplayConnect.iStatus)
		{
		TInt currentSpinner;
		Kern::HalFunction(EHalGroupDisplay, EDisplayHalGetStateSpinner, &currentSpinner, NULL, iScreenNumber);
		//display state changed
		if(currentSpinner != iDisplayStateSpinner)
			iWaitForDisplayConnect.Complete(KErrNone);
		}
		
	if (IsValidBuffer(iPostedBuffer))
		{
		// Complete any outstanding request for the buffer which was displayed.
		// This either signifies the user or composition buffer becoming
		// available 
		if (iDisplayBuffer != iPostedBuffer)
			{
			iBuffer[iDisplayBuffer].iRequest.Complete(KErrNone);
			}

		iDisplayBuffer = (RDisplayChannel::TBufferId)iPostedBuffer;
		iPostedBuffer = (RDisplayChannel::TBufferId)KBufferNotSet;

		// Update the pixel pointer used when painting the client window		
		PostMessage(iHwnd, WMU_SET_DISPLAY_BUFFER, 0,
				(LPARAM)iBuffer[iDisplayBuffer].iAddress);

		// check if the buffer format is modified
	if ((iDisplayBufferFormat.iSize.iHeight != iNewBufferFormat.iSize.iHeight) || 
		(iDisplayBufferFormat.iSize.iWidth != iNewBufferFormat.iSize.iWidth) ||
		(iDisplayBufferFormat.iPixelFormat != iNewBufferFormat.iPixelFormat))
		{
		// We post in one messages everything, but we got to pack the size
		// speculate that the maximum width and height can be represented in 16 bit
		TUint aggregateSize = ((TUint)iNewBufferFormat.iSize.iHeight << 16) & 0xffff0000; 
		aggregateSize += (TUint)iNewBufferFormat.iSize.iWidth & 0x0000ffff;
		PostMessage(iHwnd, 
				    WMU_SET_BUFFER_FORMAT, 
				    aggregateSize, 
				    iNewBufferFormat.iPixelFormat);
		iDisplayBufferFormat = iNewBufferFormat;
		}
	
		if (iDisplayRotation != iNewRotation)
			{
			iDisplayRotation = iNewRotation;

			TUint flip;
			switch (iDisplayRotation)
				{
				case RDisplayChannel::ERotation90CW:
					flip = EEmulatorFlipLeft;
					break;
				case RDisplayChannel::ERotation180:
					flip = EEmulatorFlipInvert;
					break;
				case RDisplayChannel::ERotation270CW:
					flip = EEmulatorFlipRight;
					break;
				default:	// Normal
					flip = EEmulatorFlipRestore;
					break;
				}
			PostMessage(iHwnd, WM_FLIP_MESSAGE, flip, NULL);

			iPostedRectCount = 0;	// Force full invalidation
			}

		if (iDisplayResolution.iWidth != iNewResolution.iWidth ||
				iDisplayResolution.iHeight != iNewResolution.iHeight)
			{
			iDisplayResolution = iNewResolution;
			PostMessage(iHwnd, WMU_SET_DISPLAY_SIZE, 
					iDisplayResolution.iWidth, iDisplayResolution.iHeight);
			}
	
		
		// Invalidate the window contents where necessary
		TInt count = iPostedRectCount;
		if (count)
			{
			// Order of invalidation is immaterial
			while (count--)
				{
				InvalidateRect(iHwnd, &iPostedRect[count], FALSE);
				}

			iPostedRectCount = 0;
			}
		else
			{
			InvalidateRect(iHwnd, NULL, FALSE);
			}

		// Use differences to allow for wraparound
		if ((TInt)(iWaitForPost - iLastPostCount) > 0 && (TInt)(iPostCount - iWaitForPost) >= 0)
			{
			// Post waited for is now being displayed or was dropped
			iWaitForPostRequest.Complete(KErrNone);
			}
		iLastPostCount = iPostCount;
		}
	}


/**
Set the status object for this request and the thread to complete it on, if not
already set.
@param aStatus	The new request status.
@param aThread	The thread on which to complete.
@return EFalse if the status is already set, or ETrue if the status has now been
set.
*/
TBool DDisplayChannel::TRequest::SetStatus(TThreadMessage& aMsg)
	{
	if (iStatus)
		{
		__ASSERT_DEBUG(iThread, Kern::PanicCurrentThread(RDisplayChannel::Name(), EDisplayPanicNullThreadOnSet));
		return EFalse;	// In use
		}

	__ASSERT_DEBUG(!iThread, Kern::PanicCurrentThread(RDisplayChannel::Name(), EDisplayPanicThreadAlreadySet));
	DThread* thread = aMsg.Client();
	TInt r = thread->Open();
	__ASSERT_DEBUG( r == KErrNone, Kern::PanicCurrentThread(RDisplayChannel::Name(), EDisplayPanicThreadOpenFailed));
	(TAny)r;
	iThread = thread;
	iStatus = reinterpret_cast<TRequestStatus*>(aMsg.Ptr0());
	return ETrue;
	}


/**
Complete the request with the given result. If the status has not been set, or
has already been completed, this does nothing.
@param aResult	The result of the asynchronous request.
*/
void DDisplayChannel::TRequest::Complete(TInt aResult)
	{
	if (iStatus)
		{
    	__ASSERT_DEBUG(iThread, Kern::PanicCurrentThread(RDisplayChannel::Name(), EDisplayPanicNullThreadOnComplete));
		Kern::RequestComplete(iThread, iStatus, aResult);
		Kern::SafeClose((DObject*&)iThread, NULL);
		}
	}


/**
Post the current composition buffer to the display on the next frame tick.
@param aRegion	NULL if the entire buffer has changed, or a user pointer to up
to TDisplayInfo::KMaxRectagles areas that have changed.
@param aPostCount	User pointer to an integer to receive the new post count.
@return KErrNone, or a system-wide error code
*/
TInt DDisplayChannel::PostCompositionBuffer(TAny* aRegion, RDisplayChannel::TPostCount* aPostCount)
	{
	if (!IsCompositionBuffer(iGetBuffer))
		{
		return KErrNotSupported;
		}

	if (iWaitForPost == iPostCount)
		{
		// Complete wait for post (dropped)
		iWaitForPostRequest.Complete(KErrNone);
		}

	if (IsUserBuffer(iPostedBuffer))
		{
		// Complete the user post request (not displayed)
		iBuffer[iPostedBuffer].iRequest.Complete(KErrCancel);
		}

	iPostedBuffer = iGetBuffer;
	iGetBuffer = NextCompositionBuffer(iGetBuffer);

	if (iNumCompositionBuffers > 2 && iGetBuffer == iDisplayBuffer)
		{
		// With more than two buffers, there must always be one available with
		// no waiting required, so find it.
		iGetBuffer = NextCompositionBuffer(iGetBuffer);
		}

	// Get the region
	if (aRegion)
		{
		// Set iPostedRect(Count) from aRegion.
		Panic(EDisplayPanicNotYetImplemented);
		}

	// What to do about wrapping?
	iPostCount++;
	iNewRotation = iCurrentRotation;
	iNewResolution = iCurrentResolution;
	iNewBufferFormat = iCurrentBufferFormat;
	SafePut(aPostCount, &iPostCount, sizeof(iPostCount));

	return KErrNone;
	}


/**
Post the legacy buffer to the display on the next frame tick.
@param aRegion	NULL if the entire buffer has changed, or a user pointer to up
to TDisplayInfo::KMaxRectagles areas that have changed.
@param aPostCount	User pointer to an integer to receive the new post count.
@return KErrNone, or a system-wide error code
*/
TInt DDisplayChannel::PostLegacyBuffer(TAny* aRegion, RDisplayChannel::TPostCount* aPostCount)
	{
	if (iWaitForPost == iPostCount)
		{
		// Complete wait for post (dropped)
		iWaitForPostRequest.Complete(KErrNone);
		}

	if (IsUserBuffer(iPostedBuffer))
		{
		iBuffer[iPostedBuffer].iRequest.Complete(KErrCancel);
		}

	// iBuffer should NOT be NULL here!
	__ASSERT_ALWAYS(iBuffer[KLegacyBuffer].iAddress, Panic(EDisplayPanicNoLegacyBuffer));
	iPostedBuffer = KLegacyBuffer;

	// Get the region into iRegion
	if (aRegion)
		{
		// Set iPostedRect(Count) from aRegion.
		Panic(EDisplayPanicNotYetImplemented);
		}

	iPostCount++;
	iNewRotation = iCurrentRotation;
	iNewResolution = iCurrentResolution;
	iNewBufferFormat = iCurrentBufferFormat;
	SafePut(aPostCount, &iPostCount, sizeof(iPostCount));

	return KErrNone;
	}


/**
Asynchronously request the current composition buffer. Completes immediately
if not already being displayed, or an error occurs.
@param aStatus	The request status to be completed.
@param aAddress	The user pointer to the location to put the address.
*/
TInt DDisplayChannel::GetCompositionBuffer(TAny** aAddress)
	{
	if (!IsCompositionBuffer(iGetBuffer))
		{
		// No composition buffers available for use.
		return KErrNotSupported;
		}

	// The address won't change, so may as well set it now.
	TUint idx = iGetBuffer - KFirstCompositionBuffer;
	if (SafePut(aAddress, &idx, sizeof(TUint)) != KErrNone)
		{
		return KErrArgument;
		}

	if (iNumCompositionBuffers > 1 && iGetBuffer == iDisplayBuffer)
		{
		// Multi-buffer case, and buffer is currently being displayed

		if (iBuffer[iGetBuffer].iRequest.SetStatus(*iMsg))
			{
			return KRequestPending;
			}

		// Already set
		return KErrInUse;
		}

	return KErrNone;
	}


/**
Post a user buffer to the display on the next frame tick.
@param aBufferId	The ID of the user buffer to post.
@param aRegion	NULL if the entire buffer has changed, or a user pointer to up
to TDisplayInfo::KMaxRectagles areas that have changed.
@return KErrNone, or a system-wide error code
*/
TInt DDisplayChannel::PostUserBuffer(RDisplayChannel::TBufferId* aBufferId, TAny* aRegion, RDisplayChannel::TPostCount* aPostCount)
	{
	RDisplayChannel::TBufferId bufferId;
	bufferId = reinterpret_cast <RDisplayChannel::TBufferId> (aBufferId);

	if (!IsUserBuffer(bufferId))
		{
		// Not a user buffer.
		return KErrArgument;
		}

	if (!iBuffer[bufferId].iChunk)
		{
		// User buffer not initialised.
		return KErrArgument;
		}

	if (iWaitForPost == iPostCount)
		{
		// Complete wait for post (dropped)
		iWaitForPostRequest.Complete(KErrNone);
		}

	if (IsUserBuffer(iPostedBuffer))
		{
		// Complete the user post request (not displayed)
		iBuffer[iPostedBuffer].iRequest.Complete(KErrCancel);
		}

	if (bufferId == iDisplayBuffer)	//pathological case
		{
		// Complete the current display buffer as we are superceding it with the same one
		iBuffer[bufferId].iRequest.Complete(KErrNone);
		}

	// Only one buffer can be posted at any time, and if it were this user
	// buffer, the request will have just been completed, so this shouldn't fail.
	TBool isSet = iBuffer[bufferId].iRequest.SetStatus(*iMsg);

	__ASSERT_DEBUG(isSet, Panic(EDisplayPanicInUse));
	(TAny)isSet;
	iPostedBuffer = bufferId;

	// Get the region
	if (aRegion)
		{
		// Set iPostedRect(Count) from aRegion.
		Panic(EDisplayPanicNotYetImplemented);
		}

	iPostCount++;
	iNewRotation = iCurrentRotation;
	iNewResolution = iCurrentResolution;
	iNewBufferFormat = iCurrentBufferFormat;
	SafePut(aPostCount, &iPostCount, sizeof(iPostCount));

	return KRequestPending;
	}

/**
Asynchronous request notification when the given post count is reached (or
passed).
@param aStatus	The request status to be completed.
@param aPostCount	The count to wait for.
*/
TInt DDisplayChannel::WaitForPost(RDisplayChannel::TPostCount* aPostCount)
	{
	TInt postCount;

	kumemget(&postCount, aPostCount, sizeof(RDisplayChannel::TPostCount));

	if ((TInt)(iWaitForPost - iLastPostCount) > 0 && (TInt)(iPostCount - iWaitForPost) >= 0)
		{
		// Set up the request to be completed when the post occurs
		if (iWaitForPostRequest.SetStatus(*iMsg))
			{
			iWaitForPost = postCount;
			return KRequestPending;
			}

		// Already waiting for a post
		return KErrInUse;
		}

	// Requested post already displayed/dropped
	return KErrNone;
	}

/**
Asynchronous request notification when the display connection state changes.
This occurs when the display is disconnected, connected with no list, or a list of modes becomes available or is updated.
*/
TInt DDisplayChannel::WaitForDisplayConnect()
	{
	
	Kern::HalFunction(EHalGroupDisplay, EDisplayHalGetStateSpinner, &iDisplayStateSpinner, NULL, iScreenNumber);
	
	if (iWaitForDisplayConnect.SetStatus(*iMsg))
		{
		return KRequestPending;
		}

	// Already waiting for a post
	return KErrInUse;
	}


/**
Set the rotation of the screen on the next frame. If the buffer contents will
not be valid, the flag set. The validity of the buffer is down to whether the
width and height change.

@param aNewRotation	Address in user space of the new rotation setting.
@param aDisplayConfigChanged	Address in user space of where to put whether the
orientation specific info to use has changed following the rotation setting.
@return KErrNone, or KErrArgument if an argument was invalid.
*/
TInt DDisplayChannel::SetRotation(RDisplayChannel::TDisplayRotation* aNewRotation,
		TBool* aDisplayConfigChanged)
	{
	RDisplayChannel::TDisplayRotation newRotation;

	__ASSERT_DEBUG(aNewRotation, Panic(EDisplayPanicNullArgument));

	kumemget(&newRotation, aNewRotation, sizeof(newRotation));

	if (((TInt)newRotation - 1) & newRotation)
		{
		// More than one bit is set, which is not valid
		return KErrArgument;
		}

	if ((iChannelInfo.iAvailableRotations & newRotation) == 0)
		{
		// Rotation is not supported
		return KErrArgument;
		}

	TBool displayConfigChanged = (IsFlipped(newRotation) != IsFlipped(iCurrentRotation));
	iCurrentRotation = newRotation;

	SafePut(aDisplayConfigChanged, &displayConfigChanged, sizeof(TBool));

	return KErrNone;
	}


/**
Register a user buffer. Assign an ID for it (if available) and open the chunk to
get the address.
@param aChunkHandle	The chunk handle.
@param aOffset		The offset from the chunk base address to the start of the
buffer.
@param aBufferId	The address in user space of where to put the buffer ID.
*/
TInt DDisplayChannel::RegisterUserBuffer(TInt aChunkHandle, TInt aOffset,
		RDisplayChannel::TBufferId* aBufferId)
	{
	if (!aBufferId)
		{
		return KErrArgument;
		}

	NKern::ThreadEnterCS();
	DChunk* chunk = Kern::OpenSharedChunk(iMsg->Client(), aChunkHandle,
			EFalse);
	NKern::ThreadLeaveCS();
	if (!chunk)
		{
		return KErrBadHandle;
		}

	TLinAddr kernelAddress;

	const TInt bufferSize = (iDisplayResolution.iWidth * iDisplayResolution.iHeight);
	TInt r = Kern::ChunkAddress(chunk,aOffset,bufferSize,kernelAddress);
	if(r!=KErrNone)
		{
		NKern::ThreadEnterCS();
		Kern::ChunkClose(chunk);
		NKern::ThreadLeaveCS();
		return r;
		}

	// Search for an empty slot
	for (TInt index = KFirstUserBuffer; index < (KFirstUserBuffer + RDisplayChannel::TDisplayInfo::KMaxUserBuffers); index++)
		{
		if (!iBuffer[index].iChunk)
			{
			// Found one, so fill in the details and return the index as the ID.
			iBuffer[index].iChunk = chunk;
			iBuffer[index].iAddress = (TAny*)(kernelAddress);
			kumemput(aBufferId, &index, sizeof(RDisplayChannel::TBufferId));
			return KErrNone;
			}
		}

	// No slots available.
	NKern::ThreadEnterCS();
	Kern::ChunkClose(chunk);
	NKern::ThreadLeaveCS();
	return KErrTooBig;
	}


/**
Deregister a user buffer.
@param aBufferId	The buffer ID.
*/
TInt DDisplayChannel::DeregisterUserBuffer(RDisplayChannel::TBufferId* aBufferId)
	{
	RDisplayChannel::TBufferId bufferId;
	
	__ASSERT_DEBUG(aBufferId, Panic(EDisplayPanicNullArgument));
	kumemget(&bufferId, aBufferId, sizeof(RDisplayChannel::TBufferId));

	if (!IsUserBuffer(bufferId))
		{
		// Not a valid ID
		return KErrArgument;
		}

	TBufferInfo* buffer = &iBuffer[bufferId];
	if (!buffer->iChunk)
		{
		// Not registered
		return KErrArgument;
		}

	if (iDisplayBuffer == bufferId)
		{
		return KErrInUse;
		}

	if (iPostedBuffer == bufferId)
		{
		// Was queued to be posted
		iPostedBuffer = (RDisplayChannel::TBufferId)KBufferNotSet;
		iPostedRectCount = 0;
		}

	// Cancel any outstanding request on the buffer and clear out the fields.
	buffer->iRequest.Complete(KErrCancel);
	NKern::ThreadEnterCS();
	Kern::ChunkClose(buffer->iChunk);
	NKern::ThreadLeaveCS();
	buffer->iChunk = NULL;
	buffer->iAddress = NULL;

	return KErrNone;
	}

/**
Get all resolutions available for this screen and insert to a descriptor
@return KErrNone if successful, otherwise a system wide error code.
*/
TInt DDisplayChannel::GetResolutions()
	{
	TInt r;
	__ASSERT_DEBUG(iMsg->Ptr0(), Panic(EDisplayPanicNullArgument));
	__ASSERT_DEBUG(iMsg->Ptr1(), Panic(EDisplayPanicNullArgument));
	//get the number of resolutions (as above)
	TInt numberOfResolutions = 0;
	TInt refStateSpinner, curStateSpinner;
	//Get the number of resolutions as well as the display state spinner at this point
	r = Kern::HalFunction(EHalGroupDisplay, EDisplayHalNumberOfResolutions, &numberOfResolutions, &refStateSpinner, iScreenNumber);
	if(r < KErrNone)
		{
		return r;
		}
		    	
	TInt length;
	TInt maxLength;
	Kern::KUDesInfo(*(const TDesC*)iMsg->Ptr0(),length,maxLength);
		    	
	if ((maxLength/static_cast<TInt>(sizeof(RDisplayChannel::TResolution))) < numberOfResolutions)
		{
		return KErrOverflow;
		}
		    	
	TVideoInfoV01 info;
	TPckg<TVideoInfoV01> infoPckg(info);
	RDisplayChannel::TResolution resolution = {{0,0},{0,0},RDisplayChannel::ERotationAll};
	TPtr8 tempDes(NULL,0);

	TInt i;
	for (i=0;i<numberOfResolutions;i++)
		{
		//before filling the package, the display state needs to be checked for consistency
		Kern::HalFunction(EHalGroupDisplay, EDisplayHalGetStateSpinner, &curStateSpinner, NULL, iScreenNumber);
		    			    		
		if(curStateSpinner != refStateSpinner)
			{ //display state has changed, the resolution list we trying to get becomes corrupted
			Kern::KUDesSetLength(*(TDes8*)iMsg->Ptr0(),(i)*sizeof(RDisplayChannel::TResolution));
			return KErrCorrupt;
			}

		//pass info package to be filled in, also sending the config to read, and the screen within that
		Kern::HalFunction(EHalGroupDisplay, EDisplayHalSpecificScreenInfo, &i, &infoPckg, iScreenNumber);
		//save resolution
		resolution.iPixelSize = info.iSizeInPixels;
		resolution.iTwipsSize = info.iSizeInTwips;
		//copy resolution
  		tempDes.Set((TUint8*)&resolution,sizeof(RDisplayChannel::TResolution),sizeof(RDisplayChannel::TResolution));
   		r = Kern::ThreadDesWrite(iMsg->Client(),(TDes8*)iMsg->Ptr0(),tempDes,i*sizeof(RDisplayChannel::TResolution),iMsg->Client());
		    		
   		if (r<KErrNone)
   			{
   			Kern::KUDesSetLength(*(TDes8*)iMsg->Ptr0(),(i)*sizeof(RDisplayChannel::TResolution));
   			return r;
   			}
   		}
	Kern::KUDesSetLength(*(TDes8*)iMsg->Ptr0(),(i)*sizeof(RDisplayChannel::TResolution));
   	SafePut(iMsg->Ptr1(), &numberOfResolutions, sizeof(numberOfResolutions));
   	return KErrNone;
	}



TInt DDisplayChannel::SetResolution(TSize* aSize)
	{
	if (!aSize)
		{
		ClientPanic(RDisplayChannel::ENullArgument);
		return KErrArgument;
		}

	TSize size;
	kumemget32(&size, aSize, sizeof(size));
	if (size.iWidth < 0 || size.iHeight < 0)
		{
		ClientPanic(RDisplayChannel::EInvalidResolution);
		return KErrArgument;
		}
	
	if (!Kern::CurrentThreadHasCapability(ECapabilityWriteDeviceData, 
			__PLATSEC_DIAGNOSTIC_STRING("Checked by DISPLAY0.LDD (display channel driver)")))
		{
		return KErrPermissionDenied;
		}

	// Validate size. Return KErrArgument on failure.
	//Get the number of resolutions
	TInt numberOfResolutions;
	TInt r = Kern::HalFunction(EHalGroupDisplay, EDisplayHalNumberOfResolutions, &numberOfResolutions, NULL, iScreenNumber);
	if(r < KErrNone)
		{
		return r;
		}
	TVideoInfoV01 info;
	TPckg<TVideoInfoV01> infoPckg(info);
	for (TInt i = 0; i < numberOfResolutions; i++)
		{
		//pass info package to be filled in, also sending the config to read, and the screen within that
		Kern::HalFunction(EHalGroupDisplay, EDisplayHalSpecificScreenInfo, &i, &infoPckg, iScreenNumber);
		
		if (info.iSizeInPixels.iWidth == size.iWidth &&
				info.iSizeInPixels.iHeight == size.iHeight)
			{	//matched resolution
			iCurrentResolution = size;
			iCurrentTwips = info.iSizeInTwips;
			
			if (iCurrentResolution.iHeight > iNewBufferFormat.iSize.iHeight ||
				iCurrentResolution.iWidth > iNewBufferFormat.iSize.iWidth)
				{
				// going back to initial settings and, we hope, 
				// the buffers could still be displayed correctly, but we have no guarantee
				iCurrentBufferFormat = iInitialBufferFormat;
				}
			return KErrNone;
			}
   		}
	return KErrArgument;	//if reached here, it did not match aSize to any config resolution
	}

TInt DDisplayChannel::GetResolution(TSize* aSize)
	{
	TInt numberOfResolutions;
	TInt r = Kern::HalFunction(EHalGroupDisplay, EDisplayHalNumberOfResolutions,
			&numberOfResolutions, NULL, iScreenNumber);

	if (r == KErrNone)
		{
		if (numberOfResolutions > 0)
			{
		   	SafePut(aSize, &iCurrentResolution, sizeof(iCurrentResolution));
			}
		else
			{
			TSize resolution = {0,0};
		   	SafePut(aSize, &resolution, sizeof(resolution));
			}
		}
	return r;
	}

TInt DDisplayChannel::GetTwips(TSize* aSize)
	{
	TInt numberOfResolutions;
	TInt r = Kern::HalFunction(EHalGroupDisplay, EDisplayHalNumberOfResolutions,
			&numberOfResolutions, NULL, iScreenNumber);

	if (r == KErrNone)
		{
		if (numberOfResolutions > 0)
			{
		   	SafePut(aSize, &iCurrentTwips, sizeof(iCurrentTwips));
			}
		else
			{
			TSize twips = {0,0};
		   	SafePut(aSize, &twips, sizeof(twips));
			}
		}
	return r;
	}
/**
Get all the pixel formats available and insert them to a descriptor
@return KErrNone if successful, otherwise a system wide error code.
*/
TInt DDisplayChannel::GetPixelFormats()
	{
	TInt r;
	__ASSERT_DEBUG(iMsg->Ptr0(), Panic(EDisplayPanicNullArgument));
	__ASSERT_DEBUG(iMsg->Ptr1(), Panic(EDisplayPanicNullArgument));
	//get the number of resolutions (as above)
	TPtr8 pixelFormatDes(NULL,0);
	TInt length = ((TInt)sizeof(RDisplayChannel::TPixelFormat)) * iPixelFormatArraySize;
	pixelFormatDes.Set((TUint8*)iPixelFormatArray, length, length);
	r = Kern::ThreadDesWrite(iMsg->Client(),(TDes8*)iMsg->Ptr0(), pixelFormatDes, 0, iMsg->Client());
	if (r == KErrNone)
		{
		Kern::KUDesSetLength(*(TDes8*)iMsg->Ptr0(), length);
		SafePut(iMsg->Ptr1(), &iPixelFormatArraySize, sizeof(iPixelFormatArraySize));
		}
	return r;
	}


TInt DDisplayChannel::SetBufferFormat(RDisplayChannel::TBufferFormat* aBufferFormat)
	{
	if (!aBufferFormat)
		{
		ClientPanic(RDisplayChannel::ENullArgument);
		return KErrArgument;
		}

	if (!Kern::CurrentThreadHasCapability(ECapabilityWriteDeviceData, 
			__PLATSEC_DIAGNOSTIC_STRING("Checked by DISPLAY0.LDD (display channel driver)")))
		{
		return KErrPermissionDenied;
		}

	RDisplayChannel::TBufferFormat bufferFormat;
	kumemget32(&bufferFormat, aBufferFormat, sizeof(RDisplayChannel::TBufferFormat));
	
	// Validate Size
	if (iCurrentResolution.iHeight > bufferFormat.iSize.iHeight ||
		iCurrentResolution.iWidth > bufferFormat.iSize.iWidth ||
		bufferFormat.iSize.iHeight > KMaxBufferSizeHeightAndWidth ||
		bufferFormat.iSize.iWidth > KMaxBufferSizeHeightAndWidth)
		{
		//return error on failure
		return KErrArgument;
		}
	
	// check we received one of the supported formats.
	for (TInt i = 0; i < iPixelFormatArraySize; i++)
		{
		if (bufferFormat.iPixelFormat == iPixelFormatArray[i])
			{
			// the arguments are all validated at this point, update the current format
			iCurrentBufferFormat = bufferFormat;
			return KErrNone;
			}
		}
	
		//return error on failure
		return KErrArgument;
	}

TInt DDisplayChannel::ValidateBufferFormat(const RDisplayChannel::TBufferFormat& aBufferFormat,
									 	   const RDisplayChannel::TResolution& aResolution)
	{
	// Validate the size reported in buffer format against the given resolution
	if (aResolution.iPixelSize.iHeight > aBufferFormat.iSize.iHeight ||
		aResolution.iPixelSize.iWidth > aBufferFormat.iSize.iWidth ||
		aBufferFormat.iSize.iHeight > KMaxBufferSizeHeightAndWidth ||
		aBufferFormat.iSize.iWidth > KMaxBufferSizeHeightAndWidth)
		{
		//return error on failure
		return KErrArgument;
		}
	
	// check we received one of the supported formats.
	for (TInt i = 0; i < iPixelFormatArraySize; i++)
		{
		if (aBufferFormat.iPixelFormat == iPixelFormatArray[i])
			{
			return KErrNone;
			}
		}
		
	return KErrArgument;
	}

TInt DDisplayChannel::NextPlaneOffset(RDisplayChannel::TBufferFormat* aBufferFormat, 
		            				 RDisplayChannel::TBufferFormatContext* aContext)
	{
	if (!aBufferFormat)
		{
		ClientPanic(RDisplayChannel::ENullArgument);
		return KErrArgument;
		}

	if (!Kern::CurrentThreadHasCapability(ECapabilityWriteDeviceData, 
			__PLATSEC_DIAGNOSTIC_STRING("Checked by DISPLAY0.LDD (display channel driver)")))
		{
		return KErrPermissionDenied;
		}

	RDisplayChannel::TBufferFormat bufferFormat;
	kumemget32(&bufferFormat, aBufferFormat, sizeof(bufferFormat));
	
	RDisplayChannel::TResolution resolution;
	RDisplayChannel::TDisplayRotation rotation;
	
	if (aContext)
		{
		RDisplayChannel::TBufferFormatContext context;
		kumemget32(&context, aContext, sizeof(context));
		resolution = context.iResolution;
		rotation = context.iRotation;
		}
	else
		{
		resolution.iPixelSize = iCurrentResolution;
		rotation = iCurrentRotation;
		}
	
		TInt err = ValidateBufferFormat(bufferFormat, resolution);
		
		if (err != KErrNone)
			{
			return err;
			}
		//it assumes no planar pixel formats are supported by this driver implementation
		return 0;
	}

TInt DDisplayChannel::NextLineOffset(RDisplayChannel::TBufferFormat* aBufferFormat, 
		            				 RDisplayChannel::TBufferFormatContext* aContext)
	{
	if (!aBufferFormat)
		{
		ClientPanic(RDisplayChannel::ENullArgument);
		return KErrArgument;
		}

	if (!Kern::CurrentThreadHasCapability(ECapabilityWriteDeviceData, 
			__PLATSEC_DIAGNOSTIC_STRING("Checked by DISPLAY0.LDD (display channel driver)")))
		{
		return KErrPermissionDenied;
		}

	RDisplayChannel::TBufferFormat bufferFormat;
	kumemget32(&bufferFormat, aBufferFormat, sizeof(bufferFormat));
	
	RDisplayChannel::TResolution resolution;
	RDisplayChannel::TDisplayRotation rotation;
	
	if (aContext)
		{
		RDisplayChannel::TBufferFormatContext context;
		kumemget32(&context, aContext, sizeof(context));
		resolution = context.iResolution;
		rotation = context.iRotation;
		}
	else
		{
		resolution.iPixelSize = iCurrentResolution;
		rotation = iCurrentRotation;
		}

	TInt err = ValidateBufferFormat(bufferFormat, resolution);
	
	if (err != KErrNone)
		{
		return err;
		}
	
	TInt bpp = 0;
	//validating the pixel format and getting the pixel size in bits
	switch (bufferFormat.iPixelFormat)
		{
		case EUidPixelFormatXRGB_4444:	// RGB4444
		case EUidPixelFormatARGB_4444:
		case EUidPixelFormatRGB_565:	// RGB565
			bpp = 16;
			break;
		case EUidPixelFormatXRGB_8888:	// Really 32bpp, but top 8 unused
		case EUidPixelFormatARGB_8888:
		case EUidPixelFormatARGB_8888_PRE:
			bpp = 32;
			break;
		default:
			// We got an error, it seems. The pixel format is not supported 
			// Let's panic because the pixel format has just been validated
			Panic(EDisplayPanicInvalidBitDepth);
			break;
		}
		
	TInt widthInPixel = 0;	// pixels
		
	// let's take in consideration the given rotation
	switch (rotation)
		{
		case RDisplayChannel::ERotation90CW:
		case RDisplayChannel::ERotation270CW:
			widthInPixel = bufferFormat.iSize.iHeight;
			break;
		default:	// Normal
			widthInPixel = bufferFormat.iSize.iWidth;
			break;
		}
	
	// we have to round up to 32 bits word. This is a Ms Windows limitation 
	TInt stride = _ALIGN_UP((widthInPixel * bpp), 32) >> 3;
	return stride;
	}
