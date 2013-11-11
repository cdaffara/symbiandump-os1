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
 * Description:  Definition for audio mixer unit object.
 *
 */

#ifndef ACCPOLMIXERUNIT_H
#define ACCPOLMIXERUNIT_H

#include <accpolaudiounitbase.h>

/**
 *  Class definition for audio mixer unit objects.
 *
 *  @lib accpolaudiodevicetopology.lib
 *  @since S60 5.2
 */
NONSHARABLE_CLASS ( CAccPolMixerUnit ) : public CAccPolAudioUnitBase
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolMixerUnit* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     * @param aUnitId audio unit Id of this audio unit.
     */
    IMPORT_C static CAccPolMixerUnit* NewL( const TUint8 aUnitId );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolMixerUnit* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     * @param aUnitId audio unit Id of this audio unit.
     */
    IMPORT_C static CAccPolMixerUnit* NewLC( const TUint8 aUnitId );

    /**
     * Assign operator for audio mixer unit class.
     *
     * @param CAccPolMixerUnit const reference to base class to be copied.
     * @return CAccPolMixerUnit reference to object pointer.
     */
    IMPORT_C CAccPolMixerUnit& operator= ( const CAccPolMixerUnit& );

    /**
     * Duplicates the object data.
     */
    CAccPolObjectBase* DuplicateLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Externalize object.
     */
    void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Internalize object.
     */
    void InternalizeL( RReadStream& aStream );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAccPolMixerUnit();

protected: // Functions from base classes


private:

    /**
     * C++ default constructor.
     */
    CAccPolMixerUnit( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ constructor.
     */
    CAccPolMixerUnit( const TUint8 aUnitId );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif // ACCPOLMIXERUNIT_H

