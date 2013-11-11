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
 *
 */

#ifndef ACCPOLHDMIVIDEOFORMAT_H
#define ACCPOLHDMIVIDEOFORMAT_H

// SYSTEM INCLUDE FILES
#include <accpolobjectbase.h>
#include <tvoutconfigdef.h>

// USER INCLUDE FILES

// FORWARD DECLARATIONS

// DATA TYPES

// CLASS DECLARATION

/**
 * Describes the used video format.
 *
 * @lib accpolaudiodevicetopology.lib
 * @since S60 5.2
 */
NONSHARABLE_CLASS( CAccPolHdmiVideoFormat ): public CAccPolObjectBase
    {
public:

    /**
     * Symbian two phased constructor.
     *
     * @param None.
     * @return CAccPolHdmiVideoFormat
     */
    IMPORT_C static CAccPolHdmiVideoFormat* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Symbian two phased constructor.
     *
     * @param aCeaMode, Fixed CEA mode.
     * @param aDmtMode, Fixed DMT mode.
     * @param aPixelRepeat, Pixel repeat value.
     * @param aInterlaced, Is video interlaced.
     * @return CAccPolHdmiVideoFormat
     */
    IMPORT_C static CAccPolHdmiVideoFormat* NewL( TFixedModeCea aCeaMode,
        TFixedModeDmt aDmtMode,
        TUint8 aPixelRepeat,
        TBool aInterlaced );

    /**
     * Symbian two phased constructor.
     *
     * @param None.
     * @return CAccPolHdmiVideoFormat
     */
    IMPORT_C static CAccPolHdmiVideoFormat* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Symbian two phased constructor.
     *
     * @param aCeaMode, Fixed CEA mode.
     * @param aDmtMode, Fixed DMT mode.
     * @param aPixelRepeat, Pixel repeat value.
     * @param aInterlaced, Is video interlaced.
     * @return CAccPolHdmiVideoFormat
     */
    IMPORT_C static CAccPolHdmiVideoFormat* NewLC( TFixedModeCea aCeaMode,
        TFixedModeDmt aDmtMode,
        TUint8 aPixelRepeat,
        TBool aInterlaced );

    /**
     * Assign operator for audio HDMI sink class.
     *
     * @param CAccPolHdmiSink, Const reference to base class to be copied.
     * @return CAccPolHdmiSink, Reference to object pointer.
     */
    IMPORT_C CAccPolHdmiVideoFormat& operator= ( const CAccPolHdmiVideoFormat& );

    /**
     * C++ destructor.
     */
    virtual ~CAccPolHdmiVideoFormat();
    
public:
    
    /**
     * Set supported CEA fixed mode.
     * 
     * @param aMode CEA fixed mode.
     * @return None.
     */
    IMPORT_C void SetCeaFixedMode( TFixedModeCea aMode );
    
    /**
     * Set supported DMT fixed mode.
     * 
     * @param aMode DMT fixed mode.
     * @return None.
     */
    IMPORT_C void SetDmtFixedMode( TFixedModeDmt aMode );
    
    /**
     * Set pixel repeat value.
     * 
     * @param aRepeat Repeat value.
     * @return None.
     */
    IMPORT_C void SetPixelRepeat( TUint8 aRepeat );
    
    /**
     * Set scan mode.
     * 
     * @param aMode ETrue if interlaced, otherwise progressive.
     * @return None.
     */
    IMPORT_C void SetInterlaced( TBool aInterlaced );

    /**
     * Set bit mask of supported pixel repetition times.
     * 
     * @param aPixelRepeatBitMask supported pixel repeat values.
     *      Bitmasks from KPixelSentOnce to KPixelSent10Times.
     * @return void.
     */
    IMPORT_C void SetSupportedPixelRepetitions( TUint16 aPixelRepeatBitMask );

    /**
     * Return CEA fixed mode.
     * 
     * @param None.
     * @return TFixedModeCea.
     */
    IMPORT_C TFixedModeCea CeaFixedMode() const;
    
    /**
     * Return DMT fixed mode.
     * 
     * @param None.
     * @return TFixedModeDmt.
     */
    IMPORT_C TFixedModeDmt DmtFixedMode() const;
    
    /**
     * Return pixel repeat value.
     * 
     * @param None.
     * @return TUint8.
     */
    IMPORT_C TUint8 PixelRepeat() const;
    
    /**
     * Return scan mode.
     * 
     * @param None.
     * @return ETrue if interlaced, otherwise progressive.
     */
    IMPORT_C TBool Interlaced() const;

    /**
     * Return bit mask of supported pixel repetition times.
     * 
     * @param None.
     * @return TUint16 Bitmasks from KPixelSentOnce to KPixelSent10Times.
     *      Several bits can be set.
     */
    IMPORT_C TUint16 SupportedPixelRepetitions() const;
    
public: // From base classes

    // @see CAccPolObjectBase
    virtual CAccPolObjectBase* DuplicateLC( CAccPolObjectCon* aObject = NULL );

    // @see CAccPolObjectBase
    virtual void InternalizeL( RReadStream& aStream );

    // @see CAccPolObjectBase
    virtual void ExternalizeL( RWriteStream& aStream ) const;

private:

    CAccPolHdmiVideoFormat( CAccPolObjectCon* aObject = NULL );
    CAccPolHdmiVideoFormat( TFixedModeCea aCeaMode,
        TFixedModeDmt aDmtMode,
        TUint8 aPixelRepeat,
        TBool aInterlaced );
    void ConstructL();

private: // Data
    
    /** CEA fixed mode */
    TFixedModeCea iFixedModeCea;
    
    /** DMT fixed mode */
    TFixedModeDmt iFixedModeDmt;
    
    /** Pixel repeat */
    TUint8 iPixelRepeat;
    
    /** Interlaced/Progressive */
    TBool iInterlaced;
    
    ///Bit mask of supported pixel repetation. 
    // Bitmasks from KPixelSentOnce to KPixelSent10Times.
    TUint16 iPixelRepeatBitMask;

    };

/** Video format object array */
typedef RPointerArray<CAccPolHdmiVideoFormat> RAccPolHdmiVideoFormatArray;

#endif // ACCPOLHDMIVIDEOFORMAT_H
