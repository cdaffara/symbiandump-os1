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
* Description:  Defines the data types related to OMA SUPL Velocity
*
*/


#ifndef C_COMASUPLVELOCITY_H
#define C_COMASUPLVELOCITY_H


// INCLUDES
#include <e32std.h>
#include <e32def.h>

#include <lbs/epos_comasuplinforequestlist.h>

// CLASS DECLARATION

/** @Description Enumeration representing the type of velocty
 */
 	  
enum TOMASuplVelocityType
{
	EHorizVelocity = 0,
	EHorizAndVertVelocity = 1,
	EHorizUncertVelocity,
	EHorizAndVertUncertVelocity
};

/**
* COMASuplHorizVelocity holds values for 
* - Bearing, which represents the angle on the horizontal axis towards North and
* - Horizontal Speed expressed in Kilometer per second
* More information about Horizontal Speed and Bearing can be found in 3G TS 23.032
*
*  @since Series 60 3.1u
*/
class COMASuplHorizVelocity: public CBase
    {
    
     public:  // Constructors and destructor
        
 	    	/**
        * @Description Creates a new instance of a COMASuplPosition and sets
        * the type in COMASuplInfoRequest to EOMASuplVelocity.
        *
        * @return A pointer to COMASuplPosition
        * It leaves with KErrNoMemory if object creation fails        
        */
        IMPORT_C static COMASuplHorizVelocity* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~COMASuplHorizVelocity();
    	
    	
        /**
        * C++ default constructor.
        */
        COMASuplHorizVelocity();
    
    public: // New functions
        
	   /**
	    * GetHorizVel retrieves values for
		  * - Bearing, which is the angle on the horizontal axis towards North and
		  * - Horizontal Speed expressed in Kilometer per second
	    * @param aBearing A reference Parameter that holds the bearing. The Most significant
	    * 9 bits contains the value for bearing. 
	    * @param aHorSpeed A refernce parameter that holds Horizontal Speed. All the 
	    * 16 bits hold the value for this parameter. It is expressed in Kilometer per
	    * second.
	    */     

        
        IMPORT_C void GetHorizVel(TUint16& aBearing,TUint16& aHorSpeed)const;
       
        
	   /**
	    * SetHorizVel sets values for Bearing and Horizontal Speed.
	    * @param aBearing Parameter of sets the Bearing. The Most significant
	    * 9 bits contains the value for bearing. Other bits of the value are
	    * ignored by the SUPL Framework. The endianess of the value is
	    * determined by the processor architecture. When this value is sent to the
	    * network, Network Byte order is used.
	    * @param aHorSpeed Parameter for Horizontal Speed. All the 
	    * 16 bits determines the value. It is expressed in Kilometer per hour.
	    * The endianess of the value is determined by the processor 
	    * architecture. When this value is sent to the network, Network 
	    * Byte order is used.
	    */     
        
        
      IMPORT_C void SetHorizVel(TUint16 aBearing,TUint16 aHorSpeed);
        
        
		virtual TOMASuplVelocityType VelType()const;
		
      private:


        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
  	
  	 protected:
  	 
  	 TOMASuplVelocityType iVelType;
        
    private:    // Data
        
        // Bearing
        TUint16 iBearing;
		
		//Contains the horizontal speed
		 
		TUint16 iHorSpeed;

        
    };

/**
* COMASuplHorizAndVertVelocity holds values for 
* - Bearing, which represents the angle on the horizontal axis towards North and
* - Horizontal Speed expressed in kilometer per second.
* - Vertical Direction in which Zero value indicate upward speed and 1 indicates downward speed.
* - Vertical Speed expressed in kilometer per second
* More information about Bearing, Horizontal Speed, Vertical Direction and
* Vertical Speed can be found in 3G TS 23.032
*
*  @since Series 60 3.1u
*/

