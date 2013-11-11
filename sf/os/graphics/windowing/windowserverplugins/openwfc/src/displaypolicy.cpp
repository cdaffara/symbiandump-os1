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

#include <graphics/wsgraphicdrawerinterface.h>
#include <e32cmn.h>
#include <u32hal.h>
#include "displaypolicy.h"
#include "panic.h"

CDisplayPolicy::CDisplayPolicy():iLastAppMode(KErrNotReady),iSmallestAppMode(KErrNotReady)
	{
	// No implementation required
	}

CDisplayPolicy::~CDisplayPolicy()
	{
	}

CDisplayPolicy* CDisplayPolicy::NewLC(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen,TInt aScreenNumber)
	{
	CDisplayPolicy* self = new (ELeave)CDisplayPolicy();
	CleanupStack::PushL(self);
	self->ConstructL(aEnv,aScreen,aScreenNumber);
	return self;
	}

CDisplayPolicy* CDisplayPolicy::NewL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen,TInt aScreenNumber)
	{
	CDisplayPolicy* self=CDisplayPolicy::NewLC(aEnv,aScreen,aScreenNumber);
	CleanupStack::Pop(); // self;
	return self;
	}

void CDisplayPolicy::ConstructL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen,TInt aScreenNumber)
	{
	iScreenIniFile=aEnv->ObjectInterface<MWsIniFile>();
	if (iScreenIniFile)
		{
		_LIT(KScale,"DP_SCALING");
		//_LIT(KScaleNone,"none");
		_LIT(KScaleInteger,"integer");
		_LIT(KScaleIsotropic,"isotropic");
		_LIT(KScaleAnisotropic,"anisotropic");
		_LIT(KScreenDisconnected,"SIMULATE_STARTUP_DISCONNECTED");
		
		iUiScaling = ENone;
		TPtrC scalingModeName;
		if (iScreenIniFile->FindVar(aScreenNumber,KScale,scalingModeName))
			{
			if (scalingModeName == KScaleInteger)
				{
				iUiScaling = EInteger;
				}
			else if (scalingModeName == KScaleIsotropic)
				{
				iUiScaling = EIsotropic;
				}
			else if (scalingModeName == KScaleAnisotropic)
				{
				iUiScaling = EAnisotropic;
				}
			}
		
		if (iScreenIniFile->FindVar(aScreenNumber,KScreenDisconnected))
			{
			//Simulate starting up with this screen disconnected
			TDisplayConnectState displayState = EDisconnect;
			TInt err = UserSvr::HalFunction(EHalGroupDisplay | (aScreenNumber<<16), EDisplayHalSetDisplayState, &displayState, NULL);
			STD_ASSERT_ALWAYS(err == KErrNone, EPluginPanicHalSetDisplayState);
			}
		}
	
	iScreen=aScreen;
	}

void CDisplayPolicy::NewAppModesAvailable()
	{
	//This notification allows the list to be first populated after Wserv has finished initialising itself
	//It also allows for the WServ debug method CWsScreenDevice::SetCurrentScreenModeAttributes
	GetAppModeList();	//get the app size mode list again because some settings have changed
	}

void CDisplayPolicy::GetModeInfoL(const MWsScreenConfigList& aList,TInt aIndex,TPoint& aOffset,TSize& aSize,TAppMode& aMode)const
	{
	aOffset=aList.OriginL(aIndex);
	aSize=aList.ScreenModeSizeInPixelsL(aIndex);
	aMode.iPixels=TRect(aOffset,aSize);
	aMode.iTwips=aList.ScreenModeSizeInTwipsL(aIndex);
	aMode.iOrientations=aList.AvailableOrientationsL(aIndex);
	}

void CDisplayPolicy::GetAppModeList()
	{
	if (iScreen)
		{
		iSmallestAppMode = KErrNotFound;
		iSmallestAppSize = TSize(10000,10000);
		MWsScreenConfigList* screenConfigList=iScreen->ObjectInterface<MWsScreenConfigList>();
		iNumNormalAppModes=0;
		TSize largestAppMode;
		if (screenConfigList)
			{
			RArray<TInt> goodModeIndices;
			if (screenConfigList->GetScreenSizeModeList(goodModeIndices)!=KErrNone)
				{
				//If reserve fails then list is empty later - we can't change display res from app mode request, just use current and try to centre app area
				iAppModes.Reserve(goodModeIndices.Count());
				
				//iScreenIniFile: First time in, it should be a good pointer from which to extract the basicFlags
				//Repeat calls only update the values inside app mode list entries, and the ini file should not be read again. 
				//Specify some basic flags by reading ini file for this screen number?
				TInt basicFlags=0;
				
				//Our demo policy uses the app mode list to generate a list of virtual resolutions.
				
				for (TInt modeIndex=0;modeIndex<goodModeIndices.Count();modeIndex++)
					{
					TInt modeNum=goodModeIndices[modeIndex];
					TAppMode mode;
					TSize size;
					TPoint offset;
					mode.iFlags=basicFlags;
					TRAP_IGNORE(mode.iFlags|=screenConfigList->ModeFlagsL(modeNum)); //we know modeNum is valid-wont leave
					//any further flags to read from ini file for this appmode on this screen number?
	
					//Probably should ignore modes flagged with Hal or Dynamic
					// at the moment we want all modes in the list! 
	
					TInt err = KErrNone;
					TRAP(err,GetModeInfoL(*screenConfigList,goodModeIndices[modeIndex],offset,size,mode));
					if (err == KErrNone)	//should never fail - we never ask for info for a bad mode
						{
						if (1<<CFbsBitGc::EGraphicsOrientationRotated90&mode.iOrientations 
								|| 1<<CFbsBitGc::EGraphicsOrientationRotated270&mode.iOrientations)
							{	//flip rect and twips
							mode.iPixels = TRect(mode.iPixels.iTl.iY,mode.iPixels.iTl.iX,mode.iPixels.iBr.iY,mode.iPixels.iBr.iX);
							mode.iTwips = TSize(mode.iTwips.iHeight,mode.iTwips.iWidth);
							}
						mode.iModeIndex=modeNum;
						if (modeIndex==iAppModes.Count())
							{
							iAppModes.Append(mode);	//can't fail if reserve succeeded
							}
						else if (modeIndex<iAppModes.Count())
							{
							iAppModes[modeIndex]=mode;	//update - wserv implementation means order never changes.
							}
						//This is to help policy code decide what the minimum buffer size should be 
						//Switching between modes requiring smaller screen area than this will not invoke memory reallocation, so will not fail.
						//Perhaps an implementation would also consider the current lowest physical resolution as a guide.
						//The HAL mode may be significantly larger than the app modes require
						if (!(mode.iFlags&(MWsScreenConfigList::EDynamic|MWsScreenConfigList::EHalDefault)))
							{
							TSize borderedSize=offset.AsSize()+size;
							if (borderedSize.iWidth>largestAppMode.iWidth)
								largestAppMode.iWidth=borderedSize.iWidth;
							if (borderedSize.iHeight>largestAppMode.iHeight)
								largestAppMode.iHeight=borderedSize.iHeight;
							iNumNormalAppModes++;
							}
						
						//find the smallest appmode. this will be used when display is disconnected
						if (!(mode.iFlags&(MWsScreenConfigList::EDynamic|MWsScreenConfigList::EHalDefault)))
							{
							TSize borderedSize=offset.AsSize()+size;
							if(borderedSize.iWidth*borderedSize.iHeight < iSmallestAppSize.iWidth*iSmallestAppSize.iHeight)
								{
								iSmallestAppSize = borderedSize;
								iSmallestAppMode = modeNum;
								}
							}
						}
						
					}
				}
			goodModeIndices.Close();
			}
		iMinUiBufferSize=largestAppMode;
		}
	iScreenIniFile=NULL;
	}

