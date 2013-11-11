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

#include "t_cfbsdrawdevicedata.h"
#include <hal.h>
#include <e32math.h>

_LIT(KDisplayMode, 					"displayMode");
_LIT(KDrawMode, 					"drawMode");
_LIT(KOrientationMode,				"orientationMode");
_LIT(KShadowMode, 					"shadowMode");
_LIT(KBackColor, 					"backColor");
_LIT(KNumRect,	 					"numRect");
_LIT(KRectangles, 					"rectangles");
_LIT(KRectangleSection, 			"RECTANGLES");
_LIT(KNumTestRect,		 			"numTestRect");
_LIT(KAutoUpdate,					"autoUpdate");
_LIT(KCmdNewScreenDeviceL,			"NewScreenDeviceL");
_LIT(KCmdDestructor,				"~");
_LIT(KCmdInitScreen,				"InitScreen");
_LIT(KCmdSetDisplayMode,			"SetDisplayMode");
_LIT(KCmdSetAutoUpdate,				"SetAutoUpdate");
_LIT(KCmdOrientationsAvailable,		"OrientationsAvailable");
_LIT(KCmdSetOrientation,			"SetOrientation");
_LIT(KCmdWriteRgb, 					"WriteRgb");
_LIT(KCmdWriteLine, 				"WriteLine");
_LIT(KCmdWriteBinaryLine, 			"WriteBinaryLine");
_LIT(KCmdWriteRgbMulti, 			"WriteRgbMulti");
_LIT(KCmdReadLine,		 			"ReadLine");
_LIT(KCmdWriteRGBAlphaLine, 		"WriteRGBAlphaLine");
_LIT(KCmdSetShadowMode, 			"SetShadowMode");
_LIT(KCmdShadowArea, 				"ShadowArea");
_LIT(KCmdWriteBinary, 				"WriteBinary");
_LIT(KCmdWriteBinaryLineVertical, 	"WriteBinaryLineVertical");
_LIT(KNoShadow,						"ENoShadow");
_LIT(KShadow,						"EShadow");
_LIT(KFade,							"EFade");
_LIT(KShadowFade,					"EShadowFade");
_LIT(KNone, 						"ENone");
_LIT(KGray2,						"EGray2");
_LIT(KGray4,						"EGray4");
_LIT(KGray16,						"EGray16");
_LIT(KGray256,						"EGray256");
_LIT(KColor16,						"EColor16");
_LIT(KColor256,						"EColor256");
_LIT(KColor64K,						"EColor64K");
_LIT(KColor16M,						"EColor16M");
_LIT(KRgb,							"ERgb");
_LIT(KColor4K,						"EColor4K");
_LIT(KColor16MU,					"EColor16MU");
_LIT(KColor16MA,					"EColor16MA");
_LIT(KColor16MAP,					"EColor16MAP");
_LIT(KColorLast,					"EColorLast");
_LIT(KDrawModePEN,					"EDrawModePEN");
_LIT(KDrawModeAND,					"EDrawModeAND");
_LIT(KDrawModeXOR,					"EDrawModeXOR");
_LIT(KDrawModeOR,					"EDrawModeOR");
_LIT(KDrawModeNOTSCREEN,			"EDrawModeNOTSCREEN");
_LIT(KDrawModeNOTPEN,				"EDrawModeNOTPEN");
_LIT(KDelay,						"delay");
_LIT(KOrientationNormal,			"EOrientationNormal");
_LIT(KOrientationRotated90,			"EOrientationRotated90");
_LIT(KOrientationRotated180,		"EOrientationRotated180");
_LIT(KOrientationRotated270,		"EOrientationRotated270");

/**
 * Shadow modes
 */
const CDataWrapperBase::TEnumEntryTable	CT_CFbsDrawDeviceData::iEnumShadowModes[] =
	{
	KNoShadow,		1,
	KShadow,		2,
	KFade,			3,
	KShadowFade,	4
	};

/**
 *Display Modes
 */
const CDataWrapperBase::TEnumEntryTable	CT_CFbsDrawDeviceData::iEnumDisplayModes[] =
	{
	KNone,			ENone,
	KGray2,			EGray2,
	KGray4,			EGray4,
	KGray16,		EGray16,
	KGray256,		EGray256,
	KColor16,		EColor16,
	KColor256,		EColor256,
	KColor64K,		EColor64K,
	KColor16M,		EColor16M,
	KRgb,			ERgb,
	KColor4K,		EColor4K,
	KColor16MU,		EColor16MU,
	KColor16MA,		EColor16MA,
	KColor16MA,		EColorLast,
	KColor16MAP,	EColor16MAP,
	KColorLast, 	EColor64K
	};

/**
 * Draw modes
 */
