// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestStepMmfPerformance.h
//

#if (!defined __MMF__PERFORMANCE_TESTSTEP_H__)
#define __MMF__PERFORMANCE_TESTSTEP_H__

#include <e32base.h>
#include <fbs.h>
#include <bitstd.h>
#include <bitdev.h>

/**
 *
 * RTestStepMmfPerformance, which is the base class for all 
 * the MMF performance suite test steps, providing common functionality.
 *
 * @lib "TSU_MMF_PFM_01.lib"
 *
 */
class RTestStepMmfPerformance : public RTestStep
	{
public:
	//From RTestStep
	TVerdict OpenL();
	void Close();


private:
	CActiveScheduler* iScheduler ;
	};

#endif /* __MMF__PERFORMANCE_TESTSTEP_H__ */

