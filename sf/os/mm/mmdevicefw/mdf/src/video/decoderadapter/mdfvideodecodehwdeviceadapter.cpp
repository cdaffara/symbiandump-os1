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

#include <mmf/devvideo/devvideopuconfig.h>
#include "mdfvideodecodehwdeviceadapter.h"
#include "displaymodeutils.h"

#if defined (SYMBIAN_MDFVIDEODECODERHWDEVICE_DEBUG)
#define DEBUG_PRINT RDebug::Print
#else
#define DEBUG_PRINT
#endif // defined (SYMBIAN_MDFVIDEODECODERHWDEVICE_DEBUG)

const TInt KOutputFormatsArraySize = 3;
const TInt KTenFPS = 10;
const TInt KOneSecond = 1000000;
const TInt KInputBufferSize = 8192;

_LIT(KDevVideoHwDeviceDecoderPanicCategory, "DevVideoHwDeviceDecoder");
void DevVideoHwDeviceDecoderPanic(TInt aReason)
	{
	User::Panic(KDevVideoHwDeviceDecoderPanicCategory, aReason);
	}
	
/* 
 Constructs a new instance of CMdfVideoDecodeHwDeviceAdapter.
 @return    "CMdfVideoDecodeHwDeviceAdapter*"
            A pointer to the newly constructed HwDevice
 */
CMdfVideoDecodeHwDeviceAdapter* CMdfVideoDecodeHwDeviceAdapter::NewL()
	{
	CMdfVideoDecodeHwDeviceAdapter* self = new (ELeave) CMdfVideoDecodeHwDeviceAdapter;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

/*
 Default constructor
*/
CMdfVideoDecodeHwDeviceAdapter::CMdfVideoDecodeHwDeviceAdapter()
	{
	}
	
void CMdfVideoDecodeHwDeviceAdapter::ConstructL()
	{
	// system clock without HAL
	TInt64 tt64;
	TTime ttime;
	ttime.HomeTime();
	tt64 = ttime.Int64();
	iSystemClock = tt64;

	// reset picture format
	iFormat.iDataFormat = (TImageDataFormat)0;
	
	// Load the PU Loader plugin
	iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(TUid::Uid(KUidPuLoaderImplementation), iPuLoaderDtorKey));	
	iState = EProcessingUnitLoaderLoaded;
	}

/*
 Default destructor
*/
CMdfVideoDecodeHwDeviceAdapter::~CMdfVideoDecodeHwDeviceAdapter()
	{
	DEBUG_PRINT(_L("HwDevice: Destroying"));
	// stop playing if necessary
	Stop();
	
	delete iPlayerEngine;
	delete iScreenDeviceGc;

	// destroy the buffer manager
	delete iBufferManager;
	// destroy the picture header
	delete iPictureHeader;	
	
		// Unload the DecoderPU
	if (iDecoderPU)
		{
		iPuLoader->UnloadProcessingUnit(iDecoderPU);
		}
		
	delete iPuLoader;	
	delete iPuData;
	delete iManufacturer;
	REComSession::DestroyedImplementation(iPuLoaderDtorKey);			

	iInputVidFormats.ResetAndDestroy(); // will destroy contents as well
	iOutputVidFormats.Close();
	iPictureRates.Close();

	DEBUG_PRINT(_L("HwDevice: Destroyed"));
	}

/*
 @see CMMFVideoHwDevice::CustomInterface()
 */
TAny* CMdfVideoDecodeHwDeviceAdapter::CustomInterface(TUid aInterface)
	{
	if (aInterface.iUid == KUidDevVideoHwDeviceAdapterSetup)
		{
		return static_cast<MDevVideoHwDeviceAdapterSetup*>(this);
		}
	return NULL;
	}

/*
 @see CMMFVideoPlayHwDevice::PostProcessorInfoLC()
 */
CPostProcessorInfo* CMdfVideoDecodeHwDeviceAdapter::PostProcessorInfoLC()
	{
	// post processor info will be obtained from the post processor plugin, if any
	return NULL;
	}

/*
 @see CMMFVideoPlayHwDevice::GetOutputFormatListL()
 */
void CMdfVideoDecodeHwDeviceAdapter::GetOutputFormatListL(RArray<TUncompressedVideoFormat>& aFormats)
	{
	// TUncompressedVideoFormats is a union - therefore using hard initialisation
	iOutputVidFormats.Reset();
	TUncompressedVideoFormat outputFormats[KOutputFormatsArraySize];
	
	outputFormats[0].iDataFormat = ERgbRawData;
	outputFormats[0].iRgbFormat = ERgb16bit565;
	iOutputVidFormats.AppendL(outputFormats[0]);
	aFormats.AppendL(outputFormats[0]);
	
	outputFormats[1].iDataFormat = ERgbFbsBitmap;
	outputFormats[1].iRgbFormat = EFbsBitmapColor16M;
	iOutputVidFormats.AppendL(outputFormats[1]);
	aFormats.AppendL(outputFormats[1]);
	
	outputFormats[2].iDataFormat = EYuvRawData;
	iOutputVidFormats.AppendL(outputFormats[2]);
	aFormats.AppendL(outputFormats[2]);
	}

