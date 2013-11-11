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
 * Description:  Definition for audio ouput terminal object.
 *
 */

#ifndef ACCPOLOUTPUTTERMINAL_H
#define ACCPOLOUTPUTTERMINAL_H

#include <accpolterminalbase.h>

/**
 *  Class definition for audio output terminal objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolOutputTerminal ) : public CAccPolTerminalBase
    {

public:

    /**
     * Ouput terminal types
     */
    enum TOutputTerminalType
        {
        EAPTNone = 0,
        EAPTStream,
        EAPTHeadphone,
        EAPTSpeaker
        };

    /**
     * Destructor.
     */
    virtual ~CAccPolOutputTerminal();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolOutputTerminal* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     * @param aUnitId, Audio unit Id of this audio unit.
     * @param aTerminalType, Characterize the type of terminal, see TOutputTerminalType.
     * @param aSourceId, Id of Audio unit of terminal which this audio unit is connected.
     *
     */
    IMPORT_C static CAccPolOutputTerminal* NewL( const TUint8 aUnitId,
        const TOutputTerminalType aOutputTerminalType,
        const TUint8 aSourceId );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolOutputTerminal* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     * @param aUnitId, Audio unit Id of this audio unit.
     * @param aTerminalType, Characterize the type of terminal, see TOutputTerminalType.
     * @param aSourceId, Id of the audio unit of terminal which this audio unit is connected.
     *
     */
    IMPORT_C static CAccPolOutputTerminal* NewLC( const TUint8 aUnitId,
        const TOutputTerminalType aOutputTerminalType,
        const TUint8 aSourceId );

    /**
     * Assign operator for audio output terminal class.
     *
     * @param CAccPolOutputTerminal, Const reference to base class to be copied.
     * @return CAccPolOutputTerminal, Reference to object pointer.
     */
    IMPORT_C CAccPolOutputTerminal& operator= ( const CAccPolOutputTerminal& );

    /**
     * Returns output terminal type.
     *
     * @return TOutputTerminalType, Output terminal type.
     */
    IMPORT_C TOutputTerminalType OutputTerminalType() const;

    /**
     * Set output terminal type.
     *
     * @param aOutputTerminalType, Output terminal type.
     * @return void.
     */
    IMPORT_C void SetOutputTerminalType( TOutputTerminalType aOutputTerminalType );

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
    CAccPolOutputTerminal( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ constructor.
     */
    CAccPolOutputTerminal( const TUint8 aUnitId,
        const TOutputTerminalType aTerminalType,
        const TUint8 aSourceId );
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /**
     * Output terminal type
     */
    TOutputTerminalType iOuputTerminalType;

    };

#endif // ACCPOLOUTPUTTERMINAL_H
