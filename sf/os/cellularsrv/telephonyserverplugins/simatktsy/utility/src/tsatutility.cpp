// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : TSatUtility.cpp
// Part of     : Common Sim Atk TSY / commonsimatktsy
// Sat Utility classes implementation.
// Version     : 1.0
//



// INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tsatutilityTraces.h"
#endif

#include "tsatutility.h"        // Class header
#include <ctsy/serviceapi/cmmsmsutility.h>		// TON and NPI constants

// -----------------------------------------------------------------------------
// TSatUtility::TonAndNpi
// Map TON and NPI to RSat TON and NPI values
// -----------------------------------------------------------------------------
//
EXPORT_C void TSatUtility::TonAndNpi
        (
        const TInt aTonAndNpi,                
        RSat::TTypeOfNumber* aTon,      
        RSat::TNumberingPlan* aNpi      
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_TONANDNPI_1,  "UTILITY: TSatUtility::TonAndNpi" );
    TInt ton( ( aTonAndNpi >> 4 ) & KTONMask ); // TON mask value 0x07

    switch ( ton )
        {
        case KTonUnknown:
            {
            *aTon = RSat::EUnknownNumber;
            break;
            }
        case KInternationalNumber:
            {
            *aTon = RSat::EInternationalNumber;
            break;
            }
        case KNationalNumber:
            {
            *aTon = RSat::ENationalNumber;
            break;                    
            }
        case KNetworkSpecificNumber:
            {
            *aTon = RSat::ENetworkSpecificNumber;
            break;
            }
        case KSubscriberNumber:
            {
            *aTon = RSat::EDedicatedNumber;
            break;
            }
        case KAlphanumeric:         
            {
            *aTon = RSat::EAlphanumericNumber;
            break;
            }
        case KAbbreviatedNumber:    
            {
            *aTon = RSat::EAbbreviatedNumber;
            break;
            }
        default:
            {
            *aTon = RSat::ETypeOfNumberNotSet;
            break;
            }
        }

    // Numbering plan
    TInt npi( aTonAndNpi & KNPIMask ); // NPI mask value 0x0F

    switch ( npi )
        {
        case KNpiUnknown:
            {
            *aNpi = RSat::EUnknownNumberingPlan;
            break;
            }
        case KIsdnTelephoneNumPlan:
            {
            *aNpi = RSat::EIsdnNumberPlan;
            break;
            }
        case KDataNumPlan:
            {
            *aNpi = RSat::EDataNumberPlan;
            break;
            }
        case KTelexNumPlan:
            {
            *aNpi = RSat::ETelexNumberPlan;
            break;
            }
        case KNationalNumPlan:
            {
            *aNpi = RSat::ENationalNumberPlan;
            break;
            }
        case KPrivateNumPlan:
            {
            *aNpi = RSat::EPrivateNumberPlan;
            break;
            }
        case KServiceCentreSpecificPlan1:
            {
            *aNpi = RSat::EServiceCentreSpecificPlan1;
            break;
            }
        case KServiceCentreSpecificPlan2:
            {
            *aNpi = RSat::EServiceCentreSpecificPlan2;
            break;
            }
        case KErmesNumPlan:    
            {
            *aNpi = RSat::EERMESNumberPlan;
            break;
            }
        default:
            {
            *aNpi = RSat::ENumberingPlanNotSet;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::Packed7to8Unpacked
// Converts 7-bit packed string to 8-bit unpacked format.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TSatUtility::Packed7to8Unpacked
        ( 
        const TPtrC8 aSource,   
        TDes8& aTarget    
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_PACKED7TO8UNPACKED_1,  "UTILITY: TSatUtility::Packed7to8Unpacked" );
    TInt ret( KErrNone );
    // The string is in packed GSM default alphabet format.
    // Converted to 8-bit format
    TUint8 maskRightPartOfCurrentByte( 0x7F );
    TUint8 shiftLeft( 0 );
    TUint8 leftPartFromPreviousByte( 0 );  
    TInt sourceLength = aSource.Length();  
    // Initialize the target string before starting
    aTarget.Zero();
    
    // Check out which string is the shortest and use its length as a limit
    TInt length ( Min( sourceLength, aTarget.MaxLength() ) );
    for ( TInt i = 0; i < length; i++ )
        {
        TUint8 leftPartOfCurrentChar =  static_cast<TUint8>( ( 
            aSource[i] & maskRightPartOfCurrentByte ) << shiftLeft );
            
        // Append the character in the output text string
        aTarget.Append( leftPartOfCurrentChar | leftPartFromPreviousByte );  
              
        // Updates         
        if ( 6 == shiftLeft )
            {
            // After 6 shifts, the character is in bit7..bit1, 
            // therefore it has to be shifted one bit to the right.
            aTarget.Append( aSource[i] >> 1 );
            // Restart
            leftPartFromPreviousByte = 0;
            shiftLeft = 0;
            maskRightPartOfCurrentByte = 0x7F;            
            }
        else
            {
            leftPartFromPreviousByte = static_cast<TUint8>( ( 
                aSource[i] ) >> ( 7-shiftLeft ) );
            maskRightPartOfCurrentByte = static_cast<TUint8>( 
                maskRightPartOfCurrentByte >> 1 );
            shiftLeft++;
            } 
        }   
        
    // Whe should still notify the requesting method about exceeding the length
    if ( length < sourceLength )
        {
        ret = KErrOverflow;
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_PACKED7TO8UNPACKED_2,  "UTILITY: TSatUtility::Packed7to8Unpacked,Input data too long. %d bytes could not be converted", ( sourceLength - length ) );
        }
  
    return ret;
    }

