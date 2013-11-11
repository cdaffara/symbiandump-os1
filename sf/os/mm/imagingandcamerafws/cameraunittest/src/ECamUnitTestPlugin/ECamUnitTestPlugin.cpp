// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <ecamuids.hrh>
#include <ecam.h>
#include <ecam/ecamadvsettingsintfuids.hrh>
#include "ECamUnitTestPlugin.h"
#include "ECamUnitTestPluginUids.hrh"
#include "AdvancedSettings.h"
#include <ecam/ecamcommonuids.hrh>
#include "EnhanceFunctionality.h"
#include "directviewfinder.h"
#include "extended_functionality.h"

const TBool KTestCamerasPresent[] = {ETrue, EFalse, EFalse};
const CCamera::CCameraAdvancedSettings::TCameraType KTestCamerasTypes[] = 	
									{
									CCamera::CCameraAdvancedSettings::ECameraOnBoard, 
									CCamera::CCameraAdvancedSettings::ECameraPluggable, 
									CCamera::CCameraAdvancedSettings::ECameraPluggable
									}; 

//
// Main stuff
//
static void Panic(TInt aReason)
	{
	_LIT(KDllName, "ECam Unit Test Plugin");
	User::Panic(KDllName, aReason);
	}
	
//
// CCamUnitTestPlugin
//
CCamUnitTestPlugin::CCamUnitTestPlugin()
	{
	iPresets.Reset();
	iSupportedISORates.Reset();
	iIsoRate = KDefaultIsoRate;
	for ( TInt i = 0; i< KECamSetAvailableCameras; i++)
		{
		iCameras[i] = KTestCamerasPresent[i]; 
		iCameraTypes[i] = KTestCamerasTypes[i];
		}	
		
	iVFHandleGenerater = 0;
	}
	
CCamUnitTestPlugin::~CCamUnitTestPlugin()
	{
	iSupportedISORates.Close();
	}
	
CCamUnitTestPlugin* CCamUnitTestPlugin::NewL()
	{
	FileDependencyUtil::CheckFileDependencyL();
	return new (ELeave) CCamUnitTestPlugin;
	}
	
// 
// 2nd stage ConstructL calls called by framework
//

void CCamUnitTestPlugin::Construct2L(MCameraObserver& aObserver,TInt aCameraIndex)
	{
	iObserver = &aObserver;
	iCameraIndex = aCameraIndex;
	}
	
void CCamUnitTestPlugin::Construct2DupL(MCameraObserver& aObserver,TInt aCameraHandle)
	{
	iObserver = &aObserver;
	iCameraHandle = aCameraHandle;
	}


void CCamUnitTestPlugin::Construct2L(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority)
	{
	iObserver2 = &aObserver;
	iCameraIndex = aCameraIndex;
	iPriority = aPriority;
			
	iSupportedISORates.AppendL(KMinIsoRate);
	iSupportedISORates.AppendL(KDefaultIsoRate);
	iSupportedISORates.AppendL(KIsoRate1);
	iSupportedISORates.AppendL(KIsoRate2);
	iSupportedISORates.AppendL(KIsoRate3);
	iSupportedISORates.AppendL(KIsoRate4);
	iSupportedISORates.AppendL(KIsoRate5);
	iSupportedISORates.AppendL(KMaxIsoRate);
	}
	
void CCamUnitTestPlugin::Construct2DupL(MCameraObserver2& aObserver,TInt aCameraHandle)
	{
	iObserver2 = &aObserver;
	iCameraHandle = aCameraHandle;
	
		
	iSupportedISORates.AppendL(KMinIsoRate);
	iSupportedISORates.AppendL(KDefaultIsoRate);
	iSupportedISORates.AppendL(KIsoRate1);
	iSupportedISORates.AppendL(KIsoRate2);
	iSupportedISORates.AppendL(KIsoRate3);
	iSupportedISORates.AppendL(KIsoRate4);
	iSupportedISORates.AppendL(KIsoRate5);
	iSupportedISORates.AppendL(KMaxIsoRate);
	}


	
//
// Dummy versions of virtual functions
//

void CCamUnitTestPlugin::CameraInfo(TCameraInfo& /*aInfo*/) const
	{
	}

void CCamUnitTestPlugin::Reserve()
	{
	}


void CCamUnitTestPlugin::Release()
	{
	}

void CCamUnitTestPlugin::PowerOn()
	{
	}

void CCamUnitTestPlugin::PowerOff()
	{
	}

TInt CCamUnitTestPlugin::Handle()
	{
	return 0;
	}

void CCamUnitTestPlugin::SetZoomFactorL(TInt /*aZoomFactor = 0*/)
	{
	}

TInt CCamUnitTestPlugin::ZoomFactor() const
	{
	return 0;
	}

