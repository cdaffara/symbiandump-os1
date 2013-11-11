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

#ifndef LBS_SELF_LOCATE_LOG_EVENT_H
#define LBS_SELF_LOCATE_LOG_EVENT_H

#include <lbs/lbslogevent.h>

/** log event object for self locate request */

class CLbsSelfLocateLogEvent : public CLbsLogEvent
	{
public:
	IMPORT_C static CLbsSelfLocateLogEvent* NewL(TUid aUid, TPositionInfoBase* aPosInfo);
	~CLbsSelfLocateLogEvent();
	// for self locate
	IMPORT_C TUid LocalApp() const;
	IMPORT_C void SetLocalApp(TUid aUid);

protected:
	void DoExternalizeL(RWriteStream& aWriteStream) const;
	void DoInternalizeL(RReadStream& aReadStream);
	void DoUpdateL(const CLbsLogEvent& aSourceLogEvent);
	void DoCopyL(CLbsLogEvent*& aTargetLogEvent) const;
	TInt DataSize() const;
	
private:
	CLbsSelfLocateLogEvent();
	void ConstructL(TUid aUid, TPositionInfoBase* aPosInfo);

private:
	/** The UID of local application request for the location information */
	TUid 				iLocalApp;
	}; // class CLbsSelfLocateLogEvent

#endif // LBS_SELF_LOCATE_LOG_EVENT_H
