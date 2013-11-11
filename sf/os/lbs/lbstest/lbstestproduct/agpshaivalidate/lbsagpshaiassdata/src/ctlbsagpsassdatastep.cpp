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
// @file ctlbsagpsassdatastep.cpp
// lorna: this is a sample only
// 
//

// EPOC includes.
#include <e32property.h>
#include <ecom/ecom.h> 

// LBS includes. 
#include <lbscommon.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsadmin.h>
#include <lbs/lbsmoduleinfo.h>

// LBS test includes.
// for assistance data provider
#include "ctlbsagpsassdatastep.h"
#include <lbs/test/ctlbstimerutils.h>
#include "lbstestloggermacros.h"
#include "ctlbs.h"
#include "lbstestutilities.h" 


/**
 * Destructor
 */
CT_LbsAGPSAssDataStep::~CT_LbsAGPSAssDataStep()
	{
	// In case there was a leave before we 'unload' the modules:
	UnLoadHAIModule();
	}
	

/**
 * Constructor
 */
CT_LbsAGPSAssDataStep::CT_LbsAGPSAssDataStep(CT_LbsAGPSHAIServerRoot& aParent) : CT_LbsAGPSHAIValidateStepRoot(aParent)
	{	
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::CT_LbsAGPSAssDataStep()");
	// need to change the name....
	SetTestStepName(KLbsAGPSHAIValidateStep);
	
	// install scheduler if there isn't one already installed
	if(!CActiveScheduler::Current())
		{
		CActiveScheduler::Install(iParent.iScheduler);	
		}
		
	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::CT_LbsAGPSAssDataStep()");		
	}


/**
Public, non-leaving, static constructor
*/
CT_LbsAGPSAssDataStep* CT_LbsAGPSAssDataStep::New(CT_LbsAGPSHAIServerRoot& aParent)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::New()");
	CT_LbsAGPSAssDataStep* self = NULL;
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	TRAP_IGNORE(self = NewL(aParent));
	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::New()");
	return self;
	}


/**
Static Constructor that may leave
*/
CT_LbsAGPSAssDataStep* CT_LbsAGPSAssDataStep::NewL(CT_LbsAGPSHAIServerRoot& aParent)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::NewL()");
	CT_LbsAGPSAssDataStep* self = new CT_LbsAGPSAssDataStep(aParent);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::New()");
	return self;

	}
	
/**
2nd phase Constructor that may leave
*/
void CT_LbsAGPSAssDataStep::ConstructL()
	{
	// must call the base class
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::ConstructL()");	
	// MUST _MUST_ call the parent class version first
	CT_LbsAGPSHAIValidateStepRoot::ConstructL();
	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::ConstructL()");	
	}
	

void CT_LbsAGPSAssDataStep::LoadADPModuleL()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::LoadADPModuleL()");	
		
	// load (3rd party, eg SUPL) assistance data module from an plugin.
	TRAPD(err, iAssistanceDataPlugin = CAssistanceDataSourceModuleBase::NewL(iDataProviderPluginUid, *this));
	
	if(KErrNone != err)
		{
		TESTLOG2(ELogP3, "FAILED: Could not Create Assistance Provider Implementation. Failed with error %d\n", err);
		ERR_PRINTF2(_L("<font><b>FAILED: Could not Create Assistance Provider Implementation. Got error %d.</font></b>\n"), err);
		SetTestStepResult(EFail);
		}
	else
		{
		// now get a data source
		iAssistanceData = iAssistanceDataPlugin->NewDataSourceL();
		iDataBuilder.OpenL();
		
		TLbsGpsOptions gpsOptions;
		
		TESTLOG(ELogP2, "Module Loaded\n");		
		
		gpsOptions.SetGpsMode(iAGPSMode);
		// Set the gps mode
		HAIModule()->SetGpsOptions(gpsOptions);	
		
		// get and set the reference position
		TPosition referencePosition;
		_LIT(KReferencePosition, "reference_position");
		if(GetReferencePositionFromConfigL(ConfigSection(), KReferencePosition, iReferencePosition))
			{
			// there is a reference positon in the ini file so use this
			TESTLOG(ELogP2, "Got Reference position from config\n");	
			}
		else
			{
			TESTLOG(ELogP2, "No Reference position in config\n");
			}
		}
			
	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::LoadADPModuleL()");	

	}

