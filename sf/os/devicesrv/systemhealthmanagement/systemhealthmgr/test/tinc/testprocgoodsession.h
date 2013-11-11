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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TESTPROCGOODSESSION_H
#define __TESTPROCGOODSESSION_H

#include <e32std.h>
#include <startup.hrh>
#include "testapps.h"

const TInt KProcPanic = -1234;	//Random panic reason code
/** If KTestProcGood is launched with this command line parameter, it will start a CServer2 */
_LIT(KLaunchServerCommandLineOption, "-server");

/** If KTestProcGood is launched with this command line parameter, it will not rendevouz */
_LIT(KTestProcGoodNoRendevouz, "-norv");

/** If KTestProcGood is launched with this command line parameter, it will exit with reason 
KErrAbort immediately after rendevouz */
_LIT(KFailAfterRendevouzCommandLineOption, "-failafterrv");

/** If KTestProcGood is launched with this command line parameter, it will panic with reason 
KProcPanic panic code*/
_LIT(KPanicCommandLineOption1, "-paniccode");

/** If KTestProcGood is launched with this command line parameter, it will panic with reason 
0*/
_LIT(KPanicCommandLineOption2, "-paniczero");

_LIT(KPanicFile, "c:\\panicfile.txt");

enum TMessages
	{
	EMonitorSelfWithStartupProp,
	EMonitorSelfWithSsmStartupProp,
	ECancelMonitor,
	EShutDown
	};

class RTestProcGoodSession : public RSessionBase
	{
public:
	//from RSessionBase
	inline TInt Connect()
		{
		return RSessionBase::CreateSession(KTestProcGood, TVersion(0,0,0));
		}
	inline TInt MonitorSelfWithStartupProp(TRecoveryMethod aRecoveryMethod)
		{
		return SendReceive(EMonitorSelfWithStartupProp, TIpcArgs((TInt)aRecoveryMethod));
		} //lint !e1762
	inline TInt MonitorSelfWithSsmStartupProp(TRecoveryMethod aRecoveryMethod)
		{
		return SendReceive(EMonitorSelfWithSsmStartupProp, TIpcArgs((TInt)aRecoveryMethod));
		} //lint !e1762
	inline TInt CancelMonitor()
		{
		return SendReceive(ECancelMonitor, TIpcArgs(TIpcArgs::ENothing));
		} //lint !e1762
	inline TInt ShutDown()
		{
		return SendReceive(EShutDown, TIpcArgs(TIpcArgs::ENothing));
		} //lint !e1762
	};

#endif // __TESTPROCGOODSESSION_H
