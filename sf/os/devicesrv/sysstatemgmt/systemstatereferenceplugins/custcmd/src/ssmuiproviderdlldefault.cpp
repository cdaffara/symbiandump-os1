// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @internalComponent
 @released
*/

#include "ssmuiproviderdll.h"
#include "ssmpanic.h"
#include <e32property.h>
#include "ssmdebug.h"
#include <f32file.h>
#include <ctsydomainpskeys.h>
#include <mmtsy_names.h>
#include <pathinfo.h>
#include <securitynotification.h>
#include <ssm/ssmstate.h>
#include <startupdomainpskeys.h>
#include "ssmmapperutilitystatic.h"
#include "ssmmapperutilityinternalpskeys.h"
#include "ssmsubstateext.hrh"
#include "ssmswp.hrh"
#include "trace.h"

const TUid KPSStartupDefaultUid = {0x2000E65E};
const TUid KSecurityPinNotifierUid = {0x2000E667};
const TUid KScreenOutputChannel = {0x10009D48};
const TUid KEmergencyCallPropertyCategory = {0x2001032C};

const TUint KEmergencyCallPropertyKey = 0x0101;
const TUint KSimStatusPropertyKey = 0x0102;

const TUid KSecurityStatusPropertyCategory =  {0x2000E664};

const TUid KRFStatusPropertyCategory = {0x2000D75B};
const TUint KRFStatusPropertyKey = 0x2001D2A9;
const TUid KValidateRTCPropertyCategory = {0x2000D75B};
const TUint KValidateRTCPropertyKey = 0x2001D2AB;
//Number of clusterSize to be reserve for phone memory space 
const TInt KNumberOfCluster = 2;

//For test code
/** Channel used to communicate with Security Notifier. */
static const TUid KSecurityNotifierChannel = { 0x1000598F };

// Type definitions for a buffer containing a drive id (drive letter + :).
const TInt KDriveIdLength = 2;
typedef TBuf<KDriveIdLength> TDriveId;

const TUint32 KMiscPluginPropertyKey = 0x2000E658;
const TUid KPropertyCategory={0x2000D75B};              // tcustomcmd_server SID = KSsmServerName SID (changed in tcustomcmd_server.mmp file)


CSsmUiSpecific::CSsmUiSpecific()
: iReferenceCount(1), iReservedPhoneMemory(0)
	{
	}

EXPORT_C CSsmUiSpecific::~CSsmUiSpecific()
	{
	iReservedPhoneMemoryFs.Close();
	}

EXPORT_C TUid CSsmUiSpecific::StartupPSUid()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("StartupPSUid :: Calling S60 plugins functions ");
        return KPSUidStartup;        
        }
    else
        {
        DEBUGPRINT1A("StartupPSUid :: Calling Symbian(dummy) plugins functions ");
        return KPSStartupDefaultUid;
        }	
	}

EXPORT_C TUid CSsmUiSpecific::SecurityPinNotifierUid()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("SecurityPinNotifierUid :: Calling S60 plugins functions ");
        return KSecurityNotifierUid;  
        }
    else
        {
        DEBUGPRINT1A("SecurityPinNotifierUid :: Calling Symbian(dummy) plugins functions ");
        return KSecurityPinNotifierUid;
        }   

	}

EXPORT_C TUint CSsmUiSpecific::EmergencyCallPropertyKey()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("EmergencyCallPropertyKey :: Calling S60 plugins functions ");
        return KCTsyCallState;       
        }
    else
        {
        DEBUGPRINT1A("EmergencyCallPropertyKey :: Calling Symbian(dummy) plugins functions ");
        return KEmergencyCallPropertyKey;
        }   

	}

EXPORT_C TUid CSsmUiSpecific::EmergencyCallPropertyCategory()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("EmergencyCallPropertyCategory :: Calling S60 plugins functions ");
        return KPSUidCtsyCallInformation;       
        }
    else
        {
        DEBUGPRINT1A("EmergencyCallPropertyCategory :: Calling Symbian(dummy) plugins functions ");
        return KEmergencyCallPropertyCategory;
        }   

	}