TInt CDisplayPolicy::MapCompositionToUi(const TRect& aSource, TRect& aTarget, TBool aIsReverseMapping) const
	{
	// only scaling is involved in mapping from composition to UI
	// no offset change
	TFraction widthFraction;
	TFraction heightFraction;
	if (	iCompositionSizePixels.iWidth<=0
		||	iUiSizePixels.iWidth<=0
		||	iCompositionSizePixels.iHeight<=0
		||	iUiSizePixels.iHeight<=0)
		{
		aTarget=aSource;
		return KErrNotReady;
		}
	if(aIsReverseMapping)
		{
		//Ui to composition
		widthFraction.iNumer = iCompositionSizePixels.iWidth;
		widthFraction.iDenom = iUiSizePixels.iWidth;
		heightFraction.iNumer = iCompositionSizePixels.iHeight;
		heightFraction.iDenom = iUiSizePixels.iHeight;
		}
	else
		{
		//composition to ui
		widthFraction.iNumer = iUiSizePixels.iWidth;
		widthFraction.iDenom = iCompositionSizePixels.iWidth;
		heightFraction.iNumer = iUiSizePixels.iHeight;
		heightFraction.iDenom = iCompositionSizePixels.iHeight;
		}
	
	aTarget.iTl.iX = widthFraction * aSource.iTl.iX;
	aTarget.iBr.iX = widthFraction * aSource.iBr.iX;
	aTarget.iTl.iY = heightFraction * aSource.iTl.iY;
	aTarget.iBr.iY = heightFraction * aSource.iBr.iY;
	return KErrNone;
	}

void CDisplayPolicy::MapUiToApplication(const TRect& aSource, TRect& aTarget, TBool aIsReverseMapping) const
	{
	// only offset is involved in mapping from Ui to App
	// no scaling
	TPoint offset = iAppSizePixels.iTl;;
	
	if(aIsReverseMapping)
		{
		//App to ui
		aTarget.iTl = aSource.iTl + offset;
		aTarget.iBr = aSource.iBr + offset;
		}
	else
		{
		//Ui to App
		aTarget.iTl = aSource.iTl - offset;
		aTarget.iBr = aSource.iBr - offset;
		}
	
	}

TInt CDisplayPolicy::MapUiToDSA(const TRect& aSource, TRect& aTarget, TBool aIsReverseMapping) const 
	{
	//only offset is involved in mapping from Ui to DSA
	//no scaling
	TPoint dsaOffset(0, 0);
	
	MWsScreenConfig *screenConfig = iScreen->ObjectInterface<MWsScreenConfig>();
	if(screenConfig)
		{
		dsaOffset = screenConfig->Origin();
		}
	TRect rectInApp;
	if(aIsReverseMapping)
		{
		//DSA to ui
		//DSA to App first
		rectInApp.iTl = aSource.iTl - dsaOffset;
		rectInApp.iBr = aSource.iBr - dsaOffset;
		//then app to UI
		MapUiToApplication(rectInApp, aTarget, ETrue);
		}
	else
		{
		//Ui to DSA
		//Ui to App first
		MapUiToApplication(aSource, rectInApp, EFalse);
		//then app to DSA
		aTarget.iTl = rectInApp.iTl + dsaOffset;
		aTarget.iBr = rectInApp.iBr + dsaOffset;
		}
	return KErrNone;
	}
