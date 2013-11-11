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
* Description:  Definition for input control object.
*
*/


#ifndef ACCPOLINPUTCONTROL_H
#define ACCPOLINPUTCONTROL_H

#include <accpolaudiocontrolbase.h>

/**
 *  Class definition for input control objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolInputControl ) : public CAccPolAudioControlBase
    {

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolInputControl();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolInputControl* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolInputControl* NewLC( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolInputControl* NewL( const TUint8 aUnitId );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolInputControl* NewLC( const TUint8 aUnitId );

    /**
     * Assign operator for input control class.
     *
     * @param CAccPolInputControl, Const reference to object to be copied.
     * @return CAccPolInputControl, Reference to object pointer.
     */
    IMPORT_C CAccPolInputControl& operator= ( const CAccPolInputControl& );

    /**
     * Returns input source unit Id.
     *
     * @return TUint8, Audio unit Id.
     */
    IMPORT_C TUint8 Input() const;

    /**
     * Set input source unit Id.
     *
     @ param aUnitId, Audio unit Id.
     * @return void.
     */
    IMPORT_C void SetInput( const TUint8 aUnitId );

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
    CAccPolInputControl( CAccPolObjectCon* aObject = NULL );

    /**
     * C++ constructor.
     */
    CAccPolInputControl( const TUint8 aSourceId );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:

    /**
     * Input source unit Id.
     */
    TUint8 iUnitId;

    };

#endif // ACCPOLINPUTCONTROL_H
