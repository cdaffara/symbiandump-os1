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
 * Description:  Base class for audio unit objects.
 *
 */

#ifndef ACCPOLAUDIOUNITBASE_H
#define ACCPOLAUDIOUNITBASE_H

// SYSTEM INCLUDES
#include <accpolobjectbase.h>
#include <accpolobjecttypes.h>

// FORWARD DECLARATIONS
class CAccPolAudioControlBase;

/**
 *  Base class for audio unit objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolAudioUnitBase ) : public CAccPolObjectBase
    {
public:

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAccPolAudioUnitBase();

    /**
     * Assign operator for audio unit base class.
     *
     * @param CAccPolAudioUnitBase, Const reference to base class to be copied.
     * @return CAccPolAudioUnitBase, Reference to object pointer.
     */
    IMPORT_C CAccPolAudioUnitBase& operator= ( const CAccPolAudioUnitBase& );

    /**
     * Returns the audio unit Id.
     *
     * @return TUint8, Value of the audio unit Id.
     */
    IMPORT_C TUint8 UnitId() const;

    /**
     * Returns the source Id.
     *
     * in USB case, Id where this audio unit is linked.
     * If audio unit cointains more than one Source Ids then the first item of the
     * source Ids list is returned. Use GetSourceIds method to get list of source Ids.
     *
     * @return TUint8, Value of the source Id.
     */
    IMPORT_C TUint8 SourceId() const;

    /**
     * Returns an array which contains list of source Ids.
     *
     * in USB case, Id(s) where this audio unit is linked.
     *
     * @param aSourceIds, Array which inludes source Ids.
     * @return void
     */
    IMPORT_C void GetSourceIds( RArray<TInt>& aSourceIds ) const;

    /**
     * Returns the source Id count.
     *
     * @return TInt, Source Id count.
     */
    IMPORT_C TInt SourceIdCount() const;

    /**
     * Source Id to store for this audio unit.
     *
     * @param aSourceId, Source Id to be stored.
     * @return void.
     */
    IMPORT_C void AddSourceIdL( TUint8 aSourceId );

    /**
     * Get an audio control object according to the object type and channel number.
     *
     * @param aObjectType object type, Following object types are supported EAccPolAudioVolumeControlObject,
     *                                                                      EAccPolAudioMuteControlObject,
     *                                                                      EAccPolAudioInputControlObject.
     * @param aChannel, Channel number. See accpolaudiotopologytypes.h.
     * @param aAudioControl, On return contains an audio control object if found, otherwise NULL.
     * @return Tbool, ETrue if audio control object found, otherwise EFalse.
     */
    IMPORT_C TBool GetAudioControlL( TTopologyConObjectType aObjectType,
        TUint32 aChannel,
        CAccPolAudioControlBase*& aAudioControl ) const;

    /**
     * Get a channel spesific audio control objects.
     *
     * @param aChannel, Channel number. See accpolaudiotopologytypes.h.
     * @param aAudioControls, On return contains an audio control objects.
     * If audio control objects not found empty array is returned.
     * @return void.
     */
    IMPORT_C void GetAudioControlsL( TUint32 aChannel,
        RPointerArray<CAccPolAudioControlBase>& aAudioControls ) const;

    /**
     * Gets all audio control objects of this audio unit.
     *
     * @param aAudioControls on return, contains the audio control objects. If any of an audio control objects not found
     *        empty array is returned.
     * @return void.
     */
    IMPORT_C void GetAudioControlsL( RPointerArray<CAccPolAudioControlBase>& aAudioControls ) const;

    /**
     * Get a next audio unit object where this audio unit is linked.
     *
     * If audio unit cointains several source id links then the first linked audio unit is returned.
     *
     * @param aAudioUnit, On return contains the audio unit object if found, otherwise NULL.
     * @return Tbool, ETrue if audio unit object found, otherwise EFalse.
     */
    IMPORT_C TBool GetNextAudioUnitL( CAccPolAudioUnitBase*& aAudioUnit ) const;

    /**
     * Get a previous audio unit object where this audio unit is linked.
     *
     * @param aAudioUnit, On return contains the audio unit object if found, otherwise NULL.
     * @return Tbool, ETrue if audio unit object found, otherwise EFalse.
     */
    IMPORT_C TBool GetPreviousAudioUnitL( CAccPolAudioUnitBase*& aAudioUnit ) const;

    /**
     * Get a list of audio units where this audio unit is linked.
     *
     * @param aAudioUnits, On return contains an audio unit objects.
     * If any of an audio unit objects not found empty array is returned.
     * @return void.
     */
    IMPORT_C void GetAudioUnitListL( RPointerArray<CAccPolAudioUnitBase>& aAudioUnits ) const;

    /**
     * Add an audio control object for this audio unit.
     *
     * @param aAudioControl an audio control object to be stored.
     * @return void.
     */
    IMPORT_C void AddControlL( CAccPolAudioControlBase& aAudioControl );

public:

    /**
     * Returns the previous audio unit Id ( internal use only ).
     */
    TUint8 PreviousUnitId() const;

    /**
     * Sets the previous audio unit Id ( internal use only ).
     */
    void SetPreviousUnitId( TUint8 aPreUnitId );

protected:

    /**
     * Externalize audio control base class.
     *
     * @param aStream, Write stream object.
     * @return void.
     */
    IMPORT_C void ExternalizeAudioUnitBaseL( RWriteStream& aStream ) const;

    /**
     * Internalize audio control base class.
     *
     * @param aStream, Read stream object.
     * @return void.
     */
    IMPORT_C void InternalizeAudioUnitBaseL( RReadStream& aStream );

protected:

    /**
     * C++ default constructor.
     */
    CAccPolAudioUnitBase( const TTopologyConObjectType aObjecType,
        CAccPolObjectCon* aObject = NULL );

    /**
     * C++ constructor.
     */
    CAccPolAudioUnitBase ( const TUint8 aUnitId,
        const TTopologyConObjectType aObjecType );

    /**
     * C++ constructor.
     */
    CAccPolAudioUnitBase ( const TUint8 aUnitId,
        const TUint8 aSourceId,
        const TTopologyConObjectType aObjecType );

private:
    void CopyL ( const CAccPolAudioUnitBase& aAudioUnitBaseObject );
    
private:

    /**
     * Audio unit Id.
     */
    TUint8 iUnitId;

    /**
     * Audio unit Source Ids.
     */
    RArray<TInt> iSourceId;

    /**
     * Previous audio unit Id.
     */
    TUint8 iPreUnitId;

    /**
     * Control objects of this audio unit.
     */
    RPointerArray<CAccPolAudioControlBase> iAudioControls;
    };

#endif // ACCPOLAUDIOUNITBASE_H
