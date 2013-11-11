/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Utility to Access the EnvironmentalReverb Effect
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <EnvironmentalReverbUtility.h>
#include <DrmAudioSamplePlayer.h>
#include "TempEnvironmentalReverb.h"
#include "EnvironmentalReverbUtilityInternalCRKeys.h"
#include <videoplayer.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::CEnvironmentalReverbUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEnvironmentalReverbUtility::CEnvironmentalReverbUtility()
	: iEnvironmentalReverb(NULL),
	  iPresetRepository(NULL)
	{
    }


// Destructor
CEnvironmentalReverbUtility::~CEnvironmentalReverbUtility()
    {
		iPresetArray.Close();
		iFullPresetArray.Close();
		delete iPresetRepository;
		delete iTransEnvironmentalReverb;
		delete iEnvironmentalReverb;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CMMFDevSound& aDevSound)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aDevSound);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CMdaAudioConvertUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aUtility);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CMdaAudioInputStream& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aUtility);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
   }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CMdaAudioOutputStream& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aUtility);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CMdaAudioPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aUtility);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CMdaAudioRecorderUtility& aUtility )
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aUtility,ETrue);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
   }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CMdaAudioToneUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aUtility);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CCustomCommandUtility* aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aUtility);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(MCustomInterface& aCustomInterface)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aCustomInterface);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());

    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CMidiClientUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aUtility);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CDrmPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aUtility);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::ConstructL(CVideoPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CEnvironmentalReverbUtility::ConstructL"));
#endif
		iEnvironmentalReverb = CEnvironmentalReverb::NewL(aUtility);
		iTransEnvironmentalReverb = CTempEnvironmentalReverb::NewL(iEnvironmentalReverb);
		iPresetRepository = CRepository::NewL(KCRUidEnvironmentalReverbUtilityPresets);
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		iTransEnvironmentalReverb->SetEffectData(iEnvironmentalReverb->DoEffectData());
	}
	
// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CMMFDevSound& aDevSound)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aDevSound);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CMdaAudioConvertUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CMdaAudioInputStream& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CMdaAudioOutputStream& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CMdaAudioPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CMdaAudioRecorderUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CMdaAudioToneUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CCustomCommandUtility* aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(MCustomInterface& aCustomInterface)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aCustomInterface);
		CleanupStack::Pop(self);
		return self;
    }



EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CMidiClientUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }

EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CDrmPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }

EXPORT_C CEnvironmentalReverbUtility* CEnvironmentalReverbUtility::NewL(CVideoPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NewL"));
#endif
		CEnvironmentalReverbUtility* self = new( ELeave ) CEnvironmentalReverbUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }
    
// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ApplyPresetL()
// Applies the Preset to the EnvironmentalReverb Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverbUtility::ApplyPresetL(
    TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ApplyPresetL"));
#endif
		// Get the Preset from the Central Repository
		// and Apply it to the EnvironmentalReverb Object
		// using SetBandData method. After that Just Enable() the Effect

		TBuf8<256> aDescription;
		TBufC8<256> aDes;
		TBool presetFound = EFalse;

		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		UpdateFullPresetArrayFromCenRep(iFullPresetArray);

		// Write the EnvironmentalReverb Data in the first available NULL Space
		for( TInt j = 0; j < iFullPresetArray.Count();j++)
			{
				if( (iFullPresetArray[j].iPresetNameKey == iPresetArray[aPresetIndex].iPresetNameKey) &&
						(iFullPresetArray[j].iPresetName.Length()))
					{
						iPresetRepository->Get(iFullPresetArray[j].iPresetDescriptionKey,aDescription);
						aDes = aDescription;
					

						iEnvironmentalReverb->SetEffectData(aDes);

						// Applying the Settings to the Audio EnvironmentalReverb
					//	iEnvironmentalReverb->ApplyL();

	                                        iEnvironmentalReverb->EnableL();

						presetFound =  ETrue;
						break;
					}
			}

		if(!presetFound)
			User::Leave(KErrNotFound);
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::CreatePresetL(TDesC& aName,CEnvironmentalReverb& aEnvironmentalReverb)
// Applies the Preset to the EnvironmentalReverb Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CEnvironmentalReverbUtility:: CreatePresetL(
    TDesC& aName,
    CEnvironmentalReverb& aEnvironmentalReverbUI)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::CreatePresetL"));
