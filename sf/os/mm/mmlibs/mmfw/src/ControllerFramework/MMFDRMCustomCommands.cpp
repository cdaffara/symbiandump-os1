// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <mmf/common/mmfdrmcustomcommands.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfdrmcustomcommandsenum.h>
#endif

EXPORT_C CMMFDRMCustomCommandParser* CMMFDRMCustomCommandParser::NewL(MMMFDRMCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFDRMCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFDRMCustomCommandParser::~CMMFDRMCustomCommandParser()
	{
	}

CMMFDRMCustomCommandParser::CMMFDRMCustomCommandParser(MMMFDRMCustomCommandImplementor& aImplementor) 
	: CMMFCustomCommandParserBase(KUidInterfaceMMFDRMControl),
	  iImplementor(aImplementor)
	{
	}

void CMMFDRMCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	TRAPD(err, 
			DoHandleRequestL(aMessage);
		);
	aMessage.Complete(err);
	}

void CMMFDRMCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFDRMControl)
		{
		switch (aMessage.Function())
			{
		case EMMFDRMControlEvaluateIntent:
			DoEvaluateIntentL(aMessage);
			break;
			
		case EMMFDRMControlExecuteIntent:
			DoExecuteIntentL(aMessage);
			break;
			
		case EMMFDRMControlDisableAutomaticIntent:
			DoDisableAutomaticIntentL(aMessage);
			break;
			
		case EMMFDRMControlSetAgentProperty:
			DoSetAgentPropertyL(aMessage);
			break;
		case EMMFDRMControlIsSupported:
			// we just complete the message with KErrNone
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
			}
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}

void CMMFDRMCustomCommandParser::DoExecuteIntentL(TMMFMessage& aMessage)
	{
	TPckgBuf<ContentAccess::TIntent> intentPckg;
	aMessage.ReadData1FromClientL(intentPckg);
	User::LeaveIfError(iImplementor.MdcExecuteIntent( intentPckg()));
	}
	
void CMMFDRMCustomCommandParser::DoEvaluateIntentL(TMMFMessage& aMessage)
	{
	TPckgBuf<ContentAccess::TIntent> intentPckg;
	aMessage.ReadData1FromClientL(intentPckg);
	User::LeaveIfError(iImplementor.MdcEvaluateIntent( intentPckg()));
	}
	
void CMMFDRMCustomCommandParser::DoDisableAutomaticIntentL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> boolPckg;
	aMessage.ReadData1FromClientL(boolPckg);
	iImplementor.MdcDisableAutomaticIntent( boolPckg());
	}
	
void CMMFDRMCustomCommandParser::DoSetAgentPropertyL(TMMFMessage& aMessage)
	{
	TPckgBuf<ContentAccess::TAgentProperty> propertyPckg;
	TPckgBuf<TInt> valuePckg;
	aMessage.ReadData1FromClientL(propertyPckg);
	aMessage.ReadData2FromClientL(valuePckg);
	User::LeaveIfError(iImplementor.MdcSetAgentProperty( propertyPckg(), valuePckg()));
	}


EXPORT_C RMMFDRMCustomCommands::RMMFDRMCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFDRMControl)
	{
	}

EXPORT_C TInt RMMFDRMCustomCommands::ExecuteIntent(ContentAccess::TIntent aIntent)
	{
	TPckgBuf<ContentAccess::TIntent> intentPckg(aIntent);
	
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFDRMControlExecuteIntent, 
										 intentPckg,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFDRMCustomCommands::EvaluateIntent(ContentAccess::TIntent aIntent)
	{
	TPckgBuf<ContentAccess::TIntent> intentPckg(aIntent);
	
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFDRMControlEvaluateIntent, 
										 intentPckg,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFDRMCustomCommands::DisableAutomaticIntent(TBool aBool)
	{
	TPckgBuf<TBool> boolPckg(aBool);
	
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFDRMControlDisableAutomaticIntent, 
										 boolPckg,
										 KNullDesC8);
	}

EXPORT_C TInt RMMFDRMCustomCommands::SetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue)
	{
	TPckgBuf<ContentAccess::TAgentProperty> propertyPckg(aProperty);
	TPckgBuf<TInt> valuePckg(aValue);
	
	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFDRMControlSetAgentProperty, 
										 propertyPckg,
										 valuePckg);
	}


EXPORT_C TInt RMMFDRMCustomCommands::IsSupported()
	{
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFDRMControlIsSupported, 
										 KNullDesC8,
										 KNullDesC8);
										 
	return (err == KErrNone)?ETrue : EFalse;
	}
