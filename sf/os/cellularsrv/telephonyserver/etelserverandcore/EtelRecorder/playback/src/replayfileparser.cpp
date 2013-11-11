// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definitions of file parsing related functions.
// 
//

/**
 @file
*/

#include "replayfileparser.h"
#include <f32file.h>
#include "replaylogger.h"
#include "replaymain.h"
#include "replaysession.h"


CReplayFileParser* CReplayFileParser::NewLC(CConsoleBase* aConsole, const TDesC& aName)
	{
	CReplayFileParser* p = new(ELeave) CReplayFileParser(aConsole);
	CleanupStack::PushL(p);
	p->ConstructL(aName);
	return p;
	}
	
CReplayFileParser::CReplayFileParser(CConsoleBase* aConsole)
	: iConsole(aConsole)
	{
	
	}

CReplayFileParser::~CReplayFileParser()
	{
	iFileInMemory.Close();
	}
	
void CReplayFileParser::ConstructL(const TDesC& aName)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
 
 	RFile file;
	User::LeaveIfError(file.Open(fs,aName,EFileStream |EFileRead|EFileShareReadersOnly));
	CleanupClosePushL(file);
	TInt size;	
	User::LeaveIfError(file.Size(size));
	
	//read in contents into RBuf
	iFileInMemory.CreateL(size);
	iFileInMemory.SetLength(iFileInMemory.MaxLength());
	iFileInMemory.FillZ();
				  
	iCursor.Set(iFileInMemory); //set cursor to beginning of file
	
	User::LeaveIfError(file.Read(iFileInMemory)); 
	
	//can now close file and file session
	CleanupStack::PopAndDestroy(2,&fs);
	}

using namespace RecorderByteAlignment;
	
