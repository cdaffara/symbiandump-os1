// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_reflocplugin.h"

//Hard coded position data values.  These are the same as the test Integ Module
#define POS_LAT                         49.2
#define POS_LONG                        3.5
#define POS_ALT                         50.0
#define POS_HORZ_ACCURACY               2
#define POS_VERT_ACCURACY               3
#define SPEED                           26.0
#define VERTICAL_SPEED                  20.0
#define HEADING                         25.0


CTe_LbsRefLocationSource* CTe_LbsRefLocationSource::NewL(MLbsRefLocationObserver& aObserver)
	{
	CTe_LbsRefLocationSource* self = new (ELeave) CTe_LbsRefLocationSource(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTe_LbsRefLocationSource::CTe_LbsRefLocationSource(MLbsRefLocationObserver& aObserver)
:CLbsRefLocationSourceBase(aObserver),
iReturnPosition(ETrue)
	{
	}

CTe_LbsRefLocationSource::~CTe_LbsRefLocationSource()
	{
	delete iDelayTimer;
	iTestCaseIdKey.Close();
	}

void CTe_LbsRefLocationSource::ConstructL()
	{
	   BaseConstructL();
	   iTestCaseIdKey.Attach(KSuplProxyPMSuite, KLbsPosSessionInitStepKey, EOwnerThread);
	   iDelayTimer = CTe_DelayTimer::NewL(*this);
	   iDelayTimer->SetDelay(100000);
	}

/**
 * Sets the SLP requestor address
 */
void CTe_LbsRefLocationSource::SetRequestorAddress(const TDesC& aAddress)
	{
	_LIT(KNokiaSuplAddress, "supl.nokia.com");
	//Check to see if the server is a Nokia SUPL server
	if(aAddress.Find(KNokiaSuplAddress) != KErrNone)
		{
		iReturnPosition = EFalse;
		}
	}

/**
 * Makes an asynchronous request for the reference position
 */
void CTe_LbsRefLocationSource::RequestRefPositionInfo()
	{
	//Wait for KCallBackDelay before returning the reference position
	if(iDelayTimer)
	    {
	    iDelayTimer->Start();
	    }
	}

/**
 * Cancels the outstanding asynchronous request for the reference location
 */
void CTe_LbsRefLocationSource::CancelRefPositionInfoRequest()
	{
	//cancel the timer
	iDelayTimer->Cancel();
	}


void CTe_LbsRefLocationSource::CompleteCallback()
    {
    TInt error = KErrNone;
    if(!iReturnPosition)
        {
        error = KErrNotFound;
        }
    
    //Just create test data for the moment  
    TPosition pos;
    pos.SetCoordinate(POS_LAT, POS_LONG, POS_ALT);
    pos.SetAccuracy(POS_HORZ_ACCURACY, POS_VERT_ACCURACY);
    pos.SetCurrentTime();
    
    TCourse course;
    course.SetSpeed(SPEED);
    course.SetVerticalSpeed(VERTICAL_SPEED);
    course.SetHeading(HEADING);
    
    TPositionCourseInfo courseInfo;
    courseInfo.SetPosition(pos);
    courseInfo.SetCourse(course);
    
    TPositionAreaInfo areaInfo;
    areaInfo.SetArea(TPositionAreaInfo::EAreaUnknown);
    
    //Modify the returned pos value based on the test case.
    TInt testcase, propRead;
    propRead = iTestCaseIdKey.Get(KSuplProxyPMSuite, KLbsPosSessionInitStepKey,testcase);
    
    if (propRead==KErrNone)
        {
        switch(testcase)
            {
            case ERefLocPluginPositionError:
                {
                //Return erroneous values to the SPPM. [Negative test case]
                iObserver.HandleRefPositionInformation(KErrNotFound, NULL, NULL, NULL, NULL);
                break;                
                }
            default:
                {
                //Return the data back to the SPPM.  Position1 and Area1 are also used for Position2 and Area2 
                // as the SPPM does not currently use these parameters
                iObserver.HandleRefPositionInformation(error, &courseInfo, &areaInfo, &courseInfo, &areaInfo);
                break;
                }
            }
        }
    }


