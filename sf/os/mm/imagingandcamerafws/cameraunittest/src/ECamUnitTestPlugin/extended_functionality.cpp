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
// extendedfunctionality.cpp
// 
//

#include "extended_functionality.h"
#include "AdvancedSettings.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamcapturecontrolconst.h>
#include <ecamviewfinderdef.h>
#include "ecamdef.h"
#endif

//
//pre image capture control impl
CCamPreImageCaptureControl* CCamPreImageCaptureControl::NewL(CCamUnitTestPlugin& aOwner)
	{
	return new(ELeave) CCamPreImageCaptureControl(aOwner);
	}

CCamPreImageCaptureControl::CCamPreImageCaptureControl(CCamUnitTestPlugin& aOwner): iOwner(aOwner),
																					iPreImageCaptureControlObserver(NULL)
	{
	}
	
CCamPreImageCaptureControl::~CCamPreImageCaptureControl()
	{
	}
	
void CCamPreImageCaptureControl::Release()
	{	
	delete this;
	}
	
void CCamPreImageCaptureControl::SetPreImageCaptureControlObserver(MPreImageCaptureControlObserver& aPreImageCaptureControlObserver)
	{
	iPreImageCaptureControlObserver = &aPreImageCaptureControlObserver;
	}
	
void CCamPreImageCaptureControl::GetDirectSnapshotSupportInfoL(TUint& aDirectSnapshotSupportInfo) const
	{
	aDirectSnapshotSupportInfo = CCamera::CCameraPreImageCaptureControl::EDirectSnapshotSupported;
	}
	
void CCamPreImageCaptureControl::GetSupportedEmbeddedStillCaptureSettingsL(RArray<TUid>& aSupportedEmbeddedStillCaptureSettings) const
	{
	aSupportedEmbeddedStillCaptureSettings.Reset();
	}
	
void CCamPreImageCaptureControl::GetSupportedDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::TDirectSavingType& aSupportedDirectSavingType) const
	{
	aSupportedDirectSavingType = CCamera::CCameraPreImageCaptureControl::EDirectSavingNotUsed;
	}
	
void CCamPreImageCaptureControl::SetSequentialImageFilenameL(const TDesC8& /*aFilename*/, TInt /*aStartingSequenceNumber*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamPreImageCaptureControl::SetLowerResolutionSequentialImageFilenameL(const TDesC8& /*aLowerResolutionFilename*/, TInt /*aStartingSequenceNumber*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamPreImageCaptureControl::GetDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::TDirectSavingType& aDirectSavingType) const
	{
	aDirectSavingType = CCamera::CCameraPreImageCaptureControl::EDirectSavingNotUsed;	
	}
	
void CCamPreImageCaptureControl::SetDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::TDirectSavingType /*aDirectSavingType*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamPreImageCaptureControl::GetCaptureEventSupportInfoL(TUint& aSupportedDriveModes) const
	{
	aSupportedDriveModes = 0; 	
	}
	
void CCamPreImageCaptureControl::GetImageFormatsSupportedL(TUint& aImageFormatsSupported, const TSize& /*aSize*/) const
	{
	aImageFormatsSupported = CCamera::EFormatFbsBitmapColor16M;
	}
	
void CCamPreImageCaptureControl::GetPixelAspectsSupportedL(TUint& aPixelAspectsSupported, CCamera::TFormat /*aImageFormat*/, const TSize& /*aSize*/) const
	{
	aPixelAspectsSupported = CCamera::CCameraAdvancedSettings::EPixelAspect1To1;
	}
	
void CCamPreImageCaptureControl::PrepareImageCapture(const CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& 
								aPrepareImageParameters, MCaptureImageObserver& aCaptureImageObserver)
	{
	//perform any prepare operation at hardware level eg. memory allocation etc.
	CCamera::CCameraImageCapture* cameraImageCapture = NULL;
	
	TRAPD(err,CreateCaptureImageL(cameraImageCapture, aPrepareImageParameters, aCaptureImageObserver));
	
	PreImageCaptureControlObserver()->PrepareImageComplete(cameraImageCapture, err);
	}
	
void CCamPreImageCaptureControl::GetImageMaxMemorySizeSettingSupportInfoL(TBool& aIsImageMaxMemorySizeSettingSupported) const
	{
	aIsImageMaxMemorySizeSettingSupported = EFalse;
	}
	
void CCamPreImageCaptureControl::GetImageMaxMemorySizeL(TUint& /*aMemorySize*/) const
	{
	User::Leave(KErrNotSupported);
	}

void CCamPreImageCaptureControl::GetSupportedProcessingOptionsL(TUint& /*aSupportedProcessingOptions*/) const
	{
	User::Leave(KErrNotSupported);
	}

void CCamPreImageCaptureControl::CreateCaptureImageL(CCamera::CCameraImageCapture*& aCameraImageCapture, const CCamera::
		CCameraPreImageCaptureControl::TPrepareImageParameters& aPrepareImageParameters, MCaptureImageObserver& aCaptureImageObserver)
	{
	CCamera::CCameraImageCapture* cameraImageCapture = CCamera::CCameraImageCapture::CreateL(iOwner, aPrepareImageParameters, aCaptureImageObserver);
	aCameraImageCapture = cameraImageCapture;
	}

//
//image capture impl
CCamImageCapture* CCamImageCapture::NewL(CCamUnitTestPlugin& aOwner)
	{
	return new(ELeave) CCamImageCapture(aOwner);
	}
	
CCamImageCapture::CCamImageCapture(CCamUnitTestPlugin& aOwner) : iOwner(aOwner), 
																 iCaptureImageObserver(NULL), 
																 iSnapshotImpForImage(NULL),
																 iCamImgProcImpForImage(NULL),
																 iCameraImageCapture(NULL)
	{
	iArrayPostCaptureControlId.Reset();
	iArrayPostCaptureControlHandle.Reset();
	iArrayPostCaptureControlImpl.Reset();
	}
	
CCamImageCapture::~CCamImageCapture()
	{
	iArrayPostCaptureControlId.Close();
	iArrayPostCaptureControlHandle.Reset();
	iArrayPostCaptureControlImpl.Reset();
	iArrayPostCaptureControlHandle.Close();
	iArrayPostCaptureControlImpl.Close();
	}
	
void CCamImageCapture::Release(CCamera::CCameraImageCapture* /*aCameraImageCapture*/)
	{
	delete this;
	}
	
void CCamImageCapture::CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr	= CHistogramFactoryForImage::NewL(const_cast<CCamImageCapture&>(*this));
	}
	
void CCamImageCapture::GetSnapshotImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr = CSnapshotFactoryForImage::NewL(const_cast<CCamImageCapture&>(*this), iSnapshotImpForImage);
	}
	
void CCamImageCapture::GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr = CImgProcFactoryForImage::NewL(const_cast<CCamImageCapture&>(*this), iCamImgProcImpForImage);
	}
	
void CCamImageCapture::SetCaptureImageObserver(MCaptureImageObserver& aCaptureImageObserver)
	{
	iCaptureImageObserver = &aCaptureImageObserver;
	}

void CCamImageCapture::GetPrepareImageParametersL(CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& aPrepareImageParameters) const
	{
	if(aPrepareImageParameters.Size() != sizeof(CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters) ||
		aPrepareImageParameters.Version() != KECamPrepareImageParametersCurrentVersion)
		{
		User::Leave(KErrNotSupported);
		}

	if(aPrepareImageParameters.iDriveModeAttributes.Size() != sizeof(CCamera::CCameraPreImageCaptureControl::TDriveModeDependentAttributes) ||
		aPrepareImageParameters.iDriveModeAttributes.Version() != KECamDriveModeDependentAttributesCurrentVersion)
		{
		User::Leave(KErrNotSupported);
		}

	aPrepareImageParameters = iPrepareImageParameters;
	}
	
void CCamImageCapture::SetPrepareImageParameters(const CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& aPrepareImageParameters)
	{
	iPrepareImageParameters = aPrepareImageParameters;
	}
	
TAny* CCamImageCapture::CreatePostImageCaptureControlImpl(TUid aInterface, TPostCaptureControlId aPostCaptureControlId)
	{
	TInt err=KErrNone;
	if(aInterface.iUid == KECamMCameraPostImageCaptureControlUidValue)
		{
		if(iArrayPostCaptureControlId.Find(aPostCaptureControlId) == KErrNotFound)
			{
			CCamPostImageCaptureControl* camPostImageCaptureControl = NULL;
			TRAP(err, camPostImageCaptureControl = CCamPostImageCaptureControl::NewL(const_cast<CCamImageCapture&>(*this)));
			
			if (err == KErrNone)
				{
				err = iArrayPostCaptureControlId.Append(aPostCaptureControlId);
				if(err == KErrNone)
					{
					err = iArrayPostCaptureControlImpl.Append(camPostImageCaptureControl);
					if(err != KErrNone)
						{
						iArrayPostCaptureControlId.Remove(iArrayPostCaptureControlId.Count()-1);
						delete camPostImageCaptureControl;
						return NULL;
						}
					else
						{
						return static_cast<MCameraPostImageCaptureControl*>(camPostImageCaptureControl);	
						}
					}
				else
					{
					delete camPostImageCaptureControl;
					return NULL;	
					}
				}
			else
				{
				return NULL;
				}
			}
		else
			{
			return NULL;
			}
		}
	else
		{
		return NULL;
		}	
	}
	
