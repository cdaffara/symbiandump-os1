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
 * Description:  Base class for audio terminal objects.
 *
 */

#ifndef ACCPOLTERMINALBASE_H
#define ACCPOLTERMINALBASE_H

#include <accpolaudiounitbase.h>

/**
 *  Class definition for audio terminal objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS ( CAccPolTerminalBase ) : public CAccPolAudioUnitBase
    {
public:

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAccPolTerminalBase();

    /**
     * Assign operator for audio terminal base class.
     *
     * @param CAccPolTerminalBase, Const reference to base class to be copied.
     * @return CAccPolTerminalBase, Reference to object pointer.
     */
    IMPORT_C CAccPolTerminalBase& operator= ( const CAccPolTerminalBase& );

protected:

    /**
     * Externalize object ( intended internal use only ).
     */
    IMPORT_C void ExternalizeTerminalBaseL( RWriteStream& aStream ) const;

    /**
     * Externalize object ( intended internal use only ).
     */
    IMPORT_C void InternalizeTerminalBaseL( RReadStream& aStream );

protected:

    /**
     * C++ default constructor.
     */
    IMPORT_C CAccPolTerminalBase( const TTopologyConObjectType aObjecType,
        CAccPolObjectCon* aObject = NULL );

    /**
     * C++ default constructor.
     */
    IMPORT_C CAccPolTerminalBase( const TTopologyConObjectType aObjecType,
        const TUint8 aUnitId );

    /**
     * C++ default constructor.
     */
    IMPORT_C CAccPolTerminalBase( const TTopologyConObjectType aObjecType,
        const TUint8 aUnitId,
        const TUint8 aSourceId );
    };

#endif // ACCPOLTERMINALBASE_H
