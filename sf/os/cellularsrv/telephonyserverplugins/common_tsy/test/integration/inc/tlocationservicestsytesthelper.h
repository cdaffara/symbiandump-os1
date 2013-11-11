// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TLOCATIONSERVICESTSYTESTHELPER_H
#define TLOCATIONSERVICESTSYTESTHELPER_H


#include "ttsytesthelperbase.h"
	
class TLocationServicesTsyTestHelper : public TTsyTestHelperBase
	{
public:
	TLocationServicesTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForMobileLocationServicesNotifyMtlr( 
				RMobileLocationServices& aMobileLocationServices,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileLocationServices::TNotifyMtlrV7> &aNotifyMtlrV7,
				TInt aWantedStatus );

	void WaitForMobileLocationServicesNotifyMeasurementControl( 
				RMobileLocationServices& aMobileLocationServices,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileLocationServices::TMeasurementControlV7> &aMeasurementControlV7,
				TInt aWantedStatus );

	}; // class TLocationServicesTsyTestHelper 

#endif
