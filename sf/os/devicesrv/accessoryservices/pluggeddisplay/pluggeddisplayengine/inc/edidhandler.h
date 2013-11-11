/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * CEDIDHandler class declaration.
 *
 */

#ifndef C_EDIDHANDLER_H
#define C_EDIDHANDLER_H

// INCLUDES
#include <e32base.h>
#include <ddc_access.h>
#include <tvoutconfig.h>
#include <accpolhdmiaudioformat.h>
#include <accpolhdmivideoformat.h>
#include <accpolhdmilatency.h>
#include <tvoutconfigdef.h>

#include "ediddatatypes.h"
#include "videotimings.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDdcPortAccess;
class MFSMForBody;
class CEdidParserBase;
class CCea861EdidParser;
class CTVOutConfigForHDMI;
class CAccPolHdmiSink;
class CAccPolHdmiLatency;
class CAccPolHdmiSpeakerAllocation;

// CLASS DECLARATION
/**
 *  Class to handle EDID data.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CEDIDHandler ): public CActive
    {
public:
    // Request Identifier
    enum TRequestTypeID
        {
        EUndefRequest = 0x0,
        EDdcReadRequest,
        ERetryTimerRequest,
        };
    
    // Constructors and destructor

    /**
     * Symbian two phased constructor.
     * 
     * @since S60 TB9.2
     * @param aFSM A FSM.
     * @param CTVOutConfigForHDMI TV Out Configurer.
     * @return CEDIDHandler*
     */
    static CEDIDHandler* NewL( MFSMForBody& aFSM,
        CTVOutConfigForHDMI& aTVOutConfigForHDMI );

    /**
     * Destructor.
     */
    virtual ~CEDIDHandler();

public:
    // New methods

    /**
     * Fetch EDID Data.
     * 
     * @since TB9.2
     * @param None.
     * @return Error code, KErrNone if successfull.
     */
    TInt FetchEDIDData();

    /**
     * Set video parameters of TV Out Configuration.
     * 
     * @since TB9.2
     * @param None.
     * @return Error code, KErrNone if successfull.
     */
    TInt SetVideoParameters();

    /**
     * Reset EDID Data.
     * 
     * @since TB9.2
     * @param None.
     * @return void.
     */
    void ResetData();

    /**
     * Create HDMI Sink object.
     * 
     * @since TB9.2
     * @param None.
     * @return CAccPolHdmiSink* HDMI sink object.
     */
    CAccPolHdmiSink* CreateHdmiSinkL();

    /**
     * Create HDMI Video Format object.
     * 
     * @since TB9.2
     * @param aHdmiVideoFormatArray Array of CAccPolHdmiVideoFormat pointers.
     * @return void.
     */
    void CreateHdmiVideoFormatL( RAccPolHdmiVideoFormatArray& aHdmiVideoFormatArray );

    /**
     * Create HDMI Latency object.
     * 
     * @since TB9.2
     * @param aHdmiLatencyArray Array of CAccPolHdmiLatency pointers.
     * @return None.
     */
    void CreateHdmiLatencyL( RAccPolHdmiLatencyArray& aHdmiLatencyArray );

    /**
     * Create HDMI Audio Format object.
     * 
     * @since TB9.2
     * @param aHdmiAudioFormatArray Array of CAccPolHdmiAudioFormat pointers.
     * @return void.
     */
    void CreateHdmiAudioFormatL( RAccPolHdmiAudioFormatArray& aHdmiAudioFormatArray );

    /**
     * Create HDMI Speaker Allocation object.
     * 
     * @since TB9.2
     * @param None.
     * @return CAccPolHdmiSpeakerAllocation* HDMI Speaker Allocation object.
     */
    CAccPolHdmiSpeakerAllocation* CreateHdmiSpeakerAllocationL();

    /**
     * Tells whether or not HDCP is supported at the moment.
     * 
     * @since TB9.2
     * @param None.
     * @return TBool Symbian error code. KErrNone, if ok. KErrNotFound if cannot be defined.
     */
    TInt GetHdcpSupportStatus( TBool& aHdcpSupport ) const;

    /**
     * Update overscan values from cenrep.
     * 
     * @param None.
     * @return None.
     */
    TBool UpdateOverscanValues();
    
	void GetCurrentOverscanValue( TInt& aHOverscan, TInt& aVOverscan );

private:

    /**
     * Read EDID Data from DDC Access.
     * 
     * @since TB9.2
     * @param None.
     * @return Error code. KErrNone if successful.
     */
    TInt ReadEDIDDataL();

    // Functions from base class CActive

    /**
     * RunL()
     */
    void RunL();

    /**
     * DoCancel()
     */
    void DoCancel();

    /**
     * RunError()
     */
    TInt RunError( TInt aError );

private:
    // Constructors

    /**
     * C++ constructor.
     * @param aFSM A FSM.
     * @param CTVOutConfigForHDMI TV Out Configurer.
     */
    CEDIDHandler( MFSMForBody& aFSM,
            CTVOutConfigForHDMI& aTVOutConfigForHDMI );

    /**
     * Symbian 2nd phase constructor.
     * 
     */
    void ConstructL();

