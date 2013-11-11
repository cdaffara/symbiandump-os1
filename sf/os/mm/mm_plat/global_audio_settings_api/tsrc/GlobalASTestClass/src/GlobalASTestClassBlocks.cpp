/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Global Audio Settings Test
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "GlobalASTestClass.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlobalASTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CGlobalASTestClass::Delete()
	{
    iLog->Log(_L("Deleting test class..."));
	if(iPhoneProfileSettings)
		{
		delete iPhoneProfileSettings;
		iPhoneProfileSettings = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CGlobalASTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::RunMethodL( CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

		ENTRY( "CreateGlobalAS", CGlobalASTestClass::CreateGlobalAS ),
		ENTRY( "DeleteGlobalAS", CGlobalASTestClass::DeleteGlobalAS ),
        ENTRY( "IsWarningTonesEnabled", CGlobalASTestClass::IsWarningTonesEnabled ),
        ENTRY( "IsMessageTonesEnabled", CGlobalASTestClass::IsMessageTonesEnabled ),
        ENTRY( "IsSilentProfileEnabled", CGlobalASTestClass::IsSilentProfileEnabled ),
        ENTRY( "IsVibraEnabled", CGlobalASTestClass::IsVibraEnabled ),
        ENTRY( "IsPublicSilenceEnabled", CGlobalASTestClass::IsPublicSilenceEnabled ),
        ENTRY( "RegisterAudioClientsListObserver", CGlobalASTestClass::RegisterAudioClientsListObserver ),
        ENTRY( "UnregisterAudioClientsListObserver", CGlobalASTestClass::UnregisterAudioClientsListObserver ),
        ENTRY( "GetAudioClientsList", CGlobalASTestClass::GetAudioClientsList ),
        ENTRY( "GetPausedClientsList", CGlobalASTestClass::GetPausedClientsList ),
        ENTRY( "RingingType", CGlobalASTestClass::GetRingingType ),
        ENTRY( "GetMessageToneName", CGlobalASTestClass::MessageAlertTone ),
        ENTRY( "IsMessageAlertToneEnabled", CGlobalASTestClass::IsMessageAlertToneEnabled ),
        ENTRY( "GetEmailToneName", CGlobalASTestClass::EmailAlertTone ),
        ENTRY( "IsEmailAlertToneEnabled", CGlobalASTestClass::IsEmailAlertToneEnabled ),
        ENTRY( "GetVideoCallToneName", CGlobalASTestClass::VideoCallAlertTone ),
        ENTRY( "IsVideoCallAlertToneEnabled", CGlobalASTestClass::IsVideoCallAlertToneEnabled ),
        ENTRY( "GetRingingTone1Name", CGlobalASTestClass::RingingAlertTone1 ),
        ENTRY( "IsRingingAlertTone1Enabled", CGlobalASTestClass::IsRingingAlertTone1Enabled ),
        ENTRY( "GetRingingTone2Name", CGlobalASTestClass::RingingAlertTone2 ),
        ENTRY( "IsRingingAlertTone2Enabled", CGlobalASTestClass::IsRingingAlertTone2Enabled ),
        ENTRY( "GetKeypadToneVolume", CGlobalASTestClass::KeypadAlertTone ),
        ENTRY( "IsSilenceModeActive", CGlobalASTestClass::IsSilenceModeActive )
        		
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CGlobalASTestClass::CreateGlobalAS
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::CreateGlobalAS( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::CreateGlobalAS"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
		status = KErrAlreadyExists;
		}
	else
		{
		TRAP(status,iPhoneProfileSettings = CGlobalAudioSettings::NewL(*this));
		}
		CActiveScheduler::Start();
	return status;
	}

// -----------------------------------------------------------------------------
// CGlobalASTestClass::DeleteGlobalAS
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::DeleteGlobalAS( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::DeleteGlobalAS"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
		delete iPhoneProfileSettings;
		iPhoneProfileSettings = NULL;
		}
	else
		{
		status = KErrNotFound;
		}
	return status;
	}

// -----------------------------------------------------------------------------
// CGlobalASTestClass::IsWarningTonesEnabled
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::IsWarningTonesEnabled( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::IsWarningTonesEnabled"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
		TBool enable = iPhoneProfileSettings->IsWarningTonesEnabled();
		iLog->Log(_L("CGlobalASTestClass::IsWarningTonesEnabled [%d]"),enable);
		}
	else
		{
		status = KErrNotFound;
		}
	return status;
	}
// -----------------------------------------------------------------------------
// CGlobalASTestClass::IsMessageTonesEnabled
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::IsMessageTonesEnabled( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::IsMessageTonesEnabled"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
		TBool enable = iPhoneProfileSettings->IsMessageTonesEnabled();
		iLog->Log(_L("CGlobalASTestClass::IsMessageTonesEnabled [%d]"),enable);
		}
	else
		{
		status = KErrNotFound;
		}
	return status;
	}