const CDataWrapperBase::TEnumEntryTable CT_CFbsDrawDeviceData::iEnumDrawModes[] =
	{
	KDrawModePEN,		CGraphicsContext::EDrawModePEN,
	KDrawModeAND,		CGraphicsContext::EDrawModeAND,
	KDrawModeXOR,		CGraphicsContext::EDrawModeXOR,
	KDrawModeOR,		CGraphicsContext::EDrawModeOR,
	KDrawModeNOTSCREEN,	CGraphicsContext::EDrawModeNOTSCREEN,
	KDrawModeNOTPEN,	CGraphicsContext::EDrawModeNOTPEN
	};

const CDataWrapperBase::TEnumEntryTable CT_CFbsDrawDeviceData::iEnumOrientationModes[] =
	{
	KOrientationNormal,		CFbsDrawDevice::EOrientationNormal,
	KOrientationRotated90,	CFbsDrawDevice::EOrientationRotated90,
	KOrientationRotated180,	CFbsDrawDevice::EOrientationRotated180,	 
	KOrientationRotated270,	CFbsDrawDevice::EOrientationRotated270
	};

CT_CFbsDrawDeviceData* CT_CFbsDrawDeviceData::NewL() 
	{
	CT_CFbsDrawDeviceData*	ret = new (ELeave) CT_CFbsDrawDeviceData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_CFbsDrawDeviceData::CT_CFbsDrawDeviceData()
:	iDrawDevice(NULL),	
	iDMode(CGraphicsContext::EDrawModePEN),	
	iDispMode(EColor64K),	
	iOrientation(0),	
	iSize(0,0),	
	iPostBlendShadow(ENoPostShadow),
	iTestRect(),
	iNumTestRects(0)
	{	
	}


void CT_CFbsDrawDeviceData::ConstructL()
	{
	}

/**
 * Public destructor
 */
CT_CFbsDrawDeviceData::~CT_CFbsDrawDeviceData()
	{
	DestroyData();
	}


/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_CFbsDrawDeviceData::GetObject()
	{
	return iDrawDevice;
	}

/**
 * Command for Destructor
 */
void CT_CFbsDrawDeviceData::DoCmdDestructor()
	{
	DestroyData();
	}

/**
 * Destroys used objects
 */
void CT_CFbsDrawDeviceData::DestroyData()
	{
	iTestRect.Reset();
	iTestRect.Close();
	iDispMode = ENone;
	iSize.SetSize(0,0);
	
	if(iDrawDevice)
		{		
		delete iDrawDevice;
		iDrawDevice = NULL;		
		}
	}

/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
TBool CT_CFbsDrawDeviceData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, 
		const TInt/* aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if 	( aCommand==KCmdInitScreen )
		{
		DoCmdInitScreen();
		}
	else if ( aCommand==KCmdSetDisplayMode )	
		{
		DoCmdSetDisplayMode();
		}
	else if ( aCommand==KCmdSetAutoUpdate )	
		{
		DoCmdSetAutoUpdate(aSection);
		}
	else if ( aCommand==KCmdOrientationsAvailable )	
		{
		DoCmdOrientationsAvailable();
		}
	else if ( aCommand==KCmdSetOrientation )	
		{
		DoCmdSetOrientation(aSection);
		}
	else if ( aCommand==KCmdNewScreenDeviceL )	
		{
		DoCmdNewScreenDeviceL(aSection);
		}
	else if ( aCommand==KCmdDestructor )	
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdWriteLine )	
		{
		DoCmdWriteLineL(aSection);		
		}
	else if ( aCommand==KCmdWriteBinaryLine )	
		{
		DoCmdWriteBinaryLineL(aSection);		
		}
	else if ( aCommand==KCmdWriteRgb )	
		{
		DoCmdWriteRgb(aSection);		
		}
	else if( aCommand==KCmdWriteRgbMulti )
		{
		DoCmdWriteRgbMulti(aSection);		
		}
	else if( aCommand==KCmdReadLine )
		{
		DoCmdReadLineL(aSection);		
		}
	else if( aCommand==KCmdWriteRGBAlphaLine )
		{
		DoCmdWriteRGBAlphaLineL(aSection);		
		}
	else if( aCommand==KCmdSetShadowMode )
		{
		DoCmdSetShadowMode(aSection);
		}
	else if( aCommand==KCmdShadowArea )
		{
		DoCmdShadowArea(aSection);
		}
	else if( aCommand==KCmdWriteBinary )
		{
		DoCmdWriteBinaryL(aSection);
		}
	else if( aCommand==KCmdWriteBinaryLineVertical )
		{
		DoCmdWriteBinaryLineVerticalL(aSection);
		}	
	else
		{
		ret = EFalse;
		}
	
	return ret;
	}

/**
 * Tests WriteRgbMulti method from CFbsDrawDevice
 * @param aSection			The section in the ini containing data for the command
 * @return 							KErrNone if no error found.
 */
void CT_CFbsDrawDeviceData::DoCmdWriteRgbMulti(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdWriteRgbMulti"));
	TInt drawMode;
	TInt delay=0;
	TInt nRect = 0;
	TBool dataOk=ETrue;
	
	GETFROMCONFIGMANDATORY(Int, aSection, KNumRect(),nRect,_L("Error in getting parameter %S from INI file"),dataOk);
	GETFROMCONFIGMANDATORY(Int, aSection, KDelay(),delay,_L("Error in getting parameter %S from INI file"),dataOk);
	
	if ( !GetEnumFromConfig(aSection, KDrawMode(), iEnumDrawModes, drawMode) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDrawMode());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		INFO_PRINTF2(_L("Change draw mode to: %d"), drawMode);
		iDMode=CGraphicsContext::TDrawMode(drawMode);
		
		for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
			{
			TRgb backColor = TestColor[nBackColor];
			Clear(backColor);
			for (TInt nColor = 0; nColor < KNumTestColors; nColor++)
				{
				TInt shadowMode = EPostFade;
				TRect rect = iTestRect[nRect];			
				TRgb color = TestColor[nColor];				
				SetShadowAndFadingMode(shadowMode,100, 200);
				iDrawDevice->WriteRgbMulti(rect.iTl.iX,rect.iTl.iY,rect.Width(),rect.Height(),color,iDMode);
				
				User::After(delay);
				}
			}
		}
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdWriteRgbMulti"));
	}

