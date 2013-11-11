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
#include <ss_glob.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/nifif.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/sockmes.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_mcprnodemessages.h>

#if defined (__CFLOG_ACTIVE) || defined (SYMBIAN_TRACE_ENABLE)


using namespace ESock;

EXPORT_C void ESockLogExternal::Printf(const TDesC8& aMajorTag, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(aMajorTag, aSubTag, aFmt, list);
	VA_END(list);
	}

EXPORT_C void ESockLogExternal::Printf(const TDesC8& aMajorTag, const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	TBuf8<200> buf;
	CSockManData* globs = SockManGlobals::Get();
	if(globs)
		{
		CWorkerThread* worker = globs->SelfWorker();
		if(worker)
			{
			_LIT8(KWorkerIdFormat, "W%d: ");
			buf.Format(KWorkerIdFormat, worker->WorkerId());
			}
		}
	TLogIgnoreOverflow8 overflowHandler;
	buf.AppendFormatList(aFmt, aList, &overflowHandler);
	
#ifdef SYMBIAN_TRACE_ENABLE
    const TInt KSimpleLoggingPrimaryFilter = 196;
	UTracePfAny(KSimpleLoggingPrimaryFilter, KText, ETrue, EFalse, 0, buf.Ptr(), buf.Length());
#endif

#ifdef __CFLOG_ACTIVE
	CCFLogIf::WriteFormat(aMajorTag, aSubTag, buf);
#else
	// Preventing unused variable warnings.
	(void)aMajorTag;
	(void)aSubTag;
#endif	
	}


void ESockLog::Printf(TRefByValue<const TDesC8> aFmt,...)
/**
Write a multiple argument list to the log using the generc server tag, trapping and ignoring any leave

*/
	{
	VA_LIST list;
	VA_START(list,aFmt);
	ESockLogExternal::Printf(KESockComponentTag, KESockServerTag, aFmt, list);
	VA_END(list);
	}

void ESockLog::Printf(TRefByValue<const TDesC> aFmt,...)
/**
Write a multiple argument list to the log, trapping and ignoring any leave

*/
	{
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(KESockComponentTag, KESockServerTag, aFmt, list);
	VA_END(list);
	}

void ESockLog::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	ESockLogExternal::Printf(KESockComponentTag, aSubTag, aFmt, list);
	VA_END(list);
	}

void ESockLog::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(KESockComponentTag, aSubTag, aFmt, list);
	VA_END(list);
	}
	
void ESockLog::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	Printf(KESockComponentTag, aSubTag, aFmt, aList);
	}


void ESockLog::Printf(const TDesC8& aMajorTag, const TDesC8& aSubTag, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
/**
Write a multiple argument list to the log, trapping and ignoring any leave

*/
	{
	TBuf<200> buf;
	CSockManData* globs = SockManGlobals::Get();
	if(globs)
		{
		CWorkerThread* worker = globs->SelfWorker();
		if(worker)
			{
			_LIT(KWorkerIdFormat, "W%d: ");
			buf.Format(KWorkerIdFormat, worker->WorkerId());
			}
		}
	TLogIgnoreOverflow16 overflowHandler;
	buf.AppendFormatList(aFmt, aList, &overflowHandler);
	
#ifdef SYMBIAN_TRACE_ENABLE
    const TInt KSimpleLoggingPrimaryFilter = 196;
    TBuf8<200> buf8;
    buf8.Copy(buf);
	UTracePfAny(KSimpleLoggingPrimaryFilter, KText, ETrue, EFalse, 0, buf8.Ptr(), buf8.Length());
#endif

#ifdef __CFLOG_ACTIVE	
	CCFLogIf::Write(aMajorTag, aSubTag, buf);
#else
	// Preventing unused variable warnings.
	(void)aMajorTag;
	(void)aSubTag;
#endif	
	}
	
void ESockLog::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	ESockLogExternal::Printf(KESockComponentTag, aSubTag, aFmt, aList);
	}