void CCamImageCapture::CaptureImage(CCamera::CCameraImageCapture* aCameraImageCapture)
	{
	TInt count = iArrayPostCaptureControlId.Count();
	while(count > 0)
		{
		iArrayPostCaptureControlId.Remove(count-1);
		iArrayPostCaptureControlImpl.Remove(count-1);
		delete iArrayPostCaptureControlHandle[count-1];
		iArrayPostCaptureControlHandle.Remove(count-1);
				
		count = iArrayPostCaptureControlId.Count();
		}
	
	iCameraImageCapture = aCameraImageCapture;
	
	//this will create Post Image capture control object. assume one such object is required. ( for demo purpose only)
	CCamera::CCameraPostImageCaptureControl* postImageCaptureControl = NULL;
	
	TRAPD(err,CreatePostImageCaptureControlL(postImageCaptureControl, 1));
	
	if(err == KErrNone)
		{
		err = iArrayPostCaptureControlHandle.Append(postImageCaptureControl);
		if(err != KErrNone)
			{
			iArrayPostCaptureControlId.Remove(iArrayPostCaptureControlId.Count()-1);
			iArrayPostCaptureControlImpl.Remove(iArrayPostCaptureControlImpl.Count()-1);
			delete postImageCaptureControl;
			}
		else
			{
			CaptureImageObserver()->IndividualImageControlHandle(*iCameraImageCapture, 1);
			}
		}
	
	CaptureImageObserver()->ImageCaptureComplete(*iCameraImageCapture, KErrNotSupported);
	}
	
void CCamImageCapture::CancelCaptureImage()
	{
	return;
	}
	
void CCamImageCapture::GetNumImagesExposedL(TUint& /*aNumImagesExposed*/) const
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamImageCapture::GetNumTotalImagesL(TUint& aNumTotalImages) const
	{
	aNumTotalImages = 1;	
	}
	
void CCamImageCapture::GetPostCaptureControlHandleL(CCamera::CCameraPostImageCaptureControl*& aPostCaptureControlHandle, 
									TPostCaptureControlId aPostCaptureControlId) const
	{
	TInt index = iArrayPostCaptureControlId.Find(aPostCaptureControlId);
	User::LeaveIfError(index);
	
	aPostCaptureControlHandle = iArrayPostCaptureControlHandle[index];
	}
	
void CCamImageCapture::SetCaptureImagePriorityL(TECamImagePriority /*aCaptureImagePriority*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamImageCapture::GetCaptureImagePriorityL(TECamImagePriority& /*aCaptureImagePriority*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CCamImageCapture::PauseProcessing(TUint /*aProcessingType*/)
	{
	return;
	}
	
void CCamImageCapture::ResumeProcessingL(TUint /*aProcessingType*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamImageCapture::CreatePostImageCaptureControlL(CCamera::CCameraPostImageCaptureControl*& aPostCaptureControlHandle, 
						TPostCaptureControlId aPostCaptureControlId)
	{
	CCamera::CCameraPostImageCaptureControl* postImageCaptureControl = CCamera::CCameraPostImageCaptureControl::
					CreateL(iCameraImageCapture, aPostCaptureControlId);
					
	aPostCaptureControlHandle = postImageCaptureControl;
	}
	
//
//post image capture control impl

CCamPostImageCaptureControl* CCamPostImageCaptureControl::NewL(CCamImageCapture& aCamImageCaptureImpl)
	{
	return new(ELeave) CCamPostImageCaptureControl(aCamImageCaptureImpl);
	}
	
CCamPostImageCaptureControl::CCamPostImageCaptureControl(CCamImageCapture& aCamImageCaptureImpl) : 
													iCamImageCaptureImpl(aCamImageCaptureImpl)
	{
	}
	
CCamPostImageCaptureControl::~CCamPostImageCaptureControl()
	{
	}
	
void CCamPostImageCaptureControl::Release(TPostCaptureControlId aPostCaptureControlId)
	{
	//remove this id from the CCamImageCapture arrays
	TInt index = iCamImageCaptureImpl.iArrayPostCaptureControlId.Find(aPostCaptureControlId);
	
	if(index >=0 )
		{
		iCamImageCaptureImpl.iArrayPostCaptureControlId.Remove(index);
		iCamImageCaptureImpl.iArrayPostCaptureControlImpl.Remove(index);
		iCamImageCaptureImpl.iArrayPostCaptureControlHandle.Remove(index);
		}
	
	delete this;
	}
	
void CCamPostImageCaptureControl::GetImageSequenceNumberL(TUint& /*aSequenceNumber*/) const
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamPostImageCaptureControl::CancelImage()
	{
	return;
	}
	
void CCamPostImageCaptureControl::SetImagePriorityL(TECamImagePriority /*aImagePriority*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamPostImageCaptureControl::GetImagePriorityL(TECamImagePriority& /*aImagePriority*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CCamPostImageCaptureControl::PauseProcessing(TUint /*aProcessingType*/)
	{
	return;
	}
	
void CCamPostImageCaptureControl::ResumeProcessingL(TUint /*aProcessingType*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamPostImageCaptureControl::GetImageBufferL(MCameraImageBuffer& /*aCameraImageBuffer*/) const
	{
	User::Leave(KErrNotSupported);		
	}
	
void CCamPostImageCaptureControl::GetImageStateL(CCamera::CCameraPostImageCaptureControl::TImageState& /*aImageState*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CCamPostImageCaptureControl::GetBufferStateL(CCamera::CCameraPostImageCaptureControl::TBufferState& /*aBufferState*/) const
	{
	User::Leave(KErrNotSupported);		
	}
	
//
//video capture control impl

CCamVideoCaptureControl* CCamVideoCaptureControl::NewL(CCamUnitTestPlugin& aOwner)
	{
	CCamVideoCaptureControl* self = new(ELeave) CCamVideoCaptureControl(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CCamVideoCaptureControl::ConstructL()
	{
	User::LeaveIfError(RFbsSession::Connect());
	
	iBitmap = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(TSize(320,240), EColor16M));
	}

CCamVideoCaptureControl::CCamVideoCaptureControl(CCamUnitTestPlugin& aOwner) : 
																iOwner(aOwner),
																iCaptureVideoObserver(NULL),
																iSnapshotImpForVideo(NULL)
	{
	}
	
CCamVideoCaptureControl::~CCamVideoCaptureControl()
	{
	delete iBitmap;
	RFbsSession::Disconnect();
	}
	
void CCamVideoCaptureControl::Release()
	{
	delete this;
	}
	
void CCamVideoCaptureControl::CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr	= CHistogramFactoryForVideo::NewL(const_cast<CCamVideoCaptureControl&>(*this));
	}
	
void CCamVideoCaptureControl::GetSnapshotImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr = CSnapshotFactoryForVideo::NewL(const_cast<CCamVideoCaptureControl&>(*this), iSnapshotImpForVideo);
	}
	
void CCamVideoCaptureControl::GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr = NULL;
	User::Leave(KErrNotSupported);	
	}
	
void CCamVideoCaptureControl::SetCaptureVideoObserver(MCaptureVideoObserver& aCaptureVideoObserver)
	{
	iCaptureVideoObserver = &aCaptureVideoObserver;
	}
	
void CCamVideoCaptureControl::GetVideoFormatsSupportedL(TUint& aVideoFormatsSupported, const TSize& /*aSize*/) const
	{
	aVideoFormatsSupported = CCamera::EFormatFbsBitmapColor16M;	
	}
	
void CCamVideoCaptureControl::GetPixelAspectsSupportedL(TUint& aPixelAspectsSupported, CCamera::TFormat /*aVideoFormat*/, const TSize& /*aSize*/) const
	{
	aPixelAspectsSupported = CCamera::CCameraAdvancedSettings::EPixelAspect1To1;	
	}
	
void CCamVideoCaptureControl::GetEmbeddedStillCaptureSupportInfoL(TInt& aSupportedEmbeddedStillCaptureTypes) const
	{
	aSupportedEmbeddedStillCaptureTypes = CCamera::CCameraVideoCaptureControl::EEmbeddedStillCaptureNotSupported;
	}
	
void CCamVideoCaptureControl::PrepareVideoCapture(const CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters& aPrepareVideoParameters)
	{
	if(((aPrepareVideoParameters.Size() != sizeof(CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters)) ||
		aPrepareVideoParameters.Version() != KECamPrepareVideoParametersCurrentVersion))
		{
		iOwner.Notify(KUidECamEventVideoCaptureControlPrepareComplete, KErrNotSupported);
		}
	
	iPrepareVideoParameters = aPrepareVideoParameters;
	
	iOwner.Notify(KUidECamEventVideoCaptureControlPrepareComplete, KErrNone);
	}
	
void CCamVideoCaptureControl::GetRangeAffectedSettingsL(RArray<TUid>& aRangeAffectedSettings) const
	{
	aRangeAffectedSettings.Reset();	
	}
	
void CCamVideoCaptureControl::GetValueAffectedSettingsL(RArray<TUid>& aValueAffectedSettings) const
	{
	aValueAffectedSettings.Reset();	
	}
	
void CCamVideoCaptureControl::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const
	{
	aDisabledSettings.Reset();	
	}
	
void CCamVideoCaptureControl::ReleaseVideoResource()
	{
	return;
	}
	
void CCamVideoCaptureControl::StartVideoCaptureL()
	{
	RCamBuffer2 videoBuffer(iBitmap);
	
	if(iPrepareVideoParameters.VideoCaptureType() != CCamera::CCameraVideoCaptureControl::EVideoCaptureNotSupported)
		{
		CaptureVideoObserver()->DirectVideoCaptureFailed(KErrNotSupported);
		}
	else
		{
		if(iPrepareVideoParameters.VideoCaptureType() != CCamera::CCameraVideoCaptureControl::EClientVideoCapture)	
			{
			CaptureVideoObserver()->VideoBufferReady(NULL, KErrNotSupported);	
			}
		}
	}
	
void CCamVideoCaptureControl::StopVideoCapture()
	{
	}
	
void CCamVideoCaptureControl::PauseVideoCapture()
	{
	return;
	}