EXPORT_C TBool CSsmUiSpecific::IsSimSupported()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("IsSimSupported :: Calling S60 plugins functions ");
        return SsmMapperUtility::FeatureStatus( TUid::Uid( KFeatureIdSimCard ) );       
        }
    else
        {
        DEBUGPRINT1A("IsSimSupported :: Calling Symbian(dummy) plugins functions ");
        return ETrue;
        }   

	}

EXPORT_C void CSsmUiSpecific::SetSecurityStatus(const TStrtSecurityStatus& aSecurityStatus)
	{   
	iStrtSecurityStatus = aSecurityStatus;
	}

EXPORT_C TStrtSecurityStatus CSsmUiSpecific::SecurityStatus() const
	{   
	return iStrtSecurityStatus;
	}

EXPORT_C CSsmUiSpecific* CSsmUiSpecific::InstanceL()
	{
	CSsmUiSpecific* self;

	//Check Tls data
	if (NULL == Dll::Tls())
		{
		//Instantiate CSsmUiSpecific if TLS is null
		self = new (ELeave) CSsmUiSpecific();
		CleanupStack::PushL(self);
		self->ConstructL();
		//Copy CSsmUiSpecific pointer in TLS
		User::LeaveIfError(Dll::SetTls(self));
		CleanupStack::Pop(self);
		}
	else
		{
		//CSsmUiSpecific has already been instantiated
		self = static_cast<CSsmUiSpecific*>(Dll::Tls());
		++self->iReferenceCount;
		}
	return self;
	}

EXPORT_C void CSsmUiSpecific::Release()
	{
	   TAny* tlsPtr = Dll::Tls();
	__ASSERT_DEBUG(NULL != tlsPtr, User::Panic(KPanicSsmUiSpecific, KErrNotFound));
 
    CSsmUiSpecific* self = static_cast<CSsmUiSpecific*>(tlsPtr);
    if (0 == --self->iReferenceCount)
        {
        Dll::FreeTls();
        delete self;
        }
	}

EXPORT_C TUid CSsmUiSpecific::ScreenOutputChannelUid()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("ScreenOutputChannelUid :: Calling S60 plugins functions ");
        return KSecurityNotifierChannel;       
        }
    else
        {
        DEBUGPRINT1A("ScreenOutputChannelUid :: Calling Symbian(dummy) plugins functions ");
        return KScreenOutputChannel;
        }   

	}

EXPORT_C TUint CSsmUiSpecific::SimStatusPropertyKey()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("SimStatusPropertyKey :: Calling S60 plugins functions ");
        return KPSSimStatus;        
        }
    else
        {
        DEBUGPRINT1A("SimStatusPropertyKey :: Calling Symbian(dummy) plugins functions ");
        return KSimStatusPropertyKey;
        }   
	}

EXPORT_C TBool CSsmUiSpecific::IsSimStateChangeAllowed()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("IsSimStateChangeAllowed :: Calling S60 plugins functions ");
        TSsmState state;
        TInt err = SsmMapperUtility::GetCurrentState( state );
        ERROR( err, "Failed to get current state" );
        return ( err == KErrNone &&
                 ( state.MainState() == ESsmStartup && state.SubState() == ESsmStateSecurityCheck ) ||
                 ( state.MainState() == ESsmStartup && state.SubState() == ESsmStateNonCritical ) ||
                   state.MainState() == ESsmNormal );        
        }
    else
        {
        DEBUGPRINT1A("IsSimStateChangeAllowed :: Calling Symbian(dummy) plugins functions ");
        return ETrue;
        }   

	}

EXPORT_C TBool CSsmUiSpecific::IsAmaStarterSupported()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("IsAmaStarterSupported :: Calling S60 plugins functions ");
        return SsmMapperUtility::FeatureStatus( TUid::Uid( KFeatureIdExtendedStartup ) );       
        }
    else
        {
        DEBUGPRINT1A("IsAmaStarterSupported :: Calling Symbian(dummy) plugins functions ");
        return ETrue;
        }   

	}

EXPORT_C HBufC* CSsmUiSpecific::GetTsyModuleNameL()
	{
    DEBUGPRINT1A("GetTsyModuleNameL :: Calling S60 plugins functions ");
    /*****************************************************
    *   Series 60 Customer / TSY
    *   Needs customer TSY implementation
    *****************************************************/

    HBufC* name = KMmTsyModuleName().AllocL();
    INFO_1( "TSY name: %S", name );
    return name;      
	}

