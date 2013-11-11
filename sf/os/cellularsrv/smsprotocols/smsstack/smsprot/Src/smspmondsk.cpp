// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation for a CActive based class that monitors for low disk space
// and is informed when a PDU is being received. Should memory be too low,
// an error code is returned causing a NAck. The class will then monitor
// for the point at which PDUs can be received again, and a Resume Reception
// will be sent to the network.
// The are two limits - a high limit for general SMSs and a low limit for
// class 0 SMSs. However there is only one way to resume reception of SMSs. So
// once a resume reception is sent, it resumes hoth types of SMSs. The algorithm
// is a bit simiplier because of this:
// IF sending non-Class 0 AND we are below the high limit THEN
// N'Ack SMS
// IF not waiting for free diskspace THEN
// Wait for free diskspace to rise above the high limit
// Resume SMS Reception.
// ELSE
// Continue waiting for either high limit or low limit
// Resume SMS Reception as previously intended
// END_IF
// ELSE IF sending Class 0 AND we are below the low limit THEN
// Wait for free diskspace to rise above the low limit
// Resume SMS Reception.
// END_IF
// Note that in the second and last case, SMS reception may be resumed before
// we are above the high limit, but in that case montioring will repeat when
// the next non-class 0 SMS is attempted to be delivered. This is necessary as
// there is no way to resume just class 0 SMSs.
// 
//

/**
 @file
 @internalComponent 
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smspmondskTraces.h"
#endif

#include <barsc.h>
#include <bautils.h>

#include "smspcomm.h"
#include "smspmondsk.h"
#include "smsstacklog.h"
#include "smspmain.h"
#include "smspver.h"
#include <smsu.rsg>


_LIT(KSmsResourceFile, "sms\\smsu.rsc");


/**
 *  2 phase constructor.
 *  
 *  @leave Leaves if not enough memory is available.
 *  
 *  @return a newly created CSmsMonitorDiskSpace object.
 */
CSmsMonitorDiskSpace* CSmsMonitorDiskSpace::NewL(MSmsComm& aSmsComm, RMobileSmsMessaging& aSmsMessaging,RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_NEWL_1, "CSmsMonitorDiskSpace::NewL()");

	CSmsMonitorDiskSpace*  self = new(ELeave) CSmsMonitorDiskSpace(aSmsComm, aSmsMessaging, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	} // CSmsMonitorDiskSpace::NewL


/**
 *  Private constructor used in the first phase of construction.
 */
CSmsMonitorDiskSpace::CSmsMonitorDiskSpace(MSmsComm& aSmsComm, RMobileSmsMessaging& aSmsMessaging,RFs& aFs)
  : CActive(KSmsSessionPriority),
	iState(ESmsMonitorDiskSpaceIdle),
	iSmsMessaging(aSmsMessaging),
	iFs(aFs),
	iSmsComm(aSmsComm),
	iLowLimit(8*1024),
	iHighLimit(16*1024)
	{
	CActiveScheduler::Add(this);
	} // CSmsMonitorDiskSpace::CSmsMonitorDiskSpace


/**
 *  Destructor.  Ensures any outstanding operation is cancelled.
 */
CSmsMonitorDiskSpace::~CSmsMonitorDiskSpace()
	{
    Cancel();

#ifdef _DEBUG    
    iFreeDiskSpaceProperty.Close();
#endif
    
	} // CSmsMonitorDiskSpace::~CSmsMonitorDiskSpace


/**
 *  Initialize values for LowLimitDiskSpace and HighLimitDiskSpace
 *  from resource file
 */
