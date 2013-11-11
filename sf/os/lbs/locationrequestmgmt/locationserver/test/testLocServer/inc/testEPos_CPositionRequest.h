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


#ifndef CPOSITIONREQUEST_H
#define CPOSITIONREQUEST_H

// INCLUDE FILES
#include <e32base.h>
#include "epos_proxypositionerconstructparams.h"

// CONSTANT DECLARATIONS

// FORWARD DECLARATIONS
class CPositioner;
class CPosPositionLog;
class CPosModuleSettings;
struct TPosModulesEvent;
class CPosCallbackTimer;
class CPosLocMonitorReqHandlerHub;

// CLASS DECLARATION

/**
*    Active object implementing a position request cycle. Handles
*    activities such as controlling logging and verifying privacy.
*
*/
class CPositionRequest : public CActive
    {
    public:  // Constructors and destructor
    	static CPositionRequest* NewL(CPosModuleSettings& aModuleSettings,
    			CPosLocMonitorReqHandlerHub& aLastPositionHandler,
    			TProxyPositionerConstructParams& aPositionerParams,
    			TBool aIsProxy);
        virtual ~CPositionRequest();

    public:  // New functions
        void MakeRequestL(const RMessage2& aMessage);
        void SetUpdateOptions(const TPositionUpdateOptionsBase& aOptions);
        void GetUpdateOptions(TPositionUpdateOptionsBase& aOptions) const;
        void NewTrackingSessionIfTracking();
        void HandleSettingsChangeL(TPosModulesEvent aEvent);
        void NotifyServerShutdown();
        void ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& aAdditionalTime);

    protected:  // From CActive

        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();

    private:
        CPositionRequest(CPosModuleSettings& aModuleSettings,
        		CPosLocMonitorReqHandlerHub&    aLastPositionHandler,
        		TProxyPositionerConstructParams& aPositionerParams,
        		TBool aIsProxy);

        void ConstructL();
        CPositionRequest( const CPositionRequest& );
        CPositionRequest& operator= ( const CPositionRequest& );

        void CompleteRequest(TInt aReason);
        void CompleteClient(TInt aReason);
        void CompleteSelf(TInt aReason);

        void StartPositionDataRequestPhase();
        void StartTrackingTimerWaitPhase();

        TInt PackPositionData();
        void SaveAsLastKnownPosition();

        static TInt HandleTimeOut(TAny* aPositionRequest);
        static TInt TrackingCallback(TAny* aPositionRequest);

        void HandleTrackingStateL();
        void RestartTrackingL();
        void StopTracking();
        void ContinueTrackingTimer();

        void StartPsyTrackingL();
        void StopPsyTracking();

        void LoadPositionerL();

    private:  // Data

        enum TPosRequestStage
            {
            EPosReqInactive = 0,
            EPosReqPositionRequest,
            EPosWaitForTracking
            };

        enum TPosTrackingState
            {
            EPosNoTracking = 0,         /** No tracking is ongoing */
            EPosFirstTrackingRequest,   /** This request completes immediately
                                            Next will be delayed by interval */
            EPosTracking,               /** Internal and PSYs timers started */
            EPosStopTracking            /** Tracking marked to stop */
            };

        RMessage2                   	iMessage;
    	HBufC8*                     	iPositionBuffer;
        TPosRequestStage            	iRequestPhase;
        TInt                        	iRequestId;
        CPositioner*                	iPositioner; 
        TPositionModuleInfo         	iModuleInfo;
        TProxyPositionerConstructParams  iPositionerParams;
        TBool                       	iHasProxyPositioner;

        TTime                       	iReqStartTime;        
        CPosCallbackTimer*          	iTimeoutTimer;
        TTimeIntervalMicroSeconds   	iTimeOut;
        TBool                       	iRequestTimedOut;

        TBool                       	iPositionerTrackingStarted;
        TPosTrackingState           	iTrackingState;
        CPosCallbackTimer*          	iTrackingTimer;
        TTimeIntervalMicroSeconds  	 	iTrackingUpdateInterval;
        TBool                       	iTrackingOverridden;
        const RRequestorStack*      	iTrackingRequestorStackPtr;
        TBool                       	iNewTrackingInterval;

//        CPosLocMonitorReqHandlerHub&    iLastPositionHandler;
        CPosLocMonitorReqHandlerHub&    iLocMonitorReqHandler;
        CPosModuleSettings&         	iModuleSettings;
    };  

#endif  // CPOSITIONREQUEST_H

// End of File
