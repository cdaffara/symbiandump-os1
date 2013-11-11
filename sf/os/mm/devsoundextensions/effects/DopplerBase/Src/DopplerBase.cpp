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
* Description:   Implementation of the doppler effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <DopplerBase.h>
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
// CDoppler::CDoppler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CDoppler::CDoppler()
    : 	iDopplerData(),
    	iDataPckgTo(iDopplerData),
    	iDataPckgFrom(iDopplerData)
    {
    }

// Destructor
EXPORT_C CDoppler::~CDoppler()
    {
    }


// -----------------------------------------------------------------------------
// CDoppler::CartesianVelocity
// -----------------------------------------------------------------------------
//

EXPORT_C void CDoppler::CartesianVelocity( TInt32& aX, TInt32& aY, TInt32& aZ )
	{
		aX = iDopplerData.iVelocityX;
		aY = iDopplerData.iVelocityY;
		aZ = iDopplerData.iVelocityZ;
	}

// -----------------------------------------------------------------------------
// CDoppler::Factor
// -----------------------------------------------------------------------------
//

EXPORT_C TUint32 CDoppler::Factor() const
	{
		return iDopplerData.iFactor;
	}

// -----------------------------------------------------------------------------
// CDoppler::FactorMax
// -----------------------------------------------------------------------------
//

EXPORT_C TUint32 CDoppler::FactorMax() const
	{
		return iDopplerData.iMaxFactor;
	}
// -----------------------------------------------------------------------------
// CDoppler::SetCartesianVelocityL
// -----------------------------------------------------------------------------
//

EXPORT_C void CDoppler::SetCartesianVelocityL( TInt32 aX, TInt32 aY, TInt32 aZ )
	{
	    iDopplerData.iVelocityX = aX;
		iDopplerData.iVelocityY = aY;
		iDopplerData.iVelocityZ = aZ;


        TReal SqrtXYZ = 0, squareX = 0, squareY = 0, squareZ = 0;
        Math::Pow(squareX, aX, 2);
        Math::Pow(squareY, aY, 2);
        Math::Pow(squareZ, aZ, 2);
        
        TReal sum = squareX + squareY + squareZ; 
 
        Math::Sqrt(SqrtXYZ, sum);
        
        //Singularity region 
        if(!((aX==0) && (aZ==0)))
        {

          TReal zDividedByXAtan = atan2 (-aX,  -aZ);
 
          if (zDividedByXAtan > 0)
            iDopplerData.iAzimuth = -(TInt32) (zDividedByXAtan * 1000 + 0.5);
          else
            iDopplerData.iAzimuth = -(TInt32) (zDividedByXAtan * 1000 - 0.5);
        }
	    // else { we are exactly on Y-axis and therefore azimuth is undefined; let's use the previous azimuth value instead } 

 
       if (!((aX ==0) && (aY == 0) && (aZ == 0))) 
       {

         TReal result;
         TReal yDividedBySqrtXYZ = aY/SqrtXYZ; 
         User::LeaveIfError(Math::ASin(result, yDividedBySqrtXYZ)); //was ACos
         
         if (result > 0)
            iDopplerData.iElevation = (TInt32) (result * 1000 + 0.5);
          else
            iDopplerData.iElevation = (TInt32) (result * 1000 - 0.5);
          
       }
	   // else { we are exactly in origin and therefore elevation is undefined; let's use the previous elevation value instead } 
      
       iDopplerData.iRadius= (TInt32) (SqrtXYZ + 0.5);

 
      while(iDopplerData.iElevation > PI) 
       { 
	   iDopplerData.iElevation = iDopplerData.iElevation - TWO_PI; 
       }
    
      if(iDopplerData.iElevation > QUARTER_PI) 
       {
	   iDopplerData.iElevation = iDopplerData.iElevation - (iDopplerData.iElevation - QUARTER_PI) * 2;
	   iDopplerData.iAzimuth = iDopplerData.iAzimuth + PI;
       }

      while(iDopplerData.iElevation < -PI) 
       {
	   iDopplerData.iElevation = iDopplerData.iElevation + TWO_PI;
       }
      if(iDopplerData.iElevation < -QUARTER_PI) 
       {
	   iDopplerData.iElevation = iDopplerData.iElevation + (QUARTER_PI - iDopplerData.iElevation) * 2;
	   iDopplerData.iAzimuth = iDopplerData.iAzimuth + PI;
       }
    
      while (iDopplerData.iAzimuth < 0)
	    iDopplerData.iAzimuth = iDopplerData.iAzimuth + TWO_PI;
      while (iDopplerData.iAzimuth > TWO_PI)
	   iDopplerData.iAzimuth = iDopplerData.iAzimuth - TWO_PI;
	   

	}
