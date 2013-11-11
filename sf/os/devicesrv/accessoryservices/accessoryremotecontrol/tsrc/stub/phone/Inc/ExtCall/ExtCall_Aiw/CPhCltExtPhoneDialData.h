/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It defines dial data.
*
*/


#ifndef CPHCLTEXTPHONEDIALDATA_H   
#define CPHCLTEXTPHONEDIALDATA_H  


//  INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "PhCltTypes.h"

// Constant used as window group id to indicate that we wish to go to idle.
const TInt KPhCltGoToIdle = 0;

// Constants used in Redial parameter
const TInt KPhCltRedialDefault = -1;          // Default redial.
const TInt KPhCltRedialOff = -2;              // Redial is off.
const TInt KPhCltRedialWithDefaultTime = -3;  // Redial on, with default time.
const TInt KPhCltRedial = 0;	// Forced redial.

// Other constants used in TPhCltExtPhoneDialData ( default values )
const TBool KPhCltDefaultAllowMatch = ETrue;   // Match is allowed
const TBool KPhCltDefaultNumberEntry = EFalse; // No return to number entry.
const TInt KPhCltDefaultNumberType = -1;       // No number type.


// CONSTANTS
//None.


// FORWARD DECLARATIONS
//None

// CLASS DECLARATION
/**
* It defines the structure containing dial information.
*
* Fields:
*
*  window group    - When call is ended, set window group is actived.
*
*  show number     - Permission to show number. ETrue if it is allowed.
*
*  redial          - Automatic redial override.
*
*  name            - Name corresponding to number. Name must be defined
*                    if shown number equals to EFalse.
*  contact link      - Contact link. you should put always when you are dialing
*                    from phonebook contact.
*  tel number      - String that is to be dialled.
*                    number fields are supported.
*  allow match     - Permission to allow match. ETrue if match must be done.
*                    If you are dialing from sim pd or fdn, then this should
*                    be EFalse.
*  number entry    - Returns whether to return to the number entry on failure 
*
*  end other calls - Information if the existing calls are ended before
*                    this new one is made.
*                    Not mandatory. Set only if you know that the call is an 
*                    emergency call.
*  subaddress      - The calling party subaddress. Supports SAT features.
*                    Defined in ITU-T I.330 and 3GPP TS 11.14.
*  SATCall         - Set whether this is SAT originated call or not. 
*                    Supports SAT features.
*  bearer          - Bearer capabilities. Supports SAT features.
*
*  CallType        - The type of the call, i.e. voice (default) or video.
*
*  SendKey         - Call made with SendKey, optional.
*
*  Redial maximum duration - Automatic redial maximum duration
*
*  Initiate call   - true if iniate call false otherwise 
*
*  UUI             - User to user information sent on call setup phase.
*
*  @lib phoneclient.lib
*  @since 3.2
*/
/**
*  Dial data set for PhoneClient.
*
*  @lib ServiceHandler.lib
*  @since Series 60 3.2
*/
NONSHARABLE_CLASS( CPhCltExtPhoneDialData ): public CBase
    {
   
   public:
      /* 
        *   TBools combined to one uint.
        *
        *   EPhCltCallFlagsDefault
        *               Default flags, if nothing is not specified.
        *   EPhCltCallFlagsAllowNumberShowing
        *               Number showing is allowed.
        *   EPhCltCallFlagsReturnToNumberEntryOnFailure
        *               Return to Number Entry if there is an error.
        *   EPhCltCallFlagsAllowPersonalDirectoryMatching
        *               The number is matched to Personal Directory.
        *   EPhCltCallFlagsEndBeforeDial
        *               Other calls are ended before the new one is done.
        *   EPhCltCallFlagsSAT
        *               If set means that SAT has originated the call.
        *   EPhCltCallFlagsVideoCall
        *               If set means that this is video call.
        *               Otherwise means normal, i.e. voice call.
        *   EPhCltCallFlagsVoIPCall
        *               If set means that this is VoIP call.
        *               Otherwise means normal, i.e. voice call.
        *   EPhCltCallFlagsSendKey
        *               If set means that call made with SendKey.
        */
        enum TPhCltCallFlags
            {
            EPhCltCallFlagsDefault                           = 0x0000,
            EPhCltCallFlagsAllowNumberShowing                = 0x0001,
            EPhCltCallFlagsReturnToNumberEntryOnFailure      = 0x0002,
            EPhCltCallFlagsAllowPersonalDirectoryMatching    = 0x0004,
            EPhCltCallFlagsEndBeforeDial                     = 0x0008,
            EPhCltCallFlagsSAT                               = 0x0020,
            EPhCltCallFlagsVideoCall                         = 0x0040,
            EPhCltCallFlagsVoIPCall                          = 0x0080,
            EPhCltCallFlagsSendKey                           = 0x0100   
            };
      
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPhCltExtPhoneDialData* NewL();
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPhCltExtPhoneDialData* NewLC();
        
        
       	/**
        * Two-phased constructor.
        */
        IMPORT_C static CPhCltExtPhoneDialData* NewL( const TDesC8& aPackage );
        
       	/**
        * Two-phased constructor.
        */
        IMPORT_C static CPhCltExtPhoneDialData* NewLC( const TDesC8& aPackage );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhCltExtPhoneDialData();

  	public: // New functions
    
        /**
        * Fills the parameter list.
        * @since Series 60 3.2
        * @param aInParamList In parameter used with CAiwService.
        */  
        IMPORT_C HBufC8* PackLC();
        
        
        // Calculates dialdata package size
        IMPORT_C TInt CalculatePackageSize() const;
        
        // Sets dial data parameters to CPhCltExtPhoneDialData object
        void SetParamsL( const TDesC8& aPackage ); 
               
        
   public: // Access - Getters
         
        /**
        * Defines if mathing is allowed.
        * @since Series 60 3.2
        * @return Returns the allow match setting information.
        */
        IMPORT_C  TBool AllowMatch() const;
        
        /**
        * Bearer capability.
        * @since Series 60 3.2
        * @return Returns bearer capability.
        */
        IMPORT_C const TPhCltBearerCapability& Bearer() const;
        
        /**
        * Gives the current call type.
        * @since Series 60 3.2
        * @return Current call type.
        */
        IMPORT_C  TPhCltCallType CallType() const ;   
         
        /**
        * Contact link field.
        * @since Series 60 3.2
        * @return Returns contack link item.
        */
        IMPORT_C const TDesC8& ContactLink() const;
        
        
        /**
        * End other calls param.
        * @since Series 60 3.2
        * @return Returns ETrue if other calls will be terminated.
        *				  EFalse if calls are not terminated.
        */
        IMPORT_C TBool EndOtherCalls() const;
        
        /**
        * Intiate Call
        * @since Series 60 3.2
        * @return Returns ETrue if call is itiate call EFalse otherwise
        */
        IMPORT_C  TBool InitiateCall() const;
        
        /**
        * Name.
        * @since Series 60 3.2
        * @return Returns name.
        */
        IMPORT_C  const TPhCltNameBuffer& Name() const;
        
        
        /**
        * Redial information.
        * @since Series 60 3.2
        * @return Return the redial status,
        */
        IMPORT_C  TInt Redial() const;  
        
        /**
        * Redial maximum duration
        * @since Series 60 3.2
        * @return Returns redial max duration.
        */
        IMPORT_C const TTimeIntervalSeconds RedialMaximumDuration() const;      
                   
        
        /** 
        * Return in failure.
        * @since Series 60 3.2
        * @return Returns whether to return to the number entry.
        */
        IMPORT_C TBool ReturnToNumberEntryOnFailure() const;
        
        /**
        * SAT Call.
        * @since Series 60 3.2
        * @return Returns ETrue if SAT call EFalse if not.
        */
        IMPORT_C  TBool SATCall() const;
        
        /** 
        * SendKey pressed.
        * @since Series 60 3.2
        * @return Return ETrue if SendKey used.
        */
        IMPORT_C TBool SendKeyPressed() const;
        
        /**
        * Show Number.
        * @since Series 60 3.2
        * @return Returns the number visibility.
        */
        IMPORT_C TBool ShowNumber() const;
        
        /**
        * Sub address.
        * @since Series 60 3.2
        * @return Returns SAT call sub address.
        */
        IMPORT_C  const TPhCltSubAddress& SubAddress() const;
        
        /**
        * Gives the current phone number.
        * @since Series 60 3.2
        * @return Telephone number.
        */
        IMPORT_C  const TPhCltTelephoneNumber& TelephoneNumber() const;
        
        /**
        * Gives the current window group id.
        * @since Series 60 3.2
        * @return Window group id. 
        */     
        IMPORT_C  TInt WindowGroup() const; 
        
        /**
         * Gives the user to user information.
         * @since S60 v3.2
         * @return User to user information.
         */
        IMPORT_C const TDesC& UUI() const;
        
	public: // Access - Setters
	
		/**
        * Set whether the name entered in SetName should be matched
        * against the personal directory.
        * @since Series 60 3.2
        * This should be set EFalse only if phone number has not been
        * taken from phonebook.
        *
        * @param aAllowMatch Whether to allow matches.
        */
        IMPORT_C  void SetAllowMatch( TBool aAllowMatch );
	
		/**
        * Set the bearer capability.
        * @since Series 60 3.2
        * @param aBearer The bearer capabilities as defined in GSM 04.08.
        * @leaves KErrArgument if aBearer's length is longer than KAiwBearerLength
        */
        IMPORT_C void SetBearerL( const TPhCltBearerCapability& aBearer );
        
        /**
        * Sets call type.
        * @since Series 60 3.2
        * @param aCallType calls' type.
        */
        IMPORT_C void SetCallType( TPhCltCallType aCallType );
        
        /**
        * Set contact link.
        * @since Series 60 3.2
        * @param aContactLink Contact link information.
        */
       IMPORT_C void SetContactLinkL( const TDesC8& aContactLink );
        
        
        /**
        * Set parameter for end other calls. 
        * @since Series 60 3.2
        * If this is set to ETrue, other calls will be terminated before
        * dial. Default is EFalse.
        * @param aEndCalls Are the other calls ended.
        */
        IMPORT_C void SetEndOtherCalls( TBool aEndCalls  );
        
        /**
        * Set Initiate call Etrue or EFalse
        *@since Series 60 3.2
        * @param aInitCall ETrue if call is intiate call
        */
        IMPORT_C void SetInitiateCall( TBool aInitCall );
        
        /**
        * Set the name of the person who is to be called.
        * @since Series 60 3.2
        * @param aName The name of the person being called.
        * @leaves KErrArgument if aName length is over 50 characters.
        */
        IMPORT_C void SetNameL( const TPhCltNameBuffer& aName );
        
        
        /**
        * Change the redial mode.
        * @since Series 60 3.2
        * @param aRedial The new redial status
        */
  
        IMPORT_C void SetRedial( TInt aRedial );
        
        /**
        * Set redial max duration.
        * @since Series 60 3.2
        * @param aDuration redial max duration.
        */
        IMPORT_C void SetRedialMaximumDuration( TTimeIntervalSeconds aMaximumRedialDuration );       
        
        
        /**
        * Set whether to return to number entry.
        *
        * @param aReturn Whether to return to the number entry editor.
        */
        IMPORT_C void SetReturnToNumberEntryOnFailure( TBool aReturn );
                
        /**
        * Set wheteher this is a SAT originated call or not.
        * If this function is not called, the originator is assumed to 
        * be other than SAT.
        * @since Series 60 3.2
        * @param aSATCall ETrue if SAT originated the call, EFalse otherwise.
        */
        IMPORT_C void SetSATCall( TBool aSATCall );
        
        
        /**
        * Set the value of SendKey.
        *  @since Series 60 3.2
        * @param aSendKey The value of SendKey.
        */
        IMPORT_C void SetSendKeyPressed( TBool aSendKey );
        
        /**
        * Set the number visibility in the phone app.
        * @since Series 60 3.2
        * If this is set to EFalse, then number is not shown or written to 
        * log event.
        * @param aShowNumber Whether or not to show the number.
        */
        IMPORT_C void SetShowNumber( TBool aShowNumber );
        
        /**
        * Set the calling party subaddress.
        * Defined in ITU-T I.330 and 3GPP TS 11.14.
        * @since Series 60 3.2
        * @param aSubAddress The calling party subaddress.
        * @leaves KErrArgument if aSubAddress length is longer than 
        * KAiwSubAddressLength
        */
        IMPORT_C void SetSubAddressL( const TPhCltSubAddress& aSubAddress );
        
        /**
        * Sets the phone number.
        * @since Series 60 3.2
        * @param aNumber Phone number, which maximum length is
        * KMaximumPhoneNumberLength.
        * @leaves KErrArgument if aPhoneNumber length is over KMaximumPhoneNumberLength
        */
        IMPORT_C void SetTelephoneNumber( const TPhCltTelephoneNumber& aNumber );
        
        /**
        * Sets the window group identifier. When call is ended, set window group is actived.
        * @since Series 60 3.2
        * @param aWindowGroup Window group id.
        */
        IMPORT_C void SetWindowGroup( TInt aWindowGroup );
     
        /**
        * Set the value of xSP Service Id. 
        * If __VOIP and RD_VOIP_REL_2_2 flags are Not defined,
        * function does not do anything.
        *
        * @param aServiceId The value of Service Id.
        */
        IMPORT_C void SetServiceId( TUint32 aServiceId );
        
        /** 
        * Service Id. VoIP2.2 implementation
        * If __VOIP and RD_VOIP_REL_2_2 flags are Not defined,
        * this return 0
        * @return Return service id or zero.
        */
        IMPORT_C TUint32 ServiceId() const; 
        
        /**
         * Sets user to user information. Maximum length is 
         * KPhCltUUILength.
         * @param aUUI User to user information.
         * @since S60 v3.2
         */
        IMPORT_C void SetUUIL( const TDesC& aUUI );
                      
    // Constructors
    
	 private: 
	 
  		/**
        * C++ default constructor.
        * @since Series 60 3.2
        */
        CPhCltExtPhoneDialData(); 

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since Series 60 3.2
        */
        void ConstructL();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        * @since Series 60 3.2
        * @param aPackage including dial data
        */
        void ConstructL( const TDesC8& aPackage ); 
        
     
     //Private functions
     
     private:
        /**
        * Calculates variable descriptor size
        * @since Series 60 3.2
        * @param aDescriptor which size is calculated
        */
        TInt CalculateDescriptorSize( const TDesC& aDescriptor ) const;
        
        /**
        * Calculates variable descriptor size
        * @since Series 60 3.2
        * @param aDescriptor which size is calculated
        */
        TInt CalculateDescriptorSize( const TDesC8& aDescriptor ) const;
        
        /**
        * Read descriptor value from stream
        * @since Series 60 3.2
        * @param aStream used stream
        * @param aDescriptor which is read from stream
        */
        void ReadDescriptorFromStreamL( RDesReadStream& aStream, HBufC8*& aDescriptor );
        
        /**
        * Read descriptor value from stream
        * @since Series 60 3.2
        * @param aStream used stream
        * @param aDescriptor which is read from stream
        */
        void ReadDescriptorFromStreamL( RDesReadStream& aStream, HBufC*& aDescriptor );
        
        /**
        * Write descriptor value to stream
        * @since Series 60 3.2
        * @param aStream used stream
        * @param aDescriptor which value is write to stream
        */
        void WriteDescriptorToStreamL( RDesWriteStream& aStream, const TDesC8& aDescriptor );
        
        /**
        * Write descriptor value to stream
        * @since Series 60 3.2
        * @param aStream used stream
        * @param aDescriptor which value is write to stream
        */
        void WriteDescriptorToStreamL( RDesWriteStream& aStream, const TDesC& aDescriptor );
        
        /**
        * Copy descriptor value to member descriptor
        * @since Series 60 3.2
        * @param aSource which is as copy source
        * @param aMemberDescriptor is as copy destination
        */
        void CopyDescriptorToMemberDescriptorL( const TDesC& aSource, HBufC*& aMemberDescriptor );
        
        /**
        * Copy descriptor value to member descriptor
        * @since Series 60 3.2
        * @param aSource which is as copy source
        * @param aMemberDescriptor is as copy destination
        */
        void CopyDescriptorToMemberDescriptorL( const TDesC8& aSource, HBufC8*& aMemberDescriptor );
        
        /**
        * Check descriptor max length
        * Leaves with KErrArgument if aDescriptorLength is greated that aMaxAllowedDescriptorLenght
        * @since Series 60 3.2
        * @param aDescriptorLength is descriptor length
        * @param  aMaxAllowedDescriptorLength maximum allowed descriptor length
        */
        void CheckDescriptorMaxLengthL( TInt aDescriptorLength, TInt aMaxAllowedDescriptorLength ) const;
        
        
        
       // Data
       
       private:    
   
        // Telephony number storage.
        TPhCltTelephoneNumber     iTelNumber;
         
        // Call type.
        TPhCltCallType            iCallType;
        
        // The name information storage.
        TPhCltNameBuffer          iName;
        
        
        // The window group identifier storage.
        TInt                      iWindowGroup;
        
        // The new redial status storage.
        TInt                      iRedial;
        
        //Automatic redial maximum duration
        TTimeIntervalSeconds      iRedialMaximumDuration;
        
        
        //The contact Link storage.
        HBufC8* 				  iContactLink;
        
        // The calling party subaddress.
        TPhCltSubAddress          iSubAddress;
       
         // The number visibility information storage.
        TBool                     iShowNumber;
        
        // Allow match information storage.
        TBool                     iAllowMatch;
        
        // The information whether this is SAT originated call or not.
        TBool                     iSATCall;
        
        // Information storage of the other call termination.
        TBool                     iEndOtherCalls;
        
        // The bearer capability.
        TPhCltBearerCapability    iBearer;
                 
        // SendKey
        TBool                     iSendKey;
        
        // Return to the number entry editor information storage.
        TBool                     iReturnToNumberEntryOnFailure;

        
        //Intialization call 
        TBool 					  iInitCall;
       
        // Flags value
        TUint 					  iFlags;
#if defined(__VOIP) && defined(RD_VOIP_REL_2_2)
        // service id. VoIP2.2 implementation
        TUint32                   iServiceId;                 
#endif // __VOIP && RD_VOIP_REL_2_2  

        /**
         * User to user info.
         * own.
         */
        HBufC*                   iUUI;

    };

#endif      // CPHCLTEXTPHONEDIALDATA_H
            
// End of File
