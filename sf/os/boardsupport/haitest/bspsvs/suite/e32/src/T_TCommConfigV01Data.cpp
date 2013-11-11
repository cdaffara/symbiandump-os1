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

#include "T_TCommConfigV01Data.h"

/*@{*/
_LIT(KStrNull,								" ");

_LIT(KCmdConstructor,						"new");
_LIT(KCmdDestructor,						"~");
_LIT(KCmdiDataBits,							"iDataBits");
_LIT(KCmdiFifo,								"iFifo");
_LIT(KCmdiHandshake,						"iHandshake");
_LIT(KCmdiParity,							"iParity");
_LIT(KCmdiParityError,						"iParityError");
_LIT(KCmdiParityErrorChar,					"iParityErrorChar");
_LIT(KCmdiRate,								"iRate");
_LIT(KCmdiSIREnable,						"iSIREnable");
_LIT(KCmdiSIRSettings,						"iSIRSettings");
_LIT(KCmdiSpecialRate,						"iSpecialRate");
_LIT(KCmdiStopBits,							"iStopBits");
_LIT(KCmdiTerminator,						"iTerminator");
_LIT(KCmdiTerminatorCount,					"iTerminatorCount");
_LIT(KCmdiXoffChar,							"iXoffChar");
_LIT(KCmdiXonChar,							"iXonChar");

_LIT(KFldExpected,							"expected");
_LIT(KFldExpectedIndex,						"expected%d");
_LIT(KFldValue,								"value");
_LIT(KFldValueIndex,						"value%d");

_LIT(KLogError,								"Error=%d");
_LIT(KLogErrorExpectedValue,				"Expected Value does not match actual");
/*@}*/

_LIT(KStrEData5,							"EData5");
_LIT(KStrEData6,							"EData6");
_LIT(KStrEData7,							"EData7");
_LIT(KStrEData8,							"EData8");
const CDataWrapperBase::TEnumEntryTable	CT_TCommConfigV01Data::iEnumTableDataBits[] =
	{
//	Enum as a descriptor					Enum
	KStrEData5,								EData5,
	KStrEData6,								EData6,
	KStrEData7,								EData7,
	KStrEData8,								EData8,
	KStrNull,								-1
	};

_LIT(KStrEFifoEnable,						"EFifoEnable");
_LIT(KStrEFifoDisable,						"KStrEFifoDisable");
const CDataWrapperBase::TEnumEntryTable	CT_TCommConfigV01Data::iEnumTableFifo[] =
	{
//	Enum as a descriptor					Enum
	KStrEFifoEnable,						EFifoEnable,
	KStrEFifoDisable,						EFifoDisable,
	KStrNull,								-1
	};

