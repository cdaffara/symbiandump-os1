// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TLbsGpsConTest1.cpp
// 
//

//System
#include <e32std.h>
#include <e32property.h>
#include <e32test.h>

//Project
#include <Lbs.h>
#include <lbs/LbsLocDataSourceGpsBase.h>
#include "LbsLasCommon.h"
#include "LbsGpsConTest1.h"

// Test
_LIT(KTestName, "Lbs Location Manager simple test/example");
LOCAL_D RTest test(KTestName);

/**
PressKey - Wait for the user to press a key

@internalTechnology
*/
void PressKey()
	{
	test.Printf(_L("Press any key to continue...\n"));
	test.Getch();
	}

/**
SendLocationUpdateRequest - Transmit a location request to the GPS Location Manager

@internalTechnology
*/
TInt SendLocationUpdateRequest(const TTimeIntervalSeconds& aInterval)
	{
   TTime targetTime;
   targetTime.UniversalTime();

	targetTime += aInterval;
   
   TPckgBuf<TLasRequest> locRequestBuf;
   TLasRequest& locRequest = locRequestBuf();
   
   locRequest.iVersion    = 1;
   locRequest.iRequest    = TLasRequest::ELasRequestLocation;
   locRequest.iTargetTime = targetTime;
   locRequest.iMode       = CLbsLocationSourceGpsBase::EPowerModeUnknown;
   
   TInt err = RProperty::Set(KLasPropertyCat, ELasLocationRequestBus, locRequestBuf);
   
   return err;
	}

/**
GetLocationData - read the position information from the property ELasLocationDataBus.

@internalTechnology
*/
TInt GetLocationData(TPositionInfoBase& aPosInfo)
	{
   TPckgBuf<TPositionCourseInfo> posInfoBuf;
   
   TInt baseSize = sizeof(TPositionModuleInfoBase);

   TInt err = RProperty::Get(KLasPropertyCat, ELasLocationDataBus, posInfoBuf);
   
   if (err)
   	{
   	return err;
   	}

	TPositionInfoBase& posInfo = posInfoBuf();

	TUint sourceSize = posInfo.PositionClassSize();
	TUint targetSize = aPosInfo.PositionClassSize();

	if (sourceSize < targetSize)
		{
		return KErrArgument;	
		}

	TUint8* sourceStartPtr = (reinterpret_cast<TUint8*>(&posInfo))  + baseSize;
	TUint8* targetStartPtr = (reinterpret_cast<TUint8*>(&aPosInfo)) + baseSize;

	TUint dataLength = targetSize - baseSize;

	Mem::Copy(targetStartPtr, sourceStartPtr, dataLength);

   return KErrNone;
   }

//************************************************************************************************************
//  MAIN
//************************************************************************************************************
/**
The doMainL method. The function that starts a test

Some useful info is output to the debug window

@internalTechnology
*/
void doMainL()
   {
   // Data to be exanged through the properties
   TPositionCourseInfo posInfo;
   
   //Property and notifications
   RProperty      dataBusChannel;   
   TRequestStatus dataBusStatus;
   
   // Other
   TPosition      position;
   TCourse        course;
   TTime          time;
   TBuf<100>		timeBuf;
   
   
   time.HomeTime();
   time.FormatL(timeBuf, _L("%H:%T:%S"));   
	test.Printf(_L("%S: LbsGpsConTest1 - started\n"), &timeBuf);

   // Attach to the Location Data Bus Channel (Property)
   TInt err = dataBusChannel.Attach(KLasPropertyCat,ELasLocationDataBus, EOwnerThread);
   User::LeaveIfError(err);
   
   TInt maxUpdates   = 4;
   TInt updatePeriod = 3;
   
   for (TInt count = 0; count < maxUpdates; count++)
   	{
	   // Display Current Time
	   time.HomeTime();
	   time.FormatL(timeBuf, _L("%H:%T:%S"));   

		test.Printf(_L("%S: Requesting update in %d secs\n"), &timeBuf, updatePeriod);

      TTimeIntervalSeconds updateInterval = static_cast<TTimeIntervalSeconds>(updatePeriod);

		dataBusChannel.Subscribe(dataBusStatus);

		err = SendLocationUpdateRequest(updateInterval);
		
		if (err)
			{
			test.Printf(_L("***Unable to send location update request - start LbsGpsManager first\n"));
			PressKey();
			User::Leave(err);
			}
		
		User::WaitForRequest(dataBusStatus);	

		err = GetLocationData(posInfo);

		if (err)
			{
			test.Printf(_L("***Unable to get location data - start LbsGpsManager first\n"));
			PressKey();
			User::Leave(err);
			}

   
      // Extract Position and Course from the Position Info
      posInfo.GetPosition(position);
      posInfo.GetCourse(course);

      // Display Info
      test.Printf(_L("Latitude:  %g\n"), position.Latitude());
      test.Printf(_L("Longitude: %g\n"), position.Longitude());      
      test.Printf(_L("Horizontal accuracy: %g\n"), position.HorizontalAccuracy());
      test.Printf(_L("Vertical accuracy:   %g\n"), position.VerticalAccuracy());
      test.Printf(_L("Speed: %g\n"), course.Speed());
   	}
  
   // Cancel outstanding subscriptions and Close 
   dataBusChannel.Cancel();

   time.HomeTime();
   time.FormatL(timeBuf, _L("%H:%T:%S"));   
	test.Printf(_L("%S: LbsGpsConTest1 - finished\n"), &timeBuf);

	PressKey();
   }
   
 
//************************************************************************************************************
// Program Entry
//************************************************************************************************************
/**
Program entry point

@return KErrNone
@internalTechnology
*/
GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Title();
	test.Start(KTestName);
		
	CActiveScheduler* rootScheduler = new CActiveScheduler;
	CActiveScheduler::Install(rootScheduler);
	CTrapCleanup* theCleanup=CTrapCleanup::New();

	TRAPD(ret,doMainL());	
	test.Printf(_L("\nTest Completion code: %d\n"), ret);	
	test(ret==KErrNone);
	
	delete theCleanup;	
	delete rootScheduler;
	
	test.End();
	test.Close();
		
	__UHEAP_MARKEND;
	return(KErrNone);
	}

