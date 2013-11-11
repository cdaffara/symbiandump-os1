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
* Description:  Collection of structs used to store and represent
*                the metadata in the JP2 file format.
*
*/


#ifndef __JP2KFORMAT_H__
#define __JP2KFORMAT_H__

//  INCLUDES
#include <icl/imageprocessor.h>

// CONSTANTS

// BlockSizeInBytes returns this value.
const TInt KJ2kInputBufferSize = 8192;

// JPEG 2000 signature box information
const TUint32 KJ2kSigBoxLength  = ( (TUint32) 0x0000000c );
const TUint32 KJ2kSigBoxType    = ( (TUint32) 0x6a502020 );
const TUint32 KJ2kSigBoxContent = ( (TUint32) 0x0d0a870a );

// Length for both box length and type fields together
const TUint32 KJ2kBoxTypeLength = ( (TUint32)8 );

// JPEG 2000 signaturebox length + next box type and length fields
const TInt KJ2kFileInformationSize = KJ2kSigBoxLength + KJ2kBoxTypeLength;

// File type box
const TUint32 KJ2kFileTypeBox      = ( (TUint32) 0x66747970 );
const TUint32 KJ2kFileTypeBrand    = ( (TUint32) 0x6a703220 );
const TUint32 KJ2kFileTypeMinV     = ( (TUint32) 0x00000000 );
const TUint32 KJ2kFileTypeProfile0 = ( (TUint32) 0x4a325030 );
const TUint32 KJ2kFileTypeProfile1 = ( (TUint32) 0x4a325031 );
// Restrict the file type box's length to be 1024, this is more than enough 
// and helps detect corrupted codestreams
const TUint32 KJ2kFileTypeBoxMaxLength = ( (TUint32) 1024);

// JP2 Header Box
const TUint32 KJ2kJP2HeaderBoxType = ( (TUint32) 0x6a703268 );

// Image Header Box(JP2 Header Box's sub-box) type and length in bytes
const TUint32 KJ2kImageHeaderBoxType   = ( (TUint32) 0x69686472 );
const TUint32 KJ2kImageHeaderBoxLength = ( (TUint32) 22 );
const TUint8  KJ2kImageHeaderCompressionType = ( (TUint8)7 );

// Bits Per Component Box(JP2 Header Box's sub-box) type and length
const TUint32 KJ2kBitsPerCompBoxType  = ( (TUint32) 0x62706363 );
const TUint8  KJ2kIsBPCBoxExist       = ( (TUint8) 0xff );

// Colour Specification Box(JP2 Header Box's sub-box) type and length
const TUint32 KJ2kColourSpecBoxType   = ( (TUint32) 0x636f6c72 );
const TUint32 KJ2kColourSpecGrayScale = ( (TUint32) 17 );

// Palette Box(JP2 Header Box's sub-box) type and length
const TUint32 KJ2kPaletterBoxType     = ( (TUint32) 0x70636c72 );

// Component Mapping Box(JP2 Header Box's sub-box) type and length
const TUint32 KJ2kComponentMapBoxType = ( (TUint32) 0x636d6170 );

// Channel Definition Box(JP2 Header Box's sub-box) type and length
const TUint32 KJ2kChannelDefBoxType   = ( (TUint32) 0x63646566 );

// Resolution Box(JP2 Header Box's sub-box) type and length
const TUint32 KJ2kResolutionBoxType = ( (TUint32) 0x72657320 );
const TUint32 KJ2kCaptureResBoxType = ( (TUint32) 0x72657363 );
const TUint32 KJ2kDisplayResBoxType = ( (TUint32) 0x72657364 );
const TUint32 KJ2kResSubBoxLength   = ( (TUint32)18 );

// Contiguous Codestream Box type
const TUint32 KJ2kCodestreamBoxType = ( (TUint32) 0x6a703263 );
const TUint32 KJ2kIPRBoxType        = ( (TUint32) 0x6a703269 );
const TUint32 KJ2kXMLBoxType        = ( (TUint32) 0x786d6c20 );
const TUint32 KJ2kUUIDBoxType       = ( (TUint32) 0x75756964 );
const TUint32 KJ2kUUIDInfoBoxType   = ( (TUint32) 0x75696e66 );
const TUint32 KJ2kUUIDListBoxType   = ( (TUint32) 0x756c7374 );
const TUint32 KJ2kUUIDUrlBoxType    = ( (TUint32) 0x75726c20 );

