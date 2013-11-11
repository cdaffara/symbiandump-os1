// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <mmf/devvideo/videoplayhwdevice.h>
#include <mmf/server/mmfdatabuffer.h>
#include "videoplayfile_cov.h"

#include "../../PU/video/src/Plugin/VideoTestDecoderPU/videotestdecoderpu.hrh"
#include "../../PU/video/src/Plugin/VideoTestEncoderPU/videotestencoderpu.hrh"

const TInt KInputBufferSize = 8192;
const TInt KFilePositionZero = 0;
const TInt KTBufSize = 256;
const TInt KTRegionFixSize = 1;
const TInt64 KInterval=2184;
const TInt KVideoDecoderMaxDataBufferSize = 0x40000;
const TUint KMinInputBuffers=2;
_LIT8(KMime,"video/x-symbiantest");
_LIT8(KBlank,"");


// **************************************************
// instructs the Hw Device Adapter to play a file	
// **************************************************
CPlayVideoFileCov::CPlayVideoFileCov(RTestStepVideoCodecs* aParent) : 
	CActive(EPriorityNormal),
	iState(EHwDeviceInit),
	iParent(aParent)
	{
	}

void  CPlayVideoFileCov::ConstructL()
	{
	CActiveScheduler::Add(this);
	User::LeaveIfError(RFbsSession::Connect());

	TInt err = KErrNone;
	TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(_L(""),EColor16MA));
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(_L(""),EColor16M));
		}
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(_L(""),EColor64K));
		}
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(_L(""),EColor4K));
		}
	if (err == KErrNotSupported)
		{
		TRAP(err, iScreenDevice = CFbsScreenDevice::NewL(_L(""),EColor256));
		}
	if (err == KErrNotSupported)
		{
		iScreenDevice = CFbsScreenDevice::NewL(_L(""),EColor16MAP);
		}	
	else
		{
		User::LeaveIfError(err);		
		}
	}

CPlayVideoFileCov* CPlayVideoFileCov::NewL(RTestStepVideoCodecs* aParent)
	{
	CPlayVideoFileCov* self = new (ELeave) CPlayVideoFileCov(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
	
void CPlayVideoFileCov::DoCancel()
	{
	}

CPlayVideoFileCov::~CPlayVideoFileCov()
	{
	iFile.Close();
	iFs.Close();
	
	delete iVideoHwDevice;
	delete iScreenDevice;
	RFbsSession::Disconnect();
	}
	
	
void CPlayVideoFileCov::LoadCodecL()	
	{
	iParent->InfoMessage(_L("Loading Hw Device Adapter and PU Codec"));

	RImplInfoPtrArray array;
	REComSession::ListImplementationsL(TUid::Uid(KUidMdfProcessingUnit), array);
	TBool found = EFalse;
	CImplementationInformation* info = NULL;
	for (TInt i=0;i<array.Count() && !found;i++)
		{
		info = array[i];
		if (info->ImplementationUid() == TUid::Uid(KUidVideoTestDecoderPu))
			{
			found = ETrue;
			}
		}
	TInt err = KErrNotFound;
	if (found)
		{
		TRAP(err, iVideoHwDevice = CMMFVideoDecodeHwDevice::NewPuAdapterL(*info,*this));
		}
	else
		{
		err = KErrNotFound;
		}
	CVideoDecoderInfo* vInfo=NULL;
	iParent->InfoMessage(_L("VideoDecoderInfoLC"));
	TRAP(iError, vInfo=iVideoHwDevice->VideoDecoderInfoLC(); CleanupStack::Pop(vInfo) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("VideoDecoderInfoLC Failed"));
		SetState(EHwDeviceError);
		}
	delete vInfo;
	CCompressedVideoFormat* format=CCompressedVideoFormat::NewL(KMime);
	TVideoDataUnitType dataUnitType=EDuCodedPicture;
	TVideoDataUnitEncapsulation encapsulation=EDuElementaryStream;
	TBool dataInOrder=ETrue;
	iParent->InfoMessage(_L("SetInputFormatL"));
	TRAP(iError, iVideoHwDevice->SetInputFormatL(*format,dataUnitType,encapsulation,dataInOrder));
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("SetInputFormatL Failed"));
		SetState(EHwDeviceError);
		}
	//Negative test case for  SetInputFormatL
	CCompressedVideoFormat* format2=CCompressedVideoFormat::NewL(KBlank);
	TRAP(iError, iVideoHwDevice->SetInputFormatL(*format2,dataUnitType,encapsulation,dataInOrder));
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetInputFormatL Failed"));
		SetState(EHwDeviceError);
		}
	delete format2;
	//Negative test case for SetInputFormatL
	dataUnitType=EDuVideoSegment;
	TRAP(iError, iVideoHwDevice->SetInputFormatL(*format,dataUnitType,encapsulation,dataInOrder));
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetInputFormatL Failed"));
		SetState(EHwDeviceError);
		}
	//Negative test case for SetInputFormatL
	dataUnitType=EDuCodedPicture;
	encapsulation=EDuGenericPayload;
	TRAP(iError, iVideoHwDevice->SetInputFormatL(*format,dataUnitType,encapsulation,dataInOrder));
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetInputFormatL Failed"));
		SetState(EHwDeviceError);
		}
	//Negative test case for SetInputFormatL
	encapsulation=EDuElementaryStream;
	dataInOrder=EFalse;
	TRAP(iError, iVideoHwDevice->SetInputFormatL(*format,dataUnitType,encapsulation,dataInOrder));
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetInputFormatL Failed"));
		SetState(EHwDeviceError);
		}
	delete format;
	array.ResetAndDestroy();
	User::LeaveIfError(err);

	}
	
