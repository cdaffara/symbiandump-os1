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
* Description:  Defines the data types related to OMA SUPL
*				 Position
*
*/


#ifndef C_COMASUPLPOSITION_H
#define C_COMASUPLPOSITION_H

// INCLUDES
#include <e32def.h>
#include <e32std.h>


#include <lbs/epos_comasuplinforequestlist.h>


#define LEFTSHIFT(i)        (1<<(i));

// CLASS DECLARATION
class COMASuplVelocity;

/**
* TOMASuplAltitudeInfo holds values for 
* - Altitude, which provides altitude information in meters
* - Altitude Direction , which indicates height or depth above or below the WGS84 ellipsoid in meters
* - Altitude Uncertainty, which contains the altitude uncertainty code in meters, for the correspondence between the latitude/longitude uncertainty code.
*
*  @since Series 60 3.1u
*/
class TOMASuplAltitudeInfo 
    {
  	public:
  	
  	//Default Constructor
  	IMPORT_C TOMASuplAltitudeInfo();
  	
  	public:
  	/**
     * @Description Enumeration for Altitude Direction
     *
     */
  	enum TOMASuplAltitudeDirection
  	{
	 	EHeight = 0,
		EDepth = 1
	};
	
    public: // New functions
        
    /**
    * @Description GetAltitudeInfo retrives values for the different altitude parameters
    * @param aAltitudeDirection A reference parameter that holds Altitude Direction
    * @param aAltitude A reference parameter that holds Altitude value in meters
    * @param aAltUncertainty A reference parameter that holds Altitude Uncertainty in meters
    */
    
    IMPORT_C void GetAltitudeInfo(TOMASuplAltitudeDirection& aAltitudeDirection,
    							  TInt& aAltitude,TInt& aAltUncertainty) const;
    
    /**
    * @Description SuplSetAltitudeInfo sets values for different altitude parameters
    * @param aAltitudeDirection Parameter that takes the Altitude Direction, which indicates the height or depth, above or below the WGS84 ellipsoid. 
    * Valid values are
    * - Height
    * - Depth
    * @param aAltitude Parameter that takes Altitude in meters. Valid range is 0 to 32767
    * @param aAltUncertainty Parameter that takes Altitude Uncertainty in meters. 
    * Valid range is 0 to 127.
    * @return KErrArgument if any of the parameters if not within the specified range.
    * KErrNone otherwise.
    */
    
    IMPORT_C TInt SetAltitudeInfo(TOMASuplAltitudeDirection aAltitudeDirection,
    							  TInt aAltitude,TInt aAltUncertainty);
     
    /**
    * @Description Internal method not to be used
    */

	TInt SetAltitudeInfo(const TOMASuplAltitudeInfo& aAltitudeInfo);
	
   private:    // Data
   
     //Altitude Direction
     TOMASuplAltitudeDirection iAltitudeDirection;
     //Altitude
     TInt iAltitude;
     // Uncertainity in altitude 
     TInt iAltitudeUncertainty;
     
    };

