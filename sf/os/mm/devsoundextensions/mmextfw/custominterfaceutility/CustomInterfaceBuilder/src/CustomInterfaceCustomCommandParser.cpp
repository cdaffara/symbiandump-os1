/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Custom Interface CustomCommand Parser
*
*/


#include <CustomInterfaceCustomCommandParser.h>
#include <MCustomInterfaceCustomCommandImplementor.h>
#include "CustomInterfaceBuilderTypes.h"


EXPORT_C CCustomInterfaceCustomCommandParser* CCustomInterfaceCustomCommandParser::NewL(MCustomInterfaceCustomCommandImplementor& aImplementor)
	{
	CCustomInterfaceCustomCommandParser* self = new(ELeave) CCustomInterfaceCustomCommandParser(aImplementor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCustomInterfaceCustomCommandParser::CCustomInterfaceCustomCommandParser(MCustomInterfaceCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidCustomInterfaceBuilder),
	iImplementor(aImplementor)
	{
	}

void CCustomInterfaceCustomCommandParser::ConstructL()
	{
	}

CCustomInterfaceCustomCommandParser::~CCustomInterfaceCustomCommandParser()
	{
	}

void CCustomInterfaceCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidCustomInterfaceBuilder);
	TRAPD(error, DoHandleRequestL(aMessage));
	if ( error )
		{
		aMessage.Complete(error);
		}
	}

void CCustomInterfaceCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	// call required function to do the requested operation
	switch( aMessage.Function() )
		{
		case ECibGetBuilder:
			DoGetBuilderL(aMessage);
			break;

		default:
			User::Leave(KErrNotSupported);

		}
	aMessage.Complete(KErrNone);
	}

void CCustomInterfaceCustomCommandParser::DoGetBuilderL(TMMFMessage& aMessage)
	{
	TMMFMessageDestinationPckg handlePckg(iImplementor.GetCustomInterfaceBuilderL());
	TInt error = aMessage.WriteDataToClient(handlePckg);
	User::LeaveIfError(error);
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
