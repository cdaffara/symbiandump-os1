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



// INCLUDE FILES
#include "epos_tomasuplallowedcapabilities.h"

// ============================ MEMBER FUNCTIONS OF TPosTechnology ===============================

// -----------------------------------------------------------------------------
// TPosTechnology::TPosTechnology,constuctor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TOMASuplAllowedCapabilities::TOMASuplAllowedCapabilities()
	{       
	iGpsSETAssisted  = EFalse;
	iGpsSETBased     = EFalse;
	iAutonomousGps   = EFalse;
	iFLT   			 = EFalse;
	ieOTD  			 = EFalse;
	ioTDOA  		 = EFalse;
	ieCID				=  EFalse;
	iCID        = EFalse; 
	}
// -----------------------------------------------------------------------------
// TPosTechnology::Get()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TOMASuplAllowedCapabilities::GetAllowedCapabilities(TBool&	aGpsSETAssisted,
										  	      	TBool&	aGpsSETBased,
												  	TBool&	aAutonomousGps,
												  	TBool&	aFLT,
												  	TBool&	aeOTD,
												  	TBool&	aoTDOA,
												  	TBool&  aeCID,
									    TBool&  aCID)const 
	{
	aGpsSETAssisted  = iGpsSETAssisted;
	aGpsSETBased     = iGpsSETBased;   
	aAutonomousGps   = iAutonomousGps; 
	aFLT   			 = iFLT;   		
	aeOTD  			 = ieOTD;  		
	aoTDOA  	 	 = ioTDOA; 
	aeCID        	 = ieCID;
	aCID			 = iCID;
	}
// -----------------------------------------------------------------------------
// TPosTechnology::GpsBased()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TOMASuplAllowedCapabilities::SetAllowedCapabilities(TBool	aGpsSETAssisted,
										  	      	   TBool	aGpsSETBased,
												  	   TBool	aAutonomousGps,
												  	   TBool	aFLT,
												       TBool	aeOTD,
												  	   TBool	aoTDOA,TBool  aeCID,
									    TBool  aCID)
	{
	iGpsSETAssisted = aGpsSETAssisted;  
	iGpsSETBased    = aGpsSETBased;    
	iAutonomousGps  = aAutonomousGps;   
	iFLT        	= aFLT;   			 
	ieOTD  			= aeOTD;  			 
	ioTDOA			= aoTDOA;  	
	ieCID        	= aeCID;
	iCID			= aCID; 	 
	}

//  End of File  
