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

#include "T_DigitiserDriverData.h"


//These are attributes for the touch screen device
_LIT(KEnumPen,                                  "EPen");
_LIT(KEnumPenX,                                 "EPenX");
_LIT(KEnumPenY,                                 "EPenY");
_LIT(KEnumPenState,                             "EPenState");

//These are attributes for the sound driver device
_LIT(KEnumPenClick,                             "EPenClick");
_LIT(KEnumPenClickVolumeMax,                    "EPenClickVolumeMax");
_LIT(KEnumPenClickState,                        "EPenClickState");
_LIT(KEnumPenClickVolume,                       "EPenClickVolume");

//These are the attributes for the power controller
 _LIT(KEnumPenDisplayOn,                        "EPenDisplayOn");
 
 //These are the attributes for the lcd screen device
 _LIT(KEnumDisplayState,                        "EDisplayState");
 _LIT(KEnumDisplayMode,							"EDisplayMode");
 
 //This represents a NULL attribute
_LIT(KEnumNull,                                 "");

const CT_HALData::THalTableLookup	CT_DigitiserDriverData::iTableLookup[] =
    {
/**     
*	Enum as a descriptor            Enum                            Prepare input           Prepare paramters       Validation of           Store data returned after               aFlag indicating
*																	value prior to 			prior to				value returned			successful HAL::Get or                  if validation
*																	HAL::Set call           HAL::Get call           after a successful      HAL::Set call                           is mandatory
*																													HAL::Get call							
*/	
        
    KEnumPen,                       HALData::EPen,                  SetPrepareInt,          GetPrepareDummy,        GetValidationInt,       SetPen,                                 ETrue,
    KEnumPenState,                  HALData::EPenState,             SetPrepareInt,          GetPrepareDummy,        GetValidationInt,       SetPenState,                            ETrue,
    KEnumPenX,                      HALData::EPenX,                 SetPrepareInt,          GetPrepareDummy,        GetValidationInt,       SetPenX,                                ETrue,
    KEnumPenY,                      HALData::EPenY,                 SetPrepareInt,          GetPrepareDummy,        GetValidationInt,       SetPenY,                                ETrue,	
    KEnumPenClick,                  HALData::EPenClick,             SetPrepareInt,          GetPrepareDummy,        GetValidationInt,       SetPenClick,                            ETrue,
    KEnumPenClickVolumeMax,         HALData::EPenClickVolumeMax,    SetPrepareInt,          GetPrepareDummy,        GetValidationInt,       SetPenClickVolumeMax,                   ETrue,
    KEnumPenClickState,             HALData::EPenClickState,        SetPrepareInt,          GetPrepareDummy,        GetValidationInt,       SetPenClickState,                       ETrue,
    KEnumPenClickVolume,            HALData::EPenClickVolume,       SetPrepareInt,          GetPrepareDummy,        GetValidationInt,       SetPenClickVolume,                      ETrue,
    KEnumPenDisplayOn,              HALData::EPenDisplayOn,         SetPrepareInt,          GetPrepareDummy,        GetValidationInt,       SetPenDisplayOn,                        ETrue,
    KEnumDisplayState,				HALData::EDisplayState,			SetPrepareInt,			GetPrepareDummy,		GetValidationInt,		SetDisplayState,						ETrue,
    KEnumDisplayMode,				HALData::EDisplayMode,			SetPrepareInt,			GetPrepareDummy,		GetValidationInt,		SetDisplayMode,							EFalse,
    KEnumNull,                      (HALData::TAttribute)0,         NULL,                   NULL,                   NULL,                   NULL,                                   NULL
    };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DigitiserDriverData* CT_DigitiserDriverData::NewL()
	{
	CT_DigitiserDriverData*	digit=new (ELeave) CT_DigitiserDriverData();
	CleanupStack::PushL(digit);
	digit->ConstructL();
	CleanupStack::Pop(digit);
	return digit;
	}


CT_DigitiserDriverData::CT_DigitiserDriverData()
:       CT_HALData(iTableLookup)
,       iPenState(-1)
,       iPen(-1)
,       iPenX(-1)
,       iPenY(-1)
,       iPenClick(-1)
,       iPenClickVolumeMax(-1)
,       iPenClickState(-1)
,       iPenClickVolume	(-1)
,       iPenDisplayOn(-1)
,		iDisplayState(-1)
,		iDisplayMode(-1)
	{	
	}

void CT_DigitiserDriverData::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
   
	}

CT_DigitiserDriverData::~CT_DigitiserDriverData()
/**
 * Public destructor
 */
	{
	}

TAny* CT_DigitiserDriverData::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return NULL;
	}
	
TBool CT_DigitiserDriverData::GetPrepareDummy(CDataWrapperBase* /*aThis*/, const TDesC& /*aSection*/, TInt& /*aValueStart*/, TInt& /*aValueEnd*/)
/**
 * This a dummy place holder function that does nothing see HAL_Data
 *
 * @return a boolean value 
 *
 */	
	{
	return ETrue;
	}
	
