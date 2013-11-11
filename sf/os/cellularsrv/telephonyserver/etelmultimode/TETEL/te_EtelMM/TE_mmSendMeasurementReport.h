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

#ifndef _TE_MMSENDMEASUREMENTREPORT_H_
#define _TE_MMSENDMEASUREMENTREPORT_H_

#include "TE_EtelMMTestStepBase.h"

	
class CTestSendMeasurementReport : public CEtelMMLbsTestStepBase
	{
public:
	virtual enum TVerdict doTestStepL();
//	void ValidateMRParams();
private:
	/**
		TMeasurementReportV7 contains the measurement report expected as response to the measurement control sent by the 
		network.
	*/	
//	RMobileLocationServices::TMeasurementReportV7 iMeasurementReport;

	RMobileLocationServices::TMeasurementControlFailure iMeasurementControlFailure;
	/**
		iTestCount holds test count number
	*/
	TInt iTestCount;	
	};

#endif
