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
* Description:  JP2KSubband class used to collect the subband related
*                information such as list of packets and list of subbands.
*
*/


#ifndef __JP2KSUBBAND_H__
#define __JP2KSUBBAND_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CJ2kPacket;
class CJ2kTileInfo;
class CJ2kComponentInfo;

// CLASS DECLARATION

/**
*  JP2KSubband class used to collect the subband related
*  information such as list of packets, list of subbands
*  if it is EBandLL, and etc. It contains the data to control
*  where to resume the parsing of the bitstream. It is 
*  implemented in Composite pattern.
*
*  JP2KCodec.dll
*  @since 2.6
*/
class CJ2kSubband : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CJ2kSubband();

    public: // New functions
        
        // Subband type        
        enum TSubbandType
            {
            EBandLL,  // LL subband, which may further subdivided into subband
            EBandHL,  // HL Subband
            EBandLH,  // LH Subband
            EBandHH   // HH Subband
            };

        /**
        * Static method to set up the tree structure of the DWT subbands
        * @since 2.6
        * @param aMaxLevel: maximum level of DWT subbands.
        * @param aComponent: a reference to CJ2kComponentInfo object.
        * @return CJ2kSubband*: pointer to root CJ2kSubband object.
        */
        static CJ2kSubband* BuildSubbandTreeL( TUint8 aMaxLevel, CJ2kComponentInfo& aComponent );

        /**
        * Build the possible packets that may be in the subband
        * @since 2.6
        * @param aComponent: a reference to CJ2kComponentInfo object.
        * @param aPrecinctSize: a pointer to descriptor of precinct size.
        * @param aLayer: the layer.
        */
        void BuildPacketsL( CJ2kComponentInfo& aComponent, HBufC8 *aPrecinctSiz, TUint16 aLayer );

        /**
        * At each subband, parse the bitstream with LRCP progression order
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponent: a reference to CJ2kComponentInfo object.
        * @return TUint8: true if incomplete, false if completed.
        */
        TUint8 LRCPProgressionL( CJ2kTileInfo& aTile, CJ2kComponentInfo& aComponent );

        /**
        * At each subband, parse the bitstream with RPCL progression order
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponent: a reference to CJ2kComponentInfo object.
        * @return TUint8: true if incomplete, false if completed.
        */
        TUint8 RPCLProgressionL( CJ2kTileInfo& aTile, CJ2kComponentInfo& aComponent );

        /**
        * At each subband, parse the bitstream with CPRL progression order
        * @since 2.6
        * @param aTile: a reference to CJ2kTileInfo object.
        * @param aComponent: a reference to CJ2kComponentInfo object.
        * @return TUint8: true if incomplete, false if completed.
        */
        TUint8 CPRLProgressionL( CJ2kTileInfo& aTile, CJ2kComponentInfo& aComponent );

        /**
        * Get the child subband at specific band
        * @since 2.6
        * @param aBand: the band index.
        * @return CJ2kSubband*: the child at specific band.
        */
        virtual CJ2kSubband* ChildAt( TInt aBand );

        /**
        * Get the sibling subband
        * @since 2.6
        * @return CJ2kSubband*: pointer to the sibling subband.
        */
        virtual CJ2kSubband* NextSubbandRaster();

        /**
        * Get the subband at specific resolution level
        * @since 2.6
        * @param aResLevel: the resolution level.
        * @return CJ2kSubband*: pointer to the subband at specific resolution level.
        */
        virtual CJ2kSubband* SubbandAt( TUint8 aResLevel );

        /**
        * Add the child subband into the list of child subbands
        * @since 2.6
        * @param aChild: add the child subband.
        */
        virtual void AddChildL( CJ2kSubband* aChild );

        /**
        * Get the subband tree index
        * @since 2.6
        * @return TUint16: the subband tree index.
        */
        TUint16 SubbandTreeIndex() const;

        /**
        * Get the parent subband of the current subband
        * @since 2.6
        * @return CJ2kSubband*: a pointer to the parent.
        */
        CJ2kSubband* Parent();

        /**
        * Set the parent subband of the current subband
        * @since 2.6
        * @param aParent: the parent of current subband.
        */
        void SetParent(CJ2kSubband* aParent);
 
        /**
        * Set the subband level
        * @since 2.6
        * @param aLevel: the level.
        */
        void SetSubbandLevel( TUint8 aLevel );

        /**
        * Get the subband type
        * @since 2.6
        * @return CJ2kSubband::TSubbandType: the subband's type.
        */
        CJ2kSubband::TSubbandType SubbandType() const;

        /**
        * Get the subband level
        * @since 2.6
        * @return TUint8: the subband's level.
        */
        TUint8 SubbandLevel() const;

        /**
        * Get the subband resolution level
        * @since 2.6
        * @return TUint8: the subband's resolution level.
        */
        TUint8 SubbandResLevel() const;

        /**
        * Get the subband gain
        * @since 2.6
        * @return TUint8: the subband gain.
        */
        TUint8 SubbandGain() const;

        /**
        * Get the subband high pass first
        * @since 2.6
        * @return TPoint&: a reference to the subband's high pass first.
        */
        const TPoint& HighPassFirst() const;

        /**
        * Get the subband origin
        * @since 2.6
        * @return TPoint&: a reference ot the subband's origin point.
        */
        const TPoint& SubbandOrigin() const;

        /**
        * Get the subband canvas
        * @since 2.6
        * @return TRect&: a reference to the subband's canvas.
        */
        const TRect& SubbandCanvas() const;

        /**
        * Get the subband canvas size
        * @since 2.6
        * @return TSize: the subband's canvas size.
        */
        TSize SubbandCanvasSize() const;

        /**
        * Get the list of packets
        * @since 2.6
        * @return RPointerArray<CJ2kPacket>&: a reference to the packet list.
        */
        const RPointerArray<CJ2kPacket>& PacketList() const;

        /**
        * Get the last packet processed
        * @since 2.6
        * @return TUint16: the last packet processed.
        */
        TUint16 LastPacketProcessed() const;

        /**
        * Reset the last packet processed to 0
        * @since 2.6
        */
        void ResetLastPacketProcessed();

        /**
        * Set the last subband processed
        * @since 2.6
        * @param aSubbandType: the subband type.
        */
        void SetLastSubbandProcessed( TUint8 aSubbandType );

        /**
        * Get the last subband processed
        * @since 2.6
        * @return TUint8: the last subband processed.
        */
        TUint8 LastSubbandProcessed() const;

        /**
        * Get the packet at specific location
        * @since 2.6
        * @param aIndex: the index.
        * @return CJ2kPacket&: a reference to the CJ2kPacket object.
        */
        const CJ2kPacket& PacketAt( TUint16 aIndex ) const;

    public: // Functions from base classes
        
    protected:  // New functions
        
        /**
        * C++ default constructor.
        */
        CJ2kSubband();

    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data
            
        // Parent of the Subband
        CJ2kSubband *iParent;        

        // Canvas of the Subband
        TRect iSubbandCanvas; 

        // Origin of the baSubbandnd
        TPoint iSubbandOrigin; 

        // High pass first
        TPoint iHighPassFirst; 

        // Last packet processed
        TUint16 iLastPacket;  
        
        // Type of the Subband
        TSubbandType iType;          

        // Gain of the Subband
        TUint8 iGain;  
        
        // Level of the Subband
        TUint8 iLevel;       
        
        // Resolution level of the Subband
        TUint8 iResLevel;    

        // Last Subband processed
        TUint8 iLastSubband;   

        // List of packets
        RPointerArray<CJ2kPacket> iPacketList; 

    private:    // Data

    public:     // Friend classes
       
    protected:  // Friend classes
      
    private:    // Friend classes  

    };


