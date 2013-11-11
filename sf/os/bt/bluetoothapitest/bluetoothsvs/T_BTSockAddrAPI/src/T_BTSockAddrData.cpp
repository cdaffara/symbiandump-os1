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


#include "T_BTSockAddrData.h"
#include "T_BTServiceSecurityData.h"
#include "T_BTDevAddrData.h"


//Constant Literals used.

//Parameters
_LIT(KFamily,									"family");
_LIT(KBTAddr,									"btaddr");
_LIT(KUserlen,									"userlen");
_LIT(KPort,										"port");
_LIT(KTSockAddr,								"sockaddr");
_LIT(KTBTSockAddr,								"btsockaddr");
_LIT(KSecurity,									"security");
_LIT(KExpectedCompResult,						"exp_cmp_res");
_LIT(KTBTDevAddr,								"tbtdevaddr");

_LIT(KHexBTAddrL,								"hex_btaddr_l");
_LIT(KHexBTAddrR,								"hex_btaddr_r");

_LIT(KSection,									"section");
_LIT(KConfigHexBTAddrL,							"hex_btaddr_l");
_LIT(KConfigHexBTAddrR,							"hex_btaddr_r");

// TBTSockAddr
_LIT(KCmdConstruct,								"Construct");
_LIT(KCmdCastFromTSockAddr,						"CastFromTSockAddr");
_LIT(KCmdSetBTAddr,								"SetBTAddr");
_LIT(KCmdBTAddr,								"BTAddr");
_LIT(KCmdSetSecurity,							"SetSecurity");
_LIT(KCmdBTSecurity,							"BTSecurity");
_LIT(KCmdSetFamily,								"SetFamily");
_LIT(KCmdFamily,								"Family");
_LIT(KCmdSetPort,								"SetPort");
_LIT(KCmdPort,									"Port");
_LIT(KCmdCmpPort,								"CmpPort");
_LIT(KCmdGetUserLen,							"GetUserLen");
_LIT(KCmdDistructor,							"~");

//Use of Protected methods
_LIT(KCmdPROTECTEDSetUserLen,					"PROTECTED-SetUserLen");
_LIT(KCmdPROTECTEDUserPtr,						"PROTECTED-UserPtr");
_LIT(KCmdPROTECTEDEndBTSockAddrPtr,				"PROTECTED-EndBTSockAddrPtr");

/*@}*/

CT_BTSockAddrData* CT_BTSockAddrData::NewL()
	{
	CT_BTSockAddrData*	ret=new (ELeave) CT_BTSockAddrData();
	return ret;
	}

CT_BTSockAddrData::CT_BTSockAddrData()
: iTBTSockAddr(NULL), iTBTSockAddrOwned(EFalse)
	{
	}
	
CT_BTSockAddrData::~CT_BTSockAddrData()
	{
	Cleanup();
	}
	
void CT_BTSockAddrData::Cleanup()
	{
	if (iTBTSockAddrOwned)
		{
		delete iTBTSockAddr;	
		}
	iTBTSockAddr=NULL;	
	}

