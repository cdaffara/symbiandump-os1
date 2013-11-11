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

#include "mdfvideoencodehwdeviceadapter.h"
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/devvideo/devvideobase.h>
#include <mmf/devvideo/devvideopuconfig.h>
#include <mmf/server/mmfdatabuffer.h>

// Literal descriptor for the encoder's info
_LIT8(KEncoderInfoCSInfo, "Coded by Symbian");
// Literal descriptor for the encoder's implementation info
_LIT8(KEncoderInfoISInfo, "Implemented by Symbian");
// Literal descriptor for the video encoder panic
_LIT(KDevVideoEncoderPanicCategory, "DevVideoEncoder");

// Processing unit's input port index
const TInt KEncoderPUInputPortIndex  = 0;
// Processing unit's output port index
const TInt KEncoderPUOutputPortIndex = 0;
// Processing unit's major version number
const TInt KEncoderPUInfoVersionMaj = 0;
// Processing unit's minor version number
const TInt KEncoderPUInfoVersionMin = 1;
// Processing unit's build version number
const TInt KEncoderPUInfoVersionBuild = 1;
// Bytes per pixel
const TInt KBytesPerPixel = 3;

void DevVideoEncoderPanic(TInt aReason)
	{
	User::Panic(KDevVideoEncoderPanicCategory, aReason);
	}


/** 
 Constructs a new instance of CMdfVideoEncodeHwDeviceAdapter.
 @return    "CMdfVideoEncodeHwDeviceAdapter*"
            A pointer to the newly constructed HwDevice
 */
CMdfVideoEncodeHwDeviceAdapter* CMdfVideoEncodeHwDeviceAdapter::NewL()
	{
	CMdfVideoEncodeHwDeviceAdapter* self = new(ELeave) CMdfVideoEncodeHwDeviceAdapter;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
 Default constructor
*/
CMdfVideoEncodeHwDeviceAdapter::CMdfVideoEncodeHwDeviceAdapter()
	{
	}

/**
 Safe contructor for CMdfVideoEncodeHwDeviceAdapter.
*/
void CMdfVideoEncodeHwDeviceAdapter::ConstructL()
	{
	// Load the PU Loader plugin
	iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(TUid::Uid(KUidPuLoaderImplementation), iPuLoaderDtorKey));
	}

/**
 Default destructor
*/
CMdfVideoEncodeHwDeviceAdapter::~CMdfVideoEncodeHwDeviceAdapter()
	{
	if(iEncoderPU) 
		{
		iPuLoader->UnloadProcessingUnit(iEncoderPU);		
		}

	delete iPuLoader;
	delete iPuData;
	delete iManufacturer;
	REComSession::DestroyedImplementation(iPuLoaderDtorKey);

	for (TInt i = 0; i < iOutputVideoFormats.Count(); i++)
		{
		delete iOutputVideoFormats[i];
		}
		
	iEncoderPUOutputPortsArray.Reset();
	iEncoderPUOutputPortsArray.Close();
	
	iEncoderPUInputPortsArray.Reset();
	iEncoderPUInputPortsArray.Close();
	
	iPictureRates.Reset();
	iPictureRates.Close();
	
	iInputVideoFormats.Reset();
	iInputVideoFormats.Close();
	
	iOutputVideoFormats.Reset();
	iOutputVideoFormats.Close();
		
	for (TInt i = 0; i < iDataBuffers.Count(); i++)
		{
		User::Free((TAny*)iDataBuffers[i].iData.Ptr());
		}
	iDataBuffers.Reset();
	iDataBuffers.Close();
	
	delete iInputBuffer;
	delete iOutputBuffer;
	}

void CMdfVideoEncodeHwDeviceAdapter::LoadProcessingUnitL(const CImplementationInformation& aImplInfo)
	{
	iPuUid = aImplInfo.ImplementationUid();
	
	iEncoderPU = iPuLoader->LoadProcessingUnitL(*this,iPuUid);
	// store the opaque data associated with this PU so we can extract information about 
	// the PU later
	iPuData = CCodecApiVideoOpaqueData::NewL(aImplInfo.OpaqueData());
	iManufacturer = HBufC::NewL(iPuData->Manufacturer().Length());
	iManufacturer->Des().Copy(iPuData->Manufacturer());
	}