/*
 @see CMMFVideoPlayHwDevice::SetOutputFormatL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetOutputFormatL(const TUncompressedVideoFormat& aFormat)
	{
	// check size and format are supported
	if(iOutputVidFormats.Count() == 0) 
		{
		User::Leave(KErrNotReady);
		}
	if(!iOutputVidFormats.Find(aFormat)) 
		{
		User::Leave(KErrNotSupported);
		}
	
	iFormat = aFormat;
	TDevVideoPlayPuConfig decodeConfig;
	decodeConfig.iImageFormat = iFormat;		
	decodeConfig.iInputBufferSize = KInputBufferSize;	
	TPuConfigVideoPlayback puConfig(decodeConfig);

	User::LeaveIfError(iDecoderPU->Configure(puConfig));
	}

/*
 @see CMMFVideoPlayHwDevice::SetClockSource()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetClockSource(MMMFClockSource* aClock)
	{
	iClockSource = aClock;
	}

/*
 @see CMMFVideoPlayHwDevice::SetVideoDestScreenL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetVideoDestScreenL(TBool aScreen)
	{
	if(aScreen) 
		{
		// NB we expect the output format to have been set before this is called
		if(!iFormat.iDataFormat) 
			{
			User::Leave(KErrNotReady);
			}
	
		// we support only bitmaps for DSA, not raw data
		if(iFormat.iDataFormat != ERgbFbsBitmap) 
			{
			User::Leave(KErrNotSupported);
			}
		}
	
	// ETrue = use DSA
	iDSAEnabled = aScreen;
	}

/*
 @see CMMFVideoPlayHwDevice::SetPostProcessTypesL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetPostProcessTypesL(TUint32 /* aPostProcCombination */)
	{
	// the decoder is not configurable
	User::Leave(KErrNotSupported);
	}

/*
 @see CMMFVideoPlayHwDevice::SetInputCropOptionsL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetInputCropOptionsL(const TRect& /* aRect */)
	{
	// the decoder is not configurable
	User::Leave(KErrNotSupported);
	}

/*
 @see CMMFVideoPlayHwDevice::SetYuvToRgbOptionsL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetYuvToRgbOptionsL(const TYuvToRgbOptions& /* aOptions */, const TYuvFormat& /* aYuvFormat */, TRgbFormat /* aRgbFormat */)
	{
	// the decoder is not configurable
	User::Leave(KErrNotSupported);
	}

/*
 @see CMMFVideoPlayHwDevice::SetYuvToRgbOptionsL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetYuvToRgbOptionsL(const TYuvToRgbOptions& /* aOptions */)
	{
	// the decoder is not configurable
	User::Leave(KErrNotSupported);
	}

/*
 @see CMMFVideoPlayHwDevice::SetRotateOptionsL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetRotateOptionsL(TRotationType /* aRotationType */)
	{
	// the decoder is not configurable
	User::Leave(KErrNotSupported);
	}

/*
 @see CMMFVideoPlayHwDevice::SetScaleOptionsL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetScaleOptionsL(const TSize& /* aTargetSize */, TBool /* aAntiAliasFiltering */)
	{
	// the decoder is not configurable
	User::Leave(KErrNotSupported);
	}

/*
 @see CMMFVideoPlayHwDevice::SetOutputCropOptionsL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetOutputCropOptionsL(const TRect& /* aRect */)
	{
	// the decoder is not configurable
	User::Leave(KErrNotSupported);
	}

/*
 @see CMMFVideoPlayHwDevice::SetPostProcSpecificOptionsL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetPostProcSpecificOptionsL(const TDesC8& /* aOptions */)
	{
	// the decoder is not configurable
	User::Leave(KErrNotSupported);
	}

/*
 @see CMMFVideoPlayHwDevice::Initialize()
 */
void CMdfVideoDecodeHwDeviceAdapter::Initialize()
	{
	__ASSERT_ALWAYS(iProxy, DevVideoHwDeviceDecoderPanic(0));

	TRAPD(err, InitializeL());
	if(err == KErrNone && !iPUInitialized)
		{
		err = KErrNotSupported;		
		}	
	iProxy->MdvppInitializeComplete(this, err);
	}

// private method : body of Initialize()
void CMdfVideoDecodeHwDeviceAdapter::InitializeL()
	{
	// pre-check format
	switch(iFormat.iDataFormat) 
		{
		case 0:
			User::Leave(KErrNotReady);
			break;
		case ERgbRawData:
		case ERgbFbsBitmap:
		case EYuvRawData:
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
		
	// create and initialise the buffer manager
	iBufferManager = CMdfVideoDecoderBufferManager::NewL();

	iBufferManager->Init(iFormat);
	iBufferManager->SetFrameSize(iFrameSize);
	
	DEBUG_PRINT(_L("HwDevice: Buffer Manager initialized"));

	// The actual frame rate will come from the VOL headers.
	iFrameRate = KTenFPS;
	
	// reset the player flags
	iPrimed = EFalse;
	iInputBufferWaiting = EFalse;	
	iInputEnd = EFalse;
	
	// create the player engine
	iPlayerEngine = CMdfVideoPlayerEngine::NewL(*this);

	RPointerArray<MMdfInputPort> inputPorts;
	// Get ports and set observers
	User::LeaveIfError(iDecoderPU->GetInputPorts(inputPorts));
	if (inputPorts.Count()==0)
		{
		inputPorts.Close();
		User::Leave(KErrNotFound);
		}
	iDecoderPUInputPort = inputPorts[0];
	iDecoderPUInputPort->MipSetObserver(*this);
	inputPorts.Close();

	RPointerArray<MMdfOutputPort> outputPorts;
	User::LeaveIfError(iDecoderPU->GetOutputPorts(outputPorts));
	if (outputPorts.Count()==0)
		{
		outputPorts.Close();
		User::Leave(KErrNotFound);
		}
	iDecoderPUOutputPort = outputPorts[0];
	iDecoderPUOutputPort->MopSetObserver(*this);
	outputPorts.Close();
		
	iState = EProcessingUnitLoaded;

	DEBUG_PRINT(_L("HwDevice: Decoder Processing Unit Loaded"));
	
	// Create the buffers that are associated with the input and output ports
	iInputBuffer = &iBufferManager->CreateDataBufferL(iDecoderPUInputPort->MipBufferSize());
	iPUInputBuffer = *iInputBuffer;
	User::LeaveIfError(iDecoderPUInputPort->MipUseBuffer(*iPUInputBuffer));		
	
	CVideoFrameBuffer& frameBuffer = iBufferManager->CreateFrameBufferL(TMMFDisplayModeUtils::DisplayMode(iFormat.iRgbFormat));
	User::LeaveIfError(iDecoderPUOutputPort->MopUseBuffer(frameBuffer));
	
	// VD: should not move the set up of the state after sending the Initialize() calls???
	iState = EProcessingUnitInitializing;

	// async call, that calls back to InitializeComplete()
	iDecoderPU->Initialize();	
		
	DEBUG_PRINT(_L("HwDevice: Initialized OK"));
	}

/*
 @see CMMFVideoPlayHwDevice::StartDirectScreenAccessL()
 */
void CMdfVideoDecodeHwDeviceAdapter::StartDirectScreenAccessL(const TRect& aVideoRect, CFbsScreenDevice& aScreenDevice, const TRegion& aClipRegion)
	{
	// ensure SetVideoDestScreenL() has been called
	if(!iDSAEnabled)
		{
		User::Leave(KErrNotReady);
		}
	
	DEBUG_PRINT(_L("HwDevice: Starting DSA"));

	iDSAStarted = ETrue;

	// the CDirectScreenAccess must be created by the client app.
	// the CFbsScreenDevice (only) is passed across...
	// we need to blit the bitmap into it, i.e. we need to create our own GC
	if(!iScreenDevice) 
		{		
		User::LeaveIfError(aScreenDevice.CreateContext(iScreenDeviceGc));
		iScreenDevice = &aScreenDevice;
		iScreenDeviceRect = aVideoRect;
		iScreenDeviceGc->SetClippingRegion(aClipRegion);
		}
	}

/*
 @see CMMFVideoPlayHwDevice::SetScreenClipRegion()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetScreenClipRegion(const TRegion& aRegion)
	{
	__ASSERT_ALWAYS(iScreenDeviceGc, DevVideoHwDeviceDecoderPanic(0));

	iScreenDeviceGc->SetClippingRegion(aRegion);
	}

/*
 @see CMMFVideoPlayHwDevice::SetPauseOnClipFail()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetPauseOnClipFail(TBool /* aPause */)
	{
	// not implemented - will pause by default
	}

