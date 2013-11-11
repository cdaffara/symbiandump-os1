/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* NetDial internal errors and panic codes
* 
*
*/



/**
 @file ND_STD.H
 @internalComponent
*/

#ifndef __ND_STD_H__
#define __ND_STD_H__

#include <nd_err.h>

const TInt KNetdialInternalErrorBase=KNetdialErrorBase-100;

// script errors - internal only 
//
const TInt KErrScriptCompleted=KNetdialInternalErrorBase-1;			///< Script Completed
const TInt KErrNoExpression=KNetdialInternalErrorBase-2;			///< Expression/Term not found
const TInt KErrMissingQuote=KNetdialInternalErrorBase-3;			///< Missing Quote from String
const TInt KErrVariableNotFound=KNetdialInternalErrorBase-4;		///< Variable not found
const TInt KErrInvalidNumber=KNetdialInternalErrorBase-5;			///< Invalid number
const TInt KErrNumberOutOfRange=KNetdialInternalErrorBase-6;		///< Number out of range
const TInt KErrNoEquals=KNetdialInternalErrorBase-7;				///< Could not find equals sign in SET expression
const TInt KErrNoOpenExpression=KNetdialInternalErrorBase-8;		///< Could not find Open Expression character ('{')
const TInt KErrIllegalWaitLabelIndex=KNetdialInternalErrorBase-9;	///< Illegal Wait Index detected (Internal Error)
const TInt KErrNestedLoop=KNetdialInternalErrorBase-10;				///< Nested Loops are not allowed
const TInt KErrLoopCounterOutOfRange=KNetdialInternalErrorBase-11;	///< Loop counter out of range
const TInt KErrMissingChevron=KNetdialInternalErrorBase-12;			///< Missing Chevron from Number Definition
const TInt KErrIllegalCommand=KNetdialInternalErrorBase-13;			///< Illegal Command
const TInt KErrNoDropOrRaise=KNetdialInternalErrorBase-14;			///< no "drop" or "raise" after DTR
const TInt KErrVariableNameTooLong=KNetdialInternalErrorBase-15;	///< Variable name too long
const TInt KErrMissingBracket=KNetdialInternalErrorBase-16;			///< Could not find ('[' or ']')
const TInt KErrCharacterConversionError=KNetdialInternalErrorBase-17;	///< Error occurred durinfg character set conversion
const TInt KErrCharacterSetDoesNotExist=KNetdialInternalErrorBase-18;	///< Character set does not exist
const TInt KErrLabelNameTooLong=KNetdialInternalErrorBase-19;			///< Lable name too long.

enum TNetDialPanic
	{
	EUnknown =0,									// 0
	ECannotStartDialInWithoutMode,
	ECannotStartDialInFromDialOutMode,				
	ENullStateMachineOnReconnection,
	EDialInReconnectNotSupported,
	EDialOutConnectionAlreadyStarted,
	EConnectionAlreadyStarted,
	ENotDoingDialOutBeforeCallBack,
	EConnectionModeLocked,
	ECallBackNotPending,
	EUnknownStartType,								// 10
	ENullStateMachineOnAuthentication,
	ENullStateMachineOnCancelAuthentication,
	ENullStateMachineOnAskReconnect,
	ENullStateMachineOnCancelAskReconnect,
	ENullStateMachineOnGetExcessData,
	EErrorInCancelOpen,
	EUnknownFSMState,
	EIllegalStateTransition,
	ECompleteWhenCancel,
	ENullTelServPrc,								// 20
	EGetLoginInfoCompleteCalled,
	ENotSupported,
	EIllegalReadComplete,
	EIllegalWriteComplete,
	EIllegalTimeOutComplete,
	EIllegalWritePending,
	ENullSearchArray,
	ELastCommandNotNull,
	EIllegalReadPctComplete,
	ENullScriptReader,								// 30
	ENumOfVariablesIncorrect,
	EDestroyNotificationNotCancelled,
	ENullScript,
	EEtelServerNotIdle,
	EEtelPortNotRecovered,
	EEtelCallAlreadyOpen,
	EEtelCallNotOpen,
	ENullCallBackContext,
	EOffsetExceedsLineLength,
	ENullCommChatString,							// 40
	EIllegalDbRequestForService,
	EUnknownTableName,
	EUnknownRecordId,
	ETableAlreadyOpen,
	EOverrideValueLengthTooShort,
	ENullContextPointer,
	EZeroLengthScript,
	ETelServPrcNotNull,
	EPreviousModeDbIsNull,
	EUnknownDatabaseType,							// 50
	ECallBackCompletedWithError,
	EAttemptHSCSDWhenNotGsm,
	EUnknownCallType,
	EUnknownCallDirection,
	ELoggerNotDefined,
	EScriptingUnsupportedInDialIn,
	EAttemptHSCSDWhenNotMultimode
	};

GLREF_C void NetDialPanic(TNetDialPanic aPanic);

const TText KLineFeed=10;
const TText KCarriageReturn=13;
const TInt KTxBufferSize=100;
const TInt KRxBufferSize=64;

#endif
