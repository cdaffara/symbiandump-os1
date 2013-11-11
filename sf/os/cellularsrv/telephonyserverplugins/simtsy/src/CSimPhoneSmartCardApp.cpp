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
// Implements the Phone Smart Card application and file manipulation code.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimPhoneSmartCardAppTraces.h"
#endif

#include <testconfigfileparser.h>
#include <etelmmerr.h>
#include "CSimPhone.h"

//
// CSimPhoneSmartCardApp
//

CSimPhoneSmartCardApp* CSimPhoneSmartCardApp::NewL(CSimPhone* aPhone)
/**
 * Standard two phase constructor.
 * @param aPhone The phone object from which the Phone Smart Card applications will open
 */
	{
	CSimPhoneSmartCardApp* obj=new(ELeave) CSimPhoneSmartCardApp(aPhone);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop();
	return obj;
	}

CSimPhoneSmartCardApp::CSimPhoneSmartCardApp(CSimPhone* aPhone)
: iPhone(aPhone), iCurrentSelection(RMobilePhone::EUSimSelectionAutomatic), 
	iFoundScAppTags(EFalse)
/**
 * Trivial first phase constructor.
 * @param aPhone	The phone object from which this Phone SmartCard App was opened.
 */
	{}

void CSimPhoneSmartCardApp::Init()
	{}

void CSimPhoneSmartCardApp::ConstructL()
/**
* Second phase of the 2-phase constructor.
* Constructs all the member data and retrieves all the data from the config file specific to this class.
*
* @leave Leaves due to not enough memory or if any data member does not construct for any reason.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_CONSTRUCTL_1, "Starting to parse Phone Smart Card applications additional config parameters...");
	ParseSmartCardApplicationInfoL();
	ParseSmartCardFileInfoL();
	MapUSimAppsL();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_CONSTRUCTL_2, "Finished parsing Phone Smart Card applications additional config parameters...");
	}


CSimPhoneSmartCardApp::~CSimPhoneSmartCardApp()
/**
 * Standard destructor.  Any objects created by the ::ConstructL() function
 * will be destroyed here.
 */
	{
	delete iTimer;
	if(iSmartCardAppEventList)
		{
		iSmartCardAppEventList->Delete(0, iSmartCardAppEventList->Count());
		delete iSmartCardAppEventList;
		}
	if(iSmartCardAppList)
		{
		iSmartCardAppList->Delete(0, iSmartCardAppList->Count());
		delete iSmartCardAppList;
		}
	if (iSmartCardFileList)
		{
		iSmartCardFileList->ResetAndDestroy();
		delete iSmartCardFileList;
		}
	if(iUSimAppList)
		{
		iUSimAppList->Close();
		delete iUSimAppList;
		}
	}


TInt CSimPhoneSmartCardApp::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc, const TDataPackage& aPckg)
/**
* ExtFunc is called by the server when it has an "extended", i.e. non-core ETel request
* for the TSY to process.
* A request handle, request type and request data are passed to the TSY
* 
* @param aTsyReqHandle  The request handle for completing the request 
* @param aIpc Ipc representing the request
* @param aPackage any data associated with the request
* @return err KErrNone if request completes ok
*/
	{

	TAny* dataPtr = aPckg.Ptr1();
	TAny* dataPtr2 = aPckg.Ptr2();

	// The following requests can be completed even if the completion of another request is pending.
	switch(aIpc)
		{
	case EMobilePhoneGetCurrentActiveUSimApplication:
		return GetCurrentActiveUSimApplication(aTsyReqHandle,
			reinterpret_cast<RMobilePhone::TAID*>(dataPtr));
			
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
		
	case EMobilePhoneEnumerateSmartCardApplications:
		return EnumerateScApplications(aTsyReqHandle, 
			reinterpret_cast<TUint*>(dataPtr));

	case EMobilePhoneGetSmartCardApplicationInfo:
		return GetScApplicationInfo(aTsyReqHandle, 
			reinterpret_cast<TInt*>(dataPtr), 
			aPckg.Des2n());

	case EMobilePhoneSetSmartCardApplicationStatus:
		return SetScApplicationStatus(aTsyReqHandle,
			reinterpret_cast<RMobilePhone::TAID*>(dataPtr),
			reinterpret_cast<RMobilePhone::TSmartCardApplicationAction*>(dataPtr2));

	case EMobilePhoneNotifySmartCardApplicationInfoChange:
		return NotifyScApplicationInfoChange(aTsyReqHandle, 
			reinterpret_cast<RMobilePhone::TAID*>(dataPtr), 
			reinterpret_cast<RMobilePhone::TSmartCardApplicationEvent*>(dataPtr2));

	case EMobilePhoneGetUSimAppsSelectionMode:
		return GetUSimAppsSelectionMode(aTsyReqHandle,
			reinterpret_cast<RMobilePhone::TUSimSelectionMode*>(dataPtr));

	case EMobilePhoneSetUSimAppsSelectionMode:
		return SetUSimAppsSelectionMode(aTsyReqHandle,
			reinterpret_cast<RMobilePhone::TUSimSelectionMode*>(dataPtr));

	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
		return NotifyUSIMAppsSelectionModeChange(aTsyReqHandle, 
			reinterpret_cast<RMobilePhone::TUSimSelectionMode*>(dataPtr));
			
	case EMobilePhoneGetScFileInfo:
		return GetScFileInfo(aTsyReqHandle,
			*reinterpret_cast<RMobilePhone::TScFilePath*>(dataPtr),
			aPckg.Des2n());
		
	case EMobilePhoneReadScFile:
		return ReadScFile(aTsyReqHandle,
			*reinterpret_cast<RMobilePhone::TScFilePathWithAccessOffsets*>(dataPtr),
			aPckg.Des2n());
		
	case EMobilePhoneUpdateScFile:
		return UpdateScFile(aTsyReqHandle,
			*reinterpret_cast<RMobilePhone::TScFilePathWithAccessOffsets*>(dataPtr),
			aPckg.Des2n());

	default:
		break;
		}
	return KErrNotSupported;
	}
	