class COMASuplHorizAndVertVelocity: public COMASuplHorizVelocity
    {
   
    public:  // Constructors and destructor
        
    	/**
        * @Description Creates a new instance of a COMASuplPosition.
        *
        * @return A pointer to COMASuplPosition
        * It leaves with KErrNoMemory if object creation fails        
        */
        IMPORT_C static COMASuplHorizAndVertVelocity* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~COMASuplHorizAndVertVelocity();
    
    	
        /**
        * C++ default constructor.
        */
        COMASuplHorizAndVertVelocity();
    
    
    public: // New functions
        
	   /**
	    * GetHorizAndVertVel retrieves values for
  		* - Bearing, which represents the angle on the horizontal 
  		* axis towards North
	  	* - Horizontal Speed expressed in Kilometer per second
		* - Vertical Direction in which Zero value indicate upward speed and 1 indicates downward speed
		* - vertical Speed expressed in Kilometer per second
	    * @param aBearing A reference Parameter that holds the bearing. The Most significant
	    * 9 bits hold the value for bearing.
	    * @param aHorSpeed A refernce parameter that holds Horizontal Speed.  All the 16 bits hold 
	    * the value
	    * @param aVerDirect A refernce parameter that holds Vertical Direction. The Most significant
	    * bit holds the value. Zero value indicate upward speed and 1 indicates downward speed.
	    * @param aVerSpeed A refernce parameter that holds Vertical Speed. All the 8 bits hold the value.
	    */     

        IMPORT_C void GetHorizAndVertVel(TUint16& aBearing,TUint16& aHorSpeed,
        								  TUint8& aVerDirect,TUint8& aVerSpeed)const;
        
        
	   /**
	    * SetHorizAndVertVel sets values for
		  * - Bearing, which represents the angle on the horizontal axis towards North
		  * - Horizontal Speed expressed in Kilometer per second
		  * - Vertical Direction in which Zero value indicate upward speed and 1 indicates downward speed
		  * - vertical Speed expressed in Kilometer per second
	    * @param aBearing Parameter for Bearing. The Most significant
	    * 9 bits hold the value for bearing. The other bits in the value 
	    * are ignored by the SUPL Framework. The endianess of the value is
	    * determined by the processor architecture. When this value is sent to the
	    * network, Network Byte order is used.
	    * @param aHorSpeed Parameter for Horizontal Speed. All the 16 bits hold 
	    * the value. The endianess of the value is determined by the processor 
	    * architecture. When this value is sent to the network, Network Byte order is used.
	    * @param aVerDirect Parameter for Vertical Direction. The Most significant
	    * bit holds the value. The endianess of the value is determined by the 
	    * processor architecture. When this value is sent to the network, Network 
	    * Byte order is used.
	    * @param aVerSpeed Parameter for Vertical Speed. All the 8 bits hold the value.
	    * The endianess of the value is determined by the processor architecture. 
	    * When this value is sent to the network, Network Byte order is used.
	    */     
                
        
       IMPORT_C void SetHorizAndVertVel(TInt16 aBearing,TInt16 aHorSpeed,
       									TInt8 aVerDirect,TInt8 aVerSpeed);
       
       virtual TOMASuplVelocityType VelType()const;
  private:


        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

 
    private:    // Data
        // Contains the direction of the vertical speed
        TUint8 iVertDirection;
        
        //Contains the vertical speed
        TUint8 iVertSpeed;
         
      };


/**
* COMASuplHorizUncertVelocity holds values for 
* - Bearing, which represents the angle on the horizontal axis towards North
* - Horizontal Speed expressed in Kilometer per second
* - Speed Uncertainty expressed in Kilometer per second
* More information abput Bearing, Horizontal Speed and Speed Uncertainty can be 
* found in 3G TS 23.032
*
*  @since Series 60 3.1u
*/
    
    
class COMASuplHorizUncertVelocity: public COMASuplHorizVelocity
    {
    
    public:  // Constructors and destructor
        
    	/**
        * @Description Creates a new instance of a COMASuplHorizUncertVelocity.
        *
        * @return A pointer to COMASuplHorizUncertVelocity
        * It leaves with KErrNoMemory if object creation fails        
        */
        IMPORT_C static COMASuplHorizUncertVelocity* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~COMASuplHorizUncertVelocity();
    
    
    
    public: // New functions
        
	   /**
	    * GetHorizUncertVel retrieves values for
		* - Bearing, which represents the angle on the horizontal axis towards North
		* - Horizontal Speed expressed in Kilometer per second
		* - Speed Uncertainty expressed in Kilometer per second
		* @param aBearing A reference Parameter that holds the bearing. The Most significant
	    * 9 bits hold the value for bearing.
	    * @param aHorSpeed A refernce parameter that holds Horizontal Speed. All the 16 bits 
	    * hold the value
	    * @param aUncertSpeed A refernce parameter that holds Speed uncertainty. All the 
	    * 8 bits hold the value.
	    */     
        
        IMPORT_C void GetHorizUncertVel(TUint16& aBearing,TUint16& aHorSpeed,
        							    TUint8& aUncertSpeed)const;
       
       
       
	   /**
	    * SetHorizUncertVel sets values for
		* - Bearing, which represents the angle on the horizontal axis towards North
		* - Horizontal Speed expressed in Kilometer per second
		* - Speed Uncertainty expressed in Kilometer per second
	    * @param aBearing Parameter for Bearing. The Most significant
	    * 9 bits hold the value for bearing. The other bits in the value are ignored
	    * by the SUPL Framework. The endianess of the value is
	    * determined by the processor architecture. When this value is sent to the
	    * network, Network Byte order is used.
	    * @param aHorSpeed Parameter for Horizontal Speed. All the 16 bits 
	    * hold the value. The endianess of the value is determined by the 
	    * processor architecture. When this value is sent to the network, Network 
	    * Byte order is used.
	    * @param aUncertSpeed Parameter for Vertical Direction. All the 
	    * 8 bits hold the value. The endianess of the value is
	    * determined by the processor architecture. When this value is sent to the
	    * network, Network Byte order is used.
	    */     

        
        IMPORT_C void SetHorizUncertVel(TUint16 aBearing,TUint16 aHorSpeed,
        							    TUint8 aUncertSpeed);
      
      	
      	TOMASuplVelocityType VelType()const;
      private:

        /**
        * C++ default constructor.
        */
        COMASuplHorizUncertVelocity();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
  
          
    private:    // Data
               
        //Contains the Uncertain speed
        TUint8 iUncertSpeed;
    };


  