/**
 * Tests WriteRgb method from CFbsDrawDevice
 */
void CT_CFbsDrawDeviceData::DoCmdWriteRgb(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdWriteRgb"));
	TInt drawMode;
	TInt nRect = 0;	
	TInt delay;
	TBool dataOk=ETrue;
	
	if (!GetEnumFromConfig(aSection, KDrawMode(), iEnumDrawModes,drawMode))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDrawMode());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	GETFROMCONFIGMANDATORY(Int, aSection, KDelay(),delay,_L("Error in getting parameter %S from INI file"),dataOk);
	GETFROMCONFIGMANDATORY(Int, aSection, KNumRect(),nRect,_L("Error in getting parameter %S from INI file"),dataOk);
	
	if (dataOk)
		{
		iDMode=CGraphicsContext::TDrawMode(drawMode); 	
		for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
			{			
			TRgb backColor = TestBackground[nBackColor];
			Clear(backColor);
			for (TInt nColor = 0; nColor < KNumTestColors; nColor++)
				{					
				TInt shadowMode = ENoPostShadow;																					
				TRect rect = iTestRect[nRect];
				TRgb color = TestColor[nColor];
				SetShadowAndFadingMode(shadowMode,100, 200);
				iDrawDevice->WriteRgb(rect.iTl.iX,rect.iTl.iY,color,iDMode);
				User::After(delay);
				}			
			}
		}
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdWriteRgb"));
	}

/**
 *  Validates the received parameters and handles errors returned from TestScreenDrawL
 *  @param aSection			The section in the ini containing data for the command
 * 	@return 							KErrNone if no error found.
 */
void CT_CFbsDrawDeviceData::DoCmdNewScreenDeviceL(const TTEFSectionName& aSection)
 	{
 	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdNewScreenDeviceL"));
 	
 	if(iDrawDevice)
 		{
 		delete iDrawDevice;
	 	iDrawDevice=NULL;
 		}
 	TInt drawMode;
 	TBool dataOk = ETrue;
 	if (!GetEnumFromConfig(aSection, KDisplayMode(), iEnumDisplayModes, drawMode))
 		{
 		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDisplayMode());
 	    SetBlockResult(EFail);
 	    dataOk = EFalse;
 	    }
 	if(dataOk)
 		{
 		TDisplayMode displayMode = TDisplayMode(drawMode);
 		iDispMode = displayMode;
 		TInt ScreenNo = 0;
 		
	 	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayXPixels,iSize.iWidth));
	 	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayYPixels,iSize.iHeight));
	 	INFO_PRINTF3(_L("WIDTH from HAL %i \nHEIGHT from HAL %i"),iSize.iWidth,iSize.iHeight);
	 	ASSERT(iSize.iWidth > 0 && iSize.iHeight > 0);
	 	 	
	 		 	
	 	TRAPD(ret,iDrawDevice = CFbsDrawDevice::NewScreenDeviceL(ScreenNo,displayMode));
	 		
	 	// check error code
	 	if(ret != KErrNone)
	 		{
	 		ERR_PRINTF2(_L("CFbsDrawDevice creation error: %d"), ret);
	 		SetError(ret);
	 		}
	 	else
	 		{
	 		GETFROMCONFIGMANDATORY(Int, KRectangleSection(), KNumTestRect(),iNumTestRects,_L("Error in getting parameter %S from INI file"),dataOk);
	 		
	 		if (!InitRectangles())
	 			{ 				
	 			SetBlockResult(EFail);
	 			ERR_PRINTF1(_L("Error: Test Rectangles NOT set!")); 						
	 			}
	 		else
	 			{ 	 			
	 			INFO_PRINTF1(_L("Test Rectangles are now set!"));	
	 			}
	 		}
	 	INFO_PRINTF1(_L("** DoCmdNewScreenDeviceL succeded!"));	
 		}
 	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdNewScreenDeviceL"));
 	}

