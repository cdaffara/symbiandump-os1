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
 * Description:  Latency object definition
 *
 */

#ifndef ACCPOLHDMILATENCY_H
#define ACCPOLHDMILATENCY_H

#include <e32cmn.h>
#include <accpolobjectbase.h>

namespace HdmiLatency
    {
    /** Common latency / progressive video specific latency. */
    const TUid KUidLatency =
        {
        0x1000001
        };

    /** Interlaced video specific latency. */
    const TUid KUidInterlacedLatency =
        {
        0x1000002
        };
    }

/**
 *  Store's audio and video latency parameters.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolHdmiLatency ) : public CAccPolObjectBase
    {

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolHdmiLatency();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiLatency* NewL(
        CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiLatency* NewL( const TUid aLatencyType,
        const TUint32 aAudioLatency,
        const TUint32 aVideoLatency );
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiLatency* NewLC(
        CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiLatency* NewLC( const TUid aLatencyType,
        const TUint32 aAudioLatency,
        const TUint32 aVideoLatency );

    /**
     * Assign operator for audio latency class.
     *
     * @param CAccPolHdmiLatency, Const reference to object to be copied.
     * @return CAccPolHdmiLatency, Reference to object pointer.
     */
    IMPORT_C CAccPolHdmiLatency& operator= ( const CAccPolHdmiLatency& );

    /**
     * Sets the audio format.
     *
     * @param aLatencyType, Latency type to be stored.
     * @return void.
     */
    IMPORT_C void SetLatencyType( const TUid aLatencyType );

    /**
     * Sets the audio latency.
     *
     * @param aAudioLatency, Audio latency in ms
     * @return void.
     */
    IMPORT_C void SetAudioLatency( const TUint32 aAudioLatency );

    /**
     * Sets the video latency.
     *
     * @param aVideoLatency, Video latency in ms
     * @return void.
     */
    IMPORT_C void SetVideoLatency( const TUint32 aVideoLatency );

    /**
     * Returns the latency type.
     *
     * @return TUid, Value of latency type.
     */
    IMPORT_C const TUid LatencyType() const;

    /**
     * Returns the audio latency.
     *
     * @return TUint32, Value audio latency in ms.
     */
    IMPORT_C TUint32 AudioLatency() const;

    /**
     * Returns the video latency.
     *
     * @return TUint32, Value video latency in ms.
     */
    IMPORT_C TUint32 VideoLatency() const;

public: // From base classes

    // @see CAccPolObjectBase
    virtual CAccPolObjectBase* DuplicateLC( CAccPolObjectCon* aObject = NULL );

    // @see CAccPolObjectBase
    virtual void InternalizeL( RReadStream& aStream );

    // @see CAccPolObjectBase
    virtual void ExternalizeL( RWriteStream& aStream ) const;

protected:

    /**
     * C++ default constructor.
     */
    CAccPolHdmiLatency( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ default constructor.
     */
    CAccPolHdmiLatency( const TUid aLatencyType,
        const TUint32 aAudioLatency,
        const TUint32 aVideoLatency );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /**
     * Latency Type
     */
    TUid iLatencyType;

    /**
     * Audio Latency in ms
     * Range:  0 - 500
     * Unit:   milliseconds
     * Source: HDMI Specification 1.3a; Vendor-Specific Data Block - (Interlaced_)Audio_Latency, ( expressed as (ms/2)+1 in data block: 251 in data block equals 500 ms )
     */
    TUint32 iAudioLatency;

    /**
     * Video Latency in ms
     * Range:  0 - 500
     * Unit:   milliseconds
     * Source: HDMI Specification 1.3a; Vendor-Specific Data Block - (Interlaced_)Video_Latency, ( expressed as (ms/2)+1 in data block: 251 in data block equals 500 ms )
     */
    TUint32 iVideoLatency;
    };

/** Latency object array */
typedef RPointerArray<CAccPolHdmiLatency> RAccPolHdmiLatencyArray;

#endif // ACCPOLHDMILATENCY_H