void CT_DigitiserDriverData::SetPen(CDataWrapperBase* aThis, TInt aValue)
/**
 * Store whether the pen/digitiser is availabe for input
 *
 * @param 	aThis	a pointer to the data wrapper base class
 * @param  	aValue  an integer value to use in set 
 * 
 */
	{
	static_cast<CT_DigitiserDriverData*>(aThis)->iPen=aValue;	
	}

void CT_DigitiserDriverData::SetPenX(CDataWrapperBase* aThis, TInt aValue)
 /**
 *
 * Store pen/digitiser horizontal resolution in pixels
 *
 * @param 	aThis a pointer to the data wrapper base class
 * @param  	aValue  an integer value to use in set 
 */	
 	{
    static_cast<CT_DigitiserDriverData*>(aThis)->iPenX=aValue;	
    }
 
 void CT_DigitiserDriverData::SetPenY(CDataWrapperBase* aThis, TInt aValue)
 /**
 * Store pen/digitiser vertical resolution in pixels
 * 
 * @param       aThis a pointer to the data wrapper base class
 * @param       aValue  an integer value to use in set 
 * 
 */	
    {
    static_cast<CT_DigitiserDriverData*>(aThis)->iPenY=aValue;	
    }
     
void CT_DigitiserDriverData::SetPenState(CDataWrapperBase* aThis, TInt aValue)
 /**
 * Store Pen Clicking ability
 *
 * @param 	aThis a pointer to the data wrapper base class
 * @param  	aValue  an integer value to use in set 
 * 
 */
    {
    static_cast<CT_DigitiserDriverData*>(aThis)->iPenState=aValue;
    }
    
void CT_DigitiserDriverData::SetPenClick(CDataWrapperBase* aThis, TInt aValue)
 /**
 * Store Pen Click  
 * 
 * @param 	aThis a pointer to the data wrapper base class
 * @param  	aValue  an integer value to use in set 
 */	
	{
	static_cast<CT_DigitiserDriverData*>(aThis)->iPenClick=aValue;
	}

void CT_DigitiserDriverData::SetPenClickVolumeMax(CDataWrapperBase* aThis, TInt aValue)
 /**
 * Store maximum Pen Click Volume 
 *
 * @param 	aThis a pointer to the data wrapper base class
 * @param  	aValue  an integer value to use in set 
 */	
	{
	static_cast<CT_DigitiserDriverData*>(aThis)->iPenClickVolumeMax=aValue;
	}

void CT_DigitiserDriverData::SetPenClickState(CDataWrapperBase* aThis, TInt aValue)
 /**
 * Store pen click state
 *
 * @param  	aThis a pointer to the data wrapper base class
 * @param  	aValue  an integer value to use in set 
 * 
 */
	{
	static_cast<CT_DigitiserDriverData*>(aThis)->iPenClickState=aValue;
	}

void CT_DigitiserDriverData::SetPenClickVolume(CDataWrapperBase* aThis, TInt aValue)
 /**
 * Store pen click volume
 *
 * @param  	aThis a pointer to the data wrapper base class
 * @param  	aValue  an integer value to use in set 
 */	
	{
	static_cast<CT_DigitiserDriverData*>(aThis)->iPenClickVolume=aValue;
	}

void CT_DigitiserDriverData::SetPenDisplayOn(CDataWrapperBase* aThis, TInt aValue)
 /**
 * Store PenDisplay On
 * @param 		aThis a pointer to the data wrapper base class
 * @param   	aValue  an integer value to use in set 
 * 
 */	
	{
	static_cast<CT_DigitiserDriverData*>(aThis)->iPenDisplayOn=aValue;
	}

void CT_DigitiserDriverData::SetDisplayState(CDataWrapperBase* aThis, TInt aValue)
 /**
 * Store DisplayState
 * @param 		aThis a pointer to the data wrapper base class
 * @param   	aValue  an integer value to use in set 
 * 
 */		
	{
	static_cast<CT_DigitiserDriverData*>(aThis)->iDisplayState=aValue;
	}
	
 void CT_DigitiserDriverData::SetDisplayMode(CDataWrapperBase* aThis, TInt aValue)
 /**
 * Store DisplayMode
 * @param 		aThis a pointer to the data wrapper base class
 * @param   	aValue  an integer value to use in set 
 * 
 */		
 	{
 	static_cast<CT_DigitiserDriverData*>(aThis)->iDisplayMode=aValue;
 	}
	
TBool CT_DigitiserDriverData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand                      The command to process
 * @param aSection                      The section in the ini containing data for the command
 * @param aAsyncErrorIndex              Command index for async calls to return errors to
 *
 * @return                              ETrue if the command is processed
 *
 * @leave                               System wide error
 */
	{
	TBool	ret=ETrue;
	ret=CT_HALData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
	return ret;
	}