void CCamVideoCaptureControl::ResumeVideoCaptureL()
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamVideoCaptureControl::GetFadingEffectStateL(CCamera::CCameraVideoCaptureControl::TFadingEffectState& /*aFadingEffectState*/) const
	{
	User::Leave(KErrNotSupported);	
	}

void CCamVideoCaptureControl::SetFadingEffectState(CCamera::CCameraVideoCaptureControl::TFadingEffectState /*aFadingEffectState*/)
	{
	iOwner.Notify(KUidECamEventVideoCaptureControlFadingEffect, KErrNotSupported);
	}

void CCamVideoCaptureControl::GetVideoCaptureStateL(CCamera::CCameraVideoCaptureControl::TVideoCaptureState& /*aVideoCaptureState*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CCamVideoCaptureControl::GetVideoCaptureSupportInfoL(TInt& aSupportedVideoCaptureTypes) const
	{
	aSupportedVideoCaptureTypes = KSupportedDirectVCType;	
	}
	
void CCamVideoCaptureControl::GetPrepareVideoParametersL(CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters& aPrepareVideoParameters) const
	{
	aPrepareVideoParameters = iPrepareVideoParameters;
	}

	
//
//Direct Snapshot impl for version2 direct viewfinder
//
CCamDirectSnapshot::CCamDirectSnapshot() : iCameraV2DirectVF(NULL), 
										   iDirectSnapshotState(CCamera::CCameraDirectSnapshot::EDirectSnapshotDisabled)
	{
	}

CCamDirectSnapshot* CCamDirectSnapshot::NewL()
	{
	return new (ELeave) CCamDirectSnapshot;
	}
	
CCamDirectSnapshot::~CCamDirectSnapshot()
	{
	}
	
void CCamDirectSnapshot::Release()
	{
	delete this;
	}

void CCamDirectSnapshot::SetDirectViewFinder(const CCamera::CCameraV2DirectViewFinder* aDirectViewFinder)
	{
	iCameraV2DirectVF = aDirectViewFinder;
	}
	
void CCamDirectSnapshot::GetDirectViewFinderL(CCamera::CCameraV2DirectViewFinder*& aDirectViewFinder) const
	{
	aDirectViewFinder = const_cast<CCamera::CCameraV2DirectViewFinder*>(iCameraV2DirectVF);
	}
	
void CCamDirectSnapshot::EnableDirectSnapshotL(CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters)
	{
	iDirectSnapshotState = CCamera::CCameraDirectSnapshot::EDirectSnapshotEnabled;
	iDirectSnapshotParameters = aDirectSnapshotParameters;
	}
	
void CCamDirectSnapshot::DisableDirectSnapshot()
	{
	iDirectSnapshotState = CCamera::CCameraDirectSnapshot::EDirectSnapshotDisabled;
	}
	
void CCamDirectSnapshot::GetDirectSnapshotStateL(CCamera::CCameraDirectSnapshot::TDirectSnapshotState& aDirectSnapshotState) const
	{
	aDirectSnapshotState = iDirectSnapshotState;
	}
	
void CCamDirectSnapshot::GetDirectSnapshotParametersL(CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters) const
	{
	aDirectSnapshotParameters = iDirectSnapshotParameters;	
	}
	
void CCamDirectSnapshot::SetDirectSnapshotParametersL(const CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters)
	{
	iDirectSnapshotParameters = aDirectSnapshotParameters;
	}
		
//
//V2 direct viewfinder impl
//
CCamV2DirectViewFinder* CCamV2DirectViewFinder::NewL(CCamUnitTestPlugin& aOwner)
	{
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	
	if(!globalData)
		{
		globalData = new (ELeave) CDataGlobal;
		CleanupStack::PushL(globalData);
		
		globalData->iReferenceCount = 0;
		globalData->iCamV2DirectViewFinder = new (ELeave) CCamV2DirectViewFinder(aOwner);
    	globalData->iCamV2DirectViewFinder->iRefCount = 1;
    	
    	TInt error = Dll::SetTls(globalData);
		if (error)
			{
			delete globalData->iCamV2DirectViewFinder;
			User::Leave(error);
			}
		CleanupStack::Pop(globalData);
    	
		return static_cast <CCamV2DirectViewFinder*> (globalData->iCamV2DirectViewFinder);
		}
	else
		{
		CCamV2DirectViewFinder* self = globalData->iCamV2DirectViewFinder;
		
		globalData->iReferenceCount++;
		self->iRefCount = globalData->iReferenceCount + 1;
		if (globalData->iReferenceCount == 1)
			{
			delete globalData;
			Dll::FreeTls();
			}
		else
			{
	    	TInt error = Dll::SetTls(globalData);
			if (error)
				{
				delete globalData->iCamV2DirectViewFinder;
				User::Leave(error);
				}	
			}
		return static_cast <CCamV2DirectViewFinder*> (self);		
		}
	 }
	
CCamV2DirectViewFinder::CCamV2DirectViewFinder(CCamUnitTestPlugin& aOwner) : iOwner(aOwner), iVFHandle(0), iDirectSnapshot(NULL)
	{
	//dummy scheme or impl dependent scheme
	//server may generate unique handle no. for each such objects
	iOwner.GenerateVFHandle(iVFHandle);
	}

CCamV2DirectViewFinder::~CCamV2DirectViewFinder()
	{
	Dll::FreeTls();
	}
	
void CCamV2DirectViewFinder::Release(CCamera::CCameraV2DirectViewFinder* /*aDirectViewFinderHandle*/)
	{
	iRefCount--; 
	if(!iRefCount)
		{
		delete this;
		}
	}
	
void CCamV2DirectViewFinder::Release()
	{
	iRefCount--; 
	if(!iRefCount)
		{
		delete this;
		}
	}
	
void CCamV2DirectViewFinder::GetViewFinderFadingCapabilitiesL(CCameraViewFinder::TViewFinderFadingCapabilities& aVFFadingCapabilities) const
	{
	if((aVFFadingCapabilities.Size() != sizeof(CCameraViewFinder::TViewFinderFadingCapabilities)) ||
		(aVFFadingCapabilities.Version() != KECamViewFinderFadingCapabilitiesCurrentVersion))
		{
		User::Leave(KErrNotSupported);
		}
	User::Leave(KErrNotSupported);
	}
	
void CCamV2DirectViewFinder::GetViewFinderFadingEffectL(CCameraViewFinder::TViewFinderFadingEffect& aCurrentVFFadingEffect) const
	{
	if((aCurrentVFFadingEffect.Size() != sizeof(CCameraViewFinder::TViewFinderFadingEffect)) ||
		(aCurrentVFFadingEffect.Version() != KECamViewFinderFadingEffectCurrentVersion))
		{
		User::Leave(KErrNotSupported);
		}
	User::Leave(KErrNotSupported);	
	}
	
void CCamV2DirectViewFinder::SetViewFinderFadingEffect(const CCameraViewFinder::TViewFinderFadingEffect& /*aVFFadingEffect*/)
	{
	iOwner.Notify2(KUidECamEvent2ViewFinderFadingEffect, KErrNotSupported, iVFHandle);
	}

void CCamV2DirectViewFinder::GetViewFinderHandleL(TInt& aVFHandle) const
	{
	aVFHandle = iVFHandle;
	}
	
void CCamV2DirectViewFinder::CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr	= CHistogramFactoryForDirectViewFinder::NewL(const_cast<CCamV2DirectViewFinder&>(*this));
	}
	
void CCamV2DirectViewFinder::GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr = NULL;	
	User::Leave(KErrNotSupported);
	}

void CCamV2DirectViewFinder::SetDirectViewFinderObserver(MDirectViewFinderObserver& aDirectViewFinderObserver)
	{
	iDirectViewFinderObserver = &aDirectViewFinderObserver;
	}
	
void CCamV2DirectViewFinder::SetDirectViewFinderHandle(CCamera::CCameraV2DirectViewFinder* aDirectViewFinderHandle)
	{
	iDirectViewFinderHandle = aDirectViewFinderHandle;
	}
	
void CCamV2DirectViewFinder::StartViewFinderDirectL(RWsSession& /*aWs*/, CWsScreenDevice& /*aScreenDevice*/, RWindowBase& /*aWindow*/, TRect& /*aScreenRect*/)
	{
	DirectViewFinderObserver()->DirectViewFinderFirstFrameDisplayed(DirectViewFinderHandle(), KErrNotSupported);
	}
	
void CCamV2DirectViewFinder::StartViewFinderDirectL(RWsSession& /*aWs*/, CWsScreenDevice& /*aScreenDevice*/, RWindowBase& /*aWindow*/, TRect& /*aScreenRect*/, TRect& /*aClipRect*/)
	{
	DirectViewFinderObserver()->DirectViewFinderFirstFrameDisplayed(DirectViewFinderHandle(), KErrNotSupported);
	}
	
void CCamV2DirectViewFinder::GetDirectViewFinderPropertiesL(TInt& /*aScreenNumber*/, TRect& /*aScreenRect*/, TRect& /*aClipRect*/) const
	{
	User::Leave(KErrNotSupported);		
	}

void CCamV2DirectViewFinder::PauseViewFinderDirect()
	{
	return;
	}
	
void CCamV2DirectViewFinder::ResumeViewFinderDirect()
	{
	DirectViewFinderObserver()->DirectViewFinderFirstFrameDisplayed(DirectViewFinderHandle(), KErrNotSupported);
	}

void CCamV2DirectViewFinder::GetViewFinderStateL(CCamera::CCameraV2DirectViewFinder::TViewFinderState& /*aViewFinderState*/) const
	{
	User::Leave(KErrNotSupported);		
	}
	
void CCamV2DirectViewFinder::StopDirectViewFinder()
	{
	return;
	}
	
