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
* Description:   Utility to Access the Equalizer Effect
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <AudioEqualizerUtility.h>
#include <DrmAudioSamplePlayer.h>
#include "TempAudioEqualizer.h"
#include "AudioEqUtilityInternalCRKeys.h"
#include <videoplayer.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::CAudioEqualizerUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAudioEqualizerUtility::CAudioEqualizerUtility()
	: iAudioEqualizer(NULL),
	  iBassBoost(NULL),
	  iPresetRepository(NULL)
	{
    }


// Destructor
CAudioEqualizerUtility::~CAudioEqualizerUtility()
    {
		iPresetArray.Close();
		iFullPresetArray.Close();
		delete iPresetRepository;
		delete iTransAudioEqualizer;
		delete iAudioEqualizer;
		//delete iBassBoost;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CMMFDevSound& aDevSound)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aDevSound);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aDevSound,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CMdaAudioConvertUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aUtility);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aUtility,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CMdaAudioInputStream& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aUtility);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aUtility,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CMdaAudioOutputStream& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aUtility);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aUtility,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CMdaAudioPlayerUtility& aUtility)
    {
#ifdef _DEBUG
	    RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aUtility);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aUtility,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
	}

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CMdaAudioRecorderUtility& aUtility )
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aUtility,ETrue);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aUtility,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
   }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CMdaAudioToneUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aUtility);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aUtility,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CCustomCommandUtility* aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aUtility);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aUtility,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(MCustomInterface& aCustomInterface)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aCustomInterface);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aCustomInterface,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());

    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CMidiClientUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aUtility);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aUtility,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
	}

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CDrmPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aUtility);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aUtility,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
	}

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CVideoPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::ConstructL"));
#endif
		iAudioEqualizer = CAudioEqualizer::NewL(aUtility);
		TInt numberBands = iAudioEqualizer->NumberOfBands();

		iTransAudioEqualizer = CTempAudioEqualizer::NewL(numberBands,iAudioEqualizer);

		//iBassBoost = CBassBoost::NewL(aUtility,EFalse);
		iPresetRepository = CRepository::NewL(KCRUidAudioEqUtilityPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

		iTransAudioEqualizer->SetBandDataL(2,iAudioEqualizer->DoBandDataL(1,ETrue));
		iTransAudioEqualizer->SetEffectData(iAudioEqualizer->DoEffectData());
	}
	
// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CMMFDevSound& aDevSound)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aDevSound);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CMdaAudioConvertUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CMdaAudioInputStream& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CMdaAudioOutputStream& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CMdaAudioPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CMdaAudioRecorderUtility& aUtility)
    {
#ifdef _DEBUG
	    RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CMdaAudioToneUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CCustomCommandUtility* aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(MCustomInterface& aCustomInterface)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aCustomInterface);
		CleanupStack::Pop(self);
		return self;
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CMidiClientUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CDrmPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CVideoPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		RDebug::Print(_L("CAudioEqualizerUtility::NewL"));
#endif
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }
    
// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ApplyPresetL()
// Applies the Preset to the Equalizer Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEqualizerUtility::ApplyPresetL(
    TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::ApplyPresetL"));
#endif
		// Get the Preset from the Central Repository
		// and Apply it to the Equalizer Object
		// using SetBandData method. After that Just Enable() the Effect

		TBuf8<256> aDescription;
		TBufC8<256> aDes;
		TBool presetFound = EFalse;

		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		UpdateFullPresetArrayFromCenRep(iFullPresetArray);

		// Write the Equalizer Data in the first available NULL Space
		for( TInt j = 0; j < iFullPresetArray.Count();j++)
			{
				if( (iFullPresetArray[j].iPresetNameKey == iPresetArray[aPresetIndex].iPresetNameKey) &&
						(iFullPresetArray[j].iPresetName.Length()))
					{
						iPresetRepository->Get(iFullPresetArray[j].iPresetDescriptionKey,aDescription);
						aDes = aDescription;
						iAudioEqualizer->SetBandDataL(2,aDes);

						// Applying the Settings to the Audio Equalizer
						iAudioEqualizer->EnableL();
					//	iAudioEqualizer->ApplyL();
						presetFound =  ETrue;
						break;
					}
			}

		if(!presetFound)
			User::Leave(KErrNotFound);
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::CreatePresetL(TDesC& aName,CAudioEqualizer& aAudioEqualizer)
// Applies the Preset to the Equalizer Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CAudioEqualizerUtility:: CreatePresetL(
    TDesC& aName,
    CAudioEqualizer& /*aAudioEqualizerUI*/)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::CreatePresetL"));
