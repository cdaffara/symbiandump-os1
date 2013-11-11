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

#include "T_MmcSDPsuDriverData.h"

//MMCSD Commands index
/*@{*/

_LIT(KEnumNull,							" ");

_LIT(KCmdPsuInfo,						"PsuInfo");
_LIT(KCmdPbusState,						"PbusState");
_LIT(KCmdSetPsuState,					"SetState");
_LIT(KCmdVccState,						"VccState");
_LIT(KCmdPsuVoltage,					"SetVoltage");
_LIT(KCmdPsuNumber,						"PsuNumber");
_LIT(KCmdMediaChangeNumber,				"MediaChangeNumber");
_LIT(KCmdPsuCurLimit,					"PsuCurLimit");

_LIT(KVoltage,							"expectedvoltage");
_LIT(KCurrent,							"expectedcurrent");
_LIT(KVoltageinterval,					"expectedvoltageinterval");
_LIT(KVoltagecheck,						"expectedvoltagecheck");
_LIT(KInactivityTimeOut,				"expectedpsuinactivitytimeOut");
_LIT(KNotlockedtimeOut,					"expectednotlockedtimeOut");
_LIT(KPbusState,						"expectedpbusstate");
_LIT(KVccState,							"expectedvccstate");
_LIT(KExpectedpsuNumber,				"expectedpsuNumber");
_LIT(KExpectedmediaChangeNumber,		"expectedmediaChangeNumber");
_LIT(KExpectedpsuCurLimit,				"expectedpsuCurLimit");

_LIT(KPsuState,							"psustate");
_LIT(KPsuVoltage,						"psuVoltage");
/*@}*/

_LIT(KPsuChkComparator,					"EDPsuChkComparator");
_LIT(KPsuChkADCType,					"EDPsuChkADCType");
_LIT(KPsuChkADCType2,					"EDPsuChkADCType2");

_LIT(KBusCardAbsent,					"EBusCardAbsent");
_LIT(KBusOff,							"EBusOff");
_LIT(KBusPoweringUp,					"EBusPoweringUp");
_LIT(KBusOn,							"EBusOn");
_LIT(KEBusPsuFault,						"EBusPsuFault");
_LIT(KBusPowerUpPending,				"EBusPowerUpPending");

_LIT(KPsuOff,							"EDPsuOff");
_LIT(KPsuOnCurLimit,					"EDPsuOnCurLimit");
_LIT(KPsuOnFull,						"EDPsuOnFull");

const CDataWrapperBase::TEnumEntryTable	CT_MMCSDPsuDriverData::iEnumTableVoltageCheckMethod[] =
	{
//	Enum as a descriptor					Enum
	KPsuChkComparator,						EDPsuChkComparator,
	KPsuChkADCType,							EDPsuChkADCType,
	KPsuChkADCType2,						EDPsuChkADCType2,
	KEnumNull,								-1
	};

const CDataWrapperBase::TEnumEntryTable	CT_MMCSDPsuDriverData::iEnumTablePbusState[] =
	{
//	Enum as a descriptor					Enum
	KBusCardAbsent,							EBusCardAbsent,
	KBusOff,								EBusOff,
	KBusPoweringUp,							EBusPoweringUp,
	KBusOn,									EBusOn,
	KEBusPsuFault,							EBusPsuFault,
	KBusPowerUpPending,						EBusPowerUpPending,
	KEnumNull,								-1
	};

