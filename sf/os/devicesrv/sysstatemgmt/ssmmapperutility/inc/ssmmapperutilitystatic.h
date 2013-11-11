/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of SsmMapperUtility class.
*
*/

#ifndef SSMMAPPERUTILITYSTATIC_H
#define SSMMAPPERUTILITYSTATIC_H

#include <e32base.h>

class TSsmState;

/**
* Utility class for mapping Uids, getting feature and system states and
* cenrep values.
* This class contains static versions of the utilities provided by
* CSSmMapperUtility. Use static functions provided here sparingly,
* because each function creates own instance of CSSmMapperUtility.
*/
class SsmMapperUtility
    {
public:

    /**
     * Methods for mapping p&s, cenrep and feature Uids
     *
     * @param aUid The Uid to map
     * @return The mapped Uid
     */
    IMPORT_C static TUid PsUid( const TUid& aUid );
    IMPORT_C static TUid CrUid( const TUid& aUid );
    IMPORT_C static TUid FeatureUid( const TUid& aUid );

    /**
     * Get the current system state
     *
     * @param aState On return contains the current state
     * @return KErrNone if system state read successfully.
     *         Otherwise one of Symbian error codes. 
     */
    IMPORT_C static TInt GetCurrentState( TSsmState& aState );

    /**
     * Gets the requested feature Uid's status
     *
     * @param aUid Requested feature Uid
     * @return ETrue if feature supported, otherwise EFalse
     */
    IMPORT_C static TBool FeatureStatus( const TUid& aUid );

    /**
     * Gets the requested cenrep key's value
     *
     * @param aUid Requested cenrep Uid
     * @param aKey Requested cenrep key
     * @param aValue On return contains value of requested key
     * @return KErrNone if value read successfully.
     *         Otherwise one of Symbian error codes.
     */
    IMPORT_C static TInt CrValue( const TUid& aUid, TUint32 aKey, TInt& aValue );

    /**
     * Gets the requested cenrep key's value
     *
     * @param aUid Requested cenrep Uid
     * @param aKey Requested cenrep key
     * @param aValue On return contains value of requested key
     * @return KErrNone if descriptor read successfully.
     *         Otherwise one of Symbian error codes.
     */
    IMPORT_C static TInt CrValue( const TUid& aUid, TUint32 aKey, TDes& aValue );

    /**
     * Gets the path to command lists
     *
     * @param aCmdListPath Default path. Will be modified if called from test code.
     */
    IMPORT_C static void GetCommandListPath( TDes& aCmdListPath );

    /**
     * Checks the capabilities of client
     *
     * @aMessage Client message
     * @return ETrue if capabilities ok, otherwise EFalse
     */
    IMPORT_C static TBool CheckCaps( const RMessagePtr2& aMessage );
    };

#endif // SSMMAPPERUTILITYSTATIC_H
