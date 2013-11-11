// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementattion for CSimSat Class
// This file contains the implementation of the Similator TSY Sat functionality.  
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimSatTraces.h"
#endif

#include "CSimSat.h" 
#include "CSimPhone.h"
#include "utils.h"
#include <satcs.h>
#include <e32math.h>

const TInt KSatGranularity=2;		// < Granularity for sat list array

CSimSat* CSimSat::NewL(CSimPhone* aPhone)
/**
* Standard two phase constructor
*
* @param aPhone Pointer to the phone object (CSimPhone)
* @return CSimSat pointer to the Sat object created.
* @leave Leaves if no memory or object is not created for any reason.
*/
	{
	CSimSat* simSat=new(ELeave) CSimSat(aPhone);
	CleanupStack::PushL(simSat);
	simSat->ConstructL();
	CleanupStack::Pop();
	return simSat;
	}

void CSimSat::Init()
	{}

CSimSat::CSimSat(CSimPhone* aPhone)
	: iPhone(aPhone), iSatIndex(0), iRefreshRequiredPending(EFalse),
		iMmsDownloadDataHandle(NULL),iEventDnldHandle(NULL)

/**
* Trivial Constructor.  Initialises all the data members
*
* @param aPhone pointer to the phone object that owns this class.
*/
	{
	iNotifyRefresh.iNotifyPending= EFalse;	
	}

void CSimSat::ConstructL()
/**
* Second phase of the 2-phase constructor.
* Constructs all the member data and retrieves all the data from the config file specific to this class.
*
* @leave Leaves no memory or any data member does not construct for any reason.
*/
	{
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONSTRUCTL_1, "CSimSat: Entered ConstructL()");
	
	iSatInfo	=new(ELeave) CArrayFixFlat<TSatInfo>(KSatGranularity);

	iTimer = CSimTimer::NewL(iPhone);

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONSTRUCTL_2, "Starting to parse Sat config parameters...");
	
	TInt count		=CfgFile()->ItemCount(KSatRefresh);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KSatRefresh,i);
		if(!item)
			break;

		TInt duration,type;
		TPtrC8 fileList;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KSatRefresh);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,type);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element TYPE returned %d (element no. %d) from tag %s.",ret,1,KSatRefresh);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,fileList);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONSTRUCTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element FILELIST returned %d (element no. %d) from tag %s.",ret,2,KSatRefresh);
			continue;
			}

		TSatInfo satInfo;
		satInfo.iDuration = duration;
		satInfo.iType = type;
		TInt location = fileList.Locate('\n');
		if(location > 0)
			satInfo.iFileList.Set(fileList.Left(location));
		else
			satInfo.iFileList.Set(fileList);
		iSatInfo->AppendL(satInfo);
		}
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONSTRUCTL_6, "...Finished parsing Sat config parameters...");
	
	if(iSatInfo->Count()!=0)
		{
		iSatIndex=0;
		iTimer->Start(iSatInfo->At(0).iDuration,this);
		}
	}
		
CSimSat::~CSimSat()
/**
* Trivial Destructor
* Closes all CObject type objects and destroys all other objects created in the ConstructL()
*
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_DTOR_1, "CSimSat: Entered destructor");
	delete iTimer;
	if(iSatInfo)
		{
		iSatInfo->Delete(0,iSatInfo->Count());
		delete iSatInfo;
		}
	if(iMMRetrieve)
		{
		iMMRetrieve->Delete(0,iMMRetrieve->Count());
		delete iMMRetrieve;
		}
	if(iMMSubmit)
		{
		iMMSubmit->Delete(0,iMMSubmit->Count());
		delete iMMSubmit;
		}
	if(iMMDisplay)
		{
		iMMDisplay->Delete(0,iMMDisplay->Count());
		delete iMMDisplay;
		}
	if(iSetFrms)
		{
		iSetFrms->Delete(0,iSetFrms->Count());
		delete iSetFrms;
		}
	if(iGetFrmsSts)
		{
		iGetFrmsSts->Delete(0,iGetFrmsSts->Count());
		delete iGetFrmsSts;
		}
	if(iMmsDownloadData)
		{
		iMmsDownloadData->Delete(0,iMmsDownloadData->Count());
		delete iMmsDownloadData;
		}
	if(iUssdData)
		{
		iUssdData->Delete(0,iUssdData->Count());
		delete iUssdData;
		}
	if(iLocalInfo)
		{
		iLocalInfo->Delete(0,iLocalInfo->Count());
		delete iLocalInfo;
		}
	if(iOpenChn)
		{
		iOpenChn->Delete(0,iOpenChn->Count());
		delete iOpenChn;
		}
	if(iMiscCmd)
		{
		iMiscCmd->Delete(0,iMiscCmd->Count());
		delete iMiscCmd;
		}
	if(iEventList)
		{
		iEventList->Delete(0,iEventList->Count());
		delete iEventList;
		}
	if(iSendUssd)
		{
		iSendUssd->Delete(0,iSendUssd->Count());
		delete iSendUssd;
		}
	if(iRefresh)
		{
		iRefresh->Delete(0,iRefresh->Count());
		delete iRefresh;
		}
	if(iElemFiles)
		{
		iElemFiles->Delete(0,iElemFiles->Count());
		delete iElemFiles;
		}
	if(iLnchBrwsr)
		{
		iLnchBrwsr->Delete(0,iLnchBrwsr->Count());
		delete iLnchBrwsr;
		}
	if(iSendSS)
		{
		iSendSS->Delete(0,iSendSS->Count());
		delete iSendSS;
		}
	}

TInt CSimSat::ConvertTextToFileList(const TPtrC8& aText,
 									RSat::TRefreshFileList& aFileList) const
/**
* Converts a text based file list (e.g. "6F3A6F3B") to a binary file list (e.g
* {0x6f3a, 0x6f3b}).
*
* @param aText Text string to convert.
* @param aFileList Returned file list.
* @return Error code KErrNone if request completes ok.  Otherwise a relevant error
*         code.
*/
	{
 	aFileList.Zero();

	//
	// Go through the string and take upto 4 characters at a time.  Convert each set
	// from hexidecimal to an integer and then append them to the file list.
	//
	TInt  position(0);

	while (position < aText.Length())
		{
		TInt  numberLength(4);

		if (aText.Length() - position < 4)
			{
			numberLength = aText.Length() - position;
			}

		TLex8  token(aText.Mid(position, numberLength));
		TUint  value;
		TInt  ret;

		ret = token.Val(value, EHex);
		if (ret != KErrNone)
			{
			return(ret);
			}

		aFileList.Append(value);
		position +=4;
		}

	return(KErrNone);
}

CTelObject* CSimSat::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
* Returns a pointer to an existing  object identified by name.
* If the object is deleted then reopen it then return a handle to it.
*
* @param aName name of the Sat object to be opened
* @return CTelObject pointer to the CSimSat object or null if not possible or not found
* @leave Leaves if object cannot be opened or created.
*/
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSimSat::OpenNewObjectL(TDes& /*aNewName*/)
/**
* Creates a new CSimSat object and returns a pointer to it.
*
* @param aName new name of the object created
* @return CTelObject pointer to the CSimSat object created
* @leave Leaves 
*/
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSimSat::ReqModeL(const TInt aIpc)
/**
* ReqModeL is called from the server's CTelObject::ReqAnalyserL
* in order to check the type of request it has.
* 
* 
* @param aIpc the ipc number that identifies the client request
* @return CTelObject::TReqMode The request mode to be used for this request
* @leave Leaves if not supported by this tsy
*/	
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
		case ESatNotifyRefreshPCmd:
		case ESatNotifyRefreshRequired:
		case ESatNotifyRefreshRequiredParam:
		case ESatNotifyRetrieveMultimediaMsgPCmd:
		case ESatNotifySubmitMultimediaMsgPCmd:
		case ESatNotifyDisplayMultimediaMsgPCmd:
		case ESatNotifySetFramesPCmd:
		case ESatNotifyGetFramesStatusPCmd:
		case ESatNotifyLocalInfoPCmd:
		case ESatNotifyOpenChannelPCmd:
		case ESatNotifyPlayTonePCmd:
		case ESatNotifySetUpCallPCmd:
		case ESatNotifySendUssdPCmd:
		case ESatNotifyGetInkeyPCmd:
		case ESatNotifySendSsPCmd:
		case ESatNotifyLaunchBrowserPCmd:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_REQMODEL_1, "CSimSat: ReqModeL");
			ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
			break;
		case ESatRefreshAllowed:
		case ESatTerminalRsp:
		case ESatGetMeSideSatProfile:
		case ESatEventDownload:
		case ESatClientSatProfileIndication:
		case ESatMmsTransferStatus:
		case ESatMmsNotificationDownload:
		case ESatUssdDataDownload:
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	return ret;
	}

TInt CSimSat::RegisterNotification(const TInt aIpc)
/**
* RegisterNotification is called when the server recognises that this notification
* is being posted for the first time on this sub-session object.
* 
* It enables the TSY to "turn on" any regular notification messages that it may receive 
* from the phone
*
* @param aIpc the ipc number that identifies the client request
* @return err KErrNone if fine
*/
	{
	switch (aIpc)
		{
		case ESatNotifyRefreshPCmd:
		case ESatNotifyRefreshRequired:
		case ESatNotifyRefreshRequiredParam:
		case ESatNotifyRetrieveMultimediaMsgPCmd:
		case ESatNotifySubmitMultimediaMsgPCmd:
		case ESatNotifyDisplayMultimediaMsgPCmd:
		case ESatNotifySetFramesPCmd:
		case ESatNotifyGetFramesStatusPCmd:
		case ESatGetMeSideSatProfile:
		case ESatClientSatProfileIndication:
		case ESatNotifyLocalInfoPCmd:
		case ESatNotifyOpenChannelPCmd:
		case ESatNotifyPlayTonePCmd: 
		case ESatNotifySetUpCallPCmd:
		case ESatEventDownload:
		case ESatNotifySendUssdPCmd:
		case ESatNotifyGetInkeyPCmd:
		case ESatNotifySendSsPCmd:
		case ESatNotifyLaunchBrowserPCmd:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_REGISTERNOTIFICATION_1, "CSimSat: RegisterNotification");
			return KErrNone;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_REGISTERNOTIFICATION_2, "CSimSat: Register error, unknown IPC");
			return KErrNotSupported;
		}
	}

TInt CSimSat::DeregisterNotification(const TInt aIpc)
/**
* DeregisterNotification is called when the server recognises that this notification
* will not be posted again because the last client to have a handle on this sub-session
* object has just closed the handle.
*
* It enables the TSY to "turn off" any regular notification messages that it may 
* receive from the phone
*
* @param aIpc the ipc number that identifies the client request
* @return err KErrNone if fine
*/
	{
	switch (aIpc)
		{
		case ESatNotifyRefreshPCmd:
		case ESatNotifyRefreshRequired:
		case ESatNotifyRefreshRequiredParam:
		case ESatNotifyRetrieveMultimediaMsgPCmd:
		case ESatNotifySubmitMultimediaMsgPCmd:
		case ESatNotifyDisplayMultimediaMsgPCmd:
		case ESatNotifySetFramesPCmd:
		case ESatNotifyGetFramesStatusPCmd:
		case ESatGetMeSideSatProfile:
		case ESatClientSatProfileIndication:
		case ESatNotifyLocalInfoPCmd:
		case ESatNotifyOpenChannelPCmd:
		case ESatNotifyPlayTonePCmd:
		case ESatNotifySetUpCallPCmd:
		case ESatEventDownload:
		case ESatNotifySendUssdPCmd:
		case ESatNotifyGetInkeyPCmd:
		case ESatNotifySendSsPCmd:
		case ESatNotifyLaunchBrowserPCmd:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_DEREGISTERNOTIFICATION_1, "CSimSat: DeregisterNotification");
			return KErrNone;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_DEREGISTERNOTIFICATION_2, "CSimSat: Deregister error, unknown IPC");
			return KErrNotSupported;
		}
	}

TInt CSimSat::NumberOfSlotsL(const TInt aIpc)
/**
* NumberOfSlotsL is called by the server when it is registering a new notification
* It enables the TSY to tell the server how many buffer slots to allocate for
* "repost immediately" notifications that may trigger before clients collect them
*
* @param aIpc the ipc number that identifies the client request
* @return err KErrNone if fine
*/
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
		case ESatNotifyRefreshPCmd:
		case ESatNotifyRefreshRequired:
		case ESatNotifyRefreshRequiredParam:
		case ESatNotifyRetrieveMultimediaMsgPCmd:
		case ESatNotifySubmitMultimediaMsgPCmd:
		case ESatNotifyDisplayMultimediaMsgPCmd:
		case ESatNotifySetFramesPCmd:
		case ESatNotifyGetFramesStatusPCmd:
		case ESatNotifyLocalInfoPCmd:
		case ESatNotifyOpenChannelPCmd:
		case ESatNotifyPlayTonePCmd:
		case ESatNotifySetUpCallPCmd:
		case ESatEventDownload:
		case ESatNotifySendUssdPCmd:
		case ESatNotifyGetInkeyPCmd:
		case ESatNotifySendSsPCmd:
		case ESatNotifyLaunchBrowserPCmd:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_NUMBEROFSLOTSL_1, "CSimSat: Registered with 2 slot");
			numberOfSlots=2;
			break;
		default:
			// Unknown or invalid IPC
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_NUMBEROFSLOTSL_2, "CSimSat: Number of Slots error, unknown IPC");
			User::Leave(KErrNotSupported);
			break;
		}  
	return numberOfSlots;
	}