/*
 @see CMMFVideoPlayHwDevice::AbortDirectScreenAccess()
 */
void CMdfVideoDecodeHwDeviceAdapter::AbortDirectScreenAccess()
	{
	DEBUG_PRINT(_L("HwDevice: Stopping DSA"));
	iDSAStarted = EFalse;
	}

/*
 @see CMMFVideoPlayHwDevice::IsPlaying()
 */
TBool CMdfVideoDecodeHwDeviceAdapter::IsPlaying()
	{
	return (iState == EProcessingUnitExecuting ? ETrue : EFalse);
	}

/*
 @see CMMFVideoPlayHwDevice::Redraw()
 */
void CMdfVideoDecodeHwDeviceAdapter::Redraw()
	{
	if(iDSAStarted) 
		{
		DisplayLastFrameDirect(); 
		}
	}

/*
 @see CMMFVideoPlayHwDevice::Start()
 */
void CMdfVideoDecodeHwDeviceAdapter::Start()
	{
	__ASSERT_ALWAYS(iPlayerEngine, DevVideoHwDeviceDecoderPanic(0));
	__ASSERT_ALWAYS(iDecoderPU, DevVideoHwDeviceDecoderPanic(0));
	
	if(iState != EProcessingUnitExecuting)
		{		
		CVideoFrameBuffer* frameBuffer = NULL;
		TRAPD(err, frameBuffer = &iBufferManager->GetEmptyFrameBufferL(ETrue));
		if (err != KErrNone)
			{
			iProxy->MdvppFatalError(this, err);
			return;
			}
		
		iDecoderPUOutputPort->MopReadData(*frameBuffer);
		
		iDecoderPU->Execute();
		TProcessingUnitState puState = iDecoderPU->State();
		if(puState == EProcessingUnitInvalid) 
			{
			return;	
			}
		iState = EProcessingUnitExecuting;

		// if we are using raw decoding, then play as fast as we can,
		// otherwise use the actual frame rate
		TInt playerFrameRate;
		if(iSynchronize) 
			{
			playerFrameRate = TInt(iFrameRate);
			}
		else 
			{
			playerFrameRate = CMdfVideoPlayerEngine::KUnsynchronized;
			}
		TRAP(err, iPlayerEngine->StartL(playerFrameRate));			
		if(err != KErrNone) 
			{
			// the player failed to start. this is a fatal error
			iProxy->MdvppFatalError(this, err);
			}
		else 
			{			
			iProxy->MdvppNewBuffers();
			}
		}
	}

/*
 @see CMMFVideoPlayHwDevice::Stop()
 */
void CMdfVideoDecodeHwDeviceAdapter::Stop()
	{
	if(iState == EProcessingUnitExecuting)
		{
		iPlayerEngine->Stop();
		iDecoderPU->Stop();		
		iState = EProcessingUnitIdle;
		}
	}

/*
 @see CMMFVideoPlayHwDevice::Pause()
 */
void CMdfVideoDecodeHwDeviceAdapter::Pause()
	{
	Stop();
	}

/*
 @see CMMFVideoPlayHwDevice::Resume()
 */
void CMdfVideoDecodeHwDeviceAdapter::Resume()
	{
	Start();
	}

/*
 @see CMMFVideoPlayHwDevice::SetPosition()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition)
	{
	TPuConfigTimestamp timeStamp(aPlaybackPosition);
	// we have no way report a failure here, and it is not a fatal error so any error is ignored
	iDecoderPU->Configure(timeStamp);
	}
/*
 @see CMMFVideoPlayHwDevice::FreezePicture()
 */
void CMdfVideoDecodeHwDeviceAdapter::FreezePicture(const TTimeIntervalMicroSeconds& /* aTimestamp */)
	{
	// not supported; we have no presentation timestamps
	// however should not cause a fatal error
	}

/*
 @see CMMFVideoPlayHwDevice::ReleaseFreeze()
 */
void CMdfVideoDecodeHwDeviceAdapter::ReleaseFreeze(const TTimeIntervalMicroSeconds& /* aTimestamp */)
	{
	// not supported; we have no presentation timestamps
	// however should not cause a fatal error
	}

/*
 @see CMMFVideoPlayHwDevice::PlaybackPosition()
 */