void CPlayVideoFileCov::SetDSA(TBool aUseDSA)	
	{
	// Need new version of this to load from a PU based uid
	iUseDSA = aUseDSA;
	}
	
void CPlayVideoFileCov::StartPlaybackL()	
	{
	OpenTestFileL();
	SetState(EHwDeviceInit);
	CActiveScheduler::Start();
	}

void CPlayVideoFileCov::SetState(TPlayVideoFileState aState)
	{
	iState = aState;
	SetActive();	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);	
	}

void CPlayVideoFileCov::OpenTestFileL()
	{
	TBuf<KTBufSize> buf;
	buf.Format(_L("Opening test input file %S"), &KTestPlaybackFile);
	iParent->InfoMessage(buf);
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFile.Open(iFs, KTestPlaybackFile, EFileRead));
	}

void CPlayVideoFileCov::RunL()
 	{
	switch (iState)
		{
		case EHwDeviceInit:
			{
			iParent->InfoMessage(_L("State: EHwDeviceInit"));
			TRAPD(err, InitializeL());
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("Cannot intialize HwDevice"));
				break;
				}
			break;
			}
		case EHwDeviceStartDecode:
			{
			// if we are playing a file, first we have to open the file
			iParent->InfoMessage(_L("State: EHwDeviceStartDecode"));
			TRAPD(err, StartDecodeL());
			if (err != KErrNone)
				{
				CleanupAndSetDeviceError(_L("Cannot start decoding"));
				break;
				}
			break;
			}
		case EHwDeviceAllowToComplete:
			iParent->InfoMessage(_L("State: EHwDeviceAllowToComplete"));
			break;
		case EHwDeviceDone:
			{
			iParent->InfoMessage(_L("State: EHwDeviceDeviceDone"));
			Cancel();
			CActiveScheduler::Stop();
			break;
			}				
		case EHwDeviceError:
			{
			TBuf<KTBufSize> buf;
			buf.Format(_L("State: EHwDeviceDeviceError %d"), iError);
			iParent->SetVerdict(buf, EFail);
			
			CActiveScheduler::Stop();
			break;
			}
		default:
			{
			CleanupAndSetDeviceError(_L("Unknown CPlayVideoFileCov iState"));
			break;
			}
		}
	}
	
	
