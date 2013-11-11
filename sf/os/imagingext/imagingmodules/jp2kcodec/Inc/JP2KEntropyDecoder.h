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
* Description:  CJ2kEntropyDecoder class used to perform entropy decoding
*                on the codebloc.
*
*/


#ifndef __JP2KENTROPYDECODER_H__
#define __JP2KENTROPYDECODER_H__

//  INCLUDES
#include <e32base.h>
#include "JP2KUtils.h"

// CONSTANTS
const TUint8 KSignShift = (TUint8)( KImplementationPrecision - 1 );

// Flags for positions of the contexts of the state of a pixel
const TUint8 KPositionSignificant = 14;
const TUint8 KPositionVisited     = 13;
const TUint8 KPositionHorNeg      = 12;
const TUint8 KPositionHorPos      = 11;
const TUint8 KPositionVerNeg      = 10;
const TUint8 KPositionVerPos      = 9;
const TUint8 KPositionMR          = 8;
const TUint8 KPositionLeft        = 7;
const TUint8 KPositionRight       = 6;
const TUint8 KPositionUp          = 5;
const TUint8 KPositionDown        = 4;
const TUint8 KPositionUpperLeft   = 3;
const TUint8 KPositionUpperRight  = 2;
const TUint8 KPositionLowerLeft   = 1;
const TUint8 KPositionLowerRight  = 0;

// Flags for the values of the contexts of the state of a pixel
const TInt16 KStateSignificant    = (TInt16)( 1 << KPositionSignificant );
const TInt16 KStateVisited        = (TInt16)( 1 << KPositionVisited );
const TInt16 KStateHorNegative    = (TInt16)( 1 << KPositionHorNeg );
const TInt16 KStateHorPositive    = (TInt16)( 1 << KPositionHorPos );
const TInt16 KStateVerNegative    = (TInt16)( 1 << KPositionVerNeg );
const TInt16 KStateVerPositive    = (TInt16)( 1 << KPositionVerPos );
const TInt16 KStatePreviousMR     = (TInt16)( 1 << KPositionMR );
const TInt16 KStateLeft           = (TInt16)( 1 << KPositionLeft );
const TInt16 KStateRight          = (TInt16)( 1 << KPositionRight );
const TInt16 KStateUp             = (TInt16)( 1 << KPositionUp );
const TInt16 KStateDown           = (TInt16)( 1 << KPositionDown );
const TInt16 KStateUpperLeft      = (TInt16)( 1 << KPositionUpperLeft );
const TInt16 KStateUpperRight     = (TInt16)( 1 << KPositionUpperRight );
const TInt16 KStateLowerLeft      = (TInt16)( 1 << KPositionLowerLeft );
const TInt16 KStateLowerRight     = (TInt16)( 1 << KPositionLowerRight );

const TInt16 KZcMask              = (TInt16)( ( 1 << 8 ) - 1 ); // First eight bits of states
const TUint8 KPredictionBit       = 6;
const TUint8 KScShift             = 9;
const TUint8 KScLutMask           = (TUint8)( ( 1 << 4 ) - 1 ); // First four bits of states 
const TUint8 KZcLutBits           = 8;
const TUint8 KLutSize             = 16;
const TUint8 KUniformContext      = 0;
const TUint8 KRlcContext          = 1;
const TUint8 KSegmentationMarker         = 10;   // Segmentation markers with one int
const TUint8 KErrorResilienceTermination = 0x55; // Error resilience prediction termination.
const TUint8 KStripeHeight            = 4;

// MQ related parameters
const TUint8 KNumberContexts          = 19;
const TUint8 KNumberOriginalMQEntries = 47;

// Coding pass related parameters
const TUint8 KFirstLazyPassIndex    = 9;
const TUint8 KFirstBypassTermIndex  = 10;  // Pass which is terminated first if AC bypass and no termination 

// MACROS

// DATA TYPES