TTimeIntervalMicroSeconds CMdfVideoDecodeHwDeviceAdapter::PlaybackPosition()
	{
	if(iFrameRate == 0.0) 
		{
		return 0;
		}
	// this is the total number of pictures displayed or discarded
	// NB it is NOT equivalent to the number of pictures decoded, as the
	// player may be running a few frames behind the decoder	
	return ((iPictureCounters.iPicturesDisplayed + iPictureCounters.iPicturesSkipped) * (KOneSecond / (TInt)iFrameRate));
	}

/*
 @see CMMFVideoPlayHwDevice::PictureBufferBytes()
 */
TUint CMdfVideoDecodeHwDeviceAdapter::PictureBufferBytes()
	{
	__ASSERT_ALWAYS(iBufferManager, DevVideoHwDeviceDecoderPanic(0));
	
	return (iBufferManager->FrameBufferSize() * iBufferManager->MaxFrameBuffers());
	}

/*
 @see CMMFVideoPlayHwDevice::GetPictureCounters()
 */
void CMdfVideoDecodeHwDeviceAdapter::GetPictureCounters(CMMFDevVideoPlay::TPictureCounters& aCounters)
	{
	aCounters = iPictureCounters;
	}

/*
 @see CMMFVideoPlayHwDevice::SetComplexityLevel()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetComplexityLevel(TUint /* aLevel */)
	{
	// separate complexity levels are not available; ignored
	}

/*
 @see CMMFVideoPlayHwDevice::NumComplexityLevels()
 */
TUint CMdfVideoDecodeHwDeviceAdapter::NumComplexityLevels()
	{
	// separate complexity levels are not available; return 1
	return 1;
	}

/*
 @see CMMFVideoPlayHwDevice::GetComplexityLevelInfo()
 */
void CMdfVideoDecodeHwDeviceAdapter::GetComplexityLevelInfo(TUint aLevel, CMMFDevVideoPlay::TComplexityLevelInfo& aInfo)
	{
	// ignore call if aLevel is not 0
	if(aLevel == 0) 
		{
		aInfo = iComplexityLevelInfo;
		}
	}

/*
 @see CMMFVideoPlayHwDevice::ReturnPicture()
 */
void CMdfVideoDecodeHwDeviceAdapter::ReturnPicture(TVideoPicture* aPicture)
	{
	__ASSERT_ALWAYS(iBufferManager, DevVideoHwDeviceDecoderPanic(0));

	// return the frame buffer that has been emptied
	TRAPD(err, iBufferManager->ReturnFrameBufferL(*aPicture, CVideoFrameBuffer::EEmptied));
	if (err == KErrNone)
		{
		if (!iLastFrameBufferReceived)
			{
			
			// request the next empty frame buffer
			CVideoFrameBuffer* frameBuffer = NULL;
			TRAP(err, frameBuffer = &iBufferManager->GetEmptyFrameBufferL(ETrue));
			if (err == KErrNone)
				{
				CMMFBuffer* buffer = *frameBuffer;
				iDecoderPUOutputPort->MopReadData(*buffer);
				}
			}
		// DoPostPictureNotify() will handle MdvppStreamEnd()	
		}
	else
		{
		iProxy->MdvppFatalError(this, err);
		}
	ASSERT(err == KErrNone);
	DoPostPictureNotify();
	}

/*
 @see CMMFVideoPlayHwDevice::GetSnapshotL()
 */
TBool CMdfVideoDecodeHwDeviceAdapter::GetSnapshotL(TPictureData& /*aPictureData*/, const TUncompressedVideoFormat& /*aFormat*/)
	{
	// not supported - no presentation timestamps
	User::Leave(KErrNotSupported);
	return EFalse;
	}

/*
 @see CMMFVideoPlayHwDevice::GetTimedSnapshotL()
 */
void CMdfVideoDecodeHwDeviceAdapter::GetTimedSnapshotL(TPictureData* /*aPictureData*/, const TUncompressedVideoFormat& /*aFormat*/, const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/)
	{
	// this method should be called on the post processor not on the decoder
	// ending up here is a programming error and hence a PANIC condition
	DevVideoHwDeviceDecoderPanic(0);
	}

/*
 @see CMMFVideoPlayHwDevice::GetTimedSnapshotL()
 */
void CMdfVideoDecodeHwDeviceAdapter::GetTimedSnapshotL(TPictureData* /*aPictureData*/, const TUncompressedVideoFormat& /*aFormat*/, const TPictureId& /*aPictureId*/)
	{
	// this method should be called on the post processor not on the decoder
	// ending up here is a programming error and hence a PANIC condition
	DevVideoHwDeviceDecoderPanic(0);
	}

/*
 @see CMMFVideoPlayHwDevice::CancelTimedSnapshot()
 */
void CMdfVideoDecodeHwDeviceAdapter::CancelTimedSnapshot()
	{
	// this method should be called on the post processor not on the decoder
	// ending up here is a programming error and hence a PANIC condition
	DevVideoHwDeviceDecoderPanic(0);
	}

/*
 @see CMMFVideoPlayHwDevice::GetSupportedSnapshotFormatsL()
 */
void CMdfVideoDecodeHwDeviceAdapter::GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& /*aFormats*/)
	{
	// this method should be called on the post processor not on the decoder
	// ending up here is a programming error and hence a PANIC condition
	DevVideoHwDeviceDecoderPanic(0);
	}

/*
 @see CMMFVideoPlayHwDevice::InputEnd()
 */
void CMdfVideoDecodeHwDeviceAdapter::InputEnd()
	{
	__ASSERT_ALWAYS(iBufferManager, DevVideoHwDeviceDecoderPanic(0));

	DEBUG_PRINT(_L("HwDevice: Input end"));
	
	iInputEnd = ETrue;

	// if there isn't an outstanding write to the PU, write
	// an empty buffer	
	if(!iWriteRequestOutstanding) 
		{
		SendLastBuffer();
		}
	}

/*
 @see CMMFVideoDecodeHwDevice::VideoDecoderInfoLC()
 */
