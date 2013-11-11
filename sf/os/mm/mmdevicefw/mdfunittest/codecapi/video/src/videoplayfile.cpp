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
//

#include <mmf/devvideo/videoplayhwdevice.h>
#include <mmf/server/mmfdatabuffer.h>
#include "videoplayfile.h"

#include "../../PU/video/src/Plugin/VideoTestDecoderPU/videotestdecoderpu.hrh"
#include "../../PU/video/src/Plugin/VideoTestEncoderPU/videotestencoderpu.hrh"

const TInt KInputBufferSize = 8192;
const TInt KFilePositionZero = 0;
const TInt KTBufSize = 256;
const TInt KTRegionFixSize = 1;

// **************************************************
// instructs the Hw Device Adapter to play a file	
// **************************************************
CPlayVideoFile::CPlayVideoFile(RTestStepVideoCodecs* aParent) : 
	CActive(EPriorityNormal),
	iState(EHwDeviceInit),
	iParent(aParent)
	{
	}

void  CPlayVideoFile::ConstructL()
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

CPlayVideoFile* CPlayVideoFile::NewL(RTestStepVideoCodecs* aParent)
	{
	CPlayVideoFile* self = new (ELeave) CPlayVideoFile(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
	
void CPlayVideoFile::DoCancel()
	{
	}

CPlayVideoFile::~CPlayVideoFile()
	{
	iFile.Close();
	iFs.Close();
	
	delete iVideoHwDevice;
	delete iScreenDevice;
	RFbsSession::Disconnect();
	}
	
	
void CPlayVideoFile::LoadCodecL()	
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
	array.ResetAndDestroy();
	User::LeaveIfError(err);

	}
	
void CPlayVideoFile::SetDSA(TBool aUseDSA)	
	{
	// Need new version of this to load from a PU based uid
	iUseDSA = aUseDSA;
	}
	
void CPlayVideoFile::StartPlaybackL()	
	{
	OpenTestFileL();
	SetState(EHwDeviceInit);
	CActiveScheduler::Start();
	}

void CPlayVideoFile::SetState(TPlayVideoFileState aState)
	{
	iState = aState;
	SetActive();	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);	
	}

void CPlayVideoFile::OpenTestFileL()
	{
	TBuf<KTBufSize> buf;
	buf.Format(_L("Opening test input file %S"), &KTestPlaybackFile);
	iParent->InfoMessage(buf);
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFile.Open(iFs, KTestPlaybackFile, EFileRead));
	}

void CPlayVideoFile::RunL()
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
			CleanupAndSetDeviceError(_L("Unknown CPlayVideoFile iState"));
			break;
			}
		}
	}
	
	
void CPlayVideoFile::InitializeL()
	{
	iParent->InfoMessage(_L("InitializeL()"));

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
	
	
void CPlayVideoFile::StartDecodeL()
	{
	iParent->InfoMessage(_L("StartDecodeL()"));

	if (iUseDSA)
		{
		StartDirectScreenAccessL();
		}
	// tell the HwDeviceAdapter to play the file
	iParent->InfoMessage(_L("Call VideoHwDevice->Start()"));
	iVideoHwDevice->Start();
	}
	
	
void CPlayVideoFile::ReadNextBufferL()
	{
	// check that we aren't waiting for the input buffer to be returned
	iParent->InfoMessage(_L("ReadNextBufferL()"));
	TVideoInputBuffer* inputBuffer = iVideoHwDevice->GetBufferL(KInputBufferSize);	
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
		
void CPlayVideoFile::CleanupAndSetDeviceError(TPtrC16 aText)
	{
	iParent->SetVerdict(aText, EFail);
	delete iVideoHwDevice;
	SetState(EHwDeviceError);
	}
	
void CPlayVideoFile::MdvppNewPicture(TVideoPicture* aPicture)
	{
	iParent->InfoMessage(_L("MdvppNewPicture - Received Frame"));
	iVideoHwDevice->ReturnPicture(aPicture);
	iParent->InfoMessage(_L("MdvppNewPicture - finished returning Frame"));
	}

void CPlayVideoFile::MdvppNewBuffers()
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

void CPlayVideoFile::MdvppReturnPicture(TVideoPicture* /*aPicture*/)
	{
	iParent->InfoMessage(_L("MdvppReturnPicture()"));
	}
	
void CPlayVideoFile::MdvppSupplementalInformation(const TDesC8& /*aData*/, 
	const TTimeIntervalMicroSeconds& /*aTimestamp*/, const TPictureId& /*aPictureId*/)
	{
	}
	
void CPlayVideoFile::MdvppPictureLoss()
	{
	iParent->InfoMessage(_L("MdvppPictureLoss()"));
	}
	
void CPlayVideoFile::MdvppPictureLoss(const TArray<TPictureId>& /*aPictures*/)
	{
	iParent->InfoMessage(_L("MdvppPictureLoss()"));
	}
	
void CPlayVideoFile::MdvppSliceLoss(TUint /*aFirstMacroblock*/, TUint /*aNumMacroblocks*/, const TPictureId& /*aPicture*/)
	{
	iParent->InfoMessage(_L("MdvppSliceLoss()"));
	}
	
void CPlayVideoFile::MdvppReferencePictureSelection(const TDesC8& /*aSelectionData*/)
	{
	}
	
void CPlayVideoFile::MdvppTimedSnapshotComplete(TInt /*aError*/, TPictureData* /*aPictureData*/, 
	const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/, const TPictureId& /*aPictureId*/)
	{
	}
	
void CPlayVideoFile::MdvppFatalError(CMMFVideoHwDevice* /*aDevice*/, TInt aError)
	{
	iError = aError;
	iParent->InfoMessage(_L("MdvppFatalError()"));
	SetState(EHwDeviceError);
	}
	
void CPlayVideoFile::MdvppInitializeComplete(CMMFVideoHwDevice* /*aDevice*/, TInt aError)
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
	
void CPlayVideoFile::MdvppStreamEnd()
	{
	iParent->InfoMessage(_L("MdvppStreamEnd()"));
	SetState(EHwDeviceDone);
	}

void CPlayVideoFile::StartDirectScreenAccessL()
	{
	TRegionFix<KTRegionFixSize> reg;
	TRect pos(iFrameSize);
	reg.AddRect(pos);
	
	iVideoHwDevice->StartDirectScreenAccessL(pos, *iScreenDevice, reg);
	}
