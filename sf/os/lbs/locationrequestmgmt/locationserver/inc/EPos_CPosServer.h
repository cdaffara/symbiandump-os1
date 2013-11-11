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




#ifndef CPOSSERVER_H
#define CPOSSERVER_H

//  INCLUDES
#include <e32base.h>
#include <connect/sbdefs.h>
#include "epos_mposmodulesettingslistener.h"

#include "EPos_CPosBackupListener.h"

// FORWARD DECLARATIONS
class CSession2;
class CPosModuleSettings;
class CPosModulesStatus;
class CPosServerDelayedShutdown;
class CPosLocMonitorReqHandlerHub;
class CPosBackupListener;

// CLASS DECLARATION


/**
 *  The main server side class.
 */
class CPosServer :  public CPolicyServer, 
                    public MPosModuleSettingsListener,
                    public MPosBackupEventCallback
    {
    private:
        CPosServer(TInt aPriority, const TPolicy &aPolicy, TServerType aServerType = ESharableSessions);
        void ConstructL();
        CPosServer( const CPosServer& );
        CPosServer& operator= ( const CPosServer& );

        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
        TBool FindRootProcess();

    public:  // Constructors and destructor
        static CPosServer* NewL();
        ~CPosServer();

        // Functions from base classes
        void HandleModuleSettingsChangedL(TPosModulesEvent aEvent);
        void HandleBackupOperationEventL();

        // New functions
        /**
         * This magic number has its origin in Symbian development.
         * It is they have some explaining to do, not us.
         */
        enum
            {
            EPriority=950
            };

        void IncrementSessions();
        void DecrementSessions();
        

    protected: 
        CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);

    private:     // Data
        CPosServerDelayedShutdown*  	iShutdown;
        CPosModuleSettings*         	iModuleSettings;
        TInt                        	iNumSessions;
		TTimeIntervalMicroSeconds32		iServerShutdownDelay;
        CPosModulesStatus*          	iModulesStatus;
        CPosBackupListener*         	iBackupListener;
        CPosLocMonitorReqHandlerHub*    iLocMonitorReqHandlerHub;
    };

#endif      // CPOSSERVER_H

// End of File

