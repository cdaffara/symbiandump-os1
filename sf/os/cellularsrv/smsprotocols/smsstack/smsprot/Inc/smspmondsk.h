// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for a CActive based class that monitors for low disk space and
// is informed when a PDU is being received. Should memory be too low,
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


#ifndef __SMSPMONDSK_H__
#define __SMSPMONDSK_H__

#include <etelmm.h>
#include <f32file.h>


/**
 *  The class is a simple active object derived state machine that monitors
 *  disk space needed for storage of incoming messages.
 */
class CSmsMonitorDiskSpace : public CActive
	{
public:
	static CSmsMonitorDiskSpace* NewL(MSmsComm& aSmsComm, RMobileSmsMessaging& aSmsMessaging, RFs& aFs);
	~CSmsMonitorDiskSpace();

	void CheckDiskSpaceForPDUL(TBool aPDUIsClass0);

protected:
	void RunL();
	void DoCancel();

private:
	enum TSmsMonitorDiskSpaceState
		{
		ESmsMonitorDiskSpaceIdle,
		ESmsMonitorDiskSpaceMonitorLowLimit,
		ESmsMonitorDiskSpaceMonitorHighLimit,
		ESmsMonitorDiskSpaceResumeReception
		};

	CSmsMonitorDiskSpace(MSmsComm& aSmsComm, RMobileSmsMessaging& aSmsMessaging, RFs& aFs);
	void ConstructL();

	void NotifyDiskSpace(TInt aLimit, TSmsMonitorDiskSpaceState aState);
	void ResumeSmsReception();
	TInt GetFreeDiskSpace();	

	TSmsMonitorDiskSpaceState  iState;
	RMobileSmsMessaging&  iSmsMessaging;
	RFs&  iFs;
    MSmsComm&  iSmsComm;
	TUint  iLowLimit;
	TUint  iHighLimit;
	TBool  iNAckdClassZero;
	TBool  iNAckdClassOther;
	
#ifdef _DEBUG	
	TBool     iInOODTesting ;
	RProperty iFreeDiskSpaceProperty;
#endif
	
	};

#endif // !defined __SMSPMONDSK_H__
