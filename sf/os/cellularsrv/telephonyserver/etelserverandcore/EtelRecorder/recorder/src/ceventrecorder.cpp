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

#include "et_record.h"

#ifdef ETEL_RECORDER

#include "ceventrecorder.h"
#include "crecordmessage.h"

_LIT8(KRecorderComponent, "recorder");
CEventRecorder* CEventRecorder::NewL(const TDesC8& aSubsystem)
	{
	CEventRecorder* me = new(ELeave)CEventRecorder;
	CleanupStack::PushL(me);
	me->ConstructL(aSubsystem);
	CleanupStack::Pop(me);
	return me;
	}
	

void CEventRecorder::ConstructL(const TDesC8& aSubsystem)
	{
	//due to no static method in Flogger for logging binary we need to open a Flogger connection to use.
	__FLOG_OPEN(aSubsystem,KRecorderComponent); 
	KRecorderComponent();
//	DumpLogHeader();
	}
	
CEventRecorder::~CEventRecorder()
	{
	//this may not get called due to un-graceful close down of Etel server in most situations.
	__FLOG_CLOSE;
	}
	
void CEventRecorder::Dump(const TDesC8& aDataToDump)
	{
	__FLOG_BINARY(aDataToDump);
	}

using namespace RecorderByteAlignment;

void CEventRecorder::DumpLogHeader()
	{
	//end log header flag
	TPtrC8 ptr;
	ptr.Set(const_cast<TUint8*>(reinterpret_cast<const TUint8*>(&KLogHeaderFlagDelimiter)), 2);
	__FLOG_BINARY(ptr);	//this is needed to allow us to check for multiple log sessions in one log file.  This is likely since
	//commsdbgutils appends to the log file
	
	//print out entry line
	__FLOG_BINARY(KLogVersionDesStart);
	//print out checksum used to dtermine endian of the core
	ptr.Set(reinterpret_cast<const TUint8*>(&KEndianChecksum), 2);
	__FLOG_BINARY(ptr); //intel is little-endian, so is arm i think!
	
	//print out a few structures to check byte alignment
	//this is in an attempt to work out if alignment is going to affect the logging of the data.
	//If alignment check fails then chances are the reader of the bin file will read in wrong length of data.
	//This may happen if core architecures from recorder and reader are different.
	
	//set up structure values
	TOneByteAlignmentCheck onebyte;
	onebyte.iOneByte = KOneByteVal;
	ptr.Set(const_cast<TUint8*>(reinterpret_cast<const TUint8*>(&onebyte)), sizeof(TOneByteAlignmentCheck));
	__FLOG_BINARY(ptr);
	
	TThreeByteAlignmentCheck threebyte;	
	threebyte.iTwoBytes = KTwoByteVal;
	threebyte.iOneByte = KOneByteVal;
	ptr.Set(const_cast<TUint8*>(reinterpret_cast<const TUint8*>(&threebyte)), sizeof(TThreeByteAlignmentCheck));
	__FLOG_BINARY(ptr);
	
	TFiveByteAlignmentCheck fivebyte;
	fivebyte.iTwoBytes = KTwoByteVal;
	fivebyte.iTwoBytes2 = KTwoByteVal;
	fivebyte.iOneByte = KOneByteVal;
	ptr.Set(const_cast<TUint8*>(reinterpret_cast<const TUint8*>(&fivebyte)), sizeof(TFiveByteAlignmentCheck));
	__FLOG_BINARY(ptr);
	
	//end log header flag
	ptr.Set(const_cast<TUint8*>(reinterpret_cast<const TUint8*>(&KLogHeaderFlagDelimiter)), 2);
	__FLOG_BINARY(ptr);	
	}
#endif // ETEL_RECORDER