void CPlayVideoFileCov::InitializeL()
	{
	iParent->InfoMessage(_L("InitializeL()"));

	CMMFDevVideoPlay::TBufferOptions bufferOptions;
	bufferOptions.iMaxInputBufferSize=0x40000;
	iVideoHwDevice->SetBufferOptionsL(bufferOptions);
	
	TTimeIntervalMicroSeconds Interval(KInterval);
	iVideoHwDevice->SetPosition(Interval);
	
	CMMFDevVideoPlay::TBitstreamCounters bitstreamCounters;
	iVideoHwDevice->GetBitstreamCounters(bitstreamCounters);

	CMMFDevVideoPlay::TBufferOptions Options;
	Options.iMaxInputBufferSize=0x40000;
	iVideoHwDevice->SetBufferOptionsL(Options);
	
	CMMFDevVideoPlay::TBitstreamCounters Counters;
	iVideoHwDevice->GetBitstreamCounters(Counters);
	
	TUint complexityLevel=1;
	iParent->InfoMessage(_L("SetComplexityLevel"));
	TRAP(iError, iVideoHwDevice->SetComplexityLevel(complexityLevel) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("SetComplexityLevel Failed"));
		SetState(EHwDeviceError);
		}
	THrdVbvSpecification HrdVbvSpec=EHrdVbvNone;
	iParent->InfoMessage(_L("SetHrdVbvSpec"));
	TRAP(iError, iVideoHwDevice->SetHrdVbvSpec(HrdVbvSpec,KBlank) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("SetHrdVbvSpec Failed"));
		SetState(EHwDeviceError);
		}
	TRect rect;
	iParent->InfoMessage(_L("SetInputCropOptionsL"));
	TRAP(iError, iVideoHwDevice->SetInputCropOptionsL(rect) );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetInputCropOptionsL Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("SetOutputCropOptionsL"));
	TRAP(iError, iVideoHwDevice->SetOutputCropOptionsL(rect) );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetOutputCropOptionsL Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("SetPauseOnClipFail"));
	TRAP(iError, iVideoHwDevice->SetPauseOnClipFail(ETrue) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("SetPauseOnClipFail Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("SetPostProcessTypesL"));
	TRAP(iError, iVideoHwDevice->SetPostProcessTypesL(ETrue) );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetPostProcessTypesL Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("SetPostProcSpecificOptionsL"));
	TRAP(iError, iVideoHwDevice->SetPostProcSpecificOptionsL(KBlank) );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetPostProcSpecificOptionsL Failed"));
		SetState(EHwDeviceError);
		}
	TRotationType rotType=ERotate90Clockwise;
	iParent->InfoMessage(_L("SetRotateOptionsL"));
	TRAP(iError, iVideoHwDevice->SetRotateOptionsL(rotType) );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetRotateOptionsL Failed"));
		SetState(EHwDeviceError);
		}
	TSize size(12,30);
	iParent->InfoMessage(_L("SetScaleOptionsL"));
	TRAP(iError, iVideoHwDevice->SetScaleOptionsL(size,ETrue) );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetScaleOptionsL Failed"));
		SetState(EHwDeviceError);
		}
	TYuvToRgbOptions YuvOption;
	
	iParent->InfoMessage(_L("SetYuvToRgbOptionsL"));
	TRAP(iError, iVideoHwDevice->SetYuvToRgbOptionsL(YuvOption) );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetYuvToRgbOptionsL Failed"));
		SetState(EHwDeviceError);
		}
	TYuvFormat YuvFormat;
	TRgbFormat RgbFormat=ERgb16bit444;
	iParent->InfoMessage(_L("SetYuvToRgbOptionsL"));
	TRAP(iError, iVideoHwDevice->SetYuvToRgbOptionsL(YuvOption,YuvFormat,RgbFormat) );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetYuvToRgbOptionsL Failed"));
		SetState(EHwDeviceError);
		}
	CMMFVideoPostProcHwDevice *outDevice = NULL;
	iParent->InfoMessage(_L("SetOutputDevice"));
	TRAP(iError, iVideoHwDevice->SetOutputDevice(outDevice) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("SetOutputDevice Failed"));
		SetState(EHwDeviceError);
		}
	
	TVideoPictureHeader  videoPictureHeader;
	iParent->InfoMessage(_L("ConfigureDecoderL"));
	TRAP(iError, iVideoHwDevice->ConfigureDecoderL(videoPictureHeader) );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("ConfigureDecoderL Failed"));
		SetState(EHwDeviceError);
		}
	TUncompressedVideoFormat reqFormat;
		
	reqFormat.iDataFormat = ERgbFbsBitmap;
	reqFormat.iRgbFormat = EFbsBitmapColor16M;
	
	RArray<TUncompressedVideoFormat> decodeFormats;
	iVideoHwDevice->GetOutputFormatListL(decodeFormats);
	CleanupClosePushL(decodeFormats);
	User::LeaveIfError(decodeFormats.Find(reqFormat));

	iParent->InfoMessage(_L("Call VideoHwDevice->SetOutputFormat()"));
	iVideoHwDevice->SetOutputFormatL(reqFormat);
	CleanupStack::PopAndDestroy(&decodeFormats);	
	
	CMMFDescriptorBuffer* buffer = CMMFDescriptorBuffer::NewL(KInputBufferSize);
	CleanupStack::PushL(buffer);
	TVideoInputBuffer inputBuffer;
	
	TDes8& des = buffer->Data();
	// read header data from file
	User::LeaveIfError(iFile.Read(des));
	TInt pos = KFilePositionZero;
	// seek back to start
	User::LeaveIfError(iFile.Seek(ESeekStart, pos));

	TBuf<KTBufSize> buf;
	buf.Format(_L("Read header of size %d"),inputBuffer.iData.Length());
	iParent->InfoMessage(buf);
	
	if (des.Length()>0)
		{
		// Set the pointer
		inputBuffer.iData.Set(&des[0],des.Length(),des.MaxLength());
		
		iParent->InfoMessage(_L("Call VideoHwDevice GetHeaderInformationL()"));
		TVideoPictureHeader* header = iVideoHwDevice->GetHeaderInformationL(EDuArbitraryStreamSection, 
												EDuElementaryStream,
												&inputBuffer);
		TVideoPictureHeader* header2 =NULL;
		//Call ReturnHeader for negative test
		iVideoHwDevice->ReturnHeader(header2);
		iFrameSize = header->iSizeInMemory;
		iVideoHwDevice->ReturnHeader(header);
		}
	else
		{
		// Cannot read any data from file, so no point in continuing
		User::Leave(KErrCorrupt);
		}
		
	CleanupStack::PopAndDestroy(buffer);	
	
	iParent->InfoMessage(_L("Call VideoHwDevice SetVideoDestScreenL()"));
	iVideoHwDevice->SetVideoDestScreenL(iUseDSA);
	
	iParent->InfoMessage(_L("Call VideoHwDevice->Initialize()"));
	iVideoHwDevice->Initialize();
	}
	
	
