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
// Header file: DevVideoCiuPlay tests.
// 
//

/**
 @file testdevvideociuplay.h
*/

#ifndef TESTDEVVIDEOCIUPLAY_H
#define TESTDEVVIDEOCIUPLAY_H

#include <mmf/devvideo/devvideostandardcustominterfaces.h>
#include <mmf/devvideo/videoplayhwdevice.h>
#include <mmf/devvideo/devvideoconstants.h>
#include <mmf/devvideo/devvideoplayratecustominterface.h>

#ifdef SYMBIAN_BUILD_GCE
#include <mmf/devvideo/devvideosurfacehandlecustominterface.h>
#include <mmf/devvideo/devvideovideosurfacecustominterface.h>
#endif // SYMBIAN_BUILD_GCE

#include "tsu_mmf_devvideociu.h"

/**
 * DevVideoPlay test base class
 *
 * @class CTestDevVideoCiuPlayStep
*/
class CTestDevVideoCiuPlayStep : public CTestDevVideoCiuStep, public MMMFDevVideoPlayObserver
	{
public:
	CTestDevVideoCiuPlayStep(const TDesC& aTestName);
	~CTestDevVideoCiuPlayStep();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL() = 0;
	
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
		
	// MMMFDevVideoPlayObserver
    virtual void MdvpoNewBuffers();
    virtual void MdvpoReturnPicture(TVideoPicture* aPicture);
    virtual void MdvpoSupplementalInformation(const TDesC8& aData, 
											  const TTimeIntervalMicroSeconds& aTimestamp, 
											  const TPictureId& aPictureId);
	virtual void MdvpoPictureLoss();
	virtual void MdvpoPictureLoss(const TArray<TPictureId>& aPictures);
	virtual void MdvpoSliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture);
	virtual void MdvpoReferencePictureSelection(const TDesC8& aSelectionData);
    virtual void MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& aPresentationTimestamp, 
											const TPictureId& aPictureId);
    virtual void MdvpoNewPictures();
    virtual void MdvpoFatalError(TInt aError);
    virtual void MdvpoInitComplete(TInt aError);
    virtual void MdvpoStreamEnd();

protected:
	TInt iError;
	THwDeviceId iHwDeviceId;
	CMMFDevVideoPlay* iDevVideoPlay;
	};

//
//	Video Window Control Tests
//

class CTestDevVideoCiuPlayGetAndSetLogicalWindow : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayGetAndSetLogicalWindow* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayGetAndSetLogicalWindow(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetAndSetLogicalWindow() {};
	};

//

class CTestDevVideoCiuPlayGetAndSetContentOffset : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayGetAndSetContentOffset* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayGetAndSetContentOffset(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetAndSetContentOffset() {};
	};

//

class CTestDevVideoCiuPlayGetAndSetContentAlignment : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayGetAndSetContentAlignment* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayGetAndSetContentAlignment(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetAndSetContentAlignment() {};
	};

//
//	Video Resource Management Tests
//

