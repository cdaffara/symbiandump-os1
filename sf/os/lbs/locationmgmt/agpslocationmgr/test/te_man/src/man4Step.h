/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file man4Step.h
*/
#if (!defined __MAN4_STEP_H__)
#define __MAN4_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_manSuiteStepBase.h"
//jcm #include "lbsassistancedatacacheapi.h"
#include "LbsInternalInterface.h"
#include "LbsLocSourceTestChannel.h"

class Cman4Step : public CTe_manSuiteStepBase
//				  public MLbsNetChannelObserver
	{
public:
	Cman4Step();
	~Cman4Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	void WaitAndValidateLocUpd(TRequestStatus& aStatus, TInt aError = KErrNone, TBool aConflictControl = EFalse);
	void WaitAndValidateNoLocUpd(TRequestStatus& aStatus);
	void WaitAndValidateModuleMsg(TRequestStatus& aStatus, const RAgpsModuleTestChannel::TTestMsgType& aMsg);
	void WaitNoModuleMsg(TRequestStatus& aStatus);
	// From MLbsNetChannelObserver
	//virtual void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	
private:
//	RAssistanceDataCache 	   iADCache;
	RLbsModuleStatus 		   iMSInt;
	RLbsPositionUpdateRequests iLSInt;
	RLbsPositionUpdateRequests iNRHInt;
	RLbsPositionUpdates		   iLocUpdInt;
	RAgpsModuleTestChannel	   iModuleInt;
	RProperty 				   iUpdateCodeProperty;	
	// Used for checking target time in combined requests
	TTime						iRcvdTargetTime;
	// Next 2 used for checking fix time in requests
	TInt						iRcvdFixTime;
	RProperty					iUpdateFixTimeProperty;
	};

_LIT(Kman4Step,"man4Step");

#endif