EXPORT_C TUid CSsmUiSpecific::StarterPSUid()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("StarterPSUid :: Calling S60 plugins functions ");
        return KPSStarterUid;        
        }
    else
        {
        DEBUGPRINT1A("StartupPSUid :: Calling Symbian(dummy) plugins functions ");
        return KSecurityStatusPropertyCategory;
        }   
	}

EXPORT_C HBufC* CSsmUiSpecific::PhoneTsyNameL()
	{
    DEBUGPRINT1A("PhoneTsyNameL :: Calling S60 plugins functions ");
    /*****************************************************
    *   Series 60 Customer / TSY
    *   Needs customer TSY implementation
    *****************************************************/

    HBufC* name = KMmTsyPhoneName().AllocL();
    INFO_1( "Phone name: %S", name );
    return name;     
	}

EXPORT_C TBool CSsmUiSpecific::IsSimPresent()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("IsSimPresent :: Calling S60 plugins functions ");
        TInt value( 0 );
        TInt err = RProperty::Get( SsmMapperUtility::PsUid( KPSUidStartup ), KPSSimStatus, value );
        ERROR( err, "Failed to get value of KPSUidStartup::KPSSimStatus" );
        return ( err == KErrNone &&
                 ( value == ESimUsable ||
                   value == ESimReadable ||
                   value == ESimNotReady ) );       
        }
    else
        {
        DEBUGPRINT1A("IsSimPresent :: Calling Symbian(dummy) plugins functions ");
        return ETrue;
        }   
	}

EXPORT_C TBool CSsmUiSpecific::IsSimlessOfflineSupported()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("IsSimlessOfflineSupported :: Calling S60 plugins functions ");
        return SsmMapperUtility::FeatureStatus(
            TUid::Uid( KFeatureIdFfSimlessOfflineSupport ) );      
        }
    else
        {
        DEBUGPRINT1A("IsSimlessOfflineSupported :: Calling Symbian(dummy) plugins functions ");
        return ETrue;
        }   
	}

EXPORT_C TBool CSsmUiSpecific::IsNormalBoot()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("IsNormalBoot :: Calling S60 plugins functions ");
        TInt value( 0 );
        TInt errorCode = RProperty::Get( SsmMapperUtility::PsUid( KPSUidStartup ), KPSStartupReason, value );
        ERROR( errorCode, "Failed to get value of KPSUidStartup::KPSStartupReason" );

        TBool ret = ( errorCode == KErrNone && value == ENormalStartup );
        INFO_1( "Is normal boot = %d", ret );
        return ret;     
        }
    else
        {
        DEBUGPRINT1A("IsNormalBoot :: Calling Symbian(dummy) plugins functions ");
        return ETrue;
        }   
	}

EXPORT_C TBool CSsmUiSpecific::IsSimChangedReset()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("IsSimChangedReset :: Calling S60 plugins functions ");
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
    else
        {
        DEBUGPRINT1A("IsSimChangedReset :: Calling Symbian(dummy) plugins functions ");
        return ETrue;
        }   
	}

EXPORT_C TUint CSsmUiSpecific::RFStatusPropertyKey()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("RFStatusPropertyKey :: Calling S60 plugins functions ");
        return SWP_UID_SSM_RF_STATUS;        
        }
    else
        {
        DEBUGPRINT1A("RFStatusPropertyKey :: Calling Symbian(dummy) plugins functions ");
        return KRFStatusPropertyKey;
        }   
	}

EXPORT_C TUid CSsmUiSpecific::RFStatusPropertyCategory()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("RFStatusPropertyCategory :: Calling S60 plugins functions ");
        return TUid::Uid( SWP_UID_SSM_RF_STATUS );       
        }
    else
        {
        DEBUGPRINT1A("RFStatusPropertyCategory :: Calling Symbian(dummy) plugins functions ");
        return KRFStatusPropertyCategory;
        }   
	}