CTelObject::TReqMode CSimPhoneSmartCardApp::ReqModeL(const TInt aIpc)
/**
 * This function returns the Request Mode for the request with the passed IPC value.
 * The function leaves if the IPC is not found.
 *
 * @param aIpc		The IPC number of the request.
 * @return TReqMode	The request mode.
 */
	{
	CTelObject::TReqMode ret=0;	

	switch(aIpc)
		{
	case EMobilePhoneGetCurrentActiveUSimApplication:
	case EMobilePhoneEnumerateUSimApplications:
	case EMobilePhoneSetUSimApplicationStatus:
	case EMobilePhoneGetUSimApplicationsInfo:
	case EMobilePhoneEnumerateSmartCardApplications:
	case EMobilePhoneSetSmartCardApplicationStatus:
	case EMobilePhoneGetSmartCardApplicationInfo:
	case EMobilePhoneGetUSimAppsSelectionMode:
	case EMobilePhoneSetUSimAppsSelectionMode:
	case EMobilePhoneGetScFileInfo:
	case EMobilePhoneReadScFile:
	case EMobilePhoneUpdateScFile:
		break;

	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}
	
TInt CSimPhoneSmartCardApp::RegisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the first client makes a notification
 * request.  If supported by the underlying protocol controlling the
 * signalling stack, this can be used to start requesting updates for the relevant
 * service.
 */
	{
	return KErrNone;
	}

TInt CSimPhoneSmartCardApp::DeregisterNotification(const TInt /*aIpc*/)
/**
 * The ETel Server calls this function when the last client that had previously
 * made a notification request closes its ETel Server handle.  If supported by
 * the underlying protocol controlling the	signalling stack, this can be used
 * to stop requesting updates for the relevant service.
 */
	{
	return KErrNone;
	}

CTelObject* CSimPhoneSmartCardApp::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSimPhoneSmartCardApp::OpenNewObjectL(TDes&)
/**
 * The API does not support any objects that could be opened from this one.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


TInt CSimPhoneSmartCardApp::NumberOfSlotsL(const TInt aIpc)
/**
 * Return the number of slots that the ETel Server should allocate for buffering requests
 * of the given IPC number.
 *
 * @param aIpc	The IPC number of the request.
 */
	{
	TInt numberOfSlots = 0;
	
	switch(aIpc)
		{
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
	case EMobilePhoneNotifyUSimApplicationsInfoChange:
	case EMobilePhoneNotifySmartCardApplicationInfoChange:
		numberOfSlots = KDefaultNumberOfSlots;
		break;
	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_NUMBEROFSLOTSL_1, "CSimPhoneSmartCardApp: Number of Slots error, unknown IPC");
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CSimPhoneSmartCardApp::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
 * Cancel an outstanding request.
 * @param aIpc			The IPC number of the request that is to be cancelled.
 * @param aTsyReqHandle	The TSY request handle of the request that is to be cancelled.
 * @return	Standard return value.
 */
	{

	switch(aIpc)
		{
	case EMobilePhoneEnumerateSmartCardApplications:
		return EnumerateScApplicationsCancel(aTsyReqHandle);

	case EMobilePhoneEnumerateUSimApplications:
		return EnumerateUSimApplicationsCancel(aTsyReqHandle);

	case EMobilePhoneSetUSimApplicationStatus:
		return SetUSimApplicationStatusCancel(aTsyReqHandle);
	
	case EMobilePhoneGetUSimApplicationsInfo:
		return GetUSimApplicationInfoCancel(aTsyReqHandle);

	case EMobilePhoneNotifyUSimApplicationsInfoChange:
		return NotifyUSimApplicationsInfoChangeCancel(aTsyReqHandle);

	case EMobilePhoneSetSmartCardApplicationStatus:
		return SetScApplicationStatusCancel(aTsyReqHandle);
	
	case EMobilePhoneGetSmartCardApplicationInfo:
		return GetScApplicationInfoCancel(aTsyReqHandle);

	case EMobilePhoneGetCurrentActiveUSimApplication:
		return GetCurrentActiveUSimApplicationCancel(aTsyReqHandle);

	case EMobilePhoneNotifySmartCardApplicationInfoChange:
		return NotifyScApplicationInfoChangeCancel(aTsyReqHandle);
		
	case EMobilePhoneSetUSimAppsSelectionMode:
		return SetUSimAppsSelectionModeCancel(aTsyReqHandle);
	
	case EMobilePhoneGetUSimAppsSelectionMode:
		return GetUSimAppsSelectionModeCancel(aTsyReqHandle);
	
	case EMobilePhoneNotifyUSimAppsSelectionModeChange:
		return NotifyUSIMAppsSelectionModeChangeCancel(aTsyReqHandle);
	
	case EMobilePhoneGetScFileInfo:
		return GetScFileInfoCancel(aTsyReqHandle);

	case EMobilePhoneReadScFile:
		return ReadScFileCancel(aTsyReqHandle);

	case EMobilePhoneUpdateScFile:
		return UpdateScFileCancel(aTsyReqHandle);

	default:
		break;
		}
	return KErrNone;
	}


const CTestConfigSection* CSimPhoneSmartCardApp::CfgFile()
/**
* Returns a pointer to the config file section
*
* @return CTestConfigSection a pointer to the configuration file data section
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_CFGFILE_1, ">>CSimPhoneSmartCardApp::CfgFile");
	return iPhone->CfgFile();
	}

void CSimPhoneSmartCardApp::TimerCallBack(TInt /*aId*/)
/**
* Timer callback function.  When the timer goes off, it will call back into this
* function for further processing.
*
* @param aId an id identifying which timer callback is being called
*/	
	{
	if(iNotifyScInfo.iNotifyPending || iNotifyUSimInfo.iNotifyPending)
		{
		TSmartCardAppEvent theEvent=iSmartCardAppEventList->At(iEventPointer);
		TInt ret;
		RMobilePhone::TSmartCardApplicationEvent occuredEvent;
		TBool isUSimAppAffected = EFalse;
				
		if(theEvent.iAction == RMobilePhone::EScApplicationActivate)
			{
			ret=ActivateSmartCardApp(theEvent.iAID, isUSimAppAffected);
			occuredEvent = RMobilePhone::EScApplicationActivated;
			}
		else
			{
			ret=TerminateSmartCardApp(theEvent.iAID, isUSimAppAffected);
			occuredEvent = RMobilePhone::EScApplicationTerminated;
			}
			
		if (ret == KErrNone)
			{
			iEventPointer++;

			//notify any ScApp client if such notify request is pending
			if(iNotifyScInfo.iNotifyPending)
				{
				iNotifyScInfo.iNotifyPending=EFalse;
				*(RMobilePhone::TAID*)iNotifyScInfo.iNotifyData1=theEvent.iAID;
				*(RMobilePhone::TSmartCardApplicationEvent*)iNotifyScInfo.iNotifyData2=occuredEvent;
				iPhone->ReqCompleted(iNotifyScInfo.iNotifyHandle,KErrNone);
				}
				
			//notify any USimApp client if such notify request is pending
			if(isUSimAppAffected  && iNotifyUSimInfo.iNotifyPending)
				{
				iNotifyUSimInfo.iNotifyPending=EFalse;

				if(occuredEvent == RMobilePhone::EScApplicationActivated)
					{
					*(RMobilePhone::TAID*)iNotifyUSimInfo.iNotifyData1=theEvent.iAID;
					}
				else
					{
					*(RMobilePhone::TAID*)iNotifyUSimInfo.iNotifyData1=_L8("");
					}

				*(TInt*)iNotifyUSimInfo.iNotifyData2=iUSimAppList->Count();
				iPhone->ReqCompleted(iNotifyUSimInfo.iNotifyHandle,KErrNone);
				}
			}
		}//end if(iNotifyScInfo.iNotifyPending...
	}
	
TBool CSimPhoneSmartCardApp::FoundScAppTags()
/**
* Returns a value to indicate whether SCApp tags were found in the Config file
*
*/
	{
	return iFoundScAppTags;
	}
	
	
void CSimPhoneSmartCardApp::TSmartCardFile::GetFileInfo(RMobilePhone::TScFileInfoV5& aScFileInfo)
/**
* Gets the SmartCard file's File info
* 
* @param aScFileInfo The function passes back the file info through this reference
*/
	{
	aScFileInfo.iFileID = iFileID;
	aScFileInfo.iType = iType;
	aScFileInfo.iTotalLength = iTotalLength;
	aScFileInfo.iRecordLength = iRecordLength;
	aScFileInfo.iNumberRecords = iNumberRecords;		
	}

CSimPhoneSmartCardApp::TCyclicSmartCardFile::TCyclicSmartCardFile()
	:iOldestRecord(0)
	{}
		
TInt CSimPhoneSmartCardApp::TCyclicSmartCardFile::ReadFile(
		const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer)
/**
* Concrete implementation of the ReadFile() method for reading file data from a record based
* cyclic smart card file. Reads precisely one record (iRecordLength bytes).
*
* Note: Records are numbered from 1 to n NOT 0 to n-1
*
* @param aFilePathOffsets Information about which file to read (path) and where in the file to read data from (offsets)
* @param aReadBuffer The requested data is read into this buffer
*/
	{
	//check that the client has provided a buffer of sufficient size
	if(aReadBuffer->MaxSize() < iRecordLength)
		{
		return KErrOverflow;
		}
		
	//check that the client has specified a valid start offset
	else if(aFilePathOffsets.iStartOffset < 1 || aFilePathOffsets.iStartOffset > iNumberRecords)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_TCYCLICSMARTCARDFILE_READFILE_1, "ERROR: File start offset information invalid.");
		return KErrMMEtelScRecordNotFound;	
		}
		
	else
		{
		//Var filepointer initially holds a pointer to the beginning of the specified record
		//The record to read is specified as an offset from the oldest record. I.e. Record 1 is the
		//newest record.
		TInt filePointer = iOldestRecord + (aFilePathOffsets.iStartOffset * iRecordLength);
		if(filePointer >= iTotalLength)
			{
			filePointer = filePointer - iTotalLength;	
			}
			
		aReadBuffer->SetLength(iRecordLength);
		aReadBuffer->FillZ();
		
		TInt index;
		
		for(index = 0; index < iRecordLength; index++)
			{
			(*aReadBuffer)[index] = iFileData[filePointer];
			filePointer++;
			}
		return KErrNone;		
		}
	}

