// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// enums between the location client and the location server
// 
//

#ifndef __LBSLOCSERVERMESSAGEENUMS_H__
#define __LBSLOCSERVERMESSAGEENUMS_H__

#include <e32std.h>
#include <lbscommon.h>
#include "lbsmessageenums.h"

// message enums
enum ELbsLocServerMessageEnums
	{
	ELbsServerFirstMessageId = ELastSessionMessageId + 1,
	ELbsServerCancelAsyncRequest = ELbsServerFirstMessageId,
	ELbsGetDefaultModuleId,
	ELbsGetNumModules,
	ELbsGetModuleInfoByIndex,
	ELbsGetModuleInfoById,
	ELbsGetModuleStatus,
	ELbsNotifyModuleStatusEvent,
	ELbsEmptyLastKnownPositionStore,
	ELbsServerLastMessageId,
	

	ELbsPositionerFirstMessageId = ELbsServerFirstMessageId + 0x1000,
	ELbsPositionerCancelAsyncRequest = ELbsPositionerFirstMessageId,
	ELbsPositionerOpen,
	ELbsPositionerOpenModuleId,
	ELbsPositionerOpenCriteria,
	ELbsPositionerClose,
	ELbsSetSingleRequestor,
	ELbsSetMultipleRequestors,
	ELbsSetUpdateOptions,
	ELbsGetUpdateOptions,
	ELbsGetLastKnownPosition,
	ELbsPosNotifyPositionUpdate,
	ELbsGetLastKnownPositionArea,
	ELbsPositionerLastMessageId,
	
	
	
	ELbsPositionCustomCodesFirst = ELbsPositionerFirstMessageId + 0x1000,
	ELbsPosNotifyPositionUpdateCancel = ELbsPositionCustomCodesFirst,
	ELbsGetLastKnownPositionCancel,
	ELbsPosNotifyPositionUpdateComplete,
	ELbsPositionCustomCodesLastMessageId,
	};
	
// The following constants represent the old values for EPositionerGetLastKnownPosition
// and EPositionerNotifyPositionUpdate from LbsIpc.h for both the Symbian and variant
// versions. To maintain BC both are checked for in the case statement in 
// CPosSubSession::HandleCancelAsyncRequestL.
const TInt KGetLastKnownPositionSymbian = 1109;
const TInt KGetLastKnownPositionVariant = 0x2009;

const TInt KNotifyPositionUpdateSymbian = 1110;
const TInt KNotifyPositionUpdateVariant = 0x200A;

const TInt KServerNotifyModuleStatusEventSymbian = 1006;
const TInt KServerNotifyModuleStatusEventVariant = 0x1007;

class TOpenParameters
/** This class isused to pass information from the the client to the server during
	the open of a sub-session.
	The module ID, iModuleId, is the ID of the selected position module.
	The Boolean,iOpenedUsingGPSModuleId, is ETrue when the sub-session  has been opened
	using Open(RPositionServer&,TPositionModuleId) and EFalse otherwise..
	*/
	{
public:
	TPositionModuleId 	iModuleId;
	TBool				iOpenedUsingGPSModuleId; 
	
	};	
#endif // __LBSLOCSERVERMESSAGEENUMS_H__
