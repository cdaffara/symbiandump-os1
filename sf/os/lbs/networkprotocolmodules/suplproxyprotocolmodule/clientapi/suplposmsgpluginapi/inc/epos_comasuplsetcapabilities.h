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
* Description:  Header file for the different data types that
*                can be exchanged between POS Message plugin and
*			     SUPL FW
*
*/


#ifndef C_COMASUPLSETCAPABILITIES_H
#define C_COMASUPLSETCAPABILITIES_H

#include <e32def.h>
#include <e32std.h>
#include <lbs/epos_comasuplinforequestlist.h>
#include <lbs/epos_tomasuplposprotocolversion.h>

/**
* @Description TOMASuplPosTechnology describes the positioning method supported by POS Message plug-in. Possible values for POS technology are 
* - A-GPS SET Assisted only
* - A-GPS SET Based only
* - Autonomous GPS
* - AFLT (Advanced Forward Link Trilateration)
* - EOTD (Enhanced Observed Time Difference)
* - OTDOA (Observed Time Difference of Arrival)
*
*  @since Series 60 3.1u
*/
class TOMASuplPosTechnology
{

public: 

     	IMPORT_C TOMASuplPosTechnology();

     
public:	//Getter Methods

  /**
    * @Description GetPosTechnology retrieves the different positioning technologies
    * supported by POS Message plug-in
	  * @param aGpsSETAssisted A boolean reference parameter that determines if the 
	  * POS Message plug-in supports A-GPS SET Assisted technology
	  * @param aGpsSETBased A boolean reference parameter that determines if the 
	  * POS Message plug-in supports A-GPS SET Based technology
	  * @param aAutonomousGps A boolean reference parameter that determines if the 
	  * POS Message plug-in supports Autonomous GPS technology
	  * @param aFLT A boolean reference parameter that determines if the 
	  * POS Message plug-in supports AFLT
  	* @param aeOTD A boolean reference parameter that determines if the 
	  * POS Message plug-in supports EOTD
  	* @param aoTDOA A boolean reference parameter that determines if the 
	  * POS Message plug-in supports OTDOA
	  */  
	    IMPORT_C void GetPosTechnology(TBool&	aGpsSETAssisted,
									    TBool&	aGpsSETBased,
									    TBool&	aAutonomousGps,
									    TBool&	aAFLT,
							  	        TBool&	aeOTD,
									    TBool&	aoTDOA) const;
		
        
public:		//Setter Methods

	   /**
	    * @Description SetPosTechnology is invoked by POS Message plug-in to set values for 
	    * the different positioning technologies supported.
		* @param aGpsSETAssisted A boolean parameter that determines if
		* POS Message plug-in supports A-GPS SET Assisted technology
	    * @param aGpsSETBased A boolean parameter that determines if
	    * POS Message plug-in supports GPS SET Based technology
	    * @param aAutonomousGps A boolean parameter that determines if  
	    * POS Message plug-in supports supports Autonomous GPS technology
	    * @param aFLT A boolean parameter that determines if 
	    * POS Message plug-in supports AFLT technology
 	    * @param aeOTD A boolean parameter that determines if the 
	    * POS Message plug-in supports EOTD technology
  	    * @param aoTDOA A boolean parameter that determines if the 
	    * POS Message plug-in supports OTDOA technology
	    */  
		IMPORT_C void SetPosTechnology(TBool	aGpsSETAssisted,
										   TBool	aGpsSETBased,
										   TBool	aAutonomousGps,
										   TBool	aAFLT,
										   TBool	aeOTD,
										   TBool	aoTDOA);

private :	   // Positioning Technologies

		TBool	iGpsSETAssisted;
		TBool	iGpsSETBased;
		TBool	iAutonomousGps;
		TBool	iFLT;
		TBool	ieOTD;
		TBool	ioTDOA;
	
};

// End of TPosTechnology

