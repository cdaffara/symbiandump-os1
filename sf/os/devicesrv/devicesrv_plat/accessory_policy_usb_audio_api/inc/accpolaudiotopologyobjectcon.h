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
 * Description:  Container for accessory audio topology objects.
 *
 */

#ifndef ACCPOLAUDIOTOPOLOGYOBJECTCON_H
#define ACCPOLAUDIOTOPOLOGYOBJECTCON_H

#include <accpolobjectcon.h>
#include <accpolinputterminal.h>
#include <accpoloutputterminal.h>
#include <accpolobjecttypes.h>

class CAccPolAudioUnitBase;

/**
 *  Stores an accessory audio topology objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS( CAccPolAudioTopologyObjectCon ) : public CAccPolObjectCon
    {

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolAudioTopologyObjectCon();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolAudioTopologyObjectCon* NewL(
        CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolAudioTopologyObjectCon* NewLC(
        CAccPolObjectCon* aObject = NULL );

    /**
     * Get objects according to the object type.
     * Supported object types: see accpolaudiotopologytypes.h.
     *
     * @param aObjectType, Object type.
     * @param aObjects, Includes objects which are fetched from object container.
     * @return void
     */
    IMPORT_C void GetObjectsByTypeL( TTopologyConObjectType aObjectType,
        RPointerArray<CAccPolObjectBase>& aObjects ) const;

    /**
     * Get objects according to the Unit Id.
     * Supported object types: see accpolaudiotopologytypes.h.
     *
     * @param aUnitId, Unit Id.
     * @param aObject, Contains an reference to the object pointer, otherwise NULL.
     * @return TBool, ETrue if object found, otherwise EFalse.
     */
    IMPORT_C TBool ObjectL( TUint8 aUnitId, CAccPolAudioUnitBase*& aObject ) const;

    /**
     * Get input terminal objects by type.
     *
     * @param aType, Input terminal type. See accpolinputterminal.h.
     * @param aObject, Contains an reference to the object pointer, otherwise NULL.
     * return TBool, ETrue if object found, otherwise EFalse.
     */
    IMPORT_C TBool InputTerminalL( CAccPolInputTerminal::TInputTerminalType aType,
        CAccPolAudioUnitBase*& aObject ) const;

    /**
     * Get output terminal objects by type.
     *
     * @param aType, Output terminal type. See accpoloutputterminal.h.
     * @param aObject, Contains an reference to the object pointer, otherwise NULL.
     * return TBool, ETrue if object found, otherwise EFalse.
     */
    IMPORT_C TBool OutputTerminalL( CAccPolOutputTerminal::TOutputTerminalType aType,
        CAccPolAudioUnitBase*& aObject ) const;

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

public:

    /**
     * Gets all audio units from object container.
     */
    void GetAudioUnitsL( RPointerArray<CAccPolAudioUnitBase>& aAudioUnits ) const;

private:

    /**
     * C++ default constructor.
     */
    CAccPolAudioTopologyObjectCon( CAccPolObjectCon* aObject = NULL );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif // ACCPOLAUDIOTOPOLOGYOBJECTCON_H