/**
 * Inits Screen
 */
void CT_CFbsDrawDeviceData::DoCmdInitScreen()
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdInitScreen"));
	TInt err = iDrawDevice->InitScreen();
	if(err != KErrNone)
		{		
		ERR_PRINTF2(_L("InitScreen error: %d"), err);
		SetError(err);
		}
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdInitScreen"));
	}

/**
 * Sets Display Mode
 * */
void CT_CFbsDrawDeviceData::DoCmdSetDisplayMode()
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdSetDisplayMode"));
	iDrawDevice->SetDisplayMode(iDrawDevice);
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdSetDisplayMode"));
	}

/**
 * Sets or unsets auto-update for the screen.
 */
void CT_CFbsDrawDeviceData::DoCmdSetAutoUpdate(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdSetAutoUpdate"));
	TBool autoUpdate=ETrue;
	GETFROMCONFIGOPTIONAL(Bool, aSection, KAutoUpdate(),autoUpdate,_L("Error in getting parameter %S from INI file"));
	iDrawDevice->SetAutoUpdate(autoUpdate);
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdSetAutoUpdate"));
	}

/**
 * checks for Available Orientations
 */
void CT_CFbsDrawDeviceData::DoCmdOrientationsAvailable()
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdOrientationsAvailable"));
	TBool orientation[4];
	iDrawDevice->OrientationsAvailable(orientation);
	INFO_PRINTF2(_L("Orientation Normal : %i"),orientation[CFbsDrawDevice::EOrientationNormal]);
	INFO_PRINTF2(_L("Orientation Rotated 90 : %i"),orientation[CFbsDrawDevice::EOrientationRotated90]);
	INFO_PRINTF2(_L("Orientation Rotated 180 : %i"),orientation[CFbsDrawDevice::EOrientationRotated180]);	 
	INFO_PRINTF2(_L("Orientation Rotated 270 : %i"),orientation[CFbsDrawDevice::EOrientationRotated270]);
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdOrientationsAvailable"));
	}

/**
 * Sets orientation
 */
void CT_CFbsDrawDeviceData::DoCmdSetOrientation(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdSetOrientation"));
	TBool dataOk = ETrue;
	
	
	if ( !GetEnumFromConfig(aSection, KOrientationMode(), iEnumOrientationModes, iOrientation) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KOrientationMode());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	
	if(dataOk)
		{
		
		TBool orientationSupported = iDrawDevice->SetOrientation(CFbsDrawDevice::TOrientation(iOrientation));
		
		if (orientationSupported)
			{
			INFO_PRINTF2(_L("Orientation was set to: %d"), iOrientation);
			if (iOrientation & 1)
				{
				iSize.SetSize(iSize.iHeight, iSize.iWidth);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Orientation not Supported"));
			}
		}
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdSetOrientation"));		
	}

/**
 * Tests WriteLine method from CFbsDrawDevice
 */
void CT_CFbsDrawDeviceData::DoCmdWriteLineL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdWriteLineL"));
	TInt byteSize = (ByteSize() / iSize.iHeight);
	TInt delay;
	TBool dataOk = ETrue;
	
	INFO_PRINTF2(_L("After setting bytesize %d"),byteSize);
	TUint8* backBuffer  = new(ELeave) TUint8[byteSize];
	CleanupStack::PushL(backBuffer);
	TUint8* writeBuffer = new(ELeave) TUint8[byteSize];
	CleanupStack::PushL(writeBuffer);
	TUint8* copyBuffer  = new(ELeave) TUint8[byteSize];
	CleanupStack::PushL(copyBuffer);
	TUint8* readBuffer  = new(ELeave) TUint8[byteSize];
	CleanupStack::PushL(readBuffer);
	
	INFO_PRINTF1(_L("after setting buffers"));
	
	TInt drawMode;	
	if (!GetEnumFromConfig(aSection, KDrawMode(), iEnumDrawModes,drawMode))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDrawMode());
		SetBlockResult(EFail);
		dataOk = EFalse;
	    }
	GETFROMCONFIGMANDATORY(Int, aSection, KDelay(),delay,_L("Error in getting parameter %S from INI file"),dataOk);
	
	INFO_PRINTF1(_L("Test Rectangles are now set!"));
	TInt nRect = 0;
	GETFROMCONFIGMANDATORY(Int, aSection, KNumRect(),nRect,_L("Error in getting parameter %S from INI file"),dataOk);
	
	TRect rect = iTestRect[nRect];
	TInt nBackColor = 0;
	GETFROMCONFIGMANDATORY(Int, aSection, KBackColor(),nBackColor,_L("Error in getting parameter %S from INI file"),dataOk);
	
	if(dataOk)
		{
		iDMode=CGraphicsContext::TDrawMode(drawMode);
		TRgb b = TestBackground[nBackColor];
		Clear(b);
		for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
			{
			FillBuffer(writeBuffer,byteSize,iDispMode);
				
			Mem::Copy(copyBuffer,writeBuffer,byteSize);
			iDrawDevice->WriteLine(rect.iTl.iX,yy,rect.Width(),(TUint32*)writeBuffer,iDMode);
			Mem::FillZ(readBuffer,byteSize);
			User::After(delay);
			}
		}		
		INFO_PRINTF1(_L("Deleting buffers"));	
        CleanupStack::PopAndDestroy(4);	
		INFO_PRINTF1(_L("Buffers deleted"));		
		INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdWriteLineL"));
	}

