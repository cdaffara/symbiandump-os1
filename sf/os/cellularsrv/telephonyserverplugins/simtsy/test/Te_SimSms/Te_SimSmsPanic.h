// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Te_SimTsyPanic.H
// Panic codes and panic function. This header should be kept private and not exported to 
// /epoc32/include/
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef __PANIC_H__
#define __PANIC_H__

enum TTSYPanic
	{
//
// Please add new panic codes to the end of the list so that panic code values
// do not change meaning.
// You can name the panic code as you wish.
//
	EGeneral,
	EUnexpectedState,
	EATCommand_NotConstructed,
	EATCommand_IllegalCompletionWaitExpected,
	EATCommand_IllegalCompletionWriteExpected,
	EATCommand_IllegalCompletionWriteNotExpected,
	EATCommand_IllegalCompletionReadExpected,
	EATCommand_IllegalCompletionReadNotExpected,
	EATCommand_IllegalWaitCompletion,
	EATCommand_IllegalResponse,
	EIllegalTsyReqHandle,
	EIllegalPanicOccurredValue,
	EIllegalOwnedState,
	ENotifications_Remaining,
	ELastEvents_Remaining,
	ECalls_Remaining,
	EFaxPtrNull,
	EFaxCompletionPtrNull,
	EATCommand_NotInProgress,
	ERelinquishOwnershipWithNoCall,
	EIllegalTsyNameInConnectedModemRecord,
	EIllegalCallDuration,
	EIllegalEvent,
	EPreAllocatedCallDoesNotExist,
	ENewCallDoesNotExist,
	ECallNameAlreadyExists,
	ERingingCallNotFound,
	ELineAlreadyExists,
	EAcquirerNotFound,
	EUnknownPhoneBookStorageTypeIndex,
	EIllegalPhoneBookState,
	EFaxServerNotNull,
	ENotDataCallType,
	EATSmsUtilsDescriptorOverflow,
	EATSmsUtilsOddNumberOfSemiOctets,
	EATSmsCommandsWrongRequest,
	EATSmsCommandsUnknownRequestStage,
	EATSmsCommandsUnknownRequest,
	EATSmsCommandsWrongRequestToCancel,
	EATSmsMessagingSendNullMsgAttributes,
	EATSmsMessagingSCATooLarge,
	EATSmsMessagingUnknownRequestCompleted,
	EMobileSmsMessagingUnknownIPC,		
	EMobileSmsMessagingUnknownParamVersion,				
	EMobileSmsMessagingNotYetImplemented,				
	EMobileSmsMessagingNullParameter,					
	EMobileSmsMessagingUnexpectedState,					
	EMobileSmsMessagingReceiveBufferEmpty,				
	EMobileSmsMessagingASCIIPduHasOddLength,			
	EMobileSmsMessagingDigitCountOutOfRange,			
	EMobileSmsMessagingCanNotReceiveUnstoredMessage,	
	EMobileSmsMessagingMessageNotReserved,				
	EMobileSmsMessagingClientAllocatedTooLittle,		
	EMobileSmsMessagingClientAllocatedTooMuch,			
	EMobileSmsMessagingSmsCapsNotValid,					
	EMobileSmsMessagingIllegalIPCCallForRxMode,			
	EMobileSmsMessagingAlreadyStarted,					
	EMobileSmsMessagingCanNotReceiveStoredMessage,
	ECMTHandlerDesPassedWithZeroLength,
	EATGetSignalNullParameter,
	EReceiveSmsQueueRequestOutstanding,
	EMobileSmsMessagingPhoneNotSetToAReceiveMode,
	EMobileSmsMessagingPhoneNotSetToASupportedReceiveMode,
	ESmsRxQueuePduReadNullPointers,
	ESmsRxQueuePduReadNoState,
	ESmsRxQueuePduReadStopCalled,
	ESmsRxQueuePduReadNonNullExpectString,
	ESmsRxQueuePduReadNullExpectString,
	ESmsRxQueuePduReadNullParameter,
	ESmsRxQueuePduReadSCARemovalFailure,
	EATSmsUtilsUnknownStoreName,
	EATWaitForCMTNoState,
	EATWaitForCMTSCARemovalFailure,
	EATWaitForCMTPDUTooLong,
	ETsyConfigNullDBPointer,
	ETsyConfigAmbiguousModemTables,
	ETsyConfigMMTSYNotInModemTables,
	EPacketContextsRemaining,
	EPacketContextDoesNotExist,
	EIllegalNullPtrParameter

	};

GLREF_C void Panic(TTSYPanic aPanicNumber);

#endif
