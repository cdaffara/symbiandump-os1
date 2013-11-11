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



#ifndef CMMVOICELINETSY_H
#define CMMVOICELINETSY_H

//  INCLUDES
#include "cmmlinetsy.h"

// FORWARD DECLARATIONS
class CMmCallExtInterface;
class MTelephonyAudioControl;

// CLASS DECLARATION

/**
 *  CMmLineTsy implements mode dependent Voice Line based functionality 
 *  (defined by Symbian)
 */
NONSHARABLE_CLASS( CMmVoiceLineTsy ) : public CMmLineTsy
    {
    public: // Enumerations

        /** Enumeration for Swap status */
        enum TSwapStatus
            {
            EStatusSwapNotRequested,
            EWaitingHoldAndResumeOk,
            EWaitingHoldOk,
            EWaitingResumeOk
            };

    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         *          
         *
         * @param aMmPhone Pointer to the Phone object
         * @param aMode Line mode
         * @param aName Name of this line
         * @param aMessageManager Pointer to the MessageManager
         * @param aTelephonyAudioControl Pointer Telephony Audio Control    
         * @return Created line object 
         */
        static CMmVoiceLineTsy* NewL( CMmPhoneTsy* aMmPhone, 
            RMobilePhone::TMobileService aMode, const TDesC& aName, 
            CMmMessageManagerBase* aMessageManager,
            MTelephonyAudioControl* aTelephonyAudioControl );
        
        /**
         * Destructor.
         */
        ~CMmVoiceLineTsy();

    public: // New functions
        
        /**
         * Used for determining if Swap request is ready to be completed
         *          
         *
         * @param aCallStatus Status of the latest status indication
         * @return Boolean describing if Swap is ready or not
         */
        TBool IsSwapReady( RMobileCall::TMobileCallStatus aCallStatus );

        /**
         * Used for setting Swap status
         *          
         *
         * @param aSwapStatus New status of swap
         */
        void SetSwapStatus( CMmVoiceLineTsy::TSwapStatus aSwapStatus );

        /**
         * Used for retrieving Swap status of this line
         *          
         *
         * @return Current status of swap
         */
        CMmVoiceLineTsy::TSwapStatus SwapStatus() const;

    public: // Functions from base classes

        /**
         * This method is used for notifying a client about an incoming call.
         *          
         *
         * @param aDataPackage Package containing information about the 
         * incoming call
         */
        void CompleteNotifyIncomingCall( CMmDataPackage* aDataPackage );

        /**
         * This method is used for notifying a line of a call 
         * entering Dialling state
         *          
         *
         * @param aDataPackage Package containing information about the call
         */
        void CompleteNotifyDiallingStatus( CMmDataPackage* aDataPackage );

        /**
         * This function creates a new name for a call and opens a new call.
         *          
         *
         * @param aName Name of the new call.
         * @return Call object that was opened.
         */
        CTelObject* OpenNewObjectL( TDes& aNewName );
        
    private:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CMmVoiceLineTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:  // Functions from base classes

        /**
         * Creates a call object for the call that has been initiated bypassing 
         * TSY
         *          
         *
         * @param aCallId Call id of the new call
         * @param aCallMode Call mode
         * @param aCallStatus Call status
         * @return Pointer to created call object
         */
        CMmCallTsy* CreateGhostCallObject( TInt aCallId, 
            RMobilePhone::TMobileService /*aCallMode*/,
            RMobileCall::TMobileCallStatus aCallStatus );
      
        /**
         * Initialises miscellaneous internal attributes
         *          
         *
         */
        void InitInternalAttributesL();

        /**
         * Creates and stores a Call object for incoming call
         *          
         *
         * @return Success/failure value
         */
        TInt CreateCallObjectForIncomingCall();

    private:    // Data

        /**
         * Swap status
         */         
        TSwapStatus iSwapStatus;
        
	    /**
	     * Pointer to the Telephony Audio Control instance
	     */
	    MTelephonyAudioControl* iTelephonyAudioControl;

    };

#endif      // CMMVOICELINETSY_H
            
// End of File