class CTestDevVideoCiuPlayGetAndSetResourceObserver : public CTestDevVideoCiuPlayStep,
													  public MMmfVideoResourceObserver
	{
public:
	static CTestDevVideoCiuPlayGetAndSetResourceObserver* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
	
	//from MMmfVideoResourceObserver
	void MmvroResourcesLost(TUid aMediaDevice);
	void MmvroResourcesRestored(TUid aMediaDevice);
	
private:
	CTestDevVideoCiuPlayGetAndSetResourceObserver(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetAndSetResourceObserver() {};
	};

//

class CTestDevVideoCiuPlayResourceLost : public CTestDevVideoCiuPlayStep,
										 public MMmfVideoResourceObserver
	{
public:
	static CTestDevVideoCiuPlayResourceLost* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
	
	//from MMmfVideoResourceObserver
	void MmvroResourcesLost(TUid aMediaDevice);
	void MmvroResourcesRestored(TUid aMediaDevice);
	
private:
	CTestDevVideoCiuPlayResourceLost(const TDesC& aTestName);
	~CTestDevVideoCiuPlayResourceLost() {};
private:
	TBool iCallbackReceived;
	};

//

class CTestDevVideoCiuPlayResourceRestored : public CTestDevVideoCiuPlayStep,
										     public MMmfVideoResourceObserver
	{
public:
	static CTestDevVideoCiuPlayResourceRestored* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
	
	//from MMmfVideoResourceObserver
	void MmvroResourcesLost(TUid aMediaDevice);
	void MmvroResourcesRestored(TUid aMediaDevice);
	
private:
	CTestDevVideoCiuPlayResourceRestored(const TDesC& aTestName);
	~CTestDevVideoCiuPlayResourceRestored() {};
private:
	TBool iCallbackReceived;
	};

//

class CTestDevVideoCiuPlayGetAndSetResourcePriority : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayGetAndSetResourcePriority* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayGetAndSetResourcePriority(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetAndSetResourcePriority() {};
	};

//
//	Video Properties Management Tests
//

class CTestDevVideoCiuPlaySetAndGetPropertiesObserver : public CTestDevVideoCiuPlayStep,
													  public MMmfVideoPropertiesObserver
	{
public:
	static CTestDevVideoCiuPlaySetAndGetPropertiesObserver* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
	
	//from MMmfVideoPropertiesObserver
	void MmvpoUpdateVideoProperties(const TYuvFormat& aYuvFormat, const TSize& aPictureSize);
	
private:
	CTestDevVideoCiuPlaySetAndGetPropertiesObserver(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySetAndGetPropertiesObserver() {};
	};

//

class CTestDevVideoCiuPlayUpdateProperties : public CTestDevVideoCiuPlayStep,
										     public MMmfVideoPropertiesObserver
	{
public:
	static CTestDevVideoCiuPlayUpdateProperties* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
	
	//from MMmfVideoPropertiesObserver
	void MmvpoUpdateVideoProperties(const TYuvFormat& aYuvFormat, const TSize& aPictureSize);
	
private:
	CTestDevVideoCiuPlayUpdateProperties(const TDesC& aTestName);
	~CTestDevVideoCiuPlayUpdateProperties() {};
private:
	TBool iCallbackReceived;
	};
	
//
//	Video Media Protected Tests
//

class CTestDevVideoCiuPlayCreateVideoMediaProtected : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayCreateVideoMediaProtected* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayCreateVideoMediaProtected(const TDesC& aTestName);
	~CTestDevVideoCiuPlayCreateVideoMediaProtected() {};
	};
	
//

class CTestDevVideoCiuPlaySetVideoMediaProtected : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlaySetVideoMediaProtected* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlaySetVideoMediaProtected(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySetVideoMediaProtected() {};
	};

//
//	Colour Space Header Info Tests
//

class CTestDevVideoCiuPlayCreateColourSpace : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayCreateColourSpace* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayCreateColourSpace(const TDesC& aTestName);
	~CTestDevVideoCiuPlayCreateColourSpace() {};
	};
	
//

class CTestDevVideoCiuPlayGetColourSpace : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayGetColourSpace* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayGetColourSpace(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetColourSpace() {};
	};


//
// Resource Control tests
//

class CTestDevVideoCiuPlayCreatePolicyExtension : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayCreatePolicyExtension* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayCreatePolicyExtension(const TDesC& aTestName);
	~CTestDevVideoCiuPlayCreatePolicyExtension() {};
	};
	
//

class CTestDevVideoCiuPlaySetClientPolicyExtension : public CTestDevVideoCiuPlayStep,
														public MMmfVideoHwDevClientNotifier
	{
public:
	static CTestDevVideoCiuPlaySetClientPolicyExtension* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();

	// from MMmfVideoHwDevClientNotifier
	virtual void MmvhcnVideoAccessRevoked();
	virtual void MmvhcnReleaseVideoResource();
private:
	CTestDevVideoCiuPlaySetClientPolicyExtension(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySetClientPolicyExtension() {};
	};
	
//

class CTestDevVideoCiuPlaySetPriorityPolicyExtension : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlaySetPriorityPolicyExtension* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlaySetPriorityPolicyExtension(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySetPriorityPolicyExtension() {};
	};

//

class CTestDevVideoCiuPlayClientNotifierRevoked : public CTestDevVideoCiuPlayStep,
													public MMmfVideoHwDevClientNotifier
	{
public:
	static CTestDevVideoCiuPlayClientNotifierRevoked* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();

	// from MMmfVideoHwDevClientNotifier
	virtual void MmvhcnVideoAccessRevoked();
	virtual void MmvhcnReleaseVideoResource();

private:
	CTestDevVideoCiuPlayClientNotifierRevoked(const TDesC& aTestName);
	~CTestDevVideoCiuPlayClientNotifierRevoked() {};
	
private:
	TBool iCallback;
	};

//

class CTestDevVideoCiuPlayClientNotifierResource : public CTestDevVideoCiuPlayStep,
													public MMmfVideoHwDevClientNotifier
	{
public:
	static CTestDevVideoCiuPlayClientNotifierResource* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();

	// from MMmfVideoHwDevClientNotifier
	virtual void MmvhcnVideoAccessRevoked();
	virtual void MmvhcnReleaseVideoResource();
	
private:
	CTestDevVideoCiuPlayClientNotifierResource(const TDesC& aTestName);
	~CTestDevVideoCiuPlayClientNotifierResource() {};
	
private:
	TBool iCallback;
	};
	
//
//							NEGATIVE TESTS
//
class CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder(const TDesC& aTestName);
	~CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder() {};
	};

