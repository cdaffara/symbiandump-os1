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
* Declaration of CCea861EdidParser class.
*
*/

#ifndef C_CCEA861EDIDPARSER_H
#define C_CCEA861EDIDPARSER_H

#include <e32base.h>

#include "ediddatatypes.h"
#include "cea861ediddatatypes.h"


/**
 *  CCea861EdidParser is the base class for parsing CEA 861 EDID extension data
 *
 *  @lib    edidparser.dll
 *  @since  S60 v9.2
 */
class CCea861EdidParser : public CBase
    {

public:

    /**
     * Two-phased constructor.
     *
     * Creates an instance of CCea861EdidParser
     *
     * @param   aData
     * @return  CCea861EdidParser* created instance of CCea861EdidParser
     *
     */
    IMPORT_C static CCea861EdidParser* NewL( const TExtDataBlock& aData );

    /**
     * Two-phased constructor.
     *
     * Creates an instance of CEdidParserBase
     *
     * @param   aData
     * @return  CCea861EdidParser* created instance of CCea861EdidParser
     *
     */
    IMPORT_C static CCea861EdidParser* NewLC( const TExtDataBlock& aData );

    /**
     * Destructor.
     */
    virtual ~CCea861EdidParser();

    /**
     * Parses extension block
     *
     * @param   aData Extension data block.
     * @return  TInt
     */
    IMPORT_C TInt ParseExtensionBlockL( const TExtDataBlock& aData );

    /**
     * Indicates whether or not extension data is parsed
     *
     * @param   none
     * @return  TBool
     */
    IMPORT_C TBool IsExtensionBlockParsed();

    /**
     * Gets VIC (Video Identification Code) from given video data block
     *
     * @param   aNumber
     * @return  VIC. Returns -1 if aNumber > number of VICs
     */
    IMPORT_C TInt8 GetVIC( TUint8 aNumber );

    /**
     * Indicates whether or not underscan is supported
     *
     * @param   none
     * @return  true/false
     */
    IMPORT_C TBool Underscan();

    /**
     * Indicates whether or not basic audio is supported
     *
     * @param   none
     * @return  true/false
     */
    IMPORT_C TBool BasicAudio();

    /**
     * Indicates whether or not YCbCr 4:4:4 color space is supported
     *
     * @param   none
     * @return  true/false
     */
    IMPORT_C TBool YCbCr444();

    /**
     * Indicates whether or not YCbCr 4:2:2 color space is supported
     *
     * @param   none
     * @return  true/false
     */
    IMPORT_C TBool YCbCr422();

    /**
     * Returns number of native formats.
     * Section 2.2 in CEA-861-E.pdf describes native format as below:
     * "A video format with native pixel layout and scanning method that the display device
     * accepts and displays without any internal scaling, de-interlacing, interlacing or frame
     * rate conversion"
     *
     * @param   none
     * @return  number of native formats
     */
    IMPORT_C TUint8 TotalNumberOfNativeFormats();

    /**
     * Indicates whether or not Audio Data Block is supported
     *
     * @param   none
     * @return  true/false
     */
    IMPORT_C TBool IsAudioDataBlockSupported();
    
    /**
     * Indicates whether or not Video Data Block is supported
     *
     * @param   none
     * @return  true/false
     */
    IMPORT_C TBool IsVideoDataBlockSupported();    
    
    /**
     * Indicates whether or not Vendor Specific Data Block is supported
     *
     * @param   none
     * @return  true/false
     */
    IMPORT_C TBool IsVendorSpecificDataBlockSupported();        
    
    /**
     * Indicates whether the sink has IEEE registration or not in the vendor specific data block
     *
     * @param   none
     * @return  true/false
     */
    IMPORT_C TBool HasIEEERegistration();        
    
    /**
     * Indicates whether or not Speaker Allocation Data Block is supported
     *
     *
     * @param   none
     * @return  true/false
     */
    IMPORT_C TBool IsSpeakerAllocationDataBlockSupported();

    /**
     * Returns parsed information.
     * Ownership of the pointer is not transfered.
     *
     * @param   none
     * @return  CCea861ExtEdidInformation*
     */
    IMPORT_C CCea861ExtEdidInformation* GetParsedInformation();
    
    /**
     * Returns video latency from Vendor specific data block.
     * Value is number of (milliseconds / 2) + 1 with a maximum allowed value of 251 (indicating
     * 500 millisecond duration). A value of 0 indicates that the field is not valid
     * or that the latency is unknown. A value of 255 indicates that no video is
     * supported in this device or downstream.
     *
     * @param   aVendorDataBlock
     * @return  video latency in ms
     */
    IMPORT_C TUint8 GetVideoLatency();
    
    /**
     * Returns audio latency from Vendor specific data block.
     * Value is number of (milliseconds / 2) + 1 with a maximum allowed value of 251 (indicating
     * 500 millisecond duration). A value of 0 indicates that the field is not valid
     * or that the latency is unknown. A value of 255 indicates that no audio is
     * supported in this device or downstream.
     * 
     * @param   none
     * @return  audio latency in ms
     */
    IMPORT_C TUint8 GetAudioLatency();

    /**
     * Returns interlaced video latency from Vendor specific data block.
     * Value is number of (milliseconds / 2) + 1 with a maximum allowed value of 251 (indicating
     * 500 millisecond duration). A value of 0 indicates that the field is not valid
     * or that the latency is unknown. A value of 255 indicates that no video is
     * supported in this device or downstream.
     *
     * @param   aVendorDataBlock
     * @return  video latency in ms
     */
    IMPORT_C TUint8 GetInterlacedVideoLatency();
        
    /**
     * Returns interlaced audio latency from Vendor specific data block.
     * Value is number of (milliseconds / 2) + 1 with a maximum allowed value of 251 (indicating
     * 500 millisecond duration). A value of 0 indicates that the field is not valid
     * or that the latency is unknown. A value of 255 indicates that no audio is
     * supported in this device or downstream.
     *
     * @param   none
     * @return  audio latency in ms
     */
    IMPORT_C TUint8 GetInterlacedAudioLatency();

private:

    /**
     * Default constructor.
     */
    CCea861EdidParser();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const TExtDataBlock& aData );

    /**
     * Reads CEA 861 extension v1
     * See table 36 in CEA-861-E.pdf
     *
     * @param   aData
     * @return  TBool
     */
    TInt ReadCeaVersion1L( const TExtDataBlock& aData, TInt aIndex );

    /**
     * Reads CEA 861 extension v2
     * See table 37 in CEA-861-E.pdf
     *
     * @param   aData
     * @return  TBool
     */
    TInt ReadCeaVersion2L( const TExtDataBlock& aData, TInt aIndex );

    /**
     * Reads CEA 861 extension v2
     * See table 38 in CEA-861-E.pdf
     *
     * @param   aData
     * @return  TBool
     */
    TInt ReadCeaVersion3L( const TExtDataBlock& aData, TInt aIndex );

    /**
     * Reads common parts of CEA 861 extension v2 and v3
     * See table 38 in CEA-861-E.pdf
     *
     * @param   aData
     * @return  TBool
     */
    TInt ReadCeaVersion2and3Common( const TExtDataBlock& aData, TInt& aIndex );

    /**
     * Gets EDID descriptor block
     *
     * @param   aData
     * @param   aIndex
     * @return  TEdidDescriptorBlock
     */
    TEdidDescriptorBlock GetDescriptorBlock( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets pixel clock from descriptor block
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint16 GetPixelClock( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Horizontal Addressable Video in pixels
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint16 GetHorizontalAddressableVideoPixels( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Horizontal Blanking in pixels
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint16 GetHorizontalBlanking( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Vertical Addressable Video in lines
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint16 GetVerticalAddressableVideoPixels( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Vertical Blanking in lines
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint16 GetVerticalBlanking( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Horizontal Front Porch in pixels
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint16 GetHorizontalFrontPorch( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Horizontal Sync Pulse Width in lines
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint16 GetHorizontalSyncPulseWidth( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Vertical Front Porch in lines
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint16 GetVerticalFrontPorch( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Vertical Sync Pulse Width in lines
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint8 GetVerticalSyncPulseWidth( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Horizontal Addressable Video Image Size in mm
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint16 GetHorizontalAddressableImageSizeInMm( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Vertical Addressable Video Image Size in mm
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint16
     */
    TUint16 GetVerticalAddressableImageSizeInMm( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Right Horizontal Border or Left Horizontal Border in Pixels
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint8
     */
    TUint8 GetHorizontalBorderSize( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets Top Vertical Border or Bottom Vertical Border in Lines
     *
     * @param   aData
     * @param   aIndex
     * @return  TUint8
     */
    TUint8 GetVerticalBorderSize(const TExtDataBlock& aData, TInt& aIndex) const;

    /**
     * Checks whether or not video is interlaced
     *
     * @param   aData
     * @param   aIndex
     * @return  TBool
     */
    TBool GetVideoIsInterlaced( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets stero support
     *
     * @param   aData
     * @param   aIndex
     * @return  TEdidStereoViewingSupport
     */
    TEdidStereoViewingSupport GetStereoViewingSupport( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets signal definitions
     *
     * @param   aData
     * @param   aIndex
     * @return  TEdidSyncSignalDefinitions
     */
    TEdidSyncSignalDefinitions GetSyncSignalDefinitions( const TExtDataBlock& aData, TInt& aIndex ) const;

    /**
     * Gets analog sync signal definitions
     *
     * @param   aByte17
     * @return  TEdidSyncSignalDefinitions
     */
    static TEdidSyncSignalDefinitions GetAnalogSyncSignalDefinitions( const TUint8 aByte17 );

    /**
     * Gets digital sync signal definitions
     *
     * @param   aByte17
     * @return  TEdidSyncSignalDefinitions
     */
    static TEdidSyncSignalDefinitions GetDigitalSyncSignalDefinitions( const TUint8 aByte17 );


    /**
     * CEA data block
     **/

    /**
     * Reads CEA data block collection
     *
     * @param   aData
     * @param   aIndex
     * @return  TInt
     */
    TInt ReadCeaDataBlockCollectionL( const TExtDataBlock& aData, TInt& aIndex );

    /**
     * Reads CEA 861 short audio data block
     *
     * @param   aData
     * @param   aIndex
     * @param   aLen
     * @return  none
     */
    void ReadCea861ShortAudioDataBlockL( const TExtDataBlock& aData, TInt& aIndex, const TInt8 aLen );

    /**
     * Reads CEA 861 short video data block
     *
     * @param   aData
     * @param   aIndex
     * @param   aLen
     *
     * @return  none
     */
    void ReadCea861ShortVideoDataBlockL( const TExtDataBlock& aData, TInt& aIndex, const TInt8 aLen );

    /**
     * Reads CEA 861 vendor data block
     *
     * @param   aData
     * @param   aIndex
     * @param   aLen
     * @return  none
     */
    void ReadCea861VendorSpecificDataBlockL( const TExtDataBlock& aData, TInt& aIndex, TInt8 aLen );

    /**
     * Reads CEA 861 speaker allocation data block
     *
     * @param   aData
     * @param   aIndex
     * @param   aLen
     * @return  none
     */
    void ReadCea861SpeakerAllocationDataBlock( const TExtDataBlock& aData, TInt& aIndex, const TInt8 aLen );


    /**
     * Reads video capability block
     *
     * @param   aData
     * @param   aIndex
     * @param   aLen
     * @return  none
     */
    void ReadVideoCapabilityDataBlockL( const TExtDataBlock& aData, TInt& aIndex, const TInt8 aLen );

    /**
     * Reads unknwown data block to skip it
     *
     * @param   aData
     * @param   aIndex
     * @param   aLen
     * @return  none
     */
    void ReadUnknownTagCode( const TExtDataBlock& aData, TInt& aIndex, const TInt8 aLen );

    /**
     * Determines audio block information
     *
     * @param   aAudioBlock
     * @return  none
     */
    void CCea861EdidParser::DetermineAudioBlockInformation( TCEA861AudioDataBlock* aAudioBlock );
    
    /**
     * Check if Latency_Fields_Preset bit is set as specified in HDMISpecification13a.pdf.
     * 
     * @param aByte Byte to check.
     * @return ETrue if Latency_Fields_Present bit is set.
     */
    TBool LatencyFieldsPresent( TUint8 aByte ) const;

    /**
     * Check if I_Latency_Fields_Preset bit is set as specified in HDMISpecification13a.pdf.
     * 
     * @param aByte Byte to check.
     * @return ETrue if I_Latency_Fields_Present bit is set.
     */
    TBool InterlacedLatencyFieldsPresent( TUint8 aByte ) const;
    
    /**
     * Calculate the video/audio latency in mm seconds.
     * 
     * @param aByte Byte containing the latency information.
     * @return Latency information in ms.
     */
    TUint8 LatencyInMs( TUint8 aByte ) const;

private: // data

     TBool                       iParsed;           // Indicates whether or not extension block is parsed
     TExtDataBlock               iExtensionData;    // Extension data
     CCea861ExtEdidInformation*  iParsedInfo;       // Contains Parsed information
     TBool                       iAudioDataBlockSupported;
     TBool                       iVideoDataBlockSupported;
     TBool                       iVendorSpecificDataBlockSupported;
     TBool                       iSpeakerAllocationDataBlockSupported;
     
    };


#endif // C_CCEA861EDIDPARSER_H
