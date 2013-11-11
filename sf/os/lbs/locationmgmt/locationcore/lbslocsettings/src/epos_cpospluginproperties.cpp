// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



#include <f32file.h>
#include <barsc2.h>
#include <barsread2.h>
#include <bautils.h>
#include <centralrepository.h>
#include <ecom/implementationinformation.h>

#include "epos_modulesinternal.h"
#include "epos_cpospluginproperties.h"

// ========================= LOCAL FUNCTIONS =================================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
inline void AssertTrueSecL( const TBool aCondition, const TInt aErrorCode )
    {
    if ( !aCondition )
        {
        User::Leave( aErrorCode );
        }
    }

// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosPluginProperties::CPosPluginProperties( RFs& aFsSession )
	: iFs( aFsSession )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosPluginProperties* CPosPluginProperties::NewL(
	CImplementationInformation& aEcomInfo, RFs& aFsSession )
    {
    CPosPluginProperties* self = new (ELeave) CPosPluginProperties( aFsSession );

    CleanupStack::PushL( self );
    self->ConstructL( aEcomInfo );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosPluginProperties::ConstructL( CImplementationInformation& aEcomInfo )
    {
    iModuleId = aEcomInfo.ImplementationUid();
    iVersion = aEcomInfo.Version();
    iRomBased = aEcomInfo.RomBased();
    iEcomName.Copy( aEcomInfo.DisplayName().Left( KPositionMaxModuleName ) );

    CRepository* repository = CRepository::NewL( iModuleId );
    CleanupStack::PushL( repository );

    LoadCenRepInfoL( *repository );

    CleanupStack::PopAndDestroy( repository );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosPluginProperties::~CPosPluginProperties()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TPositionModuleId CPosPluginProperties::ModuleId() const
    {
    return iModuleId;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TVersion CPosPluginProperties::Version() const
    {
    return TVersion( iVersion, 0 , 0 );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CPosPluginProperties::IsRomBased() const
	{
	return iRomBased;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CPosPluginProperties::IsVisible() const
	{
	return iDefaultVisibility;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosPluginProperties::TModulePriority CPosPluginProperties::PreferredPriority() const
	{
	return (TModulePriority) iDefaultPriority;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosPluginProperties::GetModuleInfo( TPositionModuleInfo& aInfo ) const
    {
    aInfo.SetModuleId( iModuleId );
    if ( iLocalizedName.Length() )
        {
        aInfo.SetModuleName( iLocalizedName );
        }
    else
        {
        aInfo.SetModuleName( iEcomName );
        }

    TPositionQuality quality;
    quality.SetCostIndicator( static_cast<TPositionQuality::TCostIndicator> 
                             ( iCostIndicator ) );
    quality.SetHorizontalAccuracy( static_cast<TReal32> 
                                  ( iHorizontalAccuracy ) );
    quality.SetVerticalAccuracy( static_cast<TReal32>
                                ( iVerticalAccuracy ) );
    quality.SetPowerConsumption( static_cast<TPositionQuality::TPowerConsumption> 
                                ( iPowerConsumption ) );
    quality.SetTimeToFirstFix( static_cast<TTimeIntervalMicroSeconds> 
                              ( iTimeToFirstFix ) );
    quality.SetTimeToNextFix(static_cast<TTimeIntervalMicroSeconds> 
                             ( iTimeToNextFix ) );
    aInfo.SetPositionQuality( quality );
    
    aInfo.SetCapabilities( iCapabilities );
    aInfo.SetClassesSupported( EPositionInfoFamily, iClassesSupported );
    aInfo.SetDeviceLocation( iDeviceLocation );
    aInfo.SetTechnologyType( iTechnologyType );
    
    aInfo.SetVersion( Version() );
    aInfo.SetIsAvailable( iDefaultStatus );
    }

// ========================= INTERNAL FUNCTIONS =================================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosPluginProperties::LoadCenRepInfoL( CRepository& aRepository )
    {
    // interface version, >=1
	User::LeaveIfError( aRepository.Get( KPSYInterfaceVersion, iInterfaceVersion ) );
    AssertTrueSecL( ( iInterfaceVersion >= 1 ), KErrNotFound );
    
    // priority, error is fixed
    User::LeaveIfError( aRepository.Get( KPSYPriority, iDefaultPriority ) );
    if ( iDefaultPriority < EModulePriorityAuto || 
         iDefaultPriority > EModulePriorityLast )
        {
        User::Leave( KErrCorrupt );
        }
    
    // default status
    User::LeaveIfError( aRepository.Get( KPSYStatus, iDefaultStatus ) );
    if ( iDefaultStatus != EModuleAvailable && iDefaultStatus != EModuleNotAvailable )
        {
        User::Leave( KErrCorrupt );
        }

    // TTFF, must be positive
    TInt int32;
    User::LeaveIfError( aRepository.Get( KPSYTimeToFirstFix, int32 ) );
    iTimeToFirstFix = int32;
    AssertTrueSecL( ( iTimeToFirstFix >= 0 ), KErrCorrupt );
    
    // TTNF, must be positive
    User::LeaveIfError( aRepository.Get( KPSYTimeToNextFix, int32 ) );
    iTimeToNextFix = int32;
    AssertTrueSecL( ( iTimeToNextFix >= 0 ), KErrCorrupt );
    
    // horizontal accuracy, must be positive
    User::LeaveIfError( aRepository.Get( KPSYHorizontalAccuracy, iHorizontalAccuracy ) );
    AssertTrueSecL( ( iHorizontalAccuracy >= 0 ), KErrCorrupt );
    // horizontal accuracy of 0 is treated as undefined
    if(iHorizontalAccuracy == 0)
    	{
    	TRealX nan;
    	nan.SetNaN();
    	iHorizontalAccuracy = nan;
    	}
    
    // vertical accuracy, must be positive
    User::LeaveIfError( aRepository.Get( KPSYVerticalAccuracy, iVerticalAccuracy ) );
    AssertTrueSecL( ( iVerticalAccuracy >= 0 ), KErrCorrupt );
    // vertical accuracy of 0 is treated as undefined
    if(iVerticalAccuracy == 0)
        	{
        	TRealX nan;
        	nan.SetNaN();
        	iVerticalAccuracy = nan;
        	}
    
    // cost indicator and make range check to it
    User::LeaveIfError( aRepository.Get( KPSYCostIndicator, iCostIndicator ) );
    if ( ( iCostIndicator < TPositionQuality::ECostUnknown ) ||
         ( iCostIndicator > TPositionQuality::ECostCharge ) )
        {
        User::Leave( KErrCorrupt );
        }
    
    // power consumption and make range check to it
    User::LeaveIfError( aRepository.Get( KPSYPowerConsumption, iPowerConsumption ) );
    if ( ( iPowerConsumption < TPositionQuality::EPowerUnknown ) ||
         ( iPowerConsumption > TPositionQuality::EPowerHigh ) )
        {
        User::Leave(KErrCorrupt);
        }
    
    // technology type, no error checking
    User::LeaveIfError( aRepository.Get( KPSYTechnologyType, iTechnologyType ) );
    
    // device location, no error checking
    User::LeaveIfError( aRepository.Get( KPSYDeviceLocation, iDeviceLocation ) );
    
    // capabilities, no error checking
    User::LeaveIfError( aRepository.Get( KPSYCapabilities, iCapabilities ) );
    
    // supported classes, no error checking
    User::LeaveIfError( aRepository.Get( KPSYClassesSupported, iClassesSupported ) );
    
    // visibility
    User::LeaveIfError( aRepository.Get( KPSYVisibility, iDefaultVisibility ) );
    if ( iDefaultVisibility != EModuleVisible && 
         iDefaultVisibility != EModuleInvisible )
        {
        User::Leave( KErrCorrupt );
        }
        
    // Localized name. This is optional property
    TInt err = aRepository.Get( KPSYNameResource, iLocalizedNameResource );
    if ( !err && iLocalizedNameResource.Length()>0 )
        {
        TFileName fileName;
        TBool resFound = FindPsyNameResourceFileL( iFs, iLocalizedNameResource, fileName );
        if ( resFound )
    		{
    		HBufC* name = GetPsyNameFromResourceFileLC( iFs, fileName );
            iLocalizedName = name->Left( KPositionMaxModuleName );
            CleanupStack::PopAndDestroy( name );
    		} 
        }
    // if neither R0x nor RSC file found, a name from ECOM will be used
    }
    
// -----------------------------------------------------------------------------
// Look on system drive and ROM only
// -----------------------------------------------------------------------------
//
TBool CPosPluginProperties::FindPsyNameResourceFileL(
    RFs& aFsSession,
    const TDesC& aRscFile,
    TFileName& aFileName )
    {
    // 1. Check for localized file
    
    TFileName fileName;
    FileNameOnDrive( aFsSession, EDriveZ, aRscFile, fileName );
    
    // this method checks both system and Z drive
    BaflUtils::NearestLanguageFile( aFsSession, fileName );

    const TInt KLangPartOfExtensionLen = 2;
    TBool found = ( fileName.Right( KLangPartOfExtensionLen ) !=
                    aRscFile.Right( KLangPartOfExtensionLen ) );

    // 2. If localized file not found, try SC
    if ( !found )
        {
        TDriveNumber sysDrive;
        TInt err = BaflUtils::GetSystemDrive( sysDrive );
        if ( !err && FindFile( aFsSession, sysDrive, aRscFile, fileName ) )
            {
            found = ETrue;
            }
        if ( !found && FindFile( aFsSession, EDriveZ, aRscFile, fileName ) )    
            {
            found = ETrue;
            }
        }

    if ( found )
        {
        aFileName.Copy( fileName );
        }

    return found;        
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CPosPluginProperties::FindFile(
    RFs& aFsSession,
    TInt aDrive,
    const TDesC& aFile,
    TFileName& aFileName )
    {
    TFileName fileName;
    FileNameOnDrive( aFsSession, aDrive, aFile, fileName );

    if ( BaflUtils::FileExists( aFsSession, fileName ) )
        {
        aFileName.Copy( fileName );
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CPosPluginProperties::FileNameOnDrive(
    RFs& aFsSession,
    TInt aDrive,
    const TDesC& aFile,
    TFileName& aFileName )
    {
    TParse parse;
    parse.Set( aFile, NULL, NULL );

    TChar drv;
    aFsSession.DriveToChar( aDrive, drv );

    aFileName.Zero();
    aFileName.Append( drv );
    aFileName.Append( KDriveDelimiter );
    aFileName.Append( parse.Path() );
    aFileName.Append( parse.NameAndExt() );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
HBufC* CPosPluginProperties::GetPsyNameFromResourceFileLC(
    RFs& aFsSession,
    const TDesC& aResourceFileName )
    {
    CResourceFile* resFile = CResourceFile::NewLC( aFsSession, aResourceFileName, 0, 0 );

    // localized PSY name is the second resource - it's after signature
    const TInt KPsyNameResourceOffset = 1; 
    HBufC8* buffer = resFile->AllocReadLC( KPsyNameResourceOffset );

    RResourceReader reader;
    reader.OpenLC( buffer->Des() );

    HBufC* psyName = reader.ReadHBufCL();

    CleanupStack::PopAndDestroy( &reader );
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( resFile );
    
    CleanupStack::PushL( psyName );
    return psyName;
    }
