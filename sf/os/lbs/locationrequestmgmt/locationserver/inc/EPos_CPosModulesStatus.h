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



#ifndef CPOSMODULESSTATUS_H
#define CPOSMODULESSTATUS_H

//  INCLUDES
#include <lbs.h>
#include "EPos_MPosStatusObserver.h"
#include <lbs/epos_mposmodulesobserver.h>
#include "epos_mposmodulestatusmanager.h"

// FORWARD DECLARATIONS
class CPosModules;

// CLASS DECLARATION

/**
*  Holds the status of all installed modules.
*  Used to get current module status and to get notification of 
*  module status changes.
*/
class CPosModulesStatus : public CBase,
                          public MPosStatusObserver,
                          public MPosModuleStatusManager
    {
    public:  // Data types

        struct TPosSubscriber
            {
            TAny*                   iClient;
            TInt                    iClientTimeStamp;
            TBool                   iToBeRemoved;
            TBool                   iHasRequest;
            RMessage2               iMsg;
            TPositionModuleId       iModuleId;
            TPositionModuleStatusEvent iEvent;
            };

        struct TPosModule
            {
            TPositionModuleId       iModuleId;
            TPositionModuleStatus   iStatus;
            TInt                    iDeviceTimeStamp;
            TInt                    iDataQualityTimeStamp;
            };

    public:  // Constructors and destructor
        static CPosModulesStatus* NewL(CPosModules& aModules);
        static CPosModulesStatus* NewL(CPosModules& aModules,const TTimeIntervalMicroSeconds32& aTimerInterval);
        virtual ~CPosModulesStatus();
    
    public: //From base class
        virtual void AddListenerL(MPosModuleStatusListener& aListener);
        virtual void RemoveListener(MPosModuleStatusListener& aListener);
        virtual TInt GetModuleStatus(const TPositionModuleId& aModuleId,TPositionModuleStatusBase& aStatus ) const;

    public: // New functions
        void NotifyModuleStatusEventL(const RMessage2& aMessage, TAny* aClient);
        void CancelNotifyModuleStatusEventL(TAny* aClient);
        void NotifySessionClosed(TAny* aClient);
        void HandleSettingsChangeL(TPosModulesEvent aEvent);

    protected:  // Functions from base classes
        void ReportStatus(const TPositionModuleId& aImplementationUid, const TPositionModuleStatus& aStatus);

    private:
        CPosModulesStatus(CPosModules& aModules, const TTimeIntervalMicroSeconds32& aTimerInterval);
        void ConstructL();
        CPosModulesStatus( const CPosModulesStatus& );
        CPosModulesStatus& operator= ( const CPosModulesStatus& );

        
        void ValidateRequestL(const RMessage2& aMessage, TPositionModuleId& aId, TPositionModuleStatusEvent& aEvent);
        TBool HasValidEventsQueued(TPosSubscriber& aSubscriber);
        void NotifySubscribers(TPositionModuleStatusEvent& aOccurredEvent);
        void NotifyListeners(TPositionModuleStatusEvent& aOccurredEvent);
        static TInt TimerCallback(TAny* aModulesStatus);
        TInt DoTimerCallback();
        void RequestComplete(const RMessage2& aMessage, TInt aCompleteCode);
        void NotifyEventToSubscriber(TPositionModuleStatusEvent& aOccurredEvent, const CPosModulesStatus::TPosSubscriber& aSubscriber);
        
    private:    // Data
        CPosModules&            iModules;
        RPointerArray< MPosModuleStatusListener > iListenerArray;
        RArray<TPosModule>      iModuleStatusArray;
        RArray<TPosSubscriber>  iSubscribers;
        TInt                    iSequenceNo;
        CPeriodic*              iTimer;
        TTimeIntervalMicroSeconds32 iTimerInterval;
    };

#endif      // CPOSMODULESSTATUS_H

// End of File