/**
*  CJ2kSubbandLL class is a subband class that can 
*  have children, i.e. can be divided into new subbands.
*
*  JP2KCodec.dll
*  @since 2.6
*/
class CJ2kSubbandLL : public CJ2kSubband
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CJ2kSubbandLL * NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CJ2kSubbandLL();

    public: // New functions
        
        /**
        * Get the child subband at specific band
        * @since 2.6
        * @param aBand: the band index.
        * @return CJ2kSubband*: the child at specific band.
        */
        CJ2kSubband* ChildAt( TInt aBand );

        /**
        * Get the subband at specific resolution level
        * @since 2.6
        * @param aResLevel: the resolution level.
        * @return CJ2kSubband*: pointer to the subband at specific resolution level.
        */
        CJ2kSubband* SubbandAt( TUint8 aResLevel );

        /**
        * Add the child subband into the list of child subbands
        * @since 2.6
        * @param aChild: add the child subband.
        */
        void AddChildL( CJ2kSubband* aChild );

    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data
        
        // List of child subbands
        RPointerArray<CJ2kSubband> iChildList;   

    public:     // Friend classes
       
    protected:  // Friend classes
       
    private:    // Friend classes

    };

/**
*  CJ2kSubbandNLL class is a subband class that can not
*  have children.
*
*  JP2KCodec.dll
*  @since 2.6
*/
class CJ2kSubbandNLL : public CJ2kSubband
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CJ2kSubbandNLL * NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CJ2kSubbandNLL();

    public: // New functions
        
        /**
        * Get the sibling subband
        * @since 2.6
        * @return CJ2kSubband*: pointer to the sibling subband.
        */
        CJ2kSubband* NextSubbandRaster();

    public: // Functions from base classes
       
    protected:  // New functions

    protected:  // Functions from base classes

    private:

    public:     // Data
     
    protected:  // Data

    private:    // Data

    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
        

    };

// For inliners.
#include "JP2KSubband.inl"

#endif // __JP2KSUBBAND_H__
