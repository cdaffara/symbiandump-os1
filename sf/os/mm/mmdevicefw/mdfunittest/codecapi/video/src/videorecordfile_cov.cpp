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

#include <mmf/devvideo/videorecordhwdevice.h>
#include <imageconversion.h>
#include <fbs.h>
#include "videorecordfile_cov.h"
#include "../../PU/video/src/Plugin/VideoTestEncoderPU/videotestencoderpu.hrh"
#include "../../../../mdf/inc/codecapi/mdfoutputport.h"


const TInt KTBufSize = 256;
const TInt KChannelBitErrorLevel=1;
const TReal KChannelBitErrorRate=1.2;
const TReal KChannelBitErrorStdDeviation=1.2;
const TReal KMaxPictureRate = 10;
TEncoderBufferOptions  KOptions;
const TUint KLevel =1;
const TReal KLossRate=1.2;
const TTimeIntervalMicroSeconds32 KLossBurstLength;
TBufC8<8> buf1;
const TPtr8  KCodingOptionsPtr = buf1.Des();
TColorEnhancementOptions KColorEnhancementOptions;
const TUint KComplexityLevel=1;
const TPtr8  KCustomPreProcessOptions = buf1.Des();
const TPtr8 KImplementationSpecificOptions = buf1.Des();

const TUint KNumLevels=2;
const TUint KBitRate=2;
const TUint KStrength=2;
const TSize KOutputSize;
const TUint KMaxReferencePictures=4;
const TUint KMaxPictureOrderDelay=5;
const TRect KRect;
const TUint KLayer=1;
const TUint KPeriod=2;
const TReal KMinRandomAccessRate=1.223;
const TUint KNumBitLayers=4;
const TPoint KPicturePos;
const TUint32  KPreProcessTypes=0;
const TPictureId KTestPictureId1 = {TPictureId::ETemporalReference, 999};
const TPictureId KTestPictureId2 = {TPictureId::ETemporalReference, 666};
const TPictureId KTestPictureId3 = {TPictureId::ETemporalReference, 333};
TRgbRange KRgbRange ;
TYuvFormat KYuvOutput;
TRotationType KRotationType;
TScalabilityType KScalabilityType ;
const TUint KSizeBytes=1 ;
const TUint KSizeMacroblocks=0;
const TInt KCameraHandle=2;
TYuvFormat KYuvInput;
TInLayerScalabilityType KInLayerScalabilityType;












_LIT8(KVideoMimeType,"video/mpeg");
_LIT8(KBlank, "");
_LIT(KTestAnimatedGif, "c:\\mm\\mmf\\testfiles\\mdf\\earthrot.gif");


// **************************************************
// instructs the Hw Device Adapter to record a file	
// **************************************************
CRecordVideoFileCov::CRecordVideoFileCov(RTestStepVideoCodecs* aParent) : 
	CActive(EPriorityNormal),
	iState(EHwDeviceInit),
	iParent(aParent)
	{
	}

void  CRecordVideoFileCov::ConstructL()
	{
	CActiveScheduler::Add(this);
	}
		
void CRecordVideoFileCov::SetupSourceFileL()
	{
	TBuf<KTBufSize> buf;
	buf.Format(_L("Opening test source file %S"),&KTestAnimatedGif);
	iParent->InfoMessage(buf);
	iFs.Connect();	
	iImageDecoder = CImageDecoder::FileNewL(iFs, KTestAnimatedGif);
	iNumFrames = iImageDecoder->FrameCount();
	
	User::LeaveIfError(RFbsSession::Connect());
	
	// create an array to hold all the bitmaps to encode
	iBitmaps = new CFbsBitmap*[iNumFrames];
	for (TInt i = 0;i<iNumFrames;i++)
		{
		const TFrameInfo& frameInfo = iImageDecoder->FrameInfo(iCurrentFrame);
		iBitmaps[i] = new (ELeave) CFbsBitmap;
		User::LeaveIfError(iBitmaps[i]->Create(frameInfo.iOverallSizeInPixels, EColor16M));
		}
	
	iCurrentFrame = 0;
	}