#endif
		// Get the Preset from the UI and create a DesC using
		// DoBandData() method and then store it to the
		// Central Repository

		 __ASSERT_ALWAYS(aName.Length() > 0,User::Leave(KErrArgument));


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);

		iEnvironmentalReverb = &aEnvironmentalReverbUI;

				//Create New Keys at the End of the Repository
		TInt currentNumber = 0;
				// Get the number of User Defined Presets in the Repository
		iPresetRepository->Get(KEnvironmentalReverbUtilityNumberOfUserDefinedPresets,currentNumber);

		HBufC8* presetHeap = HBufC8::NewL(sizeof(TEfEnvironmentalReverb));
		TPtr8 des = presetHeap->Des();
		des.SetLength(presetHeap->Size());
		des.Append(iEnvironmentalReverb->DoEffectData());

				// Calculate and Create the EnvironmentalReverb Data for the New Preset
		iPresetRepository->Create(currentNumber*2+KEnvironmentalReverbUtilityUserDefinedPresetStart,des);
				// Calculate and create the New Preset Name in the End
		iPresetRepository->Create(currentNumber*2+KEnvironmentalReverbUtilityUserDefinedPresetStartName,aName);
				// Update the Number of UserDefined Presets in the End
				// Since we created new Keys for the Name and the Descriptor
		currentNumber += 1;
		iPresetRepository->Set(KEnvironmentalReverbUtilityNumberOfUserDefinedPresets,currentNumber);
		iPresetRepository->Get(KEnvironmentalReverbUtilityPresetsCounter,currentNumber);
		iPresetRepository->Set(KEnvironmentalReverbUtilityPresetsCounter,currentNumber + 1);

		// Refresh the iFullPresetArray
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		delete presetHeap;
		presetHeap = NULL;

    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::GetPresetL()
// Gets the Preset from the Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CEnvironmentalReverbUtility::GetPresetL(
    TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::GetPresetL"));
#endif
		// Get the Preset from the Central Repository
		// and Apply it to the EnvironmentalReverb Object
		// using SetBandData method. After that Just Enable() the Effect


		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		TBuf8<256> aDescription;
		TBufC8<256> aDes;
		TBool presetFound = EFalse;

		UpdateFullPresetArrayFromCenRep(iFullPresetArray);

		// Write the EnvironmentalReverb Data in the first available NULL Space
		for(TInt j = 0; j < iFullPresetArray.Count();j++)
			{
				if( (iFullPresetArray[j].iPresetNameKey == iPresetArray[aPresetIndex].iPresetNameKey) &&
						(iFullPresetArray[j].iPresetName.Length()))
					{
						iPresetRepository->Get(iFullPresetArray[j].iPresetDescriptionKey,aDescription);
						aDes = aDescription;

						iEnvironmentalReverb->SetEffectData(aDes); // The First Argument should be > 1 for getting all bands
						presetFound = ETrue;
						return iFullPresetArray[j].iPresetName;
					}
			}

		if(!presetFound)
		    {		    
			User::Leave(KErrNotFound);
		    }
		    
		return KNullDesC;

    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::DeletePresetL()
// Deletes the Preset from the Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverbUtility::DeletePresetL(
    TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::DeletePresetL"));
#endif
		// Get the Name from the PresetIndex
		// This Preset Index is from the Array returned to the UI

		TInt err(KErrNone);
		TBool presetFound = EFalse;


        // Leave if it deletes a predefined preset  
		TInt currentNumberOfPredefinedPresets = 0;
				                
	    // Get the number of User Defined Presets in the Repository
		iPresetRepository->Get(KEnvironmentalReverbUtilityNumberOfPreDefinedPresets,currentNumberOfPredefinedPresets);

        if (aPresetIndex < currentNumberOfPredefinedPresets)  //it is a predefined preset, 
        {
			User::Leave(KErrArgument);        	
        }
        
        
		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

#ifdef _DEBUG
		RDebug::RawPrint(iPresetArray[aPresetIndex].iPresetName);
