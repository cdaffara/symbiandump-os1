/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef LBSAGPSNGMSGS_H
#define LBSAGPSNGMSGS_H

#include <e32std.h>
#include "lbscommoninternaldatatypes.h"
#include "lbsnetinternalmsgtypes.h"

/**
@file
@internalTechnology
@released
*/

/** Self Location request originating from the AGPS Manager
*/
NONSHARABLE_CLASS(TLbsNetSelfLocationRequestMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetSelfLocationRequestMsg(const TLbsNetSessionIdInt& aSessionId, 
										   const TLbsNetPosRequestOptionsAssistanceInt& aOptions);

	IMPORT_C const TLbsNetPosRequestOptionsAssistanceInt& Options() const;
		
private:
	TLbsNetSelfLocationRequestMsg();

private:
	TLbsNetPosRequestOptionsAssistanceInt iOptions;
	};
	
/** Self Location request cancel originating from the AGPS Manager
*/
NONSHARABLE_CLASS(TLbsNetSelfLocationCancelMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetSelfLocationCancelMsg(const TLbsNetSessionIdInt& aSessionId,
										  TInt aReason);
	IMPORT_C TInt Reason() const;

private:
	TLbsNetSelfLocationCancelMsg();
	
private:
	TInt iReason;
	};
		
/** Assistance Data request originating from the AGPS Manager
*/
NONSHARABLE_CLASS(TLbsNetAssistanceDataRequestMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetAssistanceDataRequestMsg(TLbsAsistanceDataGroupInt aDataRequestMask);
	IMPORT_C TLbsAsistanceDataGroupInt DataRequestMask() const;

private:
	TLbsNetAssistanceDataRequestMsg();
	
private:
	TLbsAsistanceDataGroupInt iDataRequestMask;
	};
		
/** Assistance Data Response originating from the NG
	Please note the message contains just the Mask. 
	The actual data is send through the XXX interface.
@see XXX
*/
NONSHARABLE_CLASS(TLbsNetAssistanceDataResponseMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetAssistanceDataResponseMsg(TLbsAsistanceDataGroupInt aDataResponseMask, TInt aReason);
	IMPORT_C TLbsAsistanceDataGroupInt DataResponseMask() const;
	IMPORT_C TInt Reason() const;

private:
	TLbsNetAssistanceDataResponseMsg();
	
private:
	TLbsAsistanceDataGroupInt iDataResponseMask;
	TInt iReason;
	};
		
/** Advice System Status originating from the AGPS Manager
*/
NONSHARABLE_CLASS(TLbsNetSystemStatusAdviceMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetSystemStatusAdviceMsg(TBool aTracking);
	IMPORT_C TBool Tracking() const;

private:
	TLbsNetSystemStatusAdviceMsg();
	
private:
	TBool iTracking;
	};	

/** Signals the end of a request. 

This is a generic 'complete' that is used to end all of the different types 
of request that are represented by the other message types.

The reason for completing the request will usually be KErrNone but it may
also signal an error. The particular error is defined by the type of request
being completed and the reason for the error.
*/	
NONSHARABLE_CLASS(TLbsNetSessionCompleteAgpsMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetSessionCompleteAgpsMsg(const TLbsNetSessionIdInt& aSessionId,
									   TInt aReason);
	
	IMPORT_C TInt Reason() const;

private:
	TLbsNetSessionCompleteAgpsMsg();
	
private:
	TInt iReason;
	};
	
#endif // LBSAGPSNGMSGS_H
