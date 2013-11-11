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
 @file Te_manSuiteStepBase.h
*/

#if (!defined __TE_MAN_STEP_BASE__)
#define __TE_MAN_STEP_BASE__
#include <test/testexecutestepbase.h>
#include "lbsnetinternalapi.h"
#include "testmessageinterface.h"

/****************************************************************************
* Base Test Step class for te_mansuite test steps
* 
****************************************************************************/
class CTe_manSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_manSuiteStepBase();
	CTe_manSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	void SetNGInterface(MNGTestInterface* aNGInterface);
	
protected:
	void WaitAndValidateNgMsg(TLbsNetInternalMsgBase::TLbsNetInternalMsgType aMsgType, TBool aFlag = EFalse);
	void SendNgMsg(const TLbsNetInternalMsgBase& aMsg);
	void SetupNGInterface();
	
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	
    // Active scheduler
	CActiveScheduler* iSched;
private:
	MNGTestInterface* iNgInterface;
	};

#endif