private:
    
    /**
     * Fills the common timing attributes which can be received from
     * edidparser or cenrep.
     * 
     * @param aTimings Timings struct which is filled.
     * @return None.
     */
    void FillCommonHdmiDviTimings( THdmiDviTimings& aTimings ) const;
        
    /**
     * Fills out the THdmiDviTimings based on the timing item.
     * 
     * @param aItem Timing item from which the data is used.
     * @param aTimings Timings struct which is filled.
     * @return None.
     */
    void FillHdmiDviTimings( const TTimingItem& aItem,
        THdmiDviTimings& aTimings ) const;
    
    /**
     * Fills out the THdmiDviTimings based on the descriptor block.
     * 
     * @param aDescBlock Descriptor block.
     * @param aTimings Timings struct which is filled.
     * @return None.
     */
    void FillHdmiDviTimings( const TEdidDescriptorBlock& aDescBlock,
        THdmiDviTimings& aTimings ) const;
    
    /**
     * Set cea modes.
     * 
     * @param aTimings Array to fill.
     * @return system error code.
     */
    TInt SetCeaModes( RArray<THdmiDviTimings>& aTimings ) const;
    
    /**
     * Set dmt modes.
     * 
     * @param aTimings Array to fill.
     * @return system error code.
     */
    TInt SetDmtModes( RArray<THdmiDviTimings>& aTimings ) const;
    
    /**
     * Set dmt modes from established timings 1 and 2.
     * 
     * @param aTimings Array to fill.
     * @return system error code.
     */
    TInt SetDmtModesFromEstablishedTimings( RArray<THdmiDviTimings>& aTimings ) const;
    
    /**
     * Set dmt modes from standard timings.
     * 
     * @param aTimings Array to fill.
     * @return None.
     */
    TInt SetDmtModesFromStandardTimings( RArray<THdmiDviTimings>& aTimings ) const;
    
    /**
     * Set dmt modes from timing descriptors.
     * 
     * @param aTimings Array to fill.
     * @return None.
     */
    TInt SetDmtModesFromTimingDescriptors( RArray<THdmiDviTimings>& aTimings ) const;
    
    /**
     * Get timing by index.
     * 
     * @param aIndex Indes of the item to get
     * @param aType Timing type: CEA or DMT.
     * @return const TTimingItem*
     */
    const TTimingItem* TimingByIndex( TInt aIndex,
        TTimingModeType aType ) const;
    
    /**
     * Get timing by resolution.
     * 
     * @param aWidth Horizontal size.
     * @param aHeight Vertical size.
     * @param 
     * @param aType Timing type: CEA or DMT.
     * @return const TTimingItem*
     */
    const TTimingItem* TimingByResolution( TUint16 aWidth,
        TUint16 aHeight,
        TUint16 aRefreshRate,
        TTimingModeType aType ) const;
    
    /**
     * Set DMT mode by resolution.
     * 
     * @param aTimings Array where to append new DMT timing.
     * @param aWidth Width of the resolution. 
     * @param aHeight Height of the resolution.
     * @param aRefreshRate Refresh rate used.
     * @return None.
     */
    TInt SetDmtModeByResolution( RArray<THdmiDviTimings>& aTimings,
        TUint16 aWidth,
        TUint16 aHeight,
        TUint16 aRefreshRate ) const;
    
    /**
     * Resolves aspect ratio from width and height.
     * 
     * @param aWidth Horizontal pixels.
     * @param aHeight Vertical lines.
     * @return TTvSettings::TAspectRatio.
     */
    TTvSettings::TAspectRatio ResolveAspectRatio( TUint16 aWidth,
        TUint16 aHeight ) const;
    
    /**
     * Calculates overscan in pixels. The top left and right bottom should
     * cover the full screen from which the overscan is calculated.
     * 
     * @param aTLCorner Top left corner.
     * @param aRBCorner Right bottom corner.
     * @return None.
     */
    void CalculateOverscan( TPoint& aTLCorner, TPoint& aBRCorner ) const;
    
	/**
	 * Filter out the unsupported TV configurations.
	 * 
	 * @param  aHdmiConfigs    Available HDMI/DVI Tv configurations.
	 * @return KErrNone if OK. KErrNotFound if the filtered list is empty.
	 *			 
	 */
	TInt FilterAvailableTvConfigList( RArray<THdmiDviTimings>& aHdmiConfigs );
    
private: // Data

    class TDataBlock
        {
    public:
        TUint8 iDataBlock[KSizeOfSegment];
        };

    // Display Data Channel
    // Own.
    CDdcPortAccess* iDdcPortAccess;

    // A FSM pointer.
    // Not own.
    MFSMForBody& iFSM;

    // CEdidParserBase is the base class for parsing EDID data
    // Own.
    CEdidParserBase* iEdidParserPtr;

    // CCea861EdidParser is the base class for parsing 
    // CEA 861 EDID extension data
    // Own.
    CCea861EdidParser* iExtensionParserPtr;

    // EDID Data Block pointer.
    // Own
    TDataBlock* iDataBlockPtr;

    // TV Out Configurer.
    // Not own.
    CTVOutConfigForHDMI& iTVOutConfigForHDMI;
    
    // Horizontal overscan in pixels (1% == 100)
    TInt iHOverscan;

    // Vertical overscan in pixels (1% == 100)
    TInt iVOverscan;
    
    // Timer for reading EDID data
    RTimer iRetryTimer;
    
    // Retry Counter
    TInt iRetryCounter;
    
    // Request ID
    TRequestTypeID iRequestID;

	// Number of extensions to read
	TInt inbrOfExtensions;

	// Current EDID block
	TInt iCurrentBlock;
    };

#endif // C_EDIDHANDLER_H
