/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MPOSPARAMETEROBSERVER_H
#define MPOSPARAMETEROBSERVER_H

// FORWARD DECLARATION
class TPositionQuality;

// CLASS DECLARATION

/**
* Interface implemented by the Location Server to supply positioning related
* parameter values.
*/
class MPosParameterObserver
    {
    public:  // New functions

        /**
        * Get the quality of service requested by the client.
        *
        * @param aPositionQuality The requested quality of service.
        * @return KErrNotFound if there is no requested quality of service,
        *   KErrNone if there is.
        */
        virtual TInt GetRequiredPositionQuality(
        /* OUT */       TPositionQuality& aPositionQuality
        ) const = 0;

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
        virtual void GetMaxAge(
        /* IN  */       TTime& aMaxAge
        ) const = 0;

        /**
        * Checks if the client allows a partial position update.
        *
        * A partial update result does not need to contain all parameters. The
        * only mandatory parameters are latitude, longitude and time of fix.
        * Everything else is optional.
        *
        * If a partial update is returned to the client in a
        * NotifyPositionUpdate call, the completion code must be set to
        * KPositionPartialUpdate.
        * 
        * @return ETrue if partial position updates are allowed, otherwise
        *   EFalse.
        */
        virtual TBool IsPartialUpdateAllowed() const = 0;
        
        
  
        /**
         * Returns the location update timeout associated with the subsession. The method returns a real timeout,
         * which is defined and used by the Location Server. It may be different from the timeout set by the client
         * or default PSY timeout.
         * 
         * @param aUpdateTimeOut [Out] The location update timeout.
         */
        virtual void GetUpdateTimeOut(TTimeIntervalMicroSeconds& aUpdateTimeOut) const = 0;
        
        /**
         * Called to inform the Location Server about the PSY default update timeout. The Location
         * Server may use this value to calculate the update timeout it uses. 
         * When this method is called, it does not affect the currect request. So, it must be called in advance
         * before a request comes from the Location Server. 
         * 
         * @param aUpdateTimeOut [In] The PSY default location update timeout.
         */
        virtual void SetPsyDefaultUpdateTimeOut(const TTimeIntervalMicroSeconds& aUpdateTimeOut) = 0;
        	

        /**
         * Extends the active request timeout by the value specified. Only the current active request is 
         * affected and the changes are not stored. That is the value returned by GetUpdateTimeOut and
         * the default PSY timeout set by SetPsyDefaultUpdateTimeOut are not changed. 
         * This method is used, for example, to apply FNP lag.   
         * 
         * @param aUpdateTimeOut [In] The time to extend timeout for.
         */
        virtual void ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& aAdditionalTime) = 0;

    };  

#endif  // MPOSPARAMETEROBSERVER_H

// End of File
