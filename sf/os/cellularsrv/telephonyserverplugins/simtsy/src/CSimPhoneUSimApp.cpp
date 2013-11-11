// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the Phone USim applications' manipulation code.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimPhoneUSimAppTraces.h"
#endif

#include "CSimPhone.h"
#include <testconfigfileparser.h>

/**
Standard factory method for two phase construction of a USim
applications object.  The USim applications object simulates the
possible operations for manipulating applications residing on the UICC.

@param aPhone The phone object from which the Phone USim
              applications was opened.
@return A pointer to a new USim applications object.
*/
CSimPhoneUSimApp* CSimPhoneUSimApp::NewL(CSimPhone* aPhone)
	{
	CSimPhoneUSimApp* obj=new(ELeave) CSimPhoneUSimApp(aPhone);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop();
	return obj;
	}

/**
Trivial first phase constructor.

@param aPhone The phone object from which this Phone USimApp was opened.
*/
CSimPhoneUSimApp::CSimPhoneUSimApp(CSimPhone* aPhone)
: iPhone(aPhone), iCurrentSelection(RMobilePhone::EUSimSelectionAutomatic),
	iFoundUSimAppTags(EFalse)
/**
 * Trivial first phase constructor.
 * @param aPhone	The phone object from which this Phone USimApp was opened.
 */
	{}

void CSimPhoneUSimApp::Init()
	{
	}

/**
Second phase constructor for allocating memory to the USim applications
specified in the SimTsy configuration file.
*/
void CSimPhoneUSimApp::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEUSIMAPP_CONSTRUCTL_1, "Starting to parse PhoneUSim applications additional config parameters...");
	
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;
	TInt i;
	TPtrC8 aid, label;
	RMobilePhone::TUSimApplicationInfoV2 appInfo;

	TRAP_IGNORE(iTimer=CSimTimer::NewL(iPhone));
	iUSimAppInfo=new(ELeave) CArrayFixFlat<RMobilePhone::TUSimApplicationInfoV2>(KMaxUSimApps+1);
	TInt count=CfgFile()->ItemCount(KUSIMAppInfo);
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEUSIMAPP_CONSTRUCTL_2, "Starting to Load and Parse USim Application Info");

	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KUSIMAppInfo,i);
		if(!item)
			break;
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0, aid);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEUSIMAPP_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element AID returned %d (element no. %d) from tag %s.",ret,0,KUSIMAppInfo);
			continue;
			}
		else
			{
			appInfo.iAID.Copy(aid);
			if(i==0)
				iActiveAID.Copy(aid);
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1, label);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEUSIMAPP_CONSTRUCTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element LABEL returned %d (element no. %d) from tag %s.",ret,1,KUSIMAppInfo);
			continue;
			}
		else 
			appInfo.iLabel.Copy(label);

		iUSimAppInfo->AppendL(appInfo);
		iFoundUSimAppTags = ETrue;
		}
	
	iUSimAppEventList=new(ELeave) CArrayFixFlat<TUSimAppEvent>(10);
	TUSimAppEvent appEvent;
	TInt duration;

	count=CfgFile()->ItemCount(KUSIMAppSwitching);
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEUSIMAPP_CONSTRUCTL_5, "Starting to Load and Parse USim Application Info");

	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KUSIMAppSwitching,i);
		if(!item)
			break;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0, duration);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEUSIMAPP_CONSTRUCTL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element DURATION returned %d (element no. %d) from tag %s.",ret,0,KUSIMAppSwitching);
			continue;
			}
		else
			appEvent.iDuration=duration;

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1, aid);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEUSIMAPP_CONSTRUCTL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element AID returned %d (element no. %d) from tag %s.",ret,1,KUSIMAppSwitching);
			continue;
			}
		else 
			appEvent.iAid.Copy(aid);

		iUSimAppEventList->AppendL(appEvent);
		}
	}

