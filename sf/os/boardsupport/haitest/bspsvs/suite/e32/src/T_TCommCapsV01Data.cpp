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

#include "T_TCommCapsV01Data.h"

/*@{*/
_LIT(KStrNull,								" ");

_LIT(KCmdConstructor,						"new");
_LIT(KCmdDestructor,						"~");
_LIT(KCmdiDataBits,							"iDataBits");
_LIT(KCmdiFifo,								"iFifo");
_LIT(KCmdiHandshake,						"iHandshake");
_LIT(KCmdiParity,							"iParity");
_LIT(KCmdiRate,								"iRate");
_LIT(KCmdiSIR,								"iSIR");
_LIT(KCmdiSignals,							"iSignals");
_LIT(KCmdiStopBits,							"iStopBits");

_LIT(KFldExpected,							"expected");

_LIT(KLogError,								"Error=%d");
/*@}*/

_LIT(KStrKCapsData5,						"KCapsData5");
_LIT(KStrKCapsData6,						"KCapsData6");
_LIT(KStrKCapsData7,						"KCapsData7");
_LIT(KStrKCapsData8,						"KCapsData8");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV01Data::iEnumTableDataBits[] =
	{
//	Enum as a descriptor					Enum
	KStrKCapsData5,							KCapsData5,
	KStrKCapsData6,							KCapsData6,
	KStrKCapsData7,							KCapsData7,
	KStrKCapsData8,							KCapsData8,
	KStrNull,								-1
	};

_LIT(KStrKCapsHasFifo,						"KCapsHasFifo");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV01Data::iEnumTableFifo[] =
	{
//	Enum as a descriptor					Enum
	KStrKCapsHasFifo,						KCapsHasFifo,
	KStrNull,								-1
	};

_LIT(KStrKCapsObeyXoffSupported,			"KCapsObeyXoffSupported");
_LIT(KStrKCapsSendXoffSupported,			"KCapsSendXoffSupported");
_LIT(KStrKCapsObeyCTSSupported,				"KCapsObeyCTSSupported");
_LIT(KStrKCapsFailCTSSupported,				"KCapsFailCTSSupported");
_LIT(KStrKCapsObeyDSRSupported,				"KCapsObeyDSRSupported");
_LIT(KStrKCapsFailDSRSupported,				"KCapsFailDSRSupported");
_LIT(KStrKCapsObeyDCDSupported,				"KCapsObeyDCDSupported");
_LIT(KStrKCapsFailDCDSupported,				"KCapsFailDCDSupported");
_LIT(KStrKCapsFreeRTSSupported,				"KCapsFreeRTSSupported");
_LIT(KStrKCapsFreeDTRSupported,				"KCapsFreeDTRSupported");
_LIT(KStrKCapsObeyDTRSupported,				"KCapsObeyDTRSupported");
_LIT(KStrKCapsObeyRTSSupported,				"KCapsObeyRTSSupported");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV01Data::iEnumTableHandshake[] =
	{
//	Enum as a descriptor					Enum
	KStrKCapsObeyXoffSupported,				KCapsObeyXoffSupported,
	KStrKCapsSendXoffSupported,				KCapsSendXoffSupported,
	KStrKCapsObeyCTSSupported,				KCapsObeyCTSSupported,
	KStrKCapsFailCTSSupported,				KCapsFailCTSSupported,
	KStrKCapsObeyDSRSupported,				KCapsObeyDSRSupported,
	KStrKCapsFailDSRSupported,				KCapsFailDSRSupported,
	KStrKCapsObeyDCDSupported,				KCapsObeyDCDSupported,
	KStrKCapsFailDCDSupported,				KCapsFailDCDSupported,
	KStrKCapsFreeRTSSupported,				KCapsFreeRTSSupported,
	KStrKCapsFreeDTRSupported,				KCapsFreeDTRSupported,
	KStrKCapsObeyDTRSupported,				KCapsObeyDTRSupported,
	KStrKCapsObeyRTSSupported,				KCapsObeyRTSSupported,
	KStrNull,								-1
	};

_LIT(KStrKCapsParityNone,					"KCapsParityNone");
_LIT(KStrKCapsParityEven,					"KCapsParityEven");
_LIT(KStrKCapsParityOdd,					"KCapsParityOdd");
_LIT(KStrKCapsParityMark,					"KCapsParityMark");
_LIT(KStrKCapsParitySpace,					"KCapsParitySpace");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV01Data::iEnumTableParity[] =
	{
//	Enum as a descriptor					Enum
	KStrKCapsParityNone,					KCapsParityNone,
	KStrKCapsParityEven,					KCapsParityEven,
	KStrKCapsParityOdd,						KCapsParityOdd,
	KStrKCapsParityMark,					KCapsParityMark,
	KStrKCapsParitySpace,					KCapsParitySpace,
	KStrNull,								-1
	};

