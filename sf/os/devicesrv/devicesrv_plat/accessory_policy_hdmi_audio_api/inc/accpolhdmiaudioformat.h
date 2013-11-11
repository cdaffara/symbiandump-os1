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
 * Description:  HDMI audio format object definition
 *
 */

#ifndef ACCPOLHDMIAUDIOFORMAT_H
#define ACCPOLHDMIAUDIOFORMAT_H

#include <e32cmn.h>
#include <accpolobjectbase.h>
#include <accpolobjecttypes.h>

/**
 * Audio coding types (page 49 table 24):
 */
namespace HdmiAudioFormat
    {
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

    /** AC-3 */
    const TUid KUidFormatAC3 =
        {
        0x000000C
        };

    /** MPEG-1 (layer 1 or 2) */
    const TUid KUidFormatMPEG1 =
        {
        0x000000D
        };

    /** MPEG2 */
    const TUid KUidFormatMPEG2 =
        {
        0x000000E
        };

    /** AAC LC */
    const TUid KUidFormatAACLC =
        {
        0x000000F
        };

    /** DTS */
    const TUid KUidFormatDTS =
        {
        0x0000010
        };

    /** ATRAC */
    const TUid KUidFormatATRAC =
        {
        0x0000011
        };

    /** DSD */
    const TUid KUidFormatDSD =
        {
        0x0000012
        };

    /** E-AC-3 */
    const TUid KUidFormatEAC3 =
        {
        0x0000013
        };

    /** DTS-HD */
    const TUid KUidFormatDTSHD =
        {
        0x0000014
        };

    /** MLP */
    const TUid KUidFormatMLP =
        {
        0x0000015
        };

    /** DST */
    const TUid KUidFormatDST =
        {
        0x0000016
        };

    /** WMAPRO */
    const TUid KUidFormatWMAPRO =
        {
        0x0000017
        };

    /** HE-AAC */
    const TUid KUidFormatHEAAC =
        {
        0x0000018
        };

    /** HE-AACV2 */
    const TUid KUidFormatHEAACV2 =
        {
        0x0000019
        };

    /** MPEG2SURROUND */
    const TUid KUidFormatMPEG2SURROUND =
        {
        0x000001A
        };
    }

