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



#ifndef CMMCONFERENCECALLEXTINTERFACE_H
#define CMMCONFERENCECALLEXTINTERFACE_H

//  INCLUDES
#include <etelmm.h>

// FORWARD DECLARATIONS
class CMmCallList;
class CMmDataPackage;

// CLASS DECLARATION
/**
 *  CMmConferenceCallExtInterface is used to provide an interface for 
 *  protocol-specific conference call extensions
 */
class CMmConferenceCallExtInterface : public CBase
    {
        public: // New functions

        /**        
         * This method allows clients to create the initial conference call
         *          
         *
         * @param RMobileConferenceCall::TMobileConferenceStatus: CC status
         * @param CMmCallList*: call list
         * @return KErrNotSupported: default implementation of the interface
         *  which extension overrides
         */
        virtual TInt CreateConferenceL( 
            RMobileConferenceCall::TMobileConferenceStatus /*aStatus*/ ) = 0;

        /**  
         * Allows a client to switch a conference call between "Active" and 
         * "Hold" states
         *          
         *
         * @param RMobileConferenceCall::TMobileConferenceStatus: CC status
         * @param CMmCallList*: call list
         * @return KErrNotSupported: default implementation of the interface
         *  which extension overrides
         */
        virtual TInt SwapL( 
            RMobileConferenceCall::TMobileConferenceStatus /*aStatus*/,
            CMmCallList* /*aCallList*/ ) = 0;

        /**  
         * This method adds the single voice call specified by the aCallName 
         * parameter to an existing conference call
         *          
         *
         * @param TName*: name of the call to be added to CC
         * @param RMobileConferenceCall::TMobileConferenceStatus: CC status
         * @param CMmCallList*: call list
         * @return KErrNotSupported: default implementation of the interface
         *  which extension overrides
         */
        virtual TInt AddCallL( const TName* /*aCallName*/,
            RMobileConferenceCall::TMobileConferenceStatus /*aStatus*/,
            CMmCallList* /*aCallList*/ ) = 0;

        /**  
         * Directed to extension to inform extension that a status of a single 
         * call has changed
         *          
         *
         * @param RMobileCall::TMobileCallStatus aNewStatus: new status of a single call
         * @param TInt aCallId: call ID of that call
         */
        virtual TBool CallStatusChanged( 
			     RMobileCall::TMobileCallStatus /*aNewStatus*/,
			     TInt /*aCallId*/ ) = 0;

		    /**
         * Checks if is possible to create a conference call
         *          
         *
         * @param RMobileConferenceCall::TMobileConferenceStatus: CC status
         * @param CMmCallList*: call list
         */
        virtual void CheckConferenceCapability(
            RMobileConferenceCall::TMobileConferenceStatus /*aStatus*/,
            CMmCallList* /*aCallList*/ ) = 0;

        /**  
         * This method terminates the whole conference call
         *          
         *
         * @param RMobileConferenceCall::TMobileConferenceStatus: CC status
         * @param CMmCallList*: call list
         * @return KErrNotSupported: default implementation of the interface
         *  which extension overrides
         */
        virtual TInt HangUpL( 
			     RMobileConferenceCall::TMobileConferenceStatus 
              /*aConferenceStatus*/ ) = 0;

        /**  
         * This method returns a current snapshot of the call information 
         * associated with the call specified by the aIndex parameter
         *          
         *
         * @param TInt*: index of the call
         * @param TDes8*: pointer where to copy the call information
         * @param CMmCallList*: call list
         * @param RMobileConferenceCall::TMobileConferenceStatus: CC status
         * @return KErrNotSupported: default implementation of the interface
         *  which extension overrides
         */
        virtual TInt GetMobileCallInfo( TInt* /*aIndex*/,TDes8* /*aCallInfo*/, 
            CMmCallList* /*aCallList*/,
            RMobileConferenceCall::TMobileConferenceStatus 
                /*aConferenceStatus*/ ) = 0;

        /**  
         * Checks if given call can be splitted from the conference call
         *          
         *
         * @param TInt*: call ID of the call to be splitted from CC
         * @param RMobileConferenceCall::TMobileConferenceStatus: CC status
         * @param CMmCallList*: call list
         * @return KErrNotSupported: default implementation of the interface
         *  which extension overrides
         */
        virtual TInt IsGoOneToOneSupported( TInt /*aCallId*/, 
            RMobileConferenceCall::TMobileConferenceStatus /*aStatus*/,
            CMmCallList* /*aCallList*/ ) = 0;

        /**  
         * Is used for getting the new updated status of Conference call
         *          
         *
         * @param RMobileConferenceCall::TMobileConferenceStatus: new CC status
         * @return KErrNotSupported: default implementation of the interface
         *  which extension overrides
         */
        virtual TInt GetNewStatus( 
            RMobileConferenceCall::TMobileConferenceStatus* 
                /*aNewStatus */ ) = 0;

        /**  
         * Resets internal attributes of Conference call extension
         */   
            virtual void ResetAttributes()= 0;

        /**  
         * Informs the Gsm extension if there is conference call
         * operation initiated bypassing the ETel (ghost operetion).
         *          
         *
         * @param aDataPackage: information about performed operation
         */
        virtual void CompleteNotifyConferenceEvent(
            CMmDataPackage* /*aDataPackage*/ ) = 0;

    };

#endif // CMMCONFERENCECALLEXTINTERFACE_H

//  End of file
