// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <cflog.h>
#include <commschan.h>
#include <cfshared.h>
using namespace CommsFW;
#include "bm_defs.h"

/**
@file
@internalComponent
*/

CTask* CTask::NewL(const TCFMessage &aMessage, const TUint aTaskId /*= 0*/)
/** Creates new CTask object
@param aMessage the message to be associated with the task
@param aTaskId the tasks identifier
@return Pointer to the new CTask object.
@leave KErrNoMemory
*/
	{
    CTask* pT = new(ELeave) CTask(aMessage, aTaskId);
    CleanupStack::PushL(pT);
    __CFLOG_2(KLogSubSysRS, KLogCode,_L8("CTask(%X)::NewL %d"), pT, aTaskId);
	CleanupStack::Pop(pT);
    return pT;
	}

CTask::CTask(const TCFMessage &aMessage, const TUint aTaskId) :
	iTaskId(aTaskId),
	iMessage(aMessage)
/** Constructor for CTask
@param aMessage the message to be associated with the task
@param aTaskId the tasks identifier
*/
	{
	}

CTask::~CTask()
/** Destructor for CTask
*/
	{
    __CFLOG_1(KLogSubSysRS, KLogCode,_L8("~CTask %d"), iTaskId);
	Dequeue();
	}

TUint CTask::TaskId() const
/** Accessor method for the task id
@return The task identifier
*/
	{
	return iTaskId;
	}

const TCFMessage& CTask::Message() const
/**
@internalComponent 
Accessor method for the message

@return the message
*/
	{
	return iMessage;
	}

void CTask::Dequeue()
/** Remove this from the linked list
*/
	{
	iLink.Deque();
	}


TBool CTask::CheckIfSame(TCFMessage &aMessage) const
/** Compares the message code and parameter block of the task's message with the 
message passed as a parameter and returns the result
@param aMessage the comparator
@return ETrue if the messages match
*/
	{
	TBool match = ETrue;

	if (iMessage.Code() != aMessage.Code())
		{
		return EFalse;
		}

	switch (iMessage.Code())
		{
	case TCFCommsMessage::ECodeBind:
		{
		const TCFBindMsg& iParam = reinterpret_cast<const TCFBindMsg&>(iMessage);
		const TCFBindMsg& aParam = reinterpret_cast<const TCFBindMsg&>(aMessage);
		if (iParam.SubModule1() != aParam.SubModule1() ||
			iParam.SubModule2() != aParam.SubModule2() ||
			iParam.Type() != aParam.Type())
			{
			match = EFalse;
			}
		}	
		break;

	case TCFCommsMessage::ECodeUnbind:
		{
		const TCFUnbindMsg& iParam = reinterpret_cast<const TCFUnbindMsg&>(iMessage);
		const TCFUnbindMsg& aParam = reinterpret_cast<const TCFUnbindMsg&>(aMessage);
		if (iParam.SubModule1() != aParam.SubModule1() ||
			iParam.SubModule2() != aParam.SubModule2() ||
			iParam.PeerIsDead() != aParam.PeerIsDead())
			{
			match = EFalse;
			}
		}	
		break;

	case TCFCommsMessage::ECodeDiscover:
	case TCFCommsMessage::ECodeShutdown:
		// no params to test
		break;

	default:
		__ASSERT_DEBUG(EFalse, User::Panic(KBindMgrPanic, EUnsupportedCFMessage));
		break;
		}

	return match;
	}


TBool CTask::NeedReply() const
/** Checks if the task's message should expect to receive a reply from the 
server or CPM
@return ETrue if a reply is expected
*/
	{
	// test message type
	TBool reply = EFalse;

	switch (iMessage.Code())
		{
	case TCFCommsMessage::ECodeBind:
	case TCFCommsMessage::ECodeUnbind:
	case TCFCommsMessage::ECodeDiscover:
		reply = ETrue;
		break;

	case TCFCommsMessage::ECodeShutdown:
		// no reply
		break;

	default:
		__ASSERT_DEBUG(EFalse, User::Panic(KBindMgrPanic, EUnsupportedCFMessage));
		break;
		}

	return reply;
	}

TBool CTask::ReplyMatches(const TCFMessage &aMessage) const
/** Determines if the message passed as a parameter is a reply to this task's 
message and returns the result
@param aMessage the reply message to test
@return ETrue if the message is the reply
*/
	{
	// test if message is correct type and has same identifier
	TBool match = EFalse;

	if (NeedReply() && 
		(iMessage.Code() == TCFCommsMessage::ECodeBind && aMessage.Code() == TCFCommsMessage::ECodeBindComplete) ||
		(iMessage.Code() == TCFCommsMessage::ECodeUnbind && aMessage.Code() == TCFCommsMessage::ECodeUnbindComplete) ||
		(iMessage.Code() == TCFCommsMessage::ECodeDiscover && aMessage.Code() == TCFCommsMessage::ECodeDiscoverResp))
		{

		TId ida = *((TId*)aMessage.ParamBlock());
		TId idi = *((TId*)iMessage.ParamBlock());


		if (ida==idi)
			{
			match = ETrue;
			}
		}
	return match;
	}


TBool CTask::GetMsgReturnCode(const TCFMessage &aMessage, TInt &aStatus)
/** Extracts the result code from the message supplied and returns it in the 
status parameter
@param aMessage the reply containing the result code
@param aStatus the status to be filled in with the result code
@return ETrue if the a result code was present
*/
	{
	TBool codePresent = EFalse;

	if (aMessage.Code() == TCFCommsMessage::ECodeBindComplete ||
		aMessage.Code() == TCFCommsMessage::ECodeUnbindComplete ||
		aMessage.Code() == TCFCommsMessage::ECodeDiscoverResp)
		{
		aStatus = ((TInt*)aMessage.ParamBlock())[1];
		codePresent = ETrue;
		}
	else
		{
		aStatus = KErrNone;
		}
	return codePresent;
	}


