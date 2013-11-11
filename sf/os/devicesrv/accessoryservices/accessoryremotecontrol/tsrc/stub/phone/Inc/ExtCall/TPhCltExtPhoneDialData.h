/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef TPHCLTEXTPHONEDIALDATA_H   
#define TPHCLTEXTPHONEDIALDATA_H  


//  INCLUDES
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

// CLASS DECLARATION

/**
* It defines the structure containing dial information.
*
* Fields:
*
*  window group    - When call is ended, set window group is actived.
*  show number     - Permission to show number. ETrue iff it is allowed.
*  redial          - Automatic redial override.
*  name            - Name corresponding to number. Name must be defined
*                    if shown number equals to EFalse.
*  contact id      - Contact id. you should put always when you are dialing
*                    from phonebook contact.
*  tel number      - String that is to be dialled.
*  number type     - Number type override. See PbkFields.hrh. Only phone
*                    number fields are supported.
*  allow match     - Permission to allow match. ETrue iff match must be done.
*                    If you are dialing from sim pd or fdn, then this should
*                    be EFalse.
*  number entry    - not used. 
*  end other calls - Information if the existing calls are ended before
*                    this new one is made.
*  emergency       - Information whether this is an emergency call or not.
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
*  @lib phoneclient.lib
*  @since 1.0
*/
class TPhCltExtPhoneDialData
    {          
    public:

        /**
        * Construct a dial data package
        */
        IMPORT_C TPhCltExtPhoneDialData();

        /**
        * Construct a dial data package
        *
        * @param aNumber Number to be used in this package.
        * @param aContactId Contact ID to be used in this package.
        */
        IMPORT_C TPhCltExtPhoneDialData( 
            const TPhCltTelephoneNumber& aNumber,
            TPhCltContactId aContactId );

        /**
        * Construct a dial data package
        *
        * @param aNumber Number to be used in the package.
        * @param aWindowGroupId Windoww Group ID to be used in this package.
        */
        IMPORT_C TPhCltExtPhoneDialData( 
            const TPhCltTelephoneNumber& aNumber,
            TInt aWindowGroupId );

        /**
        * Construct a dial data package
        *
        * @param aNumber Number to be used in the package.
        * @param aContactId Contact ID to be used in this package.
        * @param aWindowGroupId Windoww Group ID to be used in this package.
        */
        IMPORT_C TPhCltExtPhoneDialData( 
            const TPhCltTelephoneNumber& aNumber,
            TPhCltContactId aContactId, 
            TInt aWindowGroupId );

        /**
        * Construct a dial data package
        *
        * @param aName Name to be used in the package.
        * @param aNumber Number to be used in the package.
        * @param aContactId Contact ID to be used in this package.
        * @param aWindowGroupId Windoww Group ID to be used in this package.
        */
        IMPORT_C TPhCltExtPhoneDialData( 
            const TPhCltNameBuffer& aName,
            const TPhCltTelephoneNumber& aNumber, 
            TPhCltContactId aContactId, 
            TInt aWindowGroupId );


    public: // Access - Setters

        /**
        * Set the window group of requesting application.
        *
        * @param aWindowGroupId The window group identifier.
        */
        IMPORT_C void SetWindowGroup( TInt aWindowGroupId );

        /**
        * Set the number visibility in the phone app.
        *
        * If this is set to EFalse, then number is not shown or written to 
        * log event.
        *
        * @param aShowNumber Whether or not to show the number.
        */
        IMPORT_C void SetShowNumber( TBool aShowNumber );

        /**
        * Change the redial mode.
        *
        * @param aRedial The new redial status
        */
        IMPORT_C void SetRedial( TInt aRedial );
        
        /**
        * Set the value of RedialMaximumDuration. 
        *
        * @param aMaxDuration The value of duration in seconds.
        */
        IMPORT_C void SetRedialMaximumDuration( TTimeIntervalSeconds aMaximumRedialDuration );

        /**
        * Set the name of the person who is to be called.
        *
        * @param aName The name of the person being called.
        */
        IMPORT_C void SetName( const TPhCltNameBuffer& aName );

        /**
        * Set the contact Id of the person being called.
        *
        * @param aContactId The contacts model contact id of the person being
        *                   called.
        */
        IMPORT_C void SetContactId( TPhCltContactId aContactId );

        /**
        * Set the number of the person to be called.
        * This is mandatory.
        *
        * @param aNumber The number of the person being called.
        */
        IMPORT_C void SetTelephoneNumber( 
            const TPhCltTelephoneNumber& aNumber );

        /**
        * Set whether to return to number entry.
        *
        * @param aReturn Whether to return to the number entry editor.
        */
        IMPORT_C void SetReturnToNumberEntryOnFailure( TBool aReturn );

        /**
        * Set whether the name entered in SetName should be matched
        * against the personal directory (=phonebook).
        *
        * This should be set EFalse only if phone number has not been
        * taken from phonebook.
        *
        * @param aAllowMatch Whether to allow matches.
        */
        IMPORT_C void SetAllowMatch( TBool aAllowMatch );

        /**
        * Set the number type. 
        * 
        * See PbkFields.hrh. Only phone number fields are supported.
        *
        * @param aNumberType The number type.
        */
        IMPORT_C void SetNumberType( TInt aNumberType );

        /**
        * Set parameter for end other calls. 
        * 
        * If this is set to ETrue, other calls will be terminated before
        * dial. Default is EFalse.
        *
        * @param aEndCalls Are the other calls ended.
        */
        IMPORT_C void SetEndOtherCalls( TBool aEndCalls );

        /**
        * Set whether this is known to be an emergency call or not.
        * If this function is not called, it is assumed that the call
        * is not an emergency call. However, in Phone application side it is 
        * checked whether the call actually is an emergency call or not before 
        * dialing.
        *
        * @param aEmergency 
        *        ETrue if the call is known to be an emergency call.
        *        EFalse if the call is not known to be an emergency call
        *        (default).
        */
        IMPORT_C void SetEmergency( TBool aEmergency );

        /**
        * Set the calling party subaddress.
        * Defined in ITU-T I.330 and 3GPP TS 11.14.
        *
        * @param aSubAddress The calling party subaddress.
        */
        IMPORT_C void SetSubAddress( const TPhCltSubAddress& aSubAddress );

        /**
        * Set wheteher this is a SAT originated call or not.
        * If this function is not called, the originator is assumed to 
        * be other than SAT.
        *
        * @param aSATCall ETrue if SAT originated the call, EFalse otherwise.
        */
        IMPORT_C void SetSATCall( TBool aSATCall );

        /**
        * Set the bearer capability.
        *
        * @param aBearer The bearer capabilities as defined in GSM 04.08.
        */
        IMPORT_C void SetBearer( const TPhCltBearerCapability& aBearer );

        /**
        * Set the call type. 
        *
        * @param aCallType The call type.
        */
        IMPORT_C void SetCallType( TPhCltCallType aCallType );
        
        /**
        * Set the value of SendKey. 
        *
        * @param aSendKey The value of SendKey.
        */
        IMPORT_C void SetSendKeyPressed( TBool aSendKey );


    public: // Access - Getters

        /**
        * Return the window group id. 
        *
        * @return Returns KPhCltGoToIdle if no id has previously been specified.
        */
        IMPORT_C TInt WindowGroupId() const;

        /** 
        * Redial information.
        * 
        * @return Return the redial status,
        */
        IMPORT_C TInt Redial() const;
        
        /**
        * Automatic redial maximum duration . 
        *
        * @return  value of duration in seconds.
        */
        IMPORT_C TTimeIntervalSeconds RedialMaximumDuration( ) const;

        /** 
        * Number type.
        *
        * @return Return the number type.
        */
        IMPORT_C TInt NumberType() const;

        /** 
        * Return in failure.
        * 
        * @return Returns whether to return to the number entry.
        */
        IMPORT_C TBool ReturnToNumberEntryOnFailure() const;

        /** 
        * Is match allowed.
        * 
        * @return Returns the allow match setting.
        */
        IMPORT_C TBool AllowMatch() const;

        /** 
        * Number showing.
        * 
        * @return Returns the number visibility.
        */
        IMPORT_C TBool ShowNumber() const;

        /** 
        * Name
        * 
        * @return Return the name of the recipient.
        */
        IMPORT_C const TPhCltNameBuffer& Name() const;

        /** 
        * Contact ID
        * 
        * @return Returns the contact id or KPhCltNoContact if none specified.
        */
        IMPORT_C TPhCltContactId ContactId() const;

        /** 
        * Telephone number
        * 
        * @return Returns the telephone number.
        */
        IMPORT_C const TPhCltTelephoneNumber& TelephoneNumber() const;

        /** 
        * End other calls
        * 
        * @return Returns ETrue if other calls should be ended before dial.
        */
        IMPORT_C TBool EndOtherCalls() const;

        /** 
        * Emergency status (is emergency call or not).
        * 
        * @return Returns ETrue if the call is emergency call.
        *                 EFalse if the call is not emergency call.
        */
        IMPORT_C TBool Emergency() const;

        /** 
        * Calling party subaddress.
        * 
        * @return Returns the calling party subaddress.
        */
        IMPORT_C const TPhCltSubAddress& SubAddress() const;

        /** 
        * SAT call information.
        * 
        * @return Returns ETrue if SAT is the originator of the call.
        *                 EFalse otherwise.
        */
        IMPORT_C TBool SATCall( ) const;

        /** 
        * Bearer capability.
        * 
        * @return Returns the bearer capabilities as defined in GSM 04.08.
        */
        IMPORT_C const TPhCltBearerCapability& Bearer( ) const;

        /** 
        * Call type.
        *
        * @return Return the call type.
        */
        IMPORT_C TPhCltCallType CallType() const;
        
        /** 
        * SendKey pressed.
        *
        * @return Return ETrue if SendKey used.
        */
        IMPORT_C TBool SendKeyPressed() const;


    public:
        
        /** 
        * Returns the call argumets through the parameter.
        * 
        * @param aReturn The call arguments are stored here.
        */
        IMPORT_C void AsIPCStruct( TPhCltCallArgs& aReturn ) const;


    private: // Member data
        
        // The window group identifier storage.
        TInt                      iWindowGroup;

        // The new redial status storage.
        TInt                      iRedial;
        
        //Automatic redial maximum duration
        TTimeIntervalSeconds      iRedialMaximumDuration;

        // The number type information storage.
        TInt                      iNumberType;

        // The number visibility information storage.
        TBool                     iShowNumber;

        // Return to the number entry editor information storage.
        TBool                     iReturnToNumberEntryOnFailure;

        // Allow match information storage.
        TBool                     iAllowMatch;
        
        // The name information storage.
        TPhCltNameBuffer          iName;

        // The contact ID storage.
        TPhCltContactId           iContactId;

        // Telephony number storage.
        TPhCltTelephoneNumber     iTelNumber;
        
        // Information storage of the other call termination.
        TBool                     iEndOtherCalls;

        // The emergency call information.
        TBool                     iEmergencyCall;

        // The calling party subaddress.
        TPhCltSubAddress          iSubAddress;

        // The information whether this is SAT originated call or not.
        TBool                     iSATCall;

        // The bearer capability.
        TPhCltBearerCapability    iBearer;

        // Call type.
        TPhCltCallType            iCallType;
        
        // SendKey
        TBool                     iSendKey;

    };

#endif // TPHCLTEXTPHONEDIALDATA_H
            
// End of File
