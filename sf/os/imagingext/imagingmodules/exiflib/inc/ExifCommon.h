/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines Exif-related constant data used internally by the 
*                service classes, and a utility class providing internal common 
*                services.
*
*/


#ifndef EXIFCOMMON_H
#define EXIFCOMMON_H

//  INCLUDES
#include <e32base.h>
#include "ExifTag.h"

// STRUCTURES

struct TReferenceTag
    {
    enum TExtraDataType
        {
            ETagLongOrShort = 11
        };
        // Tag ID
    	TUint16 iId;

        // Tag data type
    	TInt iDataType;

        // Number of tag data elements (having tag data type).
    	TUint32 iDataCount;
    };

// CONSTANTS

// Jpeg markers
const TUint8 KMarkerStart = 0xff;
const TUint16 KSoi = 0xffd8;
const TUint8 KSoiEnd = 0xd8;
const TUint16 KSoiRev = 0xd8ff;
const TUint16 KEoi = 0xffd9;
const TUint16 KApp1Rev = 0xe1ff;
const TUint8 KApp1End = 0xe1;
const TUint16 KDqt = 0xffdb;
const TUint16 KDht = 0xffc4;
const TUint16 KDri = 0xffdd;
const TUint16 KSof0 = 0xffc0;

// Exif header related
const TUint16 KLittleEndian = 0x4949;
const TUint16 KBigEndian = 0x4d4d;
const TUint16 KExifDummy = 0x2a00;
const TUint16 KExifDummyRev = 0x002a;
const TUint32 KExifIdentifierRev = 0x66697845;
const TUint8 KExifPad = 0x00;
const TUint32 KHeaderOffset = 0x00000008;

// Masks
const TUint32 KThreeByteMask = 0x00ffffff;
const TUint32 KTwoByteMask = 0x0000ffff;
const TUint32 KOneByteMask = 0x000000ff;

const TUint KMaxApp1Size = 65535;
const TUint KIfdNo = 5;

const TInt KNoIfd0Tags = 19;
const TInt KNoMandatoryIfd0Tags = 5;
const TInt KNoIfdExifTags = 57;
const TInt KNoMandatoryIfdExifTags = 6;
const TInt KNoIfd1Tags = 22;
const TInt KNoMandatoryIfd1Tags = 6;
const TInt KNoIfdGpsTags = 31;
const TInt KNoMandatoryIfdGpsTags = 1;
const TInt KNoIfdIntOpTags = 5;
const TInt KNoMandatoryIfdIntOpTags = 0;

const TUint KAny = 0;

const TReferenceTag ifd0Tags[KNoIfd0Tags] = 
    {
    { 0x011A, CExifTag::ETagRational, 1 }, 
    { 0x011B, CExifTag::ETagRational, 1 }, 
    { 0x0128, CExifTag::ETagShort, 1 }, 
    { 0x0213, CExifTag::ETagShort, 1 }, 
    { 0x8769, CExifTag::ETagLong, 1 }, 
    { 0x0110, CExifTag::ETagAscii, KAny }, 
    { 0x0112, CExifTag::ETagShort, 1 }, 
    { 0x012D, CExifTag::ETagShort, 768 }, 
    { 0x0131, CExifTag::ETagAscii, KAny }, 
    { 0x0132, CExifTag::ETagAscii, 20 }, 
    { 0x013B, CExifTag::ETagAscii, KAny }, 
    { 0x013E, CExifTag::ETagRational, 2 }, 
    { 0x013F, CExifTag::ETagRational, 6 }, 
    { 0x0211, CExifTag::ETagRational, 3 }, 
    { 0x010E, CExifTag::ETagAscii, KAny }, 
    { 0x0214, CExifTag::ETagRational, 6 }, 
    { 0x8298, CExifTag::ETagAscii, KAny }, 
    { 0x8825, CExifTag::ETagLong, 1 }, 
    { 0x010F, CExifTag::ETagAscii, KAny }
    };

