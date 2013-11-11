// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**@file
   @internalAll */
#ifndef __DACQ_DEF_H
#define __DACQ_DEF_H
/**
@internalComponent
*/
const TInt DACQ_ASYN_TIMEOUT = 100000;
/**
@internalComponent
*/
#define DACQ_PHONE_NUMBER_TO_DIAL	 _L("01632960000")
#define DACQ_CALL_BEARER_CAPS		RCall::KBearerCapsProtocolALT
#define DACQ_CALL_BEARER_SPEED		RCall::EBearerData4800

/**
@internalComponent
*/
#define DACQ_FAX_BUF_SIZE			(50)
/**
@internalComponent
*/
typedef TBuf8<DACQ_FAX_BUF_SIZE>	TAcqFaxBuf;
/**
@internalComponent
*/
#define DACQ_FAX_BUF_DATA			_L8("1632960000")

/**
@internalComponent
*/
#define DACQ_LINE_MODE		RLine::EModeFax
#define DACQ_CALL_STATUS	RCall::EStatusIdle
#define DACQ_LINE_STATUS	RCall::EStatusIdle
#define DACQ_PHONE_STATUS	RPhone::EDetectedPresent
#define DACQ_GET_PROGRESS_SPEED (11)
#define DACQ_MYFAX _L("MY FAX")
#define DACQ_MFAXFILE _L8("Fax Header")

#endif
