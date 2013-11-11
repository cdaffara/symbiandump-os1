/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains Feature Manager related definitions for use by the test Feature Manager stub.
*
*/




#ifndef FEATURECMN_H
#define FEATURECMN_H

#include <e32cmn.h>
#include <babitflags.h>

const TInt KFeatureUnsupported( 0 );
const TInt KFeatureSupported( 1 );

/** An enumeration for accessing feature status flags.
 @see TBitFlagsT
*/
enum TFeatureFlags
    {
    /** If set, feature is supported and available for use; 
     if not, feature is not supported.
    */
    EFeatureSupported      = 0,
    /** If set, feature is upgradeable. The feature is known to the device
     but it must be upgraded to enable it. If a feature s blacklisted, 
     its upgradeable flag is unset. 
    */
    EFeatureUpgradeable    = 1,
    /** If set, the feature is modifiable and may be enabled/disabled 
     at run-time. The initial flag values for such a feature flag are
     defined in a ROM image obey file.
    */
    EFeatureModifiable     = 2,
    /** If set, the feature has been blacklisted, and may not be changed at 
     run-time. This also prevents a feature from being upgraded.
    */
    EFeatureBlackListed    = 3,
    /** If set, this flag Supported state is unknown at build-time and is
     initialised at run-time by system software. The Feature Manager will
     ignore the Supported flag in the file. A run-time call to RFeatureControl
     will be needed to set the feature's supported flag. Look ups of 
     uninitialised features result in a KErrNotReady error code.
    */
    EFeatureUninitialized  = 4,
    /** If set, this flag is saved to the system drive when modified 
     preserving its value across reboots/power downs.
    */
    EFeaturePersisted      = 5,
    
    // Bits 6..23 Reserved for Symbian to define for future use, always zero.
    
    // High byte bits are undefined, reserved for internal use
    EFeatureReserved24     = 24,
    EFeatureReserved25     = 25,
    EFeatureReserved26     = 26,
    EFeatureReserved27     = 27,
    EFeatureReserved28     = 28,
    EFeatureReserved29     = 29,
    EFeatureReserved30     = 30,
    EFeatureReserved31     = 31
    
    };

/** An enumeration for checking feature change type.
*/
enum TFeatureChangeType 
    { 
    /** Feature status not changed.
    */
    EFeatureNoChange            = 0,
    /** Feature status changed to enabled or disabled.
    */
    EFeatureStatusUpdated       = 1,
    /** Feature data changed.
    */
    EFeatureDataUpdated         = 2,    
    /** Feature status and data changed.
    */
    EFeatureStatusDataUpdated   = 3,
    /** Not used, future: complex change occurred, reload all features.
    */
    EFeatureRediscover          = 4,
    /** Not used, future: new feature has been added to system.
    */
    EFeatureFeatureCreated      = 5
    };

/**
 Feature entry class.
 Used by Feature Manager and its clients.

*/

class TFeatureEntry
    {
    public:
    
        /** 
         Default constructor.
         Needed by clients using the method 
         FeatureSupported( TFeatureEntry& aFeature ) of Feature Control API. 
        
         @param aFeature Feature UID.
        */
        IMPORT_C TFeatureEntry( TUid aFeature );

        /** 
         Default constructor.
         Needed by clients using the method 
         AddFeature( TFeatureEntry& aFeature ) of Feature Control API.
        
         @param aFeature Feature UID.
         @param aFlags Feature status flags.
         @param aData Feature data.
        */
        IMPORT_C TFeatureEntry( TUid aFeature, TBitFlags32 aFlags, TUint32 aData );
            
        /** 
         Returns feature UID.
        */
        IMPORT_C TUid FeatureUid() const;

        /** 
         Returns feature status flags.
        */
        IMPORT_C TBitFlags32 FeatureFlags() const;

        /** 
         Returns feature data.
        */
        IMPORT_C TUint32 FeatureData() const;
        
    public:
        /** 
         Default constructor.
         Used by Feature Manager.
        */
        IMPORT_C TFeatureEntry();
        
    private:

        /** UID of the feature. */ 
        TUid iFeatureID;
        /** 
         Value of the feature status flags.
         @see TFeatureFlags
        */
        TBitFlags32 iFlags;
        /** Data associated with feature. */
        TUint32 iData;
        /**
	     Reserved for future use. 
	     Initialised to 0 on construction.
	    */
        TUint32 iReserved;
    };


/**
 Defines the feature UID array. 
*/
typedef RArray<TUid> RFeatureUidArray;

/**
 Defines  the feature entry array. 
*/
typedef RArray<TFeatureEntry> RFeatureArray;

#endif  // FEATURECMN_H

// End of File
