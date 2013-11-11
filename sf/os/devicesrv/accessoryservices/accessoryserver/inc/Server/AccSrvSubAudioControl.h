/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server-side sub-session of Accessory Audio Control
*
*/



#ifndef CACCSRVSUBAUDIOCONTROL_H
#define CACCSRVSUBAUDIOCONTROL_H

// INCLUDES
#include "AccSrvSubControl.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvMainSession;

// CLASS DECLARATION

/**
*  Accessory Audio Control server-side sub-session implementation
*
*  AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSubAudioControl ) : public CAccSrvSubControl
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since S60 3.1
        * @param aSession              Handle to the ASY Proxy.
        * @param aConnectionController Handle to the Connection Controller.
        * @param aServerModel          Handle to the Server Model.
        * @return Pointer to a new subsession
        */
        static CAccSrvSubAudioControl* NewL( CASYProxyAPI* aSession,
                                             CAccSrvConnectionController* aConnectionController,
                                             CAccSrvServerModel* aServerModel,
                                             TUint aCallerThreadID );

        /**
        * Destructor.
        */
        virtual ~CAccSrvSubAudioControl();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CAccSrvSubBase
        * Handles received messages from client.
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void DispatchMessageL( const RMessage2& aMessage );

        /**
        * From MQueueObserver.
        * Called when Queue passes message completion handling to registered observers.
        *
        * @since S60 3.1
        * @param aMsgID     Message in question.
        * @param aErrorCode Error Code of the operation.
        * @param aUniqueID  Unique ID of the Generic ID in question.
        * @return ETrue if the message was consumpted by a observer.
        *               Call-back order must be renewed after consumption
        *               if further call-backs are needed.
        */
        virtual TBool CompleteMessageL( TMsgID aMsgID,
                                        TInt aErrorCode,
                                        TInt aUniqueID );

    protected: // New functions

        /**
        * C++ default constructor.
        */
        CAccSrvSubAudioControl( CASYProxyAPI* aSession,
                                CAccSrvConnectionController* aConnectionController,
                                CAccSrvServerModel* aServerModel,
                                TUint aCallerThreadID );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private: // New functions

        /**
        * Handles NotifyAccessoryAudioLinkOpenedL() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyAccessoryAudioLinkOpenedL( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryAudioLinkClosedL() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyAccessoryAudioLinkClosedL( const RMessage2& aMessage );

        /**
        * Handles AccessoryAudioLinkOpen() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void AccessoryAudioLinkOpenL( const RMessage2& aMessage );

        /**
        * Handles AccessoryAudioLinkClose() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void AccessoryAudioLinkCloseL( const RMessage2& aMessage );

        /**
        * Handles AudioRoutingStatusNotify() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void AudioRoutingStatusNotifyL( const RMessage2& aMessage );

        /**
        * Handles CancelAccessoryAudioLinkOpen() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelAccessoryAudioLinkOpen( const RMessage2& aMessage );

        /**
        * Handles CancelAccessoryAudioLinkClose() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelAccessoryAudioLinkClose( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryAudioLinkOpenedL() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelNotifyAccessoryAudioLinkOpened( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryAudioLinkClosedL() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelNotifyAccessoryAudioLinkClosed( const RMessage2& aMessage );

    public:     // Data

    protected:  // Data

    private:    // Data

        //Stored AccessoryAudioLinkOpen() client request for later completion.
        RMessage2 iAccessoryAudioLinkOpenMsg;

        //Stored AccessoryAudioLinkClose() client request for later completion.
        RMessage2 iAccessoryAudioLinkCloseMsg;

        //Stored NotifyAccessoryAudioLinkOpenedL() client request for later completion.
        RMessage2 iNotifyAccessoryAudioLinkOpenedMsg;

        //Stored NotifyAccessoryAudioLinkClosedL() client request for later completion.
        RMessage2 iNotifyAccessoryAudioLinkClosedMsg;

        //Thread id of the client-side caller for this subsession
        TUint iCallerThreadID;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVSUBAUDIOCONTROL_H

// End of File