const CDataWrapperBase::TEnumEntryTable	CT_MMCSDPsuDriverData::iEnumTableVccState[] =
	{
//	Enum as a descriptor					Enum
	KPsuOff,								EDPsuOff,
	KPsuOnCurLimit,							EDPsuOnCurLimit,
	KPsuOnFull,								EDPsuOnFull,
	KEnumNull,								-1
	};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMCSDPsuDriverData* CT_MMCSDPsuDriverData::NewL()
	{
	CT_MMCSDPsuDriverData*	ret=new (ELeave) CT_MMCSDPsuDriverData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_MMCSDPsuDriverData::CT_MMCSDPsuDriverData()
:	CT_MmcSDDriverData()
,	iAsyncErrorIndex(0)
	{
	}

void CT_MMCSDPsuDriverData::ConstructL()
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

CT_MMCSDPsuDriverData::~CT_MMCSDPsuDriverData()
/**
 * Public destructor
 */
	{
	}

TBool CT_MMCSDPsuDriverData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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

	if ( aCommand==KCmdPsuInfo )
		{
		DoCmdPsuInfo(aSection);
		}
	else if ( aCommand==KCmdPsuVoltage )
		{
		DoCmdSetPsuVoltage(aSection);
		}
	else if ( aCommand==KCmdPsuNumber )
		{
		DoCmdPsuNumber(aSection);
		}
	else if ( aCommand==KCmdMediaChangeNumber )
		{
		DoCmdMediaChangeID(aSection);
		}
	else if ( aCommand==KCmdPsuCurLimit )
		{
		DoCmdPsuCurLimit(aSection);
		}
	else if ( aCommand==KCmdPbusState)
		{
		DoCmdPbusState(aSection);
		}
	else if ( aCommand==KCmdSetPsuState)
		{
		DoCmdSetPsuState(aSection);
		}
	else if ( aCommand==KCmdVccState)
		{
		DoCmdVccState(aSection);
		}
	else
		{
		ret=CT_MmcSDDriverData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}
	return ret;
	}