// -----------------------------------------------------------------------------
// TSatUtility::UCSToPacked7
// Converts UCS2 string to 7-bit packed format
// -----------------------------------------------------------------------------
//
EXPORT_C void TSatUtility::UCSToPacked7
        ( 
        const TPtrC aInput,   
        TDes8& aOutput   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_UCSTOPACKED7_1,  "UTILITY: TSatUtility::UCSToPacked7" );
    
    TBuf8<KMaxUssdStringLengthInBytes> string;
    
    // This method interrupts converting if the target buffer is full, so
    // the length can not be exceeded
    ConvertUnicode16To7Bit( aInput, string ); 

    TUint8 move( 0 );
    TInt i( 0 );
    TInt length( string.Length() );

    // If the input data was too long, some data will miss from the end here
    for ( i = 0; i < length; i += 1 )
        {
        // Get first character
        TUint8 char1 = static_cast<TUint8>( string[i] >> move ); 
        TUint8 char2;
        
        if ( ( i + 1 ) < length )
            {
            // Get next character
            char2 = static_cast<TUint8>( string[i + 1]  << ( 7 - move ) );
            }
        else
            {
            // No more characters
            char2 = 0;
            }
        // Append packed character
        aOutput.Append( static_cast<TUint8>( char1 | char2 ) );

        if ( ( 6 == move ) && char2 )
            {
            i++;
            move = 0;
            }
        else
            {
            move++;
            }
        }

    if ( !( ( length + 1 ) % 8 ) )
        {
        // If the total number of characters in the text string equals (8n-1) 
        // where n=1,2,3 etc. then there are 7 spare bits at the end of the 
        // message. To avoid the situation where the receiving entity confuses
        // 7 binary zero pad bits as the @ character, the carriage return 
        // (i.e. <CR>) character shall be used for padding in this situation,
        // as defined in TS 23.038 [5].
        aOutput[  aOutput.Length() - 1 ] = static_cast<TUint8>( 
            aOutput[ aOutput.Length() - 1 ] | KCrShiftedOneBitToLeft );
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::BCDToAscii
// Converts BCD string to ASCII format
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TSatUtility::BCDToAscii		
        ( 
        const TPtrC8 aInput,  
        TDes8& aOutput   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_BCDTOASCII_1,  "UTILITY: TSatUtility::BCDToAscii" );
    TInt ret( KErrNone );
    
    // Convert Abbreviated dialling numbers format back to ASCII format.
    // See 3GPP TS 11.11, EFadn
    aOutput.Zero();
    // Check out which string is the shortest and use its length as a limit
    TInt length ( Min( aInput.Length(), aOutput.MaxLength() ) );
    
    for ( TInt i = 0; i < length; i++ )
        {
        // Two bcd string chars are coded in one byte,
        // 1st char is in low nibble and 2nd is in high nibble
        // if the high nibble doesn't contain a char it's value is 0xf
        TUint8 byte = aInput[i];
        aOutput.Append( KAscii[byte & 0x0F] );
        
        if ( KMaskF0 != ( byte & KMaskF0 ) )
            {
            aOutput.Append( KAscii[byte >> 4] );
            }
        }
        
    // Whe should still notify the requesting method about exceeding the length
    if ( length < aInput.Length() )
        {
        ret = KErrOverflow;
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_BCDTOASCII_2,  "UTILITY: TSatUtility::BCDToAscii,Input data too long.%d bytes could not be converted", ( aInput.Length() - length ) );
        }    
        
    return ret;
    }