// -----------------------------------------------------------------------------
// CGlobalASTestClass::IsSilentProfileEnabled
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::IsSilentProfileEnabled( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::IsSilentProfileEnabled"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
		TBool enable = iPhoneProfileSettings->IsSilentProfileEnabled();
		iLog->Log(_L("CGlobalASTestClass::IsSilentProfileEnabled [%d]"),enable);
		}
	else
		{
		status = KErrNotFound;
		}
	return status;
	}

// -----------------------------------------------------------------------------
// CGlobalASTestClass::IsVibraEnabled
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::IsVibraEnabled( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::IsVibraEnabled"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
		TBool enable = iPhoneProfileSettings->IsVibraEnabled();
		iLog->Log(_L("CGlobalASTestClass::IsVibraEnabled [%d]"),enable);
		}
	else
		{
		status = KErrNotFound;
		}
	return status;
	}

// -----------------------------------------------------------------------------
// CGlobalASTestClass::IsPublicSilenceEnabled
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::IsPublicSilenceEnabled( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::IsPublicSilenceEnabled"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
		TBool enable = iPhoneProfileSettings->IsPublicSilenceEnabled();
		iLog->Log(_L("CGlobalASTestClass::IsPublicSilenceEnabled [%d]"),enable);
		}
	else
		{
		status = KErrNotFound;
		}
	return status;
	}

// -----------------------------------------------------------------------------
// CGlobalASTestClass::RegisterAudioClientsListObserver
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::RegisterAudioClientsListObserver( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::RegisterAudioClientsListObserver"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
		status = iPhoneProfileSettings->RegisterAudioClientsListObserver(*this);
		}
	else
		{
		status = KErrNotFound;
		}
	return status;
	}

// -----------------------------------------------------------------------------
// CGlobalASTestClass::UnregisterAudioClientsListObserver
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::UnregisterAudioClientsListObserver( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::UnregisterAudioClientsListObserver"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
		status = iPhoneProfileSettings->UnregisterAudioClientsListObserver(*this);
		}
	else
		{
		status = KErrNotFound;
		}
	return status;
	}

// -----------------------------------------------------------------------------
// CGlobalASTestClass::GetAudioClientsList
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::GetAudioClientsList( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::GetAudioClientsList"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
	    RArray<TProcessId> aPIDList;
	    status = iPhoneProfileSettings->GetAudioClientsList( CGlobalAudioSettings::EActiveAudioClients, aPIDList );
	    if ( status != KErrNone )
	        {
	        iLog->Log(_L("CGlobalASTestClass:DisplayAll : Error[%d] getting Audio Clients List"), status );
	        }
	    else
	        {
	        iLog->Log(_L("CGlobalASTestClass:DisplayAll : Begins"));
	        if ( aPIDList.Count() == 0 )
	            {
	            iLog->Log(_L("No active audio client"));
	            }
	        else
	            {
	            for ( TInt index =0; index < aPIDList.Count(); index++ )
	                {
	                iLog->Log(_L("List[%d]PID[0x%x]"), index, aPIDList[index].Id());
	                }
	            }
	        iLog->Log(_L("CGlobalASTestClass:DisplayAll : Ends"));
	        }
	    aPIDList.Close();
		}
	else
		{
		status = KErrNotFound;
		}
	return status;
	}

// -----------------------------------------------------------------------------
// CGlobalASTestClass::GetAudioClientsList
// -----------------------------------------------------------------------------
//
TInt CGlobalASTestClass::GetPausedClientsList( CStifItemParser& /*aItem*/ )
	{
	iLog->Log(_L("CGlobalASTestClass::GetPausedClientsList"));
	TInt status(KErrNone);
	if(iPhoneProfileSettings)
		{
	    RArray<TProcessId> aPIDList;
	    status = iPhoneProfileSettings->GetAudioClientsList( CGlobalAudioSettings::EPausedAudioClients, aPIDList );
	    if ( status != KErrNone )
	        {
	        iLog->Log(_L("CGlobalASTestClass:DisplayAll : Error[%d] getting Paused Clients List"), status );
	        }
	    else
	        {
	        iLog->Log(_L("CGlobalASTestClass:DisplayAll : Begins"));
	        if ( aPIDList.Count() == 0 )
	            {
	            iLog->Log(_L("No paused audio client"));
	            }
	        else
	            {
	            for ( TInt index =0; index < aPIDList.Count(); index++ )
	                {
	                iLog->Log(_L("List[%d]PID[0x%x]"), index, aPIDList[index].Id());
	                }
	            }
	        iLog->Log(_L("CGlobalASTestClass:DisplayAll : Ends"));
	        }
	    aPIDList.Close();
		}
	else
		{
		status = KErrNotFound;
		}
	return status;
	}



void CGlobalASTestClass::SettingsChanged(CGlobalAudioSettings& /*aGlobalAudioSettings*/, MAudioSettingsObserver::TGASEventId aSetting)
	{
   RDebug::Printf("settings changed %d",aSetting);
    CActiveScheduler::Stop();
	}

