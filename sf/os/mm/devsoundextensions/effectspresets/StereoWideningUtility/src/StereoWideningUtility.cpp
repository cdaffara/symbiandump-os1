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
* Description:   Utility to Access the StereoWidening Effect
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <StereoWideningUtility.h>
#include <DrmAudioSamplePlayer.h>
//#include <midiclientutility.h>
#include "TempStereoWidening.h"
#include "StereoWideningInternalCRKeys.h"
#include <videoplayer.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStereoWideningUtility::CStereoWideningUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CStereoWideningUtility::CStereoWideningUtility()
	: iStereoWidening(NULL),
	  iPresetRepository(NULL)
	{
    }


// Destructor
CStereoWideningUtility::~CStereoWideningUtility()
    {
		iPresetArray.Close();
		iFullPresetArray.Close();
		delete iPresetRepository;
		delete iTransStereoWidening;
		delete iStereoWidening;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CMMFDevSound& aDevSound)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aDevSound);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CMdaAudioConvertUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aUtility);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
    }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CMdaAudioInputStream& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aUtility);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
    }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CMdaAudioOutputStream& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aUtility);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
    }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CMdaAudioPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aUtility);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
	}

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CMdaAudioRecorderUtility& aUtility )
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aUtility,ETrue,EFalse,0);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
   }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CMdaAudioToneUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aUtility);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
    }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CCustomCommandUtility* aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aUtility);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
    }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(MCustomInterface& aCustomInterface)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aCustomInterface);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);

    }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CMidiClientUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aUtility);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
	}

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CDrmPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aUtility);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
	}

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::ConstructL(CVideoPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ConstructL"));
#endif
		iStereoWidening = CStereoWidening::NewL(aUtility);
		TInt level = iStereoWidening->StereoWideningLevel();

		iTransStereoWidening = CTempStereoWidening::NewL(level,iStereoWidening);

		iPresetRepository = CRepository::NewL(KCRUidStereoWideningPresets);


		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
		UpdatePresetArray(iPresetArray,iFullPresetArray);
	}
	
// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWideningUtility* CStereoWideningUtility::NewL(CMMFDevSound& aDevSound)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aDevSound);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWideningUtility* CStereoWideningUtility::NewL(CMdaAudioConvertUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWideningUtility* CStereoWideningUtility::NewL(CMdaAudioInputStream& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWideningUtility* CStereoWideningUtility::NewL(CMdaAudioOutputStream& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWideningUtility* CStereoWideningUtility::NewL(CMdaAudioPlayerUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWideningUtility* CStereoWideningUtility::NewL(CMdaAudioRecorderUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWideningUtility* CStereoWideningUtility::NewL(CMdaAudioToneUtility& aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWideningUtility* CStereoWideningUtility::NewL(CCustomCommandUtility* aUtility)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWideningUtility* CStereoWideningUtility::NewL(MCustomInterface& aCustomInterface)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aCustomInterface);
		CleanupStack::Pop(self);
		return self;
    }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C  CStereoWideningUtility* CStereoWideningUtility::NewL(CMidiClientUtility& aUtility)
{
	
	#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
}

// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C  CStereoWideningUtility* CStereoWideningUtility::NewL(CDrmPlayerUtility& aUtility)
{
	
	#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
}

// -----------------------------------------------------------------------------
// CStereoWideningUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C  CStereoWideningUtility* CStereoWideningUtility::NewL(CVideoPlayerUtility& aUtility)
{
	
	#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NewL"));
#endif
		CStereoWideningUtility* self = new( ELeave ) CStereoWideningUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
}

// -----------------------------------------------------------------------------
// CStereoWideningUtility::ApplyPresetL()
// Applies the Preset to the StereoWidening Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CStereoWideningUtility::ApplyPresetL(
    TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ApplyPresetL"));
#endif
		// Get the Preset from the Central Repository
		// and Apply it to the StereoWidening Object
		// using SetBandData method. After that Just Enable() the Effect

		TInt level;

		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		// Write the StereoWidening Data in the first available NULL Space
		for( TInt j = 0; j < iFullPresetArray.Count();j++)
			{
				if( iFullPresetArray[j].iPresetNameKey == iPresetArray[aPresetIndex].iPresetNameKey)
					{
						iPresetRepository->Get(iFullPresetArray[j].iPresetDescriptionKey,level);
						iStereoWidening->SetStereoWideningLevelL(level);
						// Applying the Settings to the Audio StereoWidening
					//	iStereoWidening->ApplyL();

                                                iStereoWidening->EnableL();
						break;
					}
			}
    }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::CreatePresetL(TDesC& aName,CStereoWidening& aStereoWidening)
// Applies the Preset to the StereoWidening Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CStereoWideningUtility:: CreatePresetL(
    TDesC& aName,
    CStereoWidening& aStereoWideningUI)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::CreatePresetL"));
