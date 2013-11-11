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

#ifndef LBS_TRANSMIT_LOCATION_LOG_EVENT_H
#define LBS_TRANSMIT_LOCATION_LOG_EVENT_H

#include <lbs/lbslogevent.h>

/** log event object for transmit to 3rd party locate request */

class CLbsTransmitLocationLogEvent : public CLbsLogEvent
	{
public:
	IMPORT_C static CLbsTransmitLocationLogEvent* NewL(TUid aUid, TPositionInfoBase* aPosInfo, TDesC& aDestination);
	~CLbsTransmitLocationLogEvent();
	// for X3P
	IMPORT_C TUid X3PApp() const;
	IMPORT_C void SetX3PApp(TUid aUid);
	IMPORT_C void SetDestination(TDesC& aDestination);
	IMPORT_C TDesC& Destination() const;

protected:
	void DoExternalizeL(RWriteStream& aWriteStream) const;
	void DoInternalizeL(RReadStream& aReadStream);
	void DoUpdateL(const CLbsLogEvent& aSourceLogEvent);
	void DoCopyL(CLbsLogEvent*& aTargetLogEvent) const;
	TInt DataSize() const;
private:
	CLbsTransmitLocationLogEvent();
	void ConstructL(TUid aUid, TPositionInfoBase* aPosInfo, TDesC& aDestination);
	
private:
	/** The UID of 3rd party application request for the location information */
	TUid 					iX3PApp;
	/** Descriptor of the destination application */
	TBuf<64>				iDestination;
	}; // class CLbsTransmitLocationLogEvent

#endif // LBS_TRANSMIT_LOCATION_LOG_EVENT_H
