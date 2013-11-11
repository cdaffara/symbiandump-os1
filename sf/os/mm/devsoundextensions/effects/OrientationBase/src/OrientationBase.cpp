/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the Orientation effect base class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <OrientationBase.h>
#include <e32math.h>
#include <math.h>



//360 degrees:
#define TWO_PI 6283
//180 degrees:
#define PI 3142
//90 degrees:
#define QUARTER_PI 1570


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COrientation::COrientation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C COrientation::COrientation()
    : 	iOrientationData(0,0,0,0,0,0,0,0,0),
    	iDataPckgTo(iOrientationData),
    	iDataPckgFrom(iOrientationData)
    {
    }

// Destructor
EXPORT_C COrientation::~COrientation()
    {
    }

// -----------------------------------------------------------------------------
// COrientation::Orientation
// -----------------------------------------------------------------------------
//
EXPORT_C void COrientation::Orientation(
	TInt32& aHeading, TInt32& aPitch, TInt32& aRoll )
	{
	aHeading = iOrientationData.iHeading;
	aPitch = iOrientationData.iPitch;
	aRoll = iOrientationData.iRoll;
	}

// -----------------------------------------------------------------------------
// COrientation::OrientationVectors
// -----------------------------------------------------------------------------
//
EXPORT_C void COrientation::OrientationVectors(
	TInt32& aFrontX, TInt32& aFrontY, TInt32& aFrontZ,
	TInt32& aAboveX, TInt32& aAboveY, TInt32& aAboveZ )
	{
	aFrontX = iOrientationData.iFrontX;
	aFrontY = iOrientationData.iFrontY;
	aFrontZ = iOrientationData.iFrontZ;
	aAboveX = iOrientationData.iAboveX;
	aAboveY = iOrientationData.iAboveY;
	aAboveZ = iOrientationData.iAboveZ;
	
	
	
	
	}

// -----------------------------------------------------------------------------
// COrientation::SetOrientationL
// -----------------------------------------------------------------------------
//
EXPORT_C void COrientation::SetOrientationL(
	TInt32 aHeading, TInt32 aPitch, TInt32 aRoll )
	{

    while(aPitch > PI) 
      {
	  aPitch = aPitch - TWO_PI;
      }
    if(aPitch > QUARTER_PI) 
      {
	  //pitch is here between 90 and 180degs. We swap it back to between 0 and 90degs. Heading needs swaping then too.
	  aPitch = aPitch - (aPitch-QUARTER_PI)*2;
	  aHeading = aHeading + PI;
	  aRoll = aRoll + PI; //I'm not sure if this line is needed. Remove it if you don't get correct results for roll or upVector
      }

    while(aPitch < -PI) 
      {
   	 aPitch = aPitch + TWO_PI;
      }
     
     if(aPitch < -QUARTER_PI) 
     {
	  //pitch is here between -90 and -180degs. We swap it back to between 0 and -90degs. Heading needs swaping then too.
	  aPitch = aPitch + (QUARTER_PI-aPitch)*2;
	  aHeading = aHeading + PI;
	  aRoll = aRoll + PI; //I'm not sure if this line is needed. Remove it if you don't get correct results for roll or upVector
     }

     while (aHeading < 0)
     {
	    aHeading = aHeading + TWO_PI;
	 }
     
     while (aHeading > TWO_PI)
     {
	    aHeading = aHeading - TWO_PI;
     }
     
     while (aRoll < 0)
     {
        aRoll = aRoll + TWO_PI;
     }
     while (aRoll > TWO_PI)
     {
	    aRoll = aRoll - TWO_PI;
     }

    iOrientationData.iHeading = aHeading;
	iOrientationData.iPitch = aPitch;
	iOrientationData.iRoll = aRoll;


    TReal headingSin, headingCos, pitchSin, pitchCos, rollSin, rollCos;
    
    User::LeaveIfError( Math::Sin( headingSin, (TReal)aHeading / 1000 ) );
    User::LeaveIfError( Math::Cos( headingCos, (TReal)aHeading / 1000) );
    User::LeaveIfError( Math::Sin( pitchSin, (TReal)aPitch / 1000) );
    User::LeaveIfError( Math::Cos( pitchCos, (TReal)aPitch / 1000) );
    User::LeaveIfError( Math::Sin( rollSin, (TReal)aRoll / 1000) );
    User::LeaveIfError( Math::Cos( rollCos, (TReal)aRoll / 1000) );   
        		
    iOrientationData.iFrontX = -headingSin * pitchCos * 1000;
    iOrientationData.iFrontY = pitchSin * 1000;
    iOrientationData.iFrontZ = -headingCos * pitchCos * 1000;
    iOrientationData.iAboveX = (-rollSin * headingCos + rollCos * pitchSin * headingSin) * 1000;
    iOrientationData.iAboveY = pitchCos * rollCos * 1000;
    iOrientationData.iAboveZ = (rollSin * headingSin + rollCos * headingCos * pitchSin) * 1000;
	}

