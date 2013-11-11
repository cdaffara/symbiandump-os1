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

#ifndef CDISPLAYPOLICY_H
#define CDISPLAYPOLICY_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <graphics/displaycontrolbase.h>
#include <graphics/displaymapping.h>
#include <graphics/displayconfiguration.h>
#include <graphics/wsdisplaypolicy.h>
// CLASS DECLARATION
enum TCoordinateSpace;
class MWsGraphicDrawerEnvironment;
class MWsScreen;
class MWsScreenConfigList;
class MWsIniFile;
/**
 *  CDisplayPolicy
 * 
 */
class CDisplayPolicy : public CBase
	{
public:
	enum	TUiScaling
		{
		ENone=			0,
		EInteger=		1,
		EIsotropic=		2,
		EAnisotropic=	3		
		};
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CDisplayPolicy();

	/**
	 * Two-phased constructor.
	 */
	static CDisplayPolicy* NewL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen,TInt aScreenNumber);

	/**
	 * Two-phased constructor.
	 */
	static CDisplayPolicy* NewLC(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen,TInt aScreenNumber);

	void NewAppModesAvailable();
	void GetAppModeList();
	TInt MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, 
			TCoordinateSpace aTargetSpace, TRect& aTarget) const;
	void AddVirtualResolutionCount(TInt& aDisplayCount) const; 
	TInt AddVirtualResolutions(RArray<MDisplayControlBase::TResolution>& aResolutions) const;
	MDisplayControlBase::TResolution AppModeToResolution(RArray<MDisplayControlBase::TResolution>& aResolutions,TInt appMode)const;
	TInt GetSizeModeConfiguration(RArray<MDisplayControlBase::TResolution>& aResolutions,TInt aScreenSizeMode, 
			TDisplayConfiguration& aConfig, TRect& aSizeModePosition) const;
	
	/*
	 * Used when display is disconnected.
	 */
	TInt GetSizeModeConfiguration(TInt aScreenSizeMode,TDisplayConfiguration& aConfig, TRect& aSizeModePosition);
	TBool SettingConfiguration(const RArray<MDisplayControlBase::TResolution>& aResolutions,
			TDisplayConfiguration& aConfig)const;
	TBool PolicyEnabled()const{return (iUiScaling==ENone)?EFalse:ETrue;}
	
	void SetCompositionInfo (const TDisplayConfiguration& aCompositionConfig,const TDisplayConfiguration& aUiConfig);
	void SetSizeModeExtent(TRect& aExtent, TBitFlags32 /*aContext*/);
	void	AdjustStageBufferSize(const TSize& aOldSize,TSize& aNewSize);
	void	CalculateMinBufferSize(RArray<MDisplayControlBase::TResolution>& aResolutions, TInt aConnectionStatus);
	TInt 	SuitableAppMode(MWsDisplayPolicy::TDisplayStatus aSituation);
	void 	SetLastAppMode(TInt aMode);
	
	TSize	GetUiResolution();
	TSize   GetUiResolutionAsTwips() const;
	TRect 	GetPolicyAppMode();

private:

	class TAppMode
		{
	public:
		TRect	iPixels;	//<Size and pisition in pixels
		TSize	iTwips;		//<Size in TWips
		TInt	iFlags;		//<Flags
		enum	THighFlags	//<Policy flags are per app mode. Lower bits are defined by MWsScreenConfigList::TModeFlags 
			{		//Currently none
			};
		
		//The app modes are presented based on Normal rotation to calculate correctly with display resolutions
		//which are always in normal rotation
		TInt	iOrientations;	//<Index bits via CFbsBitGc::TGraphicsOrientation
		TInt	iModeIndex;	//<Which screen mode index it refers to
		
		};
	/**
	 * Constructor for performing 1st stage construction
	 */
	CDisplayPolicy();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen,TInt aScreenNumber);
	TBool FindVirtualMode(const RArray<MDisplayControlBase::TResolution>& aResolutions,
			const CDisplayPolicy::TAppMode& aAppMode,TSize& aFullAppModeSize,TInt& aIndex,TInt& aBestScale) const;
	
	TSize ResolutionSize(RArray<MDisplayControlBase::TResolution>& aResolutions, const TSize aBestAppSize,
			TInt aIndex, TInt aScale) const;
	TSize ResolutionSizeFromTwips(RArray<MDisplayControlBase::TResolution>& aResolutions, TInt aAppMode,const TSize& aBestAppSize, 
			TInt aIndex,TBool aSwapAxis) const;
	TSize ResolutionSizeFromAssumedTwips(RArray<MDisplayControlBase::TResolution>& aResolutions,const TSize& aBestAppSize,
			TInt aIndex,TBool aSwapAxis,TInt aScale) const;
	
	TBool MatchConfigToResolutions(const RArray<MDisplayControlBase::TResolution>& aResolutions,
			const TDisplayConfiguration& aConfig, TInt aStartIndex, TInt& aResolutionIndex)const;
	TBool SetConfigToResolution(TInt aAppMode, MDisplayControlBase::TResolution aResolution, TDisplayConfiguration& aConfig)const;
	void CenteredAppInUi(const TSize& aUiSize,const TRect& aAppExtent,TRect& aSizeModePosition) const;
	TInt MapCompositionToUi(const TRect& aSource, TRect& aTarget, TBool aIsReverseMapping) const;
	void MapUiToApplication(const TRect& aSource, TRect& aTarget, TBool aIsReverseMapping) const;
	TInt MapUiToDSA(const TRect& aSource, TRect& aTarget, TBool aIsReverseMapping) const;
	void GetModeInfoL(const MWsScreenConfigList& aList,TInt aIndex,TPoint& aOffset,TSize& aSize,TAppMode& aMode) const;
	TInt MinSizedModeIndex();
	/** Used when calculating how to map app mode to hardware resolutions
		Rounds up when .5
	*/
	class TFraction
		{
	public:
		TFraction();
		TInt operator*(TInt aInt) const;
	public:
		TInt iNumer;
		TInt iDenom;
		};
private:
	RArray<TAppMode>	iAppModes;
	TInt iUiScaling;
	MWsIniFile* iScreenIniFile;
	MWsScreen* iScreen;
	TSize iUiSizePixels;
	TRect iAppSizePixels;
	TSize iAppSizeTwips;
	TSize iCompositionSizeTwips;
	TSize iCompositionSizePixels;
	TSize	iMinUiBufferSize;
	TInt 	iNumNormalAppModes;
	TInt	iLastAppMode;
	TInt 	iConnectionStatus;
	TInt 	iSmallestAppMode;
	TSize 	iSmallestAppSize;
	TDisplayConfiguration::TRotation iLastUiRotation;
	TDisplayConfiguration::TRotation iLastCompositionRotation;
	};

#endif // CDISPLAYPOLICY_H