void CSmsMonitorDiskSpace::ConstructL()
	{
	RResourceFile  resourceFile;
	TFileName  fileName;
	Dll::FileName(fileName);
	TParse  parse;
	TFileName  resourceFileName;

	//
	// Create the path to the resource file...
	//
	iFs.PrivatePath(resourceFileName);
	resourceFileName.Append(KSmsResourceFile);
	parse.Set(resourceFileName, &fileName, 0);
	fileName = parse.FullName();
	BaflUtils::NearestLanguageFile(iFs, fileName);

	//
	// To enable adequate CSmsMonitorDiskSpace testing we allow the SMSU.RSC
	// file to be loaded from the C: drive if it exists. This is only for a
	// DEBUG version of course! Later WINSCW is also added to allow the testing
	// on WINSCW UREL version.
	//
#if (defined __WINSCW__) || (defined _DEBUG)
	TUint  attValue;
	TChar  oldDriveLetter = fileName[0];

	//
	// Change the filename to look on C: and then see if it exists...
	//	
	fileName[0] = 'C';

	if (iFs.Att(fileName, attValue) != KErrNone)
		{
		//
		// The file does not exist on C: so revert the change...
		//
		fileName[0] = oldDriveLetter;
		}
#endif

	//
	// Log the filename in use and whether it is ROM based...
	//
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_CONSTRUCTL_1, "CSmsMonitorDiskSpace::ConstructL(): fileName=\"%S\"", fileName);


    if (iFs.IsFileInRom(fileName) == NULL)
    	{
     	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_CONSTRUCTL_2, "CSmsMonitorDiskSpace::ConstructL(): Smsu.rsc not in ROM");
     	}

	//
	// Read the low and high limits...
	//
	TResourceReader  reader;
	HBufC8*  buf;

	resourceFile.OpenL(iFs, fileName);
	CleanupClosePushL(resourceFile);

	buf = resourceFile.AllocReadLC(R_LOWLIMIT_DISKSPACE_HOLDER);
	reader.SetBuffer(buf);
	iLowLimit = reader.ReadInt32();
	CleanupStack::PopAndDestroy(buf);

	buf = resourceFile.AllocReadLC(R_HIGHLIMIT_DISKSPACE_HOLDER);
	reader.SetBuffer(buf);
	iHighLimit=reader.ReadInt32();
	CleanupStack::PopAndDestroy(buf);
	
	CleanupStack::PopAndDestroy(&resourceFile);

#ifdef _DEBUG    
    TInt  freeSpace;
    TInt err = RProperty::Get(KUidPSSMSStackCategory, KUidPSSMSStackFreeDiskSpaceKey, freeSpace);
    if (err == KErrNone)
        {
        iInOODTesting = ETrue;
        
        TInt ret = iFreeDiskSpaceProperty.Attach(KUidPSSMSStackCategory, KUidPSSMSStackFreeDiskSpaceKey);
        if (ret != KErrNone)
            {
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_CONSTRUCTL_3, "iFreeDiskSpaceProperty.Attach(): error=%d", ret);
            User::Leave(ret);
            }                        
        }   
#endif        
    
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_CONSTRUCTL_4, "CSmsMonitorDiskSpace::ConstructL(): iLowLimit=%u, iHighLimit=%u",iLowLimit, iHighLimit);
	} // CSmsMonitorDiskSpace::ConstructL


/**
 *  Handles a completed disk monitor or resume request.
 */
void CSmsMonitorDiskSpace::RunL()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_RUNL_1, "CSmsMonitorDiskSpace:RunL(): iStatus=%d, iState=%d",iStatus.Int(), iState );

	switch (iState)
		{
		case ESmsMonitorDiskSpaceMonitorLowLimit:
			{
		 	iState = ESmsMonitorDiskSpaceIdle;
			
			//
			// If the disk space has gone above the low limit, then
			// resume reception. Otherwise (assuming the notify was
			// successful) then start monitoring the low limit again...
			//
		 	TInt  freeDiskSpace = GetFreeDiskSpace();

			if (freeDiskSpace >= iLowLimit)
				{
				ResumeSmsReception();
				}
			else if (iStatus.Int() == KErrNone)
				{
				NotifyDiskSpace(iLowLimit, ESmsMonitorDiskSpaceMonitorLowLimit);
				}
		 	}
		 	break;

		case ESmsMonitorDiskSpaceMonitorHighLimit:
			{
		 	iState = ESmsMonitorDiskSpaceIdle;
			
			//
			// If the disk space has gone above the high limit, then
			// resume reception. Otherwise (assuming the notify was
			// successful) then start monitoring the high limit again...
			//
		 	TInt  freeDiskSpace = GetFreeDiskSpace();

			if (freeDiskSpace >= iHighLimit)
				{
				ResumeSmsReception();
				}
			else if (iStatus.Int() == KErrNone)
				{
				NotifyDiskSpace(iHighLimit, ESmsMonitorDiskSpaceMonitorHighLimit);
				}
		 	}
		 	break;

		case ESmsMonitorDiskSpaceResumeReception:
			{
			//
			// We are finished. However, check if any N'Acks were sent between
			// the time we began the resume and now incase we need to restart
			// the monitoring.
			//
			if (iNAckdClassZero)
				{
				NotifyDiskSpace(iLowLimit, ESmsMonitorDiskSpaceMonitorLowLimit);
				}
			else if (iNAckdClassOther)
				{
				NotifyDiskSpace(iHighLimit, ESmsMonitorDiskSpaceMonitorHighLimit);
				}
			else
				{
				iState = ESmsMonitorDiskSpaceIdle;
				}
			}
			break;

		default:
			{
		 	SmspPanic(KSmspPanicUnexpectedState);
		 	}
			break;
		}
	} // CSmsMonitorDiskSpace::RunL