_LIT(KStrKCapsBps50,						"KCapsBps50");
_LIT(KStrKCapsBps75,						"KCapsBps75");
_LIT(KStrKCapsBps110,						"KCapsBps110");
_LIT(KStrKCapsBps134,						"KCapsBps134");
_LIT(KStrKCapsBps150,						"KCapsBps150");
_LIT(KStrKCapsBps300,						"KCapsBps300");
_LIT(KStrKCapsBps600,						"KCapsBps600");
_LIT(KStrKCapsBps1200,						"KCapsBps1200");
_LIT(KStrKCapsBps1800,						"KCapsBps1800");
_LIT(KStrKCapsBps2000,						"KCapsBps2000");
_LIT(KStrKCapsBps2400,						"KCapsBps2400");
_LIT(KStrKCapsBps3600,						"KCapsBps3600");
_LIT(KStrKCapsBps4800,						"KCapsBps4800");
_LIT(KStrKCapsBps7200,						"KCapsBps7200");
_LIT(KStrKCapsBps9600,						"KCapsBps9600");
_LIT(KStrKCapsBps19200,						"KCapsBps19200");
_LIT(KStrKCapsBps38400,						"KCapsBps38400");
_LIT(KStrKCapsBps57600,						"KCapsBps57600");
_LIT(KStrKCapsBps115200,					"KCapsBps115200");
_LIT(KStrKCapsBps230400,					"KCapsBps230400");
_LIT(KStrKCapsBps460800,					"KCapsBps460800");
_LIT(KStrKCapsBps576000,					"KCapsBps576000");
_LIT(KStrKCapsBps1152000,					"KCapsBps1152000");
_LIT(KStrKCapsBps4000000,					"KCapsBps4000000");
_LIT(KStrKCapsBps921600,					"KCapsBps921600");
_LIT(KStrKCapsBpsAutobaud,					"KCapsBpsAutobaud");
_LIT(KStrKCapsBpsSpecial,					"KCapsBpsSpecial");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV01Data::iEnumTableRate[] =
	{
//	Enum as a descriptor					Enum
	KStrKCapsBps50,							KCapsBps50,
	KStrKCapsBps75,							KCapsBps75,
	KStrKCapsBps110,						KCapsBps110,
	KStrKCapsBps134,						KCapsBps134,
	KStrKCapsBps150,						KCapsBps150,
	KStrKCapsBps300,						KCapsBps300,
	KStrKCapsBps600,						KCapsBps600,
	KStrKCapsBps1200,						KCapsBps1200,
	KStrKCapsBps1800,						KCapsBps1800,
	KStrKCapsBps2000,						KCapsBps2000,
	KStrKCapsBps2400,						KCapsBps2400,
	KStrKCapsBps3600,						KCapsBps3600,
	KStrKCapsBps4800,						KCapsBps4800,
	KStrKCapsBps7200,						KCapsBps7200,
	KStrKCapsBps9600,						KCapsBps9600,
	KStrKCapsBps19200,						KCapsBps19200,
	KStrKCapsBps38400,						KCapsBps38400,
	KStrKCapsBps57600,						KCapsBps57600,
	KStrKCapsBps115200,						KCapsBps115200,
	KStrKCapsBps230400,						KCapsBps230400,
	KStrKCapsBps460800,						KCapsBps460800,
	KStrKCapsBps576000,						KCapsBps576000,
	KStrKCapsBps1152000,					KCapsBps1152000,
	KStrKCapsBps4000000,					KCapsBps4000000,
	KStrKCapsBps921600,						KCapsBps921600,
	KStrKCapsBpsAutobaud,					KCapsBpsAutobaud,
	KStrKCapsBpsSpecial,					KCapsBpsSpecial,
	KStrNull,								-1
	};

_LIT(KStrKCapsSIR115kbps,					"KCapsSIR115kbps");
_LIT(KStrKCapsSIR2400bpsOnly,				"KCapsSIR2400bpsOnly");
_LIT(KStrKCapsSIR4Mbs,						"KCapsSIR4Mbs");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV01Data::iEnumTableSir[] =
	{
//	Enum as a descriptor					Enum
	KStrKCapsSIR115kbps,					KCapsSIR115kbps,
	KStrKCapsSIR2400bpsOnly,				KCapsSIR2400bpsOnly,
	KStrKCapsSIR4Mbs,						KCapsSIR4Mbs,
	KStrNull,								-1
	};

_LIT(KStrKCapsSignalCTSSupported,			"KCapsSignalCTSSupported");
_LIT(KStrKCapsSignalDSRSupported,			"KCapsSignalDSRSupported");
_LIT(KStrKCapsSignalDCDSupported,			"KCapsSignalDCDSupported");
_LIT(KStrKCapsSignalRNGSupported,			"KCapsSignalRNGSupported");
_LIT(KStrKCapsSignalRTSSupported,			"KCapsSignalRTSSupported");
_LIT(KStrKCapsSignalDTRSupported,			"KCapsSignalDTRSupported");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV01Data::iEnumTableSignals[] =
	{
//	Enum as a descriptor					Enum
	KStrKCapsSignalCTSSupported,			KCapsSignalCTSSupported,
	KStrKCapsSignalDSRSupported,			KCapsSignalDSRSupported,
	KStrKCapsSignalDCDSupported,			KCapsSignalDCDSupported,
	KStrKCapsSignalRNGSupported,			KCapsSignalRNGSupported,
	KStrKCapsSignalRTSSupported,			KCapsSignalRTSSupported,
	KStrKCapsSignalDTRSupported,			KCapsSignalDTRSupported,
	KStrNull,								-1
	};

