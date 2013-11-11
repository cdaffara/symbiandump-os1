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
 @file AssistanceDataReaderSetStep.cpp
*/
#include "AssistanceDataReaderSetStep.h"
#include "Te_LbsAssistanceDataSuiteDefs.h"
#include "Te_LbsAssistanceDataSuiteServer.h"
#include <s32mem.h>


CAssistanceDataReaderSetStep::~CAssistanceDataReaderSetStep()
/**
 * Destructor
 */
	{
	}

CAssistanceDataReaderSetStep::CAssistanceDataReaderSetStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KAssistanceDataReaderSetStep);
	}

TVerdict CAssistanceDataReaderSetStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual. Nothing done here.
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CAssistanceDataReaderSetStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{

		RLbsAssistanceDataReaderSet set;
		CleanupClosePushL(set);
		set.OpenL();
		
		// Access each reader and check it is not NULL.
		//
		RUEPositioningGpsAlmanacReader* almanacPtr;
		RUEPositioningGpsIonosphericModelReader* ionosphericPtr;
		RUEPositioningGpsNavigationModelReader* navigationPtr;
		RUEPositioningGpsReferenceTimeReader* referenceTimePtr;
		RUEPositioningGpsUtcModelReader*  utcPtr;
		RUEPositioningGpsAcquisitionAssistanceReader* acquisitionPtr;
		RBadSatListReader* badSatPtr1;
		RReferenceLocationReader* referenceLocationPtr;
		
		if(set.GetDataReader(almanacPtr) == KErrNone)
			{
			TESTL(almanacPtr!=NULL);
			}
		else
			{
			ERR_PRINTF1(_L("NULL almanacPtr"));
    		SetTestStepResult(EFail);
			}

		if(set.GetDataReader(ionosphericPtr) == KErrNone)
			{
			TESTL(ionosphericPtr!=NULL);
			}
		else
			{
			ERR_PRINTF1(_L("NULL ionosphericPtr"));
    		SetTestStepResult(EFail);
			}
			
		if(set.GetDataReader(navigationPtr) == KErrNone)
			{
			TESTL(navigationPtr!=NULL);
			}
		else
			{
			ERR_PRINTF1(_L("NULL navigationPtr"));
    		SetTestStepResult(EFail);
			}
			
		if(set.GetDataReader(referenceTimePtr) == KErrNone)
			{
			TESTL(referenceTimePtr!=NULL);
			}
		else
			{
			ERR_PRINTF1(_L("NULL referenceTimePtr"));
    		SetTestStepResult(EFail);
			}
			
		if(set.GetDataReader(utcPtr) == KErrNone)
			{
			TESTL(utcPtr!=NULL);
			}
		else
			{
			ERR_PRINTF1(_L("NULL utcPtr"));
    		SetTestStepResult(EFail);
			}
			
		if(set.GetDataReader(acquisitionPtr) == KErrNone)
			{
			TESTL(acquisitionPtr!=NULL);
			}
		else
			{
			ERR_PRINTF1(_L("NULL acquisitionPtr"));
    		SetTestStepResult(EFail);
			}
			
		if(set.GetDataReader(badSatPtr1) == KErrNone)
			{
			TESTL(badSatPtr1!=NULL);
			}
		else
			{
			ERR_PRINTF1(_L("NULL badSatPtr1"));
    		SetTestStepResult(EFail);
			}
			
		if(set.GetDataReader(referenceLocationPtr) == KErrNone)
			{
			TESTL(referenceLocationPtr!=NULL);
			}
		else
			{
			ERR_PRINTF1(_L("NULL referenceLocationPtr"));
    		SetTestStepResult(EFail);
			}
	
		// Set something in two of the readers
		//
        RBadSatListBuilder badSatBuilder;
        CleanupClosePushL(badSatBuilder);
        badSatBuilder.OpenL();
        RBadSatArrayBuilder badSatArray;
        badSatBuilder.GetArrayBuilder(TBadSatList::EBadSatIdArray, badSatArray);
		for (TInt ii = 0; ii<32; ii++)
			{
			badSatArray.SetElement(ii, ii);
			}
        TTime time;
        time.UniversalTime();        
        badSatBuilder.SetTimeStamp(time);
	    TPtr8 ptr(NULL,0);
	    ptr.Set(badSatPtr1->DataBuffer());
	    ptr = badSatBuilder.DataBuffer();
	    CleanupStack::PopAndDestroy(&badSatBuilder);
		
        RUEPositioningGpsReferenceTimeBuilder referenceTimeBuilder;
        CleanupClosePushL(referenceTimeBuilder);
        referenceTimeBuilder.OpenL();
		RGpsTowAssistArrayBuilder towAssistArrayBuilder;
		referenceTimeBuilder.GetArrayBuilder(TUEPositioningGpsReferenceTime::EGpsTowAssistArray, towAssistArrayBuilder);
		TUint unsignedTestData = 8;
		RGpsTowAssistBuilder towAssistBuilder;
		towAssistArrayBuilder.GetFieldBuilder(2, towAssistBuilder);
        towAssistBuilder.SetField(TGpsTowAssist::ETlmMessage, unsignedTestData);
        referenceTimeBuilder.SetTimeStamp(time);
        ptr.Set(referenceTimePtr->DataBuffer());
        ptr = referenceTimeBuilder.DataBuffer();
		CleanupStack::PopAndDestroy(&referenceTimeBuilder);
		
		// Check that data is only available for the two readers
		// populated above.
		//
		if (almanacPtr->IsDataAvailable())
			{
			ERR_PRINTF1(_L("almanacPtr not empty"));
    		SetTestStepResult(EFail);
			}
		if (ionosphericPtr->IsDataAvailable())
			{
			ERR_PRINTF1(_L("ionosphericPtr not empty"));
    		SetTestStepResult(EFail);
			}
		if (navigationPtr->IsDataAvailable())
			{
			ERR_PRINTF1(_L("navigationPtr not empty"));
    		SetTestStepResult(EFail);
			}
		if (utcPtr->IsDataAvailable())
			{
			ERR_PRINTF1(_L("utcPtr not empty"));
    		SetTestStepResult(EFail);
			}
		if (acquisitionPtr->IsDataAvailable())
			{
			ERR_PRINTF1(_L("acquisitionPtr not empty"));
    		SetTestStepResult(EFail);
			}
		if (referenceLocationPtr->IsDataAvailable())
			{
			ERR_PRINTF1(_L("referenceLocationPtr not empty"));
    		SetTestStepResult(EFail);
			}						
		if (!referenceTimePtr->IsDataAvailable())
			{
			ERR_PRINTF1(_L("referenceTimePtr is empty"));
    		SetTestStepResult(EFail);
			}
		if (!badSatPtr1->IsDataAvailable())
			{
			ERR_PRINTF1(_L("badSatPtr1 is empty"));
    		SetTestStepResult(EFail);
			}

		// Externalize to a descriptor
		HBufC8* setBuffer = HBufC8::NewLC(RLbsAssistanceDataReaderSet::MaxExternalizedBufferSize());
		RDesWriteStream writeStream;
		CleanupClosePushL(writeStream);
		TPtr8 ptrSetBuffer(setBuffer->Des());
		writeStream.Open(ptrSetBuffer);
		set.ExternalizeL(writeStream);
		writeStream.CommitL();
		CleanupStack::PopAndDestroy(&writeStream);
		

		// Close the reader
		//
		CleanupStack::Pop(setBuffer);
        CleanupStack::PopAndDestroy(&set);
        CleanupStack::PushL(setBuffer);

		// Try  to access a reader and check that 
		// a NULL pointer is returned
		//
		if(set.GetDataReader(badSatPtr1) == KErrNone)
			{
			ERR_PRINTF1(_L("unexpecter KErrNone"));
   			SetTestStepResult(EFail);	
			}

		if (badSatPtr1 != NULL)
			{
			ERR_PRINTF1(_L("badSatPtr1 is not null after closing the set"));
    		SetTestStepResult(EFail);
			}
		
		// Internalize
        CleanupClosePushL(set);
		set.OpenL();
        RDesReadStream readStream;
        CleanupClosePushL(readStream);
        readStream.Open(ptrSetBuffer);
        set.InternalizeL(readStream);
        CleanupStack::PopAndDestroy(&readStream);

        // Retrieve all the pointers in the set again
        if(set.GetDataReader(almanacPtr) == KErrNone)
            {
            TESTL(almanacPtr!=NULL);
            }
        else
            {
            ERR_PRINTF1(_L("NULL almanacPtr"));
            SetTestStepResult(EFail);
            }

        if(set.GetDataReader(ionosphericPtr) == KErrNone)
            {
            TESTL(ionosphericPtr!=NULL);
            }
        else
            {
            ERR_PRINTF1(_L("NULL ionosphericPtr"));
            SetTestStepResult(EFail);
            }
            
        if(set.GetDataReader(navigationPtr) == KErrNone)
            {
            TESTL(navigationPtr!=NULL);
            }
        else
            {
            ERR_PRINTF1(_L("NULL navigationPtr"));
            SetTestStepResult(EFail);
            }
            
        if(set.GetDataReader(referenceTimePtr) == KErrNone)
            {
            TESTL(referenceTimePtr!=NULL);
            }
        else
            {
            ERR_PRINTF1(_L("NULL referenceTimePtr"));
            SetTestStepResult(EFail);
            }
            
        if(set.GetDataReader(utcPtr) == KErrNone)
            {
            TESTL(utcPtr!=NULL);
            }
        else
            {
            ERR_PRINTF1(_L("NULL utcPtr"));
            SetTestStepResult(EFail);
            }
            
        if(set.GetDataReader(acquisitionPtr) == KErrNone)
            {
            TESTL(acquisitionPtr!=NULL);
            }
        else
            {
            ERR_PRINTF1(_L("NULL acquisitionPtr"));
            SetTestStepResult(EFail);
            }
            
        if(set.GetDataReader(badSatPtr1) == KErrNone)
            {
            TESTL(badSatPtr1!=NULL);
            }
        else
            {
            ERR_PRINTF1(_L("NULL badSatPtr1"));
            SetTestStepResult(EFail);
            }
            
        if(set.GetDataReader(referenceLocationPtr) == KErrNone)
            {
            TESTL(referenceLocationPtr!=NULL);
            }
        else
            {
            ERR_PRINTF1(_L("NULL referenceLocationPtr"));
            SetTestStepResult(EFail);
            }
    
        // Check that data is only available for the two readers
        // populated above, before externalizing.
        if (almanacPtr->IsDataAvailable())
            {
            ERR_PRINTF1(_L("almanacPtr not empty"));
            SetTestStepResult(EFail);
            }
        if (ionosphericPtr->IsDataAvailable())
            {
            ERR_PRINTF1(_L("ionosphericPtr not empty"));
            SetTestStepResult(EFail);
            }
        if (navigationPtr->IsDataAvailable())
            {
            ERR_PRINTF1(_L("navigationPtr not empty"));
            SetTestStepResult(EFail);
            }
        if (utcPtr->IsDataAvailable())
            {
            ERR_PRINTF1(_L("utcPtr not empty"));
            SetTestStepResult(EFail);
            }
        if (acquisitionPtr->IsDataAvailable())
            {
            ERR_PRINTF1(_L("acquisitionPtr not empty"));
            SetTestStepResult(EFail);
            }
        if (referenceLocationPtr->IsDataAvailable())
            {
            ERR_PRINTF1(_L("referenceLocationPtr not empty"));
            SetTestStepResult(EFail);
            }                       
        if (!referenceTimePtr->IsDataAvailable())
            {
            ERR_PRINTF1(_L("referenceTimePtr is empty"));
            SetTestStepResult(EFail);
            }
        if (!badSatPtr1->IsDataAvailable())
            {
            ERR_PRINTF1(_L("badSatPtr1 is empty"));
            SetTestStepResult(EFail);
            }
        
        
        CleanupStack::PopAndDestroy(&set);
		CleanupStack::PopAndDestroy(setBuffer);

		}
		
	  return TestStepResult();
	}


// 
//
TVerdict CAssistanceDataReaderSetStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
   	return TestStepResult();
	}