TInt CSimPhoneSmartCardApp::TCyclicSmartCardFile::UpdateFile(
		const RMobilePhone::TScFilePathWithAccessOffsets& /*aFilePathOffsets*/, TDes8* aWriteBuffer)
/**
* Concrete implementation of the UpdateFile() method for updating file data in a record based
* cyclic smart card file. Updates precisely one record (iRecordLength bytes). The record that is
* updated is the oldest record.
*
* Note: Records are numbered from 1 to n NOT 0 to n-1
*
* @param aReadBuffer The data with which the file should be updated.
*/
	{
	//Check that the client has provided a buffer of sufficient size
	if(aWriteBuffer->Size() != iRecordLength)
		{
		return KErrMMEtelScWrongLength;
		}
	else
		{
		TInt filePointer = iOldestRecord ;
			
		TInt index;

		for(index = 0; index < iRecordLength; index++)
			{
			iFileData[filePointer] = (*aWriteBuffer)[index];
			filePointer++;
			}
		UpdateOldestRecordPointer();
		return KErrNone;
		}
	}
	
void CSimPhoneSmartCardApp::TCyclicSmartCardFile::UpdateOldestRecordPointer()
	{
	//IF the OldestRecord pointer is set to point to the beginning of the buffer
	if(iOldestRecord==0)
		{
		//Set it to point to the beginning of the last record in the buffer
		iOldestRecord = iTotalLength - iRecordLength;
		}
	//ELSE set it to point to the beginning of previous record (the second oldest record)
	else
		{
		iOldestRecord = iOldestRecord - iRecordLength;
		}
	}
	
	
TInt CSimPhoneSmartCardApp::TLinearSmartCardFile::ReadFile(
		const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer)
/**
* Concrete implementation of the ReadFile() method for reading file data from a record based
* linear fixed smart card file. Reads precisely one record (iRecordLength bytes).
*
* Note: Records are numbered from 1 to n NOT 0 to n-1
*
* @param aFilePathOffsets Information about which file to read (path) and where in the file to read data from (offsets)
* @param aReadBuffer The requested data is read into this buffer
*/
	{
	//check that the client has provided a buffer of sufficient size
	if(aReadBuffer->MaxSize() < iRecordLength)
		{
		return KErrOverflow;
		}
		
	//check that the client has specified a valid start offset
	else if(aFilePathOffsets.iStartOffset < 1 || aFilePathOffsets.iStartOffset > iNumberRecords)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_TLINEARSMARTCARDFILE_READFILE_1, "ERROR: File start offset information invalid.");
		return KErrMMEtelScRecordNotFound;	
		}
	else
		{
		TInt filePointer = (aFilePathOffsets.iStartOffset - 1) * iRecordLength;
		TInt index;

		aReadBuffer->SetLength(iRecordLength);
		aReadBuffer->FillZ();
		
		for(index = 0; index < iRecordLength; index++)
			{
			(*aReadBuffer)[index] = iFileData[filePointer];
			filePointer++;
			}
		return KErrNone;
		}
	}
	
TInt CSimPhoneSmartCardApp::TLinearSmartCardFile::UpdateFile(
		const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aWriteBuffer)
/**
* Concrete implementation of the UpdateFile() method for updating data in a record based
* linear fixed smart card file. Updates precisely one record (iRecordLength bytes).
*
* Note: Records are numbered from 1 to n NOT 0 to n-1
*
* @param aFilePathOffsets Information about which file to update (path) and where in the file to write data to (offsets)
* @param aReadBuffer The data with which the file should be updated.
*/
	{
	//check that the client has provided a buffer of sufficient size
	if(aWriteBuffer->Size() != iRecordLength)
		{
		return KErrMMEtelScWrongLength;
		}
	
	//check that the client has specified a valid start offset
	else if(aFilePathOffsets.iStartOffset < 1 || aFilePathOffsets.iStartOffset > iNumberRecords)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_TLINEARSMARTCARDFILE_UPDATEFILE_1, "ERROR: File start offset information invalid.");
		return KErrMMEtelScRecordNotFound;	
		}
	else
		{
		TInt filePointer = (aFilePathOffsets.iStartOffset - 1) * iRecordLength;
		TInt index;

		for(index = 0; index < iRecordLength; index++)
			{
			iFileData[filePointer] = (*aWriteBuffer)[index];
			filePointer++;
			}
		return KErrNone;
		}
	}
	
TInt CSimPhoneSmartCardApp::TTransparentSmartCardFile::ReadFile(
		const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer)
/**
* Concrete implementation of the ReadFile() method for reading file data from a binary transparent
* smart card file.
*
* @param aFilePathOffsets Information about which file to read (path) and where in the file to read data from (offsets)
* @param aReadBuffer The requested data is read into this buffer
*/
	{
	const TInt& bytesToRead = aFilePathOffsets.iCount;
	TInt filePointer = aFilePathOffsets.iStartOffset;

	//check that the client has provided a buffer of sufficient size
	if(aReadBuffer->MaxSize() < bytesToRead)
		{
		return KErrOverflow;
		}
	
	//check that the client has specified a valid start offset
	else if(aFilePathOffsets.iStartOffset >= (TUint)iTotalLength)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_TTRANSPARENTSMARTCARDFILE_READFILE_1, "ERROR: File start offset information invalid.");
		return KErrMMEtelScReferencedDataNotFound;	
		}
	
	//check the client is not requesting to read too many bytes, resulting in reading
	//off the end of the file
	else if((bytesToRead + filePointer) > iTotalLength)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_TTRANSPARENTSMARTCARDFILE_READFILE_2, "ERROR: Path offset information results in reading off the end of the file");
		return KErrMMEtelScEofReached;
		}
	else
		{
		aReadBuffer->SetLength(bytesToRead);
		aReadBuffer->FillZ();
		
		TInt index;
		
		for(index = 0; index < bytesToRead; index++)
			{
			(*aReadBuffer)[index] = iFileData[filePointer];
			filePointer++;
			}
		return KErrNone;
		}
	}
	
TInt CSimPhoneSmartCardApp::TTransparentSmartCardFile::UpdateFile(
		const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aWriteBuffer)
/**
* Concrete implementation of the UpdateFile() method for updating data in a binary transparent
* smart card file.
*
* @param aFilePathOffsets Information about which file to update (path) and where in the file to write data to (offsets)
* @param aReadBuffer The data with which the file should be updated.
*/
	{
	const TInt& bytesToWrite = aFilePathOffsets.iCount;
	TInt filePointer = aFilePathOffsets.iStartOffset;
	TInt index;

	//check that the client has provided a buffer of sufficient size
	if(aWriteBuffer->Size() != bytesToWrite)
		{
		//The length of the provided buffer does not match the number of bytes that client has requested
		//should be updated in the file.
		return KErrArgument;
		}

	//check that the client has specified a valid start offset
	else if(aFilePathOffsets.iStartOffset >= (TUint)iTotalLength)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_TTRANSPARENTSMARTCARDFILE_UPDATEFILE_1, "ERROR: File start offset information invalid.");
		return KErrMMEtelScReferencedDataNotFound;	
		}
	
	//check the client is not requesting to write too many bytes, resulting in writing
	//off the end of the file
	else if((bytesToWrite + filePointer) > iTotalLength)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_TTRANSPARENTSMARTCARDFILE_UPDATEFILE_2, "ERROR: Path offset information results in reading off the end of the file");
		return KErrMMEtelScEofReached;
		}
	else
		{
		for(index = 0; index < bytesToWrite; index++)
			{
			iFileData[filePointer] = (*aWriteBuffer)[index];
			filePointer++;
			}
			
		return KErrNone;
		}
	}
	
