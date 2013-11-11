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
* Description:  Definition for volume control object.
*
*/


#ifndef ACCPOLVOLUMECONTROL_H
#define ACCPOLVOLUMECONTROL_H


#include <accpolaudiocontrolbase.h>


/**
 *  Class definition for volume control objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolVolumeControl ) : public CAccPolAudioControlBase
    {

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolVolumeControl();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolVolumeControl* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolVolumeControl* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     * @param aVolumeDb, Current volume level in Db.
     * @param aMinVolumeDb, Minimum volume level in Db.
     * @param aMaxVolumeDb, Maximum volume level in Db.
     * @param aVolumeResDb, Volume level resolution in Db.
     */
    IMPORT_C static CAccPolVolumeControl* NewL( TUint32 aChannel,
                                                const TReal aVolumeDb,
                                                const TReal aMinVolumeDb,
                                                const TReal aMaxVolumeDb,
                                                const TReal aVolumeResDb );

    /**
     * Two-phased constructor.
     * @param aVolumeDb, Current volume level in Db.
     * @param aMinVolumeDb, Minimum volume level in Db.
     * @param aMaxVolumeDb, Maximum volume level in Db.
     * @param aVolumeResDb, Volume level resolution in Db.
     */
    IMPORT_C static CAccPolVolumeControl* NewLC( TUint32 aChannel,
                                                 const TReal aVolumeDb,
                                                 const TReal aMinVolumeDb,
                                                 const TReal aMaxVolumeDb,
                                                 const TReal aVolumeResDb );

    /**
     * Assign operator for volume control object class.
     *
     * @param CAccPolVolumeControl, Const reference to base class to be copied.
     * @return CAccPolVolumeControl, Reference to object pointer.
     */
    IMPORT_C CAccPolVolumeControl& operator= ( const CAccPolVolumeControl& );

    /**
     * Set volume level.
     *
     * @param aVolumeDb, Volume level in Db.
     * @return void.
     */
    IMPORT_C void SetVolumeDb( TReal aVolumeDb );

    /**
     * Set minimum volume level.
     *
     * @param aMinVolumeDb, Minimum volume level in Db.
     * @return void.
     */
    IMPORT_C void SetMinVolumeDb( TReal aMinVolumeDb );

    /**
     * Set maximum volume level.
     *
     * @param aMaxVolumeDb, Maximum volume level in Db.
     * @return void.
     */
    IMPORT_C void SetMaxVolumeDb( TReal aMaxVolumeDb );

    /**
     * Sets volume level resolution.
     *
     * @param aVolumeResDb, Volume level resolution in Db.
     * @return void.
     */
    IMPORT_C void SetVolumeResDb( TReal aVolumeResDb );

    /**
     * Returns the current volume level.
     *
     * @return TReal, Current volume level in Db.
     */
    IMPORT_C TReal VolumeDb() const;

   /**
     * Returns the minimum volume level.
     *
     * @return TReal, Minimum volume level in Db.
     */
    IMPORT_C TReal MinVolumeDb() const;

    /**
     * Returns the maximum volume level.
     *
     * @return TReal, Maximum volume level in Db.
     */
    IMPORT_C TReal MaxVolumeDb() const;

    /**
     * Returns the volume level resolution.
     *
     * @return TReal, Volume level resolution in Db.
     */
    IMPORT_C TReal VolumeResDb() const;

    /**
     * Duplicates the object data.
     *
     * @return CAccPolObjectBase*, Pointer to duplicated object.
     */
    CAccPolObjectBase* DuplicateLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Externalize object.
     *
     * @param aStream, Write stream object.
     * @return void.
     */
    void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Internalize object.
     *
     * @param aStream, Read stream object.
     * @return void.
     */
    void InternalizeL( RReadStream& aStream );

public:

    /**
     * Maps volume level from specific integer value to real decibel value.
     * Range are defined in accpolvolumecontrol.cpp
     */
    TReal MapToDb( TUint16 aValue ) const;

    /**
     * Maps volume level from decibel value to specific integer value
     * Range are defined in accpolvolumecontrol.cpp
     */
    TUint16 MapFromDb( TReal aValue ) const;

private:

    /**
     * C++ default constructor.
     */
    CAccPolVolumeControl( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ default constructor.
     */
    CAccPolVolumeControl( TUint32 aChannel,
                          const TReal aVolumeDb,
                          const TReal aMinVolumeDb,
                          const TReal aMaxVolumeDb,
                          const TReal aVolumeResDb );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /**
     * Volume level.
     */
    TReal iVolumeDb;

    /**
     * Min volume level.
     */
    TReal iMinVolumeDb;

    /**
     * Max volume level.
     */
    TReal iMaxVolumeDb;

    /**
     * Volume resolution
     */
    TReal iVolumeResDb;

    };

#endif // ACCPOLVOLUMECONTROL_H