/**
* @Description TOMASuplPosProtocolVersion determines the underlying control plane
* positioning protocol version. The different possible values for the positioning 
* protocol version are 
* - Major Version (First element of the version number for POS Protocol, range: (0..255)) 
* - Technical Version (Second element of the version number, range: (0..255)) 
* - Editorial Version (Third element of the version number for RRC, range: (0..255)) 
*
*  @since Series 60 5.2
**/


/**
* @Description TOMASuplPosProtocol determines the underlying control plane
* positioning protocol. The different possible values for the positioning protocol
* are 
* - TIA 801 (Telecommunication Industry Association 801)
* - RRLP (Radio Resource LCS Protocol)
* - RRC (Radio Resource Control)
*
*  @since Series 60 3.1u
**/
class TOMASuplPosProtocol
{

public: // Construction

     IMPORT_C TOMASuplPosProtocol();
     

public:	//Getter Methods

	   /**
	    * @Description GetPosProtocol retrieves the underlying control plane positioning
	    * protocol. SUPL Framework invokes this method.
	    * @param aTIA801 A boolean reference Parameter that determines if the 
	    * POS Protocol is TIA 801
	    * @param aRRLP A boolean reference Parameter that determines if the 
	    * POS Protocol is RRLP
	    * @param aRRC A boolean reference Parameter that determines if the 
	    * POS Protocol is RRC
	    */  
	    IMPORT_C void GetPosProtocol(TBool& aTIA801, 
									  TBool& aRRLP,
									  TBool& aRRC) const;
	
public: // Setter Methods

	   /**
	    * @Description SetPosProtocol is invoked by POS Message plug-in to set the
	    * underlying control plane positioning protocol.
	    * @param aTIA801 A Boolean parameter that determines if the 
	    * POS Protocol is TIA 801
	    * @param aRRLP A Boolean parameter that determines if the 
	    * POS Protocol is RRLP
	    * @param aRRLP A Boolean parameter that determines if the 
	    * POS Protocol is RRC
	    */  
        
	  IMPORT_C void SetPosProtocol(TBool aTIA801, 
									   TBool aRRLP,
									   TBool aRRC);
	
private :	//Data members , Positioning protocols

	TBool iTIA801; 
	TBool iRRLP;
	TBool iRRC;
	
};
// End of TPosProtocol

/**
* @Description COMASuplSETCapabilities defines the capabilities of the SUPL Enabled Terminal.SET capabilities are defined in terms of
* - POS Technology
* - Preferred Method and
* - POS Protocol
*
* SUPL Framework retrieves the SET Capabilities for SUPL START and SUPL POS INIT messages
*
*  @since Series 60 3.1u
**/

class COMASuplSETCapabilities: public COMASuplInfoRequest
{
	public:  // Constructors and destructor
       /**
				 *  @Description Enumeration values for Preferred Mode
				 */
				 enum TOMASuplPrefMethod
				 {
							EOMAAGpsSETAssitedPreferred = 0,
							EOMAAGpsSETBasedPreferred = 1,
							EOMANoPreferred = 2
				 }; 
				 
     	/**
        * @Description Creates a new instance of a COMASuplSETCapabilities and sets 
        * the type in COMASuplInfoRequest to EOMASuplSETCapabilities.
        *
        * @return A pointer to COMASuplSETCapabilities. It leaves with KErrNoMemory if object creation fails        
        */
        IMPORT_C static COMASuplSETCapabilities* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C  ~COMASuplSETCapabilities();

	public:
	   /**
	    * @Description GetSETCapabilities is invoked by SUPL Framework to retrive the 
	    * capabilities of the SET. 
	    * @param aPosTechnology A Reference Parameter of type TOMASuplPosTechnology
	    * @param aPrefMethod A Reference Parameter of type TOMASuplPrefMethod
	    * @param aPosProtocol A Reference Parameter of type TOMASuplPosProtocol 
	    */  

