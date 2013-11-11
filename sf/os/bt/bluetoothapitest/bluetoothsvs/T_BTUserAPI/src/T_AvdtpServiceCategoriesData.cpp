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
#include "T_AvdtpServiceCategoriesData.h"
#include "T_BTUtil.h"

/*@{*/
//Parameters

//Fields
_LIT(KFldExpected, 					"expected");
_LIT(KFldCategory,                     "category");


//Commands
_LIT(KCmdnew,									"new");
_LIT(KCmdBitMask,								"()");
_LIT(KCmdSetCapability,							"SetCapability");
_LIT(KCmdClearCapability,						"ClearCapability");
_LIT(KCmdCapabilityPresent,						"CapabilityPresent");
_LIT(KCmdDestructor,							"~"); 

//	Logging
_LIT(KLogMissingParameter,			"Missing parameter '%S'");
/*@}*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_AvdtpServiceCategoriesData* CT_AvdtpServiceCategoriesData::NewL()
	{
	CT_AvdtpServiceCategoriesData*	ret=new (ELeave) CT_AvdtpServiceCategoriesData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_AvdtpServiceCategoriesData::CT_AvdtpServiceCategoriesData()
:	iData(NULL)
	{
	}

void CT_AvdtpServiceCategoriesData::ConstructL()
	{
	}

CT_AvdtpServiceCategoriesData::~CT_AvdtpServiceCategoriesData()
	{
	DestroyData();
	}

TAny* CT_AvdtpServiceCategoriesData::GetObject()
	{
	return iData;
	}


void CT_AvdtpServiceCategoriesData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iData = static_cast<TAvdtpServiceCategories*> (aAny);
	}

void CT_AvdtpServiceCategoriesData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_AvdtpServiceCategoriesData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_AvdtpServiceCategoriesData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_AvdtpServiceCategoriesData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	TAvdtpServiceCategories* arg=static_cast<TAvdtpServiceCategories*>(aAny);
	delete arg;
	}


TBool CT_AvdtpServiceCategoriesData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	
	if ( aCommand==KCmdnew )
		{
		DoCmdnewL();
		}
	else if( aCommand== KCmdBitMask)
		{
		DoCmdBitMask(aSection);
		}
	else if( aCommand== KCmdSetCapability)
		{
		DoCmdSetCapability(aSection);
		}
	else if( aCommand== KCmdClearCapability)
		{
		DoCmdClearCapability(aSection);
		}
	else if( aCommand== KCmdCapabilityPresent)
		{
		DoCmdCapabilityPresent(aSection);
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
Test TAvdtpServiceCategories()
*/
void CT_AvdtpServiceCategoriesData::DoCmdnewL()
	{
	INFO_PRINTF1(_L("TAvdtpServiceCategories default constructor call"));
	DestroyData();
	
	TRAPD(err, iData = new ( ELeave ) TAvdtpServiceCategories());
	
	if (err!= KErrNone)
		{
		ERR_PRINTF2(_L("TAvdtpServiceCategories default Constructor err=%d" ),err);
		SetBlockResult(EFail);
		}
	}

/**
Test operator()()
*/
void CT_AvdtpServiceCategoriesData::DoCmdBitMask(const TDesC& aSection)
	{
	TAvdtpServiceCatBitMask actual;
	TInt expected;
	INFO_PRINTF1(_L("TAvdtpServiceCategories operator () call"));
	
	if ( GetIntFromConfig(aSection, KFldExpected(), expected) )
		{
		actual = iData->operator ()();
		INFO_PRINTF2(_L("TAvdtpServiceCategories operator () result: actual = %d"), actual);
		
		if ( (TAvdtpServiceCatBitMask)expected != actual )
			{
			ERR_PRINTF1(_L("operator () not as expected"));
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
Test SetCapability()
*/
void CT_AvdtpServiceCategoriesData::DoCmdSetCapability(const TDesC& aSection)
	{
	TAvdtpServiceCategory category;
	INFO_PRINTF1(_L("TAvdtpServiceCategories SetCapability call"));
	
	if (CT_BTUtil::ReadAvdtpServiceCategory(*this, aSection, KFldCategory(), category))
		{
		INFO_PRINTF2(_L("TAvdtpServiceCategories SetCapability: category = %d"), category);
		iData->SetCapability( category );
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldCategory());
		SetBlockResult(EFail);
		}
	}

/**
Test ClearCapability()
*/
void CT_AvdtpServiceCategoriesData::DoCmdClearCapability(const TDesC& aSection)
	{
	TAvdtpServiceCategory category;
	
	INFO_PRINTF1(_L("TAvdtpServiceCategories ClearCapability call"));
	
	if (CT_BTUtil::ReadAvdtpServiceCategory(*this, aSection, KFldCategory(), category))
		{
		INFO_PRINTF2(_L("TAvdtpServiceCategories ClearCapability: category = %d"), category);
		iData->ClearCapability( category );
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldCategory());
		SetBlockResult(EFail);
		}
	}

/**
Test CapabilityPresent()
*/
void CT_AvdtpServiceCategoriesData::DoCmdCapabilityPresent(const TDesC& aSection)
	{
	TAvdtpServiceCategory category;
	TBool expected;

	INFO_PRINTF1(_L("TAvdtpServiceCategories CapabilityPresent call"));
	
	if (CT_BTUtil::ReadAvdtpServiceCategory(*this, aSection, KFldCategory(), category))
		{
		
		if (GetBoolFromConfig(aSection, KFldExpected(), expected))
			{
			INFO_PRINTF2(_L("TAvdtpServiceCategories CapabilityPresent: category = %d"), category);
			TBool actual = iData->CapabilityPresent( category );
			
			//Here "actual" may be a true TBool type, it could be a TInt, So here will not compare "actual" and "expected" directly, but compare with "!"
			if ( (!expected) != (!actual) )
				{
				ERR_PRINTF1(_L("CapabilityPresent not as expected"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(KLogMissingParameter, &KFldExpected());
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldCategory());
		SetBlockResult(EFail);
		}
	}