_LIT(KStrKConfigObeyXoff,					"KConfigObeyXoff");
_LIT(KStrKConfigSendXoff,					"KConfigSendXoff");
_LIT(KStrKConfigObeyCTS,					"KConfigObeyCTS");
_LIT(KStrKConfigFailCTS,					"KConfigFailCTS");
_LIT(KStrKConfigObeyDSR,					"KConfigObeyDSR");
_LIT(KStrKConfigFailDSR,					"KConfigFailDSR");
_LIT(KStrKConfigObeyDCD,					"KConfigObeyDCD");
_LIT(KStrKConfigFailDCD,					"KConfigFailDCD");
_LIT(KStrKConfigFreeRTS,					"KConfigFreeRTS");
_LIT(KStrKConfigFreeDTR,					"KConfigFreeDTR");
_LIT(KStrKConfigObeyDTR,					"KConfigObeyDTR");
_LIT(KStrKConfigFailDTR,					"KConfigFailDTR");
_LIT(KStrKConfigObeyRTS,					"KConfigObeyRTS");
_LIT(KStrKConfigFailRTS,					"KConfigFailRTS");
_LIT(KStrKConfigFreeDSR,					"KConfigFreeDSR");
_LIT(KStrKConfigFreeCTS,					"KConfigFreeCTS");
_LIT(KStrKConfigFreeDCD,					"KConfigFreeDCD");
_LIT(KStrKConfigFreeRI,						"KConfigFreeRI");
const CDataWrapperBase::TEnumEntryTable	CT_TCommConfigV01Data::iEnumTableHandshake[] =
	{
//	Enum as a descriptor					Enum
	KStrKConfigObeyXoff,					KConfigObeyXoff,
	KStrKConfigSendXoff,					KConfigSendXoff,
	KStrKConfigObeyCTS,						KConfigObeyCTS,
	KStrKConfigFailCTS,						KConfigFailCTS,
	KStrKConfigObeyDSR,						KConfigObeyDSR,
	KStrKConfigFailDSR,						KConfigFailDSR,
	KStrKConfigObeyDCD,						KConfigObeyDCD,
	KStrKConfigFailDCD,						KConfigFailDCD,
	KStrKConfigFreeRTS,						KConfigFreeRTS,
	KStrKConfigFreeDTR,						KConfigFreeDTR,
	KStrKConfigObeyDTR,						KConfigObeyDTR,
	KStrKConfigFailDTR,						KConfigFailDTR,
	KStrKConfigObeyRTS,						KConfigObeyRTS,
	KStrKConfigFailRTS,						KConfigFailRTS,
	KStrKConfigFreeDSR,						KConfigFreeDSR,
	KStrKConfigFreeCTS,						KConfigFreeCTS,
	KStrKConfigFreeDCD,						KConfigFreeDCD,
	KStrKConfigFreeRI,						KConfigFreeRI,
	KStrNull,								-1
	};

_LIT(KStrEParityNone,						"EParityNone");
_LIT(KStrEParityEven,						"EParityEven");
_LIT(KStrEParityOdd,						"EParityOdd");
_LIT(KStrEParityMark,						"EParityMark");
_LIT(KStrEParitySpace,						"EParitySpace");
const CDataWrapperBase::TEnumEntryTable	CT_TCommConfigV01Data::iEnumTableParity[] =
	{
//	Enum as a descriptor					Enum
	KStrEParityNone,						EParityNone,
	KStrEParityEven,						EParityEven,
	KStrEParityOdd,							EParityOdd,
	KStrEParityMark,						EParityMark,
	KStrEParitySpace,						EParitySpace,
	KStrNull,								-1
	};

_LIT(KStrKConfigParityErrorFail,			"KConfigParityErrorFail");
_LIT(KStrKConfigParityErrorIgnore,			"KConfigParityErrorIgnore");
_LIT(KStrKConfigParityErrorReplaceChar,		"KConfigParityErrorReplaceChar");
const CDataWrapperBase::TEnumEntryTable	CT_TCommConfigV01Data::iEnumTableParityError[] =
	{
//	Enum as a descriptor					Enum
	KStrKConfigParityErrorFail,				KConfigParityErrorFail,
	KStrKConfigParityErrorIgnore,			KConfigParityErrorIgnore,
	KStrKConfigParityErrorReplaceChar,		KConfigParityErrorReplaceChar,
	KStrNull,								-1
	};