TBool CT_BTSockAddrData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool								retVal=ETrue;
		
	if ( aCommand==KCmdConstruct )
		{
		TBTSockAddrConstruct(aSection);
		}		
	else if ( aCommand==KCmdCastFromTSockAddr )
		{
		TBTSockAddrCastFromTSockAddr(aSection);
		}		
	else if ( aCommand==KCmdSetBTAddr )
		{		
		TBTSockAddrSetBTAddr(aSection);
		}	
	else if ( aCommand==KCmdBTAddr )
		{				
		TBTSockAddrBTAddr(aSection);
		}			
	else if ( aCommand==KCmdSetSecurity )
		{		
		TBTSockAddrSetSecurity(aSection);	
		}	
	else if ( aCommand==KCmdBTSecurity )
		{	
		TBTSockAddrBTSecurity(aSection);
		}			
	else if ( aCommand==KCmdSetFamily )
		{		
		TBTSockAddrSetFamily(aSection);
		}
	else if ( aCommand==KCmdFamily )
		{		
		TBTSockAddrFamily(aSection);
		}		
	else if ( aCommand==KCmdSetPort )
		{		
		TBTSockAddrSetPort(aSection);
		}
	else if ( aCommand==KCmdPort )
		{		
		TBTSockAddrPort(aSection);
		}
	else if ( aCommand==KCmdCmpPort )
		{					
		TBTSockAddrCmpPort(aSection);
		}		
	else if ( aCommand==KCmdGetUserLen )
		{		
		TBTSockAddrGetUserLen(aSection);
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
		TBTSockAddrPROTECTEDSetUserLen(aSection);
		}
	else if ( aCommand==KCmdPROTECTEDUserPtr )
		{		
		TBTSockAddrPROTECTEDUserPtr();
		}	
	else if ( aCommand==KCmdPROTECTEDEndBTSockAddrPtr )
		{		
		TBTSockAddrPROTECTEDEndBTSockAddrPtr();
		}	
	else
		{
		retVal=EFalse;
		}

	return retVal;
	}	
	
void CT_BTSockAddrData::TBTSockAddrConstruct(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTSockAddr Constructor Call"));
	TPtrC myTSockAddrName;
	TSockAddr*	myTSockAddr=NULL;
	if ( !GetStringFromConfig(aSection, KTSockAddr(), myTSockAddrName) )
		{
		//Use default constructor
		Cleanup();
		TRAPD( err, iTBTSockAddr = new (ELeave)T_TBTSockAddrChild() );
		if ( err )
		    {
		    SetError(err);
		    }
		else
		    {
		    iTBTSockAddrOwned=ETrue;
		    }
		}
	else
		{
		TRAPD( err, myTSockAddr=static_cast<TSockAddr*>(GetDataObjectL(myTSockAddrName)) );
		if ( (!err ) && (myTSockAddr!=NULL) )
			{
			Cleanup();
			TRAPD( err ,iTBTSockAddr = new (ELeave) T_TBTSockAddrChild(*myTSockAddr) );
    		if ( err )
    		    {
    		    SetError(err);
    		    }
    		else
    		    {
    		    iTBTSockAddrOwned=ETrue;
    		    }
			}
		else
			{
			ERR_PRINTF1(_L("TSockAddr pointer is NULL"));		
			SetBlockResult(EFail);		
			}
		}
	}	
	
	
void CT_BTSockAddrData::TBTSockAddrCastFromTSockAddr(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TBTSockAddr Cast from TSockAddr Call"));
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
		iTBTSockAddr = &T_TBTSockAddrChild::Cast(*myTSockAddr);	
		iTBTSockAddrOwned=EFalse;
		}
	else
		{
		ERR_PRINTF1(_L("TSockAddr pointer is NULL"));		
		SetBlockResult(EFail);		
		}
	}	
		
void CT_BTSockAddrData::TBTSockAddrSetBTAddr(const TDesC& aSection)
	{		
	TInt	devId;
	if( GetIntFromConfig(aSection, KBTAddr(), devId))
		{
		INFO_PRINTF2(_L("TBTSockAddr SetBTAddr (%d)"), devId);	
		iTBTSockAddr->SetBTAddr(devId);				
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
			iTBTSockAddr->SetBTAddr(btDevAddr);		
			}
		else
			{
			TPtrC	myTBTDevAddrName;
			if( GetStringFromConfig(aSection, KTBTDevAddr(), myTBTDevAddrName))
				{
				CT_BTDevAddrData* myTBTDevAddrData=static_cast<CT_BTDevAddrData*>(GetDataWrapperL(myTBTDevAddrName));	
				iTBTSockAddr->SetBTAddr(*myTBTDevAddrData->GetAddress());
				}
			else
				{		
				TInt lhs;
				TInt rhs;
				if ( !GetHexFromConfig(aSection, KHexBTAddrL(), lhs ))
					{
					ERR_PRINTF1(_L("TBTSockAddrSetBTAddr GetHexFromConfig failed"));		
					SetBlockResult(EFail);	
					}			
				if ( !GetHexFromConfig(aSection, KHexBTAddrR(), rhs ))
					{
					ERR_PRINTF1(_L("TBTSockAddrSetBTAddr GetHexFromConfig failed"));		
					SetBlockResult(EFail);	
					}
				TBTDevAddr btDevAddr (MAKE_TINT64(lhs, rhs));
				iTBTSockAddr->SetBTAddr(btDevAddr);		
				}
			}
		}
	}
		
