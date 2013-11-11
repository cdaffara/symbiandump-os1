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

#include "T_DataRSdpDatabase.h"

/*@{*/

//Parameters
_LIT(KObjectName,							"objectName");
_LIT(KInputUUIDHex,							"inputUUIDHex");
_LIT(KInputAttrIDHex,						"inputAttrIDHex");
_LIT(KInputDesCValueEight,					"inputDesCValueEight");
_LIT(KInputDesCValueSixteen,				"inputDesCValueSixteen");
_LIT(KInputInt,								"inputInt");
_LIT(KInputArrayPosition,					"inputArrayPosition");

//Commands
_LIT(KCmdConstructor,						"Constructor");
_LIT(KCmdOpen,								"Open");
_LIT(KCmdClose,								"Close");
_LIT(KCmdCreateServiceRecordL,				"CreateServiceRecordL");
_LIT(KCmdDeleteRecordL,						"DeleteRecordL");
_LIT(KCmdDeleteRecord,						"DeleteRecord");
_LIT(KCmdDeleteAttributeL,					"DeleteAttributeL");
_LIT(KCmdDeleteAttribute,					"DeleteAttribute");
_LIT(KCmdUpdateAttributeL,					"UpdateAttributeL");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataRSdpdatabase* CT_DataRSdpdatabase::NewL()
	{
	CT_DataRSdpdatabase*	ret=new	(ELeave) CT_DataRSdpdatabase();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataRSdpdatabase::CT_DataRSdpdatabase()
:	iHandleArray(NULL)
	{
	}

void CT_DataRSdpdatabase::ConstructL()
	{
	}

CT_DataRSdpdatabase::~CT_DataRSdpdatabase()
	{
	DestroyData();
	}

void CT_DataRSdpdatabase::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iHandleArray = static_cast<CRSdpHandleArray*> (aAny);
	}

void CT_DataRSdpdatabase::DisownObjectL()
	{
	iHandleArray = NULL;
	}

void CT_DataRSdpdatabase::DestroyData()
	{
	delete iHandleArray;
	iHandleArray=NULL;
	}