// private method : body of Initialize()
void CMdfVideoEncodeHwDeviceAdapter::InitializeL()
	{
	if (!iEncoderPU)
		{
		iProxy->MdvrpInitializeComplete(this, KErrNotFound);
		return;	
		}
	
	// we have to pre-check that the image format is set correctly,
	// else an attempt to set it into the config will panic.
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

	// get the encoder input ports
	User::LeaveIfError(iEncoderPU->GetInputPorts(iEncoderPUInputPortsArray));

	// set the observer for the encoder input ports
	for(TInt i = 0; i < iEncoderPUInputPortsArray.Count(); i++)
		{
		iEncoderPUInputPortsArray[i]->MipSetObserver(*this);
		}
		
	// get the encoder output ports
	User::LeaveIfError(iEncoderPU->GetOutputPorts(iEncoderPUOutputPortsArray));

	// set the observer for the encoder input ports
	for(TInt i = 0; i < iEncoderPUOutputPortsArray.Count(); i++)
		{
		iEncoderPUOutputPortsArray[i]->MopSetObserver(*this);
		}

	TInt bufSize = iPictureSize.iHeight * iPictureSize.iWidth * KBytesPerPixel;
	iDataBuffers.Reset();

	// create the buffer. zero out all fields
	TVideoOutputBuffer buf;
	memset(&buf, 0, sizeof(buf));

	TUint8* bufData = (TUint8*)User::AllocL(bufSize);
	CleanupStack::PushL(bufData);
	
	buf.iData.Set(bufData, bufSize);
	iDataBuffers.AppendL(buf);
	
	CleanupStack::Pop(bufData); // don't destroy - owned by iDataBuffers

	TDevVideoRecordPuConfig config;
	config.iFrameSize = iPictureSize;
	config.iImageFormat = iFormat;
	config.iFrameRate = iFrameRate;
		
	TPuConfigDevVideoRecord puConfig(config);

	// initialize with config info.
	iEncoderPU->Configure(puConfig);
	
	// create input buffer
	iInputBuffer = CMMFDescriptorBuffer::NewL(bufSize);
	iEncoderPUInputPortsArray[KEncoderPUInputPortIndex]->MipUseBuffer(*iInputBuffer);
			
	// create output buffer 
	TUint32 outputPortBufferSize = 
		iEncoderPUOutputPortsArray[KEncoderPUOutputPortIndex]->MopBufferSize();
	iOutputBuffer = CMMFDescriptorBuffer::NewL(outputPortBufferSize);
	iEncoderPUOutputPortsArray[KEncoderPUOutputPortIndex]->MopUseBuffer(*iOutputBuffer);
	
	// initialize the encoder PU
	iEncoderPU->Initialize();
	
	}

/**
 @see CMMFVideoHwDevice
 */
TAny* CMdfVideoEncodeHwDeviceAdapter::CustomInterface(TUid aInterface)
	{
	if (aInterface.iUid == KUidDevVideoHwDeviceAdapterSetup)
		{
		return static_cast<MDevVideoHwDeviceAdapterSetup*>(this);
		}
	return NULL;
	}

/**
 @see CMMFVideoRecordHwDevice
 */