/* Dummy test*/
TAny* CCamV2DirectViewFinder::GetDirectSnapshotImpl(TUid aInterface) const
	{
	switch(aInterface.iUid)
		{
		case KECamMCameraDirectSnapshotUidValue:
			{
			if(iDirectSnapshot == NULL)	
				{
				TRAPD(err,iDirectSnapshot = CCamDirectSnapshot::NewL());
				if (err != KErrNone)	
					{
					return NULL;
					}
				}
			return static_cast<MCameraDirectSnapshot*>(iDirectSnapshot); 
			}
		default:
			return NULL;
		}
	}
	
//
//Client viewfinder impl	
CCamClientViewFinder* CCamClientViewFinder::NewL(CCamUnitTestPlugin& aOwner)
	{
	CDataGlobal* globalData = static_cast <CDataGlobal*> (Dll::Tls());
	
	if(!globalData)
		{
		globalData = new (ELeave) CDataGlobal;
		CleanupStack::PushL(globalData);
		
		globalData->iReferenceCount = 0;
		globalData->iCamClientViewFinder = new (ELeave) CCamClientViewFinder(aOwner);
    	globalData->iCamClientViewFinder->iRefCount = 1;
    	
		TInt error = Dll::SetTls(globalData);
		if (error)
			{
			delete globalData->iCamClientViewFinder;
			User::Leave(error);
			}
		CleanupStack::Pop(globalData);
    	
		return static_cast <CCamClientViewFinder*> (globalData->iCamClientViewFinder);
		}
	else
		{
		CCamClientViewFinder* self = globalData->iCamClientViewFinder;
		
		globalData->iReferenceCount++;
		self->iRefCount = globalData->iReferenceCount + 1;
		if (globalData->iReferenceCount == 1)
			{
			delete globalData;
			Dll::FreeTls();
			}
		else
			{
			TInt error = Dll::SetTls(globalData);
			if (error)
				{
				delete globalData->iCamClientViewFinder;
				User::Leave(error);
				}	
			}
		return static_cast <CCamClientViewFinder*> (self);		
		}
	}
	
CCamClientViewFinder::CCamClientViewFinder(CCamUnitTestPlugin& aOwner) : iOwner(aOwner), iVFHandle(0)
	{
	//dummy scheme or impl dependent scheme
	//server may generate unique handle no. for each such objects
	iOwner.GenerateVFHandle(iVFHandle);
	}

CCamClientViewFinder::~CCamClientViewFinder()
	{
	Dll::FreeTls();
	}
	
void CCamClientViewFinder::Release(CCamera::CCameraClientViewFinder* /*aClientViewFinderHandle*/)
	{
	iRefCount--; 
	if(!iRefCount)
		{
		delete this;
		}
	}
	
void CCamClientViewFinder::Release()
	{
	iRefCount--; 
	if(!iRefCount)
		{
		delete this;
		}
	}

void CCamClientViewFinder::GetViewFinderFadingCapabilitiesL(CCameraViewFinder::TViewFinderFadingCapabilities& aVFFadingCapabilities) const
	{
	if((aVFFadingCapabilities.Size() != sizeof(CCameraViewFinder::TViewFinderFadingCapabilities)) ||
		(aVFFadingCapabilities.Version() != KECamViewFinderFadingCapabilitiesCurrentVersion))
		{
		User::Leave(KErrNotSupported);
		}
	User::Leave(KErrNotSupported);
	}
	
void CCamClientViewFinder::GetViewFinderFadingEffectL(CCameraViewFinder::TViewFinderFadingEffect& aCurrentVFFadingEffect) const
	{
	if((aCurrentVFFadingEffect.Size() != sizeof(CCameraViewFinder::TViewFinderFadingEffect)) ||
		(aCurrentVFFadingEffect.Version() != KECamViewFinderFadingEffectCurrentVersion))
		{
		User::Leave(KErrNotSupported);
		}
	User::Leave(KErrNotSupported);
	}
	
void CCamClientViewFinder::SetViewFinderFadingEffect(const CCameraViewFinder::TViewFinderFadingEffect& /*aVFFadingEffect*/)
	{
	iOwner.Notify2(KUidECamEvent2ViewFinderFadingEffect, KErrNotSupported, iVFHandle);
	}

void CCamClientViewFinder::GetViewFinderHandleL(TInt& aVFHandle) const
	{
	aVFHandle = iVFHandle;
	}
	
void CCamClientViewFinder::CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr	= CHistogramFactoryForClientViewFinder::NewL(const_cast<CCamClientViewFinder&>(*this));
	}
	
void CCamClientViewFinder::GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr = NULL;
	User::Leave(KErrNotSupported);
	}
	
void CCamClientViewFinder::SetClientViewFinderObserver(MClientViewFinderObserver& aClientViewFinderObserver)
	{
	iClientViewFinderObserver = &aClientViewFinderObserver;
	}
	
void CCamClientViewFinder::SetClientViewFinderHandle(CCamera::CCameraClientViewFinder* aClientViewFinderHandle)
	{
	iClientViewFinderHandle = aClientViewFinderHandle;
	}
	
void CCamClientViewFinder::StartClientViewFinderL(TInt /*aScreenNumber*/, CCamera::TFormat /*aImageFormat*/, TSize& /*aSize*/)
	{
	ClientViewFinderObserver()->ViewFinderBufferReady(ClientViewFinderHandle(), KErrNotSupported);
	}
	
void CCamClientViewFinder::StartClientViewFinderL(TInt /*aScreenNumber*/, CCamera::TFormat /*aImageFormat*/, TSize& /*aSize*/, TRect& /*aClipRect*/)
	{
	ClientViewFinderObserver()->ViewFinderBufferReady(ClientViewFinderHandle(), KErrNotSupported);
	}
	
void CCamClientViewFinder::GetClientViewFinderStateL(TBool& /*aIsActive*/) const
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamClientViewFinder::GetClientViewFinderPropertiesL(TInt& /*aScreenNumber*/, CCamera::TFormat& /*aImageFormat*/, TSize& /*aSize*/, TRect& /*aClipRect*/) const
	{
	User::Leave(KErrNotSupported);		
	}
	
void CCamClientViewFinder::StopClientViewFinder()
	{
	return;
	}
	
void CCamClientViewFinder::GetViewFinderBufferL(MCameraBuffer2& /*aClientViewFinderBuffer*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
//
//Snapshot impl for Image
CCamSnapshotForImage* CCamSnapshotForImage::NewL(CCamUnitTestPlugin& aOwner, CCamImageCapture& aCamImageCaptureImpl)
	{
	CCamSnapshotForImage* self = new(ELeave) CCamSnapshotForImage(aOwner, aCamImageCaptureImpl);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CCamSnapshotForImage::ConstructL()
	{
	User::LeaveIfError(RFbsSession::Connect());
	
	iBitmap = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(TSize(320,240), EColor16M));
	}
	
CCamSnapshotForImage::CCamSnapshotForImage(CCamUnitTestPlugin& aOwner, CCamImageCapture& aCamImageCaptureImpl)
												: iOwner(aOwner), 
												  iCamImageCaptureImpl(aCamImageCaptureImpl),
												  iSnapshotbuffer(iBitmap),
												  iRefCount(2)
	{
	}
	
CCamSnapshotForImage::~CCamSnapshotForImage()
	{
	delete iBitmap;
	RFbsSession::Disconnect();
	}
	
TUint32 CCamSnapshotForImage::SupportedFormats()
	{
	return CCamera::EFormatFbsBitmapColor16M;
	}
	
void CCamSnapshotForImage::PrepareSnapshotL(CCamera::TFormat /*aFormat*/, const TPoint& /*aPosition*/, const TSize& /*aSize*/, const TRgb& /*aBgColor*/, TBool /*aMaintainAspectRatio*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamSnapshotForImage::PrepareSnapshotL(CCamera::TFormat /*aFormat*/, const TSize& /*aSize*/, TBool /*aMaintainAspectRatio*/)
	{
	User::Leave(KErrNotSupported);	
	}
    
void CCamSnapshotForImage::SetBgColorL(const TRgb& /*aBgColor*/)
	{
	User::Leave(KErrNotSupported);
	}

void CCamSnapshotForImage::SetPositionL(const TPoint& /*aPosition*/)
	{
	User::Leave(KErrNotSupported);
	}
	
TBool CCamSnapshotForImage::IsSnapshotActive() const
	{
	return EFalse;	
	}
	
void CCamSnapshotForImage::StartSnapshot()
	{
	return;
	}
	
void CCamSnapshotForImage::StopSnapshot()
	{
	return;
	}
	
MCameraBuffer& CCamSnapshotForImage::SnapshotDataL(RArray<TInt>& /*aFrameIndexOrder*/)
	{
	User::Leave(KErrNotSupported);
	return iSnapshotbuffer;
	}
	
void CCamSnapshotForImage::CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr	= CHistogramFactoryForImageSnapshot::NewL(const_cast<CCamSnapshotForImage&>(*this));
	}
	
void CCamSnapshotForImage::PrepareSnapshotL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)
	{
	if( aSnapshotParameters.Size() != sizeof(CCamera::CCameraSnapshot::TSnapshotParameters) ||
		aSnapshotParameters.Version() != KECamSnapshotParametersCurrentVersion )
		{
		User::Leave(KErrNotSupported);	
		}
	iSnapshotParameters = aSnapshotParameters;
	}
	
void CCamSnapshotForImage::GetSnapshotParametersL(CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)
	{
	aSnapshotParameters = iSnapshotParameters;
	}
	
void CCamSnapshotForImage::SetSnapshotParametersL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)
	{
	iSnapshotParameters = aSnapshotParameters;
	}
	
