/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common constant definitions for client-server functionality
*
*/


#ifndef MEDIATORCOMMON_H
#define MEDIATORCOMMON_H

#include <e32base.h>
#include "MediatorServiceDefs.h"

_LIT(KMediatorServerProcessName, "MediatorServer");
_LIT(KMediatorServerName,"!MediatorServer"); // name of a protected server begins with exclamation mark
_LIT(KMediatorServerExe,"MEDIATORSERVER.EXE");

const TUint KMediatorServerMajor=1;
const TUint KMediatorServerMinor=0;
const TUint KMediatorServerBuild=0;

enum TMediatorServerOpCodes
	{
	ERegisterEventList      = 0,
	ERegisterCommandList    = 1,
	EUnregisterEventList    = 2,
	EUnregisterCommandList  = 3,
	ERaiseEvent             = 4,
	// From this point up need SWEvent capability (0-4)
	ESubscribeEventList     = 5,    
	EUnsubscribeEventList   = 6,
    EIssueCommand           = 7,
    ECancelCommand          = 8,
    EIssueResponse          = 9,
    EWaitForEvent           = 10,
    EWaitForCommand         = 11,
    EWaitForCommandResponse = 12,
    EWaitForNotifications   = 13,
    ECancelNotifications    = 14,
    EGetDomains             = 15,
    EGetCategories          = 16,
    EGetEvents              = 17,
    EGetCommands            = 18,
    EFetchParameterData     = 19,
    ECancelAll              = 20,
    EFetchNotificationEventList = 21,
    EFetchNotificationCommandList = 22
	};

_LIT(KMediatorServerPanic, "MediatorServerPanic");

/// Panic enums
enum TMediatorClientPanics
    {
    EMediatorClientNoSessionActive,
    EMediatorClientBadRequest
    };

/// Panic enums
enum TMediatorServerPanics
    {
    EMediatorServerNoEventHandler,
    EMediatorServerNoCommandHandler,
    EMediatorServerNoObjectHandler,
    EMediatorServerNoPluginHandler,
    EMediatorServerIllegalParameter
    };

// enum for command/event notification message
enum TMediatorNotificationType
    {
    EMediatorEventsRegistered,
    EMediatorCommandsRegistered,
    EMediatorEventsUnregistered,
    EMediatorCommandsUnregistered,
    EMediatorCategoryUnregistered
    };

// structure for item type
enum TItemType
    {
    EItemEvent,
    EItemCommand
    };

// command status enumeration
enum TCommandStatus
    {
    ECommandPending, // command is waiting for completion
    ECommandCanceled // command has been cancelled
    };    

// Structure for commands
class TMediatorCategory
	{
public:
	TUid            iDomain;
	TUid            iCategory;
	};

// Package buffers for client-server -communication
typedef TPckgBuf<TMediatorCategory>              TMediatorCategoryBuffer;
typedef TPckgBuf<MediatorService::RDomainList>   TDomainListBuffer; 
typedef TPckgBuf<MediatorService::REventList>    TEventListBuffer;
typedef TPckgBuf<MediatorService::RCommandList>  TCommandListBuffer;
typedef TPckgBuf<MediatorService::TEvent>        TEventBuffer;
typedef TPckgBuf<MediatorService::TCommand>      TCommandBuffer;

// Return data buffers
typedef TPckg<MediatorService::RDomainList>      TDomainListRetBuffer; 
typedef TPckg<MediatorService::RCategoryList>    TCategoryListRetBuffer; 
typedef TPckg<MediatorService::REventList>       TEventListRetBuffer;
typedef TPckg<MediatorService::RCommandList>     TCommandListRetBuffer;
typedef TPckg<TMediatorCategory>                 TMediatorCategoryRetBuffer;
typedef TPckg<MediatorService::TEvent>           TEventRetBuffer;
typedef TPckg<MediatorService::TCommand>         TCommandRetBuffer;
typedef TPckg<TMediatorNotificationType>         TNotificationTypeRetBuffer;

// For Client/Server message sizes
const TInt KMediatorMaxDomainCount          = 50;
const TInt KMediatorMaxCategoryCount        = 50;
const TInt KMediatorMaxCommandCount         = 500;
const TInt KMediatorMaxEventCount           = 500;

// For data buffers
const TInt KMaxEventData = 64;
const TInt KMaxCommandData = 64;


#endif	// MEDIATORCOMMON_H

// End of File
