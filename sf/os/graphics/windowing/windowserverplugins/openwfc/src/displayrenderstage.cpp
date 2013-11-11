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

#include "displayrenderstage.h"
#include <graphics/directgdidriver.h>
#include <hal.h>
#include "openwfcwrapper.h"
#include "panic.h"
#include "rendertarget.h"
#include "utils.h"
#include "rsdisplaychangeao.h"
#include <graphics/wsdisplaycontrol.h>
#include <graphics/wsdisplaymapping.h>
#include <graphics/wsdisplaypolicy.h>
#if defined(__WINS__) && defined(_DEBUG)
#include "debugbardrawer.h"
#endif

#if defined(__WINS__) && defined(_DEBUG)
#define DEBUGOSB iRenderTarget->UpdateDebugWin();
#else
#define DEBUGOSB
#endif

CDisplayRenderStage* CDisplayRenderStage::NewL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen, TInt aScreenNumber)
	{
	CDisplayRenderStage* stage = new(ELeave) CDisplayRenderStage(aScreenNumber);
	CleanupStack::PushL(stage);
	stage->ConstructL(aEnv, aScreen, aScreenNumber);
	CleanupStack::Pop(stage);
	return stage;
	}

CDisplayRenderStage::CDisplayRenderStage(TInt aScreenNumber)
	: iScreenNumber(aScreenNumber)
	{
	}
	
CDisplayRenderStage::~CDisplayRenderStage()
	{
	iSurfaceUpdateSession.Close();
	iUpdateRegion = NULL;
	delete iDispNotifier;
	delete iConfigNotifier;
	delete iDisplayPolicy;
	delete iOpenWfcWrapper;
	delete iRenderTarget;
	if(iDGdiDriver)
		{
		iDGdiDriver->Close();
		}
	SgDriver::Close();
	
#if defined(__WINS__) && defined(_DEBUG)
	delete iDebugBarDrawer;
#endif
	}

void CDisplayRenderStage::ConstructL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, TInt aScreenNumber)
	{
	BaseConstructL();
	
	MWsIniFile* iniFile = aEnv->ObjectInterface<MWsIniFile>();
	STD_ASSERT_ALWAYS(iniFile, EPluginPanicIniFileMissing);

	//Initialise display policy and strategy segments
	iDisplayPolicy = CDisplayPolicy::NewL(aEnv,aScreen, aScreenNumber);
	iDisplayPolicyEnabled = iDisplayPolicy->PolicyEnabled();
	// initialize compositor
	iOpenWfcWrapper = COpenWfcWrapper::NewL(aScreenNumber, iDisplayPolicy);
	iOpenWfcWrapperDisplayControl = iOpenWfcWrapper->GetInterface<MWsDisplayControl>();
	if(iOpenWfcWrapperDisplayControl)
		{
		iDispNotifier = CRsDisplayChangeNotifier::NewL(iOpenWfcWrapperDisplayControl, iDisplayPolicy);	
		iDispNotifier->IssueNotificationRequest();
		iConfigNotifier = CRsConfigChangeNotifier::NewL(iOpenWfcWrapperDisplayControl);
		iConfigNotifier->IssueNotificationRequest();
		
		//calculate min UI buffer size depending on physical resolutions
		TInt ret = iOpenWfcWrapperDisplayControl->NumberOfResolutions();
		RArray<MDisplayControlBase::TResolution> resolutions;
		CleanupClosePushL(resolutions);
		if(ret > 0)
			{
			User::LeaveIfError(resolutions.Reserve(ret));
			User::LeaveIfError(iOpenWfcWrapperDisplayControl->GetResolutions(resolutions));
			}
		iDisplayPolicy->CalculateMinBufferSize(resolutions, ret);				
		CleanupStack::PopAndDestroy(&resolutions);
		}
	
	
	User::LeaveIfError(SgDriver::Open());
	User::LeaveIfError(CDirectGdiDriver::Open());
	iDGdiDriver = CDirectGdiDriver::Static();
	User::LeaveIfNull(iDGdiDriver);
	
	//Initialize the target 
	CreateRenderTargetL(aScreenNumber, iniFile);

	// make sure the compositor wrapper is aware of the UI surface
	const TSurfaceId& uiSurface = iRenderTarget->SurfaceId();
	User::LeaveIfError(iOpenWfcWrapper->SetUiSurface(uiSurface));
	
	User::LeaveIfError(iSurfaceUpdateSession.Connect());
	
	TDisplayConfiguration displayConfig;
	if(iOpenWfcWrapperDisplayControl)
		{
		iOpenWfcWrapper->GetConfiguration(displayConfig);
		}
	else
		{
		displayConfig.SetResolution(SizeInPixels());	// Both getters eventually reach RDisplayChannel
		displayConfig.SetResolutionTwips(SizeInTwips());// as iOpenWfcWrapper->GetConfiguration does
		const MWsScene::TSceneRotation rotation = iOpenWfcWrapper->ScreenRotation();
		displayConfig.SetRotation(static_cast<TDisplayConfiguration::TRotation>(rotation));
		}
	iDisplayPolicy->SetCompositionInfo(displayConfig,displayConfig);	//1 to 1 mapping
	

	
	// initialize the debug bar
#if defined(__WINS__) && defined(_DEBUG)
	_LIT(KDebugBar, "DEBUGBAR");
	if (iniFile->FindVar(KDebugBar))
		{
		iDebugBarDrawer=CDebugBarDrawer::NewL(this, iOpenWfcWrapper->ScreenSize().iWidth);
		}
#endif
	}

