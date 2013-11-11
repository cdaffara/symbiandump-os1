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


#if (!defined __T_DIGITISER_DRIVER_DATA_H__)
#define __T_DIGITISER_DRIVER_DATA_H__

//	EPOC includes
#include <e32twin.h>


//	User Includes
#include "T_HALData.h"

 
class CT_DigitiserDriverData : public CT_HALData
/**
*	Test Active Notification class
@test
@publishedPartner
@see CT_HALData
*/
	{
public:
	/**
	* Public destructor
	*/
	~CT_DigitiserDriverData();

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_DigitiserDriverData*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	
 		
protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_DigitiserDriverData();
	void	ConstructL();
	static TBool	GetPrepareData(CDataWrapperBase* aThis, const TDesC& aSection, TInt& aValueStart, TInt& /*aValueEnd*/);
	
	static TBool	GetPrepareDummy(CDataWrapperBase* /*aThis*/, const TDesC& /*aSection*/, TInt& /*aValueStart*/, TInt& /*aValueEnd*/);
   	static void 	GetValidation(CDataWrapperBase* aThis, const TDesC& aSection, TInt aValue, TBool aForceValidation);
   	//for the touch screen device
 	static void 	SetPen(CDataWrapperBase* aThis, TInt aValue);
 	static void 	SetPenX(CDataWrapperBase* aThis, TInt aValue);
 	static void 	SetPenY(CDataWrapperBase* aThis, TInt aValue);
 	static void 	SetPenState(CDataWrapperBase* aThis, TInt aValue);
 	
	//for the sound driver device
	static void 	SetPenClick(CDataWrapperBase* aThis, TInt aValue);					
	static void 	SetPenClickVolumeMax(CDataWrapperBase* aThis, TInt aValue);				
	static void 	SetPenClickState(CDataWrapperBase* aThis, TInt aValue);					
	static void 	SetPenClickVolume(CDataWrapperBase* aThis, TInt aValue);
	
	//for the power controller					
	static void 	SetPenDisplayOn(CDataWrapperBase* aThis, TInt aValue);
	
	//for the LCD screen
	static void 	SetDisplayState(CDataWrapperBase* aThis, TInt aValue);
	static void 	SetDisplayMode(CDataWrapperBase* aThis, TInt aValue);
	
private:


protected:
	TInt 		iPenState;
	TInt 		iPen;
	TInt 		iPenX;
	TInt 		iPenY;
	TInt        iPenClick;
	TInt        iPenClickVolumeMax;
	TInt		iPenClickState;
	TInt 		iPenClickVolume;
	TInt        iPenDisplayOn;
	TInt 		iDisplayState;
	TInt 		iDisplayMode;

private:
	static const THalTableLookup	iTableLookup[];
	};
 
 #endif /* __T_DIGITISER_DRIVER_DATA_H__ */
 