#endif

		// Deletes the Preset from the iFullPresetArray and then
		// rewrite it to the Central Repository
		for(TInt j = 0; j < iFullPresetArray.Count();j++)
			{

				if( (iFullPresetArray[j].iPresetNameKey == iPresetArray[aPresetIndex].iPresetNameKey) &&
						(iFullPresetArray[j].iPresetName.Length()))
					{
					
						presetFound = ETrue;
						TRAP(err,iPresetRepository->Set(iFullPresetArray[j].iPresetNameKey,KNullDesC));
						TRAP(err,iPresetRepository->Set(iFullPresetArray[j].iPresetDescriptionKey,KNullDesC));
						if(err != KErrNone)
							{
								break;
							}
						else
							{
								iFullPresetArray[j].iPresetName.FillZ(0);
								iFullPresetArray[j].iPresetNameKey = 0;
								iFullPresetArray[j].iPresetDescriptionKey = 0;
						        break;
						        
							}

					}
#ifdef _DEBUG
				RDebug::RawPrint(iFullPresetArray[j].iPresetName);
#endif
			}

		if(!presetFound)
		    {		    
			User::Leave(KErrNotFound);
		    }

		// After This Update the iPresetArray for the UI
		UpdatePresetArray(iPresetArray,iFullPresetArray);
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::DisablePresetL()
// Disable the Preset
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CEnvironmentalReverbUtility::DisableEnvironmentalReverbL()
{
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::DisableEnvironmentalReverbL"));
#endif

	if(iEnvironmentalReverb)
	{
		iEnvironmentalReverb->DisableL();
	}
}

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::EnvironmentalReverb()
// Applies the Preset to the EnvironmentalReverb Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb& CEnvironmentalReverbUtility::EnvironmentalReverb()
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::EnvironmentalReverb()"));
#endif

		return *iEnvironmentalReverb;
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ModifyPresetL()
// Applies the Preset to the EnvironmentalReverb Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverbUtility::ModifyPresetL(
    TInt aPresetIndex,
    TDesC& aName,
    CEnvironmentalReverb& aEnvironmentalReverbUI)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ModifyPresetL"));
#endif

		 __ASSERT_ALWAYS(aName.Length() > 0,User::Leave(KErrArgument));

		// Modifies the Preset Data in the Central Repository
		// with the updated Name and EnvironmentalReverb Settings for the
		// given PresetIndex

		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		iEnvironmentalReverb = &aEnvironmentalReverbUI;

		// Write the EnvironmentalReverb Data in the first available NULL Space
		for( TInt j = 0; j < iFullPresetArray.Count();j++)
			{
				if( iFullPresetArray[j].iPresetNameKey == iPresetArray[aPresetIndex].iPresetNameKey)
					{
						iPresetRepository->Set(iFullPresetArray[j].iPresetNameKey,aName);

						HBufC8* presetHeap = HBufC8::NewL(sizeof(TEfEnvironmentalReverb));
						TPtr8 des = presetHeap->Des();
						des.SetLength(presetHeap->Size());
						des.Append(iEnvironmentalReverb->DoEffectData());

						iPresetRepository->Set(iFullPresetArray[j].iPresetDescriptionKey,des);
						delete presetHeap;
						presetHeap = NULL;
						break;
					}
			}
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::NumberOfPreDefinedPresets()
// Applies the Preset to the EnvironmentalReverb Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