// -----------------------------------------------------------------------------
// CDoppler::SetFactorL
// -----------------------------------------------------------------------------
//

EXPORT_C void CDoppler::SetFactorL( TUint32 aFactor )
	{
	if ( (aFactor <= iDopplerData.iMaxFactor) )
		{
		iDopplerData.iFactor = aFactor;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}
// -----------------------------------------------------------------------------
// CDoppler::SetSphericalVelocityL
// -----------------------------------------------------------------------------
//

EXPORT_C void CDoppler::SetSphericalVelocityL( TInt32 aAzimuth, TInt32 aElevation, TInt32 aRadius )
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
	   
	
	iDopplerData.iAzimuth = aAzimuth;
	iDopplerData.iElevation = aElevation;
	iDopplerData.iRadius = aRadius;


    TReal elevation = aElevation / 1000.0; // conversion from milliradians to radians because Sin and Cos functions eat radians

    TReal elevationSin;
    TReal elevationCos;
    User::LeaveIfError( Math::Sin( elevationSin, elevation ) );
    User::LeaveIfError( Math::Cos( elevationCos, elevation ) );
    
    TReal azimuthSin;
    TReal azimuthCos;
    User::LeaveIfError( Math::Sin( azimuthSin, aAzimuth / 1000.0) );
    User::LeaveIfError( Math::Cos(azimuthCos, aAzimuth / 1000.0) );


	iDopplerData.iVelocityX = (TInt32)(0.5 + aRadius * elevationCos * azimuthSin);
    iDopplerData.iVelocityY = (TInt32)(0.5 + aRadius * elevationSin);
    iDopplerData.iVelocityZ = (TInt32)(0.5 - aRadius * elevationCos * azimuthCos);		
   }


// -----------------------------------------------------------------------------
// CDoppler::SphericalVelocity
// -----------------------------------------------------------------------------
//

EXPORT_C void CDoppler::SphericalVelocity( TInt32& aAzimuth, TInt32& aElevation, TInt32& aRadius )
	{
		aAzimuth   = iDopplerData.iAzimuth;
		aElevation = iDopplerData.iElevation;
		aRadius    = iDopplerData.iRadius ;

	}


// -----------------------------------------------------------------------------
// CDoppler::DoEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDoppler::DoEffectData()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CDoppler::DoEffectData"));
#endif
	iDataPckgTo = iDopplerData;
	return iDataPckgTo;
	}

// -----------------------------------------------------------------------------
// CDoppler::SetEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C void CDoppler::SetEffectData(
	const TDesC8& aEffectDataBuffer )
	{
#ifdef _DEBUG
    RDebug::Print(_L("CDoppler::SetEffectData"));
#endif
	TEfDopplerDataPckg dataPckg;
	dataPckg.Copy(aEffectDataBuffer);
	iDopplerData = dataPckg();
	iEnabled = iDopplerData.iEnabled;
	iEnforced = iDopplerData.iEnforced;
	iHaveUpdateRights = iDopplerData.iHaveUpdateRights;

	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File