void CDisplayRenderStage::CreateRenderTargetL(TInt aScreenNumber, MWsIniFile* aIniFile)
	{
	_LIT(KScreenMode, "SCREENMODE");
	_LIT(KWindowMode, "WINDOWMODE");
	TPtrC screenModeName;
	TDisplayMode screenMode = ENone;
	if(aIniFile->FindVar(aScreenNumber, KScreenMode, screenModeName))
		{
		screenMode = ParseDisplayMode(screenModeName);
		}
	else if(aIniFile->FindVar(aScreenNumber, KWindowMode, screenModeName))
		{
		screenMode = ParseDisplayMode(screenModeName);
		}

	TInt res = KErrNone;
	TSize screenSize = iOpenWfcWrapper->ScreenSize();
	iDisplayPolicy->AdjustStageBufferSize(TSize(0,0), screenSize);
	const TUint32 usage = ESgUsageDirectGdiTarget | ESgUsageCompositionSource | ESgUsageScreenSource;
	const TSgCpuAccess cpuAccess = ESgCpuAccessReadWrite;
	if (screenMode != ENone)
		{
		TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, screenMode, screenSize, aScreenNumber));
		if(res == KErrNone)
			return;
		}
	
	// No screen mode was specified, or we failed creating the specified screen
	// mode. Default to as high display mode as possible.

	// Try creating the render target with all available display modes, going from best to worst
	__ASSERT_COMPILE(EColorLast == 14); // if any display mode is added to TDisplayMode we must update the list below
	// (the list below contains all enums in TDisplayMode except ENone, ERgb, EColorLast)
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EColor16MAP, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EColor16MA, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EColor16MU, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EColor16M, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EColor64K, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EColor4K, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EColor256, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EColor16, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EGray256, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EGray16, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EGray4, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;
	TRAP(res, iRenderTarget = CRenderTarget::NewL(aIniFile, usage, cpuAccess, EGray2, screenSize, aScreenNumber));
	if(res == KErrNone)
		return;

	User::Leave(KErrNotSupported);
	}

