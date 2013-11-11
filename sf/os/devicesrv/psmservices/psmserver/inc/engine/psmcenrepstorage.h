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
* Description:  PSM CenRep storage class.
*
*/


#ifndef PSMCENREPSTORAGE_H
#define PSMCENREPSTORAGE_H

#include <e32base.h>
#include "psmstorage.h"

/**
 *  PSM CenRep Storage class
 *
 *  Handles Central Repository (aka. passive configurations ) changes during 
 *  power save mode change. 
 *
 *  @since S60 5.0
 */
class CPsmCenRepStorage : public CPsmStorage
    {
public:

    /**
     * Creates a new PSM storage.
     *
     * @return A pointer to the created object.
     */
    static CPsmCenRepStorage* NewL( TPsmsrvMode iMode, RFs& aFile );

    /**
     * Destructor.
     */
    virtual ~CPsmCenRepStorage();

    /**
	 * Starts parsing config file. This function blocks until file is parsed
 	 */
	void InitStorageL( const TUint32 aStorageUid );

    /**
     * Lists passive configuration UIDs
     */
    void ListPassiveConfigs( RPassiveConfigList& aList );

    /**
     * Lists set items of a specific passive configuration
     */
    void ListPassiveConfigSetL( RConfigInfoArray& aPsmConfigArray );

private:

    /**
     * C++ constructor.
     */
    CPsmCenRepStorage( TPsmsrvMode iMode, RFs& aFile );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Initializes root elements when storage file has been parsed
     */
    void InitRootElementsL();

    };

#endif // PSMCENREPSTORAGE_H 