TInt CDisplayPolicy::MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, TCoordinateSpace aTargetSpace, TRect& aTarget) const
	{
	TInt returnCode=KErrNone;
	switch (aSourceSpace)
		{
		case ECompositionSpace:
			{
			if(aTargetSpace == ECompositionSpace)
				{
				aTarget = aSource;
				}
			else if(aTargetSpace == EFullScreenSpace)
				{
				//composition to Ui
				returnCode=MapCompositionToUi(aSource, aTarget, EFalse);
				}
			else if(aTargetSpace == EApplicationSpace)
				{
				//composition to App
				TRect rectInUi;
				returnCode=MapCompositionToUi(aSource, rectInUi, EFalse);
				MapUiToApplication(rectInUi, aTarget, EFalse);
				}
			else if(aTargetSpace == EDirectScreenAccessSpace)
				{
				//composition to DSA
				TRect rectInUi;
				returnCode=MapCompositionToUi(aSource, rectInUi, EFalse);
				if(returnCode < 0)
					break;
				returnCode=MapUiToDSA(rectInUi, aTarget, EFalse);
				}
			else
				{
				return KErrNotSupported;
				}
			}
			break;
		case EFullScreenSpace:
			{
			if(aTargetSpace == ECompositionSpace)
				{
				//Ui to composition
				returnCode=MapCompositionToUi(aSource, aTarget, ETrue);
				}
			else if(aTargetSpace == EFullScreenSpace)
				{
				aTarget = aSource;
				}
			else if(aTargetSpace == EApplicationSpace)
				{
				//Ui to app
				MapUiToApplication(aSource, aTarget, EFalse);
				}
			else if(aTargetSpace == EDirectScreenAccessSpace)
				{
				//Ui to DSA
				returnCode = MapUiToDSA(aSource, aTarget, EFalse);
				}
			else
				{
				return KErrNotSupported;
				}
			}
			break;
		case EApplicationSpace:
			{
			if(aTargetSpace == ECompositionSpace)
				{
				//App to composition
				TRect rectInUi;
				MapUiToApplication(aSource, rectInUi, ETrue);
				returnCode=MapCompositionToUi(rectInUi, aTarget, ETrue);
				}
			else if(aTargetSpace == EFullScreenSpace)
				{
				//App to Ui
				MapUiToApplication(aSource, aTarget, ETrue);
				}
			else if(aTargetSpace == EApplicationSpace)
				{
				aTarget = aSource;
				}
			else if(aTargetSpace == EDirectScreenAccessSpace)
				{
				//App to DSA
				TRect rectInUi;
				MapUiToApplication(aSource, rectInUi, ETrue);
				returnCode = MapUiToDSA(rectInUi, aTarget, EFalse);
				}
			else
				{
				return KErrNotSupported;
				}
			}
			break;
		case EDirectScreenAccessSpace:
			{
			if(aTargetSpace == ECompositionSpace)
				{
				//DSA to composition
				TRect rectInUi;
				returnCode = MapUiToDSA(aSource, rectInUi, ETrue);
				if(returnCode < KErrNone)
					break;
				returnCode = MapCompositionToUi(rectInUi, aTarget, ETrue);
				}
			else if(aTargetSpace == EFullScreenSpace)
				{
				//DSA to Ui
				returnCode = MapUiToDSA(aSource, aTarget, ETrue);
				}
			else if(aTargetSpace == EApplicationSpace)
				{
				//DSA to app
				TRect rectInUi;
				returnCode = MapUiToDSA(aSource, rectInUi, ETrue);
				MapUiToApplication(rectInUi, aTarget, EFalse);
				}
			else if(aTargetSpace == EDirectScreenAccessSpace)
				{
				aTarget = aSource;
				}
			else
				{
				return KErrNotSupported;
				}
			break;
			}
		default:
			returnCode= KErrNotSupported;
		}
	return returnCode;
	}

CDisplayPolicy::TFraction::TFraction():iNumer(0),iDenom(1)
	{}

TInt CDisplayPolicy::TFraction::operator*(TInt aInt) const
	{
	if (iDenom == 0 || iNumer == 0 || aInt == 0)
		{
		return 0;
		}
	TInt aNumer = iNumer<<1;
	TInt aDenom = iDenom;
	TInt returnValue = (aNumer*aInt)/aDenom;
	returnValue ++;
	return returnValue>>1;
	}

void	CDisplayPolicy::CalculateMinBufferSize(RArray<MDisplayControlBase::TResolution>& aResolutions, TInt aConnectionStatus)
	{
	iConnectionStatus = aConnectionStatus;
	//preq2102: aResolutions is likely to be changed (in future)
	if(iUiScaling == ENone)
		{
		//this function is currently only used with no scaling
		//should not be called when display is disconnected
		//with scaling iMinUiBufferSize is calculated in CDisplayPolicy::GetAppModeList()
		TSize largestPhysicalRes = iMinUiBufferSize;		
		for(TInt i = 0;i < aResolutions.Count(); i++)
			{
			if(aResolutions[i].iPixelSize.iWidth > largestPhysicalRes.iWidth)
				{
				largestPhysicalRes.iWidth = aResolutions[i].iPixelSize.iWidth;
				}
			if(aResolutions[i].iPixelSize.iHeight > largestPhysicalRes.iHeight)
				{
				largestPhysicalRes.iHeight = aResolutions[i].iPixelSize.iHeight;
				}
			}
		
		iMinUiBufferSize = largestPhysicalRes;
		}
	}

void	CDisplayPolicy::AdjustStageBufferSize(const TSize& /*aOldSize*/,TSize& aNewSize)
	{
	if (iMinUiBufferSize.iWidth==0)
		{
		//just in case, should never happen
		iMinUiBufferSize=TSize(1,1);	
		}
		
	if ((aNewSize.iWidth == 0 || aNewSize.iHeight == 0
			|| iConnectionStatus <= 0) && iUiScaling != ENone 
			&& iSmallestAppMode >= 0) // if detached or turned off (iUiScaling != ENone) and smallestAppMode is found
		{
		aNewSize = iSmallestAppSize;
		return;
		}
	 	 			
	if (aNewSize.iWidth < iMinUiBufferSize.iWidth)
		{
		aNewSize.iWidth = iMinUiBufferSize.iWidth;
		}
	if (aNewSize.iHeight < iMinUiBufferSize.iHeight)
		{
		aNewSize.iHeight = iMinUiBufferSize.iHeight;
		}
	}

void CDisplayPolicy::AddVirtualResolutionCount(TInt& aDisplayCount) const 
	{
	if (iUiScaling != ENone && aDisplayCount>0)
		{
		aDisplayCount += iNumNormalAppModes;
		}
	} 

TInt CDisplayPolicy::AddVirtualResolutions(RArray<MDisplayControlBase::TResolution>& aResolutions) const
	{
	if (aResolutions.Count()==0 || iUiScaling == ENone)
		{
		return KErrNone;
		}
	if (aResolutions.Count()==1 && aResolutions[0].iPixelSize==TSize(0,0))
		{
		return KErrNone;
		}
		
	TInt appModeCount = iAppModes.Count();
	if (appModeCount == 0)
		{
		return KErrNone;
		}
	TInt resolutionCount = aResolutions.Count();
	TInt error = aResolutions.Reserve(iNumNormalAppModes + resolutionCount);
	if (error < KErrNone)
		{
		aResolutions.Reset();
		return error;	//could fail to reserve if out of memory 
		}
	for (TInt appMode = 0; appMode < appModeCount; appMode++)
		{
		if (!(iAppModes[appMode].iFlags&(MWsScreenConfigList::EDynamic|MWsScreenConfigList::EHalDefault)))
			{
			MDisplayControlBase::TResolution virtualResolution = AppModeToResolution(aResolutions,appMode);
			aResolutions.Append(virtualResolution);
			}
		}
	return KErrNone;
	}

