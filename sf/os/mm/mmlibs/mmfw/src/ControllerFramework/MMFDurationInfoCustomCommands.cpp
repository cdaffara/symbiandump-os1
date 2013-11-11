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

#include "mmfdurationinfocustomcommands.h"
#include <mmf/common/mmfdurationinfocustomcommands.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfdurationinfocustomcommandsimpl.h>
#include <mmf/common/mmfdurationinfocustomcommandsenums.h>
#include <mmf/common/mmfdrmcustomcommandsenum.h>
#endif

EXPORT_C CMMFDurationInfoCustomCommandParser* CMMFDurationInfoCustomCommandParser::NewL(MMMFDurationInfoCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFDurationInfoCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFDurationInfoCustomCommandParser::~CMMFDurationInfoCustomCommandParser()
	{
	}

CMMFDurationInfoCustomCommandParser::CMMFDurationInfoCustomCommandParser(MMMFDurationInfoCustomCommandImplementor& aImplementor) 
	: CMMFCustomCommandParserBase(KUidInterfaceMMFDurationInfoControl),
	  iImplementor(aImplementor)
	{
	}

void CMMFDurationInfoCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	TInt err = KErrNotSupported;
	
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFDurationInfoControl)
		{
		switch (aMessage.Function())
			{
		case EMMFGetDurationInfo:
			err = GetDurationInfo(aMessage);
			break;
		default:
			err = KErrNotSupported;
			break;
			}
		}

	aMessage.Complete(err);
	}


TInt CMMFDurationInfoCustomCommandParser::GetDurationInfo(TMMFMessage& aMessage)
	{
	TInt retRes = KErrNone;
	TPckgBuf<TMMFDurationInfo> enumPckg;
	iImplementor.GetDurationInfo( enumPckg() );
	aMessage.WriteDataToClient(enumPckg);
	return retRes;
	}

EXPORT_C CMMFDurationInfoCustomCommandParser2* CMMFDurationInfoCustomCommandParser2::NewL(MMMFDurationInfoCustomCommandImplementor2& aImplementor)
	{
	return new(ELeave) CMMFDurationInfoCustomCommandParser2(aImplementor);
	}

EXPORT_C CMMFDurationInfoCustomCommandParser2::~CMMFDurationInfoCustomCommandParser2()
	{
	}

CMMFDurationInfoCustomCommandParser2::CMMFDurationInfoCustomCommandParser2(MMMFDurationInfoCustomCommandImplementor2& aImplementor) 
	: CMMFCustomCommandParserBase(KUidInterfaceMMFDurationInfoControl2),
	  iImplementor(aImplementor)
	{
	}

void CMMFDurationInfoCustomCommandParser2::HandleRequest(TMMFMessage& aMessage)
	{
	TInt err = KErrNone;
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFDurationInfoControl2)
		{
		switch (aMessage.Function())
			{
		case EMMFGetDurationInfo:
			err = GetDurationInfo(aMessage);
			break;
		default:
			err = KErrNotSupported;
			break;
			}
		}
	else
		{
		err = KErrNotSupported;
		}	
	aMessage.Complete(err);
	}

TInt CMMFDurationInfoCustomCommandParser2::GetDurationInfo(TMMFMessage& aMessage)
	{
	TInt err = KErrNone;
	TPckgBuf<TMMFDurationInfo> enumPckg;
	err = iImplementor.GetDurationInfo(enumPckg());
	if(err==KErrNone)
		{
		err = aMessage.WriteDataToClient(enumPckg);
		}
	return err;
	}
	
