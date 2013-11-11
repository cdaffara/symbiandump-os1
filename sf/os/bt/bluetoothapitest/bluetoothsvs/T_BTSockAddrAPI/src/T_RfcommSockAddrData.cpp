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


#include "T_RfcommSockAddrData.h"
#include "T_BTServiceSecurityData.h"
#include "T_BTDevAddrData.h"


//Constant Literals used.

//Parameters
_LIT(KFamily,								"family");
_LIT(KBTAddr,								"btaddr");
_LIT(KUserlen,								"userlen");
_LIT(KPort,									"port");
_LIT(KTRfcommSockAddr,						"rfcommsockaddr");
_LIT(KTSockAddr,							"sockaddr");
_LIT(KSecurity,								"security");
_LIT(KExpectedCompResult,					"exp_cmp_res");
_LIT(KTBTDevAddr,							"tbtdevaddr");

_LIT(KHexBTAddrL,							"hex_btaddr_l");
_LIT(KHexBTAddrR,							"hex_btaddr_r");

_LIT(KSection,								"section");
_LIT(KConfigHexBTAddrL,						"hex_btaddr_l");
_LIT(KConfigHexBTAddrR,						"hex_btaddr_r");

// TRfcommSockAddr
_LIT(KCmdConstruct,							"Construct");
_LIT(KCmdCastFromTSockAddr,					"CastFromTSockAddr");
_LIT(KCmdSetBTAddr,							"SetBTAddr");
_LIT(KCmdBTAddr,							"BTAddr");
_LIT(KCmdSetSecurity,						"SetSecurity");
_LIT(KCmdBTSecurity,						"BTSecurity");
_LIT(KCmdSetFamily,							"SetFamily");
_LIT(KCmdFamily,							"Family");
_LIT(KCmdSetPort,							"SetPort");
_LIT(KCmdPort,								"Port");
_LIT(KCmdCmpPort,							"CmpPort");
_LIT(KCmdGetUserLen,						"GetUserLen");
_LIT(KCmdDistructor,						"~");

//Use of Protected methods
_LIT(KCmdPROTECTEDSetUserLen,				"PROTECTED-SetUserLen");
_LIT(KCmdPROTECTEDEndBTSockAddrPtr,			"PROTECTED-EndBTSockAddrPtr");
_LIT(KCmdPROTECTEDUserPtr,					"PROTECTED-UserPtr");

/*@}*/

CT_TRfcommSockAddrData* CT_TRfcommSockAddrData::NewL()
	{
	CT_TRfcommSockAddrData*	ret=new (ELeave) CT_TRfcommSockAddrData();
	return ret;
	}

CT_TRfcommSockAddrData::CT_TRfcommSockAddrData()
: iTRfcommSockAddr(NULL), iTRfcommSockAddrOwned(EFalse)
	{
	}
	
CT_TRfcommSockAddrData::~CT_TRfcommSockAddrData()
	{
	Cleanup();
	}
	
void CT_TRfcommSockAddrData::Cleanup()
	{
	if (iTRfcommSockAddrOwned)
		{
		delete iTRfcommSockAddr;	
		}
	iTRfcommSockAddr=NULL;	
	}
	
inline TCleanupOperation CT_TRfcommSockAddrData::CleanupOperation()
   	{
   	return CleanupOperation;
   	}
   	
void CT_TRfcommSockAddrData::CleanupOperation(TAny* aAny)
    {
    T_TRfcommSockAddrChild* arg=static_cast<T_TRfcommSockAddrChild*>(aAny);
    delete arg;
    }