CVideoDecoderInfo* CMdfVideoDecodeHwDeviceAdapter::VideoDecoderInfoLC()
	{
	// this is a complicated structure, ensure all fields are
	// filled with some valid value
	
	//if PU is not loaded panic
	if(iPuData == NULL)	
		{
		DevVideoHwDeviceDecoderPanic(KErrNotReady);	
		}
	// supported formats array
	iInputVidFormats.Reset();
	CCompressedVideoFormat* vidCV = NULL;
	vidCV = CCompressedVideoFormat::NewL(iPuData->InputDataType() , KNullDesC8);
	CleanupStack::PushL(vidCV);
	iInputVidFormats.AppendL(vidCV);
	CleanupStack::Pop(vidCV);	// CCompressedVideo object is destroyed in destructor
	

	// construct the video decoder info object
	CVideoDecoderInfo* vInfo = CVideoDecoderInfo::NewL(
		iPuUid, 
		*iManufacturer,
		KNullDesC,
		TVersion(KVideoDecoderInfoVersionMaj, KVideoDecoderInfoVersionMin, KVideoDecoderInfoVersionBuild),
		iInputVidFormats.Array(),
		EFalse, // not accelerated
		ETrue, // supports direct screen access
		iMaxPictureSize, 
		KMaxTUint32, // max bitrate supported
		iPuData->MaxPictureRates().Array(),
		EFalse, // picture loss not supported,
		EFalse, // slice loss not supported,
		KVideoDecoderInfoCSInfo,
		KVideoDecoderInfoISInfo );

	CleanupStack::PushL(vInfo);
	return vInfo;
	}


/*
 @see CMMFVideoDecodeHwDevice::GetHeaderInformationL()
 */
 
TVideoPictureHeader* CMdfVideoDecodeHwDeviceAdapter::GetHeaderInformationL(TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation, TVideoInputBuffer* aDataUnit)
	{
	// NB this may be called EITHER before OR after the decoder is initialized.
	// ensure at least that SetOutputFormatL() has been called before this, 
	// as the decoder requires it to be able to initialize.
	
	__ASSERT_ALWAYS(iFormat.iDataFormat, DevVideoHwDeviceDecoderPanic(0));
	
	TDevVideoHeaderPuConfig header;
	header.iDataUnitType = aDataUnitType;
	header.iDataUnitEncapsulation = aEncapsulation;
	header.iHeaderData.Set(aDataUnit->iData.Ptr(),aDataUnit->iData.Size());
	
	TPuConfigVideoHeader headerConfig(header);
	
	// Configure the decoder with the Header Information
	User::LeaveIfError(iDecoderPU->Configure(headerConfig));
	
	// retrieve the picture information from the config structure
	
	TPuConfigVideoPictureHeader pict; 
	User::LeaveIfError(iDecoderPU->GetConfig(pict));
	
	delete iPictureHeader;
	iPictureHeader = NULL;
	// Create a new header from the returned data
	iPictureHeader = new (ELeave)TVideoPictureHeader(
							*TPuConfigVideoPictureHeader::GetStructure(pict));
	iFrameSize = iPictureHeader->iSizeInMemory;
	return iPictureHeader;
	
	}

/*
 @see CMMFVideoDecodeHwDevice::ConfigureDecoderL()
 */
void CMdfVideoDecodeHwDeviceAdapter::ConfigureDecoderL(const TVideoPictureHeader& /* aVideoPictureHeader */)
	{
	// there is nothing configurable
	User::Leave(KErrNotSupported);
	}

/*
 @see CMMFVideoDecodeHwDevice::ReturnHeader()
 */
void CMdfVideoDecodeHwDeviceAdapter::ReturnHeader(TVideoPictureHeader* aHeader)
	{
	if(aHeader == iPictureHeader) // only free our created header
		{
		delete iPictureHeader;
		iPictureHeader = NULL;	
		}
	}

/*
 @see CMMFVideoDecodeHwDevice::SetInputFormatL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetInputFormatL(const CCompressedVideoFormat& aFormat, TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation, TBool aDataInOrder)
	{
	// EDuCodedPicture, EDuElementaryStream, aDataInOrder == ETrue
	if(aFormat.MimeType() != iPuData->InputDataType()) 
		{
		User::Leave(KErrNotSupported);	
		}
	if(aDataUnitType != EDuCodedPicture) 
		{
		User::Leave(KErrNotSupported);	
		}
	if(aEncapsulation != EDuElementaryStream) 
		{
		User::Leave(KErrNotSupported);	
		}
	if(!aDataInOrder) 
		{
		User::Leave(KErrNotSupported);	
		}
	}

/*
 @see CMMFVideoDecodeHwDevice::SynchronizeDecoding()
 */
void CMdfVideoDecodeHwDeviceAdapter::SynchronizeDecoding(TBool aSynchronize)
	{
	iSynchronize = aSynchronize;
	}

/*
 @see CMMFVideoDecodeHwDevice::SetBufferOptionsL()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetBufferOptionsL(const CMMFDevVideoPlay::TBufferOptions& aOptions)
	{
	// check values are within the constraints of this device
	if(aOptions.iPreDecoderBufferPeriod != 0) 
		{
		User::Leave(KErrNotSupported);
		}
	if(aOptions.iMaxPostDecodeBufferSize != 0) 
		{
		User::Leave(KErrNotSupported);
		}
	if(aOptions.iPreDecoderBufferPeriod != 0) 
		{
		User::Leave(KErrNotSupported);
		}
	if(aOptions.iPostDecoderBufferPeriod != 0) 
		{
		User::Leave(KErrNotSupported);
		}
	if(aOptions.iMaxInputBufferSize > KVideoDecoderMaxDataBufferSize) 
		{
		User::Leave(KErrNotSupported);
		}
	if(aOptions.iMinNumInputBuffers > 1) 
		{
		User::Leave(KErrNotSupported);
		}
	// input is OK; write it
	iBufferOptions = aOptions;	
	}

/*
 @see CMMFVideoDecodeHwDevice::GetBufferOptions()
 */