/**
 * Psu information
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuDriverData::DoCmdPsuInfo(const TDesC& aSection)
	{
 	TInt err;
 	TPBusPsuInf psuInfo;
	err = iMmcSDController->PsuInfo(psuInfo);
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("Getting PsuInfo Error %d"), err);
			SetError(err);
			}
		else
			{
			//Inactivity Timeout
			INFO_PRINTF2(_L("PSU: Bus Inactivity timeout Period    = %d \n"),psuInfo.iInactivityTimeOut);
			TInt	expectedInactivityTimeOut;
			if(GetIntFromConfig(aSection, KInactivityTimeOut(), expectedInactivityTimeOut))
				{
				if(psuInfo.iInactivityTimeOut != expectedInactivityTimeOut)
					{
					SetBlockResult(EFail);
					ERR_PRINTF3(_L("psuInfo.iInactivityTimeOut(%d) != expectedInactivityTimeOut(%d)"), psuInfo.iInactivityTimeOut, expectedInactivityTimeOut);
					}
				if(psuInfo.iInactivityTimeOut<=0 || psuInfo.iInactivityTimeOut>10)
					{
					ERR_PRINTF1(_L("MMC/SD: Error : Inactivity timeout is not programmed correctly. \n"));
					ERR_PRINTF1(_L("MMC/SD: Inactivity time out depends on platform. Possible value can be (1/3/5/10) seconds \n"));
					SetBlockResult(EFail);
					}
				}
			else
				{
				SetBlockResult(EFail);
				WARN_PRINTF1(_L("Inactivity timeout Value Missing From INI File"));
				}
			//NotLocked Timeout
			INFO_PRINTF2(_L("PSU: Bus Not Locked Timeout Period = %d \n"),psuInfo.iNotLockedTimeOut);
			TInt	expectedNotlockedtimeOut;
			if(GetIntFromConfig(aSection, KNotlockedtimeOut(), expectedNotlockedtimeOut))
				{
				if(psuInfo.iNotLockedTimeOut != expectedNotlockedtimeOut)
					{
					SetBlockResult(EFail);
					ERR_PRINTF3(_L("psuInfo.iNotLockedTimeOut(%d) != expectedNotlockedtimeOut(%d)"), psuInfo.iNotLockedTimeOut, expectedNotlockedtimeOut);
					}
				}
			else
				{
				SetBlockResult(EFail);
				WARN_PRINTF1(_L("NotlockedtimeOut Value Missing From INI File"));
				}
			//MaxCurrentInMicroAmps
			INFO_PRINTF2(_L("PSU: MaxCurrentInMicroAmps     = %d \n"),psuInfo.iMaxCurrentInMicroAmps);
			TInt	expectedMaxCurrentInMicroAmps;
			if(GetIntFromConfig(aSection, KCurrent(), expectedMaxCurrentInMicroAmps))
				{
				if(psuInfo.iMaxCurrentInMicroAmps != expectedMaxCurrentInMicroAmps)
					{
					SetBlockResult(EFail);
					ERR_PRINTF3(_L("psuInfo.iMaxCurrentInMicroAmps(%d) != expectedMaxCurrentInMicroAmps(%d)"), psuInfo.iMaxCurrentInMicroAmps, expectedMaxCurrentInMicroAmps);
					}
			 	}
			else
				{
				SetBlockResult(EFail);
				WARN_PRINTF1(_L("MaxCurrentInMicroAmps Value Missing From INI File"));
				}
			//Volatge Supported
			INFO_PRINTF2(_L("PSU: Volatge Supported(OCR Register Format)     = 0x%08x(OCR Format) \n"),psuInfo.iVoltageSupported);
			TUint	expectedVoltageSupported;
			if(GetUintFromConfig(aSection, KVoltage(), expectedVoltageSupported))
				{
				 if(psuInfo.iVoltageSupported != expectedVoltageSupported)
					{
					SetBlockResult(EFail);
					ERR_PRINTF3(_L("psuInfo.iVoltageSupported(0x%08x) != expectedVoltageSupported(0x%08x)"), psuInfo.iVoltageSupported, expectedVoltageSupported);
					}
				//Bit MASK (Refer OCR register format)
				if(psuInfo.iVoltageSupported ==0)
					{
					ERR_PRINTF1(_L("MMC/SD: iVoltageSupported should not be zero \n"));
					ERR_PRINTF1(_L("MMC/SD: iVoltageSupported depends on platform. Possible values(3.0/3.3) volts in OCR register format\n"));
					SetBlockResult(EFail);
					}
			 	}
			else
				{
				SetBlockResult(EFail);
				WARN_PRINTF1(_L("VoltageSupported Value Missing From INI File"));
				}
			//Voltage Interval
			INFO_PRINTF2(_L("PSU: Voltage Interval      = %x \n"),psuInfo.iVoltCheckInterval);
			TUint	expectedVoltCheckInterval;
			if(GetUintFromConfig(aSection, KVoltageinterval(), expectedVoltCheckInterval))
				{
				 if(psuInfo.iVoltCheckInterval != expectedVoltCheckInterval)
					{
					SetBlockResult(EFail);
					ERR_PRINTF3(_L("psuInfo.iVoltCheckInterval(%d) != expectedVoltCheckInterval(%d)"), psuInfo.iVoltCheckInterval, expectedVoltCheckInterval);
					}
			 	}
			else
				{
				SetBlockResult(EFail);
				WARN_PRINTF1(_L("VoltCheckInterval Value Missing From INI File"));
				}
			//VoltCheckMethod
			switch(psuInfo.iVoltCheckMethod)
				{
				case EDPsuChkComparator:
					{
					INFO_PRINTF1(_L("PSU VoltCheck Method  is  Comparator"));
					break;
					}
				case EDPsuChkADCType:
					{
					INFO_PRINTF1(_L("PSU VoltCheck Method  is  ADCType"));
					break;
					}
				case EDPsuChkADCType2:
					{
					INFO_PRINTF1(_L("PSU VoltCheck Method  is  ADCType2"));
					break;
					}
			default:
					ERR_PRINTF2(_L("PSU VoltCheck Method  is  Not Supported=%d"),psuInfo.iVoltCheckMethod);
				}
			TInt	expectedVoltCheckMethod;
			if ( GetEnumFromConfig(aSection, KVoltagecheck(), iEnumTableVoltageCheckMethod, expectedVoltCheckMethod) )
				{
				 if(psuInfo.iVoltCheckMethod != (TUint)expectedVoltCheckMethod)
					{
					SetBlockResult(EFail);
					ERR_PRINTF3(_L("psuInfo.iVoltCheckMethod(%d) != expectedVoltCheckMethod(%d)"), psuInfo.iVoltCheckMethod, expectedVoltCheckMethod);
					}
				}
			else
				{
				SetBlockResult(EFail);
				WARN_PRINTF1(_L("expectedVoltCheckMethod Value Missing From INI File"));
				}

			}
	}

/**
 * TPBusState Information
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuDriverData::DoCmdPbusState(const TDesC& aSection)
	{
	TInt err;
	TInt pbusState;
	err= iMmcSDController->PbusState(pbusState);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("MMC/SD : Error in getting PbusState information, error code = %d \n"),err);
			SetError(err);
			}
		else
			{
			switch(pbusState)
				{
				case EBusCardAbsent:
					{
					WARN_PRINTF1(_L("Card is absent in the slot"));
					break;
					}
				case EBusOff:
					{
					INFO_PRINTF1(_L("Bus State is OFF"));
					break;
					}
				case EBusPoweringUp:
					{
					INFO_PRINTF1(_L("Bus State is Powerwing Up"));
					break;
					}
				case EBusOn:
					{
					INFO_PRINTF1(_L("Bus State is Power ON"));
					break;
					}
				case EBusPsuFault:
					{
					WARN_PRINTF1(_L("Bus State is BusPsuFault"));
					break;
					}
				case EBusPowerUpPending:
					{
					INFO_PRINTF1(_L("Bus State is BusPowerUpPending"));
					break;
					}
			default:
					ERR_PRINTF2(_L("Bus State Not Supported = %d"),pbusState);
				}
			TInt	expectedPbusState;
			if ( GetEnumFromConfig(aSection, KPbusState(), iEnumTablePbusState, expectedPbusState) )
				{
				if(pbusState != expectedPbusState)
					{
					ERR_PRINTF3(_L("pbusState(%d) != expectedPbusState(%d)"), pbusState, expectedPbusState);
					SetBlockResult(EFail);
					}
				}
			else
				{
				SetBlockResult(EFail);
				WARN_PRINTF1(_L("expectedPbusState Value Missing From INI File"));
				}
			}

	}

/**
 * Set Power Supply State
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuDriverData::DoCmdSetPsuState(const TDesC& aSection)
	{
	TInt err;
	TInt psuState;
	GetIntFromConfig(aSection, KPsuState(), psuState);
	err=iMmcSDController->SetPsuState(psuState);
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCSD : Error in Setting PowerSupply State, error code = %d \n"),err);
		SetError(err);
		}
	}

/**
 * PSU VCC State Information
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuDriverData::DoCmdVccState(const TDesC& aSection)
	{
	TInt err;
	TInt vccState;
	err= iMmcSDController->VccState(vccState);
	if(err != KErrNone)
			{
			ERR_PRINTF2(_L("MMC/SD : Error in getting VccState information, error code = %d \n"),err);
			SetBlockResult(EFail);
			}
		else
			{
			switch(vccState)
				{
				case EDPsuOff:
					{
					INFO_PRINTF1(_L("PSU VCC State  is PsuOff "));
					break;
					}
				case EDPsuOnCurLimit:
					{
					INFO_PRINTF1(_L("PSU VCC State  is PsuOnCurLimit "));
					break;
					}
				case EDPsuOnFull:
					{
					INFO_PRINTF1(_L("PSU VCC State  is PsuOnFull "));
					break;
					}
			default:
					ERR_PRINTF2(_L("PSU VCC State Not Supported = %d"),vccState);
				}
			TInt	expectedvccState;
			if ( GetEnumFromConfig(aSection, KVccState(), iEnumTableVccState, expectedvccState) )
				{
				if(vccState != expectedvccState)
					{
					ERR_PRINTF3(_L("vccState(%d) != expectedvccState(%d)"), vccState, expectedvccState);
					SetBlockResult(EFail);
					}
				}
			else
				{
				SetBlockResult(EFail);
				WARN_PRINTF1(_L("expectedvccState Value Missing From INI File"));
				}
			}
	}
/**
 *Set Psu Voltage in OCR Format
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuDriverData::DoCmdSetPsuVoltage(const TDesC& aSection)
	{
	TInt err;
	TUint psuVoltage; //OCR Register Format
	GetUintFromConfig(aSection, KPsuVoltage(), psuVoltage);
	err=iMmcSDController->SetPsuVoltage(psuVoltage);
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCSD : Error in Setting Psu Voltage, error code = %d \n"),err);
		SetError(err);
		}
	}

/**
 *Reads the MMCSD power supply unit number
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuDriverData::DoCmdPsuNumber(const TDesC& aSection)
	{
	TInt err;
	TInt psuNumber;
	err=iMmcSDController->PsuNumber(psuNumber);
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCSD : Error in reading PSU Number from the platform, error code = %d \n"),err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("MMCSD Psu Number on platform   = %d \n"),psuNumber);
		TInt	expectedpsuNumber;
		if(GetIntFromConfig(aSection, KExpectedpsuNumber(), expectedpsuNumber))
			{
			if(psuNumber != expectedpsuNumber)
				{
				ERR_PRINTF3(_L("psuNumber(%d) != expectedpsuNumber(%d)"), psuNumber, expectedpsuNumber);
				SetBlockResult(EFail);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedpsuNumber Value Missing From INI File"));
			}
		}
	}

/**
 *Reads the MMCSD Media Change ID
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuDriverData::DoCmdMediaChangeID(const TDesC& aSection)
	{
	TInt err;
	TInt mediaChangeNumber;
	err=iMmcSDController->MediaChangeID(mediaChangeNumber);
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCSD : Error in reading MeadiaChangeID from the platform, error code = %d \n"),err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("MMCSD MeadiaChangeID on platform   = %d \n"),mediaChangeNumber);
		TInt	expectedmediaChangeNumber;
		if(GetIntFromConfig(aSection, KExpectedmediaChangeNumber(), expectedmediaChangeNumber))
			{
			if(mediaChangeNumber != expectedmediaChangeNumber)
				{
				ERR_PRINTF3(_L("mediaChangeNumber(%d) != expectedmediaChangeNumber(%d)"), mediaChangeNumber, expectedmediaChangeNumber);
				SetBlockResult(EFail);
				};
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedmediaChangeNumber Value Missing From INI File"));
			}
		}

	}

/**
 *Reads PSU current is limited to a safe level or Not
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDPsuDriverData::DoCmdPsuCurLimit(const TDesC& aSection)
	{
	TInt err;
	TBool psuCurLimit;
	err=iMmcSDController->PsuCurLimit(psuCurLimit);
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCSD : Error in reading PsuCurLimit from the platform, error code = %d \n"),err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("MMCSD PsuCurLimit on platform   = %d \n"),psuCurLimit);
		TInt	expectedpsuCurLimit;
		if(GetIntFromConfig(aSection, KExpectedpsuCurLimit(), expectedpsuCurLimit))
			{
			if(psuCurLimit != expectedpsuCurLimit)
				{
				ERR_PRINTF3(_L("psuCurLimit(%d) != expectedpsuCurLimit(%d)"), psuCurLimit, expectedpsuCurLimit);
				SetBlockResult(EFail);
				}
			}
		else
			{
			SetBlockResult(EFail);
			WARN_PRINTF1(_L("expectedpsuCurLimit Value Missing From INI File"));
			}
		}
	}