_LIT(KStrEBps50,							"EBps50");
_LIT(KStrEBps75,							"EBps75");
_LIT(KStrEBps110,							"EBps110");
_LIT(KStrEBps134,							"EBps134");
_LIT(KStrEBps150,							"EBps150");
_LIT(KStrEBps300,							"EBps300");
_LIT(KStrEBps600,							"EBps600");
_LIT(KStrEBps1200,							"EBps1200");
_LIT(KStrEBps1800,							"EBps1800");
_LIT(KStrEBps2000,							"EBps2000");
_LIT(KStrEBps2400,							"EBps2400");
_LIT(KStrEBps3600,							"EBps3600");
_LIT(KStrEBps4800,							"EBps4800");
_LIT(KStrEBps7200,							"EBps7200");
_LIT(KStrEBps9600,							"EBps9600");
_LIT(KStrEBps19200,							"EBps19200");
_LIT(KStrEBps38400,							"EBps38400");
_LIT(KStrEBps57600,							"EBps57600");
_LIT(KStrEBps115200,						"EBps115200");
_LIT(KStrEBps230400,						"EBps230400");
_LIT(KStrEBps460800,						"EBps460800");
_LIT(KStrEBps576000,						"EBps576000");
_LIT(KStrEBps1152000,						"EBps1152000");
_LIT(KStrEBps4000000,						"EBps4000000");
_LIT(KStrEBps921600,						"EBps921600");
_LIT(KStrEBpsAutobaud,						"EBpsAutobaud");
_LIT(KStrEBpsSpecial,						"EBpsSpecial");
const CDataWrapperBase::TEnumEntryTable	CT_TCommConfigV01Data::iEnumTableRate[] =
	{
//	Enum as a descriptor					Enum
	KStrEBps50,								EBps50,
	KStrEBps75,								EBps75,
	KStrEBps110,							EBps110,
	KStrEBps134,							EBps134,
	KStrEBps150,							EBps150,
	KStrEBps300,							EBps300,
	KStrEBps600,							EBps600,
	KStrEBps1200,							EBps1200,
	KStrEBps1800,							EBps1800,
	KStrEBps2000,							EBps2000,
	KStrEBps2400,							EBps2400,
	KStrEBps3600,							EBps3600,
	KStrEBps4800,							EBps4800,
	KStrEBps7200,							EBps7200,
	KStrEBps9600,							EBps9600,
	KStrEBps19200,							EBps19200,
	KStrEBps38400,							EBps38400,
	KStrEBps57600,							EBps57600,
	KStrEBps115200,							EBps115200,
	KStrEBps230400,							EBps230400,
	KStrEBps460800,							EBps460800,
	KStrEBps576000,							EBps576000,
	KStrEBps1152000,						EBps1152000,
	KStrEBps4000000,						EBps4000000,
	KStrEBps921600,							EBps921600,
	KStrEBpsAutobaud,						EBpsAutobaud,
	KStrEBpsSpecial,						EBpsSpecial,
	KStrNull,								-1
	};

_LIT(KStrESIREnable,						"ESIREnable");
_LIT(KStrESIRDisable,						"ESIRDisable");
const CDataWrapperBase::TEnumEntryTable	CT_TCommConfigV01Data::iEnumTableSir[] =
	{
//	Enum as a descriptor					Enum
	KStrESIREnable,							ESIREnable,
	KStrESIRDisable,						ESIRDisable,
	KStrNull,								-1
	};

_LIT(KStrKConfigSIRPulseWidthMaximum,		"KConfigSIRPulseWidthMaximum");
_LIT(KStrKConfigSIRPulseWidthMinimum,		"KConfigSIRPulseWidthMinimum");
_LIT(KStrKConfigSIRShutDown,				"KConfigSIRShutDown");
_LIT(KStrKConfigSIRMinimumRange,			"KConfigSIRMinimumRange");
_LIT(KStrKConfigSIRMediumRange,				"KConfigSIRMediumRange");
_LIT(KStrKConfigSIRMaximumRange,			"KConfigSIRMaximumRange");
const CDataWrapperBase::TEnumEntryTable	CT_TCommConfigV01Data::iEnumTableSirSetting[] =
	{
//	Enum as a descriptor					Enum
	KStrKConfigSIRPulseWidthMaximum,		KConfigSIRPulseWidthMaximum,
	KStrKConfigSIRPulseWidthMinimum,		KConfigSIRPulseWidthMinimum,
	KStrKConfigSIRShutDown,					KConfigSIRShutDown,
	KStrKConfigSIRMinimumRange,				KConfigSIRMinimumRange,
	KStrKConfigSIRMediumRange,				KConfigSIRMediumRange,
	KStrKConfigSIRMaximumRange,				KConfigSIRMaximumRange,
	KStrNull,								-1
	};