// -----------------------------------------------------------------------------
// TSatUtility::AsciiToBCD
// Convert ASCII string to binary coded decimal, invalid characters are dropped 
// and will not be part of bcd string.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TSatUtility::AsciiToBCD
        ( 
        const TDesC8& aInput, 
        TDes8& aOutput        
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_ASCIITOBCD_1,  "UTILITY: TSatUtility::AsciiToBCD" );
    TInt ret( KErrNone );
    TInt i;
    TInt j;
    TInt outLen( 0 );
    TInt length( aInput.Length() );
    
    aOutput.Zero();
    
    // Go through the whole string
    for ( i = 0; i < length; i++ )
        {
        TUint8 bcd = 0x0F;  // Should never be part of number
        TBool found( EFalse );
        
        // Search for the ASCII character
        for ( j = 0; ( j < KAsciiBcdTableLength ) && !found; j++ )
            {
            // If the character matches, set the corresponding value
            if ( KAsciiToBCD[j][0] == aInput[i] )
                {
                bcd = KAsciiToBCD[j][1];
                found = ETrue;
                }
            }

        // Add only valid bcd characters...
        if ( found )
            {
            if( aOutput.MaxLength() > aOutput.Length() )
            	{               	
	            // Store to string
	            if ( ( outLen % 2 ) == 0 )
	                {
	                aOutput.Append( bcd );
	                }
	            else
	                {
	                aOutput[outLen / 2] |= ( bcd << 4 );
	                }
            	}
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_ASCIITOBCD_2,  "UTILITY: TSatUtility::AsciiToBCD, Overflow!" );
                ret = KErrOverflow;
                }
                
            outLen++;
            }
        else
            {
            OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_ASCIITOBCD_3, "UTILITY: TSatUtility::AsciiToBCD -- dropped character %d at i=%d", TInt( aInput[i] ), i );
            }
            
        } // For

    // If odd number of digits add endmark
    if ( ( outLen % 2 ) != 0 )
        {
        aOutput[outLen / 2] |= KMaskF0;
        }
        
    return ret;
    }

