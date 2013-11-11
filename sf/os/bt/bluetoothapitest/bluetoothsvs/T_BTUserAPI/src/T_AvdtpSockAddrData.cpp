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

#include <bluetoothav.h>

//	User Includes
#include "T_AvdtpSockAddrData.h"
#include "T_BTSockAddrData.h"
#include "T_BTServiceSecurityData.h"
#include "T_BTDevAddrData.h"
#include "T_BTUtil.h"
/*@{*/
//Parameters

//Fields
_LIT(KFldExpected, 					    "expected");

_LIT(KFldSockAddr,                      "sockaddr");
_LIT(KFldSession,                       "session");
_LIT(KFldSeid,                          "seid");

_LIT(KFldFamily, 					    "family");
_LIT(KFldBTAddr,						"btaddr");
_LIT(KFldUserlen,						"userlen");
_LIT(KFldPort,							"port");
_LIT(KFldSecurity,						"security");
_LIT(KFldTBTDevAddr,					"tbtdevaddr");

_LIT(KFldHexBTAddrL,					"hex_btaddr_l");
_LIT(KFldHexBTAddrR,					"hex_btaddr_r");

_LIT(KFldSection,									"section");
_LIT(KFldConfigHexBTAddrL,							"hex_btaddr_l");
_LIT(KFldConfigHexBTAddrR,							"hex_btaddr_r");

//Commands
_LIT(KCmdnew,							"new");
_LIT(KCmdBTAddr,						"BTAddr");
_LIT(KCmdBTSecurity,					"BTSecurity");
_LIT(KCmdCast,							"Cast");
_LIT(KCmdFamily,						"Family");
_LIT(KCmdGetUserLen,					"GetUserLen");
_LIT(KCmdPort,							"Port");
_LIT(KCmdSetSession,					"SetSession");
_LIT(KCmdSession,						"Session");
_LIT(KCmdSetSEID,						"SetSEID");
_LIT(KCmdSEID,							"SEID");
_LIT(KCmdDestructor,					"~"); 

//	Logging
_LIT(KLogMissingParameter,			"Missing parameter '%S'");
_LIT(KLogError,					    "Error=%d");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_AvdtpSockAddrData* CT_AvdtpSockAddrData::NewL()
	{
	CT_AvdtpSockAddrData*	ret=new (ELeave) CT_AvdtpSockAddrData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_AvdtpSockAddrData::CT_AvdtpSockAddrData()
:	iData(NULL), iDataOwned(EFalse)
	{
	}

void CT_AvdtpSockAddrData::ConstructL()
	{
	}

CT_AvdtpSockAddrData::~CT_AvdtpSockAddrData()
	{
	DestroyData();
	}

TAny* CT_AvdtpSockAddrData::GetObject()
	{
	return iData;
	}


void CT_AvdtpSockAddrData::SetObject(TAvdtpSockAddr* aAvdtpSockAddr, TBool aTransferOwnership)
	{
	DestroyData();
	iData = aAvdtpSockAddr;
	iDataOwned = aTransferOwnership;
	}

void CT_AvdtpSockAddrData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_AvdtpSockAddrData::DestroyData()
	{
	if(iDataOwned)
		{
		delete iData;
		}
	
	iData=NULL;	
	}

inline TCleanupOperation CT_AvdtpSockAddrData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_AvdtpSockAddrData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	TAvdtpSockAddr* arg=static_cast<TAvdtpSockAddr*>(aAny);
	delete arg;
	}


TBool CT_AvdtpSockAddrData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	
	if ( aCommand==KCmdnew )
		{
		DoCmdnewL(aSection);
		}
	else if( aCommand== KCmdCast)
		{
		DoCmdCast(aSection);
		}
	else if( aCommand== KCmdSetSession)
		{
		DoCmdSetSession(aSection);
		}
	else if( aCommand== KCmdSession)
		{
		DoCmdSession(aSection);
		}
	else if( aCommand== KCmdSetSEID)
		{
		DoCmdSetSEID(aSection);
		}
	else if( aCommand== KCmdSEID)
		{
		DoCmdSEID(aSection);
		}
	else if( aCommand== KCmdFamily)
		{
		DoCmdFamily(aSection);
		}
	else if( aCommand== KCmdGetUserLen)
		{
		DoCmdGetUserLen(aSection);
		}
	else if( aCommand== KCmdPort)
		{
		DoCmdPort(aSection);
		}
	else if( aCommand== KCmdBTAddr)
		{
		DoCmdBTAddr(aSection);
		}
	else if( aCommand== KCmdBTSecurity)
		{
		DoCmdBTSecurity(aSection);
		}
	else if ( aCommand==KCmdDestructor )
		{
		DestroyData();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

/**
Test TAvdtpSockAddr()
*/
void CT_AvdtpSockAddrData::DoCmdnewL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSockAddr constructor call"));
	DestroyData();
	
	TInt err = KErrNone;
	TSockAddr* sockAddr = NULL;
	TPtrC strSockAddr;
	
	if ( GetStringFromConfig(aSection, KFldSockAddr, strSockAddr) )
		{
		INFO_PRINTF1(_L("TAvdtpSockAddr construct with TSockAddr object"));
		sockAddr = static_cast<TSockAddr*>(GetDataObjectL(strSockAddr));
		}
	
	if(sockAddr != NULL)
		{
		TRAP(err, iData = new ( ELeave ) TAvdtpSockAddr( *sockAddr ));
		}
	else
		{
		INFO_PRINTF1(_L("TAvdtpSockAddr default constructor"));
		TRAP(err, iData = new ( ELeave ) TAvdtpSockAddr());
		}
	
	if (err!= KErrNone)
		{
		ERR_PRINTF2(KLogError, err);
		SetBlockResult(EFail);
		}
	else
		{
		iDataOwned = ETrue;
		}
	}