// Entropy coding parameters: flags for arithmetic bypass, context
// resetting, termination, vertical stripes, prediction termination
// and segment symbols.
enum TEntropyErrorState
    {
    ENoError = 0, 
    EEntropyCodingError, 
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class J2KMQCoder;
class CJ2kEntropyDecoder;
class CJ2kImageInfo;
class CJ2kCodeBlock;
class CJ2kSynthesis;

// CLASS DECLARATION

/**
 * J2KEntropyStream class is used to handle 
 * the buffer of encoded bytes as well as 
 * reading bits and bytes from it.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class J2KEntropyStream
{

    public:  // Constructors and destructor
        
    public: // New functions

        /**
        * Read a byte from the input stream
        * @since 2.6
        * @return TUint8: a byte from the input stream.
        */
        TUint8 ReadByteFromStream();

        /**
        * Read a bit from the input stream
        * @since 2.6
        * @return TUint8: a bit from the input stream.
        */
        TUint8 ReadBitFromStream();

        /**
        * Check a prediction termination for raw coding
        * @since 2.6
        * @return TEntropyErrorState: ENoError if success.
        */
        TEntropyErrorState CheckPrediction();

        /**
        * Reset the input stream
        * @since 2.6
        */
        void ResetInputStream();

    public: // Functions from base classes
        
    protected:  // New functions       

    protected:  // Functions from base classes

    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data
        
        // The size of the buffer
        TInt32 iNumBytes;      

        // Position of the next byte in the buffer
        TInt32 iPosition;      

        // Buffer for bytes 
        TUint8* iBuffer;       
        
        // Buffer for bit-output 
        TUint8 iTinyBuffer;    

        // Flag for delaying the input of 0xFF byte
        TUint8 iDelayedFF;     

        // Position of the next bit in the the tiny_buffer
        TInt8 iTinyBufferPos; 

    public:     // Friend classes

        friend class J2KMQCoder;
        friend class CJ2kEntropyDecoder;

    protected:  // Friend classes
        
    private:    // Friend classes
};

/**
 * J2KEntropyStates class is used to handle 
 * the states and state transition information
 * for the MQ entropy decoding.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class J2KEntropyStates : public CBase
    {
    public:  // Constructors and destructor
        
    public: // New functions
        
    public: // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data

        // Pointer to the next mps state 
        J2KEntropyStates* iNextMPS;    

        // Pointer to the next lps state 
        J2KEntropyStates* iNextLPS;    

        // Current estimate for lps symbol probability
        TUint32 iQe;        
        
        // Flag for switching the mps (0->1 or 1->0)
        TInt32 iSwitchFlag; 

    public:     // Friend classes
        friend class J2KMQCoder;
    protected:  // Friend classes

    private:    // Friend classes

    };

/**
 * J2KEntropyContexts class is used to handle 
 * the context information for the MQ entropy 
 * decoding.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class J2KEntropyContexts : public CBase
    {
    public:  // Constructors and destructor
        
    public: // New functions
        
    public: // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data

        // Entropy state.
        J2KEntropyStates* iState;
        
        // Most probable symbol
        TInt32 iMPS;

    public:     // Friend classes
        friend class J2KMQCoder;
    protected:  // Friend classes

    private:    // Friend classes

    };

/**
 * J2KMQCoder class is used to perform
 * the actual MQ entropy decoding.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class J2KMQCoder : public CBase
    {
    public:  // Constructors and destructor
                
        /**
        * Destructor.
        */
        virtual ~J2KMQCoder();

    public: // New functions

        /**
        * Read a byte from the input stream
        * @since 2.6
        */
        void MqByteIn();

        /**
        * Decode the symbol
        * @since 2.6
        * @param aContextIndex: the context index.
        * @return TUint8: a decision.
        */
        TUint8 MqDecodeSymbol( TInt32 aContextIndex );

        /**
        * Initialize the MQCoder
        * @since 2.6
        */
        void MqInitDecoder();

        /**
        * Check the prediction termination
        * @since 2.6
        * @return TEntropyErrorState: ENoError if success.
        */
        TEntropyErrorState MqCheckPrediction();

        /**
        * Reset the MQCoder context list to the original state
        * @since 2.6
        */
        void ResetMqContexts();

        /**
        * Initialze MQCoder and reset the context
        * @since 2.6
        * @param aSegmentLength: the segment length.
        */
        void ResetMQDecoder( TInt32 aSegmentLength );

        /**
        * Initialize MQCoder original states table
        * @since 2.6
        */
        void InitializeOrigMqTable();

    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * Renormalize
        * @since 2.6
        */
        void ReNormalize();

        /**
        * MPS exchange
        * @since 2.6
        * @param aD: a reference to TInt32.
        */
        void MpsExchange( TInt32& aD );
        /**
        * LPS exchange
        * @since 2.6
        * @param aD: a reference to TInt32.
        */
        void LpsExchange( TInt32& aD );

    public:     // Data
    
    protected:  // Data

    private:    // Data

        // Original states table
        RPointerArray<J2KEntropyStates> iOriginalStates;

        // An array of contexts
        RPointerArray<J2KEntropyContexts> iContextList;   

        // Context corresponding to the current context index
        J2KEntropyContexts* iCurrentContext;              

        // State of the current context
        J2KEntropyStates* iCurrentState;                

        // Input stream
        J2KEntropyStream iInputStream;                 

        // The C register, i.e. the code register
        TUint32 iC;        

        // The A register, i.e. the interval register
        TUint32 iA;        

        // The counter
        TUint32 iCT;       

        // Buffer for the output byte 
        TUint8  iB;        

        // Current marker
        TUint8  iMarker;

    public:     // Friend classes
        friend class CJ2kEntropyDecoder;

    protected:  // Friend classes

    private:    // Friend classes
    };