//
//	Video Surface Support Management Tests
//

class CTestDevVideoCiuPlayGetAndSetUseSurface : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayGetAndSetUseSurface* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayGetAndSetUseSurface(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetAndSetUseSurface() {};
	};

//

class CTestDevVideoCiuPlayGetAndSetSurfaceParameters : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayGetAndSetSurfaceParameters* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayGetAndSetSurfaceParameters(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetAndSetSurfaceParameters() {};
	};

//

class CTestDevVideoCiuPlayGetAndSetSurfaceRemoved : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayGetAndSetSurfaceRemoved* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayGetAndSetSurfaceRemoved(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetAndSetSurfaceRemoved() {};
	};

//

#ifdef SYMBIAN_BUILD_GCE
	
class CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver : public CTestDevVideoCiuPlayStep,
													  public MMMFVideoSurfaceObserver
	{
public:
	static CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
	
	//from MMMFVideoSurfaceObserver
	void MmvsoSurfaceCreated();
	void MmvsoSurfaceParametersChanged();
	void MmvsoRemoveSurface();
	
private:
	CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver() {};
	};

//

class CTestDevVideoCiuPlaySurfaceCreated : public CTestDevVideoCiuPlayStep,
										     public MMMFVideoSurfaceObserver
	{
public:
	static CTestDevVideoCiuPlaySurfaceCreated* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
	
	//from MMMFVideoSurfaceObserver
	void MmvsoSurfaceCreated();
	void MmvsoSurfaceParametersChanged();
	void MmvsoRemoveSurface();
	
private:
	CTestDevVideoCiuPlaySurfaceCreated(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySurfaceCreated() {};
private:
	TBool iCallbackReceived;
	};


//

class CTestDevVideoCiuPlaySurfaceParametersChanged : public CTestDevVideoCiuPlayStep,
										     public MMMFVideoSurfaceObserver
	{
public:
	static CTestDevVideoCiuPlaySurfaceParametersChanged* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
	
	//from MMMFVideoSurfaceObserver
	void MmvsoSurfaceCreated();
	void MmvsoSurfaceParametersChanged();
	void MmvsoRemoveSurface();
	
private:
	CTestDevVideoCiuPlaySurfaceParametersChanged(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySurfaceParametersChanged() {};
private:
	TBool iCallbackReceived;
	};

//

class CTestDevVideoCiuRemoveSurface : public CTestDevVideoCiuPlayStep,
									  public MMMFVideoSurfaceObserver
	{
public:
	static CTestDevVideoCiuRemoveSurface* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
	
	//from MMMFVideoSurfaceObserver
	void MmvsoSurfaceCreated();
	void MmvsoSurfaceParametersChanged();
	void MmvsoRemoveSurface();
	
private:
	CTestDevVideoCiuRemoveSurface(const TDesC& aTestName);
	~CTestDevVideoCiuRemoveSurface() {};
private:
	TBool iCallbackReceived;
	};
	
//
//	Video Client Buffer Support Tests
//

class CTestDevVideoCiuPlayUseClientBuffers : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayUseClientBuffers* NewL(const TDesC& aTestName);
	TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayUseClientBuffers(const TDesC& aTestName);
	~CTestDevVideoCiuPlayUseClientBuffers() {};
	};

//

class CTestDevVideoCiuPlaySupplyBuffer : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlaySupplyBuffer* NewL(const TDesC& aTestName);
	TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlaySupplyBuffer(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySupplyBuffer() {};
	};

//

class CTestDevVideoCiuPlayReleaseBuffers : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayReleaseBuffers* NewL(const TDesC& aTestName);
	TVerdict DoTestL();
	TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayReleaseBuffers(const TDesC& aTestName);
	~CTestDevVideoCiuPlayReleaseBuffers() {};
	};

