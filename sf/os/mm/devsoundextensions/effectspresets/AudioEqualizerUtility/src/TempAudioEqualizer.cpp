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
* Description:   Implementation of the audio equalizer proxy class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "TempAudioEqualizer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTempAudioEqualizer::CTempAudioEqualizer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTempAudioEqualizer::CTempAudioEqualizer()
    {
    }

// Destructor
CTempAudioEqualizer::~CTempAudioEqualizer()
    {
	}

// -----------------------------------------------------------------------------
// CTempAudioEqualizer::NewL
// Static function for creating an instance of the Audio Equalizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CTempAudioEqualizer* CTempAudioEqualizer::NewL(TInt aNumberOfBands,CAudioEqualizer* aAudioEqualizer)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CTempAudioEqualizer::NewL"));
#endif

    CTempAudioEqualizer* self = new (ELeave) CTempAudioEqualizer();
    CleanupStack::PushL( self );
	self->ConstructL(aNumberOfBands,aAudioEqualizer);
	CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CTempAudioEqualizer::ConstructL
// -----------------------------------------------------------------------------
//
void CTempAudioEqualizer::ConstructL(TInt aNumberOfBands, CAudioEqualizer* aAudioEqualizer)
    {
#ifdef _DEBUG
    RDebug::Print(_L("CTempAudioEqualizer::ConstructL"));
#endif

	iAudioEqualizer = aAudioEqualizer;
	iNumberOfBands = aNumberOfBands;

	TEfAudioEqualizerBand bandData;
	for(TInt i=0; i< aNumberOfBands; i++)
		{
		if(iBandsData.Append(bandData))
			{
			User::Leave(KErrNoMemory);
			}
		}


	}


// -----------------------------------------------------------------------------
// CTempAudioEqualizer::ApplyL
// Apply the Audio Equalizer settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempAudioEqualizer::ApplyL()
	{
#ifdef _DEBUG
    RDebug::Print(_L("CTempAudioEqualizer::Apply"));
#endif

	for(TInt i = 1; i <= iNumberOfBands; i++)
		{
			if(BandLevel(i) != iAudioEqualizer->BandLevel(i))
				{
#ifdef _DEBUG
    RDebug::Print(_L("CTempAudioEqualizer::Band %d Level %d"),i,BandLevel(i));
#endif

					iAudioEqualizer->SetBandLevelL(i,BandLevel(i));
				}
		}
	if(iAudioEqualizer)
		{
		iAudioEqualizer->EnableL();
		//	iAudioEqualizer->ApplyL();
		}

	}


// -----------------------------------------------------------------------------
// CTempAudioEqualizer::EnableL
// Enable the Audio EnvironmentalReverb settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempAudioEqualizer::EnableL()
   {
    #ifdef _DEBUG
   	RDebug::Print(_L("CTempAudioEqualizer::EnableL"));
   	#endif
   	
   	iEnabled = ETrue;
    ApplyL();   
   }

// -----------------------------------------------------------------------------
// CTempAudioEqualizer::DisableL
// Disable the Audio EnvironmentalReverb settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempAudioEqualizer::DisableL()
   {
   #ifdef _DEBUG   
   	 RDebug::Print(_L("CTempAudioEqualizer::DisableL"));
   #endif
   	 
   	iEnabled = EFalse; 
	
	if(iAudioEqualizer)
	  iAudioEqualizer->DisableL();	    
   }

// -----------------------------------------------------------------------------
// CTempAudioEqualizer::IsEnableL
// Return enable flag.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CTempAudioEqualizer::IsEnabled() const
   {
    #ifdef _DEBUG 
   	 RDebug::Print(_L("CTempAudioEqualizer::IsEnabled"));
   #endif
   
    TBool ifEnabled = EFalse;
    if(iAudioEqualizer)
	   ifEnabled = iAudioEqualizer->IsEnabled();	    
    
    return ifEnabled;
   }



// -----------------------------------------------------------------------------
// CTempAudioEqualizer::IsEnforced
// Return enForce flag
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CTempAudioEqualizer::IsEnforced() const
   {
     #ifdef _DEBUG 
   RDebug::Print(_L("CTempAudioEqualizer::IsEnforced"));
   #endif
   
    TBool ifEnforced = EFalse;   
    if(iAudioEqualizer)
	  ifEnforced = iAudioEqualizer->IsEnforced();	    	
    
    return ifEnforced;
   
   }

// -----------------------------------------------------------------------------
// CTempAudioEqualizer::EnforceL
// Enforce the effect
// -----------------------------------------------------------------------------
//
EXPORT_C void CTempAudioEqualizer::EnforceL( TBool aEnforced )
   {
   #ifdef _DEBUG 
   RDebug::Print(_L("CTempAudioEqualizer::EnforceL"));
   #endif    

   iEnforced = aEnforced;
   
   if(iAudioEqualizer)
	  iAudioEqualizer->EnforceL(aEnforced);	   	
   }



// -----------------------------------------------------------------------------
// CTempAudioEqualizer::HaveUpdateRights
// Return enForce flag
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CTempAudioEqualizer::HaveUpdateRights() const
   {
   #ifdef _DEBUG 
   RDebug::Print(_L("CTempAudioEqualizer::HaveUpdateRights"));
   #endif
    
    TBool ifHaveUpdateRights = EFalse;   
    if(iAudioEqualizer)
	  ifHaveUpdateRights = iAudioEqualizer->HaveUpdateRights();	    	
    
    return ifHaveUpdateRights;
	
   }



// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