void CGlobalASTestClass::Event( const CGlobalAudioSettings& /*aGlobalAudioSettings*/, TUint aEvent )
    {
    iLog->Log(_L("CGlobalASTestClass From MAudioClientsListObserver[%d]"), aEvent);
    }
TInt CGlobalASTestClass::GetRingingType(CStifItemParser& /*aItem*/ )
    {
    RDebug::Printf("CGlobalASTestClass::GetRingingType");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
   TInt type= iPhoneProfileSettings->RingingType();
   RDebug::Printf("ringing type is %d",type);
        }
    else
        status=KErrNotFound;
    
    return status;
    }

TInt CGlobalASTestClass::MessageAlertTone(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
   // TFileName filename=iPhoneProfileSettings->MessageAlertTone();
   
   // RDebug::Print(_L("filename %S"),&filename);
    TFileName filename=iPhoneProfileSettings->MessageAlertTone();
    RDebug::Print(_L("enable:%S"),&filename);
        }
    else
        status=KErrNotFound;
    
    return status;
    }
TInt CGlobalASTestClass::IsMessageAlertToneEnabled(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
   // TFileName filename=iPhoneProfileSettings->MessageAlertTone();
   
   // RDebug::Print(_L("filename %S"),&filename);
    TBool enable=iPhoneProfileSettings->IsMessageAlertToneEnabled();
    RDebug::Print(_L("enable:%d"),enable);
        }
    else
        status=KErrNotFound;
    
    return status;
    }
TInt CGlobalASTestClass::EmailAlertTone(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
    TFileName filename=iPhoneProfileSettings->EmailAlertTone();
  RDebug::Print(_L("enable:%S"),&filename);

        }
    else
        status=KErrNotFound;
    
    return status;
    }
TInt CGlobalASTestClass::IsEmailAlertToneEnabled(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
   // TFileName filename=iPhoneProfileSettings->MessageAlertTone();
   
   // RDebug::Print(_L("filename %S"),&filename);
    TBool enable=iPhoneProfileSettings->IsEmailAlertToneEnabled();
    RDebug::Print(_L("enable:%d"),enable);
        }
    else
        status=KErrNotFound;
    
    return status;
    }
TInt CGlobalASTestClass::VideoCallAlertTone(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
    TFileName filename=iPhoneProfileSettings->VideoCallAlertTone();
    RDebug::Print(_L("enable:%S"),&filename);
        }
    else
        status=KErrNotFound;
    
    return status;
    }
TInt CGlobalASTestClass::IsVideoCallAlertToneEnabled(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
   // TFileName filename=iPhoneProfileSettings->MessageAlertTone();
   
   // RDebug::Print(_L("filename %S"),&filename);
    TBool enable=iPhoneProfileSettings->IsVideoCallAlertToneEnabled();
    RDebug::Print(_L("enable:%d"),enable);
        }
    else
        status=KErrNotFound;
    
    return status;
    }
TInt CGlobalASTestClass::RingingAlertTone1(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
    TFileName filename=iPhoneProfileSettings->RingingAlertTone1();
    RDebug::Print(_L("enable:%S"),&filename);
        }
    else
        status=KErrNotFound;
    
    return status;
    }
TInt CGlobalASTestClass::IsRingingAlertTone1Enabled(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
   // TFileName filename=iPhoneProfileSettings->MessageAlertTone();
   
   // RDebug::Print(_L("filename %S"),&filename);
    TBool enable=iPhoneProfileSettings->IsRingingAlertTone1Enabled();
    RDebug::Print(_L("enable:%d"),enable);
        }
    else
        status=KErrNotFound;
    
    return status;
    }
TInt CGlobalASTestClass::RingingAlertTone2(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
    TFileName filename=iPhoneProfileSettings->RingingAlertTone2();
    RDebug::Print(_L("enable:%S"),&filename);
        }
    else
        status=KErrNotFound;
    
    return status;
    }
TInt CGlobalASTestClass::IsRingingAlertTone2Enabled(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
   // TFileName filename=iPhoneProfileSettings->MessageAlertTone();
   
   // RDebug::Print(_L("filename %S"),&filename);
    TBool enable=iPhoneProfileSettings->IsRingingAlertTone2Enabled();
    RDebug::Print(_L("enable:%d"),enable);
        }
    else
        status=KErrNotFound;
    
    return status;
    }
TInt CGlobalASTestClass::KeypadAlertTone(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::MessageAlertTone");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
    TInt volume=iPhoneProfileSettings->KeyPadToneVolume();
    RDebug::Print(_L("enable:%d"),volume);
        }
    else
        status=KErrNotFound;
    
    return status;
    }

TInt CGlobalASTestClass::IsSilenceModeActive(CStifItemParser& /*aItem*/)
    {
    RDebug::Printf("CGlobalASTestClass::IsSilenceProfileActive");
    TInt status(KErrNone);
    if(iPhoneProfileSettings)
        {
    TInt enable=iPhoneProfileSettings->IsSilenceModeEnabled();
    RDebug::Print(_L("enable:%d"),enable);
        }
    else
        status=KErrNotFound;
    
    return status;
    }
//  End of File
