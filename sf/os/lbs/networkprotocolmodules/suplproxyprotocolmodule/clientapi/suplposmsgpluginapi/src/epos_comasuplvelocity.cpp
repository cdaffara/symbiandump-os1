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



#include <badesca.h>

#include "epos_comasuplvelocity.h"

// ============================ MEMBER FUNCTIONS of COMASuplHorizVelocity===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
COMASuplHorizVelocity::COMASuplHorizVelocity()
    {
    	
    }

//
// ---------------------------------------------------------
//  destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplHorizVelocity::~COMASuplHorizVelocity()
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
    
EXPORT_C  COMASuplHorizVelocity* COMASuplHorizVelocity::NewL()
    {
    COMASuplHorizVelocity* self = new(ELeave) COMASuplHorizVelocity;
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
void COMASuplHorizVelocity::ConstructL()
    {
    iVelType = EHorizVelocity;
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  void COMASuplHorizVelocity::GetHorizVel(TUint16& aBearing,TUint16& aHorSpeed)const
    {
	aBearing = iBearing;
	aHorSpeed = iHorSpeed;
    }           
//
// -------------------------------------------------------------
// Sets the values of horizontal velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//

EXPORT_C void COMASuplHorizVelocity::SetHorizVel(TUint16 aBearing,TUint16 aHorSpeed)
    {
	iBearing = aBearing;
	iHorSpeed = aHorSpeed;
    }           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
TOMASuplVelocityType COMASuplHorizVelocity::VelType()const
    {
	return iVelType;
    }   
// ============================ MEMBER FUNCTIONS of COMASuplHorizAndVertVelocity===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
COMASuplHorizAndVertVelocity::COMASuplHorizAndVertVelocity()
    {
    }

//
// ---------------------------------------------------------
//  destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C  COMASuplHorizAndVertVelocity::~COMASuplHorizAndVertVelocity()
    {
    }
    
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizAndVertVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  COMASuplHorizAndVertVelocity* COMASuplHorizAndVertVelocity::NewL()
    {
    COMASuplHorizAndVertVelocity* self = new(ELeave) COMASuplHorizAndVertVelocity;
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
void COMASuplHorizAndVertVelocity::ConstructL()
    {
    iVelType = EHorizAndVertVelocity;
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  void COMASuplHorizAndVertVelocity::GetHorizAndVertVel(TUint16& aBearing,TUint16& aHorSpeed,
        								  					  TUint8& aVerDirect,TUint8& aVerSpeed) const
        
    {
	GetHorizVel(aBearing,aHorSpeed);
	aVerDirect = iVertDirection;
	aVerSpeed  = iVertSpeed;
    }           
//
// -------------------------------------------------------------
// Sets the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//

EXPORT_C void COMASuplHorizAndVertVelocity::SetHorizAndVertVel(TInt16 aBearing,TInt16 aHorSpeed,
       									    		           TInt8 aVerDirect,TInt8 aVerSpeed)
    {
	SetHorizVel(aBearing,aHorSpeed);
	iVertDirection = aVerDirect;
	iVertSpeed     = aVerSpeed;
    }           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
TOMASuplVelocityType COMASuplHorizAndVertVelocity::VelType()const
    {
	return iVelType;
    }   
// ============================ MEMBER FUNCTIONS of COMASuplHorizAndVertVelocity===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
COMASuplHorizUncertVelocity::COMASuplHorizUncertVelocity()
    {
    }

//
// ---------------------------------------------------------
//  destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplHorizUncertVelocity::~COMASuplHorizUncertVelocity()
    {
    }
    
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizAndVertVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  COMASuplHorizUncertVelocity* COMASuplHorizUncertVelocity::NewL()
    {
    COMASuplHorizUncertVelocity* self = new(ELeave) COMASuplHorizUncertVelocity;
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
void COMASuplHorizUncertVelocity::ConstructL()
    {
    iVelType = EHorizUncertVelocity;
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  void COMASuplHorizUncertVelocity::GetHorizUncertVel(TUint16& aBearing,
															TUint16& aHorSpeed,TUint8& aUncertSpeed)const
        
    {
	GetHorizVel(aBearing,aHorSpeed);
	aUncertSpeed = iUncertSpeed;
    }           
//
// -------------------------------------------------------------
// Sets the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//

EXPORT_C void COMASuplHorizUncertVelocity::SetHorizUncertVel(TUint16 aBearing,TUint16 aHorSpeed,
													          TUint8 aUncertSpeed)
    {
	SetHorizVel(aBearing,aHorSpeed);
	iUncertSpeed = aUncertSpeed;
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
TOMASuplVelocityType COMASuplHorizUncertVelocity::VelType()const
    {
	return iVelType;
    }   
// ============================ MEMBER FUNCTIONS of COMASuplHorizAndVertUncertVelocity===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
COMASuplHorizAndVertUncertVelocity::COMASuplHorizAndVertUncertVelocity()
    {
    }

//
// ---------------------------------------------------------
//  destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C  COMASuplHorizAndVertUncertVelocity::~COMASuplHorizAndVertUncertVelocity()
    {
    }
    
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizAndVertVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  COMASuplHorizAndVertUncertVelocity* COMASuplHorizAndVertUncertVelocity::NewL()
    {
    COMASuplHorizAndVertUncertVelocity* self = new(ELeave) COMASuplHorizAndVertUncertVelocity;
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
void COMASuplHorizAndVertUncertVelocity::ConstructL()
    {
    iVelType = EHorizAndVertUncertVelocity;
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C  void COMASuplHorizAndVertUncertVelocity::GetHorizVertUncertVel(TUint16& aBearing,TUint16& aHorSpeed,
        								     				TUint8& aVerDirect,TUint8& aVerSpeed,
        								     				TUint8& aHorUncertSpeed,TUint8& aVerUncertSpeed)const
    {
	GetHorizAndVertVel(aBearing,aHorSpeed,aVerDirect,aVerSpeed);
	aHorUncertSpeed = iHorizUncertSpeed;
	aVerUncertSpeed = iVertUncertSpeed;
    }           
//
// -------------------------------------------------------------
// Sets the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//

EXPORT_C void COMASuplHorizAndVertUncertVelocity::SetHorizVertUncertVel(TUint16 aBearing,TUint16 aHorSpeed,
        								      	 			TUint8 aVerDirect,TUint8 aVerSpeed,
         										 			TUint8 aHorUncertSpeed,TUint8 aVerUncertSpeed)
    {
	SetHorizAndVertVel(aBearing,aHorSpeed,aVerDirect,aVerSpeed);
	iHorizUncertSpeed = aHorUncertSpeed;
	iVertUncertSpeed  = aVerUncertSpeed;
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
TOMASuplVelocityType COMASuplHorizAndVertUncertVelocity::VelType()const
    {
	return iVelType;
    }   
// ============================ MEMBER FUNCTIONS of COMASuplVelocity===============================

//
// ---------------------------------------------------------
//  constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
COMASuplVelocity::COMASuplVelocity()
    {
    	SetType(COMASuplInfoRequest::EOMASuplVelocity);
    }

//
// ---------------------------------------------------------
//  destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C  COMASuplVelocity::~COMASuplVelocity()
    {
    	delete iVelocity;
    }
    
//
// ---------------------------------------------------------
// Static Factory method to create the COMASuplHorizAndVertVelocity
// implementation
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
    
EXPORT_C  COMASuplVelocity* COMASuplVelocity::NewL()
    {
    COMASuplVelocity* self = new(ELeave) COMASuplVelocity;
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
void COMASuplVelocity::ConstructL()
    {
	}           
    
//
// -------------------------------------------------------------
// Sets the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
// 

EXPORT_C COMASuplHorizVelocity* COMASuplVelocity::Velocity() const
    {
	return iVelocity;
	}           

//
// -------------------------------------------------------------
// Sets the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
// 

EXPORT_C void COMASuplVelocity::SetVelocity(COMASuplHorizVelocity* aVelocity)
    {
    delete iVelocity;
    iVelocity = NULL;
	iVelocity = aVelocity;
	}           

//
// -------------------------------------------------------------
// Retrieves the values of horizontal and vertical velocity
// Implementation
//
// (other items were commented in a header).
// -------------------------------------------------------------
//
EXPORT_C TOMASuplVelocityType COMASuplVelocity::VelType()const
    {
	return iVelocity->VelType();
    }  
    
// -----------------------------------------------------------------------------
// COMASuplVelocity::CloneVelocity() 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
     
EXPORT_C COMASuplInfoRequest* COMASuplVelocity::CloneL()const
	{
		COMASuplVelocity* cloneVelocity = COMASuplVelocity::NewL();
		CleanupStack::PushL(cloneVelocity);
		if(iVelocity)
			{
			TOMASuplVelocityType velocityType = iVelocity->VelType();
			TUint16 bearing;
			TUint16 horSpeed;
		
			switch(velocityType)
			{
    			case EHorizVelocity:
					{
					COMASuplHorizVelocity* clone = COMASuplHorizVelocity::NewL();
				
					iVelocity->GetHorizVel(bearing,horSpeed);
					clone->SetHorizVel(bearing,horSpeed);	
					cloneVelocity->SetVelocity(clone);

					break;		
					}
				case EHorizAndVertVelocity:
					{
					COMASuplHorizAndVertVelocity* clone = COMASuplHorizAndVertVelocity::NewL();
				
					COMASuplHorizAndVertVelocity* horizVertVel = (COMASuplHorizAndVertVelocity*)iVelocity;
					TUint8 verDirect;
					TUint8 verSpeed;	
					horizVertVel->GetHorizAndVertVel(bearing,horSpeed,verDirect,verSpeed);
					clone->SetHorizAndVertVel(bearing,horSpeed,verDirect,verSpeed);	
					cloneVelocity->SetVelocity(clone);
					
					break;
					}
				case EHorizUncertVelocity:
					{
					COMASuplHorizUncertVelocity* clone = COMASuplHorizUncertVelocity::NewL();
			
					COMASuplHorizUncertVelocity* horizUncertVel = (COMASuplHorizUncertVelocity*)iVelocity;
					TUint8 uncert;
					horizUncertVel->GetHorizUncertVel(bearing,horSpeed,uncert);
					clone->SetHorizUncertVel(bearing,horSpeed,uncert);	
					cloneVelocity->SetVelocity(clone);
					break;
					}
				case EHorizAndVertUncertVelocity:
					{
					COMASuplHorizAndVertUncertVelocity* clone = COMASuplHorizAndVertUncertVelocity::NewL();
					
					COMASuplHorizAndVertUncertVelocity* horizVertUncertVel = (COMASuplHorizAndVertUncertVelocity*)iVelocity;
			        TUint8 verDirect;
					TUint8 verSpeed;	
			        TUint8 horizUncert;
           			TUint8 vertUncert;
					horizVertUncertVel->GetHorizVertUncertVel(bearing,horSpeed,verDirect,
															   verSpeed,horizUncert,vertUncert);
					clone->SetHorizVertUncertVel(bearing,horSpeed,verDirect,
													verSpeed,horizUncert,vertUncert);	
					cloneVelocity->SetVelocity(clone);
					break;	
					}		
			}				
		}

	cloneVelocity->SetStatus(Status());
    cloneVelocity->SetType(Type());
  	CleanupStack::Pop(cloneVelocity);
  	return (cloneVelocity);
	}    
//End of file
