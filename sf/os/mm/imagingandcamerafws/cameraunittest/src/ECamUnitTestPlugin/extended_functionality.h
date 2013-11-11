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
// extendedfunctionality.h
// 
//

#ifndef EXTENDEDFUNCTIONALITY_H
#define EXTENDEDFUNCTIONALITY_H

#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>

#include "ECamUnitTestPlugin.h"
#include <fbs.h>

#include <ecam/mcamerahistogram.h>
#include <ecam/mcamerasnapshot.h>
#include <ecam/ecamcapturecontrolintf.h>
#include <ecam/ecamimageprocessingintf.h>
#include <ecam/ecamviewfinderintf.h>
#include <ecam/implementationfactoryintf.h>

class CCamUnitTestPlugin;

const TInt KSupportedDirectVCType		= 0;
const TInt KSupportedClientVCType		= 1;

//
// camera buffer 2
class RCamBuffer2 : public MCameraBuffer2
	{
public:
	RCamBuffer2(CFbsBitmap*& aFrame): iAvailable(ETrue), iFrame(aFrame) {}
	virtual TInt NumFrames() {return 0;}
	virtual TDesC8* DataL(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return NULL; }
	virtual CFbsBitmap& BitmapL(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return *iFrame;}
 	virtual	RChunk& ChunkL() { return iChunk;}
	virtual TInt ChunkOffsetL(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return 0;}
	virtual TInt FrameSize(TInt aFrameIndex) {if (aFrameIndex != 0) { User::Leave(KErrArgument); } return 0; }
	virtual void Release() {iAvailable = ETrue;}
	
	virtual TInt GetInterfaceUids(RArray<TUid>& aInterfaceUids) {aInterfaceUids.Reset(); return KErrNotSupported;}
	virtual TInt CustomInterface(TUid /*aInterface*/, TAny*& aPtrInterface) {aPtrInterface = NULL; return KErrNotSupported;}
	
public:	
	TBool iAvailable;
	CFbsBitmap*& iFrame;
	RChunk iChunk;
	};

//
//pre image capture control impl
class CCamPreImageCaptureControl: public CBase, public MCameraPreImageCaptureControl
	{
public:
	static CCamPreImageCaptureControl* NewL(CCamUnitTestPlugin& aOwner);
	~CCamPreImageCaptureControl();
	
	void Release();
	void SetPreImageCaptureControlObserver(MPreImageCaptureControlObserver& aPreImageCaptureControlObserver);
	void GetDirectSnapshotSupportInfoL(TUint& aDirectSnapshotSupportInfo) const;
	void GetSupportedEmbeddedStillCaptureSettingsL(RArray<TUid>& aSupportedEmbeddedStillCaptureSettings) const;
	void GetSupportedDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::TDirectSavingType& aSupportedDirectSavingType) const;
	void SetSequentialImageFilenameL(const TDesC8& aFilename, TInt aStartingSequenceNumber);
	void SetLowerResolutionSequentialImageFilenameL(const TDesC8& aLowerResolutionFilename, TInt aStartingSequenceNumber);
	void GetDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::TDirectSavingType& aDirectSavingType) const;
	void SetDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::TDirectSavingType aDirectSavingType);
	void GetCaptureEventSupportInfoL(TUint& aSupportedDriveModes) const;
	void GetImageFormatsSupportedL(TUint& aImageFormatsSupported, const TSize& aSize) const;
	void GetPixelAspectsSupportedL(TUint& aPixelAspectsSupported, CCamera::TFormat aImageFormat, const TSize& aSize) const;
	void PrepareImageCapture(const CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& 
										aPrepareImageParameters, MCaptureImageObserver& aCaptureImageObserver);
	void GetImageMaxMemorySizeSettingSupportInfoL(TBool& aIsImageMaxMemorySizeSettingSupported) const;
	void GetImageMaxMemorySizeL(TUint& aMemorySize) const;
	void GetSupportedProcessingOptionsL(TUint& aSupportedProcessingOptions) const;
	
	inline MPreImageCaptureControlObserver* PreImageCaptureControlObserver() {return iPreImageCaptureControlObserver;}
	
private:	
	CCamPreImageCaptureControl(CCamUnitTestPlugin& aOwner);
	
	void CreateCaptureImageL(CCamera::CCameraImageCapture*& aCameraImageCapture, const CCamera::CCameraPreImageCaptureControl::
			TPrepareImageParameters& aPrepareImageParameters, MCaptureImageObserver& aCaptureImageObserver);
	
private:
	CCamUnitTestPlugin& iOwner;
	MPreImageCaptureControlObserver* iPreImageCaptureControlObserver;
	};

//
//image capture impl	
class CCamSnapshotForImage;
class CCamImgProcImpForImage;
class CCamPostImageCaptureControl;

