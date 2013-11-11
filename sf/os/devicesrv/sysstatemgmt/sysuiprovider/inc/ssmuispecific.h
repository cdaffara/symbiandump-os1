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
* Declaration of CSsmUiSpecific class.
*
*/

#ifndef SSMUISPECIFIC_H
#define SSMUISPECIFIC_H

#include <e32base.h>
#include <ssm/strtsecuritystatus.h>

/**
 * Class providing S60 specific identifiers and statuses
 * of runtime changing or configuration dependent features.
 */
NONSHARABLE_CLASS( CSsmUiSpecific ) : public CBase
    {

public:

    /**
     * Destructor.
     */
    IMPORT_C ~CSsmUiSpecific();

    /**
     * Get the instance of CSsmUiSpecific class.
     * There is at most one instance of CSsmUiSpecific per thread, and it is
     * stored in TLS.
     * This function creates the instance if it does not exist, and increments
     * reference count otherwise.
     */
    IMPORT_C static CSsmUiSpecific* InstanceL();

    /**
     * Decrements the reference count of the CSsmUiSpecific instance and
     * deletes the object if reference count reaches zero.
     */
    IMPORT_C static void Release();

    /**
     * Return KPSUidStartup.
     */
    IMPORT_C static TUid StartupPSUid();

    /**
     * Return KPSStarterUid.
     */
    IMPORT_C static TUid StarterPSUid();

    /**
     * Return KPSUidCtsyCallInformation.
     */
    IMPORT_C static TUid EmergencyCallPropertyCategory();

    /**
     * Return KSecurityNotifierUid.
     */
    IMPORT_C static TUid SecurityPinNotifierUid();

    /**
     * Return KSecurityNotifierChannel.
     */
    IMPORT_C static TUid ScreenOutputChannelUid();

    /**
     * Return SWP_UID_SSM_RF_STATUS.
     */
    IMPORT_C static TUid RFStatusPropertyCategory();

    /**
     * Return KCTsyCallState.
     */
    IMPORT_C static TUint EmergencyCallPropertyKey();

    /**
     * Return KPSSimStatus.
     */
    IMPORT_C static TUint SimStatusPropertyKey();

    /**
     * Return SWP_UID_SSM_RF_STATUS.
     */
    IMPORT_C static TUint RFStatusPropertyKey();

    /**
     * Return KMmTsyModuleName.
     */
    IMPORT_C static HBufC* GetTsyModuleNameL();

    /**
     * Return KMmTsyPhoneName.
     */
    IMPORT_C static HBufC* PhoneTsyNameL();

    /**
     * Store current security status for later use.
     */
    IMPORT_C void SetSecurityStatus( const TStrtSecurityStatus& aSecurityStatus );

    /**
     * Return the stored security status security status.
     */
    IMPORT_C TStrtSecurityStatus SecurityStatus() const;

    /**
     * Return status of KFeatureIdExtendedStartup feature flag
     */
    IMPORT_C static TBool IsAmaStarterSupported();

    /**
     * Return status of KFeatureIdSimCard feature flag.
     */
    IMPORT_C static TBool IsSimSupported();

    /**
     * Return status of KFeatureIdFfSimlessOfflineSupport feature flag.
     */
    IMPORT_C static TBool IsSimlessOfflineSupported();

    /**
     * Return ETrue if sim present.
     */
    IMPORT_C static TBool IsSimPresent();

    /**
     * Return ETrue if system state is ESsmStartup::ESsmStateSecurityCheck,
     * ESsmStartup::ESsmStateNonCritical or ESsmNormal::Any.
     */
    IMPORT_C static TBool IsSimStateChangeAllowed();

    /**
     * Return ETrue if reset reason is ENormalStartup.
     */
    IMPORT_C static TBool IsNormalBoot();

    /**
     * Return ETrue if reset reason is ESIMStatusChangeReset and the sim card
     * has been changed since last boot.
     */
    IMPORT_C static TBool IsSimChangedReset();

    /**
     * Return RTC Property Key.
     */
    IMPORT_C static TUint ValidateRTCPropertyKey();

    /**
     * Return RTC Property Category.
     */
    IMPORT_C static TUid ValidateRTCPropertyCategory();

    /**
     * Return PathInfo::PhoneMemoryRootPath().
     */
    IMPORT_C static TInt PhoneMemoryRootDriveId();

	/**
	Reserve two ClusterSize in Phone Memory Space on H/W
	and 512 bytes for __WINS__
	@return KErrNone if successful or systemwide error
	*/
	IMPORT_C TInt ReservePhoneMemorySpace();

	/**
	Free reserved bytes from Phone Memory Space. If aSpaceToFree is 0 bytes 
	then free complete reserved memory
	@param aSpaceToFree request to free memory
	@return KErrNone if successful or systemwide error
	*/
	IMPORT_C TInt FreeReservedPhoneMemorySpace(const TInt aSpaceToFree);

private:

    /**
     * Constructor.
     */
    CSsmUiSpecific();
	
	void ConstructL();

private: // data

    TStrtSecurityStatus iStrtSecurityStatus;
    TInt iReferenceCount;
	RFs iReservedPhoneMemoryFs;
	TInt iReservedPhoneMemory;
    };

#endif // SSMUISPECIFIC_H