MDisplayControlBase::TResolution CDisplayPolicy::AppModeToResolution(RArray<MDisplayControlBase::TResolution>& aResolutions,TInt appMode)const
	{
	TAppMode mode = iAppModes[appMode];
	TBool notComplete = ETrue;
	TInt bestIndex;
	TInt bestScale;
	MDisplayControlBase::TResolution tempResolution(TSize(0,0),TSize(0,0));
	TSize appBestSize;
	while (notComplete)
		{
		TBool modeFit = FindVirtualMode(aResolutions,mode,appBestSize,bestIndex,bestScale);
		
		TSize uiSize;
		if (iUiScaling == EInteger || iUiScaling == EIsotropic)
			{
			uiSize = ResolutionSize(aResolutions,appBestSize,bestIndex,bestScale);
			}
		else if (iUiScaling == EAnisotropic)
			{
			TBool swapAxis = EFalse;
			TBool fitsAppMode = EFalse;
			while (!fitsAppMode)
				{
				if (iAppModes[appMode].iFlags&MWsScreenConfigList::ETwipsSpecified)
					{
					//virtualResolution.iTwipsSize = aResolutions[bestIndex].iTwipsSize;
					//calculate based on twips
					uiSize = ResolutionSizeFromTwips(aResolutions,appMode,appBestSize,bestIndex,swapAxis);
					}
				else
					{
					//assume square pixels
					//virtualResolution.iTwipsSize = aResolutions[bestIndex].iTwipsSize;
					uiSize = ResolutionSizeFromAssumedTwips(aResolutions,appBestSize,bestIndex,swapAxis,bestScale);
					}
				
				//if pixelsize found is larger than resolution mode its designed for, try scaling using other axis
				if (uiSize.iWidth > aResolutions[bestIndex].iPixelSize.iWidth ||
						uiSize.iHeight > aResolutions[bestIndex].iPixelSize.iHeight)
					{
					if (!modeFit)	//no other mode it could fit, to avoid infinite loop,say it fits the mode - will be scaled down
						{
						fitsAppMode = ETrue;
						}
					else
						{
						STD_ASSERT_DEBUG(swapAxis == EFalse, EPluginPanicTemp);
						swapAxis = ETrue;
						}
					}
				else
					{
					fitsAppMode = ETrue;
					}
				}
			//if pixelsize found does not fit app mode, must retry with an appmode larger than the one found
			if (uiSize.iWidth < iAppModes[appMode].iPixels.iBr.iX ||
					uiSize.iHeight < iAppModes[appMode].iPixels.iBr.iY)
				{
				mode.iPixels.iBr.iX = aResolutions[bestIndex].iPixelSize.iWidth+1;
				mode.iPixels.iBr.iY = aResolutions[bestIndex].iPixelSize.iHeight+1;
				continue;
				}
			}

		//MDisplayControlBase::TResolution virtualResolution(TSize(0,0),TSize(0,0));
		
		//only supports rotations supported by both sizemode and hardware
		tempResolution.iFlags = iAppModes[appMode].iOrientations&aResolutions[bestIndex].iFlags.iFlags;
		tempResolution.iFlags.Set(MDisplayControlBase::TResolution::EIsVirtual);
		tempResolution.iTwipsSize = aResolutions[bestIndex].iTwipsSize;
		tempResolution.iPixelSize = uiSize;

		notComplete = EFalse;	//found a resolution that fits!
		}
	return tempResolution;
	}

TBool CDisplayPolicy::MatchConfigToResolutions(const RArray<MDisplayControlBase::TResolution>& aResolutions,
		const TDisplayConfiguration& aConfig, TInt aStartIndex, TInt& aResolutionIndex)const
	{
	if (aStartIndex < 0 || aStartIndex >= aResolutions.Count())
		{
		return EFalse;
		}
	aResolutionIndex = -1;
	for (TInt i = aStartIndex; i < aResolutions.Count(); i++)
		{
		if (aConfig.IsDefined(TDisplayConfigurationBase::EResolution))
			{
			TSize resolution;
			aConfig.GetResolution(resolution);
			if (resolution != aResolutions[i].iPixelSize)
				{
				continue;
				}
			}
		if (aConfig.IsDefined(TDisplayConfigurationBase::EResolutionTwips))
			{
			TSize twips;
			aConfig.GetResolutionTwips(twips);
			if (twips != aResolutions[i].iTwipsSize)
				{
				continue;
				}
			}
		
		if (aConfig.IsDefined(TDisplayConfigurationBase::ERotation))
			{
			TDisplayConfiguration1::TRotation rotation;
			aConfig.GetRotation(rotation);
			if (aResolutions[i].iFlags.IsClear(rotation))
				{
				continue;
				}
			}
		aResolutionIndex = i;
		return ETrue;
		}
	return EFalse;
	}

/*
	Checks if specified appmode is compatible with TResolution specified
	Returns ETrue if succeeded (and fills aConfig with TResolution
	Return EFalse if they are not compatible (will not touch the config) 
*/
TBool CDisplayPolicy::SetConfigToResolution(TInt aAppMode, MDisplayControlBase::TResolution aResolution, TDisplayConfiguration& aConfig)const
	{
	//find intersection of appmode and hardware rotations
	TDisplayConfiguration1::TRotation configRotation;
	TInt compatibleRotations;
	if (aConfig.GetRotation(configRotation))
		{
		compatibleRotations = iAppModes[aAppMode].iOrientations&aResolution.iFlags.iFlags&0xF&(1<<configRotation);
		}
	else
		{
		compatibleRotations = iAppModes[aAppMode].iOrientations&aResolution.iFlags.iFlags&0xF;
		}

	if (compatibleRotations > 0)
		{	//set first compatible rotation we find
		if (1<<CFbsBitGc::EGraphicsOrientationNormal & compatibleRotations)
			{
			aConfig.SetRotation(TDisplayConfiguration1::ERotationNormal);
			}
		else if (1<<CFbsBitGc::EGraphicsOrientationRotated90 & compatibleRotations)
			{
			aConfig.SetRotation(TDisplayConfiguration1::ERotation90CW);
			}
		else if (1<<CFbsBitGc::EGraphicsOrientationRotated180 & compatibleRotations)
			{
			aConfig.SetRotation(TDisplayConfiguration1::ERotation180);
			}
		else
			{
			aConfig.SetRotation(TDisplayConfiguration1::ERotation270CW);
			}
		aConfig.SetResolution(aResolution.iPixelSize);
		aConfig.SetResolutionTwips(aResolution.iTwipsSize);
		return ETrue;
		}

	return EFalse;
	}