class CCamImageCapture: public CBase, public MCameraImageCapture
	{
friend class CSnapshotFactoryForImage;
friend class CImgProcFactoryForImage;
friend class CCamPostImageCaptureControl;

public:
	static CCamImageCapture* NewL(CCamUnitTestPlugin& aOwner);
	~CCamImageCapture();
	
	void Release(CCamera::CCameraImageCapture* aCameraImageCapture);
	void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void GetSnapshotImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void SetCaptureImageObserver(MCaptureImageObserver& aCaptureImageObserver);
	void GetPrepareImageParametersL(CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& aPrepareImageParameters) const;
	void SetPrepareImageParameters(const CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& aPrepareImageParameters);
	TAny* CreatePostImageCaptureControlImpl(TUid aInterface, TPostCaptureControlId aPostCaptureControlId);
	void CaptureImage(CCamera::CCameraImageCapture* aCameraImageCapture);
	void CancelCaptureImage();
	void GetNumImagesExposedL(TUint& aNumImagesExposed) const;
	void GetNumTotalImagesL(TUint& aNumTotalImages) const;
	void GetPostCaptureControlHandleL(CCamera::CCameraPostImageCaptureControl*& aPostCaptureControlHandle, 
									TPostCaptureControlId aPostCaptureControlId) const;
	void SetCaptureImagePriorityL(TECamImagePriority aCaptureImagePriority);
	void GetCaptureImagePriorityL(TECamImagePriority& aCaptureImagePriority) const;
	void PauseProcessing(TUint aProcessingType);
	void ResumeProcessingL(TUint aProcessingType);
	
	inline MCaptureImageObserver* CaptureImageObserver() {return iCaptureImageObserver;}
	inline CCamera::CCameraImageCapture* CameraImageCapture() {return iCameraImageCapture;}
	inline CCamUnitTestPlugin& Owner() {return iOwner;}

private:
	CCamImageCapture(CCamUnitTestPlugin& aOwner);
	void CreatePostImageCaptureControlL(CCamera::CCameraPostImageCaptureControl*& aPostCaptureControlHandle, 
						TPostCaptureControlId aPostCaptureControlId);
	
private:
	CCamUnitTestPlugin& iOwner;
	MCaptureImageObserver* iCaptureImageObserver;
	CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters iPrepareImageParameters;
	
	mutable CCamSnapshotForImage* iSnapshotImpForImage;
	CCamImgProcImpForImage* iCamImgProcImpForImage;
	RArray<TInt> iArrayPostCaptureControlId;
	RArray<CCamPostImageCaptureControl*> iArrayPostCaptureControlImpl;
	RArray<CCamera::CCameraPostImageCaptureControl*> iArrayPostCaptureControlHandle;

	CCamera::CCameraImageCapture* iCameraImageCapture;
	};

//
//post image capture control impl
class CCamPostImageCaptureControl: public CBase, public MCameraPostImageCaptureControl
	{
public:
	static CCamPostImageCaptureControl* NewL(CCamImageCapture& aCamImageCaptureImpl);
	~CCamPostImageCaptureControl();
	
	void Release(TPostCaptureControlId aPostCaptureControlId);
	void GetImageSequenceNumberL(TUint& aSequenceNumber) const;
	void CancelImage();
	void SetImagePriorityL(TECamImagePriority aImagePriority);
	void GetImagePriorityL(TECamImagePriority& aImagePriority) const;
	void PauseProcessing(TUint aProcessingType);
	void ResumeProcessingL(TUint aProcessingType);
	void GetImageBufferL(MCameraImageBuffer& aCameraImageBuffer) const;
	void GetImageStateL(CCamera::CCameraPostImageCaptureControl::TImageState& aImageState) const;
	void GetBufferStateL(CCamera::CCameraPostImageCaptureControl::TBufferState& aBufferState) const;

	inline CCamImageCapture& CamImageCaptureImpl() {return iCamImageCaptureImpl;}

private:
	CCamPostImageCaptureControl(CCamImageCapture& aCamImageCaptureImpl);
	
private:
	CCamImageCapture& iCamImageCaptureImpl;
	};