const TReferenceTag ifdExifTags[KNoIfdExifTags] =
    {
    { 0x9000, CExifTag::ETagUndefined, 4 },
    { 0x9101, CExifTag::ETagUndefined, 4 },
    { 0xA000, CExifTag::ETagUndefined, 4 },
    { 0xA001, CExifTag::ETagShort, 1 },
    { 0xA002, TReferenceTag::ETagLongOrShort, 1 },
    { 0xA003, TReferenceTag::ETagLongOrShort, 1 },
    { 0x8828, CExifTag::ETagUndefined, KAny },
    { 0x9003, CExifTag::ETagAscii, 20 },
    { 0x9004, CExifTag::ETagAscii, 20 },
    { 0x8824, CExifTag::ETagAscii, KAny },
    { 0x9102, CExifTag::ETagRational, 1 },
    { 0x9201, CExifTag::ETagSrational, 1 },
    { 0x9202, CExifTag::ETagRational, 1 },
    { 0x9203, CExifTag::ETagSrational, 1 },
    { 0x9204, CExifTag::ETagSrational, 1 },
    { 0x9205, CExifTag::ETagRational, 1 },
    { 0x9206, CExifTag::ETagRational, 1 },
    { 0x9207, CExifTag::ETagShort, 1 },
    { 0x9208, CExifTag::ETagShort, 1 },
    { 0x9209, CExifTag::ETagShort, 1 },
    { 0x920A, CExifTag::ETagRational, 1 },
    { 0x9214, CExifTag::ETagShort, KAny },
    { 0x927C, CExifTag::ETagUndefined, KAny },
    { 0x9286, CExifTag::ETagUndefined, KAny },
    { 0x9290, CExifTag::ETagAscii, KAny },
    { 0x9291, CExifTag::ETagAscii, KAny },
    { 0x9292, CExifTag::ETagAscii, KAny },
    { 0x829A, CExifTag::ETagRational, 1 },
    { 0x829d, CExifTag::ETagRational, 1 },
    { 0x8822, CExifTag::ETagShort, 1 },
    { 0xA004, CExifTag::ETagAscii, 13 },
    { 0xA005, CExifTag::ETagLong, 1 },
    { 0xA20B, CExifTag::ETagRational, 1 },
    { 0xA20C, CExifTag::ETagUndefined, KAny },
    { 0x8827, CExifTag::ETagShort, KAny },
    { 0xA20E, CExifTag::ETagRational, 1 },
    { 0xA20F, CExifTag::ETagRational, 1 },
    { 0xA210, CExifTag::ETagShort, 1 },
    { 0xA214, CExifTag::ETagShort, 2 },
    { 0xA215, CExifTag::ETagRational, 1 },
    { 0xA217, CExifTag::ETagShort, 1 },
    { 0xA300, CExifTag::ETagUndefined, 1 },
    { 0xA301, CExifTag::ETagUndefined, 1 },
    { 0xA302, CExifTag::ETagUndefined, KAny },
    { 0xA401, CExifTag::ETagShort, 1 },
    { 0xA402, CExifTag::ETagShort, 1 },
    { 0xA403, CExifTag::ETagShort, 1 },
    { 0xA404, CExifTag::ETagRational, 1 },
    { 0xA405, CExifTag::ETagShort, 1 },
    { 0xA406, CExifTag::ETagShort, 1 },
    { 0xA407, CExifTag::ETagShort, 1 },
    { 0xA408, CExifTag::ETagShort, 1 },
    { 0xA409, CExifTag::ETagShort, 1 },
    { 0xA40A, CExifTag::ETagShort, 1 },
    { 0xA40B, CExifTag::ETagUndefined, KAny },
    { 0xA40C, CExifTag::ETagShort, 1 },
    { 0xA420, CExifTag::ETagAscii, 33 }
    };

const TReferenceTag ifd1Tags[KNoIfd1Tags] =
    {
    { 0x0103, CExifTag::ETagShort, 1 }, 
    { 0x011A, CExifTag::ETagRational, 1 }, 
    { 0x011B, CExifTag::ETagRational, 1 }, 
    { 0x0128, CExifTag::ETagShort, 1 }, 
    { 0x0201, CExifTag::ETagLong, 1 },
    { 0x0202, CExifTag::ETagLong, 1 },
    { 0x0112, CExifTag::ETagShort, 1 },
    { 0x012D, CExifTag::ETagShort, 768 },
    { 0x010F, CExifTag::ETagAscii, KAny },
    { 0x0110, CExifTag::ETagAscii, KAny },
    { 0x0131, CExifTag::ETagAscii, KAny },
    { 0x0132, CExifTag::ETagAscii, 20 },
    { 0x013B, CExifTag::ETagAscii, KAny },
    { 0x013E, CExifTag::ETagRational, 2 },
    { 0x013F, CExifTag::ETagRational, 6 },
    { 0x0211, CExifTag::ETagRational, 3 },
    { 0x0213, CExifTag::ETagShort, 1 },
    { 0x0214, CExifTag::ETagRational, 6 },
    { 0x010E, CExifTag::ETagAscii, KAny },
    { 0x8298, CExifTag::ETagAscii, KAny },
    { 0x8769, CExifTag::ETagLong, 1 },
    { 0x8825, CExifTag::ETagLong, 1 }
    };