_LIT(KStrEStop1,							"EStop1");
_LIT(KStrEStop2,							"EStop2");
const CDataWrapperBase::TEnumEntryTable	CT_TCommConfigV01Data::iEnumTableStopBits[] =
	{
//	Enum as a descriptor					Enum
	KStrEStop1,								EStop1,
	KStrEStop2,								EStop2,
	KStrNull,								-1
	};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_TCommConfigV01Data::CT_TCommConfigV01Data()
:	CDataWrapperBase()
,	iCommConfig(NULL)
	{
	}

CT_TCommConfigV01Data::~CT_TCommConfigV01Data()
/**
 * Public destructor
 */
	{
	DestroyData();
	}

void CT_TCommConfigV01Data::DestroyData()
	{
	delete iCommConfig;
	iCommConfig=NULL;
	}

TDes8* CT_TCommConfigV01Data::Descriptor()
/**
 * Return a pointer to the buffer
 *
 * @return	pointer to the buffer
 */
	{
	return iCommConfig;
	}

TAny* CT_TCommConfigV01Data::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iCommConfig;
	}

TBool CT_TCommConfigV01Data::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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
	TInt	err=KErrNone;

	if ( aCommand==KCmdConstructor )
		{
		err=DoCmdConstructor();
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdiDataBits )
		{
		DoCmdiDataBits(aSection);
		}
	else if ( aCommand==KCmdiFifo )
		{
		DoCmdiFifo(aSection);
		}
	else if ( aCommand==KCmdiHandshake )
		{
		DoCmdiHandshake(aSection);
		}
	else if ( aCommand==KCmdiParity )
		{
		DoCmdiParity(aSection);
		}
	else if ( aCommand==KCmdiParityError )
		{
		DoCmdiParityError(aSection);
		}
	else if ( aCommand==KCmdiParityErrorChar )
		{
		DoCmdiParityErrorChar(aSection);
		}
	else if ( aCommand==KCmdiRate )
		{
		DoCmdiRate(aSection);
		}
	else if ( aCommand==KCmdiSIREnable )
		{
		DoCmdiSIREnable(aSection);
		}
	else if ( aCommand==KCmdiSIRSettings )
		{
		DoCmdiSIRSettings(aSection);
		}
	else if ( aCommand==KCmdiSpecialRate )
		{
		DoCmdiSpecialRate(aSection);
		}
	else if ( aCommand==KCmdiStopBits )
		{
		DoCmdiStopBits(aSection);
		}
	else if ( aCommand==KCmdiTerminator )
		{
		DoCmdiTerminator(aSection);
		}
	else if ( aCommand==KCmdiTerminatorCount )
		{
		DoCmdiTerminatorCount(aSection);
		}
	else if ( aCommand==KCmdiXoffChar )
		{
		DoCmdiXoffChar(aSection);
		}
	else if ( aCommand==KCmdiXonChar )
		{
		DoCmdiXonChar(aSection);
		}
	else
		{
		ret=CDataWrapperBase::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}

	return ret;
	}

TCommConfigV01& CT_TCommConfigV01Data::GetCommConfigV01()
	{
	return (*iCommConfig)();
	}

TInt CT_TCommConfigV01Data::DoCmdConstructor()
	{
	DestroyData();
	TRAPD(err, iCommConfig=new (ELeave) TCommConfig());
	iCommConfig->FillZ();
	return err;
	}