/**
* TOMASuplUncertainty holds latitude/longitude uncertainty code associated with the major axis and minor axis in meters.
* TOMASuplUncertainty also holds the orientation in degrees of the major axis with respect to North.
*
*  @since Series 60 3.1u
*/
class TOMASuplUncertainty 
    {
  	public:
  	 
  	// Default Connstructor
  	IMPORT_C TOMASuplUncertainty();
  	
    public: // New functions
        
	/**
	* @Description GetUncertainty retrives values for different Uncertainty parameters
	* @param aUncertaintySemiMajor A reference parameter that holds the semi major axis uncertainty in meters.
	* @param aUncertaintySemiMinor A reference parameter that holds the semi minor axis uncertainty in meters.
	* @param aOrientationMajorAxis A reference parameter that holds the major Axis orientation in angles with respect to North.
	*/
        
    IMPORT_C void GetUncertainty(TInt& aUncertaintySemiMajor,
        						 TInt& aUncertaintySemiMinor,
        						 TInt& aOrientationMajorAxis)const;
               
   /**
    * @Description SetUncertainty sets values for different uncertainty parameters
    * @param aUncertaintySemiMajor Parameter for semi major axis uncertainty in meters. 
    * Valid range is 0 to 127
    * @param aUncertaintySemiMinor Parameter for semi minor axis uncertainity in meters. 
    * Valid range is 0 to 127
    * @param aOrientationMajorAxis Parameter for major axis orientation in degrees with respect to North. 
    * Valid range is 0 to 180.
    * @return KErrArgument if any of the parameters is not within the specified range.
    * KErrNone otherwise.
    */

    IMPORT_C TInt SetUncertainty(TInt aUncertaintySemiMajor,
        						 TInt aUncertaintySemiMinor,
        						 TInt aOrientationMajorAxis);
        						 
	/**
	* @Description An Internal method not be used
	*
	*/
    TInt SetUncertainty(const TOMASuplUncertainty &aUncertainty);

    
    private:    // Data
    
	      TInt iUncertaintySemiMajor;
	      
	      TInt iUncertaintySemiMinor;
	      
	      TInt iOrientationMajorAxis;
    };
    

/**
* @Description TOMASuplPositionEstimate holds values for 
* - Latitude Sign, which indicates North or South
* - Latitude, which is an integer in the range, 0 to 8388608
* - Longitude, which is an integer in the range -8388608 to 8388608
* - Uncertainty, which contains the latitude/longitude uncertainty code and also the major axis 
* 	orientation
* - Confidence, which represents the confidence by which the position is known to be within 
* 	the shape description. This is expressed as a percentage and the valid range is 0 to 100
* - AltitudeInfo, which contains altitude direction, altitude and optionally velocity
*
*  @since Series 60 3.1u
*/
class TOMASuplPositionEstimate 
    {
    public:
    // Default constructor
    IMPORT_C TOMASuplPositionEstimate();
	
	public:
		/**
		* @Description Enumeration for Altitude Direction
		*
		*/
     enum TOMASuplLatitudeSign
		{
		ENorth = 0,
		ESouth = 1
		};	
    public: // New functions
        
		/**
		* @Description GetUncertainty retrives value for Uncertainty 
		* @param aUncertainty A reference parameter that holds Uncertainty
		* @return KErrOMASuplParamNotSet if Uncertainty parameter is not set.
		* @return KErrNone if Uncertainty parameter is set
		*/
        
	    IMPORT_C TInt GetUncertainty(TOMASuplUncertainty& aUncertainty)const;
        
	   /**
	    * @Description SetUncertainty sets values for Uncertainty
	    * @param aUncertainty Parameter of type TOMASuplUncertainty 
	    * that takes Uncertainty 
	    * @return KErrArgument if any of the parameters of aUncertainty is
	    * not within the specified range. KErrNone otherwise.
	    */        
	    IMPORT_C TInt SetUncertainty(const TOMASuplUncertainty& aUncertainty);
         
         
		/**
		* @Description GetConfidence retrives value for Confidence 
		* @param aConfidence A reference parameter that holds Confidence, which represents the confidence 
		* by which the position is known to be within the shape description. 
		* This is expressed as a percentage and the valid range is 0 to 100
		* @return KErrOMASuplParamNotSet if Confidence parameter is not set.
		* @return KErrNone if confidence is set.
		*/
                
		IMPORT_C TInt GetConfidence(TInt& aConfidence)const;
        
		/**
		* @Description SetConfidence sets values for Confidence
		* @param aConfidence Parameter which represents the confidence 
		* by which the position is known to be within the shape description. 
		* This is expressed as a percentage and the valid range is 0 to 100.
		* @return KErrArgument if the parameter is not within the specified range.
		* KErrNone otherwise.
		*/          
        IMPORT_C TInt SetConfidence(TInt aConfidence);
  		
  		
		/**
		* @Description GetAltitudeInfo retrives value for AltitudeInfo 
		* @param aAltitudeInfo A reference parameter that holds Altitude Information, which contains 
		* altitude direction, altitude and altitude uncertainty
		* @return KErrOMASuplParamNotSet if Altitude parameter is not set.
		* @return KErrNone otherwise
		*/        
        IMPORT_C TInt GetAltitudeInfo(TOMASuplAltitudeInfo& aAltitudeInfo) const;
        
		/**
		* @Description SetAltitudeInfo sets values for Altitude Information, which contains 
		* altitude direction, altitude and altitude uncertainty
		* @param aAltitudeInfo Parameter of type, TOMASuplAltitudeInfo
		* @return KErrArgument is any of the parameters of aAltitudeInfo is not
		* withinh the specified range. KErrNone otherwise.
		*/          
        IMPORT_C TInt SetAltitudeInfo(const TOMASuplAltitudeInfo& aAltitudeInfo);
                
	   /**
	    * @Description GetPositionEstimate different parameters of Position Estimate
	    * @param aLatSign A reference Parameter of type TOMASuplLatitudeSign, 
	    * which indicates North or South
	    * @param aLatitude A refernce parameter that holds Latitude, which is 
	    * an integer in the range 0 to 8388607
	    * @param aLongitude A reference parameter that holds Longitude, 
	    * which is an integer in the range -8388607 to 8388607
	    */     
	    IMPORT_C void GetPositionEstimate(TOMASuplLatitudeSign& aLatSign,
        								  TInt& aLatitude,TInt& aLongitude) const;
         
         
	   /**
	    * @Description SetPositionEstimate sets values for different position estimate 
	    * parameters
	    * @param aLatSign Parameter of type TOMASuplLatitudeSign, 
	    * which indicates North or South
	    * @param aLatitude Parameter that takes Latitude. Valid range is
	    * 0 to 8388607
	    * @param aLongitude Parameter that takes Longitude. Valid 
	    * range is -8388607 to 8388607.
	    * @return KErrArgument if any of the parameters is not within the
	    * specified range. KErrNone otherwise.
	    */     
        
     IMPORT_C TInt SetPositionEstimate(TOMASuplLatitudeSign aLatSign,
        								  TInt aLatitude,TInt aLongitude); 

    
    private:    // Data
    
	      //Latitude Sign 
	      TOMASuplLatitudeSign iLatitudeSign;
	      //Latitude
	      TInt  iLatitude;
	      //Longitude
	      TInt iLongitude;
	      //Uncertainity 
	      TOMASuplUncertainty iUncertainty;
	      //Confidence
	      TInt iConfidence;
	      //AltitudeInfo
	      TOMASuplAltitudeInfo iAltitudeInfo;
	      //For setting and unsetting the optional params
	      TInt iOptionalMask;
	      
    };

