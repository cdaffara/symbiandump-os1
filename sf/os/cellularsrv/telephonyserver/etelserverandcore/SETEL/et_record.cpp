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
// Definitions of global IPC recording functions used by ETel.
// These functions will only be compiled when ETEL_RECORDER is defined.
// 
//

/**
 @file
 @internalAll
*/

#include "et_record.h"

#ifdef ETEL_RECORDER

/**
Given an IPC message, populates a TSessionOpenArgs data structure with the IPC number and arguments.

@param aMessage  The IPC message.
@param aOpenArgs  The structure to populate with message data.
@param aArg0  The first parameter in the IPC message (must be read before calling the function).
*/
GLDEF_C void CaptureArgs(const RMessage2& aMessage, TSessionOpenArgs& aOpenArgs, const TName& aArg0)
	{
	//set ipc
	aOpenArgs.iIpc = aMessage.Function();		
	//set arg0
	aOpenArgs.iArg0 = aArg0;	
	//set arg1 size - this is always an out parameter of type TDes - so we just need to get its max size
	aOpenArgs.iArg1Size = aMessage.GetDesMaxLength(1);
	//arg2 is a subsession handle - or not used depending on the ipc
	aOpenArgs.iArg2 = aMessage.Int2();	
	}

/**
Records an IPC request and the data associated with it. Only calls and traps doRecordIpcL.

@param aMessage  The client request message.
@param aSession  The server side session object.
@param aTelObject  The server side telephony object.
@param aFromFlowControlResume  Flag indicating whether the IPC should be recorded or not. This depends on where the function is being called from. EFalse by default. ETrue only if it is called from CTelObject::FlowControlResume()
@see doRecordIpcL
*/
GLDEF_C void RecordIpc(const RMessage2& aMessage, const CTelSession* aSession, const CTelObject* aTelObject, TBool aFromFlowControlResume)
	{
	TRAP_IGNORE(doRecordIpcL(aMessage,aSession,aTelObject,aFromFlowControlResume));
	}
	
/**
Reads a parameter in a client request message into a buffer.

@param aMessage  The client request message.
@param aBuf  The buffer to populate with requested argument.
@param aArgSize  The size of the argument to be read into the buffer.
@param aArgIdx  The index of the argument to be read into the buffer.
*/
template<class T0>
GLDEF_C void ReadIntoBufferL(const RMessage2& aMessage, T0& aBuf, TInt aArgSize, TInt aArgIdx)
	{
	if (aArgSize<=0)
		{
		aBuf.CreateL(0);
		aBuf.SetLength(0);		
		}
	else
		{			
		aBuf.CreateL(aArgSize);
		aMessage.ReadL(aArgIdx, aBuf);
		aBuf.SetLength(aArgSize);
		}
	}