TInt CSimSat::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
/**
* ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request
* for the TSY to process.
* A request handle, request type and request data are passed to the TSY
* 
* @param aTsyReqHandle  The request handle for completing the request 
* @param aIpc Ipc representing the request
* @param aPackage any data associated with the request
* @return err KErrNone if request completes ok
*/
	{
	//TAny* dataPtr=NULL;

	switch (aIpc)
		{
		case ESatNotifyRefreshPCmd:
			return NotifyRefreshPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyRefreshRequired:
			return NotifyRefreshRequired(aTsyReqHandle);
		case ESatNotifyRefreshRequiredParam:
			return NotifyRefreshRequired(aTsyReqHandle, aPackage.Des1n());
		case ESatRefreshAllowed:
			return RefreshAllowed(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyRetrieveMultimediaMsgPCmd:
			return NotifyRetrieveMultimediaMsgPCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatNotifySubmitMultimediaMsgPCmd:
			return NotifySubmitMultimediaMsgPCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatNotifyDisplayMultimediaMsgPCmd:
			return NotifyDisplayMultimediaMsgPCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatNotifySetFramesPCmd:
			return NotifySetFramesPCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatNotifyGetFramesStatusPCmd:
			return NotifyGetFramesStatusPCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatTerminalRsp:
			return TerminalRsp(aTsyReqHandle,
				REINTERPRET_CAST(RSat::TPCmd*,aPackage.Ptr1()),aPackage.Des2n());
		case ESatGetMeSideSatProfile:
			return GetMeSideSatProfile(aTsyReqHandle, aPackage.Des1n());
		case ESatClientSatProfileIndication:
			return ClientSatProfileIndication(aTsyReqHandle,aPackage.Des1n());
		case ESatMmsTransferStatus:
			return MmsTransferStatus(aTsyReqHandle, aPackage.Des1n());
		case ESatMmsNotificationDownload:
			return MmsNotificationDownload(aTsyReqHandle, aPackage.Des1n());
		case ESatUssdDataDownload:
			return UssdDataDownload(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyLocalInfoPCmd:
			return NotifyLocalInfoPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyOpenChannelPCmd:
			return NotifyOpenChannelPCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatNotifyPlayTonePCmd:
			return NotifyPlayTonePCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatNotifySetUpCallPCmd:
			return NotifySetupCallPCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatEventDownload:
			return EventDownload(aTsyReqHandle,
				REINTERPRET_CAST(RSat::TEventList*, aPackage.Ptr1()),aPackage.Des2n());
		case ESatNotifySendUssdPCmd:
			return NotifySendUssdPCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatNotifyGetInkeyPCmd:
			return NotifyGetInkeyPCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatNotifySendSsPCmd:
			return NotifySendSsPCmd(aTsyReqHandle,aPackage.Des1n());
		case ESatNotifyLaunchBrowserPCmd:
			return NotifyLaunchBrowserPCmd(aTsyReqHandle,aPackage.Des1n());
		default:
			return KErrNotSupported;
		}
	}

TInt CSimSat::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
* CancelService is called by the server when it is "cleaning-up" any still outstanding
* asynchronous requests before closing a client's sub-session.
* This will happen if a client closes its R-class handle without cancelling outstanding asynchronous requests.
* 
*  This function will be called when a client explicitly cancels
*
* @param aTsyReqHandle  The request handle for completing the request 
* @param aIpc Ipc representing the request
* @return err KErrNone if request completes ok
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CANCELSERVICE_1, "CSimSat: - CancelService called");
	switch (aIpc)
		{
		case ESatNotifyRefreshPCmd:
			return NotifyRefreshPCmdCancel(aTsyReqHandle);
		case ESatNotifyRefreshRequired:
		case ESatNotifyRefreshRequiredParam:
			return NotifyRefreshRequiredCancel(aTsyReqHandle);
		case ESatRefreshAllowed:
			return RefreshAllowedCancel(aTsyReqHandle);
		case ESatNotifyRetrieveMultimediaMsgPCmd:
			return NotifyRetrieveMultimediaMsgPCmdCancel(aTsyReqHandle);
		case ESatNotifySubmitMultimediaMsgPCmd:
			return NotifySubmitMultimediaMsgPCmdCancel(aTsyReqHandle);
		case ESatNotifyDisplayMultimediaMsgPCmd:
			return NotifyDisplayMultimediaMsgPCmdCancel(aTsyReqHandle);
		case ESatNotifySetFramesPCmd:
			return NotifySetFramesPCmdCancel(aTsyReqHandle);
		case ESatNotifyGetFramesStatusPCmd:
			return NotifyGetFramesStatusPCmdCancel(aTsyReqHandle);
		case ESatTerminalRsp:
			return TerminalRspCancel(aTsyReqHandle);
		case ESatGetMeSideSatProfile:
			return GetMeSideSatProfileCancel(aTsyReqHandle);
		case ESatMmsTransferStatus:
			return MmsTransferStatusCancel(aTsyReqHandle);
		case ESatMmsNotificationDownload:
			return MmsNotificationDownloadCancel(aTsyReqHandle);
		case ESatUssdDataDownload:	
			return UssdDataDownloadCancel(aTsyReqHandle);
		case ESatNotifyLocalInfoPCmd:
			return NotifyLocalInfoPCmdCancel(aTsyReqHandle);
		case ESatNotifyOpenChannelPCmd:
			return NotifyOpenChannelPCmdCancel(aTsyReqHandle);
		case ESatNotifyPlayTonePCmd:
			return NotifyPlayTonePCmdCancel(aTsyReqHandle);
		case ESatNotifySetUpCallPCmd:
			return NotifySetupCallPCmdCancel(aTsyReqHandle);
		case ESatEventDownload:
			return EventDownloadCancel(aTsyReqHandle);
		case ESatNotifySendUssdPCmd:
			return NotifySendUssdPCmdCancel(aTsyReqHandle);
		case ESatNotifyGetInkeyPCmd:
			return NotifyGetInkeyPCmdCancel(aTsyReqHandle);
		case ESatNotifySendSsPCmd:
			return NotifySendSsPCmdCancel(aTsyReqHandle);
		case ESatNotifyLaunchBrowserPCmd:
			return NotifyLaunchBrowserPCmdCancel(aTsyReqHandle);
		default:
			return KErrGeneral; 
		} 
	}


TInt CSimSat::NotifyRefreshPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_REFRESH_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}

	if(iSatInfo->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iNotifyRefresh.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	RSat::TRefreshV1Pckg* refreshPckg = (RSat::TRefreshV1Pckg*)aPCmd;
	RSat::TRefreshV1& refresh = (*refreshPckg)();
	

	iNotifyRefresh.iNotifyPending=ETrue;
	iNotifyRefresh.iNotifyHandle=aTsyReqHandle;
	iNotifyRefresh.iNotifyData = &refresh;
	
	if(refresh.iType == RSat::E3GSessionReset || refresh.iType == RSat::EFileChangeNotification)
	{
		RSat::TRefreshV2Pckg* refreshPckg = (RSat::TRefreshV2Pckg*)aPCmd;
		RSat::TRefreshV2& refresh2 = (*refreshPckg)();
		
		iNotifyRefresh.iNotifyData = &refresh2;
		iRefreshV2 = ETrue;
	}

	return KErrNone;
	}


TInt CSimSat::NotifyRefreshPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	if(iNotifyRefresh.iNotifyPending)
		{
		iNotifyRefresh.iNotifyPending=EFalse;
		// Clear this flag here since if not within refresh required will be false, and if inside refresh required can be sure results will not be processed
		iRefreshRequiredPending=EFalse; 
		ReqCompleted(iNotifyRefresh.iNotifyHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimSat::NotifyRefreshRequired(const TTsyReqHandle aTsyReqHandle)
	{
	if(iSatInfo->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iRefreshRequired.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iRefreshRequired.iNotifyPending=ETrue;
	iRefreshRequired.iNotifyHandle=aTsyReqHandle;
	iRefreshRequired.iNotifyData = NULL;
	return KErrNone;	
	}

TInt CSimSat::NotifyRefreshRequired(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(iSatInfo->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iRefreshRequired.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	RSat::TRefreshV1Pckg* refreshPckg = (RSat::TRefreshV1Pckg*)aPCmd;
	RSat::TRefreshV1& refresh = (*refreshPckg)();

	iRefreshRequired.iNotifyPending=ETrue;
	iRefreshRequired.iNotifyHandle=aTsyReqHandle;
	iRefreshRequired.iNotifyData = &refresh;
	return KErrNone;	
	}

TInt CSimSat::NotifyRefreshRequiredCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	if(iRefreshRequired.iNotifyPending)
		{
		iRefreshRequired.iNotifyPending=EFalse;
		iRefreshRequiredPending=EFalse;
		ReqCompleted(iRefreshRequired.iNotifyHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimSat::RefreshAllowed(const TTsyReqHandle aTsyReqHandle, TDes8* aRefreshAllowedRsp)
	{
	if(iSatInfo->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	RSat::TRefreshRspV1Pckg* aRspPckg = (RSat::TRefreshRspV1Pckg*)aRefreshAllowedRsp;
	RSat::TRefreshRspV1& rspV1 = (*aRspPckg)();

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_REFRESHALLOWED_1, "RefreshAllowed: - aRefreshAllowedResult %d",rspV1.iGeneralResult);

	/* Set simtsy status to reflect clients status */
	RSat::TPCmdResult refreshAllowedResult = rspV1.iGeneralResult;

	__ASSERT_ALWAYS(!iRefreshAllowed.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));

	ReqCompleted(aTsyReqHandle, KErrNone);

	iRefreshAllowed.iNotifyPending=EFalse;

	if(iRefreshRequiredPending && iNotifyRefresh.iNotifyPending && refreshAllowedResult == RSat::KSuccess)
		{
		iNotifyRefresh.iNotifyPending=EFalse;
		iRefreshRequiredPending=EFalse;

 		RSat::TRefreshV1&  refreshData = *(RSat::TRefreshV1*)iNotifyRefresh.iNotifyData;

  		refreshData.iType = iType;
  		refreshData.iFileList = iFileList;
  		
  		ReqCompleted(iNotifyRefresh.iNotifyHandle, KErrNone);
		}

	return KErrNone;	
	}

TInt CSimSat::RefreshAllowedCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	if(iRefreshAllowed.iNotifyPending)
		{
		iRefreshAllowed.iNotifyPending=EFalse;
		ReqCompleted(iRefreshAllowed.iNotifyHandle,KErrCancel);
		}
	return KErrNone;
	}

TInt CSimSat::NotifyRetrieveMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd)
	{
	
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_RET_MM_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			TRAP(err, ret = ConfigL(SIMTSY_SUB_MM_PCMD_NUMBER));
			if(err != KErrNone)
				{
				ret = err;
				}

			if(ret!=KErrNone)
				{
				return ret;
				}
			else
				{
				TRAP(err, ret = ConfigL(SIMTSY_ELEMFILES_CMD_NUMBER));
				if(err != KErrNone)
					{
					ret = err;
					}
				if(ret!=KErrNone)
					{
					return ret;
					}
				else
					{
					iConfigDone = ETrue;
					}
				}
			}
		}
	
	if(iMMRetrieve->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iNotifyRetrieveMM.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	
	RSat::TRetrieveMultimediaMessageV6Pckg *aRspPckg = (RSat::TRetrieveMultimediaMessageV6Pckg*)aPCmd;
	RSat::TRetrieveMultimediaMessageV6& rspRetMM = (*aRspPckg)();
	
	iNotifyRetrieveMM.iNotifyPending=ETrue;
	iNotifyRetrieveMM.iNotifyHandle=aTsyReqHandle;
	iNotifyRetrieveMM.iNotifyData = &rspRetMM;
	
	iRefreshEf= ETrue;
		
	return KErrNone;
	
	}

TInt CSimSat::NotifyRetrieveMultimediaMsgPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	
	if(iNotifyRetrieveMM.iNotifyPending)
		{
		iNotifyRetrieveMM.iNotifyPending=EFalse;
		ReqCompleted(iNotifyRetrieveMM.iNotifyHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(iNotifyRetrieveMM.iNotifyHandle,KErrNone);
		}
	return KErrNone;
		
	}

TInt CSimSat::NotifyLaunchBrowserPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd)
	{
	
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_LNCH_BRWSR_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			TRAP(err, ret = ConfigL(SIMTSY_EVENT_LIST_CMD_NUMBER));
			if(err != KErrNone)
				{
				ret = err;
				}

			if(ret != KErrNone)
				{
				return ret;
				}
			else
				{
				iConfigDone = ETrue;
				}
			}
		}

	if(iLnchBrwsr->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iNotifyLnchBrwsr.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	RSat::TLaunchBrowserV6Pckg *aRspPckg = (RSat::TLaunchBrowserV6Pckg*)aPCmd;
	RSat::TLaunchBrowserV6& rspLncBrwsr = (*aRspPckg)();
	
	iNotifyLnchBrwsr.iNotifyPending=ETrue;
	iNotifyLnchBrwsr.iNotifyHandle=aTsyReqHandle;
	iNotifyLnchBrwsr.iNotifyData = &rspLncBrwsr;
	
	if(!iRefreshEf)
		{
		iRefreshEf= ETrue;
		}
	
	return KErrNone;
	
	}


TInt CSimSat::NotifyLaunchBrowserPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	
	if(iNotifyLnchBrwsr.iNotifyPending)
		{
		iNotifyLnchBrwsr.iNotifyPending=EFalse;
		ReqCompleted(iNotifyLnchBrwsr.iNotifyHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(iNotifyLnchBrwsr.iNotifyHandle,KErrNone);
		}
	return KErrNone;
		
	}
	
TInt CSimSat::NotifySubmitMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd)
	{
	
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_SUB_MM_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			TRAP(err, ret = ConfigL(SIMTSY_ELEMFILES_CMD_NUMBER));
			if(err != KErrNone)
				{
				ret = err;
				}

			if(ret!=KErrNone)
				{
				return ret;
				}
			else
				{
				iConfigDone = ETrue;
				}
			}
		}
	
	if(iMMSubmit->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iNotifySubmitMM.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	RSat::TSubmitMultimediaMessageV6Pckg *aRspPckg = (RSat::TSubmitMultimediaMessageV6Pckg*)aPCmd;
	RSat::TSubmitMultimediaMessageV6& rspRetMM = (*aRspPckg)();
	
	iNotifySubmitMM.iNotifyPending=ETrue;
	iNotifySubmitMM.iNotifyHandle=aTsyReqHandle;
	iNotifySubmitMM.iNotifyData = &rspRetMM;
	
	if(!iRefreshEf)
		{
		iRefreshEf= ETrue;
		}
	
	return KErrNone;
	
	}

TInt CSimSat::NotifySubmitMultimediaMsgPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	
	if(iNotifySubmitMM.iNotifyPending)
		{
		iNotifySubmitMM.iNotifyPending=EFalse;
		ReqCompleted(iNotifySubmitMM.iNotifyHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(iNotifySubmitMM.iNotifyHandle,KErrNone);
		}
	return KErrNone;
		
	}

TInt CSimSat::NotifyDisplayMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd)
	{
	
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_DISP_MM_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}

	if(iMMDisplay->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iNotifyDisplayMM.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	RSat::TDisplayMultimediaMessageV6Pckg *aRspPckg = (RSat::TDisplayMultimediaMessageV6Pckg*)aPCmd;
	RSat::TDisplayMultimediaMessageV6& rspRetMM = (*aRspPckg)();
	
	iNotifyDisplayMM.iNotifyPending=ETrue;
	iNotifyDisplayMM.iNotifyHandle=aTsyReqHandle;
	iNotifyDisplayMM.iNotifyData = &rspRetMM;
	
	return KErrNone;
	
	}

TInt CSimSat::NotifyDisplayMultimediaMsgPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	
	if(iNotifyDisplayMM.iNotifyPending)
		{
		iNotifyDisplayMM.iNotifyPending=EFalse;
		ReqCompleted(iNotifyDisplayMM.iNotifyHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(iNotifyDisplayMM.iNotifyHandle,KErrNone);
		}
	return KErrNone;
		
	}

TInt CSimSat::NotifySetFramesPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd)
	{

	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_SET_FRMS_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}

	if(iSetFrms->Count() == 0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}
	
	__ASSERT_ALWAYS(!iNotifySetFrms.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	RSat::TSetFramesV6Pckg *aRspPckg = (RSat::TSetFramesV6Pckg*)aPCmd;
	RSat::TSetFramesV6& rspSetFrmsV6 = (*aRspPckg)();
	
	iNotifySetFrms.iNotifyPending=ETrue;
	iNotifySetFrms.iNotifyHandle=aTsyReqHandle;
	iNotifySetFrms.iNotifyData = &rspSetFrmsV6;
	
	return KErrNone;
	}

TInt CSimSat::NotifySetFramesPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	
	if(iNotifySetFrms.iNotifyPending)
		{
		iNotifySetFrms.iNotifyPending=EFalse;
		ReqCompleted(iNotifySetFrms.iNotifyHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(iNotifySetFrms.iNotifyHandle,KErrNone);
		}
	return KErrNone;
		
	}

TInt CSimSat::NotifyGetFramesStatusPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd)
	{
	
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_GET_FRMS_STS_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}

	if(iGetFrmsSts->Count() == 0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}
	
	__ASSERT_ALWAYS(!iNotifyGetFrmsSts.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	RSat::TGetFramesStatusV6Pckg *aRspPckg = (RSat::TGetFramesStatusV6Pckg*)aPCmd;
	RSat::TGetFramesStatusV6& rspGetFrmsV6 = (*aRspPckg)();
	
	iNotifyGetFrmsSts.iNotifyPending = ETrue;
	iNotifyGetFrmsSts.iNotifyHandle = aTsyReqHandle;
	iNotifyGetFrmsSts.iNotifyData = &rspGetFrmsV6;
	
	return KErrNone;

	}

TInt CSimSat::NotifyGetFramesStatusPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
	
	if(iNotifyGetFrmsSts.iNotifyPending)
		{
		iNotifyGetFrmsSts.iNotifyPending=EFalse;
		ReqCompleted(iNotifyGetFrmsSts.iNotifyHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(iNotifyGetFrmsSts.iNotifyHandle,KErrNone);
		}
	
	return KErrNone;
		
	}

TInt CSimSat::UssdDataDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aDes)
	{
	// Has the config file been parsed for this functionality 
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_USSD_CMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}
		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}
	
	TInt err(KErrNone);
	TBool found(EFalse);
	TInt count(iUssdData->Count());
	
	// If the config file does not contain any USSD Download related data
	// then complete the request with KErrNotSupported
	if(!count)
		{
		err = KErrNotSupported;
		}
	else 
		{
		// otherwise extract the data supplied by the client 
		RSat::TUssdDataDownloadV6Pckg *ussdDataV6Pckg = (RSat::TUssdDataDownloadV6Pckg*)aDes;
		RSat::TUssdDataDownloadV6& ussdDataV6 = (*ussdDataV6Pckg)();
		
		for(TInt i=0; i<count; i++)
			{
			TUssdData tempUssdData =  iUssdData->At(i);
			
			// Compare the supplied data to the data retrieved from the config file 
			// and proceed only if they match 
			if((ussdDataV6.iDeviceId == STATIC_CAST(RSat::TDeviceId, tempUssdData.iSrc)) &&
			   (ussdDataV6.iUssdString.iDcs == tempUssdData.iDcs) && 
			   (ussdDataV6.iUssdString.iUssdString.Compare(tempUssdData.iUssdStr) == 0))
				{
				found = ETrue;
				
				if(tempUssdData.iUICCRsp == SIMTSY_UICC_SUCC_RSP)
					{
					err = KErrNone;
					}
				else if(tempUssdData.iUICCRsp == SIMTSY_UICC_RETRY_RSP)
					{
					err = KErrNotReady;
					}
				else if(tempUssdData.iUICCRsp  == SIMTSY_UICC_FLR_RSP_1  || 
					    tempUssdData.iUICCRsp  == SIMTSY_UICC_FLR_RSP_2)
					{
					err = KErrNotSupported;
					}
				
				iUSSDDataDownloadHandle  = aTsyReqHandle;
				break;
				}
			} // end for loop 
		} // end else
	
	// the supplied data does not match the one in the config file, so complete the request with an error
	if(!found)
		err = KErrCorrupt;
		
	ReqCompleted(aTsyReqHandle, err);
	return KErrNone;
	}