#endif
		// Get the Preset from the UI and create a DesC using
		// DoBandData() method and then store it to the
		// Central Repository

		 __ASSERT_ALWAYS(aName.Length() > 0,User::Leave(KErrArgument));
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);

		iTransStereoWidening = &aStereoWideningUI;

				//Create New Keys at the End of the Repository
		TInt currentNumber = 0;
				// Get the number of User Defined Presets in the Repository
		iPresetRepository->Get(KStereoWideningNumberOfUserDefinedPresets,currentNumber);

		TUint8 level = iTransStereoWidening->StereoWideningLevel();

				// Calculate and Create the StereoWidening Data for the New Preset
		iPresetRepository->Create(currentNumber*2+KStereoWideningUserDefinedPresetStart,level);
				// Calculate and create the New Preset Name in the End
		iPresetRepository->Create(currentNumber*2+KStereoWideningUserDefinedPresetStartName,aName);
				// Update the Number of UserDefined Presets in the End
				// Since we created new Keys for the Name and the Descriptor
		currentNumber += 1;
		iPresetRepository->Set(KStereoWideningNumberOfUserDefinedPresets,currentNumber);
		iPresetRepository->Get(KStereoWideningPresetsCounter,currentNumber);
		iPresetRepository->Set(KStereoWideningPresetsCounter,currentNumber + 1);

		// Refresh the iFullPresetArray
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);

    }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::DeletePresetL()
// Deletes the Preset from the Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CStereoWideningUtility::GetPresetL(
    TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::GetPresetL"));
#endif
		// Get the Preset from the Central Repository
		// and Apply it to the StereoWidening Object
		// using SetBandData method. After that Just Enable() the Effect


		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		TBool presetFound = EFalse;
		TInt level;

		// Write the StereoWidening Data in the first available NULL Space
		for(TInt j = 0; j < iFullPresetArray.Count();j++)
			{
				if( iFullPresetArray[j].iPresetNameKey == iPresetArray[aPresetIndex].iPresetNameKey)
					{
						iPresetRepository->Get(iFullPresetArray[j].iPresetDescriptionKey,level);
						iTransStereoWidening->SetStereoWideningLevelL(level);
						presetFound = ETrue;
						return iFullPresetArray[j].iPresetName;
					}
			}
		if(!presetFound)
			User::Leave(KErrNotFound);

		return KNullDesC;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::DeletePresetL()
// Deletes the Preset from the Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CStereoWideningUtility::DeletePresetL(
    TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::DeletePresetL"));
#endif
		// Get the Name from the PresetIndex
		// This Preset Index is from the Array returned to the UI

		TInt err(KErrNone);
		TBool presetFound = EFalse;

        // Leave if it deletes a predefined preset  
		TInt currentNumberOfPredefinedPresets = 0;
				                
	    // Get the number of User Defined Presets in the Repository
		iPresetRepository->Get(KStereoWideningNumberOfPreDefinedPresets,currentNumberOfPredefinedPresets);

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
			User::Leave(KErrNotFound);
		// After This Update the iPresetArray for the UI
		UpdatePresetArray(iPresetArray,iFullPresetArray);
    }

// -----------------------------------------------------------------------------
// CStereoWideningUtility::DisablePresetL()
// Disable the Preset
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C void CStereoWideningUtility::DisableStereoWideningL()
{
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::DisableStereoWideningL"));
#endif

	if(iStereoWidening)
	{
		iStereoWidening->DisableL();
	}
}

// -----------------------------------------------------------------------------
// CStereoWideningUtility::StereoWidening()
// Applies the Preset to the StereoWidening Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening& CStereoWideningUtility::StereoWidening()
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::StereoWidening()"));
#endif

		return *iTransStereoWidening;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::ModifyPresetL()
// Applies the Preset to the StereoWidening Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CStereoWideningUtility::ModifyPresetL(
    TInt aPresetIndex,
    TDesC& aName,
    CStereoWidening& aStereoWideningUI)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ModifyPresetL"));
#endif

		 __ASSERT_ALWAYS(aName.Length() > 0,User::Leave(KErrArgument));
		// Modifies the Preset Data in the Central Repository
		// with the updated Name and StereoWidening Settings for the
		// given PresetIndex

		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		TUint8 level;
		TBuf<32> presetName;
		presetName = iPresetArray[aPresetIndex].iPresetName;

		iTransStereoWidening = &aStereoWideningUI;

		// Write the StereoWidening Data in the first available NULL Space
		for( TInt j = 0; j < iFullPresetArray.Count();j++)
			{
				if( iFullPresetArray[j].iPresetName == presetName)
					{
						iPresetRepository->Set(iFullPresetArray[j].iPresetNameKey,aName);
						level = iTransStereoWidening->StereoWideningLevel();
						iPresetRepository->Set(iFullPresetArray[j].iPresetDescriptionKey,level);
						break;
					}
			}
		UpdateFullPresetArrayFromCenRep(iFullPresetArray);
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::NumberOfPreDefinedPresets()
// Applies the Preset to the StereoWidening Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