//
//video capture control impl
class CCamSnapshotForVideo;	
class CCamVideoCaptureControl : public CBase, public MCameraVideoCaptureControl
	{
friend class CSnapshotFactoryForVideo;

public:
	static CCamVideoCaptureControl* NewL(CCamUnitTestPlugin& aOwner);
	~CCamVideoCaptureControl();
	
	void Release();
	void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void GetSnapshotImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void SetCaptureVideoObserver(MCaptureVideoObserver& aCaptureVideoObserver);
	void GetVideoFormatsSupportedL(TUint& aVideoFormatsSupported, const TSize& aSize) const;
	void GetPixelAspectsSupportedL(TUint& aPixelAspectsSupported, CCamera::TFormat aVideoFormat, const TSize& aSize) const;
	void GetEmbeddedStillCaptureSupportInfoL(TInt& aSupportedEmbeddedStillCaptureTypes) const;
	void PrepareVideoCapture(const CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters& aPrepareVideoParameters);
	void GetRangeAffectedSettingsL(RArray<TUid>& aRangeAffectedSettings) const;
	void GetValueAffectedSettingsL(RArray<TUid>& aValueAffectedSettings) const;
	void GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const;
	void ReleaseVideoResource();
	void StartVideoCaptureL();
	void StopVideoCapture();
	void PauseVideoCapture();
	void ResumeVideoCaptureL();
	void GetFadingEffectStateL(CCamera::CCameraVideoCaptureControl::TFadingEffectState& aFadingEffectState) const;
	void SetFadingEffectState(CCamera::CCameraVideoCaptureControl::TFadingEffectState aFadingEffectState);
	void GetVideoCaptureStateL(CCamera::CCameraVideoCaptureControl::TVideoCaptureState& aVideoCaptureState) const;
	void GetVideoCaptureSupportInfoL(TInt& aSupportedVideoCaptureTypes) const;
	void GetPrepareVideoParametersL(CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters& aPrepareVideoParameters) const;

	inline MCaptureVideoObserver* CaptureVideoObserver() {return iCaptureVideoObserver;}
	inline CCamUnitTestPlugin& Owner() {return iOwner;}
	
private:
	CCamVideoCaptureControl(CCamUnitTestPlugin& aOwner);
	void ConstructL();
	
private:
	CCamUnitTestPlugin& iOwner;	
	MCaptureVideoObserver* iCaptureVideoObserver;
	CCamSnapshotForVideo* iSnapshotImpForVideo;
	CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters iPrepareVideoParameters;
	CFbsBitmap* iBitmap;
	};
	
//	
//direct snapshot implementation to be used by version2 direct viewfinder.
class CCamDirectSnapshot : public CBase, public MCameraDirectSnapshot
	{
public:
	
	static CCamDirectSnapshot* NewL();
	~CCamDirectSnapshot();

	void Release();
	void SetDirectViewFinder(const CCamera::CCameraV2DirectViewFinder* aDirectViewFinder);
	void GetDirectViewFinderL(CCamera::CCameraV2DirectViewFinder*& aDirectViewFinder) const;
	void EnableDirectSnapshotL(CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters);
	void DisableDirectSnapshot();
	void GetDirectSnapshotStateL(CCamera::CCameraDirectSnapshot::TDirectSnapshotState& aDirectSnapshotState) const;
	void GetDirectSnapshotParametersL(CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters) const;
	void SetDirectSnapshotParametersL(const CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters& aDirectSnapshotParameters);
	
private:
	CCamDirectSnapshot();

private:
	const CCamera::CCameraV2DirectViewFinder* iCameraV2DirectVF;
	CCamera::CCameraDirectSnapshot::TDirectSnapshotParameters iDirectSnapshotParameters;
	CCamera::CCameraDirectSnapshot::TDirectSnapshotState iDirectSnapshotState;
	};
	
//
//V2 direct viewfinder impl
class CCamV2DirectViewFinder: public CBase, public MCameraViewFinder, public MCameraV2DirectViewFinder
	{
public:
	static CCamV2DirectViewFinder* NewL(CCamUnitTestPlugin& aOwner);
	~CCamV2DirectViewFinder();
	
	void Release(CCamera::CCameraV2DirectViewFinder* aDirectViewFinderHandle);
	void Release();
	void GetViewFinderFadingCapabilitiesL(CCameraViewFinder::TViewFinderFadingCapabilities& aVFFadingCapabilities) const;
	void GetViewFinderFadingEffectL(CCameraViewFinder::TViewFinderFadingEffect& aCurrentVFFadingEffect) const;
	void SetViewFinderFadingEffect(const CCameraViewFinder::TViewFinderFadingEffect& aVFFadingEffect);
	void GetViewFinderHandleL(TInt& aVFHandle) const;
	
	void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void SetDirectViewFinderObserver(MDirectViewFinderObserver& aDirectViewFinderObserver);
	void SetDirectViewFinderHandle(CCamera::CCameraV2DirectViewFinder* aDirectViewFinderHandle);
	void StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow, TRect& aScreenRect);
	void StartViewFinderDirectL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindowBase& aWindow, TRect& aScreenRect, TRect& aClipRect);
	void GetDirectViewFinderPropertiesL(TInt& aScreenNumber, TRect& aScreenRect, TRect& aClipRect) const;
	void PauseViewFinderDirect();
	void ResumeViewFinderDirect();
	void GetViewFinderStateL(CCamera::CCameraV2DirectViewFinder::TViewFinderState& aViewFinderState) const;
	void StopDirectViewFinder();
	TAny* GetDirectSnapshotImpl(TUid aInterface) const;
		
	inline MDirectViewFinderObserver* DirectViewFinderObserver() {return iDirectViewFinderObserver;}
	inline CCamera::CCameraV2DirectViewFinder& DirectViewFinderHandle() {return *iDirectViewFinderHandle;}
	inline CCamUnitTestPlugin& Owner() {return iOwner;}
	