void CT_LbsAGPSAssDataStep::UnLoadADPModule()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::UnloadDPModuleL()");		
	iDataBuilder.Close();
	iAssistanceData->CancelGetAssistanceData();//needed?
	delete iAssistanceData;
	delete iAssistanceDataPlugin;
	REComSession::FinalClose();	
	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::UnloadDPModuleL()");	
	}
	

void CT_LbsAGPSAssDataStep::DoImmediateRequestWithUnsolicitedDataTestCase()
	{
	// tese case code
	StartFirstRequest();	
	CT_LbsAGPSHAIValidateStepRoot::StrayUpdatesTimer() = ETrue;	
	TLbsAsistanceDataGroup dataMask = EAssistanceDataAquisitionAssistance | EAssistanceDataBadSatList 
									| EAssistanceDataNavigationModel | EAssistanceDataReferenceTime 
									| EAssistanceDataIonosphericModel | EAssistanceDataDgpsCorrections 
									| EAssistanceDataReferenceLocation | EAssistanceDataAlmanac
									| EAssistanceDataPositioningGpsUtcModel;
	
	iAssistanceData->GetAssistanceData(iDataBuilder, dataMask, iReferencePosition);

	// start scheduler
	CActiveScheduler::Start();
	VerifyStandardResults();
	}	
	
