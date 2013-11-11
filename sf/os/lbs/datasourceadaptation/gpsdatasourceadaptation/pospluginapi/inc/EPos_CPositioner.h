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
* Name        : EPos_CPositioner.h
* Part of     : Mobile Location Framework / Positioning Plugin FW
* Interface   : Positioning Plug-in API
* Base class for all positioners
* Version     : %version: 2 %
*
*/




/**
 @file
 @publishedPartner
 @released
*/

#ifndef CPOSITIONER_H
#define CPOSITIONER_H

class MPositionerStatus;
class TPositionInfoBase;
class TPositionQuality;
class CPositionerExtension;

/**
 * This class is used when Location Framework needs to acquire a position and 
 * quality from the PSY. The interface of this class is used for loading and 
 * using PSYs and as a framework for developing PSYs. It requires that users 
 * define new subclasses to the API classes. It is also used for implementing
 * more advanced PSY features, e.g. periodic position updates, and the maximum 
 * age for positions. It can also handle parallel requests.
 */
class CPositioner : public CBase 
    {
    public:
        
    	IMPORT_C static CPositioner* NewL( TAny* aConstructionParameters );
        IMPORT_C static CPositioner* NewL(
            TUid aImplementationUid,
            const CPositioner& aPositioner
        );

        IMPORT_C ~CPositioner();

    protected:

    	IMPORT_C CPositioner();
        IMPORT_C void BaseConstructL( TAny* aConstructionParameters );

    public:

        /**
        * Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        *                 The position (TPosition) of this position info object
        *                 must be in WGS84 geodetic datum.
        * @param aStatus The request status
        *
        */
        virtual void NotifyPositionUpdate(
            TPositionInfoBase& aPosInfo,
            TRequestStatus& aStatus
        ) = 0;

        /**
        * Cancels position info request.
        */
        virtual void CancelNotifyPositionUpdate() = 0;

        IMPORT_C virtual TBool TrackingOverridden() const;
        IMPORT_C virtual void StartTrackingL(
            const TTimeIntervalMicroSeconds& aInterval
        );

        IMPORT_C virtual void StopTracking();
        IMPORT_C virtual void ServiceL( RMessage2& aMessage );
        IMPORT_C TUid ImplementationUid() const;

    protected:

        IMPORT_C TInt GetRequiredPositionQuality(
            TPositionQuality& aPositionQuality
        ) const;

        IMPORT_C void GetMaxAge( TTime& aMaxAge ) const;
        IMPORT_C TBool IsPartialUpdateAllowed() const;
        IMPORT_C MPositionerStatus* PositionerStatus();

    public:        
        IMPORT_C virtual void CancelNotifyPositionUpdate(TInt aError);
        
    private:
    	//IMPORT_C virtual void CPositioner_Reserved1();
	    // Reserved for future expansion - do not call
	    IMPORT_C virtual void CPositioner_Reserved2();
	    
    protected:
    	IMPORT_C void GetUpdateTimeOut(TTimeIntervalMicroSeconds& aUpdateTimeOut) const;
    	IMPORT_C void SetPsyDefaultUpdateTimeOut(const TTimeIntervalMicroSeconds& aUpdateTimeOut);
    	IMPORT_C void ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& aAdditionalTime);

    private:

        TUid iDtorIdKey;
        CPositionerExtension* iExtension;
    };  

#endif  // CPOSITIONER_H
