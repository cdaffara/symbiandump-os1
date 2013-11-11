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

#include "T_DataRSdp.h"

/*@{*/

//Parameters
_LIT(KExpected,								"expected");
_LIT(KVersionName,							"name");
_LIT(KVersionBuild,							"build");
_LIT(KVersionMajor,							"major");
_LIT(KVersionMinor,							"minor");

//Commands
_LIT(KCmdConstructor,						"Constructor");
_LIT(KCmdConnect,							"Connect");
_LIT(KCmdClose,								"Close");
_LIT(KCmdVersion,							"Version");
_LIT(KCmdResourceCountMarkStart,			"ResourceCountMarkStart");
_LIT(KCmdResourceCountMarkEnd,				"ResourceCountMarkEnd");
_LIT(KCmdResourceCount,						"ResourceCount");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataRSdp* CT_DataRSdp::NewL()
	{
	CT_DataRSdp*	ret=new	(ELeave) CT_DataRSdp();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataRSdp::CT_DataRSdp()
:	iRsdp(NULL)
	{
	}

void CT_DataRSdp::ConstructL()
	{
	}

CT_DataRSdp::~CT_DataRSdp()
	{
	DestroyData();
	}

void CT_DataRSdp::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iRsdp	= static_cast<RSdp*> (aAny);
	}

void CT_DataRSdp::DisownObjectL()
	{
	iRsdp	= NULL;
	}

void CT_DataRSdp::DestroyData()
	{
	delete iRsdp;
	iRsdp=NULL;
	}

inline TCleanupOperation CT_DataRSdp::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataRSdp::CleanupOperation(TAny* aAny)
	{
	RSdp* sdp=static_cast<RSdp*>(aAny);
	delete sdp;
	}

/**
test script	command	entry point
@internalAll
@return	ETrue if syncronous	command	otherwise EFalse
@param aParam1 descriptor containing the command value
@param aParam2 descriptor containing the command parameter
@pre N/A
@post N/A
*/
TBool CT_DataRSdp::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdConstructor )
		{
		DoCmdConstructor();
		}
	else if( aCommand==KCmdConnect )
		{
		DoCmdConnect();
		}
	else if( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if( aCommand==KCmdVersion )
		{
		DoCmdVersion(aSection);
		}
	else if( aCommand==KCmdResourceCountMarkStart )
		{
		DoCmdResourceCountMarkStart();
		}
	else if( aCommand==KCmdResourceCountMarkEnd	)
		{
		DoCmdResourceCountMarkEnd();
		}
	else if( aCommand==KCmdResourceCount )
		{
		DoCmdResourceCount(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


void CT_DataRSdp::DoCmdConstructor()
	{
	DestroyData();

	TRAPD(err,	iRsdp = new (ELeave) RSdp());
	if(err	!= KErrNone)
		{
		ERR_PRINTF2(_L("**** RSdp Constructor failed with error %d"), err);
		SetError(err);
		}
	}

void CT_DataRSdp::DoCmdConnect()
	{
	TInt err = iRsdp->Connect();
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("**** RSdp Connect failed with error %d"), err);
		SetError(err);
		}
	}

void CT_DataRSdp::DoCmdClose()
	{
	iRsdp->Close();
	}

void CT_DataRSdp::DoCmdVersion(const TDesC& aSection)
	{
	TVersion		version=iRsdp->Version();
	TVersionName	versionName	= version.Name();

	INFO_PRINTF1(_L("Version name  :"));
	INFO_PRINTF1(versionName);
	INFO_PRINTF2(_L("Version build : %d"), (TInt)version.iBuild);
	INFO_PRINTF2(_L("Version major : %d"), (TInt)version.iMajor);
	INFO_PRINTF2(_L("Version minor : %d"), (TInt)version.iMinor);

	TPtrC	name;
	if ( GetStringFromConfig(aSection, KVersionName(), name) )
		{
		if ( name != versionName )
			{
			ERR_PRINTF1(_L("Name does not match expected name"));
			SetBlockResult(EFail);
			}
		}
	else
		{
  		ERR_PRINTF2(_L("Missing expected value %S"), &KVersionName());
  		SetBlockResult(EFail);		
		}
		
	/*
	The following expected values are optional since the version name already includes these info.
	*/
	TInt	intTemp;
	if ( GetIntFromConfig(aSection, KVersionBuild(), intTemp) )
		{
		if ( intTemp != version.iBuild )
			{
			ERR_PRINTF1(_L("Build does not match expected build"));
			SetBlockResult(EFail);
			}
		}

	if ( GetIntFromConfig(aSection, KVersionMajor(), intTemp) )
		{
		if ( intTemp != version.iMajor )
			{
			ERR_PRINTF1(_L("Major does not match expected major"));
			SetBlockResult(EFail);
			}
		}

	if ( GetIntFromConfig(aSection, KVersionMinor(), intTemp) )
		{
		if ( intTemp != version.iMinor )
			{
			ERR_PRINTF1(_L("Minor does not match expected minor"));
			SetBlockResult(EFail);
			}
		}

	if (versionName == _L("") && version.iBuild == 0)
		{
		ERR_PRINTF1(_L("Some version fields are not set!"));
		SetBlockResult(EFail);
		}
		
	if (version.iMajor == 0 && version.iMinor == 0)
		{
		ERR_PRINTF1(_L("Some version fields are not set!"));
		SetBlockResult(EFail);
		}	
	}

void CT_DataRSdp::DoCmdResourceCountMarkStart()
	{
	iRsdp->ResourceCountMarkStart();
	}

void CT_DataRSdp::DoCmdResourceCountMarkEnd()
	{
	iRsdp->ResourceCountMarkEnd();
	}

void CT_DataRSdp::DoCmdResourceCount(const TDesC& aSection)
	{
	TInt	actual=iRsdp->ResourceCount();
	INFO_PRINTF2(_L("RSdp::ResourceCount = %d"), actual);

	TInt	expected;
	if ( GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(_L("ResourceCount is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	}