// -----------------------------------------------------------------------------
// TSatUtility::RemoveWildAndExpansionDigit
// Remove Wild 'w' and Expansion digit '.' from EFadn string. Used in SetUpCall 
// proactive command.
// -----------------------------------------------------------------------------
//
EXPORT_C void TSatUtility::RemoveWildAndExpansionDigit
        (
        const TPtrC8 aString,  
        TDes8& aOutput   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_REMOVEWILDANDEXPANSIONDIGIT_1,  "UTILITY: TSatUtility::RemoveWildAndExpansionDigit" );
    aOutput.Zero();    
    TUint8 i( 0 );
    TInt maxLength = aOutput.MaxLength();
    TInt length( aString.Length() );
    
    // Append as many characters as there is room for in the target string
    for ( i = 0; ( i < length ) && ( aOutput.Length() < maxLength ); i++ )
        {
        if ( ( 'w' != aString[i] ) && ( '.' != aString[i] ) )
            {
            aOutput.Append( aString[i] );
            }
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::SetAlphaId
// Set Alpha identifier as a Unicode text string and according to the alphabet 
// used
// -----------------------------------------------------------------------------
//
EXPORT_C void TSatUtility::SetAlphaId				
        ( 
        const TPtrC8 aRawData, 
        TDes& aAlphaId  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_SETALPHAID_1,  "UTILITY: TSatUtility::SetAlphaId" );
    if ( ( KUCS2ArabicCoding == aRawData[0] )
        || ( KUCS2GreekCoding == aRawData[0] )
        || ( KUCS2TurkishCoding == aRawData[0] ) )
        {
        ConvertAlphaFieldsToUnicode( aRawData, aAlphaId );
        }
    else
        {
        // 8-bit 
        TBuf8<RSat::KAlphaIdMaxSize> rawData8;
        rawData8.Copy( aRawData );
        Convert7BitToUnicode16( rawData8, aAlphaId );
        }     
    }

// -----------------------------------------------------------------------------
// TSatUtility::ConvertToSemiOctet
// Convert integer to BCD format. Only two last digits is used.
// Example TInt 2004 -> 0x40.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint8 TSatUtility::ConvertToSemiOctet
        ( 
        const TInt aTime  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_CONVERTTOSEMIOCTET_1,  "UTILITY: TSatUtility::ConvertToSemiOctet" );
    // Converting given time to meet the TP-Service-Centre-Time-Stamp format in
    // 3GPP TS 23.040.

    TInt msd( ( aTime / 10 ) % 10 ); // Most significant decimal
    TInt lsd( ( aTime % 10 ) );      // Least significant decimal
    TUint8 ret( TUint8( ( lsd << 4 ) | ( msd ) ) );
    return ret;
    }

// -----------------------------------------------------------------------------
// TSatUtility::Convert7BitToUnicode16
// Convert a text from GSM 7 bit default alphabet to Unicode format.
// -----------------------------------------------------------------------------
//
EXPORT_C void TSatUtility::Convert7BitToUnicode16
        (
        const TDesC8& aInput,
        TDes16& aOutput  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_CONVERT7BITTOUNICODE16_1,  "UTILITY: TSatUtility::Convert7BitToUnicode16" );
    TInt i( 0 );
    
    aOutput.Zero();
    
    // Check out which string is the shortest and use its length as a limit
    TInt length ( Min( aInput.Length(), aOutput.MaxLength() ) );
    
    for( i = 0; i < length; i++ )
        {
        TUint8 character( aInput[i] );

        // This code is an escape to an extension of the 7 bit default alphabet
        // table. 
        if ( 0x1B == character )
            {
            // Extension table
            switch ( aInput[i+1] )
                {
                case 0x28: //{
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7B ) );
                    break;
                    }
                case 0x29: //}
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7D ) );
                    break;
                    }
                case 0x3C: //[
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5B ) );
                    break;
                    }
                case 0x3E: //]
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5D ) );
                    break;
                    }
                case 0x3D: //~
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7E ) );
                    break;
                    }
                case 0x2F: 
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5C ) );
                    break;
                    }
                case 0x14: // ^
                    {
                    aOutput.Append( static_cast<TUint16>( 0x5E ) );
                    break;
                    }
                case 0x65: // Euro 0x20AC
                    {
                    aOutput.Append( static_cast<TUint16>( 0x20AC ) );
                    break;
                    }
                case 0x40: // |
                    {
                    aOutput.Append( static_cast<TUint16>( 0x7C ) );
                    break;
                    }
                default:
                    {
                    // A space if not found in the table
                    aOutput.Append( static_cast<TUint16>( 0x20 ) );
                    break;
                    }
                }
            // Characters in extension table takes two bytes
            i++;
            }
        // Check that bit 8 is set to '0'
        else if ( 0x7F >= character )
            {
            // Character is in normal 7-bit table.
            aOutput.Append( KUnicode[ character ] );
            }
        else
            {
            // Do nothing
            }
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::ConvertUnicode16To7Bit
// Converts unicode16 string to GSM 7 bit default alphabet character mode
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TSatUtility::ConvertUnicode16To7Bit
        (
        const TDesC16& aInput,
        TDes8& aOutput   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_CONVERTUNICODE16TO7BIT_1,  "UTILITY: TSatUtility::ConvertUnicode16To7Bit" );
    TInt i( 0 );
    TInt j( 0 );
    TInt ret( KErrNone );
    TBool found( EFalse );
    TInt outputMaxLength = aOutput.MaxLength();
    
    // Check out which string is the shortest and use its length as a limit
    TInt length ( Min( aInput.Length(), aOutput.MaxLength() ) );
    for ( i = 0; i < length; i++ )
        {
        for ( j = 0; j < KSizeOfConversionArray; j++ )
            {
            if ( KUnicode16ToSms7[j][0] == aInput[i] )
                {

                aOutput.Append( static_cast<TUint8>( 
                    KUnicode16ToSms7[j][1] ) );
                found = ETrue;
                }
            }
            
        if ( !found )
            {
            aOutput.Append( static_cast<TUint8>( aInput[i] & 0x00FF ) );
            }
            
        found = EFalse;                        
        }
    
    // Whe should still notify the requesting method about exceeding the length
    if ( length < aInput.Length() )
        {
        ret = KErrOverflow;
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_CONVERTUNICODE16TO7BIT_2,  "UTILITY: TSatUtility::ConvertUnicode16To7Bit,Input data too long. %d bytes could not be converted", ( aInput.Length() - length ) );
        }
        
    return ret;
    }