/**
 * Tests WriteBinaryLine from CFbsDrawDevice
 */
void CT_CFbsDrawDeviceData::DoCmdWriteBinaryLineL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdWriteBinaryLineL"));
	TInt drawMode;
	TInt nRect = 0;
	TInt delay;
	TBool dataOk = ETrue;
	
	if (!GetEnumFromConfig(aSection, KDrawMode(),iEnumDrawModes, drawMode))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDrawMode());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	GETFROMCONFIGMANDATORY(Int, aSection, KDelay(),delay,_L("Error in getting parameter %S from INI file"),dataOk);
	
	if (!GetIntFromConfig(aSection, KNumRect(), nRect))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KNumRect());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt nBackColor = 0;
	if (!GetIntFromConfig(aSection, KBackColor(), nBackColor))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KBackColor());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	TInt shadowMode = ENoPostShadow;	
	if (!GetEnumFromConfig(aSection, KShadowMode(), iEnumShadowModes, shadowMode))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KShadowMode());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	
	if(dataOk)
		{
		iDMode=CGraphicsContext::TDrawMode(drawMode);
		
		TInt byteSize = ByteSize() / iSize.iHeight;
		TInt wordSize = (byteSize + 3) / 4;
		TUint32* writeBuffer = new(ELeave) TUint32[wordSize];
		CleanupStack::PushL(writeBuffer);
		INFO_PRINTF1(_L("Test Rectangles are now set!"));
		
		
		TRect rect = iTestRect[nRect];
			
		
			
		for (TInt nColor = 0; nColor < KNumTestColors; nColor++)
			{
			if (rect.Width() > KRectWidth)
				{
				rect.iBr.iX = rect.iTl.iX + KRectWidth;
				}
			if (rect.Width() < 1)
				{
				rect.iBr.iX = rect.iTl.iX + 1;
				}
				
			TRgb backgroundColor = TestBackground[nBackColor];
			Clear(backgroundColor);
			TRgb testColor = TestColor[nColor];
				
			iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
			FillBuffer((TUint8*)writeBuffer,byteSize,iDispMode);
			iDrawDevice->WriteBinaryLine(rect.iTl.iX,rect.iTl.iY,writeBuffer,rect.Width(),testColor,iDMode);
			User::After(delay);
			}		
		CleanupStack::PopAndDestroy();
		writeBuffer = NULL;
		}
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdWriteBinaryLineL"));
	}

/**
 * Tests ReadLine from CFbsDrawDevice
 * @param aSection			The section in the ini containing data for the command
 * @return 					KErrNone if no error found.
 */
void CT_CFbsDrawDeviceData::DoCmdReadLineL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdReadLineL"));
	TInt byteSize = ByteSize() / iSize.iHeight;
	TUint8* writeBuffer = new(ELeave) TUint8[byteSize];
	CleanupStack::PushL(writeBuffer);
	TUint8* readBuffer = new(ELeave) TUint8[iSize.iWidth * sizeof(TRgb)];
	CleanupStack::PushL(readBuffer);
	
	FillBuffer((TUint8*)writeBuffer,byteSize,iDispMode);
	FillBuffer((TUint8*)readBuffer,byteSize,iDispMode);
	
	TInt nBackColor = 0;
	if (!GetIntFromConfig(aSection, KBackColor(), nBackColor))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KBackColor());
		SetBlockResult(EFail);
	    }
	
	TRgb b = TestColor[nBackColor];
	Clear(b);

	for (TInt nDispMode = 0; nDispMode < KNumDispModes; nDispMode++)
		{
		TDisplayMode dispMode = TestDisplayMode[nDispMode];
		
		for (TInt cnt=0;cnt<2;cnt++)
			{
			if (cnt==0)  //first time 
				{
				iDrawDevice->SetUserDisplayMode(dispMode);
				}
			else
				{
				iDrawDevice->SetUserDisplayMode(iDispMode);
				}
				
			for (TInt nRect = 0; nRect < iNumTestRects; nRect++)
				{
				TRect rect = iTestRect[nRect];
				for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
					{
					FillBuffer(writeBuffer,byteSize,iDispMode);
					Mem::FillZ(readBuffer,byteSize);
					iDrawDevice->ReadLine(rect.iTl.iX,yy,rect.Width(),(TUint32*)readBuffer,dispMode);
					Mem::FillZ(readBuffer,byteSize);
					}
				}
			}
		}	
    CleanupStack::PopAndDestroy(2);
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdReadLineL"));
	}

