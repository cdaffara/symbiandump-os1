/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMMTSYREQHANDLESTORE_H
#define CMMTSYREQHANDLESTORE_H

//  INCLUDES
#include <et_tsy.h>
#include "MmTsy_timeoutdefs.h"

// FORWARD DECLARATIONS
class CMmPhoneTsy;

// CLASS DECLARATION

/**
*  CMmTsyReqHandleStore class.
*/
NONSHARABLE_CLASS( CMmTsyReqHandleStore ) : public CBase
    {
    public: //constructors and destructor

#ifdef REQHANDLE_TIMER

        /**
         * Two-phased constructor.
         * @param *aCTelObject Pointer to CTelObject
         * @param *aPhone Pointer to phone object
         * @param aNumberOfRequests 
         * @param aFirstElement
         * @return CMmTsyReqHandleStore* created object 
         */
        static CMmTsyReqHandleStore* NewL(CTelObject * aCTelObject,
                                          CMmPhoneTsy *aPhone,
                                          TInt aNumberOfRequests,
                                          TTsyReqHandle* aFirstElement);
                                           
#endif
        /**
         * Two-phased constructor.
         * @param TInt aNumberOfRequests: number of requests
         * @return CMmTsyReqHandleStore*: created object 
         */
        static CMmTsyReqHandleStore* NewL(TInt aNumberOfRequests,
            TTsyReqHandle* aFirstElement);


        /**
         * Destructor.
         */
        virtual ~CMmTsyReqHandleStore();

        // New functions

        /**
         * Get TSY request handle by index.
         *          
         * 
         * @param const TInt aIndex: index where to get the req handle.
         * return TTsyReqHandle: tsy req handle.
         */
        TTsyReqHandle GetTsyReqHandle( const TInt aIndex );

        /**
         * Sets TSY request handle for given index.
         *          
         * 
         * @param aIndex index where to store the req handle.
         * @param aTsyReqHandle req handle to be stored.
         */
        void SetTsyReqHandle( const TInt aIndex, 
                                       const TTsyReqHandle aTsyReqHandle );

        /**
         * Reset TSY request handle by index.
         *          
         * 
         * @param aIndex index which req handle to reset.
         */
        TTsyReqHandle ResetTsyReqHandle( const TInt aIndex );
        
        /**
         * Reset TSY request handle by req handle value.
         *           
         * @param aTsyReqHanle req handle to be reset.
         * return TInt EFalse if handle not found
         */
        TBool FindAndResetTsyReqHandle( const TTsyReqHandle aTsyReqHandle);
        
#ifdef REQHANDLE_TIMER

        /**
         * Sets TSY request handle for given index.
         *          
         * 
         * @param aIndex index where to store the req handle.
         * @param aTsyReqHandle req handle to be stored.
         * @param aTimeout value of the timeout
         * @param aIPC IPC number of the operation
         */
        void SetTsyReqHandle( const TInt aIndex, 
                              const TTsyReqHandle aTsyReqHandle,
                              TInt aTimeout,
                              TInt aIPC = 0 );
                              
        /**
         * Sets a new timeout for a given handle type
         *          
         * 
         * @param aIndex index where to store the req handle.
         * @param aTimeout Time out value
         */
        void PostponeTimeout( const TInt aHandle, 
                              const TInt aTimeout );
                              
        /**
         * deletes the timeout stored for the given handle
         *          
         * 
         * @param aHandle a req handle
         */
        void StopTimeout ( const TInt aHandle );

#endif        

    private:    // Data

        /**
         * Number of requests
         */         
        TInt            iNumOfRequests;

        /**
         * A pointer for request handles
         * Not own.
         */                  
        TTsyReqHandle*   iReqHandles;

#ifdef REQHANDLE_TIMER

        /**
         * Pointer to phone
         * NOt own.         
         */          
        CMmPhoneTsy *iPhone;

        /**
         * A Pointer to caller CEtelObject object
         */         
        CTelObject * iCTelObject;

#endif 
    };

#endif // CMMTSYREQHANDLESTORE_H 

//  End of file