inline TCleanupOperation CT_DataRSdpdatabase::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataRSdpdatabase::CleanupOperation(TAny* aAny)
	{
	CRSdpHandleArray* handleArray=static_cast<CRSdpHandleArray*>(aAny);
	delete handleArray;
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
TBool CT_DataRSdpdatabase::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdConstructor )
		{
		DoCmdConstructor();
		}
	else if	( aCommand==KCmdOpen )
		{
		DoCmdOpen(aSection);
		}
	else if( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if( aCommand==KCmdCreateServiceRecordL	)
		{
		DoCmdCreateServiceRecordL(aSection);
		}
	else if( aCommand==KCmdDeleteRecordL )
		{
		DoCmdDeleteRecordL(aSection);
		}
	else if( aCommand==KCmdDeleteRecord )
		{
		DoCmdDeleteRecord(aSection);
		}
	else if( aCommand==KCmdDeleteAttributeL )
		{
		DoCmdDeleteAttributeL(aSection);
		}
	else if( aCommand==KCmdDeleteAttribute )
		{
		DoCmdDeleteAttribute(aSection);
		}
	else if( aCommand==KCmdUpdateAttributeL )
		{
		DoCmdUpdateAttributeL(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


void CT_DataRSdpdatabase::DoCmdConstructor()
	{
	DestroyData();

	TRAPD(err,	iHandleArray =	new	(ELeave) CRSdpHandleArray());
	if(err	!= KErrNone)
		{
		ERR_PRINTF2(_L("**** RSdpDatabase	Constructor	failed	with error %d"), err);
		SetError(err);
		}
	}

void CT_DataRSdpdatabase::DoCmdOpen(const TDesC& aSection)
	{
	TPtrC		objectName;

	if( GetStringFromConfig(aSection, KObjectName(), objectName) )
		{
		RSdp*	rsdp = static_cast<RSdp*>(GetDataObjectL(objectName));

		TInt error	= iHandleArray->iDatabase.Open(*rsdp);
		if(error !=	KErrNone)
			{
			ERR_PRINTF2(_L("**** RSdpDatabase	Open failed	with error %d"), error);
			SetError(error);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KObjectName());
		SetBlockResult(EFail);
		}
	}

void CT_DataRSdpdatabase::DoCmdClose()
	{
	iHandleArray->iDatabase.Close();
	}

void CT_DataRSdpdatabase::DoCmdCreateServiceRecordL(const TDesC& aSection)
	{
	TInt	inputHex=0;

	if(	GetHexFromConfig(aSection, KInputUUIDHex(), inputHex))
		{
		INFO_PRINTF2(_L("Input Hex = %d"),inputHex);

		TRAPD(errOne,	iHandleArray->iDatabase.CreateServiceRecordL(TUUID(TUint32(inputHex)), iRecHandle));
		if(errOne	!= KErrNone)
			{
			ERR_PRINTF2(_L("**** CreateServiceRecordL	failed with	error %d"),	errOne);
			SetError(errOne);
			}

		INFO_PRINTF1(_L("RSdpDatabase::CreateServiceRecordL"));
		}
	else
		{
		TPtrC				objectName;

		if( GetStringFromConfig(aSection, KObjectName(), objectName) )
			{
			CSdpAttrValueDES*	tempUUIDList = static_cast<CSdpAttrValueDES*>(GetDataObjectL(objectName));

			TRAPD(errTwo,	iHandleArray->iDatabase.CreateServiceRecordL(*tempUUIDList, iRecHandle));
			if(errTwo	!= KErrNone)
				{
				ERR_PRINTF2(_L("**** CreateServiceRecordL	failed with	error %d"),	errTwo);
				SetError(errTwo);
				}

			INFO_PRINTF1(_L("RSdpDatabase::CreateServiceRecordL (list)"));
			}
		else
			{
			ERR_PRINTF2(_L("Missing parameter %S"), &KObjectName());
			SetBlockResult(EFail);
			}
		}

	INFO_PRINTF2(_L("iRecHandle = %d"), iRecHandle);
	iHandleArray->iRecHandleArray.Append(iRecHandle);
	}

void CT_DataRSdpdatabase::DoCmdDeleteRecord(const TDesC& aSection)
	{
	TInt	temp=0;
	if(	GetIntFromConfig(aSection, KInputArrayPosition(), temp))
		{
		iHandleArray->iDatabase.DeleteRecord(iHandleArray->iRecHandleArray[temp]);
		iHandleArray->iRecHandleArray.Remove(temp);
		}
	else
		{
		TInt lastPosition=iHandleArray->iRecHandleArray.Count()-1;
		INFO_PRINTF2(_L("DeleteRecord iRecHandle = %d"), iHandleArray->iRecHandleArray[lastPosition]);
		iHandleArray->iDatabase.DeleteRecord(iHandleArray->iRecHandleArray[lastPosition]);
		iHandleArray->iRecHandleArray.Remove(lastPosition);
		}
	}

void CT_DataRSdpdatabase::DoCmdDeleteRecordL(const TDesC& aSection)
	{

	TInt	temp=0;
	if(	GetIntFromConfig(aSection, KInputArrayPosition(), temp))
		{
		iHandleArray->iDatabase.DeleteRecordL(iHandleArray->iRecHandleArray[temp]);
		iHandleArray->iRecHandleArray.Remove(temp);
		}
	else
		{
		TInt lastPosition=iHandleArray->iRecHandleArray.Count()-1;
		INFO_PRINTF2(_L("DeleteRecord iRecHandle = %d"), iHandleArray->iRecHandleArray[lastPosition]);
		iHandleArray->iDatabase.DeleteRecordL(iHandleArray->iRecHandleArray[lastPosition]);
		iHandleArray->iRecHandleArray.Remove(lastPosition);
		}
	}

void CT_DataRSdpdatabase::DoCmdDeleteAttribute(const TDesC& aSection)
	{
	TInt		inputHex=0;
	if(	GetHexFromConfig(aSection, KInputAttrIDHex(), inputHex))
		{
		iHandleArray->iDatabase.DeleteAttribute(iRecHandle, inputHex);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInputAttrIDHex());
		SetBlockResult(EFail);
		}
	}

void CT_DataRSdpdatabase::DoCmdDeleteAttributeL(const TDesC& aSection)
	{
	TInt		inputHex=0;

	if(	GetHexFromConfig(aSection, KInputAttrIDHex(), inputHex))
		{
		iHandleArray->iDatabase.DeleteAttributeL(iRecHandle, inputHex);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInputAttrIDHex());
		SetBlockResult(EFail);
		}
	}

void CT_DataRSdpdatabase::DoCmdUpdateAttributeL(const TDesC& aSection)
	{
	TInt				inputHex=0;
	TInt				inputInt=0;
	TPtrC				inputStr;
	TPtrC				objectName;
	CSdpAttrValue*		attrVal= NULL;

	if(	GetHexFromConfig(aSection, KInputAttrIDHex(), inputHex))
		{
		INFO_PRINTF2(_L("Input Hex = %d"),inputHex);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInputAttrIDHex());
		SetBlockResult(EFail);
		}

	TInt	err=KErrNone;
	if(	GetIntFromConfig(aSection, KInputInt(), inputInt))
		{
		INFO_PRINTF2(_L("Input Int = %d"),inputInt);

		TRAP(err, iHandleArray->iDatabase.UpdateAttributeL(iRecHandle, inputHex, inputInt));
		INFO_PRINTF1(_L("RSdpDatabase::UpdateAttributeL (Int)"));
		}
	else
		{
		if( GetStringFromConfig(aSection, KObjectName(), objectName) )
			{
			attrVal = static_cast<CSdpAttrValue*>(GetDataObjectL(objectName));

			TRAP(err, iHandleArray->iDatabase.UpdateAttributeL(iRecHandle, inputHex, *attrVal));
			INFO_PRINTF1(_L("RSdpDatabase::UpdateAttributeL (attribute)"));
			}
		else
			{
			if(	GetStringFromConfig(aSection, KInputDesCValueEight(),	inputStr))
				{
				HBufC8*		tempStr = HBufC8::NewLC(inputStr.Length());
				tempStr->Des().Copy(inputStr);

				TRAP(err, iHandleArray->iDatabase.UpdateAttributeL(iRecHandle, inputHex, *tempStr));
				INFO_PRINTF1(_L("RSdpDatabase::UpdateAttributeL (descriptor TDesC8)"));
				CleanupStack::PopAndDestroy(tempStr);
				}
			else
				{
				if(	GetStringFromConfig(aSection, KInputDesCValueSixteen(),	inputStr))
					{
					TRAP(err, iHandleArray->iDatabase.UpdateAttributeL(iRecHandle, inputHex, inputStr));
					INFO_PRINTF1(_L("RSdpDatabase::UpdateAttributeL (descriptor TDesC16)"));
					}
				else
					{
					ERR_PRINTF2(_L("Missing parameter %S"), &KInputDesCValueSixteen());
					SetBlockResult(EFail);
					}
				}
			}
		}
	if(err	!= KErrNone)
		{
		ERR_PRINTF2(_L("**** UpdateAttributeL	failed with	error %d"),	err);
		SetError(err);
		}

	INFO_PRINTF2(_L("iRecHandle = %d"), iRecHandle);
	}