/**
 * Tests RGBAlphaLine from CFbsDrawDevice
 * @param aSection			The section in the ini containing data for the command
 * @return 					KErrNone if no error found.
 */
void CT_CFbsDrawDeviceData::DoCmdWriteRGBAlphaLineL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdWriteRGBAlphaLineL"));
	TInt drawMode;
	
 	if (!GetEnumFromConfig(aSection, KDrawMode(), iEnumDrawModes,drawMode))
 		{
 		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDrawMode());
 		SetBlockResult(EFail);
 	    }
 	
 	iDMode=CGraphicsContext::TDrawMode(drawMode);
 	TInt byteSize = ByteSize() / iSize.iHeight;
	INFO_PRINTF1(_L("In TestWriteRGBAlphaLine"));
	
	TUint8* writeBuffer = new(ELeave) TUint8[iSize.iWidth * 4];
	CleanupStack::PushL(writeBuffer);
	TUint8* writeBuffer2 = new(ELeave) TUint8[iSize.iWidth * 4];
	CleanupStack::PushL(writeBuffer2);
	TUint8* maskBuffer = new(ELeave) TUint8[iSize.iWidth * 4];
	CleanupStack::PushL(maskBuffer);
	
	FillBuffer(writeBuffer,byteSize,iDispMode);
	FillBuffer(writeBuffer2,byteSize,iDispMode);
	FillBuffer(maskBuffer,byteSize,iDispMode);
	
	TInt nRect;
	for (TInt maskFillCount=0; maskFillCount<KMaskFill;maskFillCount++)
		{
		for (nRect = 0; nRect < iNumTestRects; nRect++)
			{
			for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
				{
				TRgb testColor = TestColor[nBackColor];
				Clear(testColor);
				for (TInt nColor = 0; nColor < KNumTestColors; nColor++)
					{
					//ensure the colour has the 0xff mask value
					TRgb checkColor;
					TRgb c = TestColor[nColor];
					TUint32 internal = c.Internal();
					TInt yy;
					TRect rect = iTestRect[nRect];
					TInt shadowMode =ENoPostShadow;
					//use a combined alpha for the check colour
					TUint32 combinedAlpha = MaskFill[maskFillCount]*((internal&0xff000000)>>24);
					combinedAlpha = ((combinedAlpha <<16) & 0xff000000);
					checkColor.SetInternal((internal&0x00ffffff)|combinedAlpha);
					SetShadowAndFadingMode(shadowMode,100, 200);
					for (yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
						{
						iDrawDevice->WriteRgbAlphaLine(rect.iTl.iX,yy,rect.Width(),
								writeBuffer,
								writeBuffer2,
								maskBuffer,
								iDMode);
						}
					//require to Shadow After the checkColor, no shadow with a zero mask
					TBool shadowModeChanged = EFalse;
					if (MaskFill[maskFillCount])
						{
						iPostBlendShadow = (TPostShadowMode) shadowMode;
						shadowMode = 0;
						shadowModeChanged = ETrue;
						}
					if(shadowModeChanged)
						{
						shadowMode = iPostBlendShadow;
						}
					iPostBlendShadow = ENoPostShadow;
					}
				}
			}
		}
	INFO_PRINTF1(_L("** delete buffers"));
    CleanupStack::PopAndDestroy(3);
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdWriteRGBAlphaLineL"));
	}

/**
 * Tests ShadowArea from CFbsDrawDevice
 * @return 	KErrNone if no error found.
 */
void CT_CFbsDrawDeviceData::DoCmdShadowArea(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdShadowArea"));
	TInt delay=0;
	TBool dataOk = ETrue;
	
	GETFROMCONFIGMANDATORY(Int, aSection, KDelay(),delay,_L("Error in getting parameter %S from INI file"),dataOk);
	
	if(dataOk)
		{
		INFO_PRINTF1(_L("In TestShadow()"));
		TInt nBackColor = 1;	 	
		TRgb testColor = TestColor[nBackColor];
		Clear(testColor);

		for (TInt shadowMode = 0; shadowMode < KNumShadowModes; shadowMode++)
			{
			for (TInt nRect = 0; nRect < iNumTestRects; nRect++)
				{
				for (TInt nColor = 0; nColor < KNumTestColors; nColor++)
					{
					TRect rect = iTestRect[nRect];
					iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
					iDrawDevice->ShadowArea(rect);
					
					User::After(delay);
					}
				}
			}
		}
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdShadowArea"));
			
	}

