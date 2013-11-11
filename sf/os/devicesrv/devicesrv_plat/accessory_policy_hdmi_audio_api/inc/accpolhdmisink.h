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
 * Description:  HDMI sink object definition
 *
 */

#ifndef ACCPOLHDMISINK_H
#define ACCPOLHDMISINK_H

#include <accpolobjectbase.h>

/**
 *  Describes the basic information of the HDMI sink.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS( CAccPolHdmiSink ) : public CAccPolObjectBase
    {

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolHdmiSink();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiSink* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     *
     */
    IMPORT_C static CAccPolHdmiSink* NewL( const TBool aBasicAudioSupport,
        const TBool aHdcpSupport );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiSink* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     *
     */
    IMPORT_C static CAccPolHdmiSink* NewLC( const TBool aBasicAudioSupport,
        const TBool aHdcpSupport );

    /**
     * Assign operator for audio HDMI sink class.
     *
     * @param CAccPolHdmiSink, Const reference to base class to be copied.
     * @return CAccPolHdmiSink, Reference to object pointer.
     */
    IMPORT_C CAccPolHdmiSink& operator= ( const CAccPolHdmiSink& );

    /**
     * Returns basic audio support.
     *
     * @return TBool, Basic audio support.
     */
    IMPORT_C TBool BasicAudioSupport() const;

    /**
     * Returns HDCP support.
     *
     * @return TBool, HDCP support.
     */
    IMPORT_C TBool HdcpSupport() const;

    /**
     * Set basic audio support
     *
     * @param TBool, basic audio support
     * @return void.
     */
    IMPORT_C void SetBasicAudioSupport( TBool aBasicAudioSupport );

    /**
     * Set HDCP support
     *
     * @param TBool, HDCP support
     * @return void.
     */
    IMPORT_C void SetHdcpSupport( TBool aHdcpSupport );

public: // From base classes

    // @see CAccPolObjectBase
    virtual CAccPolObjectBase* DuplicateLC( CAccPolObjectCon* aObject = NULL );

    // @see CAccPolObjectBase
    virtual void InternalizeL( RReadStream& aStream );

    // @see CAccPolObjectBase
    virtual void ExternalizeL( RWriteStream& aStream ) const;

private:

    /**
     * C++ default constructor.
     */
    CAccPolHdmiSink( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ constructor.
     */
    CAccPolHdmiSink( const TBool aBasicAudioSupport,
        const TBool aHdcpSupport );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /** BASIC audio is uncompressed 2 channel digital audio: 2 channel LPCM, 32, 44, 48kHz, 16 bps */
    TBool iBasicAudioSupport;

    /** Sink level -> audio needs this -> from TV-out config? */
    TBool iHdcpSupport;
    };

/** HDMI sink object array */
typedef RPointerArray<CAccPolHdmiSink> RAccPolHdmiSinkArray;

#endif // ACCPOLHDMISINK_H