TBool CT_TRfcommSockAddrData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool retVal=ETrue;

	if ( aCommand==KCmdConstruct )
		{
		TRfcommSockAddrConstruct(aSection);
		}		
	else if ( aCommand==KCmdCastFromTSockAddr )
		{
		TRfcommSockAddrCastFromTSockAddr(aSection);		
		}							
	else if ( aCommand==KCmdSetBTAddr )
		{		
		TRfcommSockAddrSetBTAddr(aSection);	
		}	
	else if ( aCommand==KCmdBTAddr )
		{				
		TRfcommSockAddrBTAddr(aSection);
		}			
	else if ( aCommand==KCmdSetSecurity )
		{		
		TRfcommSockAddrSetSecurity(aSection);
		}	
	else if ( aCommand==KCmdBTSecurity )
		{	
		TRfcommSockAddrBTSecurity(aSection);	
		}				
			
	else if ( aCommand==KCmdSetFamily )
		{		
		TRfcommSockAddrSetFamily(aSection);
		}
	else if ( aCommand==KCmdFamily )
		{						
		TRfcommSockAddrFamily(aSection);
		}		
	else if ( aCommand==KCmdSetPort )
		{		
		TRfcommSockAddrSetPort(aSection);
		}
	else if ( aCommand==KCmdPort )
		{				
		TRfcommSockAddrPort(aSection);
		}			
	else if ( aCommand==KCmdCmpPort )
		{					
		TRfcommSockAddrCmpPort(aSection);
		}
	else if ( aCommand==KCmdGetUserLen )
		{				
		TRfcommSockAddrGetUserLen(aSection);
		}		
	else if ( aCommand==KCmdDistructor )
		{				
		Cleanup();
		}
	
	//-------------------------------------------------------------------------
	// Test of protected members				
	//-------------------------------------------------------------------------	

	else if ( aCommand==KCmdPROTECTEDSetUserLen )
		{		
		TRfcommSockAddrPROTECTEDSetUserLen(aSection);
		}
	else if ( aCommand==KCmdPROTECTEDUserPtr )
		{		
		TRfcommSockAddrPROTECTEDUserPtr();
		}
	else if ( aCommand==KCmdPROTECTEDEndBTSockAddrPtr )
		{		
		TRfcommSockAddrPROTECTEDEndBTSockAddrPtr();
		}						
	else
		{
		retVal=EFalse;
		}
	return retVal;
	}

void CT_TRfcommSockAddrData::TRfcommSockAddrConstruct(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TRfcommSockAddr Constructor Call"));					
	TPtrC myTSockAddrName;
	TSockAddr*	myTSockAddr=NULL;
	if ( !GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName) )
		{
		Cleanup();
		//Use default constructor		
		TRAPD( err, iTRfcommSockAddr = new (ELeave) T_TRfcommSockAddrChild() );
		if ( !err )
		    {
		    iTRfcommSockAddrOwned=ETrue;
		    }
		else
		    {
		    SetError(err);
		    }
		}
	else
		{
		TRAPD( err, myTSockAddr=static_cast<TSockAddr*>(GetDataObjectL(myTSockAddrName)) );	
		if ( (!err) && (myTSockAddr!=NULL) )
			{
			Cleanup();
			TRAP( err, iTRfcommSockAddr = new (ELeave) T_TRfcommSockAddrChild(*myTSockAddr) );
    		if ( !err )
    		    {
    		    iTRfcommSockAddrOwned=ETrue;
    		    }
    		else
    		    {
    		    SetError(err);
    		    }
			}
		else
			{
			ERR_PRINTF1(_L("TRfcommSockAddrConstructFromTSockAddr TSockAddr is NULL"));
			SetBlockResult(EFail);	
			}	
		}
	}


void CT_TRfcommSockAddrData::TRfcommSockAddrCastFromTSockAddr(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TRfcommSockAddr Cast from TSockAddr Call"));
		
	TPtrC myTSockAddrName;
	TSockAddr*	myTSockAddr=NULL;
	if ( !GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName) )
		{
		ERR_PRINTF2(_L("No %S"), &KTSockAddr());
		SetBlockResult(EFail);
		}
	else
		{
		myTSockAddr=static_cast<TSockAddr*>(GetDataObjectL(myTSockAddrName));	
		}
	if ( myTSockAddr!=NULL )
		{
		Cleanup();
		iTRfcommSockAddr = &T_TRfcommSockAddrChild::Cast(*myTSockAddr);	
		iTRfcommSockAddrOwned=EFalse;
		}		
	else
		{
		ERR_PRINTF1(_L("TRfcommSockAddrCastFromTSockAddr TSockAddr is NULL"));
		SetBlockResult(EFail);				
		}
	}

