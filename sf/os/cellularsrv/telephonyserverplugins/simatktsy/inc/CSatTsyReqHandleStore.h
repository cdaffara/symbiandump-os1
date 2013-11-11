/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : CSatTsyReqHandleStore.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* The header file of the CSatTsyReqHandleStore class.
* Version     : 1.0
*
*/




#ifndef CSATTSYREQHANDLESTORE_H
#define CSATTSYREQHANDLESTORE_H

// INCLUDES
#include <et_tsy.h>

// FORWARD DECLARATIONS
class CSatTsy;

// DESCRIPTION
/**
* Implementation of the CSatTsyReqHandleStore class.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( CSatTsyReqHandleStore ) : public CBase
    {

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aNumberOfRequests: number of requests
        * @param aFirstElement: Pointer to the first element
        * @return created object 
        */
        static CSatTsyReqHandleStore* NewL(TInt aNumberOfRequests,
            TTsyReqHandle* aFirstElement);

        /**
        * Destructor.
        */
        ~CSatTsyReqHandleStore();

    private:  

        /**
        * Constructor is accessible to inheriting classes.
        */
        CSatTsyReqHandleStore();

        /**
        * By default EPOC constructor is private.
        * @return none
        */
        void ConstructL();

    public: // New functions

        /**
        * Get TSY request handle by index.
        * @param aIndex: index where to get the req handle.
        * @return tsy req handle.
        */
        TTsyReqHandle TsyReqHandle( const TInt aIndex );

        /**
        * Sets TSY request handle for given index.
        * @param aIndex: index where to store the req handle.
        * @param req handle to be stored.
        * @return None
        */
        void SetTsyReqHandle( const TInt aIndex, 
                              const TTsyReqHandle aTsyReqHandle );
       
        /**
        * Reset TSY request handle by index.
        * @param aIndex: index which req handle to reset.
        * @return tsy req handle
        */
        TTsyReqHandle ResetTsyReqHandle( const TInt aIndex );

    private: // Data

        // Number of requests
        TInt iNumOfRequests;

        // A pointer for request handles
        TTsyReqHandle* iReqHandles;

    };

#endif // CSATTSYREQHANDLESTORE_H 

//  End of File