EXPORT_C TUint32 CStereoWideningUtility::NumberOfPreDefinedPresets() const
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::NumberOfPreDefinedPresets"));
#endif

		TInt numberOfPresets;
		iPresetRepository->Get(KStereoWideningNumberOfPreDefinedPresets,numberOfPresets);
		return numberOfPresets;
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::Presets()
// Applies the Preset to the StereoWidening Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TArray<TEfStereoWideningUtilityPreset> CStereoWideningUtility::Presets()
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::Presets"));
#endif

		// returns all the Presets from the System Defined and
		// User Defined Preset Array in the Central Repository
		UpdatePresetArray(iPresetArray,iFullPresetArray);
		return iPresetArray.Array();
    }


// -----------------------------------------------------------------------------
// CStereoWideningUtility::ResetPresetL(TInt aPresetIndex)
// Applies the Preset to the StereoWidening Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CStereoWideningUtility::ResetPresetL(TInt aPresetIndex)
    {
#ifdef _DEBUG
		    RDebug::Print(_L("CStereoWideningUtility::ResetPresetL"));
#endif

		if(aPresetIndex > (iPresetArray.Count() - 1))
			User::Leave(KErrArgument);

		TBuf<32> aName;
		aName = iPresetArray[aPresetIndex].iPresetName;

		TInt currentNumber = 0;
		// Get the number of System Defined Presets in the Repository
		iPresetRepository->Get(KStereoWideningNumberOfPreDefinedPresets,currentNumber);

		if(aPresetIndex >= currentNumber)
			User::Leave(KErrNotSupported);


		for( TInt j = 0; j < currentNumber;j++)
				{
					if( iFullPresetArray[j].iPresetName == aName)
						{
							iPresetRepository->Reset(iFullPresetArray[j].iPresetDescriptionKey);
							iPresetRepository->Reset(iFullPresetArray[j].iPresetNameKey);
						}
				}

		UpdatePresetArray(iPresetArray,iFullPresetArray);
	}


// -----------------------------------------------------------------------------
// CStereoWideningUtility::UpdatePresetArrayFromCenRep(RArray<TEfStereoWideningUtilityPreset> &aFullPresetArray)
// Updates the Array with the Values in Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CStereoWideningUtility::UpdateFullPresetArrayFromCenRep(RArray<TEfStereoWideningUtilityPreset>& aFullPresetArray)
	{

		aFullPresetArray.Reset();
 		TEfStereoWideningUtilityPreset tempPreset;


		TInt numberOfUserDefinedPresets = 0;
		TInt numberOfPreDefinedPresets = 0;

		iPresetRepository->Get(KStereoWideningNumberOfUserDefinedPresets,numberOfUserDefinedPresets);
		iPresetRepository->Get(KStereoWideningNumberOfPreDefinedPresets,numberOfPreDefinedPresets);

		for( TInt j = 0; j < (numberOfPreDefinedPresets); j++)
				{
					tempPreset.iPresetDescriptionKey = KStereoWideningPreDefinedPresetStart + (2 * j);
					tempPreset.iPresetNameKey = KStereoWideningPreDefinedPresetStartName + (2 * j);
					iPresetRepository->Get((KStereoWideningPreDefinedPresetStartName + (2 * j)) ,tempPreset.iPresetName);
#ifdef _DEBUG
					RDebug::RawPrint(tempPreset.iPresetName);
#endif
					aFullPresetArray.Append(tempPreset);

				}

			for( TInt j = 0; j < (numberOfUserDefinedPresets); j++)
				{
					tempPreset.iPresetDescriptionKey = KStereoWideningUserDefinedPresetStart + (2 * j);
					tempPreset.iPresetNameKey = KStereoWideningUserDefinedPresetStartName + (2 * j);
					iPresetRepository->Get((KStereoWideningUserDefinedPresetStartName + (2 * j)) ,tempPreset.iPresetName);
#ifdef _DEBUG
					RDebug::RawPrint(tempPreset.iPresetName);
#endif
					aFullPresetArray.Append(tempPreset);

				}

	}

// -----------------------------------------------------------------------------
// CStereoWideningUtility::UpdatePresetArray(RArray<TDesC>& aPresetArray,const RArray<TEfStereoWideningUtilityPreset> &aFullPresetArray)
// Creates the Descriptor Array of Names of the Presets Stored in Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CStereoWideningUtility::UpdatePresetArray(RArray<TEfStereoWideningUtilityPreset>& aPresetArray,const RArray<TEfStereoWideningUtilityPreset>& aFullPresetArray)
	{

		aPresetArray.Reset();
		TEfStereoWideningUtilityPreset	tempClientArray;

		// Write the StereoWidening Data in the first available NULL Space
		for( TInt j = 0; j < aFullPresetArray.Count();j++)
			{
				if( aFullPresetArray[j].iPresetName.Length() != 0)
					{

						tempClientArray.iPresetName = aFullPresetArray[j].iPresetName;
						tempClientArray.iPresetNameKey = aFullPresetArray[j].iPresetNameKey;
						tempClientArray.iPresetDescriptionKey = aFullPresetArray[j].iPresetDescriptionKey;
						aPresetArray.Append(tempClientArray);
					}
				RDebug::Print(_L("Name Length %d "),aFullPresetArray[j].iPresetName.Length());
				RDebug::RawPrint(aFullPresetArray[j].iPresetName);
			}

	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



//  End of File