void ESockLog::ConnectionInfoPrintf(const TDesC8& aConnectionInfo, TRefByValue<const TDesC> aFmt,...)
/**
Write a multiple argument list to the log, trapping and ignoring any leave
*/
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TBuf<200> buf;
	buf.AppendFormatList(aFmt, list);
	const TSoIfConnectionInfo *info = REINTERPRET_CAST(const TSoIfConnectionInfo*, aConnectionInfo.Ptr());
	buf.AppendFormat(_L(" IAP %d, Network %d"), info->iIAPId, info->iNetworkId);
	
#ifdef SYMBIAN_TRACE_ENABLE
    const TInt KSimpleLoggingPrimaryFilter = 196;
    TBuf8<200> buf8;
    buf8.Copy(buf);
	UTracePfAny(KSimpleLoggingPrimaryFilter, KText, ETrue, EFalse, 0, buf8.Ptr(), buf8.Length());
#endif

#ifdef __CFLOG_ACTIVE
	CCFLogIf::Write(KESockComponentTag, KESockConnectionTag, buf);
#endif
	
	VA_END(list);
	}


const TText8* ESockLog::IPCMessName(TSockMess aMess)
/**
Write a multiple argument list to the log, trapping and ignoring any leave
*/
	{
	switch(aMess)
		{
		case ESSNumProtocols: return _S8("ESSNumProtocols");
		case ESSProtocolInfo: return _S8("ESSProtocolInfo");
		case ESSProtocolInfoByName: return _S8("ESSProtocolInfoByName");
		case ESSProtocolStart: return _S8("ESSProtocolStart");
		case ESSProtocolStop: return _S8("ESSProtocolStop");
		case ESSInstallExtension: return _S8("ESSInstallExtension");
		case ESoCreate: return _S8("ESoCreate");
		case ESoCreateNull: return _S8("ESoCreateNull");
		case ESoSend: return _S8("ESoSend");
		case ESoSendNoLength: return _S8("ESoSendNoLength");
		case ESoRecv: return _S8("ESoRecv");
		case ESoRecvNoLength: return _S8("ESoRecvNoLength");
		case ESoRecvOneOrMore: return _S8("ESoRecvOneOrMore");
        case ESoRecvOneOrMoreNoLength: return _S8("ESoRecvOneOrMoreNoLength");
		case ESoRead: return _S8("ESoRead");
		case ESoWrite: return _S8("ESoWrite");
		case ESoSendTo: return _S8("ESoSendTo");
		case ESoSendToNoLength: return _S8("ESoSendToNoLength");
		case ESoRecvFrom: return _S8("ESoRecvFrom");
		case ESoRecvFromNoLength: return _S8("ESoRecvFromNoLength");
		case ESoConnect: return _S8("ESoConnect");
		case ESoBind: return _S8("ESoBind");
		case ESoAccept: return _S8("ESoAccept");
		case ESoListen: return _S8("ESoListen");
		case ESoSetOpt: return _S8("ESoSetOpt");
		case ESoGetOpt: return _S8("ESoGetOpt");
		case ESoIoctl: return _S8("ESoIoctl");
		case ESoGetDiscData: return _S8("ESoGetDiscData");
		case ESoGetLocalName: return _S8("ESoGetLocalName");
		case ESoGetRemoteName: return _S8("ESoGetRemoteName");
		case ESoClose: return _S8("ESoClose");
		case ESoShutdown: return _S8("ESoShutdown");
		case ESoCancelIoctl: return _S8("ESoCancelIoctl");
		case ESoCancelRecv: return _S8("ESoCancelRecv");
		case ESoCancelSend: return _S8("ESoCancelSend");
		case ESoCancelConnect: return _S8("ESoCancelConnect");
		case ESoCancelAccept: return _S8("ESoCancelAccept");
		case ESoCancelAll: return _S8("ESoCancelAll");
		case ESoSocketInfo: return _S8("ESoSocketInfo");
		case ESoReference: return _S8("ESoReference");
		case ESoTransfer: return _S8("ESoTransfer");
		case EHRCreate: return _S8("EHRCreate");
		case EHRGetByName: return _S8("EHRGetByName");
		case EHRNext: return _S8("EHRNext");
		case EHRGetByAddress: return _S8("EHRGetByAddress");
		case EHRGetHostName: return _S8("EHRGetHostName");
		case EHRSetHostName: return _S8("EHRSetHostName");
		case EHRCancel: return _S8("EHRCancel");
		case EHRClose: return _S8("EHRClose");
		case ESRCreate: return _S8("ESRCreate");
		case ESRGetByName: return _S8("ESRGetByName");
		case ESRGetByNumber: return _S8("ESRGetByNumber");
		case ESRRegisterService: return _S8("ESRRegisterService");
		case ESRRemoveService: return _S8("ESRRemoveService");
		case ESRCancel: return _S8("ESRCancel");
		case ESRClose: return _S8("ESRClose");
		case ENDCreate: return _S8("ENDCreate");
		case ENDQuery: return _S8("ENDQuery");
		case ENDAdd: return _S8("ENDAdd");
		case ENDRemove: return _S8("ENDRemove");
		case ENDCancel: return _S8("ENDCancel");
		case ENDClose: return _S8("ENDClose");
		case ESoCreateWithConnection: return _S8("ESoCreateWithConnection");
		case EHRCreateWithConnection: return _S8("EHRCreateWithConnection");
		case ECNCreate: return _S8("ECNCreate");
		case ECNCreateWithName: return _S8("ECNCreateWithName");
		case ECNClose: return _S8("ECNClose");
		case ECNReference: return _S8("ECNReference");
		case ECNStart: return _S8("ECNStart");
		case ECNSetStartPrefs: return _S8("ECNSetStartPrefs");
		case ECNStop: return _S8("ECNStop");
		case ECNProgress: return _S8("ECNProgress");
		case ECNProgressNotification: return _S8("ECNProgressNotification");
		case ECNCancelProgressNotification: return _S8("ECNCancelProgressNotification");
		case ECNLastProgressError: return _S8("ECNLastProgressError");
		case ECNServiceChangeNotification: return _S8("ECNServiceChangeNotification");
		case ECNCancelServiceChangeNotification: return _S8("ECNCancelServiceChangeNotification");
		case ECNGetIntSetting: return _S8("ECNGetIntSetting");
		case ECNGetBoolSetting: return _S8("ECNGetBoolSetting");
		case ECNGetDes8Setting: return _S8("ECNGetDes8Setting");
		case ECNGetDes16Setting: return _S8("ECNGetDes16Setting");
		case ECNGetLongDesSetting: return _S8("ECNGetLongDesSetting");
		case ECNEnumerateConnections: return _S8("ECNEnumerateConnections");
		case ECNGetConnectionInfo: return _S8("ECNGetConnectionInfo");
		case ECNControl: return _S8("ECNControl");
		case ECNAttach: return _S8("ECNAttach");
		case ECNAllInterfaceNotification: return _S8("ECNAllInterfaceNotification");
		case ECNCancelAllInterfaceNotification: return _S8("ECNCancelAllInterfaceNotification");
		case ESCPSStop: return _S8("ESCPSStop");
		case ESCPSProgress: return _S8("ESCPSProgress");
		case ESCPSProgressNotification: return _S8("ESCPSProgressNotification");
		case ESCPSCancelProgressNotification: return _S8("ESCPSCancelProgressNotification");
		case ECNEnumerateSubConnections: return _S8("ECNEnumerateSubConnections");
		case ESCPSGetSubConnectionInfo: return _S8("ESCPSGetSubConnectionInfo");
		case ECNAllSubConnectionNotification: return _S8("ECNAllSubConnectionNotification");
		case ECNCancelAllSubConnectionNotification: return _S8("ECNCancelAllSubConnectionNotification");
		case ESCPSDataTransferred: return _S8("ESCPSDataTransferred");
		case ESCPSDataTransferredCancel: return _S8("ESCPSDataTransferredCancel");
		case ESCPSDataSentNotificationRequest: return _S8("ESCPSDataSentNotificationRequest");
		case ESCPSDataSentNotificationCancel: return _S8("ESCPSDataSentNotificationCancel");
		case ESCPSDataReceivedNotificationRequest: return _S8("ESCPSDataReceivedNotificationRequest");
		case ESCPSDataReceivedNotificationCancel: return _S8("ESCPSDataReceivedNotificationCancel");
		case ESCPSIsSubConnectionActiveRequest: return _S8("ESCPSIsSubConnectionActiveRequest");
		case ESCPSIsSubConnectionActiveCancel: return _S8("ESCPSIsSubConnectionActiveCancel");
		case ESCCreate: return _S8("ESCCreate");
		case ESCProgressNotification: return _S8("ESCProgressNotification");
		case ESCCancelProgressNotification: return _S8("ESCCancelProgressNotification");
		case ESCProgress: return _S8("ESCProgress");
		case ESCIsSubConnectionActiveRequest: return _S8("ESCIsSubConnectionActiveRequest");
		case ESCIsSubConnectionActiveCancel: return _S8("ESCIsSubConnectionActiveCancel");
		case ESCClose: return _S8("ESCClose");
		case ESCAddSocket: return _S8("ESCAddSocket");
		case ESCRemoveSocket: return _S8("ESCRemoveSocket");
		case ESCSetParameters: return _S8("ESCSetParameters");
		case ESCGetParameters: return _S8("ESCGetParameters");
		case ESCGetParametersLength: return _S8("ESCGetParametersLength");
		case ESCEventNotification: return _S8("ESCEventNotification");
		case ESCEventNotificationCancel: return _S8("ESCEventNotificationCancel");
		case ESCControl: return _S8("ESCControl");
		case ESoCreateWithSubConnection: return _S8("ESoCreateWithSubConnection");
		case ESSDbgMarkHeap: return _S8("ESSDbgMarkHeap");
		case ESSDbgCheckHeap: return _S8("ESSDbgCheckHeap");
		case ESSDbgMarkEnd: return _S8("ESSDbgMarkEnd");
		case ESSDbgFailNext: return _S8("ESSDbgFailNext");
		case ESSDbgCheckFailNext: return _S8("ESSDbgCheckFailNext");
		case ESSDbgFailNextMbuf: return _S8("ESSDbgFailNextMbuf");
		case ESSDbgSetMbufPoolLimit: return _S8("ESSDbgSetMbufPoolLimit");
		case ESSDbgCheckMbuf: return _S8("ESSDbgCheckMbuf");
		case ESSDbgMbufFreeSpace: return _S8("ESSDbgMbufFreeSpace");
		case ESSDbgMbufTotalSpace: return _S8("ESSDbgMbufTotalSpace");

		case ESSDbgControl: return _S8("ESSDbgControl");

		case ESSExclusiveMode: return _S8("ESSExclusiveMode");
		case ESSClearExclusiveMode: return _S8("ESSClearExclusiveMode");
		case EHrQuery: return _S8("EHrQuery");
		case EHrQueryNext: return _S8("EHrQueryNext");
		case ECNIoctl: return _S8("ECNIoctl");
		case ECNCancelIoctl: return _S8("ECNCancelIoctl");
		case ESSRequestOptimalDealer: return _S8("ESSRequestOptimalDealer");
		case ECommsApiExtBindIface: return _S8("ECommsApiExtBindIface");
		case ECommsApiExtIfaceSendReceive: return _S8("ECommsApiExtIfaceSendReceive");
		case ECommsApiExtIfaceSend: return _S8("ECommsApiExtIfaceSend");
		case ECommsApiExtIfaceClose: return _S8("ECommsApiExtIfaceClose");
		case ECNWaitForIncoming: return _S8("ECNWaitForIncoming");
		case ECNCancelWaitForIncoming: return _S8("ECNCancelWaitForIncoming");
		case ESCStart: return _S8("ESCStart");
		case ESCStop: return _S8("ESCStop");
		case ESCEventAllNotifications: return _S8("ESCEventAllNotifications");
		case ESCEventNotificationSetup: return _S8("ESCEventNotificationSetup");
		case ECNGetOrSetParameters: return _S8("ECNGetOrSetParameters");
		case ECNGetParametersResponseLength: return _S8("ECNGetParametersResponseLength");
		case ECNGetParametersResponse: return _S8("ECNGetParametersResponse");
		case EHRSetOpt: return _S8("EHRSetOpt");

		default:
			__DEBUGGER();	/* Don't panic, do read on - you've hit this breakpoint because 
							aMess has no helpful explananatory text for logging. Almost certainly 
							this means an IPC function was added without updating this switch. Do
							it now and make the world very slightly better! 
							*/
		}
	return NULL; 	// unknown
	}

