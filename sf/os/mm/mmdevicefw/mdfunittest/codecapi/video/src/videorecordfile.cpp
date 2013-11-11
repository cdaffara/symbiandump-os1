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

#include <mmf/devvideo/videorecordhwdevice.h>
#include <imageconversion.h>
#include <fbs.h>
#include "videorecordfile.h"
#include "../../PU/video/src/Plugin/VideoTestEncoderPU/videotestencoderpu.hrh"

const TInt KTBufSize = 256;
const TReal KMaxPictureRate = 10;
_LIT(KTestAnimatedGif, "c:\\mm\\mmf\\testfiles\\mdf\\earthrot.gif");


// **************************************************
// instructs the Hw Device Adapter to record a file	
// **************************************************
CRecordVideoFile::CRecordVideoFile(RTestStepVideoCodecs* aParent) : 
	CActive(EPriorityNormal),
	iState(EHwDeviceInit),
	iParent(aParent)
	{
	}

void  CRecordVideoFile::ConstructL()
	{
	CActiveScheduler::Add(this);
	}
		
void CRecordVideoFile::SetupSourceFileL()
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

CRecordVideoFile* CRecordVideoFile::NewL(RTestStepVideoCodecs* aParent)
	{
	CRecordVideoFile* self = new (ELeave) CRecordVideoFile(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CRecordVideoFile::LoadCodecL()	
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
	
	
void CRecordVideoFile::StartRecordL()	
	{
	SetupSourceFileL();
	SetupRecordFileL();
	SetState(EHwDeviceLoadImages);
	CActiveScheduler::Start();
	}
	
void CRecordVideoFile::DoCancel()
	{
	}

CRecordVideoFile::~CRecordVideoFile()
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

void CRecordVideoFile::SetState(TRecordVideoFileState aState)
	{
	iState = aState;
	SetActive();	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);	
	}
	
	
void CRecordVideoFile::EncodeNextFrameL()
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
	TRAPD(err, iVideoHwDevice->WritePictureL(&iVideoPicture));
	iBitmaps[iCurrentFrame]->UnlockHeap(ETrue);
	if(err != KErrNone) 
		{
		User::Leave(err);
		}
	iCurrentFrame++;
	}

void CRecordVideoFile::WriteDataToFile(const TDesC8& aData)
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
	
void CRecordVideoFile::SetupRecordFileL()
	{
	iParent->InfoMessage(_L("creating the file to record to"));
	User::LeaveIfError(iFileServerSession.Connect());
	User::LeaveIfError(iRecordFile.Replace(iFileServerSession, KTestRecordedFile, EFileWrite|EFileShareExclusive));	
	}

void CRecordVideoFile::CloseRecordFile()
	{
	iRecordFile.Close();
	iFileServerSession.Close();	
	}

void CRecordVideoFile::RunL()
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
				
			iParent->InfoMessage(_L("Initialize HwDevice"));
			iVideoHwDevice->Initialize();
			break;
			}					
		case EHwDeviceStartEncode:
			{		
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
			CleanupAndSetDeviceError(_L("Unknown CRecordVideoFile iState"));
			break;
			}
		}
	}


void CRecordVideoFile::CleanupAndSetDeviceError(TPtrC16 aText)
	{
	iParent->SetVerdict(aText, EFail);
	delete iVideoHwDevice;
	SetState(EHwDeviceError);
	}
	
void CRecordVideoFile::MdvrpReturnPicture(TVideoPicture* /*aPicture*/)
	{
	iParent->InfoMessage(_L("Picture buffer returned"));
	SetState(EHwDeviceEncodeNextFrame);
	}
	
void CRecordVideoFile::MdvrpSupplementalInfoSent()
	{
	}

void CRecordVideoFile::MdvrpNewBuffer(TVideoOutputBuffer* aOutputBuffer)
	{
	TBuf<KTBufSize> buf;
	buf.Format(_L("Output Buffer Returned: Size %d"), aOutputBuffer->iData.Size());
	iParent->InfoMessage(buf);
	WriteDataToFile(aOutputBuffer->iData);
	iVideoHwDevice->ReturnBuffer(aOutputBuffer);
	}
	
void CRecordVideoFile::MdvrpFatalError(CMMFVideoHwDevice* /*aHwDevice*/, TInt /*aError*/)
	{
	}
	
void CRecordVideoFile::MdvrpInitializeComplete(CMMFVideoHwDevice* /*aHwDevice*/, TInt aError)
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

void CRecordVideoFile::MdvrpStreamEnd()
	{
	iParent->InfoMessage(_L("Received End of Stream"));
	SetState(EHwDeviceEncodeCheckData);	
	}

