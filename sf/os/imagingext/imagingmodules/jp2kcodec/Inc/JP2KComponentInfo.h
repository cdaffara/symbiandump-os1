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
* Description:  CJ2kComponentInfo class used to collect component related
*                information such as precincts size at each resolution level and
*                list of subbands
*
*/


#ifndef __JP2KCOMPONENTINFO_H__
#define __JP2KCOMPONENTINFO_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CJ2kTileInfo;
class CJ2kSubband;

// CLASS DECLARATION

/**
 * JP2KComponentInfo class used to collect component related
 * information such as precincts size at each resolution level,
 * list of subbands, and etc. It contains the data to control
 * where to resume the parsing of the bitstream.
 *
 * JP2KCodec.dll
 * @since 2.6
 */
class CJ2kComponentInfo : public CBase
{
public:
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CJ2kComponentInfo* NewLC( CJ2kTileInfo& aTile, TUint16 aIndex );

        /**
        * Destructor.
        */
        virtual ~CJ2kComponentInfo();

    public: // New functions

        /**
        * At each component, parse the bitstream with LRCP progression order
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @return TUint8: true if incomplete, false if completed.
        */
        TUint8 LRCPProgressionL( CJ2kTileInfo& aTile );

        /**
        * At each component, parse the bitstream with RPCL progression order
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @return TUint8: true if incomplete, false if completed.
        */
        TUint8 RPCLProgressionL( CJ2kTileInfo& aTile );

        /**
        * At each component, parse the bitstream with CPRL progression order
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @return TUint8: true if incomplete, false if completed.
        */
        TUint8 CPRLProgressionL( CJ2kTileInfo& aTile );

        /**
        * Get the subband at specific resolution level
        * @since 2.6
        * @param aResLevel: resolution level.
        * @return CJ2kSubband*: pointer to CJ2kSubband object.
        */
        const CJ2kSubband* SubbandAt( TUint8 aResLevel ) const;

        /**
        * Get the number of packets at specific resolution level
        * @since 2.6
        * @param aResLevel: resolution level.
        * @return TUint32: number of packets.
        */
        TUint32 NumOfPackets( TUint8 aResLevel ) const;

        /**
        * Get the quantization exponent value for transformation
        * @since 2.6
        * @param aIndex: subband index.
        * @return TUint8: exponent value.
        */
        TUint8 Exponent( TUint16 aIndex ) const;

        /**
        * Get the quantization mantissa value for transformation
        * @since 2.6
        * @param aIndex: subband index.
        * @return TUint16: mantissa value.
        */
        TUint16 Mantissa( TUint16 aIndex ) const;

        /**
        * Get the magnitude bits
        * @since 2.6
        * @param aIndex: subband index.
        * @return TUint8: magnitude bits.
        */
        TUint8 MagnitudeBits( TUint16 aIndex ) const;

        /**
        * Reset the last packet processed for each subband
        * @since 2.6
        */
        void ResetLastPacketProcessed();

        /**
        * Get the resolution level
        * @since 2.6
        * @return TUint8: resolution level.
        */
        TUint8 Levels() const;

        /**
        * Get the codeblock style
        * @since 2.6
        * @return TUint8: codeblock style.
        */
        TUint8 CodeBlockStyle() const;

        /**
        * Get the number of guard
        * @since 2.6
        * @return TUint8: number of guard.
        */
        TUint8 NumGuard() const;

        /**
        * Get the quantization style
        * @since 2.6
        * @return TUint8: quantization style.
        */
        TUint8 QuantizationStyle() const;

        /**
        * Get the region of interest shifting
        * @since 2.6
        * @return TUint8: region of interest shifting.
        */
        TUint8 RoiShift() const;

        /**
        * Get the codeblock size
        * @since 2.6
        * @return TSize&: a reference to the codeblock size.
        */
        const TSize& CodeBlockSize() const;

        /**
        * Get the canvas of the component
        * @since 2.6
        * @return TRect&: a reference to the component's canvas.
        */
        const TRect& ComponentCanvas() const;

        /**
        * Get the precinct size at specific resolution level
        * @since 2.6
        * @param aResLevel: resolution level.
        * @return TSize&: a reference to the precinct size.
        */
        const TSize& PrecinctSizeAt( TUint8 aResLevel ) const;

        /**
        * Get the minimum grid size
        * @since 2.6
        * @return TSize&: a reference to the minimum grid.
        */
        const TSize& MinGrid() const;

        /**
        * Is coding style shows SOP marker may exists in codestream
        * @since 2.6
        * @return TUInt8: true if SOP marker used.
        */
        TUint8 IsSOPMarkerUsed() const;

        /**
        * Is coding style shows EPH marker may exists in codestream
        * @since 2.6
        * @return TUint8: true if EPH marker used.
        */
        TUint8 IsEPHMarkerUsed() const;

        /**
        * Is codeblock style shows AC bypass
        * @since 2.6
        * @return TUint8: true if AC bypass.
        */
        TUint8 IsACByPass() const;

        /**
        * Is codeblock style shows termination
        * @since 2.6
        * @return TUint8: true if termination.
        */
        TUint8 IsTermination() const;

        /**
        * Is transformation reversible
        * @since 2.6
        * @return TUint8: true if reversible.
        */
        TUint8 IsReversible() const;  

    public: // Functions from base classes
      
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CJ2kComponentInfo();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CJ2kTileInfo& aTile, TUint16 aIndex );

    public:     // Data
   
    protected:  // Data

    private:    // Data

        // Canvas of the component
        TRect iComponentCanvas;  

        // Root of the subbands
        CJ2kSubband *iRootSubband; 

        // Precincts at each resolution level
        RArray<TSize> iPrecinctSizeList;

        // List of grid
        RArray<TSize> iGridList;         

        // Minimum grid size
        TSize iMinGrid;          

        // Size of codeblock
        TSize iCodeBlockSiz;           

        // Number of resolution level
        TUint8 iNumOfLevels; 
        
        // Codeblock style
        TUint8 iCodeBlockStyle;         

        // DWT transformation
        TUint8 iWaveletTransformation;  

        // Coding style
        TUint8 iCod;                    
    
        // Quantization
        TUint8 iQc;                     

        // Region of interest
        TUint8 iRoiShift;    
        
        // Quantization values for transformation        
        HBufC8 *iExponentList;

        // Quantization values for transformation
        HBufC16 *iMantissaList;           

    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
};

// For inliners.
#include "JP2KComponentInfo.inl"

#endif // __JP2KCOMPONENTINFO_H__
