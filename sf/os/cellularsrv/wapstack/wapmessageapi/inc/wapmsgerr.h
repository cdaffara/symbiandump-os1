// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __WAPMSGERR_H__
#define __WAPMSGERR_H__

#include <e32base.h>

namespace Wap
/** 
* Defines WAP-related types and error codes. 
*/
	{
	_LIT(KWapMsgPanicDescriptor, "WAPMESSAGE");

/** Defines start values for categories of WAP-related error codes. */
enum 
	{
	/** Base value for all error codes. */
	ERetCodeBase      = -5500,
	/** Base value for connection-orientated WSP error codes. */
	EWSPCORetCodeBase = ERetCodeBase + 25,
	/** Base value for connectionless WSP error codes. */
	EWSPCLRetCodeBase = ERetCodeBase + 50,
	/** Base value for WTP error codes. */
	EWTPRetCodeBase   = ERetCodeBase + 75,
	/** Base value for WDP error codes. */
	EWDPRetCodeBase   = ERetCodeBase + 100,
	/** Base value for WTLS error codes. */
	EWTLSRetCodeBase  = ERetCodeBase + 125,
	/** Maximum value for WAP-related error codes. */
	ERetCodeTop       = ERetCodeBase + 200
	};
	
/** General WAP error codes. */
enum TGeneralReturnCodes
	{  
	/** There is more data to be read. */
	EMoreData           = ERetCodeBase + 0,
	/** One of: no bearer set; unknown bearer; a connection exists with different bearer; 
	* unable to get a local host name from bearer. */
	EBearerError        = ERetCodeBase + 1,
	/** The port is already used in another bound connection. */
	EPortAlreadyBound   = ERetCodeBase + 2,
	/** Local port cannot be opened. */
	ECannotOpenPort     = ERetCodeBase + 3,
	/** There are too many existing connections. */
	ETooManyConnections = ERetCodeBase + 4,
	/** Connection handle is invalid. */
	EBadConnection      = ERetCodeBase + 5,
	/** Passed buffer is too small. */
	EBufferTooSmall     = ERetCodeBase + 6
	};

/** WTLS-related error codes. */
enum TWtlsReturnCodes
	{
	/** Out of memory. */
	EWtlsOutOfMemory            = EWTLSRetCodeBase + 0,
	/** Socket error. */
	EWtlsSocketError            = EWTLSRetCodeBase + 1,
	/** Invalid parameter used. */
	EWtlsBadParameters          = EWTLSRetCodeBase + 2,
	/** Socket error. */
	EWtlsSocketError2           = EWTLSRetCodeBase + 3,
	/** Resume ID not found. */
	EWtlsResumeIDNotFound       = EWTLSRetCodeBase + 4,
	/** Resume error. */
	EWtlsResumeError            = EWTLSRetCodeBase + 5,
	/** Renegotiation request error. */
	EWtlsRenegotiationRequest   = EWTLSRetCodeBase + 6,
	/** Illegal call. */
	EWtlsIllegalCall            = EWTLSRetCodeBase + 7,
	/** Connection close notify error. */
	EWtlsConnectionCloseNotify  = EWTLSRetCodeBase + 8,
	/** Close notify error. */
	EWtlsSessionCloseNotify     = EWTLSRetCodeBase + 9,
	/** No connection. */
	EWtlsNoConnection           = EWTLSRetCodeBase + 10,
	/** Unexpected message. */
	EWtlsUnexpectedMessage      = EWTLSRetCodeBase + 11,
	/** Bad MAC record. */
	EWtlsBadRecordMac           = EWTLSRetCodeBase + 12,
	/** Decryption failed. */
	EWtlsDecryptionFailed       = EWTLSRetCodeBase + 13,
	/** Record overflow. */
	EWtlsRecordOverflow         = EWTLSRetCodeBase + 14,
	/** Handshake failure. */
	EWtlsHandshakeFailure       = EWTLSRetCodeBase + 15,
	/** Decompression failure. */
	EWtlsDecompressionFailure   = EWTLSRetCodeBase + 16,
	/** Bad certificate. */
	EWtlsBadCertificate         = EWTLSRetCodeBase + 17,
	/** Unsupported certificate. */
	EWtlsUnsupportedCertificate = EWTLSRetCodeBase + 18,
	/** Certificate revoked. */
	EWtlsCertificateRevoked     = EWTLSRetCodeBase + 19,
	/** Certificate expired. */
	EWtlsCertificateExpired     = EWTLSRetCodeBase + 20,
	/** Certificate unknown. */
	EWtlsCertificateUnknown     = EWTLSRetCodeBase + 21,
	/** Illegal parameter. */
	EWtlsIllegalParameter       = EWTLSRetCodeBase + 22,
	/** Unknown certificate authority. */
	EWtlsUnknownCA              = EWTLSRetCodeBase + 23,
	/** Access denied. */
	EWtlsAccessDenied           = EWTLSRetCodeBase + 24,
	/** Decoding error. */
	EWtlsDecodeError            = EWTLSRetCodeBase + 25,
	/** Decryption error. */
	EWtlsDecryptError           = EWTLSRetCodeBase + 26,
	/** Unknown key ID. */
	EWtlsUnknownKeyId           = EWTLSRetCodeBase + 27,
	/** Disabled key ID. */
	EWtlsDisableKeyId           = EWTLSRetCodeBase + 28,
	/** Key exchange disabled. */
	EWtlsKeyExchangeDisabled    = EWTLSRetCodeBase + 29,
	/** Session not ready. */
	EWtlsSessionNotReady        = EWTLSRetCodeBase + 30,
	/** Unknown parameter index. */
	EWtlsUnknownParameterIndex  = EWTLSRetCodeBase + 31,
	/** Duplicate finished Recv() call. */
	EWtlsDuplicateFinishedRecv  = EWTLSRetCodeBase + 32,
	/** Export restriction error. */
	EWtlsExportRestriction      = EWTLSRetCodeBase + 33,
	/** Protocol version error. */
	EWtlsProtocolVersion        = EWTLSRetCodeBase + 34,
	/** Insufficient security. */
	EWtlsInsufficientSecurity   = EWTLSRetCodeBase + 35,
	/** Internal error. */
	EWtlsInternalError          = EWTLSRetCodeBase + 36,
	/** User cancelled operation. */
	EWtlsUserCanceled           = EWTLSRetCodeBase + 37,
	/** No renegotiation. */
	EWtlsNoRenegotiation        = EWTLSRetCodeBase + 38,

	/** General error. */
	EWtlsErrGeneral             = EWTLSRetCodeBase + 39,
	/** Bulk algorithm error. */
	EWtlsErrBulk                = EWTLSRetCodeBase + 40,
	/** MAC algorithm error. */
	EWtlsErrMac                 = EWTLSRetCodeBase + 41,
	/** Sequence number mode error. */
	EWtlsErrSequenceNumberMode  = EWTLSRetCodeBase + 42,
	/** Key refresh rate error. */
	EWtlsErrKeyRefreshRate      = EWTLSRetCodeBase + 43,

	};

/** Connection-orientated WSP error codes. */
enum TWspCoReturnCodes
	{
	/** Given buffer is too small. */
	EWspCoBufferTooSmall      = EWSPCORetCodeBase + 0,
	/** Session handle is not valid. */
	EWspCoInvalidSession      = EWSPCORetCodeBase + 1,
	/** Invalid transaction. */
	EWspCoInvalidTransaction  = EWSPCORetCodeBase + 2,
	/** Given parameter is invalid. */
	EWspCoParameterError      = EWSPCORetCodeBase + 3,
	/** Session is not connected. */
	EWspCoSessionNotConnected = EWSPCORetCodeBase + 4,
	/** Functionality is not supported in this session. */
	EWspCoNotSupported        = EWSPCORetCodeBase + 5,
	/** Function is not applicable in this state of an object. */
	EWspCoInvalidState        = EWSPCORetCodeBase + 6,
	/** Session has been closed. */
	EWspCoSessionClosed       = EWSPCORetCodeBase + 7,
	/** Server MRU exceeded. */
	EWspCoMRUExceeded         = EWSPCORetCodeBase + 8,
	/** Invalid buffer size. */
	EWspCoInvalidBufferSize   = EWSPCORetCodeBase + 9,
	/** Data not available. */
	EWspCoDataNotAvailable    = EWSPCORetCodeBase + 10,
	/** Timeout error. */
	EWspCoErrorTimeout        = EWSPCORetCodeBase + 11
	};
	

/** Connectionless WSP error codes. */
enum TWspClReturnCodes
	{			
	/** Given buffer is too small. */
	EWspClBufferTooSmall      = EWSPCLRetCodeBase + 0,
	/** Session handle is not valid. */
	EWspClInvalidSession      = EWSPCLRetCodeBase + 1,
	/** Given parameter is invalid. */
	EWspClParameterError      = EWSPCLRetCodeBase + 2,
	/** Session is not connected. */
	EWspClSessionNotConnected = EWSPCLRetCodeBase + 3,
	/** Functionality is not supported in this session. */
	EWspClNotSupported        = EWSPCLRetCodeBase + 4,
	/** Function is not applicable in this state of an object. */
	EWspClInvalidState        = EWSPCLRetCodeBase + 5,
	/** Session has been closed. */
	EWspClSessionClosed       = EWSPCLRetCodeBase + 6,
	/** Server MRU exceeded. */
	EWspClMRUExceeded		 = EWSPCLRetCodeBase + 7,	
	/** Invalid buffer size. */
	EWspClInvalidBufferSize   = EWSPCLRetCodeBase + 8,
	/** Data not available. */
	EWspClDataNotAvailable    = EWSPCLRetCodeBase + 9,
	/** Timeout error. */
	EWspClErrorTimeout        = EWSPCLRetCodeBase + 10
	};

/** WTP-related error codes. */
enum TWtpReturnCodes
	{
	/** Out of memory. */
	EWtpNoMemory                 = EWTPRetCodeBase + 0,
	/** Invalid argument. */
	EWtpBadArgument              = EWTPRetCodeBase + 1,
	/** Provider aborted. */
	EWtpProviderProtoErr         = EWTPRetCodeBase + 2,
	/** Provider aborted. */
	EWtpProviderAbortInvalidTID  = EWTPRetCodeBase + 3,
	/** Provider aborted. */
	EWtpProviderAbortNoClass2    = EWTPRetCodeBase + 4,
	/** Provider aborted. */
	EWtpProviderAbortNoSAR       = EWTPRetCodeBase + 5,
	/** Provider aborted. */
	EWtpProviderAbortNoUACK      = EWTPRetCodeBase + 6,
	/** Provider aborted. */
	EWtpProviderAbortVers1       = EWTPRetCodeBase + 7,
	/** Provider aborted. */
	EWtpProviderAbortNoResponse  = EWTPRetCodeBase + 8,
	/** Provider aborted. */
	EWtpProviderAbortMsgTooLarge = EWTPRetCodeBase + 9,
	/** Provider aborted. */
	EWtpProviderAbortGeneral     = EWTPRetCodeBase + 10,
	/** Service has shutdown. */
	EWtpShutdownError            = EWTPRetCodeBase + 11
	};
}

#endif // __WAPMSGERR_H__
