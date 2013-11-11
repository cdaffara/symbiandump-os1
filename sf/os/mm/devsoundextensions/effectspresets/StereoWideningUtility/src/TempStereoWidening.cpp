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
* Description:   Implementation of the audio StereoWidening proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "TempStereoWidening.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTempStereoWidening::CTempStereoWidening
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTempStereoWidening::CTempStereoWidening()
    {
    }

// Destructor
CTempStereoWidening::~CTempStereoWidening()
    {
	}

// -----------------------------------------------------------------------------
// CTempStereoWidening::NewL
// Static function for creating an instance of the Audio StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CTempStereoWidening* CTempStereoWidening::NewL(TInt aLevel,CStereoWidening* aStereoWidening)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CTempStereoWidening::NewL"));
#endif

    CTempStereoWidening* self = new (ELeave) CTempStereoWidening();
    CleanupStack::PushL( self );
	self->ConstructL(aLevel,aStereoWidening);
	CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CTempStereoWidening::ConstructL
// -----------------------------------------------------------------------------
//
void CTempStereoWidening::ConstructL(TInt aLevel, CStereoWidening* aStereoWidening)
    {
#ifdef _DEBUG
    RDebug::Print(_L("CTempStereoWidening::ConstructL"));
#endif

	iStereoWidening = aStereoWidening;
	iStereoWideningData.iLevel = aLevel;
	}


// -----------------------------------------------------------------------------
// CTempStereoWidening::ApplyL
// Apply the Audio StereoWidening settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempStereoWidening::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CTempStereoWidening::Apply"));
#endif

    if (StereoWideningLevel() !=  iStereoWidening->StereoWideningLevel())
       iStereoWidening->SetStereoWideningLevelL(StereoWideningLevel());

	if(iStereoWidening)
	  iStereoWidening->EnableL();

//		iStereoWidening->ApplyL();

	}


// -----------------------------------------------------------------------------
// CTempStereoWidening::EnableL
// Enable the Audio StereoWidening settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempStereoWidening::EnableL()
   {
    #ifdef _DEBUG
   	RDebug::Print(_L("CTempStereoWidening::EnableL"));
   	#endif
   	
   	iEnabled = ETrue;
    ApplyL();  
   }

// -----------------------------------------------------------------------------
// CTempStereoWidening::DisableL
// Disable the Audio StereoWidening settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempStereoWidening::DisableL()
   {
   #ifdef _DEBUG   
   	 RDebug::Print(_L("CTempStereoWidening::DisableL"));
   #endif
   	 
   iEnabled = EFalse; 	
	
   if(iStereoWidening)
	 iStereoWidening->DisableL();	    
   
   }

// -----------------------------------------------------------------------------
// CTempStereoWidening::IsEnableL
// Return enable flag.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CTempStereoWidening::IsEnabled() const
   {

   #ifdef _DEBUG 
   RDebug::Print(_L("CTempStereoWidening::IsEnabled"));
   #endif   
   
    TBool ifEnabled = EFalse;
    if(iStereoWidening)
	   ifEnabled = iStereoWidening->IsEnabled();	    
    return ifEnabled;
   }



// -----------------------------------------------------------------------------
// CTempStereoWidening::IsEnforced
// Return enForce flag
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CTempStereoWidening::IsEnforced() const
   {
   #ifdef _DEBUG 
   RDebug::Print(_L("CTempStereoWidening::IsEnforced"));
   #endif
   
   
    TBool ifEnforced = EFalse;   
    if(iStereoWidening)
	  ifEnforced = iStereoWidening->IsEnforced();	    	
    return ifEnforced;
   
   }

// -----------------------------------------------------------------------------
// CTempStereoWidening::EnforceL
// Enforce the effect
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempStereoWidening::EnforceL( TBool aEnforced )
   {
   #ifdef _DEBUG 
   RDebug::Print(_L("CTempStereoWidening::EnforceL"));
   #endif    

   iEnforced = aEnforced;
   
   if(iStereoWidening)
	  iStereoWidening->EnforceL(aEnforced);	   	
   }



// -----------------------------------------------------------------------------
// CTempStereoWidening::HaveUpdateRights
// Return enForce flag
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CTempStereoWidening::HaveUpdateRights() const
   {
   #ifdef _DEBUG 
   RDebug::Print(_L("CTempStereoWidening::HaveUpdateRights"));
   #endif
    
    TBool ifHaveUpdateRights = EFalse;   
    if(iStereoWidening)
	  ifHaveUpdateRights = iStereoWidening->HaveUpdateRights();	    	
    
    return ifHaveUpdateRights;
	
   }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