void CCamSnapshotForImage::GetSnapshotStatusL(CCamera::CCameraSnapshot::TSnapshotState& /*aSnapshotState*/) const
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamSnapshotForImage::SelectSnapshotVideoFramesL(CCamera::CCameraSnapshot::TSnapshotVideoFrames /*aSnapshotVideoFrames*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamSnapshotForImage::EnableSnapshot()
	{
	CamImageCaptureImpl().CaptureImageObserver()->ClientSnapshotForImageReady(CameraImageCapture(), 1, NULL, KErrNotSupported);
	}
	
void CCamSnapshotForImage::DisableSnapshot()
	{
	return;
	}

/* dummy */
void CCamSnapshotForImage::SetClientViewFinderId(TInt /*aClientViewFinderId*/)
	{
	return;
	}
	
void CCamSnapshotForImage::Release()
	{
	iRefCount--;
	if(iRefCount == 0)
		{
		delete this;	
		}
	}

//
//Snapshot factory for Image
CSnapshotFactoryForImage* CSnapshotFactoryForImage::NewL(CCamImageCapture& aCamImageCaptureImpl, CCamSnapshotForImage* aSnapshotImpForImage)
	{
	CSnapshotFactoryForImage* self = new(ELeave) CSnapshotFactoryForImage();
	
	CleanupStack::PushL(self);
	self->ConstructL(aCamImageCaptureImpl, aSnapshotImpForImage);
	CleanupStack::Pop();
	
	return self;
	}
	
void CSnapshotFactoryForImage::ConstructL(CCamImageCapture& aCamImageCaptureImpl, CCamSnapshotForImage* aSnapshotImpForImage)
	{
	if(aSnapshotImpForImage == NULL)
		{
		iSnapshotImpForImage = CCamSnapshotForImage::NewL(aCamImageCaptureImpl.Owner(), aCamImageCaptureImpl);
		aCamImageCaptureImpl.iSnapshotImpForImage = iSnapshotImpForImage;
		}
	}

CSnapshotFactoryForImage::CSnapshotFactoryForImage() : iSnapshotImpForImage(NULL),
													   iIsSnapshotImpPassed(EFalse)
	{
	}
	
CSnapshotFactoryForImage::~CSnapshotFactoryForImage()
	{
	if(!iIsSnapshotImpPassed)
		{
		delete iSnapshotImpForImage;
		}
	iSnapshotImpForImage = NULL;
	}
	
TInt CSnapshotFactoryForImage::GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const
	{
	switch(aIfaceUid.iUid)	
		{
		//fall through
		case KECamMCameraSnapshotUidValue:
			{
			aIfPtr = static_cast<MCameraSnapshot*>(iSnapshotImpForImage);
			iIsSnapshotImpPassed = ETrue;
			return KErrNone;	
			}
			
		case KECamMCameraSnapshot2UidValue:
			{
			aIfPtr = static_cast<MCameraSnapshot2*>(iSnapshotImpForImage);
			iIsSnapshotImpPassed = ETrue;
			return KErrNone;	
			}
		default:
			{
			aIfPtr = NULL;
			return KErrNotSupported;	
			}
		}	
	}
	
TInt CSnapshotFactoryForImage::GetImpl1(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/) const
	{
	return KErrNotSupported;		
	}
	
TInt CSnapshotFactoryForImage::GetImpl2(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/, TECamImplFactoryParam /*aParam2*/) const
	{
	return KErrNotSupported;	
	}
	
void CSnapshotFactoryForImage::Release()
	{
	delete this;
	}
	
//
//Snapshot impl for Video
CCamSnapshotForVideo* CCamSnapshotForVideo::NewL(CCamUnitTestPlugin& aOwner, CCamVideoCaptureControl& aCamVideoCaptureControlImpl)
	{
	CCamSnapshotForVideo* self = new(ELeave) CCamSnapshotForVideo(aOwner, aCamVideoCaptureControlImpl);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CCamSnapshotForVideo::ConstructL()
	{
	User::LeaveIfError(RFbsSession::Connect());
	
	iBitmap = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(TSize(320,240), EColor16M));
	}
	
CCamSnapshotForVideo::CCamSnapshotForVideo(CCamUnitTestPlugin& aOwner, CCamVideoCaptureControl& aCamVideoCaptureControlImpl) : 
															iOwner(aOwner),
															iCamVideoCaptureControlImpl(aCamVideoCaptureControlImpl),
															iSnapshotbuffer(iBitmap),
															iRefCount(2)
															
	{
	}
	
CCamSnapshotForVideo::~CCamSnapshotForVideo()
	{
	delete iBitmap;
	RFbsSession::Disconnect();
	}
	
TUint32 CCamSnapshotForVideo::SupportedFormats()
	{
	return CCamera::EFormatFbsBitmapColor16M;
	}
	
void CCamSnapshotForVideo::PrepareSnapshotL(CCamera::TFormat /*aFormat*/, const TPoint& /*aPosition*/, const TSize& /*aSize*/, const TRgb& /*aBgColor*/, TBool /*aMaintainAspectRatio*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamSnapshotForVideo::PrepareSnapshotL(CCamera::TFormat /*aFormat*/, const TSize& /*aSize*/, TBool /*aMaintainAspectRatio*/)
	{
	User::Leave(KErrNotSupported);	
	}
    
void CCamSnapshotForVideo::SetBgColorL(const TRgb& /*aBgColor*/)
	{
	User::Leave(KErrNotSupported);
	}

void CCamSnapshotForVideo::SetPositionL(const TPoint& /*aPosition*/)
	{
	User::Leave(KErrNotSupported);
	}
	
TBool CCamSnapshotForVideo::IsSnapshotActive() const
	{
	return EFalse;	
	}
	
void CCamSnapshotForVideo::StartSnapshot()
	{
	return;
	}
	
void CCamSnapshotForVideo::StopSnapshot()
	{
	return;
	}
	
MCameraBuffer& CCamSnapshotForVideo::SnapshotDataL(RArray<TInt>& /*aFrameIndexOrder*/)
	{
	User::Leave(KErrNotSupported);
	return iSnapshotbuffer;
	}
	
void CCamSnapshotForVideo::CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const
	{
	aImplFactoryPtr	= NULL;		
	}
	
void CCamSnapshotForVideo::PrepareSnapshotL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)
	{
	if( aSnapshotParameters.Size() != sizeof(CCamera::CCameraSnapshot::TSnapshotParameters) ||
		aSnapshotParameters.Version() != KECamSnapshotParametersCurrentVersion )
		{
		User::Leave(KErrNotSupported);	
		}
	iSnapshotParameters = aSnapshotParameters;
	}
	
void CCamSnapshotForVideo::GetSnapshotParametersL(CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)
	{
	aSnapshotParameters = iSnapshotParameters;
	}
	
void CCamSnapshotForVideo::SetSnapshotParametersL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters)
	{
	iSnapshotParameters = aSnapshotParameters;
	}
	
void CCamSnapshotForVideo::GetSnapshotStatusL(CCamera::CCameraSnapshot::TSnapshotState& /*aSnapshotState*/) const
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamSnapshotForVideo::SelectSnapshotVideoFramesL(CCamera::CCameraSnapshot::TSnapshotVideoFrames /*aSnapshotVideoFrames*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CCamSnapshotForVideo::EnableSnapshot()
	{
	CamVideoCaptureControlImpl().CaptureVideoObserver()->ClientSnapshotReady(NULL, KErrNotSupported);
	}
	
void CCamSnapshotForVideo::DisableSnapshot()
	{
	return;
	}
	
/* dummy */
void CCamSnapshotForVideo::SetClientViewFinderId(TInt /*aClientViewFinderId*/)
	{
	return;
	}
	
void CCamSnapshotForVideo::Release()
	{
	iRefCount--;
	if(iRefCount == 0)
		{
		delete this;	
		}
	}

//
//snapshot factory for video
CSnapshotFactoryForVideo* CSnapshotFactoryForVideo::NewL(CCamVideoCaptureControl& aCamVideoCaptureControlImpl, CCamSnapshotForVideo* aSnapshotImpForVideo)
	{
	CSnapshotFactoryForVideo* self = new(ELeave) CSnapshotFactoryForVideo();
	
	CleanupStack::PushL(self);
	self->ConstructL(aCamVideoCaptureControlImpl, aSnapshotImpForVideo);
	CleanupStack::Pop();
	
	return self;
	}
	
void CSnapshotFactoryForVideo::ConstructL(CCamVideoCaptureControl& aCamVideoCaptureControlImpl, CCamSnapshotForVideo* aSnapshotImpForVideo)
	{
	if(aSnapshotImpForVideo == NULL)
		{
		iSnapshotImpForVideo = CCamSnapshotForVideo::NewL(aCamVideoCaptureControlImpl.Owner(), aCamVideoCaptureControlImpl);
		aCamVideoCaptureControlImpl.iSnapshotImpForVideo = iSnapshotImpForVideo;
		}
	}

CSnapshotFactoryForVideo::CSnapshotFactoryForVideo() : iSnapshotImpForVideo(NULL),
													   iIsSnapshotImpPassed(EFalse)
	{
	}
	
CSnapshotFactoryForVideo::~CSnapshotFactoryForVideo()
	{
	if(!iIsSnapshotImpPassed)
		{
		delete iSnapshotImpForVideo;
		}
	iSnapshotImpForVideo = NULL;
	}
	
TInt CSnapshotFactoryForVideo::GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const
	{
	switch(aIfaceUid.iUid)	
		{
		//fall through
		case KECamMCameraSnapshotUidValue:
			{
			aIfPtr = static_cast<MCameraSnapshot*>(iSnapshotImpForVideo);
			iIsSnapshotImpPassed = ETrue;
			return KErrNone;	
			}
			
		case KECamMCameraSnapshot2UidValue:
			{
			aIfPtr = static_cast<MCameraSnapshot2*>(iSnapshotImpForVideo);
			iIsSnapshotImpPassed = ETrue;
			return KErrNone;	
			}
		default:
			{
			aIfPtr = NULL;
			return KErrNotSupported;	
			}
		}	
	}
	
TInt CSnapshotFactoryForVideo::GetImpl1(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/) const
	{
	return KErrNotSupported;		
	}
	
TInt CSnapshotFactoryForVideo::GetImpl2(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/, TECamImplFactoryParam /*aParam2*/) const
	{
	return KErrNotSupported;	
	}
	
void CSnapshotFactoryForVideo::Release()
	{
	delete this;
	}
	
//
// Histogram impl for video
CHistogramImpForVideo* CHistogramImpForVideo::NewL(CCamUnitTestPlugin& aOwner, CCamVideoCaptureControl& aCamVideoCaptureControlImpl)
	{
	return new(ELeave) CHistogramImpForVideo(aOwner, aCamVideoCaptureControlImpl);
	}

CHistogramImpForVideo::CHistogramImpForVideo(CCamUnitTestPlugin& aOwner, CCamVideoCaptureControl& aCamVideoCaptureControlImpl):
													iOwner(aOwner),	iCamVideoCaptureControlImpl(aCamVideoCaptureControlImpl)
	{
	}

CHistogramImpForVideo::~CHistogramImpForVideo()
	{
	}
	
void CHistogramImpForVideo::Release(CCamera::CCameraV2Histogram* /*aHistogramHandle*/)
	{
	delete this;
	}
	
void CHistogramImpForVideo::SetHistogramHandle(CCamera::CCameraV2Histogram* /*aHistogramHandle*/)
	{
	return;
	}
	
void CHistogramImpForVideo::GetSupportedHistogramsL(TUint& aSupportedHistogramType) const
	{
	aSupportedHistogramType = CCamera::CCameraV2Histogram::EHistNone;
	}

void CHistogramImpForVideo::GetDirectHistogramSupportInfoL(TBool& /*aIsDirectHistogramSupported*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForVideo::PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType /*aType*/)
	{
	User::Leave(KErrNotSupported);
	}

void CHistogramImpForVideo::PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters)
	{
	if((aDirectHistogramParameters.Size() != sizeof(CCamera::CCameraV2Histogram::TDirectHistogramParameters)) ||
		(aDirectHistogramParameters.Version() != KECamDirectHistogramParametersCurrentVersion))
		{
		User::Leave(KErrNotSupported);
		}
	User::Leave(KErrNotSupported);
	}
	
