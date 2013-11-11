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



#ifndef RACCESSORYMODE_H
#define RACCESSORYMODE_H

//  INCLUDES
#include <AccessorySubsessionBase.h>
#include <AccPolAccessoryMode.h>
#include <AccessoryTypes.h>

// FORWARD DECLARATIONS
class RAccessoryServer;


// CLASS DECLARATION

/**
*  Accessory Mode sub-session.
*
*  This class is not intended for user derivation.
*
*  @lib AccClient.lib
*  @since S60 3.1
*/
class RAccessoryMode : public RAccessorySubsessionBase
    {

    public:

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessoryMode();

    public:

        /**
        * Creates a new sub-session within an existing session.
        *
        * @since S60 3.1
        * @param aSession The session to which this sub-session will belong.
        * @return KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C TInt CreateSubSession( RAccessoryServer& aSession );

        /**
        * Closes the RAccessoryMode sub-session.
        *
        * @since S60 3.1
        * @return KErrNone if successful, system-wide error code if failed.
        * @see CreateSubSession()
        */
        IMPORT_C TInt CloseSubSession();

        /**
        * Returns the current accessory mode.
        *
        * @since S60 3.1
        * @param aAccessoryMode On return, contains the Accessory mode.
        *                       TAccPolAccessoryMode is declared
        *                       in AccPolAccessoryMode.h.
        * @return KErrNone if successful.
        *         KErrNotReady if a device is not ready to start operation
        */
        IMPORT_C TInt GetAccessoryMode( TAccPolAccessoryMode& aAccessoryMode ) const;

        /**
        * Returns the current accessory mode.
        *
        * @since S60 3.1
        * @param aAccessoryMode On successful request completion, contains the
        *                       Accessory mode. TAccPolAccessoryMode is declared
        *                       in AccPolAccessoryMode.h.
        * @param aStatus        Indicates the completion status of a request.
        *                       KErrNone if successful, system-wide error code if failed.
        *                       Specifically:
        *                       KErrAlreadyExists, if request allready exist
        *                       KErrCancel, if request is cancelled
        * @return void
        */
        IMPORT_C void GetAccessoryMode( TRequestStatus& aStatus,
                                        TAccPolAccessoryMode& aAccessoryMode );

        /**
        * Issues a request for accessory mode changed notification.
        * Request must be renewed after completion if further notifications
        * are needed.
        *
        * @since S60 3.1
        * @param aAccessoryMode On successful request completion, contains
        *                       the updated Accessory Mode. TAccPolAccessoryMode
        *                       is declared in AccPolAccessoryMode.h.
        * @param aStatus Indicates the completion status of a request.
        *                KErrNone if successful, system-wide error code if failed.
        *                Specifically:
        *                KErrAlreadyExists, if request allready exist
        *                KErrCancel, if request is cancelled
        * @return void
        */
        IMPORT_C void NotifyAccessoryModeChanged( TRequestStatus& aStatus,
                                                  TAccPolAccessoryMode& aAccessoryMode );

        /**
        * Cancel NotifyAccessoryModeChanged().
        *
        * @since S60 3.1
        * @return void
        */
        IMPORT_C void CancelNotifyAccessoryModeChanged() const;

        /**
        * Cancel GetAccessoryMode().
        *
        * @since S60 3.1
        * @return void
        */
        IMPORT_C void CancelGetAccessoryMode() const;

    private:

        TAccSrvAccessoryModePckg iNotifyAccessoryModeChangedPckg;
        TAccSrvAccessoryModePckg iGetAccessoryModePckg;
        TInt                     iFlags;
        TPckg<TInt>              iFlagsPckg;
        TPckg<TInt>              iReserved1;
        TPckg<TInt>              iReserved2;

    };

#endif      // RACCESSORYMODE_H

// End of File