/**
 *  Store's an audio stream format parameters.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS( CAccPolHdmiAudioFormat ) : public CAccPolObjectBase
    {

public:
    
    /**
     * Bit resolution.
     */
    enum TBitResolution
        {
        EUnknownBitsPerSample = 0x00,
        EBitsPerSample16 = 0x01,
        EBitsPerSample20 = 0x02,
        EBitsPerSample24 = 0x04
        };

    /**
     * Sampling Frequency.
     */
    enum TSamplingFrequency
        {
        EUnknownFrequency = 0x00,
        ESamplingFreq32KHz = 0x01,
        ESamplingFreq44KHz = 0x02,
        ESamplingFreq48KHz = 0x04,
        ESamplingFreq88KHz = 0x08,
        ESamplingFreq96KHz = 0x10,
        ESamplingFreq176KHz = 0x20,
        ESamplingFreq192KHz = 0x40
        };

    /**
     * Destructor.
     */
    virtual ~CAccPolHdmiAudioFormat();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiAudioFormat* NewL(
        CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiAudioFormat* NewL(
        const TUid aAudioFormat,
        const TUint32 aMaxChannelCount,
        const TUint32 aBitResolution,
        const TUint32 aMaxBitResolution,
        const TUint32 aSamFreq,
        const TUint32 aFormatDependentValue );
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiAudioFormat* NewLC(
        CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiAudioFormat* NewLC(
        const TUid aAudioFormat,
        const TUint32 aMaxChannelCount,
        const TUint32 aBitResolution,
        const TUint32 aMaxBitResolution,
        const TUint32 aSamFreq,
        const TUint32 aFormatDependentValue );

    /**
     * Assign operator for audio stream format class.
     *
     * @param CAccPolHdmiAudioFormat, Const reference to object to be copied.
     * @return CAccPolHdmiAudioFormat, Reference to object pointer.
     */
    IMPORT_C CAccPolHdmiAudioFormat& operator= ( const CAccPolHdmiAudioFormat& );

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
     * @param aMaxChannelCount, Stream type to be stored.
     * Possible channel types are defined in AccPolAudioStreamFormat.h.
     * @return void.
     */
    IMPORT_C void SetChannelCount( const TUint32 aMaxChannelCount );

    /**
     * Sets the bit resolution.
     *
     * @param aBitResolution, Bit resolution to be stored as a bitmask of TBitResolution.
     * Possible bit resolution values are defined in AccPolAudioStreamFormat.h.
     * @return void.
     */
    IMPORT_C void SetBitResolution( const TUint32 aBitResolution );

    /**
     * Sets the bit resolution.
     *
     * @param aBitResolution, Bit resolution to be stored as a bitmask of TBitResolution.
     * Possible bit resolution values are defined in AccPolAudioStreamFormat.h.
     * @return void.
     */
    IMPORT_C void SetMaxBitResolution( const TUint32 aMaxBitResolution );

    /**
     * Sets the bit resolution.
     *
     * @param aBitResolution, Bit resolution to be stored as a bitmask of TBitResolution.
     * Possible bit resolution values are defined in AccPolAudioStreamFormat.h.
     * @return void.
     */
    IMPORT_C void SetFormatDependentValue( const TUint32 aFormatDependentValue );

    /**
     * Sets the sampling frequency.
     *
     * @param aSamFreq, Sampling frequency bitmask of TSamplingFrequency to be stored.
     * @return void.
     */
    IMPORT_C void SetSamFreq( const TUint32 aSamFreq );

    /**
     * Returns the audio format.
     *
     * @return TUid, Value of audio format.
     */
    IMPORT_C const TUid AudioFormat() const;

    /**
     * Returns the audio stream type.
     *
     * @return TUint32, Value of stream type.
     */
    IMPORT_C TUint32 ChannelCount() const;

    /**
     * Returns the bit resolution.
     *
     * @return TBitResolution, Value of bit resolution as a bitmask of TBitResolution.
     */
    IMPORT_C TUint32 BitResolution() const;

    /**
     * Returns the bit resolution.
     * @param  aBitResolution, Bit resolution support of which is to be queried.
     * @return TBitResolution, Value of bit resolution as a bitmask of TBitResolution.
     */
    IMPORT_C TBool BitResolution( TBitResolution aBitResolution ) const;

    /**
     * Returns the bit resolution.
     *
     * @return TUint32, Value of bit resolution as a bitmask of TBitResolution.
     */
    IMPORT_C TUint32 MaxBitResolution() const;

    /**
     * Returns the discrete sampling frequency.
     *
     * @return TUint32, Value of sampling frequency  as a bitmask of TSamplingFrequency.
     */
    IMPORT_C TUint32 SamFreq() const;

    /**
     * Returns the discrete sampling frequency.
     * @param  aSamFreq, Sampling frequency support of which is to be queried.
     * @return TBool, True if sampling frequency supported
     */
    IMPORT_C TBool SamFreq( TSamplingFrequency aSamFreq ) const;

    /**
     * Returns the discrete sampling frequency.
     *
     * @return TUint32, Value of sampling frequency  as a bitmask of TSamplingFrequency.
     */
    IMPORT_C TUint32 FormatDependentValue() const;

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
    CAccPolHdmiAudioFormat( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ default constructor.
     */
    CAccPolHdmiAudioFormat( const TUid aAudioFormat,
        const TUint32 aMaxChannelCount,
        const TUint32 aBitResolution,
        const TUint32 aMaxBitResolution,
        const TUint32 aSamFreq,
        const TUint32 aFormatDependentValue );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /**
     * Audio format.
     */
    TUid iAudioFormat;

    /**
     * Channel type.
     */
    TUint32 iMaxChannelCount;

    /**
     * Bit resolution bitmap.
     */
    TUint32 iBitResolution;

    /**
     * Maximum Bit resolution.
     */
    TUint32 iMaxBitResolution;

    /**
     * Sampling frequency bitmap.
     */
    TUint32 iSamFreq;

    /**
     * Format dependent value.
     */
    TUint32 iFormateDependentValue;
    };

/** Format object array */
typedef RPointerArray<CAccPolHdmiAudioFormat> RAccPolHdmiAudioFormatArray;

#endif // ACCPOLHDMIAUDIOFORMAT_H
