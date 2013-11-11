/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Provides the interface between SUPL Framework and POS
*				 Message plugin. 
*
*/




#include <badesca.h>

#include "epos_comasuplposition.h"
#include "epos_comasuplvelocity.h"
#include "epos_eomasuplposerrors.h"


// Constant Definitions
const TInt KMINAltitude = 0;
const TInt KMAXAltitude = 32767;
const TInt KMINAltitudeUncert = 0;
const TInt KMAXAltitudeUncert = 127;

const TInt KMINUncertSemiMajor = 0;
const TInt KMAXUncertSemiMajor = 127;
const TInt KMINUncertSemiMinor = 0;
const TInt KMAXUncertSemiMinor = 127;
const TInt KMINOrientationMajorAxis = 0;
const TInt KMAXOrientationMajorAxis = 180;

const TInt KMINConfidence = 0;
const TInt KMAXConfidence = 100;

const TInt KMINLatitude = 0;
const TInt KMAXLatitude = 8388607;

const TInt KMINLongitude = -8388608;
const TInt KMAXLongitude = 8388607;

const TInt KMINZoneCode = 0;
const TInt KMAXZoneCode = 1;

// ============================ MEMBER FUNCTIONS of TOMASuplAltitudeInfo===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TOMASuplAltitudeInfo::TOMASuplAltitudeInfo()
    {
    }



//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
TInt TOMASuplAltitudeInfo::SetAltitudeInfo(const TOMASuplAltitudeInfo& aAltitudeInfo)
    {
    // Check for parameter validity before setting the value
	if(aAltitudeInfo.iAltitude < KMINAltitude || aAltitudeInfo.iAltitude > KMAXAltitude)
	{
		return KErrArgument;
	}
	
	if(aAltitudeInfo.iAltitudeUncertainty < KMINAltitudeUncert || aAltitudeInfo.iAltitudeUncertainty > KMAXAltitudeUncert)
	{
		return KErrArgument;
	}
	
	iAltitudeDirection   = aAltitudeInfo.iAltitudeDirection;
	iAltitude            = aAltitudeInfo.iAltitude;
	iAltitudeUncertainty = aAltitudeInfo.iAltitudeUncertainty;
	
	return KErrNone;
    }           

