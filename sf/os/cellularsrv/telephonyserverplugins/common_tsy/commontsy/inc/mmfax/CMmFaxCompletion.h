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



#ifndef CMMFAXCOMPLETION_H
#define CMMFAXCOMPLETION_H

//  INCLUDES
#include "et_tsy.h"
#include "faxstd.h"
#include "faxserv.h"

// FORWARD DECLARATIONS
class CMmCallTsy;
class CTelObject;

// CLASS DECLARATION

/**
*  Internal Fax completion operations in MMTSY.
*/
class CMmFaxCompletion : public MFaxCompletionBase, public CBase
    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        ~CMmFaxCompletion();

    	// New functions
        
        /**
         * Completes ongoing write/read operation.
         *          
         *
         * @param aError, error value
         */
        void CompleteOperation( TInt aError );

        /**
         * Configure completion object
         *          
         *
         * @param aTsyReqHandle, request handle 
         * @param aTelObject 
         */
        void Configure( const TTsyReqHandle aTsyReqHandle, 
            CTelObject* aTelObject);
        
        /**
         * Last ring of Cadence and Time.
         *          
         *
         * @param aCadence, Cadence
         * @param aTime, Time
         */
        virtual void GetCadenceAndTimeOfLastRing(
            TTimeIntervalMicroSeconds& aCadence, TTime& aTime );
        
        /**
         * Connection of Fax Reception completed.
         *          
         *
         * @param aError, error value
         */
        virtual void RxConnectComplete( TInt aError );
        
        /**
         * Fax Reception Data completed.
         *          
         *
         * @param aError, error value
         */
        virtual void RxFaxDataComplete( TInt aError );
        
        /**
         * Fax Reception's PostPage transfered.
         *          
         *
         * @param aError, error value
         */
        virtual void RxPostPageComplete( TInt aError );

        /**
         * Connection of Fax Transmission completed.
         *          
         *
         * @param aError, error value
         */
        virtual void TxConnectComplete( TInt aError );

        /**
         * Fax Tansmission Data completed.
         *          
         *
         * @param aError, error value
         */
        virtual void TxFaxDataComplete( TInt aError );

        /**
         * Fax Transmission's PostPage transfered.
         *          
         *
         * @param aError, error value
         */
        virtual void TxPostPageComplete( TInt aError );
       
	private:
		
		/**
          * C++ default constructor.
          */
        CMmFaxCompletion();
        
        /**
          * Checks progress information chunk and fills 
          *           
          * iAnswerBack field
          *
          * @param aMmCall, ponter to Call object
          */
        void FillAnswerback( CMmCallTsy* aMmCall );
		
    private:  //Data

        /**
         * Used for client-side synchronous requests
         */        
        TTsyReqHandle iTsyReqHandle;

        /**
         * TelObject.
         */                
        CTelObject* iTelObject;
    };

#endif      // CMMFAXCOMPLETION_H 
            
// End of File

