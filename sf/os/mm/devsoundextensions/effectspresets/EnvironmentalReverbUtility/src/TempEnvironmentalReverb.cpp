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
* Description:   Implementation of the audio EnvironmentalReverb proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "TempEnvironmentalReverb.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTempEnvironmentalReverb::CTempEnvironmentalReverb
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTempEnvironmentalReverb::CTempEnvironmentalReverb()
    {
    }

// Destructor
CTempEnvironmentalReverb::~CTempEnvironmentalReverb()
    {
	}

// -----------------------------------------------------------------------------
// CTempEnvironmentalReverb::NewL
// Static function for creating an instance of the Audio EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CTempEnvironmentalReverb* CTempEnvironmentalReverb::NewL(CEnvironmentalReverb* aEnvironmentalReverb)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CTempEnvironmentalReverb::NewL"));
#endif

    CTempEnvironmentalReverb* self = new (ELeave) CTempEnvironmentalReverb();
    CleanupStack::PushL( self );
	self->ConstructL(aEnvironmentalReverb);
	CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CTempEnvironmentalReverb::ConstructL
// -----------------------------------------------------------------------------
//
void CTempEnvironmentalReverb::ConstructL(CEnvironmentalReverb* aEnvironmentalReverb)
    {
#ifdef _DEBUG
    RDebug::Print(_L("CTempEnvironmentalReverb::ConstructL"));
#endif

	iEnvironmentalReverb = aEnvironmentalReverb;
	}


// -----------------------------------------------------------------------------
// CTempEnvironmentalReverb::ApplyL
// Apply the Audio EnvironmentalReverb settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempEnvironmentalReverb::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CTempEnvironmentalReverb::Apply"));
#endif
    if (DecayHFRatio() != iEnvironmentalReverb->DecayHFRatio())
      iEnvironmentalReverb->SetDecayHFRatioL (DecayHFRatio());
    
    if (DecayTime() != iEnvironmentalReverb->DecayTime())
      iEnvironmentalReverb->SetDecayTimeL (DecayTime());
    
    if (Density() != iEnvironmentalReverb->Density())
      iEnvironmentalReverb->SetDensityL ( Density());
    
    if (Diffusion() != iEnvironmentalReverb->Diffusion())
      iEnvironmentalReverb->SetDiffusionL (Diffusion());
    
    if (ReflectionsLevel() != iEnvironmentalReverb->ReflectionsLevel())
      iEnvironmentalReverb->SetReflectionsLevelL (ReflectionsLevel());
    
    if (ReflectionsDelay() != iEnvironmentalReverb->ReflectionsDelay())
      iEnvironmentalReverb->SetReflectionsDelayL (ReflectionsDelay());
    
    if (ReverbDelay() != iEnvironmentalReverb->ReverbDelay())
      iEnvironmentalReverb->SetReverbDelayL (ReverbDelay());
    
    if (ReverbLevel() != iEnvironmentalReverb->ReverbLevel())
      iEnvironmentalReverb->SetReverbLevelL (ReverbLevel());
    
    if (RoomHFLevel() != iEnvironmentalReverb->RoomHFLevel())
      iEnvironmentalReverb->SetRoomHFLevelL (RoomHFLevel());
    
    if (RoomLevel() != iEnvironmentalReverb->RoomLevel())
      iEnvironmentalReverb->SetRoomLevelL (RoomLevel());
 
 
    if(iEnvironmentalReverb)
	  iEnvironmentalReverb->EnableL();
	    
	}



// -----------------------------------------------------------------------------
// CTempEnvironmentalReverb::EnableL
// Enable the Audio EnvironmentalReverb settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempEnvironmentalReverb::EnableL()
   {
   #ifdef _DEBUG
   	RDebug::Print(_L("CTempEnvironmentalReverb::EnableL"));
   	#endif
   	
   	iEnabled = ETrue;
    ApplyL();  
   }

// -----------------------------------------------------------------------------
// CTempEnvironmentalReverb::DisableL
// Disable the Audio EnvironmentalReverb settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempEnvironmentalReverb::DisableL()
   {
   #ifdef _DEBUG   
   	 RDebug::Print(_L("CTempEnvironmentalReverb::DisableL"));
   #endif
   	 
   	iEnabled = EFalse;   	 
	if(iEnvironmentalReverb)
	  iEnvironmentalReverb->DisableL();	    
   }

// -----------------------------------------------------------------------------
// CTempEnvironmentalReverb::IsEnableL
// Return enable flag.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CTempEnvironmentalReverb::IsEnabled() const
   {
   #ifdef _DEBUG 
   	 RDebug::Print(_L("CTempEnvironmentalReverb::IsEnabled"));
   #endif

    TBool ifEnabled = EFalse;
    if(iEnvironmentalReverb)
	   ifEnabled = iEnvironmentalReverb->IsEnabled();	    
    
    
    return ifEnabled;
   }



// -----------------------------------------------------------------------------
// CTempEnvironmentalReverb::IsEnforced
// Return enForce flag
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CTempEnvironmentalReverb::IsEnforced() const
   {
   #ifdef _DEBUG 
   RDebug::Print(_L("CTempEnvironmentalReverb::IsEnforced"));
   #endif

    TBool ifEnforced = EFalse;   
    if(iEnvironmentalReverb)
	  ifEnforced = iEnvironmentalReverb->IsEnforced();	    	
    
    return ifEnforced;
   
   }


// -----------------------------------------------------------------------------
// CTempEnvironmentalReverb::EnforceL
// Enforce the effect
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempEnvironmentalReverb::EnforceL( TBool aEnforced )
   {
   #ifdef _DEBUG 
   RDebug::Print(_L("CTempEnvironmentalReverb::EnforceL"));
   #endif    

   iEnforced = aEnforced;
   
   if(iEnvironmentalReverb)
	  iEnvironmentalReverb->EnforceL(aEnforced);	   	
   }



// -----------------------------------------------------------------------------
// CTempEnvironmentalReverb::HaveUpdateRights
// Return enForce flag
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CTempEnvironmentalReverb::HaveUpdateRights() const
   {
   #ifdef _DEBUG 
   RDebug::Print(_L("CTempEnvironmentalReverb::HaveUpdateRights"));
   #endif
    
    TBool ifHaveUpdateRights = EFalse;   
    if(iEnvironmentalReverb)
	  ifHaveUpdateRights = iEnvironmentalReverb->HaveUpdateRights();	    	
    
    return ifHaveUpdateRights;
	
   }



// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File