TInt CSimSat::UssdDataDownloadCancel(const TTsyReqHandle aTsyReqHandle)
	{	
	if(iUSSDDataDownloadHandle == aTsyReqHandle)
		{
		ReqCompleted(aTsyReqHandle, KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle, KErrCorrupt);	
		}
	return KErrNone;
	}

TInt CSimSat::NotifyLocalInfoPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd)
	{
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_PRV_LCL_INFO_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}

	if(iLocalInfo->Count() == 0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}
	
	RSat::TLocalInfoV6Pckg *aRspPckg = (RSat::TLocalInfoV6Pckg*)aPCmd;
	RSat::TLocalInfoV6& rspLocalInfo = (*aRspPckg)();
	
	iNotifyLocalInfo.iNotifyPending = ETrue;
	iNotifyLocalInfo.iNotifyHandle  = aTsyReqHandle;
	iNotifyLocalInfo.iNotifyData    = &rspLocalInfo;

	return KErrNone;
	}

TInt CSimSat::NotifyLocalInfoPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
{
	if(iNotifyLocalInfo.iNotifyPending)
		{
		iNotifyLocalInfo.iNotifyPending= EFalse;
		ReqCompleted(iNotifyLocalInfo.iNotifyHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(iNotifyLocalInfo.iNotifyHandle,KErrNone);
		}
	return KErrNone;
}


TInt CSimSat::NotifyOpenChannelPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_OPEN_CHAN_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}

	if(iOpenChn->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iNotifyOpenChn.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	RSat::TOpenLocalLinksChannelV5Pckg* opnChnPckg = (RSat::TOpenLocalLinksChannelV5Pckg*)aPCmd;
	RSat::TOpenLocalLinksChannelV5 & opnChn = (*opnChnPckg)();

	iNotifyOpenChn.iNotifyPending=ETrue;
	iNotifyOpenChn.iNotifyHandle=aTsyReqHandle;
	iNotifyOpenChn.iNotifyData = &opnChn;
	return KErrNone;
	}


TInt CSimSat::NotifyOpenChannelPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
		if(iNotifyOpenChn.iNotifyPending)
			{
			iNotifyOpenChn.iNotifyPending= EFalse;
			ReqCompleted(iNotifyOpenChn.iNotifyHandle,KErrCancel);
			}
		else
			{
			ReqCompleted(iNotifyOpenChn.iNotifyHandle,KErrNone);
			}
	return KErrNone;
	}

TInt CSimSat::NotifyPlayTonePCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_PLAY_TONE_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}

	if(iMiscCmd->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}
	if(!iPlayTone)
		{
		iPlayTone = ETrue;
		}

	__ASSERT_ALWAYS(!iNotifyMiscCmd.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	RSat::TPlayToneV6Pckg* playTonePckg = (RSat::TPlayToneV6Pckg*)aPCmd;
	RSat::TPlayToneV6& playTone = (*playTonePckg)();
	
	iNotifyMiscCmd.iNotifyPending=ETrue;
	iNotifyMiscCmd.iNotifyHandle=aTsyReqHandle;
	iNotifyMiscCmd.iNotifyData = &playTone;
	return KErrNone;
	}


TInt CSimSat::NotifyPlayTonePCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
		if(iNotifyMiscCmd.iNotifyPending)
			{
			iNotifyMiscCmd.iNotifyPending= EFalse;
			iMiscCmdIndex ++;
			ReqCompleted(iNotifyMiscCmd.iNotifyHandle,KErrCancel);
			}
		else
			{
			ReqCompleted(iNotifyMiscCmd.iNotifyHandle,KErrNone);
			}
	
	return KErrNone;
	}
	
TInt CSimSat::NotifySetupCallPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_SETUP_CALL_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}

	if(iMiscCmd->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}
	
	if(!iPlayTone && !iSetupCall)
		{
		iMiscCmdIndex += SIMTSY_PLAYTONE_TESTCASES_NUMBER;
		}
			
	if(!iSetupCall)
		{
		iSetupCall = ETrue;
		}

	__ASSERT_ALWAYS(!iNotifyMiscCmd.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	RSat::TSetUpCallV6Pckg* setupCallPckg = (RSat::TSetUpCallV6Pckg*)aPCmd;
	RSat::TSetUpCallV6& setupCall = (*setupCallPckg)();
	
	iNotifyMiscCmd.iNotifyPending=ETrue;
	iNotifyMiscCmd.iNotifyHandle=aTsyReqHandle;
	iNotifyMiscCmd.iNotifyData  = &setupCall;
	return KErrNone;
	}

TInt CSimSat::NotifySetupCallPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
		if(iNotifyMiscCmd.iNotifyPending)
			{
			iNotifyMiscCmd.iNotifyPending= EFalse;
			iMiscCmdIndex ++;
			ReqCompleted(iNotifyMiscCmd.iNotifyHandle,KErrCancel);
			}
		else
			{
			ReqCompleted(iNotifyMiscCmd.iNotifyHandle,KErrNone);
			}
	
	return KErrNone;
	}
	
	
TInt CSimSat::NotifyGetInkeyPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{

	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_GET_INKEY_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			TRAP(err, ret = ConfigL(SIMTSY_SENDSS_CMD_NUMBER));
			if(err != KErrNone)
				{
				ret = err;
				}
			
			if(ret != KErrNone)
				{
				return ret;
				}
			else
				{
				iConfigDone = ETrue;
				}
			}
		}

	if(iMiscCmd->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}
		
	if(!iGetInkey)
		{
		iMiscCmdIndex += (SIMTSY_PLAYTONE_TESTCASES_NUMBER + SIMTSY_SETUPCALL_TESTCASES_NUMBER);
		iGetInkey = ETrue;
		}
		
	__ASSERT_ALWAYS(!iNotifyMiscCmd.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	RSat::TGetInkeyV6Pckg* getInkeyPckg = (RSat::TGetInkeyV6Pckg*)aPCmd;
	RSat::TGetInkeyV6& getInkey = (*getInkeyPckg)();
	
	iNotifyMiscCmd.iNotifyPending=ETrue;
	iNotifyMiscCmd.iNotifyHandle=aTsyReqHandle;
	iNotifyMiscCmd.iNotifyData  = &getInkey;
	return KErrNone;
	}

TInt CSimSat::NotifyGetInkeyPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
		if(iNotifyMiscCmd.iNotifyPending)
			{
			iNotifyMiscCmd.iNotifyPending= EFalse;
			iMiscCmdIndex ++;
			ReqCompleted(iNotifyMiscCmd.iNotifyHandle,KErrCancel);
			}
		else
			{
			ReqCompleted(iNotifyMiscCmd.iNotifyHandle,KErrNone);
			}
	
	return KErrNone;
	}

TInt CSimSat::NotifySendSsPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	
	if(iSendSS->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iNotifySendSs.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	RSat::TSendSsV6Pckg *sendSsV6Pckg = (RSat::TSendSsV6Pckg*)aPCmd;
	RSat::TSendSsV6 & sendSsV6 = (*sendSsV6Pckg)();
	
	iNotifySendSs.iNotifyPending=ETrue;
	iNotifySendSs.iNotifyHandle=aTsyReqHandle;
	iNotifySendSs.iNotifyData  = &sendSsV6;
	return KErrNone;
	}
	
TInt CSimSat::NotifySendSsPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
		if(iNotifySendSs.iNotifyPending)
			{
			iNotifySendSs.iNotifyPending= EFalse;
			iSendSsIndex++;
			ReqCompleted(iNotifySendSs.iNotifyHandle,KErrCancel);
			}
		else
			{
			ReqCompleted(iNotifySendSs.iNotifyHandle,KErrNone);
			}
		
	return KErrNone;
	}	

TInt CSimSat::NotifySendUssdPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{

	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_SEND_USSD_PCMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}

	if(iSendUssd->Count()==0)
		{
		ReqCompleted(aTsyReqHandle,KErrNotSupported);
		return KErrNone;
		}

	__ASSERT_ALWAYS(!iNotifySendUssd.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	RSat::TSendUssdV6Pckg* sendUSSDPckg = (RSat::TSendUssdV6Pckg*)aPCmd;
	RSat::TSendUssdV6& sendUSSD = (*sendUSSDPckg)();
	
	iNotifySendUssd.iNotifyPending=ETrue;
	iNotifySendUssd.iNotifyHandle=aTsyReqHandle;
	iNotifySendUssd.iNotifyData  = &sendUSSD;
	return KErrNone;
	}
	
TInt CSimSat::NotifySendUssdPCmdCancel(const TTsyReqHandle /*aTsyReqHandle*/)
	{
		if(iNotifySendUssd.iNotifyPending)
			{
			iNotifySendUssd.iNotifyPending= EFalse;
			iSendUssdIndex ++;
			ReqCompleted(iNotifySendUssd.iNotifyHandle,KErrCancel);
			}
		else
			{
			ReqCompleted(iNotifySendUssd.iNotifyHandle,KErrNone);
			}
	
	return KErrNone;
	}
	
TInt CSimSat::TerminalRsp(const TTsyReqHandle aTsyReqHandle, RSat::TPCmd* aPCmd, TDes8* aRsp)
{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_TERMINALRSP_1, ">>CSimSat::TerminalRsp");
	iPendingTerRsp = ETrue;
	
	switch (*aPCmd)
		{
		case RSat::ERetrieveMultimediaMsg:
			return RetrieveMultimediaRsp(aTsyReqHandle, aRsp);
		case RSat::ESubmitMultimediaMsg:
			return SubmitMultimediaRsp(aTsyReqHandle, aRsp);
		case RSat::EDisplayMultimediaMsg:
			return DisplayMultimediaRsp(aTsyReqHandle, aRsp);
		case RSat::ESetFrames:
			return SetFramesRsp(aTsyReqHandle,aRsp);
		case RSat::EGetFramesStatus:
			return GetFramesStatusRsp(aTsyReqHandle,aRsp);
		case RSat::ELocalInfo:
			return ProvideLocalInfoRsp(aTsyReqHandle,aRsp);
		case RSat::EOpenChannelGprs:
        case RSat::EOpenChannelLocal:
        case RSat::ECloseChannel:
        	return OpenChannelRsp(aTsyReqHandle,aRsp);
		case RSat::EPlayTone:
			return PlayToneRsp(aTsyReqHandle,aRsp);
		case RSat::ESetUpCall:
			return SetupCallRsp(aTsyReqHandle,aRsp);
		case RSat::ERefresh:
			return RefreshRsp(aTsyReqHandle,aRsp);
		case RSat::ESendUssd:
			return SendUssdRsp(aTsyReqHandle,aRsp);
		case RSat::EGetInkey:
			return GetInkeyRsp(aTsyReqHandle,aRsp);
		case RSat::ELaunchBrowser:
			return LaunchBrowserRsp(aTsyReqHandle,aRsp);
		default:
			iPendingTerRsp = EFalse;
			return KErrGeneral;
		}
}

TInt CSimSat::TerminalRspCancel(const TTsyReqHandle aTsyReqHandle)
{
	if(iPendingTerRsp)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
}