/**
* COMASuplHorizAndVertUncertVelocity holds values for 
* - Bearing, which represents the angle on the horizontal axis towards North
* - Horizontal Speed expressed in Kilometer per second
* - Vertical Direction in which Zero value indicate upward speed and 1 indicates 
* downward speed.
* - Vertical Speed expressed in Kilometer per second
* - Horizontal Speed Uncertainty expressed in Kilometer per second
* - Vertical Speed Uncertainty expressed in Kilometer per second
*
* More information abput Bearing, Horizontal Speed and Speed Uncertainty can be 
* found in 3G TS 23.032
*
*  @since Series 60 3.1u
*/
class COMASuplHorizAndVertUncertVelocity:public COMASuplHorizAndVertVelocity
    {
    
       public:  // Constructors and destructor
        
    	/**
        * @Description Creates a new instance of a COMASuplHorizAndVertUncertVelocity.
        *
        * @return A pointer to COMASuplHorizAndVertUncertVelocity
        * It leaves with KErrNoMemory if object creation fails        
        */
        IMPORT_C static COMASuplHorizAndVertUncertVelocity* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~COMASuplHorizAndVertUncertVelocity();
  
   
    	public: // New functions
        
	   /**
	    * GetHorizVertUncertVel retrieves values for
		  * - Bearing, which represents the angle on the horizontal axis towards North
		  * - Horizontal Speed expressed in Kilometer per second
		  * - Vertical Direction in which Zero value indicate upward speed and 1 indicates 
		  * downward speed.
		  * - Vertical Speed expressed in Kilometer per second
		  * - Horizontal Speed Uncertainty expressed in Kilometer per second
		  * - Vertical Speed Uncertainty expressed in Kilometer per second
	    * @param aBearing A reference Parameter that holds the bearing. The Most significant
	    * 9 bits hold the value for bearing. 
	    * @param aHorSpeed A refernce parameter that holds Horizontal Speed. All the 16 bits 
	    * hold the value.
	    * @param aVerDirect A refernce parameter that holds Vertical Direction. All the 8 bits hold 
	    * the value.
	    * @param aVerSpeed A refernce parameter that holds Vertical Speed. All the 8 bits hold 
	    * the value
	    * @param aHorUncertSpeed A refernce parameter that holds Horizontal Speed Uncertainty.
	    * All the 8 bits hold the value. 
   	    * @param aHorUncertSpeed A refernce parameter that holds Vertical Speed Uncertainty. 
   	    * All the 8 bits hold the value.
	    */    
     
        IMPORT_C void GetHorizVertUncertVel(TUint16& aBearing,TUint16& aHorSpeed,
        								     TUint8& aVerDirect,TUint8& aVerSpeed,
        								     TUint8& aHorUncertSpeed,TUint8& aVerUncertSpeed)const;
       
       
       
	   /**
	    * SetHorizAndVertVel sets values for
		* - Bearing, which represents the angle on the horizontal axis towards North
		* - Horizontal Speed expressed in Kilometer per second
		* - Vertical Direction in which Zero value indicate upward speed and 1 indicates 
		* downward speed.
		* - vertical Speed expressed in Kilometer per second
	    * @param aBearing Parameter for Bearing. The Most significant
	    * 9 bits hold the value for bearing. The other bits in the value are
	    * ignored by the SUPL Framework. The endianess of the value is
	    * determined by the processor architecture. When this value is sent to the
	    * network, Network Byte order is used.
	    * @param aHorSpeed Parameter for Horizontal Speed. All the 16 bits 
	    * hold the value. The endianess of the value is determined by the 
	    * processor architecture. When this value is sent to the network, 
	    * Network Byte order is used.
	    * @param aVerDirect Parameter for Vertical Direction. The Most significant
	    * bit holds the value. The other bits in the value are ignored by the 
	    * SUPL Framework. The endianess of the value is determined by the processor 
	    * architecture. When this value is sent to the network, Network Byte order is used.
	    * @param aVerSpeed Parameter for Vertical Speed. All the 8 bits hold 
	    * the value. The endianess of the value is determined by the processor 
	    * architecture. When this value is sent to the network, Network Byte order is used.
	    * @param aHorUncertSpeed Parameter for Horizontal Speed Uncertainty. 
	    * All the 8 bits hold the value. The endianess of the value is determined by 
	    * the processor architecture. When this value is sent to the network, Network 
	    * Byte order is used.
		* @param aVerUncertSpeed Parameter for Vertical Speed Uncertainty. 
	    * All the 8 bits hold the value. The endianess of the value is determined 
	    * by the processor architecture. When this value is sent to the network, 
	    * Network Byte order is used.
	    */             
        
         IMPORT_C void SetHorizVertUncertVel(TUint16 aBearing,TUint16 aHorSpeed,
        								      	 TUint8 aVerDirect,TUint8 aVerSpeed,
         										 TUint8 aHorUncertSpeed,TUint8 aVerUncertSpeed);
        
		TOMASuplVelocityType VelType()const;
    private:

        /**
        * C++ default constructor.
        */
        COMASuplHorizAndVertUncertVelocity();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

 
    private:    // Data
        // Contains the Uncertain Horizontal speed
        TUint8 iHorizUncertSpeed;
        
        //Contains the vertical uncertain speed
        TUint8 iVertUncertSpeed;
      };