	IMPORT_C void GetSETCapabilities(TOMASuplPosTechnology& aPosTechnology,
				  				 	  TOMASuplPrefMethod&    aPrefMethod,
				  				 	  TOMASuplPosProtocol&   aPosProtocol) const;

	   /**
	    * @Description SetSETCapabilities is invoked by POS Message plug-in to set the capabilities
	    * supported by the POS Module.
	    * @param aPosTechnology A parameter of type TOMASuplPosTechnology 
	    * that sets values for POS Technology
	    * @param aPrefMethod A parameter of type TOMASuplPrefMethod 
	    * that sets values for Preferred Method
	    * @param aPosProtocol A parameter of type TOMASuplPosProtocol 
	    * that sets values for POS Protocol
	    */  
	    IMPORT_C void SetSETCapabilities(const TOMASuplPosTechnology& aPosTechnology,
					  				 	 const TOMASuplPrefMethod&    aPrefMethod,
					  				 	 const TOMASuplPosProtocol&   aPosProtocol);

	   /**
	    * @Description GetSETCapabilities is invoked by SUPL Framework to retrive the 
	    * capabilities of the SET. 
	    * @param aPosTechnology A Reference Parameter of type TOMASuplPosTechnology
	    * @param aPrefMethod A Reference Parameter of type TOMASuplPrefMethod
	    * @param aPosProtocol A Reference Parameter of type TOMASuplPosProtocol 
	    * @param aPosProtocolVersion A Reference Parameter of type TOMASuplPosProtocolVersion 
	    */  

	IMPORT_C void GetSETCapabilities(TOMASuplPosTechnology& aPosTechnology,
				  				 	  TOMASuplPrefMethod&    aPrefMethod,
				  				 	  TOMASuplPosProtocol&   aPosProtocol,
                                      TOMASuplPosProtocolVersion&   aPosProtocolVersion) const;

	   /**
	    * @Description SetSETCapabilities is invoked by POS Message plug-in to set the capabilities
	    * supported by the POS Module.
	    * @param aPosTechnology A parameter of type TOMASuplPosTechnology 
	    * that sets values for POS Technology
	    * @param aPrefMethod A parameter of type TOMASuplPrefMethod 
	    * that sets values for Preferred Method
	    * @param aPosProtocol A parameter of type TOMASuplPosProtocol 
	    * that sets values for POS Protocol
	    * @param aPosProtocolVersion A parameter of type TOMASuplPosProtocolVersion 
	    * that sets values for POS Protocol Version
	    */  
	    IMPORT_C void SetSETCapabilities(const TOMASuplPosTechnology& aPosTechnology,
					  				 	 const TOMASuplPrefMethod&    aPrefMethod,
					  				 	 const TOMASuplPosProtocol&   aPosProtocol,
					  				 	 const TOMASuplPosProtocolVersion&   aPosProtocolVersion);
	public: //From Base Class
		
     	/**
        * @Description CloneL method clones the set capabilities object and creates a
        * new set capabilities object with the same values. SUPL Framework invokes this method
        * to create a copy of the set capabilities information before terminating the 
        * POS Session in POS Message plug-in.
        *
        * @return COMASuplInfoRequest object pointer pointing to the new set capabilities
        * object created.
        * This method leaves with KErrNoMemory in case of non-availability
        * of memory
        * 
        */

		COMASuplInfoRequest* CloneL()const;
	
	private:
	
	// Default Constructor
	COMASuplSETCapabilities();
	//Two phase construction
	void ConstructL();
	
	private:
    // Pos Tec
	TOMASuplPosTechnology iPosTechnology;
	TOMASuplPrefMethod    iPrefMethod;
  	TOMASuplPosProtocol   iPosProtocol;
  	TOMASuplPosProtocolVersion   iPosProtocolVersion;
	};



#endif //  _SUPLSETCAPABILITIES_H