private:
	CCamV2DirectViewFinder(CCamUnitTestPlugin& aOwner);

public:
	TInt iRefCount;

private:
	CCamUnitTestPlugin& iOwner;
	MDirectViewFinderObserver* iDirectViewFinderObserver;
	CCamera::CCameraV2DirectViewFinder* iDirectViewFinderHandle;
	TInt iVFHandle;
	mutable CCamDirectSnapshot* iDirectSnapshot;
	};

//
//client viewfinder impl
class CCamClientViewFinder: public CBase, public MCameraViewFinder, public MCameraClientViewFinder
	{
public:
	static CCamClientViewFinder* NewL(CCamUnitTestPlugin& aOwner);
	~CCamClientViewFinder();
	
	void Release(CCamera::CCameraClientViewFinder* aClientViewFinderHandle);
	void Release();
	void GetViewFinderFadingCapabilitiesL(CCameraViewFinder::TViewFinderFadingCapabilities& aVFFadingCapabilities) const;
	void GetViewFinderFadingEffectL(CCameraViewFinder::TViewFinderFadingEffect& aCurrentVFFadingEffect) const;
	void SetViewFinderFadingEffect(const CCameraViewFinder::TViewFinderFadingEffect& aVFFadingEffect);
	void GetViewFinderHandleL(TInt& aVFHandle) const;
	
	void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void GetImageProcessingImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void SetClientViewFinderObserver(MClientViewFinderObserver& aClientViewFinderObserver);
	void SetClientViewFinderHandle(CCamera::CCameraClientViewFinder* aClientViewFinderHandle);
	
	void StartClientViewFinderL(TInt aScreenNumber, CCamera::TFormat aImageFormat, TSize& aSize);
	void StartClientViewFinderL(TInt aScreenNumber, CCamera::TFormat aImageFormat, TSize& aSize, TRect& aClipRect);
	void GetClientViewFinderStateL(TBool& aIsActive) const;
	void GetClientViewFinderPropertiesL(TInt& aScreenNumber, CCamera::TFormat& aImageFormat, TSize& aSize, TRect& aClipRect) const;
	void StopClientViewFinder();
	void GetViewFinderBufferL(MCameraBuffer2& aClientViewFinderBuffer) const;
	
	inline MClientViewFinderObserver* ClientViewFinderObserver() {return iClientViewFinderObserver;}
	inline CCamera::CCameraClientViewFinder& ClientViewFinderHandle() {return *iClientViewFinderHandle;}
	inline CCamUnitTestPlugin& Owner() {return iOwner;}
	
private:
	CCamClientViewFinder(CCamUnitTestPlugin& aOwner);

public:
	TInt iRefCount;

private:
	CCamUnitTestPlugin& iOwner;
	MClientViewFinderObserver* iClientViewFinderObserver;
	CCamera::CCameraClientViewFinder* iClientViewFinderHandle;
	TInt iVFHandle;
	};
	
//
//snapshot impl for image
class CCamSnapshotForImage : public CBase, public MCameraSnapshot, public MCameraSnapshot2
	{
friend class CHistogramImpForImageSnapshot;

public:	
	static CCamSnapshotForImage* NewL(CCamUnitTestPlugin& aOwner, CCamImageCapture& aCamImageCaptureImpl);
	~CCamSnapshotForImage();
	
	TUint32 SupportedFormats();
    void PrepareSnapshotL(CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio);
    void PrepareSnapshotL(CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio);
    
    void SetBgColorL(const TRgb& aBgColor); 
	void SetPositionL(const TPoint& aPosition);
	
    TBool IsSnapshotActive() const;
    void StartSnapshot();
	void StopSnapshot();
	MCameraBuffer& SnapshotDataL(RArray<TInt>& aFrameIndexOrder);
	
	void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void PrepareSnapshotL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);
	void GetSnapshotParametersL(CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);
	void SetSnapshotParametersL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);
	void GetSnapshotStatusL(CCamera::CCameraSnapshot::TSnapshotState& aSnapshotState) const;
	void SelectSnapshotVideoFramesL(CCamera::CCameraSnapshot::TSnapshotVideoFrames aSnapshotVideoFrames);
	void EnableSnapshot();
	void DisableSnapshot();
	void SetClientViewFinderId(TInt aClientViewFinderId);
	
	void Release();
	
	inline CCamImageCapture& CamImageCaptureImpl() {return iCamImageCaptureImpl;}
	inline CCamera::CCameraImageCapture& CameraImageCapture() {return *(CamImageCaptureImpl().CameraImageCapture());}
	inline CCamUnitTestPlugin& Owner() {return iOwner;}
