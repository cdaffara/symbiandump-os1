/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef _CMMENSTOREGSMEXT_H_
#define _CMMENSTOREGSMEXT_H_

//  INCLUDES
#include <et_phone.h>
#include "cmmenstoreextinterface.h"

// FORWARD DECLARATIONS
class CMmENStoreTsy;

// CLASS DECLARATION

/**
 *  CMmENStoreGsmExt contains mode-dependent phone functionality for GSM phone
 *  Extension request handles are stored in MM Phone object.
 */
NONSHARABLE_CLASS( CMmENStoreGsmExt ) : public CMmENStoreExtInterface
    {
    public: // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CMmENStoreGsmExt();

        /**
         * Destructor
         */
        virtual ~CMmENStoreGsmExt();

        /**
         * Creates a new GSM ENStore specific object instance
         *          
         *
         * @param aMmENStoreTsy pointer to EN store TSY
         * @return pointer to EN store GSM extension
         */
        static CMmENStoreGsmExt* NewL( CMmENStoreTsy* aMmENStoreTsy );

        /**
         * Read reguest of EN store numbers
         *          
         *
         * @param aIpc value
         * @param aIndext index to be read
         * @return success or failure
         */
        virtual TInt Read( TInt aIpc, TInt aIndex );

    private:

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:     // Data

        /**
         * Pointer to the Phone TSY object
         */         
        CMmENStoreTsy* iMmENStoreTsy;
    };

#endif /* _CMMENSTOREGSMEXT_H_ */

// End of File