// -----------------------------------------------------------------------------
// COrientation::SetOrientationVectorsL
// -----------------------------------------------------------------------------
//
EXPORT_C void COrientation::SetOrientationVectorsL(
	TInt32 aFrontX, TInt32 aFrontY, TInt32 aFrontZ,
	TInt32 aAboveX, TInt32 aAboveY, TInt32 aAboveZ )
	{

    iOrientationData.iFrontX = aFrontX;
	iOrientationData.iFrontY = aFrontY;
	iOrientationData.iFrontZ = aFrontZ;
	iOrientationData.iAboveX = aAboveX;
	iOrientationData.iAboveY = aAboveY;
	iOrientationData.iAboveZ = aAboveZ;
	
	TInt32 previous_valid_value_of_pitch = iOrientationData.iPitch;
	
	
	if(!((aFrontX==0) && (aFrontZ==0))) 
	{
		TReal xDividedByZAtan;
        xDividedByZAtan = atan2 (-aFrontX, -aFrontZ);
        
        if(xDividedByZAtan > 0)
	      iOrientationData.iHeading = (TInt32) (xDividedByZAtan * 1000 + 0.5);
        else
 	      iOrientationData.iHeading = (TInt32) (xDividedByZAtan * 1000 - 0.5);
        
	}
  
    if(aFrontX == 0 && aFrontZ == 0) 
    {
	  if (aFrontY > 0)
	     iOrientationData.iPitch = KPi/2 * 1000 + 0.5; //verify the units!
	  else if (aFrontY == 0)
	     iOrientationData.iPitch = previous_valid_value_of_pitch;
	  else if (aFrontY < 0)
	     iOrientationData.iPitch = -KPi/2 * 1000 - 0.5; //verify the units!
    } 
    else 
    {

      TReal sqrtXZ = 0, squareFx = 0, squareFz = 0;
      Math::Pow(squareFx, aFrontX, 2);
      Math::Pow(squareFz, aFrontZ, 2);
      TReal total = squareFx + squareFz;

      Math::Sqrt (sqrtXZ, total);
      
      TReal yTotalAtan =  atan2 ((TReal)aFrontY, sqrtXZ);
      
      if (yTotalAtan > 0)
   	    iOrientationData.iPitch = (TInt32) (yTotalAtan * 1000 + 0.5);
      else
    	 iOrientationData.iPitch = (TInt32) (yTotalAtan * 1000 - 0.5);
     
     }  
		
    TReal rot_aboveX = 0;
    
    rot_aboveX = cos(-((TReal)iOrientationData.iHeading)/1000) * (((TReal)aAboveX)/1000) + sin(-((TReal)iOrientationData.iHeading)/1000) * (((TReal)aAboveZ)/1000);

    TReal rot_aboveY = 0;
    
    rot_aboveY = sin(-((TReal)iOrientationData.iHeading)/1000) * sin(-((TReal)iOrientationData.iPitch)/1000) * (((TReal)aAboveX)/1000) + cos(-((TReal)iOrientationData.iPitch)/1000) * (((TReal)aAboveY)/1000) - sin(-(TReal)iOrientationData.iPitch/1000) * cos(-((TReal)iOrientationData.iHeading)/1000)* (((TReal)aAboveZ)/1000);

    TReal roll = atan2(-rot_aboveX, rot_aboveY);
   	
   	if (roll > 0)
   	  iOrientationData.iRoll = (TInt32) (roll * 1000 + 0.5);  
	else
   	  iOrientationData.iRoll = (TInt32) (roll * 1000 - 0.5);  
	

     while (iOrientationData.iHeading < 0)
	    iOrientationData.iHeading = iOrientationData.iHeading + TWO_PI;
	    
     while (iOrientationData.iHeading > TWO_PI)
	    iOrientationData.iHeading = iOrientationData.iHeading - TWO_PI;

     while (iOrientationData.iPitch < 0)
	    iOrientationData.iPitch = iOrientationData.iPitch + TWO_PI;
	    
     while (iOrientationData.iPitch > TWO_PI)
	    iOrientationData.iPitch = iOrientationData.iPitch - TWO_PI;
	    
     while (iOrientationData.iRoll < 0)
	    iOrientationData.iRoll = iOrientationData.iRoll + TWO_PI;
     
     while (iOrientationData.iRoll > TWO_PI)
	    iOrientationData.iRoll = iOrientationData.iRoll - TWO_PI;

	}

// -----------------------------------------------------------------------------
// COrientation::DoEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& COrientation::DoEffectData()
	{
#ifdef _DEBUG
    RDebug::Print(_L("COrientation::DoEffectData"));
#endif
	iDataPckgTo = iOrientationData;
	return iDataPckgTo;
	}

// -----------------------------------------------------------------------------
// COrientation::SetEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C void COrientation::SetEffectData(
	const TDesC8& aEffectDataBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("COrientation::SetEffectData"));
#endif
	TEfOrientationDataPckg dataPckg;
	dataPckg.Copy(aEffectDataBuffer);
	iOrientationData = dataPckg();
	iEnabled = iOrientationData.iEnabled;
	iEnforced = iOrientationData.iEnforced;
	iHaveUpdateRights = iOrientationData.iHaveUpdateRights;

	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
