// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __WSGRAPHICMSGBUF_H__
#define __WSGRAPHICMSGBUF_H__

#include <e32std.h>

class TWsGraphicMsgBufParser
/** Utility class for parsing incoming data associated with individual draw commands
@see CWindowGc::DrawWsGraphic
@see RWsGraphicMsgBuf
@publishedPartner
@released
*/	{
public:
	IMPORT_C TWsGraphicMsgBufParser(const TDesC8& aData);
	IMPORT_C TInt Verify() const;
	IMPORT_C TInt Count() const;
	IMPORT_C TUid Uid(TInt aIndex) const;
	IMPORT_C TPtrC8 Data(TInt aIndex) const;
	IMPORT_C TInt Find(TUid aUid,TInt aStartingFrom = 0) const;
	IMPORT_C TBool LoadFixed(TUid aUid,TAny* aMsg,TInt aMsgSize,TInt aStartingFrom = 0) const;
protected:
	TInt IntAt(TInt aOfs) const;
protected:
	const TDesC8& iData;
	};

class TWsGraphicMsgAnimation
/** Utility class for parsing incoming TWsGraphicAnimation messages
@see CWindowGc::DrawWsGraphic
@see RWsGraphicMsgBuf
@publishedPartner
@released
*/	{
public:
	IMPORT_C TWsGraphicMsgAnimation();
	IMPORT_C TInt Load(const TWsGraphicMsgBufParser& aData);
	IMPORT_C TInt Load(const TWsGraphicMsgBufParser& aData,TInt aIndex);
	IMPORT_C TTimeIntervalMicroSeconds AnimationTime(const TTime& aNow,const TTimeIntervalMicroSeconds& aAnimationLength) const;
	IMPORT_C TBool IsPlaying(const TTime& aNow,const TTimeIntervalMicroSeconds& aAnimationLength) const;
private:
	enum
		{
		// animation state in lowest nibble
		EStopped	= 0x00,
		EPlaying	= 0x01,
		EStopping	= 0x02,
		EPaused		= 0x03,
		EStateMask	= 0x0F,
		// other flags above
		ELoop		= 0x10
		};
	TUint iFlags;
	TTime iPlay;
	TTime iPauseOrStopping;
	};

#endif //__WSGRAPHICMSGBUF_H__