TInt CDisplayPolicy::GetSizeModeConfiguration(RArray<MDisplayControlBase::TResolution>& aResolutions,
		TInt aScreenSizeMode, TDisplayConfiguration& aConfig, TRect& aSizeModePosition) const
	{
	//find appMode corresponding to screensizemode
	TInt appMode;
	TInt appModeCount = iAppModes.Count();
	for (appMode = 0; appMode < appModeCount; appMode++)
		{
		if (iAppModes[appMode].iModeIndex == aScreenSizeMode)
			{
			break;
			}
		}
	if (appModeCount == appMode)
		{
		return KErrArgument;	//invalid screen size mode
		}
	if (!aConfig.IsDefined(TDisplayConfigurationBase::EResolution)&&
			!aConfig.IsDefined(TDisplayConfigurationBase::EResolutionTwips))
		{
		if (iAppModes[appMode].iFlags&MWsScreenConfigList::EDynamic)
			{
			TSize resSize = iCompositionSizePixels;
			TSize twipsSize = iCompositionSizeTwips;
			if (iLastCompositionRotation&TDisplayConfiguration::ERotation90CW ||
					iLastCompositionRotation&TDisplayConfiguration::ERotation270CW)
				{
				TInt tempVal = resSize.iWidth;
				resSize.iWidth = resSize.iHeight;
				resSize.iHeight = tempVal;
				tempVal = twipsSize.iWidth;
				twipsSize.iWidth = twipsSize.iHeight;
				twipsSize.iHeight = tempVal;
				}
			aConfig.SetResolution(resSize);
			aConfig.SetResolutionTwips(twipsSize);
			}
		else
			{
			MDisplayControlBase::TResolution virtualResolution = AppModeToResolution(aResolutions,appMode);
			aConfig.SetResolution(virtualResolution.iPixelSize);
			aConfig.SetResolutionTwips(virtualResolution.iTwipsSize);
			}
		}

	//check config is valid from set of resolutions (inc virtual)
	TInt error = AddVirtualResolutions(aResolutions);
	if (error < KErrNone)
		{
		return error;
		}
	TInt startIndex=0;
	while (startIndex < aResolutions.Count())
		{
		TInt resolutionIndex;
		TBool boolError = MatchConfigToResolutions(aResolutions,aConfig,startIndex,resolutionIndex);
		if (boolError == EFalse)
			{
			return KErrArgument;
			}
		
		//if is larger than current app mode and same rotation,we have found a match and can break;
		TBool boolSet = SetConfigToResolution(appMode,aResolutions[resolutionIndex],aConfig);
		if (boolSet)
			{	//new configuration is compatible with app mode and has been set
			//center appmode within the UI
			if (iAppModes[appMode].iFlags&MWsScreenConfigList::EDynamic)
				{
				aSizeModePosition = aResolutions[resolutionIndex].iPixelSize;
				}
			else
				{
				CenteredAppInUi(aResolutions[resolutionIndex].iPixelSize,iAppModes[appMode].iPixels,aSizeModePosition);
				}
			
			TDisplayConfiguration::TRotation tempRot;
			aConfig.GetRotation(tempRot);
			if (tempRot&1)
				{
				aSizeModePosition = TRect(aSizeModePosition.iTl.iY,aSizeModePosition.iTl.iX,
						aSizeModePosition.iBr.iY,aSizeModePosition.iBr.iX);
				}

			return KErrNone;
			}
		//otherwise
		startIndex = resolutionIndex+1;	//match found will not fit current size mode, continue looking
		if (startIndex == aResolutions.Count())
			{
			return KErrArgument;
			}
		}
	STD_ASSERT_DEBUG(EFalse, EPluginPanicTemp);
	return KErrGeneral;	//shouldnt be able to get here
	}

void CDisplayPolicy::CenteredAppInUi(const TSize& aUiSize,const TRect& aAppExtent,TRect& aSizeModePosition) const
	{
	if (aUiSize.iWidth > aAppExtent.Width())
		{
		aSizeModePosition.iTl.iX = (aUiSize.iWidth - aAppExtent.Width()) / 2;
		if (aSizeModePosition.iTl.iX < aAppExtent.iTl.iX)
			{	//we want to obey screenmode offset as a minumum, so cannot center on this axis
			aSizeModePosition.iTl.iX = aAppExtent.iTl.iX;
			aSizeModePosition.iBr.iX = aAppExtent.iBr.iX;
			}
		else
			{
			aSizeModePosition.iBr.iX = aSizeModePosition.iTl.iX + aAppExtent.Width();
			}
		}
	else
		{
		aSizeModePosition.iTl.iX = aAppExtent.iTl.iX;
		aSizeModePosition.iBr.iX = aAppExtent.iBr.iX;
		}
	
	if (aUiSize.iHeight > aAppExtent.Height())
		{
		aSizeModePosition.iTl.iY = (aUiSize.iHeight - aAppExtent.Height()) / 2;
		if (aSizeModePosition.iTl.iY < aAppExtent.iTl.iY)
			{	//we want to obey screenmode offset as a minumum, so cannot center on this axis
			aSizeModePosition.iTl.iY = aAppExtent.iTl.iY;
			aSizeModePosition.iBr.iY = aAppExtent.iBr.iY;
			}
		else
			{
			aSizeModePosition.iBr.iY = aSizeModePosition.iTl.iY + aAppExtent.Width();
			}
		}
	else
		{
		aSizeModePosition.iTl.iY = aAppExtent.iTl.iY;
		aSizeModePosition.iBr.iY = aAppExtent.iBr.iY;
		}
	}