void CT_BTSockAddrData::TBTSockAddrBTAddr(const TDesC& aSection)
	{				
	TInt	devId;

	TBTDevAddr retrivedDevAddr = iTBTSockAddr->BTAddr();			
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
		
void CT_BTSockAddrData::TBTSockAddrSetSecurity(const TDesC& aSection)
	{		
	INFO_PRINTF1(_L("TBTSockAddr SetSecurity to current iBTServiceSecurity"));	
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
		iTBTSockAddr->SetSecurity(*myBTServiceSecurity);	
		}
	else
		{
		ERR_PRINTF1(_L("TBTSockAddrSetSecurity BTServiceSecurity is NULL"));		
		SetBlockResult(EFail);		
		}	
	}	
	
void CT_BTSockAddrData::TBTSockAddrBTSecurity(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TBTSockAddr set iBTServiceSecurity from BTSecurity"));			
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
		TBTServiceSecurity tmp = iTBTSockAddr->BTSecurity();
		myBTServiceSecurityData->CopyObjectL(&tmp);
		}
	else
		{
		ERR_PRINTF1(_L("TBTSockAddrBTSecurity BTServiceSecurity is NULL"));		
		SetBlockResult(EFail);		
		}	
	}	
			
void CT_BTSockAddrData::TBTSockAddrSetFamily(const TDesC& aSection)
	{		
	TInt	family;
	if( GetIntFromConfig(aSection, KFamily(), family))
		{
		INFO_PRINTF2(_L("TBTSockAddr SetFamily (%d)"), family);	
		iTBTSockAddr->SetFamily(family);				
		}
	else
		{
		ERR_PRINTF1(_L("TBTSockAddrSetFamily GetIntFromConfig failed"));		
		SetBlockResult(EFail);		
		}		
	}
	
