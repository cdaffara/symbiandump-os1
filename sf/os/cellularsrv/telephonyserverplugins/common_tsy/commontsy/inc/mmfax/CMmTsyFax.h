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



#ifndef CMMTSYFAX_H
#define CMMTSYFAX_H

//  INCLUDES
#include "cmmfaxcompletion.h"
#include "et_phone.h"

// FORWARD DECLARATIONS
class CMmFaxExt;

//  CLASS DEFINITIONS 

/** 
*  CMmTsyFax contains Fax operation methods.  
*/

class CMmTsyFax
        : public CFaxBase
    {
    public:

        /**
         * Two-phased constructor.
         */
        static CMmTsyFax* NewL( CMmFaxExt* aFaxExt );

        /**
         * Destructor.
         */
        ~CMmTsyFax();
   
    public: //New functions

        /**
         * Init. Etel server call this. Do nothing.
         *         
         *
         */
        virtual void Init();
        
        /**
         * Utility to cleanup stack.
         *         
         *
         * @param aObj, pointer to object to be closed
         */
        static void CloseFax( TAny* aObj );
        
        /**
         * Request mode check. Etel server calls this.
         *         
         *
         * @param aIpc, IPC value
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );
        
        /**
         * Read fax data into descriptor, asynchronously.
         *         
         *
         * @param aTsyReqHandle, reqhandle
         * @param aFaxPage, pointer to object to be read
         * @return Error value
         */
        virtual TInt Read( const TTsyReqHandle aTsyReqHandle,
            TDes8* aFaxPage );
        
        /**
         * Send fax data in descriptor to fax module, asynchronously.
         *         
         *
         * @param aTsyReqHandle, reqhandle
         * @param aFaxPage, pointer to object to be read
         * @return Error value
         */
        virtual TInt Write( const TTsyReqHandle aTsyReqHandle,
            TDesC8* aFaxPage );
        
        /**
         * Send request for notification of end of page to fax module.
         *         
         *
         * @param aTsyReqHandle, reqhandle
         * @return Error value
         */
        virtual TInt WaitForEndOfPage( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Cancel the entire fax session.
         *         
         *
         * @param aTsyReqHandle, reqhandle
         * @return Error value
         */
        virtual TInt TerminateFaxSession( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Registers notification in use.
         *         
         *
         * @param aIpc, IPC value
         * @return Error value
         */
       virtual TInt RegisterNotification( const TInt aIpc );
        
        /**
         * Deregisters notification in use.
         *         
         *
         * @param aIpc, IPC value
         * @return Error value
         */
        virtual TInt DeregisterNotification( const TInt aIpc );
        
        /**
          * Setup the terminate fax flag.
          *         
          *
          */
		void Terminate();
    
    private:
    
		/**
          * C++ default constructor.
          */        
        CMmTsyFax( CMmFaxExt* aFaxExt );
        
        /**
          * By default Symbian 2nd phase constructor is private.
          */
        void ConstructL();
        
    public: //Data
    
        // direction of fax data flow
    	enum TDataDirection
        	{
        	ESendData,
        	EReceiveData,
        	EUnknown
        	};	
    
    private: // Data

        /**
         * Fax extension object pointer.
         * Own.         
         */
        CMmFaxExt* iFaxExt;

        /**
         * Fax call direction.
         */        
        TDataDirection iDataDirection;

        /**
         * Fax terminate flag.
         */        
        TBool iTerminate;
    };

#endif    // CMMTSYFAX_H  
            
// End of File
