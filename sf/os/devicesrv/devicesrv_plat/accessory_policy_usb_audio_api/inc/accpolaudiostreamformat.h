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
 * Description:  Accessory audio stream format.
 *
 */

#ifndef ACCPOLAUDIOSTREAMFORMAT_H
#define ACCPOLAUDIOSTREAMFORMAT_H

#include <e32cmn.h>
#include <accpolobjectbase.h>
#include <accpolobjecttypes.h>

/** Pulse Code Modulation. A common linear audio encoding format. */
const TUid KUidFormatPCM16 =
    {
    0x0000001
    };

/** Umbrella name for MPEG1 Layer 3, MPEG2 Layer 3 and
 MPEG 2.5 Layer 3 audio codec. */
const TUid KUidFormatMP3 =
    {
    0x0000002
    };

/** Adaptive multi-rate wide band speech codec. */
const TUid KUidFormatAWB =
    {
    0x0000003
    };

/** Low Complexity Sub Band Coding. */
const TUid KUidFormatSBC =
    {
    0x0000004
    };

/** Mobile baseline speech. */
const TUid KUidFormatAMR =
    {
    0x0000005
    };

/** MPEG-4 advanced audio coding. */
const TUid KUidFormatAAC =
    {
    0x0000006
    };

/** Extended AAC version for low bit rates. */
const TUid KUidFormatEAAC =
    {
    0x0000007
    };

/** Speech coding by scalar quantization. */
const TUid KUidFormatG711 =
    {
    0x0000008
    };

/** Internet low bit rate codec. */
const TUid KUidFormatILBC =
    {
    0x0000009
    };

/** A low bit rate speech coder standard  */
const TUid KUidFormatG729 =
    {
    0x000000A
    };

/** Used to refer non coded 8 bit linear audion */
const TUid KUidFormatRA8 =
    {
    0x000000B
    };

/**
 *  Store's an audio stream format parameters.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolAudioStreamFormat ) : public CAccPolObjectBase
    {

public:

    /**
     * Stream type.
     */
    enum TStreamType
        {
        EStreamNone = 0,
        EFormatSpecificStreamType,
        EMono,
        EStereo
        };

    /**
     * Bit resolution.
     */
    enum TBitResolution
        {
        EFormatNone = 0,
        EFormatSpecificBitResolution,
        EBitsPerSample16,
        EBitsPerSample24,
        EBitsPerSample32,
        };

    /**
     * Destructor.
     */
    virtual ~CAccPolAudioStreamFormat();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolAudioStreamFormat* NewL(
        const TStreamConObjectType aObjectType,
        CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolAudioStreamFormat* NewL(
        const TStreamConObjectType aObjectType,
        const TUint8 aUnitId,
        const TUid aAudioFormat,
        const TStreamType aStreamType,
        const TBitResolution aBitResolution,
        const TUint32 aSamFreq );
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolAudioStreamFormat* NewLC(
        const TStreamConObjectType aObjectType,
        CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolAudioStreamFormat* NewLC(
        const TStreamConObjectType aObjectType,
        const TUint8 aUnitId,
        const TUid aAudioFormat,
        const TStreamType aStreamType,
        const TBitResolution aBitResolution,
        const TUint32 aSamFreq );

    /**
     * Assign operator for audio stream format class.
     *
     * @param CAccPolAudioStreamFormat, Const reference to object to be copied.
     * @return CAccPolAudioStreamFormat, Reference to object pointer.
     */
    IMPORT_C CAccPolAudioStreamFormat& operator= (
        const CAccPolAudioStreamFormat& );

    /**
     * Sets the unit id.
     *
     * @param aUnitId, Unit id to be stored.
     * In USB case input or output terminal unit id where the audio stream format
     * parameters are associated.
     * @return void.
     */
    IMPORT_C void SetUnitId( const TUint8 aUnitId );

    /**
     * Sets the audio format.
     *
     * @param aAudioFormat, Audio format to be stored.
     * Possible TUid's are defined in AccPolAudioStreamFormat.h.
     * @return void.
     */
    IMPORT_C void SetAudioFormat( const TUid aAudioFormat );

    /**
     * Sets the audio data stream type.
     *
     * @param aStreamType, Stream type to be stored.
     * Possible channel types are defined in AccPolAudioStreamFormat.h.
     * @return void.
     */
    IMPORT_C void SetStreamType( const TStreamType aStreamType );

    /**
     * Sets the bit resolution.
     *
     * @param aBitResolution, Bit resolution to be stored.
     * Possible bit resolution values are defined in AccPolAudioStreamFormat.h.
     * @return void.
     */
    IMPORT_C void SetBitResolution( const TBitResolution aBitResolution );

    /**
     * Sets the sampling frequency.
     *
     * @param aSamFreq, Sampling frequency to be stored (Hz).
     * @return void.
     */
    IMPORT_C void SetSamFreq( const TUint32 aSamFreq );

    /**
     * Returns the unit id.
     *
     * @return TUint8, Value of unit id.
     */
    IMPORT_C TUint8 UnitId() const;

    /**
     * Returns the audio format.
     *
     * @return TUid, Value of audio format.
     */
    IMPORT_C const TUid AudioFormat() const;

    /**
     * Returns the audio stream type.
     *
     * @return TStreamType, Value of stream type.
     */
    IMPORT_C TStreamType StreamType() const;

    /**
     * Returns the bit resolution.
     *
     * @return TBitResolution, Value of bit resolution.
     */
    IMPORT_C TBitResolution BitResolution() const;

    /**
     * Returns the discrete sampling frequency.
     *
     * @return TUint32, Value of sampling frequency.
     */
    IMPORT_C TUint32 SamFreq() const;

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

protected:

    /**
     * C++ default constructor.
     */
    CAccPolAudioStreamFormat( const TStreamConObjectType aObjectType,
        CAccPolObjectCon* aObject = NULL );

    /**
     * C++ default constructor.
     */
    CAccPolAudioStreamFormat( const TStreamConObjectType aObjectType,
        const TUint8 aUnitId,
        const TUid aAudioFormat,
        const TStreamType aStreamType,
        const TBitResolution aBitResolution,
        const TUint32 aSamFreq );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /**
     * Unit id.
     */
    TUint8 iUnitId;

    /**
     * Audio format.
     */
    TUid iAudioFormat;

    /**
     * Channel type.
     */
    TStreamType iStreamType;

    /**
     * Bit resolution.
     */
    TBitResolution iBitResolution;

    /**
     * Sampling frequency / Hz.
     */
    TUint32 iSamFreq;
    };

#endif // ACCPOLAUDIOSTREAMFORMAT_H