//
//	Video SurfaceId Custom Interface Tests
//

class CTestDevVideoCiuPlaySurfaceHandleControl : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlaySurfaceHandleControl* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();

private:
	CTestDevVideoCiuPlaySurfaceHandleControl(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySurfaceHandleControl() {};
	};

class CTestDevVideoCiuPlaySurfaceRedrawBuffer : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlaySurfaceRedrawBuffer* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();

private:
	CTestDevVideoCiuPlaySurfaceRedrawBuffer(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySurfaceRedrawBuffer() {};
	};

#endif // SYMBIAN_BUILD_GCE

//
//	Video Play Rate Tests
//

class CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities() {};
	};

//

class CTestDevVideoCiuPlaySetAndGetPlayRate : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlaySetAndGetPlayRate* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlaySetAndGetPlayRate(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySetAndGetPlayRate() {};
	};

//

class CTestDevVideoCiuPlayGetAndSetStepFrame : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuPlayGetAndSetStepFrame* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
private:
	CTestDevVideoCiuPlayGetAndSetStepFrame(const TDesC& aTestName);
	~CTestDevVideoCiuPlayGetAndSetStepFrame() {};
	};

//

class CTestDevVideoCiuPlaySetAndGetPlayRateObserver : public CTestDevVideoCiuPlayStep,
													  public MMmfVideoPlayRateObserver
	{
public:
	static CTestDevVideoCiuPlaySetAndGetPlayRateObserver* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
	
	//from MMmfVideoPlayRateObserver
	void MmvproStepFrameComplete(TTimeIntervalMicroSeconds aTimestamp);
	void MmvproKeyFrameModeRequest();
	
private:
	CTestDevVideoCiuPlaySetAndGetPlayRateObserver(const TDesC& aTestName);
	~CTestDevVideoCiuPlaySetAndGetPlayRateObserver() {};
	};

//

class CTestDevVideoCiuPlayStepFrameComplete : public CTestDevVideoCiuPlayStep, 
											public MMmfVideoPlayRateObserver
	{
public:
	static CTestDevVideoCiuPlayStepFrameComplete* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
		
	//from MMmfVideoPlayRateObserver
	void MmvproStepFrameComplete(TTimeIntervalMicroSeconds aTimestamp);
	void MmvproKeyFrameModeRequest();
		
private:
	CTestDevVideoCiuPlayStepFrameComplete(const TDesC& aTestName);
	~CTestDevVideoCiuPlayStepFrameComplete() {};
private:
	TBool iCallbackReceived;
	};

//

class CTestDevVideoCiuPlayKeyFrameModeRequest : public CTestDevVideoCiuPlayStep,
												public MMmfVideoPlayRateObserver
	{
public:
	static CTestDevVideoCiuPlayKeyFrameModeRequest* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();
			
	//from MMmfVideoPlayRateObserver
	void MmvproStepFrameComplete(TTimeIntervalMicroSeconds aTimestamp);
	void MmvproKeyFrameModeRequest();
		
private:
	CTestDevVideoCiuPlayKeyFrameModeRequest(const TDesC& aTestName);
	~CTestDevVideoCiuPlayKeyFrameModeRequest() {};
private:
	TBool iCallbackReceived;
	};

// Test MMmfVideoHwDeviceTunnel custom interface
class CTestDevVideoCiuHwDeviceTunnel : public CTestDevVideoCiuPlayStep
	{
public:
	static CTestDevVideoCiuHwDeviceTunnel* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL();
	virtual TVerdict DoTestStepPreambleL();

private:
	CTestDevVideoCiuHwDeviceTunnel(const TDesC& aTestName);
	~CTestDevVideoCiuHwDeviceTunnel() {};
	};

#endif //TESTDEVVIDEOCIUPLAY_H
