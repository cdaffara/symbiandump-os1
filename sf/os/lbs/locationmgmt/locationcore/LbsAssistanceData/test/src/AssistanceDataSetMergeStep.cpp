// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file AssistanceDataSetMergeStep.cpp
*/
#include "AssistanceDataSetMergeStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define badSatListReader iServer.iBadSatListReader
#define badSatArrayReader iServer.iBadSatArrayReader

#define gpsReferenceTimeReader iServer.iGpsReferenceTimeReader
#define gpsTowAssistArrayReader iServer.iGpsTowAssistArrayReader
#define gpsTowAssistReader iServer.iGpsTowAssistReader

#define gpsAlmanacReader iServer.iGpsAlmanacReader
#define almanacSatInfoArrayReader iServer.iAlmanacSatInfoArrayReader
#define almanacSatInfoReader iServer.iAlmanacSatInfoReader

CAssistanceDataSetMergeStep::~CAssistanceDataSetMergeStep()
/**
 * Destructor
 */
	{
	}

CAssistanceDataSetMergeStep::CAssistanceDataSetMergeStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KAssistanceDataSetMergeStep);
	}

TVerdict CAssistanceDataSetMergeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual. Nothing done here.
 */
	{
	TTimeIntervalMinutes oneMinute(1);			
	time1.UniversalTime();
	time2 = time1 + oneMinute; //later than time1
	set1.OpenL();
	set2.OpenL();

	//------------------------------------------------------------------------------
	//This tests sets the following values in builders of set1:
	//
	//RBadSatListBuilder: Field value set to 1; TimeStamp: earliest
	//RUEPositioningGpsReferenceTimeBuilder: Field value set to 1; TimeStamp: latest
	//RUEPositioningGpsAlmanacBuilder: not set (empty)
	//
	//And the following values in builders of set2
	//
	//RBadSatListBuilder: Field value set to 2; TimeStamp: latest
	//RUEPositioningGpsReferenceTimeBuilder: Field value set to 2; TimeStamp: earliest
	//RUEPositioningGpsAlmanacBuilder: any values; Timestamp: earliest
	//		
	//After the merge with set2, set1 will contain the following builders only
	//
	//
	//RBadSatListBuilder: Field value set to 2; TimeStamp: latest
	//RUEPositioningGpsReferenceTimeBuilder: Field value set to 1; TimeStamp: latest
	//RUEPositioningGpsAlmanacBuilder: values, TimeStamp: earliest
	//
	//------------------------------------------------------------------------------

		
    //-------------------------------------------------------------------
    //--- Set values and time stamps in two of the builders in set 1 ----
    //-------------------------------------------------------------------

	RUEPositioningGpsAlmanacBuilder* almanacPtr1;
	RUEPositioningGpsReferenceTimeBuilder* referenceTimePtr1;
	RBadSatListBuilder* badSatPtr1;
	
	if(set1.GetDataBuilder(almanacPtr1) == KErrNone)
		{
		TESTL(almanacPtr1!=NULL);
		}
	else
		{
		ERR_PRINTF1(_L("NULL almanacPtr1"));
   		SetTestStepResult(EFail);
		}

	if(set1.GetDataBuilder(referenceTimePtr1) == KErrNone)
		{
		TESTL(referenceTimePtr1!=NULL);
		}
	else
		{
		ERR_PRINTF1(_L("NULL referenceTimePtr1"));
   		SetTestStepResult(EFail);
		}

	if(set1.GetDataBuilder(badSatPtr1) == KErrNone)
		{
		TESTL(badSatPtr1!=NULL);
		}
	else
		{
		ERR_PRINTF1(_L("NULL badSatPtr1"));
   		SetTestStepResult(EFail);
		}

	// Set something in RBadSatArrayBuilder of set1
	//
	RBadSatArrayBuilder badSatArray;
	badSatPtr1->GetArrayBuilder(TBadSatList::EBadSatIdArray, badSatArray);
	TInt value1 = 1;
	// Set first element to 1
	badSatArray.SetElement(0,value1);
	// Set time stamp to time1 (earliest)
	badSatPtr1->SetTimeStamp(time1);

	// Set something in RGpsTowAssistArrayBuilder of set1
	//
	RGpsTowAssistArrayBuilder towAssistArrayBuilder;
	referenceTimePtr1->GetArrayBuilder(TUEPositioningGpsReferenceTime::EGpsTowAssistArray, towAssistArrayBuilder);
	// Set to 1 the ETlmMessage field of the third node in
	// the array
	TUint unsignedTestData1 = 1;
	RGpsTowAssistBuilder towAssistBuilder;
	towAssistArrayBuilder.GetFieldBuilder(2, towAssistBuilder);
    towAssistBuilder.SetField(TGpsTowAssist::ETlmMessage, unsignedTestData1);
    // Set timestamp to time2 (latest)
	referenceTimePtr1->SetTimeStamp(time2);
	
		
    //-------------------------------------------------------------------
    //--- Set values and time stamps in three of the builders in set 2 --
    //-------------------------------------------------------------------

	RUEPositioningGpsAlmanacBuilder* almanacPtr2;
	RUEPositioningGpsReferenceTimeBuilder* referenceTimePtr2;
	RBadSatListBuilder* badSatPtr2;
	
	if(set2.GetDataBuilder(almanacPtr2) == KErrNone)
		{
		TESTL(almanacPtr2!=NULL);
		}
	else
		{
		ERR_PRINTF1(_L("NULL almanacPtr2"));
   		SetTestStepResult(EFail);
		}

	if(set2.GetDataBuilder(referenceTimePtr2) == KErrNone)
		{
		TESTL(referenceTimePtr2!=NULL);
		}
	else
		{
		ERR_PRINTF1(_L("NULL referenceTimePtr2"));
   		SetTestStepResult(EFail);
		}
	
	if(set2.GetDataBuilder(badSatPtr2) == KErrNone)
		{
		TESTL(badSatPtr2!=NULL);
		}
	else
		{
		ERR_PRINTF1(_L("NULL badSatPtr2"));
   		SetTestStepResult(EFail);
		}

	// Set values in RBadSatArrayBuilder of set2
	//
	RBadSatArrayBuilder badSatArray2;
	badSatPtr2->GetArrayBuilder(TBadSatList::EBadSatIdArray, badSatArray2);
	TInt value2 = 2;
	// Set first element to 1
	badSatArray2.SetElement(0,value2);
	// Set time stamp to time2 (latest)
	badSatPtr2->SetTimeStamp(time2);

	// Set values in RGpsTowAssistArrayBuilder of set2
	//
	RGpsTowAssistArrayBuilder towAssistArrayBuilder2;
	referenceTimePtr2->GetArrayBuilder(TUEPositioningGpsReferenceTime::EGpsTowAssistArray, towAssistArrayBuilder2);
	// Set to 1 the ETlmMessage field of the third node in
	// the array
	TUint unsignedTestData2 = 2;
	RGpsTowAssistBuilder towAssistBuilder2;
	towAssistArrayBuilder2.GetFieldBuilder(2, towAssistBuilder2);
    towAssistBuilder2.SetField(TGpsTowAssist::ETlmMessage, unsignedTestData2);
    // Set timestamp to time1 (earliest)
	referenceTimePtr2->SetTimeStamp(time1);

	// Set values in RUEPositioningGpsAlmanacBuilder
	//
	RAlmanacSatInfoArrayBuilder almanacSatInfoArrayBuilder;
	almanacPtr2->GetArrayBuilder(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray, almanacSatInfoArrayBuilder);
	// Set some data in the array and check that it exists
	TUint unsignedTestData3 = 8;
	RAlmanacSatInfoBuilder satInfoBuilder;
	almanacSatInfoArrayBuilder.GetFieldBuilder(2, satInfoBuilder);
    satInfoBuilder.SetField(TAlmanacSatInfo::ESatID, unsignedTestData3);
    // Set timestamp to time1 (earliest)
	almanacPtr2->SetTimeStamp(time1);
    

	SetTestStepResult(EPass);
	return TestStepResult();
}

TVerdict CAssistanceDataSetMergeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
        set1.MergeL(set2);
		}
		
	// Verify expected values in RBadSatListBuilder
	// (using readers owned by test server)
	//
	RUEPositioningGpsAlmanacBuilder* almanacPtr1;
	RUEPositioningGpsReferenceTimeBuilder* referenceTimePtr1;
	RBadSatListBuilder* badSatPtr1;
	
	if(set1.GetDataBuilder(almanacPtr1) == KErrNone)
		{
		TESTL(almanacPtr1!=NULL);
		}
	else
		{
		ERR_PRINTF1(_L("NULL almanacPtr1"));
   		SetTestStepResult(EFail);
		}

	if(set1.GetDataBuilder(referenceTimePtr1) == KErrNone)
		{
		TESTL(referenceTimePtr1!=NULL);
		}
	else
		{
		ERR_PRINTF1(_L("NULL referenceTimePtr1"));
   		SetTestStepResult(EFail);
		}

	if(set1.GetDataBuilder(badSatPtr1) == KErrNone)
		{
		TESTL(badSatPtr1!=NULL);
		}
	else
		{
		ERR_PRINTF1(_L("NULL badSatPtr1"));
   		SetTestStepResult(EFail);
		}

	// -------------------------------------
	// Check values in RBadSatListBuilder
	// -------------------------------------
	//
	badSatListReader.OpenL();
    badSatListReader.DataBuffer() = badSatPtr1->DataBuffer();
    if(!badSatListReader.FieldExists(TBadSatList::EBadSatIdArray))
    	{
		ERR_PRINTF1(_L("TBadSatList::EBadSatIdArray field didn't exist"));
    	SetTestStepResult(EFail);
    	User::Leave(KErrNotFound); 		
    	}
    else
    	{
    	badSatListReader.GetArrayReader(TBadSatList::EBadSatIdArray, badSatArrayReader);
    	}

    // Get element zero and check its value.
	if(!badSatArrayReader.ElementExists(0))
		{
		ERR_PRINTF2(_L("Missing element %d"), 0);
    	SetTestStepResult(EFail);	
		}
	else
		{
		TUint readValue;
		badSatArrayReader.GetElement(0, readValue);
		if (readValue != 2)
			{
			ERR_PRINTF1(_L("Incorrect value for element"));
    		SetTestStepResult(EFail);						
			}
		}
		
	if(badSatPtr1->TimeStamp() != time2)
		{
		ERR_PRINTF1(_L("Incorrect timestamp in badSatPtr1"));
    	SetTestStepResult(EFail);	
		}	

	badSatListReader.Close();
	
	// -----------------------------------------------------
	// Check values in RUEPositioningGpsReferenceTimeBuilder
	// -----------------------------------------------------
	//
	gpsReferenceTimeReader.OpenL();
	gpsReferenceTimeReader.DataBuffer() = referenceTimePtr1->DataBuffer();
   	if(gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EGpsTowAssistArray))
		{
		gpsReferenceTimeReader.GetArrayReader(TUEPositioningGpsReferenceTime::EGpsTowAssistArray, gpsTowAssistArrayReader);
		//Get a reader for the node at index 2
   		//
   		gpsTowAssistArrayReader.GetFieldReader(2, gpsTowAssistReader);
   		}
   	else
   		{
		ERR_PRINTF1(_L("Array doesn't exist in reader of GpsReferenceTime"));
    	SetTestStepResult(EFail);
   		}

    // Check that the value in the node is 2 for ETlmMessage
    TUint unsignedTestData;
	gpsTowAssistReader.GetField(TGpsTowAssist::ETlmMessage, unsignedTestData);
	if (unsignedTestData != 1)
		{
		ERR_PRINTF1(_L("Unexpected value for TGpsTowAssist::ETlmMessage"));
    	SetTestStepResult(EFail);		
		}
	// Check the timestamp
	if(referenceTimePtr1->TimeStamp() != time2)
		{
		ERR_PRINTF1(_L("Incorrect timestamp in referenceTimePtr1"));
    	SetTestStepResult(EFail);	
		}	
	gpsReferenceTimeReader.Close();
	
	// -----------------------------------------------------
	// Check values in RUEPositioningGpsAlmanacBuilder
	// -----------------------------------------------------
	//	
	gpsAlmanacReader.OpenL();
	
	gpsAlmanacReader.DataBuffer() = almanacPtr1->DataBuffer();
	
   	if(gpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray))
		{
		gpsAlmanacReader.GetArrayReader(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray, almanacSatInfoArrayReader);

		//Get a reader for the node at index 2
   		//
   		almanacSatInfoArrayReader.GetFieldReader(2, almanacSatInfoReader);
   		}
   	else
   		{
		ERR_PRINTF1(_L("Array doesn't exist in gpsAlmanacReader"));
    	SetTestStepResult(EFail);
   		}
    // Check that the value in the node is 8 for TAlmanacSatInfo::ESatID
    TUint testData;
	almanacSatInfoReader.GetField(TAlmanacSatInfo::ESatID, testData);
	if (testData != 8)
		{
		ERR_PRINTF1(_L("Unexpected value for TAlmanacSatInfo::ESatID"));
    	SetTestStepResult(EFail);		
		}
		
	// Check the timestamp
	if(almanacPtr1->TimeStamp() != time1)
		{
		ERR_PRINTF1(_L("Incorrect timestamp in badSatPtr1"));
    	SetTestStepResult(EFail);	
		}	
	
	gpsAlmanacReader.Close();
	
	return TestStepResult();
	}


// 
//
TVerdict CAssistanceDataSetMergeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	set1.Close();
	set2.Close();
   	return TestStepResult();
	}
