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
* Description:  Definition for audio selector unit object.
*
*/

#ifndef ACCPOLSELECTORUNIT_H
#define ACCPOLSELECTORUNIT_H

#include <accpolaudiounitbase.h>

/**
 *  Class definition for audio selector unit objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolSelectorUnit ) : public CAccPolAudioUnitBase
    {

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolSelectorUnit();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolSelectorUnit* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     * @param aUnitId, Audio unit Id of this audio unit.
     * @param aNrInPins, Count of input pins of this unit.
     *
     */
    IMPORT_C static CAccPolSelectorUnit* NewL( const TUint8 aUnitId,
                                               const TUint8 aNrInPins );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolSelectorUnit* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     * @param aUnitId, Audio unit Id of this audio unit.
     * @param aNrInPins, Count of input pins of this unit.
     *
     */
    IMPORT_C static CAccPolSelectorUnit* NewLC( const TUint8 aUnitId,
                                                const TUint8 aNrInPins );

    /**
     * Assign operator for audio selector unit class.
     *
     * @param CAccPolSelectorUnit, Const reference to base class to be copied.
     * @return CAccPolSelectorUnit, Reference to object pointer.
     */
    IMPORT_C CAccPolSelectorUnit& operator= ( const CAccPolSelectorUnit& );

    /**
     * Returns count of input pins.
     *
     * @return TUint8, Count of input pins.
     */
    IMPORT_C TUint8 NrInPins() const;

    /**
     * Duplicates object data.
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
    CAccPolSelectorUnit( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ default constructor.
     */
    CAccPolSelectorUnit( const TUint8 aUnitId,
                         const TUint8 aNrInPins );
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /**
     * Count of input pins.
     */
    TUint8 iNrInPins;

    };

#endif // ACCPOLSELECTORUNIT_H
