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



#ifndef CMMDTMFTSY_H
#define CMMDTMFTSY_H

// INCLUDES
#include <e32base.h>  
#include <et_tsy.h>   

// CONSTANTS
const TUint16 KMaxDataBufferSize( 256 );
// FORWARD DECLARATIONS
class TDataPackage;
class CMmPhoneTsy;
class CMmTsyReqHandleStore;
class CMmDataPackage;

// CLASS DECLARATION
/**
 *  DTMF-related functionality of Phone Tsy
 */
NONSHARABLE_CLASS( CMmDtmfTsy ) : public CBase
    {
    public: // types

        /**
         * DTMF type (single digit or string)
         */
        enum TDtmfType
            {
            EDtmfTypeUnknown, // 0 // unknown DTMF type
            EDtmfTypeDigit, // the currently processed DTMF is one digit
            EDtmfTypeString // the currently processed DTMF is a string
            };

    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         * @param aPhoneTsy The Phone Tsy object
         */
        static CMmDtmfTsy* NewL( CMmPhoneTsy* aPhoneTsy );

        /**
         * Destructor.
         */
        virtual ~CMmDtmfTsy();

    public: // New functions
        
        /**
         * Handles DTMF requests
         * 
         *                  
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param const TInt aIpc: request IPC number
         * @param const TDataPackage& aPackage: packaged request parameters
         * @return result of the request
         */
        TInt DoExtFuncL(
                const TTsyReqHandle aTsyReqHandle, 
                const TInt aIpc, 
                const TDataPackage& aPackage );

        /**
         * Cancels DTMF request
         * 
         *                  
         * @param const TInt aIpc: request IPC number
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return result of the request
         */
        virtual TInt CancelService( 
                    const TInt aIpc, 
                    const TTsyReqHandle aTsyReqHandle );

        /**
         * Calls the needed complete method due timer expiration
         * 
         *                  
         * @param const TInt aReqHandleType: req handle type.
         * @param TInt aError: error value
         * @return None
         */
        void Complete( TInt aReqHandleType, TInt aError );

        //DTMF METHODS

        /**
         * Get DTMF capabilities
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         *  
         *                  
         * @param TUint32* aCaps: DTMF capabilities 
         * @return Return value to the ETel Server
         */
        TInt GetDTMFCaps( const TTsyReqHandle aTsyReqHandle, TUint32* aCaps );
        
        /**
         * Request for DTMF caps change notifications
         * 
         *                  
         * @param TUint32* aCaps: DTMF capabilities
         * @return Return value to the ETel Server
         */       
        TInt NotifyDTMFCapsChange( TUint32* aCaps );

        /**
         * Cancels DTMF caps change notifications
         * 
         *                  
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return Return value to the ETel Server
         */        
        TInt NotifyDTMFCapsChangeCancel( const TTsyReqHandle aTsyReqHandle );
     
#ifdef USING_CTSY_DISPATCHER         
        TInt GetConnectedCallId(TInt& aCallId);
        
        /**
         * Complete the Start DTMF Tone request.
         */
        void CompleteStartDTMFTone(TInt aResult);

        /**
         * Complete the Stop DTMF Tone request.
         */        
        void CompleteStopDTMFTone(TInt aResult);
#endif // USING_CTSY_DISPATCHER       

        /**
         * Sends DTMF string
         * 
         *                  
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param const TDesC* aTones, DTMF string
         * @return Return value to the ETel Server
         */
        TInt SendDTMFTonesL( const TTsyReqHandle aTsyReqHandle, 
            const TDesC* aTones );

        /**
         * Cancels DTMF string sending
         * 
         *                  
         * @return Return value to the ETel Server
         */
        TInt SendDTMFTonesCancelL();

        /**
         * Starts sending a single DTMF digit
         * 
         *                  
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param TChar* aTone: tone to be played.
         * @return Return value to the ETel Server
         */
        TInt StartDTMFToneL( const TTsyReqHandle aTsyReqHandle, TChar* aTone );

        /**
         * Stops sending a single DTMF digit
         * 
         *                  
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return Return value to the ETel Server
         */
        TInt StopDTMFToneL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Notifies when the DTMF string sending has stopped
         * 
         *                  
         * @return Return value to the ETel Server
         */
        TInt NotifyStopInDTMFString();

        /**
         * Cancels NotifyStopInDTMFString notifications
         * 
         *                  
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyStopInDTMFStringCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Called when client wants notification of a DTMF Event.
         * 
         * 
         * @param aInfo for storing the type of event                 
         * @return Return KErrNone
         */
        TInt NotifyDTMFEvent( TInt* aInfo );

        /**
         * Cancels a NotifyDTMFEvent notification.
         *         
		 * @param aTsyReqHandle TSY request handle                
         * @return KErrNone
         */
        TInt NotifyDTMFEventCancel( const TTsyReqHandle aTsyReqHandle );		
		
        /**
         * Continues or cancels sending of stopped DTMF string
         * 
         *                  
         * @param const TTsyReqHandle aTsyReqHandle: request handle
         * @param TBool aContinue: Boolean value that tells to 
         * continue or cancel
         * @return: Return value to the Etel Server
         */
        TInt ContinueDTMFStringSendingL( 
				const TTsyReqHandle aTsyReqHandle, 
				TBool aContinue );         

        /**
         * Complete DTMF capabilities change notification
         * 
         *                  
         */
        void CompleteNotifyDTMFCaps();

        /**
         * Complete sending DTMF tones
         * 
         *                  
         * @param TInt aResult, error code
         */
        virtual void CompleteSendDTMFTones( TInt aResult );
        
        /**
         * Complete cancel of sending DTMF tones
         * 
         *                  
         * @param TInt aResult, error code
         */
        virtual void CompleteSendDTMFTonesCancel( TInt aResult );
          
        /**
         * Notifies about stop mark ('w') in DTMF string sent.
         * 
         *                  
         * @param CMmDataPackage* aDataPackage: contains DTMF tones that 
         * have not been sent yet
         */
        virtual void CompleteNotifyStopInDTMFString( 
        	CMmDataPackage* aDataPackage );

		/**
         * Called when notice of a DTMF tone has been received.
         * 
         *  
         *	@param aTsyReqHandle TSY request handle
         */
        virtual void CompleteNotifyDTMFEvent( TInt aTsyReqHandle );				
			
        /**
         * Gets the type of the currently processed DTMF
         * 
         *                  
         * @return value of iDtmfType
         */
        virtual TDtmfType GetDtmfType();

    private:

        /**
         * C++ default constructor.
         */
        CMmDtmfTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
        
        /**
         * Return the number of active calls 
         */
        TInt ActiveCalls() const;

    private: // member data

        /**
         * Pointer to phone Tsy       
         */
        CMmPhoneTsy* iMmPhone;

        /**
         * Member variable for storing the SendDTMFTonesCancelFlag.         
         */
        TBool iSendDTMFTonesCancelFlag;

        /**
         * Type of the currently processed dtmf (digit or string)        
         */
        TDtmfType iDtmfType;

        /**
         * Buffer for storing DTMF string after 'w' mark in the DTMF string         
         */
        TBuf<KMaxDataBufferSize> iDTMFStringToContinue;

        /**
         * return pointer to DtmfCaps         
         */
        TUint32* iRetDtmfCaps;

        TInt* iDtmfInfo;
    };

#endif  // CMMDTMFTSY_H            

// End of File
