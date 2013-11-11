// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "C32LOG.H"
#include "cs_glob.h"

#if (defined __FLOG_ACTIVE)

#include <cflog.h>

template <class T> void DoWrite(TBool aStatic, const TDesC8& aSubTag, T& aBuf);

EXPORT_C void TC32Log::Printf(TBool aStatic, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(aStatic, aSubTag, aFmt, list);
	VA_END(list);
	}


void TC32Log::Printf(TBool aStatic, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	TBuf8<KLogBufferSize> buf;
	CTLSRedirector* tls = CRedirectorInTls();
	if ((tls) && tls->IsC32GlobalDataSet())
		{
		CC32WorkerThread* worker = tls->C32GlobalData()->SelfWorker();
		if(worker)
			{
			_LIT8(KWorkerIdFormat, "W%d: ");
			buf.Format(KWorkerIdFormat, worker->WorkerId());
			}
		}
		
	TLogIgnoreOverflow8 overflowHandler;
	buf.AppendFormatList(aFmt, aList, &overflowHandler);
	DoWrite<TDes8>(aStatic, aSubTag, buf);
	}




EXPORT_C void TC32Log::Printf(TBool aStatic, const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(aStatic, aSubTag, aFmt, list);
	VA_END(list);
	}




void TC32Log::Printf(TBool aStatic, const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
/**
Write a mulitple argument list to the log, trapping and ignoring any leave

*/
	{
	TBuf<KLogBufferSize> buf;
	CTLSRedirector* tls = CRedirectorInTls();
	if ((tls) && tls->IsC32GlobalDataSet())
		{
		CC32WorkerThread* worker = tls->C32GlobalData()->SelfWorker();
		if(worker)
			{
			_LIT(KWorkerIdFormat, "W%d: ");
			buf.Format(KWorkerIdFormat, worker->WorkerId());
			}
		}
		
	TLogIgnoreOverflow16 overflowHandler;
	buf.AppendFormatList(aFmt, aList, &overflowHandler);
	DoWrite<TDes>(aStatic, aSubTag, buf);
	}

template <class T> void DoWrite(TBool aStatic, const TDesC8& aSubTag, T& aBuf)
	{
	if(!aBuf.Length())
		return;
	
	if(aStatic)
		{
		__FLOG_STATIC_VA ((KC32SubSystem, aSubTag, aBuf));
		}
	else
		{
		CCFLogIf::Write(KC32SubSystem, aSubTag, aBuf);
		}
	}


_LIT8(KLitCommAccessExclusive,   "ECommExclusive");
_LIT8(KLitCommAccessShared,      "ECommShared");
_LIT8(KLitCommAccessPreemptable, "ECommPreemptable");
_LIT8(KLitCommAccessUnsupported, "**Unsupported**");



/**
Returns a string for a comm access mode

@param aMode Comm Access Mode
@return String name representation of aMode
*/
const TDesC8& TC32Log::CommAccessStr(const TCommAccess& aMode)
	{
	switch(aMode)
		{
	case ECommExclusive:
		return KLitCommAccessExclusive;
	case ECommShared:
		return KLitCommAccessShared;
	case ECommPreemptable:
		return KLitCommAccessPreemptable;
	default:
		return KLitCommAccessUnsupported;
		}
	}

_LIT(KLitCommAccessExclusive16,   "ECommExclusive");
_LIT(KLitCommAccessShared16,      "ECommShared");
_LIT(KLitCommAccessPreemptable16, "ECommPreemptable");
_LIT(KLitCommAccessUnsupported16, "**Unsupported**");

	
const TDesC& TC32Log::CommAccessStr16(const TCommAccess& aMode)
// in some cases it is nice to be able to have the mode with the name of the module/port.
// However, to get it onto one log line without fuss the whole log string needs to be 16-bits
// since the module/port is a TDesC, so instead we provide a 16-bit version of this function.
	{
	switch(aMode)
		{
	case ECommExclusive:
		return KLitCommAccessExclusive16;
	case ECommShared:
		return KLitCommAccessShared16;
	case ECommPreemptable:
		return KLitCommAccessPreemptable16;
	default:
		return KLitCommAccessUnsupported16;
		}
	}
	
	
_LIT8(KLitIntCommExclusive,			"EIntCommExclusive");
_LIT8(KLitIntCommShared,			"EIntCommShared");
_LIT8(KLitIntCommPreemptable,		"EIntCommPreemptable");
_LIT8(KLitIntCommWaitUntilAvailable,"EIntCommWaitUntilAvailable");
_LIT8(KLitIntCommUnsupported,		"**Unsupported**");



/**
Returns a string for a internal comm access mode

@param aMode Internal Comm Access Mode
@return readable string for aMode
*/
const TDesC8& TC32Log::InternalCommAccessStr(TInternalCommAccess aMode)
	{
	switch(aMode)
		{
		case EIntCommExclusive:
			return KLitIntCommExclusive;
		case EIntCommShared:
			return KLitIntCommShared;
		case EIntCommPreemptable:
			return KLitIntCommPreemptable;
		case EIntCommWaitUntilAvailable:
			return KLitIntCommWaitUntilAvailable;
		default:
			return KLitIntCommUnsupported;
		}
	}



_LIT8(KLitCommRoleDTE,			"ECommRoleDTE");
_LIT8(KLitCommRoleDCE,			"ECommRoleDCE");
_LIT8(KLitCommRoleUnsupported,	"**Unsupported**");


/**
Returns a string for a comm role

@param aRole Comm Role
@return readable string for aRole
*/
const TDesC8& TC32Log::CommRoleStr(TCommRole aRole)
	{
	switch(aRole)
		{
		case ECommRoleDTE:
			return KLitCommRoleDTE;
		case ECommRoleDCE:
			return KLitCommRoleDCE;
		default:
			return KLitCommRoleUnsupported;
		}
	}


_LIT(KLitCommRoleDTE16,			"ECommRoleDTE");
_LIT(KLitCommRoleDCE16,			"ECommRoleDCE");
_LIT(KLitCommRoleUnsupported16,	"**Unsupported**");


/**
Returns a string for a comm role

@param aRole Comm Role
@return readable string for aRole
*/
const TDesC& TC32Log::CommRoleStr16(TCommRole aRole)
	{
	switch(aRole)
		{
		case ECommRoleDTE:
			return KLitCommRoleDTE16;
		case ECommRoleDCE:
			return KLitCommRoleDCE16;
		default:
			return KLitCommRoleUnsupported16;
		}
	}


_LIT8(KLitCommLoadCommModule,"ECommLoadCommModule");
_LIT8(KLitCommCloseCommModule,"ECommCloseCommModule");
_LIT8(KLitCommPortInfo, "ECommPortInfo");
_LIT8(KLitCommPortInfoByName,"ECommPortInfoByName");
_LIT8(KLitCommPortInfoByNumber,"ECommPortInfoByNumber");
_LIT8(KLitCommNumPorts,"ECommNumPorts");

_LIT8(KLitCommStartServerThread,"ECommStartServerThread");
_LIT8(KLitCommOpen,"ECommOpen");
_LIT8(KLitCommRead,"ECommRead");
_LIT8(KLitCommReadCancel,"ECommReadCancel");
_LIT8(KLitCommQueryReceiveBuffer,"ECommQueryReceiveBuffer");

_LIT8(KLitCommResetBuffers,"ECommResetBuffers");
_LIT8(KLitCommWrite,"ECommWrite");
_LIT8(KLitCommWriteCancel,"ECommWriteCancel");
_LIT8(KLitCommBreak,"ECommBreak");
_LIT8(KLitCommBreakCancel,"ECommBreakCancel");
_LIT8(KLitCommCancel,"ECommCancel");
_LIT8(KLitCommConfig,"ECommConfig");
_LIT8(KLitCommSetConfig,"ECommSetConfig");
_LIT8(KLitCommCaps,"ECommCaps");
_LIT8(KLitCommSetMode,"ECommSetMode");

_LIT8(KLitCommGetMode,"ECommGetMode");
_LIT8(KLitCommSignals,"ECommSignals");
_LIT8(KLitCommSetSignalsToMark,"ECommSetSignalsToMark");
_LIT8(KLitCommSetSignalsToSpace,"ECommSetSignalsToSpace");
_LIT8(KLitCommReceiveBufferLength,"ECommReceiveBufferLength");
_LIT8(KLitCommSetReceiveBufferLength,"ECommSetReceiveBufferLength");
_LIT8(KLitCommClose,"ECommClose");
_LIT8(KLitCommDbgMarkHeap,"ECommDbgMarkHeap");
_LIT8(KLitCommDbgCheckHeap,"ECommDbgCheckHeap");
_LIT8(KLitCommDbgMarkEnd,"ECommDbgMarkEnd");

_LIT8(KLitCommDbgFailNext,"ECommDbgFailNext");
_LIT8(KLitCommDbgSetDebugPrintMask,"ECommDbgSetDebugPrintMask");
_LIT8(KLitCommDbgDoDumpDebugInfo,"ECommDbgDoDumpDebugInfo");

_LIT8(KLitCommGetRole,"ECommGetRole");
_LIT8(KLitCommNotifySignals,"ECommNotifySignals");
_LIT8(KLitCommNotifySignalsCancel,"ECommNotifySignalsCancel");
_LIT8(KLitCommNotifyFlowControl,"ECommNotifyFlowControl");
_LIT8(KLitCommNotifyFlowControlCancel,"ECommNotifyFlowControlCancel");
_LIT8(KLitCommGetFlowControl,"ECommGetFlowControl");
_LIT8(KLitCommNotifyConfigChange,"ECommNotifyConfigChange");

_LIT8(KLitCommNotifyConfigChangeCancel,"ECommNotifyConfigChangeCancel");
_LIT8(KLitCommNotifyBreak,"ECommNotifyBreak");
_LIT8(KLitCommNotifyBreakCancel,"ECommNotifyBreakCancel");
_LIT8(KLitCommNotifyDataAvailable,"ECommNotifyDataAvailable");
_LIT8(KLitCommNotifyDataAvailableCancel,"ECommNotifyDataAvailableCancel");
_LIT8(KLitCommNotifyOutputEmpty,"ECommNotifyOutputEmpty");
_LIT8(KLitCommNotifyOutputEmptyCancel,"ECommNotifyOutputEmptyCancel");

_LIT8(KLitCommSetAccess,"ECommSetAccess");
_LIT8(KLitCommDebugState,"ECommDebugState");
_LIT8(KLitCommOpenWhenAvailable,"ECommOpenWhenAvailable");

_LIT8(KLitCommReqUnsupported,		"**Unsupported**");

/**
Returns a string for C32 IPC Requests.

@param aIPC IPC code
@return readable string for IPC request name
*/
const TDesC8& TC32Log::C32RequestStr(TInt aIPC)
	{
	switch(aIPC)
		{
		case ECommLoadCommModule:
			return KLitCommLoadCommModule;
		case ECommCloseCommModule:
			return KLitCommCloseCommModule;
		case ECommPortInfo:
			return KLitCommPortInfo;
		case ECommPortInfoByName:
			return KLitCommPortInfoByName;
		case ECommPortInfoByNumber:
			return KLitCommPortInfoByNumber;
		case ECommNumPorts:
			return KLitCommNumPorts;
		case ECommStartServerThread:
			return KLitCommStartServerThread;
		case ECommOpen:
			return KLitCommOpen;
		case ECommRead:
			return KLitCommRead;
		case ECommReadCancel:
			return KLitCommReadCancel;
		case ECommQueryReceiveBuffer:
			return KLitCommQueryReceiveBuffer;
		case ECommResetBuffers:
			return KLitCommResetBuffers;
		case ECommWrite:
			return KLitCommWrite;
		case ECommWriteCancel:
			return KLitCommWriteCancel;
		case ECommBreak:
			return KLitCommBreak;
		case ECommBreakCancel:
			return KLitCommBreakCancel;
		case ECommCancel:
			return KLitCommCancel;
		case ECommConfig:
			return KLitCommConfig;
		case ECommSetConfig:
			return KLitCommSetConfig;
		case ECommCaps:
			return KLitCommCaps;
		case ECommSetMode:
			return KLitCommSetMode;
		case ECommGetMode:
			return KLitCommGetMode;
		case ECommSignals:
			return KLitCommSignals;
		case ECommSetSignalsToMark:
			return KLitCommSetSignalsToMark;
		case ECommSetSignalsToSpace:
			return KLitCommSetSignalsToSpace;
		case ECommReceiveBufferLength:
			return KLitCommReceiveBufferLength;
		case ECommSetReceiveBufferLength:
			return KLitCommSetReceiveBufferLength;
		case ECommClose:
			return KLitCommClose;
		case ECommDbgMarkHeap:
			return KLitCommDbgMarkHeap;
		case ECommDbgCheckHeap:
			return KLitCommDbgCheckHeap;
		case ECommDbgMarkEnd:
			return KLitCommDbgMarkEnd;
		case ECommDbgFailNext:
			return KLitCommDbgFailNext;
		case ECommDbgSetDebugPrintMask:
			return KLitCommDbgSetDebugPrintMask;
		case ECommDbgDoDumpDebugInfo:
			return KLitCommDbgDoDumpDebugInfo;
		case ECommGetRole:
			return KLitCommGetRole;
		case ECommNotifySignals:
			return KLitCommNotifySignals;
		case ECommNotifySignalsCancel:
			return KLitCommNotifySignalsCancel;
		case ECommNotifyFlowControl:
			return KLitCommNotifyFlowControl;
		case ECommNotifyFlowControlCancel:
			return KLitCommNotifyFlowControlCancel;
		case ECommGetFlowControl:
			return KLitCommGetFlowControl;
		case ECommNotifyConfigChange:
			return KLitCommNotifyConfigChange;
		case ECommNotifyConfigChangeCancel:
			return KLitCommNotifyConfigChangeCancel;
		case ECommNotifyBreak:
			return KLitCommNotifyBreak;
		case ECommNotifyBreakCancel:
			return KLitCommNotifyBreakCancel;
		case ECommNotifyDataAvailable:
			return KLitCommNotifyDataAvailable;
		case ECommNotifyDataAvailableCancel:
			return KLitCommNotifyDataAvailableCancel;
		case ECommNotifyOutputEmpty:
			return KLitCommNotifyOutputEmpty;
		case ECommNotifyOutputEmptyCancel:
			return KLitCommNotifyOutputEmptyCancel;
		case ECommSetAccess:
			return KLitCommSetAccess;
		case ECommDebugState:
			return KLitCommDebugState;
		case ECommOpenWhenAvailable:
			return KLitCommOpenWhenAvailable;
		default:
			return KLitCommReqUnsupported;
		}
	}
#else
EXPORT_C void TC32Log::Printf(TBool /* aStatic */, const TDesC8& /* aSubTag */, TRefByValue<const TDesC> /* aFmt */, ...)
	{
	
	}
EXPORT_C void TC32Log::Printf(TBool /* aStatic */, const TDesC8& /* aSubTag */, TRefByValue<const TDesC8> /* aFmt */, ...)
	{
	
	}
	
#endif // __FLOG_ACTIVE