void CT_BTSockAddrData::TBTSockAddrFamily(const TDesC& aSection)
	{		
	TInt	family;

	TInt familyReceived = static_cast<TInt>(iTBTSockAddr->Family());
	INFO_PRINTF2(_L("TBTSockAddr Family: actual   (%d)" ), familyReceived);
	
	if( GetIntFromConfig(aSection, KFamily(), family))
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
	
void CT_BTSockAddrData::TBTSockAddrSetPort(const TDesC& aSection)
	{		
	TInt	port;
	if( GetIntFromConfig(aSection, KPort(), port))
		{
		INFO_PRINTF2(_L("TBTSockAddr SetPort (%d)"), port);	
		iTBTSockAddr->SetPort(port);				
		}
	else
		{
		ERR_PRINTF1(_L("TBTSockAddrSetPort GetIntFromConfig failed"));		
		SetBlockResult(EFail);		
		}
	}
	
void CT_BTSockAddrData::TBTSockAddrPort(const TDesC& aSection)
	{		
	TInt	port;
	
	TInt portReceived = static_cast<TInt>(iTBTSockAddr->Port());
	INFO_PRINTF2(_L("TBTSockAddr Port: actual   (%d)" ), portReceived);
	
	if( GetIntFromConfig(aSection, KPort(), port))
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
	
void CT_BTSockAddrData::TBTSockAddrCmpPort(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TBTSockAddrCmpPort"));
	TPtrC myTBTSockAddrName;
	TBTSockAddr* myTBTSockAddr=NULL;
	
	TUint portReceived = iTBTSockAddr->Port();
	TUint familyReceived = iTBTSockAddr->Family();
	INFO_PRINTF3(_L("TBTSockAddrCmpPort Port: actual   (%d), Family: actual   (%d)"), portReceived, familyReceived );
	
	if ( !GetStringFromConfig(aSection, KTBTSockAddr(), myTBTSockAddrName) )
		{
		ERR_PRINTF2(_L("No %S"), &KTBTSockAddr());
		SetBlockResult(EFail);
		}
	else
		{
		myTBTSockAddr=static_cast<TBTSockAddr*>(GetDataObjectL(myTBTSockAddrName));	
		}
	if ( myTBTSockAddr!=NULL )
		{
		
		TUint port = myTBTSockAddr->Port();
		TUint family = myTBTSockAddr->Family();
		
		INFO_PRINTF3(_L("TBTSockAddrCmpPort Port: expected (%d), Family: expected (%d)"), port, family );

		TBool expectedCompResult =ETrue;
		
		GetBoolFromConfig(aSection, KExpectedCompResult(), expectedCompResult);
		TBool actualCompResult = myTBTSockAddr->CmpPort(*iTBTSockAddr);
		INFO_PRINTF3(_L("TBTSockAddrCmpPort Compare result expected (%d), actual (%d)"), expectedCompResult, actualCompResult );
		if( actualCompResult != expectedCompResult)
			{
			ERR_PRINTF1(_L("TBTSockAddrCmpPort failed"));		
			SetBlockResult(EFail);				
			}	
		}
	else
		{
		ERR_PRINTF1(_L("TBTSockAddr is NULL"));		
		SetBlockResult(EFail);		
		}
	}				

void CT_BTSockAddrData::TBTSockAddrGetUserLen(const TDesC& aSection)
	{						
	TInt	userLen;
	
	TInt userLenReceived = static_cast<TInt>(iTBTSockAddr->GetUserLen());
	INFO_PRINTF2(_L("TBTSockAddr Protected UserLen: actual   (%d)"), userLenReceived);
	
	if( GetIntFromConfig(aSection, KUserlen(), userLen))
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
	
void CT_BTSockAddrData::TBTSockAddrPROTECTEDSetUserLen(const TDesC& aSection)
	{		
	TInt	userLen;
	if( GetIntFromConfig(aSection, KUserlen(), userLen))
		{
		INFO_PRINTF2(_L("TBTSockAddr Protected SetUserLen (%d)"), userLen);
		iTBTSockAddr->T_SetUserLen(userLen);				
		}
	else
		{
		ERR_PRINTF1(_L("TBTSockAddrPROTECTEDSetUserLen GetIntFromConfig failed"));		
		SetBlockResult(EFail);		
		}		
	}	
		
void CT_BTSockAddrData::TBTSockAddrPROTECTEDUserPtr()
	{		
	TUint8* userPtr = iTBTSockAddr->T_UserPtr();
	INFO_PRINTF2(_L("TBTSockAddr Protected userPtr: (%d)" ), userPtr);
	if(!userPtr)
		{
		ERR_PRINTF1(_L("Null pointer retrived!"));
		SetBlockResult(EFail);				
		}
	}
			
void CT_BTSockAddrData::TBTSockAddrPROTECTEDEndBTSockAddrPtr()
	{		
	TAny * endBTSockAddrPtr = iTBTSockAddr->T_EndBTSockAddrPtr();
	INFO_PRINTF2(_L("TBTSockAddr Protected EndBTSockAddrPtr: (%d)" ), endBTSockAddrPtr);
	if(!endBTSockAddrPtr)
		{
		ERR_PRINTF1(_L("Null pointer retrived!"));
		SetBlockResult(EFail);				
		}
	}	
					
	
TAny* CT_BTSockAddrData::GetObject()
	{
	return iTBTSockAddr;
	}
	
	
void CT_BTSockAddrData::SetObject(T_TBTSockAddrChild* aTBTSockAddr, TBool aTransferOwnership)
	{
	iTBTSockAddr = aTBTSockAddr;
	iTBTSockAddrOwned = aTransferOwnership;
	}	
