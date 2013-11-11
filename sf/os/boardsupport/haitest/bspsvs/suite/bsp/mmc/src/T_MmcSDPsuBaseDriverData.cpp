/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "T_MmcSDPsuBaseDriverData.h"

//MMCSD Commands index
/*@{*/

_LIT(KCmdIsLocked,							"IsLocked");
_LIT(KCmdIsOff,								"IsOff");
_LIT(KCmdMaxCurrentInMicroAmps,				"MaxCurrentInMicroAmps");
_LIT(KCmdResetInactivityTimer,				"ResetInactivityTimer");
_LIT(KCmdVoltageSupported,					"VoltageSupported");
_LIT(KCmdInactivityCount,					"InactivityCount");
_LIT(KCmdNotLockedCount,					"NotLockedCount");
_LIT(KCmdInactivityTimeout,					"InactivityTimeout");
_LIT(KCmdNotLockedTimeout,					"NotLockedTimeout");
_LIT(KCmdMemVoltageSupported,				"MemVoltageSupported");
_LIT(KCmdMemMaxCurrentInMicroAmps,			"MemMaxCurrentInMicroAmps");
_LIT(KCmdVoltCheckInterval,					"VoltCheckInterval");


_LIT(KExpectedLockState,					"expectedlockstate");
_LIT(KExpectedPsuState,						"expectedpsustate");
_LIT(KExpectedCurrent,						"expectedcurrent");
_LIT(KExpectedVoltage,						"expectedvoltage");
_LIT(KExpectedInactivityCount,				"expectedinactivityCount");
_LIT(KExpectedNotLockedCount,				"expectednotLockedCount");
_LIT(KExpectedInactivityTimeout,			"expectedinactivityTimeout");
_LIT(KExpectedNotLockedTimeout,				"expectednotlockedtimeOut");
_LIT(KExpectedMemVoltageSupported,			"expectedmemVoltageSupported");
_LIT(KExpectedMemMaxCurrentInMicroAmps,		"expectedmemMaxCurrentInMicroAmps");
_LIT(KExpectedVoltageinterval,				"expectedvoltageinterval");

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMCSDPsuBaseDriverData* CT_MMCSDPsuBaseDriverData::NewL()
	{
	CT_MMCSDPsuBaseDriverData*	ret=new (ELeave) CT_MMCSDPsuBaseDriverData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_MMCSDPsuBaseDriverData::CT_MMCSDPsuBaseDriverData()
:	CT_MmcSDDriverData()
,	iAsyncErrorIndex(0)
	{
	}

void CT_MMCSDPsuBaseDriverData::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	CT_MmcSDDriverData::ConstructL();
	}

CT_MMCSDPsuBaseDriverData::~CT_MMCSDPsuBaseDriverData()
/**
 * Public destructor
 */
	{
	}

TBool CT_MMCSDPsuBaseDriverData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
 	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdIsLocked )
		{
		DoCmdIsLocked(aSection);
		}
	else if ( aCommand==KCmdIsOff )
		{
		DoCmdIsOff(aSection);
		}
	else if ( aCommand==KCmdMaxCurrentInMicroAmps )
		{
		DoCmdMaxCurrentInMicroAmps(aSection);
		}
	else if ( aCommand==KCmdResetInactivityTimer )
		{
		DoCmdResetInactivityTimer();
		}
	else if ( aCommand==KCmdVoltageSupported)
		{
		DoCmdVoltageSupported(aSection);
		}
	else if ( aCommand==KCmdInactivityCount)
		{
		DoCmdInactivityCount(aSection);
		}
	else if ( aCommand==KCmdNotLockedCount)
		{
		DoCmdNotLockedCount(aSection);
		}
	else if ( aCommand==KCmdInactivityTimeout)
		{
		DoCmdInactivityTimeout(aSection);
		}
	else if ( aCommand==KCmdNotLockedTimeout)
		{
		DoCmdNotLockedTimeout(aSection);
		}
	else if ( aCommand==KCmdMemVoltageSupported)
		{
		DoCmdMemVoltageSupported(aSection);
		}
	else if ( aCommand==KCmdMemMaxCurrentInMicroAmps)
		{
		DoCmdMemMaxCurrentInMicroAmps(aSection);
		}
	else if ( aCommand==KCmdVoltCheckInterval)
		{
		DoCmdVoltCheckInterval(aSection);
		}
	else
		{
		ret=CT_MmcSDDriverData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}
	return ret;
	}