const TReferenceTag ifdGpsTags[KNoIfdGpsTags] =
    {
    { 0x0000, CExifTag::ETagByte, 4 }, 
    { 0x0001, CExifTag::ETagAscii, 2 },
    { 0x0002, CExifTag::ETagRational, 3 }, 
    { 0x0003, CExifTag::ETagAscii, 2 },
    { 0x0004, CExifTag::ETagRational, 3 },
    { 0x0005, CExifTag::ETagByte, 1 },
    { 0x0006, CExifTag::ETagRational, 1 },
    { 0x0007, CExifTag::ETagRational, 3 },
    { 0x0008, CExifTag::ETagAscii, KAny },
    { 0x0009, CExifTag::ETagAscii, 2 },
    { 0x000A, CExifTag::ETagAscii, 2 },
    { 0x000B, CExifTag::ETagRational, 1 },
    { 0x000C, CExifTag::ETagAscii, 2 },
    { 0x000D, CExifTag::ETagRational, 1 },
    { 0x000E, CExifTag::ETagAscii, 2 },
    { 0x000F, CExifTag::ETagRational, 1 },
    { 0x0010, CExifTag::ETagAscii, 2 },
    { 0x0011, CExifTag::ETagRational, 1 },
    { 0x0012, CExifTag::ETagAscii, KAny },
    { 0x0013, CExifTag::ETagAscii, 2 },
    { 0x0014, CExifTag::ETagRational, 3 },
    { 0x0015, CExifTag::ETagAscii, 2 },
    { 0x0016, CExifTag::ETagRational, 3 },
    { 0x0017, CExifTag::ETagAscii, 2 },
    { 0x0018, CExifTag::ETagRational, 1 },
    { 0x0019, CExifTag::ETagAscii, 2 },
    { 0x001A, CExifTag::ETagRational, 1 },
    { 0x001B, CExifTag::ETagUndefined, KAny },
    { 0x001C, CExifTag::ETagUndefined, KAny },
    { 0x001D, CExifTag::ETagAscii, 11 },
    { 0x001E, CExifTag::ETagShort, 1 }
    };

const TReferenceTag ifdIntOpTags[KNoIfdIntOpTags] = 
    { 
    { 0x0001, CExifTag::ETagAscii, KAny },
    { 0x0002, CExifTag::ETagUndefined, 4 },
    { 0x1000, CExifTag::ETagAscii, KAny },
    { 0x1001, TReferenceTag::ETagLongOrShort, 1 },
    { 0x1002, TReferenceTag::ETagLongOrShort, 1 }
    };

// MACROS

// Logging support
// Serial port logging is enabled in _DEBUG builds by default
// USE_FILE_LOGGER or USE_SERIAL_LOGGER can also be explicitely defined e.g. in MMP file.
//

// #define USE_FILE_LOGGER

#if defined (_DEBUG) && !defined(USE_FILE_LOGGER)                   
    #define USE_SERIAL_LOGGER 
#endif
                                    
#if defined (USE_FILE_LOGGER)
    #include <flogger.h>
    _LIT(KLogFile,"exiflib.txt");
    _LIT(KLogFolder,"exiflib");
    #define LOGTEXT(AAA)                RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend,AAA)
    #define LOGTEXT2(AAA,BBB)           RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB)
    #define LOGTEXT3(AAA,BBB,CC)        RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB,CC)
    #define LOGTEXT4(AAA,BBB,CC,DDD)    RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(AAA),BBB,CC,DDD)
    #define LOGHEXDUMP(AAA,BBB,CC,DDD)  RFileLogger::HexDump(KLogFolder(),KLogFile(),EFileLoggingModeAppend,AAA,BBB,CC,DDD)    
