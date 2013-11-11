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
// TE_mmLocationServices.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef _TE_MMNOTIFYMEASUREMENTCONTROL_H_
#define _TE_MMNOTIFYMEASUREMENTCONTROL_H_

#include "TE_EtelMMTestStepBase.h"


	
// Test successfull Mobile Terminated Location Request(MTLR) and Measurement Control request
class CTestNotifyMeasurementControl : public CEtelMMLbsTestStepBase
	{
public:
	virtual enum TVerdict doTestStepL();
//	void ValidateMCParams();	
//	void ValidateMCRefTimeParams();
//	void ValidateMCRefLocParams();
//	void ValidateMCModelParams();
	
private:
	/**
		TNotifyMtlrV7 holds the details of the MTLR request being made by the client
	*/
	RMobileLocationServices::TNotifyMtlrV7   iNotifyMtlr;
 	/**
		TMtlrResponse holds the type of response being sent back for the MTLR request made
	*/
	RMobileLocationServices::TMtlrResponse iMtlrResponse;
	/**
		TMeasurementControlV7 contains the GPS assistance data needed by the UE to compute the location 
		of the UE. The UE computes the location once it receives all required information.
	*/	
//	RMobileLocationServices::TMeasurementControlV7 iMeasurementControl;
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

#endif
