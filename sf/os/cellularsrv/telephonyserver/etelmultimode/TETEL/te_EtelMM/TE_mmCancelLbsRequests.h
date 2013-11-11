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
//

/**
 @file
 @internalComponent
*/

//Cancel all MTLR requests
class CTestCancelLbsRequests : public CEtelMMLbsTestStepBase
	{
public:
	virtual enum TVerdict doTestStepL();
private:
	/**
		TNotifyMtlrV7 holds the details of the MTLR request being made by the client
	*/
	RMobileLocationServices::TNotifyMtlrV7   iNotifyMtlr; 
	/**
	TStartMolrV7 lists the details sent by UE to Network when a Mobile Originating Location Reqquest(MOLR) request is being made.
	*/	
	RMobileLocationServices::TStartMolrV7    iStartMolr;
 	/**
		TMtlrResponse holds the type of response being sent back for the MTLR request made
	*/
	RMobileLocationServices::TMtlrResponse iMtlrResponse;
	/**
		TMolrReturnResultV7 lists all the MOLR return results that can be expected for the MOLR request being made.	
	*/		
	RMobileLocationServices::TMolrReturnResultV7 iMolrReturnResult;
	/**
		TMeasurementControlV7 contains the GPS assistance data needed by the UE to compute the location 
		of the UE. The UE computes the location once it receives all required information.
	*/	
	RMobileLocationServices::TMeasurementControlV7 iMeasurementControl;
	/**
		TMeasurementReportV7 contains the measurement report expected as response to the measurement control sent by the 
		network.
	*/	
	RMobileLocationServices::TMeasurementReportV7 iMeasurementReport;	
	/**
		iTestCount holds test count number
	*/
	TInt iTestCount;		
	};