_LIT(KStrKCapsStop1,						"KCapsStop1");
_LIT(KStrKCapsStop2,						"KCapsStop2");
const CDataWrapperBase::TEnumEntryTable	CT_TCommCapsV01Data::iEnumTableStopBits[] =
	{
//	Enum as a descriptor					Enum
	KStrKCapsStop1,							KCapsStop1,
	KStrKCapsStop2,							KCapsStop2,
	KStrNull,								-1
	};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_TCommCapsV01Data::CT_TCommCapsV01Data()
:	CDataWrapperBase()
,	iCommCaps(NULL)
	{
	}

CT_TCommCapsV01Data::~CT_TCommCapsV01Data()
/**
 * Public destructor
 */
	{
	DestroyData();
	}

void CT_TCommCapsV01Data::DestroyData()
	{
	delete iCommCaps;
	iCommCaps=NULL;
	}

TDes8* CT_TCommCapsV01Data::Descriptor()
/**
 * Return a pointer to the buffer
 *
 * @return	pointer to the buffer
 */
	{
	return iCommCaps;
	}

TAny* CT_TCommCapsV01Data::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
	{
	return iCommCaps;
	}

TBool CT_TCommCapsV01Data::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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
	else if ( aCommand==KCmdiRate )
		{
		DoCmdiRate(aSection);
		}
	else if ( aCommand==KCmdiSIR )
		{
		DoCmdiSIR(aSection);
		}
	else if ( aCommand==KCmdiSignals )
		{
		DoCmdiSignals(aSection);
		}
	else if ( aCommand==KCmdiStopBits )
		{
		DoCmdiStopBits(aSection);
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

TCommCapsV01& CT_TCommCapsV01Data::GetCommCapsV01()
	{
	return (*iCommCaps)();
	}

TInt CT_TCommCapsV01Data::DoCmdConstructor()
	{
	DestroyData();
	TRAPD(err, iCommCaps=new (ELeave) TCommCaps());
	iCommCaps->FillZ();
	return err;
	}

void CT_TCommCapsV01Data::DoCmdDestructor()
	{
	DestroyData();
	}

void CT_TCommCapsV01Data::DoCmdiDataBits(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV01().iDataBits;
	INFO_PRINTF2(_L("iDataBits : 0x%X"), actual);

	TUint	expected;
	if ( GetOrFromConfig(aSection, KFldExpected(), iEnumTableDataBits, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_TCommCapsV01Data::DoCmdiFifo(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV01().iFifo;
	INFO_PRINTF2(_L("iFifo : 0x%X"), actual);

	TUint	expected;
	if ( GetOrFromConfig(aSection, KFldExpected(), iEnumTableFifo, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_TCommCapsV01Data::DoCmdiHandshake(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV01().iHandshake;
	INFO_PRINTF2(_L("iHandshake : 0x%X"), actual);

	TUint	expected;
	if ( GetOrFromConfig(aSection, KFldExpected(), iEnumTableHandshake, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_TCommCapsV01Data::DoCmdiParity(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV01().iParity;
	INFO_PRINTF2(_L("iParity : 0x%X"), actual);

	TUint	expected;
	if ( GetOrFromConfig(aSection, KFldExpected(), iEnumTableParity, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_TCommCapsV01Data::DoCmdiRate(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV01().iRate;
	INFO_PRINTF2(_L("iRate : 0x%X"), actual);

	TUint	expected;
	if ( GetOrFromConfig(aSection, KFldExpected(), iEnumTableRate, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_TCommCapsV01Data::DoCmdiSIR(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV01().iSIR;
	INFO_PRINTF2(_L("iSIR : 0x%X"), actual);

	TUint	expected;
	if ( GetOrFromConfig(aSection, KFldExpected(), iEnumTableSir, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_TCommCapsV01Data::DoCmdiSignals(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV01().iSignals;
	INFO_PRINTF2(_L("iSignals : 0x%X"), actual);

	TUint	expected;
	if ( GetOrFromConfig(aSection, KFldExpected(), iEnumTableSignals, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_TCommCapsV01Data::DoCmdiStopBits(const TDesC& aSection)
	{
	TUint	actual=GetCommCapsV01().iStopBits;
	INFO_PRINTF2(_L("iStopBits : 0x%X"), actual);

	TUint	expected;
	if ( GetOrFromConfig(aSection, KFldExpected(), iEnumTableStopBits, expected) )
		{
		if ( expected!=actual )
			{
			ERR_PRINTF1(_L("Expected Value does not match actual"));
			SetBlockResult(EFail);
			}
		}
	}
