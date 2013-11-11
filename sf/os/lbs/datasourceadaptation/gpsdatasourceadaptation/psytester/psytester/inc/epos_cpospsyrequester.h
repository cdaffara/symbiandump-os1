/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSPSYREQUESTER_H
#define CPOSPSYREQUESTER_H

//  INCLUDES
#include <lbspositioninfo.h>
#include "EPos_MPosParameterObserver.h"
#include "EPos_MPosStatusObserver.h"
#include "EPos_PositionerConstructParams.h"
#include <lbs/epos_cpositioner.h>
#include "epos_mposrequestobserver.h"

// DATA TYPES
enum _StatusChangesDone
	{
	EInitialisingDone = 0x1,
	EInactiveDone = 0x02,
	EReadyDone = 0x04,
    EDisabledDone = 0x08,
    EActiveDone = 0x16, 
    EQualityUnknown= 0x32
	};

// CONSTANTS
const TInt KRequesterMessageBufSize = 100;

// CLASS DECLARATION
/**
* Class handling location requests. 
* This is done by an instance of the CPositioner class.
*/
class CPosPSYRequester : public CActive,
	                     public MPosParameterObserver, 
                         public MPosStatusObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYRequester* NewL(
        /* IN */  MPosRequestObserver* aParent
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYRequester();

    public: // New functions

        /**
        * Creates positioner instance
        */
        void OpenPositionerL(
        /* IN */ const TUid aImplementationUid,
        /* IN */ TBool aCheckStatus = EFalse
        );

		/**
        * Starts a position request
        * @param aPosInfo, information to be filled in in the location request
        */
        void MakeRequest(
        /* OUT */ TPositionInfo& aPosInfo
        );

        /**
        * Cancels an outstanding position request
        */
        void CancelRequest();

        /**
        * Gets the result for a location request
        * @param aStatus the status of the completed request
        * @param aRequestTime time time the request took
        */
		void GetResult(
        /* OUT */ TInt& aStatus, 
        /* OUT */ TTimeIntervalMicroSeconds& aRequestTime
        );

        /**
        * Deletes positioner instance, used for status update tests
        */ 
        void ClosePositioner();
           
        /**
        * Sets the maxage that the PSY can request.
        * @param aMaxAgeTime The TTime that the PSY can request.
        */
        void SetMaxAge(
		/* IN  */	const TTime	aMaxAgeTime
		);

		/**
        * Checks if max age was requested during position request from PSY.
        * @return TBool telling if max age was requested or not
        */
        TBool MaxAgeRequested() const;

		/**
        * SetMaxAgeRequested. Sets queryid was requested.
        */
		void SetMaxAgeRequested(TBool aBool);

		/**
        * Sets the query id that the PSY can request.
        * @param aQueryId The TInt that the PSY can request.
        */
        void SetQueryId(
		/* IN  */	TBool aQueryIdStrategy
		);

		/**
        * Checks if query id was requested during position request from PSY.
        * @return TBool telling if query id was requested or not
        */
        TBool QueryIdRequested() const;

		/**
        * SetQueryIdRequested. Sets queryid was requested.
        */
		void SetQueryIdRequested(TBool aBool);

		/**
        * SetPartialUpdateAllowed. Sets the partialupdate state that the PSY can request.
        * @param aPartialUpdate The TBool that the PSY can request.
        */
        void SetPartialUpdateAllowed(
		/* IN  */	const TBool aPartialUpdate
		);

        /**
        * Checks the module status for consistency 
        */
        void CheckStatus();

		/**
        * Clears status history when status is tested.
        */
		void ClearStatusHistory();

	public: // Functions from base classes

        /**
        * Returns if the current PSY supports tracking or not
        * @return telling if tracking is supported
        */
        TBool TrackingOverridden();

        /**
        * StartTrackingL. Starts tracking with specified interval
        * @param aInterval the tracking interval to be set.
        */
        void StartTrackingL(
        /* IN */ const TTimeIntervalMicroSeconds& aInterval
        );
	
        /**
        * Stops PSY tracking  
        */
        void StopTracking();
          
        /**
        * Getter for status changes
        * @param aChanges bitmask containing the reported statuses from PSY.
        * @param aStatusReported tells if status has been reported or not
        * @param aModuleStatus the last status reported from PSY
        */
        void StatusChanges(
        /* OUT */ TInt& aChanges,
        /* OUT */ TBool& aStatusReported,
        /* OUT */ TPositionModuleStatus& aModuleStatus
        );
        
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
        * Get the current max age set by the client.
        *
        * The PSY may save the result from the latest position request and
        * return the same result in the next position request if the client
        * says it's ok. Max age tells the PSY how old the stored position is
        * allowed to be. If the stored position is too old or the PSY does not
        * support max age, a normal positioning should be performed.
        *
        * @param aMaxAge On return, will contain the max age set by the client.
        *   If no max age is defined aMaxAge will contain a time set to zero.
        */
        void GetMaxAge(
        /* IN  */       TTime& aMaxAge
        ) const;

	    /**
        * Checks if the client allows a partial position update.
        *
        * A partial update is a positioning result which does not contain for
        * instance longitude or latitude.
        *
        * If a partial update is returned to the client in a
        * NotifyPositionUpdate() call, the completion code must be set to
        * KPositionPartialUpdate.
        * 
        * @return ETrue if partial position updates are allowed, otherwise
        *   EFalse .
        */
		TBool IsPartialUpdateAllowed() const;
		
		void GetUpdateTimeOut(TTimeIntervalMicroSeconds& aUpdateTimeOut) const;
		void SetPsyDefaultUpdateTimeOut(const TTimeIntervalMicroSeconds& aUpdateTimeOut);
		void ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& aAdditionalTime);

    public: // Functions from base classes

		/**
        * From MPosParameterObserver GetRequiredPositionQuality. 
        * Gets the position quality requested by the client.
        * not used in this class only dummy implemented because virtual
        * @param aRequestedPosQuality The requested position quality.
        */
        TInt GetRequiredPositionQuality(
        /* IN  */ TPositionQuality& aRequestedPosQuality
        ) const;

    public: // Functions from base classes

		/**
        * From MPosStatusObserver ReportStatus.
        * Reports that the positioner status has changed.
        *
        * @param aImplementationUid The UID of the CPositioner implementation 
        *   which is the originator of the status updates.
        * @param aStatus The new status.
        */
        void ReportStatus(
        /* IN  */ const TPositionModuleId& aImplementationUid,
        /* IN  */ const TPositionModuleStatus& aStatus
        );

    public: // Functions from base classes

        /**
        * From CActive RunL
        */
        void RunL();

        /**
        * From CActive DoCancel
        */
        void DoCancel();

        /**
        * From CActive RunError
        */
        TInt RunError(TInt aError);

    private:

        /**
        * C++ default constructor.
        */
        CPosPSYRequester();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(
        /* IN */  MPosRequestObserver* aParent
        );

        // By default, prohibit copy constructor
        CPosPSYRequester( const CPosPSYRequester& );
        
        // Prohibit assigment operator
        CPosPSYRequester& operator= ( const CPosPSYRequester& );

    private:    // Data
        
        TBool						iQueryIdStrategy;
        TBool						iQueryIdRequested;
        TInt                        iStatusChangesDone;
        TBool						iPartialUpdate;
        TBool						iCheckStatus;
        TBool                       iStatusReported;
        TPositionInfo*				iPosInfo;
        TPositionModuleStatus       iModuleStatus;
		TTime						iStartTime;
		TTime						iMaxAgeTime;
        TBool						iMaxAgeRequested;
        TTimeIntervalMicroSeconds	iRequestTime;
        MPosRequestObserver*	    iParent;
        CPositioner*				iPositioner;
    };

#endif      // CPOSPSYREQUESTER_H

// End of File
