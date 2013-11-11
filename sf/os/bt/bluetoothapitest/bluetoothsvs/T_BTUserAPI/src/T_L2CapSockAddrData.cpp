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

#include "T_L2CapSockAddrData.h"
#include "T_BTServiceSecurityData.h"
#include "T_BTDevAddrData.h"


/*@{*/
//Parameters
_LIT(KFamily,					"family");
_LIT(KTSockAddr,				"sockaddr");
_LIT(KPort,						"port");
_LIT(KSecurity,					"security");
_LIT(KBTAddr,					"btaddr");
_LIT(KTBTDevAddr,				"tbtdevaddr");

_LIT(KHexBTAddrL,				"hex_btaddr_l");
_LIT(KHexBTAddrR,				"hex_btaddr_r");

_LIT(KSection,					"section");
_LIT(KConfigHexBTAddrL,			"hex_btaddr_l");
_LIT(KConfigHexBTAddrR,			"hex_btaddr_r");

//Commands
_LIT(KCmdTL2CAPSockAddr,		"TL2CAPSockAddr");
_LIT(KCmdCast,					"Cast");
_LIT(KCmdSetPort,				"SetPort");
_LIT(KCmdPort,					"Port");
_LIT(KCmdSetSecurity,			"SetSecurity");
_LIT(KCmdSetBTAddr,				"SetBTAddr");
_LIT(KCmdBTAddr,				"BTAddr");
_LIT(KCmdSetFamily,				"SetFamily");

//	Logging
_LIT(KLogMissingParameter,			"Missing parameter '%S'");

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_L2CAPSockAddrData* CT_L2CAPSockAddrData::NewL()
	{
	CT_L2CAPSockAddrData*	ret=new (ELeave) CT_L2CAPSockAddrData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_L2CAPSockAddrData::CT_L2CAPSockAddrData()
:	iData(NULL)
	{
	}

void CT_L2CAPSockAddrData::ConstructL()
	{
	}

CT_L2CAPSockAddrData::~CT_L2CAPSockAddrData()
	{
	DestroyData();
	}

TAny* CT_L2CAPSockAddrData::GetObject()
	{
	return iData;
	}

void CT_L2CAPSockAddrData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iData = static_cast<TL2CAPSockAddr*> (aAny);
	}

void CT_L2CAPSockAddrData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_L2CAPSockAddrData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_L2CAPSockAddrData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_L2CAPSockAddrData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	TL2CAPSockAddr* arg=static_cast<TL2CAPSockAddr*>(aAny);
	delete arg;
	}

TBool CT_L2CAPSockAddrData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdTL2CAPSockAddr )
		{
		DoCmdTL2CAPSockAddr(aSection);
		}
	else if ( aCommand==KCmdCast )
		{
		DoCmdCast(aSection);	
		}
	else if ( aCommand==KCmdPort )
		{
		DoCmdPort(aSection);
		}
	else if ( aCommand==KCmdSetPort )
		{
		DoCmdSetPort(aSection);
		}
	else if ( aCommand==KCmdSetSecurity )
		{		
		DoCmdSetSecurity(aSection);
		}
	else if ( aCommand==KCmdSetBTAddr )
		{		
		DoCmdSetBTAddr(aSection);	
		}
	else if ( aCommand==KCmdBTAddr )
		{				
		DoCmdBTAddr(aSection);
		}
	else if ( aCommand==KCmdSetFamily )
		{		
		DoCmdSetFamily(aSection);
		}			
		
	else
		{
		ret=EFalse;
		}
	return ret;
	}


/**
Test TL2CAPSockAddr()
*/
void CT_L2CAPSockAddrData::DoCmdTL2CAPSockAddr(const TDesC& aSection)
	{
		
	TPtrC myTSockAddrName;
	
	if ( !GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName) )
		{
		DestroyData();
		//Use default constructor
		INFO_PRINTF1(_L("TL2CAPSockAddr Constructor Call"));
		iData = new (ELeave) TL2CAPSockAddr();
		}
	else
		{
		TSockAddr* myTSockAddr = static_cast<TSockAddr*>(GetDataObjectL(myTSockAddrName));
		
		if ( myTSockAddr!=NULL )
			{
			DestroyData();
			INFO_PRINTF1(_L("TL2CAPSockAddr Constructor Call"));
			iData = new (ELeave) TL2CAPSockAddr(*myTSockAddr);				
			}
		else
			{
			ERR_PRINTF1(_L("TL2CAPSockAddr Construct From TSockAddr. TSockAddr is NULL"));
			SetBlockResult(EFail);	
			}	
		}
	}