private:	
	CCamSnapshotForImage(CCamUnitTestPlugin& aOwner, CCamImageCapture& aCamImageCaptureImpl);
	void ConstructL();

private:
	CCamUnitTestPlugin& iOwner;
	CCamImageCapture& iCamImageCaptureImpl;
	CFbsBitmap* iBitmap;
	RCamBuffer2 iSnapshotbuffer;
	CCamera::CCameraSnapshot::TSnapshotParameters iSnapshotParameters;
	
	TInt iRefCount;
	};

//
//snapshot factory for image
class CSnapshotFactoryForImage : public CBase, public MImplementationFactory
	{
public:
	static CSnapshotFactoryForImage* NewL(CCamImageCapture& aCamImageCaptureImpl, CCamSnapshotForImage* aSnapshotImpForImage);
	~CSnapshotFactoryForImage();
	
	TInt GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const;
	TInt GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const;
	TInt GetImpl2(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1, TECamImplFactoryParam aParam2) const;
	void Release();
	
private:
	CSnapshotFactoryForImage();
	void ConstructL(CCamImageCapture& aCamImageCaptureImpl, CCamSnapshotForImage* aSnapshotImpForImage);

private:
	CCamSnapshotForImage* iSnapshotImpForImage;
	mutable TBool iIsSnapshotImpPassed;
	};

//
//snapshot impl for video
class CCamSnapshotForVideo : public CBase, public MCameraSnapshot, public MCameraSnapshot2
	{
public:	
	static CCamSnapshotForVideo* NewL(CCamUnitTestPlugin& aOwner, CCamVideoCaptureControl& aCamVideoCaptureControlImpl);
	~CCamSnapshotForVideo();
	
	TUint32 SupportedFormats();
    void PrepareSnapshotL(CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio);
    void PrepareSnapshotL(CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio);
    
    void SetBgColorL(const TRgb& aBgColor); 
	void SetPositionL(const TPoint& aPosition);
	
    TBool IsSnapshotActive() const;
    void StartSnapshot();
	void StopSnapshot();
	MCameraBuffer& SnapshotDataL(RArray<TInt>& aFrameIndexOrder);
	
	void CreateHistogramImplFactoryL(MImplementationFactory*& aImplFactoryPtr) const;
	void PrepareSnapshotL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);
	void GetSnapshotParametersL(CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);
	void SetSnapshotParametersL(const CCamera::CCameraSnapshot::TSnapshotParameters& aSnapshotParameters);
	void GetSnapshotStatusL(CCamera::CCameraSnapshot::TSnapshotState& aSnapshotState) const;
	void SelectSnapshotVideoFramesL(CCamera::CCameraSnapshot::TSnapshotVideoFrames aSnapshotVideoFrames);
	void EnableSnapshot();
	void DisableSnapshot();
	void SetClientViewFinderId(TInt aClientViewFinderId);
	
	void Release();
	
	inline CCamVideoCaptureControl& CamVideoCaptureControlImpl() {return iCamVideoCaptureControlImpl;}

private:	
	CCamSnapshotForVideo(CCamUnitTestPlugin& aOwner, CCamVideoCaptureControl& aCamVideoCaptureControlImpl);
	void ConstructL();
	
private:
	CCamUnitTestPlugin& iOwner;
	CCamVideoCaptureControl& iCamVideoCaptureControlImpl;
	CFbsBitmap* iBitmap;
	RCamBuffer2 iSnapshotbuffer;
	CCamera::CCameraSnapshot::TSnapshotParameters iSnapshotParameters;
	
	TInt iRefCount;
	};

//
//snapshot factory for video
class CSnapshotFactoryForVideo : public CBase, public MImplementationFactory
	{
public:
	static CSnapshotFactoryForVideo* NewL(CCamVideoCaptureControl& aCamVideoCaptureControlImpl, CCamSnapshotForVideo* aSnapshotImpForVideo);
	~CSnapshotFactoryForVideo();
	
	TInt GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const;
	TInt GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const;
	TInt GetImpl2(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1, TECamImplFactoryParam aParam2) const;
	void Release();
	
private:
	CSnapshotFactoryForVideo();
	void ConstructL(CCamVideoCaptureControl& aCamVideoCaptureControlImpl, CCamSnapshotForVideo* aSnapshotImpForVideo);

private:
	CCamSnapshotForVideo* iSnapshotImpForVideo;
	mutable TBool iIsSnapshotImpPassed;
	};

