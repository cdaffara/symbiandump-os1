// wsp_misc.cpp
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

#include <e32def.h>
#include <winsock2.h>
#include "wsp_protocolfamily.h"
#include "wsp_panic.h"
#include "wsp_log.h"


extern "C"
	{
	IMPORT_C CProtocolFamilyBase* InstallWinSockPrt();
	}

EXPORT_C CProtocolFamilyBase* InstallWinSockPrt()
	{
	WSP_LOG(User::LeaveIfError(WspLog::Open()));
	WSP_LOG(WspLog::Write(_L("InstallWinSockPrt")));
	return CWinSockProtocolFamily::NewL();
	}

void Panic(TWspPanic aPanic)
	{
	_LIT(KWspPanicCategory, "WinSockPrt");
	User::Panic(KWspPanicCategory, aPanic);
	}

TInt MapWinSockError(TInt aWinSockError)
	{
	switch (aWinSockError)
		{
		case 0:
			return KErrNone;
		case WSAEINTR:
			return KErrCancel;
		case WSAEBADF:
			return KErrUnknown;
		case WSAEACCES:
			return KErrAccessDenied;
		case WSAEFAULT:
			return KErrBadName;
		case WSAEINVAL:
			return KErrArgument;
		case WSAEMFILE:
			Panic(EWinSockPrtUnexpectedTooManyFilesWinSockError);
		case WSAEWOULDBLOCK:
			Panic(EWinSockPrtUnexpectedWouldBlockWinSockError);
		case WSAEINPROGRESS:
			Panic(EWinSockPrtUnexpectedInProgressWinSockError);
		case WSAEALREADY:
			Panic(EWinSockPrtUnexpectedAlreadyInProgressWinSockError);
		case WSAENOTSOCK:
			Panic(EWinSockPrtUnexpectedNotASocketWinSockError);
		case WSAEDESTADDRREQ:
			return KErrArgument;
		case WSAEMSGSIZE:
			return KErrTooBig;
		case WSAEPROTOTYPE:
			return KErrArgument;
		case WSAENOPROTOOPT:
			return KErrArgument;
		case WSAEPROTONOSUPPORT:
			return KErrNotSupported;
		case WSAESOCKTNOSUPPORT:
			return KErrNotSupported;
		case WSAEOPNOTSUPP:
			return KErrNotSupported;
		case WSAEPFNOSUPPORT:
			return KErrNotSupported;
		case WSAEAFNOSUPPORT:
			return KErrNotSupported;
		case WSAEADDRINUSE:
			return KErrInUse;
		case WSAEADDRNOTAVAIL:
			return KErrArgument;
		case WSAENETDOWN:
			return KErrDisconnected;
		case WSAENETUNREACH:
			return KErrGeneral;
		case WSAENETRESET:
			return KErrDisconnected;
		case WSAECONNABORTED:
			return KErrCouldNotConnect;
		case WSAECONNRESET:
			return KErrDisconnected;
		case WSAENOBUFS:
			return KErrNoMemory;
		case WSAEISCONN:
			return KErrAlreadyExists;
		case WSAENOTCONN:
			return KErrDisconnected;
		case WSAESHUTDOWN:
			return KErrDisconnected;
		case WSAETOOMANYREFS:
			return KErrTooBig;
		case WSAETIMEDOUT:
			return KErrTimedOut;
		case WSAECONNREFUSED:
			return KErrAccessDenied;
		case WSAELOOP:
			return KErrUnknown;
		case WSAENAMETOOLONG:
			return KErrTooBig;
		case WSAEHOSTDOWN:
			return KErrCouldNotConnect;
		case WSAEHOSTUNREACH:
			return KErrGeneral;
		case WSAENOTEMPTY:
			return KErrUnknown;
		case WSAEPROCLIM:
			return KErrTooBig;
		case WSAEUSERS:
			return KErrUnknown;
		case WSAEDQUOT:
			return KErrUnknown;
		case WSAESTALE:
			return KErrUnknown;
		case WSAEREMOTE:
			return KErrUnknown;
		case WSASYSNOTREADY:
			return KErrNotReady;
		case WSAVERNOTSUPPORTED:
			return KErrNotSupported;
		case WSANOTINITIALISED:
			return KErrGeneral;
		case WSAEDISCON:
			Panic(EWinSockPrtUnexpectedShutdownInProgressWinSockError);
		case WSAENOMORE:
			return KErrGeneral;
		case WSAECANCELLED:
			return KErrCancel;
		case WSAEINVALIDPROCTABLE:
			return KErrArgument;
		case WSAEINVALIDPROVIDER:
			return KErrArgument;
		case WSAEPROVIDERFAILEDINIT:
			return KErrGeneral;
		case WSASYSCALLFAILURE:
			return KErrArgument;
		case WSASERVICE_NOT_FOUND:
			return KErrNotFound;
		case WSATYPE_NOT_FOUND:
			return KErrNotFound;
		case WSA_E_NO_MORE:
			return KErrGeneral;
		case WSA_E_CANCELLED:
			return KErrCancel;
		case WSAEREFUSED:
			return KErrAccessDenied;
		default:
			return KErrUnknown;
		}
	}
