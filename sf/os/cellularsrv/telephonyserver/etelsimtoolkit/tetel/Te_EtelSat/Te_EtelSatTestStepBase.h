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

#ifndef __TE_ETELSAT_TESTSTEP_BASE__
#define __TE_ETELSAT_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include "f32file.h"
#include "c32comm.h"

#include <e32base.h>

#include "testdef.h"
#include "TSatLog.h"
#include "Etelsat.h"
#include <satcs.h>

#include "Tetelcs.h"

class CRSatTestStep : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
protected:
	RTelServer iTelServer;
	TInt iTestCount;
	RPhone phone;
	RSat sat;
	TRequestStatus reqStatus;
	TInt action;
	TInt icon;
	RSat::TItem item;
	RSat::TTextAttribute iTextAttribute;
	TInt index;
private:
	CActiveScheduler* iTestScheduler;
	};

class RTestSat : public RSat
	{
public:
	RTestSat();
	TInt TestCustomIPC1();
	};

#endif // __TE_ETELSAT_TESTSTEP_BASE__


