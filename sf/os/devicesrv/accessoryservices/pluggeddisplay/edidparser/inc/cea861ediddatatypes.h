/*
* Copyright (c) 2008,2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of EDID data types for CEA 861 extension.
*
*/

#ifndef CEA861EDIDDATATYPES_H
#define CEA861EDIDDATATYPES_H

#include <e32base.h>

#include "ediddatatypes.h"

/**
 * Video data block
 * See table 44 CEA-861-E.pdf
 *
 * @lib    edidparser.dll
 * @since  S60 v9.2
 */
class TCEA861VideoDataBlock
    {
public:

    /**
    * Default C++ constructor
    *
    */
    inline TCEA861VideoDataBlock():
        iNext( NULL ),
        iNative( EFalse ),
        iVIC( 0 )
            {}

public:

    TCEA861VideoDataBlock* iNext;
    TBool iNative;
    TUint8 iVIC;
    };

/** Audio format code bit mask */
const TUint KAudioFormatCodeNA     = 0x00; // 0000
const TUint KAudioFormatCodePCM    = 0x01; // 0001
const TUint KAudioFormatCodeAC3    = 0x02; // 0010
const TUint KAudioFormatCodeMPEG1  = 0x03; // 0011
const TUint KAudioFormatCodeMP3    = 0x04; // 0100
const TUint KAudioFormatCodeMPEG2  = 0x05; // 0101
const TUint KAudioFormatCodeAACLC  = 0x06; // 0110
const TUint KAudioFormatCodeDTS    = 0x07; // 0111
const TUint KAudioFormatCodeATRAC  = 0x08; // 1000
const TUint KAudioFormatCodeDSD    = 0x09; // 1001
const TUint KAudioFormatCodeEAC3   = 0x0A; // 1010
const TUint KAudioFormatCodeDTSHD  = 0x0B; // 1011
const TUint KAudioFormatCodeMLP    = 0x0C; // 1100
const TUint KAudioFormatCodeDST    = 0x0D; // 1101
const TUint KAudioFormatCodeWMAPRO = 0x0E; // 1110
const TUint KAudioFormatCodeExt    = 0x0F; // 1111

/**
 * Audio data block
 * There can be up to 10 short audio descriptors in the audio data block.
 * Each audio descriptor consists of 3 bytes. See page 66 in CEA-861-E.pdf
 *
 * @lib    edidparser.dll
 * @since  S60 v9.2
 *
 */
class TCEA861AudioDataBlock
    {
public:
    /**
     * Default C++ constructor
     *
     */
    inline TCEA861AudioDataBlock():
        iNext( NULL ),
        iAudioFormatCode( 0 ),
        iSupport192kHz( EFalse ),
        iSupport176kHz( EFalse ),
        iSupport96kHz( EFalse ),
        iSupport88kHz( EFalse ),
        iSupport48kHz( EFalse ),
        iSupport44kHz( EFalse ),
        iSupport32kHz( EFalse ),
        iMaxChannels( 0 ),
        iByte1( 0 ),
        iByte2( 0 ),
        iByte3( 0 ),
        iSupport24Bit( EFalse ),
        iSupport20Bit( EFalse ),
        iSupport16Bit( EFalse ),
        iMaxBitrate( 0 ),
        iAudioFormatCodeExtension( 0 )
            {}

    /**
     * BitDataSupported
     *
     * @since   S60 v9.2
     * @param   none
     * @return  ETrue if Support24Bit, Support20Bit and Support16Bit booleans are used.
     */
    inline TBool BitDataSupported() const
        {
        return iAudioFormatCode == 1;
        }

    /**
     * MaximumBitRateValueSupported
     *
     * @since   S60 v9.2
     * @param   none
     * @return  ETrue if maximum bit rate is supported
     */
    inline TBool MaximumBitRateValueSupported() const
        {
        return ( iAudioFormatCode > 1 ) && ( iAudioFormatCode <= 8 );
        }

    /**
     * AudioFormatCodeExtensionSupported
     *
     * @since   S60 v9.2
     * @param   none
     * @return  ETrue if AudioFormatCode has Extension
     */
    inline TBool AudioFormatCodeExtensionSupported() const
        {
        return iAudioFormatCode == 15;
        }

public:

    TCEA861AudioDataBlock* iNext;
    TUint8  iAudioFormatCode; // 4 bits used
    TBool   iSupport192kHz;
    TBool   iSupport176kHz;
    TBool   iSupport96kHz;
    TBool   iSupport88kHz;
    TBool   iSupport48kHz;
    TBool   iSupport44kHz;
    TBool   iSupport32kHz;
    TUint8  iMaxChannels;

    /**
     * Bytes 1,2 and 3 are for private use
     *
     */
    TUint8  iByte1;
    TUint8  iByte2;
    TUint8  iByte3;

    /**
     * Used only if supported
     *
     */
    TBool   iSupport24Bit; // 24Bit audio supported
    TBool   iSupport20Bit; // 20Bit audio supported
    TBool   iSupport16Bit; // 16Bit audio supported
    TUint16 iMaxBitrate;   // maximum bitrate
    TUint8  iAudioFormatCodeExtension; // the 8-bit extension for Audio Format Code
    };