/**
 *  Handles a request to cancel the current state machine operation.
 */
void CSmsMonitorDiskSpace::DoCancel()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_DOCANCEL_1, "CSmsMonitorDiskSpace::DoCancel(): iState=%d", iState);

    switch (iState)
		{
		case ESmsMonitorDiskSpaceIdle:
			{
			// NOP
			}
	 		break;

		case ESmsMonitorDiskSpaceMonitorLowLimit:
		case ESmsMonitorDiskSpaceMonitorHighLimit:
			{
#ifdef _DEBUG			
			if (iInOODTesting)
			    {
			    iFreeDiskSpaceProperty.Cancel();			    
			    }
			else
#endif			    
                {
                iFs.NotifyDiskSpaceCancel();
                }
			    			    			    			    
		 	}
		 	break;

		case ESmsMonitorDiskSpaceResumeReception:
			{
		   	iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingResumeSmsReception);
			}
			break;

		default:
			{
		 	SmspPanic(KSmspPanicUnexpectedState);
		 	}
		}

	iState = ESmsMonitorDiskSpaceIdle;
	} // CSmsMonitorDiskSpace::DoCancel


/**
 *  Checks for enough disk space, and starts monitoring disk space if needed.
 */
void CSmsMonitorDiskSpace::CheckDiskSpaceForPDUL(TBool aPDUIsClass0)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_CHECKDISKSPACEFORPDUL_1, "CSmsMonitorDiskSpace::CheckDiskSpaceForPDUL(): aPDUIsClass0=%d",aPDUIsClass0);

	//
	// First check the actual disk space before working out what to do.
	// 
	// If enough space exists, then we don't need to do anything else (even if
	// we where previously monitoring, because that will be handled later as
	// it would without this function call).
	//
	TInt  freeDiskSpace = GetFreeDiskSpace();
	
	if (freeDiskSpace >= iHighLimit  ||
		(freeDiskSpace >= iLowLimit  &&  aPDUIsClass0))
		{
		//
		// There is enough space for the PDU, so nothing more to do.
		//
		return;
		}
	
	//
	// There is not enough diskspace so record that a PDU will be N'Ack'd.
	//
	if (aPDUIsClass0)
		{
		iNAckdClassZero = ETrue;
		}
	else
		{
		iNAckdClassOther = ETrue;
		}
	
	//
	// Ensure we are monitoring the disk space. This will then cause a
	// ResumeReception() to be sent when the disk space increases above the
	// appropriate limit.
	//
    if (freeDiskSpace < iLowLimit)
    	{
    	//
    	// Start montioring the low limit, if not already monitoring or if
    	// we are currently monitoring the high limit.
    	//
		if (iState == ESmsMonitorDiskSpaceIdle  ||
			iState == ESmsMonitorDiskSpaceMonitorHighLimit)
			{
	    	NotifyDiskSpace(iLowLimit, ESmsMonitorDiskSpaceMonitorLowLimit);
	    	}
		}
    else // (freeDiskSpace < iHighLimit)
    	{
    	//
    	// Start monitoring the high limit if not already monitoring...
    	//
		if (iState == ESmsMonitorDiskSpaceIdle)
			{
	    	NotifyDiskSpace(iHighLimit, ESmsMonitorDiskSpaceMonitorHighLimit);
	    	}
    	}

	//
	// Imform the caller that the SMS should be N'Ack'd...
	//
	User::Leave(KErrDiskFull);
	} // CSmsMonitorDiskSpace::CheckDiskSpaceForPDUL


/**
 *  Starts monitoring the free disk space to spot when it rises above the
 *  requested limit.
 *
 *  Unfortunately the File System API does not distiguish between going below
 *  or above a limit, so some checking is needed to ensure that the correct
 *  behaviour is requested, since it is possible in theory for the disk space
 *  to change between the calls.
 *
 *  In theory this is unlikely since the priority of the SMS Stack is higher
 *  but not impossible to happen. We guard against it anyway since the mere
 *  fact it could happen poses questions when the defects come in from handsets.
 *
 *  @note The current disk space is assumed to be below the limit requested.
 *        If it is found to be above, this will cause an imediate completion.
 *
 *  @param aLimit  Limit to monitor.
 *  @param aState  State to transition to when this request is made.
 */
