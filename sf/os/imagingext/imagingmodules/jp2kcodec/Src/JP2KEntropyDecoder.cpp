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
* Description:  Entropy decoder functionality..
*
*/


// INCLUDE FILES
#include "JP2KCodeBlock.h"
#include "JP2KTileInfo.h"
#include "JP2KImageInfo.h"
#include "JP2KEntropyDecoder.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// Mq decoder state information, all of the 47 states contain
// the probability estimate of LPS in hexadecimal, the next
// mps state index, the next lps state index and the flag
// indicating a mps switch.
const TUint32 KMqQeStates[KNumberOriginalMQEntries] =  {0x5601,0x3401,0x1801,0x0ac1,0x0521,0x0221,0x5601,
                                                        0x5401,0x4801,0x3801,0x3001,0x2401,0x1c01,0x1601,
                                                        0x5601,0x5401,0x5101,0x4801,0x3801,0x3401,0x3001,
                                                        0x2801,0x2401,0x2201,0x1c01,0x1801,0x1601,0x1401,
                                                        0x1201,0x1101,0x0ac1,0x09c1,0x08a1,0x0521,0x0441,
                                                        0x02a1,0x0221,0x0141,0x0111,0x0085,0x0049,0x0025,
                                                        0x0015,0x0009,0x0005,0x0001,0x5601};

const TInt32 KMqMpsStates[KNumberOriginalMQEntries] =  {1,2,3,4,5,38,7,8,9,10,11,12,13,29,15,16,17,18,19,
                                                        20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,
                                                        36,37,38,39,40,41,42,43,44,45,45,46};

const TInt32 KMqLpsStates[KNumberOriginalMQEntries] =  {1,6,9,12,29,33,6,14,14,14,17,18,20,21,14,14,15,16,
                                                        17,18,19,19,20,21,22,23,24,25,26,27,28,29,30,31,
                                                        32,33,34,35,36,37,38,39,40,41,42,43,46};

const TInt32 KMqSwitchFlagStates[KNumberOriginalMQEntries] =  {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,
                                                               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                               0,0,0,0,0,0,0};