RMobilePhone::TAID CSimPhoneSmartCardApp::GetActiveUSim()
/**
* Returns the active aid (internally)
*
*/
	{
	return iActiveUSimApp;
	}
	
TInt CSimPhoneSmartCardApp::GetCurrentActiveUSimApplication(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID)
/**
* Returns the active aid
*
* @param aReqHandle	The TSY request handle for this request.
* @param aAID	Returns the AID of the Active USim Application
* @return KErrNone
*/
	{
	if(iActiveUSimApp.Size() == 0)
		{
		iPhone->ReqCompleted(aTsyReqHandle, KErrMMEtelScApplicationNotActive);
		}
	else
		{
		*aAID=iActiveUSimApp;
		iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
		}
	return KErrNone;	
	}
	
TInt CSimPhoneSmartCardApp::GetCurrentActiveUSimApplicationCancel(const TTsyReqHandle aTsyReqHandle)	
/**
* Attempts to cancel a pending GetActiveUSimApplication request (unsuccessfully)
*
*/
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;		
	}
	

TInt CSimPhoneSmartCardApp::EnumerateUSimApplications(TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID)
/**
* Enumerates the number of USim applications in the UICC and returns the
* AID of the active application.
*
* @param aTsyReqHandle A handle belonging to the client that created the
*                    session with this TSY server.
* @param aCount Returns the number of USim applications.
* @param aActiveAID Returns the AID of the currently active application.
* @return KErrNone.
*/
	{
	*aCount=iUSimAppList->Count();
	aActiveAID->Copy(iActiveUSimApp);
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


TInt CSimPhoneSmartCardApp::EnumerateUSimApplicationsCancel(TTsyReqHandle aTsyReqHandle)
/**
*Attempts to cancel a pending EnumerateUSimApplications request (unsuccesfully).
*
*@param aTsyReqHandle A handle belonging to the client that created the
*                     session with this TSY server.
*/
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


TInt CSimPhoneSmartCardApp::GetUSimApplicationInfo(TTsyReqHandle aTsyReqHandle, TInt* aUSimAppIndex, TDes8* aInfo)
/**
* Retrieves information for a specified USim application
*
* @param aTsyReqHandle A handle belonging to the client that created the
*                      session with this TSY server.
* @param aUSimAppIndex Used to specify the index of the application.
* @param aInfo Returns the information of the specified appplication.
* @return KErrNone.
*/	
	{
	if(*aUSimAppIndex>=iUSimAppList->Count())
		{
		iPhone->ReqCompleted(aTsyReqHandle, KErrNotFound);
		return KErrNone;
		}
	else
		{
		TPckg<RMobilePhone::TUSimApplicationInfoV2>* infoPckg = 
			(TPckg<RMobilePhone::TUSimApplicationInfoV2>*)aInfo;
			
		RMobilePhone::TUSimApplicationInfoV2& info = (*infoPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(info);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aTsyReqHandle, err);
			return KErrNone;
			}
	
		info.iAID = (*iUSimAppList)[*aUSimAppIndex]->iAID;
		info.iLabel = (*iUSimAppList)[*aUSimAppIndex]->iLabel;
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
TInt CSimPhoneSmartCardApp::GetUSimApplicationInfoCancel(TTsyReqHandle aTsyReqHandle)
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


TInt CSimPhoneSmartCardApp::SetUSimApplicationStatus(TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TUSimAppAction* aUsimAppAction)
/**
* Attempts to set the status of a specific USim application.
*
* @param aTsyReqHandle A handle belonging to the client that created the
*                      session with this TSY server.
* @param aAID The specific application's ID whose status is to be set.
* @param aUsimAppAction The status to apply to the application identified
*                       by aAID.
* @return KErrNone on completion of request, unless the aAID given is not
*         identifiable, in which case KErrArgument is returned.
*/
	{
	TInt ret;
	RMobilePhone::TAID aAidToAction;
	RMobilePhone::TSmartCardApplicationEvent aScEvent;
	TBool isUSimAppAffected = EFalse;
	
	//if the request is to activate a USimApp
	if(*aUsimAppAction == RMobilePhone::EUSimActivate)
		{
		//if another USimApp is active. 
		if(iActiveUSimApp.Size() != 0)
			{
			//terminate active USimApp
			aAidToAction = iActiveUSimApp;
			aScEvent = RMobilePhone::EScApplicationTerminated;
			isUSimAppAffected = EFalse;
			ret = TerminateSmartCardApp(aAidToAction, isUSimAppAffected);

			//if No error terminating
			if (ret == KErrNone)
				{			
				//notify ScApp client if notify request is pending
				if(iNotifyScInfo.iNotifyPending)
					{
					*(RMobilePhone::TAID*)iNotifyScInfo.iNotifyData1=aAidToAction;
					*(RMobilePhone::TSmartCardApplicationEvent*)iNotifyScInfo.iNotifyData2=aScEvent;
					iNotifyScInfo.iNotifyPending=EFalse;
					iPhone->ReqCompleted(iNotifyScInfo.iNotifyHandle,KErrNone);				
					}
				}
			}//end if(iActiveUSimApp.Size() != 0)
			
		//activate the requested application
		aAidToAction = *aAID;
		aScEvent = RMobilePhone::EScApplicationActivated;
		isUSimAppAffected = EFalse;
		ret=ActivateSmartCardApp(*aAID, isUSimAppAffected);

		//if no error activating
		if (ret == KErrNone)
			{
			//notify any ScApp client if such notify request is pending
			if(iNotifyScInfo.iNotifyPending)
				{
				iNotifyScInfo.iNotifyPending=EFalse;
				*(RMobilePhone::TAID*)iNotifyScInfo.iNotifyData1=aAidToAction;
				*(RMobilePhone::TSmartCardApplicationEvent*)iNotifyScInfo.iNotifyData2=aScEvent;
				iPhone->ReqCompleted(iNotifyScInfo.iNotifyHandle,KErrNone);
				}
			//notify any USimApp client if such notify request is pending
			if(iNotifyUSimInfo.iNotifyPending)
				{
				iNotifyUSimInfo.iNotifyPending=EFalse;
				*(RMobilePhone::TAID*)iNotifyUSimInfo.iNotifyData1=aAidToAction;
				*(TInt*)iNotifyUSimInfo.iNotifyData2=iUSimAppList->Count();
				iPhone->ReqCompleted(iNotifyUSimInfo.iNotifyHandle,KErrNone);
				}
			}
			
		//complete requests and return
		iPhone->ReqCompleted(aTsyReqHandle,ret);
		return KErrNone;
			
		}
	
	else
		{
		//the request is to terminate a USimApp
		aAidToAction = *aAID;
		aScEvent = RMobilePhone::EScApplicationTerminated;	

		//terminate active USimApp
		isUSimAppAffected = EFalse;
		ret = TerminateSmartCardApp(aAidToAction, isUSimAppAffected);

		//if No error terminating
		if (ret == KErrNone)
			{			
			//notify ScApp client if such notify request is pending
			if(iNotifyScInfo.iNotifyPending)
				{
				*(RMobilePhone::TAID*)iNotifyScInfo.iNotifyData1=aAidToAction;
				*(RMobilePhone::TSmartCardApplicationEvent*)iNotifyScInfo.iNotifyData2=aScEvent;
				iNotifyScInfo.iNotifyPending=EFalse;
				iPhone->ReqCompleted(iNotifyScInfo.iNotifyHandle,KErrNone);				
				}
			//notify any USimApp client if such notify request is pending
			if(iNotifyUSimInfo.iNotifyPending)
				{
				*(RMobilePhone::TAID*)iNotifyUSimInfo.iNotifyData1=_L8("");
				*(TInt*)iNotifyUSimInfo.iNotifyData2=iUSimAppList->Count();
				iNotifyUSimInfo.iNotifyPending=EFalse;
				iPhone->ReqCompleted(iNotifyUSimInfo.iNotifyHandle,KErrNone);
				}			
			}
		//complete requests and return
		iPhone->ReqCompleted(aTsyReqHandle,ret);
		return KErrNone;
		}
	}


TInt CSimPhoneSmartCardApp::SetUSimApplicationStatusCancel(TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending SetUSimApplicationStatus request (unsuccesfully).
*
* @param aTsyReqHandle A handle belonging to the client that created the
*                      session with this TSY server.
* @return KErrNone.
*/
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


TInt CSimPhoneSmartCardApp::NotifyUSimApplicationsInfoChange(const TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID)
/**
* Notifies the client when the specified USim application's information
* changes.
*
* @param aTsyReqHandle A handle belonging to the client that created the
*                      session with this TSY server.
* @param aCount Returns the total number of USim applications when the
*               request completes.
* @param aActiveAID The application's AID whose infromation change will
*                   be notified to the client.
* @return KErrNone.
*/
	{
	__ASSERT_ALWAYS(!iNotifyUSimInfo.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));

	TInt count=iSmartCardAppEventList->Count();

	if(iEventPointer<count)
		{
		if(iTimerStarted)
			{
			delete iTimer;
			iTimer = NULL;
			TRAP_IGNORE(iTimer=CSimTimer::NewL(iPhone));
			}

		TSmartCardAppEvent event=iSmartCardAppEventList->At(iEventPointer);
		
		if (iTimer != NULL)
			{
			iTimer->Start(event.iDuration,this);
			iTimerStarted = ETrue;
			}
		}

	iNotifyUSimInfo.iNotifyPending=ETrue;
	iNotifyUSimInfo.iNotifyHandle=aTsyReqHandle;
	iNotifyUSimInfo.iNotifyData1=aActiveAID;
	iNotifyUSimInfo.iNotifyData2=aCount;		
	return KErrNone;
	}


/**
*Attempts to cancel a pending NotifyUSimApplicationsInfoChange request.
*
*@param aTsyReqHandle A handle belonging to the client that created the
*                     session with this TSY server.
*@param KErrNone
*/
TInt CSimPhoneSmartCardApp::NotifyUSimApplicationsInfoChangeCancel(TTsyReqHandle aTsyReqHandle)
	{
	if(iNotifyUSimInfo.iNotifyPending)
		{
		iNotifyUSimInfo.iNotifyPending=EFalse;
		iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}
	

TInt CSimPhoneSmartCardApp::EnumerateScApplications(const TTsyReqHandle aTsyReqHandle, TUint* aCount)
/**
* Enumerates the number of Smart Card applications on the UICC
*
* @param aReqHandle	The TSY request handle for this request
* @param aCount	Returns the number of Smart Card applications
* @return KErrNone
*/

	{
	*aCount=iSmartCardAppList->Count();
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimPhoneSmartCardApp::EnumerateScApplicationsCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending EnumerateUSimApplications request (unsuccessfully)
*
* @param aReqHandle	The TSY request handle for this request
*/
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimPhoneSmartCardApp::GetScApplicationInfo(const TTsyReqHandle aTsyReqHandle, TInt* aAppIndex, TDes8* aInfo)
/**
* Retrieves information for a specified Smart Card application
*
* @param aReqHandle	The TSY request handle for this request
* @param aAppIndex	is used to specify the index of the application 
* @param aInfo	Returns the information of the specified appplication
* @return KErrNone
*/
	{
	if(*aAppIndex>=iSmartCardAppList->Count())
		{
		iPhone->ReqCompleted(aTsyReqHandle, KErrNotFound);
		}
	else
		{
		TPckg<RMobilePhone::TSmartCardApplicationInfoV5>* infoPckg = 
			(TPckg<RMobilePhone::TSmartCardApplicationInfoV5>*)aInfo;

		RMobilePhone::TSmartCardApplicationInfoV5& info = (*infoPckg)();

		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(info);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aTsyReqHandle, err);
			return KErrNone;
			}

		info.iAID=iSmartCardAppList->At(*aAppIndex).iAID;
		info.iLabel=iSmartCardAppList->At(*aAppIndex).iLabel;
		if (info.ExtensionId() == KEtelExtMultimodeV6)
			{
			RMobilePhone::TUiccApplicationEapInfoV6& withEapInfo = static_cast<RMobilePhone::TUiccApplicationEapInfoV6&>(info);
			withEapInfo.iEapTypeList = iSmartCardAppList->At(*aAppIndex).iEapList;
			}
		iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
		}
	
	return KErrNone;
	}

TInt CSimPhoneSmartCardApp::GetScApplicationInfoCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending GetScApplicationInfo request (unsuccessfully)
*
*/
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimPhoneSmartCardApp::NotifyScApplicationInfoChange(const TTsyReqHandle aTsyReqHandle, 
		RMobilePhone::TAID* aAID, RMobilePhone::TSmartCardApplicationEvent* aEvent)
/**
* Notifies of any Smart Card application information changes
*
* @param aReqHandle	The TSY request handle for this request
* @param aAID				Return the AID of the application whose status has changed 
* @param aEvent				Return the event that took place on application aAID
* @return KErrNone
*/

	{		
	__ASSERT_ALWAYS(!iNotifyScInfo.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	
	TInt count=iSmartCardAppEventList->Count();

	if(iEventPointer<count)
		{
		if(iTimerStarted)
			{
			delete iTimer;
			iTimer = NULL;
			TRAP_IGNORE(iTimer=CSimTimer::NewL(iPhone));
			}
		
		TSmartCardAppEvent event=iSmartCardAppEventList->At(iEventPointer);
		
		if (iTimer != NULL)
			{
			iTimer->Start(event.iDuration,this);
			iTimerStarted = ETrue;
			}
		}
	iNotifyScInfo.iNotifyPending=ETrue;
	iNotifyScInfo.iNotifyHandle=aTsyReqHandle;
	iNotifyScInfo.iNotifyData1=aAID;
	iNotifyScInfo.iNotifyData2=aEvent;
	return KErrNone;
	}

TInt CSimPhoneSmartCardApp::NotifyScApplicationInfoChangeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending NotifyUSimApplicationsInfoChange request
*
*/
	{
	if(iNotifyScInfo.iNotifyPending)
		{
		iNotifyScInfo.iNotifyPending=EFalse;
		iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}
	

TInt CSimPhoneSmartCardApp::SetScApplicationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TSmartCardApplicationAction* aAction)
/**
* Sets the Application Status for specified smart card application
* 
* @param aReqHandle	The TSY request handle for this request
* @param aAID		The AID of the application whose status should be changed
* @param aAction	The action that should take place on the specified application
* @return KErrNone
*/
	{
	TInt ret;
	RMobilePhone::TSmartCardApplicationEvent event;
	TBool isUSimAppAffected = EFalse;
	
	if(*aAction==RMobilePhone::EScApplicationActivate)
		{
		event=RMobilePhone::EScApplicationActivated;
		ret=ActivateSmartCardApp(*aAID, isUSimAppAffected);
		}
	else
		{
		event=RMobilePhone::EScApplicationTerminated;
		ret=TerminateSmartCardApp(*aAID, isUSimAppAffected);
		}
		
	if(ret==KErrNone)
		{
		if(iNotifyScInfo.iNotifyPending)
			{
			iNotifyScInfo.iNotifyPending=EFalse;
			*(RMobilePhone::TAID*)iNotifyScInfo.iNotifyData1=*aAID;
			*(RMobilePhone::TSmartCardApplicationEvent*)iNotifyScInfo.iNotifyData2=event;
			iPhone->ReqCompleted(iNotifyScInfo.iNotifyHandle,KErrNone);
			}
		
		//If the affected ScApp is a USimApp...
		//notify any USimApp client if such notify request is pending
		if(isUSimAppAffected  && iNotifyUSimInfo.iNotifyPending)
			{
			iNotifyUSimInfo.iNotifyPending=EFalse;

			if(event == RMobilePhone::EScApplicationActivated)
				{
				*(RMobilePhone::TAID*)iNotifyUSimInfo.iNotifyData1=*aAID;
				}
			else
				{
				*(RMobilePhone::TAID*)iNotifyUSimInfo.iNotifyData1=_L8("");
				}

			*(TInt*)iNotifyUSimInfo.iNotifyData2=iUSimAppList->Count();
			iPhone->ReqCompleted(iNotifyUSimInfo.iNotifyHandle,KErrNone);
			}
		}

	iPhone->ReqCompleted(aTsyReqHandle,ret);
	return KErrNone;
	}

TInt CSimPhoneSmartCardApp::SetScApplicationStatusCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending SetScApplicationStatus request (unsuccessfully)
*
*/
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimPhoneSmartCardApp::SetUSimAppsSelectionMode(TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode)
/**
* Sets the Selection mode for USim applications
*
* @param aReqHandle	The TSY request handle for this request
* @param aUSimSelectionMode	is used to specify the new selection mode for applications
* @return KErrNone
*/	
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
	
TInt CSimPhoneSmartCardApp::SetUSimAppsSelectionModeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending SetUSimAppsSelectionMode request (unsuccessfully)
*
*/
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}	

TInt CSimPhoneSmartCardApp::GetUSimAppsSelectionMode(TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode)
/**
* Gets the Selection mode for USim applications
*
* @param aReqHandle	The TSY request handle for this request
* @param aUSimSelectionMode	Returns the selection mode for applications
* @return KErrNone
*/	
	{
	*aUSimSelectionMode=iCurrentSelection;
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}
	
TInt CSimPhoneSmartCardApp::GetUSimAppsSelectionModeCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending GetUSimAppsSelectionMode request (unsuccessfully)
*
*/
	{
	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}

TInt CSimPhoneSmartCardApp::NotifyUSIMAppsSelectionModeChange(TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode)
/**
* Notifies for any Selection mode changes for USim applications
*
* @param aReqHandle	The TSY request handle for this request
* @param aUSimSelectionMode	Returns the new selection mode for applications
* @return KErrNone
*/	
	{
	__ASSERT_ALWAYS(!iNotifyMode.iNotifyPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iNotifyMode.iNotifyPending=ETrue;
	iNotifyMode.iNotifyHandle=aTsyReqHandle;
	iNotifyMode.iNotifyData=aUSimSelectionMode;
	return KErrNone;
	}

TInt CSimPhoneSmartCardApp::NotifyUSIMAppsSelectionModeChangeCancel(TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending NotifyUSIMAppsSelectionModeChange request
*
*/
	{
	if(iNotifyMode.iNotifyPending)
		{
		iNotifyMode.iNotifyPending=EFalse;
		iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
		}
	return KErrNone;
	}

	
TInt CSimPhoneSmartCardApp::GetScFileInfo(const TTsyReqHandle aTsyReqHandle, 
		const RMobilePhone::TScFilePath& aFilePathId, TDes8* aInfo)
/**
* Gets information about the specified smart card file
*
* @param aReqHandle	The TSY request handle for this request
* @param aFilePathId Used to specify path of the file for which information should be retrieved
* @param aInfo Used to return the retrieved file information
*/
	{
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_GETSCFILEINFO_1, "CSimPhoneSmartCardApp::GetScFileInfo called");

	RMobilePhone::TScFileInfoV5Pckg *scFileInfoPckg = reinterpret_cast<RMobilePhone::TScFileInfoV5Pckg*>(aInfo);
	RMobilePhone::TScFileInfoV5 &scFileInfo = (*scFileInfoPckg)();

	// Check that the data structure is supported by the simulated TSY version
	TInt err = iPhone->CheckSimTsyVersion(scFileInfo);
	if(err != KErrNone)
		{
		iPhone->ReqCompleted(aTsyReqHandle, err);
		return KErrNone;
		}
		
	if(scFileInfo.ExtensionId() == KEtelExtMultimodeV5)
		{
		TSmartCardFile* scFile = NULL;
		TInt ret = FindScFile(aFilePathId, scFile);
		if(ret==KErrNone)
			{
			scFile->GetFileInfo(scFileInfo);
			iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else
			{
			iPhone->ReqCompleted(aTsyReqHandle,ret);
			}
		}

	else
		{
		iPhone->ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}	
	
	return KErrNone;
	}


TInt CSimPhoneSmartCardApp::GetScFileInfoCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending GetScFileInfo request (unsuccessfully)
*
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_GETSCFILEINFOCANCEL_1, "CSimPhoneSmartCardApp::GetScFileInfoCancel called");

	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


TInt CSimPhoneSmartCardApp::ReadScFile(const TTsyReqHandle aTsyReqHandle, 
		const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aReadBuffer)
/**
* Reads a specified amount of data from a specified smart card file
*
* @param aReqHandle	The TSY request handle for this request
* @param aFilePathOffsets Used to specify which file to read (path) and where in the file to read data from (offsets)
* @param aReadBuffer Used to return the read file data
*/
	{
	TSmartCardFile* scFile = NULL;
	TInt ret = FindScFile((RMobilePhone::TScFilePath)aFilePathOffsets, scFile);
	if(ret==KErrNone)
		{
		ret = scFile->ReadFile(aFilePathOffsets, aReadBuffer);
		}
	
	iPhone->ReqCompleted(aTsyReqHandle,ret);
	return KErrNone;
	}


TInt CSimPhoneSmartCardApp::ReadScFileCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending ReadScFileInfo request (unsuccessfully)
*
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_READSCFILECANCEL_1, "CSimPhoneSmartCardApp::ReadScFileInfoCancel called");

	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


TInt CSimPhoneSmartCardApp::UpdateScFile(const TTsyReqHandle aTsyReqHandle, 
		const RMobilePhone::TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8* aWriteBuffer)
/**
* Writes a specified amount of data to a specified smart card file
*
* @param aReqHandle	The TSY request handle for this request
* @param aFilePathOffsets Used to specify which file to update (path) and where in the file to write data to (offsets)
* @param aReadBuffer Used to return the read file data
*/
	{
	TSmartCardFile* scFile = NULL;
	TInt ret = FindScFile((RMobilePhone::TScFilePath)aFilePathOffsets, scFile);
	if(ret==KErrNone)
		{
		ret = scFile->UpdateFile(aFilePathOffsets, aWriteBuffer);
		}
	
	iPhone->ReqCompleted(aTsyReqHandle,ret);
	return KErrNone;
	}


TInt CSimPhoneSmartCardApp::UpdateScFileCancel(const TTsyReqHandle aTsyReqHandle)
/**
* Attempts to cancel a pending UpdateScFileInfo request (unsuccessfully)
*
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_UPDATESCFILECANCEL_1, "CSimPhoneSmartCardApp::UpdateScFileInfoCancel called");

	iPhone->ReqCompleted(aTsyReqHandle, KErrNone);
	return KErrNone;
	}


void CSimPhoneSmartCardApp::ParseSmartCardApplicationInfoL()
/**
* Parses all Smart Card Application related tags from the config file
*
*/
	{
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;
	TInt index;
	TPtrC8 activeUSimApp, aid, label, eap;
	TInt type;
	
	TSmartCardApplication appInfo;
	
	TRAP_IGNORE(iTimer=CSimTimer::NewL(iPhone));
	

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_1, "Starting to Parse MaxActiveSmartCardApp Info");
	iMaxActiveSmartCardApps = CfgFile()->ItemValue(KMaxActiveSmartCardApps ,KDefaultMaxActiveSmartCardApps);


	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_2, "Starting to Parse ActiveUSIMApp Info");
	activeUSimApp.Set(CfgFile()->ItemValue(KActiveUSIMApp, KEmptyString));
	if(activeUSimApp.Length() == 0)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_3, "ERROR IN CONFIGURATION FILE PARSING: ACTIVE USIM APP NOT SPECIFIED!");
		}


	TInt count = CfgFile()->ItemCount(KSmartCardAppInfo);
	iSmartCardAppList=new(ELeave) CArrayFixFlat<TSmartCardApplication>(count+1);

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_4, "Starting to Load and Parse Smart Card Application Info");
	for(index=0;index<count;index++)
		{
		item=CfgFile()->Item(KSmartCardAppInfo,index);
		if(!item)
			{
			break;
			}
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,aid);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_5, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD APP INFO TAG");
			continue;
			}
		else
			{
			appInfo.iAID.Copy(aid);
			}
			
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,label);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_6, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD APP INFO TAG");
			continue;
			}
		else 
			{
			appInfo.iLabel.Copy(label);
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,type);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_7, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD APP INFO TAG");
			continue;
			}
		else
			{
			appInfo.iAppType = (TSmartCardAppType)type;
			}
		
		if((appInfo.iAppType == EUSimApp) && (appInfo.iAID == activeUSimApp))
			{
			//Make this the active USim App
			iActiveUSimApp = appInfo.iAID;
			appInfo.iAppStatus = EActive;
			iNumActiveSmartCardApps++;
			}
		else
			{
			appInfo.iAppStatus = EInactive;
			}

		ret = CTestConfig::GetElement(item->Value(), KStdDelimiter, 3, eap);
		if (ret != KErrNone)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_8, "CONFIGURATION FILE PARSING: BAD OR NO (OPTIONAL) EAP LIST @ SMARTCARD APP INFO TAG [%d]", index);
			_LIT8(emptyEapList, "");
			eap.Set(emptyEapList);
			}
		// Append empty string if no info is given
		appInfo.iEapList.Copy(eap);

		iSmartCardAppList->AppendL(appInfo);
		iFoundScAppTags = ETrue;
		}//end of FOR loop
		
	if(iActiveUSimApp.Length() == 0)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_9, "ERROR IN CONFIGURATION FILE PARSING: AID FOR ACTIVE USIM APP NOT FOUND!");
		}
		

	count=CfgFile()->ItemCount(KSmartCardAppEvent);
	iSmartCardAppEventList=new(ELeave) CArrayFixFlat<TSmartCardAppEvent>(count+1);
	TSmartCardAppEvent appEvent;
	TInt duration, action;
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_10, "Starting to Load and Parse Smart Card Application Event Info");

	for(index=0;index<count;index++)
		{
		item=CfgFile()->Item(KSmartCardAppEvent,index);
		if(!item)
			{
			break;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0, duration);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_11, "ERROR IN CONFIGURATION FILE PARSING: BAD SMART CARD APP EVENT TAG");
			continue;
			}
		else
			{
			appEvent.iDuration=duration;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1, aid);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_12, "ERROR IN CONFIGURATION FILE PARSING; BAD SMART CARD APP EVENT TAG");
			continue;
			}
		else 
			{
			appEvent.iAID.Copy(aid);
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2, action);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDAPPLICATIONINFOL_13, "ERROR IN CONFIGURATION FILE PARSING; BAD SMART CARD APP EVENT TAG");
			continue;
			}
		else
			{
			appEvent.iAction = (RMobilePhone::TSmartCardApplicationAction)action;
			}

		iSmartCardAppEventList->AppendL(appEvent);
		}//end FOR loop	
	}