/**
Standard destructor.  Any objects created by the ::ConstructL() function
will be destroyed here.
*/
CSimPhoneUSimApp::~CSimPhoneUSimApp()
	{
	delete iTimer;
	if(iUSimAppEventList)
		{
		iUSimAppEventList->Delete(0, iUSimAppEventList->Count());
		delete iUSimAppEventList;
		}
	if(iUSimAppInfo)
		{
		iUSimAppInfo->Delete(0, iUSimAppInfo->Count());
		delete iUSimAppInfo;
		}
	}

/**
Dispatch function for all USim application requests.

@param aReqHandle The TSY request handle for this request.
@param aIpc The IPC number of this request.
@param aPckg The parameter packaged data related to this request.
@return A system wide error code indicating the status of the request.
*/
TInt CSimPhoneUSimApp::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc, const TDataPackage& aPckg)
	{
	TAny* dataPtr = aPckg.Ptr1();
	TAny* dataPtr2 = aPckg.Ptr2();

// The following requests can be completed even if the completion of another request is pending.
	switch(aIpc)
		{

	case EMobilePhoneEnumerateUSimApplications:
		return EnumerateUSimApplications(aTsyReqHandle, 
			reinterpret_cast<TInt*> (dataPtr),
			reinterpret_cast<RMobilePhone::TAID*> (dataPtr2));

	case EMobilePhoneSetUSimApplicationStatus:
		return SetUSimApplicationStatus(aTsyReqHandle,
			reinterpret_cast<RMobilePhone::TAID*> (dataPtr),
			reinterpret_cast<RMobilePhone::TUSimAppAction*> (dataPtr2));

	case EMobilePhoneGetUSimApplicationsInfo:
		return GetUSimApplicationInfo(aTsyReqHandle, 
			reinterpret_cast<TInt*> (dataPtr),
			aPckg.Des2n());

	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		return NotifyUSimApplicationsInfoChange(aTsyReqHandle, 
			reinterpret_cast<TInt*> (dataPtr),
			reinterpret_cast<RMobilePhone::TAID*> (dataPtr2));

	case EMobilePhoneGetUSimAppsSelectionMode:
		return GetUSimAppsSelectionMode(aTsyReqHandle,
			reinterpret_cast<RMobilePhone::TUSimSelectionMode*> (dataPtr));

	case EMobilePhoneSetUSimAppsSelectionMode:
		return SetUSimAppsSelectionMode(aTsyReqHandle,
			reinterpret_cast<RMobilePhone::TUSimSelectionMode*> (dataPtr));

	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
		return NotifyUSIMAppsSelectionModeChange(aTsyReqHandle, 
			reinterpret_cast<RMobilePhone::TUSimSelectionMode*> (dataPtr));

	default:
		break;
		}
	return KErrNotSupported;
	}

/**
The API does not support any objects that could be opened from this one.
*/
CTelObject* CSimPhoneUSimApp::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/**
The API does not support any objects that could be opened from this one.
*/
CTelObject* CSimPhoneUSimApp::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/**
This function returns the Request Mode for the request with the passed
IPC value.

@param aIpc The IPC number of the request.
@return TReqMode The request mode.
*/
CTelObject::TReqMode CSimPhoneUSimApp::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;	

	switch(aIpc)
		{
	case EMobilePhoneEnumerateUSimApplications:
	case EMobilePhoneSetUSimApplicationStatus:
	case EMobilePhoneGetUSimApplicationsInfo:
	case EMobilePhoneGetUSimAppsSelectionMode:
	case EMobilePhoneSetUSimAppsSelectionMode:
		break;

	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

/**
The ETel Server calls this function when the first client makes a notification
request.  If supported by the underlying protocol controlling the
signalling stack, this can be used to start requesting updates for the relevant
service.
*/
TInt CSimPhoneUSimApp::RegisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

/**
The ETel Server calls this function when the last client that had previously
made a notification request closes its ETel Server handle.  If supported by
the underlying protocol controlling the	signalling stack, this can be used
to stop requesting updates for the relevant service.
*/
TInt CSimPhoneUSimApp::DeregisterNotification(const TInt /*aIpc*/)
	{
	return KErrNone;
	}

/**
Return the number of slots that the ETel Server should allocate for
buffering requests of the given IPC number.
*/
TInt CSimPhoneUSimApp::NumberOfSlotsL(const TInt aIpc)
	{
	switch(aIpc)
		{
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		return KDefaultNumberOfSlots;

	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEUSIMAPP_NUMBEROFSLOTSL_1, "CSimPhoneUSimApp: Number of Slots error, unknown IPC");
		User::Leave(KErrNotSupported);
		break;
		}
	return KDefaultNumberOfSlots;
	}