// MQCoder initial states
const TInt32 KMqInitStates[KNumberContexts] = {46,3,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// J2KEntropyStream::ReadByteFromStream
// Read a byte from the input stream
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
TUint8 J2KEntropyStream::ReadByteFromStream()
    {
    if ( iPosition < iNumBytes )
        {
        return ( TUint8 )iBuffer[iPosition++];
        }
    else
        {
        // 0xFF to represent EOF
        return ( 0xFF );   
        }
    }

// -----------------------------------------------------------------------------
// J2KEntropyStream::ReadBitFromStream
// Read a bit from the input stream
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
TUint8 J2KEntropyStream::ReadBitFromStream() 
    {
    if ( iTinyBufferPos == 0 )
        {
        // Input the byte buffer
        iTinyBuffer = ReadByteFromStream();

        if ( iDelayedFF )
            {
            iTinyBufferPos = 7;
            }
        else
            {
            iTinyBufferPos = 8;
            }

        if ( iTinyBuffer != 0xFF ) // No bit-stuffing needed
            {
            iDelayedFF = 0;
            }
        else  // We need to do bit stuffing on next byte
            {
            iDelayedFF = 1;
            }
        }

    return (TUint8)( ( iTinyBuffer >> ( --iTinyBufferPos ) ) & 0x01 );
    }

// -----------------------------------------------------------------------------
// J2KEntropyStream::CheckPrediction
// Check a prediction termination for raw coding
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
TEntropyErrorState J2KEntropyStream::CheckPrediction()
    {
    // Error resilient sequence in last byte
    TInt32 errorResSeq = 0; 
 
    // If tiny buffer is empty and equal to 0xFF, 
    // get the next byte for error resilience marker
    if ( iTinyBufferPos == 0 && iTinyBuffer == 0xFF ) 
        {
        iTinyBuffer = ReadByteFromStream();
        iTinyBufferPos = 7;
        }
  
    // Search for the error resilience marker
    if ( iTinyBufferPos > 0 ) 
        {
        // Get the error resilience sequence
        errorResSeq = iTinyBuffer & ( ( 1 << iTinyBufferPos ) - 1 );

        // Compare the read error resilience marker to the constant 01010101 byte
        if ( errorResSeq != ( KErrorResilienceTermination >> ( 8 - iTinyBufferPos ) ) ) 
            {
            return EEntropyCodingError;
            }
        }
  
    // If we are not at the end of this stream, 
    // next byte should be smaller than 0x80.
    if ( iPosition < iNumBytes )
        {
        if ( iTinyBuffer == 0xFF && iTinyBufferPos == 1 ) 
            {
            if ( ReadByteFromStream() >= 0x80 ) 
                {
                return EEntropyCodingError;
                }
            }
        else 
            {
            if ( ReadByteFromStream() != 0xFF ) 
                {
                return EEntropyCodingError;
                }
            }
        }

    // No error detected
    return ENoError;
    }

// -----------------------------------------------------------------------------
// J2KEntropyStream::ResetInputStream
// Reset the input stream
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void J2KEntropyStream::ResetInputStream()
    {
    // Initialize the stream parameters. Note that stream->position is not reset here,
    // it is reset only when the stream is created for the first time.
    iTinyBuffer = 0;
    iTinyBufferPos = 0;
    iDelayedFF = 0;
    }

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
J2KMQCoder::~J2KMQCoder()
    {
    iOriginalStates.ResetAndDestroy();
    iContextList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// J2KMQCoder::MqByteIn
// Read a byte from the input stream
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void J2KMQCoder::MqByteIn()
    {
    if ( !iMarker )
        {
        if ( iB == 0xff )
            {
            iB = iInputStream.ReadByteFromStream();

            if ( iB > 0x8f )
                {
                iMarker = 1;
                iCT = 8;
                }
            else
                {
                iC += 0xFE00 - ( iB << 9 );
                iCT = 7;
                }
            }
        else
            {
            iB = iInputStream.ReadByteFromStream();
            iC += 0xFF00 - ( iB << 8 );
            iCT = 8;
            }   
        }
    else
        {
        iCT = 8;
        }
    }

// -----------------------------------------------------------------------------
// J2KMQCoder::MqDecodeSymbol
// Decode the symbol
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
TUint8 J2KMQCoder::MqDecodeSymbol( TInt32 aContextIndex )
    {
    // Set the current context to point to the right place in the context list
    iCurrentContext = iContextList[aContextIndex];
    iCurrentState = iCurrentContext->iState;

    iA -= iCurrentState->iQe;

    // Decision returned ( i.e. decoded symbol )
    TInt32 decision = 0;  
    if ( ( iC >> 16 ) < iA )  // Chigh >= A
        {
        MpsExchange( decision );
        }
    else   // lps exchange
        {
        LpsExchange( decision );
        }

    // Return decision 
    return (TUint8)decision;   
    }

// -----------------------------------------------------------------------------
// J2KMQCoder::MqInitDecoder
// Initialize the MQCoder
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void J2KMQCoder::MqInitDecoder()
    {
    iMarker = 0;
    iB = iInputStream.ReadByteFromStream();

    // Software conventions decoder initialization 
    iC = ( iB ^ 0xFF ) << 16;
    MqByteIn();
  
    iC <<= 7;
    iCT -= 7;
    iA = 0x8000;
    }

// -----------------------------------------------------------------------------
// J2KMQCoder::MqCheckPrediction
// Check the prediction termination
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
TEntropyErrorState J2KMQCoder::MqCheckPrediction()
    {
    // If no marker was found, B should be == 0xFF and counter zero, 
    // otherwise there is an error
    if ( !iMarker && ( iCT != 0 || iB != 0xFF ) ) 
        {
        return EEntropyCodingError;
        }
  
    // If counter CT is 1, no need to check any further
    if ( iCT == 1 )
        {
        return ENoError;
        }
  
    // If counter CT is zero, then next byte must be larger than 0x8F, if the terminating
    // marker has not been reached yet
    if ( iCT == 0 )  
        {
        if ( !iMarker ) 
            {
            // Get next byte and check that it is larger than 0x8F
            iB = iInputStream.ReadByteFromStream();
            if ( iB <= 0x8F )
                {
                return EEntropyCodingError;
                }
            }
        // Set the counter back to 8
        iCT = 8;
        }

    // Number of bits that where added in the termination process
    // Compute the number of bits, k for error resilience information
    TUint32 k = (TUint32)( iCT - 1 );
  
    // Check for a coded LPS.
    TUint32 q = 0x8000 >> k;
  
    // Check that we can decode an LPS interval of probability 'q'
    iA -= q;
    if ( ( iC >> 16 ) < iA )   // software convention
        {
        // MPS interval was decoded, thus error occured
        return EEntropyCodingError;
        }
 
    // Check for coded LPS interval.
    iA = q;

    do  // Renormalization
        {
        if ( iCT == 0 )
            {
            MqByteIn();
            }

        iA <<= 1;
        iC <<= 1;
        iCT--;

        } while ( iA < 0x8000 );
  
    // No error was found
    return ENoError;
    }

// -----------------------------------------------------------------------------
// J2KMQCoder::ResetMqContexts
// Reset the MQCoder context list to the original state
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void J2KMQCoder::ResetMqContexts()
    {
    // Reset all context states to initial states and set the mps
    // of every context to 0.
    for ( TInt32 i = KNumberContexts - 1; i >= 0; i-- )
        {
        iContextList[i]->iState = iOriginalStates[KMqInitStates[i]];
        iContextList[i]->iMPS = 0;
        }
    }

// -----------------------------------------------------------------------------
// J2KMQCoder::ResetMQDecoder
// Initialze MQCoder and reset the context
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void J2KMQCoder::ResetMQDecoder( TInt32 aSegmentLength )
    {
    // Reset all contexts ( states and mps )
    ResetMqContexts();              
    iInputStream.ResetInputStream();
    iInputStream.iPosition = 0;
    iInputStream.iNumBytes = aSegmentLength;
    MqInitDecoder();
    }

// -----------------------------------------------------------------------------
// J2KMQCoder::InitializeOrigMqTable
// Initialize MQCoder original states table
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void J2KMQCoder::InitializeOrigMqTable()
    {
    for ( TUint8 i = 0; i < KNumberOriginalMQEntries; i++ )
        {
        iOriginalStates[i]->iQe = KMqQeStates[i];
        iOriginalStates[i]->iNextMPS = iOriginalStates[KMqMpsStates[i]];
        iOriginalStates[i]->iNextLPS = iOriginalStates[KMqLpsStates[i]];
        iOriginalStates[i]->iSwitchFlag = KMqSwitchFlagStates[i];
        }
    }

// -----------------------------------------------------------------------------
// J2KMQCoder::ReNormalize
// Renormalize 
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void J2KMQCoder::ReNormalize()
    {
    do
        {
        if ( iCT == 0 )
            {
            MqByteIn();
            }

        iA <<= 1;
        iC <<= 1;
        --iCT;
        } while ( !( iA & 0x8000 ) );
    }

// -----------------------------------------------------------------------------
// J2KMQCoder::MpsExchange
// MPS exchange
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void J2KMQCoder::MpsExchange( TInt32& aD )
    {
    aD = iCurrentContext->iMPS;
    if ( !( iA & 0x8000 ) )
        {
        if ( iA < iCurrentState->iQe )
            {
            aD ^= 1;
            if ( iCurrentState->iSwitchFlag )
                {
                iCurrentContext->iMPS ^= 1;
                }

            iCurrentContext->iState = iCurrentState->iNextLPS;
            }
        else
            {
            iCurrentContext->iState = iCurrentState->iNextMPS;
            }

        ReNormalize();
        }
    }

// -----------------------------------------------------------------------------
// J2KMQCoder::LpsExchange
// LPS exchange
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void J2KMQCoder::LpsExchange( TInt32& aD )
    {
    iC -= iA << 16;
    aD = iCurrentContext->iMPS;
    if ( iA < iCurrentState->iQe )
        {
        iCurrentContext->iState = iCurrentState->iNextMPS;
        }
    else
        {
        aD ^= 1;
        if ( iCurrentState->iSwitchFlag )
            {
            iCurrentContext->iMPS ^= 1;
            }

        iCurrentContext->iState = iCurrentState->iNextLPS;
        }
    iA = iCurrentState->iQe;
    ReNormalize(  );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJ2kEntropyDecoder* CJ2kEntropyDecoder::NewL( CJ2kImageInfo& aImageInfo )
    {
    CJ2kEntropyDecoder *self = new ( ELeave ) CJ2kEntropyDecoder;

    CleanupStack::PushL( self );
    self->ConstructL( aImageInfo );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CJ2kEntropyDecoder::~CJ2kEntropyDecoder()
    {  
    delete iMRLutBuf;
    iMRLutBuf = 0;

    delete iSCLutBuf;
    iSCLutBuf = 0;
  
    delete iZcLutLL;
    iZcLutLL = 0;

    delete iZcLutHL;
    iZcLutHL = 0;

    delete iZcLutHH;
    iZcLutHH = 0;
    
    TJ2kUtils::Free2DArray( iData );
    User::Free( iStates );
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::SetNewSizeL
// Set the size of internal buffer and other control data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::SetNewSizeL( const TSize& aSize )
    {
    if ( aSize.iWidth <= iCurrentSize.iWidth &&
         aSize.iHeight <= iCurrentSize.iHeight )
        {
        return;
        }
    else
        {
        iCurrentSize = aSize;

        TJ2kUtils::Free2DArray( iData );
        iData = 0;
        iData = TJ2kUtils::Alloc2DArrayL( iCurrentSize.iHeight, iCurrentSize.iWidth );

        iMaxBlockWidth = (TUint8)( iCurrentSize.iWidth );

        // We can compute some parameters for entropy decoding here not to compute them for every pass!!!
        iBlockDataWidth        = iMaxBlockWidth;
        iStateWidth            = iMaxBlockWidth + 2;
        iStateSize             = ( iCurrentSize.iHeight + 2 ) * ( iCurrentSize.iWidth + 2 );
        iDataSamplesPerStripe  = iBlockDataWidth * KStripeHeight;
        iStateSamplesPerStripe = iStateWidth * KStripeHeight;

        TInt16* ptrTemp = STATIC_CAST( TInt16*, User::ReAlloc( iStates, iStateSize * sizeof( TInt16 ) ) );
        if ( !ptrTemp )
            {
            User::Leave( KErrNoMemory );
            }
        iStates = ptrTemp;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::SetCurrentZCLUT
// Set the current pointer to point to the right LUT depending on  the current subband
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::SetCurrentZCLUT( TUint8 aBandIndex )
    {
    if ( aBandIndex == 1 )
        {
        // For HL band
        iCurrentZcLutPtr = (TUint8*)iZcLutHL->Des().Ptr();
        }
    else if ( aBandIndex == 3 )
        {
        // For HH band
        iCurrentZcLutPtr = (TUint8*)iZcLutHH->Des().Ptr();
        }
    else 
        {
        // For LL and LH band
        iCurrentZcLutPtr = (TUint8*)iZcLutLL->Des().Ptr();
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::DecodeCodeblock
// Decode the coded codeblock
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::DecodeCodeblock( CJ2kCodeBlock& aCodeblock, TUint8 aCblkStyle, 
                                          TUint8 aMagBits )
    {
    TInt32 startBitplane = 0;     // The bitplane where coding starts
    TInt32 endBitplane = 0;       // The last coded bitplane
    TInt32 terminationLength = 0;
    TUint8 tempPassIndex = 0;
    TUint8 passIndex = 0;

    iMQDecoder.iInputStream.iBuffer = CONST_CAST( TUint8*, aCodeblock.Data() );
    TSize codeBlockSize = aCodeblock.CodeBlockCanvas().Size();

    iVerticalCausalContextUsed     = ( ( aCblkStyle & EVerticalStripe ) != 0 );
    iResetContexts                 = ( ( aCblkStyle & EResetContext ) != 0 );
    iPredictableTerminationUsed    = ( ( aCblkStyle & EPredictableTermination ) != 0 );
    TUint8 terminateEachPass       = ( ( aCblkStyle & ETermination ) != 0 );
    TUint8 segmentationSymbolsUsed = ( ( aCblkStyle & ESegmentationSymbols ) != 0 );
    TUint8 arithmeticBypass        = ( ( aCblkStyle & EArithmeticBypass ) != 0 );
    aCodeblock.ResetPassIndex();
  
  
    // Initialize the stream and the mq coder for this code block:
    // also set mq coder's stream to point to aCodeblock's stream 
    if ( !arithmeticBypass && !terminateEachPass )  // No termination
        {
        iMQDecoder.ResetMQDecoder( aCodeblock.DataLength() );
        }
    else
        {
        // Compute the length of the first terminated segment. If we are 
        // terminating on each pass, terminated lengths are equal to 
        // codeblock lengths. On the other hand if we are using arithmetic
        // bypass ( without terminating each pass ), the first termination length
        // is the codeblock lengths summed up to the point where we have last AC
        // pass before first raw coding pass.

        if ( terminateEachPass )
            {
            terminationLength = aCodeblock.CblkLength( (TUint16)passIndex++ );
            }
        else  // AC bypass without termination on each pass
            {
            // Loop adding codeblock lengths until enough passes have been consumed 
            while ( ( tempPassIndex < KFirstBypassTermIndex ) && ( passIndex <= aCodeblock.LastPass() ) )
                {
                tempPassIndex = (TUint8)( tempPassIndex + aCodeblock.PassesPerSegment( passIndex ) );
                terminationLength = (TUint32)( terminationLength + aCodeblock.CblkLength( passIndex++ ) );
                }
            }

        // Now initialize the iMQDecoder with the right terminated length
        iMQDecoder.ResetMQDecoder( terminationLength );
        }
  
    // Initialize data array 
    for ( TInt i = codeBlockSize.iHeight - 1; i >= 0; i-- )
        {
        Mem::FillZ( iData[i], codeBlockSize.iWidth * sizeof( TPrecInt ) );
        }
  
    // Initialize iStates array
    Mem::FillZ( iStates, iStateSize * sizeof( TInt16 ) );
  
    // Compute different bitplanes; empty bitplanes tells how many bitplanes are 
    // empty in this codeblock compared to the whole subband, iStartBitplane gives
    // the index of the first bitplane to be coded for this block and iEndBitplane
    // is the index of the last bitplane coded for this codeblock.
  
    // Compute coded block's magnitude bits 
    aMagBits = (TUint8)( aMagBits - aCodeblock.EmptyBitplanes() );
    startBitplane = ( KImplementationPrecision - 2 ) - aCodeblock.EmptyBitplanes();
    endBitplane = startBitplane - aMagBits;
 
    // Compute the number of stripes for this codeblock 
    iNumStripes = (TUint16)( ( codeBlockSize.iHeight + KStripeHeight - 1 ) / KStripeHeight );
    iBlockWidth = codeBlockSize.iWidth;

    // Compute the last stripe's height once, 
    // so that we don't have to compute it for all passes!
    iLastStripeHeight = codeBlockSize.iHeight - ( iNumStripes - 1 ) * KStripeHeight;

    iCurrentBitplane = (TUint8)( startBitplane );

    // First do only the normalization pass 
    iTerminateThisPass = ( terminateEachPass || ( iCurrentBitplane == endBitplane ) );

    CleanupPass( segmentationSymbolsUsed );

    aCodeblock.IncrementPassIndex();

    iCurrentBitplane--;
  
    // Then repeat the three passes: significance, refinement and normalization 
    // for the remaining bitplanes.
    while ( aCodeblock.PassIndex() <= aCodeblock.LastPass() )
        {
        // Significance pass, terminate only if terminating after each pass
        iTerminateThisPass = terminateEachPass;

        if ( ( arithmeticBypass == 0 ) || ( aCodeblock.PassIndex() < KFirstLazyPassIndex ) )
            {
            if ( terminateEachPass )  // if last pass was terminated, initialize MQ-coder
                {
                // Update the number of available TUint8s for this terminated sequence
                iMQDecoder.iInputStream.iNumBytes += aCodeblock.CblkLength( passIndex++ );
                iMQDecoder.MqInitDecoder();
                }

            SignificancePass();
            }
        else
            {
            // if we are here, then the previous pass was terminated
            if ( terminateEachPass )
                {
                // Update the number of available bytes for this terminated sequence 
                iMQDecoder.iInputStream.iNumBytes += aCodeblock.CblkLength( passIndex++ );
                }
            else  // AC bypass without termination on each pass 
                {
                // Here we have two cases:
                // 1. If this codeblock length is for this pass only, then for the terminated length 
                //    we have to add the next pass codeblock length also. 
                // 2. This codeblock length is for both this pass ( significance ) and the next pass 
                //    ( refinement ). Terminated length is equal to the CblkLength[iPassIndex].
                if ( aCodeblock.PassesPerSegment( passIndex ) == 2 )
                    {
                    iMQDecoder.iInputStream.iNumBytes += aCodeblock.CblkLength( passIndex++ );
                    }
                else
                    {
                    iMQDecoder.iInputStream.iNumBytes += aCodeblock.CblkLength( passIndex ) + 
                                                         aCodeblock.CblkLength( (TUint16)( passIndex + 1 ) );
                    passIndex += 2;
                    }
                }
            iMQDecoder.iInputStream.ResetInputStream();
            LazySignificancePass();
            }
    
        aCodeblock.IncrementPassIndex();
        if ( aCodeblock.PassIndex() > aCodeblock.LastPass() )
            {
            // We have fully decoded this codeblock
            return;
            }
    
        // Magnitude refinement pass, terminate if raw coding or termination after each pass
        iTerminateThisPass = ( terminateEachPass || ( arithmeticBypass && ( aCodeblock.PassIndex() > 
                                                                            KFirstLazyPassIndex ) ) );

        if ( arithmeticBypass == 0 || ( aCodeblock.PassIndex() < KFirstLazyPassIndex ) )
            {
            if ( terminateEachPass ) // if previous pass was terminated, initialize MQ-coder 
                {
                // Update the number of available bytes for this terminated sequence
                iMQDecoder.iInputStream.iNumBytes += aCodeblock.CblkLength( passIndex++ );
                iMQDecoder.MqInitDecoder();
                }

            RefinementPass();
            }
        else
            {
            if ( terminateEachPass )  // if previous pass was terminated
                {
                // Update the number of available bytes for this terminated sequence
                iMQDecoder.iInputStream.iNumBytes += aCodeblock.CblkLength( passIndex++ );
                iMQDecoder.iInputStream.ResetInputStream();
                }

            LazyRefinementPass();
            }
    
        aCodeblock.IncrementPassIndex();
        if ( aCodeblock.PassIndex() > aCodeblock.LastPass() )
            {
            // We have fully decoded this codeblock
            return;
            }
    
        // Cleanup pass, terminate if coding raw, termination after each pass, last bitplane
        // before starting raw coding or last bitplane in this codeblock.
        iTerminateThisPass = ( terminateEachPass || ( iCurrentBitplane == endBitplane ) || 
                             ( arithmeticBypass && ( aCodeblock.PassIndex() >= KFirstLazyPassIndex ) ) );
    
        // If last ( AC ) pass was terminated, initialize MQ-coder
        if ( terminateEachPass || ( arithmeticBypass && ( aCodeblock.PassIndex() > KFirstLazyPassIndex ) ) )
            {
            // Update the number of available bytes for this terminated sequence
            iMQDecoder.iInputStream.iNumBytes += aCodeblock.CblkLength( passIndex++ );
            iMQDecoder.MqInitDecoder();
            }

        CleanupPass( segmentationSymbolsUsed );
    
        aCodeblock.IncrementPassIndex();
        if ( aCodeblock.PassIndex() > aCodeblock.LastPass() )
            {
            // We have fully decoded this codeblock
            return;
            }

        // move onto next bitplane
        if ( iCurrentBitplane == 0 )  // This might happen with 16-bit and ROI
            {
            // We have fully decoded this codeblock
            return;
            }

        iCurrentBitplane--;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::ConstructL( CJ2kImageInfo& aImageInfo )
    {
    iCurrentSize = aImageInfo.MaxBlockSize();

    for ( TUint16 i = 0; i < KNumberContexts; i++ )
        {
        J2KEntropyContexts *context = new ( ELeave ) J2KEntropyContexts;
        CleanupStack::PushL( context );
        User::LeaveIfError( iMQDecoder.iContextList.Append( context ) );
        CleanupStack::Pop(1);
        }

    for ( TUint16 ii = 0; ii < KNumberOriginalMQEntries; ii++ )
        {
        J2KEntropyStates *state = new ( ELeave ) J2KEntropyStates;
        CleanupStack::PushL( state );
        User::LeaveIfError( iMQDecoder.iOriginalStates.Append( state ) );
        CleanupStack::Pop(1);
        }

    // Allocate memory for the SC Look up table
    iSCLutBuf = HBufC8::NewL( KLutSize );
    iSCLut = CONST_CAST( TUint8*, iSCLutBuf->Des().Ptr() );

    // Allocate memory for the MR Look up table
    iMRLutBuf = HBufC8::NewL( KLutSize );
    iMRLut = CONST_CAST( TUint8*, iMRLutBuf->Des().Ptr() );

    // Allocate memory for the Zero coding Look up table
    iZcLutLL = HBufC8::NewL( ( 1 << KZcLutBits ) );
    iZcLutHL = HBufC8::NewL( ( 1 << KZcLutBits ) );
    iZcLutHH = HBufC8::NewL( ( 1 << KZcLutBits ) );

    iData = TJ2kUtils::Alloc2DArrayL( iCurrentSize.iHeight, iCurrentSize.iWidth );

    iMaxBlockWidth = (TUint16)( iCurrentSize.iWidth );

    // We can compute some parameters for entropy decoding here not to compute them for every pass!!!
    iBlockDataWidth        = iMaxBlockWidth;
    iStateWidth            = iMaxBlockWidth + 2;
    iStateSize             = ( iCurrentSize.iHeight + 2 ) * ( iCurrentSize.iWidth + 2 );
    iDataSamplesPerStripe  = iBlockDataWidth * KStripeHeight;
    iStateSamplesPerStripe = iStateWidth * KStripeHeight;

    iStates = STATIC_CAST( TInt16*, User::AllocL( iStateSize * sizeof( TInt16 ) ) );

    // Initialization of the MQ tables needed since
    // the original table could not be used as it was constant!
    iMQDecoder.InitializeOrigMqTable();

    iMaxBitDepth = 0;
    for ( TUint16 iii = 0; iii < aImageInfo.NumOfComponents(); ++iii )
        {
        iMaxBitDepth = Max( iMaxBitDepth, aImageInfo.DepthOfComponent( iii ) );
        }

    // Initialize ZC lookup table
    InitializeZCLut();

    // Initialize SC/MR lookup table
    InitializeScMrLut();
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::InitializeZCLut
// Initialize ZC lookup table
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::InitializeZCLut()
    {
    // Create pointers for all the ZC LUT's
    TUint8* iZcLutLLPtr = CONST_CAST( TUint8*, iZcLutLL->Des().Ptr() );
    TUint8* iZcLutHLPtr = CONST_CAST( TUint8*, iZcLutHL->Des().Ptr() );
    TUint8* iZcLutHHPtr = CONST_CAST( TUint8*, iZcLutHH->Des().Ptr() );

    TInt32 ctxt = 0; 
    TUint16 sh = 0;     // horizontal sum
    TUint16 sv = 0;     // vertical sum
    TUint16 sd = 0;     // diagonal sum

    for ( TUint16 i = 0; i <= KZcMask; i++ )
        {
        sh = (TUint16)( ( ( i >> KPositionRight ) & 1 ) + ( ( i >> KPositionLeft ) & 1 ) );
        sv = (TUint16)( ( ( i >> KPositionDown ) & 1 ) + ( ( i >> KPositionUp ) & 1 ) );
        sd = (TUint16)( ( ( i >> KPositionUpperLeft ) & 1 ) + ( ( i >> KPositionUpperRight ) & 1 ) + 
                      ( ( i >> KPositionLowerLeft ) & 1 ) + ( ( i >> KPositionLowerRight ) & 1 ) );

        if ( sh == 2 )
            {
            ctxt = 10;
            }
        else if ( sh == 1 )
            {
            if ( sv )
                {
                ctxt = 9;
                }
            else if ( sd )
                {
                ctxt = 8;
                }
            else
                {
                ctxt = 7;
                }
            }
        else
            {
            if ( sv )
                {
                ctxt = 4 + sv;
                }
            else
                {
                ctxt = 2 + ( ( sd > 2 ) ? 2 : sd );
                }
            }

        iZcLutLLPtr[i] = (TUint8) ctxt;


        if ( sv == 2 )
            {
            ctxt = 10;
            }
        else if ( sv == 1 )
            {
            if ( sh )
                {
                ctxt = 9;
                }
            else if ( sd )
                {
                ctxt = 8;
                }
            else
                {
                ctxt = 7;
                }
            }
        else
            {
            if ( sh )
                {
                ctxt = 4 + sh;
                }
            else
                {
                ctxt = 2 + ( ( sd > 2 ) ? 2 : sd );
                }
            }

        iZcLutHLPtr[i] = (TUint8) ctxt;
    
        if ( sd >= 3 )
            {
            ctxt = 10;
            }
        else if ( sd == 2 )
            {
            if ( ( sv + sh ) >= 1 )
                {
                ctxt = 9;
                }
            else
                {
                ctxt = 8;
                }
            }
        else if ( sd == 1 )
            {
            ctxt = 5 + ( ( ( sv + sh ) > 2 ) ? 2 : ( sv + sh ) );
            }
        else
            {
            ctxt = 2 + ( ( ( sv + sh ) > 2 ) ? 2 : ( sv + sh ) );
            }
    
        iZcLutHHPtr[i] = (TUint8) ctxt;
        } 
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::InitializeScMrLut
// Initialize SC/MR lookup table
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::InitializeScMrLut()
    {
    TInt32 ctxt = 0; 
    TInt32 vpos = 0;    // vert +
    TInt32 vneg = 0;    // vert -
    TInt32 hpos = 0;    // hor +
    TInt32 hneg = 0;    // hor -
    TInt32 hc = 0;      // horizontal contribution
    TInt32 vc = 0;      // vertical contribution
    TInt16 predict = 0; // the xor bit for differentiating states. ( positioned at the 6th bit )

    for ( TUint8 i = 0; i < 16; i++ )
        {
        vpos = i & 1;
        vneg = ( i >> 1 ) & 1;
        hpos = ( i >> 2 ) & 1;
        hneg = ( i >> 3 ) & 1;
        hc = hpos - hneg;
        vc = vpos - vneg;
        predict = 0;
        if ( hc < 0 )
            {
            predict = 1;
            hc = -hc;
            vc = -vc;
            }
        if ( hc == 0 )
            {
            if ( vc < 0 )
                {
                predict = 1;
                vc = -vc;
                }
            ctxt = 11 + vc;
            }
        else
            {
            ctxt = 14 + vc;
            }

        iSCLut[i] = (TUint8) ( ctxt | ( predict << KPredictionBit ) );
        }
    iMRLut[0] = 16;
    iMRLut[1] = 17;
    iMRLut[2] = 18;
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::GetCausal
// Get the casual
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::GetCausal( TPrecInt*& aDataCol, TInt16*& aStateCol )
    {
    TInt32 vpos = 0;
    TInt32 vneg = 0;
    TUint16 state = 0;
    TInt16 causalityMask = 0;

    aDataCol--;
    for ( TInt32 col = iStateWidth; col > 0; col-- )
        {
        state = *aStateCol;
        vpos = 0;
        vneg = 0;
        if ( ( ( state >> KPositionDown ) & 1 ) == 1 )
            {
            if ( ( ( state >> KPositionUp ) & 1 ) == 1 )
                {
                if ( col < iStateWidth )
                    {
                    if ( *aDataCol >= 0 )
                        {
                        vneg = 1;
                        }
                    else
                        {
                        vpos = 1;
                        }
                    }
                }
            else
                { 
                vpos = 1;
                vneg = 1;
                }
            }
        causalityMask = (TInt16)( ~( ( KStateDown| KStateLowerLeft | KStateLowerRight ) | 
                                     ( vneg << KPositionVerNeg ) | ( vpos << KPositionVerPos ) ) );

        ( *aStateCol ) &= causalityMask;
        aDataCol++;
        aStateCol++;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::DecodeSignificance
// Decode the significance bit
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::DecodeSignificance( TPrecInt*& aDataValue, TInt16*& aStateValue, TPrecInt aMask )
    {
    TUint32 state = *aStateValue;
    TInt32 context = state & KZcMask;
    if ( ( state & KStateSignificant ) == 0 && context > 0 )
        { 
        if ( iMQDecoder.MqDecodeSymbol( iCurrentZcLutPtr[context] ) != 0 )
            {
            TUint32 ctxt = iSCLut[( state >> KScShift ) & KScLutMask];
            TInt32 symbol = iMQDecoder.MqDecodeSymbol( ( ctxt & KScLutMask ) ) ^ ( ( ctxt >> KPredictionBit ) & 1 );
            *aDataValue = ( symbol << KSignShift ) | aMask;
            *aStateValue |= KStateSignificant | KStateVisited;
            UpdateSignificance( aStateValue, symbol );
            }
        else
            {
            ( *aStateValue ) |= KStateVisited;
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::DecodeRawSignificance
// Decode the lazy significance bit
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::DecodeRawSignificance( TPrecInt*& aDataValue, TInt16*& aStateValue, TPrecInt aMask )
    {
    TInt32 state = *aStateValue;
    if ( ( state & KStateSignificant ) == 0 && ( state & KZcMask ) > 0 )
        { 
        if ( iMQDecoder.iInputStream.ReadBitFromStream() )
            {
            TInt32 symbol = iMQDecoder.iInputStream.ReadBitFromStream();
            *aDataValue = ( symbol << KSignShift ) | aMask;
            *aStateValue |= KStateSignificant | KStateVisited;
            UpdateSignificance( aStateValue, symbol );
            }
        else
            {
            ( *aStateValue ) |= KStateVisited;
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::DecodeRefinement
// Decode the refinement bit
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::DecodeRefinement( TPrecInt*& aDataValue, TInt16*& aStateValue, 
                                           TPrecInt aMask, TInt32 aResetMask )
    {
    TInt32 state = *aStateValue;
    if ( ( state & ( KStateSignificant | KStateVisited ) ) == KStateSignificant )
        {
        TInt16 mrContext = (TInt16)( state & KStatePreviousMR );
        TInt32 ctxt = 0;
        if ( mrContext )
            {
            ctxt = 2;
            }
        else
            {
            if ( ( state & KZcMask ) == 0 )
                {
                ctxt = 0;
                }
            else
                {
                ctxt = 1;
                }
            } 
        TInt32 symbol = iMQDecoder.MqDecodeSymbol( iMRLut[ctxt] );
        ( *aDataValue ) &= aResetMask;
        ( *aDataValue ) |= ( symbol << iCurrentBitplane ) | aMask;
        ( *aStateValue ) |= KStatePreviousMR;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::DecodeRawRefinement
// Decode the lazy refinement bit
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::DecodeRawRefinement( TPrecInt*& aDataValue, TInt16*& aStateValue, 
                                              TPrecInt aMask, TInt32 aResetMask )
    {
    if ( ( ( *aStateValue ) & ( KStateSignificant | KStateVisited ) ) == KStateSignificant )
        {
        TInt32 symbol = iMQDecoder.iInputStream.ReadBitFromStream();
        ( *aDataValue ) &= aResetMask;
        ( *aDataValue ) |= ( symbol << iCurrentBitplane ) | aMask;
        ( *aStateValue ) |= KStatePreviousMR;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::DecodeNormalization
// Perform the normalization
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::DecodeNormalization( TPrecInt*& aDataValue, TInt16*& aStateValue, 
                                              TPrecInt aMask, TInt32& aZCValue )
    {
    TUint32 state = *aStateValue;
    if ( state < (TUint32)KStateVisited )
        {
        TInt32 symbol = 0;
        TUint32 ctxt = 0;
        if ( aZCValue == 1 )
            {
            ( *aStateValue ) &= ~KStateVisited;
            if ( iMQDecoder.MqDecodeSymbol( iCurrentZcLutPtr[state & KZcMask] ) != 0 )
                {
                ctxt = iSCLut[( state >> KScShift ) & KScLutMask];
                symbol = iMQDecoder.MqDecodeSymbol( ( ctxt & KScLutMask ) ) ^( ( ctxt >> KPredictionBit ) & 1 );
                ( *aDataValue ) = ( symbol << KSignShift ) | aMask;
                ( *aStateValue ) |= KStateSignificant;
                UpdateSignificance( aStateValue, symbol );
                aZCValue = 1;
                }
            }
        else
            {
            ctxt = iSCLut[( state >> KScShift ) & KScLutMask];
            symbol = iMQDecoder.MqDecodeSymbol( ( ctxt & KScLutMask ) ) ^ ( ( ctxt >> KPredictionBit ) & 1 );
            ( *aDataValue ) = ( symbol << KSignShift ) | aMask;
            ( *aStateValue ) |= KStateSignificant; \
            UpdateSignificance( aStateValue, symbol );
            aZCValue = 1; 
            }
        } 
    else
        {
        ( *aStateValue ) &= ( ~KStateVisited );
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::UpdateSignificance
// Update the significance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::UpdateSignificance( TInt16*& aStateValue, TInt32 aSymbol )
    {
    TInt16* u = ( aStateValue ) - ( iStateWidth );
    TInt16* d = ( aStateValue ) + ( iStateWidth );
    u[-1] |= KStateLowerRight;
    u[1] |= KStateLowerLeft;
    d[-1] |= KStateUpperRight;
    d[1] |= KStateUpperLeft;

    if ( aSymbol != 0 )
        {
        aStateValue[-1] |= KStateRight | KStateHorNegative;
        aStateValue[1] |= KStateLeft | KStateHorNegative;
        *d |= KStateUp | KStateVerNegative;
        *u |= KStateDown | KStateVerNegative;
        }
    else
        {
        aStateValue[-1] |= KStateRight | KStateHorPositive;
        aStateValue[1] |= KStateLeft | KStateHorPositive;
        *d |= KStateUp | KStateVerPositive;
        *u |= KStateDown | KStateVerPositive;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::SignificancePass
// Perform the significance pass
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::SignificancePass()
    {
    // Mask for the current bitplane is actually also an approximation of the 
    // lower bitplanes, i.e. mask might be 00001100 for the fourth bitplane instead
    // of 00001000, thus the data will be increased by the current bitplane and
    // half of the sum of the lower bitplanes.
    TPrecInt mask = ( 1 << iCurrentBitplane ) | ( 1 << ( iCurrentBitplane - 1 ) );
  
    TPrecInt* dataRow = &( iData[0][0] );
    TInt16*   stateRow = CONST_CAST( TInt16*, GetFirstStateRow(  ) );

    TPrecInt* dataCol = 0;
    TPrecInt* dataValue = 0; 
    TInt16*   stateCol = 0;
    TInt16*   stateValue = 0;
    
    // Column index in the stripe
    TInt32 col = 0; 
    TInt32 row = 0;

    // Code stripe by stripe
    for ( TInt32 s = iNumStripes - 1; s > 0; s-- )
        {
        // If using stripe causal context formation, reset necessary bits
        // in state of first line in next stripe ( not on last
        // stripe ).
        if ( iVerticalCausalContextUsed )
            {
            dataCol = dataRow + 2 * iBlockDataWidth;
            stateCol = stateRow + iStateSamplesPerStripe - iStateWidth - 1;
            GetCausal( dataCol, stateCol );
            }
  
        dataCol = dataRow;
        stateCol = stateRow;
    
        // Scan column by column in each stripe
        for ( col = iBlockWidth; col > 0; col--, dataCol++, stateCol++ )
            {
            stateValue = stateCol;
            dataValue = dataCol;

            for ( row = KStripeHeight; row > 0; row-- )
                {
                DecodeSignificance( dataValue, stateValue, mask );
                dataValue += iBlockDataWidth;
                stateValue += iStateWidth;
                }
            }

        dataRow += iDataSamplesPerStripe;
        stateRow += iStateSamplesPerStripe;
        }

    // Deal with the last stripe separately
    dataCol = dataRow;
    stateCol = stateRow;
    
    // Scan column by column in each stripe
    for ( col = iBlockWidth; col > 0; col--, dataCol++, stateCol++ )
        {
        stateValue = stateCol;
        dataValue = dataCol;

        for ( row = iLastStripeHeight; row > 0; row-- )
            {
            DecodeSignificance( dataValue, stateValue, mask );
            if ( row > 1 )
                {
                dataValue += iBlockDataWidth;
                }
            stateValue += iStateWidth;
            }
        }

    // If there is predictable termination, check for errors
    if ( iTerminateThisPass && iPredictableTerminationUsed )
        {
        iErrorState = iMQDecoder.MqCheckPrediction();
        }
  
    // Reset the MQ context states if we need to
    if ( iResetContexts )
        {
        iMQDecoder.ResetMqContexts();
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::LazySignificancePass
// Perform the lazy significance pass
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::LazySignificancePass()
    {
    // Mask for the current bitplane 
    TPrecInt mask = ( 1 << iCurrentBitplane ) | ( 1 << ( iCurrentBitplane - 1 ) );
  
    TPrecInt* dataRow = &( iData[0][0] );
    TInt16*   stateRow = CONST_CAST( TInt16*, GetFirstStateRow() );
    TPrecInt* dataCol = 0;
    TPrecInt* dataValue = 0; 
    TInt16* stateCol = 0;
    TInt16* stateValue = 0;

    // Column index in the stripe
    TInt32 col = 0; 
    TInt32 row = 0;

    for ( TInt32 s = iNumStripes - 1; s > 0; s-- )
        {
        // If using stripe causal context formation, reset necessary bits
        // in state of first line in next stripe ( not on last
        // stripe ).
        if ( iVerticalCausalContextUsed )
            {
            dataCol = dataRow + 2 * iBlockDataWidth;
            stateCol = stateRow + iStateSamplesPerStripe - iStateWidth - 1;
            GetCausal( dataCol, stateCol );
            }

        // Scan column by column in each stripe
        dataCol = dataRow;
        stateCol = stateRow;
    
        // Scan column by column in each stripe 
        for ( col = iBlockWidth; col > 0; col--, dataCol++, stateCol++ )
            {
            stateValue = stateCol;
            dataValue = dataCol;

            for ( row = KStripeHeight; row > 0; row-- )
                {
                DecodeRawSignificance( dataValue, stateValue, mask );
                dataValue += iBlockDataWidth;
                stateValue += iStateWidth;
                }
            }
        dataRow += iDataSamplesPerStripe;
        stateRow += iStateSamplesPerStripe;
        }

    // Deal with the last stripe separately
    // Scan column by column in each stripe
    dataCol = dataRow;
    stateCol = stateRow;
  
    // Line index
    TInt32 l = 0;
    
    // Scan column by column in each stripe 
    for ( col = iBlockWidth; col > 0; col--, dataCol++, stateCol++ )
        {
        stateValue = stateCol;
        dataValue = dataCol;

        l = iLastStripeHeight;
        DecodeRawSignificance( dataValue, stateValue, mask );
        if ( --l <= 0 )
            {
            continue;   //lint !e960    Continue is OK, no need to go end of for.
            }
    
        dataValue += iBlockDataWidth;
        stateValue += iStateWidth;
        DecodeRawSignificance( dataValue, stateValue, mask );
        if ( --l <= 0 )
            {
            continue;   //lint !e960    Continue is OK, no need to go end of for.
            }
    
        dataValue += iBlockDataWidth;
        stateValue += iStateWidth;
        DecodeRawSignificance( dataValue, stateValue, mask );
        if ( --l <= 0 )
            {
            continue;   //lint !e960    Continue is OK, no need to go end of for.
            }
    
        dataValue += iBlockDataWidth;
        stateValue += iStateWidth;
        DecodeRawSignificance( dataValue, stateValue, mask );

        }
  
    // If there is predictable termination, check for errors 
    if ( iTerminateThisPass && iPredictableTerminationUsed )
        {
        iErrorState = iMQDecoder.iInputStream.CheckPrediction();
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::RefinementPass
// Perform the refinement pass
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::RefinementPass()
    {
    // To reset the effects of the approximation done in significance and
    // normalization passes we have the resetmask.
    TInt32 resetMask = ( -1 ) << ( iCurrentBitplane + 1 );
    TUint32 mask = ( ( TUint32 )( 1 << iCurrentBitplane ) ) >> 1;
  
    TPrecInt* dataRow = &( iData[0][0] );
    TInt16*   stateRow = CONST_CAST( TInt16*, GetFirstStateRow() );

    TPrecInt* dataCol = 0;
    TPrecInt* dataValue = 0; 
    TInt16* stateCol = 0;
    TInt16* stateValue = 0;

    // Column index in the stripe
    TInt32 col = 0; 
    TInt32 row = 0;
  
    // Code stripe by stripe 
    for ( TInt32 s = iNumStripes - 1; s > 0; s-- )
        {
        // If using stripe causal context formation, reset necessary bits
        // in state of first line in next stripe ( not on last
        // stripe ).
        if ( iVerticalCausalContextUsed )
            {
            dataCol = dataRow + 2 * iBlockDataWidth;
            stateCol = stateRow + iStateSamplesPerStripe - iStateWidth - 1;
            GetCausal( dataCol, stateCol );
            }
    
        // Scan column by column in each stripe
        dataCol = dataRow;
        stateCol = stateRow;
    
        // Scan column by column in each stripe
        for ( col = iBlockWidth; col > 0; col--, dataCol++, stateCol++ )
            {
            dataValue  = dataCol;
            stateValue = stateCol;

            for ( row = KStripeHeight; row > 0; row-- )
                {
                DecodeRefinement( dataValue, stateValue, mask, resetMask );
                dataValue += iBlockDataWidth;
                stateValue += iStateWidth;
                }
            }
        dataRow += iDataSamplesPerStripe;
        stateRow += iStateSamplesPerStripe;
        }

    // Deal with the last stripe separately
    // Scan column by column in each stripe
    dataCol = dataRow;
    stateCol = stateRow;
  
    // Scan column by column in the last stripe
    for ( col = iBlockWidth; col > 0; col--, dataCol++, stateCol++ )
        {
        dataValue  = dataCol;
        stateValue = stateCol;

        for ( row = iLastStripeHeight; row > 0; row-- )
            {
            DecodeRefinement( dataValue, stateValue, mask, resetMask );
            if ( row > 1 )
                {
                dataValue += iBlockDataWidth;
                }
            stateValue += iStateWidth;
            }
        }
  
    // If there is predictable termination, check for errors 
    if ( iTerminateThisPass && iPredictableTerminationUsed )
        {
        iErrorState = iMQDecoder.MqCheckPrediction();
        }
  
    // Reset the MQ context states if we need to
    if ( iResetContexts )
        {
        iMQDecoder.ResetMqContexts();
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::LazyRefinementPass
// Perform the lazy refinement pass
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::LazyRefinementPass()
    {  
    TInt32 resetMask = ( -1 ) << ( iCurrentBitplane + 1 );
    TUint32 mask = ( ( TUint32 )( 1 << iCurrentBitplane ) ) >> 1;
    TPrecInt* dataRow = &( iData[0][0] );
    TInt16*   stateRow = CONST_CAST( TInt16*, GetFirstStateRow() );
    TPrecInt* dataCol = 0;
    TPrecInt* dataValue = 0; 
    TInt16* stateCol = 0;
    TInt16* stateValue = 0;

    // Column index in the stripe
    TInt32 col = 0; 
    TInt32 row = 0;

    // Code stripe by stripe
    for ( TInt32 s = iNumStripes - 1; s > 0; s-- )
        {
        // If using stripe causal context formation, reset necessary bits
        // in state of first line in next stripe ( not on last
        // stripe ).
        if ( iVerticalCausalContextUsed )
            {
            dataCol = dataRow + 2 * iBlockDataWidth;
            stateCol = stateRow + iStateSamplesPerStripe - iStateWidth - 1;
            GetCausal( dataCol, stateCol );
            }
    
        // Scan column by column in each stripe
        dataCol = dataRow;
        stateCol = stateRow;
    
        // Scan column by column in each stripe
        for ( col = iBlockWidth; col > 0; col--, dataCol++, stateCol++ )
            {
            dataValue  = dataCol;
            stateValue = stateCol;
            for ( row = KStripeHeight; row > 0; row-- )
                {
                DecodeRawRefinement( dataValue, stateValue, mask, resetMask );
                dataValue += iBlockDataWidth;
                stateValue += iStateWidth;
                }
            }
        dataRow += iDataSamplesPerStripe;
        stateRow += iStateSamplesPerStripe;
        }

    // Deal with the last stripe separately
    // Scan column by column in each stripe
    dataCol = dataRow;
    stateCol = stateRow;
  
    // Scan column by column in each stripe
    for ( col = iBlockWidth; col > 0; col--, dataCol++, stateCol++ )
        {
        dataValue  = dataCol;
        stateValue = stateCol;

        for ( row = iLastStripeHeight; row > 0; row-- )
            {
            DecodeRawRefinement( dataValue, stateValue, mask, resetMask );
            dataValue += iBlockDataWidth;
            stateValue += iStateWidth;
            }
        stateValue = stateCol;
        dataValue = dataCol;
        }
  
    // If there is predictable termination, check for errors
    if ( iTerminateThisPass && iPredictableTerminationUsed )
        {
        iErrorState = iMQDecoder.iInputStream.CheckPrediction();
        }

    // Because this is always terminated, reset stream
    iMQDecoder.iInputStream.ResetInputStream();
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::CleanupPass
// Perform the cleanup pass
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kEntropyDecoder::CleanupPass( TUint8 aSegSymbols )
    {
    TInt32 zcCoding = 0;

    // Symbol to be coded 
    TInt32 symbol = 0;   
  
    TPrecInt mask = ( 1 << iCurrentBitplane ) | ( 1 << ( iCurrentBitplane - 1 ) );
  
    TPrecInt* dataRow = &( iData[0][0] );
    TInt16* stateRow = CONST_CAST( TInt16*, GetFirstStateRow() );
    TPrecInt* dataCol = 0;
    TPrecInt* dataValue = 0; 
    TInt16* stateCol = 0;
    TInt16* stateValue = 0;

    // Column index in the stripe
    TInt32 col = 0; 
    TInt32 row = 0;

    // Code stripe by stripe
    for ( TInt32 s = iNumStripes - 1; s > 0; s-- )
        {
        // If using stripe causal context formation, reset necessary bits
        // in state of first line in next stripe 
        if ( iVerticalCausalContextUsed )
            {
            dataCol = dataRow + 2 * iBlockDataWidth;
            stateCol = stateRow + iStateSamplesPerStripe - iStateWidth - 1;
            GetCausal( dataCol, stateCol );
            }
    
        // Scan column by column in each stripe
        dataCol = dataRow;
        stateCol = stateRow;
    
        for ( col = iBlockWidth; col > 0; col--, dataCol++, stateCol++ )
            {
            stateValue = stateCol;
            dataValue = dataCol;

            // If sample is insignificant and not yet visited in the
            // current bitplane, then apply the normalization pass
            if ( ( *stateValue == 0 ) &&
                ( stateValue += iStateWidth, ( *stateValue ) == 0 ) &&    //lint !e960    Comma is OK.
                ( stateValue += iStateWidth, ( *stateValue ) == 0 ) &&    //lint !e960    Comma is OK.
                ( stateValue += iStateWidth, ( *stateValue ) == 0 ) )      //lint !e960    Comma is OK.
                {
                // Use RLC
                if ( iMQDecoder.MqDecodeSymbol( KRlcContext ) != 0 ) //some sample( s ) not zero
                    {
                    // Decode the symbol most significant bit first 
                    symbol  = (TUint8)( iMQDecoder.MqDecodeSymbol( KUniformContext ) << 1 );    // MSB 
                    symbol |= (TUint8)( iMQDecoder.MqDecodeSymbol( KUniformContext ) & 0x01 );  // LSB
          
                    // Update data and state pointers
                    stateValue = stateCol + symbol * iStateWidth;
                    dataValue  = dataCol + symbol * iBlockDataWidth;

                    // Set zcCoding flag to zero 
                    zcCoding = 0;
                    row = KStripeHeight - symbol;
                    }
                else // All samples zero 
                    {
                    // Move to next column
                    continue;   //lint !e960    Continue is OK, no need to go end of for.
                    }
                }
            else  // No RLC used 
                {
                stateValue = stateCol;

                // Set zcCoding flag to one
                zcCoding = 1;
                row = KStripeHeight;
                }

            for ( ; row > 0; row-- )
                {
                DecodeNormalization( dataValue, stateValue, mask, zcCoding );
                dataValue += iBlockDataWidth;
                stateValue += iStateWidth;
                }
            }
        dataRow += iDataSamplesPerStripe;
        stateRow += iStateSamplesPerStripe;
        }
  
    // Deal with last stripe separately
    // Scan column by column in each stripe
    dataCol = dataRow;
    stateCol = stateRow;
  
    // Scan column by column in each stripe
    for ( col = iBlockWidth; col > 0; col--, stateCol++, dataCol++ )
        {
        stateValue = stateCol;
        dataValue = dataCol;

        // If sample is insignificant and not yet visited in the
        // current bitplane, then apply the normalization pass
        if( ( iLastStripeHeight == KStripeHeight ) && ( *stateValue ==0 ) &&
           ( stateValue += iStateWidth, ( *stateValue ) == 0 ) &&    //lint !e960    Comma is OK.
           ( stateValue += iStateWidth, ( *stateValue ) == 0 ) &&    //lint !e960    Comma is OK.
           ( stateValue += iStateWidth, ( *stateValue ) == 0 ) )      //lint !e960    Comma is OK.
            {
            // Use RLC:
            if ( iMQDecoder.MqDecodeSymbol( KRlcContext ) != 0 )  //some sample( s ) not zero
                {
                // Decode the symbol most significant bit first
                symbol  = ( TUint8 )( iMQDecoder.MqDecodeSymbol( KUniformContext ) << 1 );   // MSB 
                symbol |= ( TUint8 )( iMQDecoder.MqDecodeSymbol( KUniformContext ) & 0x01 ); // LSB 
        
                // Update data and state pointers 
                stateValue = stateCol + symbol * iStateWidth;
                dataValue  = dataCol + symbol * iBlockDataWidth;

                // Set zcCoding flag to zero
                zcCoding = 0;
                row = KStripeHeight - symbol;
                }
            else   // All samples zero
                {
                // Move to next column
                continue;      //lint !e960    Continue is OK, no need to go end of for.
                }
            }
        else  // No RLC used 
            {
            row = iLastStripeHeight;
            stateValue = stateCol;

            // Set zcCoding flag to one 
            zcCoding = 1;
            }

        for ( ; row > 0; row-- )
            {
            DecodeNormalization( dataValue, stateValue, mask, zcCoding );
            if ( row > 1 )
                {
                dataValue += iBlockDataWidth;
                }
            stateValue += iStateWidth;
            }
        }
  
    // Decode a segment marker if we need to
    if ( aSegSymbols )
        {
        symbol  = (TUint8)( iMQDecoder.MqDecodeSymbol( KUniformContext ) << 3 );
        symbol |= (TUint8)( iMQDecoder.MqDecodeSymbol( KUniformContext ) << 2 );
        symbol |= (TUint8)( iMQDecoder.MqDecodeSymbol( KUniformContext ) << 1 );
        symbol |= (TUint8)( iMQDecoder.MqDecodeSymbol( KUniformContext ) );

        if ( symbol != KSegmentationMarker )
            {
            iErrorState = EEntropyCodingError;
            }
        } 
  
    // If there is predictable termination, check for errors 
    if ( iTerminateThisPass && iPredictableTerminationUsed )
        {
        iErrorState = iMQDecoder.MqCheckPrediction();
        }
  
    // Reset the MQ context states if we need to
    if ( iResetContexts )
        {
        iMQDecoder.ResetMqContexts();
        }
    }

// -----------------------------------------------------------------------------
// CJ2kEntropyDecoder::GetFirstStateRow
// Get the first state row
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TInt16* CJ2kEntropyDecoder::GetFirstStateRow() const
    {
    return iStates + ( iStateWidth + 1 );
    }

