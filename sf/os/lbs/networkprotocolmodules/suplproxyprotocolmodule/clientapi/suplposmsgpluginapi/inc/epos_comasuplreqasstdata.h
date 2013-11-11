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
*				 POS Requested Assistance Data
*
*/


#ifndef C_COMASUPLREQASSTDATA_H
#define C_COMASUPLREQASSTDATA_H


// INCLUDES
#include <e32std.h>
#include <e32def.h>
#include <e32cmn.h>
#include <lbs/epos_comasuplinforequestlist.h>


// CLASS DECLARATION

/**
* TOMASuplSatelliteInfoElement holds values for
* - Satellite ID and
* - IODE (Issue Of Data Ephemeris), which is the Satellite Sequence Number
*
*  @since Series 60 3.1u
*/
class TOMASuplSatelliteInfoElement 
    {
   
    public:
    
    IMPORT_C TOMASuplSatelliteInfoElement();
    
    public: // New functions
        
	   /**
	    * @Description GetSatInfoElement gets values for
	    * - Satellite ID
	    * - IODE
	    * @param aSatId A reference Parameter that holds the Satellite ID
	    * aIode A refernce parameter that holds IODE, which is the satellite sequence number
	    */     
        
      IMPORT_C void GetSatInfoElement(TInt& aSatId,TInt& aIode )const;
        
	   /**
	    * @Description SetSatInfoElement sets values for
	    * - Satellite ID
	    * - IODE
	    * @param aSatId Parameter to set Satellite ID. The valid range is 0 to 63
	    * @param aIode Parameter to set the satellite sequence number. The valid range is 0 to 255
	    * @return KErrNone if parameters are within the specified range. KErrArgument if any of the
	    * parameters is not within the specified range.
	    */            
        
      IMPORT_C TInt SetSatInfoElement(TInt aSatId,TInt aIode );
        

    
    public:     // Data
    
    	//Satetllite Id
    	TInt iSatId;
    	//IODE
    	TInt iIode;    
       };