// -----------------------------------------------------------------------------
// TSatUtility::FillDurationStructure
// Fill in a TDuration structure
// -----------------------------------------------------------------------------
//
EXPORT_C void TSatUtility::FillDurationStructure
        (
        CBerTlv& aBerTlv,             
        RSat::TDuration& aTDuration   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_FILLDURATIONSTRUCTURE_1,  "UTILITY: TSatUtility::FillDurationStructure" );
    CTlv duration;
    aTDuration.iTimeUnit = RSat::ENoDurationAvailable;
    TInt returnValue( aBerTlv.TlvByTagValue( 
        &duration, KTlvDurationTag ) );
        
    if ( KErrNotFound != returnValue )
        {
        TUint8 durationTimeUnit = duration.GetShortInfo( ETLV_TimeUnit );
        switch ( durationTimeUnit )
            {
            case KMinutes:
                {
                // Minutes
                aTDuration.iTimeUnit = RSat::EMinutes;
                break;
                }
            case KSeconds:
                {
                // Seconds
                aTDuration.iTimeUnit = RSat::ESeconds;
                break;
                }
            case KTenthsOfSeconds:
                {
                // Tenths of seconds
                aTDuration.iTimeUnit = RSat::ETenthsOfSeconds;
                break;
                }
            default:
                {
                aTDuration.iTimeUnit = RSat::ETimeUnitNotSet;
                }
            }
            // Time interval
            aTDuration.iNumOfUnits = duration.GetShortInfo( ETLV_TimeInterval );     
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::FillIconStructure
// Fill in a TIconId structure
// -----------------------------------------------------------------------------
//
EXPORT_C void TSatUtility::FillIconStructure
        ( 
        CBerTlv& aBerTlv,        
        RSat::TIconId& aTIconId, 
        const TInt aItemNmb            
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_FILLICONSTRUCTURE_1,  "UTILITY: TSatUtility::FillIconStructure" );
    CTlv iconId;
    aTIconId.iQualifier = RSat::ENoIconId;
    TInt returnValue( aBerTlv.TlvByTagValue( &iconId, KTlvIconIdentifierTag, 
        aItemNmb ) );
    
    if ( KErrNotFound != returnValue )
        {
        TUint8 iconQualifier = iconId.GetShortInfo( ETLV_IconQualifier );        
        aTIconId.iIdentifier = iconId.GetShortInfo( ETLV_IconIdentifier );
        // The icon qualifier indicates to the ME how the icon is to be used.
        if ( iconQualifier )
            {
            aTIconId.iQualifier = RSat::ENotSelfExplanatory;
            }
        else
            {
            aTIconId.iQualifier = RSat::ESelfExplanatory;
            }
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::SetText
// Set Text string as a Unicode text string and according to the alphabet used
// -----------------------------------------------------------------------------
//
EXPORT_C void TSatUtility::SetText 
        (
        CTlv& aTextTlv, 
        TDes& aUnicodeOutput   
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_SETTEXT_1,  "UTILITY: TSatUtility::SetText" );
    if ( aTextTlv.GetLength() )
        {
        TPtrC8 sourceString;

        sourceString.Set( aTextTlv.GetData( ETLV_TextString ) );

        TBuf8<KTextBufferMaxSize> string( sourceString );
        // SMS default alphabet DCS
        if ( !( aTextTlv.GetShortInfo( ETLV_DataCodingScheme ) 
            & KPacked7BitTextMask ) )
            {
            // Unpack
            Packed7to8Unpacked( sourceString, string );
            
            if ( aUnicodeOutput.MaxLength() < string.Length() )
                {
                string.SetLength( aUnicodeOutput.MaxLength() );
                }
            // Convert to unicode format
            Convert7BitToUnicode16( string, aUnicodeOutput );
            }
        // UCS2 DCS
        else if ( aTextTlv.GetShortInfo( ETLV_DataCodingScheme )
            & KUCS2DCS )
            {
            Copy8to16LE( sourceString, aUnicodeOutput );
            }
        // 8-bit DCS
        else if ( aTextTlv.GetShortInfo( ETLV_DataCodingScheme )
            & K8BitDCS )
            {            
            // 8-bit string to 16-bit string
            Convert7BitToUnicode16( string, aUnicodeOutput );
            }
        else // Reserved cases: SMS default alphabet
            {
             // Unpack
            Packed7to8Unpacked( sourceString, string );
            
            if ( aUnicodeOutput.MaxLength() < string.Length() )
                {
                string.SetLength( aUnicodeOutput.MaxLength() );
                }
            // Convert to unicode format
            Convert7BitToUnicode16( string, aUnicodeOutput );           
            }
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::ConvertAlphaFieldsToUnicode
// Convert Alpha fields format to Unicode format.
// -----------------------------------------------------------------------------
//
EXPORT_C void TSatUtility::ConvertAlphaFieldsToUnicode
        (
        const TDesC8& aSource,
        TDes& aTarget 
        )
    {   
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_CONVERTALPHAFIELDSTOUNICODE_1,  "UTILITY: TSatUtility::ConvertAlphaFieldsToUnicode" );
    // ArabicCoding, GreekCoding and TurkishCoding have different coding 
    // methods. There is a tag for each type of alphabet (resp. 80, 81 or 82) 
    // before the text, and there are base pointers used for expanding 1 byte 
    // to 2 bytes as required by UCS2  
    // Ref: 3GPP TS 11.11, Annex B

    // Base pointer is a "half-page" in the UCS2 code space
    TUint16 basePointer( 0 );
    TInt offset( 0 );
   
    TInt ret( KErrNone );

    switch ( aSource[0] )
        {
        case KUCS2ArabicCoding:
            {
            // Copy everything after tag byte
            Copy8to16LE( aSource.Mid( 1, aSource.Length() - 1 ), aTarget );
            //Check if any text present
            if ( aTarget.Length() )
                {
                // Remove padding bytes if any
                if ( 0xFFFF == aTarget[ aTarget.Length() - 1 ] )
                    {
                    aTarget.SetLength( aTarget.Length() - 1 );
                    }
                }
            // No expanding needed, already in unicode format.
            ret = KErrNotFound;
            break;
            }
        case KUCS2GreekCoding:
            {
            // Characters starts at position 3
            offset = 3;
            // Base pointer is given in one byte
            // and needs to be sifted 7-bit to left to get correct base pointer
            basePointer = TUint16( aSource[2] << 7 ) ;
            break;
            }
        case KUCS2TurkishCoding:
            {
            // Characters starts at position 4
            offset = 4;
            // Base pointer is two bytes, 16-bit
            TSatUtility::CopyTwo8toOne16LE( aSource, basePointer, 2 );
            break;
            }
        default:
            // Do nothing
            break;
        }
    
    if ( ( KErrNone == ret ) && ( 2 < aSource.Length() ) )
        {
         // The second octet contains a value indicating the number of 
         // characters in the string. (Not in the case of Arabic string)
         TInt length( aSource[1] );
         
        // Expanding 1 byte format to 2 bytes
        while ( length )
            {
            // If bit 8 of the octet is set to zero, the remaining 7 bits of 
            // the octet contain a GSM Default Alphabet character, whereas if
            // bit 8 of the octet is set to one, the remaining seven bits are
            // an offset value added to the base pointer defined in octets 
            // three and four, and the resultant 16 bit value is a UCS2 code 
            // point, and defines a UCS2 character.
            if ( 0x7F <= aSource[offset] )
                {
                // Append base pointer and offset
                aTarget.Append( basePointer + ( aSource[offset] & 0x7F ) );
                }
            else
                {
                // GSM default alphabet.
                TBuf<1> dest16bit;
                TBuf8<1> src7bit;
                // Get character
                src7bit.Append( aSource[offset] );
                // Convert GSM default alphabet character to unicode
                Convert7BitToUnicode16( src7bit, dest16bit );
                // Append converted character to output string
                aTarget.Append( dest16bit );
                }
            offset++;
            length--;
            }
        }
    }

// -----------------------------------------------------------------------------
// TSatUtility::Copy16to8LE
// Converts 16-bit data to 8-bit data with little endian.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TSatUtility::Copy16to8LE
        (
        const TDesC16& aSource,  
        TDes8& aTarget            
        )
    {   
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_COPY16TO8LE_1,  "UTILITY: TSatUtility::Copy16to8LE" );
    TInt ret( KErrNone );
    TInt length( 0 );
    
    // Checks that data length are acceblable
    if ( aSource.Size() > aTarget.MaxSize() )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_COPY16TO8LE_2,  "UTILITY: TSatUtility::Copy16to8LE, Length exceeded!" );
        ret = KErrOverflow;
        length = ( aTarget.MaxSize() / 2 );
        }
    else
        {
        length = aSource.Length();
        }

    // Append source to target using rigth endian
    for ( TInt i = 0; i < length; i++ )
        {
        aTarget.Append( TUint8( aSource[i] >> 8) );
        aTarget.Append( TUint8( aSource[i] & 0xff ) );
        }
        
    return ret;
    }
    