/**
Records an IPC request and the data associated with it.

@param aMessage  The client request message.
@param aSession  The server side session object.
@param aTelObject  The server side telephony object.
@param aFromFlowControlResume  @param aFromFlowControlResume  Flag indicating whether the IPC should be recorded or not. This depends on where the function is being called from. EFalse by default. ETrue only if it is called from CTelObject::FlowControlResume()
*/
GLDEF_C void doRecordIpcL(const RMessage2& aMessage, const CTelSession* aSession, const CTelObject* aTelObject, TBool aFromFlowControlResume)
	{	
	if(aFromFlowControlResume)
		{
		//this ipc has been triggered from the FlowControlResume which means it is a duplicate and we have already logged it
		//so do not log again
		return;
		}
	TInt arg1=aMessage.Int1();
	TInt ipc=aMessage.Function();
	TInt handle = aMessage.Int3();
	TInt basicMsgType = arg1&(~KPriorityClientReq);
	
	//need to know the size of the data to read.
	//if we're setting data, get length
	// if we're reading (getting/writing back) data, get max length
	
	TInt sizeArg0=0;
	TInt sizeArg2=0;
	switch(basicMsgType)
		{
	case EIsaNull:
	case EIsaCancelSubSession: //same NULL - only arg1 used
		RECORD_IPC_LOG5(aSession, aTelObject, handle, ipc, arg1);
	break;
	case EIsaDesTobeSet:
	case EIsaDesTobeRead:
		{
		if(basicMsgType==EIsaDesTobeSet)
			{
			sizeArg0=aMessage.GetDesLength(0); // set data
			}
		else //EIsaDesTobeRead
			{
			sizeArg0=aMessage.GetDesMaxLength(0); // get data	
			}		
		//read the arg 0 data from the message
		RBuf8 bufArg0;
		CleanupClosePushL(bufArg0);
		ReadIntoBufferL(aMessage, bufArg0, sizeArg0, 0);
		//log out
		RECORD_IPC_LOG6(aSession, aTelObject, handle, ipc, arg1, bufArg0);
		//cleanup
		CleanupStack::PopAndDestroy(&bufArg0);
		}	
	break;
	case EIsaDoubleDesTobeSet:
	case EIsaDoubleDesTobeRead:
	case EIsaNarrowDesToSetAndGet:
		{
		if(basicMsgType==EIsaDoubleDesTobeSet)
			{
			sizeArg0=aMessage.GetDesLength(0); // set data
			sizeArg2=aMessage.GetDesLength(2);	 // set data
			}
		else if(basicMsgType==EIsaDoubleDesTobeRead)
			{
			sizeArg0=aMessage.GetDesMaxLength(0); // get data
			sizeArg2=aMessage.GetDesMaxLength(2); // get data
			}
		else //EIsaNarrowDesToSetAndGet
			{
			sizeArg0=aMessage.GetDesLength(0);    // set data
			sizeArg2=aMessage.GetDesMaxLength(2); // get data
			}	
		//read the arg 0 data from the message
		RBuf8 bufArg0;
		CleanupClosePushL(bufArg0);
		ReadIntoBufferL(aMessage, bufArg0, sizeArg0, 0);
		//read the arg 2 data from the message
		RBuf8 bufArg2;
		CleanupClosePushL(bufArg2);
		ReadIntoBufferL(aMessage, bufArg2, sizeArg2, 2);
		//log out
		RECORD_IPC_LOG7(aSession, aTelObject, handle, ipc, arg1, bufArg0, bufArg2);
		//cleanup
		CleanupStack::PopAndDestroy(2,&bufArg0); // bufArg2, bufArg0
		}
	break;
	case EIsaCancelMessage:
		{
		//arg0 contains ipc to cancel
		TInt ipcToCancel = aMessage.Int0();	
		//log out
		RECORD_IPC_LOG6(aSession, aTelObject, handle, ipc, arg1, ipcToCancel);
		}
	break;
	case EIsaNarrowAndUnicodeDoubleDesTobeSet:
	case EIsaNarrowAndUnicodeDoubleDesTobeRead:
	case EIsaNarrowDesToGetUnicodeDesToSet:
	case EIsaUnicodeDesToGetNarrowDesToSet:
		{
		if(basicMsgType==EIsaNarrowAndUnicodeDoubleDesTobeSet)
			{
			sizeArg0=aMessage.GetDesLength(0);    // set data
			sizeArg2=aMessage.GetDesLength(2); // set data
			}
		else if(basicMsgType == EIsaNarrowAndUnicodeDoubleDesTobeRead)
			{
			sizeArg0=aMessage.GetDesMaxLength(0); // get data
			sizeArg2=aMessage.GetDesMaxLength(2); // get data
			}
		else if(basicMsgType == EIsaNarrowDesToGetUnicodeDesToSet)
			{
			sizeArg0=aMessage.GetDesMaxLength(0); // get data
			sizeArg2=aMessage.GetDesLength(2);    // set data
			}
		else // EIsaUnicodeDesToGetNarrowDesToSet:
			{
			sizeArg0=aMessage.GetDesLength(0);		 // set data
			sizeArg2=aMessage.GetDesMaxLength(2);    // get data
			}
		//read the arg 0 data from the message
		RBuf8 bufArg0;
		CleanupClosePushL(bufArg0);
		ReadIntoBufferL(aMessage, bufArg0, sizeArg0, 0);
		//read the arg 2 data from the message
		RBuf16 bufArg2;
		CleanupClosePushL(bufArg2);
		ReadIntoBufferL(aMessage, bufArg2, sizeArg2, 2);
		//log out
		RECORD_IPC_LOG7(aSession, aTelObject, handle, ipc, arg1, bufArg0, bufArg2);
		//cleanup
		CleanupStack::PopAndDestroy(2,&bufArg0); // bufArg2, bufArg0
		}		
	break;
	case EIsaUnicodeDesToSetAndGet:
		{
		sizeArg0=aMessage.GetDesLength(0);		// set data
		sizeArg2=aMessage.GetDesMaxLength(2);	// get data
		//read the arg 0 data from the message
		RBuf16 bufArg0;
		CleanupClosePushL(bufArg0);
		ReadIntoBufferL(aMessage, bufArg0, sizeArg0, 0);
		//read the arg 2 data from the message
		RBuf16 bufArg2;
		CleanupClosePushL(bufArg2);
		ReadIntoBufferL(aMessage, bufArg2, sizeArg2, 2);
		//log out
		RECORD_IPC_LOG7(aSession, aTelObject, handle, ipc, arg1, bufArg0, bufArg2);
		//cleanup
		CleanupStack::PopAndDestroy(2,&bufArg0); // bufArg2, bufArg0
		}		
	break;
	case EIsaUnicodeDesTobeSet: //== EIsaDesTobeSet | KUnicodeReq
	case EIsaUnicodeDesTobeRead: //== EIsaDesTobeRead | KUnicodeReq
		{
		if(basicMsgType==EIsaUnicodeDesTobeSet)
			{
			sizeArg0=aMessage.GetDesLength(0); // set data
			}
		else //EIsaUnicodeDesTobeRead
			{
			sizeArg0=aMessage.GetDesMaxLength(0); // get data	
			}		
		//read the arg 0 data from the message
		RBuf16 bufArg0;
		CleanupClosePushL(bufArg0);
		ReadIntoBufferL(aMessage, bufArg0, sizeArg0, 0);
		//log out
		RECORD_IPC_LOG6(aSession, aTelObject, handle, ipc, arg1, bufArg0);
		//cleanup
		CleanupStack::PopAndDestroy(&bufArg0);
		}
	break;
	case EIsaUnicodeDoubleDesTobeSet: //== EIsaDoubleDesTobeSet | KUnicodeReq
	case EIsaUnicodeDoubleDesTobeRead: //== EIsaDoubleDesTobeRead | KUnicodeReq
		{
		if(basicMsgType==EIsaUnicodeDoubleDesTobeSet)
			{
			sizeArg0=aMessage.GetDesLength(0); // set data
			sizeArg2=aMessage.GetDesLength(2);	 // set data
			}
		else if(basicMsgType==EIsaUnicodeDoubleDesTobeRead)
			{
			sizeArg0=aMessage.GetDesMaxLength(0); // get data
			sizeArg2=aMessage.GetDesMaxLength(2); // get data
			}
		//read the arg 0 data from the message
		RBuf16 bufArg0;
		CleanupClosePushL(bufArg0);
		ReadIntoBufferL(aMessage, bufArg0, sizeArg0, 0);
		//read the arg 2 data from the message
		RBuf16 bufArg2;
		CleanupClosePushL(bufArg2);
		ReadIntoBufferL(aMessage, bufArg2, sizeArg2, 2);
		//log out
		RECORD_IPC_LOG7(aSession, aTelObject, handle, ipc, arg1, bufArg0, bufArg2);
		//cleanup
		CleanupStack::PopAndDestroy(2,&bufArg0); // bufArg2, bufArg0
		}
	break;

	default:
		//unknown type do nothing
		break;
		}
	}
	
#endif // ETEL_RECORDER