TInt CDisplayPolicy::GetSizeModeConfiguration(TInt aScreenSizeMode,TDisplayConfiguration& aConfig, TRect& aSizeModePosition)
	{
	//This function is used when display is disconnected. Set UI size same as app mode. We don't care about rotation
	
	//find appMode corresponding to screensizemode
	TInt appMode;
	TInt appModeCount = iAppModes.Count();
	for (appMode = 0; appMode < appModeCount; appMode++)
		{
		if (iAppModes[appMode].iModeIndex == aScreenSizeMode)
			{
			break;
			}
		}
	if (appModeCount == appMode)
		{
		return KErrArgument;	//invalid screen size mode
		}
	
	if (1<<CFbsBitGc::EGraphicsOrientationRotated90&iAppModes[appMode].iOrientations 
			|| 1<<CFbsBitGc::EGraphicsOrientationRotated270&iAppModes[appMode].iOrientations)
		{//width and height were already flipped on construction. we need to flip it back
		TRect appRect = iAppModes[appMode].iPixels;
		TRect uiResRect(appRect.iTl.iX, appRect.iTl.iY, appRect.iBr.iY, appRect.iBr.iX);
		aConfig.SetResolution(uiResRect.Size());
		}
	else
		{
		aConfig.SetResolution(iAppModes[appMode].iPixels.iBr.AsSize());
		}
	
	CFbsBitGc::TGraphicsOrientation cFbsOrientation=CFbsBitGc::EGraphicsOrientationNormal;
	//we know aScreenSizeMode is valid-OrientationL wont leave
	TRAP_IGNORE(cFbsOrientation=iScreen->ObjectInterface<MWsScreenConfigList>()->OrientationL(aScreenSizeMode));
	aConfig.SetRotation(TDisplayConfiguration::TRotation(cFbsOrientation));
	aSizeModePosition = iAppModes[appMode].iPixels;
	return KErrNone;
	}

TBool CDisplayPolicy::SettingConfiguration(const RArray<MDisplayControlBase::TResolution>& aResolutions,TDisplayConfiguration& aConfig)const
	{	//converts configuration to appmode
	if (iUiScaling == ENone)
		{
		if (aConfig.IsDefined(aConfig.EResolution))
			{
			TSize res;
			aConfig.GetResolution(res);
			TInt index;
			for (index=0;index<aResolutions.Count();index++)
				{
				if  (aResolutions[index].iPixelSize == res)
					{
					break;
					}
				}
			if (index == aResolutions.Count())
				{	//failed to validate the resolution
				aConfig.Clear(aConfig.EResolution);
				}
			}
		return EFalse;
		}
	
	
	TAppMode appMode;
	
	TBool set;
	TRect zeroRect (0,0,0,0);
	TSize zeroSize (0,0);
	TSize size;
	set = aConfig.GetResolution(size);
	appMode.iPixels = set ? size : zeroRect;
	
	set = aConfig.GetResolutionTwips(size);
	appMode.iTwips = set ? size : zeroSize;
	TDisplayConfiguration::TRotation flags;
	set = aConfig.GetRotation(flags);	//grr orientation again
	appMode.iOrientations = set? flags : 0;	//will set the specific rotation
	
	TInt index=0;
	TInt scale=0;
	TSize bestAppSize;
	FindVirtualMode(aResolutions,appMode,bestAppSize,index,scale);
	
	//set aConfig to resolution returned
	aConfig.SetResolution(aResolutions[index].iPixelSize);
	aConfig.SetResolutionTwips(aResolutions[index].iTwipsSize);
	if (aResolutions[index].iFlags.IsSet(MDisplayControlBase::TResolution::ERotationNormalSupported)
			&& appMode.iOrientations == TDisplayConfiguration::ERotationNormal)
		{
		aConfig.SetRotation(TDisplayConfiguration1::ERotationNormal);
		}
	else if (aResolutions[index].iFlags.IsSet(MDisplayControlBase::TResolution::ERotation90Supported)
			&& appMode.iOrientations == TDisplayConfiguration::ERotation90CW)
		{
		aConfig.SetRotation(TDisplayConfiguration1::ERotation90CW);
		}
	else if (aResolutions[index].iFlags.IsSet(MDisplayControlBase::TResolution::ERotation180Supported)
			&& appMode.iOrientations == TDisplayConfiguration::ERotation180)
		{
		aConfig.SetRotation(TDisplayConfiguration1::ERotation180);
		}
	else if (aResolutions[index].iFlags.IsSet(MDisplayControlBase::TResolution::ERotation270Supported)
			&& appMode.iOrientations == TDisplayConfiguration::ERotation270CW)
		{
		aConfig.SetRotation(TDisplayConfiguration1::ERotation270CW);
		}
	else
		{
		STD_ASSERT_DEBUG(EFalse, EPluginPanicTemp);
		return EFalse;	
		}
	return ETrue;
	}

void CDisplayPolicy::SetCompositionInfo (const TDisplayConfiguration& aCompositionConfig,
		const TDisplayConfiguration& aUiConfig)
	{
	TDisplayConfiguration::TRotation rotation;
	TSize sizePixels;
	TSize sizeTwips;
	if (aCompositionConfig.GetResolution(sizePixels))
		{
		aCompositionConfig.GetResolutionTwips(sizeTwips);
		if(aCompositionConfig.IsDefined(TDisplayConfiguration::ERotation))
			{
			aCompositionConfig.GetRotation(rotation);
			iLastCompositionRotation=rotation;
			if(rotation & TDisplayConfiguration::ERotation90CW)
				{
				//swap width and height to be stored in iCompositionSizePixels and iCompositionSizeTwips
				iCompositionSizePixels.iHeight = sizePixels.iWidth;
				iCompositionSizePixels.iWidth = sizePixels.iHeight;
				iCompositionSizeTwips.iHeight = sizeTwips.iWidth;
				iCompositionSizeTwips.iWidth = sizeTwips.iHeight;
				}
			else
				{
				iCompositionSizePixels = sizePixels;
				iCompositionSizeTwips = sizeTwips;
				}
			}
		else
			{
			iCompositionSizePixels = sizePixels;
			iCompositionSizeTwips = sizeTwips;
			}
		
		aUiConfig.GetResolution(sizePixels);
		aUiConfig.GetResolutionTwips(sizeTwips);
		if(aUiConfig.IsDefined(TDisplayConfiguration::ERotation))
			{
			aUiConfig.GetRotation(rotation);
			iLastUiRotation=rotation;
			if(rotation & TDisplayConfiguration::ERotation90CW)
				{
				//swap width and height to be stored in iUiSizePixels and iAppSizeTwips
				iUiSizePixels.iHeight = sizePixels.iWidth;
				iUiSizePixels.iWidth = sizePixels.iHeight;
				iAppSizeTwips.iHeight = sizeTwips.iWidth;
				iAppSizeTwips.iWidth = sizeTwips.iHeight;
				}
			else
				{
				iUiSizePixels = sizePixels;
				iAppSizeTwips = sizeTwips;
				}
			}
		else
			{
			iUiSizePixels = sizePixels;
			iAppSizeTwips = sizeTwips;
			}	
		}
	else
		{
		if (aUiConfig.GetRotation(rotation))
			{
			if ((rotation^iLastUiRotation)&TDisplayConfiguration::ERotation90CW)
				{
				TInt swapHeight=iUiSizePixels.iHeight;
				iUiSizePixels.iHeight = iUiSizePixels.iWidth;
				iUiSizePixels.iWidth = swapHeight;
				}
			iLastUiRotation=rotation;
			}
		
		if (aCompositionConfig.GetRotation(rotation))
			{
			if ((rotation^iLastCompositionRotation)&TDisplayConfiguration::ERotation90CW)
				{
				TInt swapHeight=iCompositionSizePixels.iHeight;
				iCompositionSizePixels.iHeight = iCompositionSizePixels.iWidth;
				iCompositionSizePixels.iWidth = swapHeight;
				swapHeight=iCompositionSizeTwips.iHeight;
				iCompositionSizeTwips.iHeight = iCompositionSizeTwips.iWidth;
				iCompositionSizeTwips.iWidth = swapHeight;
				}
			iLastCompositionRotation=rotation;
			}
		}
	}

