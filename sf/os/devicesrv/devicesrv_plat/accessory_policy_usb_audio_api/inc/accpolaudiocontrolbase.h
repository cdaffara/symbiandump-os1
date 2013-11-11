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
 * Description:  Base class for audio control objects.
 *
 */

#ifndef ACCPOLAUDIOCONTROLBASE_H
#define ACCPOLAUDIOCONTROLBASE_H

// SYSTEM INCLUDES
#include <accpolobjectbase.h>
#include <accpolobjecttypes.h>

// DATA TYPES
/**
 * Supported audio controls for feature unit.
 */
const TUint32 KAccMuteControl   = 0x1; // 00000 00000 00000 00001
const TUint32 KAccVolumeControl = 0x2; // 00000 00000 00000 00010

/**
 * Predefined channel types.
 */
const TUint32 KAccMasterChannel     = 0x0;
const TUint32 KAccLeftFrontChannel  = 0x1; // 00000 00000 00000 00001
const TUint32 KAccRightFrontChannel = 0x2; // 00000 00000 00000 00010
const TUint32 KAccChannelCount      = 3;

/**
 *  Base class for audio control objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolAudioControlBase ) : public CAccPolObjectBase
    {
public:

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAccPolAudioControlBase();

    /**
     * Assign operator for audio control base class.
     *
     * @param CAccPolAudioControlBase, Const reference to object to be copied.
     * @return CAccPolAudioControlBase, Reference to object pointer.
     */
    IMPORT_C CAccPolAudioControlBase& operator= ( const CAccPolAudioControlBase& );

    /**
     * Returns the audio unit Id, where the audio control parameters are associated.
     *
     * @return TInt, Value of the audio unit Id.
     */
    IMPORT_C TUint8 UnitId() const;

    /**
     * Sets the unit Id.
     *
     * @param aUnitId, Unit Id to be stored.
     * @return void.
     */
    IMPORT_C void SetUnitId( TUint8 aUnitId );

    /**
     * Sets a channel number where this audio control parameter(s) is/are associated.
     *
     * @param aChannel, Channel number. See accpolaudiotopologytypes.h.
     * @return void.
     */
    IMPORT_C void SetChannel( TUint32 aChannel );

    /**
     * Returns the channel number where the audio control parameter(s) is/are associated.
     *
     * @return TUint32, Value of channel number.
     */
    IMPORT_C TUint32 Channel() const;

protected:

    /**
     * Externalize audio control base class.
     *
     * @param aStream, Write stream object.
     * @return void.
     */
    IMPORT_C void ExternalizeAudioControlbaseL( RWriteStream& aStream ) const;

    /**
     * Internalize audio control base class.
     *
     * @param aStream, Read stream object.
     * @return void.
     */
    IMPORT_C void InternalizeAudioControlbaseL( RReadStream& aStream );

protected:

    /**
     * C++ default constructor.
     */
    IMPORT_C CAccPolAudioControlBase( TTopologyConObjectType aObjectType,
        CAccPolObjectCon* aObject = NULL );

    /**
     * C++ default constructor.
     */
    IMPORT_C CAccPolAudioControlBase( TTopologyConObjectType aObjectType,
        TUint32 aChannel );

private:

    /**
     * Unit Id.
     */
    TUint8 iUnitId;

    /**
     * Channel number.
     */
    TUint32 iChannel;
    };

#endif // ACCPOLAUDIOCONTROLBASE_H
