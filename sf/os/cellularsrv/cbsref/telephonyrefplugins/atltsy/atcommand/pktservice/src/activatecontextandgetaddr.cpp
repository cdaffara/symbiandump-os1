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
// CActivatContextAndGetAddr
// 

#include <ctsy/ltsy/ltsylogger.h>

#include "activatecontextandgetaddr.h"
#include "mslogger.h"
#include "athelpercommand.h"
#include "atgprscontextactivate.h"
#include "atgprsgetcontextaddr.h"
#include "atgprscontextconnect.h"
#include "commengine.h"

_LIT8(KATDeleteContext2, "AT+CGDCONT=2\r");
_LIT8(KATDeleteContext3, "AT+CGDCONT=3\r");
_LIT8(KATDeleteContext4, "AT+CGDCONT=4\r");

// CActivatContextAndGetAddr
// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::NewL()
// other items were commented in a header
// ---------------------------------------------------------------------------
CActivatContextAndGetAddr* CActivatContextAndGetAddr::NewL(CGlobalPhonemanager& aGloblePhone, 
	                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CActivatContextAndGetAddr* self = new(ELeave) CActivatContextAndGetAddr(aGloblePhone,
			                                      aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self);
	return self ;
	}

// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::CActivatContextAndGetAddr()
// other items were commented in a header
// ---------------------------------------------------------------------------
CActivatContextAndGetAddr::CActivatContextAndGetAddr(CGlobalPhonemanager& aGloblePhone, 
	                        CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CATCommandControlBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::CActivatContextAndGetAddr()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CActivatContextAndGetAddr::ConstructL()
	{
	iATCommandName = EATNone;
	iATHelperCommand = CATHelperCommand::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	
	iATGprsContextActivate = CATGprsContextActivate::NewL(iPhoneGlobals,
			                                              iCtsyDispatcherCallback);
	iATGPRSGetContextAddr = CATGPRSGetContextAddr::NewL(iPhoneGlobals,
								                        iCtsyDispatcherCallback);
	iATGprsContextConnect = CATGprsContextConnect::NewL(iPhoneGlobals,
								                        iCtsyDispatcherCallback);
	}

// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::~CActivatContextAndGetAddr()
// other items were commented in a header
// ---------------------------------------------------------------------------
CActivatContextAndGetAddr::~CActivatContextAndGetAddr()
	{
	delete iATHelperCommand;
	iATHelperCommand = NULL;
	
	delete iATGprsContextActivate;
	iATGprsContextActivate = NULL;
	
	delete iATGPRSGetContextAddr;
	iATGPRSGetContextAddr = NULL;
	
	delete iATGprsContextConnect;
	iATGprsContextConnect = NULL;
	}

// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::AtCommandExecuteComplete()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CActivatContextAndGetAddr::AtCommandExecuteComplete(TAtEventSource aEventSource, 
		                                                 TInt aStatus)
	{
	if((aEventSource == EWriteCompletion)&&(KErrNone == aStatus))
		{
		return;
		}
	
	TInt ret = KErrNone;
	switch(iATType)
		{
		case ELtsyAT_Packet_DeleteContext2:
            ret = iATHelperCommand->GetErrorValue();
		    if ((KErrNone == ret) && (KErrNone == aStatus))
		        {
		        ExecuteATHelpCommand(KATDeleteContext3,ELtsyAT_Packet_DeleteContext3);
		        }
		    else
		        {
	            EndATCommand(iATHelperCommand);
                iPhoneGlobals.iEventSignalActive = EFalse;
                LOGTEXT3(_L8("[Ltsy] DeleteContext2 failed (%d or %d) in CActivatContextAndGetAddr"), ret, aStatus);
                DispatcherCallback(KErrGeneral);
		        }
			break;
		case ELtsyAT_Packet_DeleteContext3:
            ret = iATHelperCommand->GetErrorValue();
            if ((KErrNone == ret) && (KErrNone == aStatus))
                {
                ExecuteATHelpCommand(KATDeleteContext4,ELtsyAT_Packet_DeleteContext4);
                }
            else
                {
                EndATCommand(iATHelperCommand);
                iPhoneGlobals.iEventSignalActive = EFalse;
                LOGTEXT3(_L8("[Ltsy] DeleteContext3 failed (%d or %d) in CActivatContextAndGetAddr"), ret, aStatus);
                DispatcherCallback(KErrGeneral);
                }
			break;
		case ELtsyAT_Packet_DeleteContext4:
            ret = iATHelperCommand->GetErrorValue();
            EndATCommand(iATHelperCommand);
            if ((KErrNone == ret) && (KErrNone == aStatus))
                {
                iATGprsContextActivate->SetContext(iContextId);
                StartATCommand(iATGprsContextActivate);
                }
            else
                {
                iPhoneGlobals.iEventSignalActive = EFalse;
                LOGTEXT3(_L8("[Ltsy] DeleteContext4 failed (%d or %d) in CActivatContextAndGetAddr"), ret, aStatus);
                DispatcherCallback(KErrGeneral);
                }
			break;
		case ELtsyAT_Packet_ActivateContext:
			EndATCommand(iATGprsContextActivate);
			if(aStatus == KErrNone)
				{
				ret = iATGprsContextActivate->GetErrorValue();
				if(KErrNone == ret)
					{
					iATGPRSGetContextAddr->SetContext(iContextId);
					StartATCommand(iATGPRSGetContextAddr);
					}
				else
				    {
				    iPhoneGlobals.iEventSignalActive = EFalse;
				    DispatcherCallback(ret);
				    }
				}
			else
				{
				iPhoneGlobals.iEventSignalActive = EFalse;
				DispatcherCallback(aStatus);
				}
			break;
		case ELtsyAT_Packet_GetContextAddr: 
			EndATCommand(iATGPRSGetContextAddr);
			if(aStatus == KErrNone)
				{
				ret = iATGPRSGetContextAddr->GetErrorValue();
				if(KErrNone == ret)
					{
					iATGprsContextConnect->SetContext(iContextId);
					StartATCommand(iATGprsContextConnect);
					}
				else
				    {
                    iPhoneGlobals.iEventSignalActive = EFalse;
                    DispatcherCallback(ret);
				    }
				}
			else
				{
                iPhoneGlobals.iEventSignalActive = EFalse;
				DispatcherCallback(aStatus);
				}
			break;
		case ELtsyAT_Packet_ConnectContext: 
			EndATCommand(iATGprsContextConnect);
			if(aStatus == KErrNone)
				{
				ret = iATGprsContextConnect->GetErrorValue();
				if(KErrNone == aStatus)
					{
					iConnectionSpeed = iATGprsContextConnect->GetConnectionSpeed();
					}
				DispatcherCallback(ret);
				}
			else
				{
				DispatcherCallback(aStatus);
				}
            iPhoneGlobals.iEventSignalActive = EFalse;
			break;
		default:
			break;
		}
	}

// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CActivatContextAndGetAddr::StartRequest()
	{
	iATHelperCommand->SetAtCommandObserver(this);
	ExecuteATHelpCommand(KATDeleteContext2,ELtsyAT_Packet_DeleteContext2);
	}

// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::SetContext()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CActivatContextAndGetAddr::SetContext(const TInt aContextId)
	{
	iContextId = aContextId;
	}

// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::ExecuteNextATHelpCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CActivatContextAndGetAddr::ExecuteATHelpCommand(const TDesC8& aATString,TLtsyATCommandType aATType)
	{
	iATHelperCommand->Complete();
	iATHelperCommand->SetAtCommandString(aATString);
	iATType = aATType;
	iATHelperCommand->StartRequest();
	}

// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::StartATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CActivatContextAndGetAddr::StartATCommand(CAtCommandBase* aAtCommand)
	{
	aAtCommand->SetAtCommandObserver(this);
	iATType = aAtCommand->AtType();
	aAtCommand->StartRequest();
	}

// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::EndATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CActivatContextAndGetAddr::EndATCommand(CAtCommandBase* aAtCommand)
	{
	aAtCommand->RemoveAtCommandObserver();
	aAtCommand->Complete();
	}

// ---------------------------------------------------------------------------
// CActivatContextAndGetAddr::DispatcherCallback()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CActivatContextAndGetAddr::DispatcherCallback(TInt aError)
	{
	TInt count = iPhoneGlobals.iContextList.Count();
	TInt i = 0;
	
	//Get the context from context list
	for(i = 0; i < count;i++)
		{
		if(iPhoneGlobals.iContextList[i]->iContextID == iContextId)
			{
			break;
			}
		}
	iCtsyDispatcherCallback.CallbackPacketServicesActivatePdpContextComp(aError, 
			                iPhoneGlobals.iContextList[i]->iInfo.iName,
			                iPhoneGlobals.iContextList[i]->iDefContextConfigGPRS.iAccessPointName,
			                iPhoneGlobals.iContextList[i]->iDefContextConfigGPRS.iPdpType);
	if(aError == KErrNone)
		{
		iPhoneGlobals.iContextList[i]->iInfo.iStatus = RPacketContext::EStatusActive;
		TContextMisc contextMisc;
		contextMisc.iStatus = RPacketContext::EStatusActive;
		iPhoneGlobals.iPhoneStatus.iMode = RPhone::EModeOnlineData;
		iCtsyDispatcherCallback.CallbackPacketServicesNotifyPdpContextStatusChangeInd(aError,
								iPhoneGlobals.iContextList[i]->iInfo.iName,
								contextMisc);
		iCtsyDispatcherCallback.CallbackPacketServicesNotifyPdpContextConfigChangedInd(aError, 
				                iPhoneGlobals.iContextList[i]->iInfo.iName, 
				                iPhoneGlobals.iContextList[i]->iDefContextConfigGPRS.iAccessPointName, 
				                iPhoneGlobals.iContextList[i]->iDefContextConfigGPRS.iPdpAddress, 
				                0, 
				                iPhoneGlobals.iContextList[i]->iDefContextConfigGPRS.iProtocolConfigOption.iMiscBuffer);
		iCtsyDispatcherCallback.CallbackPacketServicesNotifyPdpContextConnectionSpeedChangeInd(aError,
				                iPhoneGlobals.iContextList[i]->iInfo.iName,
				                iConnectionSpeed);
		iPhoneGlobals.iCommEngine->CommCancel();
		}
	}

//
// End of file