void CDisplayPolicy::SetSizeModeExtent(TRect& aExtent, TBitFlags32 /*aContext*/)
	{
	iAppSizePixels = aExtent;
	}

/**	Finds the best resolution that the specified appmode can fit into
	Returns ETrue if it fits a mode
	Returns EFalse to warn if it didnt fit in that mode
	For both, it will set index and scale to the best fit
*/
TBool CDisplayPolicy::FindVirtualMode(const RArray<MDisplayControlBase::TResolution>& aResolutions,
		const CDisplayPolicy::TAppMode& aAppMode,TSize& aFullAppModeSize,TInt& aIndex,TInt& aBestScale) const
	{
	const TInt KBestFit = 0x7000000; //a really big number that should get reduced during the search.
	TInt bestFit=KBestFit;
	TInt resDiffX;
	TInt resDiffY;
	TInt64 biggestArea(0);
	TInt biggestAreaIndex=0;
	TBool fullAppSize;
	
	if (aAppMode.iPixels.iTl.iX == 0 && aAppMode.iPixels.iTl.iY == 0)
		{	//no point trying again with a border on only 2 sides, as there is no border anyway!
		fullAppSize = EFalse;
		}
	else
		{
		fullAppSize = ETrue;
		}
	aFullAppModeSize = TSize(aAppMode.iPixels.iBr.iX + aAppMode.iPixels.iTl.iX,
			aAppMode.iPixels.iBr.iY + aAppMode.iPixels.iTl.iY);	//try fitting with border all 4 sides

	while(1)
		{
		//find hardware mode that fits this best
		for (TInt j = 0; j < aResolutions.Count(); j++)
			{
			if (bestFit == 0)
				{	//found a perfect match
				break;
				}
			if (aResolutions[j].iFlags[MDisplayControlBase::TResolution::EIsVirtual])
				{	//ignore virtual resolutions
				continue;
				}
			TInt maxScaling = 1;
			if (iUiScaling == EInteger)
				{
				maxScaling = 4;
				}
			for (TInt k = maxScaling; k > 0; k--)
				{	//for every avalable integer scale
				if (bestFit == 0)
					{	//found a perfect match
					break;
					}
				TInt64 area=TInt64(aResolutions[j].iPixelSize.iWidth)*aResolutions[j].iPixelSize.iHeight;
				if (area>biggestArea)
					{
					biggestArea=area;
					biggestAreaIndex=j;
					}
				resDiffX = (aResolutions[j].iPixelSize.iWidth/k) - aFullAppModeSize.iWidth;
				resDiffY = (aResolutions[j].iPixelSize.iHeight/k) - aFullAppModeSize.iHeight;

				if (resDiffX < 0 || resDiffY < 0)
					{	//app mode too large for this resolution
					continue;
					}
				if (resDiffX+resDiffY < bestFit)
					{
					aIndex = j;
					bestFit =  resDiffX + resDiffY;
					aBestScale = k;
					continue;
					}
				}
			}
		if (bestFit != KBestFit)
			{	//it found the best resolution to scale into
			break;
			}
		else if (fullAppSize == EFalse)
			{	//app does not fit any resolution, it will have to be scaled down to fit
			aIndex=biggestAreaIndex;
			aBestScale=1;
			return EFalse;
			}
		else
			{
			aFullAppModeSize = TSize(aAppMode.iPixels.iBr.iX,
					aAppMode.iPixels.iBr.iY);	//try fitting with border only top and left
			fullAppSize = EFalse;
			}
		}
	return ETrue;
	}

TSize CDisplayPolicy::ResolutionSize(RArray<MDisplayControlBase::TResolution>& aResolutions,
		const TSize aBestAppSize,TInt aIndex, TInt aScale) const
	{
	TSize returnSize;
	
	if (iUiScaling == EInteger)	//only supporting integral scales
		{	//just calculate the scaled resolution
		returnSize = TSize(aResolutions[aIndex].iPixelSize.iWidth/aScale,
				aResolutions[aIndex].iPixelSize.iHeight/aScale);
		}
	else
		{	//find which axis scales best, create virtual resolution that fits this axis
		TFraction bestAxisAsFraction;

		if ((TInt64)aResolutions[aIndex].iPixelSize.iWidth * aBestAppSize.iHeight > 
				(TInt64)aResolutions[aIndex].iPixelSize.iHeight * aBestAppSize.iWidth)
			{	//y axis scales closest
			bestAxisAsFraction.iNumer = aResolutions[aIndex].iPixelSize.iWidth;
			bestAxisAsFraction.iDenom = aResolutions[aIndex].iPixelSize.iHeight;
			returnSize = TSize(bestAxisAsFraction*aBestAppSize.iHeight,
					aBestAppSize.iHeight);
			}
		else
			{	//x axis scales closest
			bestAxisAsFraction.iNumer = aResolutions[aIndex].iPixelSize.iHeight;
			bestAxisAsFraction.iDenom = aResolutions[aIndex].iPixelSize.iWidth;
			returnSize = TSize(aBestAppSize.iWidth,
					bestAxisAsFraction*aBestAppSize.iWidth);
			}
		}
	return returnSize;
	}