EXPORT_C TUint32 CEnvironmentalReverbUtility::NumberOfPreDefinedPresets() const
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::NumberOfPreDefinedPresets"));
#endif

		TInt numberOfPresets;
		iPresetRepository->Get(KEnvironmentalReverbUtilityNumberOfPreDefinedPresets,numberOfPresets);
		return numberOfPresets;
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::Presets()
// Applies the Preset to the EnvironmentalReverb Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TArray<TEfEnvironmentalReverbUtilityPreset> CEnvironmentalReverbUtility::Presets()
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::Presets"));
#endif

		// returns all the Presets from the System Defined and
		// User Defined Preset Array in the Central Repository
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		return iPresetArray.Array();
    }


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::ResetPresetL(TInt aPresetIndex)
// Applies the Preset to the EnvironmentalReverb Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverbUtility::ResetPresetL(TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CEnvironmentalReverbUtility::ResetPresetL"));
#endif

		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		TInt currentNumber = 0;
		// Get the number of System Defined Presets in the Repository
		iPresetRepository->Get(KEnvironmentalReverbUtilityNumberOfPreDefinedPresets,currentNumber);

		if(aPresetIndex >= currentNumber)
			User::Leave(KErrNotSupported);


		for( TInt j = 0; j < currentNumber;j++)
				{
					if(  iFullPresetArray[j].iPresetNameKey == iPresetArray[aPresetIndex].iPresetNameKey)
						{
							iPresetRepository->Reset(iFullPresetArray[j].iPresetDescriptionKey);
							iPresetRepository->Reset(iFullPresetArray[j].iPresetNameKey);
						}
				}

		UpdatePresetArray(iPresetArray,iFullPresetArray);
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::UpdatePresetArrayFromCenRep(RArray<TEfEnvironmentalReverbUtilityPreset> &aFullPresetArray)
// Updates the Array with the Values in Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CEnvironmentalReverbUtility::UpdateFullPresetArrayFromCenRep(RArray<TEfEnvironmentalReverbUtilityPreset>& aFullPresetArray)
	{

		aFullPresetArray.Reset();
 		TEfEnvironmentalReverbUtilityPreset tempPreset;


		TInt numberOfUserDefinedPresets = 0;
		TInt numberOfPreDefinedPresets = 0;

		iPresetRepository->Get(KEnvironmentalReverbUtilityNumberOfUserDefinedPresets,numberOfUserDefinedPresets);
		iPresetRepository->Get(KEnvironmentalReverbUtilityNumberOfPreDefinedPresets,numberOfPreDefinedPresets);

		for( TInt j = 0; j < (numberOfPreDefinedPresets); j++)
				{
					tempPreset.iPresetDescriptionKey = KEnvironmentalReverbUtilityPreDefinedPresetStart + (2 * j);
					tempPreset.iPresetNameKey = KEnvironmentalReverbUtilityPreDefinedPresetStartName + (2 * j);
					iPresetRepository->Get((KEnvironmentalReverbUtilityPreDefinedPresetStartName + (2 * j)) ,tempPreset.iPresetName);
#ifdef _DEBUG
					RDebug::RawPrint(tempPreset.iPresetName);
#endif
					aFullPresetArray.Append(tempPreset);

				}

			for( TInt j = 0; j < (numberOfUserDefinedPresets); j++)
				{
					tempPreset.iPresetDescriptionKey = KEnvironmentalReverbUtilityUserDefinedPresetStart + (2 * j);
					tempPreset.iPresetNameKey = KEnvironmentalReverbUtilityUserDefinedPresetStartName + (2 * j);
					iPresetRepository->Get((KEnvironmentalReverbUtilityUserDefinedPresetStartName + (2 * j)) ,tempPreset.iPresetName);
#ifdef _DEBUG
					RDebug::RawPrint(tempPreset.iPresetName);
#endif
					aFullPresetArray.Append(tempPreset);

				}

	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverbUtility::UpdatePresetArray(RArray<TDesC>& aPresetArray,const RArray<TEfEnvironmentalReverbUtilityPreset> &aFullPresetArray)
// Creates the Descriptor Array of Names of the Presets Stored in Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEnvironmentalReverbUtility::UpdatePresetArray(RArray<TEfEnvironmentalReverbUtilityPreset>& aPresetArray,const RArray<TEfEnvironmentalReverbUtilityPreset>& aFullPresetArray)
	{

		aPresetArray.Reset();
		TEfEnvironmentalReverbUtilityPreset	tempClientArray;

		// Write the EnvironmentalReverb Data in the first available NULL Space
		for( TInt j = 0; j < aFullPresetArray.Count();j++)
			{
				if( aFullPresetArray[j].iPresetName.Length() != 0)
					{

						tempClientArray.iPresetName = aFullPresetArray[j].iPresetName;
						tempClientArray.iPresetNameKey = aFullPresetArray[j].iPresetNameKey;

						aPresetArray.Append(tempClientArray);
					}
#ifdef _DEBUG
				RDebug::Print(_L("Name Length %d "),aFullPresetArray[j].iPresetName.Length());
				RDebug::RawPrint(aFullPresetArray[j].iPresetName);
#endif
			}

	}


// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File