void ESockLog::IPCMessName(TSockMess aMessNum, TDes8& aMessBuf)
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


const TText8* ESockLog::ConnServIPCMessName(TConnectionServerMessage aMess)
/**
Write a multiple argument list to the log, trapping and ignoring any leave
*/
	{
	switch(aMess)
		{
		case ECMCreate: return _S8("ECMCreate");
		case ECMClose: return _S8("ECMClose");
		case ECMAttachToTierManager: return _S8("ECMAttachToTierManager");
		case ECMAccessPointStatusQuery_DoThenGetResultOrSize: return _S8("ECMAccessPointStatusQuery_DoThenGetResultOrSize");
		case ECMAccessPointStatusQuery_GetResult: return _S8("ECMAccessPointStatusQuery_GetResult");
		case ECMAccessPointStatusQuery_Cancel: return _S8("ECMAccessPointStatusQuery_Cancel");
		case ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize: return _S8("ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize");
		case ECMAccessPointNotification_AwaitThenGetResultOrSize: return _S8("ECMAccessPointNotification_AwaitThenGetResultOrSize");
		case ECMAccessPointNotification_GetResult: return _S8("ECMAccessPointNotification_GetResult");
		case ECMAccessPointNotification_Cancel: return _S8("ECMAccessPointNotification_Cancel");
		case ECMApiExtBindIface: return _S8("ECommsApiExtBindIface");
		case ECMApiExtIfaceSendReceive: return _S8("ECommsApiExtIfaceSendReceive");
		case ECMApiExtIfaceSend: return _S8("ECommsApiExtIfaceSend");
		case ECMApiExtIfaceClose: return _S8("ECommsApiExtIfaceClose");
		default:
			__DEBUGGER();	/* Don't panic, do read on - you've hit this breakpoint because 
							aMess has no helpful explananatory text for logging. Almost certainly 
							this means an IPC function was added without updating this switch. Do
							it now and make the world very slightly better! 
							*/
		}
	return NULL; 	// unknown
	}

