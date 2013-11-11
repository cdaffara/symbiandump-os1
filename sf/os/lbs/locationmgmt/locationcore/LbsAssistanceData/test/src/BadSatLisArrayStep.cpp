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
 @file BadSatLisArrayStep.cpp
*/
#include "BadSatLisArrayStep.h"
#include "Te_LbsAssistanceDataSuiteServer.h"

#define  badSatListBuilder iServer.iBadSatListBuilder
#define  badSatArrayBuilder iServer.iBadSatArrayBuilder

#define  badSatListReader iServer.iBadSatListReader
#define  badSatArrayReader iServer.iBadSatArrayReader


CBadSatLisArrayStep::~CBadSatLisArrayStep()
/**
 * Destructor
 */
	{
	}

CBadSatLisArrayStep::CBadSatLisArrayStep(CTe_LbsAssistanceDataSuite& aOwningServer):
	CTe_LbsAssistanceDataSuiteStepBase(aOwningServer)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KBadSatLisArrayStep);
	}

TVerdict CBadSatLisArrayStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CBadSatLisArrayStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		badSatListBuilder.GetArrayBuilder(TBadSatList::EBadSatIdArray, badSatArrayBuilder);
		
		// Set some data in the array
		//
		for (TInt ii = 0; ii<32; ii++)
			{
			badSatArrayBuilder.SetElement(ii, ii);
			}

	    // Check if a reader can access the array
    	//
    	badSatListReader.DataBuffer() = badSatListBuilder.DataBuffer();
    	if(!badSatListReader.FieldExists(TBadSatList::EBadSatIdArray))
    		{
			ERR_PRINTF1(_L("TBadSatList::EBadSatIdArray field didn't exist"));
    		SetTestStepResult(EFail);
    		User::Leave(KErrNotFound); 		
    		}
    		
    	badSatListReader.GetArrayReader(TBadSatList::EBadSatIdArray, badSatArrayReader);

        // Get each element, check its value and clear it from the array.
        //
		for (TInt ii = 0; ii<32; ii++)
			{
			if(!badSatArrayReader.ElementExists(ii))
				{
				ERR_PRINTF2(_L("Missing element %d"), ii);
    			SetTestStepResult(EFail);	
				}
			else
				{
				TUint readValue;
				badSatArrayReader.GetElement(ii, readValue);
				if (readValue != ii)
					{
					ERR_PRINTF2(_L("Incorrect value for element number %d"), ii);
    				SetTestStepResult(EFail);						
					}
					
				// Clear this element from the array
				//
				badSatArrayBuilder.ClearElement(ii);
				
				// Get all data from builder to reader again
				// after deleting each element.
				//
				badSatListReader.DataBuffer() = badSatListBuilder.DataBuffer();
				if(!badSatListReader.FieldExists(TBadSatList::EBadSatIdArray))
    				{
    				if (ii != 31)
    					{
						ERR_PRINTF1(_L("TBadSatList::EBadSatIdArray field didn't exist"));
    					SetTestStepResult(EFail);
    					User::Leave(KErrNotFound); 		
    					}
    				else
    				    {
    					INFO_PRINTF1(_L("Array has been correctly cleared after having deleted all the elements"));
    					break;
    				    }
    				}
    			else
    				{
    	        	badSatListReader.GetArrayReader(TBadSatList::EBadSatIdArray, badSatArrayReader);
    				}
				}
			}
		}

	  return TestStepResult();
	}



TVerdict CBadSatLisArrayStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	badSatListBuilder.Close();
	badSatListReader.Close();
	return TestStepResult();
	}
