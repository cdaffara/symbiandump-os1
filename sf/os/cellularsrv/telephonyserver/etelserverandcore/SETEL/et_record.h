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
// Declaration of global IPC recording functions and macros.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __ET_RECORD_H__
#define __ET_RECORD_H__

// enable/disable EtelRecorder in UDEB/UREL builds depending on which macros are defined in the MMP file
#ifdef ETEL_RECORDER_ON_UDEB
	#ifdef _DEBUG
		#ifndef ETEL_RECORDER
			#define ETEL_RECORDER	
		#endif
	#endif
#endif

#ifdef ETEL_RECORDER_ON_UREL
	#ifndef _DEBUG
		#ifndef ETEL_RECORDER
			#define ETEL_RECORDER
		#endif
	#endif
#endif

#ifdef ETEL_RECORDER

  	#include "ET_SSTD.H"
  	#include "crecordmessage.h"
  
	struct TSessionOpenArgs
		{
		TName iArg0; //arg0 is an in parsmrter of type TDesC
		TInt iArg1Size; //arg1 is an out parameter of type TDes, so we don't car for its contents we just want its size
		TInt iArg2; //arg2 is a subsession handle
		TInt iIpc;
		};

	GLREF_C void CaptureArgs(const RMessage2& aMessage, TSessionOpenArgs& aOpenArgs, const TName& aArg0);

	GLREF_C void RecordIpc(const RMessage2& aMessage, const CTelSession* aSession, const CTelObject* aTelObject, TBool aFromFlowControlResume);
	GLREF_C void doRecordIpcL(const RMessage2& aMessage, const CTelSession* aSession, const CTelObject* aTelObject, TBool aFromFlowControlResume);
	template<class T0>
	GLREF_C void ReadIntoBufferL(const RMessage2& aMessage, T0& aBuf, TInt aArgSize, TInt aArgIdx);


	#define	RECORD_IPC(A, B, C, D) \
		RecordIpc(A, B, C, D);

	#define RECORD_IPC_LOG2(A, B) \
		CRecordMessage::LogIpc(A, B);

	#define RECORD_IPC_LOG3(A, B, C) \
		CRecordMessage::LogIpc(A, B, C);

	#define RECORD_IPC_LOG4(A, B, C, D) \
		CRecordMessage::LogIpc(A, B, C, D);
			
	#define RECORD_IPC_LOG5(A, B, C, D, E) \
		CRecordMessage::LogIpc(A, B, C, D, E);

	#define RECORD_IPC_LOG6(A, B, C, D, E, F) \
		CRecordMessage::LogIpc(A, B, C, D, E, F);

	#define RECORD_IPC_LOG7(A, B, C, D, E, F, G) \
		CRecordMessage::LogIpc(A, B, C, D, E, F, G);

	#define RECORD_COMPLETE(A, B, C)	\
		CRecordMessage::LogMessageComplete(A, B, C);

	#define RECORD_COMPLETE_SUB(A, B, C, D, E)	\
		CRecordMessage::LogMessageComplete(A, B, C, D, E);							

	#define RECORD_HEADER \
		CRecordMessage::LogRecordHeader();
		
	#define RECORD_SESSION_NEW(A) \
		CRecordMessage::LogNewSession(A);
		
	#define RECORD_SESSION_CLOSE(A) \
		CRecordMessage::LogCloseSession(A);
		
	#define RECORD_SUBSESSION_NEW(A, B, C, D, E, F, G) \
		CRecordMessage::LogNewSubSession(A, B, C, D, E, F, G);
		
	#define RECORD_SUBSESSION_CLOSE(A, B, C, D) \
		CRecordMessage::LogCloseSubSession(A, B, C, D);

#else

	#define	RECORD_IPC(A, B, C, D)
	#define RECORD_IPC_LOG2(A, B)
	#define RECORD_IPC_LOG3(A, B, C)
	#define RECORD_IPC_LOG4(A, B, C, D)
	#define RECORD_IPC_LOG5(A, B, C, D, E)
	#define RECORD_IPC_LOG6(A, B, C, D, E, F)
	#define RECORD_IPC_LOG7(A, B, C, D, E, F, G)
	#define RECORD_COMPLETE(A, B, C)
	#define RECORD_COMPLETE_SUB(A, B, C, D, E)	
	#define RECORD_HEADER
	#define RECORD_SESSION_NEW(A)
	#define RECORD_SESSION_CLOSE(A)
	#define RECORD_SUBSESSION_NEW(A, B, C, D, E, F, G)
	#define RECORD_SUBSESSION_CLOSE(A, B, C, D)

#endif // ETEL_RECORDER


#endif // __ET_RECORD_H__