TAny* CDisplayRenderStage::ResolveObjectInterface(TUint aTypeId)
	{
	switch(aTypeId)
		{
	case MWsScreenDevice::EWsObjectInterfaceId:
		return static_cast<MWsScreenDevice*>(this);
	case MWsScene::EWsObjectInterfaceId:
		return static_cast<MWsScene*>(this);
#if defined(__WINS__) && defined(_DEBUG)	
	case MWsDebugBar::EWsObjectInterfaceId:		
		return static_cast<MWsDebugBar*>(iDebugBarDrawer);
#endif		
	case MWsDisplayControl::EWsObjectInterfaceId:
#ifndef FAKE_DISPLAY_CONFIGURATION_FOR_SINGLE_DISPLAY
		if (iOpenWfcWrapperDisplayControl)
#endif
			{	//only support the interface if the compositor does
				//otherwise we need to fake single-element support.
			return static_cast<MWsDisplayControl*>(this);
			}
		break;
	case MWsDisplayMapping::EWsObjectInterfaceId:
		return static_cast<MWsDisplayMapping*>(this);
	case MWsDisplayPolicy::EWsObjectInterfaceId:
			{
			if (iDisplayPolicyEnabled)
				{
				return static_cast<MWsDisplayPolicy*>(this);
				}
			else
				{
				return NULL;
				}
			}
		
		}

	TAny* interface = NULL;
	if(!interface)
		interface = iRenderTarget->ResolveObjectInterface(aTypeId);
	if(!interface)
		interface = CWsRenderStage::ResolveObjectInterface(aTypeId);
	return interface;
	}

void CDisplayRenderStage::Begin(const TRegion* aRegion)
	{
	STD_ASSERT_DEBUG(!aRegion || !aRegion->CheckError(), EPluginPanicRegionError);
	iUpdateRegion = aRegion;
	iOpenWfcWrapper->Begin(aRegion);
	}
	
void CDisplayRenderStage::End(TRequestStatus* aFinished)
	{
#if 0
	// For testing purposes, enable this code to signal directly.
	*aFinished = KRequestPending;
	User::RequestComplete(aFinished, KErrNone);
#endif
	
	const TBool compositionDue = iOpenWfcWrapper->CompositionDue();
	
	// Always submit updates if the update region is set.
	// The compositor signal is used when both a composition and surface update are due.
	if (iUpdateRegion && !iUpdateRegion->IsEmpty())
		{
		iDGdiDriver->Finish();
		const TSurfaceId& surfaceId = iRenderTarget->SurfaceId();
		// If composition is due, then we will let the compositor drive the finished signal.
		if (!compositionDue && aFinished)
			iSurfaceUpdateSession.NotifyWhenDisplayed(*aFinished, iTimeStamp);
		iSurfaceUpdateSession.SubmitUpdate(KAllScreens, surfaceId, 0, iUpdateRegion);
		}
	else if (!compositionDue && aFinished) 
		{
		// No re-composition required and the update region is empty.
	    // Signal that we are ready for more updates.
		*aFinished = KRequestPending;
		User::RequestComplete(aFinished, KErrNone);
		}

	if(compositionDue)
		iOpenWfcWrapper->Compose(aFinished);

	iOpenWfcWrapper->End();
	iUpdateRegion = NULL;

	DEBUGOSB
	}

TInt CDisplayRenderStage::ScreenNumber() const
	{
	return iScreenNumber;
	}

TDisplayMode CDisplayRenderStage::DisplayMode() const
	{
	const TInt KThirtyTwoBpp = 32;
	const TDisplayMode dm = iRenderTarget->DisplayMode();
	const TInt bpp = TDisplayModeUtils::NumDisplayModeBitsPerPixel(dm);
	return bpp == KThirtyTwoBpp ? CFbsDevice::DisplayMode16M() : dm;
	}

TSize CDisplayRenderStage::SizeInPixels() const
	{
	TSize size(0, 0);
	if (iOpenWfcWrapperDisplayControl)
		{
		size=iDisplayPolicy->GetUiResolution();
		if (size.iWidth)
			{
			return size;
			}
		}
	return iRenderTarget->SizeInPixels();
	}