EXPORT_C TUint CSsmUiSpecific::ValidateRTCPropertyKey()
    {
    if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("ValidateRTCPropertyKey :: Calling S60 plugins functions ");
        return KRTCPropertyKey;       
        }
    else
        {
        DEBUGPRINT1A("ValidateRTCPropertyKey :: Calling Symbian(dummy) plugins functions ");
        return KValidateRTCPropertyKey;
        }   

    }

EXPORT_C TUid CSsmUiSpecific::ValidateRTCPropertyCategory()
    {
    if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("ValidateRTCPropertyCategory :: Calling S60 plugins functions ");
        return KPSStarterUid;        
        }
    else
        {
        DEBUGPRINT1A("ValidateRTCPropertyCategory :: Calling Symbian(dummy) plugins functions ");
        return KValidateRTCPropertyCategory;
        }   
    }

EXPORT_C TInt CSsmUiSpecific::PhoneMemoryRootDriveId()
	{
	if (!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("PhoneMemoryRootDriveId :: Calling S60 plugins functions ");
        TInt driveId = EDriveC;
        TDriveId driveIdBuf = PathInfo::PhoneMemoryRootPath().Left( KDriveIdLength );
        if ( driveIdBuf.Length() > 0 )
            {
            TInt err = RFs::CharToDrive( driveIdBuf[0], driveId ) ;
            INFO_1( "PhoneMemoryRootDriveId - CharToDrive returns: %d", err );            
            }

        INFO_1( "Phone memory root path ID: %d", driveId );
        return driveId;      
        }
    else
        {
        DEBUGPRINT1A("PhoneMemoryRootDriveId :: Calling Symbian(dummy) plugins functions ");
        return EDriveC;
        }   
	}

/**
Leaving construction inside ConstructL
*/
void CSsmUiSpecific::ConstructL()
	{
	User::LeaveIfError( iReservedPhoneMemoryFs.Connect() );
	}

/**
    Helper function to check for P&S Key
*/
TBool CSsmUiSpecific::IsTestPsKeyDefined()
    {
    TBool testPsKeyDefined = EFalse;
    TInt result = RProperty::Get(KPropertyCategory, KMiscPluginPropertyKey, testPsKeyDefined);
    DEBUGPRINT3(_L("KMiscPluginPropertyKey %d Error %d"), testPsKeyDefined, result);
    if ((KErrNone != result) && (KErrNotFound != result))
        {
        User::Leave(result);
        }
    return testPsKeyDefined;
    }

/**
Reserve two ClusterSize in Phone Memory Space on H/W
and 512 bytes for __WINS__
@return KErrNone if successful or systemwide error
*/
EXPORT_C TInt CSsmUiSpecific::ReservePhoneMemorySpace()
    {
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
Free reserved bytes from Phone Memory Space. If aSpaceToFree is 0 bytes 
then free complete reserved memory
@param aSpaceToFree request to free memory
@return KErrNone if successful or systemwide error
*/
EXPORT_C TInt CSsmUiSpecific::FreeReservedPhoneMemorySpace(const TInt aSpaceToFree)
    {
    TInt errorCode(KErrGeneral);
	DEBUGPRINT3A("Reserved memory is = %d bytes, Request to free memory is = %d bytes", iReservedPhoneMemory, aSpaceToFree);
    if(0 < iReservedPhoneMemory)
        {
		if(0 == aSpaceToFree)
		    {
		   	//Free complete reserved phone memory
			errorCode = iReservedPhoneMemoryFs.ReserveDriveSpace( PhoneMemoryRootDriveId(), 0 );
			DEBUGPRINT2A("Freeing memory completed with = %d", errorCode);
			iReservedPhoneMemory = 0; 
		    }
		else
		    {
		    TInt newReserveSize = iReservedPhoneMemory - aSpaceToFree;
		    newReserveSize = newReserveSize >= 0 ? newReserveSize : 0;
		    errorCode = iReservedPhoneMemoryFs.ReserveDriveSpace( PhoneMemoryRootDriveId(), newReserveSize );
			DEBUGPRINT2A("Freeing partial phone memory completed with = %d", errorCode);
		    if(KErrNone == errorCode)
			    {
			    iReservedPhoneMemory = newReserveSize;
			    }
		    }
    	}
    return errorCode;
    }