void CT_TCommConfigV01Data::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_TCommConfigV01Data::DoCmdiDataBits(const TDesC& aSection)
	{
	TDataBits	actual=GetCommConfigV01().iDataBits;
	INFO_PRINTF2(_L("iDataBits : %d"), actual);

	TInt	expected;
	if ( GetEnumFromConfig(aSection, KFldExpected(), iEnumTableDataBits, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetEnumFromConfig(aSection, KFldValue(), iEnumTableDataBits, value) )
		{
		GetCommConfigV01().iDataBits=(TDataBits)value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiFifo(const TDesC& aSection)
	{
	TFifo	actual=(TFifo)GetCommConfigV01().iFifo;
	INFO_PRINTF2(_L("iFifo : %d"), actual);

	TInt	expected;
	if ( GetEnumFromConfig(aSection, KFldExpected(), iEnumTableFifo, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetEnumFromConfig(aSection, KFldValue(), iEnumTableFifo, value) )
		{
		GetCommConfigV01().iFifo=(TUint)value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiHandshake(const TDesC& aSection)
	{
	TUint	actual=GetCommConfigV01().iHandshake;
	INFO_PRINTF2(_L("iHandshake : %d"), actual);

	TUint	expected;
	if ( GetOrFromConfig(aSection, KFldExpected(), iEnumTableHandshake, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TUint	value;
	if ( GetOrFromConfig(aSection, KFldValue(), iEnumTableHandshake, value) )
		{
		GetCommConfigV01().iHandshake=value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiParity(const TDesC& aSection)
	{
	TParity	actual=GetCommConfigV01().iParity;
	INFO_PRINTF2(_L("iParity : %d"), actual);

	TInt	expected;
	if ( GetEnumFromConfig(aSection, KFldExpected(), iEnumTableParity, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetEnumFromConfig(aSection, KFldValue(), iEnumTableParity, value) )
		{
		GetCommConfigV01().iParity=(TParity)value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiParityError(const TDesC& aSection)
	{
	TUint	actual=GetCommConfigV01().iParityError;
	INFO_PRINTF2(_L("iParityError : %d"), actual);

	TInt	expected;
	if ( GetEnumFromConfig(aSection, KFldExpected(), iEnumTableParityError, expected) )
		{
		if ( expected!=(TInt)actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetEnumFromConfig(aSection, KFldValue(), iEnumTableParityError, value) )
		{
		GetCommConfigV01().iParityError=(TUint)value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiParityErrorChar(const TDesC& aSection)
	{
	TText8	actual=GetCommConfigV01().iParityErrorChar;
	INFO_PRINTF2(_L("iParityErrorChar : 0x%x"), (TUint)actual);

	TInt	expected;
	if ( GetHexFromConfig(aSection, KFldExpected(), expected) )
		{
		if ( expected!=(TInt)actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetHexFromConfig(aSection, KFldValue(), value) )
		{
		GetCommConfigV01().iParityErrorChar=(TText8)value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiRate(const TDesC& aSection)
	{
	TBps	actual=GetCommConfigV01().iRate;
	INFO_PRINTF2(_L("iRate : %d"), actual);

	TInt	expected;
	if ( GetEnumFromConfig(aSection, KFldExpected(), iEnumTableRate, expected) )
		{
		if ( expected!=(TInt)actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetEnumFromConfig(aSection, KFldValue(), iEnumTableRate, value) )
		{
		GetCommConfigV01().iRate=(TBps)value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiSIREnable(const TDesC& aSection)
	{
	TSir	actual=GetCommConfigV01().iSIREnable;
	INFO_PRINTF2(_L("iSIREnable : %d"), actual);

	TInt	expected;
	if ( GetEnumFromConfig(aSection, KFldExpected(), iEnumTableSir, expected) )
		{
		if ( expected!=(TInt)actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetEnumFromConfig(aSection, KFldValue(), iEnumTableSir, value) )
		{
		GetCommConfigV01().iSIREnable=(TSir)value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiSIRSettings(const TDesC& aSection)
	{
	TUint	actual=GetCommConfigV01().iSIRSettings;
	INFO_PRINTF2(_L("iSIRSettings : %d"), actual);

	TInt	expected;
	if ( GetEnumFromConfig(aSection, KFldExpected(), iEnumTableSirSetting, expected) )
		{
		if ( expected!=(TInt)actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetEnumFromConfig(aSection, KFldValue(), iEnumTableSirSetting, value) )
		{
		GetCommConfigV01().iSIRSettings=(TUint)value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiSpecialRate(const TDesC& aSection)
	{
	TInt	actual=GetCommConfigV01().iSpecialRate;
	INFO_PRINTF2(_L("iSpecialRate : %d"), actual);

	TInt	expected;
	if ( GetIntFromConfig(aSection, KFldExpected(), expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetIntFromConfig(aSection, KFldValue(), value) )
		{
		GetCommConfigV01().iSpecialRate=value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiStopBits(const TDesC& aSection)
	{
	TStopBits	actual=GetCommConfigV01().iStopBits;
	INFO_PRINTF2(_L("iStopBits : %d"), actual);

	TInt	expected;
	if ( GetEnumFromConfig(aSection, KFldExpected(), iEnumTableStopBits, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetEnumFromConfig(aSection, KFldValue(), iEnumTableStopBits, value) )
		{
		GetCommConfigV01().iStopBits=(TStopBits)value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiTerminator(const TDesC& aSection)
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;

	for ( TInt index=0; index<KConfigMaxTerminators; ++index )
		{
		TText8	actual=GetCommConfigV01().iTerminator[index];
		INFO_PRINTF3(_L("iTerminator[%d] : 0x%x"), index, (TUint)actual);

		tempStore.Format(KFldExpectedIndex(), index);
		TInt	expected;
		if ( GetHexFromConfig(aSection, tempStore, expected) )
			{
			if ( expected!=(TInt)actual )
				{
				ERR_PRINTF1(KLogErrorExpectedValue);
				SetBlockResult(EFail);
				}
			}

		tempStore.Format(KFldValueIndex(), index);
		TInt	value;
		if ( GetHexFromConfig(aSection, tempStore, value) )
			{
			GetCommConfigV01().iTerminator[index]=(TText8)value;
			}
		}
	}

void CT_TCommConfigV01Data::DoCmdiTerminatorCount(const TDesC& aSection)
	{
	TInt	actual=GetCommConfigV01().iTerminatorCount;
	INFO_PRINTF2(_L("iTerminatorCount : %d"), actual);

	TInt	expected;
	if ( GetIntFromConfig(aSection, KFldExpected(), expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetIntFromConfig(aSection, KFldValue(), value) )
		{
		GetCommConfigV01().iTerminatorCount=value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiXoffChar(const TDesC& aSection)
	{
	TText8	actual=GetCommConfigV01().iXoffChar;
	INFO_PRINTF2(_L("iXoffChar : 0x%x"), (TUint)actual);

	TInt	expected;
	if ( GetHexFromConfig(aSection, KFldExpected(), expected) )
		{
		if ( expected!=(TInt)actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetHexFromConfig(aSection, KFldValue(), value) )
		{
		GetCommConfigV01().iXoffChar=(TText8)value;
		}
	}

void CT_TCommConfigV01Data::DoCmdiXonChar(const TDesC& aSection)
	{
	TText8	actual=GetCommConfigV01().iXonChar;
	INFO_PRINTF2(_L("iXonChar : 0x%x"), (TUint)actual);

	TInt	expected;
	if ( GetHexFromConfig(aSection, KFldExpected(), expected) )
		{
		if ( expected!=(TInt)actual )
			{
			ERR_PRINTF1(KLogErrorExpectedValue);
			SetBlockResult(EFail);
			}
		}

	TInt	value;
	if ( GetHexFromConfig(aSection, KFldValue(), value) )
		{
		GetCommConfigV01().iXonChar=(TText8)value;
		}
	}