#endif
		// Get the Preset from the UI and create a DesC using
		// DoBandData() method and then store it to the
		// Central Repository

		 __ASSERT_ALWAYS(aName.Length() > 0,User::Leave(KErrArgument));


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);

	//	iTransAudioEqualizer = &aAudioEqualizerUI;

				//Create New Keys at the End of the Repository
		TInt currentNumber = 0;
				// Get the number of User Defined Presets in the Repository
		iPresetRepository->Get(KAudioEqUtilityNumberOfUserDefinedPresets,currentNumber);

		HBufC8* presetHeap = HBufC8::NewL(iAudioEqualizer->NumberOfBands() * sizeof(TEfAudioEqualizerBand));
		TPtr8 des = presetHeap->Des();
		des.SetLength(presetHeap->Size());
		des.Append(iTransAudioEqualizer->DoBandDataL(2,ETrue));

				// Calculate and Create the Equalizer Data for the New Preset
		iPresetRepository->Create(currentNumber*2+KAudioEqUtilityUserDefinedPresetStart,des);
				// Calculate and create the New Preset Name in the End
		iPresetRepository->Create(currentNumber*2+KAudioEqUtilityUserDefinedPresetStartName,aName);
				// Update the Number of UserDefined Presets in the End
				// Since we created new Keys for the Name and the Descriptor
		currentNumber += 1;
		iPresetRepository->Set(KAudioEqUtilityNumberOfUserDefinedPresets,currentNumber);
		iPresetRepository->Get(KAudioEqUtilityPresetsCounter,currentNumber);
		iPresetRepository->Set(KAudioEqUtilityPresetsCounter,currentNumber + 1);

		// Refresh the iFullPresetArray
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		delete presetHeap;
		presetHeap = NULL;

    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::GetPresetL()
// Gets the Preset from the Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CAudioEqualizerUtility::GetPresetL(
    TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::GetPresetL"));
#endif
		// Get the Preset from the Central Repository
		// and Apply it to the Equalizer Object
		// using SetBandData method. After that Just Enable() the Effect


		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		TBuf8<256> aDescription;
		TBufC8<256> aDes;
		TBool presetFound = EFalse;

		UpdateFullPresetArrayFromCenRep(iFullPresetArray);

		// Write the Equalizer Data in the first available NULL Space
		for(TInt j = 0; j < iFullPresetArray.Count();j++)
			{
				if( (iFullPresetArray[j].iPresetNameKey == iPresetArray[aPresetIndex].iPresetNameKey) &&
						(iFullPresetArray[j].iPresetName.Length()))
					{
						iPresetRepository->Get(iFullPresetArray[j].iPresetDescriptionKey,aDescription);
						aDes = aDescription;

						iTransAudioEqualizer->SetBandDataL(2,aDes); // The First Argument should be > 1 for getting all bands
						presetFound = ETrue;
						return iFullPresetArray[j].iPresetName;
					}
			}

		if(!presetFound)
			User::Leave(KErrNotFound);

		return KNullDesC;

    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::DeletePresetL()
// Deletes the Preset from the Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEqualizerUtility::DeletePresetL(
    TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::DeletePresetL"));
#endif
		// Get the Name from the PresetIndex
		// This Preset Index is from the Array returned to the UI

		TInt err(KErrNone);
		TBool presetFound = EFalse;


        // Leave if it deletes a predefined preset  
		TInt currentNumberOfPredefinedPresets = 0;
				                
	    // Get the number of User Defined Presets in the Repository
		iPresetRepository->Get(KAudioEqUtilityNumberOfPreDefinedPresets,currentNumberOfPredefinedPresets);

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

        // update the KAudioEqUtilityNumberOfUserDefinedPresets in CR
		if(!presetFound)
			User::Leave(KErrNotFound);

		// After This Update the iPresetArray for the UI
		UpdatePresetArray(iPresetArray,iFullPresetArray);
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::DisablePresetL()
// Disable the Preset
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CAudioEqualizerUtility::DisableEqualizerL()
{
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::DisableEqualizerL"));
#endif

	if(iAudioEqualizer)
	{
		iAudioEqualizer->DisableL();
	}
}

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::Equalizer()
// Applies the Preset to the Equalizer Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer& CAudioEqualizerUtility::Equalizer()
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::Equalizer()"));
#endif

		return *iTransAudioEqualizer;
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::BassBoost()
// Applies the Preset to the Equalizer Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost& CAudioEqualizerUtility::BassBoost()
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::BassBoost()"));
#endif
		return *iBassBoost;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ModifyPresetL()
// Applies the Preset to the Equalizer Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEqualizerUtility::ModifyPresetL(
    TInt aPresetIndex,
    TDesC& aName,
    CAudioEqualizer& aAudioEqualizerUI)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::ModifyPresetL"));
