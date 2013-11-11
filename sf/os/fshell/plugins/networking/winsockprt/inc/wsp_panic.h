// wsp_panic.h
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __WSP_PANIC_H__
#define __WSP_PANIC_H__

enum TWspPanic
	{
	EWinSockPrtThreadAlreadyExists,
	EWinSockPrtInvalidCompletionStatus,
	EWinSockPrtInvalidMessageParentThreadHandle,
	EWinSockPrtInvalidMessageReadBuffer,
	EWinSockPrtInvalidMessageWriteBuffer,
	EWinSockPrtInvalidFactoryOppCode,
	EWinSockPrtInvalidSocketOppCode,
	EWinSockPrtInvalidSubSessionType,
	EWinSockPrtActiveQueueNotEmpty,
	EWinSockPrtUnexpectedWaitError,
	EWinSockPrtTooManyWaitObjects,
	EWinSockPrtUnableToFindSubSession,
	EWinSockPrtUnexpectedBindRequestFromPeer,
	EWinSockPrtUnexpectedBindToRequestFromSocketServer,
	EWinSockPrtUnexpectedSendDownCallFromBindee,
	EWinSockPrtUnexpectedProcessUpCallFromBindee,
	EWinSockPrtUnexpectedErrorUpCallFromBindee,
	EWinSockPrtUnexpectedActiveOpenCallWithConnectionData,
	EWinSockPrtUnexpectedActiveShutdownWithConnectionData,
	EWinSockPrtUnexpectedActivePassiveOpenWithConnectionData,
	EWinSockPrtSocketWrapperConnectWhilstActive,
	EWinSockPrtSocketWrapperSendWhilstActive,
	EWinSockPrtSocketWrapperSendToWhilstActive,
	EWinSockPrtSocketWrapperReceiveWhilstActive,
	EWinSockPrtSocketWrapperReceiveFromWhilstActive,
	EWinSockPrtSocketWrapperAcceptWhilstActive,
	EWinSockPrtSocketWrapperUnexpectedRunError,
	EWinSockPrtInvalidTcpSocketRequestType,
	EWinSockPrtInvalidUdpSocketRequestType,
	EWinSockPrtSubSessionInvalidMessage,
	EWinSockPrtUnexpectedWSAEnumNetworkEventsError,
	EWinSockPrtUnexpectedWSANetworkEvent,
	EWinSockPrtUnexpectedCWin32ActiveObjectCloseHandleError,
	EWinSockPrtUnexpectedWSASendCompletionError,
	EWinSockPrtUnexpectedWSASendToCompletionError,
	EWinSockPrtUnexpectedWSAReceiveCompletionError,
	EWinSockPrtUnexpectedWSAReceiveFromCompletionError,
	EWinSockPrtCWin32SocketMultipleConnectRequests,
	EWinSockPrtCWin32SocketMultipleSendRequests,
	EWinSockPrtCWin32SocketMultipleSendToRequests,
	EWinSockPrtCWin32SocketMultipleReceiveRequests,
	EWinSockPrtCWin32SocketMultipleReceiveFromRequests,
	EWinSockPrtMultipleRequests,
	EWinSockPrtInvalidMessage,
	EWinSockPrtSubSessionListNotEmpty,
	EWinSockPrtCWin32ResolverMultipleRequests,
	EWinSockPrtInvalidResolverOppCode,
	EWinSockPrtResolverWrapperGetByNameWhilstActive,
	EWinSockPrtResolverWrapperGetByAddressWhilstActive,
	EWinSockPrtResolverWrapperUnexpectedRunError,
	EWinSockPrtResolverInvalidWorkerThreadRequest,
	EWinSockPrtInvalidFactoryOpCode,
	EWinSockPrtResolverInvalidFamily,
	EWinSockPrtResolverInvalidAddressLength,
	EWinSockPrtResolverUnexpectedSetEventError,
	EWinSockPrtSocketWrapperInvalidSocketOppCode,
	EWinSockPrtUnexpectedWouldBlockWinSockError,
	EWinSockPrtUnexpectedInProgressWinSockError,
	EWinSockPrtUnexpectedAlreadyInProgressWinSockError,
	EWinSockPrtUnexpectedNotASocketWinSockError,
	EWinSockPrtUnexpectedTooManyFilesWinSockError,
	EWinSockPrtUnexpectedShutdownInProgressWinSockError,
	EWinSockPrtCWin32SocketGetSocketNameInvalidSocketAddress,
	EWinSockPrtCWin32SocketGetPeerNameInvalidSocketAddress,
	EWinSockPrtCWin32SocketCreateSocketUnexpectedValidSocket,
	EWinSockPrtCWin32SocketMultipleAcceptRequests,
	EWinSockPrtCWin32SocketAcceptBlankSocketNotNull,
	EWinSockPrtCWinSockTcpProviderDoPassiveOpenLAcceptSapNotNull,
	EWinSockPrtCWin32SocketHandleAcceptReadyUnexpectedEventSelectError,
	EWinSockPrtCWinSockTcpProviderSetListeningSapAlreadySet,
	EWinSockPrtCWinSockTcpProviderStartAcceptSapNotNull,
	EWinSockPrtCWinSockUdpProviderActiveOpenUnexpectedCall,
	EWinSockPrtCWinSockUdpProviderPassiveOpenUnexpectedCall,
	EWinSockPrtCWin32SocketConstructLInvalidSocketType,
	EWinSockPrtCWin32SocketSendToCompletionAllDataNotSent,
	EWinSockPrtCWin32SocketReceiveFromCompletionInvalidAddressLength,
	EWinSockPrtCWinSockProtocolNewSAPLInvalidProtocol,
	EWinSockPrtCWin32SocketGetOptionInvalidParameters,
	EWinSockPrtCWin32SocketSetOptionInvalidParameters
	};

void Panic(TWspPanic aPanic);

#endif // __WSP_PANIC_H__
