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


#ifndef C_TOMASUPLALLOWEDCAPABILITIES_H
#define C_TOMASUPLALLOWEDCAPABILITIES_H

#include <e32def.h>
#include <e32std.h>


/**
* @Description TOMASuplAllowedCapabilities describes the common part of the capabilites received from the client 
							 and the capabilities received from the POS Plug-in
*  Possible values for POS technology are 
* - A-GPS SET Assisted only
* - A-GPS SET Based only
* - Autonomous GPS
* - AFLT (Advanced Forward Link Trilateration)
* - EOTD (Enhanced Observed Time Difference)
* - OTDOA (Observed Time Difference of Arrival)
* - ECID (Enhanced Cell Id)
* - CID (Cell ID)
*
*  @since Series 60 9.1tb
*/
class TOMASuplAllowedCapabilities
{

public: 

     	IMPORT_C TOMASuplAllowedCapabilities();

     
public:	//Getter Methods

    /**
    * @Description GetAllowedCapabilities retrieves the positioning technologies
    * requested by the SUPL Client
	  * @param aGpsSETAssisted A boolean reference parameter that determines if the 
	  * SUPL Client requests for A-GPS SET Assisted technology
	  * @param aGpsSETBased A boolean reference parameter that determines if the 
	  * SUPL Client requests for A-GPS SET Based technology
	  * @param aAutonomousGps A boolean reference parameter that determines if the 
	  * SUPL Client requests for Autonomous GPS technology
	  * @param aFLT A boolean reference parameter that determines if the 
	  * SUPL Client requests for  AFLT
  	* @param aeOTD A boolean reference parameter that determines if the 
	  * SUPL CLIENT requests for EOTD
  	* @param aoTDOA A boolean reference parameter that determines if the 
	  * SUPL CLIENT requests for OTDOA
	  * @param aeCID A boolean reference parameter that determines if the
	  * SUPL CLIENT requests for ECID
	  * @param aCID A boolean reference parameter that determines if the
	  * SUPL CLIENT requests for CID
	  */ 
	    IMPORT_C void GetAllowedCapabilities(TBool&	aGpsSETAssisted,
									    TBool&	aGpsSETBased,
									    TBool&	aAutonomousGps,
									    TBool&	aAFLT,
							  	        TBool&	aeOTD,
									    TBool&	aoTDOA,
									    TBool&  aeCID,
									    TBool&  aCID) const;
		
        
public:		//Setter Methods

	   /**
    * @Description SetAllowedCapabilities is used to set the positioning technologies
    * requested by the SUPL Client
	  * @param aGpsSETAssisted A boolean reference parameter that determines if the 
	  * SUPL Client requests for A-GPS SET Assisted technology
	  * @param aGpsSETBased A boolean reference parameter that determines if the 
	  * SUPL Client requests for A-GPS SET Based technology
	  * @param aAutonomousGps A boolean reference parameter that determines if the 
	  * SUPL Client requests for Autonomous GPS technology
	  * @param aFLT A boolean reference parameter that determines if the 
	  * SUPL Client requests for  AFLT
  	* @param aeOTD A boolean reference parameter that determines if the 
	  * SUPL CLIENT requests for EOTD
  	* @param aoTDOA A boolean reference parameter that determines if the 
	  * SUPL CLIENT requests for OTDOA
	  * @param aeCID A boolean reference parameter that determines if the
	  * SUPL CLIENT requests for ECID
	  * @param aCID A boolean reference parameter that determines if the
	  * SUPL CLIENT requests for CID
	  */ 
	  
		IMPORT_C void SetAllowedCapabilities(TBool	aGpsSETAssisted,
										   TBool	aGpsSETBased,
										   TBool	aAutonomousGps,
										   TBool	aAFLT,
										   TBool	aeOTD,
										   TBool	aoTDOA,
										   TBool  aeCID,
									       TBool  aCID);

private :	   // Positioning Technologies

		TBool	iGpsSETAssisted;
		TBool	iGpsSETBased;
		TBool	iAutonomousGps;
		TBool	iFLT;
		TBool	ieOTD;
		TBool	ioTDOA;
		TBool 	ieCID;
		TBool   iCID;
	
};

// End of TOMASuplAllowedCapabilities



#endif //  _SUPLALLOWEDCAPABILITIES_H