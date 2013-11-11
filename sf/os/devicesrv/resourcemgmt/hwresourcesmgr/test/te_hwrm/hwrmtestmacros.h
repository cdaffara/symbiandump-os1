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
//

#ifndef hwrmtestmacros_h
#define hwrmtestmacros_h
		
#define ASSERT_LEAVE(err,x) 								\
{ 															\
	ExpectLeave(err); 										\
	x;														\
	EndExpectLeaveL(err,(TText8*)__FILE__,__LINE__);		\
}

// ISO C++ compliant version of ADD_TEST_STEP which uses correct member function
// address syntax (e.g., &ClassName::MemberName rather than &MemberName).  This
// is necessary to compile without errors for the x86gcc target.
#define ADD_TEST_STEP_ISO_CPP(suiteClassName, stepName) \
	_LIT( KTest ## stepName, #stepName); \
	AddTestCase(lTestSuite, KTest ## stepName, &suiteClassName::stepName);
		
#endif //hwrmtestmacros_h
