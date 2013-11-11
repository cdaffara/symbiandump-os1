/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#if (!defined __T_SCREEN_DRIVER_DATA_H__)
#define __T_SCREEN_DRIVER_DATA_H__


//	User Includes
#include "T_HALData.h"

/**
 * Test Active Notification class
 *
 */
class CT_ScreenDriverData : public CT_HALData
	{
public:
	/**
	* Public destructor
	*/
	~CT_ScreenDriverData();

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_ScreenDriverData*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_ScreenDriverData();

	void	ConstructL();

	static TBool	GetPrepareColorIndex(CDataWrapperBase* aThis, const TDesC& aSection, TInt& aValueStart, TInt& aValueEnd);
	static TBool	GetPrepareDisplayMode(CDataWrapperBase* aThis, const TDesC& aSection, TInt& aValueStart, TInt& aValueEnd);

	static void		SetDisplayState(CDataWrapperBase* aThis, TInt aValue);
	static void		SetBacklightState(CDataWrapperBase* aThis, TInt aValue);
	static void		SetBacklight(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayMaxBrightness(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayBrightness(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayMaxContrast(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayContrast(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayMemoryAddress(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayIsPixelOrderRGB(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayIsPixelOrderLandscape(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayMode(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayNumModes(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayColors(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayPaletteEntry(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayXPixels(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayYPixels(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayXTwips(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayYTwips(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayBitsPerPixel(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayOffsetToFirstPixel(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayOffsetBetweenLines(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayIsPalettized(CDataWrapperBase* aThis, TInt aValue);
	static void		SetDisplayNumberOfScreens(CDataWrapperBase* aThis, TInt aValue);

private:
	void	DoCmdUtilityFillColorL(const TDesC& aSection);
	void	DoCmdUtilityFillMultiColorL(const TDesC& aSection);
	void	DoCmdUtilityMovingRectangleL(const TDesC& aSection);
	void	DoCmdUtilityFillHorizontalStripL(const TDesC& aSection);
	void	DoCmdUtilityFillVerticalStripL(const TDesC& aSection);

	//	Screen Driver Utils
	TBool	GetRgbFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint& aRed, TUint& aGreen, TUint& aBlue, TUint& aAlpha);
	TBool	GetRgbFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aRed, TInt& aGreen, TInt& aBlue, TInt& aAlpha);
	TBool	GetColorFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint& aColor, TPtrC& aLogColor);
	void	DrawLine(TUint aColor, TUint8* aMemoryAddress, TInt aLeftMargin, TInt aPixels);

protected:
	TInt			iDisplayState;
	TInt			iBacklightState;
	TInt			iBacklight;
	TInt			iDisplayMaxBrightness;
	TInt			iDisplayBrightness;
	TInt			iDisplayMaxContrast;
	TInt			iDisplayContrast;
	TUint8*			iDisplayMemoryAddress;
	TInt			iDisplayIsPixelOrderRGB;
	TInt			iDisplayIsPixelOrderLandscape;
	TInt			iDisplayMode;
	TInt			iDisplayNumModes;
	TInt			iDisplayColors;
	TInt			iDisplayPaletteEntry;
	TInt			iDisplayXPixels;
	TInt			iDisplayYPixels;
	TInt			iDisplayXTwips;
	TInt			iDisplayYTwips;
	TInt			iDisplayBitsPerPixel;
	TInt			iDisplayOffsetToFirstPixel;
	TInt			iDisplayOffsetBetweenLines;
	TBool			iDisplayIsPalettized;
	TInt			iDisplayNumberOfScreens;

	//	Color palette
	RArray<TInt>	iPalette;

private:
	static const THalTableLookup	iTableLookup[];
	};

#endif /* __T_SCREEN_DRIVER_DATA_H__ */
