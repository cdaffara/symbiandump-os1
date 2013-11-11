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
// Logging definitions
// 
//

/**
 @file 	
 @internalTechnology
*/

#ifndef CCTSYTESTLOGGING_H_
#define CCTSYTESTLOGGING_H_

#include <comms-infras/commsdebugutility.h>

_LIT8(KTSYSubSystem1, "tsy");
_LIT8(KTSYCompnt1, "ctsytestframework");

#define TEST_FRAMEWORK_LOG1(AAA)             	{ __FLOG_STATIC0(KTSYSubSystem1, KTSYCompnt1, (AAA)); }
#define TEST_FRAMEWORK_LOG2(AAA,BBB)         	{ __FLOG_STATIC1(KTSYSubSystem1, KTSYCompnt1, (AAA), (BBB)); }
#define TEST_FRAMEWORK_LOG3(AAA,BBB,CCC)     	{ __FLOG_STATIC2(KTSYSubSystem1, KTSYCompnt1, (AAA), (BBB), (CCC)); }
#define TEST_FRAMEWORK_LOG4(AAA,BBB,CCC,DDD) 	{ __FLOG_STATIC3(KTSYSubSystem1, KTSYCompnt1, (AAA), (BBB), (CCC), (DDD)); }
#define TEST_FRAMEWORK_LOG5(AAA,BBB,CCC,DDD,EEE){ __FLOG_STATIC4(KTSYSubSystem1, KTSYCompnt1, (AAA), (BBB), (CCC), (DDD), (EEE)); }
#define TEST_FRAMEWORK_LOG6(AAA,BBB,CCC,DDD,EEE,FFF){ __FLOG_STATIC5(KTSYSubSystem1, KTSYCompnt1, (AAA), (BBB), (CCC), (DDD), (EEE), (FFF)); }

#endif /*CCTSYTESTLOGGING_H_*/
