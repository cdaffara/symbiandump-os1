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



#ifndef CSIMPSYREQUESTER_H
#define CSIMPSYREQUESTER_H

//  INCLUDES
#include <e32base.h>
#include <lbspositioninfo.h>
#include "EPos_PositionerConstructParams.h"
#include "EPos_MPosParameterObserver.h"
#include "EPos_MPosStatusObserver.h"
// CONSTANTS

// FUNCTION TYPES

// FORWARD DECLARATIONS
class TPositionInfoBase;
class TPositionInfo;
class CPositioner;
class CSimPsyMultiRequester;
class MPosStatusObserver;
class MPosParameterObserver;

// CLASS DECLARATION

/**
*  Active object used to make position requests
*
*/
class CSimPsyRequester : public CActive,
	public MPosParameterObserver, public MPosStatusObserver
    {
    public:  // Constructors and destructor
        
        static CSimPsyRequester* NewL(CSimPsyMultiRequester* aParent);

        /**
        * Destructor.
        */
        ~CSimPsyRequester();

    private:

        /**
        * C++ default constructor.
        */
        CSimPsyRequester(CSimPsyMultiRequester* aParent);

        void ConstructL();    

    public: // Functions from base classes
        
        void ClosePositioner();

        /**
        * Starts a position request
        */
        void MakeRequest();

        /**
        * Cancels a position request
        */
        void CancelRequest();

        /**
        * Get result
        */
        void GetResult(
            TInt& aStatus, 
            TPositionInfo& aPosition, 
            TTimeIntervalMicroSeconds& aRequestTime
            );
            
        /**
        * Get module status 
        */
        TBool GetModuleStatus();

        /**
        * Number of status reports 
        */
        TInt ReportedStatuses();
        
        /*
        *  Reset number of reported statuses.
        */
        void ResetReportedStatuses();

		/**
        * Module status
        */
        TPositionModuleStatus ModuleStatus();

		/**
        * Get the classes which the PSY is requested to support.
        *
        * @return Classes which the PSY is requested to support.
        */
        TPositionInfoClassType RequestedClasses() const;

        /**
        * Get the position quality requested by the client.
        *
        * @param aRequestedPosQuality The requested position quality.
        */
        TInt GetRequiredPositionQuality(
        /* IN  */       TPositionQuality& aRequestedPosQuality
        ) const;

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
        TBool IsPartialUpdateAllowed() const;

		/**
        * Reports that the positioner status has changed.
        *
        * @param aImplementationUid The UID of the CPositioner implementation 
        *   which is the originator of the status updates.
        * @param aStatus The new status.
        */
        void ReportStatus(
        /* IN  */   const TPositionModuleId& aImplementationUid,
        /* IN  */   const TPositionModuleStatus& aStatus
        );
       
    private:

        void RunL();

        void DoCancel();

    private:

		TPositionModuleStatus			iModuleStatus;
		TBool							iCorrectModuleStatus;
		TInt                            iReportedStatuses;

        TPositionInfo					iPosition;
        CPositioner*					iPositioner;
        TTime							iStartTime;
		TPositionerConstructParams		iConstructParams;
        TTimeIntervalMicroSeconds		iRequestTime;
        CSimPsyMultiRequester*			iParent;
    };

#endif      // CSIMPSYREQUESTER_H   
            
// End of File
