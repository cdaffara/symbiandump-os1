/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of CStrtSecNoteRequestQueue class
*/



#ifndef C_STRTSECNOTEREQUESTQUEUE_H
#define C_STRTSECNOTEREQUESTQUEUE_H

#include <e32base.h>

#include "strtsecuritynotetype.h"

/*
* @publishedPartner
* @released
*/

/**
 *  A queue of TStrtSecurityNoteType items.
 *  The queue contains at most one item of each type (no duplicates).
 *  Items can be added to the end of the queue, and taken out from the beginning.
 *  An item can be removed anywhere from the queue by type.
 *
 *  lib : strtsecobs.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CStrtSecNoteRequestQueue ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CStrtSecNoteRequestQueue* NewL();

    /**
    * Destructor.
    */
    virtual ~CStrtSecNoteRequestQueue();

    /**
     * Add an item to the queue.
     * If there already is identical item in the queue, don't add the new item.
     *
     * @param aItem The item to add.
     */
    TInt Add( const TStrtSecurityNoteType aItem );

    /**
     * Remove an item from the queue, if there is an item matching the one given
     * as parameter.
     *
     * @param aItem The item to remove.
     */
    void Remove( const TStrtSecurityNoteType aItem );

    /**
     * Remove and return the first item in the queue, or ESecNoteNone if the
     * queue is empty.
     *
     * @return The item that was in front of the queue.
     */
    TStrtSecurityNoteType GetFirst();

private:

    /**
     * First phase constructor.
     */
    CStrtSecNoteRequestQueue();

    /**
     * Second phase constructor.
     */
    void ConstructL();

private: // data

    /**
     * Array for holding the queue contents.
     */
    RArray<TStrtSecurityNoteType> iArray;

#ifdef TEST_CLAYER_MACRO
	friend class CLayerTestSsmEventObserver;
#endif //TEST_CLAYER_MACRO
    };

#endif // C_STRTSECNOTEREQUESTQUEUE_H
