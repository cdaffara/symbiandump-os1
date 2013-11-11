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

#include "T_L2CapConfigData.h"
#include "T_BTUtil.h"

/*@{*/
//Parameters
_LIT(KMTUSize,									"mtusize");
_LIT(KMRUSize,									"mrusize");
_LIT(KSpecified,								"specified");
_LIT(KTimer,									"timer");
_LIT(KPriority,									"priority");
_LIT(KChannelReliability,						"reliability");

//Commands
_LIT(KCmdTL2CapConfig,							"TL2CapConfig");
_LIT(KCmdSetMaxTransmitUnitSize,				"SetMaxTransmitUnitSize");
_LIT(KCmdMaxTransmitUnitSize,					"MaxTransmitUnitSize");
_LIT(KCmdSetMaxReceiveUnitSize,					"SetMaxReceiveUnitSize");
_LIT(KCmdMaxReceiveUnitSize,					"MaxReceiveUnitSize");
_LIT(KCmdConfigureReliableChannel,				"ConfigureReliableChannel");
_LIT(KCmdConfigureUnreliableChannel,			"ConfigureUnreliableChannel");
_LIT(KCmdChannelReliability,					"ChannelReliability");
_LIT(KCmdConfigureChannelPriority,				"ConfigureChannelPriority");
_LIT(KCmdChannelPriority,						"ChannelPriority");

//	Logging
_LIT(KLogMissingParameters,			"Missing parameters '%S' or '%S'");
_LIT(KLogMissingParameter,			"Missing parameter '%S'");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_L2CapConfigData* CT_L2CapConfigData::NewL()
	{
	CT_L2CapConfigData*	ret=new (ELeave) CT_L2CapConfigData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_L2CapConfigData::CT_L2CapConfigData()
:	iData(NULL)
	{
	}


void CT_L2CapConfigData::ConstructL()
	{
	}


CT_L2CapConfigData::~CT_L2CapConfigData()
	{
	DestroyData();
	}

TAny* CT_L2CapConfigData::GetObject()
	{
	return iData;
	}

void CT_L2CapConfigData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iData = static_cast<TL2CapConfig*> (aAny);
	}


void CT_L2CapConfigData::DisownObjectL()
	{
	iData = NULL;
	}


void CT_L2CapConfigData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_L2CapConfigData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_L2CapConfigData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	TL2CapConfig* arg=static_cast<TL2CapConfig*>(aAny);
	delete arg;
	}

TBool CT_L2CapConfigData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	if ( aCommand==KCmdTL2CapConfig )
		{
		DoCmdTL2CapConfig();
		}
	else if ( aCommand==KCmdSetMaxTransmitUnitSize )
		{
		DoCmdSetMaxTransmitUnitSize(aSection);
		}
	else if ( aCommand==KCmdMaxTransmitUnitSize )
		{
		DoCmdMaxTransmitUnitSize(aSection);
		}
	else if ( aCommand==KCmdSetMaxReceiveUnitSize )
		{
		DoCmdSetMaxReceiveUnitSize(aSection);
		}
	else if ( aCommand==KCmdMaxReceiveUnitSize )
		{
		DoCmdMaxReceiveUnitSize(aSection);
		}
	else if ( aCommand==KCmdConfigureReliableChannel )
		{
		DoCmdConfigureReliableChannel(aSection);
		}
	else if ( aCommand==KCmdConfigureUnreliableChannel )
		{
		DoCmdConfigureUnreliableChannel(aSection);
		}
	else if ( aCommand==KCmdChannelReliability )
		{
		DoCmdChannelReliability(aSection);
		}
	else if ( aCommand==KCmdConfigureChannelPriority )
		{
		DoCmdConfigureChannelPriority(aSection);
		}
	else if ( aCommand==KCmdChannelPriority )
		{
		DoCmdChannelPriority(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test TL2CapConfig()
*/
void CT_L2CapConfigData::DoCmdTL2CapConfig()
	{
	DestroyData();
	INFO_PRINTF1(_L("TL2CapConfig Constructor Call"));
	iData = new (ELeave) TL2CapConfig();
	
	if ( iData == NULL ) 
		{
		ERR_PRINTF1(_L("TL2CapConfig is NULL"));
		SetBlockResult(EFail);
		}
	}


/**
Test SetMaxTransmitUnitSize()
*/
void CT_L2CapConfigData::DoCmdSetMaxTransmitUnitSize(const TDesC& aSection)
	{
	TInt size = 0;
	if(GetIntFromConfig(aSection, KMTUSize(), size) )
		{
		INFO_PRINTF1(_L("TL2CapConfig SetMaxTransmitUnitSize Call"));
		TInt err = iData->SetMaxTransmitUnitSize((TUint16)size);
		INFO_PRINTF2(_L("SetMaxTransmitUnitSize result (%d)"), err);
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("SetMaxTransmitUnitSize failed with result (%d)"), err);
			SetError(err);	
			}
		}
	else		
		{
		ERR_PRINTF2(KLogMissingParameter, &KMTUSize());
		SetBlockResult(EFail);
		}
	}


