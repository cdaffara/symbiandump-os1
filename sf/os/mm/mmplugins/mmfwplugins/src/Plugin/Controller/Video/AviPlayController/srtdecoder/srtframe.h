// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology 
*/

#ifndef SRTFRAME_H
#define SRTFRAME_H

#include <e32cmn.h> 

/** A class to parse a SRT subtitle frame */ 
NONSHARABLE_CLASS(CSrtFrame) : public CBase
    {
public:   
    /** Allocates and constructs this class */ 
    static CSrtFrame* NewL();
    
	virtual ~CSrtFrame();
	
	/** Parse a srt subtitle source */
	void ParseL(const TDesC8& aFrame, TBool aFilteredByVideoPosition, TInt64 aVideoPosition);
	
	/** Return the start time of the subtitle frame after being parsed */
	TInt64 StartTime() const;
	
	/** Return the duration of the subtitle frame after being parsed */
	TInt64 Duration() const;
	
	/** Return the end time of the subtitle frame after being parsed */
	TInt64 EndTime() const;
	
	/** Return the content of the subtitle frame after being parsed */
	const TDesC8& Content() const;
	
private:
    CSrtFrame();

	/** Parse the description of a time in a srt subtitle frame	*/
	static void ParseTimeL(const TDesC8& aData, TInt64& aTime);
    
    /** Parse a row containing start and end time in a srt subtitle frame */
    static void ParseTimeRowL(
        const TDesC8& aData, 
        TInt64& aStartTime,
        TInt64& aEndTime);
        
    /** Parse a row containing presentation position in a srt subtitle frame */
	static void ParsePresentationPositionRowL(
	    const TDesC8& aData, 
	    TUint& aPresentationPosition, 
	    TInt& aNextAlphaDigitCharPosition);
	    
	/** Calculate a time value in 64-bit integer */
	static TInt64 CreateInt64Time(TInt aHour, TInt aMinute, TInt aSecond, TInt aMillisecond);
	
private:
	/** The content of the subtitle frame */
	HBufC8* iText;
	
	/** The start time of the subtitile frame */
	TInt64 iStart;
	
	/** The duration of the subtitle frame */
	TInt64 iDuration;
	
	/** The presentation number of the subtitle frame */
	TUint iPosition;
    };
    
#endif //SRTFRAME_H