/**
 * Tests WriteBinaryLineVertical from CFbsDrawDevice
 * @return 			KErrNone if no error found.
 */
void CT_CFbsDrawDeviceData::DoCmdWriteBinaryLineVerticalL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdWriteBinaryLineVerticalL"));
	TInt drawMode;
 	TInt delay;
 	TBool dataOk = ETrue;
	if (!GetEnumFromConfig(aSection, KDrawMode(), iEnumDrawModes,drawMode))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDrawMode());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	GETFROMCONFIGMANDATORY(Int, aSection, KDelay(),delay,_L("Error in getting parameter %S from INI file"),dataOk);
	TInt nRect = 0;
	if (!GetIntFromConfig(aSection, KNumRect(), nRect))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KNumRect());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}

	if(dataOk)
		{
		iDMode=CGraphicsContext::TDrawMode(drawMode);
		
		TInt byteSize = ByteSize() / iSize.iHeight;
		TInt wordSize = (byteSize + 3) / 4;
		TUint32* writeBuffer = new(ELeave) TUint32[wordSize];
		TInt nBackColor = 1;
		TInt nColor = 4;					
		
		TRect rect = iTestRect[nRect];
		if (rect.Width() > KRectWidth)
			{
			rect.iBr.iX = rect.iTl.iX + KRectWidth;
			}
		if (rect.Width() < 1)
			{
			rect.iBr.iX = rect.iTl.iX + 1;
			}
		
		TRgb b = TestBackground[nBackColor];
		TRgb c = TestColor[nColor];
		Clear(b);


		//NOTE Call Command within script: SetShadowmode
		FillBuffer((TUint8*)writeBuffer,byteSize,iDispMode);
		iDrawDevice->WriteBinaryLineVertical(rect.iTl.iX,rect.iBr.iY - 1,writeBuffer,rect.Height(),c,iDMode,ETrue);

		User::After(delay);
		delete [] writeBuffer;
		writeBuffer = NULL;
		}
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdWriteBinaryLineVerticalL"));
	}

/**
 * Sets the current shadowing mode
 * @param aSection			The section in the ini containing data for the command
 */
void CT_CFbsDrawDeviceData::DoCmdSetShadowMode(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdSetShadowMode"));
	TInt shadowMode = ENoPostShadow;
	if(!GetEnumFromConfig(aSection, KShadowMode(), iEnumShadowModes, shadowMode))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KShadowMode());
		SetBlockResult(EFail);
		}
	else
		{
		iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
		}
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdSetShadowMode"));
	}

/**
 * Tests WriteBinary method from CFbsDrawDevice
 * @param aSection			The section in the ini containing data for the command
 * @return 							KErrNone if no error found.
 */
void CT_CFbsDrawDeviceData::DoCmdWriteBinaryL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::DoCmdWriteBinaryL"));
	TInt drawMode;
	TInt delay;
	TBool dataOk = ETrue;
	if (!GetEnumFromConfig(aSection, KDrawMode(), iEnumDrawModes,drawMode))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDrawMode());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	GETFROMCONFIGMANDATORY(Int, aSection, KDelay(),delay,_L("Error in getting parameter %S from INI file"),dataOk);
	TInt nRect = 0;
	if (!GetIntFromConfig(aSection, KNumRect(), nRect))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KNumRect());
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	if(dataOk)
		{
		iDMode=CGraphicsContext::TDrawMode(drawMode);
		TInt byteSize = ByteSize() / iSize.iHeight;
		TInt wordSize = (byteSize + 3) / 4;
		TUint32* writeBuffer = new (ELeave)TUint32[wordSize];
		
		TRect rect = iTestRect[nRect];
		TInt nBackColor = 0;
		TInt nColor = 4;

		if (rect.Width() > KRectWidth)
			{
			rect.iBr.iX = rect.iTl.iX + KRectWidth;
			}
		if (rect.Width() < 1)
			{
			rect.iBr.iX = rect.iTl.iX + 1;
			}
		
		TRgb b = TestBackground[nBackColor];
		Clear(b);
		TRgb c = TestColor[nColor];
		//NOTE Call Command within script: SetShadowmode
		FillBuffer((TUint8*)writeBuffer,byteSize,iDispMode);
		iDrawDevice->WriteBinary(rect.iTl.iX,rect.iTl.iY,writeBuffer,rect.Width(),rect.Height(),c,iDMode);
		
		User::After(delay);
		delete [] writeBuffer;
		writeBuffer = NULL;
		}
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::DoCmdWriteBinaryL"));
	}

/**
 * Clears screen with specified color
 */
void CT_CFbsDrawDeviceData::Clear(TRgb aColor)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::Clear"));
	iDrawDevice->SetShadowMode(CFbsDrawDevice::ENoShadow);
	iDrawDevice->WriteRgbMulti(0,0,iSize.iWidth,iSize.iHeight,aColor,CGraphicsContext::EDrawModeWriteAlpha);
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::Clear"));
	}