/**
* COMASuplVelocity holds values for 
* - Bearing, which represents the angle on the horizontal axis towards North
* - Horizontal Speed expressed in Kilometer per second
* - Vertical Direction in which Zero value indicate upward speed and 1 indicates 
* downward speed.
* - Vertical Speed expressed in Kilometer per second
* - Horizontal Speed Uncertainty expressed in Kilometer per second
* - Vertical Speed Uncertainty expressed in Kilometer per second
* More information abput Bearing, Horizontal Speed, Vertical Direction, 
* Vertical Speed, Horizontal Speed Uncertainty and Vertical Speed Uncertainty 
* can be found in 3G TS 23.032
*
*  @since Series 60 3.1u
*/
class COMASuplVelocity : public COMASuplInfoRequest
    {
 		
 	 public:  // Constructors and destructor
        
    	/**
        * @Description Creates a new instance of a COMASuplVelocity and sets
        * the type in COMASuplInfoRequest to EOMASuplVelocity.
        *
        * @return A pointer to COMASuplPosition
        * It leaves with KErrNoMemory if object creation fails        
        */
        IMPORT_C static COMASuplVelocity* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~COMASuplVelocity();	
 		
 	  public:
 	  

      public: // New functions
        
	   /**
	    * @Description retrieves the type of velocity
	    * @return Type of velocity provided by TOMASuplVelocityType
		*/                   
        IMPORT_C TOMASuplVelocityType VelType()const;
                     
 	   /**
	    * @Description Retrieves velocity which of type COMASuplHorizVelocity
		* @return A pointer of type COMASuplHorizVelocity
	    */    
        
        IMPORT_C COMASuplHorizVelocity* Velocity()const;
        
 	   /**
	    * @Description Sets velocity
	    * @param aVelocity A pointer of type COMASuplHorizVelocity
	    */            
        IMPORT_C void SetVelocity(COMASuplHorizVelocity* aVelocity);
        
     	/**
        * @Description CloneL clones the Velocity object and creates a
        * new velocity object with the same values
        * @return COMASuplPosition object pointer.
        * This method leaves with KErrNoMemory in case of non-availability
        * of memory
        * 
        */
        
        IMPORT_C COMASuplInfoRequest* CloneL()const;
        private:

        /**
        * C++ default constructor.
        */
        COMASuplVelocity();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
  
       
        private:    // Data
       
        // Contains the velocity
        COMASuplHorizVelocity* iVelocity;
   };



#endif      // SUPLVELOCTIY_H   
            
// End of File