/**
* TOMASuplUtcTime  holds values for 
* - Time Stamp, which is the timp stamp of the position. Time Stamp provides the Greenwich Mean Time at 
* which position was calculated
* - Zone and 
* - Zone Code which provides offset from UTC
*
*  @since Series 60 3.1u
*/
class TOMASuplUtcTime 
    {
  	public:
  	 
  	// Default Connstructor
  	IMPORT_C TOMASuplUtcTime();
  	
    public: // New functions
        
	   /**
	    * @Description GetUtcTime retrieves values for different UTC parameters
	    * @param aTimeStamp A reference Parameter of type TDateTime
	    * @param aZoneCode A refernce parameter that holds Zone Code
	    * @param aZone A reference parameter that holds zone, which is the offset with respect to 
	    * UTC
	    */     
        
      IMPORT_C void GetUtcTime(TDateTime& aTimeStamp,TInt& aZoneCode,TInt& aZone)const;
               
	   /**
	    * @Description SetUtcTime sets values for different UTC parameters
	    * @param aTimeStamp Parameter of type TDateTime
	    * @param aZoneCode Parameter that takes Zone Code. Valid values for ZoneCode are
	    * - 0  and
	    * - 1
	    * @param aZone Parameter that takes Zone. Zone is the offset with respect to
	    * UTC. Any valid Integer is accepted for this parameter
	    * @return KErrArgument if any of the parameters is not within the range.
	    * KErrNone otherwise.
	    */     
      IMPORT_C TInt SetUtcTime(const TDateTime& aTimeStamp,TInt aZoneCode,TInt aZone);
        
    private:    // Data

      TDateTime iTimeStamp;
      
      TInt iZoneCode;
      
      TInt iZone;
    };
        
