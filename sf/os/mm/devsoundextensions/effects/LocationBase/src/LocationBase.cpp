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
* Description:   Implementation of the Location effect base class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <LocationBase.h>
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
// CLocation::CLocation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CLocation::CLocation()
    : 	iLocationData(0,0,0,0,0,0),
    	iDataPckgTo(iLocationData),
    	iDataPckgFrom(iLocationData)
    {
    }

// Destructor
EXPORT_C CLocation::~CLocation()
    {
    }

// -----------------------------------------------------------------------------
// CLocation::LocationCartesian
// -----------------------------------------------------------------------------
//
EXPORT_C void CLocation::LocationCartesian(
	TInt32& aX, TInt32& aY, TInt32& aZ )
	{
	aX = iLocationData.iXCoordinate;
	aY = iLocationData.iYCoordinate;
	aZ = iLocationData.iZCoordinate;
	}

// -----------------------------------------------------------------------------
// CLocation::LocationSpherical
// -----------------------------------------------------------------------------
//
EXPORT_C void CLocation::LocationSpherical(
	TInt32& aAzimuth, TInt32& aElevation, TInt32& aRadius  )
	{
	aAzimuth = iLocationData.iAzimuth;
	aElevation = iLocationData.iElevation;
	aRadius = iLocationData.iRadius;
	}

// -----------------------------------------------------------------------------
// CLocation::SetLocationCartesianL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLocation::SetLocationCartesianL(
	TInt32& aX, TInt32& aY, TInt32& aZ )
	{
	iLocationData.iXCoordinate = aX;
	iLocationData.iYCoordinate = aY;
	iLocationData.iZCoordinate = aZ;


    TReal SqrtXYZ = 0, squareX = 0, squareY = 0, squareZ = 0;
    Math::Pow(squareX, aX, 2);
    Math::Pow(squareY, aY, 2);
    Math::Pow(squareZ, aZ, 2);
    TReal sum = squareX + squareY + squareZ; 
    Math::Sqrt(SqrtXYZ, sum);
        
  
    //Singularity region 

       if(!((aX==0) && (aZ==0))) //was aY
       {

            TReal zDividedByXAtan = atan2 ((TReal)-aX, (TReal)-aZ);
            
            if (zDividedByXAtan > 0)
	          iLocationData.iAzimuth = - (TInt32)(zDividedByXAtan * 1000 + 0.5); // minus sign added (shouldn't affect the result since it is compensated three rows above, but to be sure...)
            else
 	          iLocationData.iAzimuth = - (TInt32)(zDividedByXAtan * 1000 - 0.5); // minus sign added (shouldn't affect the result since it is compensated three rows above, but to be sure...)
             
       }
	// else { we are exactly on Y-axis and therefore azimuth is undefined; let's use the previous azimuth value instead } 

 
       if (!((aX ==0) && (aY == 0) && (aZ == 0))) 
       {

         TReal result;
         TReal yDividedBySqrtXYZ = aY/SqrtXYZ; 
         User::LeaveIfError(Math::ASin(result, yDividedBySqrtXYZ)); //was ACos

         if (result > 0)
           iLocationData.iElevation = (TInt32) (result * 1000 + 0.5);
         else
           iLocationData.iElevation = (TInt32) (result * 1000 - 0.5);
           
       }
	   // else { we are exactly in origin and therefore elevation is undefined; let's use the previous elevation value instead } 
      
       iLocationData.iRadius= (TInt32) (SqrtXYZ + 0.5);
  
  
      while(iLocationData.iElevation > PI) 
       { 
	   iLocationData.iElevation = iLocationData.iElevation - TWO_PI; 
       }
    
      if(iLocationData.iElevation > QUARTER_PI) 
       {
	   iLocationData.iElevation = iLocationData.iElevation - (iLocationData.iElevation - QUARTER_PI) * 2;
	   iLocationData.iAzimuth = iLocationData.iAzimuth + PI;
       }

      while(iLocationData.iElevation < -PI) 
       {
	   iLocationData.iElevation = iLocationData.iElevation + TWO_PI;
       }
      if(iLocationData.iElevation < -QUARTER_PI) 
       {
	   iLocationData.iElevation = iLocationData.iElevation + (QUARTER_PI - iLocationData.iElevation) * 2;
	   iLocationData.iAzimuth = iLocationData.iAzimuth + PI;
       }
    
      while (iLocationData.iAzimuth < 0)
	    iLocationData.iAzimuth = iLocationData.iAzimuth + TWO_PI;
      while (iLocationData.iAzimuth > TWO_PI)
	   iLocationData.iAzimuth = iLocationData.iAzimuth - TWO_PI;
	   
	}

// -----------------------------------------------------------------------------
// CLocation::SetLocationSphericalL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLocation::SetLocationSphericalL(
	TInt32& aAzimuth, TInt32& aElevation, TInt32& aRadius  )
	{
	
     while(aElevation > PI) 
      { 
         aElevation = aElevation - TWO_PI; 
      }
    
     if(aElevation > QUARTER_PI) 
       {
	   aElevation = aElevation - (aElevation - QUARTER_PI) * 2;
	   aAzimuth = aAzimuth + PI;
       }

     while(aElevation < -PI) 
       {
	   aElevation = aElevation + TWO_PI;
       }
     if(aElevation < -QUARTER_PI) 
       {
	   aElevation = aElevation + (QUARTER_PI - aElevation) * 2;
	   aAzimuth = aAzimuth + PI;
       }
   
     while (aAzimuth < 0)
       aAzimuth = aAzimuth + TWO_PI;
     while (aAzimuth > TWO_PI)
	   aAzimuth = aAzimuth - TWO_PI;

	
   	iLocationData.iAzimuth = aAzimuth;
	iLocationData.iElevation = aElevation;
	iLocationData.iRadius = aRadius;


    TReal elevation = aElevation / 1000.0; // conversion from milliradians to radians because Sin and Cos functions eat radians
    
    TReal elevationSin;
    TReal elevationCos;
    User::LeaveIfError( Math::Sin( elevationSin, elevation ) );
    User::LeaveIfError( Math::Cos( elevationCos, elevation ) );
    
    TReal azimuthSin;
    TReal azimuthCos;
    User::LeaveIfError( Math::Sin( azimuthSin, aAzimuth / 1000.0) );
    User::LeaveIfError( Math::Cos( azimuthCos, aAzimuth / 1000.0) );


	iLocationData.iXCoordinate = (TInt32)(0.5 + aRadius * elevationCos * azimuthSin);
    iLocationData.iYCoordinate = (TInt32)(0.5 + aRadius * elevationSin);
    iLocationData.iZCoordinate = (TInt32)(0.5 - aRadius * elevationCos * azimuthCos);		
	
	}

// -----------------------------------------------------------------------------
// CLocation::DoEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CLocation::DoEffectData()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CLocation::DoEffectData"));
#endif
	iDataPckgTo = iLocationData;
	return iDataPckgTo;
	}

// -----------------------------------------------------------------------------
// CLocation::SetEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C void CLocation::SetEffectData(
	const TDesC8& aEffectDataBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CLocation::SetEffectData"));
#endif
	TEfLocationDataPckg dataPckg;
	dataPckg.Copy(aEffectDataBuffer);
	iLocationData = dataPckg();
	iEnabled = iLocationData.iEnabled;
	iEnforced = iLocationData.iEnforced;
	iHaveUpdateRights = iLocationData.iHaveUpdateRights;

	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