TSize CDisplayRenderStage::SizeInTwips() const
    {  
    if (iOpenWfcWrapperDisplayControl)
        {
        TSize size(0, 0);
        size = iDisplayPolicy->GetUiResolutionAsTwips();
        if (size.iWidth)
            {
            return size;
            }     
        }
	const TSize size = SizeInPixels();
	const MWsScene::TSceneRotation screenRotation = iOpenWfcWrapper->ScreenRotation();
	const TBool rotated = (screenRotation == ESceneAntiClockwise90 || screenRotation == ESceneAntiClockwise270);
	
	TInt twips = 0;
	TInt res = HAL::Get(iScreenNumber, rotated ? HALData::EDisplayYTwips : HALData::EDisplayXTwips,twips);
	__ASSERT_DEBUG(res==KErrNone && twips!=0, Panic(EPluginPanicSizeInTwipsFailed));
	const TInt horzTwipsPerThousandPixels = twips * 1000 / size.iWidth; 
	const TInt horzSizeInTwips = (horzTwipsPerThousandPixels * size.iWidth + 500) / 1000;
	
	twips = 0;
	res = HAL::Get(iScreenNumber, rotated ? HALData::EDisplayXTwips : HALData::EDisplayYTwips,twips);
	__ASSERT_DEBUG(res==KErrNone && twips!=0, Panic(EPluginPanicSizeInTwipsFailed));
	const TInt vertTwipsPerThousandPixels = twips * 1000 / size.iHeight; 
	const TInt vertSizeInTwips = (vertTwipsPerThousandPixels * size.iHeight + 500) / 1000;

	return TSize (horzSizeInTwips, vertSizeInTwips);
	}

TRect CDisplayRenderStage::PointerRect() const
	{
	if (iScreenNumber != 0)
	    {
	    return TRect();  // Only screen 0 supports a digitizer
	    }

	TMachineInfoV1Buf macInfo;
	UserHal::MachineInfo(macInfo);
	return TRect(-macInfo().iOffsetToDisplayInPixels, macInfo().iXYInputSizeInPixels);    
	}

void CDisplayRenderStage::GetPixel(TRgb& aColor, const TPoint& aPixel) const
	{
	iRenderTarget->GetPixel(aColor, aPixel);
	}

void CDisplayRenderStage::GetScanLine(TDes8& aScanLine, const TPoint& aStartPixel, TInt aPixelLength, TDisplayMode aDispMode) const
	{
	iRenderTarget->GetScanLine(aScanLine, aStartPixel, aPixelLength, aDispMode);
	}

TBool CDisplayRenderStage::RectCompare(const TRect& aRect1, const TRect& aRect2) const
	{
	return iRenderTarget->RectCompare(aRect1, aRect2);
	}

void CDisplayRenderStage::CopyScreenToBitmapL(CFbsBitmap* aBitmap, const TRect& aRect) const
	{
	iRenderTarget->CopyToBitmapL(aBitmap, aRect);
	}

MWsElement* CDisplayRenderStage::CreateSceneElementL()
	{
	return iOpenWfcWrapper->CreateElementL();
	}

void CDisplayRenderStage::DestroySceneElement(MWsElement* aElement)
    { 
	iOpenWfcWrapper->DestroyElement(aElement);
	}

TInt CDisplayRenderStage::InsertSceneElement(MWsElement* aInsertElement, MWsElement* aSubordinateElement)
	{
	return iOpenWfcWrapper->AddElement(aInsertElement, aSubordinateElement);
	}

TInt CDisplayRenderStage::RemoveSceneElement(MWsElement* aRemoveElement)
	{
	iOpenWfcWrapper->RemoveElement(aRemoveElement);
	return KErrNone;
	}

void CDisplayRenderStage::ComposePendingScene(TSurfaceId& aOffScreenTarget, TRequestStatus* aCompleted)
    {
    /*
     * If the supplied off-screen target is NULL and If the optionally provided TRequestStatus
     * is present, signal immediately and return.
     */
    if (aOffScreenTarget.IsNull())
        {
        if (aCompleted)
            {           
            User::RequestComplete(aCompleted, KErrArgument);
            return;
            }
        }

    iOpenWfcWrapper->RegisterSurface(aOffScreenTarget);
    iOpenWfcWrapper->Compose(aOffScreenTarget, aCompleted);
    iOpenWfcWrapper->UnregisterSurface(aOffScreenTarget);
    }