/**
Test Cast()
*/
void CT_AvdtpSockAddrData::DoCmdCast(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSockAddr Cast from TSockAddr Call"));
	TPtrC myTSockAddrName;
	TSockAddr*	myTSockAddr=NULL;
	if ( !GetStringFromConfig(aSection, KFldSockAddr(), myTSockAddrName) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldSockAddr());
		SetBlockResult(EFail);
		}
	else
		{
		myTSockAddr=static_cast<TSockAddr*>(GetDataObjectL(myTSockAddrName));	
		
		if ( myTSockAddr!=NULL )
			{
			DestroyData();
			iData = &TAvdtpSockAddr::Cast(*myTSockAddr);	
			iDataOwned = EFalse;
			}
		else
			{
			ERR_PRINTF1(_L("TSockAddr pointer is NULL"));		
			SetBlockResult(EFail);		
			}
		}
	}

/**
Test SetSession()
*/
void CT_AvdtpSockAddrData::DoCmdSetSession(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSockAddr SetSession call"));
	
	TAvdtpTransportSessionType session;
	
	if(CT_BTUtil::ReadAvdtpTransportSessionType(*this, aSection, KFldSession, session))
		{
		INFO_PRINTF2(_L("TAvdtpSockAddr SetSession: session = %d"),session);
		iData->SetSession(session);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldSession());
		SetBlockResult(EFail);
		}
	}

