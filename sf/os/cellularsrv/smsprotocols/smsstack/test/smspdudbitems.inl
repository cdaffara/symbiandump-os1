// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent 
*/


#ifndef __SMS_PDU_DB_ITEMS_INL__
#define __SMS_PDU_DB_ITEMS_INL__

inline CSmsPduDbBase::CSmsPduDbBase()
: iId(KErrNone), iDescription(KNullDesC8), iSmsMessage(NULL), iSmsMessageError(KErrNone)
	{
	}

inline CSmsPduDbPdu::CSmsPduDbPdu(CSmsPDU::TSmsPDUType aType)
: CSmsPduDbBase(), iType(aType), iDecodeError(KErrNone)
	{
	}

inline CSmsPduDbConcatSegment::CSmsPduDbConcatSegment(CSmsPDU::TSmsPDUType aType)
: CSmsPduDbPdu(aType), iSegment(KErrNotFound)
	{
	}

inline CSmsPduDbConcat::CSmsPduDbConcat()
: CSmsPduDbBase()
	{
	}

inline CSmsPduDbMessage::CSmsPduDbMessage(CSmsPDU::TSmsPDUType aType)
: CSmsPduDbPdu(aType), iPdus(2)
	{
	}

#endif