void CSimPhoneSmartCardApp::ParseSmartCardFileInfoL()
/**
* Parses all Smart Card File related tags from the config file
*
*/
	{
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;
	TPtrC8 fileId, filePath, fileData;
	TInt aidIndex, fileType, fileLength, recordLength, numRecords, index;
	TSmartCardFile* file = NULL;
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_1, "Starting to Parse Smart Card File Info");
	TInt count = CfgFile()->ItemCount(KSmartCardFile);
	iSmartCardFileList = new(ELeave) CArrayPtrFlat<TSmartCardFile>(count+1);

	for(index=0;index<count;index++)
		{
		item=CfgFile()->Item(KSmartCardFile,index);
		if(!item)
			{
			break;
			}
			
		//Get the file's type information
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,fileType);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_2, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD FILE TAG");
			continue;
			}
		else
			{
			switch ((RMobilePhone::TScFileType)fileType)
				{
			case RMobilePhone::ELinearFixed:
				file = new(ELeave) TLinearSmartCardFile;
				break;
			case RMobilePhone::ECyclic:
				file = new(ELeave) TCyclicSmartCardFile;
				break;
			case RMobilePhone::ETransparent:
				file = new(ELeave) TTransparentSmartCardFile;
				break;
			default:
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_3, "ERROR IN CONFIGURATION FILE PARSING:");
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_4, "ENCOUNTERED UNKNOWN SMART CARD FILE TYPE.");
				continue;				
				}
			file->iType = (RMobilePhone::TScFileType)fileType;
			}

		//Get the FID of the file
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,fileId);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_5, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD FILE TAG");
			delete file;
			continue;
			}
		//check that the FID is precisely 4 bytes long	
		else if(fileId.Length() != KMaxHexFIDLen)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_6, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD FILE FID");
			delete file;
			continue;
			}
		else
			{
			TBuf8<KMaxBinFIDLen> fileIdBuffer;
			TextToBin(fileId, fileIdBuffer);
			file->iFileID = (RMobilePhone::TScFileID)((fileIdBuffer[0] <<8) + (fileIdBuffer[1]));
			}
			
		//Get the Index of the AID of the application the file belongs to
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,aidIndex);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_7, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD FILE TAG");
			delete file;
			continue;
			}
		else
			{
			file->iAIDIndex = aidIndex;
			}
			
		//Get the file's path information
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,filePath);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_8, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD FILE TAG");
			delete file;
			continue;
			}
		else
			{
			file->iPath = filePath;
			}
		
		//Get the information about the total length of the file
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,fileLength);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_9, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD FILE TAG");
			delete file;
			continue;
			}
		else
			{
			file->iTotalLength = fileLength;
			}
			
		//Get information about the length of each record in the file
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,5,recordLength);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_10, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD FILE TAG");
			delete file;
			continue;
			}
		else
			{
			file->iRecordLength = recordLength;
			}
			
		//Get information about the number of records in the file
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,6,numRecords);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_11, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD FILE TAG");
			delete file;
			continue;
			}
		else
			{
			file->iNumberRecords = numRecords;
			}
		
		//If the file is Cyclic or Linear record based file...
		//Check that record Length * number of records = Total length of file
		if((fileType != RMobilePhone::ETransparent) && ((recordLength*numRecords) != fileLength))
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_12, "ERROR IN CONFIGURATION FILE PARSING:...");
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_13, "...Record Length * Number of Records != File Length");
			delete file;
			continue;
			}
			
		//Get file data
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,7,fileData);
		if(ret!=KErrNone)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_14, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD FILE TAG");
			delete file;
			continue;
			}
		else if((fileData.Length() % 2 ) != 0)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_15, "ERROR IN CONFIGURATION FILE PARSING: BAD SMARTCARD FILE DATA");
			continue;
			}
		else
			{				
			TBuf8<KMaxFileSize> fileDataBuffer;
			TextToBin(fileData, fileDataBuffer);
			if(fileDataBuffer.Length() != fileLength)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_PARSESMARTCARDFILEINFOL_16, "ERROR IN CONFIGURATION FILE PARSING:INCONSISTANT DATA FILE LENGTH");
				delete file;
				continue;		
				}
			file->iFileData = fileDataBuffer;
			}		
		
		iSmartCardFileList->AppendL(file);	
		}//end FOR Loop
	}
	