/**
* TOMASuplSatelliteInfoElement holds values for
* - GPS Week, which contains values for GPS Week of the Assistance Data currently held in the SET. The valid range is 0 to 1023.
* - GPS Toe, which contains the GPS time of Ephemeris in hours of the newest set of Ephemeris contained in the SUPL Enabled Terminal. Valid range is 0 to 167.
* - NSat, which contains the number of satellites to be considered for the current GPS Assistance data request. Valid range is 0 to 32.
* - Toe Limit, which contains the Ephemeris age tolerance of the SUPL Enabled Terminal in hours. Valid range is 0 to 10.
* - A list of Satellite Info objects. The number of satellite info elements held by the list is same as NSat. Each Satellite Info element contains the Satellite ID and Satellite Sequence Number.
* 
*  @since Series 60 3.1u
*/
class TOMASuplNavigationModel 
    {
    public:
    
    //default constructor
    IMPORT_C TOMASuplNavigationModel();
   
   	//Destructor
    IMPORT_C ~TOMASuplNavigationModel();
    
    public: // New functions
        
	   /**
	    * @Description GetSatInfoElement gets the set of all Satellite Info elements
	    * held by TOMASuplNavigationModel
	    * @param aSatInfo A reference parameter that holds the Satellite Info Elements
	    */     
	            
        IMPORT_C TInt GetSatInfoElement(RArray<TOMASuplSatelliteInfoElement>& aSatInfo) const;
        
	   /**
	    * @Description SetSatInfoElement sets the Satellite Info Elements
	    * @param aSatInfo Parameter of type RArray, that has the Satellite Info elements
	    */            
        
        IMPORT_C void SetSatInfoElement(const RArray<TOMASuplSatelliteInfoElement>& aSatInfo );
        
	   /**
	    * @Description GetSatInfoElement gets Satellite Info element at a given index
	    * @param aSatInfo A reference parameter that holds the Satellite Info Element
	    * @param aIndex The index for the Satellite Info Element to be retrieved
	    */     

        IMPORT_C TInt GetSatInfoElement(TOMASuplSatelliteInfoElement& aSatInfo,TInt aIndex) const;
        
	   /**
	    * @Description SetSatInfoElement sets Satellite Info element at a given index
	    * @param aSatInfo Parameter that holds the Satellite Info Element
	    * @param aIndex The index at which the Satellite Info element has to be set
	    */     

        
        IMPORT_C TInt SetSatInfoElement(const TOMASuplSatelliteInfoElement& aSatInfo,TInt aIndex );
        
	   /**
	    * @Description GetNavigationModel retrieves values for the different parameters of Navigation Data.
	    * @param aGpsWeek A reference parameter that holds the GPS Week of the assistance data currently held.
	    * @param aGpsToe A reference parameter that holds the GPS Time of Ephemeris in hours of the newest set of Ephemeris.
	    * @param aNSAT A reference parameter that holds Number of satellites to be considered for the current GPS assistance data request.
	    * @param aToeLimit A reference parameter that holds T-Toe Limit, that contains the age tolerance of the SET to the network in hours.
	    */     
        
      IMPORT_C void GetNavigationModel(TInt& aGpsWeek,TInt& aGpsToe,TInt& aNSAT,
        								 TInt& aToeLimit )const;
        
	   /**
	    * @Description SetNavigationModel sets values for the different parameters of Navigation Data
	    * @param aGpsWeek GPS Week of the assistance data currently held. 
	    * Valid range is 0 to 1023
	    * @param aGpsToe GPS Time of Ephemeris in hours of the newest set of Ephemeris. 
	    * Valid range is 0 to 167
	    * @param aNSAT Number of satellites to be considered for the current GPS assistance data request.
	    * Valid range is 0 to 31. 
	    * If the SUPL Enabled Terminal has no ephemeris data, this field shall be
	    * set to zero. 
	    * If the SUPL Enabled Terminal has ephemeris data whose age exceeds the T-Toe Limit,
	    * this field may be set to zero.
	    * If the network receives a zero value for this field, it ignores the GPS Week, GPS Toe
	    * fields and assumes that the SUPL Enabled Terminal has no ephemeris value
	    * @param aToeLimit T-Toe Limit, that contains the age tolerance of the SET to the network in 
	    * hours. Valid range is 0 to 10.
	    * @return KErrNone if all the parameters are within the specified range.
	    * KErrArgument if any of the parameters is not within the valid range.
	    */    
        
      IMPORT_C  TInt SetNavigationModel(TInt aGpsWeek,TInt aGpsToe,TInt aNSAT,
        								  TInt aToeLimit );
        
     	TOMASuplNavigationModel& operator=(const TOMASuplNavigationModel& aNaviModel);
    private:     // Data
    
    	TInt iGPSWeek;
    	
    	TInt iGPSToe;
    	
    	TInt iNSat;
    	
    	TInt iToeLimit;
    	
    	TBool iSatInfoPresent;
    	
      RArray<TOMASuplSatelliteInfoElement> iSatInfo;
    	 
    
    };

