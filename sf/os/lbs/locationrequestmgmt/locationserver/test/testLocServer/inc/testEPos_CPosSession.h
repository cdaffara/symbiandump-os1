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


#ifndef CPOSSESSION_H
#define CPOSSESSION_H

//  INCLUDES
#include <e32base.h>
#include <lbscommon.h>

// TODO - Validate *****************************
#include "testEPos_CPosLocMonitorReqHandlerHub.h"

// FORWARD DECLARATIONS
class CPosServer;
class CPosSubSession;
class CPosModuleSettings;
struct TPosModulesEvent;
class CPrivacyClientArray;
class TPositionModuleInfo;
class CPosSubsessionRegistry;
class CPosModulesStatus;
class CPosLocMonitorReqHandlerHub;

#ifdef _DEBUG
class CPosTrace;
#endif

// CLASS DECLARATION

/**
*  The EPos session class.
*
*  This inherits from CSession2
*/
class CPosSession : public CSession2
    {
    public:  // Constructors and destructor
        ~CPosSession();
        static CPosSession* NewL(CPosServer& aServer,
        		CPosModuleSettings& aModuleSettings,
	        	CPosModulesStatus& aModulesStatus,
	        	CPosLocMonitorReqHandlerHub& aLocMonitorReqHandlerHub);

    public:     // New functions
        void HandleSettingsChangeL(TPosModulesEvent aEvent);
        void NotifyServerShutdown();

    public:		// Functions from CSession2
        void ServiceL(const RMessage2& aMessage);
        void ServiceError(const RMessage2& aMessage, TInt aError);

	private:
        CPosSession(CPosServer& aServer,
        		CPosModuleSettings& aModuleSettings,
                CPosModulesStatus& aModulesStatus,
                CPosLocMonitorReqHandlerHub& aLocMonitorReqHandlerHub);

        void ConstructL();
        CPosSession( const CPosSession& );
        CPosSession& operator= ( const CPosSession& );

        void OpenFromModuleIdL(const RMessage2& aMessage);
        void OpenDefaultPositionerL(const RMessage2& aMessage);
        void OpenPositionerFromCriteriaL(const RMessage2& aMessage);
        void ClosePositioner(const RMessage2& aMessage);
        void GetDefaultModuleIdL(const RMessage2& aMessage);
        void GetNumModulesL(const RMessage2& aMessage);
        void GetModuleInfoByIndexL(const RMessage2& aMessage);
        void GetModuleInfoByIdL(const RMessage2& aMessage);
        void GetModuleStatusL(const RMessage2& aMessage);
        void NotifyModuleStatusEventL(const RMessage2& aMessage);
        void NotifyModuleStatusEventCancelL(const RMessage2& aMessage);
        void HandleCancelAsyncRequestL(const RMessage2& aMessage);
        void EmptyLastKnownPositionStore(const RMessage2& aMessage);		// TODO Verify
        void EmptyLastKnownPositionStoreCancelL(const RMessage2& aMessage); // TODO Verify

        void ForwardToSubSessionL(const RMessage2& aMessage);
        void CreateSubSessionL(TPositionModuleId aId, TBool aIsProxy, const RMessage2& aMessage);
        void RequestComplete(const RMessage2& aMessage, TInt aCompleteCode);

    private:    // Data
        CPosServer&                 iPosServer;
        CPosSubsessionRegistry*     iSubSessionRegistry;
        CPosModuleSettings&         iModuleSettings;
        CPosModulesStatus&          iModulesStatus;
        TBool                       iDecrementSessions;
        TBool                       iServerShutdown;
        CPosLocMonitorReqHandlerHub&    iLocMonitorReqHandlerHub;
    };

#endif      // CPOSSESSION_H

// End of File
