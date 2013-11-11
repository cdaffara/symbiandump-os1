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
//

/**
 @file
 @internalComponent
*/

#ifndef __AsciiRqstDefs_H__
#define __AsciiRqstDefs_H__

// Request Format (Master to Slave)
//-----------------------------------

//	MsgTypeRqst,MsgLen,MsgNum,OpCode,ArgumentDescriptor1...ArgumentDescriptorN,EOM
	
//	no spaces between commas
//  MsgLen is of the entire msg including the MsgTypeRqst and MsgLen fields
//	MsgNum increments for each request msg and re-cycles after "99" back to "00"
//	ArgumentDescriptor fields are optional


// Number of elements in a request msg, not including arguments
const TUint KRqstMsgElementsNoArgs = 5;
const TUint KMaxMsgLenChars = 4; // in practise, max reqst length should be less than 9999 chars!
const TUint KMaxMsgNumChars = 2; // "01" to "99" max
const TUint KRqstOffset 	= 0;
const TUint KMsgLenOffset 	= KRqstOffset+1;
const TUint KMsgNumOffset 	= KMsgLenOffset+1;
const TUint KOpCodeOffset 	= KMsgNumOffset+1;
const TUint KArg1Offset 	= KOpCodeOffset+1;
const TUint KArg2Offset 	= KArg1Offset+1;
const TUint KArg3Offset 	= KArg2Offset+1;

const TUint	KMaxElementSize	= 100;	// Max size of a data field

const TUint KArgNumber1 = 1;
const TUint KArgNumber2 = 2;
const TUint KArgNumber3 = 3;



// Response Format (Slave to Master)
//-----------------------------------

//	MsgTypeResp,MsgLen,MsgNum,RspCode,ArgumentDescriptor1 ... N,EOM
	
//	no spaces between commas
//  MsgLen is of the entire msg including the MsgTypeResp and MsgLen fields
//	MsgNum is that of the request msg to which this is the response
//	ArgumentDescriptor fields are optional


// Number of elements in a response msg, not including arguments
const TUint KRespMsgElementsNoArgs = 5;
						

// MsgType and EOM
_LIT8(KMsgTypeRqst,	"Rqst");
_LIT8(KMsgTypeResp,	"Resp");
const TUint KMsgTypeRqstLen = 4;
const TUint KMsgTypeRespLen = 4;

// Comma seperator
_LIT8(KComma,		",");
const TUint KCommaLen = 1;

// Size of Socket buffer required to be able to read the rqst msg length portion (and the msg header)
const TUint	KReqstSocketPreReadSize = KMsgTypeRqstLen+KCommaLen+KMaxMsgLenChars;

// Size of Socket buffer required to be able to read the resp msg length portion (and the msg header)
const TUint	KRespSocketPreReadSize = KMsgTypeRespLen+KCommaLen+KMaxMsgLenChars;

// OpCodes
// Encoding means less data is transmitted.
// Using textual description means easier debugging but once this is working, it's working!
// Requests
_LIT8(KCallNumber,		"CallNumber");		// Arg1: line Arg2: Tel no
_LIT8(KAnswerNextCall,	"AnswerNextCall");	// Arg1: line
_LIT8(KRejectNextCall,	"RejectNextCall");	// Arg1: line
_LIT8(KIgnoreNextCall,	"IgnoreNextCall");	// Arg1: line
_LIT8(KHangUp,			"HangUp");			// Arg1: line Arg2: Hang up delay (sec)
_LIT8(KHold,			"Hold");			// Arg1: line
_LIT8(KResume,			"Resume");			// Arg1: line

_LIT8(KSendDataBackOnDataPort,	"SendDataBackOnDataPort");	// Arg1: desc data to send

// Ganged requests
_LIT8(KDialHangUp,				"DialHangUp");	 			// Arg1 line Arg2: Tel no. Arg3: Hang up delay (sec)
_LIT8(KDialHangUpBeforeConnect,	"DialHangUpBeforeConnect"); // Arg1 line Arg2: Tel no. Arg3: Hang up delay (sec)			
_LIT8(KDialHangUpAfterConnect,	"DialHangUpAfterConnect");  // Arg1 line Arg2: Tel no. Arg3: Hang up delay (sec)
_LIT8(KAnswerNextCallHangUp,	"AnswerNextCallHangUp");	// Arg1 line Arg2: Hang up delay (sec)
_LIT8(KAnswerNextCallHold, 		"KAnswerNextCallHold");		// Arg1 line Arg2: Hang up delay (sec)
_LIT8(KHangUpNextCallBeforeConnect,	"KHangUpNextCallBeforeConnect");	// Arg1: line
_LIT8(KCleanup,						"KCleanup");			// no Arg
	

// Handshake messages. These are requests from Master to Slave that occur during startup and involve
// data exchange. They are not requests for Etel to perform telephony actions, and should not normally
// be used by external clients.
_LIT8(KHShakeReqstSlaveTelNumbers, "ReqstSlaveTelNumbers"); // Requests Slave's tel numbers (Order is Voice, Data, Fax)

_LIT8(KCheckRpsPresent, "CheckRpsPresent");

// Responses
_LIT8(KOk, 			"OK   "); 
_LIT8(KError, 		"Error");
const TUint KOKLen 		= 5;
const TUint KErrorLen 	= 5;

// EOM
_LIT8(KEOM,			"+EOM+");
const TUint KEOMLen = 5;

// Error Codes
_LIT8(KErrCorruptMsg, 	"CorruptMsg");
_LIT8(KErrUnknownMsg, 	"UnknownErrorInMsg");
_LIT8(KTimedOut, 		"TimedOut");
_LIT8(KRqstIgnored, 	"RequestIgnored");

//lines
_LIT16(KRpsVoiceLine, 	"Voice");
_LIT16(KRpsDataLine, 	"Data");
_LIT16(KRpsFaxLine, 	"Fax");

enum TEtelLine
	{
	EVoiceLine,
	EDataLine,
	EFaxLine
	};

// Request and Response lengths not including OpCodes, Arguments or Comma seperators
const TUint KRqstLengthNoOpCodeNoArgsNoCommas = KMsgTypeRqstLen+KMaxMsgLenChars+KMaxMsgNumChars+KEOMLen;
const TUint KRespLengthNoOpCodeNoArgsNoCommas = KMsgTypeRespLen+KMaxMsgLenChars+KMaxMsgNumChars+KEOMLen;

// Signals that the session with Slave is to be Terminated
_LIT8(KTerminateSession, 	"+TermSess+");


// Misc
_LIT(KPanicMsgFormat, "RPS Slave Panic");
//panic codes
enum TRPSSlavePanicCodes
	{
	KInitError,
	KBadMsgFormat,
	KTelNumberTooBig,
	KListenerNotActive,
	KIncorrectPreReadSize,
	KBadNumberOfArgs,
	KUnsupportedCall,
	KRequestAlreadyActive,
	KObserverAlreadyRegistered
	};


#endif  // __AsciiRqstDefs_H__