/**
 * Speaker Allocation Data block
 * See table 50 and pages 66-67 in CEA-861-E.pdf
 *
 * @lib    edidparser.dll
 * @since  S60 v9.2
 *
 */
class TCEA861SpeakerAllocationData
    {
public:

    /**
     * Default C++ constructor
     *
     */
    inline TCEA861SpeakerAllocationData():
        iByte1( 0 ),
        iByte2( 0 ),
        iByte3( 0 )
            {}

    /**
     * FLW_FRW - Front Left Wide / Front Right Wide
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Front Left Wide / Front Right Wide
     */
    inline TBool FLW_FRW() const
        {
        return ( KBit7 & iByte1 ? ETrue : EFalse );
        }

    /**
     * RLC_RRC - Rear Left Center / Rear Right Center
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Rear Left Center / Rear Right Center
     */
    inline TBool RLC_RRC() const
        {
        return ( KBit6 & iByte1 ? ETrue : EFalse );
        }

    /**
     * FLC_FRC - Front Left Center / Front Right Center
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Front Left Center / Front Right Center
     */
    inline TBool FLC_FRC() const
        {
        return ( KBit5 & iByte1 ? ETrue : EFalse );
        }

    /**
     * RC - Rear Center
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Rear Center
     */
    inline TBool RC() const
        {
        return ( KBit4 & iByte1 ? ETrue : EFalse );
        }

    /**
     * RL_RR - Rear Left / Rear Right
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Rear Left / Rear Right
     */
    inline TBool RL_RR() const
        {
        return ( KBit3 & iByte1 ? ETrue : EFalse );
        }

    /**
     * FC - Front Center
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Front Center
     */
    inline TBool FC() const
        {
        return ( KBit2 & iByte1 ? ETrue : EFalse );
        }

    /**
     * LFE - Low Frequency Effect
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Low Frequency Effect
     */
    inline TBool LFE() const
        {
        return ( KBit1 & iByte1 ? ETrue : EFalse );
        }

    /**
     * FL_FR - Front Left / Front Right
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Front Left / Front Right
     */
    inline TBool FL_FR() const
        {
        return ( KBit0 & iByte1 ? ETrue : EFalse );
        }

    //
    /**
     * FCH - Front Center High
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Front Center High
     */
    inline TBool FCH() const
        {
        return ( KBit2 & iByte2 ? ETrue : EFalse );
        }

    /**
     * TC - Top Center
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Top Center
     */
    inline TBool TC() const
        {
        return ( KBit1 & iByte2 ? ETrue : EFalse );
        }

    /**
     * FLH_FRH - Front left High / Front Right High
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TBool Front Left High / Front Right High
     */
    inline TBool FLH_FRH() const
        {
        return ( KBit0 & iByte2 ? ETrue : EFalse );
        }

public:

    TUint8 iByte1; // holds the data for parsers internal purposes
    TUint8 iByte2;
    TUint8 iByte3;
    };

/**
 * Vendor Specific Data Block payload
 *
 * @lib    edidparser.dll
 * @since  S60 v9.2
 *
 */