TInt CDisplayRenderStage::SetSceneRotation(const MWsScene::TSceneRotation aSceneRotation)
    {
	const TSceneRotation oldRotation = iOpenWfcWrapper->ScreenRotation();

	//rotate in compositor 
	iOpenWfcWrapper->SetScreenRotation(aSceneRotation);

	switch (aSceneRotation)
	    {
	    case ESceneAntiClockwise0:
	    case ESceneAntiClockwise180:
	        iRenderTarget->SetAspectRatio(CRenderTarget::ENormalAspectRatio);
	        break;
	    default:
	        iRenderTarget->SetAspectRatio(CRenderTarget::EInvertedAspectRatio);
	    }
		
	TDisplayConfiguration rotate;
	rotate.SetRotation((TDisplayConfiguration::TRotation)aSceneRotation);
	iDisplayPolicy->SetCompositionInfo(rotate,rotate);
	//update ui element
	const TSurfaceId& uiSurface = iRenderTarget->SurfaceId();
	return iOpenWfcWrapper->SetUiSurface(uiSurface);;
	}

MWsScene::TSceneRotation CDisplayRenderStage::SceneRotation() const
    {
    return iOpenWfcWrapper->ScreenRotation();
    }

void CDisplayRenderStage::ClearDisplay(TRgb aBackColor)
	{
	STD_ASSERT_DEBUG(!iOpenWfcWrapper->WithinBeginEnd(), EPluginPanicBadBeginEndSequence);
	if (!iOpenWfcWrapper->WithinBeginEnd())
		{
		CDirectGdiContext& gc = iRenderTarget->DirectGdiGc()->DirectGdiGc();
		const TRect screenRect(SizeInPixels());
		const TRegionFix<1> screenRegion(screenRect);
		Begin(&screenRegion);
		gc.SetBrushStyle(DirectGdi::ESolidBrush);
		gc.SetPenStyle(DirectGdi::ENullPen);
		gc.SetBrushColor(aBackColor);
		gc.SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
		gc.DrawRect(screenRect);
		gc.SetDrawMode(DirectGdi::EDrawModePEN);
		End(NULL);
		}	
	}
//
//
// Implementation of MWsDisplayConfiguration/MDisplayConfiguration interface
//
//


void CDisplayRenderStage::NotifyOnDisplayChange(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	if(iDispNotifier)
		{
		iDispNotifier->RegisterActiveStatus(aStatus);
		}
	else
		{
		TRequestStatus* reqPtr=&aStatus;	//indirection is required
		User::RequestComplete(reqPtr, KErrNotSupported);
		}
	}

void CDisplayRenderStage::NotifyOnDisplayChangeCancel()
	{
	if(iDispNotifier)
		{
		iDispNotifier->CancelNotificationRequest();
		}
	}

void CDisplayRenderStage::NotifyOnConfigChange(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	if(iConfigNotifier)
		{
		iConfigNotifier->RegisterActiveStatus(aStatus);
		}
	else
		{
		TRequestStatus* reqPtr=&aStatus;	//indirection is required
		User::RequestComplete(reqPtr, KErrNotSupported);
		}
	}

void CDisplayRenderStage::NotifyOnConfigChangeCancel()
	{
	if(iConfigNotifier)
		{
		iConfigNotifier->CancelNotificationRequest();
		}
	}