// -----------------------------------------------------------------------------
// TSatUtility::Copy8to16LE
// Converts 8-bit data to 16-bit data with little endian
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TSatUtility::Copy8to16LE
        (
        const TDesC8& aSource,  
        TDes16& aTarget         
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_COPY8TO16LE_1,  "UTILITY: TSatUtility::Copy8to16LE" );
    TInt ret( KErrNone );
    // Check out which string is the shortest and use its length as a limit
    TInt length ( Min( ( ( aSource.Length() / 2 ) * 2 ), aTarget.MaxSize() ) );

    // Append source to target using right endian
    for ( TInt i = 0; i < length; i += 2 )
        {
        aTarget.Append( TUint16( aSource[i + 1] | ( aSource[i] << 8) ) );
        }
        
    // Whe should still notify the requesting method about exceeding the length
    if ( length < aSource.Length() )
        {
        ret = KErrOverflow;
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_COPY8TO16LE_2,  "UTILITY: TSatUtility::ConvertUnicode16To7Bit,Input data too long. %d bytes could not be converted", ( aSource.Length() - length ) );
        }
        
    return ret;
    }

// -----------------------------------------------------------------------------
// TSatUtility::DecodeCbsDcs
// Finds whether the data coding scheme, coded in CBS format, is 7-bit, 8-bit 
// or 16-bit. Converts the input DCS from CBS format to SMS format. 
// Reference: 3gpp ts 23.038
// -----------------------------------------------------------------------------
//
EXPORT_C TSmsDcs TSatUtility::DecodeCbsDcs
		(
    	const TUint8 aDcs 
    	)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_DECODECBSDCS_1, "UTILITY: TSatUtility::DecodeCbsDcs");
    
    // Constant values are not defined in order to
    // avoid confusion with too many constants names.
    
    // Coding group: 4 left most significant bits
    TUint8 codingGroup = ( aDcs & 0xF0 )>>4;
    // Lower quartet: 4 right most significant bits
    TUint8 lowQuartet = ( aDcs & 0x0F );
    // Character set: bit 2 and 3, in [bit7...bit0]
    TUint8 characterSet = ( aDcs & 0x0C )>>2;
    // Initialize output to Unknown or Reserved
    TSmsDcs decodedDcs( ESmsUnknownOrReservedDcs );
    // Switch according to the coding group
    switch ( codingGroup )
        {
        // Language specified, usually in 7-bit
        // b0000 or b0010 or b0011
        case 0x00:
        case 0x02:
        case 0x03:
            {
            // 7-bit alphabet
            decodedDcs = ESms7BitDcs;
            break;
            }
        // b0001
        // Message preceded by language indication
        case 0x01: 
            {
            
            if ( 0x00==lowQuartet )
                {
                decodedDcs = ESms7BitDcs;
                }
            else if ( 0x01==lowQuartet )
                {
                decodedDcs = ESms16BitDcs;
                }
            break;
            }
        // General data coding indication
        // b01xx  or b1001
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x09:
            {
            // The character set determines the alphabet/compression
            if ( 0x00 == characterSet )
                {
                decodedDcs = ESms7BitDcs;
                }
            else if ( 0x01 == characterSet )
                {
                decodedDcs = ESms8BitDcs;
                }
            else if ( 0x02 == characterSet )
                {
                decodedDcs = ESms16BitDcs;
                }
            break;
            }
        // Data coding / Message handling
        // either 8-bit or 7-bit
        case 0x0F:
            {
            // only bit 2 informs about the
            // character set.
            if ( aDcs & 0x04 )
                {
                decodedDcs = ESms8BitDcs;
                }
            else
                {
                decodedDcs = ESms7BitDcs;
                }
            break;
            }
        default:
            {
            // the DCS value is reserved.
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_DECODECBSDCS_2, "UTILITY: TSatUtility::DecodeCbsDcs, reserved value");
            break;
            }
        }
    return decodedDcs;
    }

// -----------------------------------------------------------------------------
// TSatUtility::CopyTwo8toOne16LE
// Gets two 8-bit bytes and copies them to the one 16-bit byte with little 
// endianes. Used when reading data from response or indication. 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TSatUtility::CopyTwo8toOne16LE
        (
        const TDesC8& aSource,
        TUint16& aTarget,       
        const TInt aIndex         
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_COPYTWO8TOONE16LE_1, "UTILITY: TSatUtility::CopyTwo8toOne16LE");
    TInt ret( KErrNone );
    // Check first that we dont try to read data that is not there..
    if ( aSource.Length() > aIndex + 1 )
        {
        aTarget = static_cast<TUint16>( aSource[aIndex] << 8 );         
        aTarget = static_cast<TUint16>( aTarget | aSource[aIndex + 1] );
        }
    else
        {
        ret = KErrOverflow;
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSATUTILITY_COPYTWO8TOONE16LE_2, "UTILITY: TSatUtility::CopyTwo8toOne16LE, Index too highIndex: %d, Source data length: %d", aIndex, aSource.Length() );
        }
        
    return ret;
    }

            
// End of file