/**
 * Checks whether this PSU is powering a bus containing a locked device
 * i.e. one that is recognised and in use by a client
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdIsLocked(const TDesC& aSection)
	{
 	TInt err;
 	TBool lockstate;
 	err = iMmcSDController->IsLocked(lockstate);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("Getting Lock Information Error %d"), err);
		SetError(err);
		}
	else
		{
		//IsLocked?
		INFO_PRINTF2(_L("PSUBASE: IsLocked    = %d \n"),lockstate);
		TBool	expectedlockstate;
		if(GetBoolFromConfig(aSection, KExpectedLockState(), expectedlockstate))
			{
			if(lockstate != expectedlockstate)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("lockstate(%d) != expectedlockstate(%d)"), lockstate, expectedlockstate);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedlockstate Value Missing From INI File"));
			}
		}
	}

/**
 * Checks whether the PSU is off
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdIsOff(const TDesC& aSection)
	{
 	TInt err;
 	TBool psustate;
 	err = iMmcSDController->IsOff(psustate);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("Getting PSU State Information Error %d"), err);
		SetError(err);
		}
	else
		{
		//IsLocked?
		INFO_PRINTF2(_L("PSUBASE: IsOff    = %d \n"),psustate);
		TBool	expectedpsustate;
		if(GetBoolFromConfig(aSection, KExpectedPsuState(), expectedpsustate))
			{
			if(psustate != expectedpsustate)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("psustate(%d) != expectedpsustate(%d)"), psustate, expectedpsustate);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedpsustate Value Missing From INI File"));
			}
		}
	}

/**
 * Gets the maximum current (in microAmps) that the PSU is able to supply
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdMaxCurrentInMicroAmps(const TDesC& aSection)
	{
 	TInt err;
 	TInt current;
 	err = iMmcSDController->MaxCurrentInMicroAmps(current);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("Getting MaxCurrentInMicroAmps Error %d"), err);
		SetError(err);
		}
	else
		{
		//MaxCurrentInMicroAmps
		INFO_PRINTF2(_L("PSUBASE: MaxCurrentInMicroAmps   = %d \n"),current);
		TInt	expectedcurrent;
		if(GetIntFromConfig(aSection, KExpectedCurrent(), expectedcurrent))
			{
			if(current != expectedcurrent)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("current(%d) != expectedcurrent(%d)"), current, expectedcurrent);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedcurrent Value Missing From INI File"));
			}
		}
	}

/**
 * Resets inactivity and not-locked counts
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdResetInactivityTimer()
	{
 	TInt err;
 	err = iMmcSDController->ResetInactivityTimer();
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("ResetInactivityTimer Error %d"), err);
		SetError(err);
		}
	}

/**
 * Gets the voltage level, or range of supported voltage levels
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdVoltageSupported(const TDesC& aSection)
	{
 	TInt err;
 	TUint volt;
 	err = iMmcSDController->VoltageSupported(volt);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("VoltageSupported Error %d"), err);
		SetError(err);
		}
	else
		{
		//VoltageSupported
		INFO_PRINTF2(_L("PSUBASE: VoltageSupported  = %x \n"),volt);
		TUint	expectedvolt;
		if(GetUintFromConfig(aSection, KExpectedVoltage(), expectedvolt))
			{
			if(volt != expectedvolt)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("volt(%x) != expectedvolt(%x)"), volt, expectedvolt);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedvolt Value Missing From INI File"));
			}
		}
	}

/**
 * Bus inactivity counter
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdInactivityCount(const TDesC& aSection)
	{
 	TInt err;
 	TInt inactivityCount;
 	err = iMmcSDController->InactivityCount(inactivityCount);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("InactivityCount Error %d"), err);
		SetError(err);
		}
	else
		{
		//InactivityCount
		INFO_PRINTF2(_L("PSUBASE: InactivityCount  = %d \n"),inactivityCount);
		TInt	expectedinactivityCount;
		if(GetIntFromConfig(aSection, KExpectedInactivityCount(), expectedinactivityCount))
			{
			if(inactivityCount != expectedinactivityCount)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("inactivityCount(%d) != expectedinactivityCount(%d)"), inactivityCount, expectedinactivityCount);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedinactivityCount Value Missing From INI File"));
			}
		}
	}

/**
 * Bus not locked counter
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdNotLockedCount(const TDesC& aSection)
	{
 	TInt err;
 	TInt notLockedCount;
 	err = iMmcSDController->NotLockedCount(notLockedCount);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("NotLockedCount Error %d"), err);
		SetError(err);
		}
	else
		{
		//NotLockedCount
		INFO_PRINTF2(_L("PSUBASE: NotLockedCount  = %d \n"),notLockedCount);
		TInt	expectednotLockedCount;
		if(GetIntFromConfig(aSection, KExpectedNotLockedCount(), expectednotLockedCount))
			{
			if(notLockedCount != expectednotLockedCount)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("notLockedCount(%d) != expectednotLockedCount(%d)"), notLockedCount, expectednotLockedCount);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectednotLockedCount Value Missing From INI File"));
			}
		}
	}

/**
 * Bus inactivity timeout period, in seconds, when clients are registered
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdInactivityTimeout(const TDesC& aSection)
	{
 	TInt err;
 	TInt inactivityTimeout;
 	err = iMmcSDController->InactivityTimeout(inactivityTimeout);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("InactivityTimeout Error %d"), err);
		SetError(err);
		}
	else
		{
		//InactivityTimeout
		INFO_PRINTF2(_L("PSUBASE: InactivityTimeout  = %d \n"),inactivityTimeout);
		TInt	expectedinactivityTimeout;
		if(GetIntFromConfig(aSection, KExpectedInactivityTimeout(), expectedinactivityTimeout))
			{
			if(inactivityTimeout != expectedinactivityTimeout)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("inactivityTimeout(%d) != expectedinactivityTimeout(%d)"), inactivityTimeout, expectedinactivityTimeout);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedinactivityTimeout Value Missing From INI File"));
			}
		}
	}

/**
 * Bus not locked timeout period, in seconds, when no clients are registered
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdNotLockedTimeout(const TDesC& aSection)
	{
 	TInt err;
 	TInt notLockedTimeout;
 	err = iMmcSDController->NotLockedTimeout(notLockedTimeout);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("NotLockedTimeout Error %d"), err);
		SetError(err);
		}
	else
		{
		//NotLockedTimeout
		INFO_PRINTF2(_L("PSUBASE: NotLockedTimeout  = %d \n"),notLockedTimeout);
		TInt	expectednotLockedTimeout;
		if(GetIntFromConfig(aSection, KExpectedNotLockedTimeout(), expectednotLockedTimeout))
			{
			if(notLockedTimeout != expectednotLockedTimeout)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("notLockedTimeout(%d) != expectednotLockedTimeout(%d)"), notLockedTimeout, expectednotLockedTimeout);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectednotLockedTimeout Value Missing From INI File"));
			}
		}
	}

/**
 * Indicates the voltage level, or range of voltages supported
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdMemVoltageSupported(const TDesC& aSection)
	{
 	TInt err;
 	TUint memVoltageSupported;
 	err = iMmcSDController->MemVoltageSupported(memVoltageSupported);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("MemVoltageSupported Error %u"), err);
		SetError(err);
		}
	else
		{
		//MemVoltageSupported
		INFO_PRINTF2(_L("PSUBASE: MemVoltageSupported  = %x \n"),memVoltageSupported);
		TUint	expectedmemVoltageSupported;
		if(GetUintFromConfig(aSection, KExpectedMemVoltageSupported(), expectedmemVoltageSupported))
			{
			if(memVoltageSupported != expectedmemVoltageSupported)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("memVoltageSupported(%x) != expectedmemVoltageSupported(%x)"), memVoltageSupported, expectedmemVoltageSupported);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedmemVoltageSupported Value Missing From INI File"));
			}
		}
	}

/**
 * The maximum current (in microAmps) that the PSU is able to supply
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdMemMaxCurrentInMicroAmps(const TDesC& aSection)
	{
 	TInt err;
 	TInt memMaxCurrentInMicroAmps;
 	err = iMmcSDController->MemMaxCurrentInMicroAmps(memMaxCurrentInMicroAmps);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("MemMaxCurrentInMicroAmps Error %d"), err);
		SetError(err);
		}
	else
		{
		//MemMaxCurrentInMicroAmps
		INFO_PRINTF2(_L("PSUBASE: MemMaxCurrentInMicroAmps  = %d \n"),memMaxCurrentInMicroAmps);
		TInt	expectedmemMaxCurrentInMicroAmps;
		if(GetIntFromConfig(aSection, KExpectedMemMaxCurrentInMicroAmps(), expectedmemMaxCurrentInMicroAmps))
			{
			if(memMaxCurrentInMicroAmps != expectedmemMaxCurrentInMicroAmps)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("memMaxCurrentInMicroAmps(%d) != expectedmemMaxCurrentInMicroAmps(%d)"), memMaxCurrentInMicroAmps, expectedmemMaxCurrentInMicroAmps);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedmemMaxCurrentInMicroAmps Value Missing From INI File"));
			}
		}
	}

/**
 * Indicates whether the platform hardware has support for checking whether the voltage level of the
 * PSU is within its expected voltage limit while turned on
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuBaseDriverData::DoCmdVoltCheckInterval(const TDesC& aSection)
	{
 	TInt err;
 	TUint voltCheckInterval;
 	err = iMmcSDController->VoltCheckInterval(voltCheckInterval);
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("VoltCheckInterval Error %d"), err);
		SetError(err);
		}
	else
		{
		//VoltCheckInterval
		INFO_PRINTF2(_L("PSUBASE: VoltCheckInterval  = %u \n"),voltCheckInterval);
		TUint	expectedvoltCheckInterval;
		if(GetUintFromConfig(aSection, KExpectedVoltageinterval(), expectedvoltCheckInterval))
			{
			if(voltCheckInterval != expectedvoltCheckInterval)
				{
				SetBlockResult(EFail);
				ERR_PRINTF3(_L("voltCheckInterval(%u) != expectedvoltCheckInterval(%u)"), voltCheckInterval, expectedvoltCheckInterval);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedvoltCheckInterval Value Missing From INI File"));
			}
		}
	}