/**
Test Cast()
*/
void CT_L2CAPSockAddrData::DoCmdCast(const TDesC& aSection)
	{			
	TPtrC myTSockAddrName;
	
	if (GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName) )
		{
		TSockAddr* myTSockAddr = static_cast<TSockAddr*>(GetDataObjectL(myTSockAddrName));
		if ( myTSockAddr!=NULL )
			{
			INFO_PRINTF1(_L("TL2CAPSockAddr Cast Call"));
			TL2CAPSockAddr& tl2CAPSockAddr = TL2CAPSockAddr::Cast(*myTSockAddr);	
			INFO_PRINTF1(_L("Downcast a TSockAddr to a TL2CAPSockAddr succeeded"));			
			}		
		else
			{
			ERR_PRINTF1(_L("TSockAddr is NULL"));
			SetBlockResult(EFail);	
			}	
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KTSockAddr());
		SetBlockResult(EFail);
		}
	}


/**
Test Port()
*/
void CT_L2CAPSockAddrData::DoCmdPort(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TL2CAPSockAddr Port Call"));	
	TL2CAPPort portReceived = iData->Port();
	INFO_PRINTF2(_L("TL2CAPSockAddr Port: actual  (%d)" ), portReceived);
	
	TInt	port=0;
	if(GetIntFromConfig(aSection, KPort(), port))
		{
		INFO_PRINTF2(_L("TL2CAPSockAddr Port: expected (%d)" ), port);
		
		if(port != portReceived)
			{
			ERR_PRINTF1(_L("Port is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KPort());
		SetBlockResult(EFail);				
		}	
	}
	
	
/**
Test SetPort()
*/
void CT_L2CAPSockAddrData::DoCmdSetPort(const TDesC& aSection)
	{
	TInt	port=0;
	if(GetIntFromConfig(aSection, KPort(), port))
		{
		INFO_PRINTF1(_L("TL2CAPSockAddr SetPort Call"));	
		iData->SetPort((TL2CAPPort)port);	
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KPort());
		SetBlockResult(EFail);				
		}			
	}	


void CT_L2CAPSockAddrData::DoCmdSetSecurity(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TL2CAPSockAddr SetSecurity to current iBTServiceSecurity"));		
	TPtrC myBTServiceSecurityName;
	TBTServiceSecurity*	myBTServiceSecurity=NULL;
	if ( !GetStringFromConfig(aSection, KSecurity(), myBTServiceSecurityName) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KSecurity());
		SetBlockResult(EFail);
		}
	else
		{
		myBTServiceSecurity=static_cast<TBTServiceSecurity*>(GetDataObjectL(myBTServiceSecurityName));	
		}
	if ( myBTServiceSecurity!=NULL )
		{
		iData->SetSecurity(*myBTServiceSecurity);	
		}
	else
		{
		ERR_PRINTF1(_L("TL2CAPSockAddr SetSecurity TBTServiceSecurity is NULL"));
		SetBlockResult(EFail);				
		}			
	}



void CT_L2CAPSockAddrData::DoCmdSetBTAddr(const TDesC& aSection)
	{		
	TInt devId;
	if(GetIntFromConfig(aSection, KBTAddr(), devId))
		{
		INFO_PRINTF2(_L("TBTSockAddr SetBTAddr (%d)"), devId);	
		iData->SetBTAddr(devId);				
		}
	else
		{
		TPtrC	configSectionName;
		if(GetStringFromConfig(aSection, KSection(), configSectionName))
			{
			INFO_PRINTF2(_L("Getting BT Adress from config section: (%S)"), &configSectionName);	
			TInt lhs;
			TInt rhs;
			if ( !GetHexFromConfig(aSection, KConfigHexBTAddrL(), lhs ))
				{
				ERR_PRINTF1(_L("TBTSockAddrSetBTAddr GetHexFromConfig failed"));		
				SetBlockResult(EFail);	
				}			
			if ( !GetHexFromConfig(aSection, KConfigHexBTAddrR(), rhs ))
				{
				ERR_PRINTF1(_L("TBTSockAddrSetBTAddr GetHexFromConfig failed"));		
				SetBlockResult(EFail);	
				}
			TBTDevAddr btDevAddr (MAKE_TINT64(lhs, rhs));
			iData->SetBTAddr(btDevAddr);		
			}
		else
			{
			TPtrC	myTBTDevAddrName;
			if(GetStringFromConfig(aSection, KTBTDevAddr(), myTBTDevAddrName))
				{
				CT_BTDevAddrData* myTBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(myTBTDevAddrName));	
				iData->SetBTAddr(*myTBTDevAddrData->GetAddress());
				TBTDevAddr retrivedDevAddr = *myTBTDevAddrData->GetAddress();			
				TBuf<KMaxSockAddrSize> tmpBuf;
				retrivedDevAddr.GetReadable(tmpBuf);
				INFO_PRINTF2(_L("Bluetooth address from stored object:   actual = 0x%S"),&tmpBuf);
				}
			else
				{		
				TInt lhs;
				TInt rhs;
				if ( !GetHexFromConfig(aSection, KHexBTAddrL(), lhs ))
					{
					ERR_PRINTF1(_L("TL2CAPSockAddr GetHexFromConfig failed"));		
					SetBlockResult(EFail);	
					}			
				if ( !GetHexFromConfig(aSection, KHexBTAddrR(), rhs ))
					{
					ERR_PRINTF1(_L("TL2CAPSockAddr SetBTAddr GetHexFromConfig failed"));		
					SetBlockResult(EFail);	
					}
				TBTDevAddr btDevAddr (MAKE_TINT64(lhs, rhs));
				iData->SetBTAddr(btDevAddr);
				}
			}
		}
	}	