void CHistogramImpForVideo::UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& /*aDirectHistogramParameters*/)
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForVideo::GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& /*aDirectHistogramParameters*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForVideo::StartHistogram()
	{
	RCamHistogramV2Buffer camHistogramV2Buffer;
	
	CamVideoCaptureControlImpl().CaptureVideoObserver()->ClientHistogramReady(NULL, KErrNotSupported);
	}
	
void CHistogramImpForVideo::StopHistogram()
	{
	return;
	}

void CHistogramImpForVideo::GetHistogramStateL(TBool& /*aIsHistogramActive*/) const
	{
	User::Leave(KErrNotSupported);		
	}

//Hisotgram factory for Video
CHistogramFactoryForVideo* CHistogramFactoryForVideo::NewL(CCamVideoCaptureControl& aCamVideoCaptureControlImpl)
	{
	CHistogramFactoryForVideo* self = new(ELeave) CHistogramFactoryForVideo();
	
	CleanupStack::PushL(self);
	self->ConstructL(aCamVideoCaptureControlImpl);
	CleanupStack::Pop();
	
	return self;
	}
	
void CHistogramFactoryForVideo::ConstructL(CCamVideoCaptureControl& aCamVideoCaptureControlImpl)
	{
	iHistogramImpForVideo = CHistogramImpForVideo::NewL(aCamVideoCaptureControlImpl.Owner(), aCamVideoCaptureControlImpl);
	}

CHistogramFactoryForVideo::CHistogramFactoryForVideo() : iHistogramImpForVideo(NULL),
														 iIsHistogramImpPassed(EFalse)
	{
	}
	
CHistogramFactoryForVideo::~CHistogramFactoryForVideo()
	{
	if(!iIsHistogramImpPassed)
		{
		delete iHistogramImpForVideo;
		}
	iHistogramImpForVideo = NULL;
	}
	
TInt CHistogramFactoryForVideo::GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const
	{
	switch(aIfaceUid.iUid)	
		{
		case KECamMCameraV2HistogramUidValue:
			{
			aIfPtr = static_cast<MCameraV2Histogram*>(iHistogramImpForVideo);
			iIsHistogramImpPassed = ETrue;
			return KErrNone;	
			}
		default:
			{
			aIfPtr = NULL;
			return KErrNotSupported;	
			}
		}	
	}
	
TInt CHistogramFactoryForVideo::GetImpl1(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/) const
	{
	return KErrNotSupported;		
	}
	
TInt CHistogramFactoryForVideo::GetImpl2(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/, TECamImplFactoryParam /*aParam2*/) const
	{
	return KErrNotSupported;	
	}
	
void CHistogramFactoryForVideo::Release()
	{
	delete this;
	}
	
// Histogram impl for image
CHistogramImpForImage* CHistogramImpForImage::NewL(CCamUnitTestPlugin& aOwner, CCamImageCapture& aCamImageCaptureImpl)
	{
	return new(ELeave) CHistogramImpForImage(aOwner, aCamImageCaptureImpl);
	}

CHistogramImpForImage::CHistogramImpForImage(CCamUnitTestPlugin& aOwner, CCamImageCapture& aCamImageCaptureImpl):
													iOwner(aOwner),	iCamImageCaptureImpl(aCamImageCaptureImpl)
	{
	}

CHistogramImpForImage::~CHistogramImpForImage()
	{
	}
	
void CHistogramImpForImage::Release(CCamera::CCameraV2Histogram* /*aHistogramHandle*/)
	{
	delete this;
	}
	
void CHistogramImpForImage::SetHistogramHandle(CCamera::CCameraV2Histogram* aHistogramHandle)
	{
	iHistogramHandle = aHistogramHandle;
	}
	
void CHistogramImpForImage::GetSupportedHistogramsL(TUint& aSupportedHistogramType) const
	{
	aSupportedHistogramType = CCamera::CCameraV2Histogram::EHistNone;
	}

void CHistogramImpForImage::GetDirectHistogramSupportInfoL(TBool& /*aIsDirectHistogramSupported*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForImage::PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType /*aType*/)
	{
	User::Leave(KErrNotSupported);
	}

void CHistogramImpForImage::PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters)
	{
	if((aDirectHistogramParameters.Size() != sizeof(CCamera::CCameraV2Histogram::TDirectHistogramParameters)) ||
		(aDirectHistogramParameters.Version() != KECamDirectHistogramParametersCurrentVersion))
		{
		User::Leave(KErrNotSupported);
		}	
	User::Leave(KErrNotSupported);
	}
	
void CHistogramImpForImage::UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& /*aDirectHistogramParameters*/)
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForImage::GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& /*aDirectHistogramParameters*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForImage::StartHistogram()
	{
	RCamHistogramV2Buffer camHistogramV2Buffer;
	CamImageCaptureImpl().CaptureImageObserver()->ClientHistogramForImageReady(CameraImageCapture(), 1, NULL, KErrNotSupported);
	}
	
void CHistogramImpForImage::StopHistogram()
	{
	return;
	}

void CHistogramImpForImage::GetHistogramStateL(TBool& /*aIsHistogramActive*/) const
	{
	User::Leave(KErrNotSupported);		
	}
	
//Hisotgram factory for Image
CHistogramFactoryForImage* CHistogramFactoryForImage::NewL(CCamImageCapture& aCamImageCaptureImpl)
	{
	CHistogramFactoryForImage* self = new(ELeave) CHistogramFactoryForImage();
	
	CleanupStack::PushL(self);
	self->ConstructL(aCamImageCaptureImpl);
	CleanupStack::Pop();
	
	return self;
	}
	
void CHistogramFactoryForImage::ConstructL(CCamImageCapture& aCamImageCaptureImpl)
	{
	iHistogramImpForImage = CHistogramImpForImage::NewL(aCamImageCaptureImpl.Owner(), aCamImageCaptureImpl);
	}

CHistogramFactoryForImage::CHistogramFactoryForImage() : iHistogramImpForImage(NULL),
														 iIsHistogramImpPassed(EFalse)
	{
	}
	
CHistogramFactoryForImage::~CHistogramFactoryForImage()
	{
	if(!iIsHistogramImpPassed)
		{
		delete iHistogramImpForImage;
		}
	iHistogramImpForImage = NULL;
	}
	
TInt CHistogramFactoryForImage::GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const
	{
	switch(aIfaceUid.iUid)	
		{
		case KECamMCameraV2HistogramUidValue:
			{
			aIfPtr = static_cast<MCameraV2Histogram*>(iHistogramImpForImage);
			iIsHistogramImpPassed = ETrue;
			return KErrNone;	
			}
		default:
			{
			aIfPtr = NULL;
			return KErrNotSupported;	
			}
		}	
	}
	
TInt CHistogramFactoryForImage::GetImpl1(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/) const
	{
	return KErrNotSupported;		
	}
	
TInt CHistogramFactoryForImage::GetImpl2(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/, TECamImplFactoryParam /*aParam2*/) const
	{
	return KErrNotSupported;	
	}
	
void CHistogramFactoryForImage::Release()
	{
	delete this;
	}
	
// Histogram impl for direct view finder
CHistogramImpForDirectViewFinder* CHistogramImpForDirectViewFinder::NewL(CCamUnitTestPlugin& aOwner, CCamV2DirectViewFinder& aCamV2DirectViewFinderImpl)
	{
	return new(ELeave) CHistogramImpForDirectViewFinder(aOwner, aCamV2DirectViewFinderImpl);
	}