CRecordVideoFileCov* CRecordVideoFileCov::NewL(RTestStepVideoCodecs* aParent)
	{
	CRecordVideoFileCov* self = new (ELeave) CRecordVideoFileCov(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CRecordVideoFileCov::LoadCodecL()	
	{
	// Need new version of this to load from a PU based uid
	RImplInfoPtrArray array;
	REComSession::ListImplementationsL(TUid::Uid(KUidMdfProcessingUnit), array);
	TBool found = EFalse;
	CImplementationInformation* info = NULL;
	for (TInt i=0;i<array.Count() && !found;i++)
		{
		info = array[i];
		if (info->ImplementationUid() == TUid::Uid(KUidVideoTestEncoderPu))
			{
			found = ETrue;
			}
		}
	TInt err = KErrNotFound;
	if (found)
		{
		TRAP(err, iVideoHwDevice = CMMFVideoEncodeHwDevice::NewPuAdapterL(*info,*this));
		}
	else
		{
		err = KErrNotFound;
		}
	array.ResetAndDestroy();
	User::LeaveIfError(err);
	}
	
	
void CRecordVideoFileCov::StartRecordL()	
	{
	SetupSourceFileL();
	SetupRecordFileL();
	SetState(EHwDeviceLoadImages);
	CActiveScheduler::Start();
	}
	
void CRecordVideoFileCov::DoCancel()
	{
	}

CRecordVideoFileCov::~CRecordVideoFileCov()
	{
	if (iBitmaps)
		{
		for (TInt i=0;i<iNumFrames; i++)
			{
			delete iBitmaps[i];
			}
		}
	
	delete iBitmaps;
	delete iVideoHwDevice;
	delete iImageDecoder;
	RFbsSession::Disconnect();
	}	

void CRecordVideoFileCov::SetState(TRecordVideoFileState aState)
	{
	iState = aState;
	SetActive();	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);	
	}
	
	
void CRecordVideoFileCov::EncodeNextFrameL()
	{
	TBuf<KTBufSize> buf;
	buf.Format(_L("Encoding frame #%d"),iCurrentFrame);
	iParent->InfoMessage(buf);
	iBitmaps[iCurrentFrame]->LockHeap(ETrue);
	TUint32* dataAddress = iBitmaps[iCurrentFrame]->DataAddress();
	TSize theSize = iBitmaps[iCurrentFrame]->SizeInPixels();
	TInt frameSize = theSize.iHeight * theSize.iWidth * 3;
	TPictureData& thePictureData = iVideoPicture.iData;
	TPtr8 theRawData((TUint8*)dataAddress, frameSize, frameSize);
	thePictureData.iDataSize = theSize;
	thePictureData.iRawData = &theRawData;

	iParent->InfoMessage(_L("call WritePicture"));
	//make the size zero
	//call writepicture and expect KErrArgument
	TSize zeroSize(0,0);
	TVideoPicture picture;
	
	picture.iData.iDataSize=zeroSize;
	TRAPD(err, iVideoHwDevice->WritePictureL(&picture));
	iBitmaps[iCurrentFrame]->UnlockHeap(ETrue);
	if(err != KErrArgument) 
		{
		User::Leave(err);
		}
	iCurrentFrame++;
	}

void CRecordVideoFileCov::WriteDataToFile(const TDesC8& aData)
	{		
	iParent->InfoMessage(_L("Write Data To File"));
	iRecordFile.Write(aData);
	// keep record of amount of data and the number of buffers written out
	iWrittenDataTotal += aData.Size();	
	iBuffersWrittenCount++;	
	TBuf<KTBufSize> buf;
	buf.Format(_L("Buffer Size: %d Bytes Written: %d Buffers Written: %d"),
				aData.Size(),
				iWrittenDataTotal,
				iBuffersWrittenCount);
	iParent->InfoMessage(buf);
	}
	
void CRecordVideoFileCov::SetupRecordFileL()
	{
	iParent->InfoMessage(_L("creating the file to record to"));
	User::LeaveIfError(iFileServerSession.Connect());
	User::LeaveIfError(iRecordFile.Replace(iFileServerSession, KTestRecordedFile, EFileWrite|EFileShareExclusive));	
	}

void CRecordVideoFileCov::CloseRecordFile()
	{
	iRecordFile.Close();
	iFileServerSession.Close();	
	}

void CRecordVideoFileCov::RunL()
	{
	TBuf<KTBufSize> buf;
	switch (iState)
		{		
		case EHwDeviceLoadImages:			
			buf.Format(_L("Loading test frame %d"),iCurrentFrame);
			iParent->InfoMessage(buf);
			if (iCurrentFrame<iNumFrames)
				{
				iImageDecoder->Convert(&iStatus, *iBitmaps[iCurrentFrame], iCurrentFrame);
				SetActive();
				iCurrentFrame++;
				}
			else
				{
				iCurrentFrame = 0;
				SetState(EHwDeviceInit);			
				}			
			break;
		case EHwDeviceInit:
			{	
			iParent->InfoMessage(_L("State EHwDeviceInit"));
			// Configure Encoder HwDevice
			TSize size = iBitmaps[iCurrentFrame]->SizeInPixels();
		
			// set video format.
			TUncompressedVideoFormat videoFormat;
			videoFormat.iDataFormat = ERgbFbsBitmap;
			videoFormat.iRgbFormat = EFbsBitmapColor16M;

			iParent->InfoMessage(_L("SetInputFormat"));
			// set encoder settings
			TRAP(iError, iVideoHwDevice->SetInputFormatL(videoFormat, size) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetInputFormat Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetSourceMemory"));
			TRAP(iError, iVideoHwDevice->SetSourceMemoryL(KMaxPictureRate, ETrue, ETrue) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetSourceMemory Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetChannelBitErrorRate"));
			TRAP(iError, iVideoHwDevice->SetChannelBitErrorRate(KChannelBitErrorLevel, KChannelBitErrorRate, KChannelBitErrorStdDeviation) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetChannelBitErrorRate Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetBufferOptions"));
			TRAP(iError, iVideoHwDevice->SetBufferOptionsL(KOptions) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetBufferOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetChannelPacketLossRate"));
			TRAP(iError, iVideoHwDevice->SetChannelPacketLossRate(KLevel,KLossRate,KLossBurstLength) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetChannelPacketLossRate Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetCodingStandardSpecificOptionsL"));
			TRAP(iError, iVideoHwDevice->SetCodingStandardSpecificOptionsL(KCodingOptionsPtr) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetCodingStandardSpecificOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetColorEnhancementOptionsL"));
			TRAP(iError, iVideoHwDevice->SetColorEnhancementOptionsL(KColorEnhancementOptions) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetColorEnhancementOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetComplexityLevel"));
			TRAP(iError, iVideoHwDevice->SetComplexityLevel(KComplexityLevel) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetComplexityLevel Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetCustomPreProcessOptionsL"));
			TRAP(iError, iVideoHwDevice->SetCustomPreProcessOptionsL(KCustomPreProcessOptions) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetCustomPreProcessOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetErrorProtectionLevelL"));
			TRAP(iError, iVideoHwDevice->SetErrorProtectionLevelL(KLevel,KBitRate,KStrength) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetErrorProtectionLevelL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetErrorProtectionLevelsL"));
			TRAP(iError, iVideoHwDevice->SetErrorProtectionLevelsL(KNumLevels,ETrue) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetErrorProtectionLevelsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetErrorsExpected"));
			TRAP(iError, iVideoHwDevice->SetErrorsExpected(ETrue,ETrue) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetErrorsExpected Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetFrameStabilisationOptionsL"));
			TRAP(iError, iVideoHwDevice->SetFrameStabilisationOptionsL(KOutputSize,ETrue) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetFrameStabilisationOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetGlobalReferenceOptions"));
			TRAP(iError, iVideoHwDevice->SetGlobalReferenceOptions(KMaxReferencePictures, KMaxPictureOrderDelay) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetGlobalReferenceOptions Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetInputCropOptionsL"));
			TRAP(iError, iVideoHwDevice->SetInputCropOptionsL(KRect) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetInputCropOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetLayerPromotionPointPeriod"));
			TRAP(iError, iVideoHwDevice->SetLayerPromotionPointPeriod(KLayer,KPeriod) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetLayerPromotionPointPeriod Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetLayerReferenceOptions"));
			TRAP(iError, iVideoHwDevice->SetLayerReferenceOptions(KLayer, KMaxReferencePictures , KMaxPictureOrderDelay ) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetLayerReferenceOptions Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetMinRandomAccessRate"));
			TRAP(iError, iVideoHwDevice->SetMinRandomAccessRate(KMinRandomAccessRate ) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetMinRandomAccessRate Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetNumBitrateLayersL"));
			TRAP(iError, iVideoHwDevice->SetNumBitrateLayersL(KNumBitLayers) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetNumBitrateLayersL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetOutputCropOptionsL"));
			TRAP(iError, iVideoHwDevice->SetOutputCropOptionsL(KRect) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetOutputCropOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetOutputPadOptionsL"));
			TRAP(iError, iVideoHwDevice->SetOutputPadOptionsL(KOutputSize, KPicturePos) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetOutputPadOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetOutputRectL"));
			TRAP(iError, iVideoHwDevice->SetOutputRectL(KRect) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetOutputRectL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetPreProcessTypesL"));
			TRAP(iError, iVideoHwDevice->SetPreProcessTypesL(KPreProcessTypes) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetPreProcessTypesL Failed"));
				SetState(EHwDeviceError);
				}
			TRateControlOptions KRateOptions;
			iParent->InfoMessage(_L("SetRateControlOptions"));
			TRAP(iError, iVideoHwDevice->SetRateControlOptions(KLayer, KRateOptions) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetRateControlOptions Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetRgbToYuvOptionsL"));
			TRAP(iError, iVideoHwDevice->SetRgbToYuvOptionsL(KRgbRange, KYuvOutput) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetRgbToYuvOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetRotateOptionsL"));
			TRAP(iError, iVideoHwDevice->SetRotateOptionsL(KRotationType) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetRotateOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetScalabilityLayerTypeL"));
			TRAP(iError, iVideoHwDevice->SetScalabilityLayerTypeL(KLayer,KScalabilityType ) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetScalabilityLayerTypeL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetScaleOptionsL"));
			TRAP(iError, iVideoHwDevice->SetScaleOptionsL(KOutputSize,EFalse ) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetScaleOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetSegmentTargetSize"));
			TRAP(iError, iVideoHwDevice->SetSegmentTargetSize(KLayer,KSizeBytes, KSizeMacroblocks ) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetSegmentTargetSize Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetSourceCameraL"));
			TRAP(iError, iVideoHwDevice->SetSourceCameraL(KCameraHandle,KMaxPictureRate ) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetSourceCameraL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetYuvToYuvOptionsL"));
			TRAP(iError, iVideoHwDevice->SetYuvToYuvOptionsL(KYuvInput,KYuvOutput ) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetYuvToYuvOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("SetImplementationSpecificEncoderOptionsL"));
			TRAP(iError, iVideoHwDevice->SetImplementationSpecificEncoderOptionsL(KImplementationSpecificOptions ) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetImplementationSpecificEncoderOptionsL Failed"));
				SetState(EHwDeviceError);
				}
			 CCompressedVideoFormat* Format=CCompressedVideoFormat::NewL(KVideoMimeType,KBlank) ;
			 TVideoDataUnitType DataUnitType=EDuCodedPicture ; 					
			 TVideoDataUnitEncapsulation DataEncapsulation=EDuElementaryStream;
			  
			iParent->InfoMessage(_L("SetOutputFormatL"));
			TRAP(iError, iVideoHwDevice->SetOutputFormatL(*Format,DataUnitType,DataEncapsulation,ETrue ) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetOutputFormatL Failed"));
				SetState(EHwDeviceError);
				}
			delete Format;
			iParent->InfoMessage(_L("Initialize HwDevice"));
			iVideoHwDevice->Initialize();
			break;
			}					
		case EHwDeviceStartEncode:
			{
			TRect Rect;
			iParent->InfoMessage(_L("GetFrameStabilisationOutput"));
			TRAP(iError, iVideoHwDevice->GetFrameStabilisationOutput(Rect ) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("GetFrameStabilisationOutput Failed"));
				SetState(EHwDeviceError);
				}
			TUint NumFreeBuffers, TotalFreeBytes;
			iParent->InfoMessage(_L("GetOutputBufferStatus"));
			TRAP(iError, iVideoHwDevice->GetOutputBufferStatus(NumFreeBuffers, TotalFreeBytes ) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("GetOutputBufferStatus Failed"));
				SetState(EHwDeviceError);
				}
			CMMFDevVideoRecord::TPictureCounters Counters;
			iParent->InfoMessage(_L("GetPictureCounters"));
			TRAP(iError, iVideoHwDevice->GetPictureCounters(Counters ) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("GetPictureCounters Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("NumComplexityLevels"));
			TRAP(iError, iVideoHwDevice->NumComplexityLevels() );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("NumComplexityLevels Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("NumComplexityLevels"));
			TRAP(iError, iVideoHwDevice->NumComplexityLevels() );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("NumComplexityLevels Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("PictureLoss"));
			TRAP(iError, iVideoHwDevice->PictureLoss() );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("PictureLoss Failed"));
				SetState(EHwDeviceError);
				}
			
			iParent->InfoMessage(_L("PreProcessorInfoLC"));
			TRAP(iError, iVideoHwDevice->PreProcessorInfoLC() );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("PreProcessorInfoLC Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("CancelSupplementalInfo"));
			TRAP(iError, iVideoHwDevice->CancelSupplementalInfo() );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("CancelSupplementalInfo Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("CancelSupplementalInfo"));
			TRAP(iError, iVideoHwDevice->CancelSupplementalInfo() );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("CancelSupplementalInfo Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("CodingStandardSpecificInitOutputLC"));
			TRAP(iError, iVideoHwDevice->CodingStandardSpecificInitOutputLC() );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("CodingStandardSpecificInitOutputLC Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("ImplementationSpecificInitOutputLC"));
			TRAP(iError, iVideoHwDevice->ImplementationSpecificInitOutputLC() );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("ImplementationSpecificInitOutputLC Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("CodingStandardSpecificSettingsOutputLC"));
			TRAP(iError, iVideoHwDevice->CodingStandardSpecificSettingsOutputLC() );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("CodingStandardSpecificSettingsOutputLC Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("ImplementationSpecificSettingsOutputLC"));
			TRAP(iError, iVideoHwDevice->ImplementationSpecificSettingsOutputLC() );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("ImplementationSpecificSettingsOutputLC Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("CommitL"));
			TRAP(iError, iVideoHwDevice->CommitL() );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("CommitL Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("Revert"));
			TRAP(iError, iVideoHwDevice->Revert() );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("Revert Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("InputEnd"));
			TRAP(iError, iVideoHwDevice->InputEnd() );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("InputEnd Failed"));
				SetState(EHwDeviceError);
				}
			TBufC8<8> buffer;
			TPtr8  PictureSelection = buffer.Des();
			iParent->InfoMessage(_L("ReferencePictureSelection"));
			TRAP(iError, iVideoHwDevice->ReferencePictureSelection(PictureSelection) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("ReferencePictureSelection Failed"));
				SetState(EHwDeviceError);
				}
			TPtr8  SupplementalInfo = buffer.Des();
			iParent->InfoMessage(_L("SendSupplementalInfoL"));
			TRAP(iError, iVideoHwDevice->SendSupplementalInfoL(SupplementalInfo) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SendSupplementalInfoL Failed"));
				SetState(EHwDeviceError);
				}
			TTimeIntervalMicroSeconds TimeStamp;
			iParent->InfoMessage(_L("SendSupplementalInfoL"));
			TRAP(iError, iVideoHwDevice->SendSupplementalInfoL(SupplementalInfo,TimeStamp) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SendSupplementalInfoL Failed"));
				SetState(EHwDeviceError);
				}
			
			CVideoEncoderInfo* EncoderInfo=NULL;
			iParent->InfoMessage(_L("VideoEncoderInfoLC"));
			TRAP(iError, EncoderInfo=iVideoHwDevice->VideoEncoderInfoLC();CleanupStack::Pop(EncoderInfo) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("VideoEncoderInfoLC Failed"));
				SetState(EHwDeviceError);
				}
			delete EncoderInfo;
			
			iParent->InfoMessage(_L("Freeze"));
			TRAP(iError, iVideoHwDevice->Freeze());
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("Freeze Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("ReleaseFreeze"));
			TRAP(iError, iVideoHwDevice->ReleaseFreeze());
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("ReleaseFreeze Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("Pause"));
			TRAP(iError, iVideoHwDevice->Pause());
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("Pause Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("Resume"));
			TRAP(iError, iVideoHwDevice->Resume());
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("Resume Failed"));
				SetState(EHwDeviceError);
				}
			TTimeIntervalMicroSeconds Pos;
			iParent->InfoMessage(_L("RecordingPosition"));
			TRAP(iError, Pos=iVideoHwDevice->RecordingPosition());
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("RecordingPosition Failed"));
				SetState(EHwDeviceError);
				}
			iParent->InfoMessage(_L("Stop"));
			TRAP(iError, iVideoHwDevice->Stop());
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("Stop Failed"));
				SetState(EHwDeviceError);
				}
			TUint FirstMacroblock=1, NumMacroblocks=2;
			TPictureId picId;
			iParent->InfoMessage(_L("SliceLoss"));
			TRAP(iError, iVideoHwDevice->SliceLoss(FirstMacroblock,NumMacroblocks,picId));
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SliceLoss Failed"));
				SetState(EHwDeviceError);
				}
			RArray<TPictureId> testIds;
			User::LeaveIfError(testIds.Append(KTestPictureId1));
			User::LeaveIfError(testIds.Append(KTestPictureId2));
			User::LeaveIfError(testIds.Append(KTestPictureId3));
			TArray<TPictureId> testIds2=testIds.Array();
			iParent->InfoMessage(_L("PictureLoss"));
			TRAP(iError, iVideoHwDevice->PictureLoss(testIds2) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("PictureLoss Failed"));
				SetState(EHwDeviceError);
				}
			testIds.Close();
			CSystemClockSource *time1 = NULL;
			iParent->InfoMessage(_L("SetClockSource"));
			TRAP(iError, iVideoHwDevice->SetClockSource(time1) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetClockSource Failed"));
				SetState(EHwDeviceError);
				}
			CMMFVideoPreProcHwDevice *inpDevice = NULL;
			iParent->InfoMessage(_L("SetInputDevice"));
			TRAP(iError, iVideoHwDevice->SetInputDevice(inpDevice) );
			if (iError!=KErrNone)
				{
				iParent->InfoMessage(_L("SetInputDevice Failed"));
				SetState(EHwDeviceError);
				}
			TUint layer=1;
			TUint numSteps=2;
			TInLayerScalabilityType scalabilityType=EInLScalabilityTemporal;
			RArray<TUint> bitRate;
			User::LeaveIfError(bitRate.Append(0));
			const TArray<TUint> bitRate2=bitRate.Array();
			iParent->InfoMessage(_L("SetInLayerScalabilityL"));
			TRAP(iError, iVideoHwDevice->SetInLayerScalabilityL(layer,numSteps,scalabilityType,bitRate2,bitRate2) );
			if (iError!=KErrNotSupported)
				{
				iParent->InfoMessage(_L("SetInLayerScalabilityL Failed"));
				SetState(EHwDeviceError);
				}
			bitRate.Close();
			iVideoHwDevice->Start();				
			TRAP(iError, EncodeNextFrameL());
			if (iError!=KErrNone)
				{
				SetState(EHwDeviceError);
				}

			break;
			}			
		case EHwDeviceEncodeNextFrame:
			{
			if (iCurrentFrame<iNumFrames)
				{
				
				TRAP(iError, EncodeNextFrameL());
				if (iError!=KErrNone)
					{
					SetState(EHwDeviceError);
					}
				}
			else
				{
				iParent->InfoMessage(_L("End of input frames"));

				iVideoHwDevice->InputEnd();
				}
			break;
			}		
		case EHwDeviceEncodeCheckData:			
			{
			SetState(EHwDeviceDone);
			break;
			}
		case EHwDeviceAllowToComplete:
			break;
		case EHwDeviceDone:
			{
			Cancel();
			CActiveScheduler::Stop();
			CloseRecordFile();
			break;
			}
		case EHwDeviceError:
			{
			buf.Format(_L("HwDevice Error %d"),iError);
			iParent->SetVerdict(buf, EFail);
			CActiveScheduler::Stop();
			CloseRecordFile();
			break;
			}
		default:
			{
			CleanupAndSetDeviceError(_L("Unknown CRecordVideoFileCov iState"));
			break;
			}
		}
	}


void CRecordVideoFileCov::CleanupAndSetDeviceError(TPtrC16 aText)
	{
	iParent->SetVerdict(aText, EFail);
	delete iVideoHwDevice;
	SetState(EHwDeviceError);
	}
	
void CRecordVideoFileCov::MdvrpReturnPicture(TVideoPicture* /*aPicture*/)
	{
	iParent->InfoMessage(_L("Picture buffer returned"));
	SetState(EHwDeviceEncodeNextFrame);
	}
	
void CRecordVideoFileCov::MdvrpSupplementalInfoSent()
	{
	}

void CRecordVideoFileCov::MdvrpNewBuffer(TVideoOutputBuffer* aOutputBuffer)
	{
	TBuf<KTBufSize> buf;
	buf.Format(_L("Output Buffer Returned: Size %d"), aOutputBuffer->iData.Size());
	iParent->InfoMessage(buf);
	WriteDataToFile(aOutputBuffer->iData);
	iVideoHwDevice->ReturnBuffer(aOutputBuffer);
	}
	
void CRecordVideoFileCov::MdvrpFatalError(CMMFVideoHwDevice* /*aHwDevice*/, TInt /*aError*/)
	{
	}
	
void CRecordVideoFileCov::MdvrpInitializeComplete(CMMFVideoHwDevice* /*aHwDevice*/, TInt aError)
	{
	iParent->InfoMessage(_L("MdvrpInitializeComplete"));
	iError = aError;
	if (iError==KErrNone)
		{
		iParent->InfoMessage(_L("Successful initialization, start encoding"));
		SetState(EHwDeviceStartEncode);			
		}
	else
		{
		iParent->InfoMessage(_L("Initialization failed"));
		SetState(EHwDeviceError);
		}
	}

void CRecordVideoFileCov::MdvrpStreamEnd()
	{
	iParent->InfoMessage(_L("Received End of Stream"));
	SetState(EHwDeviceEncodeCheckData);	
	}

void CRecordVideoFileCov::NegTests1()
	{
	TUncompressedVideoFormat videoFormat;
	videoFormat.iDataFormat = EYuvRawData;
	videoFormat.iRgbFormat = EFbsBitmapColor16M;
	TSize size; 
	iParent->InfoMessage(_L("SetInputFormat"));
	// set encoder settings
	TRAP(iError, iVideoHwDevice->SetInputFormatL(videoFormat, size) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("SetInputFormat Failed"));
		SetState(EHwDeviceError);
		}
	iVideoHwDevice->Initialize();
	Cancel();
	
	
	}

void CRecordVideoFileCov::NegTests2()
	{
	TUncompressedVideoFormat videoFormat;
	videoFormat.iDataFormat = ERgbRawData;
	videoFormat.iRgbFormat = EFbsBitmapColor16M;
	TSize size; 
	iParent->InfoMessage(_L("SetInputFormat"));
	// set encoder settings
	TRAP(iError, iVideoHwDevice->SetInputFormatL(videoFormat, size) );
	if (iError!=KErrNone)
		{
		iParent->InfoMessage(_L("SetInputFormat Failed"));
		SetState(EHwDeviceError);
		}
	iVideoHwDevice->Initialize();
	Cancel();
	

	}

void CRecordVideoFileCov::NegTests3()
	{
	
	
//	iFormat.iDataFormat = (TImageDataFormat)0
	
	TVideoPicture* ptrVideoPicture=NULL;
	TRAPD(err, iVideoHwDevice->WritePictureL(ptrVideoPicture));
	
	if(err != KErrArgument) 
		{
		SetState(EHwDeviceError);
		}
	iCurrentFrame++;
	

	}

