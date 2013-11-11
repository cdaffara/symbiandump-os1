/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Defines all constants used by the TelephonyAudioRouting
*                Server and its clients.
*
*/



#ifndef TELEPHONYAUDIOROUTNGCLIENTSERVER_H
#define TELEPHONYAUDIOROUTNGCLIENTSERVER_H


#include <e32base.h>
#include <e32def.h>
#include <e32svr.h>

/**
* Debug
*/
#ifdef _DEBUG
	#define TELAUDRTNG_RDEBUG(X)		  RDebug::Print(X)
    #define TELAUDRTNG_RDEBUG1(X,Y)       RDebug::Print(X,Y)
#else
	#define TELAUDRTNG_RDEBUG(X)
    #define TELAUDRTNG_RDEBUG1(X,Y)
#endif

// server name
_LIT(KTelAudRtngServName,"telephonyaudioroutingserver");

// A version must be specifyed when creating a session with the server
const TUint KTelAudRtngServMajorVersionNumber=0;
const TUint KTelAudRtngServMinorVersionNumber=0;
const TUint KTelAudRtngServBuildVersionNumber=0;

// opcodes used in message passing between client and server
enum TTelAudRtngServRqst
	{
	ETelAudRtngServDoSetOutput,
	ETelAudRtngServNotifyIfOutputChanged,
	ETelAudRtngServNotifyIfAvailOutputsChanged,
	ETelAudRtngServGetNoOutputs,
	ETelAudRtngServGetAvailableOutputs,
	ETelAudRtngServMonitorOutputChange,
	ETelAudRtngServSetPolicySessionId,
	ETelAudRtngServOutputChangeComplete,
	ETelAudRtngDoAvailableOutputsChanged,
	ETelAudRtngServCancelRequest,
	ETelAudRtngServInitialize,
	ETelAudRtngServOutputChangedByPolicy,
	ETelAudRtngServOutputChangeRequested,
	ETelAudRtngServGetDefaultValues
	};
	
//opcodes used by server to indicate which asynchronous service
//has completed
enum TTelAudRtngServRqstComplete
    {
    ETelAudRtngServSetOutputComplete = 1,
    ETelAudRtngServOutputChanged,
    ETelAudRtngServAvailableOutputsChanged,
    EPolicyOutputChangeRequest
    };

#endif // #ifndef TELEPHONYAUDIOROUTNGCLIENTSERVER_H