void CCamUnitTestPlugin::SetDigitalZoomFactorL(TInt /*aDigitalZoomFactor = 0*/)
	{
	}

TInt CCamUnitTestPlugin::DigitalZoomFactor() const
	{
	return 0;
	}

void CCamUnitTestPlugin::SetContrastL(TInt /*aContrast*/)
	{
	}

TInt CCamUnitTestPlugin::Contrast() const
	{
	return 0;
	}

void CCamUnitTestPlugin::SetBrightnessL(TInt /*aBrightness*/)
	{
	}

TInt CCamUnitTestPlugin::Brightness() const
	{
	return 0;
	}


void CCamUnitTestPlugin::SetFlashL(TFlash /*aFlash = EFlashNone*/)
	{
	Panic(EPanicUnimplemented);
	}

CCamera::TFlash CCamUnitTestPlugin::Flash() const
	{
	Panic(EPanicUnimplemented);
	return EFlashNone;
	}

void CCamUnitTestPlugin::SetExposureL(TExposure /*aExposure = EExposureAuto*/)
	{
	Panic(EPanicUnimplemented);
	}

CCamera::TExposure CCamUnitTestPlugin::Exposure() const
	{
	Panic(EPanicUnimplemented);
	return EExposureAuto;
	}

void CCamUnitTestPlugin::SetWhiteBalanceL(TWhiteBalance /*aWhiteBalance = EWBAuto*/)
	{
	Panic(EPanicUnimplemented);
	}

CCamera::TWhiteBalance CCamUnitTestPlugin::WhiteBalance() const
	{
	Panic(EPanicUnimplemented);
	return EWBAuto;
	}

void CCamUnitTestPlugin::StartViewFinderDirectL(RWsSession& /*aWs*/,CWsScreenDevice& /*aScreenDevice*/,RWindowBase& /*aWindow*/,TRect& /*aScreenRect*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::StartViewFinderDirectL(RWsSession& /*aWs*/,CWsScreenDevice& /*aScreenDevice*/,RWindowBase& /*aWindow*/,TRect& /*aScreenRect*/,TRect& /*aClipRect*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::StartViewFinderBitmapsL(TSize& /*aSize*/)
	{
	Panic(EPanicUnimplemented);
	}
	
void CCamUnitTestPlugin::StartViewFinderBitmapsL(TSize& /*aSize*/,TRect& /*aClipRect*/)
	{
	Panic(EPanicUnimplemented);
	}


void CCamUnitTestPlugin::StartViewFinderL(TFormat /*aImageFormat*/,TSize& /*aSize*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::StartViewFinderL(TFormat /*aImageFormat*/,TSize& /*aSize*/,TRect& /*aClipRect*/)
	{
	Panic(EPanicUnimplemented);
	}


void CCamUnitTestPlugin::StopViewFinder()
	{
	Panic(EPanicUnimplemented);
	}

TBool CCamUnitTestPlugin::ViewFinderActive() const
	{
	Panic(EPanicUnimplemented);
	return EFalse;
	}

void CCamUnitTestPlugin::SetViewFinderMirrorL(TBool /*aMirror*/)
	{
	Panic(EPanicUnimplemented);
	}

TBool CCamUnitTestPlugin::ViewFinderMirror() const
	{
	Panic(EPanicUnimplemented);
	return EFalse;
	}

void CCamUnitTestPlugin::PrepareImageCaptureL(TFormat /*aImageFormat*/,TInt /*aSizeIndex*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::PrepareImageCaptureL(TFormat /*aImageFormat*/,TInt /*aSizeIndex*/,const TRect& /*aClipRect*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::CaptureImage()
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::CancelCaptureImage()
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::EnumerateCaptureSizes(TSize& /*aSize*/,TInt /*aSizeIndex*/,TFormat /*aFormat*/) const
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::PrepareVideoCaptureL(TFormat /*aFormat*/,TInt /*aSizeIndex*/,TInt /*aRateIndex*/,TInt /*aBuffersToUse*/,TInt /*aFramesPerBuffer*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::PrepareVideoCaptureL(TFormat /*aFormat*/,TInt /*aSizeIndex*/,TInt /*aRateIndex*/,TInt /*aBuffersToUse*/,TInt /*aFramesPerBuffer*/,const TRect& /*aClipRect*/)
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::StartVideoCapture()
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::StopVideoCapture()
	{
	Panic(EPanicUnimplemented);
	}

TBool CCamUnitTestPlugin::VideoCaptureActive() const
	{
	Panic(EPanicUnimplemented);
	return EFalse;
	}

void CCamUnitTestPlugin::EnumerateVideoFrameSizes(TSize& /*aSize*/,TInt /*aSizeIndex*/,TFormat /*aFormat*/) const
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::EnumerateVideoFrameRates(TReal32& /*aRate*/,TInt /*aRateIndex*/,TFormat /*aFormat*/,TInt /*aSizeIndex*/,TExposure /*aExposure = EExposureAuto*/) const
	{
	Panic(EPanicUnimplemented);
	}