void CT_TRfcommSockAddrData::TRfcommSockAddrSetBTAddr(const TDesC& aSection)
	{		
	TInt	devId;
	if( GetIntFromConfig(aSection, KBTAddr(), devId))
		{
		INFO_PRINTF2(_L("TBTSockAddr SetBTAddr (%d)"), devId);	
		iTRfcommSockAddr->SetBTAddr(devId);				
		}
	else
		{
		TPtrC	configSectionName;
		if( GetStringFromConfig(aSection, KSection(), configSectionName))
			{
			INFO_PRINTF2(_L("Getting BT Adress from config section: (%S)"), &configSectionName);	
			TInt lhs;
			TInt rhs;
			if ( !GetHexFromConfig(configSectionName, KConfigHexBTAddrL(), lhs ))
				{
				ERR_PRINTF1(_L("TBTSockAddrSetBTAddr GetHexFromConfig failed"));		
				SetBlockResult(EFail);	
				}			
			if ( !GetHexFromConfig(configSectionName, KConfigHexBTAddrR(), rhs ))
				{
				ERR_PRINTF1(_L("TBTSockAddrSetBTAddr GetHexFromConfig failed"));		
				SetBlockResult(EFail);	
				}
			TBTDevAddr btDevAddr (MAKE_TINT64(lhs, rhs));
			iTRfcommSockAddr->SetBTAddr(btDevAddr);		
			}
		else
			{
			TPtrC	myTBTDevAddrName;
			if( GetStringFromConfig(aSection, KTBTDevAddr(), myTBTDevAddrName))
				{
				CT_BTDevAddrData* myTBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(myTBTDevAddrName));	
				iTRfcommSockAddr->SetBTAddr(*myTBTDevAddrData->GetAddress());
				}
			else
				{		
				TInt lhs;
				TInt rhs;
				if ( !GetHexFromConfig(aSection, KHexBTAddrL(), lhs ))
					{
					ERR_PRINTF1(_L("TRfcommSockAddrSetBTAddr GetHexFromConfig failed"));		
					SetBlockResult(EFail);	
					}			
				if ( !GetHexFromConfig(aSection, KHexBTAddrR(), rhs ))
					{
					ERR_PRINTF1(_L("TRfcommSockAddrSetBTAddr GetHexFromConfig failed"));		
					SetBlockResult(EFail);	
					}
				TBTDevAddr btDevAddr (MAKE_TINT64(lhs, rhs));
				iTRfcommSockAddr->SetBTAddr(btDevAddr);
				}
			}
		}
	}	
	