/**
* COMASuplPosition describes the position of the terminal. It holds the following values
* - Time when Position fix was calculated
* - Position Estimate and 
* - Optionally Velocity
*  
*
*  @since Series 60 3.1u
*/

class COMASuplPosition : public COMASuplInfoRequest
    {
    public:  // Constructors and destructor
        
    	/**
        * @Description Creates a new instance of a COMASuplPosition and sets the 
        * type in COMASuplInfoRequest to EOMASuplPosition
        *
        * @return A pointer to COMASuplPosition
        * It leaves with KErrNoMemory if object creation fails        
        */
        IMPORT_C static COMASuplPosition* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~COMASuplPosition();
        
    public: // New functions
        
		/**
		* @Description GetPosition retrieves values for
		* - Time when the position was calculated and 
		* - Position Estimate
		* @param aUtcTime A reference Parameter of type TOMASuplUtcTime
		* @param aPosEstimate A refernce parameter of type TOMASuplPositionEstimate
		*/     

        IMPORT_C void GetPosition (TOMASuplUtcTime& aUtcTime,
        						       TOMASuplPositionEstimate& aPosEstimate)const;
        
		/**
		* @Description SetPosition sets values for
		* - Time when the position was calculated and 
		* - Position Estimate
		* @param aUtcTime Parameter of type TOMASuplUtcTime
		* aPosEstimate Parameter of type TOMASuplPositionEstimate
		* @return KErrArgument if any of the parameters is not within the
		* specified range. KErrNone otherwise.
		*/     
        
      IMPORT_C void SetPosition(const TOMASuplUtcTime& aUtcTime,
        							  const TOMASuplPositionEstimate& aPosEstimate);
         
	   /**
	    * @Description GetVelocity retrieves values for velocity
	    * @param aSuplVelocity A reference Parameter of type COMASuplVelocity*
	    * @return KErrOMASuplParamNotSet if the Velocity is not present.
	    * KErrNone otherwise
	    */     
                        
        IMPORT_C TInt GetVelocity(COMASuplVelocity*& aSuplVelocity)const; 
        
	   /**
	    * @Description SetSuplVelocity sets values for velocity. It is the resposnsibility
	    * of the caller of the method to allocate memory to COMASuplVelocity. Ownership is then
	    * transfered to Positiona and on deletion of position, Velocity is also deleted.
	    * @param aSuplVelocity parameter of type COMASuplVelocity*
	    * @retrun KErrArgument if any of the parameters in aSuplVelocity
	    * is not within the specified range. KErrNone otherwise.
	    */     
                        
        IMPORT_C void SetSuplVelocity(COMASuplVelocity* aSuplVelocity); 
     
     	/**
        * @Description CloneL method clones the position object and creates a
        * new position object with the same values. SUPL Framework invokes this method
        * to create a copy of the position information before terminating the POS Session
        * in POS Message plug-in.
        * @return COMASuplPosition object pointer pointing to the new position created.
        * This method leaves with KErrNoMemory in case of non-availability
        * of memory
        * 
        */
     	
     	IMPORT_C COMASuplInfoRequest* CloneL()const;
     	
     private:

        /**
        * C++ default constructor.
        */
        COMASuplPosition();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
   

    
    private:    // Data
    	
    	//Utc Time
    	TOMASuplUtcTime iUtcTime;
    	
    	// Position Estimate
    	TOMASuplPositionEstimate iPosEstimate;
    	
    	//Velocity
    	COMASuplVelocity* iVelocity;
    	
    	//Boolean to indiacte whether velocity is present or not
    	TBool iVelocityPresent;
    };


#endif      // COMASuplPosition  
            
// End of File
