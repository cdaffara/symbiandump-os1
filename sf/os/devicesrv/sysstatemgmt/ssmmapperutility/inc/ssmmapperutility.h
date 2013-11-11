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
* Declaration of CSsmMapperUtility class.
*
*/

#ifndef SSMMAPPERUTILITY_H
#define SSMMAPPERUTILITY_H

#include <e32base.h>

class TSsmState;

/**
* Utility class for mapping Uids, getting feature
* and system states and cenrep values.
*
*/
class CSsmMapperUtility : public CBase
    {
public:
    
    /**
     * Two-phased constructor. Clients of mapper utility
     * should use this method for creating instance.
     */
    IMPORT_C static CSsmMapperUtility* NewL();

    /**
     * Destructor
     */
    IMPORT_C virtual ~CSsmMapperUtility();

    /**
     * Methods for mapping p&s, cenrep and feature Uids
     *
     * @param aUid The Uid to map
     * @return The mapped Uid 
     */
    virtual TUid PsUid( const TUid& aUid );
    virtual TUid CrUid( const TUid& aUid );
    virtual TUid FeatureUid( const TUid& aUid );

    /**
     * Get the current system state
     *
     * @param aState On return contains the current state
     * @return KErrNone if system state read successfully.
     *         Otherwise one of Symbian error codes. 
     */
    virtual TInt GetCurrentState( TSsmState& aState ) const;

    /**
     * Gets the requested feature Uid's status
     *
     * @param aUid Requested feature Uid
     * @return ETrue if feature supported, otherwise EFalse
     */
    IMPORT_C TBool FeatureStatus( const TUid& aUid );

    /**
     * Gets the requested cenrep key's value
     *
     * @param aUid Requested cenrep Uid
     * @param aKey Requested cenrep key
     * @param aValue On return contains value of requested key
     * @return KErrNone if value read successfully.
     *         Otherwise one of Symbian error codes.
     */
    IMPORT_C TInt CrValue( const TUid& aUid, TUint32 aKey, TInt& aValue );

    /**
     * Gets the requested cenrep key's value
     *
     * @param aUid Requested cenrep Uid
     * @param aKey Requested cenrep key
     * @param aValue On return contains value of requested key
     * @return KErrNone if descriptor read successfully.
     *         Otherwise one of Symbian error codes.
     */
    IMPORT_C TInt CrValue( const TUid& aUid, TUint32 aKey, TDes& aValue );

    /**
     * Gets the path to command lists
     *
     * @param aCmdListPath Default path. Will be modified if called from test code.
     */
    virtual void GetCommandListPath( TDes& aCmdListPath );

    /**
     * Checks the capabilities of client
     *
     * @aMessage Client message
     * @return ETrue if capabilities ok, otherwise EFalse
     */
    IMPORT_C virtual TBool CheckCaps( const RMessagePtr2& aMessage );
    
    /**
    * First phase constructor. Exported for 
    * deriver classes.
    */
    IMPORT_C CSsmMapperUtility();

private:
    
    /**
    * Second phase of construction.
    */
    void ConstructL();

private:

    // Extension library handle
    RLibrary iExtensionLib;
        
    // Extension instance. Can be NULL when extension is missing. Own.
    CSsmMapperUtility *iExtension;
    };

#endif // SSMMAPPERUTILITY_H