CPreProcessorInfo* CMdfVideoEncodeHwDeviceAdapter::PreProcessorInfoLC()
	{
	// we have no preprocessor info
	return NULL;
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetInputFormatL(const TUncompressedVideoFormat& aFormat, const TSize& aPictureSize)
	{
	iFormat = aFormat;
	iPictureSize = aPictureSize;
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetSourceCameraL(TInt /* aCameraHandle */, TReal /* aPictureRate */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetSourceMemoryL(TReal aMaxPictureRate, TBool /* aConstantPictureRate */, TBool /* aProcessRealtime */)
	{
	iFrameRate = (TInt)aMaxPictureRate;
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::Initialize()
	{
	TRAPD(err, InitializeL());
	if(err != KErrNone) 
		{
		iProxy->MdvrpInitializeComplete(this, err);
		}
	}
	
/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::WritePictureL(TVideoPicture* aPicture)
	{
	__ASSERT_ALWAYS(iEncoderPU, DevVideoEncoderPanic(0));
	
	if(!aPicture) 
		{
		User::Leave(KErrArgument);
		}
		
	// the picture size MUST be the same as the size the encoder has
	// been initialized with. 
	if(aPicture->iData.iDataSize != iPictureSize) 
	{
		User::Leave(KErrArgument);
	}
	
	// Picture received : increment picture count
	iPictureCounters.iInputPictures++;
	iCurrentPicture = aPicture;
	TDes8& data = iInputBuffer->Data();
	data.SetLength((*aPicture->iData.iRawData).Length());
	data.Copy(*aPicture->iData.iRawData);
	iEncoderPUInputPortsArray[KEncoderPUInputPortIndex]->MipWriteData(*iInputBuffer);
	iEncoderPUOutputPortsArray[KEncoderPUOutputPortIndex]->MopReadData(*iOutputBuffer);	
	
	// Picture encoded : increment picture count
	iPictureCounters.iPicturesProcessed++;
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::InputEnd()
	{
	// The client has notified us it has reached the end of the input stream
	iInputStreamEnd = ETrue;
	iProxy->MdvrpStreamEnd();
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::Start()
	{
	__ASSERT_ALWAYS(iEncoderPU, DevVideoEncoderPanic(0));
	iEncoderPU->Execute();
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::Stop()
	{
	iProxy->MdvrpFatalError(this, KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::Pause()
	{
	iProxy->MdvrpFatalError(this, KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::Resume()
	{
	iProxy->MdvrpFatalError(this, KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::Freeze()
	{
	iProxy->MdvrpFatalError(this, KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::ReleaseFreeze()
	{
	iProxy->MdvrpFatalError(this, KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
TTimeIntervalMicroSeconds CMdfVideoEncodeHwDeviceAdapter::RecordingPosition()
	{
	// return picture count times frame rate
	return iPictureCounters.iPicturesProcessed * (1000000 / iFrameRate);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::GetPictureCounters(CMMFDevVideoRecord::TPictureCounters& aCounters)
	{
	aCounters = iPictureCounters;
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::GetFrameStabilisationOutput(TRect& aRect)
	{
	aRect = TRect(iPictureSize);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
TUint CMdfVideoEncodeHwDeviceAdapter::NumComplexityLevels()
	{
	// separate complexity levels are not available; return 1
	return 1;
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetComplexityLevel(TUint /* aLevel */)
	{
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
CVideoEncoderInfo* CMdfVideoEncodeHwDeviceAdapter::VideoEncoderInfoLC()
	{
	//if PU is not loaded panic
	if(iPuData == NULL)	
		{
		DevVideoEncoderPanic(KErrNotReady);	
		}
	// output formats array
	iOutputVideoFormats.Reset();
	CCompressedVideoFormat* videoCV = NULL;
	videoCV = CCompressedVideoFormat::NewL(iPuData->OutputDataType() , KNullDesC8);
	CleanupStack::PushL(videoCV);
	iOutputVideoFormats.AppendL(videoCV);
	// Note; CCompressedVideo object is destroyed in destructor
	CleanupStack::Pop(videoCV);
	
	// input formats array
	iInputVideoFormats.Reset();
	TUncompressedVideoFormat inputFormats[3];
	inputFormats[0].iDataFormat = ERgbRawData;
	inputFormats[0].iRgbFormat = ERgb16bit565;
	iInputVideoFormats.AppendL(inputFormats[0]);
	inputFormats[1].iDataFormat = ERgbFbsBitmap;
	inputFormats[1].iRgbFormat = EFbsBitmapColor16M;
	iInputVideoFormats.AppendL(inputFormats[1]);
	inputFormats[2].iDataFormat = EYuvRawData;
    memset(&inputFormats[2].iYuvFormat, 0, sizeof(TYuvFormat));
	iInputVideoFormats.AppendL(inputFormats[2]);
		
	// construct the video Encoder info object
	CVideoEncoderInfo* vInfo = CVideoEncoderInfo::NewL(
		iPuUid,
		*iManufacturer,
		KNullDesC,
		TVersion(KEncoderPUInfoVersionMaj, KEncoderPUInfoVersionMin, KEncoderPUInfoVersionBuild),
		EFalse, // not accelerated
		EFalse, // does not support direct capture
		iInputVideoFormats.Array(),
		iOutputVideoFormats.Array(),
		iPuData->MaxPictureSize(),
		EDuCodedPicture, // data unit type(s)
		EDuElementaryStream, // data encapsulation type
		1, // num bitrate layers
		EFalse, // does not support supplemental enhancement info
		1, // unequal error protection levels not supported
		KMaxTUint32, // max bitrate supported
		iPuData->MaxPictureRates().Array(),
		1, // in-layer scalability not supported
		0, // no supported picture options
		EFalse, // picture loss not supported,
		EFalse, // slice loss not supported,
		KEncoderInfoCSInfo,
		KEncoderInfoISInfo);

	CleanupStack::PushL(vInfo);
	return vInfo;
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetOutputFormatL(const CCompressedVideoFormat& /* aFormat */, 
					  TVideoDataUnitType /* aDataUnitType */, 					
					  TVideoDataUnitEncapsulation /* aDataEncapsulation */,
					  TBool /* aSegmentationAllowed */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetOutputRectL(const TRect& /* aRect */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetInputDevice(CMMFVideoPreProcHwDevice* /*aDevice*/)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetErrorsExpected(TBool /* aBitErrors */, TBool /* aPacketLosses */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetMinRandomAccessRate(TReal /* aRate */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetNumBitrateLayersL(TUint /* aNumLayers */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetScalabilityLayerTypeL(TUint /* aLayer */, TScalabilityType /* aScalabilityType */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetGlobalReferenceOptions(TUint /* aMaxReferencePictures */, TUint /* aMaxPictureOrderDelay */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetLayerReferenceOptions(TUint /* aLayer */, TUint /* aMaxReferencePictures */, TUint /* aMaxPictureOrderDelay */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetBufferOptionsL(const TEncoderBufferOptions& /* aOptions */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetCodingStandardSpecificOptionsL(const TDesC8& /* aOptions */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetImplementationSpecificEncoderOptionsL(const TDesC8& /* aOptions */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
HBufC8* CMdfVideoEncodeHwDeviceAdapter::CodingStandardSpecificInitOutputLC()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
HBufC8* CMdfVideoEncodeHwDeviceAdapter::ImplementationSpecificInitOutputLC()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetErrorProtectionLevelsL(TUint /* aNumLevels */, TBool /* aSeparateBuffers */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetErrorProtectionLevelL(TUint /* aLevel */, TUint /* aBitrate */, TUint /* aStrength */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetChannelPacketLossRate(TUint /* aLevel */, 
							  TReal /* aLossRate */, 
							  TTimeIntervalMicroSeconds32 /* aLossBurstLength */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetChannelBitErrorRate(TUint /* aLevel */, TReal /* aErrorRate */, TReal /* aStdDeviation */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetSegmentTargetSize(TUint /* aLayer */, TUint /* aSizeBytes */, TUint /* aSizeMacroblocks */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetRateControlOptions(TUint /* aLayer */, const TRateControlOptions& /* aOptions */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetInLayerScalabilityL(TUint /* aLayer */, TUint /* aNumSteps */,
							TInLayerScalabilityType /* aScalabilityType */,
							const TArray<TUint>& /* aBitrateShare */,
							const TArray<TUint>& /* aPictureShare */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetLayerPromotionPointPeriod(TUint /* aLayer */, TUint /* aPeriod */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
HBufC8* CMdfVideoEncodeHwDeviceAdapter::CodingStandardSpecificSettingsOutputLC()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
HBufC8* CMdfVideoEncodeHwDeviceAdapter::ImplementationSpecificSettingsOutputLC()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SendSupplementalInfoL(const TDesC8& /* aData */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SendSupplementalInfoL(const TDesC8& /* aData */, const TTimeIntervalMicroSeconds& /* aTimestamp */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::CancelSupplementalInfo()
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::GetOutputBufferStatus(TUint& aNumFreeBuffers, TUint& aTotalFreeBytes)
	{
	// We have one output buffer, which has a max size of one raw frame.
	aNumFreeBuffers = 1;
	aTotalFreeBytes = iDataBuffers[0].iData.Size();
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::ReturnBuffer(TVideoOutputBuffer* /*aBuffer*/)
	{
	// Receive a used output buffer (from DevVideoRecord)
	// We do nothing - we have one buffer, which is going to be re-used.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::PictureLoss()
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::PictureLoss(const TArray<TPictureId>& /* aPictures */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SliceLoss(TUint /* aFirstMacroblock */, TUint /* aNumMacroblocks */, const TPictureId& /* aPicture */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::ReferencePictureSelection(const TDesC8& /* aSelectionData */)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::CommitL()
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::Revert()
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoEncodeHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetProxy(MMMFDevVideoRecordProxy& aProxy)
	{
	ASSERT(!iProxy);
	iProxy = &aProxy;
	}

void CMdfVideoEncodeHwDeviceAdapter::MipoWriteDataComplete(const MMdfInputPort* aInputPort, CMMFBuffer* aBuffer, TInt aErrorCode) 
	{
	if (aErrorCode != KErrNone)
		{
		iProxy->MdvrpFatalError(this, aErrorCode);
		return;
		}
	
	if (aInputPort == iEncoderPUInputPortsArray[KEncoderPUInputPortIndex])
		{
		if (aBuffer->LastBuffer())
			{
			iEncoderPU->Stop();
			}	
		else
			{
			iProxy->MdvrpReturnPicture(iCurrentPicture);
			iCurrentPicture = NULL;		
			}
		}
	}
	

void CMdfVideoEncodeHwDeviceAdapter::MipoDisconnectTunnelComplete(const MMdfInputPort* /*aInputPort*/, TInt aErrorCode)
	{
	if (aErrorCode != KErrNone)
		{
		iProxy->MdvrpFatalError(this, aErrorCode);
		}	
	}
	
void CMdfVideoEncodeHwDeviceAdapter::MopoDisconnectTunnelComplete(const MMdfOutputPort* /*aOutputPort*/, TInt aErrorCode)
	{
	if (aErrorCode != KErrNone)
		{
		iProxy->MdvrpFatalError(this, aErrorCode);
		}		
	}	


void CMdfVideoEncodeHwDeviceAdapter::MopoReadDataComplete(const MMdfOutputPort* aOutputPort, CMMFBuffer* aBuffer, TInt aErrorCode)
	{
	if (aErrorCode != KErrNone)
		{
		iProxy->MdvrpFatalError(this, aErrorCode);
		return;
		}		
	if (aOutputPort == iEncoderPUOutputPortsArray[KEncoderPUOutputPortIndex])
		{
		TVideoOutputBuffer buf;
		CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(aBuffer);
		TUint bufSize = dataBuffer->BufferSize();
		buf.iData.Set(const_cast<TUint8*>((dataBuffer->Data()).Ptr()), dataBuffer->BufferSize());

		if (aBuffer->LastBuffer())
			{
			iEncoderPU->Stop();
			iProxy->MdvrpStreamEnd();
			}	
		else
			{
					
			if (!iInputStreamEnd)
				{
				// still expecting more buffers, so let client know
				iProxy->MdvrpNewBuffer(&buf);
				}
			else
				{
				dataBuffer->Data().SetLength(0);
				dataBuffer->SetLastBuffer(ETrue);
				// Write a zero sized input buffer to the port, with the last buffer flag set
				iEncoderPUInputPortsArray[KEncoderPUInputPortIndex]->MipWriteData(*iInputBuffer);
				}
			}
		}		
	}
	

void CMdfVideoEncodeHwDeviceAdapter::InitializeComplete(const CMdfProcessingUnit* /*aPu*/, TInt aErrorCode)
	{
	// KB: We have an error code, so use it not fatal error in this case	
	iProxy->MdvrpInitializeComplete(this, aErrorCode);
	}
	
void CMdfVideoEncodeHwDeviceAdapter::ExecuteComplete(const CMdfProcessingUnit* /*aPu*/, TInt aErrorCode)
	{
	if (aErrorCode != KErrNone)
		{
		iProxy->MdvrpFatalError(this, aErrorCode);
		return;
		}				
	}
	
void CMdfVideoEncodeHwDeviceAdapter::MipoRestartTunnelComplete(const MMdfInputPort* /*aInputPort*/, TInt /*aErrorCode*/)
	{
	// Function is not supported.
	}	
	
void CMdfVideoEncodeHwDeviceAdapter::MopoRestartTunnelComplete(const MMdfOutputPort* /*aOutputPort*/,  TInt /*aErrorCode*/)
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetClockSource(MMMFClockSource* /* aClock */) 
	{
	// Function is not supported.
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetPreProcessTypesL(TUint32 /* aPreProcessTypes */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetRgbToYuvOptionsL(TRgbRange /* aRange */, const TYuvFormat& /* aOutputFormat */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetYuvToYuvOptionsL(const TYuvFormat& /* aInputFormat */, const TYuvFormat& /* aOutputFormat */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetRotateOptionsL(TRotationType /* aRotationType */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetScaleOptionsL(const TSize& /* aTargetSize */, TBool /* aAntiAliasFiltering */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetInputCropOptionsL(const TRect& /* aRect */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetOutputCropOptionsL(const TRect& /* aRect */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetOutputPadOptionsL(const TSize& /* aOutputSize */, const TPoint& /* aPicturePos */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetColorEnhancementOptionsL(const TColorEnhancementOptions& /* aOptions */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetFrameStabilisationOptionsL(const TSize& /* aOutputSize */, TBool /* aFrameStabilisation */)
	{
	User::Leave(KErrNotSupported);
	}

/**
 @see CMMFVideoRecordHwDevice
 */
void CMdfVideoEncodeHwDeviceAdapter::SetCustomPreProcessOptionsL(const TDesC8& /* aOptions */)
	{
	User::Leave(KErrNotSupported);
	}


