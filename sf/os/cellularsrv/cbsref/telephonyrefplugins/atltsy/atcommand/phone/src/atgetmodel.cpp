// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// CATGetModel

#include "atgetmodel.h"
#include "mslogger.h"


_LIT8(KGetModelCommand,"AT+CGMM\r");
_LIT(KSpaceSeparator," ");
// Class CATGetModel
// ---------------------------------------------------------------------------
// CATGetModel::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetModel* CATGetModel::NewL(CGlobalPhonemanager& aGloblePhone, 
	                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetModel* self = new(ELeave) CATGetModel(aGloblePhone,aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATGetModel::CATGetModel
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetModel::CATGetModel(CGlobalPhonemanager& aGloblePhone, 
	                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATGetModel::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetModel::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Phone_GetModel;
	}
// ---------------------------------------------------------------------------
// CATGetModel::CATGetModel
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetModel::~CATGetModel()
	{
	}
// ---------------------------------------------------------------------------
// CATGetModel::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetModel::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATGetModel::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetModel::ExecuteCommand( )
	{
	iTxBuffer.Copy(KGetModelCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATGetModel::GetPhoneModel
// other items were commented in a header
// ---------------------------------------------------------------------------
RMobilePhone::TMobilePhoneIdentityV1 CATGetModel::GetPhoneModel()
	{
	return iPhoneId;
	}
// ---------------------------------------------------------------------------
// CATGetModel::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGetModel::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATGetModel::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetModel::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) != 0)
		{
		iError = KErrGeneral;
		return ;
		}
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array,PrecedingLine());
	iMoreInfoFlag = EFalse;
	TInt Count = array.Count();
	if (Count < 1)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrGeneral;
		return ;
		}
	for(TInt i = 0; i < Count; i++)
		{
		//lex.Assign(array[2]);
		
		if  (iMoreInfoFlag)
			{
			TBuf<RMobilePhone::KPhoneModelIdSize> Data;
			Data.Zero();
			TInt remainingBufferSize = RMobilePhone::KPhoneModelIdSize - 
									   iPhoneId.iModel.Length() - 1;
			if(array[i].Length() > remainingBufferSize)
				{
				Data.Copy(array[i].Mid(0,remainingBufferSize));
				}
			else
				{
				Data.Copy(array[i]);
				}
			iPhoneId.iModel.Append(KSpaceSeparator);
			iPhoneId.iModel.Append(Data);
			}
		else
			{
			if(array[i].Length() > RMobilePhone::KPhoneModelIdSize)
				{
				iPhoneId.iModel.Copy(array[i].Mid(0,RMobilePhone::KPhoneModelIdSize));
				}
			else
				{
				iPhoneId.iModel.Copy(array[i]);
				}
			iMoreInfoFlag = ETrue;
			}
		}
	iMoreInfoFlag = EFalse;
	CleanupStack::PopAndDestroy();
	iError = KErrNone;
	}
//
// End of file