void CT_TRfcommSockAddrData::TRfcommSockAddrBTAddr(const TDesC& aSection)
	{				
	TInt	devId;

	TBTDevAddr retrivedDevAddr = iTRfcommSockAddr->BTAddr();			
	TBuf<KMaxSockAddrSize> tmpBuf;
	retrivedDevAddr.GetReadable(tmpBuf);
	INFO_PRINTF2(_L("Bluetooth address:   actual = 0x%S"),&tmpBuf);
	
	if( GetIntFromConfig(aSection, KBTAddr(), devId))
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
		if( GetStringFromConfig(aSection, KSection(), configSectionName))
			{
			INFO_PRINTF2(_L("Getting BT Adress from config section: (%S)"), &configSectionName);	
			TInt lhs;
			TInt rhs;
			if ( !GetHexFromConfig(configSectionName, KConfigHexBTAddrL(), lhs ))
				{
				ERR_PRINTF1(_L("TBTSockAddrSetBTAddr GetHexFromConfig failed"));		
				SetBlockResult(EFail);	
				}			
			if ( !GetHexFromConfig(configSectionName, KConfigHexBTAddrR(), rhs ))
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
			if( GetStringFromConfig(aSection, KTBTDevAddr(), myTBTDevAddrName))
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
				if ( GetHexFromConfig(aSection, KHexBTAddrL(), lhs ))
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

void CT_TRfcommSockAddrData::TRfcommSockAddrSetSecurity(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TRfcommSockAddr SetSecurity to current iBTServiceSecurity"));		
	TPtrC		myBTServiceSecurityName;
	TBTServiceSecurity*	myBTServiceSecurity=NULL;
	if ( !GetStringFromConfig(aSection, KSecurity(), myBTServiceSecurityName) )
		{
		ERR_PRINTF2(_L("No %S"), &KSecurity());
		SetBlockResult(EFail);
		}
	else
		{
		myBTServiceSecurity=static_cast<TBTServiceSecurity*>(GetDataObjectL(myBTServiceSecurityName));	
		}
	if ( myBTServiceSecurity!=NULL )
		{
		iTRfcommSockAddr->SetSecurity(*myBTServiceSecurity);	
		}
	else
		{
		ERR_PRINTF1(_L("TRfcommSockAddrSetSecurity TBTServiceSecurity is NULL"));
		SetBlockResult(EFail);				
		}			
	}

void CT_TRfcommSockAddrData::TRfcommSockAddrBTSecurity(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TRfcommSockAddr get iBTServiceSecurity from BTSecurity"));			
	TPtrC		myBTServiceSecurityName;
	CT_BTServiceSecurityData*	myBTServiceSecurityData=NULL;
	if ( !GetStringFromConfig(aSection, KSecurity(), myBTServiceSecurityName) )
		{
		ERR_PRINTF2(_L("No %S"), &KSecurity());
		SetBlockResult(EFail);
		}
	else
		{
		myBTServiceSecurityData=static_cast<CT_BTServiceSecurityData*>(GetDataWrapperL(myBTServiceSecurityName));	
		}
	if ( myBTServiceSecurityData!=NULL )
		{
		TBTServiceSecurity tmp = iTRfcommSockAddr->BTSecurity();
		myBTServiceSecurityData->CopyObjectL(&tmp);
		}
	else
		{
		ERR_PRINTF1(_L("TRfcommSockAddrBTSecurity TBTServiceSecurity is NULL"));
		SetBlockResult(EFail);				
		}		
	}

void CT_TRfcommSockAddrData::TRfcommSockAddrSetFamily(const TDesC& aSection)
	{	
	TInt	family;
	if( GetIntFromConfig(aSection, KFamily(), family))
		{
		INFO_PRINTF2(_L("TRfcommSockAddr SetFamily (%d)"), family);
			
		iTRfcommSockAddr->SetFamily(family);				
		}
	else
		{
		ERR_PRINTF1(_L("TRfcommSockAddrSetFamily GetIntFromConfig failed"));
		SetBlockResult(EFail);				
		}		
	}

void CT_TRfcommSockAddrData::TRfcommSockAddrFamily(const TDesC& aSection)
	{						
	TInt	family;
	TInt familyReceived = static_cast<TInt>(iTRfcommSockAddr->Family());
	INFO_PRINTF2(_L("TRfcommSockAddr Family: actual   (%d)" ), familyReceived);

	if( GetIntFromConfig(aSection, KFamily(), family))
		{
		INFO_PRINTF2(_L("TRfcommSockAddr Family: expected (%d)" ), family);
		if(family != familyReceived)
			{
			ERR_PRINTF1(_L("Family is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		ERR_PRINTF1(_L("TRfcommSockAddrFamily GetIntFromConfig failed"));
		SetBlockResult(EFail);				
		}		
	}

void CT_TRfcommSockAddrData::TRfcommSockAddrSetPort(const TDesC& aSection)
	{	
	TInt	port;
	if( GetIntFromConfig(aSection, KPort(), port))
		{
		INFO_PRINTF2(_L("TRfcommSockAddr SetPort (%d)"), port);
			
		iTRfcommSockAddr->SetPort(port);				
		}
	else
		{
		ERR_PRINTF1(_L("TRfcommSockAddrSetPort GetIntFromConfig failed"));
		SetBlockResult(EFail);				
		}		
	}
		
void CT_TRfcommSockAddrData::TRfcommSockAddrPort(const TDesC& aSection)
	{		
	TInt	port;
	TInt portReceived = static_cast<TInt>(iTRfcommSockAddr->Port());
	INFO_PRINTF2(_L("TRfcommSockAddr Port: actual   (%d)" ), portReceived);
	
	if( GetIntFromConfig(aSection, KPort(), port))
		{
		INFO_PRINTF2(_L("TRfcommSockAddr Port: expected (%d)" ), port);
		if(port != portReceived)
			{
			ERR_PRINTF1(_L("Port is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		ERR_PRINTF1(_L("TRfcommSockAddrPort GetIntFromConfig failed"));
		SetBlockResult(EFail);				
		}		
	}		
	
void CT_TRfcommSockAddrData::TRfcommSockAddrCmpPort(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TRfcommSockAddrCmpPort"));
	TPtrC myTRfcommSockAddrName;
	TRfcommSockAddr* myTRfcommSockAddr=NULL;
	
	TUint portReceived = iTRfcommSockAddr->Port();
	TUint familyReceived = iTRfcommSockAddr->Family();
	INFO_PRINTF3(_L("TRfcommSockAddrCmpPort Port: expected (%d), Family: expected (%d)"), portReceived, familyReceived );
	
	if ( !GetStringFromConfig(aSection, KTRfcommSockAddr(), myTRfcommSockAddrName) )
		{
		ERR_PRINTF2(_L("No %S"), &KTRfcommSockAddr());
		SetBlockResult(EFail);
		}
	else
		{
		myTRfcommSockAddr=static_cast<TRfcommSockAddr*>(GetDataObjectL(myTRfcommSockAddrName));	
		}
	if ( myTRfcommSockAddr!=NULL )
		{
		
		TUint port = myTRfcommSockAddr->Port();
		TUint family = myTRfcommSockAddr->Family();
		
		INFO_PRINTF3(_L("TRfcommSockAddrCmpPort Port: actual   (%d), Family: actual   (%d)"), port, family );
		TBool expectedCompResult =ETrue;
		
		GetBoolFromConfig(aSection, KExpectedCompResult(), expectedCompResult);
		TBool actualCompResult =myTRfcommSockAddr->CmpPort(*iTRfcommSockAddr);
		INFO_PRINTF3(_L("TRfcommSockAddrCmpPort Compare result expected (%d), actual (%d)"), expectedCompResult, actualCompResult );
		if( actualCompResult != expectedCompResult)
			{
			ERR_PRINTF1(_L("TRfcommSockAddrCmpPort failed"));		
			SetBlockResult(EFail);				
			}	
		}
	else
		{
		ERR_PRINTF1(_L("TRfcommSockAddr is NULL"));		
		SetBlockResult(EFail);		
		}
	}		

void CT_TRfcommSockAddrData::TRfcommSockAddrGetUserLen(const TDesC& aSection)
	{		
	TInt	userLen;
	TInt userLenReceived = static_cast<TInt>(iTRfcommSockAddr->GetUserLen());
	INFO_PRINTF2(_L("TRfcommSockAddrChild UserLen: actual   (%d)" ), userLenReceived);
	
	if( GetIntFromConfig(aSection, KUserlen(), userLen))
		{
		INFO_PRINTF2(_L("TRfcommSockAddrChild UserLen: expected (%d)" ), userLen);
		if(userLen != userLenReceived)
			{
			ERR_PRINTF1(_L("UserLen is not as expected!"));
			SetBlockResult(EFail);				
			}
		}
	else
		{
		ERR_PRINTF1(_L("TRfcommSockAddrGetUserLen GetIntFromConfig failed"));
		SetBlockResult(EFail);				
		}		
	}
	

void CT_TRfcommSockAddrData::TRfcommSockAddrPROTECTEDSetUserLen(const TDesC& aSection)
	{
	TInt	userLen;
	if( GetIntFromConfig(aSection, KUserlen(), userLen))
		{
		INFO_PRINTF2(_L("TRfcommSockAddr Protected SetUserLen (%d)"), userLen);
		iTRfcommSockAddr->T_SetUserLen(userLen);			
		}
	else
		{
		ERR_PRINTF1(_L("TRfcommSockAddrPROTECTEDSetUserLen GetIntFromConfig failed"));
		SetBlockResult(EFail);				
		}		
	}
		

void CT_TRfcommSockAddrData::TRfcommSockAddrPROTECTEDUserPtr()
	{		
	TUint8* userPtr = iTRfcommSockAddr->T_UserPtr();

	INFO_PRINTF2(_L("TRfcommSockAddr Protected userPtr: (%d)" ), userPtr);
	if(!userPtr)
		{
		ERR_PRINTF1(_L("Null pointer retrived!"));
		SetBlockResult(EFail);				
		}
	}	
	
void CT_TRfcommSockAddrData::TRfcommSockAddrPROTECTEDEndBTSockAddrPtr()
	{		
	TAny * endBTSockAddrPtr = iTRfcommSockAddr->T_EndBTSockAddrPtr();
	INFO_PRINTF2(_L("iTRfcommSockAddr Protected EndBTSockAddrPtr: (%d)" ), endBTSockAddrPtr);
	if(!endBTSockAddrPtr)
		{
		ERR_PRINTF1(_L("Null pointer retrived!"));
		SetBlockResult(EFail);				
		}
	}		
			
TAny* CT_TRfcommSockAddrData::GetObject()
	{
	return iTRfcommSockAddr;
	}
	
void CT_TRfcommSockAddrData::SetObjectL(TAny* aObject)
	{
	delete iTRfcommSockAddr;
	iTRfcommSockAddr = static_cast<T_TRfcommSockAddrChild*>(aObject);
	}

void CT_TRfcommSockAddrData::DisownObjectL()
 	{
 	iTRfcommSockAddr=NULL;
 	}
