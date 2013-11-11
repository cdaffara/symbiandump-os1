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
* Description:  PSM configuration storage class.
*
*/


#ifndef PSMCONFIGSTORAGE_H
#define PSMCONFIGSTORAGE_H

#include <e32base.h>
#include "psmstorage.h"

/**
 *  PSM configuration Storage class
 *
 *  This storage is used when configurations for certain power save mode
 *  are needed.
 *
 *  @since S60 5.0
 */
class CPsmConfigStorage : public CPsmStorage
                    
    {
public:

    /**
     * Creates a new PSM storage.
     *
     * @return A pointer to the created object.
     */
    static CPsmConfigStorage* NewL( TPsmsrvMode iMode, RFs& aFile );
    static CPsmConfigStorage* NewLC( TPsmsrvMode iMode, RFs& aFile );

    /**
     * Destructor.
     */
    virtual ~CPsmConfigStorage();

    /**
	 * Starts parsing config file. This function blocks until file is parsed
     */
	void InitStorageL( const TUint32 aStorageUid );

protected:

    /**
     * Initializes root elements when storage file has been parsed
     */
    void InitRootElementsL();

private:

    /**
     * C++ constructor.
     */
    CPsmConfigStorage( TPsmsrvMode iMode, RFs& aFile );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data

    };

#endif // PSMCONFIGSTORAGE_H 
