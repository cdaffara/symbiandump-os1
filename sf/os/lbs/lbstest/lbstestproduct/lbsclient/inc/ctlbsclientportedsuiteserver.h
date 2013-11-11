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
// ctlbsclientportedserver.h
// Test server for ported client tests
// 
//

#ifndef __CT_LBS_CLIENT_PORTED_SERVER_H__
#define __CT_LBS_CLIENT_PORTED_SERVER_H__

#include <test/testexecuteserverbase.h>

#include "ctlbsserver.h"

// CONSTANTS
_LIT(KTestProcedure1, "TP1");
_LIT(KTestProcedure2, "TP2");
_LIT(KTestProcedure11, "TP11");
_LIT(KTestProcedure17, "TP17");
_LIT(KTestProcedure21, "TP21");
_LIT(KTestProcedure22, "TP22");
_LIT(KTestProcedure25, "TP25");
_LIT(KTestProcedure27, "TP27");
_LIT(KTestProcedure76, "TP76");
_LIT(KTestProcedure93, "TP93");
_LIT(KTestProcedure169, "TP169");
_LIT(KTestProcedure173, "TP173");
_LIT(KTestProcedure176, "TP176");
_LIT(KTestProcedure178, "TP178");
_LIT(KTestProcedure179, "TP179");
_LIT(KTestProcedure188, "TP188");
_LIT(KTestProcedure189, "TP189");
_LIT(KTestProcedure193, "TP193");
_LIT(KTestProcedure194, "TP194");
_LIT(KTestProcedure195, "TP195");
_LIT(KTestProcedure201, "TP201");
_LIT(KTestProcedure203, "TP203");
_LIT(KTestProcedure204, "TP204");
_LIT(KTestProcedure208, "TP208");
_LIT(KTestProcedure213, "TP213");
_LIT(KTestProcedure217, "TP217");
_LIT(KTestProcedure218, "TP218");
_LIT(KTestProcedure222, "TP222");
_LIT(KTestProcedure223, "TP223");
_LIT(KTestProcedure224, "TP224");
_LIT(KTestProcedure227, "TP227");
_LIT(KTestProcedure254, "TP254");
_LIT(KTestProcedure256, "TP256");
_LIT(KTestProcedure257, "TP257");
_LIT(KTestProcedure259, "TP259");
_LIT(KTestProcedure261, "TP261");
_LIT(KTestProcedure269, "TP269");
_LIT(KTestProcedure271, "TP271");
_LIT(KTestProcedure272, "TP272");
_LIT(KTestProcedure274, "TP274");
_LIT(KTestProcedure275, "TP275");
_LIT(KTestProcedure4701, "TP4701");
_LIT(KTestProcedure4702, "TP4702");
_LIT(KTestProcedure4703, "TP4703");
_LIT(KTestProcedure4704, "TP4704");
_LIT(KTestProcedure4705, "TP4705");
_LIT(KTestProcedure4706, "TP4706");
_LIT(KTestProcedure4707, "TP4707");
_LIT(KTestProcedure4708, "TP4708");
_LIT(KTestProcedure4709, "TP4709");
_LIT(KTestProcedure4722, "TP4722");
_LIT(KTestProcedure4723, "TP4723");
_LIT(KTestProcedure300, "TP300");
_LIT(KTestProcedure301, "TP301");
_LIT(KTestProcedure302, "TP302");
_LIT(KTestProcedure304, "TP304");

class CT_LbsClientPortedSuite : public CT_LbsServer
	{
public:
	static CT_LbsClientPortedSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

// Please Add/modify your class members
private:
	};

#endif // __CT_LBS_CLIENT_PORTED_SERVER_H__
