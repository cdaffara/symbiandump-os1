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
 * Description:  Speaker allocation object definition
 *
 */

#ifndef ACCPOLSPEAKERALLOCATION_H
#define ACCPOLSPEAKERALLOCATION_H

#include <e32cmn.h>
#include <accpolobjectbase.h>

/**
 *  Store's audio and video latency parameters.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolHdmiSpeakerAllocation ): public CAccPolObjectBase
    {
public:
    
    /**
     * HDMI sink speaker allocation bitmap.
     * Left&Right combined to single bit as in "Speaker Allocation Data Block".
     */
    enum THdmiSpeakerAllocation
        {
        EHdmiSpeakerUnknown                = 0x00000,
        EHdmiSpeakerFrontLeftRight         = 0x00001,
        EHdmiSpeakerLowFrequencyEffect     = 0x00002,
        EHdmiSpeakerFrontCenter            = 0x00004,
        EHdmiSpeakerRearLeftRight          = 0x00008,
        EHdmiSpeakerRearCenter             = 0x00010,
        EHdmiSpeakerFrontLeftRightCenter   = 0x00020,
        EHdmiSpeakerRearLeftRightCenter    = 0x00040,
        EHdmiSpeakerFrontLeftRightWide     = 0x00080,
        EHdmiSpeakerFrontLeftRightHigh     = 0x00100,
        EHdmiSpeakerTopCenter              = 0x00200,
        EHdmiSpeakerFrontCenterHigh        = 0x00400
        };

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolHdmiSpeakerAllocation();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiSpeakerAllocation* NewL(
        CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiSpeakerAllocation* NewL(
        const TUint32 aSpeakerAllocation );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiSpeakerAllocation* NewL(
        const TBool aFrontSpeakers,
        const TBool aRearSpeakers,
        const TBool aLowFrequencyEffect,
        const TBool aFrontCenter,
        const TBool aFrontCenterHigh,
        const TBool aTopCenter,
        const TBool aRearCenter,
        const TBool aFrontLeftRightCenter,
        const TBool aRearLeftRightCenter,
        const TBool aFrontWideSpeakers,
        const TBool aFrontHighSpeakers );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiSpeakerAllocation* NewLC(
        CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiSpeakerAllocation* NewLC(
        const TUint32 aSpeakerAllocation );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiSpeakerAllocation* NewLC(
        const TBool aFrontSpeakers,
        const TBool aRearSpeakers,
        const TBool aLowFrequencyEffect,
        const TBool aFrontCenter,
        const TBool aFrontCenterHigh,
        const TBool aTopCenter,
        const TBool aRearCenter,
        const TBool aFrontLeftRightCenter,
        const TBool aRearLeftRightCenter,
        const TBool aFrontWideSpeakers,
        const TBool aFrontHighSpeakers );

    /**
     * Assign operator for audio latency class.
     *
     * @param CAccPolHdmiSpeakerAllocation, Const reference to object to be copied.
     * @return CAccPolHdmiSpeakerAllocation, Reference to object pointer.
     */
    IMPORT_C CAccPolHdmiSpeakerAllocation& operator= (
        const CAccPolHdmiSpeakerAllocation& );

    /**
     * Sets the speaker allocation.
     *
     * @param aSpeakerAllocation, a bitmap of TSpeakerAllocation
     *
     * @return void.
     */
    IMPORT_C void SetSpeakerAllocation( const TUint32 aSpeakerAllocation );

    /**
     * Sets the front speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetFrontSpeakers( const TBool aValue );

    /**
     * Sets the rear speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetRearSpeakers( const TBool aValue );

    /**
     * Sets the LFE speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetLowFrequencyEffect( const TBool aValue );

    /**
     * Sets the front center speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetFrontCenter( const TBool aValue );

    /**
     * Sets the front center high speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetFrontCenterHigh( const TBool aValue );

    /**
     * Sets the top center speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetTopCenter( const TBool aValue );

    /**
     * Sets the rear center speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetRearCenter( const TBool aValue );

    /**
     * Sets the front left/right center speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetFrontLeftRightCenter( const TBool aValue );

    /**
     * Sets the rear left/right center speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetRearLeftRightCenter( const TBool aValue );

    /**
     * Sets the front wide speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetFrontWideSpeakers( const TBool aValue );

    /**
     * Sets the front high speaker allocation.
     *
     * @param aValue, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C void SetFrontHighSpeakers( const TBool aValue );

    /**
     * Get the speaker count.
     *
     * @return TUint32, total count of speakers
     *
     * @return void.
     */
    IMPORT_C TUint32 SpeakerCount() const;

    /**
     * Get the speaker allocation.
     *
     * @return TUint32, a bitmap of TSpeakerAllocation
     *
     * @return void.
     */
    IMPORT_C TUint32 SpeakerAllocation() const;

    /**
     * Get the front speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool FrontSpeakers() const;

    /**
     * Get the rear speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool RearSpeakers() const;

    /**
     * Get the LFE speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool LowFrequencyEffect() const;

    /**
     * Get the front center speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool FrontCenter() const;

    /**
     * Get the front center high speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool FrontCenterHigh() const;

    /**
     * Get the top center speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool TopCenter() const;

    /**
     * Get the rear center speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool RearCenter() const;

    /**
     * Get the front left/right center speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool FrontLeftRightCenter() const;

    /**
     * Get the rear  left/right center speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool RearLeftRightCenter() const;

    /**
     * Get the front wide speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool FrontWideSpeakers() const;

    /**
     * Get the front high speaker allocation.
     *
     * @return TBool, if true, speakers are present, if false, not present
     *
     * @return void.
     */
    IMPORT_C TBool FrontHighSpeakers() const;

public: // From base classes

    // @see CAccPolObjectBase
    virtual CAccPolObjectBase* DuplicateLC( CAccPolObjectCon* aObject = NULL );

    // @see CAccPolObjectBase
    virtual void InternalizeL( RReadStream& aStream );

    // @see CAccPolObjectBase
    virtual void ExternalizeL( RWriteStream& aStream ) const;

protected:

    /**
     * C++ default constructor.
     */
    CAccPolHdmiSpeakerAllocation( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ default constructor.
     */
    CAccPolHdmiSpeakerAllocation( const TBool aFrontSpeakers,
        const TBool aRearSpeakers,
        const TBool aLowFrequencyEffect,
        const TBool aFrontCenter,
        const TBool aFrontCenterHigh,
        const TBool aTopCenter,
        const TBool aRearCenter,
        const TBool aFrontLeftRightCenter,
        const TBool aRearLeftRightCenter,
        const TBool aFrontWideSpeakers,
        const TBool aFrontHighSpeakers );

    /**
     * C++ default constructor.
     */
    CAccPolHdmiSpeakerAllocation( const TUint32 aSpeakerAllocation );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /**
     * Speaker allocation as bitmap of TSpeakerAllocation
     */
    TUint32 iSpeakerAllocation;

    /**
     * Speaker count
     */
    TUint32 iSpeakerCount;
    };

/** Speaker allocation object array */
typedef RPointerArray<CAccPolHdmiSpeakerAllocation> RAccPolHdmiSpeakerAllocationArray;

#endif // ACCPOLSPEAKERALLOCATION_H