void CCamUnitTestPlugin::GetFrameSize(TSize& /*aSize*/) const
	{
	Panic(EPanicUnimplemented);
	}

TReal32 CCamUnitTestPlugin::FrameRate() const
	{
	Panic(EPanicUnimplemented);
	return 0.0;
	}

TInt CCamUnitTestPlugin::BuffersInUse() const
	{
	Panic(EPanicUnimplemented);
	return 0;
	}

TInt CCamUnitTestPlugin::FramesPerBuffer() const
	{
	Panic(EPanicUnimplemented);
	return 0;
	}

void CCamUnitTestPlugin::SetJpegQuality(TInt /*aQuality*/)
	{
	Panic(EPanicUnimplemented);
	}

TInt CCamUnitTestPlugin::JpegQuality() const
	{
	Panic(EPanicUnimplemented);
	return 0;
	}

TAny* CCamUnitTestPlugin::CustomInterface(TUid aInterfaceUid)
	{
	TAny* advSettingObject = NULL;
	// use the fact that the parent is a friend of the class
	
	TRAPD(err, advSettingObject = CCamera::CustomInterface(aInterfaceUid));
	
	if (err == KErrNone)
		{
		return advSettingObject;
		}
	/*else if (err == KErrNoMemory)
		{
		User::Leave(KErrNoMemory);
		}*/
	return NULL;
	}