void CReplayFileParser::CheckHeaderValidity()
	{
	//parses the log header and checks it is valid w.r.t to endian and byte-aligment.
	//There should be a two-byte flag of value 0xFFFF
	

	
	const TUint16 delimitercheck = *reinterpret_cast<const TUint16*>(iCursor.Ptr());
	if(delimitercheck == KLogHeaderFlagDelimiter)
		{
		LOGLINE3(_L("Delimiter check OK. (exp=0x%x, act=0x%x)."), KLogHeaderFlagDelimiter, delimitercheck);
		}
	else
		{
		LOGLINE3(_L("Delimiter check FAILED. (exp=0x%x, act=0x%x)."), KLogHeaderFlagDelimiter, delimitercheck);
		ReplaySession::Panic(ReplaySession::EMissingDelimiter);
		}
	ForwardCursor(sizeof(KLogHeaderFlagDelimiter));
		
	//1st bytes are the log header line
	TPtrC8 ptrToMatch(KLogVersionDesStart);
	TPtrC8 ptrToCheck = iCursor.Left(ptrToMatch.Length());
	if(ptrToCheck == ptrToMatch)
		{
		TBuf16<255> buf16;
		buf16.Copy(ptrToCheck);		 //need to copy into 16-bit descriptor for logging to console
		LOGLINE2(_L("Log Version descriptor matched OK (%S)"), &buf16);
		}
	//forward the cursor to next field.
	ForwardCursor(ptrToMatch.Length());
	
	//read in two bytes to determine byte-ordering
	const TUint16 endiancheck = *reinterpret_cast<const TUint16*>(iCursor.Ptr());

	if(endiancheck == KEndianChecksum)
		{
		LOGLINE3(_L("Endian checksum matched OK (exp=0x%x, act=0x%x). Byte-ordering OK."), KEndianChecksum, endiancheck);
		iNeedByteReversing = EFalse;
		}
	else if(endiancheck == KReverseEndianChecksum)
		{
		LOGLINE3(_L("Endian checksum reversed(exp=0x%x, act=0x%x). Byte-ordering when recorded is reverse of current."), KEndianChecksum, endiancheck);
		iNeedByteReversing = ETrue;
		ReplaySession::Panic(ReplaySession::EBadByteOrdering); //TO DO: support reversing of bytes. For now just panic. ADDED LATER: We can't support
		//reversing the bye ordering as we don't know on how on what boundary to reverse the bytes.  E.g. A TUint32 is on a 4-byte boundary
		//but a TIn16 is on 2-byte boundary and since we don't know what the binary data being read from the file represents we can not do the conversion.
		}
	else
		{
		LOGLINE3(_L("Endian checksum failed (exp=0x%x, act=0x%x). Unkown byte ordering."), KEndianChecksum, endiancheck);
		ReplaySession::Panic(ReplaySession::EBadByteOrdering);
		}

	//forward the cursor to next field.
	ForwardCursor(sizeof(KEndianChecksum));
	
	//following sections confirm if byte-alignment will cause problems.  We read in some structures and and check contents.
	//If they don't match with expected then byte-alignment of this core is different to when file was recorded.
	//If byte-alignment check fails we panic since casting to t-classes will fail.
	
	//1st check is for TOneByteAlignmentCheck
	const TOneByteAlignmentCheck* onebytecheck = reinterpret_cast<const TOneByteAlignmentCheck*>(iCursor.Ptr());
	//check contents
	if(onebytecheck->iOneByte == KOneByteVal)
		{
		LOGLINE3(_L("One-byte alignment check OK. (exp=0x%x, act=0x%x)."), KOneByteVal, onebytecheck->iOneByte);
		}
	else
		{
		LOGLINE3(_L("One-byte alignment check FAILED. (exp=0x%x, act=0x%x). Unknown byte-alignment."), KOneByteVal, onebytecheck->iOneByte);
		ReplaySession::Panic(ReplaySession::EBadByteAlignment);		
		}
		
	//forward the cursor to next field - TThreeByteAlignmentCheck.
	ForwardCursor(sizeof(TOneByteAlignmentCheck));
	
	//2nd check is for TThreeByteAlignmentCheck
	const TThreeByteAlignmentCheck* threebytecheck = reinterpret_cast<const TThreeByteAlignmentCheck*>(iCursor.Ptr());
	//check contents
	if(threebytecheck->iTwoBytes == KTwoByteVal && threebytecheck->iOneByte == KOneByteVal)
		{
		LOGLINE5(_L("Three-byte alignment check OK. (exp=0x%x,0x%x, act=0x%x,0x%x)."), KTwoByteVal, KOneByteVal, threebytecheck->iTwoBytes, threebytecheck->iOneByte);
		}
	else
		{
		LOGLINE5(_L("Three-byte alignment check FAILED. (exp=0x%x,0x%x, act=0x%x,0x%x).Unknown byte-alignment."), KTwoByteVal, KOneByteVal, threebytecheck->iTwoBytes, threebytecheck->iOneByte);
		ReplaySession::Panic(ReplaySession::EBadByteAlignment);
		}
		
	//forward the cursor to next field - TThreeByteAlignmentCheck.
	ForwardCursor(sizeof(TThreeByteAlignmentCheck));
	
	//3rd check is for TFiveByteAlignmentCheck
	const TFiveByteAlignmentCheck* fivebytecheck = reinterpret_cast<const TFiveByteAlignmentCheck*>(iCursor.Ptr());
	//check contents
	if(fivebytecheck->iTwoBytes == KTwoByteVal && fivebytecheck->iTwoBytes2 == KTwoByteVal && fivebytecheck->iOneByte == KOneByteVal)
		{
		LOGLINE7(_L("Five-byte alignment check OK. (exp=0x%x,0x%x,0x%x, act=0x%x,0x%x,0x%x)."), KTwoByteVal, KTwoByteVal, KOneByteVal, fivebytecheck->iTwoBytes, fivebytecheck->iTwoBytes2, fivebytecheck->iOneByte);
		}
	else
		{
		LOGLINE7(_L("Five-byte alignment check FAILED. (exp=0x%x,0x%x,0x%x, act=0x%x,0x%x,0x%x). Unknown byte-alignment."), KTwoByteVal, KTwoByteVal, KOneByteVal, fivebytecheck->iTwoBytes, fivebytecheck->iTwoBytes2, fivebytecheck->iOneByte);
		ReplaySession::Panic(ReplaySession::EBadByteAlignment);
		}
	
	ForwardCursor(sizeof(TFiveByteAlignmentCheck));
	
	//final part is a 2byte flag indicating end of header section

	const TUint16 endflag = *reinterpret_cast<const TUint16*>(iCursor.Ptr());
	
	if(endflag == KLogHeaderFlagDelimiter)
		{
		LOGLINE3(_L("End flag for Header matched OK (exp=0x%x, act=0x%x)."), KLogHeaderFlagDelimiter, endflag);
		}
	else
		{
		LOGLINE3(_L("End flag for Header FAILED. (exp=0x%x, act=0x%x)."), KLogHeaderFlagDelimiter, endflag);
		ReplaySession::Panic(ReplaySession::EMissingDelimiter);
		}
		
	ForwardCursor(sizeof(KLogHeaderFlagDelimiter));		

	return;
	}
	
void CReplayFileParser::ForwardCursor(TInt aNoBytesToForward)
	{
	if(aNoBytesToForward > iCursor.Length())
		{
		ReplaySession::Panic(ReplaySession::EBufferOverflow);
		}		
	
	iCursor.Set(iCursor.Mid(aNoBytesToForward));
	}
	
CRecordMessage* CReplayFileParser::GetNextMessageL()
	{
	CRecordMessage* message = CRecordMessage::NewLC();
	TInt msgsize(0);
	TInt ret = message->InternalizeL(iCursor, msgsize);
	
	if(ret ==KErrEof)
		{
		//we have found a new log session
		LOGLINE1(_L("Found new log session - checking header validity."));
		CleanupStack::PopAndDestroy(message);
		message = NULL;
		CheckHeaderValidity();
		message = GetNextMessageL(); //should be passed header so now read a message
		message->SetAsFirstMessage(); //this is the first message from the new log session.  We need to subtract UTC offsets
		return message;
		}
	else if(ret != KErrNone)
		{
		LOGLINE1(_L("No more messages to add to the model."));
		CleanupStack::PopAndDestroy(message);
		return NULL;		
		}
	
	CleanupStack::Pop(message); //message ownership is going to be transferred
	ForwardCursor(msgsize);		
	return message;
	}