//
// V2 buffer for histogram: MHistogramV2Buffer
class RCamHistogramV2Buffer : public MHistogramV2Buffer
	{

public:
	RCamHistogramV2Buffer(): iAvailable(ETrue) {}
	CCamera::CCameraV2Histogram::THistogramType Type() {return CCamera::CCameraV2Histogram::EHistNone;}
	TInt ElementSize() {return -5;}
	TInt NumOfElements() {return -5;}
	TDesC8* DataL() {User::Leave(KErrNotSupported); return NULL;}
	RChunk& ChunkL() {User::Leave(KErrNotSupported); return iChunk;}
	TInt ChunkOffsetL() {User::Leave(KErrNotSupported); return -5;}
	void Release() {iAvailable = EFalse;}

public:

	TBool iAvailable;
	RChunk iChunk;	
	};
	
//	
//histogram impl for video
class CHistogramImpForVideo : public CBase, public MCameraV2Histogram 
	{

public:
	static CHistogramImpForVideo* NewL(CCamUnitTestPlugin& aOwner, CCamVideoCaptureControl& aCamVideoCaptureControlImpl);
	~CHistogramImpForVideo();
	
	void Release(CCamera::CCameraV2Histogram* aHistogramHandle);
	void SetHistogramHandle(CCamera::CCameraV2Histogram* aHistogramHandle);
	void GetSupportedHistogramsL(TUint& aSupportedHistogramType) const;
	void GetDirectHistogramSupportInfoL(TBool& aIsDirectHistogramSupported) const;
	void PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType aType);
	void PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters);	
	void UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters);
	void GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters) const;
	void StartHistogram();
	void StopHistogram();
	void GetHistogramStateL(TBool& aIsHistogramActive) const;
	
	inline CCamVideoCaptureControl& CamVideoCaptureControlImpl() {return iCamVideoCaptureControlImpl;}
	
private:
	CHistogramImpForVideo(CCamUnitTestPlugin& aOwner, CCamVideoCaptureControl& aCamVideoCaptureControlImpl);

private:
	CCamUnitTestPlugin& iOwner;
	CCamVideoCaptureControl& iCamVideoCaptureControlImpl;
	};

//	
//histogram factory for video	
class CHistogramFactoryForVideo : public CBase, public MImplementationFactory
	{
public:
	static CHistogramFactoryForVideo* NewL(CCamVideoCaptureControl& aCamVideoCaptureControlImpl);
	~CHistogramFactoryForVideo();
	
	TInt GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const;
	TInt GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const;
	TInt GetImpl2(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1, TECamImplFactoryParam aParam2) const;
	void Release();
	
private:
	CHistogramFactoryForVideo();
	void ConstructL(CCamVideoCaptureControl& aCamVideoCaptureControlImpl);

private:
	CHistogramImpForVideo* iHistogramImpForVideo;
	mutable TBool iIsHistogramImpPassed;
	};
	
//	
//histogram impl for image
class CHistogramImpForImage : public CBase, public MCameraV2Histogram 
	{

public:
	static CHistogramImpForImage* NewL(CCamUnitTestPlugin& aOwner, CCamImageCapture& aCamImageCaptureImpl);
	~CHistogramImpForImage();
	
	void Release(CCamera::CCameraV2Histogram* aHistogramHandle);
	void SetHistogramHandle(CCamera::CCameraV2Histogram* aHistogramHandle);
	void GetSupportedHistogramsL(TUint& aSupportedHistogramType) const;
	void GetDirectHistogramSupportInfoL(TBool& aIsDirectHistogramSupported) const;
	void PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType aType);
	void PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters);	
	void UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters);
	void GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters) const;
	void StartHistogram();
	void StopHistogram();
	void GetHistogramStateL(TBool& aIsHistogramActive) const;
	
	inline CCamImageCapture& CamImageCaptureImpl() {return iCamImageCaptureImpl;}
	inline CCamera::CCameraImageCapture& CameraImageCapture() {return *(CamImageCaptureImpl().CameraImageCapture());}
	
private:
	CHistogramImpForImage(CCamUnitTestPlugin& aOwner, CCamImageCapture& aCamImageCaptureImpl);

private:
	CCamUnitTestPlugin& iOwner;
	CCamImageCapture& iCamImageCaptureImpl;
	CCamera::CCameraV2Histogram* iHistogramHandle;
	};

//	
//histogram factory for image	
class CHistogramFactoryForImage : public CBase, public MImplementationFactory
	{
public:
	static CHistogramFactoryForImage* NewL(CCamImageCapture& aCamImageCaptureImpl);
	~CHistogramFactoryForImage();
	
	TInt GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const;
	TInt GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const;
	TInt GetImpl2(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1, TECamImplFactoryParam aParam2) const;
	void Release();
	
private:
	CHistogramFactoryForImage();
	void ConstructL(CCamImageCapture& aCamImageCaptureImpl);

private:
	CHistogramImpForImage* iHistogramImpForImage;
	mutable TBool iIsHistogramImpPassed;
	};
	