//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  void TOMASuplAltitudeInfo::GetAltitudeInfo(TOMASuplAltitudeDirection& aAltitudeDirection, 
											       TInt& aAltitude,TInt& aAltUncertainty) const
    {
	aAltitudeDirection = iAltitudeDirection;
	aAltitude 		   = iAltitude;  
	aAltUncertainty	   = iAltitudeUncertainty ;
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  TInt TOMASuplAltitudeInfo::SetAltitudeInfo(TOMASuplAltitudeDirection aAltitudeDirection,
													   TInt aAltitude,TInt aAltUncertainty)
    {
    // Check for parameter validity before setting the value
	if(aAltitude < KMINAltitude || aAltitude > KMAXAltitude)
	{
		return KErrArgument;
	}
	
	if(aAltUncertainty < KMINAltitudeUncert || aAltUncertainty > KMAXAltitudeUncert)
	{
		return KErrArgument;
	}
	
	iAltitudeDirection   = aAltitudeDirection;
	iAltitude            = aAltitude;
	iAltitudeUncertainty = aAltUncertainty;
	
	return KErrNone;
    }           


// ============================ MEMBER FUNCTIONS of TOMASuplUncertainty===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TOMASuplUncertainty::TOMASuplUncertainty()
    {
    }


//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TOMASuplUncertainty::SetUncertainty(const TOMASuplUncertainty &aUncertainty)
    {
    if(aUncertainty.iUncertaintySemiMajor < KMINUncertSemiMajor || aUncertainty.iUncertaintySemiMajor > KMAXUncertSemiMajor)
    {
    	return KErrArgument;
    }
    
    if(aUncertainty.iUncertaintySemiMinor < KMINUncertSemiMajor || aUncertainty.iUncertaintySemiMinor > KMAXUncertSemiMajor)
    {
    	return KErrArgument;
    }
    
    if(aUncertainty.iOrientationMajorAxis < KMINOrientationMajorAxis || aUncertainty.iOrientationMajorAxis > KMAXOrientationMajorAxis)
    {
    	return KErrArgument;
    }
    
    iUncertaintySemiMajor = aUncertainty.iUncertaintySemiMajor;
    iUncertaintySemiMinor = aUncertainty.iUncertaintySemiMinor;
    iOrientationMajorAxis = aUncertainty.iOrientationMajorAxis;
    
    return KErrNone;
    }

//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  void TOMASuplUncertainty::GetUncertainty(TInt& aUncertaintySemiMajor,
												 TInt& aUncertaintySemiMinor,
												 TInt& aOrientationMajorAxis)const
    {
	aUncertaintySemiMajor =  iUncertaintySemiMajor;
    aUncertaintySemiMinor =  iUncertaintySemiMinor;           
    aOrientationMajorAxis =  iOrientationMajorAxis; 
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  TInt TOMASuplUncertainty::SetUncertainty(TInt aUncertaintySemiMajor,
													TInt aUncertaintySemiMinor,
													TInt aOrientationMajorAxis)
    {
    // Check the parameter range before setting the values
    if(aUncertaintySemiMajor < KMINUncertSemiMajor || aUncertaintySemiMajor > KMAXUncertSemiMajor)
    {
    	return KErrArgument;
    }
    
    if(aUncertaintySemiMinor < KMINUncertSemiMinor || aUncertaintySemiMinor > KMAXUncertSemiMinor)
    {
    	return KErrArgument;
    }
    
    if(aOrientationMajorAxis < KMINOrientationMajorAxis || aOrientationMajorAxis > KMAXOrientationMajorAxis)
    {
    	return KErrArgument;	
    }
    
	iUncertaintySemiMajor =  aUncertaintySemiMajor;
    iUncertaintySemiMinor =  aUncertaintySemiMinor;           
    iOrientationMajorAxis =  aOrientationMajorAxis ; 
    
    return KErrNone;
    
    }
// ============================ MEMBER FUNCTIONS of TOMASuplPositionEstimate===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TOMASuplPositionEstimate::TOMASuplPositionEstimate()
    {
    iOptionalMask = 0;
    }

//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  TInt TOMASuplPositionEstimate::GetUncertainty(TOMASuplUncertainty& aUncertainty)const
    {
    if(iOptionalMask & (1<<(3)))
    	{
		aUncertainty =  iUncertainty;    		
		return KErrNone;
    	}
    else
    	{
    	return KErrOMASuplParamNotSet;
    	}

	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  TInt TOMASuplPositionEstimate::SetUncertainty(const TOMASuplUncertainty& aUncertainty)
    {
    TInt err;
    //iOptionalMask &= 0xFFFB;
    iOptionalMask &= 0xFFF7;
	if(KErrNone == (err = (iUncertainty.SetUncertainty(aUncertainty))))
	{
		iOptionalMask |= LEFTSHIFT(3);	
	}
	
	return err;
    }
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  TInt TOMASuplPositionEstimate::GetConfidence(TInt& aConfidence)const
    {
    if(iOptionalMask & (1<<(4)))
    //if(iOptionalMask & (LEFTSHIFT(4)))
    	{
    	aConfidence =  iConfidence;
    	return KErrNone;		
    	}
    else
    	{
    	return KErrOMASuplParamNotSet;	
    	}
	
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  TInt TOMASuplPositionEstimate::SetConfidence(TInt aConfidence)
    {
    //iOptionalMask &= 0xFFF7;
    iOptionalMask &= 0xFFEF;
    if(aConfidence < KMINConfidence || aConfidence > KMAXConfidence)
    {
    	//iOptionalMask &= 0xFFF7;
    	return KErrArgument;
    }
    
	iConfidence = aConfidence;
	iOptionalMask |= LEFTSHIFT(4);
	
	return KErrNone;
    }
//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  TInt TOMASuplPositionEstimate::GetAltitudeInfo(TOMASuplAltitudeInfo& aAltitudeInfo)const
    {
    if(iOptionalMask & (1<<(5)))
    	{
    	aAltitudeInfo = iAltitudeInfo;
    	return KErrNone;
    	}
    else
    	{
    	return KErrOMASuplParamNotSet;		
    	}
	
    }
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  TInt TOMASuplPositionEstimate::SetAltitudeInfo(const TOMASuplAltitudeInfo& aAltitudeInfo)
    {
    TInt err = KErrNone;
    //iOptionalMask &= 0xFFEF;
    iOptionalMask &= 0xFFBF;
	if(KErrNone == (err = iAltitudeInfo.SetAltitudeInfo(aAltitudeInfo)))
	{
		iOptionalMask |= LEFTSHIFT(5);	
	}
	
	return err;
	}           
//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  void TOMASuplPositionEstimate::GetPositionEstimate(TOMASuplLatitudeSign& aLatSign,
														   TInt& aLatitude,TInt& aLongitude)const
    {
	aLatSign   = iLatitudeSign;
	aLatitude  = iLatitude;
	aLongitude = iLongitude;
    }
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  TInt TOMASuplPositionEstimate::SetPositionEstimate(TOMASuplLatitudeSign aLatSign,
															  TInt aLatitude,TInt aLongitude)
    {
    if(aLatitude < KMINLatitude || aLatitude > KMAXLatitude)
    {
    	return KErrArgument;
    }
    
    if(aLongitude < KMINLongitude || aLongitude > KMAXLongitude)
    {
    	return KErrArgument;	
    }
    
	iLatitudeSign = aLatSign;
	iLatitude     = aLatitude;
    iLongitude    = aLongitude;
    
    return KErrNone;
    }

// ============================ MEMBER FUNCTIONS of TOMASuplUncertainty===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TOMASuplUtcTime::TOMASuplUtcTime()
    {
    }

//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  void TOMASuplUtcTime::GetUtcTime(TDateTime& aTimeStamp,
										       TInt& aZoneCode,TInt& aZone)const
    {
	aTimeStamp =  iTimeStamp;
    aZoneCode  =  iZoneCode;           
    aZone 	   =  iZone; 
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  TInt TOMASuplUtcTime::SetUtcTime(const TDateTime& aTimeStamp,
										       TInt aZoneCode,TInt aZone)
    {
    if(aZoneCode < KMINZoneCode || aZoneCode > KMAXZoneCode)
    {
    	return KErrArgument;
    }
    
	iTimeStamp =  aTimeStamp;
    iZoneCode  =  aZoneCode;           
    iZone 	   =  aZone; 
    
    return KErrNone;
    } 
// ============================ MEMBER FUNCTIONS of COMASuplPosition===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
COMASuplPosition::COMASuplPosition()
    {
    	SetType(COMASuplInfoRequest::EOMASuplPosition);
    }

//
// ---------------------------------------------------------
//  destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplPosition::~COMASuplPosition()
    {
    delete iVelocity;
    
    }
    
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  COMASuplPosition* COMASuplPosition::NewL()
    {
    COMASuplPosition* self = new(ELeave) COMASuplPosition;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}           

//
// -------------------------------------------------------------
// Two Phase constructor
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
void COMASuplPosition::ConstructL()
    {
    iVelocityPresent = EFalse;
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  void COMASuplPosition::GetPosition(TOMASuplUtcTime& aUtcTime,
										   TOMASuplPositionEstimate& aPosEstimate)const
    {
	aUtcTime 	 = iUtcTime;
	aPosEstimate = iPosEstimate;
    }           
//
// -------------------------------------------------------------
// Sets the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//

EXPORT_C void COMASuplPosition::SetPosition(const TOMASuplUtcTime& aUtcTime,
											 	const TOMASuplPositionEstimate& aPosEstimate)
    {
    iUtcTime 	 = aUtcTime;
    iPosEstimate = aPosEstimate;
    }

//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  TInt COMASuplPosition::GetVelocity(COMASuplVelocity*& aSuplVelocity)const
    {
    if(iVelocityPresent)
    	{
    	aSuplVelocity = iVelocity;
    	return KErrNone;	
    	}
    else
    	{
    	return KErrOMASuplParamNotSet;
    	}
    
    }           
//
// -------------------------------------------------------------
// Sets the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//

EXPORT_C void COMASuplPosition::SetSuplVelocity(COMASuplVelocity* aSuplVelocity)
    {
    
    delete iVelocity;
    iVelocity = NULL;

    iVelocity = aSuplVelocity;
    if(iVelocity)
    	{
    	iVelocityPresent = ETrue;		
    	}
    else
    	{
    	iVelocityPresent = EFalse;	
    	}    
    }       

// -----------------------------------------------------------------------------
// COMASuplPosition::ClonePosition() 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
     
EXPORT_C COMASuplInfoRequest* COMASuplPosition::CloneL()const
	{
	
	COMASuplPosition* clonePosition  = COMASuplPosition::NewL();
	CleanupStack::PushL(clonePosition);
	
	clonePosition->iUtcTime 	   = iUtcTime;
	clonePosition->iPosEstimate = iPosEstimate;
		
			
	if(iVelocityPresent)
		{
		COMASuplVelocity* cloneVelocity = static_cast <COMASuplVelocity *>(iVelocity->CloneL());
		clonePosition->SetSuplVelocity(cloneVelocity);
  		}
  		
    clonePosition->SetStatus(Status());
    clonePosition->SetType(Type());
  	CleanupStack::Pop(clonePosition);
  	return (clonePosition);
	}