CHistogramImpForDirectViewFinder::CHistogramImpForDirectViewFinder(CCamUnitTestPlugin& aOwner, 
					CCamV2DirectViewFinder& aCamV2DirectViewFinderImpl): iOwner(aOwner),	
																		 iCamV2DirectViewFinderImpl(aCamV2DirectViewFinderImpl)
	{
	}

CHistogramImpForDirectViewFinder::~CHistogramImpForDirectViewFinder()
	{
	}
	
void CHistogramImpForDirectViewFinder::Release(CCamera::CCameraV2Histogram* /*aHistogramHandle*/)
	{
	delete this;
	}

void CHistogramImpForDirectViewFinder::SetHistogramHandle(CCamera::CCameraV2Histogram* aHistogramHandle)
	{
	iHistogramHandle = aHistogramHandle;
	}

void CHistogramImpForDirectViewFinder::GetSupportedHistogramsL(TUint& aSupportedHistogramType) const
	{
	aSupportedHistogramType = CCamera::CCameraV2Histogram::EHistNone;
	}

void CHistogramImpForDirectViewFinder::GetDirectHistogramSupportInfoL(TBool& /*aIsDirectHistogramSupported*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForDirectViewFinder::PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType /*aType*/)
	{
	User::Leave(KErrNotSupported);
	}

void CHistogramImpForDirectViewFinder::PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters)
	{
	if((aDirectHistogramParameters.Size() != sizeof(CCamera::CCameraV2Histogram::TDirectHistogramParameters)) ||
		(aDirectHistogramParameters.Version() != KECamDirectHistogramParametersCurrentVersion))
		{
		User::Leave(KErrNotSupported);
		}
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForDirectViewFinder::UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& /*aDirectHistogramParameters*/)
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForDirectViewFinder::GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& /*aDirectHistogramParameters*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForDirectViewFinder::StartHistogram()
	{
	RCamHistogramV2Buffer camHistogramV2Buffer;
	DirectViewFinderImpl().DirectViewFinderObserver()->ClientHistogramReady(DirectViewFinderImpl().DirectViewFinderHandle(), NULL, KErrNotSupported);
	}
	
void CHistogramImpForDirectViewFinder::StopHistogram()
	{
	return;
	}

void CHistogramImpForDirectViewFinder::GetHistogramStateL(TBool& /*aIsHistogramActive*/) const
	{
	User::Leave(KErrNotSupported);		
	}

//Hisotgram factory for Direct View Finder
CHistogramFactoryForDirectViewFinder* CHistogramFactoryForDirectViewFinder::NewL(CCamV2DirectViewFinder& aCamV2DirectViewFinderImpl)
	{
	CHistogramFactoryForDirectViewFinder* self = new(ELeave) CHistogramFactoryForDirectViewFinder();
	
	CleanupStack::PushL(self);
	self->ConstructL(aCamV2DirectViewFinderImpl);
	CleanupStack::Pop();
	
	return self;
	}
	
void CHistogramFactoryForDirectViewFinder::ConstructL(CCamV2DirectViewFinder& aCamV2DirectViewFinderImpl)
	{
	iHistogramImpForDirectViewFinder = CHistogramImpForDirectViewFinder::NewL(aCamV2DirectViewFinderImpl.Owner(), aCamV2DirectViewFinderImpl);
	}

CHistogramFactoryForDirectViewFinder::CHistogramFactoryForDirectViewFinder() : iHistogramImpForDirectViewFinder(NULL),
																			   iIsHistogramImpPassed(EFalse)
	{
	}
	
CHistogramFactoryForDirectViewFinder::~CHistogramFactoryForDirectViewFinder()
	{
	if(!iIsHistogramImpPassed)
		{
		delete iHistogramImpForDirectViewFinder;
		}
	iHistogramImpForDirectViewFinder = NULL;
	}
	
TInt CHistogramFactoryForDirectViewFinder::GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const
	{
	switch(aIfaceUid.iUid)	
		{
		case KECamMCameraV2HistogramUidValue:
			{
			aIfPtr = static_cast<MCameraV2Histogram*>(iHistogramImpForDirectViewFinder);
			iIsHistogramImpPassed = ETrue;
			return KErrNone;	
			}
		default:
			{
			aIfPtr = NULL;
			return KErrNotSupported;	
			}
		}	
	}
	
TInt CHistogramFactoryForDirectViewFinder::GetImpl1(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/) const
	{
	return KErrNotSupported;		
	}
	
TInt CHistogramFactoryForDirectViewFinder::GetImpl2(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/, TECamImplFactoryParam /*aParam2*/) const
	{
	return KErrNotSupported;	
	}
	
void CHistogramFactoryForDirectViewFinder::Release()
	{
	delete this;
	}	
	
// Histogram impl for client view finder
CHistogramImpForClientViewFinder* CHistogramImpForClientViewFinder::NewL(CCamUnitTestPlugin& aOwner, CCamClientViewFinder& aCamClientViewFinderImpl)
	{
	return new(ELeave) CHistogramImpForClientViewFinder(aOwner, aCamClientViewFinderImpl);
	}

CHistogramImpForClientViewFinder::CHistogramImpForClientViewFinder(CCamUnitTestPlugin& aOwner, 
					CCamClientViewFinder& aCamClientViewFinderImpl): iOwner(aOwner), 
															  		 iCamClientViewFinderImpl(aCamClientViewFinderImpl)
	{
	}

CHistogramImpForClientViewFinder::~CHistogramImpForClientViewFinder()
	{
	}
	
void CHistogramImpForClientViewFinder::Release(CCamera::CCameraV2Histogram* /*aHistogramHandle*/)
	{
	delete this;
	}
	
void CHistogramImpForClientViewFinder::SetHistogramHandle(CCamera::CCameraV2Histogram* aHistogramHandle)
	{
	iHistogramHandle = aHistogramHandle;
	}
	
void CHistogramImpForClientViewFinder::GetSupportedHistogramsL(TUint& aSupportedHistogramType) const
	{
	aSupportedHistogramType = CCamera::CCameraV2Histogram::EHistNone;
	}

void CHistogramImpForClientViewFinder::GetDirectHistogramSupportInfoL(TBool& /*aIsDirectHistogramSupported*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForClientViewFinder::PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType /*aType*/)
	{
	User::Leave(KErrNotSupported);
	}

void CHistogramImpForClientViewFinder::PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters)
	{
	if((aDirectHistogramParameters.Size() != sizeof(CCamera::CCameraV2Histogram::TDirectHistogramParameters)) ||
		(aDirectHistogramParameters.Version() != KECamDirectHistogramParametersCurrentVersion))
		{
		User::Leave(KErrNotSupported);
		}
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForClientViewFinder::UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& /*aDirectHistogramParameters*/)
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForClientViewFinder::GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& /*aDirectHistogramParameters*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForClientViewFinder::StartHistogram()
	{
	RCamHistogramV2Buffer camHistogramV2Buffer;
	ClientViewFinderImpl().ClientViewFinderObserver()->ClientHistogramReady(ClientViewFinderHandle(), NULL, KErrNotSupported);
	}
	
void CHistogramImpForClientViewFinder::StopHistogram()
	{
	return;
	}

void CHistogramImpForClientViewFinder::GetHistogramStateL(TBool& /*aIsHistogramActive*/) const
	{
	User::Leave(KErrNotSupported);		
	}
	
//Hisotgram factory for Client View Finder
CHistogramFactoryForClientViewFinder* CHistogramFactoryForClientViewFinder::NewL(CCamClientViewFinder& aCamClientViewFinderImpl)
	{
	CHistogramFactoryForClientViewFinder* self = new(ELeave) CHistogramFactoryForClientViewFinder();
	
	CleanupStack::PushL(self);
	self->ConstructL(aCamClientViewFinderImpl);
	CleanupStack::Pop();
	
	return self;
	}
	
void CHistogramFactoryForClientViewFinder::ConstructL(CCamClientViewFinder& aCamClientViewFinderImpl)
	{
	iHistogramImpForClientViewFinder = CHistogramImpForClientViewFinder::NewL(aCamClientViewFinderImpl.Owner(), aCamClientViewFinderImpl);
	}

CHistogramFactoryForClientViewFinder::CHistogramFactoryForClientViewFinder() : iHistogramImpForClientViewFinder(NULL),
																			   iIsHistogramImpPassed(EFalse)
	{
	}
	
CHistogramFactoryForClientViewFinder::~CHistogramFactoryForClientViewFinder()
	{
	if(!iIsHistogramImpPassed)
		{
		delete iHistogramImpForClientViewFinder;
		}
		
	iHistogramImpForClientViewFinder = NULL;
	}
	
TInt CHistogramFactoryForClientViewFinder::GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const
	{
	switch(aIfaceUid.iUid)	
		{
		case KECamMCameraV2HistogramUidValue:
			{
			aIfPtr = static_cast<MCameraV2Histogram*>(iHistogramImpForClientViewFinder);
			iIsHistogramImpPassed = ETrue;
			return KErrNone;	
			}
		default:
			{
			aIfPtr = NULL;
			return KErrNotSupported;	
			}
		}	
	}
	
TInt CHistogramFactoryForClientViewFinder::GetImpl1(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/) const
	{
	return KErrNotSupported;		
	}
	
TInt CHistogramFactoryForClientViewFinder::GetImpl2(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/, TECamImplFactoryParam /*aParam2*/) const
	{
	return KErrNotSupported;	
	}
	
void CHistogramFactoryForClientViewFinder::Release()
	{
	delete this;
	}