/**	Determine the display configuration and application rendering
	offset to use for a given screen size mode.

	The Window Server uses this function when a request to change to a
	new screen size mode or new display configuration is made by a
	client. The display configuration passed in may be partially filled,
	and the combination of the defined entries in it and the requested
	screen size mode values shall be used to define the rest of the
	configuration and new application offset to be used.

	Only undefined entries in the configuration may be set by this
	function. Entries already defined must retain the same values. If
	the combination of defined entries and size mode values is not
	acceptable to the implementer, the function shall return EFalse.

	If the function returns ETrue, the Window Server shall set the
	returned display configuration, notify render stages of the new
	application offset and change to use the screen size mode. If
	either this function returns EFalse or the call to set the display
	configuration fails for any reason (such as KErrNoMemory, for
	example), then no further changes are made.

	@see MWsScreenConfigList

	@param aScreenSizeMode    The requested screen size mode. If static,
	the size in pixels, size in twips, and the rotation must be
	respected. If dynamic, only the rotation must be respected.
	@param aConfig    The requested display configuration on input, the
	required configuration on return. Only undefined entries may be set.

	@param aSizeModePosition    The screen size mode offset and size to be filled in
	
	@return KErrNone if no error, otherwise a system wide error code
*/
TInt CDisplayRenderStage::GetSizeModeConfiguration(TInt aScreenSizeMode, TDisplayConfiguration& aConfig, TRect& aSizeModePosition) const
	{
	TInt reply;
	RArray<TResolution> resolutions;
	reply = iOpenWfcWrapperDisplayControl->GetResolutions(resolutions);
	if (reply < KErrNone &&
			reply != KErrDisconnected)
		{
		resolutions.Close();
		return reply;
		}
	TBool isDisconnected=EFalse;
	if (reply == KErrDisconnected || resolutions.Count() == 0)
		{
		isDisconnected=ETrue;
		}
	else if (resolutions.Count() == 1 && resolutions[0].iPixelSize==TSize(0,0))
		{
		isDisconnected=ETrue;
		}
	
	if (isDisconnected)
		{//in this case aScreenSizeMode should be the smallest app mode
		reply = iDisplayPolicy->GetSizeModeConfiguration(aScreenSizeMode, aConfig, aSizeModePosition);
		}
	else
		{
		reply = iDisplayPolicy->GetSizeModeConfiguration(resolutions,aScreenSizeMode,aConfig,aSizeModePosition);
		}

	resolutions.Close();
	return reply;
	}

void CDisplayRenderStage::SetSizeModeExtent(TRect& aExtent, TBitFlags32 aContext)
	{
	if (aContext.iFlags&KOffMap)
		{
		iDisplayPolicy->SetSizeModeExtent(aExtent,aContext);
		aContext.iFlags&=~KOffMap;
		}
	if (aContext.iFlags&KOffGc)
		{
		iRenderTarget->SetDrawDeviceOffset(aExtent.iTl);
		aContext.iFlags&=~KOffGc;
		}
	//Could chain through again if required?
	}

TInt CDisplayRenderStage::NumberOfResolutions() const
	{
	if(iOpenWfcWrapperDisplayControl)
		{
		TInt returnValue = iOpenWfcWrapperDisplayControl->NumberOfResolutions();
		iDisplayPolicy->AddVirtualResolutionCount(returnValue);
		return returnValue;
		}
#ifdef FAKE_DISPLAY_CONFIGURATION_FOR_SINGLE_DISPLAY	
	return 1;
#else
	return KErrNotSupported;
#endif
	}

TInt CDisplayRenderStage::GetResolutions(RArray<TResolution>& aResolutions) const	
	{
	if(iOpenWfcWrapperDisplayControl)
		{
		TInt returnValue = iOpenWfcWrapperDisplayControl->GetResolutions(aResolutions);
		if (returnValue >= KErrNone)
			{
			//add virtual resolutions
			returnValue = iDisplayPolicy->AddVirtualResolutions(aResolutions);
			}
		return returnValue;
		}
	else
		{
#ifdef FAKE_DISPLAY_CONFIGURATION_FOR_SINGLE_DISPLAY	
		TSize newsize=iScreenDevice->SizeInPixels();
		if (iScreenDevice->DeviceOrientation()&(EDeviceOrientation90CW|EDeviceOrientation270CW))
			{	//width and height are reported swapped if evice orientation changes!
			TInt w=newsize.iHeight;
			newsize.iHeight=newsize.iWidth;
			newsize.iWidth=w;
			}
		while (aResolutions.Count()>1)
			{	//don't want to reallocate and risk failure
			aResolutions.Remove(aResolutions.Count()-1);
			}
		if (aResolutions.Count()==1)
			{
			aResolutions[0]=newsize;
			return KErrNone;
			}
		else
			{
			return aResolutions.Append(newsize);
			}
#else
		return KErrNotSupported;
#endif
		}
	}


