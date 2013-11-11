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

#ifndef TESTSTEPCIG711DECODERCONFIG_H
#define TESTSTEPCIG711DECODERCONFIG_H

#include <e32base.h>
#include <testframework.h>

// Custom interface definitions
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>


#include "devsoundciutestdevices.hrh"

// Test system includes
#include "tsu_mmf_devsound_ciu_step.h"
#include "tsu_mmf_devsound_ciu_suite.h"


class CTestStepCIG711DecoderConfig : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIG711DecoderConfig(const TDesC& aTestName):CTestStepCustominterfaceUnificationUnit(aTestName){}
	TVerdict DoTestStepL();

private:
	TVerdict DoTestStep0040L();
	TVerdict DoTestStep0041L();
	TVerdict DoTestStep0042L();
	TVerdict DoTestStep0043L();
	TVerdict DoTestStep0044L();
	TVerdict DoTestStep0094L();
	TVerdict DoTestStep0095L();
	};

#endif
