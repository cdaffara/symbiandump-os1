// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TCUSTCMD_CONST_H__
#define __TCUSTCMD_CONST_H__

#include <e32property.h>

//Swp policy file for sim status swp
_LIT(KTestSimStatusSwpPolicy, "ssm.swp.policy.simstatus.dll");

//Exe name which defines startup PS keys
_LIT (KExeToDefineStartUpPS, "\\sys\\bin\\definestartupps.exe");
//Exe name which defines startup PS keys
_LIT (KExeToDefineStartUpStatePS, "\\sys\\bin\\definesecuritystateps.exe");


_LIT(KDirNameOfTestCasesNumFile, "c:\\cmdsecuritychecktest\\");
// Use to write the PIN Check Security test case nos.
_LIT(KTestCmdSecurityCheckTestFile, "c:\\cmdsecuritychecktest\\pinchecksecuritycaseno.txt");

//Exe name which defines emergency call PS keys
_LIT (KExeToDefineEmergencyCallPS, "\\sys\\bin\\defineemergencycallps.exe");

_LIT(KDllName, "customcmds.dll");

//static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);

const TUid KPropertyCategory={0x2000D75B};				// tcustomcmd_server SID = KSsmServerName SID (changed in tcustomcmd_server.mmp file)
const TUid KCustomcmdServerSID = KPropertyCategory;

const TUint KCustomcmdServerPropertyKey = 0x0012AC;

const TUint KTestSwp={0x00B0BE};


const TInt KSecurityCustCmdOrdinal = 1;
const TInt KActivateEmergencyCallOrdinal = 4;
const TInt KDeActivateEmergencyCallOrdinal = 6;

enum TRPropertyValCustCmd
	{
	//create and execute ActivateEmergencyCall custom command
	ERPropertyValForActivateEmergencyCall = 11,
	//create and execute DeActivateEmergencyCall custom command
	ERPropertyValForDeActivateEmergencyCall = 22,
	//create and execute security custom command with execution behaviour WaitForSignal
	ERPropertyValForSecCustCmdWaitForSignal = 33,
	//create and execute security custom command with execution behaviour DeferredWaitForSignal
	ERPropertyValForSecCustCmdDeferrdWaitForSignal = 44,
	//create and execute security custom command with execution behaviour fireandforget
	ERPropertyValForSecCustCmdFireAndForget = 55,
		//create and execute security custom command with execution behaviour WaitForSignal
	ERPropertyValForDeviceSecCustCmdWaitForSignal = 333,
	//create and execute security custom command with execution behaviour DeferredWaitForSignal
	ERPropertyValForDeviceSecCustCmdDeferrdWaitForSignal = 444,
	//create and execute security custom command with execution behaviour fireandforget
	ERPropertyValForDeviceSecCustCmdFireAndForget = 555,
	// stop simulating key in of security pin check when execution is waitforsignal
	ERPropertyValForStopSchedulerCmdWaitForSignal = 66,
	// stop simulating key in of security pin check when execution is fireandforget and DeferredWaitForSignal
	ERPropertyValForStopSchedulerCmdDeferredWaitForSignal = 77,
	// stop simulating key in of security pin check when execution is fireandforget and FireAndForget
	ERPropertyValForStopSchedulerCmdFireAndForget = 88,
	// to issue a cancel request for security pin check custom command
	ERPropertyValForSecCustCmdCancelRequest = 99
	};	

_LIT(KTestSwpPolicyCustomCmd, "ssm.swp.policy.test.customcmd.dll");
const TUint32 KEmergencyCallRfAdaptationPluginPropertyKey = 0x2000E657;

#endif // __TCUSTCMD_CONST_H__