void ESockLog::ConnServIPCMessName(TConnectionServerMessage aMessNum, TDes8& aMessBuf)
	{
	const TText8* mess = ConnServIPCMessName(aMessNum);
	if (mess == NULL)
		{
		_LIT8(KUnknown, "IPC %d (UNKNOWN NAME - edit ConnServIPCMessName())");
		aMessBuf.Format(KUnknown(), aMessNum);
		}
	else
		{
		aMessBuf.Copy(mess);
		}
	}

    
#else // defined(__FLOG_ACTIVE) || defined (SYMBIAN_TRACE_ENABLE)

EXPORT_C void ESockLogExternal::Printf(const TDesC8& /*aMajorTag*/, const TDesC8& /*aSubTag*/, TRefByValue<const TDesC8> /*aFmt*/, ...)
	{
	}

EXPORT_C void ESockLogExternal::Printf(const TDesC8& /*aMajorTag*/, const TDesC8& /*aSubTag*/, TRefByValue<const TDesC8> /*aFmt*/, VA_LIST& /*aList*/)
	{
	}
#endif


EXPORT_C TInt ESockLogExternal::WorkerId()
    {
    CSockManData* globs = SockManGlobals::Get();
	if (globs)
		{
		CWorkerThread* worker = globs->SelfWorker();
		if (worker)
		    {
		    return (TInt)worker->WorkerId();
			}
		}
    return KErrNotFound;
    }