//	
//histogram impl for DirectViewFinder
class CHistogramImpForDirectViewFinder : public CBase, public MCameraV2Histogram 
	{

public:
	static CHistogramImpForDirectViewFinder* NewL(CCamUnitTestPlugin& aOwner, CCamV2DirectViewFinder& aCamV2DirectViewFinderImpl);
	~CHistogramImpForDirectViewFinder();
	
	void Release(CCamera::CCameraV2Histogram* aHistogramHandle);
	void SetHistogramHandle(CCamera::CCameraV2Histogram* aHistogramHandle);
	void GetSupportedHistogramsL(TUint& aSupportedHistogramType) const;
	void GetDirectHistogramSupportInfoL(TBool& aIsDirectHistogramSupported) const;
	void PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType aType);
	void PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters);	
	void UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters);
	void GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters) const;
	void StartHistogram();
	void StopHistogram();
	void GetHistogramStateL(TBool& aIsHistogramActive) const;
	
	inline CCamV2DirectViewFinder& DirectViewFinderImpl() {return iCamV2DirectViewFinderImpl;}
	
private:
	CHistogramImpForDirectViewFinder(CCamUnitTestPlugin& aOwner, CCamV2DirectViewFinder& aCamV2DirectViewFinderImpl);

private:
	CCamUnitTestPlugin& iOwner;
	CCamV2DirectViewFinder& iCamV2DirectViewFinderImpl;
	CCamera::CCameraV2Histogram* iHistogramHandle;
	};

//	
//histogram factory for DirectViewFinder
class CHistogramFactoryForDirectViewFinder : public CBase, public MImplementationFactory
	{
public:
	static CHistogramFactoryForDirectViewFinder* NewL(CCamV2DirectViewFinder& aCamV2DirectViewFinderImpl);
	~CHistogramFactoryForDirectViewFinder();
	
	TInt GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const;
	TInt GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const;
	TInt GetImpl2(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1, TECamImplFactoryParam aParam2) const;
	void Release();
	
private:
	CHistogramFactoryForDirectViewFinder();
	void ConstructL(CCamV2DirectViewFinder& aCamV2DirectViewFinderImpl);

private:
	CHistogramImpForDirectViewFinder* iHistogramImpForDirectViewFinder;
	mutable TBool iIsHistogramImpPassed;
	};

//	
//histogram impl for ClientViewFinder
class CHistogramImpForClientViewFinder : public CBase, public MCameraV2Histogram 
	{

public:
	static CHistogramImpForClientViewFinder* NewL(CCamUnitTestPlugin& aOwner, CCamClientViewFinder& aCamClientViewFinderImpl);
	~CHistogramImpForClientViewFinder();
	
	void Release(CCamera::CCameraV2Histogram* aHistogramHandle);
	void SetHistogramHandle(CCamera::CCameraV2Histogram* aHistogramHandle);
	void GetSupportedHistogramsL(TUint& aSupportedHistogramType) const;
	void GetDirectHistogramSupportInfoL(TBool& aIsDirectHistogramSupported) const;
	void PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType aType);
	void PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters);	
	void UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters);
	void GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters) const;
	void StartHistogram();
	void StopHistogram();
	void GetHistogramStateL(TBool& aIsHistogramActive) const;
	
	inline CCamClientViewFinder& ClientViewFinderImpl() {return iCamClientViewFinderImpl;}
	inline CCamera::CCameraClientViewFinder& ClientViewFinderHandle() {return ClientViewFinderImpl().ClientViewFinderHandle();}
	
private:
	CHistogramImpForClientViewFinder(CCamUnitTestPlugin& aOwner, CCamClientViewFinder& aCamClientViewFinderImpl);

private:
	CCamUnitTestPlugin& iOwner;
	CCamClientViewFinder& iCamClientViewFinderImpl;
	CCamera::CCameraV2Histogram* iHistogramHandle;
	};

//	
//histogram factory for ClientViewFinder		
class CHistogramFactoryForClientViewFinder : public CBase, public MImplementationFactory
	{
public:
	static CHistogramFactoryForClientViewFinder* NewL(CCamClientViewFinder& aCamClientViewFinderImpl);
	~CHistogramFactoryForClientViewFinder();
	
	TInt GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const;
	TInt GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const;
	TInt GetImpl2(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1, TECamImplFactoryParam aParam2) const;
	void Release();
	
private:
	CHistogramFactoryForClientViewFinder();
	void ConstructL(CCamClientViewFinder& aCamClientViewFinderImpl);

private:
	CHistogramImpForClientViewFinder* iHistogramImpForClientViewFinder;
	mutable TBool iIsHistogramImpPassed;
	};