#endif

		 __ASSERT_ALWAYS(aName.Length() > 0,User::Leave(KErrArgument));

		// Modifies the Preset Data in the Central Repository
		// with the updated Name and Equalizer Settings for the
		// given PresetIndex

		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		iTransAudioEqualizer = &aAudioEqualizerUI;

		// Write the Equalizer Data in the first available NULL Space
		for( TInt j = 0; j < iFullPresetArray.Count();j++)
			{
				if( iFullPresetArray[j].iPresetNameKey == iPresetArray[aPresetIndex].iPresetNameKey)
					{
						iPresetRepository->Set(iFullPresetArray[j].iPresetNameKey,aName);

						HBufC8* presetHeap = HBufC8::NewL(iAudioEqualizer->NumberOfBands() * sizeof(TEfAudioEqualizerBand));
						TPtr8 des = presetHeap->Des();
						des.SetLength(presetHeap->Size());
						des.Append(iTransAudioEqualizer->DoBandDataL(2,ETrue));

						iPresetRepository->Set(iFullPresetArray[j].iPresetDescriptionKey,des);
						delete presetHeap;
						presetHeap = NULL;
						break;
					}
			}
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NumberOfPreDefinedPresets()
// Applies the Preset to the Equalizer Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


EXPORT_C TUint32 CAudioEqualizerUtility::NumberOfPreDefinedPresets() const
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::NumberOfPreDefinedPresets"));
#endif

		TInt numberOfPresets;
		iPresetRepository->Get(KAudioEqUtilityNumberOfPreDefinedPresets,numberOfPresets);
		return numberOfPresets;
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::Presets()
// Applies the Preset to the Equalizer Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TArray<TEfAudioEqualizerUtilityPreset> CAudioEqualizerUtility::Presets()
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::Presets"));
#endif

		// returns all the Presets from the System Defined and
		// User Defined Preset Array in the Central Repository
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		return iPresetArray.Array();
    }


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ResetPresetL(TInt aPresetIndex)
// Applies the Preset to the Equalizer Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEqualizerUtility::ResetPresetL(TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CAudioEqualizerUtility::ResetPresetL"));
#endif

		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		TInt currentNumber = 0;
		// Get the number of System Defined Presets in the Repository
		iPresetRepository->Get(KAudioEqUtilityNumberOfPreDefinedPresets,currentNumber);

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
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
	}


// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::UpdatePresetArrayFromCenRep(RArray<TEfAudioEqualizerUtilityPreset> &aFullPresetArray)
// Updates the Array with the Values in Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CAudioEqualizerUtility::UpdateFullPresetArrayFromCenRep(RArray<TEfAudioEqualizerUtilityPreset>& aFullPresetArray)
	{

		aFullPresetArray.Reset();
 		TEfAudioEqualizerUtilityPreset tempPreset;


		TInt numberOfUserDefinedPresets = 0;
		TInt numberOfPreDefinedPresets = 0;

		iPresetRepository->Get(KAudioEqUtilityNumberOfUserDefinedPresets,numberOfUserDefinedPresets);
		iPresetRepository->Get(KAudioEqUtilityNumberOfPreDefinedPresets,numberOfPreDefinedPresets);

		for( TInt j = 0; j < (numberOfPreDefinedPresets); j++)
				{
					tempPreset.iPresetDescriptionKey = KAudioEqUtilityPreDefinedPresetStart + (2 * j);
					tempPreset.iPresetNameKey = KAudioEqUtilityPreDefinedPresetStartName + (2 * j);
					iPresetRepository->Get((KAudioEqUtilityPreDefinedPresetStartName + (2 * j)) ,tempPreset.iPresetName);
#ifdef _DEBUG
					RDebug::RawPrint(tempPreset.iPresetName);
#endif
					aFullPresetArray.Append(tempPreset);

				}

			for( TInt j = 0; j < (numberOfUserDefinedPresets); j++)
				{
					tempPreset.iPresetDescriptionKey = KAudioEqUtilityUserDefinedPresetStart + (2 * j);
					tempPreset.iPresetNameKey = KAudioEqUtilityUserDefinedPresetStartName + (2 * j);
					iPresetRepository->Get((KAudioEqUtilityUserDefinedPresetStartName + (2 * j)) ,tempPreset.iPresetName);
#ifdef _DEBUG
					RDebug::RawPrint(tempPreset.iPresetName);
#endif
					aFullPresetArray.Append(tempPreset);

				}

	}

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::UpdatePresetArray(RArray<TDesC>& aPresetArray,const RArray<TEfAudioEqualizerUtilityPreset> &aFullPresetArray)
// Creates the Descriptor Array of Names of the Presets Stored in Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::UpdatePresetArray(RArray<TEfAudioEqualizerUtilityPreset>& aPresetArray,const RArray<TEfAudioEqualizerUtilityPreset>& aFullPresetArray)
	{

		aPresetArray.Reset();
		TEfAudioEqualizerUtilityPreset	tempClientArray;

		// Write the Equalizer Data in the first available NULL Space
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
