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
* Description:   Implementation of the Phone Profile Settings API
*
*/




// INCLUDE FILES
#include <e32std.h>
#include <GlobalAudioSettings.h>
#include "GlobalAudioSettingsImpl.h"
#include <MGlobalAudioSettingsObserver.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlobalAudioSettings::CGlobalAudioSettings();
// 
// -----------------------------------------------------------------------------
//
CGlobalAudioSettings::CGlobalAudioSettings()
	{
	}
	
// -----------------------------------------------------------------------------
// CGlobalAudioSettings::NewL();
// 
// -----------------------------------------------------------------------------
//
EXPORT_C CGlobalAudioSettings* CGlobalAudioSettings::NewL(
                               MAudioSettingsObserver& aAudioSettingsObserver)
	{
	CGlobalAudioSettings* self = new (ELeave) CGlobalAudioSettings();
    CleanupStack::PushL( self );
    self->ConstructL( aAudioSettingsObserver );
    CleanupStack::Pop( self );
    return self;
	}
	
// -----------------------------------------------------------------------------
// CGlobalAudioSettings::ConstructL();
// 
// -----------------------------------------------------------------------------
//
void CGlobalAudioSettings::ConstructL( MAudioSettingsObserver& aAudioSettingsObserver )
	{
	iBody = CGlobalAudioSettingsImpl::NewL(*this, aAudioSettingsObserver );
	}

// Destructor
EXPORT_C CGlobalAudioSettings::~CGlobalAudioSettings()
    {
    delete iBody;
    }

// -----------------------------------------------------------------------------
// CGlobalAudioSettings::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlobalAudioSettings::IsWarningTonesEnabled()
	{
	return (iBody->IsWarningTonesEnabled());
	}

// -----------------------------------------------------------------------------
// CGlobalAudioSettings::IsMessageTonesEnabled
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlobalAudioSettings::IsMessageTonesEnabled()
	{
	return (iBody->IsMessageTonesEnabled());
	}

// -----------------------------------------------------------------------------
// CGlobalAudioSettings::IsSilentProfileEnabled
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlobalAudioSettings::IsSilentProfileEnabled()
	{
	return (iBody->IsSilentProfileEnabled());
	}

// -----------------------------------------------------------------------------
// CGlobalAudioSettings::IsVibraEnabled
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlobalAudioSettings::IsVibraEnabled()
	{
	return (iBody->IsVibraEnabled());
	}
	
// -----------------------------------------------------------------------------
// CGlobalAudioSettings::IsPublicSilenceEnabled
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlobalAudioSettings::IsPublicSilenceEnabled()
	{
	return (iBody->IsPublicSilenceEnabled());
	}

EXPORT_C TDesC& CGlobalAudioSettings::MessageAlertTone()
    {
    return (iBody->MessageAlertTone());
    }

EXPORT_C TBool CGlobalAudioSettings::IsMessageAlertToneEnabled()
    {
    return (iBody->IsMessageAlertToneEnabled());
    }
EXPORT_C TDesC& CGlobalAudioSettings::EmailAlertTone()
    {
    return (iBody->EmailAlertTone());
    }
EXPORT_C TBool CGlobalAudioSettings::IsEmailAlertToneEnabled()
    {
    return (iBody->IsEmailAlertToneEnabled());
    }
EXPORT_C CGlobalAudioSettings::TGASRingingType CGlobalAudioSettings::RingingType()
    {
     return (iBody->RingingType());
    }
EXPORT_C TDesC& CGlobalAudioSettings::VideoCallAlertTone()
    {
    return (iBody->VideoCallAlertTone());
    }
EXPORT_C TBool CGlobalAudioSettings::IsVideoCallAlertToneEnabled()
    {
    return (iBody->IsVideoCallAlertToneEnabled());
    }
EXPORT_C TDesC& CGlobalAudioSettings::RingingAlertTone1()
    {
    return (iBody->RingingAlertTone1());
    }
EXPORT_C TBool CGlobalAudioSettings::IsRingingAlertTone1Enabled()
    {
    return (iBody->IsRingingAlertTone1Enabled());
    }
EXPORT_C TDesC& CGlobalAudioSettings::RingingAlertTone2()
    {
    return (iBody->RingingAlertTone2());
    }
EXPORT_C TBool CGlobalAudioSettings::IsRingingAlertTone2Enabled()
    {
    return (iBody->IsRingingAlertTone2Enabled());
    }
EXPORT_C CGlobalAudioSettings::TGASKeypadVolume CGlobalAudioSettings::KeyPadToneVolume()
    {
    return (iBody->KeyPadToneVolume());
    }

EXPORT_C TBool CGlobalAudioSettings::IsSilenceModeEnabled()
    {
    return (iBody->IsSilenceModeEnabled());
    }
// -----------------------------------------------------------------------------
// CGlobalAudioSettings::RegisterAudioClientsListObserver
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlobalAudioSettings::RegisterAudioClientsListObserver(
                            MAudioClientsListObserver& aObserver )
	{
	return (iBody->RegisterAudioClientsListObserver(aObserver));
	}
        
// -----------------------------------------------------------------------------
// CGlobalAudioSettings::UnregisterAudioClientsListObserver
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlobalAudioSettings::UnregisterAudioClientsListObserver(
                            MAudioClientsListObserver& aObserver)
	{
	return (iBody->UnregisterAudioClientsListObserver(aObserver));
	}

// -----------------------------------------------------------------------------
// CGlobalAudioSettings::GetAudioClientsList
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlobalAudioSettings::GetAudioClientsList(
                        TAudioClientListType aType,
                        RArray<TProcessId>& aList )
	{
	return (iBody->GetAudioClientsList(aType, aList));
	}

//End of file