/**
Cancels an outstanding client request.

@param aIpc The IPC number of the request that is to be cancelled.
@param aTsyReqHandle The TSY request handle of the request that is to
                     be cancelled.
@return A system wide error code indicating the status of the request
        on cancellation.
*/
TInt CSimPhoneUSimApp::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	switch(aIpc)
		{
	case EMobilePhoneEnumerateUSimApplications:
		return EnumerateUSimApplicationsCancel(aTsyReqHandle);

	case EMobilePhoneSetUSimApplicationStatus:
		return SetUSimApplicationStatusCancel(aTsyReqHandle);
	
	case EMobilePhoneGetUSimApplicationsInfo:
		return GetUSimApplicationInfoCancel(aTsyReqHandle);

	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		return NotifyUSimApplicationsInfoChangeCancel(aTsyReqHandle);
	
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
		return NotifyUSIMAppsSelectionModeChangeCancel(aTsyReqHandle);

	default:
		break;
		}
	return KErrNone;
	}

/**
Returns a pointer to the SimTsy configuration file section.

@return A CTestConfigSection pointer, which contains a section of the
        parsed information in the config file.
*/
const CTestConfigSection* CSimPhoneUSimApp::CfgFile()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONEUSIMAPP_CFGFILE_1, ">>CSimPhoneUSimApp::CfgFile");
	return iPhone->CfgFile();
	}

/**
Timer callback function.  When the timer's delay ends, it will call
back into this function for completion of the pending request.
Currently, this call back is only used to complete a notify info change
request on a specific application (NotifyUSimApplicationsInfoChange).

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
@param aId an id identifying which timer callback is being called
*/	
void CSimPhoneUSimApp::TimerCallBack(TInt /*aId*/)
	{
	if(iNotifyInfo.iNotifyPending)
		{
		TUSimAppEvent event=iUSimAppEventList->At(iEventPointer);
		*(RMobilePhone::TAID*)iNotifyInfo.iNotifyData=event.iAid;
		*((TInt*)iNotifyInfo.iNotifyDataXtra)=iUSimAppInfo->Count();
		iActiveAID.Copy(event.iAid);
		iNotifyInfo.iNotifyPending=EFalse;
		iEventPointer++;
		iPhone->ReqCompleted(iNotifyInfo.iNotifyHandle,KErrNone);
		}
	}
	
TBool CSimPhoneUSimApp::FoundUSimAppTags()
/**
* Returns a value to indicate whether USim App tags were found in the Config file
*
*/
	{
	return iFoundUSimAppTags;
	}