/**
Test Session()
*/
void CT_AvdtpSockAddrData::DoCmdSession(const TDesC& aSection)
	{
	TAvdtpTransportSessionType actual;
	TAvdtpTransportSessionType expected;
	
	INFO_PRINTF1(_L("TAvdtpSockAddr Session call"));
	actual = iData->Session();
	
	if (CT_BTUtil::ReadAvdtpTransportSessionType(*this, aSection, KFldExpected, expected))
		{
		if ( actual != expected )
			{
			ERR_PRINTF1(_L("Session not match as expected"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldExpected());
		SetBlockResult(EFail);
		}
	}

/**
Test SetSEID()
*/
void CT_AvdtpSockAddrData::DoCmdSetSEID(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSockAddr SetSEID call"));
	TSEID* seid = NULL;
	TPtrC strSeid;
	
	if(GetStringFromConfig(aSection, KFldSeid, strSeid))
		{
		seid = static_cast<TSEID*>(GetDataObjectL(strSeid));

		if (seid != NULL)
			{
			iData->SetSEID(*seid);
			}
		else
			{
			ERR_PRINTF1(_L("TSEID pointer is NULL"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldSeid());
		SetBlockResult(EFail);		
		}
	

		
	}

/**
Test SEID()
*/
void CT_AvdtpSockAddrData::DoCmdSEID(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSockAddr SEID call"));
	TSEID* expectedSEID = NULL;
	TPtrC strSeid;
	
	if(GetStringFromConfig(aSection, KFldExpected, strSeid))
		{
		expectedSEID = static_cast<TSEID*>(GetDataObjectL(strSeid));
		
		if(expectedSEID != NULL)
			{
			TSEID actual = iData->SEID();
			if(*expectedSEID != actual)
				{
				ERR_PRINTF1(_L("Result is not as expected"));
				SetBlockResult(EFail);				
				}
			}
		else
			{
			ERR_PRINTF1(_L("TSEID pointer is NULL"));
			SetBlockResult(EFail);		
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldExpected());
		SetBlockResult(EFail);		
		}
	}

void CT_AvdtpSockAddrData::DoCmdFamily(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TAvdtpSockAddr Family call"));
	TInt	family;

	TInt familyReceived = static_cast<TInt>(iData->Family());
	INFO_PRINTF2(_L("TBTSockAddr Family: actual   (%d)" ), familyReceived);
	
	if( GetIntFromConfig(aSection, KFldFamily(), family))
		{
		INFO_PRINTF2(_L("TBTSockAddr Family: expected (%d)" ), family);

		if(family != familyReceived)
			{
			ERR_PRINTF1(_L("Family is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		ERR_PRINTF1(_L("TBTSockAddrFamily GetIntFromConfig failed"));		
		SetBlockResult(EFail);		
		}
	}

void CT_AvdtpSockAddrData::DoCmdGetUserLen(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSockAddr GetUserLen call"));
	
	TInt	userLen;
	
	TInt userLenReceived = static_cast<TInt>(iData->GetUserLen());
	INFO_PRINTF2(_L("TBTSockAddr Protected UserLen: actual   (%d)"), userLenReceived);
	
	if( GetIntFromConfig(aSection, KFldUserlen(), userLen))
		{
		INFO_PRINTF2(_L("TBTSockAddr Protected UserLen: expected (%d)" ), userLen);
		if(userLen != userLenReceived)
			{
			ERR_PRINTF1(_L("UserLen is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		ERR_PRINTF1(_L("TBTSockAddrGetUserLen GetIntFromConfig failed"));		
		SetBlockResult(EFail);		
		}	
	}
void CT_AvdtpSockAddrData::DoCmdPort(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSockAddr Port call"));
	
	TInt	port;
	
	TInt portReceived = static_cast<TInt>(iData->Port());
	INFO_PRINTF2(_L("TBTSockAddr Port: actual   (%d)" ), portReceived);
	
	if( GetIntFromConfig(aSection, KFldPort(), port))
		{
		INFO_PRINTF2(_L("TBTSockAddr Port: expected (%d)" ), port);
		if(port != portReceived)
			{
			ERR_PRINTF1(_L("Port is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		ERR_PRINTF1(_L("TBTSockAddrPort GetIntFromConfig failed"));		
		SetBlockResult(EFail);		
		}
	}
void CT_AvdtpSockAddrData::DoCmdBTAddr(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSockAddr BTAddr call"));
	
	TInt	devId;

	TBTDevAddr retrivedDevAddr = iData->BTAddr();			
	TBuf<KMaxSockAddrSize> tmpBuf;
	retrivedDevAddr.GetReadable(tmpBuf);
	INFO_PRINTF2(_L("Bluetooth address:   actual = 0x%S"),&tmpBuf);
	
	if( GetIntFromConfig(aSection, KFldBTAddr(), devId))
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
		if( GetStringFromConfig(aSection, KFldSection(), configSectionName))
			{
			INFO_PRINTF2(_L("Getting BT Adress from config section: (%S)"), &configSectionName);	
			TInt lhs;
			TInt rhs;
			if ( !GetHexFromConfig(configSectionName, KFldConfigHexBTAddrL(), lhs ))
				{
				ERR_PRINTF1(_L("TBTSockAddrSetBTAddr GetHexFromConfig failed"));		
				SetBlockResult(EFail);	
				}			
			if ( !GetHexFromConfig(configSectionName, KFldConfigHexBTAddrR(), rhs ))
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
			if( GetStringFromConfig(aSection, KFldTBTDevAddr(), myTBTDevAddrName))
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
				if ( GetHexFromConfig(aSection, KFldHexBTAddrL(), lhs ))
					{
					if ( !GetHexFromConfig(aSection, KFldHexBTAddrR(), rhs ))
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
void CT_AvdtpSockAddrData::DoCmdBTSecurity(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSockAddr BTSecurity call"));
		
	TPtrC		myBTServiceSecurityName;
	CT_BTServiceSecurityData*	myBTServiceSecurityData=NULL;
	if ( !GetStringFromConfig(aSection, KFldSecurity(), myBTServiceSecurityName) )
		{
		ERR_PRINTF2(_L("No %S"), &KFldSecurity());
		SetBlockResult(EFail);
		}
	else
		{
		myBTServiceSecurityData=static_cast<CT_BTServiceSecurityData*>(GetDataWrapperL(myBTServiceSecurityName));	
		}
	if ( myBTServiceSecurityData!=NULL )
		{
		TBTServiceSecurity tmp = iData->BTSecurity();
		myBTServiceSecurityData->CopyObjectL(&tmp);
		}
	else
		{
		ERR_PRINTF1(_L("TBTSockAddrBTSecurity BTServiceSecurity is NULL"));		
		SetBlockResult(EFail);		
		}	
	}
