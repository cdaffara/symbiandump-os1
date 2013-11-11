// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LBS_TEST_LOGGER_MACROS_H
#define LBS_TEST_LOGGER_MACROS_H

#include "lbstestlogger.h"

#ifdef ENABLE_LBS_TEST_LOGGER
	#define TESTLOG(L1,L2) 							LbsTestLogger::Write(ELogNormal, L1, _L(L2))
	#define TESTLOG2(L1,L2,L3) 						LbsTestLogger::Write(ELogNormal, L1, _L(L2), L3)
	#define TESTLOG3(L1,L2,L3,L4) 					LbsTestLogger::Write(ELogNormal, L1, _L(L2), L3, L4)
	#define TESTLOG4(L1,L2,L3,L4,L5) 				LbsTestLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5)
	#define TESTLOG5(L1,L2,L3,L4,L5,L6) 			LbsTestLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5, L6)
	#define TESTLOG6(L1,L2,L3,L4,L5,L6,L7)			LbsTestLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5, L6, L7)
	#define TESTLOG7(L1,L2,L3,L4,L5,L6,L7,L8)		LbsTestLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5, L6, L7, L8)
	#define TESTLOG8(L1,L2,L3,L4,L5,L6,L7,L8,L9)	LbsTestLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5, L6, L7, L8, L9)	
	#define TESTLOGHEX(L1,L2,L3)					LbsTestLogger::HexDump(L1,L2,L3)


	
	
	#define TESTLOG_ERR(L1,L2) 					LbsTestLogger::Write(ELogError, L1, _L(L2))
	#define TESTLOG_ERR2(L1,L2,L3) 				LbsTestLogger::Write(ELogError, L1, _L(L2), L3)
	#define TESTLOG_ERR3(L1,L2,L3,L4) 			LbsTestLogger::Write(ELogError, L1, _L(L2), L3, L4)
	#define TESTLOG_ERR4(L1,L2,L3,L4,L5) 		LbsTestLogger::Write(ELogError, L1, _L(L2), L3, L4, L5)
	#define TESTLOG_ERR5(L1,L2,L3,L4,L5,L6) 	LbsTestLogger::Write(ELogError, L1, _L(L2), L3, L4, L5, L6)
	#define TESTLOG_ERR6(L1,L2,L3,L4,L5,L6,L7)	LbsTestLogger::Write(ELogError, L1, _L(L2), L3, L4, L5, L6, L7)

	#define TESTLOG_WARN(L1,L2) 				LbsTestLogger::Write(ELogWarning, L1, _L(L2))
	#define TESTLOG_WARN2(L1,L2,L3) 			LbsTestLogger::Write(ELogWarning, L1, _L(L2), L3)
	#define TESTLOG_WARN3(L1,L2,L3,L4) 			LbsTestLogger::Write(ELogWarning, L1, _L(L2), L3, L4)
	#define TESTLOG_WARN4(L1,L2,L3,L4,L5) 		LbsTestLogger::Write(ELogWarning, L1, _L(L2), L3, L4, L5)
	#define TESTLOG_WARN5(L1,L2,L3,L4,L5,L6) 	LbsTestLogger::Write(ELogWarning, L1, _L(L2), L3, L4, L5, L6)
	#define TESTLOG_WARN6(L1,L2,L3,L4,L5,L6,L7)	LbsTestLogger::Write(ELogWarning, L1, _L(L2), L3, L4, L5, L6, L7)
#else
	#define TESTLOG(L1,L2)
	#define TESTLOG2(L1,L2,L3)
	#define TESTLOG3(L1,L2,L3,L4)
	#define TESTLOG4(L1,L2,L3,L4,L5)
	#define TESTLOG5(L1,L2,L3,L4,L5,L6)
	#define TESTLOG6(L1,L2,L3,L4,L5,L6,L7)
	#define TESTLOG7(L1,L2,L3,L4,L5,L6,L7,L8)	
	#define TESTLOG8(L1,L2,L3,L4,L5,L6,L7,L8,L9)
	
	#define TESTLOGHEX(L1,L2,L3)
	
	#define TESTLOG_ERR(L1,L2)
	#define TESTLOG_ERR2(L1,L2,L3)
	#define TESTLOG_ERR3(L1,L2,L3,L4)
	#define TESTLOG_ERR4(L1,L2,L3,L4,L5)
	#define TESTLOG_ERR5(L1,L2,L3,L4,L5,L6)
	#define TESTLOG_ERR6(L1,L2,L3,L4,L5,L6,L7)

	#define TESTLOG_WARN(L1,L2)
	#define TESTLOG_WARN2(L1,L2,L3)
	#define TESTLOG_WARN3(L1,L2,L3,L4)
	#define TESTLOG_WARN4(L1,L2,L3,L4,L5)
	#define TESTLOG_WARN5(L1,L2,L3,L4,L5,L6)
	#define TESTLOG_WARN6(L1,L2,L3,L4,L5,L6,L7)
#endif // ENABLE_LBS_TEST_TESTLOGGER

#endif //LBS_TEST_LOGGER_MACROS_H

