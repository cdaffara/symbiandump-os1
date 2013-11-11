/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @publishedPartner
 @released
*/

#ifndef LBS_ASSISTANCE_DATA_LOG_EVENT_H
#define LBS_ASSISTANCE_DATA_LOG_EVENT_H

#include <lbs/lbslogevent.h>

/** log event object for assistance data request */

class CLbsAssistanceDataLogEvent : public CLbsLogEvent
	{
public:
	IMPORT_C static CLbsAssistanceDataLogEvent* NewL(const TUint32 aRequestType, const TUint32 aReceiveType);
	~CLbsAssistanceDataLogEvent();
	// for assistance data
	IMPORT_C void SetRequestAssistanceDataType(const TUint32 aMask);
	IMPORT_C TUint32 RequestAssistanceDataType() const;
	IMPORT_C void SetReceiveAssistanceDataType(const TUint32 aMask);
	IMPORT_C TUint32 ReceiveAssistanceDataType() const;

protected:
	void DoExternalizeL(RWriteStream& aWriteStream) const;
	void DoInternalizeL(RReadStream& aReadStream);
	void DoUpdateL(const CLbsLogEvent& aSourceLogEvent);
	void DoCopyL(CLbsLogEvent*& aTargetLogEvent) const;
	TInt DataSize() const;
private:
	CLbsAssistanceDataLogEvent();
	void ConstructL(const TUint32 aRequestType, const TUint32 aReceiveType);
private:
	/** requested assistance data type from application */
	TUint32	iRequestAssistanceDataType;
	/** received assistance data type from network */
	TUint32	iReceiveAssistanceDataType;

	}; // class CLbsAssistanceDataLogEvent


#endif // LBS_ASSISTANCE_LOG_EVENT_H
