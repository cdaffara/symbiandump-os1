/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Tt3MPosParameterObserver declaration
* This file is for supporting creation of CPositioner -instance
* directly without using MLFW at all.
*
*/



/**
 @file tdbt3mposparameterobserver.h
 @internalTechnology
*/

#ifndef _TT3MPOSPARAMETEROBSERVER_H_
#define _TT3MPOSPARAMETEROBSERVER_H_

#include "EPos_MPosParameterObserver.h" 

NONSHARABLE_CLASS(Tt3MPosParameterObserver): public MPosParameterObserver
    {
    public:
        Tt3MPosParameterObserver();

    public:  // From MPosParameterObserver

        /**
        * Get the quality of service requested by the client.
        *
        * @param aPositionQuality The requested quality of service.
        * @return KErrNotFound if there is no requested quality of service,
        *   KErrNone if there is.
        */
        TInt GetRequiredPositionQuality(TPositionQuality& aPositionQuality) const;

        /**
        * Get the current query id.
        *
        * Query id is a reference to the latest request verification and can be
        * used by a PSY, which encapsulate a positioning service that requires
        * its own verification.
        *
        * The query id is only valid during an outstanding
        * NotifyPositionUpdate() call.
        *
        * @return The query id of the current position request.
        */
        TInt QueryId() const;

        /**
        * Get the earliest allowed time of an old position fix, based on the
        * current max age set by the client.
        *
        * The PSY may save the result from the latest position request and
        * return the same result in the next position request if the client
        * says it's ok. Max age tells the PSY how old the stored position is
        * allowed to be. If the stored position is too old or the PSY does not
        * support max age, a normal positioning should be performed.
        *
        * @param aMaxAge On return, will contain the earliest allowed time of 
        *   an old position fix. If no max age is defined aMaxAge will contain 
        *   a time set to zero.
        */
        void GetMaxAge(TTime& aMaxAge) const;

        /**
        * Checks if the client allows a partial position update.
        *
        * A partial update result does not need to contain all parameters. The
        * only mandatory parameters are latitude, longitude and time of fix.
        * Everything else is optional.
        *
        * If a partial update is returned to the client in a
        * NotifyPositionUpdate() call, the completion code must be set to
        * KPositionPartialUpdate.
        * 
        * @return ETrue if partial position updates are allowed, otherwise
        *   EFalse.
        */
        TBool IsPartialUpdateAllowed() const;

        void GetUpdateTimeOut(TTimeIntervalMicroSeconds& aUpdateTimeOut) const;
        void SetPsyDefaultUpdateTimeOut(const TTimeIntervalMicroSeconds& aUpdateTimeOut);
        void ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& aAdditionalTime);
    };  

#endif 

// End of file