class TCEA861VendorSpecificDataBlockPayload
    {
public:

    /**
     * Default C++ constructor
     *
     */
    inline TCEA861VendorSpecificDataBlockPayload():
        iNext( NULL ),
        iData( 0 )
            {}

public:

    TCEA861VendorSpecificDataBlockPayload* iNext;
    TUint8 iData;
    };


/**
 * Vendor Specific Data block. See table 39 in CEA-861-E.pdf
 *
 * @lib    edidparser.dll
 * @since  S60 v9.2
 *
 */
class TCEA861VendorSpecificDataBlock
    {
public:

    /**
     * Default C++ constructor
     *
     */
    inline TCEA861VendorSpecificDataBlock():
        iIEEE24bitRegistrationIdentifier( 0 ),
        iVendorSpecificPayloadStart( NULL )
            {}

public:

    TUint32 iIEEE24bitRegistrationIdentifier; //24-bits (converted from LSB to MSB)
    TCEA861VendorSpecificDataBlockPayload* iVendorSpecificPayloadStart;
    };


/**
 * A linked list that holds all descriptor blocks
 *
 * @lib    edidparser.dll
 * @since  S60 v9.2
 *
 */
class TCEA861TEdidDescriptorBlockList
    {
public:

    /**
     * Default C++ constructor
     *
     */
    inline TCEA861TEdidDescriptorBlockList():
        iNext( NULL )
            {
            Mem::FillZ( ( TAny* )&iData, sizeof( iData ) );
            }

public:

    TEdidDescriptorBlock iData;
    TCEA861TEdidDescriptorBlockList* iNext;
    };

/**
 * Video Capability Data block.
 *
 * @lib    edidparser.dll
 * @since  S60 v9.2
 *
 */
class TCEA861VideoCapabilityDataBlock
    {
public:

    /**
     * Default C++ constructor
     *
     */
    inline TCEA861VideoCapabilityDataBlock():
        iQuantizationRange( EFalse ),
        iQuantizationRangeSelectable( EFalse ),
        iS_PT1( EFalse ),
        iS_PT0( EFalse ),
        iS_IT1( EFalse ),
        iS_IT0( EFalse ),
        iS_CE1( EFalse ),
        iS_CE0( EFalse ),
        iInitialized( EFalse ),
        iNext( NULL )
            {}

    /**
     * IsInitialized
     * Is video capability block initialized
     *
     * @since   S60 v9.2
     * @param   none
     * @return  ETrue if underscan data is suppored
     */
    inline TBool IsInitialized() const
        {
        return iInitialized;
        }

    /**
     * PTOverscanUnderscanBehaviour
     *
     * @since   S60 v9.2
     * @param   none
     * @return  Tint 0 if No Data, 1 if always overscanned, 2 if always underscanned, 3 if supports both over- and underscan
     */
    inline TUint8 PTOverscanUnderscanBehaviour() const
        {
        TInt8 retval = 0;
        if ( iS_PT1 )
            {
            retval += 2;
            }
        if ( iS_PT0 )
            {
            retval += 1;
            }
        return retval;
        };

    /**
     * ITOverscanUnderscanBehaviour
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TInt 0 if IT video formats not supported, 1 if always overscanned, 2 if always underscanned, 3 if supports both over- and underscan
     */
    inline TUint8 ITOverscanUnderscanBehaviour() const
        {
        TInt8 retval = 0;
        if ( iS_IT1 )
            {
            retval += 2;
            }
        if ( iS_IT0 )
            {
            retval += 1;
            }
        return retval;
        };

    /**
     * CEOverscanUnderscanBehaviour
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TInt 0 if CE video formats not supported, 1 if always overscanned, 2 if always underscanned, 3 ifsupports both over- and underscan
     */
    inline TUint8 CEOverscanUnderscanBehaviour() const
        {
        TInt8 retval = 0;
        if ( iS_CE1 )
            {
            retval += 2;
            }
        if ( iS_CE0 )
            {
            retval += 1;
            }
        return retval;
        };

public:

    TBool iQuantizationRange; //applies to YCC only
    TBool iQuantizationRangeSelectable; //applies to RGB only
    TBool iS_PT1;
    TBool iS_PT0;
    TBool iS_IT1;
    TBool iS_IT0;
    TBool iS_CE1;
    TBool iS_CE0;
    TBool iInitialized;
    TCEA861VideoCapabilityDataBlock* iNext; // to form a linked list
    };