/**
 * Sets Shadow and fading modes
 * @param one of the supported shadow modes ENoShadow,EShadow,EFade,EShadowFade
 * @param a black point
 * @param a white point
 */
void CT_CFbsDrawDeviceData::SetShadowAndFadingMode(TInt shadowMode,TInt blackPoint, TInt whitePoint)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::SetShadowAndFadingMode"));
	iDrawDevice->SetFadingParameters(blackPoint,whitePoint);  
	iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::SetShadowAndFadingMode"));
	}


/**
 * Fills a buffer to be displayed on screen
 * 
 * @param aBuffer
 * @param aByteSize
 * @param aDispMode
 */
void CT_CFbsDrawDeviceData::FillBuffer(TUint8* aBuffer,TInt aByteSize,TDisplayMode aDispMode)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::FillBuffer"));
	TUint8* bufferLimit = aBuffer + aByteSize;

	TInt64 seed = TInt64(TInt(aBuffer) * aByteSize * aDispMode * User::TickCount());

	if (aDispMode != EColor4K)
		{
		while (aBuffer < bufferLimit)
			{
			*aBuffer++ = (TUint8)Math::Rand(seed);
			}
		}
	else
		{
		while (aBuffer < bufferLimit)
			{
			if (TInt(aBuffer) & 3 == 3)
				{
				*aBuffer++ = 0;
				}
			else
				{
				*aBuffer++ = (TUint8)Math::Rand(seed);
				}
			}
		}
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::FillBuffer"));
	}

/**
 * Sets the Byte Size based on screen Width and Height
 */
TInt CT_CFbsDrawDeviceData::ByteSize()
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::ByteSize"));	
	TInt wordSize = iSize.iWidth;
	switch (iDispMode)
		{
	case EGray2:
		wordSize = (wordSize + 31) / 32;
		break;
	case EGray4:
		wordSize = (wordSize + 15) / 16;
		break;
	case EGray16:
	case EColor16:
		wordSize = (wordSize + 7) / 8;
		break;
	case EGray256:
	case EColor256:
		wordSize = (wordSize + 3) / 4;
		break;
	case EColor4K:
	case EColor64K:
		wordSize = (wordSize + 1) / 2;
		break;
	case EColor16M:
		wordSize = (((wordSize * 3) + 11) / 12) * 3;
		break;
	case EColor16MU:
	case EColor16MA:
		//Should not be changed!
		break;
	default:
		break;
		};
	
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::ByteSize"));	
	return wordSize * iSize.iHeight * 4;
	}

/**
 *	This method reads a string with this form: (a,,b,c,d) and splits it in an TInt array. 
 * 
 */
void CT_CFbsDrawDeviceData::ReadRectL(HBufC& aString, RArray<TInt>& aResult)
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::ReadRectL"));
	TChar chr;
	TLex lex(aString);
	TBuf<256> buf;
	
	lex.Inc();			
	while(!lex.Eos())
        {
        chr = lex.Get();
        // Check if there was a list separator
        if (chr == ',')
            {
            HBufC* param = buf.AllocLC();
            buf.Zero();
            TInt x = 0;
            TLex lexX(*param);
            lexX.Val(x);
            aResult.Append(x);
            CleanupStack::Pop(param); // pointer to buf is stored in RPointerArray
            }
        // If not separator character we can store the character into array
        else
            {
            if (chr != ')')
            	{
            	buf.Append(chr);
            	}
            }
        }
        
	HBufC* param = buf.AllocLC();
	TInt x = 0;
    TLex lexX(*param);
    lexX.Val(x);
    aResult.Append(x);	
	CleanupStack::PopAndDestroy(param);
	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::ReadRectL"));
	}

/**
 *	This method reads a string with this form: (a,,b,c,d) and splits it in an TInt array. 
 * 
 */
TBool CT_CFbsDrawDeviceData::InitRectangles()
	{
	INFO_PRINTF1(_L("*START* CT_CFbsDrawDeviceData::InitRectangles"));
	TBool readResult = ETrue;
	RPointerArray<HBufC> dimensions;
	RArray<TInt> result;
	TInt err = KErrNone;
	
	if ( GetArrayRectFromConfig( KRectangleSection(), KRectangles(), dimensions))//its validated by the above call	
		{
		for (TInt i = 0; i < iNumTestRects; i++)
			{
			TRAP(err , ReadRectL(*dimensions[i], result));
			if (err != KErrNone)
				{
				readResult = EFalse;
				break;
				}
			else
				{							
				iTestRect.Append(TRect(result[0],result[1],result[2],result[3]));
				result.Reset();
				}
			}		
		} 
	else
		{		
		readResult = EFalse;
		}
	dimensions.ResetAndDestroy();
	result.Reset();
	result.Close();

	INFO_PRINTF1(_L("*END* CT_CFbsDrawDeviceData::InitRectangles"));
  	return readResult;

	}