void CSimSat::TimerCallBack(TInt /*aId*/)
/**
* Timer callback function.  When the timer goes off, it will call back into this
* function for further processing.
*
* @param aId an id identifying which timer callback is being called
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_TIMERCALLBACK_1, ">>CSimSat::TimerCallBack");
		
	//
	// Convert the SIM TSY text version into the RSat binary version...
	//
	/* Obtain type and file list if present and cache for completion by Refresh Allowed */
	if(iSatIndex < iSatInfo->Count() & !(iRefreshEf))
		{
		iType = STATIC_CAST(RSat::TRefreshType,iSatInfo->At(iSatIndex).iType);
		iConvertTextToFileReturnCode = ConvertTextToFileList(iSatInfo->At(iSatIndex).iFileList,
 		                                                 iFileList);
 		iSatIndex++;
		}
	
	// Only process NotifyRefreshPCmd here if not enclosed in RefreshRequired cycle
	if(iNotifyRefresh.iNotifyPending && !iRefreshRequired.iNotifyPending)
		{
		iNotifyRefresh.iNotifyPending=EFalse;

 		RSat::TRefreshV1&  refreshData = *(RSat::TRefreshV1*)iNotifyRefresh.iNotifyData;
 		
 		refreshData.iType = iType;
 		refreshData.iFileList = iFileList;
 		
 		if(iRefreshV2)
			{
			RSat::TRefreshV2& refresh2 = *(RSat::TRefreshV2*)iNotifyRefresh.iNotifyData;
			refresh2.iType = STATIC_CAST(RSat::TRefreshType,iRefresh->At(iRefreshIndex).iType);
			ConvertTextToTBuf16(iRefresh->At(iRefreshIndex).iFileList,refresh2.iAid);
			iRefreshIndex++;
			}
		
	    if(iRefreshEf)
	   		{
	    	refreshData.iType = STATIC_CAST(RSat::TRefreshType,iElemFiles->At(iElemFilesIndex).iType);
	    	ConvertTextToFileList(iElemFiles->At(iElemFilesIndex).iFileList,refreshData.iFileList);
	    	iElemFilesIndex++;
	    	}
		
		ReqCompleted(iNotifyRefresh.iNotifyHandle, iConvertTextToFileReturnCode);
		}

	if(iRefreshRequired.iNotifyPending)
		{
		if (iConvertTextToFileReturnCode == KErrNone)
		    {
		    iRefreshRequired.iNotifyPending=EFalse;
		    iRefreshRequiredPending = ETrue;

 		    if (iRefreshRequired.iNotifyData != NULL)
 			    {
 			    RSat::TRefreshV1&  refreshData = *(RSat::TRefreshV1*)iRefreshRequired.iNotifyData;
  		
			    refreshData.iType = iType;
 			    refreshData.iFileList = iFileList;
 			    }
		    }
        
		ReqCompleted(iRefreshRequired.iNotifyHandle, iConvertTextToFileReturnCode);
		}
		
	if(iNotifyRetrieveMM.iNotifyPending)
		{
			
			RSat::TRetrieveMultimediaMessageV6 *tMMMessageV6 = (RSat::TRetrieveMultimediaMessageV6*)iNotifyRetrieveMM.iNotifyData;
					
			tMMMessageV6->iDestination = STATIC_CAST(RSat::TDeviceId,iMMRetrieve->At(iRetIndex).iDestn);
			tMMMessageV6->iAlphaId.iStatus = STATIC_CAST(RSat::TAlphaIdStatus,iMMRetrieve->At(iRetIndex).iAlphaStatus);
		
			ConvertTextToTBuf254(iMMRetrieve->At(iRetIndex).iAlphaIDBuf,tMMMessageV6->iAlphaId.iAlphaId);
		
			tMMMessageV6->iIconId.iQualifier = STATIC_CAST(RSat::TIconQualifier,iMMRetrieve->At(iRetIndex).iIconID);
			tMMMessageV6->iIconId.iIdentifier = STATIC_CAST(TUint8,iMMRetrieve->At(iRetIndex).iIconIDBuf);
		
			ConvertTextToTBuf255(iMMRetrieve->At(iRetIndex).iMMMsgRef,tMMMessageV6->iMultimediaMessageRef);
			ConvertTextToTBuf242(iMMRetrieve->At(iRetIndex).iRcptnFile,tMMMessageV6->iReceptionFile);
			ConvertTextToTBuf255(iMMRetrieve->At(iRetIndex).iContentId,tMMMessageV6->iMultimediaContentId);
			ConvertTextToTBuf255(iMMRetrieve->At(iRetIndex).iMsgId,tMMMessageV6->iMultimediaMessageId);
			
			tMMMessageV6->iTextAttribute.iStatus = STATIC_CAST(RSat::TTextAttributeStatus, iMMRetrieve->At(iRetIndex).iTextAttStatus);
		
			ConvertTextToTBuf4(iMMRetrieve->At(iRetIndex).iTextAttBuf,tMMMessageV6->iTextAttribute.iTextAttributeData);
					
			iNotifyRetrieveMM.iNotifyPending = EFalse;
			if(iRetIndex < iMMRetrieve->Count())
				{
				iRetTIndex = iRetIndex;
				iRetIndex++;	
				}
						
			ReqCompleted(iNotifyRetrieveMM.iNotifyHandle, KErrNone);
		
		}
		
	if(iNotifySubmitMM.iNotifyPending)
		{
			
			RSat::TSubmitMultimediaMessageV6 *tMMMessageV6 = (RSat::TSubmitMultimediaMessageV6*)iNotifySubmitMM.iNotifyData;
			
		
			tMMMessageV6->iDestination = STATIC_CAST(RSat::TDeviceId,iMMSubmit->At(iSubIndex).iDestn);
			tMMMessageV6->iAlphaId.iStatus = STATIC_CAST(RSat::TAlphaIdStatus,iMMSubmit->At(iSubIndex).iAlphaStatus);
		
			ConvertTextToTBuf254(iMMSubmit->At(iSubIndex).iAlphaIDBuf,tMMMessageV6->iAlphaId.iAlphaId);
		
			tMMMessageV6->iIconId.iQualifier = STATIC_CAST(RSat::TIconQualifier,iMMSubmit->At(iSubIndex).iIconID);
			tMMMessageV6->iIconId.iIdentifier = STATIC_CAST(TUint8,iMMSubmit->At(iSubIndex).iIconIDBuf);
		
			ConvertTextToTBuf242(iMMSubmit->At(iSubIndex).iSbmsnFile,tMMMessageV6->iSubmissionFile);
			ConvertTextToTBuf255(iMMSubmit->At(iSubIndex).iMsgId,tMMMessageV6->iMultimediaMessageId);
			
			tMMMessageV6->iTextAttribute.iStatus = STATIC_CAST(RSat::TTextAttributeStatus, iMMSubmit->At(iSubIndex).iTextAttStatus);
		
			ConvertTextToTBuf4(iMMSubmit->At(iSubIndex).iTextAttBuf,tMMMessageV6->iTextAttribute.iTextAttributeData);
					
			iNotifySubmitMM.iNotifyPending = EFalse;
			
			if(iSubIndex < iMMSubmit->Count())
				{
				iSubTIndex = iSubIndex;
				iSubIndex++;	
				}
							
			ReqCompleted(iNotifySubmitMM.iNotifyHandle, KErrNone);
		
		}
		
	if(iNotifyDisplayMM.iNotifyPending)
	{
			
			RSat::TDisplayMultimediaMessageV6 *tMMMessageV6 = (RSat::TDisplayMultimediaMessageV6*)iNotifyDisplayMM.iNotifyData;
			
		
			tMMMessageV6->iDestination = STATIC_CAST(RSat::TDeviceId,iMMDisplay->At(iDispIndex).iDestn);
			tMMMessageV6->iDisplayPriority = STATIC_CAST(RSat::TDisplayPriority,iMMDisplay->At(iDispIndex).iDispPri);
			tMMMessageV6->iClearScreenTrigger = STATIC_CAST(RSat::TClearScreenTrigger,iMMDisplay->At(iDispIndex).iClrScr);
			
		
			ConvertTextToTBuf242(iMMDisplay->At(iDispIndex).iSbmsnFile,tMMMessageV6->iSubmissionFile);
			ConvertTextToTBuf255(iMMDisplay->At(iDispIndex).iMsgId,tMMMessageV6->iMultimediaMessageId);
			tMMMessageV6->iImmediateRsp = STATIC_CAST(RSat::TImmediateRsp,iMMDisplay->At(iDispIndex).iImmRsp);
			
			iNotifyDisplayMM.iNotifyPending = EFalse;
			
			if(iDispIndex < iMMDisplay->Count())
				{
				iDispTIndex = iDispIndex;
				iDispIndex++;	
				}
						
			ReqCompleted(iNotifyDisplayMM.iNotifyHandle, KErrNone);
		
		}
	if(iNotifySetFrms.iNotifyPending)
		{
		RSat::TSetFramesV6 *tSetFramesV6 = (RSat::TSetFramesV6*)iNotifySetFrms.iNotifyData;
		
		tSetFramesV6->iDestination = STATIC_CAST(RSat::TDeviceId,iSetFrms->At(iSetFrmsIndex).iDestn);
		tSetFramesV6->iFrameId = iSetFrms->At(iSetFrmsIndex).iFrmId;
		tSetFramesV6->iFrameLayout.iFramesLayout = STATIC_CAST(RSat::TFramesLayout,iSetFrms->At(iSetFrmsIndex).iFrmLayout);
		
		ConvertTextToTBuf243(iSetFrms->At(iSetFrmsIndex).iFrmLayoutBuf,tSetFramesV6->iFrameLayout.iFrameLayoutBuf);
		
		tSetFramesV6->iDefaultFrameId = iSetFrms->At(iSetFrmsIndex).iDefFrmId;
		tSetFramesV6->iFramesSeparator = STATIC_CAST(RSat::TFramesSeparator,iSetFrms->At(iSetFrmsIndex).iFrmSeparator);
		
		iNotifySetFrms.iNotifyPending = EFalse;
		
		if(iSetFrmsIndex < iSetFrms->Count())
			{
			iSetFrmsIndex++;
			}
		
		ReqCompleted(iNotifySetFrms.iNotifyHandle, KErrNone);
		}
	
	if(iNotifyGetFrmsSts.iNotifyPending)
		{
		RSat::TGetFramesStatusV6 *tGetFramesV6 = (RSat::TGetFramesStatusV6*) iNotifyGetFrmsSts.iNotifyData;
		
		tGetFramesV6->iDestination = STATIC_CAST(RSat::TDeviceId,iGetFrmsSts->At(iGetFrmsStsIndex).iDestn);
		
		iNotifyGetFrmsSts.iNotifyPending = EFalse;
		
		if(iGetFrmsStsIndex < iGetFrmsSts->Count())
			{
			iGetFrmsStsIndex++;
			}
		
		ReqCompleted(iNotifyGetFrmsSts.iNotifyHandle,KErrNone);	
		}
	
	if(iNotifyLnchBrwsr.iNotifyPending)
		{
		RSat::TLaunchBrowserV6 *tLnchBrwsr = (RSat::TLaunchBrowserV6*)iNotifyLnchBrwsr.iNotifyData;
		
		tLnchBrwsr->iFrameId = iLnchBrwsr->At(iLnchBrwsrIndex).iFrameId;
		
		iNotifyLnchBrwsr.iNotifyPending = EFalse;
		
		if(iLnchBrwsrIndex < iLnchBrwsr->Count())
			{
			iLnchBrwsrIndex++;
			}
		
		ReqCompleted(iNotifyLnchBrwsr.iNotifyHandle,KErrNone);	
		}
	
		
	if(iNotifyLocalInfo.iNotifyPending)
		{
		RSat::TLocalInfoV6 *tLocal = (RSat::TLocalInfoV6*)iNotifyLocalInfo.iNotifyData;
		
		if(iLocalInfoIndex == 2)
			{
			iLocalInfoIndex ++;
			}
		
		tLocal->iDevideId = 
			STATIC_CAST(RSat::TDeviceId,iLocalInfo->At(iLocalInfoIndex).iDeviceId);
		tLocal->iInfoType = 
			STATIC_CAST(RSat::TLocaInfomationType,iLocalInfo->At(iLocalInfoIndex).iLocalInfoType);
		tLocal->iUtranMeasurementQualifier = 
			STATIC_CAST(RSat::TUtranMeasurementQualifier,iLocalInfo->At(iLocalInfoIndex).iUTRANQlfr);
		
		iNotifyLocalInfo.iNotifyPending = EFalse;
		
		if(iLocalInfoIndex< iLocalInfo->Count())
			{
			iLocalInfoIndex++;	
			}
		
		if(iLocalInfoIndex == 1 || iLocalInfoIndex == 4 )
			{
			ReqCompleted(iNotifyLocalInfo.iNotifyHandle,KErrNone);
			}
		if(iLocalInfoIndex == 2 || iLocalInfoIndex == 5 || iLocalInfoIndex == 6)
			{
			ReqCompleted(iNotifyLocalInfo.iNotifyHandle,KErrNotSupported);
			}
		else
			{
			ReqCompleted(iNotifyLocalInfo.iNotifyHandle,KErrCorrupt);
			}
		}
	
	if(iNotifyOpenChn.iNotifyPending)
		{
		RSat::TOpenLocalLinksChannelV5 *tOpenChannel = (RSat::TOpenLocalLinksChannelV5*) iNotifyOpenChn.iNotifyData;
		
		tOpenChannel->iBearer.iType = STATIC_CAST(RSat::TBearerType,iOpenChn->At(iOpenChnIndex).iBearerType);
		
		iNotifyOpenChn.iNotifyPending = EFalse;
		
		if(iOpenChnIndex < iOpenChn->Count())
			{
			}
		
		ReqCompleted(iNotifyOpenChn.iNotifyHandle,KErrNone);
		}
	
	if((iEventDnldHandle) && iEventDnldIndex <= iEventList->Count())
		{
		if(iSendRspEventDnld )
			{
			ReqCompleted(iEventDnldHandle,KErrNone);	
			}
		else
			{
			ReqCompleted(iEventDnldHandle,KErrCorrupt);
			}
		}
	
	if(iNotifySendUssd.iNotifyPending)
		{
		
		RSat::TSendUssdV6 *tSendUssd = (RSat::TSendUssdV6*)iNotifySendUssd.iNotifyData;
		
		tSendUssd->iTextAttribute.iStatus = STATIC_CAST(RSat::TTextAttributeStatus,iSendUssd->At(iSendUssdIndex).iTextAttStatus);
		
		ConvertTextToTBuf4(iSendUssd->At(iSendUssdIndex).iTextAttBuf,tSendUssd->iTextAttribute.iTextAttributeData);
				
		iNotifySendUssd.iNotifyPending = EFalse;
		
		if(iSendUssdIndex < iSendUssd->Count())
			{
			iSendUssdIndex++;
			}
		
		ReqCompleted(iNotifySendUssd.iNotifyHandle,KErrNone);
		}
	
	if(iNotifyMiscCmd.iNotifyPending)
		{
		if((iPlayTone) && (iMiscCmdIndex < SIMTSY_PLAYTONE_TESTCASES_NUMBER))
			{
			RSat::TPlayToneV6* tPlayTone = (RSat::TPlayToneV6*)iNotifyMiscCmd.iNotifyData;
		
			tPlayTone->iFrameId = iMiscCmd->At(iMiscCmdIndex).iFrameId;	
			}
		else if((iSetupCall) && (iMiscCmdIndex < (SIMTSY_SETUPCALL_TESTCASES_NUMBER + SIMTSY_PLAYTONE_TESTCASES_NUMBER)))
			{
			RSat::TSetUpCallV6* tSetupCall = (RSat::TSetUpCallV6*)iNotifyMiscCmd.iNotifyData;
			
			tSetupCall->iFrameId = iMiscCmd->At(iMiscCmdIndex).iFrameId;
			}
		else if(iMiscCmdIndex < iMiscCmd->Count())
			{
			RSat::TGetInkeyV6* tGetInkey = (RSat::TGetInkeyV6*)iNotifyMiscCmd.iNotifyData;
			
			tGetInkey->iFrameId = iMiscCmd->At(iMiscCmdIndex).iFrameId;
			}

		iNotifyMiscCmd.iNotifyPending = EFalse;
		
		if(iMiscCmdIndex < iMiscCmd->Count())
			{
			iMiscCmdIndex ++;
			}
		
		ReqCompleted(iNotifyMiscCmd.iNotifyHandle,KErrNone);
		}
	
	
	if(iNotifySendSs.iNotifyPending)
		{
		RSat::TSendSsV6 *tSendSs = (RSat::TSendSsV6*)iNotifySendSs.iNotifyData;
		tSendSs->iTextAttribute.iStatus= STATIC_CAST(RSat::TTextAttributeStatus,iSendSS->At(iSendSsIndex).iTextAttStatus);
		
		ConvertTextToTBuf4(iSendSS->At(iSendSsIndex).iTextAttBuf,tSendSs->iTextAttribute.iTextAttributeData);
				
		iNotifySendSs.iNotifyPending = EFalse;
		
		if(iSendSsIndex < iSendSS->Count())
			{
			iSendSsIndex++;
			}
		
		ReqCompleted(iNotifySendSs.iNotifyHandle,KErrNone);
		}
	
	
	if(iSatIndex < iSatInfo->Count())
		{
		iTimer->Start(iSatInfo->At(iSatIndex).iDuration,this);
		}	
	else
		{
		iTimer->Start(randTime(),this);
		}
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_TIMERCALLBACK_2, "<<CSimSat::TimerCallBack");
	}


const CTestConfigSection* CSimSat::CfgFile()
/**
 * Returns a pointer to the current configuration file section.
 *
 * @return CTestConfigSection	A pointer to the current configuration file section.
 */
	{
	return iPhone->CfgFile();
	}

TInt CSimSat::ConvertTextToTBuf242(const TPtrC8& aText,
 									TBuf<242>& aFileList) const
/**
* Converts a text based file list (e.g. "6F3A6F3B") to a binary file list (e.g
* {0x6f3a, 0x6f3b}).
*
* @param aText Text string to convert.
* @param aFileList Returned file list.
* @return Error code KErrNone if request completes ok.  Otherwise a relevant error
*         code.
*/
	{
 	aFileList.Zero();

	//
	// Go through the string and take upto 4 characters at a time.  Convert each set
	// from hexidecimal to an integer and then append them to the file list.
	//
	TInt  position(0);

	while (position < aText.Length())
		{
		TInt  numberLength(4);

		if (aText.Length() - position < 4)
			{
			numberLength = aText.Length() - position;
			}

		TLex8  token(aText.Mid(position, numberLength));
		TUint  value;
		TInt  ret;

		ret = token.Val(value, EHex);
		if (ret != KErrNone)
			{
			aFileList.Zero();
			return(ret);
			}

		aFileList.Append(value);
		position +=4;
		}

	return(KErrNone);
}

TInt CSimSat::ConvertTextToTBuf243(const TPtrC8& aText,
 									TBuf<243>& aFileList) const
/**
* Converts a text based file list (e.g. "6F3A6F3B") to a binary file list (e.g
* {0x6f3a, 0x6f3b}).
*
* @param aText Text string to convert.
* @param aFileList Returned file list.
* @return Error code KErrNone if request completes ok.  Otherwise a relevant error
*         code.
*/
	{
 	aFileList.Zero();

	//
	// Go through the string and take upto 4 characters at a time.  Convert each set
	// from hexidecimal to an integer and then append them to the file list.
	//
	TInt  position(0);

	while (position < aText.Length())
		{
		TInt  numberLength(4);

		if (aText.Length() - position < 4)
			{
			numberLength = aText.Length() - position;
			}

		TLex8  token(aText.Mid(position, numberLength));
		TUint  value;
		TInt  ret;

		ret = token.Val(value, EHex);
		if (ret != KErrNone)
			{
			aFileList.Zero();
			return(ret);
			}

		aFileList.Append(value);
		position +=4;
		}

	return(KErrNone);
}


TInt CSimSat::ConvertTextToTBuf254(const TPtrC8& aText,
 									TBuf<254>& aFileList) const
/**
* Converts a text based file list (e.g. "6F3A6F3B") to a binary file list (e.g
* {0x6f3a, 0x6f3b}).
*
* @param aText Text string to convert.
* @param aFileList Returned file list.
* @return Error code KErrNone if request completes ok.  Otherwise a relevant error
*         code.
*/
	{
 	aFileList.Zero();

	//
	// Go through the string and take upto 4 characters at a time.  Convert each set
	// from hexidecimal to an integer and then append them to the file list.
	//
	TInt  position(0);

	while (position < aText.Length())
		{
		TInt  numberLength(4);

		if (aText.Length() - position < 4)
			{
			numberLength = aText.Length() - position;
			}

		TLex8  token(aText.Mid(position, numberLength));
		TUint  value;
		TInt  ret;

		ret = token.Val(value, EHex);
		if (ret != KErrNone)
			{
			aFileList.Zero();
			return(ret);
			}

		aFileList.Append(value);
		position +=4;
		}

	return(KErrNone);
}


TInt CSimSat::ConvertTextToTBuf255(const TPtrC8& aText,
 									TBuf<255>& aFileList) const