/**
 * CJ2kEntropyDecoder class is used to perform
 * the initializations and coding passes of 
 * the entropy decoding process.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class CJ2kEntropyDecoder : public CBase  
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CJ2kEntropyDecoder* NewL( CJ2kImageInfo& aImageInfo );
        
        /**
        * Destructor.
        */
        virtual ~CJ2kEntropyDecoder();

    public: // New functions
        
        /**
        * Set the size of internal buffer and other control data
        * @since 2.6
        * @param aSize: the size of the internal buffer.
        */
        void SetNewSizeL( const TSize& aSize );

        /**
        * Set the current pointer to point to the right LUT 
        * depending on the current subband
        * @since 2.6
        * @param aBandIndex: the subband index.
        */
        void SetCurrentZCLUT( TUint8 aBandIndex );

        /**
        * Decode the coded codeblock
        * @since 2.6
        * @param aCodeblock: a reference to CJ2kCodeBlock object.
        * @param aCblkStyle: the codeblock style.
        * @param aMagBits: the magniture bits.
        */
        void DecodeCodeblock( CJ2kCodeBlock& aCodeblock, TUint8 aCblkStyle, TUint8 aMagBits );

    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CJ2kImageInfo& aImageInfo );

        /**
        * Initialize ZC lookup table
        * @since 2.6
        */
        void InitializeZCLut();

        /**
        * Initialize SC/MR lookup table
        * @since 2.6
        */
        void InitializeScMrLut();

        /**
        * Get the casual
        * @since 2.6
        * @param aDataCol: a reference to the pointer of data column.
        * @param aStateCol: a reference to the pointer of state column.
        */
        void GetCausal( TPrecInt*& aDataCol, TInt16*& aStateCol );

        /**
        * Decode the significance bit
        * @since 2.6
        * @param aDataValue: a reference to the pointer of data value.
        * @param aStateValue: a reference to the pointer of state value.
        * @param aMask: the mask.
        */
        void DecodeSignificance( TPrecInt*& aDataValue, TInt16*& aStateValue, TPrecInt aMask );

        /**
        * Decode the lazy significance bit
        * @since 2.6
        * @param aDataValue: a reference to the pointer of data value.
        * @param aStateValue: a reference to the pointer of state value.
        * @param aMask: the mask.
        */
        void DecodeRawSignificance( TPrecInt*& aDataValue, TInt16*& aStateValue, TPrecInt aMask );

        /**
        * Decode the refinement bit
        * @since 2.6
        * @param aDataValue: a reference to the pointer of data value.
        * @param aStateValue: a reference to the pointer of state value.
        * @param aMask: the mask.
        * @param aResetMask: the reset mask.
        */
        void DecodeRefinement( TPrecInt*& aDataValue, TInt16*& aStateValue, TPrecInt aMask, TInt32 aResetMask );

        /**
        * Decode the lazy refinement bit
        * @since 2.6
        * @param aDataValue: a reference to the pointer of data value.
        * @param aStateValue: a reference to the pointer of state value.
        * @param aMask: the mask.
        * @param aResetMask: the reset mask.
        */
        void DecodeRawRefinement( TPrecInt*& aDataValue, TInt16*& aStateValue, TPrecInt aMask, TInt32 aResetMask );

        /**
        * Perform the normalization
        * @since 2.6
        * @param aDataValue: a reference to the pointer of data value.
        * @param aStateValue: a reference to the pointer of state value.
        * @param aMask: the mask.
        * @param aZCValue: a reference to ZC value.
        */
        void DecodeNormalization( TPrecInt*& aDataValue, TInt16*& aStateValue, TPrecInt aMask, TInt32& aZCValue );

        /**
        * Update the significance
        * @since 2.6
        * @param aStateValue: a reference to the pointer of state value.
        * @param aSymbol: the symbol.
        */
        void UpdateSignificance( TInt16*& aStateValue, TInt32 aSymbol );

        /**
        * Perform the significance pass
        * @since 2.6
        */
        void SignificancePass();

        /**
        * Perform the lazy significance pass
        * @since 2.6
        */
        void LazySignificancePass();

        /**
        * Perform the refinement pass
        * @since 2.6
        */
        void RefinementPass();

        /**
        * Perform the lszy refinement pass
        * @since 2.6
        */
        void LazyRefinementPass();

        /**
        * Perform the cleanup pass
        * @since 2.6
        * @param aSegSymbols: segmentation symbols at the end of coding pass.
        */
        void CleanupPass( TUint8 aSegSymbols );

        /**
        * Get the first state row
        * @since 2.6
        * @return TInt16*: a pointer to the first state row.
        */ 
        const TInt16* GetFirstStateRow() const;

    public:     // Data

    protected:  // Data

    private:    // Data

        // Coding flags
        enum TCodingFlags
            {
            EArithmeticBypass         = 1,
            EResetContext             = 1 << 1,
            ETermination              = 1 << 2,
            EVerticalStripe           = 1 << 3,
            EPredictableTermination   = 1 << 4,
            ESegmentationSymbols      = 1 << 5
            };

        // MQCoder
        J2KMQCoder iMQDecoder;           

        // States.
        TInt16* iStates; 

        // SC lut buffer
        HBufC8* iSCLutBuf;            
    
        // Pointer to SC lut buffer
        TUint8* iSCLut;   
        
        // MR lut buffer
        HBufC8* iMRLutBuf;            

        // Pointer to MR lut buffer
        TUint8* iMRLut;               

        // ZC lut for LL/LH band
        HBufC8* iZcLutLL;    
        
        // ZC lut for HL band
        HBufC8* iZcLutHL;        
        
        // ZC lut for HH band
        HBufC8* iZcLutHH;             

        // Pointer to one of the ZC lut 
        TUint8* iCurrentZcLutPtr;     
    
        // Image max block size
        TSize iCurrentSize;        
        
        // Number of stripes in the current codeblock
        TUint16 iNumStripes;     
        
        // Image max block width
        TUint16 iMaxBlockWidth;       
    
        // Width of data samples in the codeblock 
        TInt32 iBlockDataWidth;          

        // Width of state samples in the codeblock
        TInt32 iStateWidth;              

        // Size of the iStates array 
        TInt32 iStateSize;               

        // Number of data samples in one full stripe 
        TInt32 iDataSamplesPerStripe;    

        // Number of state samples in one full stripe
        TInt32 iStateSamplesPerStripe;   

        // Height of the last stripe in the codeblock
        TInt32 iLastStripeHeight;        

        // Width of the current codeblock
        TInt32 iBlockWidth;              

        // Max bit depth of the image
        TInt32 iMaxBitDepth;             

        // Error state of the Entropy decoder
        TEntropyErrorState iErrorState;    

        // Code block style has Reset Context Probabilities
        TUint8 iResetContexts;             

        // Code block style has Vertical Context Casual
        TUint8 iVerticalCausalContextUsed; 

        // Code block style has Predictable Termination
        TUint8 iPredictableTerminationUsed;

        // Current bitplane
        TUint8 iCurrentBitplane;           

        // Code block style has Termination on each coding pass
        TUint8 iTerminateThisPass;         

        // Internal two dimensional data array
        TPrecInt** iData;       
        
    public:     // Friend classes
        friend class CJ2kSynthesis;

    protected:  // Friend classes
         
    private:    // Friend classes         
    };

#endif // __JP2KENTROPYDECODER_H__
