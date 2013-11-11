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
* Declaration of CEdidParserBase class.
*
*/

#ifndef C_CEDIDPARSERBASE_H
#define C_CEDIDPARSERBASE_H

#include <e32base.h>

#include "ediddatatypes.h"

// FORWARD DECLERATION
class CCea861EdidParser;


/**
 *  CEdidParserBase is the base class for parsing EDID data
 *
 *  @lib    edidparser.dll
 *  @since  S60 v9.2
 */
NONSHARABLE_CLASS( CEdidParserBase ): public CBase
    {
public:

    /**
     * Two-phased constructor.
     * Creates an instance of CEdidParserBase
     *
     * @param   aData
     * @return  CEdidParserBase* created instance of CEdidParserBase
     */
    IMPORT_C static CEdidParserBase* NewL( const TDesC8& aBinaryData );

    /**
     * Two-phased constructor.
     * Creates an instance of CEdidParserBase
     *
     * @param   aData
     * @return  CEdidParserBase* created instance of CEdidParserBase
     */
    IMPORT_C static CEdidParserBase* NewLC( const TDesC8& aBinaryData );

    /**
     * Destructor.
     */
    virtual ~CEdidParserBase();


public:

    /**
     * Checks if data is valid EDID data
     *
     * @since   S60 9.2
     * @param   aBinaryData (EDID data block)
     * @return  TBool
     */
    IMPORT_C TBool IsEdidData( const TDesC8& aBinaryData );

    /**
     * Gets manufacturer code from EDID data.
     * Manufacturer PNPIDs are issued by Microsoft
     *
     * @since   S60 9.2
     * @param   aCode
     * @return  none
     */
    IMPORT_C void GetManufacturerCode( TDes& aCode );

    /**
     * Gets product id from EDID data.
     *
     * @since   S60 9.2
     * @param   aCode
     * @return  none
     */
    IMPORT_C void GetProductId( TDes& aCode );

    /**
     * Gets week of manufacture from EDID data.
     *
     * @since   S60 9.2
     * @param   none
     * @return  TUint16
     */
    IMPORT_C TUint8 GetManufactureWeek();

    /**
     * Gets year of manufacture from EDID data.
     * Year of manufacture stored in EDID data is "Year of Manufacture - 1990"
     *
     * @since   S60 9.2
     * @param   none
     * @return  TUint16
     */
    IMPORT_C TUint16 GetManufactureYear();

    /**
     * Gets EDID version from data block
     *
     * @since   S60 9.2
     * @param   none
     * @return  EDID version (TUint8)
     */
    IMPORT_C TUint8 GetVersion();

    /**
     * Gets EDID revision from data block
     *
     * @since   S60 9.2
     * @param   none
     * @return  EDID revision (TUint8)
     */
    IMPORT_C TUint8 GetRevision();

    /**
     * Gets Video Input Definition block
     * Bit #8 indicates if input is analog or digital
     *
     * @since   S60 9.2
     * @param   none
     * @return  Video Input Definition data
     */
    IMPORT_C TUint8 GetVideoInputDefinition();

    /**
     * Gets horizontal screen size
     * Range is between 1-255  cm
     *
     * @since   S60 9.2
     * @param   none
     * @return  Horizontal screen size in cm
     */
    IMPORT_C TUint8 GetHorizontalScreenSize();

    /**
     * Gets vertical screen size
     * Range is between 1-255  cm
     *
     * @since   S60 9.2
     * @param   none
     * @return  Vertical screen size in cm
     */
    IMPORT_C TUint8 GetVerticalScreenSize();


    /**
     * Gets aspect ratio
     *
     * @since   S60 9.2
     * @param   none
     * @return  TUint8 (aspect ratio landscape value)
     */
    IMPORT_C TUint8 GetAspectRatioLandscape();

    
    /**
     * Gets aspect ratio
     *
     * @since   S60 9.2
     * @param   none
     * @return  TUint8 (aspect ratio landscape value)
     */
    IMPORT_C TUint8 GetAspectRatioPortrait();    
    

    /**
     * Returns true/false depending on Aspect Ratio is used
     *
     * If byte 15h != 00h then byte 16h = Vertical Screen Size in cm.
     * (Range is 1 cm -> 255 cm)
     * If byte 15h == 00h then byte 16h = Aspect Ratio (Portrait)
     * (Range is 0.28 : 1 AR -> 0.99 : 1 AR)
     *
     * If byte 16h != 00h then byte 15h = Horizontal Screen Size in cm.
     * (Range is 1 cm -> 255 cm)
     * If byte 16h == 00h then byte 15h = Aspect Ratio (Landscape)
     * (Range is 1 : 1 AR -> 3.54 : 1 AR)
     *
     * @since   S60 9.2
     * @param   none
     * @return  ETrue (Aspect Ratio is used) EFalse (Aspect Ratio is not used)
     */
    IMPORT_C TBool IsAspectRatioUsed();

    /**
     * Gets Display Transfer Characteristics
     * Gamma value = ( dec( Display Transfer Characteristics ) + 100 )/100
     * Gamma value range is from 1.00 -> 3.54
     *
     * @since   S60 9.2
     * @param   none
     * @return  Vertical screen size in cm
     */
    IMPORT_C TUint8 DisplayTransferCharacteristics();

    /**
     * Gets red x chromaticity coordinate
     *
     * @since   S60 9.2
     * @param   none
     * @return  Red x chromaticity coordinate
     */
    IMPORT_C TUint16 GetColorCoordinatesRedX();

    /**
     * Gets red y chromaticity coordinate
     *
     * @since   S60 9.2
     * @param   none
     * @return  Red y chromaticity coordinate
     */
    IMPORT_C TUint16 GetColorCoordinatesRedY();

    /**
     * Gets green x chromaticity coordinate
     *
     * @since   S60 9.2
     * @param   none
     * @return  Green x chromaticity coordinate
     */
    IMPORT_C TUint16 GetColorCoordinatesGreenX();

    /**
     * Gets green y chromaticity coordinate
     *
     * @since   S60 9.2
     * @param   none
     * @return  Green y chromaticity coordinate
     */
    IMPORT_C TUint16 GetColorCoordinatesGreenY();

    /**
     * Gets blue x chromaticity coordinate
     *
     * @since   S60 9.2
     * @param   none
     * @return  Blue x chromaticity coordinate
     */
    IMPORT_C TUint16 GetColorCoordinatesBlueX();

    /**
     * Gets blue y chromaticity coordinate
     *
     * @since   S60 9.2
     * @param   none
     * @return  Blue y chromaticity coordinate
     */
    IMPORT_C TUint16 GetColorCoordinatesBlueY();

    /**
     * Gets white x chromaticity coordinate
     *
     * @since   S60 9.2
     * @param   none
     * @return  White x chromaticity coordinate
     */
    IMPORT_C TUint16 GetColorCoordinatesWhiteX();

    /**
     * Gets white y chromaticity coordinate
     *
     * @since   S60 9.2
     * @param   none
     * @return  White y chromaticity coordinate
     */
    IMPORT_C TUint16 GetColorCoordinatesWhiteY();

    /**
     * Gets Established Timings 1
     * EEDID standatrd R3EDID Plug & Play compliant displays shall show
     * support for the BASE VIDEO MODE (640 × 480 @ 60Hz) and shall
     * indicate support in the Established Timing I data field
     *
     * @since   S60 9.2
     * @param   none
     * @return  Bits for Established Timings 1
     */
    IMPORT_C TUint8 GetEstablishedTimings1();

    /**
     * Gets Established Timings 2
     *
     * @since   S60 9.2
     * @param   none
     * @return  Bits for Established Timings 2
     */
    IMPORT_C TUint8 GetEstablishedTimings2();

    /**
     * Gets Manufacturer Timings
     *
     * @since   S60 9.2
     * @param   none
     * @return  Bits for Manufacturer Timings
     */
    IMPORT_C TUint8 GetManufacturerTimings();
    
    /**
     * Gets standard timings.
     * 
     * @param aIndex Byte between 0-15.
     * @return Bits for selected byte
     */
    IMPORT_C TUint8 GetStandardTimings( TInt aIndex );

    /**
     * Gets descriptor block
     *
     * @since   S60 9.2
     * @param   aDescriptorBlockNumber
     * @return  Desriptor block
     */
    IMPORT_C TEdidDescriptorBlock GetDescriptorBlock( TUint8 aDescriptorBlockNumber )  const;

    /**
     * Gets number of extensions
     *
     * @since   S60 9.2
     * @param   none
     * @return  Number of extensions
     */
    IMPORT_C TUint8 GetNumberOfExtensions();

    /**
     * Reads extension block.
     * Block number 1 = first extension block, ..
     * Data is copied to iExtensionData
     *
     * @since   S60 9.2
     * @param   aNumber - Extension number
     * @return  Possible return values:
     *  KErrNone Extension read succesfully
     *  KErrNotSupported Extension not supported
     */
    IMPORT_C TInt ReadExtension( TInt aNumber );

    /**
     * Gets extension type
     * See TEdidExtensionTagCode
     *
     * @since   S60 9.2
     * @param   aNumber - Extension number
     * @return  TEdidExtensionTagCode
     */
    IMPORT_C TEdidExtensionTagCode GetExtensionType( TInt aNumber );

    /**
     * Creates CEA 861 extension parser
     *
     * @since   S60 9.2
     * @param   aNumber - Extension number
     * @return  CCea861EdidParser* (Pointer to the parser object)
     */
    IMPORT_C CCea861EdidParser* CreateCea861ExtensionParserL( TInt aNumber );

    /**
     * Gets checksum value
     *
     * @since   S60 9.2
     * @param   none
     * @return  Checksum value
     */
    IMPORT_C TUint8 GetChecksum();

	IMPORT_C void UpdateRawDataL( const TDesC8& aBinaryData );
	IMPORT_C void UpdateCea861ExtensionL( TInt aNumber, CCea861EdidParser* aParser );

private:

    /**
     * Default constructor.
     */
    CEdidParserBase();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const TDesC8& aBinaryData );

    /**
     * Gets pixel clock from descriptor block
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint16 GetPixelClock( TUint8 aDataBlockNumber )  const;

    /**
     * Gets Horizontal Addressable Video in pixels
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint16 GetHorizontalAddressableVideo( TUint8 aDataBlockNumber )  const;

    /**
     * Gets Horizontal Blanking in pixels
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint16 GetHorizontalBlanking( TUint8 aDataBlockNumber )  const;

    /**
     * Gets Vertical Addressable Video in lines
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint16 GetVerticalAddressableVideo( TUint8 aDataBlockNumber )  const;

    /**
     * Gets Vertical Blanking in lines
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint16 GetVerticalBlanking( TUint8 aDataBlockNumber ) const;

    /**
     * Gets Horizontal Front Porch in pixels
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint16 GetHorizontalFrontPorch( TUint8 aDataBlockNumber ) const;

    /**
     * Gets Horizontal Sync Pulse Width in lines
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint16 GetHorizontalSyncPulseWidth( TUint8 aDataBlockNumber ) const;

    /**
     * Gets Vertical Front Porch in lines
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint16 GetVerticalFrontPorch( TUint8 aDataBlockNumber ) const;

    /**
     * Gets Vertical Sync Pulse Width in lines
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint8 GetVerticalSyncPulseWidth( TUint8 aDataBlockNumber ) const;

    /**
     * Gets Horizontal Addresable Image Size
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint16 GetHorizontalAddressableImageSize( TUint8 aDataBlockNumber ) const;

    /**
     * Gets Vertical Addresable Image Size
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint16
     */
    TUint16 GetVerticalAddressableImageSize( TUint8 aDataBlockNumber ) const;

    /**
     * Gets Right Horizontal Border or Left Horizontal Border in Pixels
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint8
     */
    TUint8 GetHorizontalBorderSize( TUint8 aDataBlockNumber ) const;

    /**
     * Gets Right Vertical Border or Left Horizontal Border in Pixels
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TUint8
     */
    TUint8 GetVerticalBorderSize( TUint8 aDataBlockNumber ) const;

    /**
     * Checks whether or not video is interlaced
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TBool
     */
    TBool GetVideoIsInterlaced( TUint8 aDataBlockNumber ) const;

    /**
     * Gets stero support
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TEdidStereoViewingSupport
     */
    TEdidStereoViewingSupport GetStereoViewingSupport( TUint8 aDataBlockNumber ) const;

    /**
     * Gets signal definitions
     *
     * @since   S60 9.2
     * @param   aDataBlockNumber
     * @return  TEdidSyncSignalDefinitions
     */
    TEdidSyncSignalDefinitions GetSyncSignalDefinitions( TUint8 aDataBlockNumber ) const;

    /**
     * Gets analog sync signal definitions
     *
     * @since   S60 9.2
     * @param   aByte17
     * @return  TEdidSyncSignalDefinitions
     */
    static TEdidSyncSignalDefinitions GetAnalogSyncSignalDefinitions( const TUint aByte17 );

    /**
     * Gets digital sync signal definitions
     *
     * @since   S60 9.2
     * @param   aByte17
     * @return  TEdidSyncSignalDefinitions
     */
    static TEdidSyncSignalDefinitions GetDigitalSyncSignalDefinitions( const TUint aByte17 );

private: // data

     TEdidBaseBlock  iBaseEdidData;             // Base EDID data
     HBufC8*         iRawData;                  // Raw data that contains all EDID data
     TUint           iNumberOfExtensionBlocks;  // Number of extensionblocks for this base edid block
     TExtDataBlock   iExtensionData;            // Extension data
     TInt            iRawdataLength;            // Raw data length
    };

#endif // C_CEDIDPARSERBASE_H