/**
 * CCea861ExtEdidInformation
 *
 * @lib    edidparser.dll
 * @since  S60 v9.2
 *
 */
NONSHARABLE_CLASS( CCea861ExtEdidInformation ): public CBase
    {
public:

    /**
     * Two-phased constructor.
     * Creates an instance of CCea861ExtEdidInformation
     *
     * @return  CCea861ExtEdidInformation* created instance of CCea861ExtEdidInformation
     */
    IMPORT_C static CCea861ExtEdidInformation* NewL();

    /**
     * Two-phased constructor.
     * Creates an instance of CCea861ExtEdidInformation
     *
     * @return  CCea861ExtEdidInformation* created instance of CCea861ExtEdidInformation
     */
    IMPORT_C static CCea861ExtEdidInformation* NewLC();

    /**
     * Destructor.
     */
    ~CCea861ExtEdidInformation();

    /**
     * UnderscanDataSupported
     *
     * @since   S60 v9.2
     * @param   none
     * @return  ETrue if underscan data is suppored
     */
    inline TBool UnderscanDataSupported() const
        {
        return iRevision > 1;
        }

    /**
     * AudioDataSupported
     *
     * @since   S60 v9.2
     * @param   none
     * @return  ETrue if audio data is supported
     */
    inline TBool AudioDataSupported() const
        {
        return iRevision > 1;
        }

    /**
     * YCbCr444DataSupported
     *
     * @since   S60 v9.2
     * @param   none
     * @return  ETrue if YCbCr444 data is supported
     */
    inline TBool YCbCr444DataSupported() const
        {
        return iRevision > 1;
        }

    /**
     * YCbCr422DataSupported
     *
     * @since   S60 v9.2
     * @param   none
     * @return  ETrue if YCbCr422 data is supported
     */
    inline TBool YCbCr422DataSupported() const
        {
        return iRevision > 1;
        }

    /**
     * TotalNumberOfNativeFormats
     *
     * @since   S60 v9.2
     * @param   none
     * @return  TInt total number of native formats
     */
    inline TUint8 TotalNumberOfNativeFormats() const
        {
        return iTotalNumberOfNativeFormats;
        }

private:

    /**
     * Default constructor
     *
     */
    CCea861ExtEdidInformation();

    /**
     * By default Symbian 2nd phase constructor is private
     *
     */
    void ConstructL();

public:

    /**
     * Extension tag
     *
     */
    TUint8 iExtensionTag;

    /**
     * Revision
     *
     */
    TUint8 iRevision;

    /**
     * Byte number offset d where 18-byte descriptors begin (d=offset for the byte following
     * the reserved data block. If no data is provided in the reserved data block then d=4.
     * if no DTDs are provided, then d=0
     */
    TUint8 iOffset;

    /**
     * Checksum
     *
     */
    TUint iChecksum;

    /**
     * True if sink underscans IT video formats by default
     *
     */
    TBool iUnderscan;

    /**
     * True if sink supports basic audio
     *
     */
    TBool iAudio;

    /**
     * True if sink supports YCbCr 4:4:4 in addition to RGB
     *
     */
    TBool iYCbCr444;

    /**
     * True if sink supports YCbCr 4:2:2 in addition to RGB
     *
     */
    TBool iYCbCr422;

    /**
     * Total number of DTDs (see section 2.2 for definition on native format)
     *
     */
    TUint8 iTotalNumberOfNativeFormats;

    /**
     * Linked lists that hold the info for different Revision 3 data blocks
     *
     */
    TCEA861VideoDataBlock*           iShortVideoDescriptors;    // use only if supported
    TCEA861AudioDataBlock*           iShortAudioDescriptors;    // use only if supported
    TCEA861SpeakerAllocationData     iSpeakerAllocationData;    // use only if supported
    TCEA861VendorSpecificDataBlock*  iVendorSpecificData;       // use only if supported
    TCEA861VideoCapabilityDataBlock* iVideoCapabilityDataBlock; // use only if supported
    TCEA861TEdidDescriptorBlockList* iDescriptorBlocks;         // use only if supported
    };

#endif // CEA861EDIDDATATYPES_H
