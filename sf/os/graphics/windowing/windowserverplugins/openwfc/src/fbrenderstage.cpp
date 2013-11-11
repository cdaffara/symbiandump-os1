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

#include "fbrenderstage.h"
#include <graphics/wsscreendevice.h>
#include "panic.h"
#include <graphics/wsdisplaycontrol.h>
#include "displaypolicy.h"
#include <graphics/sgutils.h>
#include "utils.h"

#if defined(__WINS__) && defined(_DEBUG)
#define DEBUGOSB iRenderTarget->UpdateDebugWin();
#else
#define DEBUGOSB
#endif

//
// class CAnimationThrottle
//

static const TInt KAnimationThrottlePeriod (1000); //1ms

CAnimationThrottle::CAnimationThrottle (CFbRenderStage* aRenderStage) :
	CTimer (EPriorityUserInput), 
	iRenderStage (aRenderStage) 
	{
	CActiveScheduler::Add(this);
	}

CAnimationThrottle::~CAnimationThrottle () 
	{
	Cancel();
	}

CAnimationThrottle* CAnimationThrottle::NewL (CFbRenderStage* aRenderStage)
	{
	CAnimationThrottle* self = new(ELeave)CAnimationThrottle(aRenderStage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}

void CAnimationThrottle::ConstructL ()
	{
	CTimer::ConstructL();
	}

void CAnimationThrottle::RunL()
	{
	iRenderStage->DoEnd();
	}

//
// class CRenderStageController
//

TBool CRenderStageController::IsActive()
    {
    return (CActive::IsActive() || iAnimationThrottle->IsActive());
    }

CRenderStageController::CRenderStageController(CFbRenderStage* aRenderStage) :
	CActive(EPriorityUserInput),
	iRenderStage(aRenderStage)
	{
	CActiveScheduler::Add(this);
	}

CRenderStageController::~CRenderStageController()
	{
	delete (iAnimationThrottle);
    Cancel();
	}

CRenderStageController* CRenderStageController::NewL (CFbRenderStage* aRenderStage)
	{
	CRenderStageController* self = new(ELeave)CRenderStageController(aRenderStage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CRenderStageController::ConstructL()
	{
	iAnimationThrottle = CAnimationThrottle::NewL (iRenderStage);
	}

void CRenderStageController::DoCancel()
	{
	}

void CRenderStageController::RunL()
	{
	/*
	 * Delay the call to DoEnd() by a short delay of KAnimationThrottlePeriod
	 * This allows lower priority WServ client code to get a chance to run.
	 * If we didn't do this, WServ could get live-locked performing the
	 * animation loop.  Animations are scheduled with a high priority
	 * to allow them to meet deadlines to achieve "smooth animations"
	 * but they should not continually run.
	 */
	iAnimationThrottle->After (KAnimationThrottlePeriod);
	}

void CRenderStageController::SetActive()
	{
	CActive::SetActive();
	}

//
// class CFbRenderStage
//

CFbRenderStage* CFbRenderStage::NewL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen * aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage)
	{
	CFbRenderStage* stage = new(ELeave) CFbRenderStage;
	CleanupStack::PushL(stage);
	stage->ConstructL(aEnv, aScreen, aScreenRedraw, aNextStage);
	CleanupStack::Pop(stage);
	return stage;
	}
	
void CFbRenderStage::ConstructL(MWsGraphicDrawerEnvironment * aEnv,MWsScreen * aScreen, MWsScreenRedraw* /*aScreenRedraw*/, CWsRenderStage* aNextStage)
	{
	BaseConstructL();
	iNextScreenDevice = aNextStage->ObjectInterface<MWsScreenDevice>();
	STD_ASSERT_ALWAYS(iNextScreenDevice, EPluginPanicScreenDeviceMissing);
	iNextScene = aNextStage->ObjectInterface<MWsScene>();
	STD_ASSERT_ALWAYS(iNextScene, EPluginPanicSceneMissing);
	iDisplayControl = aNextStage->ObjectInterface<MWsDisplayControl>();	//can be NULL
	iDisplayMapping = aNextStage->ObjectInterface<MWsDisplayMapping>();	//can be NULL
	MWsIniFile* iniFile = aEnv->ObjectInterface<MWsIniFile>();
	STD_ASSERT_ALWAYS(iniFile, EPluginPanicIniFileMissing);

	//initialize render target
	_LIT(KFlickerBufferMode,"FLICKERBUFFERMODE");
	TPtrC flickerBufferModeName;
	TDisplayMode displayMode = ENone;
	if (iniFile->FindVar(iNextScreenDevice->ScreenNumber(), KFlickerBufferMode, flickerBufferModeName))
		displayMode = ParseDisplayMode(flickerBufferModeName);
	if (displayMode == ENone)
		{
		// Display render stage MWsScreenDevice::DisplayMode now reports the supported screendriver display mode in 32bpp. 
		// It is necessary in order to maintain BC with 3rd party apps and DSA framework.
		// The reported display mode may be different from the actual UI surface pixel format, so flicker buffer
	    // must not rely on DisplayMode() to determine pixel format for its rendering target.
		//
		MWsUiBuffer* uiBuf = aNextStage->ObjectInterface<MWsUiBuffer>();
		if (uiBuf)
			{
			displayMode = SgUtils::PixelFormatToDisplayMode(uiBuf->PixelFormat());
			}
		}
	STD_ASSERT_DEBUG(displayMode!=ENone, EPluginPanicNoDisplayModeFound);
	
	const TUint32 usage = ESgUsageDirectGdiTarget | ESgUsageWindowGcSource;
	const TSgCpuAccess cpuAccess = ESgCpuAccessReadWrite;
	TSize size;
	if (iDisplayControl)
		{
		iDisplayPolicy = CDisplayPolicy::NewL(aEnv,aScreen,iNextScreenDevice->ScreenNumber());
		
		//calculate min UI buffer size depending on physical resolutions
		TInt ret = iDisplayControl->NumberOfResolutions();
		RArray<MDisplayControlBase::TResolution> resolutions;
		CleanupClosePushL(resolutions);
		if(ret > 0)
			{
			User::LeaveIfError(resolutions.Reserve(ret));
			User::LeaveIfError(iDisplayControl->GetResolutions(resolutions));
			}
		iDisplayPolicy->CalculateMinBufferSize(resolutions, ret);				
		CleanupStack::PopAndDestroy(&resolutions);
		
		if (iDisplayPolicy->PolicyEnabled())
			{
			iDisplayPolicy->AdjustStageBufferSize(TSize(0,0),size);
			}
		else
			{
			size = iNextScreenDevice->SizeInPixels();
			}
		}
	else
		{
		size = iNextScreenDevice->SizeInPixels();
		}

	iRenderTarget = CRenderTarget::NewL(iniFile, usage, cpuAccess, displayMode, size, iNextScreenDevice->ScreenNumber());
	
	//initialize drawable sources
	MWsGraphicsContext* gc = aNextStage->ObjectInterface<MWsGraphicsContext>();
	STD_ASSERT_ALWAYS(gc, EPluginPanicNoContext);
	iDrawableSourceProvider = gc->ObjectInterface<MWsDrawableSourceProvider>();
	STD_ASSERT_ALWAYS(iDrawableSourceProvider, EPluginPanicNoDrawableSourceProvider);

	for(TInt i = 0; i < CRenderTarget::EAspectRatioCount; i++)
		{
		CRenderTarget::TAspectRatio aspectRatio = (CRenderTarget::TAspectRatio)i;
		const TInt err = iDrawableSourceProvider->CreateDrawableSource(iRenderTarget->ImageId(aspectRatio), iDrawableSources[aspectRatio]);
		STD_ASSERT_ALWAYS(err == KErrNone, EPluginPanicDrawableSourceInitializationFailed);
		}
	
	iController = CRenderStageController::NewL(this);
	
	// Pre-allocate update region buffers. 
	const TInt KUpdateRegionPreAllocationBufferLen = 8;
	TRect* rectangleList = (TRect*) (User::Alloc(KUpdateRegionPreAllocationBufferLen * sizeof(TRect)));
	if (rectangleList)
		{
		CleanupStack::PushL(rectangleList);
		iUpdateRegion = new (ELeave) RRegion(KUpdateRegionPreAllocationBufferLen, rectangleList);
		CleanupStack::Pop(rectangleList);
		}
	else
		{
		iUpdateRegion = new (ELeave) RRegion();
		}

	//
	SetNext(aNextStage);
	}
	
CFbRenderStage::CFbRenderStage()
	{
	}
	
CFbRenderStage::~CFbRenderStage()
	{
	iUpstreamRequestStatus = NULL;
	iNextScreenDevice = NULL;
	iNextScene = NULL;
	if (iUpdateRegion)
		{
		iUpdateRegion->Close();
		}
	delete iUpdateRegion;
	delete iController;

	if(iDrawableSourceProvider)
		{
		for(TInt i = 0; i < CRenderTarget::EAspectRatioCount; i++)
			{
			CRenderTarget::TAspectRatio aspectRatio = (CRenderTarget::TAspectRatio)i;
			if(!iDrawableSources[aspectRatio])
				continue;
			iDrawableSourceProvider->CloseDrawableSource(iDrawableSources[aspectRatio]);
			iDrawableSources[aspectRatio] = NULL;
			}
		iDrawableSourceProvider = NULL;
		}

	delete iRenderTarget;
	delete iDisplayPolicy;
	}

TAny* CFbRenderStage::ResolveObjectInterface(TUint aTypeId)
	{
	switch(aTypeId)
		{
	case MWsScene::EWsObjectInterfaceId:
		return static_cast<MWsScene*>(this);
	case MWsDisplayControl::EWsObjectInterfaceId:
		if (iDisplayControl)
			{
			return static_cast<MWsDisplayControl*>(this);
			}
		break;
	case MWsDisplayMapping::EWsObjectInterfaceId:
		if (iDisplayMapping)
			{
			return static_cast<MWsDisplayMapping*>(this);
			}
		break;
		}

	TAny* interface = NULL;
	if(!interface)
		interface = iRenderTarget->ResolveObjectInterface(aTypeId);
	if(!interface)
		interface = CWsRenderStage::ResolveObjectInterface(aTypeId);
	return interface;
	}

void CFbRenderStage::Begin(const TRegion* aRegion)
	{
	STD_ASSERT_DEBUG(aRegion && !aRegion->CheckError(), EPluginPanicRegionError);
	
	// Need to take a copy of aRegion because the actual update may be propagated
	// downstream sometime after CFbRenderStage::End() is invoked, and there
	// is no guarantee that aRegion will not change after CFbRenderStage::End() 
	// is called.
	iUpdateRegion->Copy(*aRegion);	
	}

/**
@param aFinished Completion status of the update. This may be signalled synchronously on this
				call path or asynchronously on a different active object or thread.

@pre There are no pending updates, otherwise the client must not use signalling. 
*/
void CFbRenderStage::End(TRequestStatus* aFinished)
	{
	STD_ASSERT_DEBUG(!iRequestPending || !aFinished, EPluginPanicMultipleUpdateRequests);
	
	iRequestPending=ETrue;
	iUpstreamRequestStatus = aFinished;
	
	// If the client is using signalling and an update is in progress then 
	// mark the operation as pending and return immediatley, otherwise
	// perform the operation immediatley.

  if (iUpstreamRequestStatus)
    *iUpstreamRequestStatus = KRequestPending;	

	if (!aFinished || !iController->IsActive())
		DoEnd();
	}

/**
Invoked by either client or signal completion from the previous update.
@pre The downstream pipline is ready to accept more data
*/
void CFbRenderStage::DoEnd ()
	{
	if (iRequestPending && Next())
		{ 
		RRegionBuf<5> copyHere;		
		copyHere.Copy(*iUpdateRegion);
		
		TRegion* offsetUpdateRegion=&copyHere;
		TRegionFix<1> fixedRegion;
		
		if (copyHere.CheckError())
			{	//fall back to bounding box on failure
			if (!iUpdateRegion->CheckError())
				{
				fixedRegion.AddRect(iUpdateRegion->BoundingRect());
				fixedRegion.Offset(iRenderTarget->iOffset);
				offsetUpdateRegion=&fixedRegion;
				}
			}
		else
			{
			copyHere.Offset(iRenderTarget->iOffset);
			}
		Next()->Begin(offsetUpdateRegion);

		if(!iUpdateRegion->IsEmpty() && !iUpdateRegion->CheckError())
			{
			MWsGraphicsContext* gc = Next()->ObjectInterface<MWsGraphicsContext>();
			STD_ASSERT_DEBUG(gc, EPluginPanicNoContext);
			if(gc)
				{
				const TAny* drawableSource = iDrawableSources[iRenderTarget->AspectRatio()];

				gc->SetDrawMode(MWsGraphicsContext::EDrawModeWriteAlpha);
				gc->SetClippingRegion(*offsetUpdateRegion);
				iDrawableSourceProvider->DrawResource(drawableSource, TPoint(0, 0), CWindowGc::EGraphicsRotationNone);
				gc->SetDrawMode(MWsGraphicsContext::EDrawModePEN);
				gc->ResetClippingRegion();
				}
			}

		// Use downstream signalling only if the upstream uses it.
		if (iUpstreamRequestStatus)
			{
			iController->SetActive();
			Next()->End(&iController->iStatus);
			if (*iUpstreamRequestStatus == KRequestPending)
			    User::RequestComplete(iUpstreamRequestStatus, KErrNone);
			}
		else
			{
			Next()->End(NULL);
			}
		iRequestPending = EFalse;
		copyHere.Close();
		}
	DEBUGOSB
	}


MWsElement* CFbRenderStage::CreateSceneElementL()
	{
	return iNextScene->CreateSceneElementL();
	}

void CFbRenderStage::DestroySceneElement(MWsElement* aElement)
	{
    iNextScene->DestroySceneElement(aElement);
	}

TInt CFbRenderStage::InsertSceneElement(MWsElement* aInsertElement, MWsElement* aSubordinateElement)
	{
	return iNextScene->InsertSceneElement(aInsertElement, aSubordinateElement);
	}

TInt CFbRenderStage::RemoveSceneElement(MWsElement* aRemoveElement)
	{
	return iNextScene->RemoveSceneElement(aRemoveElement);
	}

TInt CFbRenderStage::SetSceneRotation(const TSceneRotation aSceneRotation)
	{
	TInt err = iNextScene->SetSceneRotation(aSceneRotation);
	if (err != KErrNone)
	    return err;

	const CRenderTarget::TAspectRatio aspectRatio =
            ((aSceneRotation == MWsScene::ESceneAntiClockwise0 ||
                    aSceneRotation == MWsScene::ESceneAntiClockwise180) ?
	    CRenderTarget::ENormalAspectRatio :
        CRenderTarget::EInvertedAspectRatio);
	iRenderTarget->SetAspectRatio(aspectRatio);
	return err;
	}

MWsScene::TSceneRotation CFbRenderStage::SceneRotation() const
    {  
    return iNextScene->SceneRotation();
	}

void CFbRenderStage::ComposePendingScene(TSurfaceId& aOffScreenTarget, TRequestStatus* aCompleted)
    {
    iNextScene->ComposePendingScene(aOffScreenTarget, aCompleted);
    }

void CFbRenderStage::NotifyOnDisplayChange(TRequestStatus& aStatus)
	{
	iDisplayControl->NotifyOnDisplayChange(aStatus);
	}

void CFbRenderStage::NotifyOnDisplayChangeCancel()
	{
	iDisplayControl->NotifyOnDisplayChangeCancel();
	}

void CFbRenderStage::NotifyOnConfigChange(TRequestStatus& aStatus)
	{
	iDisplayControl->NotifyOnConfigChange(aStatus);
	}

void CFbRenderStage::NotifyOnConfigChangeCancel()
	{
	iDisplayControl->NotifyOnConfigChangeCancel();
	}

TInt CFbRenderStage::NumberOfResolutions() const
	{
	return iDisplayControl->NumberOfResolutions();
	}

TInt CFbRenderStage::GetResolutions(RArray<TResolution>& aResolutions) const	
	{
	return iDisplayControl->GetResolutions(aResolutions);
	}


void CFbRenderStage::SetSizeModeExtent(TRect& aExtent, TBitFlags32 aContext)
	{
	if (aContext.iFlags&KOffGc)
		{
		if (iRenderTarget->SetDrawDeviceOffset(aExtent.iTl)>=KErrNone)
			{	//If this isn't supported then the classic behaviour implemented in the display device can work instead.
			aContext.iFlags&=~KOffGc;
			}
		}
	
	iDisplayMapping->SetSizeModeExtent(aExtent, aContext);
	}

TInt CFbRenderStage::MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, TCoordinateSpace aTargetSpace, TRect& aTarget) const
	{
	if(iDisplayMapping)
		{
		return iDisplayMapping->MapCoordinates(aSourceSpace, aSource, aTargetSpace, aTarget);
		}

	return KErrNotSupported;
	}

void CFbRenderStage::GetConfiguration(TDisplayConfiguration& aConfig) const	
	{
	iDisplayControl->GetConfiguration(aConfig);
	}

TInt CFbRenderStage::SetConfiguration(const TDisplayConfiguration& aConfig)	
	{
	//May need to resize the FB buffer
	TInt result;
	TSize reqUiSize;
	TBool reallocFbBuffer=EFalse;
	TSize newFbSize;
	TInt err;
		
	RWsOffScreenImageTarget newTarget;
	TAny* newDrawableSources[CRenderTarget::EAspectRatioCount];
	if (aConfig.GetResolution(reqUiSize))
		{
		newFbSize=reqUiSize;

		iDisplayPolicy->AdjustStageBufferSize(iRenderTarget->SizeInPixels(),newFbSize);


		//if realloc needed
		if (!(newFbSize == iRenderTarget->SizeInPixels()))
			{
			reallocFbBuffer = ETrue;
			err = iRenderTarget->AllocNewTarget(newTarget,newFbSize);	//size should never be zero
			if (err < KErrNone)
				{
				newTarget.Close();
				return err;
				}
			if(iDrawableSourceProvider)
				{
				for(TInt i = 0; i < CRenderTarget::EAspectRatioCount; i++)
					{
					const TSgDrawableId& targetID = newTarget.iImages[i].Id();
					CRenderTarget::TAspectRatio aspectRatio = (CRenderTarget::TAspectRatio)i;
					err = iDrawableSourceProvider->CreateDrawableSource(targetID, newDrawableSources[aspectRatio]);
					//STD_ASSERT_ALWAYS(err == KErrNone, EPluginPanicDrawableSourceInitializationFailed);
					if (err < KErrNone)
						{
						newTarget.Close();
						for(TInt j = 0; j < i; j++)
							{
							CRenderTarget::TAspectRatio aspectRatio = (CRenderTarget::TAspectRatio)j;
							if(!newDrawableSources[aspectRatio])
								continue;
							iDrawableSourceProvider->CloseDrawableSource(newDrawableSources[j]);
							newDrawableSources[j] = NULL;
							}
						return err;
						}
					}
				}
			}
		}
	
	result=iDisplayControl->SetConfiguration(aConfig);
	
	if (result<KErrNone && result != KErrDisconnected)
		{
		if (reallocFbBuffer)
			{
			newTarget.Close();
			if(iDrawableSourceProvider)
				{
				for(TInt i = 0; i < CRenderTarget::EAspectRatioCount; i++)
					{
					CRenderTarget::TAspectRatio aspectRatio = (CRenderTarget::TAspectRatio)i;
					if(!newDrawableSources[aspectRatio])
						continue;
					iDrawableSourceProvider->CloseDrawableSource(newDrawableSources[i]);
					newDrawableSources[i] = NULL;
					}
				}
			}
		}
	else
		{
		if (reallocFbBuffer)
			{
			iRenderTarget->SwitchTarget(newTarget);	//now newTarget shouldn't be closed
			
			if(iDrawableSourceProvider)
				{
				for(TInt i = 0; i < CRenderTarget::EAspectRatioCount; i++)
					{
					CRenderTarget::TAspectRatio aspectRatio = (CRenderTarget::TAspectRatio)i;
					if(!iDrawableSources[aspectRatio])
						continue;
					iDrawableSourceProvider->CloseDrawableSource(iDrawableSources[aspectRatio]);
					iDrawableSources[aspectRatio] = NULL;
					}

				for(TInt i = 0; i < CRenderTarget::EAspectRatioCount; i++)
					{
					CRenderTarget::TAspectRatio aspectRatio = (CRenderTarget::TAspectRatio)i;
					if(newDrawableSources[aspectRatio])
						{
						iDrawableSources[i] = newDrawableSources[i];
						}
					}
				}
			}
		}
	return result;
	}

TInt CFbRenderStage::PreferredDisplayVersion() const	
	{
	return iDisplayControl->PreferredDisplayVersion();
	}

TInt CFbRenderStage::RegisterSurface(const TSurfaceId& aSurface)
    {
    return iNextScene->RegisterSurface(aSurface);
    }

TInt CFbRenderStage::UnregisterSurface(const TSurfaceId& aSurface)
    {
    return iNextScene->UnregisterSurface(aSurface);
    }