/**
Enumerates the number of USim applications in the UICC and returns the
AID of the active application.

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
@param aCount Returns the number of USim applications.
@param aActiveAID Returns the AID of the currently active application.
@return KErrNone.
*/
TInt CSimPhoneUSimApp::EnumerateUSimApplications(TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID)
	{
	*aCount=iUSimAppInfo->Count();
	aActiveAID->Copy(iActiveAID);
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

/**
Attempts to cancel a pending EnumerateUSimApplications request
(unsuccesfully).

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
*/
TInt CSimPhoneUSimApp::EnumerateUSimApplicationsCancel(TTsyReqHandle aTsyReqHandle)
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

/**
Retrieves information for a specified USim application

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
@param aUSimAppIndex Used to specify the index of the application.
@param aInfo Returns the information of the specified appplication.
@return KErrNone.
*/
TInt CSimPhoneUSimApp::GetUSimApplicationInfo(TTsyReqHandle aTsyReqHandle, TInt* aUSimAppIndex, TDes8* aInfo)
	{
	if(*aUSimAppIndex>=iUSimAppInfo->Count())
		{
		iPhone->ReqCompleted(aTsyReqHandle, KErrNotFound);
		return KErrNone;
		}
	else
		{
		TPckg<RMobilePhone::TUSimApplicationInfoV2>* infoPckg = (TPckg<RMobilePhone::TUSimApplicationInfoV2>*)aInfo;
		RMobilePhone::TUSimApplicationInfoV2& info = (*infoPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(info);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aTsyReqHandle, err);
			return KErrNone;
			}

		info=iUSimAppInfo->At(*aUSimAppIndex);
		iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
		}
	
	return KErrNone;
	}

/**
Attempts to cancel a pending GetUSimApplicationInfo request
(unsuccesfully).

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
*/
TInt CSimPhoneUSimApp::GetUSimApplicationInfoCancel(TTsyReqHandle aTsyReqHandle)
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

/**
Notifies the client when the specified USim application's information
changes.

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
@param aCount Returns the total number of USim applications when the
              request completes.
@param aActiveAID The application's AID whose infromation change will
                  be notified to the client.
@return KErrNone.
*/
TInt CSimPhoneUSimApp::NotifyUSimApplicationsInfoChange(TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID)
	{
	__ASSERT_ALWAYS(!iNotifyInfo.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));

	TInt count=iUSimAppEventList->Count();

	iNotifyInfo.iNotifyPending=ETrue;
	iNotifyInfo.iNotifyHandle=aTsyReqHandle;
	iNotifyInfo.iNotifyData=aActiveAID;
	iNotifyInfo.iNotifyDataXtra=aCount;

	if(iEventPointer<count)
		{
		if(iTimerStarted)
			{
			delete iTimer;
			iTimer = NULL;
			TRAP_IGNORE(iTimer=CSimTimer::NewL(iPhone));
			}

		TUSimAppEvent event=iUSimAppEventList->At(iEventPointer);

		if (iTimer != NULL)
			{
			iTimer->Start(event.iDuration,this);
			iTimerStarted = ETrue;
			}
		}

	return KErrNone;
	}

