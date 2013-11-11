/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Implementation of CSsmUiSpecific class.
*
*/

#include <ctsydomainpskeys.h>
#include <f32file.h>
#include <mmtsy_names.h>
#include <pathinfo.h>
#include <securitynotification.h>
#include <ssm/ssmstate.h>
#include <startupdomainpskeys.h>

#include "ssmuispecific.h"
#include "ssmmapperutilitystatic.h"
#include "ssmmapperutilityinternalpskeys.h"
#include "ssmsubstateext.hrh"
#include "ssmswp.hrh"
#include "trace.h"

/** Channel used to communicate with Security Notifier. */
static const TUid KSecurityNotifierChannel = { 0x1000598F };

// Type definitions for a buffer containing a drive id (drive letter + :).
const TInt KDriveIdLength = 2;
//Number of clusterSize to be reserve for phone memory space 
const TInt KNumberOfCluster = 2;
typedef TBuf<KDriveIdLength> TDriveId;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmUiSpecific::~CSsmUiSpecific
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmUiSpecific::~CSsmUiSpecific()
	{
    FUNC_LOG;
    iReservedPhoneMemoryFs.Close();
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::InstanceL
// ---------------------------------------------------------------------------
//
EXPORT_C CSsmUiSpecific* CSsmUiSpecific::InstanceL()
	{
    FUNC_LOG;

	CSsmUiSpecific* self( NULL );
	if ( Dll::Tls() )
		{
		self = static_cast<CSsmUiSpecific*>( Dll::Tls() );
		self->iReferenceCount++;
		}
	else
		{
		self = new ( ELeave ) CSsmUiSpecific;
		CleanupStack::PushL( self );
		self->ConstructL();
		User::LeaveIfError( Dll::SetTls( self ) );
		CleanupStack::Pop( self );
		}

	return self;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::Release
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmUiSpecific::Release()
	{
    FUNC_LOG;

    TAny* tlsPtr = Dll::Tls();
	ASSERT_TRACE( tlsPtr );

    CSsmUiSpecific* self = static_cast<CSsmUiSpecific*>( tlsPtr );
    if ( --self->iReferenceCount == 0 )
        {
        Dll::FreeTls();
        delete self;
        }
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::StartupPSUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CSsmUiSpecific::StartupPSUid()
	{
    FUNC_LOG;

	return KPSUidStartup;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::StarterPSUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CSsmUiSpecific::StarterPSUid()
	{
    FUNC_LOG;

	return KPSStarterUid;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::EmergencyCallPropertyCategory
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CSsmUiSpecific::EmergencyCallPropertyCategory()
	{
    FUNC_LOG;

	return KPSUidCtsyCallInformation;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::SecurityPinNotifierUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CSsmUiSpecific::SecurityPinNotifierUid()
	{
    FUNC_LOG;

	return KSecurityNotifierUid;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::ScreenOutputChannelUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CSsmUiSpecific::ScreenOutputChannelUid()
	{
    FUNC_LOG;

	return KSecurityNotifierChannel;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::RFStatusPropertyCategory
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CSsmUiSpecific::RFStatusPropertyCategory()
    {
    FUNC_LOG;

	return TUid::Uid( SWP_UID_SSM_RF_STATUS );
    }


// ---------------------------------------------------------------------------
// CSsmUiSpecific::EmergencyCallPropertyKey
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CSsmUiSpecific::EmergencyCallPropertyKey()
	{
    FUNC_LOG;

	return KCTsyCallState;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::SimStatusPropertyKey
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CSsmUiSpecific::SimStatusPropertyKey()
	{
    FUNC_LOG;

	return KPSSimStatus;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::RFStatusPropertyKey
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CSsmUiSpecific::RFStatusPropertyKey()
    {
    FUNC_LOG;

	return SWP_UID_SSM_RF_STATUS;
    }


// ---------------------------------------------------------------------------
// CSsmUiSpecific::GetTsyModuleNameL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CSsmUiSpecific::GetTsyModuleNameL()
	{
    FUNC_LOG;

    /*****************************************************
    *   Series 60 Customer / TSY
    *   Needs customer TSY implementation
    *****************************************************/

	HBufC* name = KMmTsyModuleName().AllocL();
    INFO_1( "TSY name: %S", name );
	return name;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::PhoneTsyNameL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CSsmUiSpecific::PhoneTsyNameL()
	{
    FUNC_LOG;

    /*****************************************************
    *   Series 60 Customer / TSY
    *   Needs customer TSY implementation
    *****************************************************/

	HBufC* name = KMmTsyPhoneName().AllocL();
    INFO_1( "Phone name: %S", name );
	return name;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::SetSecurityStatus
// ---------------------------------------------------------------------------
//
EXPORT_C void CSsmUiSpecific::SetSecurityStatus(
    const TStrtSecurityStatus& aSecurityStatus )
	{
    FUNC_LOG;

	iStrtSecurityStatus = aSecurityStatus;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::SecurityStatus
// ---------------------------------------------------------------------------
//
EXPORT_C TStrtSecurityStatus CSsmUiSpecific::SecurityStatus() const
	{
    FUNC_LOG;

	return iStrtSecurityStatus;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::IsAmaStarterSupported
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmUiSpecific::IsAmaStarterSupported()
	{
    FUNC_LOG;

	return SsmMapperUtility::FeatureStatus( TUid::Uid( KFeatureIdExtendedStartup ) );
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::IsSimSupported
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmUiSpecific::IsSimSupported()
	{
    FUNC_LOG;

    return SsmMapperUtility::FeatureStatus( TUid::Uid( KFeatureIdSimCard ) );
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::IsSimlessOfflineSupported
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmUiSpecific::IsSimlessOfflineSupported()
	{
    FUNC_LOG;

    return SsmMapperUtility::FeatureStatus(
        TUid::Uid( KFeatureIdFfSimlessOfflineSupport ) );
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::IsSimPresent
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmUiSpecific::IsSimPresent()
	{
    FUNC_LOG;

    TInt value( 0 );
    TInt err = RProperty::Get( SsmMapperUtility::PsUid( KPSUidStartup ), KPSSimStatus, value );
    ERROR( err, "Failed to get value of KPSUidStartup::KPSSimStatus" );
    return ( err == KErrNone &&
             ( value == ESimUsable ||
               value == ESimReadable ||
               value == ESimNotReady ) );
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::IsSimStateChangeAllowed
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmUiSpecific::IsSimStateChangeAllowed()
	{
    FUNC_LOG;

    TSsmState state;
    TInt err = SsmMapperUtility::GetCurrentState( state );
    ERROR( err, "Failed to get current state" );
    return ( err == KErrNone &&
             ( state.MainState() == ESsmStartup && state.SubState() == ESsmStateSecurityCheck ) ||
             ( state.MainState() == ESsmStartup && state.SubState() == ESsmStateNonCritical ) ||
               state.MainState() == ESsmNormal );
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::IsNormalBoot
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmUiSpecific::IsNormalBoot()
	{
    FUNC_LOG;

    TInt value( 0 );
    TInt errorCode = RProperty::Get( SsmMapperUtility::PsUid( KPSUidStartup ), KPSStartupReason, value );
    ERROR( errorCode, "Failed to get value of KPSUidStartup::KPSStartupReason" );

    TBool ret = ( errorCode == KErrNone && value == ENormalStartup );
    INFO_1( "Is normal boot = %d", ret );
    return ret;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::IsSimChangedReset
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSsmUiSpecific::IsSimChangedReset()
	{
    FUNC_LOG;

    TUid startupPsUid( SsmMapperUtility::PsUid( KPSUidStartup ) );
    TInt value( 0 );
    TInt errorCode = RProperty::Get( startupPsUid, KPSStartupReason, value );
    ERROR( errorCode, "Failed to get value of KPSUidStartup::KPSStartupReason" );

    TBool ret( EFalse );
	if ( errorCode == KErrNone && value == ESIMStatusChangeReset )
        {
	    errorCode = RProperty::Get( startupPsUid, KPSSimChanged, value );
        ret = ( errorCode == KErrNone && value == ESimChanged );
        }

    INFO_1( "Is SIM changed reset = %d", ret );
    return ret;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::ValidateRTCPropertyKey
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CSsmUiSpecific::ValidateRTCPropertyKey()
	{
    FUNC_LOG;

	return KRTCPropertyKey;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::ValidateRTCPropertyCategory
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CSsmUiSpecific::ValidateRTCPropertyCategory()
	{
    FUNC_LOG;

	return KPSStarterUid;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::PhoneMemoryRootDriveId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSsmUiSpecific::PhoneMemoryRootDriveId()
	{
    FUNC_LOG;

    TInt driveId = EDriveC;
    TDriveId driveIdBuf = PathInfo::PhoneMemoryRootPath().Left( KDriveIdLength );
    if ( driveIdBuf.Length() > 0 )
        {
        RFs::CharToDrive( driveIdBuf[0], driveId ) ;
        }

    INFO_1( "Phone memory root path ID: %d", driveId );
	return driveId;
	}


// ---------------------------------------------------------------------------
// CSsmUiSpecific::CSsmUiSpecific
// ---------------------------------------------------------------------------
//
CSsmUiSpecific::CSsmUiSpecific() : iReferenceCount(1), iReservedPhoneMemory(0)
	{
    FUNC_LOG;
    }
/**
Leaving construction inside ConstructL
*/
void CSsmUiSpecific::ConstructL()
	{
	FUNC_LOG;
	
	User::LeaveIfError( iReservedPhoneMemoryFs.Connect() );
	}

/**
ReservePhoneMemorySpace
*/
EXPORT_C TInt CSsmUiSpecific::ReservePhoneMemorySpace()
    {
	FUNC_LOG;
	TVolumeIOParamInfo volumeParamInfo; 
	const TInt phoneMemoryDriveID = PhoneMemoryRootDriveId();
	TInt errorCode = iReservedPhoneMemoryFs.VolumeIOParam(phoneMemoryDriveID, volumeParamInfo); 
	if( KErrNone == errorCode )
		{
#ifdef __WINS__
		//512 bytes for __WINS__
		const TInt reservedMemory = 512;
#else
		//Reserving two clusterSize Phone memory 
		const TInt reservedMemory = KNumberOfCluster * (volumeParamInfo.iClusterSize);
#endif //__WINS__
		errorCode = iReservedPhoneMemoryFs.ReserveDriveSpace(phoneMemoryDriveID, reservedMemory);

		if ( KErrNone == errorCode )
			{
			iReservedPhoneMemory = reservedMemory;
			}
		}
    return errorCode;
    }

/**
FreeReservedPhoneMemorySpace
*/
EXPORT_C TInt CSsmUiSpecific::FreeReservedPhoneMemorySpace(const TInt aSpaceToFree)
    {
	FUNC_LOG;
    TInt errorCode(KErrGeneral);
	INFO_2("Reserved memory is = %d bytes, Request to free memory is = %d bytes", iReservedPhoneMemory, aSpaceToFree);
    if(0 < iReservedPhoneMemory)
        {
		if(0 == aSpaceToFree)
		    {
		   	//Free complete reserved phone memory
			errorCode = iReservedPhoneMemoryFs.ReserveDriveSpace( PhoneMemoryRootDriveId(), 0 );
			INFO_1("Freeing memory completed with = %d", errorCode);
			iReservedPhoneMemory = 0; 
		    }
		else
		    {
		    TInt newReserveSize = iReservedPhoneMemory - aSpaceToFree;
		    newReserveSize = newReserveSize >= 0 ? newReserveSize : 0;
		    errorCode = iReservedPhoneMemoryFs.ReserveDriveSpace( PhoneMemoryRootDriveId(), newReserveSize );
			INFO_1("Freeing partial phone memory completed with = %d", errorCode);
		    if(KErrNone == errorCode)
			    {
			    iReservedPhoneMemory = newReserveSize;
			    }
		    }
		}
    return errorCode;
    }