void CSimPhoneSmartCardApp::MapUSimAppsL()
	{
	iUSimAppList=new(ELeave) RPointerArray<TSmartCardApplication>(iSmartCardAppList->Count() + 1);
	TInt index;
	TSmartCardApplication* SmartCardApp = NULL;
	
	//iterate through the list of smart card applications
	for(index=0; index < iSmartCardAppList->Count(); index++)
		{
		SmartCardApp = &(iSmartCardAppList->At(index));
		
		//if the smart card application is a USim App
		if(SmartCardApp->iAppType == EUSimApp)
			{
			//add a pointer to it in the list of USim Apps
			iUSimAppList->AppendL(SmartCardApp);
			}
		}//end FOR loop

	}
	
	
TInt CSimPhoneSmartCardApp::FindScFile(const RMobilePhone::TScFilePath& aFilePathInfo, TSmartCardFile*& aSmartCardFile)
/**
 * Helper function
 *
 * Searches through the SimTSYs internal list of Smart Card files and returns a pointer
 * to the requested file. If the file is under an application a check is done to ensure
 * the application is active.
 *
 * @param aFilePathInfo Some information about the file that the client is looking for
 * @param aSmartCardFile A pointer to the Smart Card File the client was looking for
 */
	{
	TInt index;
	TInt aidIndex = KNoAID;
	
	//If the user has specified a file that is under an application
	if(aFilePathInfo.iAID.Size() != 0)
		{
		//search the list of Smart Card Applications and get an index for the specifed AID
		for(index=0; index < iSmartCardAppList->Count(); index++)
			{
			if(iSmartCardAppList->At(index).iAID == aFilePathInfo.iAID)
				{
				aidIndex = index;
				break;
				}
			}
			
		//if the AID is not found
		if(aidIndex == KNoAID)
			{
			return KErrNotFound;
			}
		else
			{
			//return an error if the specified application is inactive
			if(iSmartCardAppList->At(aidIndex).iAppStatus == EInactive)
				{
				return KErrMMEtelScApplicationNotActive;
				}
			}
		}//end if(aFilePathInfo.iAID.Size() != 0)
	
	//Find the specified file
	TSmartCardFile* scFile=NULL;
		
	for(index = 0; index < iSmartCardFileList->Count(); index++)
		{
		scFile = iSmartCardFileList->At(index);
		if((scFile->iAIDIndex == aidIndex) && (scFile->iPath == aFilePathInfo.iPath))
			{
			aSmartCardFile = scFile;
			return KErrNone;
			}
		}
		
	return KErrMMEtelScFileNotFound;
	}