//	
//histogram impl for image snapshot
class CHistogramImpForImageSnapshot : public CBase, public MCameraV2Histogram 
	{

public:
	static CHistogramImpForImageSnapshot* NewL(CCamUnitTestPlugin& aOwner, CCamSnapshotForImage& aCamSnapshotForImageImpl);
	~CHistogramImpForImageSnapshot();
	
	void Release(CCamera::CCameraV2Histogram* aHistogramHandle);
	void SetHistogramHandle(CCamera::CCameraV2Histogram* aHistogramHandle);
	void GetSupportedHistogramsL(TUint& aSupportedHistogramType) const;
	void GetDirectHistogramSupportInfoL(TBool& aIsDirectHistogramSupported) const;
	void PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType aType);
	void PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters);	
	void UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters);
	void GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters) const;
	void StartHistogram();
	void StopHistogram();
	void GetHistogramStateL(TBool& aIsHistogramActive) const;
	
	inline CCamSnapshotForImage& CamSnapshotForImageImpl() {return iCamSnapshotForImageImpl;}
	inline CCamera::CCameraV2Histogram* HistogramHandle() {return iHistogramHandle;}
	inline CCamImageCapture& CamImageCaptureImpl() {return CamSnapshotForImageImpl().CamImageCaptureImpl();}
	inline CCamera::CCameraImageCapture& CameraImageCapture() {return *(CamImageCaptureImpl().CameraImageCapture());}
	
private:
	CHistogramImpForImageSnapshot(CCamUnitTestPlugin& aOwner, CCamSnapshotForImage& aCamSnapshotForImageImpl);

private:
	CCamUnitTestPlugin& iOwner;
	CCamSnapshotForImage& iCamSnapshotForImageImpl;
	CCamera::CCameraV2Histogram* iHistogramHandle;
	};	

//	
//histogram factory for image snapshot
class CHistogramFactoryForImageSnapshot : public CBase, public MImplementationFactory
	{
public:
	static CHistogramFactoryForImageSnapshot* NewL(CCamSnapshotForImage& aCamSnapshotForImageImpl);
	~CHistogramFactoryForImageSnapshot();
	
	TInt GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const;
	TInt GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const;
	TInt GetImpl2(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1, TECamImplFactoryParam aParam2) const;
	void Release();
	
private:
	CHistogramFactoryForImageSnapshot();
	void ConstructL(CCamSnapshotForImage& aCamSnapshotForImageImpl);

private:
	CHistogramImpForImageSnapshot* iHistogramImpForImageSnapshot;
	mutable TBool iIsHistogramImpPassed;
	};

//	
//img proc impl for still images.
class CCamImgProcImpForImage : public CBase, public MCameraImageProcessing
	{
public:
	static CCamImgProcImpForImage* NewL(CCamUnitTestPlugin& aOwner);
	~CCamImgProcImpForImage();
	
	void Release();
	void GetSupportedTransformationsL(RArray<TUid>& aTransformations) const;
	void GetActiveTransformationsL(RArray<TUid>& aTransformations) const;
	void GetTransformationSupportedValuesL(TUid aTransformation, RArray<TInt>& aValues, TValueInfo& aInfo) const;
	TInt TransformationValue(TUid aTransformation) const;
	TInt GetTransformationValue(TUid aTransformation, TInt& aTransformationValue) const;
	void SetTransformationValue(TUid aTransformation, TInt aValue);
	void GetActiveTransformSequenceL(RArray<TUid>& aTransformSequence) const;
	void SetActiveTransformSequenceL(RArray<TUid>& aTransformSequence);
	void SetSourceRect(const TRect& aRect);
	void GetSourceRect(TRect& aRect) const;

private:
	CCamImgProcImpForImage(CCamUnitTestPlugin& aOwner);
	
private:
	CCamUnitTestPlugin& iOwner;
	};

//	
//img proc factory for still images.
class CImgProcFactoryForImage : public CBase, public MImplementationFactory
	{
public:
	static CImgProcFactoryForImage* NewL(CCamImageCapture& aCamImageCaptureImpl, CCamImgProcImpForImage* aCamImgProcImpForImage);
	~CImgProcFactoryForImage();
	
	TInt GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const;
	TInt GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const;
	TInt GetImpl2(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1, TECamImplFactoryParam aParam2) const;
	void Release();
	
private:
	CImgProcFactoryForImage();
	void ConstructL(CCamImageCapture& aCamImageCaptureImpl, CCamImgProcImpForImage* aCamImgProcImpForImage);

private:
	CCamImgProcImpForImage* iCamImgProcImpForImage;
	mutable TBool iIsImgProcImpPassed;
	};

#endif // EXTENDEDFUNCTIONALITY_H
