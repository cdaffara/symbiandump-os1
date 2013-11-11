// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ReqstParser.h"
#include "rpsasciirqstdefs.h"
#include "RequestHandler.h"
#include "EtelMngr.h"	
#include "rpscommon.h"
//
// ExtractMsgLenL() (Static)
// Parses the msg header and length fields of the passed in descriptor.
// Returns the value of the msg length.
// 
TInt CReqstParser::ExtractMsgLenL(const TDesC8& aHeaderAndLen)
	{
	// Msg Length
	TUint msgLen(0);
	TLex8 lex(aHeaderAndLen.Right(KMaxMsgLenChars));
	lex.Val(msgLen);
	return msgLen;
	}
	
//
// Checks if the request is a handshake type request, and if so
// returns true and supplies the requested data. Otherwise returns false.
//	
TBool CHandShakeReqstParser::ParseHSReqstL(CRequestHandler& aRqstHndlr, CRpsMsg& aReqstMsg, CDesCArrayFlat* aHSResponseDataArray)
	{
	TBool ret(EFalse);
	
	TPtrC8 opCode8 = aReqstMsg.GetOpCode();
		
	if(!opCode8.Compare(KHShakeReqstSlaveTelNumbers))
		{
		// Return Slave's own tel numbers
		if(aRqstHndlr.EtelMngr())
			{
			aHSResponseDataArray->AppendL(aRqstHndlr.EtelMngr()->OwnNumbers().OwnVoiceNumber());
			aHSResponseDataArray->AppendL(aRqstHndlr.EtelMngr()->OwnNumbers().OwnDataNumber());
			aHSResponseDataArray->AppendL(aRqstHndlr.EtelMngr()->OwnNumbers().OwnFaxNumber());
			}
		ret = ETrue;
		}
		
	return ret;
	
	}
