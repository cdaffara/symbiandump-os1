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

#ifndef LBS_NETWORK_LOCATE_LOG_EVENT_H
#define LBS_NETWORK_LOCATE_LOG_EVENT_H

#include <lbs/lbslogevent.h>

/** log event object for network locate request */

class CLbsNetworkLocateLogEvent : public CLbsLogEvent
	{
public:
	IMPORT_C static CLbsNetworkLocateLogEvent* NewL(TUid aUid, TPositionInfoBase* aPosInfo);
	~CLbsNetworkLocateLogEvent();
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
	CLbsNetworkLocateLogEvent();
	void ConstructL(TUid aUid, TPositionInfoBase* aPosInfo);
		
private:
	/** The UID of local application request for the location information */
	TUid 				iLocalApp;

	}; // class CLbsNetworkLocateLogEvent


#endif // LBS_NETWORK_LOCATE_LOG_EVENT_H