TInt CSimPhoneSmartCardApp::ActivateSmartCardApp(const RMobilePhone::TAID aAID, TBool& aActivatedUSimApp)
/**
* Performs the logic for Activating a Smart Card application
*
* @param aAID The AID of the application that should be activated
*/
	{
	TInt index;
	TSmartCardApplication* smartCardApp = NULL;

	for(index=0; index < iSmartCardAppList->Count(); index++)
		{
		smartCardApp = &(iSmartCardAppList->At(index));
		if(smartCardApp->iAID == aAID)	
			{
			//Check that 1)iMaxActiveSmartCardApps will not be exceeded by activating
			//a new smart card application; 2)The application is not already active
			if((iNumActiveSmartCardApps<iMaxActiveSmartCardApps) &&
				(smartCardApp->iAppStatus != CSimPhoneSmartCardApp::EActive))
				{
				//if we are activating a USIM and another USIM app is already active,
				//fail with KErrAlreadyExists
				if((smartCardApp->iAppType == CSimPhoneSmartCardApp::EUSimApp) &&
					(iActiveUSimApp.Length() != 0))
					{
					return KErrAlreadyExists;
					}
				else
					{
					smartCardApp->iAppStatus = CSimPhoneSmartCardApp::EActive;
					iNumActiveSmartCardApps++;

					if(smartCardApp->iAppType == CSimPhoneSmartCardApp::EUSimApp)
						{
						iActiveUSimApp = smartCardApp->iAID;
						aActivatedUSimApp = ETrue;
						}
					
					return KErrNone;
					}
				}

			else
				{
				if(smartCardApp->iAppStatus == CSimPhoneSmartCardApp::EActive)
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_ACTIVATESMARTCARDAPP_1, "REACTIVATING AN ALREADY ACTIVE APPLICATION");
					return KErrNone;
					}
				else
					{
					//iMaxActiveSmartCardApps will be exceeded
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_ACTIVATESMARTCARDAPP_2, "WARNING: CANNOT PROCESS ACTIVATING APPLICATION");
					return KErrMMEtelScMaxApplicationsActive;
					}
				}

			}
		}//end for loop
		
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_ACTIVATESMARTCARDAPP_3, "WARNING: AID NOT FOUND. CANNOT PROCESS EVENT");
		return KErrNotFound;
	}