/**
* Converts a text based file list (e.g. "6F3A6F3B") to a binary file list (e.g
* {0x6f3a, 0x6f3b}).
*
* @param aText Text string to convert.
* @param aFileList Returned file list.
* @return Error code KErrNone if request completes ok.  Otherwise a relevant error
*         code.
*/
	{
 	aFileList.Zero();

	//
	// Go through the string and take upto 4 characters at a time.  Convert each set
	// from hexidecimal to an integer and then append them to the file list.
	//
	TInt  position(0);

	while (position < aText.Length())
		{
		TInt  numberLength(4);

		if (aText.Length() - position < 4)
			{
			numberLength = aText.Length() - position;
			}

		TLex8  token(aText.Mid(position, numberLength));
		TUint  value;
		TInt  ret;

		ret = token.Val(value, EHex);
		if (ret != KErrNone)
			{
			aFileList.Zero();
			return(ret);
			}

		aFileList.Append(value);
		position +=4;
		}

	return(KErrNone);
}



TInt CSimSat::ConvertTextToTBuf4(const TPtrC8& aText,
 									TBuf8<4>& aFileList) const
/**
* Converts a text based file list (e.g. "6F3A6F3B") to a binary file list (e.g
* {0x6f3a, 0x6f3b}).
*
* @param aText Text string to convert.
* @param aFileList Returned file list.
* @return Error code KErrNone if request completes ok.  Otherwise a relevant error
*         code.
*/
	{
 	aFileList.Zero();

	//
	// Go through the string and take upto 4 characters at a time.  Convert each set
	// from hexidecimal to an integer and then append them to the file list.
	//
	TInt  position(0);

	while (position < aText.Length())
		{
		TInt  numberLength(4);

		if (aText.Length() - position < 4)
			{
			numberLength = aText.Length() - position;
			}

		TLex8  token(aText.Mid(position, numberLength));
		TUint  value;
		TInt  ret;

		ret = token.Val(value, EHex);
		if (ret != KErrNone)
			{
			aFileList.Zero();
			return(ret);
			}

		aFileList.Append(value);
		position +=4;
		}

	return(KErrNone);
	}

TInt CSimSat::ConvertTextToTBuf16(const TPtrC8& aText,
 									TBuf8<16>& aFileList) const
/**
* 
* @param aText Text string to convert.
* @param aFileList Returned file list.
* @return Error code KErrNone if request completes ok.  Otherwise a relevant error
*         code.
*/
	{
 	aFileList.Zero();

	//
	// Go through the string and take upto 4 characters at a time.  Convert each set
	// from hexidecimal to an integer and then append them to the file list.
	//
	TInt  position(0);

	while (position < aText.Length())
		{
		TInt  numberLength(4);

		if (aText.Length() - position < 4)
			{
			numberLength = aText.Length() - position;
			}

		TLex8  token(aText.Mid(position, numberLength));
		TUint  value;
		TInt  ret;

		ret = token.Val(value, EHex);
		if (ret != KErrNone)
			{
			aFileList.Zero();
			return(ret);
			}

		aFileList.Append(value);
		position +=4;
		}

	return(KErrNone);
	}


/* Function that generates time(in seconds) at random for the SimTSY 
 * Maximum of ten seconds; zero seconds also included just to simulate the 
 * synchronous call scenario 
 */

TInt CSimSat::randTime()
	{
	TInt ranT= Math::Random()%10; 
	return(ranT);
	}