void CT_L2CAPSockAddrData::DoCmdBTAddr(const TDesC& aSection)
	{				
	TInt	devId;

	TBTDevAddr retrivedDevAddr = iData->BTAddr();			
	TBuf<KMaxSockAddrSize> tmpBuf;
	retrivedDevAddr.GetReadable(tmpBuf);
	INFO_PRINTF2(_L("Bluetooth address:   actual = 0x%S"),&tmpBuf);
	
	if(GetIntFromConfig(aSection, KBTAddr(), devId))
		{
		TBTDevAddr devAddr= TBTDevAddr(devId);
		TBuf<KMaxSockAddrSize> tmpBuf2;
		devAddr.GetReadable(tmpBuf2);
		INFO_PRINTF2(_L("Bluetooth address: expected = 0x%S"),&tmpBuf2);						

		if(devAddr != retrivedDevAddr)
			{
			ERR_PRINTF1(_L("BTAddr is not as expected!"));
			SetBlockResult(EFail);				
			}
		else
			{
			INFO_PRINTF1(_L("TBTSockAddr BTAddr as expected"));	
			}
		}
	else
		{
		TPtrC	configSectionName;
		if(GetStringFromConfig(aSection, KSection(), configSectionName))
			{
			INFO_PRINTF2(_L("Getting BT Adress from config section: (%S)"), &configSectionName);	
			TInt lhs;
			TInt rhs;
			if ( !GetHexFromConfig(aSection, KConfigHexBTAddrL(), lhs ))
				{
				ERR_PRINTF1(_L("TBTSockAddrSetBTAddr GetHexFromConfig failed"));		
				SetBlockResult(EFail);	
				}			
			if ( !GetHexFromConfig(aSection, KConfigHexBTAddrR(), rhs ))
				{
				ERR_PRINTF1(_L("TBTSockAddrSetBTAddr GetHexFromConfig failed"));		
				SetBlockResult(EFail);	
				}
			TBTDevAddr devAddr (MAKE_TINT64(lhs, rhs));	
			if(devAddr != retrivedDevAddr)
				{
				ERR_PRINTF1(_L("BTAddr is not as expected!"));
				SetBlockResult(EFail);				
				}
			else
				{
				INFO_PRINTF1(_L("TBTSockAddr BTAddr as expected"));	
				}			
			}	
		
		else
			{
			TPtrC	myTBTDevAddrName;
			if(GetStringFromConfig(aSection, KTBTDevAddr(), myTBTDevAddrName))
				{
				CT_BTDevAddrData* myTBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(myTBTDevAddrName));	
				TBTDevAddr btDevAddr = *myTBTDevAddrData->GetAddress();
				TBuf<KMaxSockAddrSize> tmpBuf2;
				btDevAddr.GetReadable(tmpBuf2);
				if (tmpBuf2 != tmpBuf)
					{
					ERR_PRINTF3(_L("Expected address (%S) != actual address (%S)"),&tmpBuf2,&tmpBuf);
					SetBlockResult(EFail);	
					}
				}
			else
				{
				TInt lhs;
				TInt rhs;
				if (GetHexFromConfig(aSection, KHexBTAddrL(), lhs ))
					{
					if ( !GetHexFromConfig(aSection, KHexBTAddrR(), rhs ))
						{
						ERR_PRINTF1(_L("TBTSockAddrBTAddr GetHexFromConfig failed"));		
						SetBlockResult(EFail);	
						}
					TBTDevAddr devAddr (MAKE_TINT64(lhs, rhs));	
					if(devAddr != retrivedDevAddr)
						{
						ERR_PRINTF1(_L("BTAddr is not as expected!"));
						SetBlockResult(EFail);				
						}
					else
						{
						INFO_PRINTF1(_L("TBTSockAddr BTAddr as expected"));	
						}			
					}
				}
			}
		}
	}


void CT_L2CAPSockAddrData::DoCmdSetFamily(const TDesC& aSection)
	{	
	TInt	family;
	if(GetIntFromConfig(aSection, KFamily(), family))
		{
		INFO_PRINTF2(_L("TL2CAPSockAddr SetFamily (%d)"), family);
		iData->SetFamily(family);				
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFamily());
		SetBlockResult(EFail);				
		}		
	}

