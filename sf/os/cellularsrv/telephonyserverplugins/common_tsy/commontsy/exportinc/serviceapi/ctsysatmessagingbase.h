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




/**
 @file
 @publishedPartner
 @released
*/
#ifndef CMMSATMESSAGINGBASE_H
#define CMMSATMESSAGINGBASE_H

//  INCLUDES
#include <e32base.h>
#include <et_phone.h>
#include <ctsy/serviceapi/mstktsysatservice.h>
#ifdef USING_CTSY_DISPATCHER
#include <ctsy/serviceapi/mstktsycallorigin.h>
#endif //USING_CTSY_DISPATCHER

// CONSTANTS 
_LIT( KSatToolkit,"TOOLKIT" );

#ifdef USING_CTSY_DISPATCHER
// FORWARD DECLARATIONS
class MmMessageManagerCallback;
#endif //end USING_CTSY_DISPATCHER

// CLASS DECLARATION

/**
 *  Sat messaging base class.
 *
 */

class CTsySatMessagingBase : public CSubSessionExtBase,
    public MStkTsySatService
#ifdef USING_CTSY_DISPATCHER
    , public MStkTsyCallOrigin
#endif //USING_CTSY_DISPATCHER

    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CTsySatMessagingBase* NewL()
    		    {
    		// dummy implementation
    		    return new ( ELeave ) CTsySatMessagingBase();
    		    }

        // New functions

        /**
         * From MStkTsySatService 
         * Dummy implementation for CompleteSendSmsMessage function
         *          
         * @param aError Error value
         * @return Return always KErrNone
         */ 
        virtual TInt CompleteSendSmsMessage( TInt /*aError*/ )
            {
    		// dummy implementation
            return KErrNone;
            }
    
        /**
         * From MStkTsySatService 
         * Dummy implementation for IsMoSmControlBySimActivated function
         *          
         * @return Return always EFalse
         */     
        virtual TBool IsMoSmControlBySimActivated()
            {
    		    // dummy implementation
            return EFalse;
            } 

#ifdef USING_CTSY_DISPATCHER
        /**
		 * From MStkTsyCallOrigin.h
		 * Dummy implementation for IsSimOriginatedCall
         *          
         * @param aAddress The number whose origin is required.
         * @return ETrue if call originates from a Proactive Command
         */
        virtual TBool IsSimOriginatedCall( const TDesC8& /*aAddress*/)
        	{
        	//dummy implementation
        	return EFalse;
        	}
#endif //USING_CTSY_DISPATCHER
       
        /**
         * Opens a new object identified by name.
         *          
         * @param aName Name to open
         * @return TelObject
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& /*aName*/ )
        	{
    		// dummy implementation
    		return NULL;
    		}
        
        /**
         * Opens a new object identified by new name, not supported.
         *          
         * @param aNewName Name to open
         * @return TelObject
         */
        virtual CTelObject* OpenNewObjectL( TDes& /*aNewName*/ )
    		{
    		 // dummy implementation
        	return NULL;
    		}
    
        /**
         * Command handler for server messages.
         *          
         * @param aTsyReqHandle TsyReqHandle
         * @param aIpc command identifier
         * @param aPackage Data pointer
         * @return status
         */
        virtual TInt ExtFunc( const TTsyReqHandle /*aTsyReqHandle*/, 
            const TInt /*aIpc*/,const TDataPackage& /*aPackage*/ )
        	{
        	// dummy implementation
        	return KErrNotSupported;
    		}
                            
        /**
         * ReqMode method for checking what modes this component supports
         *          
         * @param aIpc command identifier
         * @return reqmode
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt /*aIPC*/ )
		    {
    		// dummy implementation
    		return 0;
		    }
    
        /**
         * Cancels the ongoing service
         *
         * @param aIpc command identifier
         * @param aTsyReqHandle TsyReqHandle
         * @return status
         */
        virtual TInt CancelService( const TInt /*aIpc*/, 
                                    const TTsyReqHandle /*aTsyReqHandle*/ )
		    {
    		// dummy implementation
    		return KErrNone;
		    }
                                    
        /**        
         * Initialize method
         *          
         * @return none
         */
        virtual void Init()
		    {
		    // dummy implementation
		    }
    
        /**
         * Register given command, checks if the given command is supported
         *         
         * @param aIpc command identifier
         * @return status
         */
        virtual TInt RegisterNotification( const TInt /*aIpc*/ )
		    {
    		// dummy implementation
    		return KErrNotSupported;
		    }
        
        /**
         * DeRegister given command, checks if the given command is supported
         *          
         * @param aIpc command identifier
         * @return status
         */
        virtual TInt DeregisterNotification( const TInt /*aIpc*/ )
		    {
    		// dummy implementation
    		return KErrNotSupported;
		    }
    
        /**
         * Returns number of slots to be used for given IPC
         *          
         * @param aIpc command identifier
         * @return status
         */
        virtual TInt NumberOfSlotsL( const TInt /*aIpc*/ )
		    {
    		// dummy implementation
    		return KErrNotSupported;
		    }
#ifdef USING_CTSY_DISPATCHER
        /**
         *  Returns a pointer to the message manager callback installed
         * 
         *  @return Pointer to the message manager callback
         */
        virtual MmMessageManagerCallback* GetMessageManagerCallback()
			{
			// dummy implementation
			return NULL;
			}
#endif //end USING_CTSY_DISPATCHER
    };

#endif  // CMMSATMESSAGINGBASE_H

// End of File
