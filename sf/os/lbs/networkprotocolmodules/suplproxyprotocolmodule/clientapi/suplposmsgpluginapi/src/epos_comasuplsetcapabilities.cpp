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
#include "epos_comasuplsetcapabilities.h"

// ============================ MEMBER FUNCTIONS OF TPosTechnology ===============================

// -----------------------------------------------------------------------------
// TPosTechnology::TPosTechnology,constuctor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C TOMASuplPosTechnology::TOMASuplPosTechnology()
	{       
	iGpsSETAssisted  = EFalse;
	iGpsSETBased     = EFalse;
	iAutonomousGps   = EFalse;
	iFLT   			 = EFalse;
	ieOTD  			 = EFalse;
	ioTDOA  		 = EFalse;
	}
// -----------------------------------------------------------------------------
// TPosTechnology::Get()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TOMASuplPosTechnology::GetPosTechnology(TBool&	aGpsSETAssisted,
										  	      	TBool&	aGpsSETBased,
												  	TBool&	aAutonomousGps,
												  	TBool&	aFLT,
												  	TBool&	aeOTD,
												  	TBool&	aoTDOA)const 
	{
	aGpsSETAssisted  = iGpsSETAssisted;
	aGpsSETBased     = iGpsSETBased;   
	aAutonomousGps   = iAutonomousGps; 
	aFLT   			 = iFLT;   		
	aeOTD  			 = ieOTD;  		
	aoTDOA  	 	 = ioTDOA; 
	}
// -----------------------------------------------------------------------------
// TPosTechnology::GpsBased()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TOMASuplPosTechnology::SetPosTechnology(TBool	aGpsSETAssisted,
										  	      	   TBool	aGpsSETBased,
												  	   TBool	aAutonomousGps,
												  	   TBool	aFLT,
												       TBool	aeOTD,
												  	   TBool	aoTDOA)
	{
	iGpsSETAssisted = aGpsSETAssisted;  
	iGpsSETBased    = aGpsSETBased;    
	iAutonomousGps  = aAutonomousGps;   
	iFLT        	= aFLT;   			 
	ieOTD  			= aeOTD;  			 
	ioTDOA			= aoTDOA;  	 	 
	}

// -----------------------------------------------------------------------------
// TPosProtocol::TPosProtocol() ,Constuctor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TOMASuplPosProtocol::TOMASuplPosProtocol()
	{
	iTIA801 = EFalse; 
	iRRLP   = EFalse;
	iRRC    = EFalse;
	}
// -----------------------------------------------------------------------------
// TPosProtocol::TIA801() 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TOMASuplPosProtocol::GetPosProtocol(TBool& aTIA801, 
											 	TBool& aRRLP,
											 	TBool& aRRC) const
	{
	aTIA801 = iTIA801; 
	aRRLP  = iRRLP;   
	aRRC   = iRRC ;   
	}
// -----------------------------------------------------------------------------
// TPosProtocol::RRLP() 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TOMASuplPosProtocol::SetPosProtocol(TBool aTIA801, 
												   TBool aRRLP,
												   TBool aRRC) 
	{
	iTIA801 = aTIA801; 
	iRRLP   = aRRLP;   
	iRRC    = aRRC ;   
	}


// ============================ MEMBER FUNCTIONS OF COMASuplSETCapabilities ===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
COMASuplSETCapabilities::COMASuplSETCapabilities()
    {
    	SetType(COMASuplInfoRequest::EOMASuplSETCapabilities);
    }

//
// ---------------------------------------------------------
//  destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplSETCapabilities::~COMASuplSETCapabilities()
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
    
EXPORT_C  COMASuplSETCapabilities* COMASuplSETCapabilities::NewL()
    {
    COMASuplSETCapabilities* self = new(ELeave) COMASuplSETCapabilities;
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
void COMASuplSETCapabilities::ConstructL()
    {
    iPrefMethod = EOMANoPreferred;
	}           

// -----------------------------------------------------------------------------
// COMASuplSETCapabilities::PosTechnology
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void COMASuplSETCapabilities::GetSETCapabilities(TOMASuplPosTechnology& aPosTechnology,
				  				 						TOMASuplPrefMethod&    aPrefMethod,
				  				                        TOMASuplPosProtocol&   aPosProtocol) const
    {
	aPosTechnology = iPosTechnology;
    aPrefMethod    = iPrefMethod;
	aPosProtocol   = iPosProtocol;
    }
// -----------------------------------------------------------------------------
// COMASuplSETCapabilities::PrefferedMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void COMASuplSETCapabilities::SetSETCapabilities(const TOMASuplPosTechnology& aPosTechnology,
					  				 				const TOMASuplPrefMethod&    aPrefMethod,
					  				 				const TOMASuplPosProtocol&   aPosProtocol)
	{
	iPosTechnology = aPosTechnology;
    iPrefMethod    = aPrefMethod;
	iPosProtocol   = aPosProtocol;
	}
	
	// -----------------------------------------------------------------------------
// COMASuplSETCapabilities::PosTechnology
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void COMASuplSETCapabilities::GetSETCapabilities(TOMASuplPosTechnology& aPosTechnology,
				  				 						TOMASuplPrefMethod&    aPrefMethod,
				  				            TOMASuplPosProtocol&   aPosProtocol,
				  				            TOMASuplPosProtocolVersion&   aPosProtocolVersion) const
    {
	aPosTechnology = iPosTechnology;
    aPrefMethod = iPrefMethod;
	aPosProtocol = iPosProtocol;
	aPosProtocolVersion = iPosProtocolVersion;
    }
// -----------------------------------------------------------------------------
// COMASuplSETCapabilities::PrefferedMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void COMASuplSETCapabilities::SetSETCapabilities(const TOMASuplPosTechnology& aPosTechnology,
					  				 				const TOMASuplPrefMethod&    aPrefMethod,
					  				 				const TOMASuplPosProtocol&   aPosProtocol,
					  				 				const TOMASuplPosProtocolVersion&   aPosProtocolVersion)
	{
	iPosTechnology = aPosTechnology;
    iPrefMethod = aPrefMethod;
	iPosProtocol = aPosProtocol;
	iPosProtocolVersion = aPosProtocolVersion;
	}

// -----------------------------------------------------------------------------
// COMASuplSETCapabilities::CloneL() 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
EXPORT_C COMASuplInfoRequest* COMASuplSETCapabilities::CloneL()const
	{
	COMASuplSETCapabilities*	cloneCapabilities = COMASuplSETCapabilities::NewL();

	cloneCapabilities->iPosTechnology = iPosTechnology;
	cloneCapabilities->iPrefMethod = iPrefMethod;
	cloneCapabilities->iPosProtocol = iPosProtocol;
	
	cloneCapabilities->SetStatus(Status());
    cloneCapabilities->SetType(Type());	
	return (cloneCapabilities);
	}
//  End of File  