/**
Test MaxTransmitUnitSize()
*/
void CT_L2CapConfigData::DoCmdMaxTransmitUnitSize(const TDesC& aSection)
	{
	TBool specified = EFalse;
    INFO_PRINTF1(_L("TL2CapConfig MaxTransmitUnitSize Call"));
	TUint16	result = iData->MaxTransmitUnitSize(specified);
	INFO_PRINTF3(_L("Return value(%d) and specified result (%d)"), result, specified);
	
	TInt	size = 0;
	TBool expected = EFalse;
	TBool foundParameter = EFalse;
	if(GetBoolFromConfig(aSection, KSpecified(), expected) )
		{
		foundParameter = ETrue;
		if ( specified != expected)
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), specified, expected);
			SetBlockResult(EFail);
			}
		else
			{
			if ( specified )
				{
				INFO_PRINTF1(_L("Verify that MTU has been set with the expected value"));
				if(GetIntFromConfig(aSection, KMTUSize(), size) )
					{
					foundParameter = ETrue;
					TUint16 mtu = (TUint16) size;
					if ( result != mtu )
						{
						ERR_PRINTF3(_L("MTU value (%d) is not as expected (%d)"), result, mtu);
						SetBlockResult(EFail);
						}
					}
				}
			}
		}
		
	if ( !foundParameter )
		{
 		ERR_PRINTF2(KLogMissingParameter, &KMTUSize());
		SetBlockResult(EFail);
		}
	}


/**
Test SetMaxReceiveUnitSize()
*/
void CT_L2CapConfigData::DoCmdSetMaxReceiveUnitSize(const TDesC& aSection)
	{	
	TInt size = 0;
	
	if(GetIntFromConfig(aSection, KMRUSize(), size) )
		{
		INFO_PRINTF1(_L("TL2CapConfig SetMaxReceiveUnitSize Call"));
		TInt err = iData->SetMaxReceiveUnitSize((TUint16)size);
		INFO_PRINTF2(_L("SetMaxReceiveUnitSize result (%d)"), err);
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("SetMaxReceiveUnitSize failed with result (%d)"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KMRUSize());
		SetBlockResult(EFail);
		}
	}


/**
Test MaxReceiveUnitSize()
*/
void CT_L2CapConfigData::DoCmdMaxReceiveUnitSize(const TDesC& aSection)
	{
	TBool specified = EFalse;
	INFO_PRINTF1(_L("TL2CapConfig MaxReceiveUnitSize Call"));
	TUint16	result = iData->MaxReceiveUnitSize(specified);
	INFO_PRINTF3(_L("Return value(%d) and specified result (%d)"), result, specified);
	
	TInt	size = 0;
	TBool	expected = EFalse;
	TBool	foundParameter = EFalse;
	if(GetBoolFromConfig(aSection, KSpecified(), expected) )
		{
		foundParameter = ETrue;
		if ( specified != expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), specified, expected);
			SetBlockResult(EFail);
			}
		else
			{
			if ( specified )
				{
				INFO_PRINTF1(_L("Verify that MRU has been set with the expected value"));
				if(GetIntFromConfig(aSection, KMRUSize(), size) )
					{
					foundParameter = ETrue;
					TUint16 mru = (TUint16) size;
					if ( result != mru )
						{
						ERR_PRINTF3(_L("MRU value (%d) is not as expected (%d)"), result, mru);
						SetBlockResult(EFail);
						}
					}
				}
			}
		}
		
	if ( !foundParameter )
		{
 		ERR_PRINTF2(KLogMissingParameter, &KMRUSize());
		SetBlockResult(EFail);
		}
	}


/**
Test ConfigureReliableChannel()
*/
void CT_L2CapConfigData::DoCmdConfigureReliableChannel(const TDesC& aSection)
	{
	TInt time = 0;
	
	if(GetIntFromConfig(aSection, KTimer(), time) )
		{
		INFO_PRINTF1(_L("TL2CapConfig ConfigureReliableChannel Call"));
		TInt	err = iData->ConfigureReliableChannel((TUint16)time);
		INFO_PRINTF2(_L("ConfigureReliableChannel result (%d)"), err);
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("ConfigureReliableChannel failed with result (%d)"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KTimer());
		SetBlockResult(EFail);
		}
	}	
	