/**
* @Description Requested Assistance Data is applicable for A-GPS
* positioning methods only. It describes the requested A-GPS assistance data
* in the form of a bitmap of the following
*
* - Almanac Indicator
* - UTC Model 
* - Ionospheric Model
* - DGPS Corrections
* - Reference Location
* - Reference Time
* - Acquisition Assistance
* - Real Time Integrity
* 
*  @since Series 60 3.1u
*/
class COMASuplReqAsstData : public COMASuplInfoRequest
    {
   public:  // Constructors and destructor
        
    	/**
        * @Description Creates a new instance of a COMASuplReqAsstData and sets 
        * the type in COMASuplInfoRequest to EOMASuplReqAsstData.
        *
        * @return A pointer to COMASuplReqAsstData
        * It leaves with KErrNoMemory if object creation fails        
        */
        IMPORT_C static COMASuplReqAsstData* NewL();
    
    /**
    * Destructor.
    */
    IMPORT_C ~COMASuplReqAsstData();  

    
    public: // New functions
        
	   /**
	    * @Description GetNavigationData retrieves the Navigation Data if Navigation Model
	    * indicator is set.
	    * @param aNaviModel A reference parameter of type TOMASuplNavigationModel, 
	    * which holds the navigation Model. Navigation Model is an optinal parameter
	    * of OMA Supl requested Assistance Data
	    * @return KErrNone if Navigation Model indicator is set. KErrOMASuplParamNotSet otherwise
	    */     

        IMPORT_C  TInt GetNavigationData(TOMASuplNavigationModel& aNaviModel )const;
        
	   /**
	    * @Description SetNavigationData sets the Navigation Data as well as the 
	    * Navigation Model indicator. 
	    * @param aNaviModel Parameter of type TOMASuplNavigationModel, 
	    * which holds the Navigation Model. Navigation Model is an optinal parameter
	    * of OMA Supl requested Assistance Data. When the SetNavigationData method
	    * is  invoked, Navigation Model indicator is turned on
	    * @return KErrArgument if any of the parameters in aNaviModel is not within
	    * the specified range. KErrNone otherwise.
	    */     
        IMPORT_C  void SetNavigationData(const TOMASuplNavigationModel& aNaviModel );
        
	   /**
	    * @Description GetReqAsstData retrieves Requested Assistance Data
	    * @param aAlmanacReq A reference Boolean parameter which determines if Almanac 
	    * is requested for
	    * @param aUtcModel A reference Boolean parameter which determines if UTC Model 
	    * is requested for
	    * @param aIonModel A reference Boolean parameter which determines if ION Model 
	    * is requested for
	    * @param aDgpsCorrect A reference Boolean parameter which determines if DGPS Correction 
	    * is requested for
	    * @param aRefeLocation A reference Boolean parameter which determines if a Reference 
	    * Location is requested for
	    * @param aReferenceTimeRequested A reference Boolean parameter which determines if a Reference 
	    * Time is requested for
	    * @param aAcquisition A reference Boolean parameter which determines if a Acquisition 
	    * assistance is requested for
   	  * @param aRealTime A reference Boolean parameter which determines if a Real Time Integrity 
	    * is requested for
	    */     
        
        IMPORT_C  void GetReqAsstData(TBool& aAlmanacReq,TBool& aUtcModel,TBool& aIonModel,
        							  TBool& aDgpsCorrect,TBool& aRefeLocation,
        							  TBool& aReferenceTimeRequested,TBool& aAcquisition,
		        					   TBool& aRealTime)const;
    	      
           
        
	   /**
	    * @Description SetReqAsstData sets the different parameters in Requested Assistance Data
	    * @param aAlmanacReq A Boolean parameter which determines if Almanac 
	    * is present
	    * @param aUtcModel A Boolean parameter which determines if UTC Model 
	    * is present
	    * @param aIonModel A Boolean parameter which determines if ION Model 
	    * is present
	    * @param aDgpsCorrect A Boolean parameter which determines if DGPS Correction 
	    * is present
	    * @param aRefeLocation A Boolean parameter which determines if a Reference 
	    * Location is present
	    * @param aReferenceTimeRequested A Boolean parameter which determines if a Reference 
	    * Time is present
	    * @param aAcquisition A Boolean parameter which determines if a Acquisition 
	    * Assistance is present
   	    * @param aRealTime A Boolean parameter which determines if a Real Time Integrity 
	    * is present
	    */     

        
         IMPORT_C void SetReqAsstData(TBool aAlmanacReq,TBool aUtcModel,TBool aIonModel,
         							  TBool aDgpsCorrect,TBool aRefeLocation,
         							  TBool aReferenceTimeRequested,TBool aAcquisition,
        					    	  TBool aRealTime);
        
      	/**
        * @Description CloneL method clones the requested assistance data object and creates a
        * new requested assistance data object with the same values. SUPL Framework invokes this method
        * to create a copy of the requested assistance data information.
        *
        * @return COMASuplInfoRequest object pointer pointing to the new requested assistance
        * data object created.
        * This method leaves with KErrNoMemory in case of non-availability
        * of memory
        * 
        */
        IMPORT_C COMASuplInfoRequest* CloneL()const;
        
    private:

        /**
        * C++ default constructor.
        */
        COMASuplReqAsstData();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
     
    	
    private:     // Data
    
    	TBool iAlmanacRequested;
    	
    	TBool iUtcModelRequested;
    	
    	TBool iIonosphericModelRequested ;
    	
    	TBool iDgpsCorrectionsRequested ;
    	
    	TBool iReferenceLocationRequested;
    	
    	TBool iReferenceTimeRequested;
    	
    	TBool iAcquisitionAssistance;
    	
    	TBool iRealTimeIntegrityRequested; 
    	
    	TBool iNavigationModelRequested; 
    	
    	TOMASuplNavigationModel iNavigationData;
    };
        

#endif      // SUPLREQASSTDATA_H   
            
// End of File
