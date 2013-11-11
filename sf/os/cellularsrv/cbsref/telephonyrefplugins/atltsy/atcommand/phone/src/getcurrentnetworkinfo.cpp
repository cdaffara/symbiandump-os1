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
// CATGetModemStatus
// Description:
// CGetCurrentNetworkInfo

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include "getcurrentnetworkinfo.h"
#include "mslogger.h"
#include "atgetnetworkinfo.h"

// define the AT command format
_LIT8(KATSetFormatForLongName,  "AT+COPS=3,0\r");
_LIT8(KATSetFormatForShortName, "AT+COPS=3,1\r");
_LIT8(KATSetNumbericFormat,     "AT+COPS=3,2\r");

// Class CGetCurrentNetworkInfo
// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetCurrentNetworkInfo* CGetCurrentNetworkInfo::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CGetCurrentNetworkInfo* self = new(ELeave) CGetCurrentNetworkInfo(aGloblePhone,
			                                              aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop();
	return self ;
	}

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::CGetCurrentNetworkInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetCurrentNetworkInfo::CGetCurrentNetworkInfo(CGlobalPhonemanager& aGloblePhone, 
	                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CATCommandControlBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy Phone Control] Starting CATAnswer::ConstructL"));
	iState = EATUnknown;
	iATNetworkInfo = CATNetworkInfo::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	iATHelperCommand = CATHelperCommand::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	}

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::~CGetCurrentNetworkInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetCurrentNetworkInfo::~CGetCurrentNetworkInfo()
	{
	LOGTEXT(_L8("[Ltsy Phone Control] Starting CATAnswer::~CGetCurrentNetworkInfo()"));
	delete iATHelperCommand;
	iATHelperCommand = NULL;
	delete iATNetworkInfo;
	iATNetworkInfo = NULL;
	}

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::AtCommandExecuteComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus)
	{
	if((aEventSource == EWriteCompletion)&&(KErrNone == aStatus))
		{
		return;
		}
	
	switch(iState)
		{
		case EATSetLongFormat:
		    LOGTEXT(_L8("[Ltsy Phone Control] complete EATSetLongFormat"));
		    CompeteATCommand(iATHelperCommand);
		    if(aStatus != KErrNone)
		        {
		        HandleGetCurNetworkError(aStatus);
		        }
		    else 
		        {
		        aStatus = iATHelperCommand->GetErrorValue();
		        if(aStatus != KErrNone)
		            {
		            HandleGetCurNetworkError(aStatus);
		            }
		        else
		            {
		            iState = EATGetLongName;
		            GetNetworkLongName();
		            }
		        }
			break;
			
		case EATGetLongName:
		    LOGTEXT(_L8("[Ltsy Phone Control] complete EATGetLongName"));
		    CompeteATCommand(iATNetworkInfo);
		    if(aStatus != KErrNone)
		        {
		        HandleGetCurNetworkError(aStatus);
		        }
		    else
				{
				TInt err = iATNetworkInfo->GetErrorValue();
				if(err != KErrNone)
				    {
				    HandleGetCurNetworkError(err);
				    }
				else
				    {
				    iState = EATSetShortFormat;
				    SetShortNameFormat();
				    }
				}
			break;
			
		case EATSetShortFormat:
		    LOGTEXT(_L8("[Ltsy Phone Control] complete EATSetShortFormat"));
		    CompeteATCommand(iATHelperCommand);
            if(aStatus != KErrNone)
                {
                HandleGetCurNetworkError(aStatus);
                }
            else 
                {
                aStatus = iATHelperCommand->GetErrorValue();
                if(aStatus != KErrNone)
                    {
                    HandleGetCurNetworkError(aStatus);
                    }
                else
                    {
                    iState = EATGetShortName;
                    GetNetworkShortName();
                    }
                }		    
		    break;
		    
		case EATGetShortName:
		    LOGTEXT(_L8("[Ltsy Phone Control] complete EATGetShortName"));
            CompeteATCommand(iATNetworkInfo);
            if(aStatus != KErrNone)
                {
                HandleGetCurNetworkError(aStatus);
                }
            else
                {
                TInt err = iATNetworkInfo->GetErrorValue();
                if(err != KErrNone)
                    {
                    HandleGetCurNetworkError(err);
                    }
                else
                    {
                    iState = EATSetNumbericFormat;
                    SetNumbericFormat();
                    }
                }
		    break;
		case EATSetNumbericFormat:
		    LOGTEXT(_L8("[Ltsy Phone Control] complete EATSetNumbericFormat"));
            CompeteATCommand(iATHelperCommand);
            if(aStatus != KErrNone)
                {
                HandleGetCurNetworkError(aStatus);
                }
            else 
                {
                aStatus = iATHelperCommand->GetErrorValue();
                if(aStatus != KErrNone)
                    {
                    HandleGetCurNetworkError(aStatus);
                    }
                else
                    {
                    iState = EATGetCountryCodeAndNetwrokId;
                    GetCountryCodeAndNetworkId();
                    }
                }           
            break;
        case EATGetCountryCodeAndNetwrokId:
            LOGTEXT(_L8("[Ltsy Phone Control] complete EATGetCountryCodeAndNetwrokId"));
            CompeteATCommand(iATNetworkInfo);
            if(aStatus != KErrNone)
                {
                HandleGetCurNetworkError(aStatus);
                }
            else
                {
                TInt err = iATNetworkInfo->GetErrorValue();
                if(err != KErrNone)
                    {
                    HandleGetCurNetworkError(err);
                    }
                else
                    {
                    iState = EATUnknown;
                    HandleGetCurNetworkComplete();
                    }
                }
            break;
            
		default:
			break;
		}
	}

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::StartRequest()
	{
	LOGTEXT(_L8("[Ltsy Phone Control] Starting CATAnswer::StartRequest()"));
	iState = EATSetLongFormat;
	SetLongNameFormat();
	}

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::StartATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::StartATCommand(CAtCommandBase* aAtCommand)
	{
	aAtCommand->SetAtCommandObserver(this);
	aAtCommand->StartRequest();
	}

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::EndATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::CompeteATCommand(CAtCommandBase* aAtCommand)
	{
	aAtCommand->RemoveAtCommandObserver();
	aAtCommand->Complete();
	}

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::SetLongNameFormat
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::SetLongNameFormat()
    {
    iATHelperCommand->SetAtCommandString(KATSetFormatForLongName);
    StartATCommand(iATHelperCommand);
    }

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::SetLongNameFormat
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::GetNetworkLongName()
    {
    StartATCommand(iATNetworkInfo);
    }

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::SetLongNameFormat
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::SetShortNameFormat()
    {
    iATHelperCommand->SetAtCommandString(KATSetFormatForShortName);
    StartATCommand(iATHelperCommand);
    }

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::GetNetworkShortName
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::GetNetworkShortName()
    {
    StartATCommand(iATNetworkInfo);
    }

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::SetShortNumbericFormat
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::SetNumbericFormat()
    {
    iATHelperCommand->SetAtCommandString(KATSetNumbericFormat);
    StartATCommand(iATHelperCommand);
    }

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::SetShortNumbericFormat
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::GetCountryCodeAndNetworkId()
    {
    StartATCommand(iATNetworkInfo);
    }

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::HandleGetCurNetworkError
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::HandleGetCurNetworkError(TInt aError)
    {
    iCtsyDispatcherCallback.CallbackPhoneGetCurrentNetworkInfoComp(aError,
                                    iNetworkInfo, 
                                    iPhoneGlobals.iPhoneStatus.iLocationArea);
    }

// ---------------------------------------------------------------------------
// CGetCurrentNetworkInfo::HandleGetCurNetworkComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetCurrentNetworkInfo::HandleGetCurNetworkComplete()
    {
    LOGTEXT(_L8("[Ltsy Phone Control] CGetCurrentNetworkInfo::HandleGetCurNetworkComplete()"));
    iNetworkInfo = iATNetworkInfo->GetNetWorkInfo();
    iPhoneGlobals.iPhoneStatus.iCurrentNetwork = iNetworkInfo;
    iPhoneGlobals.iEventSignalActive = EFalse;
    iCtsyDispatcherCallback.CallbackPhoneGetCurrentNetworkInfoComp(KErrNone,
                            iNetworkInfo, 
                            iPhoneGlobals.iPhoneStatus.iLocationArea);
    
    if(iIpcCompleteObserver)
        {
        iIpcCompleteObserver->NotifyIpcRequestComplete(MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId);
        }
    }

// End of file