TInt CSimSat::RetrieveMultimediaRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{

	RSat::TRetrieveMultimediaMessageRspV6Pckg *tMMRetRspV6Pckg = (RSat::TRetrieveMultimediaMessageRspV6Pckg*)aRsp;	
	RSat::TRetrieveMultimediaMessageRspV6& tMMRetRspV6 = (*tMMRetRspV6Pckg)();
	
	if(iRetTIndex == 0 || iRetTIndex ==3 || ((iRetTIndex >=5) && (iRetTIndex <=7)))
		{
		if((tMMRetRspV6.iGeneralResult != RSat::KSuccess)	||
	    (tMMRetRspV6.iInfoType != RSat::KNoAdditionalInfo))
	    	{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if(iRetTIndex ==1 || iRetTIndex == 2)
		{
		if((tMMRetRspV6.iGeneralResult != RSat::KMMSTemporaryProblem)	||
	    (tMMRetRspV6.iInfoType != RSat::KMeProblem) ||
	    (tMMRetRspV6.iAdditionalInfo.Compare(SIMTSY_MMMESSAGE_ADD_INFO_1) != 0))
	   		{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if(iRetTIndex ==4)
		{
		if((tMMRetRspV6.iGeneralResult != RSat::KMMSError)	||
	    (tMMRetRspV6.iInfoType != RSat::KMeProblem) ||
	    (tMMRetRspV6.iAdditionalInfo.Compare(SIMTSY_MMMESSAGE_ADD_INFO_1) != 0))
	  		{	
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}	
			
			
	iPendingTerRsp = EFalse;	
	return KErrNone;

	}

TInt CSimSat::SubmitMultimediaRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{

	RSat::TSubmitMultimediaMessageRspV6Pckg *tMMSubRspV6Pckg= (RSat::TSubmitMultimediaMessageRspV6Pckg*)aRsp;	
	RSat::TSubmitMultimediaMessageRspV6& tMMSubRspV6 = (*tMMSubRspV6Pckg)();
	
	if(iSubTIndex == 0 || iSubTIndex ==3 || iSubTIndex ==5 || iSubTIndex ==6)
		{
		if((tMMSubRspV6.iGeneralResult != RSat::KSuccess)	||
	    (tMMSubRspV6.iInfoType != RSat::KNoAdditionalInfo))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if(iSubTIndex ==1 || iSubTIndex ==2)
		{
		if((tMMSubRspV6.iGeneralResult != RSat::KMMSTemporaryProblem)	||
	    (tMMSubRspV6.iInfoType != RSat::KMeProblem) ||
	    (tMMSubRspV6.iAdditionalInfo.Compare(SIMTSY_MMMESSAGE_ADD_INFO_1) != 0))
	   		{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if(iSubTIndex ==4)
		{
		if((tMMSubRspV6.iGeneralResult != RSat::KMMSError)	||
	    (tMMSubRspV6.iInfoType != RSat::KMeProblem) ||
	    (tMMSubRspV6.iAdditionalInfo.Compare(SIMTSY_MMMESSAGE_ADD_INFO_1) != 0))
	  		{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else
		{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}	
		
	iPendingTerRsp = EFalse;	
	return KErrNone;

	}

TInt CSimSat::DisplayMultimediaRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{

	RSat::TDisplayMultimediaMessageRspV6Pckg *tMMDispRspV6Pckg= (RSat::TDisplayMultimediaMessageRspV6Pckg*)aRsp;	
	RSat::TDisplayMultimediaMessageRspV6& tMMDispRspV6 = (*tMMDispRspV6Pckg)();

	if(iDispTIndex == 0 || iDispTIndex == 1 || iDispTIndex == 6 ||
	 ((iDispTIndex >=8) && (iDispTIndex <=10)) || iDispTIndex == 12)
		{
		if((tMMDispRspV6.iGeneralResult != RSat::KSuccess)	||
	    (tMMDispRspV6.iInfoType != RSat::KNoAdditionalInfo))
	 		{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if (iDispTIndex ==2)
		{
		if((tMMDispRspV6.iGeneralResult != RSat::KPSessionTerminatedByUser)	||
	    (tMMDispRspV6.iInfoType != RSat::KNoAdditionalInfo))
	 		{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if (iDispTIndex ==3)
		{
		if((tMMDispRspV6.iGeneralResult != RSat::KBackwardModeRequestedByUser)	||
	    (tMMDispRspV6.iInfoType != RSat::KNoAdditionalInfo))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if (iDispTIndex ==4)
		{
		if((tMMDispRspV6.iGeneralResult != RSat::KNoResponseFromUser)	||
	    (tMMDispRspV6.iInfoType != RSat::KNoAdditionalInfo))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if (iDispTIndex ==5)
		{
		if((tMMDispRspV6.iGeneralResult != RSat::KMMSError)	||
	    (tMMDispRspV6.iInfoType != RSat::KMeProblem) ||
	    (tMMDispRspV6.iAdditionalInfo.Compare(SIMTSY_MMMESSAGE_ADD_INFO_1) != 0))
	   		{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if (iDispTIndex == 7 || iDispTIndex == 11 || iDispTIndex ==13)
		{
		if((tMMDispRspV6.iGeneralResult != RSat::KMeUnableToProcessCmd)	||
	    (tMMDispRspV6.iInfoType != RSat::KMeProblem) ||
	    (tMMDispRspV6.iAdditionalInfo.Compare(SIMTSY_MMMESSAGE_ADD_INFO_2) != 0))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}

	iPendingTerRsp = EFalse;	
	return KErrNone;
	
	}

TInt CSimSat::SetFramesRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{

	RSat::TSetFramesRspV6Pckg *tSetFramesV6Pckg= (RSat::TSetFramesRspV6Pckg*)aRsp;	
	RSat::TSetFramesRspV6& tSetFramesRspV6 = (*tSetFramesV6Pckg)();
	
	if(iSetFrmsIndex == 1)
		{
		if((tSetFramesRspV6.iGeneralResult != RSat::KSuccess)	||
		   (tSetFramesRspV6.iInfoType != RSat::KNoAdditionalInfo))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if (iSetFrmsIndex ==2)
		{
		if((tSetFramesRspV6.iGeneralResult != RSat::KFramesError) || 
			(tSetFramesRspV6.iInfoType != RSat::KMeProblem) ||
			(tSetFramesRspV6.iAdditionalInfo.Compare(SIMTSY_SET_FRMS_ADD_INFO_1) != 0))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else if (iSetFrmsIndex ==3)
		{
		if((tSetFramesRspV6.iGeneralResult != RSat::KFramesError) || 
			(tSetFramesRspV6.iInfoType != RSat::KMeProblem) ||
			(tSetFramesRspV6.iAdditionalInfo.Compare(SIMTSY_SET_FRMS_ADD_INFO_2) != 0))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	else
		{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	
	iPendingTerRsp = EFalse;
	return KErrNone;
	}

TInt CSimSat::ProvideLocalInfoRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	
	RSat::TLocalInfoRspV3Pckg *tLocalInfoV3Pckg = (RSat::TLocalInfoRspV3Pckg*)aRsp;
	RSat::TLocalInfoRspV3& tLocalInfoV3 = (*tLocalInfoV3Pckg)();
	
	if((iLocalInfoIndex==1 || iLocalInfoIndex ==4) &&
		((tLocalInfoV3.iGeneralResult != RSat::KSuccess) ||
		(tLocalInfoV3.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iLocalInfoIndex==2) && ((tLocalInfoV3.iGeneralResult != RSat::KMeUnableToProcessCmd) ||
		(tLocalInfoV3.iInfoType != RSat::KMeProblem) ||
		(tLocalInfoV3.iAdditionalInfo.Compare(SIMTSY_PRV_LCLINFO_ADD_INFO_1) != 0)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iLocalInfoIndex ==5) && ((tLocalInfoV3.iGeneralResult !=RSat::KSuccessLimitedService) ||
			(tLocalInfoV3.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iLocalInfoIndex == 6) && ((tLocalInfoV3.iGeneralResult !=RSat::KMeUnableToProcessCmd) ||
			(tLocalInfoV3.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);	
		}

	return KErrNone;
	}

TInt CSimSat::OpenChannelRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{

	RSat::TOpenChannelRspV2Pckg *tOpenChnRspPckg = (RSat::TOpenChannelRspV2Pckg*)aRsp;
	RSat::TOpenChannelRspV2 &tOpenChnRsp = (*tOpenChnRspPckg)();
	
	if((iOpenChnIndex == 1) && ((tOpenChnRsp.iGeneralResult != RSat::KSuccess) ||
		(tOpenChnRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iOpenChnIndex ==2) && ((tOpenChnRsp.iGeneralResult != RSat::KNetworkUnableToProcessCmd) ||
		(tOpenChnRsp.iInfoType != RSat::KMeProblem) || 
		(tOpenChnRsp.iAdditionalInfo.Compare(SIMTSY_MMMESSAGE_ADD_INFO_1) != 0)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iOpenChnIndex == 3) && ((tOpenChnRsp.iGeneralResult != RSat::KPerformedWithModifications) ||
		(tOpenChnRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);	
		}
	
	return KErrNone;
	}

TInt CSimSat::PlayToneRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	
	RSat::TPlayToneRspV1Pckg *tPlayToneRspPckg = (RSat::TPlayToneRspV1Pckg *)aRsp;
	RSat::TPlayToneRspV1& tPlayToneRsp = (*tPlayToneRspPckg)();
	
	if((iMiscCmdIndex == 1) && ((tPlayToneRsp.iGeneralResult != RSat::KPlayTonePerformedSuccessfully) ||
		(tPlayToneRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iMiscCmdIndex == 2) && ((tPlayToneRsp.iGeneralResult != RSat::KPSessionTerminatedByUser) ||
		(tPlayToneRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iMiscCmdIndex == 3) && ((tPlayToneRsp.iGeneralResult != RSat::KCmdBeyondMeCapabilities) ||
		(tPlayToneRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);	
		}
	return KErrNone;
	}

TInt CSimSat::SetupCallRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	
	RSat::TSetUpCallRspV2Pckg *tSetupCallRspPckg = (RSat::TSetUpCallRspV2Pckg*)aRsp;
	RSat::TSetUpCallRspV2& tSetupCallRsp = (*tSetupCallRspPckg)();
	
	if((iMiscCmdIndex == 5) && ((tSetupCallRsp.iGeneralResult != RSat::KSuccess) ||
		(tSetupCallRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iMiscCmdIndex == 6) && ((tSetupCallRsp.iGeneralResult != RSat::KCmdBeyondMeCapabilities) ||
		(tSetupCallRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iMiscCmdIndex == 7) && ((tSetupCallRsp.iGeneralResult != RSat::KMeUnableToProcessCmd) ||
		(tSetupCallRsp.iInfoType != RSat::KMeProblem) || 
		(tSetupCallRsp.iAdditionalInfo.Compare(SIMTSY_SETUPCALL_ADD_INFO_1) != 0)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iMiscCmdIndex == 8) && ((tSetupCallRsp.iGeneralResult != RSat::KNetworkUnableToProcessCmd) ||
		(tSetupCallRsp.iInfoType != RSat::KMeProblem) || 
		(tSetupCallRsp.iAdditionalInfo.Compare(SIMTSY_SETUPCALL_ADD_INFO_1) != 0)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);	
		}
	
	return KErrNone;
	}

TInt CSimSat::RefreshRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TRefreshRspV1Pckg *tRefRspPckg = (RSat::TRefreshRspV1Pckg*)aRsp;
	RSat::TRefreshRspV1& tRefRsp = (*tRefRspPckg)();
	
	if((iRefreshIndex ==1) && ((tRefRsp.iGeneralResult != RSat::KSuccess) ||
		(tRefRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iRefreshIndex ==2) && ((tRefRsp.iGeneralResult != RSat::KMeUnableToProcessCmd) ||
		(tRefRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);	
		}
	
	return KErrNone;
	}

TInt CSimSat::GetFramesStatusRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{

	RSat::TGetFramesStatusRspV6Pckg *tGetFramesStsV6Pckg= (RSat::TGetFramesStatusRspV6Pckg*)aRsp;	
	RSat::TGetFramesStatusRspV6& tGetFramesStsV6 = (*tGetFramesStsV6Pckg)();
	
	TInt iGetFrmsStsLocalIndex =0 ;
	
	if((iGetFrmsStsIndex == 1) && ((tGetFramesStsV6.iGeneralResult != RSat::KSuccess) || 
		(tGetFramesStsV6.iInfoType != RSat::KNoAdditionalInfo) ||
		(tGetFramesStsV6.iFramesInformation.iFrameId != iGetFrmsSts->At(iGetFrmsStsLocalIndex).iFrmId ) ||
		(tGetFramesStsV6.iFramesInformation.iFrameList.Compare(iGetFrmsSts->At(iGetFrmsStsLocalIndex).iFrmList))))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iGetFrmsStsIndex == 2) && ((tGetFramesStsV6.iGeneralResult != RSat::KFramesError) || 
		(tGetFramesStsV6.iInfoType != RSat::KNoFrameIdentified) ||
		(tGetFramesStsV6.iFramesInformation.iFrameId != iGetFrmsSts->At(iGetFrmsStsLocalIndex).iFrmId ) ||
		(tGetFramesStsV6.iFramesInformation.iFrameList.Compare(iGetFrmsSts->At(iGetFrmsStsLocalIndex).iFrmList))))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}

	iGetFrmsStsLocalIndex++;
	iSendRspEventDnld  = ETrue;
	return KErrNone;
	}

TInt CSimSat::SendUssdRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{

	RSat::TSendUssdRspV1Pckg *tSendUssdRspPckg= (RSat::TSendUssdRspV1Pckg*)aRsp;	
	RSat::TSendUssdRspV1& tSendUssdRsp = (*tSendUssdRspPckg)();
	
	if((iSendUssdIndex ==1) && ((tSendUssdRsp.iGeneralResult != RSat::KSuccess) || 
		(tSendUssdRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iSendUssdIndex == 2) && ((tSendUssdRsp.iGeneralResult != RSat::KMeUnableToProcessCmd) ||
		(tSendUssdRsp.iInfoType != RSat::KMeProblem) || 
		(tSendUssdRsp.iAdditionalInfo.Compare(SIMTSY_SEND_USSD_ADD_INFO) != 0)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if((iSendUssdIndex == 3) && ((tSendUssdRsp.iGeneralResult != RSat::KMeUnableToProcessCmd) ||
		(tSendUssdRsp.iInfoType != RSat::KMeProblem) || 
		(tSendUssdRsp.iAdditionalInfo.Compare(SIMTSY_SEND_USSD_ADD_INFO_1) != 0)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);	
		}

	return KErrNone;
	}

TInt CSimSat::LaunchBrowserRsp(const TTsyReqHandle aTsyReqHandle,TDes8* aRsp)
	{
	RSat::TLaunchBrowserRspV2Pckg* tLBRspPckg= (RSat::TLaunchBrowserRspV2Pckg*)aRsp;
	RSat::TLaunchBrowserRspV2& tLBRsp =(*tLBRspPckg)();
	
	if((iLnchBrwsrIndex == 1) && ((tLBRsp.iGeneralResult != (iLnchBrwsr->At(iLnchBrwsrIndex-1).iTerRsp)) || 
		(tLBRsp.iInfoType != RSat::KNoAdditionalInfo)))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	else if((iLnchBrwsrIndex == 2) && ((tLBRsp.iGeneralResult != (iLnchBrwsr->At(iLnchBrwsrIndex-1).iTerRsp)) || 
		(tLBRsp.iInfoType != RSat::KMeProblem) ||
		(tLBRsp.iAdditionalInfo.Compare(SIMTSY_LNCH_BRWSR_ADD_INFO1) != 0)))
		{	
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	else if((iLnchBrwsrIndex == 3) && ((tLBRsp.iGeneralResult != (iLnchBrwsr->At(iLnchBrwsrIndex-1).iTerRsp)) || 
		(tLBRsp.iInfoType != RSat::KMeProblem) ||
		(tLBRsp.iAdditionalInfo.Compare(SIMTSY_LNCH_BRWSR_ADD_INFO2) != 0)))
		{	
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	else if((iLnchBrwsrIndex == 4) && ((tLBRsp.iGeneralResult != (iLnchBrwsr->At(iLnchBrwsrIndex-1).iTerRsp)) || 
		(tLBRsp.iInfoType != RSat::KMeProblem) ||
		(tLBRsp.iAdditionalInfo.Compare(SIMTSY_LNCH_BRWSR_ADD_INFO3) != 0)))
		{	
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	else if((iLnchBrwsrIndex == 5) && ((tLBRsp.iGeneralResult != (iLnchBrwsr->At(iLnchBrwsrIndex-1).iTerRsp)) || 
		(tLBRsp.iInfoType != RSat::KMeProblem) ||
		(tLBRsp.iAdditionalInfo.Compare(SIMTSY_LNCH_BRWSR_ADD_INFO4) != 0)))
		{	
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	return KErrNone;
	}

TInt CSimSat::GetInkeyRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{

	RSat::TGetInkeyRspV2Pckg *tGetInkeyRspPckg= (RSat::TGetInkeyRspV2Pckg*)aRsp;	
	RSat::TGetInkeyRspV2& tGetInkeyRsp = (*tGetInkeyRspPckg)();
	
	if((tGetInkeyRsp.iGeneralResult != RSat::KSuccess) || 
		(tGetInkeyRsp.iInfoType != RSat::KNoAdditionalInfo))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
		
	return KErrNone;
	}

TInt CSimSat::GetMeSideSatProfile(const TTsyReqHandle aTsyReqHandle, TDes8* aMeSimSatProfile)
	{
	RSat::TSatProfileV6Pckg* profileV6Pckg = (RSat::TSatProfileV6Pckg*)aMeSimSatProfile;
	RSat::TSatProfileV6& profileV6 = (*profileV6Pckg)();
	
	profileV6.iSatProfileByte22 = RSat::KCapsRetrieveMultimediaMessage | RSat::KCapsSubmitMultimediaMessage;
	
	profileV6.iSatProfileByte23 = RSat::KCapsMmsNotificationDownload;
	
		
	if(iLocalInfoIndex !=1)
		{
		profileV6.iSatProfileByte18 = RSat::KCapsProvideLocalInfoIMEISV | 
											RSat::KCapsProvideLocalInfoSearchModeChange;
		if(iLocalInfoIndex == 3 )
			{
			profileV6.iSatProfileByte22 |= RSat::KCapsProvideLocalInfoUTRAN |
											RSat::KCapsProvideLocalInfoBatteryState;
			}
		else
			{
			profileV6.iSatProfileByte22 |= RSat::KCapsProvideLocalInfoBatteryState;
			}
			profileV6.iSatProfileByte23 |= RSat::KCapsProvideLocalInfoMEID;
		}
	profileV6.iSatProfileByte25 = RSat::KCapsMmsTransferStatusEvent;
	
	if(iMiscCmdIndex == 0 || iMiscCmdIndex == 5 || iMiscCmdIndex == 10)
		{
		profileV6.iSatProfileByte22 |= (RSat::KCapsPlayThemedAndMelodyTone 
				| RSat::KCapsSetUpCallMultimediaCall | RSat::KCapsRefreshGBA );
		}
		
	profileV6.iSatProfileByte6 = RSat::KCapsNetworkSearchModeChangeEvent;
	profileV6.iSatProfileByte25 |= 	(RSat::KCapsFrameParametersChangeEvent | 
									RSat::KCapsBrowsingStatusEvent);
									
	profileV6.iSatProfileByte4 = RSat::KCapsSendUSSD;	
	
	// The UICC is set to support the USSD Data Download by default to remove any dependencies on the test code
	// If clients find it useful to be able to set this capability on demand, this should be made updatable 
	// via the config file 
	profileV6.iSatProfileByte23 |= RSat::KCapsUssdDataDownload;
	
	if(iLnchBrwsrIndex != 1)
		{
		profileV6.iSatProfileByte21 = RSat::KCapsXHTML;
		}
	
	iGetMeSideProfileHandle = aTsyReqHandle;
	ReqCompleted(aTsyReqHandle,KErrNone);
	
	return KErrNone;
	}

TInt CSimSat::GetMeSideSatProfileCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if(aTsyReqHandle == iGetMeSideProfileHandle)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}

TInt CSimSat::ClientSatProfileIndication(const TTsyReqHandle aTsyReqHandle, TDes8* aClientSatProfile)
	{
	RSat::TSatProfileV6Pckg* aClientSatProfilePckg = (RSat::TSatProfileV6Pckg*)aClientSatProfile;
	RSat::TSatProfileV6& satProfV6 = (*aClientSatProfilePckg)();

	if(iRetTIndex ==6 || iRetTIndex ==7)	
		{
		if(!(satProfV6.iSatProfileByte22 & (RSat::KCapsRetrieveMultimediaMessage))
		|| !(satProfV6.iSatProfileByte25 & (RSat::KCapsMmsTransferStatusEvent)))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}

	else if(iSubTIndex == 5 || iSubTIndex ==6)
		{
		if(!(satProfV6.iSatProfileByte22 & (RSat::KCapsSubmitMultimediaMessage))
		|| !(satProfV6.iSatProfileByte25 & (RSat::KCapsMmsTransferStatusEvent)))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	
	else if(iLocalInfoIndex == 0)	
		{
		if(!(((satProfV6.iSatProfileByte18&RSat::KCapsProvideLocalInfoIMEISV)&&
			(satProfV6.iSatProfileByte18&RSat::KCapsProvideLocalInfoSearchModeChange)) ||
			(satProfV6.iSatProfileByte22&RSat::KCapsProvideLocalInfoBatteryState) ||
			(satProfV6.iSatProfileByte23&RSat::KCapsProvideLocalInfoMEID)))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	else if(iLocalInfoIndex == 1)
		{
		if(((satProfV6.iSatProfileByte18&RSat::KCapsProvideLocalInfoIMEISV)&&
			(satProfV6.iSatProfileByte18&RSat::KCapsProvideLocalInfoSearchModeChange)) ||
			(satProfV6.iSatProfileByte22&RSat::KCapsProvideLocalInfoBatteryState) ||
			(satProfV6.iSatProfileByte23&RSat::KCapsProvideLocalInfoMEID))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}	
		}
	else if(iLocalInfoIndex == 3)
		{
		if(!(((satProfV6.iSatProfileByte18&RSat::KCapsProvideLocalInfoIMEISV)&&
			(satProfV6.iSatProfileByte18&RSat::KCapsProvideLocalInfoSearchModeChange)) ||
			((satProfV6.iSatProfileByte22&RSat::KCapsProvideLocalInfoUTRAN)&&
			(satProfV6.iSatProfileByte22&RSat::KCapsProvideLocalInfoBatteryState)) ||
			(satProfV6.iSatProfileByte23&RSat::KCapsProvideLocalInfoMEID)))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	else if(iMiscCmdIndex == 0 || iMiscCmdIndex == 5 || iMiscCmdIndex == 10) 
		{
		if(!(satProfV6.iSatProfileByte22 & RSat::KCapsPlayThemedAndMelodyTone) ||
		!(satProfV6.iSatProfileByte22 & RSat::KCapsSetUpCallMultimediaCall) || 
		!(satProfV6.iSatProfileByte22 & RSat::KCapsRefreshGBA) )
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	else if(iEventDnldIndex == 0)
		{
		if(!(satProfV6.iSatProfileByte6 & (RSat::KCapsNetworkSearchModeChangeEvent)))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	else if(iLnchBrwsrIndex == 1)
		{
		if(!(satProfV6.iSatProfileByte21 & (RSat::KCapsXHTML)))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	else 
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	
	return KErrNone;
	}

TInt CSimSat::MmsTransferStatus(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	RSat::TMmsTransferStatusV6Pckg* mMSTransferStatusV6PCmdPckg = (RSat::TMmsTransferStatusV6Pckg*)aPCmd;	
	RSat::TMmsTransferStatusV6& mMSTransferStatusV6 = (*mMSTransferStatusV6PCmdPckg)();	
	
	if(iRetTIndex ==6 || iRetTIndex ==7)	
		{
		if((mMSTransferStatusV6.iDeviceId != RSat::KME)
	 	||(mMSTransferStatusV6.iMMSTransferFile.Compare(SIMTSY_MMS_TRF_FILE_PATH) !=0)
	 	||(mMSTransferStatusV6.iMultimediaMessageStatus.Compare(SIMTSY_MMS_MSG_BUF) !=0)
	 	||(mMSTransferStatusV6.iMultimediaMessageId.Compare(SIMTSY_MMS_MSG_ID) != 0))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}

	else if(iSubTIndex == 5 || iSubTIndex ==6)
		{
		if((mMSTransferStatusV6.iDeviceId != RSat::KME)
	 	||(mMSTransferStatusV6.iMMSTransferFile.Compare(SIMTSY_MMS_TRF_FILE_PATH) !=0)
	 	||(mMSTransferStatusV6.iMultimediaMessageStatus.Compare(SIMTSY_MMS_MSG_BUF) !=0)
	 	||(mMSTransferStatusV6.iMultimediaMessageId.Compare(SIMTSY_MMS_MSG_ID) != 0))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
		
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	
	iMMSTransferStatusHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSimSat::MmsTransferStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{

	if(aTsyReqHandle == iMMSTransferStatusHandle)	
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	return KErrNone;
	
	}

TInt CSimSat::MmsNotificationDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aDes)
	{
	// Has the config file been parsed for this functionality 
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_MMSNOTIFICATIONDOWNLOAD_CMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}
		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}
		
	TInt err(KErrNone);	
	TInt count(iMmsDownloadData->Count());
	TBool found(EFalse);
			
	// If the config file does not contain any USSD Download related data
	// then complete the request with KErrNotSupported
	if(!count)
		{
		err = KErrNotSupported;
		}
	else 
		{
		// otherwise extract the data supplied by the client 
		RSat::TMmsNotificationDownloadV6Pckg* mmsNotificationDownloadV6Pckg = (RSat::TMmsNotificationDownloadV6Pckg*)aDes;	
		RSat::TMmsNotificationDownloadV6& mmsNotificationDownloadV6 = (*mmsNotificationDownloadV6Pckg)();	
		
		for(TInt i=0; i<count; i++)
			{
			TMmsNotificationDownload tempMmsData =  iMmsDownloadData->At(i);
			
			// Compare the supplied data to the data retrieved from the config file 
			if((mmsNotificationDownloadV6.iDeviceId == STATIC_CAST(RSat::TDeviceId, tempMmsData.iDestn)) &&
			   (mmsNotificationDownloadV6.iMMSNotification.Compare(tempMmsData.iMMSNotfn) == 0) && 
			   (mmsNotificationDownloadV6.iLastEnvelope == tempMmsData.iLastEnv)) 
				{
				found = ETrue;
				
				if(tempMmsData.iUICCRsp == SIMTSY_UICC_SUCC_RSP)
					{
					err = KErrNone;
					}
				else if(tempMmsData.iUICCRsp == SIMTSY_UICC_RETRY_RSP)
					{
					err = KErrNotReady;
					}
				else if(tempMmsData.iUICCRsp == SIMTSY_UICC_FLR_RSP_1 || 
					    tempMmsData.iUICCRsp == SIMTSY_UICC_FLR_RSP_2)
					{
					err = KErrNotSupported;
					}
				
				iMmsDownloadDataHandle = aTsyReqHandle; 			
				break;
				}
			} // end for loop  
		} // end else
	
	// the supplied data does not match the one in the config file, so complete the request with an error
	if(!found)
		err = KErrCorrupt;
		
	ReqCompleted(aTsyReqHandle, err);
	return KErrNone;
	}

TInt CSimSat::MmsNotificationDownloadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if(aTsyReqHandle == iMmsDownloadDataHandle)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	return KErrNone;
	
	}

TInt CSimSat::EventDownload(const TTsyReqHandle aTsyReqHandle, RSat::TEventList* aPCmd, TDes8* aRsp)
	{
	
	if(!iConfigDone)
		{
		TInt ret = KErrNone;
		TRAPD(err, ret = ConfigL(SIMTSY_EVENT_LIST_CMD_NUMBER));
		if(err != KErrNone)
			{
			ret = err;
			}

		if(ret!= KErrNone)
			{
			return ret;
			}
		else
			{
			iConfigDone = ETrue;
			}
		}

	switch(*aPCmd)
		{
		case RSat::	KNetworkSearchModeChange:
			{
			RSat::TNetworkSearchModeChangeEventV6Pckg* networkSrchPckg = (RSat::TNetworkSearchModeChangeEventV6Pckg*)aRsp;	
			RSat::TNetworkSearchModeChangeEventV6& networkSrch = (*networkSrchPckg)();	
	
			iEventDnldHandle = aTsyReqHandle;
	
			if(iEventDnldIndex == 0 || iEventDnldIndex == 1 )
				{
				if(networkSrch.iNetworkSearchMode == (STATIC_CAST(RSat::TNetworkSearchMode,iEventList->At(iEventDnldIndex).iVar)))
					{
					iSendRspEventDnld  = ETrue;
					}
				else
					{
					iSendRspEventDnld  = EFalse;
					}
				}
			}
		break;
		
		case RSat::KFramesInformationChange:
			{
			RSat::TFramesInformationChangedEventV6Pckg* frmsChgPckg = (RSat::TFramesInformationChangedEventV6Pckg*)aRsp;
			RSat::TFramesInformationChangedEventV6& frmsChg = (*frmsChgPckg)();
			
			iEventDnldHandle = aTsyReqHandle;
			
			if(iEventDnldIndex == 4 || iEventDnldIndex == 5 )
				{
				if((frmsChg.iFramesInformation.iFrameId == (iEventList->At(iEventDnldIndex).iVar)) || 
				(!(frmsChg.iFramesInformation.iFrameList.Compare(iEventList->At(iEventDnldIndex).iFrameList))))
					{
					iSendRspEventDnld  = ETrue;
					}
				else
					{
					iSendRspEventDnld  = EFalse;
					}
				}
			}
		break;
		
		case RSat::KBrowsingStatusChange:
			{
			RSat::TBrowsingStatusEventV6Pckg *brwStsPckg = (RSat::TBrowsingStatusEventV6Pckg*)aRsp;
			RSat::TBrowsingStatusEventV6& brwSts = (*brwStsPckg)();
				
			iEventDnldHandle = aTsyReqHandle;
			
			if(iEventDnldIndex == 2 || iEventDnldIndex == 3 )
				{
				if(brwSts.iBrowserStatus == (iEventList->At(iEventDnldIndex).iVar)) 
					{
					iSendRspEventDnld  = ETrue;
					}
				else
					{
					iSendRspEventDnld  = EFalse;
					}
				}
			else if(iEventDnldIndex == 0)
				{
				iSendRspEventDnld  = ETrue;
				}
			}
		break;
		
		default:
			iSendRspEventDnld  = EFalse;
		break;
		}

	
	if(iEventDnldIndex < iEventList->Count())
		{
		iEventDnldIndex++;
		}
	
	return KErrNone;

	}

TInt CSimSat::EventDownloadCancel(const TTsyReqHandle aTsyReqHandle)
	{
		
	if(aTsyReqHandle == iEventDnldHandle)
		{
		ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	return KErrNone;
	}


TInt CSimSat::ConfigL(unsigned int aCmd)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_1, "CSimSat: Entered ConfigL()");

	TInt dcs,terRsp,src,UICCRsp,lastEnv,duration,type,frameId,frmLayout,defFrmLayout,frameSeparator;
	TInt immRsp,dispPrio,clrScr,utranQlfr,infoType,brType,location,destn,alphaIDSts;
	TInt iconid,iconidqlfr,textStatus,ret=KErrNone;
	TInt i;			

	TPtrC8 ussdStr,mmsNotfn,subFile,framesList,FileList,frmLayoutBuf;
	TPtrC8 dispFile,alphaIDBuf,msgRef,rcpFile,conId,msgId,txtAttr;
	
	const CTestConfigItem* item=NULL;
	

	TMmsNotificationDownload tMmsNotificationDownload;
	TSendUSSD tSendSS;
	TEventDnld tEventDnld;
	TUssdData tUssdData;
	TSendUSSD tSendUssd;
	TLnchBrwsr tLnchBrwsr;
	TMMRetrieve tMMRetrieve;
	TMiscPCmd tMiscCmd;
	TMMSubmit tMMSubmit;
	TSetFrms tSetFrms;
	TMMDisplay tMMDisplay;
	TGetFrmsSts tGetFrmsSts;
	TLocalInfo tLocalInfo;
	TOpenChannel tOpenChn;
	
	TInt lnchBCount =CfgFile()->ItemCount(KLaunchBrowser);
	TInt ussdCount	=CfgFile()->ItemCount(KSendUssd);
	TInt refCount	=CfgFile()->ItemCount(KRefresh2);	
	TInt miscCount	=CfgFile()->ItemCount(KMiscCmd);
	TInt openCount	=CfgFile()->ItemCount(KOpenChnl);
	TInt localCount	=CfgFile()->ItemCount(KLocalInfo);
	TInt getFSCount	=CfgFile()->ItemCount(KGetFramesStatus);
	TInt setFCount	=CfgFile()->ItemCount(KSetFrames);
	TInt dispCount	=CfgFile()->ItemCount(KMMDisplay);
	TInt subCount	=CfgFile()->ItemCount(KMMSubmit);
	TInt retrCount	=CfgFile()->ItemCount(KMMRetrieve);
	TInt notfnDCount=CfgFile()->ItemCount(KMmsNotificationDownload);
	TInt ussdDnCount=CfgFile()->ItemCount(KUssdDataDownload);
	TInt eventCount	=CfgFile()->ItemCount(KEventDnld);		
	TInt ssCount	=CfgFile()->ItemCount(KSendSs);
	TInt efCount	=CfgFile()->ItemCount(KElemFiles);				
		
	switch(aCmd)
		{
		case SIMTSY_RET_MM_PCMD_NUMBER:
			/* Retrieving the information related to the RETRIEVE MM command */	
		
			iMMRetrieve = new(ELeave) CArrayFixFlat<TMMRetrieve>(KSatGranularity);
			for (i=0;i<retrCount;i++)
				{
			
				item=CfgFile()->Item(KMMRetrieve,i);
				if(!item)
					{
					break;
					}

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,destn);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element DESTINATION returned %d (element no. %d) from tag %s.",ret,0,KMMRetrieve);
					continue;
					}
				tMMRetrieve.iDestn = destn;

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,alphaIDSts);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element ALPHAIDSTATUS returned %d (element no. %d) from tag %s.",ret,1,KMMRetrieve);
					continue;
					}
				tMMRetrieve.iAlphaStatus=alphaIDSts;

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,alphaIDBuf);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element ALPHAIDBUF returned %d (element no. %d) from tag %s.",ret,2,KMMRetrieve);
					continue;
					}
				location = alphaIDBuf.Locate('\n');
				if(location > 0)
					{
					tMMRetrieve.iAlphaIDBuf.Set(alphaIDBuf.Left(location));
					}
				else
					{
					tMMRetrieve.iAlphaIDBuf.Set(alphaIDBuf);
					}

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,iconid);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element ICONID IDENTIFIER returned %d (element no. %d) from tag %s.",ret,3,KMMRetrieve);
					continue;
					}
				tMMRetrieve.iIconID = iconid;
			
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,iconidqlfr);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element ICONID QUALIFIER returned %d (element no. %d) from tag %s.",ret,4,KMMRetrieve);
					continue;
					}
				tMMRetrieve.iIconIDBuf = iconidqlfr;

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,msgRef);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element MESSAGE REFERENCE returned %d (element no. %d) from tag %s.",ret,5,KMMRetrieve);
					continue;
					}
				location = msgRef.Locate('\n');
				if(location > 0)
					{
					tMMRetrieve.iMMMsgRef.Set(msgRef.Left(location));
					}
				else
					{
					tMMRetrieve.iMMMsgRef.Set(msgRef);
					}

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,rcpFile);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_8, "WARNING - CONFIGURATION FILE PARSING - Reading element RECEPTION FILE returned %d (element no. %d) from tag %s.",ret,6,KMMRetrieve);
					continue;
					}
				location = rcpFile.Locate('\n');
				if(location > 0)
					{
					tMMRetrieve.iRcptnFile.Set(rcpFile.Left(location));
					}
				else
					{
					tMMRetrieve.iRcptnFile.Set(rcpFile);
					}

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,conId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_9, "WARNING - CONFIGURATION FILE PARSING - Reading element CONTENT IDENTIFIER returned %d (element no. %d) from tag %s.",ret,7,KMMRetrieve);
					continue;
					}
				location = conId.Locate('\n');
				if(location > 0)
					{		
					tMMRetrieve.iContentId.Set(conId.Left(location));
					}
				else
					{
					tMMRetrieve.iContentId.Set(conId);
					}

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,msgId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_10, "WARNING - CONFIGURATION FILE PARSING - Reading element MESSAGE IDENTIFIER returned %d (element no. %d) from tag %s.",ret,8,KMMRetrieve);
					continue;
					}
				location = msgId.Locate('\n');
				if(location > 0)
					{
					tMMRetrieve.iMsgId.Set(msgId.Left(location));
					}
				else
					{
					tMMRetrieve.iMsgId.Set(msgId);
					}
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,9,textStatus);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_11, "WARNING - CONFIGURATION FILE PARSING - Reading element TEXT ATTRIBUTE STATUS returned %d (element no. %d) from tag %s.",ret,9,KMMRetrieve);
					continue;
					}
				tMMRetrieve.iTextAttStatus = textStatus;

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,10,txtAttr);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_12, "WARNING - CONFIGURATION FILE PARSING - Reading element TEXT ATTRIBUTE returned %d (element no. %d) from tag %s.",ret,10,KMMRetrieve);
					continue;
					}
				location = txtAttr.Locate('\n');
				if(location > 0)
					{
					tMMRetrieve.iTextAttBuf.Set(txtAttr.Left(location));
					}
				else
					{
					tMMRetrieve.iTextAttBuf.Set(txtAttr);
					}

				iMMRetrieve->AppendL(tMMRetrieve);		
			
				}
			break;
		case SIMTSY_SUB_MM_PCMD_NUMBER:
			/* Retrieving the information related to the SUBMIT MULTIMEDIA command */	
		
			iMMSubmit   = new(ELeave) CArrayFixFlat<TMMSubmit>(KSatGranularity);
			for(i=0;i<subCount;i++)	
				{
				item=CfgFile()->Item(KMMSubmit,i);
				if(!item)
					{
					break;
					}
							
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,destn);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_13, "WARNING - CONFIGURATION FILE PARSING - Reading element DESTINATION returned %d (element no. %d) from tag %s.",ret,0,KMMSubmit);
					continue;
					}
				tMMSubmit.iDestn = destn;

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,alphaIDSts);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_14, "WARNING - CONFIGURATION FILE PARSING - Reading element ALPHAIDSTATUS returned %d (element no. %d) from tag %s.",ret,1,KMMSubmit);
					continue;
					}
				tMMSubmit.iAlphaStatus=alphaIDSts;

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,alphaIDBuf);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_15, "WARNING - CONFIGURATION FILE PARSING - Reading element ALPHAIDBUF returned %d (element no. %d) from tag %s.",ret,2,KMMSubmit);
					continue;
					}
				location = alphaIDBuf.Locate('\n');
				if(location > 0)
					tMMSubmit.iAlphaIDBuf.Set(alphaIDBuf.Left(location));
				else
					tMMSubmit.iAlphaIDBuf.Set(alphaIDBuf);

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,iconid);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_16, "WARNING - CONFIGURATION FILE PARSING - Reading element ICONID IDENTIFIER returned %d (element no. %d) from tag %s.",ret,3,KMMSubmit);
					continue;
					}
				tMMSubmit.iIconID = iconid;
			
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,iconidqlfr);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_17, "WARNING - CONFIGURATION FILE PARSING - Reading element ICONID QUALIFIER returned %d (element no. %d) from tag %s.",ret,4,KMMSubmit);
					continue;
					}
				tMMSubmit.iIconIDBuf = iconidqlfr;
				
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,subFile);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_18, "WARNING - CONFIGURATION FILE PARSING - Reading element SUBMISSION FILE returned %d (element no. %d) from tag %s.",ret,5,KMMSubmit);
					continue;
					}
				location = subFile.Locate('\n');
				if(location > 0)
					tMMSubmit.iSbmsnFile.Set(subFile.Left(location));
				else
					tMMSubmit.iSbmsnFile.Set(subFile);
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,msgId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_19, "WARNING - CONFIGURATION FILE PARSING - Reading element MESSAGE IDENTIFIER returned %d (element no. %d) from tag %s.",ret,6,KMMSubmit);
					continue;
					}
				location = msgId.Locate('\n');
				if(location > 0)
					tMMSubmit.iMsgId.Set(msgId.Left(location));
				else
					tMMSubmit.iMsgId.Set(msgId);
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,textStatus);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_20, "WARNING - CONFIGURATION FILE PARSING - Reading element TEXT ATTRIBUTE STATUS returned %d (element no. %d) from tag %s.",ret,7,KMMSubmit);
					continue;
					}
				tMMSubmit.iTextAttStatus = textStatus;

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,8,txtAttr);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_21, "WARNING - CONFIGURATION FILE PARSING - Reading element TEXT ATTRIBUTE returned %d (element no. %d) from tag %s.",ret,8,KMMSubmit);
					continue;
					}
				location = txtAttr.Locate('\n');
				if(location > 0)
					tMMSubmit.iTextAttBuf.Set(txtAttr.Left(location));
				else
					tMMSubmit.iTextAttBuf.Set(txtAttr);

				iMMSubmit->AppendL(tMMSubmit);		
			
				}

			break;
		case SIMTSY_DISP_MM_PCMD_NUMBER:
			/* Retrieving the information related to the DISPLAY MULTIMEDIA command */	
		
			iMMDisplay  = new(ELeave) CArrayFixFlat<TMMDisplay>(KSatGranularity);
			for(i=0;i<dispCount;i++)
				{
				item=CfgFile()->Item(KMMDisplay,i);
				if(!item)
					{
					break;
					}
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,destn);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_22, "WARNING - CONFIGURATION FILE PARSING - Reading element DESTINATION returned %d (element no. %d) from tag %s.",ret,0,KMMDisplay);
					continue;
					}
				tMMDisplay.iDestn = destn;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,dispPrio);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_23, "WARNING - CONFIGURATION FILE PARSING - Reading element DISPLAY PRIORITY returned %d (element no. %d) from tag %s.",ret,1,KMMDisplay);
					continue;
					}
				tMMDisplay.iDispPri = dispPrio;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,clrScr);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_24, "WARNING - CONFIGURATION FILE PARSING - Reading element CLEARSCREEN returned %d (element no. %d) from tag %s.",ret,2,KMMDisplay);
					continue;
					}
				tMMDisplay.iClrScr = clrScr;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,dispFile);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_25, "WARNING - CONFIGURATION FILE PARSING - Reading element SUBMISSION FILE returned %d (element no. %d) from tag %s.",ret,3,KMMDisplay);
					continue;
					}
				location = dispFile.Locate('\n');
				if(location > 0)
					tMMDisplay.iSbmsnFile.Set(dispFile.Left(location));
				else
					tMMDisplay.iSbmsnFile.Set(dispFile);
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,msgId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_26, "WARNING - CONFIGURATION FILE PARSING - Reading element MESSAGE IDENTIFIER returned %d (element no. %d) from tag %s.",ret,4,KMMSubmit);
					continue;
					}
				location = msgId.Locate('\n');
				if(location > 0)
					tMMDisplay.iMsgId.Set(msgId.Left(location));
				else
					tMMDisplay.iMsgId.Set(msgId);
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,immRsp);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_27, "WARNING - CONFIGURATION FILE PARSING - Reading element IMMEDIATE RESPONSE returned %d (element no. %d) from tag %s.",ret,5,KMMDisplay);
					continue;
					}
				tMMDisplay.iImmRsp = immRsp;
					
				iMMDisplay->AppendL(tMMDisplay);		
			
				}
		
			break;
		case SIMTSY_SET_FRMS_PCMD_NUMBER:
			/* Retrieving the information related to the SET FRAMES command */
		
			iSetFrms    = new(ELeave) CArrayFixFlat<TSetFrms>(KSatGranularity);
			for(i=0;i<setFCount;i++)
				{
				item=CfgFile()->Item(KSetFrames,i);
				if(!item)
					{
					break;
					}
			
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,destn);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_28, "WARNING - CONFIGURATION FILE PARSING - Reading element DESTINATION returned %d (element no. %d) from tag %s.",ret,0,KSetFrames);
					continue;
					}
				tSetFrms.iDestn = destn;
						
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,frameId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_29, "WARNING - CONFIGURATION FILE PARSING - Reading element FRAME IDENTIFIER returned %d (element no. %d) from tag %s.",ret,1,KSetFrames);
					}
				tSetFrms.iFrmId = frameId;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,frmLayout);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_30, "WARNING - CONFIGURATION FILE PARSING - Reading element FRAME LAYOUT returned %d (element no. %d) from tag %s.",ret,2,KSetFrames);
					}
				tSetFrms.iFrmLayout = frmLayout;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,frmLayoutBuf);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_31, "WARNING - CONFIGURATION FILE PARSING - Reading element FRAME LAYOUT BUFFER returned %d (element no. %d) from tag %s.",ret,3,KSetFrames);
					}
				location = frmLayoutBuf.Locate('\n');
				if(location > 0)
					tSetFrms.iFrmLayoutBuf.Set(frmLayoutBuf.Left(location));
				else
					tSetFrms.iFrmLayoutBuf.Set(frmLayoutBuf);
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,defFrmLayout);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_32, "WARNING - CONFIGURATION FILE PARSING - Reading element DEFAULT FRAME LAYOUT returned %d (element no. %d) from tag %s.",ret,4,KSetFrames);
					}
				tSetFrms.iDefFrmId = defFrmLayout;
						
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,frameSeparator);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_33, "WARNING - CONFIGURATION FILE PARSING - Reading element FRAME SEPARATOR returned %d (element no. %d) from tag %s.",ret,5,KSetFrames);
					}
				tSetFrms.iFrmSeparator = frameSeparator;
				
				iSetFrms->AppendL(tSetFrms);
				
				}
			break;
		case SIMTSY_GET_FRMS_STS_PCMD_NUMBER:
			/* Retrieving the information related to the GET FRAMES STATUS command */
		
			iGetFrmsSts = new(ELeave) CArrayFixFlat<TGetFrmsSts>(KSatGranularity);
			for(i=0;i<getFSCount;i++)
				{
				item=CfgFile()->Item(KGetFramesStatus,i);
				if(!item)
					{
					break;
					}

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,destn);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_34, "WARNING - CONFIGURATION FILE PARSING - Reading element DESTINATION returned %d (element no. %d) from tag %s.",ret,0,KGetFramesStatus);
					continue;
					}
				tGetFrmsSts.iDestn = destn;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,frameId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_35, "WARNING - CONFIGURATION FILE PARSING - Reading element FRAME ID returned %d (element no. %d) from tag %s.",ret,1,KGetFramesStatus);
					continue;
					}
				tGetFrmsSts.iFrmId = frameId;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,frmLayoutBuf);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_36, "WARNING - CONFIGURATION FILE PARSING - Reading element FRAMELAYOUT BUFFER returned %d (element no. %d) from tag %s.",ret,1,KGetFramesStatus);
					continue;
					}
				location = frmLayoutBuf.Locate('\n');
				if(location > 0)
					tGetFrmsSts.iFrmList.Set(frmLayoutBuf.Left(location));
				else
					tGetFrmsSts.iFrmList.Set(frmLayoutBuf);
				
				iGetFrmsSts->AppendL(tGetFrmsSts);
				
				}
			break;
		case SIMTSY_PRV_LCL_INFO_PCMD_NUMBER:
			/* Retrieving the information regarding PROVIDE LOCAL INFORMATION */
		
			iLocalInfo  = new(ELeave) CArrayFixFlat<TLocalInfo>(KSatGranularity);
			for(i=0;i<localCount;i++)
				{
				item=CfgFile()->Item(KLocalInfo,i);
				if(!item)
					{
					break;
					}
					
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,destn);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_37, "WARNING - CONFIGURATION FILE PARSING - Reading element DESTINATION returned %d (element no. %d) from tag %s.",ret,0,KLocalInfo);
					continue;
					}
				tLocalInfo.iDeviceId = destn;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,infoType);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_38, "WARNING - CONFIGURATION FILE PARSING - Reading element INFORMATION TYPE returned %d (element no. %d) from tag %s.",ret,1,KLocalInfo);
					continue;
					}
				tLocalInfo.iLocalInfoType = infoType;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,utranQlfr);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_39, "WARNING - CONFIGURATION FILE PARSING - Reading element UTRAN QLFR returned %d (element no. %d) from tag %s.",ret,2,KLocalInfo);
					continue;
					}
				tLocalInfo.iUTRANQlfr = utranQlfr;
				
				iLocalInfo->AppendL(tLocalInfo);
				
				}
			break;
		case SIMTSY_OPEN_CHAN_PCMD_NUMBER:
			/* Retrieving the information regarding OPEN CHANNEL command */
		
			iOpenChn    = new(ELeave) CArrayFixFlat<TOpenChannel>(KSatGranularity);		
			for(i=0;i<openCount;i++)
				{
				item=CfgFile()->Item(KOpenChnl,i);
				if(!item)
					{
					break;
					}
					
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,brType);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_40, "WARNING - CONFIGURATION FILE PARSING - Reading element DESTINATION returned %d (element no. %d) from tag %s.",ret,0,KOpenChnl);
					continue;
					}
				tOpenChn.iBearerType = brType;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,frameId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_41, "WARNING - CONFIGURATION FILE PARSING - Reading element INFORMATION TYPE returned %d (element no. %d) from tag %s.",ret,1,KOpenChnl);
					continue;
					}
				tOpenChn.iFrameId = frameId;
				
				iOpenChn->AppendL(tOpenChn);
				
				}
			break;
		case SIMTSY_PLAY_TONE_PCMD_NUMBER:
		case SIMTSY_SETUP_CALL_PCMD_NUMBER:
		case SIMTSY_GET_INKEY_PCMD_NUMBER:
			/* Retrieving the information about the miscellaneous commands */
		
			iMiscCmd    = new(ELeave) CArrayFixFlat<TMiscPCmd>(KSatGranularity);
			for(i=0;i<miscCount;i++)
				{
				item=CfgFile()->Item(KMiscCmd,i);
				if(!item)
					{
					break;
					}
					
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,frameId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_42, "WARNING - CONFIGURATION FILE PARSING - Reading element FRAMEID returned %d (element no. %d) from tag %s.",ret,0,KMiscCmd);
					continue;
					}
				tMiscCmd.iFrameId = frameId;
				
					iMiscCmd->AppendL(tMiscCmd);
				
				}
			break;
		case SIMTSY_REFRESH_PCMD_NUMBER:
			/* Retrieving the information about the REFRESH command */
		
			iRefresh    = new(ELeave) CArrayFixFlat<TSatInfo>(KSatGranularity);
			for(i=0;i<refCount;i++)
				{
				item=CfgFile()->Item(KRefresh2,i);
				if(!item)
					{
					break;
					}

				TInt duration,type;
				TPtrC8 applId;

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_43, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KRefresh2);
					continue;
					}
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,type);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_44, "WARNING - CONFIGURATION FILE PARSING - Reading element REFRESH TYPE returned %d (element no. %d) from tag %s.",ret,1,KRefresh2);
					continue;
					}
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,applId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_45, "WARNING - CONFIGURATION FILE PARSING - Reading element APPLICATION ID returned %d (element no. %d) from tag %s.",ret,2,KRefresh2);
					continue;
					}

				TSatInfo satInfo;
				satInfo.iDuration = duration;
				satInfo.iType = type;
				
				TInt location = applId.Locate('\n');
				if(location > 0)
					satInfo.iFileList.Set(applId.Left(location));
				else
					satInfo.iFileList.Set(applId);
				iRefresh->AppendL(satInfo);
				
				}

			break;
		case SIMTSY_SEND_USSD_PCMD_NUMBER:	
			/* Retrieving the information about the SEND USSD command */
		
			iSendUssd   = new(ELeave) CArrayFixFlat<TSendUSSD>(KSatGranularity);
			for(i=0;i<ussdCount;i++)
				{
				item=CfgFile()->Item(KSendUssd,i);
				if(!item)
					{
					break;
					}

				ret= CTestConfig::GetElement(item->Value(),KStdDelimiter,0,textStatus);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_46, "WARNING - CONFIGURATION FILE PARSING - Reading element TEXT ATTRIBUTE STATUS returned %d (element no. %d) from tag %s.",ret,0,KSendUssd);
					continue;
					}
				tSendUssd.iTextAttStatus = textStatus;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,txtAttr);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_47, "WARNING - CONFIGURATION FILE PARSING - Reading element TEXT ATTRIBUTE BUFFER returned %d (element no. %d) from tag %s.",ret,1,KSendUssd);
					continue;
					}
				TInt location = txtAttr.Locate('\n');
				if(location > 0)
					tSendUssd.iTextAttBuf.Set(txtAttr.Left(location));
				else
					tSendUssd.iTextAttBuf.Set(txtAttr);
				
				iSendUssd->AppendL(tSendUssd);
				
				}
			break;
		case SIMTSY_LNCH_BRWSR_PCMD_NUMBER:
			/*	Retrieving the information related to the LAUNCH BROWSER command */
		
			iLnchBrwsr  = new(ELeave) CArrayFixFlat<TLnchBrwsr>(KSatGranularity);
			for(i=0;i<lnchBCount;i++)
				{
				item=CfgFile()->Item(KLaunchBrowser,i);
				if(!item)
					{
					break;
					}
							
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,frameId);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_48, "WARNING - CONFIGURATION FILE PARSING - Reading element FRAMEID returned %d (element no. %d) from tag %s.",ret,0,KLaunchBrowser);
					continue;
					}
				tLnchBrwsr.iFrameId = frameId;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,terRsp);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_49, "WARNING - CONFIGURATION FILE PARSING - Reading element TERMINAL RESPONSE returned %d (element no. %d) from tag %s.",ret,1,KLaunchBrowser);
					continue;
					}
				tLnchBrwsr.iTerRsp = terRsp;
				
				iLnchBrwsr->AppendL(tLnchBrwsr);
				
				}
			break;
		
		case SIMTSY_USSD_CMD_NUMBER:
			/* Retrieving the information related to the USSD DATA DOWNLOAD command */
		
			iUssdData   = new(ELeave) CArrayFixFlat<TUssdData>(KSatGranularity);
			for(i=0;i<ussdDnCount;i++)
				{
				item=CfgFile()->Item(KUssdDataDownload,i);
				if(!item)
					{
					break;
					}

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,src);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_50, "WARNING - CONFIGURATION FILE PARSING - Reading element SOURCE returned %d (element no. %d) from tag %s.",ret,0,KUssdDataDownload);
					continue;
					}
				tUssdData.iSrc = src;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,destn);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_51, "WARNING - CONFIGURATION FILE PARSING - Reading element DESTINATION returned %d (element no. %d) from tag %s.",ret,1,KUssdDataDownload);
					continue;
					}
				tUssdData.iDestn = destn;

				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,dcs);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_52, "WARNING - CONFIGURATION FILE PARSING - Reading element DATA CODING SCHEME returned %d (element no. %d) from tag %s.",ret,2,KUssdDataDownload);
					continue;
					}
					tUssdData.iDcs = dcs;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,ussdStr);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_53, "WARNING - CONFIGURATION FILE PARSING - Reading element USSD STRING returned %d (element no. %d) from tag %s.",ret,3,KUssdDataDownload);
					continue;
					}
				else
					{
					TUint16  value;
					
					if(AsciiToNum(ussdStr, value) == KErrNone)
						{
						tUssdData.iUssdStr.SetLength(1);
						tUssdData.iUssdStr[0] = value;
						}
					else
						{
						OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_54, "WARNING - CONFIGURATION FILE PARSING - Reading element USSDDATADOWNLOAD::USSD STRING returned %d (element no. %d) from tag %s.",KErrArgument,0,KUssdDataDownload);
						continue;
						}
					}
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,UICCRsp);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_55, "WARNING - CONFIGURATION FILE PARSING - Reading element UICC RESPONSE returned %d (element no. %d) from tag %s.",ret,4,KUssdDataDownload);
					continue;
					}
					tUssdData.iUICCRsp = UICCRsp;
				
				iUssdData->AppendL(tUssdData);
				
				}	
		
			break;
		case SIMTSY_EVENT_LIST_CMD_NUMBER:
			/* Retrieving the information about the EVENT DOWNLOAD command */
		
			iEventList  = new(ELeave) CArrayFixFlat<TEventDnld>(KSatGranularity);
			for(i=0;i<eventCount;i++)
				{
				item=CfgFile()->Item(KEventDnld,i);
				if(!item)
					{
					break;
					}

				ret= CTestConfig::GetElement(item->Value(),KStdDelimiter,0,infoType);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_56, "WARNING - CONFIGURATION FILE PARSING - Reading element INFORMATION TYPE returned %d (element no. %d) from tag %s.",ret,0,KEventDnld);
					continue;
					}
				tEventDnld.iVar = infoType;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,framesList);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_57, "WARNING - CONFIGURATION FILE PARSING - Reading element FRAMES LIST returned %d (element no. %d) from tag %s.",ret,1,KEventDnld);
					continue;
					}
				TInt location = framesList.Locate('\n');
				if(location > 0)
					tEventDnld.iFrameList.Set(framesList.Left(location));
				else
					tEventDnld.iFrameList.Set(framesList);
				
				iEventList->AppendL(tEventDnld);
				
				}

			break;
		case SIMTSY_SENDSS_CMD_NUMBER:
			/*  Retrieving the information about the Send SS command */
		
			iSendSS     = new(ELeave) CArrayFixFlat<TSendUSSD>(KSatGranularity);
			for(i=0;i<ssCount;i++)
				{
				item=CfgFile()->Item(KSendSs,i);
				if(!item)
					{
					break;
					}
					
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,textStatus);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_58, "WARNING - CONFIGURATION FILE PARSING - Reading element TEXT ATTR STATUS returned %d (element no. %d) from tag %s.",ret,0,KSendSs);
					continue;
					}
					
				tSendSS.iTextAttStatus = textStatus;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,txtAttr);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_59, "WARNING - CONFIGURATION FILE PARSING - Reading element TEXT ATTRIBUTE returned %d (element no. %d) from tag %s.",ret,1,KSendSs);
					continue;
					}
				
				TInt location = txtAttr.Locate('\n');
							
				if(location > 0)
					tSendSS.iTextAttBuf.Set(txtAttr.Left(location));
				else
					tSendSS.iTextAttBuf.Set(txtAttr);
					
				iSendSS->AppendL(tSendSS);
				
				}

			break;
		case SIMTSY_ELEMFILES_CMD_NUMBER:
			/*  Retrieving the information about the elementary files change */
		
			iElemFiles  = new(ELeave) CArrayFixFlat<TSatInfo>(KSatGranularity);
			for(i=0;i<efCount;i++)
				{
				item=CfgFile()->Item(KElemFiles,i);
				if(!item)
					{
					break;
					}
			
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,duration);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_60, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KElemFiles);
					continue;
					}
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,type);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_61, "WARNING - CONFIGURATION FILE PARSING - Reading element REFRESH TYPE returned %d (element no. %d) from tag %s.",ret,1,KElemFiles);
					continue;
					}
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,FileList);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_62, "WARNING - CONFIGURATION FILE PARSING - Reading element APPLICATION ID returned %d (element no. %d) from tag %s.",ret,2,KElemFiles);
					continue;
					}

				TSatInfo satInfo;
				satInfo.iDuration = duration;
				satInfo.iType = type;
				
				TInt location = FileList.Locate('\n');
				if(location > 0)
					satInfo.iFileList.Set(FileList.Left(location));
				else
					satInfo.iFileList.Set(FileList);
				iElemFiles->AppendL(satInfo);
				
				}
			break;
			
		case SIMTSY_MMSNOTIFICATIONDOWNLOAD_CMD_NUMBER:
			/* Retrieving the information related to the MMS NOTIFICATION DOWNLOAD command */
			iMmsDownloadData = new (ELeave) CArrayFixFlat<TMmsNotificationDownload>(KSatGranularity);
			
			for(i=0;i<notfnDCount;i++)
				{
				item=CfgFile()->Item(KMmsNotificationDownload,i);
				if(!item)
					{
					break;
					}
					
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,src);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_63, "WARNING - CONFIGURATION FILE PARSING - Reading element SOURCE returned %d (element no. %d) from tag %s.",ret,0,KMmsNotificationDownload);
					continue;
					}
				tMmsNotificationDownload.iSrc = src;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,destn);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_64, "WARNING - CONFIGURATION FILE PARSING - Reading element DESTINATION returned %d (element no. %d) from tag %s.",ret,1,KMmsNotificationDownload);
					continue;
					}
				tMmsNotificationDownload.iDestn = destn;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,mmsNotfn);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_65, "WARNING - CONFIGURATION FILE PARSING - Reading element MMS NOTIFICATION returned %d (element no. %d) from tag %s.",ret,2,KMmsNotificationDownload);
					continue;
					}
				location = mmsNotfn.Locate('\n');
				
				if(location > 0)
					tMmsNotificationDownload.iMMSNotfn.Copy(mmsNotfn.Left(location));
				else
					tMmsNotificationDownload.iMMSNotfn.Copy(mmsNotfn);
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,lastEnv);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_66, "WARNING - CONFIGURATION FILE PARSING - Reading element LAST ENVELOPE returned %d (element no. %d) from tag %s.",ret,3,KMmsNotificationDownload);
					continue;
					}
				tMmsNotificationDownload.iLastEnv = lastEnv;
				
				ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,UICCRsp);
				if(ret!=KErrNone)
					{
					OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_67, "WARNING - CONFIGURATION FILE PARSING - Reading element UICC RESPONSE returned %d (element no. %d) from tag %s.",ret,4,KMmsNotificationDownload);
					continue;
					}
				tMmsNotificationDownload.iUICCRsp = UICCRsp;
				
				iMmsDownloadData->AppendL(tMmsNotificationDownload);	
				}
			break;
		default:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMSAT_CONFIGL_68, "Unknown command to process");
			return KErrNotSupported;

		}
		return KErrNone;

		
	}