TInt CSimPhoneSmartCardApp::TerminateSmartCardApp(const RMobilePhone::TAID aAID, TBool& aTerminatedUSimApp)
/**
* Performs the logic for terminating a Smart Card application
*
* @param aAID The AID of the application that should be terminated
*/
	{
	TInt index;
	TSmartCardApplication* smartCardApp = NULL;
	for(index=0; index < iSmartCardAppList->Count(); index++)
		{
		smartCardApp = &(iSmartCardAppList->At(index));
		if(smartCardApp->iAID == aAID)	
			{	
			if(smartCardApp->iAppStatus == CSimPhoneSmartCardApp::EActive)
				{
				smartCardApp->iAppStatus = CSimPhoneSmartCardApp::EInactive;
				iNumActiveSmartCardApps--;
				
				if(smartCardApp->iAppType == CSimPhoneSmartCardApp::EUSimApp)
					{
					iActiveUSimApp.Zero();
					aTerminatedUSimApp = ETrue;
					}
				
				return KErrNone;
				}
			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_TERMINATESMARTCARDAPP_1, "ATTEMPTED TO TERMINATE NON-ACTIVE APPLICATION");
				return KErrMMEtelScApplicationNotActive;
				}
			}
	
		}//end for loop
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPHONESMARTCARDAPP_TERMINATESMARTCARDAPP_2, "WARNING: AID NOT FOUND. CANNOT PROCESS EVENT");
	return KErrNotFound;
	}
	
	
void CSimPhoneSmartCardApp::TextToBin(const TDesC8& aSrc, TDes8& aDst)
/**
 * Helper function
 *
 * Converts a text descriptor containing Hex information to binary.
 *
 * @param aSrc The input buffer
 * @param aDst The output buffer
 */
	{
	//LOG_ASSERT((aSrc.Length() % 2) == 0, "Not passed in an even-length string?");

	aDst.SetLength(aSrc.Length() / 2);

	for (TInt i = 0; i < aSrc.Length(); i += 2)
		{
		TInt val = 0;
		if ((aSrc[i] >= '0') && (aSrc[i] <= '9'))
			{
			val = ((aSrc[i] - '0') << 4);
			}
		else if ((aSrc[i] >= 'A') && (aSrc[i] <= 'F'))
			{
			val = ((aSrc[i] - 'A' + 10) << 4);
			}
		else
			{
			__DEBUGGER();
			}

		if ((aSrc[i+1] >= '0') && (aSrc[i+1] <= '9'))
			{
			val += (aSrc[i+1] - '0');
			}
		else if ((aSrc[i+1] >= 'A') && (aSrc[i+1] <= 'F'))
			{
			val += (aSrc[i+1] - 'A' + 10);
			}
		else
			{
			__DEBUGGER();
			}

		aDst[i/2] = (TUint8) val;
		}
	}
