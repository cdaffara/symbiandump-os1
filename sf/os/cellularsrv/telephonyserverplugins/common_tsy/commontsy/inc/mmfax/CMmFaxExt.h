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



#ifndef CMMFAXEXT_H
#define CMMFAXEXT_H

//  INCLUDES
#include "CMmFaxExtInterface.h"
#include "cmmtsyfax.h"
#include "faxstd.h"
#include "cmmfaxcalltsy.h"

// CONSTANTS
_LIT(KFaxServerName,"FaxSvr.dll");

// DATA TYPES
typedef CFaxSession* (*TFaxServerEntry)();

// CLASS DECLARATION

/* 
*  CMmFaxExt contains extended fax call functionality. 
*/ 
class CMmFaxExt : public CMmFaxExtInterface
    {
    friend class CMmTsyFax;

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static  CMmFaxExt* NewL( CMmCallTsy* aMmCall );
            
        /**
         * Destructor.
         */
        virtual ~CMmFaxExt();

    public: // New functions


        /**
         * Start waiting for a call. Called from CmmCallTSY object
         * AnswerIcomingCall method.
         *          
         *
         * @param aTsyReqHandle, reqhandle
         * @return Error value
         */
        virtual TInt AnswerIncomingCall( const TTsyReqHandle& aTsyReqHandle );

        /**
         * Start Fax call dialing. Call from CMmCallTsy object Dial method.
         *          
         *
         * @param aTsyReqHandle, reqhandle
         * @param aTelNumber, descriptor including telnumber
         * @return Error value
         */
        virtual TInt Dial( const TTsyReqHandle& aTsyReqHandle,
            TDesC* aTelNumber );

        /**
         * Set terminate flag true on CMmTsyFax object.
         *          
         *
         */
        virtual void DialCancel();

        /**  
         * CMmFaxTsy object creation.
         *          
         *
         * @param aName, object name
         * @return CTelObject*, pointer to new object
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /**
         * Close fax session.
         *          
         *
         */
        virtual void HangUp();

        /**
         * Transfers fax settings to local variables.
         *          
         *
         * @param aSettings, pointer to settings
         * @return Error value
         */
        virtual TInt SetFaxSettings(
            const RCall::TFaxSessionSettings* aSettings );

        /**
         * Transfers fax settings to from variables.
         *          
         *
         * @param aSettings, pointer to settings
         * @return Error value
         */
        virtual TInt GetFaxSettings( RCall::TFaxSessionSettings* aSettings );

        /**
         * Completes ongoing write/read operation on CMmFaxCompletion       
         * object. 
         *          
         *
         * @param aError, error value
         */
        virtual void CompleteOperation( TInt aError );

        /**
         * Sets fax request handle to CMmFaxCompletion object.
         *
		* @param aTsyReqHandle, reqhandle
         * @param aTelObject pointer to Etel object
         */
        void ConfigureCompletion( const TTsyReqHandle aTsyReqHandle, 
            CTelObject* aTelObject );

    private:
		
		/**
         * C++ default constructor.
         */
         CMmFaxExt( CMmCallTsy* aCall );
         
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
         
        /**
         * Start Fax Server connection handling.
         *          
         *
         * @param aError, error value
         */
        TInt FaxConnectHandler();
       
        /**
         * Returns pointer to Fax Session.
         *          
         *
         */
        CFaxSession* GetFaxSession();

        /**
         * FaxSession creation.
         *          
         *
         */
        void GetFaxBaseL();
        
        /**
         * Sets fax settings and opens fax session.
         *          
         *
         * @param aTelNumber, telnumber
         * @param aFaxMode, faxmode
         */
        TInt OpenFaxServer( const TDesC* aTelNumber, const TFaxMode aFaxMode );

        /**
         * Close fax session.
         *          
         *
         */
        void CleanUpFaxServer();

        /**
         * Returns Call status;
         *          
         *
         */
        RCall::TStatus GetCallStatus() const;

    private:     // Data
    
        /**
         * Pointer to owner CMmFaxCallTsy
         * Not own.         
         */        
        CMmFaxCallTsy* iMmCall;

        /**
         * Pointer to CMmTSyFax object
         * Not own.         
         */
        CMmTsyFax* iFax;

        /**
         * Pointer to CFaxSession object
         * Not own.         
         */
        CFaxSession* iFaxSession; 
        
        /**
         * Pointer to CMmFaxCompletion object
         * Not own.         
         */         
        CMmFaxCompletion* iFaxCompletion;
        
        /**
         * Fax server library         
         */                
        RLibrary iFaxServerLib;

        /**
         * Fax settings         
         */                
        RCall::TFaxSessionSettings iFaxSettings;

        /**
         * DataPort loaned         
         */                
        TBool iDataPortLoaned;
    };

#endif      // CMMFAXEXT_H 

// End of File