TAny* CCamera::CustomInterface(TUid aInterfaceUid)
	{
	switch (aInterfaceUid.iUid)
		{
		// presets
		case KECamPresetsUidValue:
			{
			CCamera::CCameraPresets* presets = new (ELeave)CCamera::CCameraPresets(*this); 
			CleanupStack::PushL(presets);
			presets->ConstructL();
			CleanupStack::Pop(presets);
			
			return static_cast<TAny*>(presets); 
			}	

		case KECamMCameraPresetsUidValue:
			{
			return static_cast<MCameraPresets*>(CCamPresets::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		case KECamMCameraPresets2UidValue:
			{
			return static_cast<MCameraPresets2*>(CCamPresets::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		// image processing 
		case KECamImageProcessingUidValue:
			{
			CCamera::CCameraImageProcessing* processing = new (ELeave)CCamera::CCameraImageProcessing(*this); 
			CleanupStack::PushL(processing);
			processing->ConstructL();
			CleanupStack::Pop(processing);
			
			return static_cast<TAny*>(processing); 
			}
						
		case KECamMCameraImageProcessingUidValue:
			{
			return static_cast<MCameraImageProcessing*>(CCamImgProc::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		case KECamMCameraImageProcessing2UidValue:
			{
			return static_cast<MCameraImageProcessing2*>(CCamImgProc::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		case KECamMCameraImageProcessing3UidValue:
			{
			return static_cast<MCameraImageProcessing3*>(CCamImgProc::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		// advanced settings
		case KECamAdvancedSettingUidValue:
			{
			CCamera::CCameraAdvancedSettings* settings = new (ELeave)CCamera::CCameraAdvancedSettings(*this); 
			CleanupStack::PushL(settings);
			settings->ConstructL();
			CleanupStack::Pop(settings);
			
			return static_cast<TAny*>(settings); 
			}
			
		case KECamMCameraAdvancedSettingsUidValue:
			{
			return static_cast<MCameraAdvancedSettings*>(CCamAdvSet::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
		
		case KECamMCameraAdvancedSettings2UidValue:
			{
			return static_cast<MCameraAdvancedSettings2*>(CCamAdvSet::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}	
			
		case KECamMCameraAdvancedSettings3UidValue:
			{
			return static_cast<MCameraAdvancedSettings3*>(CCamAdvSet::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		case KECamMCameraAdvancedSettings4UidValue:
			{
			return static_cast<MCameraAdvancedSettings4*>(CCamAdvSet::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
		
		case KUidCameraManagementUidValue:
			{
			return (CCamManagement::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}

		//		
		// enhance camera functionality //
		//
		case KECamHistogramUidValue:
			{
			return static_cast<TAny*>(CCamera::CCameraHistogram::NewL(*this)); 
			}
			
		case KECamMCameraHistogramUidValue:
			{
			return static_cast<MCameraHistogram*>(CCamHistogram::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		case KECamOverlayUidValue:
			{
			return static_cast<TAny*>(CCamera::CCameraOverlay::NewL(*this)); 
			}	
			
		case KECamMCameraOverlayUidValue:
			{
			return static_cast<MCameraOverlay*>(CCamOverlay::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}	
			
		case KECamMCameraOverlay2UidValue:
			{
			return static_cast<MCameraOverlay2*>(CCamOverlay::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		case KECamSnapshotUidValue:
			{
			return static_cast<TAny*>(CCamera::CCameraSnapshot::NewL(*this)); 
			}
					
		case KECamMCameraSnapshotUidValue:
			{
			return static_cast<MCameraSnapshot*>(CCamSnapshot::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}	
			
		//		
		// direct viewfinder functionality //
		//			
		case KECamDirectViewFinderUidValue:
			{
			return static_cast<TAny*>(CCamera::CCameraDirectViewFinder::NewL(*this));  
			}		
			
		case KECamMCameraDirectViewFinderUidValue:
			{
			return static_cast<MCameraDirectViewFinder*>(CCamDirectViewFinder::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}	
		
		case KECamMCameraPreImageCaptureControlUidValue:
			{
			return static_cast<MCameraPreImageCaptureControl*>(CCamPreImageCaptureControl::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
		
		case KECamMCameraImageCaptureUidValue:
			{
			return static_cast<MCameraImageCapture*>(CCamImageCapture::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}

		case KECamMCameraVideoCaptureControlUidValue:
			{
			return static_cast<MCameraVideoCaptureControl*>(CCamVideoCaptureControl::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		case KECamMCameraClientViewFinderUidValue:
			{
			return static_cast<MCameraClientViewFinder*>(CCamClientViewFinder::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		case KECamMCameraBaseClientViewFinderUidValue:
			{
			return static_cast<MCameraViewFinder*>(CCamClientViewFinder::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		case KECamMCameraV2DirectViewFinderUidValue:
			{
			return static_cast<MCameraV2DirectViewFinder*>(CCamV2DirectViewFinder::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
			
		case KECamMCameraBaseV2DirectViewFinderUidValue:
			{
			return static_cast<MCameraViewFinder*>(CCamV2DirectViewFinder::NewL(static_cast<CCamUnitTestPlugin&>(*this)));
			}
		default:
			{		
			return NULL;
			}
		}
	}
	
void CCamUnitTestPlugin::GenerateVFHandle(TInt& aVFHandle)
	{
	iVFHandleGenerater++;
	aVFHandle = iVFHandleGenerater;
	}
	
void CCamUnitTestPlugin::Notify(TUid aMessageId, TInt aError)
	{
	if (iObserver2 != NULL)
		{
		iObserver2->HandleEvent(TECAMEvent(aMessageId, aError));
		}
	}
	
void CCamUnitTestPlugin::Notify2(TUid aMessageId, TInt aError, TInt aParam)
	{
	if (iObserver2 != NULL)
		{
		iObserver2->HandleEvent(TECAMEvent2(aMessageId, aError, aParam));	
		}
	}
	
//
// CCamUnitTestPluginInfo
//

CCamUnitTestPluginInfo::CCamUnitTestPluginInfo()
	{
	}
	
CCamUnitTestPluginInfo::~CCamUnitTestPluginInfo()
	{
	}
	
CCamUnitTestPluginInfo* CCamUnitTestPluginInfo::NewL()
	{
	FileDependencyUtil::CheckFileDependencyL();
	return new (ELeave) CCamUnitTestPluginInfo;
	}
	
TInt CCamUnitTestPluginInfo::CamerasAvailable()
	{
	return KECamSetAvailableCameras;
	}

//
// CCamUnitTestPluginSecureId
//	

CCamUnitTestPluginSecureId::CCamUnitTestPluginSecureId()
	{
	}
	
CCamUnitTestPluginSecureId::~CCamUnitTestPluginSecureId()
	{
	}
	
CCamUnitTestPluginSecureId* CCamUnitTestPluginSecureId::NewL()
	{
	CCamUnitTestPluginSecureId* pluginSecureId = new (ELeave) CCamUnitTestPluginSecureId;
	return pluginSecureId;
	}
	
void CCamUnitTestPluginSecureId::Release()
	{
	delete this;
	}
	
void CCamUnitTestPluginSecureId::GetSecureIdL(TInt& aSecureId) const
	{
	TSecureId secureId = User::CreatorSecureId();	
	aSecureId = secureId.iId;
	}

void FileDependencyUtil::CheckFileDependencyL()
	{
	RFs fsSession;
  	RFile file;
    
  	User::LeaveIfError(fsSession.Connect());
  	CleanupClosePushL(fsSession);
    TInt err = file.Open(fsSession, KECamUnitTestPluginName, EFileRead);
  	file.Close();
  	
  	if(err != KErrNone)
  		{
	    User::LeaveIfError(KErrNotSupported);
  		}
  	CleanupStack::PopAndDestroy(); //fsSession 
	
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KUidCameraUnitTestPlugin,	CCamUnitTestPlugin::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KUidCameraUnitTestInfo,	CCamUnitTestPluginInfo::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KUidCameraUnitTestSecureId,	CCamUnitTestPluginSecureId::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