void CDisplayRenderStage::GetConfiguration(TDisplayConfiguration& aConfig) const	
	{
	aConfig.ClearAll();
	if(iOpenWfcWrapperDisplayControl)
		{
		iOpenWfcWrapperDisplayControl->GetConfiguration(aConfig);
		if (aConfig.IsDefined(aConfig.EResolution))
			{
			TSize uiRes=iDisplayPolicy->GetUiResolution();
			TDisplayConfiguration::TRotation rotation;
			if(aConfig.GetRotation(rotation))
				{
				if(rotation&TDisplayConfiguration::ERotation90CW)
					{
					TInt height = uiRes.iHeight;
					uiRes.iHeight = uiRes.iWidth;
					uiRes.iWidth = height;
					}
				}
			if (uiRes.iWidth)
				{
				aConfig.SetResolution(uiRes);
				}
			}
		}
	}

TInt CDisplayRenderStage::SetConfigurationAndBuffer(const TDisplayConfiguration& aConfig,const TSize* aUiSize)
	{
	//May need to resize the FB buffer
	TInt result = KErrNone;
	TSize reqUiSize;
	TBool reallocBuffer=EFalse;
	TSize newFbSize;
		
	RWsOffScreenImageTarget newTarget;
	if (aUiSize)
		{
		newFbSize=*aUiSize;

		iDisplayPolicy->AdjustStageBufferSize(iRenderTarget->SizeInPixels(),newFbSize);

		//if realloc needed	
		if (!(newFbSize == iRenderTarget->SizeInPixels()))
			{ 
			reallocBuffer = ETrue;
			result = iRenderTarget->AllocNewTarget(newTarget,newFbSize);	//newFbSize should not be zero here.
			if (result < KErrNone)
				{
				newTarget.Close();
				return result;
				}
			}
		}
	TDisplayConfiguration displayCopy;
	iOpenWfcWrapper->GetConfiguration(displayCopy);
	if (aConfig.IsDefined(aConfig.EResolution)||aConfig.IsDefined(aConfig.ERotation))
		{
		result=iOpenWfcWrapper->FlushedSetConfiguration(aConfig);
		}
	if (result<KErrNone && result != KErrDisconnected)
		{
		TInt tempResult=iOpenWfcWrapper->FlushedSetConfiguration(displayCopy);
		STD_ASSERT_DEBUG((tempResult==KErrNone)||(tempResult==KErrDisconnected),EPluginPanicTemp);
		if (reallocBuffer)
			{
			newTarget.Close();
			}
		}
	else
		{
		if (reallocBuffer)
			{
			const TSurfaceId& uiSurface = newTarget.iImageCollections
					[iRenderTarget->AspectRatio()].SurfaceId();
			result = iOpenWfcWrapper->SetUiSurface(uiSurface);
			if (result == KErrNone)
				{
				iRenderTarget->SwitchTarget(newTarget);	//now newTarget shouldn't be closed
				}
			else
				{
				TInt tempResult = iOpenWfcWrapper->FlushedSetConfiguration(displayCopy);
				STD_ASSERT_DEBUG((tempResult==KErrNone)||(result==KErrDisconnected),EPluginPanicTemp);
				newTarget.Close();
				}
			}
		}
	return result;
	}

