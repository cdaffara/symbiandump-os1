// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalAll 
*/


#if !defined(__SMSPMAIN_H__)
#define __SMSPMAIN_H__

#include <e32base.h>
#include "smsstacklog.h"
#include "etelmm.h"

/**
 *  @internalComponent
 */
const TInt KSmsSessionPriority = CActive::EPriorityStandard;

/**
 *  @internalComponent
 */
GLREF_C TInt ErrorToRPError(TInt aError);
GLREF_C TInt ErrorToTPError(TInt aError);
GLREF_C void GsmFindOutTonAndNpi(TUint aTypeOfAddress,RMobilePhone::TMobileTON& aTon,RMobilePhone::TMobileNPI& aNpi);
GLREF_C TInt FailureCauseToError(TInt aFailureCause);


//GLREF_C TBool IsRecoverable(TInt aError);

/**
 *  @internalComponent
 */
enum TSmspPanic
	{
//  SMSPSTOR.CPP
	KSmspPanicBadIndexArray=0x0000,
	KSmspPanicBadReferenceArray,
	KSmspPanicNotSubmit,
	KSmspPanicNotCommand,
	KSmspPanicNotStatusReport,
	KSmspPanicEntryWithSameLogServerId,
	KSmspPanicEntryWithLogServerIdNotFound,
	KSmspPanicMessageConcatenated,
	KSmspPanicPreallocatedFileCorrupt,
	KSmspPanicPreallocatedFileNoFreeContainer,
//  SMSPUTIL.CPP
	ESmspSmsActiveAlreadyActive=0x0100,
//  SMSPSEND.CPP
	KSmspPanicUnexpectedState=0x0200,
	KSmspPanicNoServiceCenterAddress,
//  SMSPROT.CPP
	ESmspCantCreateServiceResolver=0x0300,
	ESmspCantCreateNetDatabase,
	ESmspCantCreateHostResolver,
	ESmspCantBind,
	ESmspCantBindTo,
	ESmspCantSend,
	ESmspCantProcess,
	ESmspCantGetOption,
	ESmspCantSetOption,
	ESmspCantError,
	ESmspMessageObserverNotFound,
	ESmspMessageWrongObserver,
	ESmspMessageSendObserverNotFound,
	ESmspMessageWriteObserverNotFound,
	ESmspMessageDeleteObserverNotFound,
	ESmspPhoneNotFound,
	ESmspCorruptObserverList,
	ESmspUnknownSmsAddressFamily,
	KSmspPanicOriginalSmsMessageIsNull,
	KSmspPanicUnknownModemDetection,
	ESmspPanicAddrFamilyNotAllowed,
	ESmspPDUReadProcessNotFound,
	KSmspPhoneHasNotTurnedOn,
	KSmspPhoneHasNotTurnedOff,
	KSmspStartMustOnlyBeCalledOnce,
	KSmspSlotArrayEmpty,
//  SMSPROV.CPP
	KSmspUndefinedLevel=0x0400,
	KSmspUndefinedName,
	ESmspBadIoctlName,
	KSmspPanicBadClientIoctlCall,
	KSmspPanicBadClientMessageRead,
	KSmspPanicOptionBufferNull,
	KSmspPanicWrongSmsAddressFamily,
	KSmspPanicIoctlAlreadyOutstanding,
    KSmspPanicParameterBufferNull,
//  SMSPROC.CPP
	ESmspUnexpectedSmsPDUType=0x0500,
	KSmspPanicUnexpectedCommand,
//  SMSPENUM.CPP
	KSmspPanicStoreInfoNULL=0x0600,
	KSmspPanicStoreInfoArrayNULL,
	KSmspPanicBadSlotIndex,
//  SMSPIMG.CPP
	ESmspSlotAlreadyPresent=0x0700,
	ESmspSlotInvalid,
//	SMSPBEAR.CPP
	ESmspGprsSessionAlreadyOpen=0x0800,
	ESmspCommDbSmsBearerReadError,
	ESmspUnexpectedErrorSettingSmsBearer,
//  SMSPPARA.CPP
    KSmspPanicCorruptedSmspList=0x0900,
//	SMSPREAD.CPP
	KSmspPanicLogIdNull=0x1000,
	KSmspPanicUnexpectedStateEncountered,
//	Queuing
	KSmspQueueNotEmpty=0x2000
	};

/**
 *  @internalComponent
 */
GLDEF_C void SmspPanic(TSmspPanic aPanic);

#endif

