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
//

#ifndef TSI_MMF_MVS_PLUGIN_INFO_H
#define TSI_MMF_MVS_PLUGIN_INFO_H

#include <badesca.h>

#include "tsi_mvs_agents_suite.h"
#include "tsi_mvs_agents_stepbase.h"


class RTestStepMVSPluginInfo;

/**
 * 
 * RTestStepMVSPluginInfoPos
 *   MVS Plugin Info Positive Tests
 * 
 */
class RTestStepMVSPluginInfoPos : public RTestStepMVSPluginInfo
    {
public:
    RTestStepMVSPluginInfoPos(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid aExpectedMediaType, const TUid aControllerUid);
    //RTestStep
    virtual TVerdict DoTestStepL();
    virtual TVerdict PerformTestL();
    };

/**
 * 
 * RTestStepMVSPluginInfoNeg
 *   MVS Plugin Info Negative Tests
 * 
 */
class RTestStepMVSPluginInfoNeg : public RTestStepMVSPluginInfo
    {
public:
    RTestStepMVSPluginInfoNeg(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid aExpectedMediaType, const TUid aControllerUid);
    //RTestStep
    virtual TVerdict DoTestStepL();
    virtual TVerdict PerformTestL();
    };


#endif// __TSI_MMF_MVS_PLUGIN_INFO_H__