/**
Test ConfigureUnreliableChannel()
*/
void CT_L2CapConfigData::DoCmdConfigureUnreliableChannel(const TDesC& aSection)
	{
	TInt time = 0;
	
	if(GetIntFromConfig(aSection, KTimer(), time) )
		{
		INFO_PRINTF1(_L("TL2CapConfig ConfigureUnreliableChannel Call"));
		TInt	err = iData->ConfigureUnreliableChannel((TUint16)time);
		INFO_PRINTF2(_L("ConfigureUnreliableChannel result (%d)"), err);
		
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("ConfigureUnreliableChannel failed with result (%d)"), err);
			SetError(err);	
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KTimer());
		SetBlockResult(EFail);
		}
	}	
	

/**
Test ChannelReliability()
*/
void CT_L2CapConfigData::DoCmdChannelReliability(const TDesC& aSection)
	{
	TBool specified = EFalse;
	TUint16 associateTimer = 0;
	INFO_PRINTF1(_L("TL2CapConfig ChannelReliability Call"));
	TL2CapConfig::TChannelReliability result= iData->ChannelReliability (specified, associateTimer);
	INFO_PRINTF4(_L("ChannelReliability return result (%d), Specified (%d), Time (%d)"), result, specified, associateTimer);

	TBool foundParameter = EFalse;
	TBool expected = EFalse;
	TInt timer = 0;
	TL2CapConfig::TChannelReliability channeltype = TL2CapConfig::EReliableChannel;

	if(GetBoolFromConfig(aSection, KSpecified(), expected) )
		{
		foundParameter = ETrue;
		if ( specified != expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), specified, expected);
			SetBlockResult(EFail);
			}
		else
			{
			if ( specified )
				{
				INFO_PRINTF1(_L("Verify that TChannelReliability has been set with the expected value"));
				CT_BTUtil::GetChannel(*this, channeltype, aSection, KChannelReliability());
				if ( result != channeltype )
					{
					ERR_PRINTF3(_L("TChannelReliability (%d) is not as expected (%d)"), result, channeltype);
					SetBlockResult(EFail);
					}
				else
					{
					INFO_PRINTF1(_L("Verify that aAssociatedTimer has been set with the expected value"));
					if(GetIntFromConfig(aSection, KTimer(), timer) )
						{
						foundParameter = ETrue;
						TUint16 expTimer = (TUint16) timer;
						if (  associateTimer != expTimer )
							{
							ERR_PRINTF3(_L("aAssociatedTimer (%d) is not as expected (%d)"), associateTimer, expTimer);
							SetBlockResult(EFail);
							}
						}
					}
				}
			}
		}

	if ( foundParameter == EFalse)
		{
		ERR_PRINTF3(KLogMissingParameters, &KSpecified(), &KTimer());
		SetBlockResult(EFail);
		}
	}	
	

/**
Test ConfigureChannelPriority()
*/
void CT_L2CapConfigData::DoCmdConfigureChannelPriority(const TDesC& aSection)
	{	
	TL2CapConfig::TChannelPriority priority;
	CT_BTUtil::GetChannelPriority(*this, priority, aSection, KPriority());
	INFO_PRINTF1(_L("TL2CapConfig ConfigureChannelPriority Call"));
	TInt err = iData->ConfigureChannelPriority (priority);
	INFO_PRINTF2(_L("ConfigureChannelPriority result (%d)"), err);
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("ConfigureChannelPriority failed with result (%d)"), err);
		SetError(err);
		}
	}	
	

/**
Test ChannelPriority()
*/
void CT_L2CapConfigData::DoCmdChannelPriority(const TDesC& aSection)
	{	
	TBool specified = EFalse;
	INFO_PRINTF1(_L("TL2CapConfig ChannelPriority Call"));
	TL2CapConfig::TChannelPriority result = iData->ChannelPriority(specified);
	INFO_PRINTF3(_L("Return value (%d) and specified result (%d)"), result, specified);
	
	TBool expected = EFalse;
	
	if(GetBoolFromConfig(aSection, KSpecified(), expected) )
		{
		if ( specified != expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), specified, expected);
			SetBlockResult(EFail);			
			}
		else
			{
			if ( specified )
				{
				INFO_PRINTF1(_L("Verify that TChannelPriority has been set with the expected value"));
				TL2CapConfig::TChannelPriority priority;
				CT_BTUtil::GetChannelPriority(*this, priority, aSection, KPriority());
				if ( result != priority )
					{
					ERR_PRINTF3(_L("TChannelPriority (%d) is not as expected (%d)"), result, priority);
					SetBlockResult(EFail);
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KSpecified());
		SetBlockResult(EFail);
		}	
	}	


