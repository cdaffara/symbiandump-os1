// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The PSY base test step.
// 
//

#if (!defined __TE_LBSPSYBASE_STEP__)
#define __TE_LBSPSYBASE_STEP__

#include <e32base.h>

#include "epos_mposmodulestatusmanager.h"
#include "epos_mposmodulesettingsmanager.h"

#include "Te_BaseStep.h"
#include "LbsInternalInterface.h"

#include "te_active.h"

const TInt KMaxPosInfoNumber = 5;
const TInt KOldPosUpdateAge = 10;  //in seconds
const TInt KUsualTimeout = 1000000; //in microseconds 
const TInt KTrackingInterval = 12000000; //in microseconds 
const TInt KWarmDownInterval = 15000000; //in microseconds
const TInt KLongTimeoutInterval = 15000000; //in microseconds

_LIT(KPsyUid, "PsyUid");
_LIT(KExtraPositioner, "ExtraPositioner");
_LIT(KPosInfo, "PosInfo");
_LIT(KCourseInfo, "CourseInfo");
_LIT(KSatInfo, "SatInfo");
_LIT(KGenericInfo, "GenericInfo");

class CTe_PsyPositioner;
class CTe_PsyRequest;
class MTe_LbsPsyStaticData;

/**
The base PSY test step.
*/
class CTe_PsyBaseStep : public CTe_BaseStep, public MTe_ActiveObserver
	{
public:		
	//From MTe_ActiveObserver
	IMPORT_C virtual void OnRunLCall(const CTe_Active& aActive);
	
protected:
	IMPORT_C CTe_PsyBaseStep(MTe_LbsPsyStaticData* aPsyStaticData);
	void BaseConstructL();
	IMPORT_C virtual ~CTe_PsyBaseStep();
	IMPORT_C void StartSchedulerWaitL(TUint aTimeout, TUint aAsyncCallsExpected);
	
	IMPORT_C void WaitCheckCancelReceivedL(CTe_PsyRequest& aUpdateRequest);
	IMPORT_C void WaitCheckNotifyReqReceivedL(CTe_PsyRequest& aUpdateRequest);
	
	IMPORT_C virtual void ReallyDoTestStepL();
	virtual void ReallyDoTestStepImpL() = 0;
	
	IMPORT_C CTe_PsyPositioner* MakePositionerLC();
	IMPORT_C CTe_PsyPositioner* MakePositionerLC(TInt aPsyUid);
	TInt PsyUid();	
protected:	
	CTe_PsyPositioner* iExtraPositioner;
	
private:
	TInt iAsyncCalls;
	TInt iPsyUid;
	MTe_LbsPsyStaticData* iPsyStaticData;
	};
		
#endif //__TE_LBSPSYBASE_STEP__