TInt CDisplayRenderStage::SetConfiguration(const TDisplayConfiguration& aConfig)	
	{
	if(iOpenWfcWrapperDisplayControl)
		{

		TInt returnValue = iOpenWfcWrapperDisplayControl->NumberOfResolutions();
		TBool success=EFalse;
		//Both "no resolution" and KErrDisconnected are treated as disconnected
		if(returnValue == KErrDisconnected || returnValue == 0)
			{
			//Ui Res is set to same as smallest appmode
			//compsotion res is set to zero

			TSize uiSize;
			TDisplayConfiguration emptyConvertedConfig;
			emptyConvertedConfig.SetResolution(TSize(0,0));
			aConfig.GetResolution(uiSize);
			returnValue=SetConfigurationAndBuffer(emptyConvertedConfig,&uiSize);
			iDisplayPolicy->SetCompositionInfo(emptyConvertedConfig,aConfig);
			return returnValue;
			}
		
		TDisplayConfiguration convertedConfig(aConfig);
		RArray<TResolution> resolutions;
		returnValue = resolutions.Reserve(returnValue);
		
		if (returnValue < KErrNone)
			{
			resolutions.Close();
			return returnValue;
			}
		returnValue = iOpenWfcWrapperDisplayControl->GetResolutions(resolutions);
		
		if (returnValue < KErrNone)
			{
			resolutions.Close();
			return returnValue;
			}
		success = iDisplayPolicy->SettingConfiguration(resolutions,convertedConfig);	//if virtual, converts to real
		resolutions.Close();

		TSize uiSize;
		TBool isUiSize = aConfig.GetResolution(uiSize);
		if (success)
			{
			TInt reply;
			if (isUiSize)
				{
				reply=SetConfigurationAndBuffer(convertedConfig,&uiSize);
				}
			else
				{
				reply=SetConfigurationAndBuffer(convertedConfig,NULL);
				}
			if (reply>=KErrNone)
				{
				iDisplayPolicy->SetCompositionInfo(convertedConfig,aConfig);
//				iBitGcWrapper->BitGc().Activate(iScreenDevice);
				}
			return reply;
			}
		else
			{
			TSize tempSize;
			
			if (!convertedConfig.GetResolution(tempSize))
				{	//signals the config is totally invalid and should not be used
				return KErrArgument;
				}
			TInt reply;
			if (isUiSize)
				{
				reply=SetConfigurationAndBuffer(convertedConfig,&uiSize);
				}
			else
				{
				reply=SetConfigurationAndBuffer(convertedConfig,NULL);
				}
			if (reply>=KErrNone)
				{
				iDisplayPolicy->SetCompositionInfo(aConfig,aConfig);
//				iBitGcWrapper->BitGc().Activate(iScreenDevice);
				}
			return reply;
			}
		}
	else
		{	//This will never work when faked
		return KErrNotSupported;
		}
	}

TInt CDisplayRenderStage::PreferredDisplayVersion() const	
	{
	if(iOpenWfcWrapperDisplayControl)
		{
		return iOpenWfcWrapperDisplayControl->PreferredDisplayVersion();
		}
	else
		{
#ifdef FAKE_DISPLAY_CONFIGURATION_FOR_SINGLE_DISPLAY	
		return TDisplayConfiguration().Version();
#else
		return KErrNotSupported;
#endif
		}
	}
TInt CDisplayRenderStage::MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, TCoordinateSpace aTargetSpace, TRect& aTarget) const
	{
	return iDisplayPolicy->MapCoordinates(aSourceSpace, aSource, aTargetSpace, aTarget);
	}

void CDisplayRenderStage::NewAppModesAvailable()
	{
	iDisplayPolicy->NewAppModesAvailable();
	}

TInt CDisplayRenderStage::SuitableAppMode(TDisplayStatus aSituation)
	{
	return iDisplayPolicy->SuitableAppMode(aSituation);
	}

void CDisplayRenderStage::SetLastAppMode(TInt aMode)
	{
	iDisplayPolicy->SetLastAppMode(aMode);
	}

TInt CDisplayRenderStage::RegisterSurface(const TSurfaceId& aSurface)
    {
    return iOpenWfcWrapper->RegisterSurface(aSurface);
    }

TInt CDisplayRenderStage::UnregisterSurface(const TSurfaceId& aSurface)
    {
    return iOpenWfcWrapper->UnregisterSurface(aSurface);
    }