void CPlayVideoFileCov::StartDecodeL()
	{
	iParent->InfoMessage(_L("SynchronizeDecoding"));
	TRAP(iError, iVideoHwDevice->SynchronizeDecoding(ETrue) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("SynchronizeDecoding Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("StartDecodeL()"));

	if (iUseDSA)
		{
		StartDirectScreenAccessL();
		}
	// tell the HwDeviceAdapter to play the file
	iParent->InfoMessage(_L("Call VideoHwDevice->Start()"));
	iVideoHwDevice->Start();
	CSystemClockSource *time1= NULL;
	
	iParent->InfoMessage(_L("SetClockSource"));
	TRAP(iError, iVideoHwDevice->SetClockSource(time1) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("SetClockSource Failed"));
		SetState(EHwDeviceError);
		}
	
	delete time1;
	iVideoHwDevice->PictureBufferBytes();
	CMMFDevVideoPlay::TComplexityLevelInfo aInfo;
	iVideoHwDevice->GetComplexityLevelInfo(0,aInfo);
	//Call GetComplexityLevelInfo for False condition coverage
	iVideoHwDevice->GetComplexityLevelInfo(1,aInfo);
	iVideoHwDevice->IsPlaying();
	CMMFDevVideoPlay::TPictureCounters PictureCounters;
	iParent->InfoMessage(_L("GetPictureCounters"));
	TRAP(iError, iVideoHwDevice->GetPictureCounters(PictureCounters) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("GetPictureCounters Failed"));
		SetState(EHwDeviceError);
		}
	TPictureData PictureData;
	TUncompressedVideoFormat UncomprossedVideoFormat;
	iParent->InfoMessage(_L("GetSnapshotL"));
	TRAP(iError, iVideoHwDevice->GetSnapshotL(PictureData,UncomprossedVideoFormat) );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("GetSnapshotL Failed"));
		SetState(EHwDeviceError);
		}
	CMMFDevVideoPlay::TBufferOptions bufferOptions;
	iVideoHwDevice->GetBufferOptions(bufferOptions);
	iVideoHwDevice->NumComplexityLevels();

	iParent->InfoMessage(_L("NumFreeBuffers"));
	TRAP(iError, iVideoHwDevice->NumFreeBuffers() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("NumFreeBuffers Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("PlaybackPosition"));
	TRAP(iError, iVideoHwDevice->PlaybackPosition() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("PlaybackPosition Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("DecodingPosition"));
	TRAP(iError, iVideoHwDevice->DecodingPosition() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("DecodingPosition Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("Redraw"));
	TRAP(iError, iVideoHwDevice->Redraw() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("Redraw Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("AbortDirectScreenAccess"));
	TRAP(iError, iVideoHwDevice->AbortDirectScreenAccess() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("AbortDirectScreenAccess Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("CommitL"));
	TRAP(iError, iVideoHwDevice->CommitL() );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("CommitL Failed"));
		SetState(EHwDeviceError);
		}
	TTimeIntervalMicroSeconds Timestamp;
	iParent->InfoMessage(_L("FreezePicture"));
	TRAP(iError, iVideoHwDevice->FreezePicture(Timestamp) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("FreezePicture Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("PostProcessorInfoLC"));
	TRAP(iError, iVideoHwDevice->PostProcessorInfoLC() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("PostProcessorInfoLC Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("PreDecoderBufferBytes"));
	TRAP(iError, iVideoHwDevice->PreDecoderBufferBytes() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("PreDecoderBufferBytes Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("ReleaseFreeze"));
	TRAP(iError, iVideoHwDevice->ReleaseFreeze(Timestamp) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("ReleaseFreeze Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("Revert"));
	TRAP(iError, iVideoHwDevice->Revert() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("Revert Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("Redraw"));
	TRAP(iError, iVideoHwDevice->Redraw() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("Redraw Failed"));
		SetState(EHwDeviceError);
		}
	
	}
	

	
void CPlayVideoFileCov::ReadNextBufferL()
	{
	// check that we aren't waiting for the input buffer to be returned
	iParent->InfoMessage(_L("ReadNextBufferL()"));
	TVideoInputBuffer* inputBuffer = iVideoHwDevice->GetBufferL(KInputBufferSize);	
	CMMFDevVideoPlay::TBufferOptions aOptions;
	iVideoHwDevice->GetBufferOptions(aOptions);
	iVideoHwDevice->NumComplexityLevels();
	
	
	if (inputBuffer)
		{
		TBuf<KTBufSize> buf;
		buf.Format(_L("inputBuffer size %d"),inputBuffer->iData.Length());
		iParent->InfoMessage(buf);
		User::LeaveIfError(iFile.Read(inputBuffer->iData));
		buf.Format(_L("Read from file %d bytes"),inputBuffer->iData.Length());
		iParent->InfoMessage(buf);
		if (inputBuffer->iData.Length()>0)
			{
			iParent->InfoMessage(_L("Call HwDevice->WriteCodedData()"));
			
			iVideoHwDevice->WriteCodedDataL(inputBuffer);
			}
		else
			{
			iParent->InfoMessage(_L("End of input stream"));
			iParent->InfoMessage(_L("Call HwDevice->InputEnd()"));
			iVideoHwDevice->InputEnd();
			}
		}
	}
		
void CPlayVideoFileCov::CleanupAndSetDeviceError(TPtrC16 aText)
	{
	iParent->SetVerdict(aText, EFail);
	delete iVideoHwDevice;
	SetState(EHwDeviceError);
	}
	
void CPlayVideoFileCov::MdvppNewPicture(TVideoPicture* aPicture)
	{
	iParent->InfoMessage(_L("MdvppNewPicture - Received Frame"));
	iVideoHwDevice->ReturnPicture(aPicture);
	iParent->InfoMessage(_L("MdvppNewPicture - finished returning Frame"));
	}

void CPlayVideoFileCov::MdvppNewBuffers()
	{
	iParent->InfoMessage(_L("MdvppNewBuffers()"));
	TRAP(iError, ReadNextBufferL());
	if (iError != KErrNone)
		{
		iParent->SetVerdict(_L("Error reading next buffer"),EFail);		
		iVideoHwDevice->Stop();
		SetState(EHwDeviceError);
		}
	}

void CPlayVideoFileCov::MdvppReturnPicture(TVideoPicture* /*aPicture*/)
	{
	iParent->InfoMessage(_L("MdvppReturnPicture()"));
	}
	
void CPlayVideoFileCov::MdvppSupplementalInformation(const TDesC8& /*aData*/, 
	const TTimeIntervalMicroSeconds& /*aTimestamp*/, const TPictureId& /*aPictureId*/)
	{
	}
	
void CPlayVideoFileCov::MdvppPictureLoss()
	{
	iParent->InfoMessage(_L("MdvppPictureLoss()"));
	}
	
void CPlayVideoFileCov::MdvppPictureLoss(const TArray<TPictureId>& /*aPictures*/)
	{
	iParent->InfoMessage(_L("MdvppPictureLoss()"));
	}
	
void CPlayVideoFileCov::MdvppSliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId& /*aPicture*/)
	{
	iParent->InfoMessage(_L("MdvppSliceLoss()"));
	}
	
void CPlayVideoFileCov::MdvppReferencePictureSelection(const TDesC8& /*aSelectionData*/)
	{
	}
	
void CPlayVideoFileCov::MdvppTimedSnapshotComplete(TInt /*aError*/, TPictureData* /*aPictureData*/, 
	const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/, const TPictureId& /*aPictureId*/)
	{
	}
	
void CPlayVideoFileCov::MdvppFatalError(CMMFVideoHwDevice* /*aDevice*/, TInt aError)
	{
	iError = aError;
	iParent->InfoMessage(_L("MdvppFatalError()"));
	SetState(EHwDeviceError);
	}
	
void CPlayVideoFileCov::MdvppInitializeComplete(CMMFVideoHwDevice* /*aDevice*/, TInt aError)
	{
	iParent->InfoMessage(_L("MdvppInitializeComplete()"));
	iError = aError;
	if (iError == KErrNone)
		{
		iParent->InfoMessage(_L("The Hw Device Adapter initialised correctly"));
		SetState(EHwDeviceStartDecode);
		}
	else
		{
		iParent->InfoMessage(_L("Failure intialising the hw device adapter"));
		SetState(EHwDeviceError);
		}
	}
	
void CPlayVideoFileCov::MdvppStreamEnd()
	{
	iParent->InfoMessage(_L("MdvppStreamEnd()"));
	SetState(EHwDeviceDone);
	}

void CPlayVideoFileCov::StartDirectScreenAccessL()
	{
	TRegionFix<KTRegionFixSize> reg;
	TRect pos(iFrameSize);
	reg.AddRect(pos);
	iVideoHwDevice->StartDirectScreenAccessL(pos, *iScreenDevice, reg);
	iParent->InfoMessage(_L("Pause"));
	TRAP(iError, iVideoHwDevice->Pause() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("Pause Failed"));
		SetState(EHwDeviceError);
		}
	iParent->InfoMessage(_L("Resume"));
	TRAP(iError, iVideoHwDevice->Resume() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("Resume Failed"));
		SetState(EHwDeviceError);
		}
	RRegion region;
	iParent->InfoMessage(_L("SetScreenClipRegion"));
	TRAP(iError, iVideoHwDevice->SetScreenClipRegion(region) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("SetScreenClipRegion Failed"));
		SetState(EHwDeviceError);
		}
	}
void CPlayVideoFileCov::CancelTimedSnapshot()
	{
iParent->InfoMessage(_L("CancelTimedSnapshot"));
TRAP(iError, iVideoHwDevice->CancelTimedSnapshot() );
if (iError!=KErrNone)
	{
	iParent->InfoMessage(_L("CancelTimedSnapshot Failed"));
	SetState(EHwDeviceError);
	}
}

void CPlayVideoFileCov::GetTimedSnapshot1()
	{
TPictureData* pictureData=NULL;
TUncompressedVideoFormat format;
TTimeIntervalMicroSeconds presentationTimestamp;
iParent->InfoMessage(_L("GetTimedSnapshotL"));
TRAP(iError, iVideoHwDevice->GetTimedSnapshotL(pictureData,format,presentationTimestamp) );
if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("GetTimedSnapshotL Failed"));
		SetState(EHwDeviceError);
		}

	}

void CPlayVideoFileCov::GetTimedSnapshot2()
	{
TPictureData* pictureData=NULL;
TUncompressedVideoFormat format;
TPictureId pictureId;
iParent->InfoMessage(_L("GetTimedSnapshotL"));
TRAP(iError, iVideoHwDevice->GetTimedSnapshotL(pictureData,format,pictureId) );
if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("GetTimedSnapshotL Failed"));
		SetState(EHwDeviceError);
		}

	}

void CPlayVideoFileCov::GetSupportedSnapshotFormats()
	{
RArray<TUncompressedVideoFormat> formats;
iParent->InfoMessage(_L("GetSupportedSnapshotFormatsL"));
TRAP(iError, iVideoHwDevice->GetSupportedSnapshotFormatsL(formats) );
if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("GetSupportedSnapshotFormatsL Failed"));
		SetState(EHwDeviceError);
		}

	}



void CPlayVideoFileCov::NegTests()
	{
	//call PlayBackPosition Before initializing iFrameRate
	TRAP(iError, iVideoHwDevice->PlaybackPosition() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("PlaybackPosition Failed"));
		
		}
	//call DecodingPosition Before initializing iFrameRate
	TRAP(iError, iVideoHwDevice->DecodingPosition() );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("DecodingPosition Failed"));
		
		}
	
	//call StartDirectScreenAccess before DSA is enabled
	TRegionFix<KTRegionFixSize> reg;
	TRect pos(iFrameSize);
	reg.AddRect(pos);
	
	TRAP(iError, iVideoHwDevice->StartDirectScreenAccessL(pos, *iScreenDevice, reg) );
	if (iError!=KErrNotReady)
		{
		iParent->InfoMessage(_L("StartDirectScreenAccessL Failed"));
		
		}
	TUncompressedVideoFormat reqFormat;
	reqFormat.iDataFormat = ERgbFbsBitmap;
	reqFormat.iRgbFormat = EFbsBitmapColor16M;
	
	
	
	iParent->InfoMessage(_L("Call VideoHwDevice->SetOutputFormat()"));
	
	TRAP(iError, iVideoHwDevice->SetOutputFormatL(reqFormat) );
	if (iError!=KErrNotReady)
		{
		iParent->InfoMessage(_L("SetOutputFormatL Failed"));
		
		}
	
	
	

	//Call initialize without settting output format
	//Negative Test to improve code coverage
	iVideoHwDevice->Initialize();
	iVideoHwDevice->IsPlaying();
	Cancel(); //Cancel pending requests
	
	
	

	}

void CPlayVideoFileCov::InitializeCov1()
	{
	
	
	//Negative test for Buffer Options
	CMMFDevVideoPlay::TBufferOptions bufferOptions;
	bufferOptions.iMaxInputBufferSize=0x40000;
	bufferOptions.iPreDecoderBufferPeriod=1;
	TRAP(iError, iVideoHwDevice->SetBufferOptionsL(bufferOptions); );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetBufferOptionsL Failed"));
		
		}
	bufferOptions.iPreDecoderBufferPeriod=0;
	bufferOptions.iMaxPostDecodeBufferSize=1;
	TRAP(iError, iVideoHwDevice->SetBufferOptionsL(bufferOptions); );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetBufferOptionsL Failed"));
		
		}
	bufferOptions.iMaxPostDecodeBufferSize=0;
	bufferOptions.iPostDecoderBufferPeriod=1;
	TRAP(iError, iVideoHwDevice->SetBufferOptionsL(bufferOptions); );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetBufferOptionsL Failed"));
		
		}
	bufferOptions.iPostDecoderBufferPeriod=0;
	bufferOptions.iMaxInputBufferSize=KVideoDecoderMaxDataBufferSize+1;
	TRAP(iError, iVideoHwDevice->SetBufferOptionsL(bufferOptions); );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetBufferOptionsL Failed"));
		
		}
	bufferOptions.iMaxInputBufferSize=0;
	bufferOptions.iMinNumInputBuffers=KMinInputBuffers;
	TRAP(iError, iVideoHwDevice->SetBufferOptionsL(bufferOptions); );
	if (iError!=KErrNotSupported)
		{
		iParent->InfoMessage(_L("SetBufferOptionsL Failed"));
		
		}
	//Call SetVideoDest before iFormat has been set
	iParent->InfoMessage(_L("Call VideoHwDevice SetVideoDestScreenL()"));
	TRAP(iError, iVideoHwDevice->SetVideoDestScreenL(ETrue); );
	if (iError!=KErrNotReady)
		{
		iParent->InfoMessage(_L("SetVideoDestScreenL Failed"));
		
		}
	
	
	//SetOutputFormat which is not supported
	TUncompressedVideoFormat reqFormat;
	reqFormat.iDataFormat = ERgbRawData;
	reqFormat.iRgbFormat = EFbsBitmapColor16M;

	RArray<TUncompressedVideoFormat> decodeFormats;
	TRAP(iError,iVideoHwDevice->GetOutputFormatListL(decodeFormats));
	CleanupClosePushL(decodeFormats);
	User::LeaveIfError(decodeFormats.Find(reqFormat));

	iParent->InfoMessage(_L("Call VideoHwDevice->SetOutputFormat()"));
	TRAP(iError, iVideoHwDevice->SetOutputFormatL(reqFormat) );
	if (iError!=KErrNotReady)
		{
		iParent->InfoMessage(_L("SetOutputFormatL Failed"));
		
		}
	//Call WriteCodedDataL with a NULL buffer
	TVideoInputBuffer* inputBuffer=NULL;
	iParent->InfoMessage(_L("Call VideoHwDevice->WriteCodedDataL()"));
	TRAP(iError, iVideoHwDevice->WriteCodedDataL(inputBuffer) );
	if (iError!=KErrArgument)
		{
		iParent->InfoMessage(_L("WriteCodedDataL Failed"));
		
		}
	CleanupStack::PopAndDestroy(&decodeFormats);	
	
	iVideoHwDevice->Initialize();
	
	Cancel();

	}

void CPlayVideoFileCov::InitializeCov2()
	{
	
	
	
	TUncompressedVideoFormat reqFormat;
	reqFormat.iDataFormat = ERgbRawData;
	reqFormat.iRgbFormat = EFbsBitmapColor16M;
	
	RArray<TUncompressedVideoFormat> decodeFormats;
	TRAP(iError,iVideoHwDevice->GetOutputFormatListL(decodeFormats));
	CleanupClosePushL(decodeFormats);
	User::LeaveIfError(decodeFormats.Find(reqFormat));

	iParent->InfoMessage(_L("Call VideoHwDevice->SetOutputFormat()"));
	iVideoHwDevice->SetOutputFormatL(reqFormat);
	CleanupStack::PopAndDestroy(&decodeFormats);	
	
	iVideoHwDevice->Initialize();

	}
	


