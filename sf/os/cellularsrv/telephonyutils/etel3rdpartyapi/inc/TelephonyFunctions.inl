/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Inline code, logically part of telephonyfunctions.h
*
*/


/**
 @file
*/

inline TRequestStatus** CTelephonyFunctions::RequestStatus()
	//
	//Provide address of ptr-to-ReqStat. When User::RequestComplete() is used,
	//the ptr-to-ReqStat (iRequestStatus in here) gets zeroed, I think to prevent
	//the RequestComplete message ever getting sent twice. (This zeroing feature
	//does not appear to be documented in Symbian SDK 6.0).
	//
	//@return	address of Pointer-to-callers-RequestStatus member.
	//
	{
	return &iRequestStatus;
	}

inline TRequestStatus** CTelephonyFunctions::GetRequestStatus(CTelephonyFunctions::TOperations aOp)
	//
	//Provide address of ptr-to-ReqStat. When User::RequestComplete() is used,
	//the ptr-to-ReqStat (iRequestStatus in here) gets zeroed, I think to prevent
	//the RequestComplete message ever getting sent twice. (This zeroing feature
	//does not appear to be documented in Symbian SDK 6.0).
	//
	//@return	address of Pointer-to-callers-RequestStatus member.
	//
	{
	return &(iReqStatusTable[aOp]);
	}

inline TBool CTelephonyFunctions::IsRequestPending(CTelephonyFunctions::TOperations aOp)
	{
	return iReqPendingTable[aOp];
	}

inline void	CTelephonyFunctions::SetRequestPending(CTelephonyFunctions::TOperations aOp, TBool aBool)
	//
	//Notifier Active Objects use this when RunL() runs.
	//
	//Sets EFalse, which in turn prevents any later cancellation by the user.
	//
	{
	iReqPendingTable[aOp] = aBool;
	}
	
inline RMobileCall* CTelephonyFunctions::Call()
	//
	//Active Objects will use this.
	//
	//@return Pointer to the RCall in use
	//
	{
	return &iCall;
	}

inline RMobileCall* CTelephonyFunctions::Call(const CTelephony::TCallId aCallId)
	//
	//Active Objects will use this.
	//
	//@return Pointer to the RCall in use
	//
	{
	if(iCallPoolStatus[aCallId]!=EUnset)
		return &(iCallPool[aCallId]);
	else
		return NULL;
	}

/**
 *  Active Objects will use this function to get the required line handle.
 *
 *  @param aLine  Line to return.
 *  @return  Pointer to the RLine in use
 */
inline RMobileLine* CTelephonyFunctions::Line(const CTelephony::TPhoneLine aLine)
	{
	if (aLine == CTelephony::EVoiceLine)
		{
		return &iLineVoice;
		}
	else if (aLine == CTelephony::EDataLine)
		{
		return &iLineData;
		}
	else if (aLine == CTelephony::EFaxLine)
		{
		return &iLineFax;
		}
	else
		{
		return NULL;
		}
	}
	
inline RMobilePhone* CTelephonyFunctions::Phone()
	{
	return &iPhone;
	}

inline void	CTelephonyFunctions::SetAsyncRequestOutstandingFalse()
	{
	iAsyncRequestOutstanding = EFalse;
	}

inline RProperty* CTelephonyFunctions::PhonePowerProperty()
	{
	return &iPhonePowerProperty;
	}