void CMdfVideoDecodeHwDeviceAdapter::GetBufferOptions(CMMFDevVideoPlay::TBufferOptions& aOptions)
	{
	aOptions = iBufferOptions;	
	}

/*
 @see CMMFVideoDecodeHwDevice::SetHrdVbvSpec()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetHrdVbvSpec(THrdVbvSpecification /* aHrdVbvSpec */, const TDesC8& /* aHrdVbvParams */)
	{
	// the decoder is not configurable
	// however this should not cause a fatal error
	}

/*
 @see CMMFVideoDecodeHwDevice::SetOutputDevice()
 */
void CMdfVideoDecodeHwDeviceAdapter::SetOutputDevice(CMMFVideoPostProcHwDevice* aDevice)
	{
	// we allow output to be sent to a post-processor,
	// even though this plugin doesn not itself provide one
	iPostProcOutputDevice = aDevice;
	}

/*
 @see CMMFVideoDecodeHwDevice::DecodingPosition()
 */
TTimeIntervalMicroSeconds CMdfVideoDecodeHwDeviceAdapter::DecodingPosition()
	{
	if(iFrameRate == 0.0) 
		{
		return 0;
		}
	// this is the total number of pictures decoded	
	return (iPictureCounters.iPicturesDecoded * (KOneSecond / (TInt)iFrameRate));
	}

/*
 @see CMMFVideoDecodeHwDevice::PreDecoderBufferBytes()
 */
TUint CMdfVideoDecodeHwDeviceAdapter::PreDecoderBufferBytes()
	{
	return 0;
	}

/*
 @see CMMFVideoDecodeHwDevice::GetBitstreamCounters()
 */
void CMdfVideoDecodeHwDeviceAdapter::GetBitstreamCounters(CMMFDevVideoPlay::TBitstreamCounters& aCounters)
	{
	aCounters = iBitstreamCounters;
	}

/*
 @see CMMFVideoDecodeHwDevice::NumFreeBuffers()
 */
TUint CMdfVideoDecodeHwDeviceAdapter::NumFreeBuffers()
	{
	__ASSERT_ALWAYS(iBufferManager, DevVideoHwDeviceDecoderPanic(0));

	if(!FrameBufferAvailable()) 
		{
		// if there are too many output buffers waiting, we refuse an input buffer
		// until some output buffers have been processed.
		return 0;
		}
	else 
		{
		return (iBufferManager->DataBuffersAvailable());
		}
	}

/*
 @see CMMFVideoDecodeHwDevice::GetBufferL()
 
 This will return a buffer of the requested size, within min/max constraints
 */
TVideoInputBuffer* CMdfVideoDecodeHwDeviceAdapter::GetBufferL(TUint /* aBufferSize */)
	{
	__ASSERT_ALWAYS(iBufferManager, DevVideoHwDeviceDecoderPanic(0));

	TVideoInputBuffer& buf = iBufferManager->GetDataBufferL();
	return &buf;
	}

/*
 @see CMMFVideoDecodeHwDevice::WriteCodedDataL()
 */
void CMdfVideoDecodeHwDeviceAdapter::WriteCodedDataL(TVideoInputBuffer* aInputBuffer)
	{
	// Leave on null input buffer
	if(!aInputBuffer) 
		{
		User::Leave(KErrArgument);
		}
		
	// increment received buffer count
	iBitstreamCounters.iTotalPackets++;
	
	CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(iPUInputBuffer);
	dataBuffer->Data().SetLength(aInputBuffer->iData.Length());
	dataBuffer->SetPosition(0);
	iDecoderPUInputPort->MipWriteData(*iPUInputBuffer);
	iWriteRequestOutstanding = ETrue;	
	}
	
/*
 @see CMMFVideoPlayHwDevice::CommitL()
 */
void CMdfVideoDecodeHwDeviceAdapter::CommitL()
	{
	// the decoder is not configurable
	User::Leave(KErrNotSupported);
	}
	
/*
 @see CMMFVideoPlayHwDevice::Revert()
 */
void CMdfVideoDecodeHwDeviceAdapter::Revert()
	{
	// the decoder is not configurable
	// however this should not cause a fatal error
	}

void CMdfVideoDecodeHwDeviceAdapter::SetProxy(MMMFDevVideoPlayProxy& aProxy)
	{
	ASSERT(!iProxy);
	iProxy = &aProxy;
	}
	

/*
 @see MMdfDecoderHwDeviceObserver::FrameBufferAvailable()
 */
TBool CMdfVideoDecodeHwDeviceAdapter::FrameBufferAvailable() const
	{
	__ASSERT_ALWAYS(iBufferManager, DevVideoHwDeviceDecoderPanic(0));

	return (iBufferManager->EmptyFrameBuffersAvailable() > 0);
	}

/*
 @see MMdfVideoPlayerHwDeviceObserver::Time()
 */
TUint CMdfVideoDecodeHwDeviceAdapter::Time() const 
	{
	
	if(iClockSource) 
		{
		return iClockSource->Time().Int64();
		}
	else 
		{
		// system clock without HAL
		TInt64 tt64;
		TTime ttime;
		ttime.HomeTime();
		tt64 = ttime.Int64();
		return (tt64 - iSystemClock);
		}
	}

/*
 @see MMdfVideoPlayerHwDeviceObserver::DisplayFrame()
 */