// Histogram impl for image snapshot
CHistogramImpForImageSnapshot* CHistogramImpForImageSnapshot::NewL(CCamUnitTestPlugin& aOwner, CCamSnapshotForImage& aCamSnapshotForImageImpl)
	{
	return new(ELeave) CHistogramImpForImageSnapshot(aOwner, aCamSnapshotForImageImpl);
	}

CHistogramImpForImageSnapshot::CHistogramImpForImageSnapshot(CCamUnitTestPlugin& aOwner, CCamSnapshotForImage& aCamSnapshotForImageImpl):
									 iOwner(aOwner), 
									 iCamSnapshotForImageImpl(aCamSnapshotForImageImpl)
	{
	}

CHistogramImpForImageSnapshot::~CHistogramImpForImageSnapshot()
	{
	}
	
void CHistogramImpForImageSnapshot::Release(CCamera::CCameraV2Histogram* /*aHistogramHandle*/)
	{
	delete this;
	}
	
void CHistogramImpForImageSnapshot::SetHistogramHandle(CCamera::CCameraV2Histogram* aHistogramHandle)
	{
	iHistogramHandle = aHistogramHandle;
	}
	
void CHistogramImpForImageSnapshot::GetSupportedHistogramsL(TUint& aSupportedHistogramType) const
	{
	aSupportedHistogramType = CCamera::CCameraV2Histogram::EHistNone;
	}

void CHistogramImpForImageSnapshot::GetDirectHistogramSupportInfoL(TBool& /*aIsDirectHistogramSupported*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForImageSnapshot::PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType /*aType*/)
	{
	User::Leave(KErrNotSupported);
	}

void CHistogramImpForImageSnapshot::PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters)
	{
	if((aDirectHistogramParameters.Size() != sizeof(CCamera::CCameraV2Histogram::TDirectHistogramParameters)) ||
		(aDirectHistogramParameters.Version() != KECamDirectHistogramParametersCurrentVersion))
		{
		User::Leave(KErrNotSupported);
		}	
	User::Leave(KErrNotSupported);
	}
	
void CHistogramImpForImageSnapshot::UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& /*aDirectHistogramParameters*/)
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForImageSnapshot::GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& /*aDirectHistogramParameters*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
void CHistogramImpForImageSnapshot::StartHistogram()
	{
	RCamHistogramV2Buffer camHistogramV2Buffer;

	CamImageCaptureImpl().CaptureImageObserver()->ClientHistogramForSnapshotReady(CameraImageCapture(), 1, NULL, KErrNotSupported);
	}
	
void CHistogramImpForImageSnapshot::StopHistogram()
	{
	return;
	}

void CHistogramImpForImageSnapshot::GetHistogramStateL(TBool& /*aIsHistogramActive*/) const
	{
	User::Leave(KErrNotSupported);		
	}

//Hisotgram factory for Image Snapshot
CHistogramFactoryForImageSnapshot* CHistogramFactoryForImageSnapshot::NewL(CCamSnapshotForImage& aCamSnapshotForImageImpl)
	{
	CHistogramFactoryForImageSnapshot* self = new(ELeave) CHistogramFactoryForImageSnapshot();
	
	CleanupStack::PushL(self);
	self->ConstructL(aCamSnapshotForImageImpl);
	CleanupStack::Pop();
	
	return self;
	}
	
void CHistogramFactoryForImageSnapshot::ConstructL(CCamSnapshotForImage& aCamSnapshotForImageImpl)
	{
	iHistogramImpForImageSnapshot = CHistogramImpForImageSnapshot::NewL(aCamSnapshotForImageImpl.Owner(), aCamSnapshotForImageImpl);
	}

CHistogramFactoryForImageSnapshot::CHistogramFactoryForImageSnapshot() : iHistogramImpForImageSnapshot(NULL),
																		 iIsHistogramImpPassed(EFalse)
	{
	}
	
CHistogramFactoryForImageSnapshot::~CHistogramFactoryForImageSnapshot()
	{
	if(!iIsHistogramImpPassed)
		{
		delete iHistogramImpForImageSnapshot;
		}
	iHistogramImpForImageSnapshot = NULL;
	}
	
TInt CHistogramFactoryForImageSnapshot::GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const
	{
	switch(aIfaceUid.iUid)	
		{
		case KECamMCameraV2HistogramUidValue:
			{
			aIfPtr = static_cast<MCameraV2Histogram*>(iHistogramImpForImageSnapshot);
			iIsHistogramImpPassed = ETrue;
			return KErrNone;	
			}
		default:
			{
			aIfPtr = NULL;
			return KErrNotSupported;	
			}
		}	
	}
	
TInt CHistogramFactoryForImageSnapshot::GetImpl1(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/) const
	{
	return KErrNotSupported;		
	}
	
TInt CHistogramFactoryForImageSnapshot::GetImpl2(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/, TECamImplFactoryParam /*aParam2*/) const
	{
	return KErrNotSupported;	
	}
	
void CHistogramFactoryForImageSnapshot::Release()
	{
	delete this;
	}
	
//	
//img proc impl for still images.
CCamImgProcImpForImage* CCamImgProcImpForImage::NewL(CCamUnitTestPlugin& aOwner)
	{
	return new(ELeave) CCamImgProcImpForImage(aOwner);
	}
	
CCamImgProcImpForImage::CCamImgProcImpForImage(CCamUnitTestPlugin& aOwner): iOwner(aOwner)
	{
	}
	
CCamImgProcImpForImage::~CCamImgProcImpForImage()
	{
	}
	
void CCamImgProcImpForImage::Release()
	{
	delete this;
	}
	
void CCamImgProcImpForImage::GetSupportedTransformationsL(RArray<TUid>& aTransformations) const
	{
	aTransformations.Reset();
	}
	
void CCamImgProcImpForImage::GetActiveTransformationsL(RArray<TUid>& aTransformations) const
	{
	aTransformations.Reset();
	}
	
void CCamImgProcImpForImage::GetTransformationSupportedValuesL(TUid /*aTransformation*/, RArray<TInt>& aValues, TValueInfo& aInfo) const
	{
	aValues.Reset();
	aInfo = ENotActive;
	}
	
TInt CCamImgProcImpForImage::TransformationValue(TUid /*aTransformation*/) const
	{
	return KErrNotSupported;	
	}
	
TInt CCamImgProcImpForImage::GetTransformationValue(TUid /*aTransformation*/, TInt& /*aTransformationValue*/) const
	{
	return KErrNotSupported;	
	}
	
void CCamImgProcImpForImage::SetTransformationValue(TUid /*aTransformation*/, TInt /*aValue*/)
	{
	return;
	}
	
void CCamImgProcImpForImage::GetActiveTransformSequenceL(RArray<TUid>& aTransformSequence) const
	{
	aTransformSequence.Reset();
	}
	
void CCamImgProcImpForImage::SetActiveTransformSequenceL(RArray<TUid>& aTransformSequence)
	{
	aTransformSequence.Reset();	
	}
	
void CCamImgProcImpForImage::SetSourceRect(const TRect& /*aRect*/)
	{
	return;
	}

void CCamImgProcImpForImage::GetSourceRect(TRect& /*aRect*/) const
	{
	return;		
	}

//	
//img proc factory for still images.
CImgProcFactoryForImage* CImgProcFactoryForImage::NewL(CCamImageCapture& aCamImageCaptureImpl, CCamImgProcImpForImage* aCamImgProcImpForImage)
	{
	CImgProcFactoryForImage* self = new(ELeave) CImgProcFactoryForImage();
	
	CleanupStack::PushL(self);
	self->ConstructL(aCamImageCaptureImpl, aCamImgProcImpForImage);
	CleanupStack::Pop();
	
	return self;
	}
	
void CImgProcFactoryForImage::ConstructL(CCamImageCapture& aCamImageCaptureImpl, CCamImgProcImpForImage* aCamImgProcImpForImage)
	{
	if(aCamImgProcImpForImage == NULL)
		{
		iCamImgProcImpForImage = CCamImgProcImpForImage::NewL(aCamImageCaptureImpl.Owner());
		aCamImageCaptureImpl.iCamImgProcImpForImage = iCamImgProcImpForImage;
		}
	}

CImgProcFactoryForImage::CImgProcFactoryForImage() : iCamImgProcImpForImage(NULL),
													 iIsImgProcImpPassed(EFalse)
	{
	TAny* ptr = NULL;
	TUint value = 0;
	TInt param = -4;
	TECamImplFactoryParam factoryParam(ptr);
	TECamImplFactoryParam factoryParam1(value);
	TECamImplFactoryParam factoryParam2(param);
	}
	
CImgProcFactoryForImage::~CImgProcFactoryForImage()
	{
	if(!iIsImgProcImpPassed)
		{
		delete iCamImgProcImpForImage;
		}
	iCamImgProcImpForImage = NULL;
	}
	
TInt CImgProcFactoryForImage::GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const
	{
	switch(aIfaceUid.iUid)	
		{
		//fall through
		case KECamMCameraImageProcessingUidValue:
			{
			aIfPtr = static_cast<MCameraImageProcessing*>(iCamImgProcImpForImage);
			iIsImgProcImpPassed = ETrue;
			return KErrNone;	
			}
		default:
			{
			aIfPtr = NULL;
			return KErrNotSupported;	
			}
		}	
	}
	
TInt CImgProcFactoryForImage::GetImpl1(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/) const
	{
	return KErrNotSupported;		
	}
	
TInt CImgProcFactoryForImage::GetImpl2(TAny*& /*aIfPtr*/, TUid /*aIfaceUid*/, TECamImplFactoryParam /*aParam1*/, TECamImplFactoryParam /*aParam2*/) const
	{
	return KErrNotSupported;	
	}
	
void CImgProcFactoryForImage::Release()
	{
	delete this;
	}
