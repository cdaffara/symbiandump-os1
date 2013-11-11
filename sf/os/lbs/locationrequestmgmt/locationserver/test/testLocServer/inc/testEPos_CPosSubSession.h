/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSSUBSESSION_H
#define CPOSSUBSESSION_H

// INCLUDES
#include <e32base.h>
#include <lbs.h>
#include <lbs/lbsadmin.h>
#include "EPos_MPosParameterObserver.h"
#include "LbsNetRegStatusInt.h" // for RLbsNetworkRegistrationStatus

// CONSTANTS

// FORWARD DECLARATIONS
struct TPosModulesEvent;
class CPosModuleSettings;
class CPositioner;
class CPositionRequest;
class MPosStatusObserver;
class MPosModuleStatusManager;
class CPosLocMonitorReqHandlerHub;
// TODO Verify
class CPosLastPositionAreaRequest;
// TODO to be removed - included to compile code before completion of 
// location monitor


// CLASS DECLARATION

/**
* This class provides functionality common to all EPos subsessions.
*/
class CPosSubSession : public CObject, public MPosParameterObserver
    {
	public:  // Constructor & Destructor
        static CPosSubSession* NewLC(
        			CPosModuleSettings& aModuleSettings,
        			CPosLocMonitorReqHandlerHub& aLocMonitorReqHandlerHub,
        			TUid aImplementationUid,
        			TBool aIsProxy,
        			MPosStatusObserver* aStatusObserver,
        			MPosModuleStatusManager* aModuleStatusManager);
		virtual ~CPosSubSession();

	public:		// New functions
        void ServiceL(const RMessage2& aMessage);
        void HandleSettingsChangeL(TPosModulesEvent aEvent);
        void NotifyServerShutdown();

    protected:  // Functions from MPosParameterObserver
        TInt GetRequiredPositionQuality(TPositionQuality& aPositionQuality) const;
        void GetMaxAge(TTime& aMaxAge) const;
        TBool IsPartialUpdateAllowed() const;
        
        virtual void GetUpdateTimeOut(TTimeIntervalMicroSeconds& aUpdateTimeOut) const;
        virtual void SetPsyDefaultUpdateTimeOut(const TTimeIntervalMicroSeconds& aUpdateTimeOut);
        virtual void ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& aAdditionalTime);
        
    private:
        CPosSubSession();

        void ConstructL(
            CPosModuleSettings& aModuleSettings,
            CPosLocMonitorReqHandlerHub& aLocMonitorReqHandlerHub,
            TPositionModuleId aImplementationUid,
            MPosStatusObserver* aStatusObserver,
            MPosModuleStatusManager* aModuleStatusManager,
            TBool aIsProxy);

        CPosSubSession( const CPosSubSession& );
        CPosSubSession& operator= ( const CPosSubSession& );

        void HandleSetRequestorL(const RMessage2& aMessage);
        void HandleSetRequestorStackL(const RMessage2& aMessage);
		void HandleSetUpdateOptionsL(const RMessage2& aMessage);
		void HandleGetUpdateOptionsL(const RMessage2& aMessage);
		void HandleGetLastKnownPositionL(const RMessage2& aMessage);
		void HandleNotifyPositionUpdateL(const RMessage2& aMessage);
		void HandleGetLastKnownPositionAreaL(const RMessage2& aMessage);   		// TODO Verify
        void HandleNotifyPositionUpdateCancelL(const RMessage2& aMessage);
        void HandleGetLastKnownPositionCancelL(const RMessage2& aMessage);
        void HandleGetLastKnownPositionAreaCancelL(const RMessage2& aMessage); 	// TODO Verify
        void HandleCancelAsyncRequestL(const RMessage2& aMessage);
        
        void ReadModuleInfoL();        

        void ValidateRequestorL(CRequestor* aRequestor);

        void RequestComplete(const RMessage2& aMessage, TInt aCompleteCode);

        TBool IsSelfLocateEnabled();
        
    private:    // Data	
    	RRequestorStack             iRequestors;
        CPositionRequest*           iPositionRequest;        
        TBool                       iWaitingForFirstTrackingRequest;
        TTimeIntervalMicroSeconds   iMaxUpdateAge;
        TUid                        iImplementationUid;
        TUint32                     iClassesSupported;
        TBool                       iAcceptPartialUpdates;
        TBool                       iTracking;
        CPosLocMonitorReqHandlerHub*    iLocMonitorReqHandlerHub;

        TTimeIntervalMicroSeconds	iUpdateTimeOut;
        TTimeIntervalMicroSeconds	iPsyDefaultUpdateTimeOut;
        TBool 						iLocationManagementSupported;    
        CLbsAdmin*                  iAdmin;
    	/** internal interface used to broadcast the network registration (roaming) status */
    	RLbsNetworkRegistrationStatus	iNetRegStatus;        
    };


#endif  // CPOSSUBSESSION_H

// End of File