void CMdfVideoDecodeHwDeviceAdapter::DisplayFrame()
	{
	__ASSERT_ALWAYS(iBufferManager, DevVideoHwDeviceDecoderPanic(0));

	CVideoFrameBuffer* frameBuffer = NULL;
	TRAPD(err, frameBuffer = &iBufferManager->GetFilledFrameBufferL(EFalse));
	if(err != KErrNone) 
		{
		// the buffer manager has failed
		DEBUG_PRINT(_L("HwDevice: Buffer Manager failed"));
		iProxy->MdvppFatalError(this, err);
		}
	else	
		{
		// is this the last buffer?
		iLastFrameBufferReceived = frameBuffer->LastBuffer();
		
		TVideoPicture& thePicture = *frameBuffer;
		
		// increment picture counter
		CMMFBuffer& theBuffer = *frameBuffer;
		if(theBuffer.BufferSize()) 
			{
			iPictureCounters.iPicturesDisplayed++;
			}

		iDisplayPictureAvailable = EFalse;
		
		// send it to post-processor output, DSA, or the client
		if(iPostProcOutputDevice) 
			{
			TRAP(err, iPostProcOutputDevice->WritePictureL(&thePicture));
			}
		else if(iDSAStarted) 
			{
			DisplayFrameDirect(&thePicture);
			TRAP(err, iBufferManager->ReturnFrameBufferL(thePicture, CVideoFrameBuffer::EEmptied));
		
			if (!iLastFrameBufferReceived)
				{
				CVideoFrameBuffer* frameBuffer = NULL;
				TRAP(err, frameBuffer = &iBufferManager->GetEmptyFrameBufferL(ETrue));
				if (err == KErrNone)
					{
					iDecoderPUOutputPort->MopReadData(*frameBuffer);
					}
				else 
					{
					iProxy->MdvppFatalError(this, err);		
					return;
					}
				}
			// DoPostPictureNotify() will handle MdvppStreamEnd()	
			// NB we need to notify the client here as there will be no callback
			DoPostPictureNotify(); 
			}
		else 
			{
			iProxy->MdvppNewPicture(&thePicture);
			}
		}
	}

/*
 @see MMdfVideoPlayerHwDeviceObserver::DiscardFrame()
 */
void CMdfVideoDecodeHwDeviceAdapter::DiscardFrame()
	{
	__ASSERT_ALWAYS(iBufferManager, DevVideoHwDeviceDecoderPanic(0));

	CVideoFrameBuffer* frameBuffer = NULL;
	TVideoPicture* thePicture = NULL;
	TRAPD(err, frameBuffer = &iBufferManager->GetFilledFrameBufferL(EFalse));
	if(err != KErrNone) 
		{
		// the buffer manager has failed
		DEBUG_PRINT(_L("HwDevice: Buffer Manager failed"));
		iProxy->MdvppFatalError(this, err);
		}
	else	
		{
		thePicture = *frameBuffer;

		// increment picture counter
		CMMFBuffer& theBuffer = *frameBuffer;
		if(theBuffer.BufferSize()) 
			{
			iPictureCounters.iPicturesSkipped++;	
			}

		iDisplayPictureAvailable = EFalse;

		// this should never leave, as thePicture has been given to us by 
		// iBufferManager to start with.
		TRAP(err, iBufferManager->ReturnFrameBufferL(*thePicture, CVideoFrameBuffer::EEmptied));
		if (err != KErrNone)
			{			
			DEBUG_PRINT(_L("HwDevice: ReturnFrameBufferL failed"));
			iProxy->MdvppFatalError(this, err);
			}		

		TRAP(err, frameBuffer = &iBufferManager->GetEmptyFrameBufferL(ETrue));
		if (err == KErrNone)
			{
			iDecoderPUOutputPort->MopReadData(*frameBuffer);
			}
	
		// NB we need to notify the client here as there will be no callback
		DoPostPictureNotify(); 
		}
	}

/*
 @see MMdfVideoPlayerHwDeviceObserver::FrameAvailable()
 */
TBool CMdfVideoDecodeHwDeviceAdapter::FrameAvailable() const
	{
	return iDisplayPictureAvailable;
	}
	
/*
 @see MMdfInputPortObserver::MipoWriteDataComplete()
 */
void CMdfVideoDecodeHwDeviceAdapter::MipoWriteDataComplete(const MMdfInputPort* aInputPort,
		CMMFBuffer* /* aBuffer */, TInt aErrorCode)
	{
	iWriteRequestOutstanding = EFalse;
	__ASSERT_ALWAYS(aInputPort == iDecoderPUInputPort, DevVideoHwDeviceDecoderPanic(0));
	if (aErrorCode != KErrNone)
		{
		iProxy->MdvppFatalError(this, aErrorCode);
		return;
		}
	// Return the data buffer, the position tells us how much data
	// was used	
	if (!iInputEnd)
		{
		TRAPD(err, iBufferManager->ReturnDataBufferL(*iInputBuffer));
		if(err == KErrNone)
			{
			iProxy->MdvppNewBuffers();
			}	
		else
			{
			iProxy->MdvppFatalError(this, err);
			}
		}
	else
		{
		// Send an empty buffer to the decoder to signal the
		// end of the stream
		SendLastBuffer();
		}
	}
	
/*
 @see MMdfInputPortObserver::MipoStopComplete()
 */		
void CMdfVideoDecodeHwDeviceAdapter::MipoDisconnectTunnelComplete(const MMdfInputPort* /* aInputPort */,
	TInt /* aErrorCode */)
	{	
	}
	
/*
 @see MMdfInputPortObserver::MipoRestartComplete()
 */		
void CMdfVideoDecodeHwDeviceAdapter::MipoRestartTunnelComplete(const MMdfInputPort* /* aInputPort */,
	TInt /* aErrorCode */)
	{	
	}
	
/*
 @see MMdfOutputPortObserver::MopoReadDataComplete()
 */
void CMdfVideoDecodeHwDeviceAdapter::MopoReadDataComplete(const MMdfOutputPort* aOutputPort,
	CMMFBuffer* aBuffer, TInt aErrorCode)	
	{
	if(aErrorCode != KErrNone)
		{
		iProxy->MdvppFatalError(this, aErrorCode);
		return;
		}
	
	if(aOutputPort != iDecoderPUOutputPort)
		{
		iProxy->MdvppFatalError(this, KErrArgument);
		return;
		}
		
	// increment iPicturesDecoded
	if(aBuffer->BufferSize())
		{
		iPictureCounters.iPicturesDecoded++; 
		}

	iDisplayPictureAvailable = ETrue;
	
	TRAPD(err, iBufferManager->ReturnFrameBufferL(aBuffer, CVideoFrameBuffer::EFilled, aBuffer->LastBuffer()));
	if(err != KErrNone)
		{
		iProxy->MdvppFatalError(this, err);
		}			
	}

