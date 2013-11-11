// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sd_log.h"
#include "sd_apiextensionipc.h"

#include <rscommon.h>

using namespace Den;

EXPORT_C void Logging::Printf(
#ifdef SYMBIAN_TRACE_ENABLE
	TInt aWorkerId, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...)
#else
	TInt, const TDesC8&, TRefByValue<const TDesC8>, ...)
#endif //#ifdef SYMBIAN_TRACE_ENABLE
	{
#ifdef SYMBIAN_TRACE_ENABLE
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(aWorkerId, aSubTag, Logging::ELogInfo, aFmt, list);
	VA_END(list);
#endif //#ifdef SYMBIAN_TRACE_ENABLE
	}

EXPORT_C void Logging::Printf(
#ifdef SYMBIAN_TRACE_ENABLE
	TInt aWorkerId, const TDesC8& aSubTag, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, ...)
#else
	TInt, const TDesC8&, TLogEntryType, TRefByValue<const TDesC8>, ...)
#endif //#ifdef SYMBIAN_TRACE_ENABLE
	{
#ifdef SYMBIAN_TRACE_ENABLE
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(aWorkerId, aSubTag, aType, aFmt, list);
	VA_END(list);
#endif //#ifdef SYMBIAN_TRACE_ENABLE
	}

EXPORT_C void Logging::Printf(
#ifdef SYMBIAN_TRACE_ENABLE
	TInt aWorkerId, const TDesC8& /*aSubTag*/, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
#else
	TInt, const TDesC8&, TLogEntryType, TRefByValue<const TDesC8>, VA_LIST&)
#endif //#ifdef SYMBIAN_TRACE_ENABLE
/**
Write a mulitple argument list to the log, trapping and ignoring any leave

*/
	{
#ifdef SYMBIAN_TRACE_ENABLE
	TLogTextBuf buf;
	_LIT8(KWorkerIdFormat, "W%d: ");
	buf.Format(KWorkerIdFormat, aWorkerId);
	buf.AppendFormatIgnoreOverflow(aFmt, aList);
	UTracePfAny(KPrimaryFilter, aType, ETrue, EFalse, buf.Length(), buf.Ptr(), buf.Length());
#endif //#ifdef SYMBIAN_TRACE_ENABLE
	}

const TText8* Logging::IPCMessName(TInt aMess)
/**
Write a mulitple argument list to the log, trapping and ignoring any leave
*/
	{
	switch(aMess)
		{
		/*case RootServer::ESSNumProtocols: return _S8("ESSNumProtocols");

		*/
		
		case EIpcSubSessionApiExtIfaceOpen: return _S8("EIpcSubSessionApiExtIfaceOpen");
		case EIpcSubSessionApiExtIfaceSend: return _S8("EIpcSubSessionApiExtIfaceSend");
		case EIpcSubSessionApiExtIfaceSendReceive: return _S8("EIpcSubSessionApiExtIfaceSendReceive");
		case EIpcSubSessionApiExtIfaceClose: return _S8("EIpcSubSessionApiExtIfaceClose");
		case EIpcSessionApiExtIfaceOpen: return _S8("EIpcSessionApiExtIfaceOpen");
		case EIpcSessionApiExtIfaceSend: return _S8("EIpcSessionApiExtIfaceSend");
		case EIpcSessionApiExtIfaceSendReceive: return _S8("EIpcSessionApiExtIfaceSendReceive");
		case EIpcSessionApiExtIfaceClose: return _S8("EIpcSessionApiExtIfaceClose");

		case RootServer::RSIsCallerConfigurator: return _S8("RSIsCallerConfigurator");
		case RootServer::RSLoadModule: return _S8("RSLoadModule");
		case RootServer::RSUnloadModule: return _S8("RSUnloadModule");
		case RootServer::RSBind: return _S8("RSBind");
		case RootServer::RSUnbind: return _S8("RSUnbind");
		case RootServer::RSGetModuleInfo: return _S8("RSGetModuleInfo");
		case RootServer::RSEnumerateModules: return _S8("RSEnumerateModules");
		case RootServer::RSEnumerateSubModules: return _S8("RSEnumerateSubModules");
		case RootServer::RSEnumerateBindings: return _S8("RSEnumerateBindings");
		case RootServer::RSCancelLoadModule: return _S8("RSCancelLoadModule");
		case RootServer::RSCancelUnloadModule: return _S8("RSCancelUnloadModule");
		case RootServer::RSCancelBind: return _S8("RSCancelBind");
		case RootServer::RSCancelUnbind: return _S8("RSCancelUnbind");
		case RootServer::RSCloseSession: return _S8("RSCloseSession");
		case RootServer::RSShutdown: return _S8("RSShutdown");
		case RootServer::RSSendMessage: return _S8("RSSendMessage");
		case RootServer::RSDbgMarkHeap: return _S8("RSDbgMarkHeap");
		case RootServer::RSDbgCheckHeap: return _S8("RSDbgCheckHeap");
		case RootServer::RSDbgMarkEnd: return _S8("RSDbgMarkEnd");
		case RootServer::RSDbgFailNext: return _S8("RSDbgFailNext");

		default:
			break;
//			__DEBUGGER();

			/* Don't panic, do read on - you've hit this breakpoint because
			aMess has no helpful explananatory text for logging. Almost certainly
			this means an IPC function was added without updating this switch. Do
			it now and make the world very slightly better!
			*/
		}
	return _S8("Unknown"); 	// unknown
	}

void Logging::IPCMessName(TInt aMessNum, TDes8& aMessBuf)
	{
	const TText8* mess = IPCMessName(aMessNum);
	if (mess == NULL)
		{
		_LIT8(KUnknown, "IPC %d (UNKNOWN NAME - edit IPCMessName())");
		aMessBuf.Format(KUnknown(), aMessNum);
		}
	else
		{
		aMessBuf.Copy(mess);
		}
	}