/*
	Verify assistance data details that should be true for every preferterminalmode test:
*/
void CT_LbsAGPSAssDataStep::VerifyStandardAssistanceDataDetails()	
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::VerifyStandardAssistanceDataDetails()");	
			
	if(!iAssistanceDataRequestedCount)
		{
		TESTLOG(ELogP3, "FAILED: No assistance data was requested during this test\n");
		WARN_PRINTF1(_L("<font><b>FAILED: No assistance data was requested during this test\n"));			
		SetTestStepResult(EFail);
		}
	else if (iAssistanceDataRequestedCount > iMaxExpectedDataRequests)
		{
		TESTLOG3(ELogP3, "WARNING: Assistance data was requested more times (%d) during this test than expected (%d)\n", iAssistanceDataRequestedCount, iMaxExpectedDataRequests);
		WARN_PRINTF3(_L("<font><b>WARNING: Assistance data was requested more times (%d) during this test than expected (%d)</b></font>\n"), iAssistanceDataRequestedCount, iMaxExpectedDataRequests);					
		}
	
	TESTLOG2(ELogP3, "Assistance data mask requested last: 0x%x", iLastRequestedAssistanceDataMask);	
	TESTLOG2(ELogP3, "Assistance data that GPS Module was notified of = 0x%x\n", iAssistanceDataNotified);			
	TESTLOG2(ELogP3, "Assistance data returned to module: 0x%x", iAssistanceDataReturned);
		
	}
	
	
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsAGPSAssDataStep::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.	
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::doTestStepL()");		

	if (TestStepResult() == EPass)
		{		
		// Carryout unique test actions.
		if (GetParametersFromConfigL())
			{
			LoadHAIModuleL();		
			LoadADPModuleL();
	
			TESTLOG2(ELogP2, "				----------- Test Case %d -----------				\n\n", iTestCaseId);		
						
			iMaxExpectedDataRequests = 1;	// most test cases expect gps module to request assistance data once
			
			switch (iTestCaseId)
				{
				// gota add case 901 to a script file....
				case 901:		// Load and Unload the HAI Module.
					{
					DoLoadUnLoadTestCase();
					}
					break;
				// gota add these test cases too...
				case 912:// Request location with low accuracy.
				case 902:	// 'immediate' location request with long max fix time and default accuracy.
				case 903:	// non-immediate location request with long max fix time (default accuracy)
					{
					DoImmediateRequestTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;
				case 904:	// Request a second location at different, earlier, target time. 
					{
					DoSecondLocationRequestTestCase();
					VerifyStandardAssistanceDataDetails();					
					}
					break;
					
				case 905:	// Request a second location at different, later, target time before first targettime reached.
					{
					// because first location request tgt time is less than a minute away, we expect the module to ask for assistance data before it's implicitly cancelled:
					iMaxExpectedDataRequests = 2;	
					DoSecondLocationRequestTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;
					   
				case 906: // Request with low accuracy requirement implicitly cancelled by a second location at same target time with higher accuracy.
				case 907:	// Request with low accuracy requirement implicitly cancelled by a second location at same target time with lower accuracy.
					{
					DoLowAccuracyLocationRequestTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;
					
				case 909: // Request an 'immediate' location with default maxfix time, then on maxfix time, request further 'immediate' update(s) with shorter maxfixtime (> TTNF).	
					{
					
					DoTrackingRequestTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;
					
				case 910:	// 'immediate' location request with max fix time = TTFF + processing time (default accuracy).
					{
					DoImmediateRequestWithMaxFixTimeTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;
					
				case 911:	// 'far off' location request with very short max fix time (default accuracy).
					{
					DoFarOffLocationRequestTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;
							
				case 913:	// Make an immediate request with very high accuracy and long maxfix time and then cancel it.
					{					
					DoImmediateRequestWithCancelTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;
					
				case 915:	// Verify that HAI module reports 'Active' device state during location update and drops back to 'Inactive' when no outstanding requests.
					{
					DoHAIToggleActiveStatusTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;
					
				case 916:	// Verify that the HAI module drops back to a low power state when the next location is a long way off. 
					{
					iMaxExpectedDataRequests = 2;	
					DoHAIPowerDropTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;						
					
				case 930:		// Send unsolicited assistance data (Note we're in autonomous mode for this test)
					{
					DoImmediateRequestWithUnsolicitedDataTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;
										
				case 931:		// Return error when assistance data requested.
					{
					AssistanceDataError() = KErrNotFound;
					DoImmediateRequestTestCase();
					VerifyStandardAssistanceDataDetails();
					}
					break;
					
				}
			TESTLOG(ELogP2, "Unloading HAI Module\n");
			UnLoadADPModule();
			UnLoadHAIModule();	

			TESTLOG2(ELogP2, "				----------- End Test Case %d -----------				\n\n", iTestCaseId);									
			}
		else
			{
			TESTLOG(ELogP3, "FAILED: Test case has no entry in config file.");
			ERR_PRINTF1(_L("<font><b>FAILED: Test case has no entry in config file.</font></b>\n"));
			SetTestStepResult(EFail);
			}
		}


	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::doTestStepL()");		
	return TestStepResult();
	}	

	
// from MLbsLocationSourceGpsObserver
// request from integration module
TInt CT_LbsAGPSAssDataStep::GetAssistanceDataItem(TLbsAssistanceDataItem aItem, RDataReaderRootBase& aDataRoot, TTime& aTimeStamp)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::GetAssistanceDataItemL()");
	
	TInt err = KErrNone;
	TBool dataAvailable = EFalse;
	switch(aItem)
    	{
        case EAssistanceDataAquisitionAssistance:
        	{
        	TESTLOG(ELogP1, "GetAssistanceDataItem: EAssistanceDataAquisitionAssistance item requested");
        	iAssistanceDataReturned |= EAssistanceDataAquisitionAssistance;
        	RUEPositioningGpsAcquisitionAssistanceBuilder* builder;
        	iDataBuilder.GetDataBuilder(builder);
        	aDataRoot.DataBuffer() = builder->DataBuffer();	// populates things
            dataAvailable = reinterpret_cast<RUEPositioningGpsAcquisitionAssistanceReader*>(&aDataRoot)->IsDataAvailable();
        	}
            break;    	
        case EAssistanceDataBadSatList:
        	{
        	TESTLOG(ELogP1, "GetAssistanceDataItem: EAssistanceDataBadSatList item requested");
        	iAssistanceDataReturned |= EAssistanceDataBadSatList;
        	RBadSatListBuilder* builder;
        	iDataBuilder.GetDataBuilder(builder);
        	aDataRoot.DataBuffer() = builder->DataBuffer();	
        	dataAvailable = reinterpret_cast<RBadSatListReader*>(&aDataRoot)->IsDataAvailable();
        	}
            break;
        case EAssistanceDataNavigationModel:
        	{
        	TESTLOG(ELogP1, "GetAssistanceDataItem: EAssistanceDataNavigationModel item requested");
        	iAssistanceDataReturned |= EAssistanceDataNavigationModel;
       		RUEPositioningGpsNavigationModelBuilder* builder;
	   		iDataBuilder.GetDataBuilder(builder);
       		aDataRoot.DataBuffer() = builder->DataBuffer();	
			dataAvailable = reinterpret_cast<RUEPositioningGpsNavigationModelReader*>(&aDataRoot)->IsDataAvailable();
        	}
            break;
        case EAssistanceDataReferenceTime:
        	{
        	TESTLOG(ELogP1, "GetAssistanceDataItem: EAssistanceDataReferenceTime item requested");
        	iAssistanceDataReturned |= EAssistanceDataReferenceTime;
       		RUEPositioningGpsReferenceTimeBuilder* builder;
       		iDataBuilder.GetDataBuilder(builder);
       		aDataRoot.DataBuffer() = builder->DataBuffer();	
        	dataAvailable = reinterpret_cast<RUEPositioningGpsReferenceTimeReader*>(&aDataRoot)->IsDataAvailable();
        	}
            break;
        case EAssistanceDataIonosphericModel:
			{	
			TESTLOG(ELogP1, "GetAssistanceDataItem: EAssistanceDataIonosphericModel item requested");
			iAssistanceDataReturned |= EAssistanceDataIonosphericModel;
      		RUEPositioningGpsIonosphericModelBuilder* builder;
      		iDataBuilder.GetDataBuilder(builder);
       		aDataRoot.DataBuffer() = builder->DataBuffer();	        
        	dataAvailable = reinterpret_cast<RUEPositioningGpsIonosphericModelReader*>(&aDataRoot)->IsDataAvailable();
			}
        	break;
        case EAssistanceDataReferenceLocation:
        	{
        	TESTLOG(ELogP1, "GetAssistanceDataItem: EAssistanceDataReferenceLocation item requested");
        	iAssistanceDataReturned |= EAssistanceDataReferenceLocation;
        	RReferenceLocationBuilder* builder;
       		iDataBuilder.GetDataBuilder(builder);
       		aDataRoot.DataBuffer() = builder->DataBuffer();	
        	dataAvailable = reinterpret_cast<RReferenceLocationReader*>(&aDataRoot)->IsDataAvailable();
			}        	
            break;
        case EAssistanceDataAlmanac:
        	{
        	TESTLOG(ELogP1, "GetAssistanceDataItem: EAssistanceDataAlmanac item requested");
        	iAssistanceDataReturned |= EAssistanceDataAlmanac;
       		RUEPositioningGpsAlmanacBuilder* builder;
       		iDataBuilder.GetDataBuilder(builder);
       		aDataRoot.DataBuffer() = builder->DataBuffer();	
        	dataAvailable = reinterpret_cast<RUEPositioningGpsAlmanacReader*>(&aDataRoot)->IsDataAvailable();
        	}
        	break;
        case EAssistanceDataPositioningGpsUtcModel:
        	{
        	TESTLOG(ELogP1, "GetAssistanceDataItem: EAssistanceDataPositioningGpsUtcModel item requested");
        	iAssistanceDataReturned |= EAssistanceDataPositioningGpsUtcModel;
       		RUEPositioningGpsUtcModelBuilder* builder;
       		iDataBuilder.GetDataBuilder(builder);
       		aDataRoot.DataBuffer() = builder->DataBuffer();	
        	dataAvailable = reinterpret_cast<RUEPositioningGpsUtcModelReader*>(&aDataRoot)->IsDataAvailable();
       		}
            break;
        default:
        	TESTLOG(ELogP1, "GetAssistanceDataItem: unknown item requested");
            //Intentionally empty. dataAvailable remains EFalse.
            break;
        }
        
	if (!dataAvailable)
		{
		err = KErrNotFound;
		TESTLOG(ELogP3, "WARNING: AGPS Module asked for assistance data that is unavailable");
		WARN_PRINTF1(_L( "WARNING: AGPS Module asked for assistance data that is unavailable"));		
		}
	else
		{
		aTimeStamp = aDataRoot.TimeStamp();	
		
		TESTLOG(ELogP3, "Timestamp on requested assistance data item:");
		LbsTestUtilities::PrintfDateTimeToDebugLog(aTimeStamp);
		
		}		
	
	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::GetAssistanceDataItemL()");
	return err;
	}


// from MLbsLocationSourceGpsObserver
// request from integration module
TInt CT_LbsAGPSAssDataStep::GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem aItem, TTime& aTimeStamp)
	{
	
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::GetAssistanceDataItemTimeStamp()");
	TInt err = KErrNone;
	TBool dataAvailable = EFalse;
	switch(aItem)
    	{
        case EAssistanceDataAquisitionAssistance:
        	RUEPositioningGpsAcquisitionAssistanceBuilder* builder;
        	iDataBuilder.GetDataBuilder(builder);
            if(builder->IsDataAvailable())
            	{
				aTimeStamp = builder->TimeStamp();	 
				dataAvailable = ETrue;           
            	}
            break;    	
        case EAssistanceDataBadSatList:
        	{
        	RBadSatListBuilder* builder;
        	iDataBuilder.GetDataBuilder(builder);
            if(builder->IsDataAvailable())
            	{
				aTimeStamp = builder->TimeStamp();	            
				dataAvailable = ETrue;           
            	}
        	}
            break;
        case EAssistanceDataNavigationModel:
        	{
       		RUEPositioningGpsNavigationModelBuilder* builder;
	   		iDataBuilder.GetDataBuilder(builder);
            if(builder->IsDataAvailable())
            	{
				aTimeStamp = builder->TimeStamp();	            
				dataAvailable = ETrue;           
            	}
        	}
            break;
        case EAssistanceDataReferenceTime:
        	{
       		RUEPositioningGpsReferenceTimeBuilder* builder;
       		iDataBuilder.GetDataBuilder(builder);
            if(builder->IsDataAvailable())
            	{
				aTimeStamp = builder->TimeStamp();	            
				dataAvailable = ETrue;           
            	}
        	}
            break;
        case EAssistanceDataIonosphericModel:
			{	
      		RUEPositioningGpsIonosphericModelBuilder* builder;
      		iDataBuilder.GetDataBuilder(builder); 
            if(builder->IsDataAvailable())
            	{
				aTimeStamp = builder->TimeStamp();	            
				dataAvailable = ETrue;           
            	}
			}
        	break;
        case EAssistanceDataReferenceLocation:
        	{
        	RReferenceLocationBuilder* builder;
       		iDataBuilder.GetDataBuilder(builder);
            if(builder->IsDataAvailable())
            	{
				aTimeStamp = builder->TimeStamp();	            
				dataAvailable = ETrue;           
            	}
			}        	
            break;
        case EAssistanceDataAlmanac:
        	{
       		RUEPositioningGpsAlmanacBuilder* builder;
       		iDataBuilder.GetDataBuilder(builder);
            if(builder->IsDataAvailable())
            	{
				aTimeStamp = builder->TimeStamp();	            
				dataAvailable = ETrue;           
            	}
        	}
        	break;
        case EAssistanceDataPositioningGpsUtcModel:
        	{
       		RUEPositioningGpsUtcModelBuilder* builder;
       		iDataBuilder.GetDataBuilder(builder);
            if(builder->IsDataAvailable())
            	{
				aTimeStamp = builder->TimeStamp();	            
				dataAvailable = ETrue;           
            	}
       		}
            break;
        default:
            //Intentionally empty. dataAvailable remains EFalse.
            break;
        }
        
	if (!dataAvailable)
		{
		err = KErrNotFound;
		}		
	
	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::GetAssistanceDataItemTimeStamp()");			
	return err;
	}


// from MLbsLocationSourceGpsObserver
// Integration module requests assistance data from network
void CT_LbsAGPSAssDataStep::RequestAssistanceData(TLbsAsistanceDataGroup aDataItemMask)
	{
	TESTLOG2(ELogP1, ">> CT_LbsAGPSAssDataStep::RequestAssistanceData() mask = 0x%x", aDataItemMask);
	
	// gps module should not ask for assistance data during tracking:
	if(CT_LbsAGPSHAIValidateStepRoot::Tracking()) 
		{
		TESTLOG(ELogP3, "WARNING: AGPS Module should not request assistance data while tracking.\n");
		WARN_PRINTF1(_L("<font><b>WARNING: AGPS Module should not request assistance data while tracking.</b></font>\n"));
		}
		
	// If module has already asked for assistance data during this request, it should not be asking for more.
	// It may, however, be 'cancelling' a previous request by asking for less which is ok
	if(iAssistanceDataRequestedCount)	// this is not the first request for assistance data during this test case
		{
		// subsequent request should be for less or no data, not more:
		if(aDataItemMask && (aDataItemMask & (~iLastRequestedAssistanceDataMask)))
			{
			TESTLOG3(ELogP3, "WARNING: AGPS Module should not request more assistance data during a request. Previously requested 0x%x, now requesting 0x%x\n", iLastRequestedAssistanceDataMask, aDataItemMask);
			WARN_PRINTF3(_L("<font><b>WARNING: AGPS Module should not request more assistance data during a request. Previously requested 0x%x, now requesting 0x%x</b></font>\n"), iLastRequestedAssistanceDataMask, aDataItemMask);
			}
		// subsequent requests should not ask for data already returned
		if(aDataItemMask & iAssistanceDataReturned) 
			{
			TESTLOG3(ELogP3, "WARNING: AGPS Module should not request assistance data that it has already received during a request. Previously received 0x%x, now requesting 0x%x\n", iAssistanceDataReturned, aDataItemMask);
			WARN_PRINTF3(_L("<font><b>WARNING: AGPS Module should not request assistance data that it has already received during a request. Previously received 0x%x, now requesting 0x%x</b></font>\n"), iAssistanceDataReturned, aDataItemMask);			
			}
		
		}
		
	iLastRequestedAssistanceDataMask = aDataItemMask;
	iAssistanceDataRequestedCount++;
	
	if(KErrNone == AssistanceDataError())				
		{
		// Ask provider for assistance data:
		iAssistanceData->GetAssistanceData(iDataBuilder, aDataItemMask, iReferencePosition);
		}
	else	// pretend there's a problem getting assistance data
		{
		TLbsAssistanceDataGroup mask = EAssistanceDataNone;
		HAIModule()->AssistanceDataEvent(AssistanceDataError(), mask);
		}
	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::RequestAssistanceData()");				
	}


//----------------------- end MLbsLocationSourceGpsObserver --------------------------


// ----------------------- from MLbsAssistanceDataSourceObserver ---------------------------
	
void CT_LbsAGPSAssDataStep::DataSourceNotification(const TInt& aError)
	{
	TESTLOG2(ELogP1, ">> CT_LbsAGPSAssDataStep::DataSourceNotification(%d)", aError);
	
	// figure out what data was returned and inform module:	
	TLbsAssistanceDataGroup mask;
	RUEPositioningGpsAlmanacBuilder* almanacPtr;
	RUEPositioningGpsIonosphericModelBuilder* ionosphericPtr;
	RUEPositioningGpsNavigationModelBuilder* navigationPtr;
	RUEPositioningGpsReferenceTimeBuilder* referenceTimePtr;
	RUEPositioningGpsUtcModelBuilder*  utcPtr;
	RUEPositioningGpsAcquisitionAssistanceBuilder* acquisitionPtr;
	RBadSatListBuilder* badSatPtr;
	RReferenceLocationBuilder* referenceLocationPtr;
	
	mask = EAssistanceDataNone;

	if (KErrNone == iDataBuilder.GetDataBuilder(almanacPtr))
		{
		if (almanacPtr->IsDataAvailable())
			{
			mask |= EAssistanceDataAlmanac;
			}
		}
	if (KErrNone == iDataBuilder.GetDataBuilder(acquisitionPtr))
		{
		if (acquisitionPtr->IsDataAvailable())
			{
			mask |= EAssistanceDataAquisitionAssistance;
			}
		}
	if (KErrNone == iDataBuilder.GetDataBuilder(badSatPtr))
		{
		if (badSatPtr->IsDataAvailable())
			{
			mask |= EAssistanceDataBadSatList;
			}
		}
	if (KErrNone == iDataBuilder.GetDataBuilder(navigationPtr))
		{
		if (navigationPtr->IsDataAvailable())
			{
			mask |= EAssistanceDataNavigationModel;
			}
		}
	if (KErrNone == iDataBuilder.GetDataBuilder(referenceTimePtr))
		{
		if (referenceTimePtr->IsDataAvailable())
			{
			mask |= EAssistanceDataReferenceTime;
			}
		}
	if (KErrNone == iDataBuilder.GetDataBuilder(ionosphericPtr))
		{
		if (ionosphericPtr->IsDataAvailable())
			{
			mask |= EAssistanceDataIonosphericModel;
			}
		}
	if (KErrNone == iDataBuilder.GetDataBuilder(referenceLocationPtr))
		{
		if (referenceLocationPtr->IsDataAvailable())
			{
			mask |= EAssistanceDataReferenceLocation;
			}
		}
	if (KErrNone == iDataBuilder.GetDataBuilder(utcPtr))
		{
		if (utcPtr->IsDataAvailable())
			{
			mask |= EAssistanceDataPositioningGpsUtcModel;
			}
		}
	
	TESTLOG2(ELogP2, "Notifying module of available assistance data, mask = 0x%x", mask);	
	
	// keep track of the assistance data we've received:
	iAssistanceDataNotified |= mask;
	
	HAIModule()->AssistanceDataEvent(aError, mask);

	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::DataSourceNotification()");
	}

// helper extended config reader
TBool CT_LbsAGPSAssDataStep::GetParametersFromConfigL()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSAssDataStep::GetParametersFromConfigL()");
	
	_LIT(KAssistanceDataModuleId, "assistance_module_uid");
	_LIT(KAgpsMode, "agps_mode");
	TUint32 uidValue;
	CLbsAdmin::TGpsMode mode;
	TBool res = GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId) &&
			GetUidFromConfig(ConfigSection(), KAssistanceDataModuleId, uidValue) &&
			GetAGPSModeFromConfig(ConfigSection(), KAgpsMode, mode);
	iDataProviderPluginUid.iUid = uidValue;
	TESTLOG2(ELogP2, "Data Provider Plugin UID = 0x%x", iDataProviderPluginUid.iUid);
	switch(mode)
		{		
		case CLbsAdmin::EGpsPreferTerminalBased:
			TESTLOG(ELogP2, "AGPS Mode = EGpsPreferTerminalBased");
			break;
		
		case CLbsAdmin::EGpsAutonomous:
			TESTLOG(ELogP2, "AGPS Mode = EGpsAutonomous");
			break;
			
		case CLbsAdmin::EGpsModeUnknown:
		case CLbsAdmin::EGpsAlwaysTerminalBased:
		case CLbsAdmin::EGpsPreferTerminalAssisted:
		case CLbsAdmin::EGpsAlwaysTerminalAssisted:
		default:
			{
			TESTLOG2(ELogP2, "FAILED: AGPS Mode = %d - Bad mode!", mode);
			SetTestStepResult(EFail);
			}
		}
	iAGPSMode = mode;
	TESTLOG(ELogP1, "<< CT_LbsAGPSAssDataStep::GetParametersFromConfigL()");
	return res;		
	}
	
