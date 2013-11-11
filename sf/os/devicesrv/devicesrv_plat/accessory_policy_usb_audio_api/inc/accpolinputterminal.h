/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Definition for audio input terminal object.
 *
 */

#ifndef ACCPOLINPUTTERMINAL_H
#define ACCPOLINPUTTERMINAL_H

#include <accpolterminalbase.h>

/**
 *  Class definition for audio input terminal objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolInputTerminal ) : public CAccPolTerminalBase
    {

public:

    /**
     * Input terminal types
     */
    enum TInputTerminalType
        {
        EAPTNone = 0,
        EAPTStream,
        EAPTMicrophone,
        EAPTRadioReceiver
        };

    /**
     * Destructor.
     */
    virtual ~CAccPolInputTerminal();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolInputTerminal* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolInputTerminal* NewL( const TUint8 aUnitId,
        const TInputTerminalType aInputTerminalType,
        TUint8 aChannelCount,
        TUint16 aChannelConfig );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolInputTerminal* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolInputTerminal* NewLC( const TUint8 aUnitId,
        const TInputTerminalType aInputTerminalType,
        TUint8 aChannelCount,
        TUint16 aChannelConfig );

    /**
     * Assign operator for audio input terminal class.
     *
     * @param CAccPolInputTerminal, Const reference to base class to be copied.
     * @return CAccPolInputTerminal, Reference to object pointer.
     */
    IMPORT_C CAccPolInputTerminal& operator= ( const CAccPolInputTerminal& );

    /**
     * Set a logical output channels count.
     *
     * @param aChannelCount, Channel count.
     * @return void.
     */
    IMPORT_C void SetChannelCount( TUint8 aChannelCount );

    /**
     * Returns a logical output channels count.
     *
     * @return TUint8, Channel count.
     */
    IMPORT_C TUint8 ChannelCount() const;

    /**
     * Set channel config. Describes the spatial location
     * of the logical channel.
     * See predefined channels in accpolaudiotopologytypes.h.
     *
     * @param aChannelConfig, Channel config.
     * @return void.
     */
    IMPORT_C void SetChannelConfig( TUint32 aChannelConfig );

    /**
     * Return channel config. Describes the spatial location
     * of the logical channel.
     * See predefined channels in accpolaudiotopologytypes.h.
     *
     * @return TUint32, Channel config.
     */
    IMPORT_C TUint32 ChannelConfig() const;

    /**
     * Tells whether the supported logical channels are given as parameter are
     * suppported.
     *
     * @param aChannelConfigBitMask, Logical channel(s) bitmask definition.
     *        Can contain one or more definitions if the given parameter is
     *        a result of OR -bit operation, for instance
     *        TUint32 KParamBitMask = KAccLeftFrontChannel | KAccRightFrontChannel.
     *        Bitmasks are defined in accpolaudiotopologytypes.h.
     * @return TBool indicating whether logical channels are supported (Etrue) or not (EFalse).
     */
    IMPORT_C TBool ChannelConfig( TUint32 aChannelConfigBitMask ) const;

    /**
     * Returns input terminal type.
     *
     * @return TInputTerminalType, Input terminal type.
     */
    IMPORT_C TInputTerminalType InputTerminalType() const;

    /**
     * Set input terminal type.
     *
     * @param aInputTerminalType, Input terminal type.
     * @return void.
     */
    IMPORT_C void SetInputTerminalType( TInputTerminalType aInputTerminalType );

    /**
     * Duplicates the object data.
     *
     * @return CAccPolObjectBase*, Pointer to duplicated object.
     */
    CAccPolObjectBase* DuplicateLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Externalize object.
     *
     * @param aStream, Write stream object.
     * @return void.
     */
    void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Internalize object.
     *
     * @param aStream, Read stream object.
     * @return void.
     */
    void InternalizeL( RReadStream& aStream );

private:

    /**
     * C++ default constructor.
     */
    CAccPolInputTerminal( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ constructor.
     */
    CAccPolInputTerminal( const TUint8 aUnitId,
        const TInputTerminalType ainputTerminalType,
        TUint8 aChannelCount,
        TUint16 aChannelconfig );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /**
     * Input terminal type.
     */
    TInputTerminalType iInputTerminalType;

    /**
     * Channel count.
     */
    TUint8 iChannelCount;

    /**
     * Channel config
     */
    TUint32 iChannelConfig;
    };

#endif // ACCPOLINPUTTERMINAL_H