TSize CDisplayPolicy::GetUiResolution()
	{
	return this->iUiSizePixels;
	}

TSize CDisplayPolicy::GetUiResolutionAsTwips() const
    {
    return this->iCompositionSizeTwips;
    }

TRect CDisplayPolicy::GetPolicyAppMode()
	{
	return iAppSizePixels;
	}

TSize CDisplayPolicy::ResolutionSizeFromTwips(RArray<MDisplayControlBase::TResolution>& aResolutions, TInt aAppMode, const TSize& aBestAppSize,
		TInt aIndex,TBool aSwapAxis) const
	{
	TSize returnSize;
	TFraction tempFraction;
	TBool yScalesClosest;
	
	if ((TInt64)aResolutions[aIndex].iPixelSize.iWidth * aBestAppSize.iHeight > (TInt64)aResolutions[aIndex].iPixelSize.iHeight * aBestAppSize.iWidth)
		{	//y axis scales closest
		yScalesClosest = aSwapAxis?EFalse:ETrue;
		}
	else
		{	//x axis scales closest
		yScalesClosest = aSwapAxis?ETrue:EFalse;
		}
	
	if (yScalesClosest)
		{	//y axis scales closest
		tempFraction.iNumer = aBestAppSize.iHeight;	//bordered app height in pixels
		tempFraction.iDenom = iAppModes[aAppMode].iPixels.iBr.iY - iAppModes[aAppMode].iPixels.iTl.iY;	//app height in pixels
		TInt uiYTwips=tempFraction*iAppModes[aAppMode].iTwips.iHeight;	//bordered app height in twips
		
		tempFraction.iNumer = aResolutions[aIndex].iTwipsSize.iWidth;	//display width in twips
		tempFraction.iDenom = aResolutions[aIndex].iTwipsSize.iHeight;	//display height in twips
		TInt uiXTwips=tempFraction*uiYTwips;	//virtual width in twips
		
		tempFraction.iNumer = iAppModes[aAppMode].iPixels.iBr.iX - iAppModes[aAppMode].iPixels.iTl.iX;	//app width in pixels
		tempFraction.iDenom = iAppModes[aAppMode].iTwips.iWidth;	//display width in twips
		TInt uiXPixels=tempFraction*uiXTwips;	//virtual width in pixels
		
		returnSize.iWidth = uiXPixels;
		returnSize.iHeight = aBestAppSize.iHeight;
		}
	else
		{	//x axis scales closest
		tempFraction.iNumer = aBestAppSize.iWidth;	//bordered app width in pixels
		tempFraction.iDenom = iAppModes[aAppMode].iPixels.iBr.iX - iAppModes[aAppMode].iPixels.iTl.iX;	//app width in pixels
		TInt uiXTwips=tempFraction*iAppModes[aAppMode].iTwips.iWidth;	//bordered app width in twips
		
		tempFraction.iNumer = aResolutions[aIndex].iTwipsSize.iHeight;	//display height in twips
		tempFraction.iDenom = aResolutions[aIndex].iTwipsSize.iWidth;	//display width in twips
		TInt uiYTwips=tempFraction*uiXTwips;	//virtual height in twips
		
		tempFraction.iNumer = iAppModes[aAppMode].iPixels.iBr.iY - iAppModes[aAppMode].iPixels.iTl.iY;	//app height in pixels
		tempFraction.iDenom = iAppModes[aAppMode].iTwips.iHeight;	//display height in twips
		TInt uiYPixels=tempFraction*uiYTwips;	//virtual width in pixels
		
		returnSize.iWidth = aBestAppSize.iWidth;
		returnSize.iHeight = uiYPixels;
		}
	
	return returnSize;
	}

TSize CDisplayPolicy::ResolutionSizeFromAssumedTwips(RArray<MDisplayControlBase::TResolution>& aResolutions,const TSize& aBestAppSize,
		TInt aIndex,TBool aSwapAxis, TInt aScale) const
	{
	TSize returnSize;
	
	if (iUiScaling == EInteger)	//only supporting integral scales
		{	//just calculate the scaled resolution
		returnSize = TSize(aResolutions[aIndex].iTwipsSize.iWidth/aScale,
				aResolutions[aIndex].iTwipsSize.iHeight/aScale);
		}
	else
		{	//find which axis scales best, create virtual resolution that fits this axis
		TBool yScalesClosest;
		TFraction bestAxisAsFraction;
		
		if ((TInt64)aResolutions[aIndex].iPixelSize.iWidth * aBestAppSize.iHeight > 
				(TInt64)aResolutions[aIndex].iPixelSize.iHeight * aBestAppSize.iWidth)
			{	//y axis scales closest
			yScalesClosest = aSwapAxis?EFalse:ETrue;
			}
		else
			{	//x axis scales closest
			yScalesClosest = aSwapAxis?ETrue:EFalse;
			}
		
		if (yScalesClosest)
			{	//y axis scales closest
			bestAxisAsFraction.iNumer = aResolutions[aIndex].iTwipsSize.iWidth;
			bestAxisAsFraction.iDenom = aResolutions[aIndex].iTwipsSize.iHeight;
			returnSize = TSize(bestAxisAsFraction*aBestAppSize.iHeight,
					aBestAppSize.iHeight);
			}
		else
			{	//x axis scales closest
			bestAxisAsFraction.iNumer = aResolutions[aIndex].iTwipsSize.iHeight;
			bestAxisAsFraction.iDenom = aResolutions[aIndex].iTwipsSize.iWidth;
			returnSize = TSize(aBestAppSize.iWidth,
					bestAxisAsFraction*aBestAppSize.iWidth);
			}
		}
	return returnSize;
	}

TInt CDisplayPolicy::MinSizedModeIndex()
	{
	return iSmallestAppMode;
	}

TInt CDisplayPolicy::SuitableAppMode(MWsDisplayPolicy::TDisplayStatus aSituation)
	{
	switch(aSituation)
		{
		case MWsDisplayPolicy::EAttach:
			{
			return iLastAppMode;
			}
		case MWsDisplayPolicy::EDetach:
			{
			return MinSizedModeIndex();
			}
		default:
			return KErrNotSupported;
		}

	}

void CDisplayPolicy::SetLastAppMode(TInt aMode)
	{
	iLastAppMode = aMode;
	}


