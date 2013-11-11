// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_ACOD_H__
#define __TSU_MMF_ACOD_H__

#include <testframework.h>
#include <mmf/server/mmfcodec.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>

#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <mmf/plugin/mmfplugininterfaceuids.hrh>

#include "TSU_MMF_ACOD_BufferSizes.h"

/** @xxxx
 * @class CTestStep_MMF_SwCodecDevices
 * @brief Test class that enables MMF Audio Codec tests.
 * It's a base class for all test steps.
 */
class CTestSuite_MMF_SwCodecDevices;
class CTestStep_MMF_SwCodecDevices : public CTestStep
	{
public:
	CTestStep_MMF_SwCodecDevices();
	virtual ~CTestStep_MMF_SwCodecDevices();
	void SetSuite( CTestSuite_MMF_SwCodecDevices* aPtr );
    CTestSuite_MMF_SwCodecDevices* GetSuite();
private:
	CTestSuite_MMF_SwCodecDevices* iRefSuite;
	};

#endif// TSU_MMF_ACOD_H__