void CSmsMonitorDiskSpace::NotifyDiskSpace(TInt aLimit,
										   TSmsMonitorDiskSpaceState aState)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_NOTIFYDISKSPACE_1, "CSmsMonitorDiskSpace::NotifyDiskSpace(): aLimit=%d, aState=%d",aLimit, aState);

	//
	// Cancel any previously outstanding requests...
	//
	if (iState != ESmsMonitorDiskSpaceIdle)
		{
		Cancel();
		}
		
	//
	// Start the notification...
	//
	iState = aState;

#ifdef _DEBUG	
	if (iInOODTesting)
        {
        iFreeDiskSpaceProperty.Subscribe(iStatus);
        }
    else
#endif                
        {
        iFs.NotifyDiskSpace(aLimit, EDriveC, iStatus);
        }	
	
	iSmsComm.DiskSpaceMonitorStateChange(ESmsDiskSpaceFull);
	SetActive();
	
	//
	// Now, as a sanity check, check the diskspace again. If the disk space
	// has advanced above the limit in the time between the request and now,
	// or since the last call to GetFreeDiskSpace() then the request will
	// either be invalid, or have completed. If either of these are the case
	// we need to control the situation and ensure a completion occurs...
	//
	TInt  freeDiskSpace = GetFreeDiskSpace();
	
	if (freeDiskSpace > aLimit)
		{
		//
		// Cancel the request...
		//
		Cancel();
		
		//
		// Force a fake completion for this state...
		//
		TRequestStatus*  status = &iStatus;

		iState = aState;
		iStatus = KRequestPending;
		SetActive();
		User::RequestComplete(status, KErrNone);
		}
	} // CSmsMonitorDiskSpace::NotifyDiskSpace


/**
 *  Performs the required actions for the case when leaving either the low
 *  limit or the high limit. It doesn't really matter which limit you are
 *  leaving since the resume applies for all SMSs. This gives three scenarios:
 *
 *      1) You leave the high limit. This is not a problem as all SMSs can
 *         now be received.
 *      2) You leave the low limit and go above the high limit. This is also
 *         not a problem as all SMSs can be received.
 *      3) You leave the low limit but stay below the high limit. In this case
 *         the N'Ack'd SMS was class 0 and you can now receive it. A non-class 0
 *         SMS may attempt to be delivered, but monitoring will restart as
 *         before. So again no problem.
 */
void CSmsMonitorDiskSpace::ResumeSmsReception()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_RESUMESMSRECEPTION_1, "CSmsMonitorDiskSpace::ResumeSmsReception");

	//
	// Cancel any previously outstanding requests...
	//
	if (iState != ESmsMonitorDiskSpaceIdle)
		{
		Cancel();
		}

	//
	// Clear the N'Ackd flags...
	//
	iNAckdClassZero = EFalse;
	iNAckdClassOther = EFalse;

	//
	// Start the resume request...
	//
	iState = ESmsMonitorDiskSpaceResumeReception;
	iSmsMessaging.ResumeSmsReception(iStatus);
	iSmsComm.DiskSpaceMonitorStateChange(ESmsDiskSpaceAvailable);
 	SetActive();
	} // CSmsMonitorDiskSpace::ResumeSmsReception


/**
 *  Utilility function to determine amount of free disk space. The actual
 *  free space is limited at 2GB.
 */
TInt CSmsMonitorDiskSpace::GetFreeDiskSpace()
	{
	TVolumeInfo  volumeInfo;
	TInt  ret;
	
#ifdef _DEBUG
    if( iInOODTesting )
        {
        TInt value = 0;
        ret = iFreeDiskSpaceProperty.Get(value);
        
        // update volumeInfo with value regardless of error as error handled later
        volumeInfo.iFree = value;
        }
    else    
#endif                
        {
        ret = iFs.Volume(volumeInfo, EDriveC);
        }
    
	//
	// Get the volume info for drive C:...
	//
    
	if (ret != KErrNone) 
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_GETFREEDISKSPACE_1, "CSmsMonitorDiskSpace::GetFreeDiskSpace(): error=%d", ret);
		return KErrGeneral;
		}

	//
	// Convert the disk space value to a TInt (allowing 2GB limits)...
	//	
	TInt  freeSpace;
	
	if (volumeInfo.iFree >= 0x7fffffff)
		{
		freeSpace = 0x7fffffff;
		}
	else
		{
		freeSpace = (TInt) volumeInfo.iFree;
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMONITORDISKSPACE_GETFREEDISKSPACE_2, "CSmsMonitorDiskSpace::GetFreeDiskSpace(): freeSpace=%d", freeSpace);

	return freeSpace;
	} // CSmsMonitorDiskSpace::GetFreeDiskSpace
