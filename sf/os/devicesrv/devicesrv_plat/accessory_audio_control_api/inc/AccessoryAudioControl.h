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
* Description: ?Description
*
*/


#ifndef RACCESSORYAUDIOCONTROL_H
#define RACCESSORYAUDIOCONTROL_H

//  INCLUDES
#include <AccessoryControl.h>


// CLASS DECLARATION

/**
*  Accessory Audio Control sub-session.
*
*  This class is not intended for user derivation.
*
*  @lib AccAudioControl.lib
*  @since S60 3.1
*/
class RAccessoryAudioControl : public RAccessoryControl
    {
    public:

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessoryAudioControl();

    public:

        /**
        * Creates a new sub-session within an existing session.
        *
        * @since S60 3.1
        * @param aSession The session to which this sub-session will belong.
        * @return KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C virtual TInt CreateSubSession(RAccessoryServer& aSession);

        /**
        * Closes the RAccessoryAudioControl sub-session.
        *
        * @since S60 3.1
        * @return KErrNone if successful, system-wide error code if failed.
        * @see CreateSubSession()
        */
        IMPORT_C virtual TInt CloseSubSession();

        /**
        * Request to open accessory audio connection.
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aGenericId Generic ID of the accessory
        * @param aAudioType Type of the audio connection
        * @return void
        */
        IMPORT_C void AccessoryAudioLinkOpen( TRequestStatus& aStatus,
                                              const TAccPolGenericID& aGenericId,
                                              TAccAudioType aAudioType );

        /**
        * Request to close accessory audio connection.
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aGenericId Generic ID of the accessory
        * @param aAudioType Type of the audio connection
        * @return void
        */
        IMPORT_C void AccessoryAudioLinkClose( TRequestStatus& aStatus,
                                               const TAccPolGenericID& aGenericId,
                                               TAccAudioType aAudioType );

        /**
        * Notifies change of audio routing. 
        * Audio subsystem uses this method to notify to which accessory audio
        * is routed or audio routing is stopped.
        *
        * @since S60 3.1
        * @param aActiveAudio ETrue  = Audio is routed according to the
        *                              aGenericId parameter
        *                     EFalse = No audio, aGenericId parameter is ignored
        * @param aGenericId   Generic ID of the accessory.
        *                     Empty Generic ID means that audio is routed to phone.
        * @return void
        */
        IMPORT_C void AudioRoutingStatusNotify( TBool aActiveAudio,
                                                const TAccPolGenericID& aGenericId );

        /**
        * Issues a request for audio link opened notification.
        * Request must be renewed after completion if further notifications
        * are needed.
        *
        * The notification is not sent to the thread where
        * AccessoryAudioLinkOpen() is called. The thread is defined when
        * a sub-session is created.
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aGenericId On succesfull request completion, contains
        *                   the updated Generic ID of the accessory
        * @param aAudioType On successful request completion, contains
        *                   the type of audio connection
        * @return void
        */
        IMPORT_C void NotifyAccessoryAudioLinkOpened( TRequestStatus& aStatus,
                                                      TAccPolGenericID& aGenericId,
                                                      TAccAudioType& aAudioType );

        /**
        * Issues a request for audio link closed notification.
        * Request must be renewed after completion if further notifications
        * are needed.
        *
        * The notification is not sent to the thread where
        * AccessoryAudioLinkClose() is called. The thread is defined when
        * a sub-session is created.
        *
        * @since S60 3.1
        * @param aStatus    Indicates the completion status of a request.
        *                   KErrNone if successful, system-wide error code if failed.
        *                   Specifically:
        *                   KErrAlreadyExists, if request allready exist
        *                   KErrCancel, if request is cancelled
        * @param aGenericId On succesfull request completion, contains
        *                   the updated Generic ID of the accessory
        * @param aAudioType On successful request completion, contains
        *                   the type of audio connection
        * @return void
        */
        IMPORT_C void NotifyAccessoryAudioLinkClosed( TRequestStatus& aStatus,
                                                      TAccPolGenericID& aGenericId,
                                                      TAccAudioType& aAudioType );

        /**
        * Cancel AccessoryAudioLinkOpen().
        * This does not close the accessory audio connection, 
        * it must be closed with AccessoryAudioLinkClose().
        *
        * @since S60 3.1
        * @return void
        * @see AccessoryAudioLinkOpen
        */
        IMPORT_C void CancelAccessoryAudioLinkOpen() const;

        /**
        * Cancel AccessoryAudioLinkClose().
        * This does not leave the accessory audio connection open, 
        * it must be opened with AccessoryAudioLinkOpen().
        *
        * @since S60 3.1
        * @return void
        * @see AccessoryAudioLinkClose
        */
        IMPORT_C void CancelAccessoryAudioLinkClose() const;

        /**
        * Cancel NotifyAccessoryAudioLinkOpened().
        *
        * @since S60 3.1
        * @return void
        * @see NotifyAccessoryAudioLinkOpened
        */
        IMPORT_C void CancelNotifyAccessoryAudioLinkOpened() const;

        /**
        * Cancel NotifyAccessoryAudioLinkClosed().
        *
        * @since S60 3.1
        * @return void
        * @see NotifyAccessoryAudioLinkClosed
        */
        IMPORT_C void CancelNotifyAccessoryAudioLinkClosed() const;

    private:  //Data

        TAccSrvGenericIDPckgBuf    iAudioLinkOpenGIDPckgBuf;
        TAccSrvGenericIDPckgBuf    iAudioLinkCloseGIDPckgBuf;
        TAccSrvGenericIDPckg       iNotifyAudioLinkOpenedGIDPckg;
        TAccSrvAudioTypePckg       iNotifyAudioLinkOpenedAudioTypePckg;
        TAccSrvGenericIDPckg       iNotifyAudioLinkClosedGIDPckg;
        TAccSrvAudioTypePckg       iNotifyAudioLinkClosedAudioTypePckg;
        TAccSrvAudioTypePckgBuf    iAudioLinkOpenAudioTypePckgBuf;
        TAccSrvAudioTypePckgBuf    iAudioLinkCloseAudioTypePckgBuf;
        TInt                       iFlags;
        TPckg<TInt>                iFlagsPckg;
        TPckg<TInt>                iReserved1;
        TPckg<TInt>                iReserved2;

    };

#endif      // RACCESSORYAUDIOCONTROL_H

// End of File

