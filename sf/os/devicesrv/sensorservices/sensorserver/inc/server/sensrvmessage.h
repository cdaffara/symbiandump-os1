/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor server message wrapper
*
*/


#ifndef SENSRVMESSAGE_H
#define SENSRVMESSAGE_H

#include <e32base.h>
#include "sensrvtypes.h"

class CSensrvSession;

/**
*  Wraps the client message for sensor server internal handling. 
*
*  @since S60 5.0
*/
class CSensrvMessage : public CBase
    {
    public: 
        
        /**
        * Two-phased constructor for external message
        *
        * @since S60 5.0
        * @return New CSensrvMessage instance
        */
        static CSensrvMessage* NewL();

        /**
        * Destructor.
        * @since S60 5.0
        */
        virtual ~CSensrvMessage();

        /**
        * Initializes the message. Must be done before message is
        * used first time and before use after each complete.
        *
        * @since S60 5.0
        * @param aMessage Client message to wrap
        */
        void Initialize(const RMessage2& aMessage);

        /**
        * Completes the message
        *
        * @since S60 5.0
        * @param aReason Completion reason identifier. Usually error code.
        */
        void Complete(TInt aReason);
        
        /**
        * Gets the function requested by client.
        *
        * @since S60 5.0
        * @return Function id.
        */
        inline TInt Function() const { return iMessage.Function(); };
        
        /**
        * Gets session related to this message.
        *
        * @since S60 5.0
        * @return Pointer to related session.
        */
        CSensrvSession* Session();
        
        /**
        * Writes return data to wrapped message.
        *
        * @since S60 5.0
        * @param aParam Parameter index to write
        * @param aDes Source descriptor for written data.
        * @param aOffset The offset from the start of the client's descriptor. 
        *        If not explicitly specified, the offset defaults to zero.
        * @return KErrArgument aParam is invalid, or if aOffset is negative.
        *         KErrBadDescriptor Parameter is not a descriptor.
        *         KErrOverflow Target descriptor is too small.
        */
        TInt Write(TInt aParam, const TDesC8& aDes, TInt aOffset=0);
        
        /**
        * Gets message parameter descriptor maximum lenght.
        *
        * @since S60 5.0
        * @param aParam Index of message parameter
        * @return If >= 0, indicates maximum lenght of descriptor parameter.
        *         Otherwise indicates error code: KErrArgument if aParam is invalid,
        *         or KErrBadDescriptor if message parameter specified
        *         aParam is not of the descriptor type.
        */
        TInt GetDesMaxLength(TInt aParam) const;
        
        /**
        * Gets message parameter descriptor lenght.
        *
        * @since S60 5.0
        * @param aParam Index of message parameter
        * @return If >= 0, indicates maximum lenght of descriptor parameter.
        *         Otherwise indicates error code: KErrArgument if aParam is invalid,
        *         or KErrBadDescriptor if message parameter specified
        *         aParam is not of the descriptor type.
        */
        TInt GetDesLength(TInt aParam) const;

        /**
        * Gets client process SID
        *
        * @since S60 5.0
        * @return Secure Id of the client process.
        */
        TSecureId SecureId() const;
        
        /**
        * Reads data from wrapped message.
        *
        * @since S60 5.0
        * @param aParam The index value identifying the argument. 
        *         This is a value in the range 0 to (KMaxMessageArguments-1) inclusive.
        * @param aDes The target descriptor into which the client data is to be written. 
        * @return KErrArgument If aParam has a value outside the valid range.
        *         KErrBadDescriptor If the message argument is not an 8-bit descriptor.
        */
        TInt Read( const TInt aParam, TDes8 &aDes ) const;

        /**
        * Reads integer data from wrapped message.
        *
        * @since S60 5.0
        * @param aParam The index value identifying the argument. 
        *         This is a value in the range 0 to (KMaxMessageArguments-1) inclusive.
        * @param aInt The target integer where the value is read.
        */
        void ReadInt( const TInt aParam, TInt &aInt ) const;

        /**
        * Extracts channel id from message.
        *
        * @since S60 5.0
        * @param aChannelId The found channel id is returned here.
        * @return KErrNotSupported If message data doesn't include channel id.
        */
        TInt GetChannelId(TSensrvChannelId& aChannelId) const;
        
        /**
        * Gets the security info of the client message
        *
        * @since S60 5.0
        * @param aSecurityInfo This object will contain client security info
        */
        void GetSecurityInfo(TSecurityInfo& aSecurityInfo) const;
        
        /**
        * Returns message handle. Used to determine if message is still valid.
        *
        * @since S60 5.0
        * @return Message handle. Handle will be zero if message is not currently valid.
        */
        inline TInt Handle() const { return iMessage.Handle(); };
        
        /**
        * Returns the message as read only.
        *
        * @since S60 5.0
        * @return The message.
        */
        inline const RMessage2& GetMessage() const { return iMessage; };
        
    private:

        /**
        * C++ constructor for external message.
        */
        CSensrvMessage();

    private:    // Data

        /**
        * Wrapped message
        */
        RMessage2 iMessage;
    };


#endif // SENSRVMESSAGE_H 
