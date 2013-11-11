/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CDiskSpaceReservationPlugIn class.
*
*/


#ifndef C_DISK_SPACE_RESERVATION_PLUG_IN_H
#define C_DISK_SPACE_RESERVATION_PLUG_IN_H

#include <systemstartupextension.h>
#include <shareddataclient.h>

/**
* System Startup Extension plug-in for making initial disk space reservation
* on system boot time.
*
*  @lib diskspacereservationplugin.lib
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CDiskSpaceReservationPlugIn )
  : public CSystemStartupExtension
    {

public:

    /**
    * Two-phased constructor.
    *
    * @since S60 3.2
    *
    * @param aConstructionParameters Parameters from the creating object.
    */
    static CDiskSpaceReservationPlugIn* NewL( TAny* aConstructionParameters );

    /**
    * Destructor.
    */
    virtual ~CDiskSpaceReservationPlugIn();

    /**
    * Do the initial disk space reservation.
    *
    * @since S60 3.2
    *
    * @param aStatus The request status to complete after the task has been
    * finished.
    */
    virtual void ExecuteL( TRequestStatus& aStatus, const TDesC& aParams );

    /**
    * Cancel pending request.
    *
    * @since S60 3.2
    */
    virtual void Cancel();

private:

    /**
    * First phase constructor.
    *
    * @since S60 3.2
    *
    * @param aConstructionParameters Parameters from the creating object.
    */
    CDiskSpaceReservationPlugIn( TAny* aConstructionParameters );

    /**
    * Second phase constructor.
    *
    * @since S60 3.2
    */
    void ConstructL();

private:

    /** Session with shared data server. */
    RSharedDataClient iSharedData;

    };

#endif // C_DISK_SPACE_RESERVATION_PLUG_IN_H