#elif defined (USE_SERIAL_LOGGER) 
    #include <e32svr.h>
    #define LOGTEXT(AAA)                RDebug::Print(AAA)
    #define LOGTEXT2(AAA,BBB)           RDebug::Print(AAA,BBB)
    #define LOGTEXT3(AAA,BBB,CC)        RDebug::Print(AAA,BBB,CC)
    #define LOGTEXT4(AAA,BBB,CC,DDD)    RDebug::Print(AAA,BBB,CC,DDD)
    #define LOGHEXDUMP(AAA,BBB,CC,DDD)
#else
    #define LOGTEXT(AAA)                
    #define LOGTEXT2(AAA,BBB)           
    #define LOGTEXT3(AAA,BBB,CC)       
    #define LOGTEXT4(AAA,BBB,CC,DDD)       
    #define LOGHEXDUMP(AAA,BBB,CC,DDD) 
#endif                              

// CLASS DECLARATION

/**
*  Internal common utility class.
*  Provides commonly used functions to other internal service classes.
*
*  @lib ExifLib
*  @since 2.6 
*/
class TExifCommon
    {
    public: // New functions

        /**
        * Puts the given 32-bit unsigned integer to the 4-byte location, 
        * whose start point is specified with the given 8-bit unsigned integer 
        * pointer.
        * @since 2.6
        * @param aPtr 8-bit unsigned integer pointer, locating where the 
        *             32-bit unsigned integer is written.
        * @param aUint32 32-bit unsigned integer that is written.
        * @return void
        */
        static TInt SetUint32( TUint8* aPtr, TUint32 aUint32 );

        /**
        * Puts the given 16-bit unsigned integer to the 2-byte location, whose 
        * start point is specified with the given 8-bit unsigned integer pointer
        * @since 2.6
        * @param aPtr 8-bit unsigned integer pointer, locating where the 16-bit 
        *             unsigned integer is written.
        * @param aUint16 16-bit unsigned integer that is written.
        * @return void
        */
        static TInt SetUint16( TUint8* aPtr, TUint16 aUint16 );

        /**
        * Returns a 32-bit unsigned integer from the location, whose start point 
        * is specified with the given 8-bit unsigned integer pointer. 
        * @since 2.6
        * @param aPtr 8-bit unsigned integer pointer, locating where the 32-bit 
        *             unsigned integer is read.
        * @return 32-bit unsigned integer that is read from the specified 
        * location
        */
        static TUint32 Uint32L( TUint8* aPtr );

        /**
        * Returns a 16-bit unsigned integer from the location, whose start point 
        * is specified with the given 8-bit unsigned integer pointer. 
        * @since 2.6
        * @param aPtr 8-bit unsigned integer pointer, locating where the 16-bit
        *             unsigned integer is read.
        * @return 16-bit unsigned integer that is read from the specified 
        * location
        */
        static TUint16 Uint16L( TUint8* aPtr );

        /**
        * Returns a 32-bit signed integer from the location, whose start point 
        * is specified with the given 8-bit unsigned integer pointer. 
        * @since 2.6
        * @param aPtr 8-bit unsigned integer pointer, locating where the 32-bit 
        *             signed integer is read.
        * @return 32-bit signed integer that is read from the specified location
        */
        static TInt32 Int32L( TUint8* aPtr );

        /**
        * Checks if the given 16-bit unsigned integer is one of the defined tag 
        * IDs
        * @since 2.6
        * @param aTagId 16-bit unsigned integer that is checked for validity
        * @return Boolean stating whether the tag is valid or not.
        */
        static TBool IsValidTagId( TUint16 aTagId );

        /**
        * Locates the pointer to the first specified Jpeg Marker from the 
        * begining
        * inside the given interval
        * @since 2.6
        * @param aMarker The Jpeg marker that is searched for.
        * @param aStartPtr The start of the interval
        * @param aEndPtr The end of the interval
        * @return The pointer to the marker. NULL if not found.
        */
        static TUint8* LocateJpegMarkerPtr( 
            TUint16 aMarker, 
            TUint8* aStartPtr, 
            TUint8* aEndPtr);

        /**
        * Locates the pointer to the first specified Jpeg Marker from the end
        * inside the given interval
        * @since 2.6
        * @param aMarker The Jpeg marker that is searched for.
        * @param aStartPtr The start of the interval
        * @param aEndPtr The end of the interval
        * @return The pointer to the marker. NULL if not found.
        */
        static TUint8* LocateJpegMarkerPtrFromEnd( 
            TUint16 aMarker, 
            TUint8* aStartPtr, 
            TUint8* aEndPtr);
    };

#endif // EXIFCOMMON_H

// End of File