/*
 @see MMdfOutputPortObserver::MopoStopComplete()
 */	
void CMdfVideoDecodeHwDeviceAdapter::MopoDisconnectTunnelComplete(const MMdfOutputPort* /* aOutputPort */,
	TInt /* aErrorCode */)
	{	
	}

/*
 @see MMdfOutputPortObserver::MopoRestartComplete()
 */	
void CMdfVideoDecodeHwDeviceAdapter::MopoRestartTunnelComplete(const MMdfOutputPort* /* aOutputPort */,
	TInt /* aErrorCode */)
	{	
	}
	
/*
 @see MMdfProcessingUnitObserver::InitializeComplete()
 */	
void CMdfVideoDecodeHwDeviceAdapter::InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
	{
	__ASSERT_ALWAYS(aPu == iDecoderPU, DevVideoHwDeviceDecoderPanic(0));
	
	if(aErrorCode != KErrNone)
		{
		iProxy->MdvppFatalError(this, aErrorCode);
		return;
		}
			
	iPUInitialized = ETrue;	
	}
	
/*
 @see MMdfProcessingUnitObserver::ExecuteComplete()
 */	
void CMdfVideoDecodeHwDeviceAdapter::ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
	{
	__ASSERT_ALWAYS(aPu == iDecoderPU, DevVideoHwDeviceDecoderPanic(0));
	
	if(aErrorCode != KErrNone)
		{
		iProxy->MdvppFatalError(this, aErrorCode);
		return;
		}	
	
	if (iState == EProcessingUnitExecuting)
		{
		DEBUG_PRINT(_L("HwDevice: 3() - Stream end"));
		iProxy->MdvppStreamEnd();
		iState = EProcessingUnitIdle;
		}			
	}	

// private method - DoPostPictureNotify
void CMdfVideoDecodeHwDeviceAdapter::DoPostPictureNotify()
	{
	__ASSERT_ALWAYS(iBufferManager, DevVideoHwDeviceDecoderPanic(0));

	DEBUG_PRINT(_L("HwDevice: PostPictureNotify"));
	// notify the client that an input buffer is available if we weren't able to
	// do it before
	if(iInputBufferWaiting && iBufferManager->DataBuffersAvailable()) 
		{
		iInputBufferWaiting = EFalse;
		iProxy->MdvppNewBuffers();
		}
	
	// at this point, if there are no frames left, the input buffer is free
	// and InputEnd() has been called, then we have finished.
	if(iInputEnd && !iBufferManager->FilledFrameBuffersAvailable() && iBufferManager->DataBuffersAvailable()) 
		{
		iLastFrameBufferReceived = ETrue;
		}
		
	// NB iLastFrameBufferReceived may have been set beforehand
	if(iLastFrameBufferReceived)
		{
		DEBUG_PRINT(_L("HwDevice: Stream end"));
		iProxy->MdvppStreamEnd();
		}
	}

// private method - display the frame using direct screen access
void CMdfVideoDecodeHwDeviceAdapter::DisplayFrameDirect(TVideoPicture* aPicture) 
	{
	__ASSERT_ALWAYS(iScreenDevice, DevVideoHwDeviceDecoderPanic(0));
	
	DEBUG_PRINT(_L("HwDevice: Displaying frame using DSA"));
	// draw into our GC
	// NB this will only work if the picture is a CFbsBitmap
	CFbsBitmap* theBitmap = aPicture->iData.iRgbBitmap;
	iScreenDeviceGc->DrawBitmap(iScreenDeviceRect, theBitmap);
	iScreenDevice->Update();	
	}
	
// private method - display the last frame using direct screen access
void CMdfVideoDecodeHwDeviceAdapter::DisplayLastFrameDirect() 
	{
	__ASSERT_ALWAYS(iScreenDevice, DevVideoHwDeviceDecoderPanic(0));

	DEBUG_PRINT(_L("HwDevice: Redrawing last frame using DSA"));
	// redraw
	// NB this will only work if the picture is a CFbsBitmap
	const TPictureData* lastFrame = NULL;
	TRAPD(err, lastFrame = &iBufferManager->LastPictureL());
	if(!err) 
		{
		CFbsBitmap* theBitmap = lastFrame->iRgbBitmap;
		iScreenDeviceGc->DrawBitmap(iScreenDeviceRect, theBitmap);
		iScreenDevice->Update();
		}
	}

void CMdfVideoDecodeHwDeviceAdapter::LoadProcessingUnitL(const CImplementationInformation& aImplInfo)
	{
	__ASSERT_ALWAYS(iPuLoader, DevVideoHwDeviceDecoderPanic(0));
	iPuUid = aImplInfo.ImplementationUid();
	iDecoderPU = iPuLoader->LoadProcessingUnitL(*this, iPuUid);	
	// store the opaque data associated with this PU so we can extract information about 
	// the PU later
	iPuData = CCodecApiVideoOpaqueData::NewL(aImplInfo.OpaqueData());
	iManufacturer = HBufC::NewL(iPuData->Manufacturer().Length());
	iManufacturer->Des().Copy(iPuData->Manufacturer());
	iMaxPictureSize = iPuData->MaxPictureSize();
	}

void CMdfVideoDecodeHwDeviceAdapter::SendLastBuffer()
	{
	__ASSERT_ALWAYS(iPUInputBuffer, DevVideoHwDeviceDecoderPanic(0));
	__ASSERT_ALWAYS(iDecoderPUInputPort, DevVideoHwDeviceDecoderPanic(0));
	// Send an empty buffer to the decoder to signal the
	// end of the stream
	iPUInputBuffer->SetLastBuffer(ETrue);
	CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(iPUInputBuffer);
	dataBuffer->Data().SetLength(0);
	dataBuffer->SetPosition(0);
	iDecoderPUInputPort->MipWriteData(*iPUInputBuffer);	
	}