/**
Attempts to cancel a pending NotifyUSimApplicationsInfoChange request.

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
*/
TInt CSimPhoneUSimApp::NotifyUSimApplicationsInfoChangeCancel(TTsyReqHandle aTsyReqHandle)
	{
	if(iNotifyInfo.iNotifyPending)
		{
		iNotifyInfo.iNotifyPending=EFalse;
		iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

/**
Sets the Selection mode for USim applications.

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
@param aUSimSelectionMode Used to specify the new selection mode for
                          applications.
@return KErrNone.
*/
TInt CSimPhoneUSimApp::SetUSimAppsSelectionMode(TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode)
	{
	iCurrentSelection=*aUSimSelectionMode;
	if(iNotifyMode.iNotifyPending)
		{
		iNotifyMode.iNotifyPending=EFalse;
		*(RMobilePhone::TUSimSelectionMode*)iNotifyMode.iNotifyData=iCurrentSelection;
		iPhone->ReqCompleted(iNotifyMode.iNotifyHandle,KErrNone);
		}
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

/**
Requests the selection mode of a particular USim applications.

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
@param aUSimSelectionMode Returns the selection mode for applications.
@return KErrNone.
*/
TInt CSimPhoneUSimApp::GetUSimAppsSelectionMode(TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode)
	{
	*aUSimSelectionMode=iCurrentSelection;
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

/**
Notifies the client on a selection mode change of USim applications.

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
@param aUSimSelectionMode Returns the new selection mode for applications.
@return KErrNone.
*/
TInt CSimPhoneUSimApp::NotifyUSIMAppsSelectionModeChange(TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode)
	{
	__ASSERT_ALWAYS(!iNotifyMode.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iNotifyMode.iNotifyPending=ETrue;
	iNotifyMode.iNotifyHandle=aTsyReqHandle;
	iNotifyMode.iNotifyData=aUSimSelectionMode;
	return KErrNone;
	}

/**
Attempts to cancel a pending NotifyUSIMAppsSelectionModeChange request.

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
*/
TInt CSimPhoneUSimApp::NotifyUSIMAppsSelectionModeChangeCancel(TTsyReqHandle aTsyReqHandle)
	{
	if(iNotifyMode.iNotifyPending)
		{
		iNotifyMode.iNotifyPending=EFalse;
		iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

/**
Attempts to set the status of a specific USim application.

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
@param aAID The specific application's ID whose status is to be set.
@param aUsimAppAction The status to apply to the application identified
                      by aAID.
@return KErrNone on completion of request, unless the aAID given is not
        identifiable, in which case KErrArgument is returned.
*/
TInt CSimPhoneUSimApp::SetUSimApplicationStatus(TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TUSimAppAction* aUsimAppAction)
	{
	TInt count=iUSimAppInfo->Count();
	for(TInt i=0;i<count;i++)
		{
		RMobilePhone::TUSimApplicationInfoV2 info=iUSimAppInfo->At(i);

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(info);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aTsyReqHandle, err);
			return KErrNone;
			}
		
			if(info.iAID.Compare(*aAID)==0)
				{
				if(*aUsimAppAction==RMobilePhone::EUSimActivate)
					{
					iActiveAID=*aAID;
					if(iNotifyInfo.iNotifyPending)
						{
						iNotifyInfo.iNotifyPending=EFalse;
						iNotifyInfo.iNotifyData=&iActiveAID;
						*(TInt*)iNotifyInfo.iNotifyDataXtra=iUSimAppInfo->Count();
						iPhone->ReqCompleted(iNotifyInfo.iNotifyHandle,KErrNone);
						}
					iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
					return KErrNone;
					}
				else if((*aUsimAppAction==RMobilePhone::EUSimTerminate) &&
						(iActiveAID.Compare(*aAID)==0))
					{
						//if the active is the last one
						if(i==KMaxUSimApps-1)
							info=iUSimAppInfo->At(0);
						else
							info=iUSimAppInfo->At(i+1);
						iActiveAID=info.iAID;

						if(iNotifyInfo.iNotifyPending)
						{
						iNotifyInfo.iNotifyPending=EFalse;
						iNotifyInfo.iNotifyData=&iActiveAID;
						*(TInt*)iNotifyInfo.iNotifyDataXtra=iUSimAppInfo->Count();
						iPhone->ReqCompleted(iNotifyInfo.iNotifyHandle,KErrNone);
						}
					iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
					return KErrNone;
					}
					else if((*aUsimAppAction==RMobilePhone::EUSimTerminate) &&
					(iActiveAID.Compare(*aAID)!=0))
						{
						iPhone->ReqCompleted(aTsyReqHandle,KErrArgument);
						return KErrArgument;
						}
				}
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrNotFound);
	return KErrNone;
	}

/**
Attempts to cancel a pending SetUSimApplicationStatus request
(unsuccesfully).

@param aTsyReqHandle A handle belonging to the client that created the
                     session with this TSY server.
@return KErrNone.
*/
TInt CSimPhoneUSimApp::SetUSimApplicationStatusCancel(TTsyReqHandle aTsyReqHandle)
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

/**
Returns the active application's aid (internally).
*/
RMobilePhone::TAID CSimPhoneUSimApp::GetActiveUSim()
	{
	return iActiveAID;
	}