// Reader Requirements Box
const TUint32 KJ2kReaderReqBox = ( (TUint32) 0x72726571 );

// JP2 Codestream only - SOC + SIZ marker
const TUint32 KJ2kSOCType           = ( (TUint32) 0xff4fff51 );

// Twip = 1/1440inch.  1/1440*0,0254 meters
const TReal KJ2kTwipM = 0.000017638; //( (TReal)( 1 / 1440.0 ) * 0.0254 );

// Maximum number of palette entries
const TInt KMaxPaletteEntries = 1024;

// Maximum number of components possible
const TInt KMaxComponents = 16384;

// Maximum bitdepth supported
const TInt KMaxBitdepth = 32;


// MACROS

// DATA TYPES

/**
 * Collection of structs used to store and represent
 * the metadata in the JP2 file format.
 *
 * JP2KCodec.dll
 * @since 7.0
 */
struct TPalette
    {
    /**
    * Destructor
    * @since 7.0
    */
    ~TPalette();

    // Bi list
    RArray<TUint> iBList;    

    // Cij matrix
    RPointerArray<RArray<TUint> > iC2DArray; 
    };

struct TComponentMap
    {
    /**
    * Constructor
    * @since 7.0
    * @param aCmp: the CMP field.
    * @param aMtyp: the MTYP field.
    * @param aPcol: the PCOL field.
    */
    TComponentMap( TUint16 aCmp, TUint8 aMtyp, TUint8 aPcol );

    // CMP field
    TUint16 iCmp;  

    // MTYP field
    TUint8  iMtyp; 

    // PCOL field
    TUint8  iPcol; 
    };

struct TChannelDefinition
    {
    /**
    * Constructor
    * @since 7.0
    * @param aCn: the Cn field.
    * @param aTyp: the Typ field.
    * @param aAsoc: the Asoc field.
    */
    TChannelDefinition( TUint16 aCn, TUint16 aTyp, TUint16 aAsoc );

    // Cn field
    TUint16 iCn;
    
    // Typ field
    TUint16 iTyp;  

    // Asoc field
    TUint16 iAsoc; 
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * TJ2kInfo class is used to gather and store the 
 * JP2 file format information
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class TJ2kInfo
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TJ2kInfo();         

        /**
        * Destructor.
        */
        ~TJ2kInfo();

    public: // New functions
        
    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:        

    public:     // Data
        
        // Flags to indicate the existance of some parameters        
        enum TOption
            {
            EIPR      =  0x01, // To indicate that IPR Box exists //lint !e769 will be referenced in next release.
            EProfile0 =  0x02, // To indicate the JP2 is restricted to Profile 0
            EProfile1 =  0x04, // To indicate the JP2 is restricted to Profile 1
            EJP2file  =  0x08, // To indicate that JP2 file format exists
            EJP2Header=  0x10, // To indicate that JP2 Header processed
            EColorSpec=  0x20  // To indicate that Colour Spec Box exists
            };
    
        // Image size
        TSize iImageSize;        

        // Number of components
        TUint16 iNC;               

        // Bits per component
        TUint8 iBPC;              

        // Bit vector of flags
        TUint8 iOption;           

        // Length of the first codestream box
        TUint32 iCSBoxLength;      

        // Colour spec
        TUint32 iEnumCS;         
        
        // Start position of codestream in file
        TUint32 iCSOffset;         

        // Display resolution
        TSize iImageSizeInTwips; 
    
        // Bits per component list
        RArray<TUint> iBPCList;    

        // ICC Profile
        HBufC8 *iICCProfile; 

        // Component Mapping list
        RArray<TComponentMap> iCMPList;    

        // Channel Definition list
        RArray<TChannelDefinition> iCNList;    
        
        // Palette
        TPalette iPalette;   

        // Intellectual Property box
        RPointerArray<HBufC8> iIPRList; 

        // XML box
        RPointerArray<HBufC8> iXMLList;

        // UUID box
        RPointerArray<HBufC8> iUUIDList;

        // UUIDInfo List box
        RPointerArray<HBufC8> iUUIDInfoListList;

        // UUIDInfo Data Entry Url box
        RPointerArray<HBufC8> iUUIDInfoUrlList;

    protected:  // Data

    private:    // Data

    public:     // Friend classes
  
    protected:  // Friend classes
      
    private:    // Friend classes
    };

#endif // __JP2KFORMAT_H__

