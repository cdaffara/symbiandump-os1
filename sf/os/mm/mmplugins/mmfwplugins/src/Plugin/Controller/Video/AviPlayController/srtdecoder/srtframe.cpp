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

#include <e32base.h>
#include <e32const.h> 
#include <utf.h> 

#include "srtframe.h"

const TInt64 KMMFMicroSecondsInAMillisecond = 1000;
const TInt64 KMMFMicroSecondsInASecond = KMMFMicroSecondsInAMillisecond * 1000;
const TInt64 KMMFMicroSecondsInAMinute = KMMFMicroSecondsInASecond * 60;
const TInt64 KMMFMicroSecondsInAnHour = KMMFMicroSecondsInAMinute * 60;

#define KMMFSubtitleLF '\n'
#define KMMFSubtitleColon ':'
#define KMMFSubtitleComma ','

/** Allocates and constructs this class  
	
@return A pointer to the new object. 
@leave If an error occurs, the method will leave with a system-wide error code. 
*/ 
CSrtFrame* CSrtFrame::NewL()
    {
    CSrtFrame* self = new(ELeave)CSrtFrame();
    return self;
    }

/** Default constructor */
CSrtFrame::CSrtFrame()
    {
    }
    
/** Destructor */    
CSrtFrame::~CSrtFrame()
    {
    delete iText;
    }

/** Return the start time of the subtitle frame after being parsed 

@return The start time of the subtitle frame 
*/   
TInt64 CSrtFrame::StartTime() const
    {
    return iStart;
    }

/** Return the end time of the subtitle frame after being parsed 

@return The end time of the subtitle frame 
*/
TInt64 CSrtFrame::EndTime() const
    {
    return iStart + iDuration;
    }

/** Return the duration of the subtitle frame after being parsed 

@return The duration of the subtitle frame 
*/    
TInt64 CSrtFrame::Duration() const
    {
    return iDuration;
    }

/** Return the content of the subtitle frame after being parsed 

@return The content of the subtitle frame 
*/
const TDesC8& CSrtFrame::Content() const
    {
    if (iText)
        {
        return *iText;
        }
    else
        {
        return KNullDesC8();
        }
    }

/** Parse the description of a time in a srt subtitle frame 
	
@param aData The input descriptor containing time information
@param aTime The time parsed
@leave If an error occurs, the method will leave with a system-wide error code. 
*/    
void CSrtFrame::ParseTimeL(const TDesC8& aData, TInt64& aTime)
    {
    // Sample format: 00:00:45,000
    const TInt KMMFSubtitleExpectedNumOfColon = 2;
    const TInt KMMFSubtitleExpectedNumOfComma = 1;
    
    TInt tHour = 0;
    TInt tMinute = 0;
    TInt tSecond = 0;
    TInt tMilliSecond = 0;
    TInt numOfColon = 0;
    TInt numOfComma = 0;

    TLex8 inputData(aData);
    inputData.SkipSpace();
    
    TInt err = inputData.Val(tHour);
    if (KErrNone != err)
        {
        User::Leave(KErrArgument);
        }
    
    while(!inputData.Eos())
        {
        TChar ch = inputData.Get();
        
        if (KMMFSubtitleColon == ch)
            {
            numOfColon++;

            if (numOfColon > 1)
                {
                err = inputData.Val(tSecond);
                }
            else
                {
                err = inputData.Val(tMinute);
                }
            }
        else if (KMMFSubtitleComma == ch)
            {
            numOfComma++;
            
            err = inputData.Val(tMilliSecond);
            }
        
        if (KErrNone != err)
            {
            User::Leave(KErrArgument);
            }        
            
        if ((numOfColon > KMMFSubtitleExpectedNumOfColon) || (numOfComma > KMMFSubtitleExpectedNumOfComma))
            {
            break;
            }
        }
        
    if ((numOfColon != KMMFSubtitleExpectedNumOfColon) || (numOfComma != KMMFSubtitleExpectedNumOfComma))
        {
        User::Leave(KErrArgument);
        }
     
    if ((tHour < 0) || (tMinute < 0) || (tSecond < 0) || (tMilliSecond < 0) ||
    (tMinute > 59) || (tSecond > 59) || (tMilliSecond > 999))
        {
        User::Leave(KErrArgument);
        }
           
    aTime = CreateInt64Time(tHour, tMinute, tSecond, tMilliSecond); 
    }

/** Parse a row containing start and end time in a srt subtitle frame 
	
@param aData The row to be parsed
@param aStartTime The start time in the description
@param aEndTime The end time in the description
@leave If an error occurs, the method will leave with a system-wide error code. 
*/
void CSrtFrame::ParseTimeRowL(const TDesC8& aData, TInt64& aStartTime, TInt64& aEndTime)
    {
    // Sample format: 00:00:45,000 --> 00:00:50,400
    _LIT8( KSeperator, "-->" );
    const TInt KSeperatorLength = 3;
    TInt pos = aData.FindF(KSeperator);
    
    if (KErrNotFound == pos)
        {
        User::Leave(KErrArgument);
        }

    TPtrC8 ptrLeft = aData.Left(pos);
    TPtrC8 ptrRight = aData.Right(aData.Length() - pos - KSeperatorLength);

    ParseTimeL(ptrLeft, aStartTime);
    ParseTimeL(ptrRight, aEndTime);

    if (aStartTime > aEndTime)
        {
        User::Leave(KErrArgument);
        }
    }

/** Parse a row containing presentation position in a srt subtitle frame 
	
@param aData The row to be parsed
@param aPresentationPosition The output value of presentation position
@param aNextAlphaDigitCharPosition The position of the next alpha character
@leave If an error occurs, the method will leave with a system-wide error code. 
*/
void CSrtFrame::ParsePresentationPositionRowL(const TDesC8& aData, TUint& aPresentationPosition, TInt& aNextAlphaDigitCharPosition)
    {
    // Sample format: 1
    TInt startPos = KErrNotFound;
    aPresentationPosition = 0;
    aNextAlphaDigitCharPosition = KErrNotFound;
    TInt i = 0;
    TInt dataLen = aData.Length();
    
    for (i = 0; i < dataLen; i++)
        {
        TChar ch = aData[i];
        if (ch.IsAlphaDigit())
            {
            if (!ch.IsDigit())
                {
                User::Leave(KErrArgument);
                }
            startPos = i;
            break;
            }
        }
        
    if (KErrNotFound == startPos)
        {
        User::Leave(KErrArgument);
        }
        
    TPtrC8 ptrRest = aData.Right(aData.Length() - startPos);
    
    TInt pos = ptrRest.Locate(KMMFSubtitleLF);
    if (KErrNotFound == pos)
        {
        User::Leave(KErrArgument);
        }
        
    TPtrC8 ptrNumber = ptrRest.Left(pos);
    
    TLex8 lexNumber(ptrNumber);
    TUint positionNum = 0;
    TInt err = lexNumber.Val(positionNum, EDecimal);
    if (KErrNone != err)
        {
        User::Leave(KErrArgument);
        }
    
    TInt curPos = startPos + pos;
    if (curPos >= aData.Length())
        {
        User::Leave(KErrArgument);
        }
    
    TInt nextAlphaDigitCharPosition = KErrNotFound;
    for (i = curPos; i < dataLen; i++)
        {
        TChar ch = aData[i];
        if (ch.IsAlphaDigit())
            {
            nextAlphaDigitCharPosition = i;
            break;
            }
        }
    
    if (KErrNotFound == nextAlphaDigitCharPosition)
        {
        User::Leave(KErrArgument);
        }
    
    aPresentationPosition = positionNum;
    aNextAlphaDigitCharPosition = nextAlphaDigitCharPosition;
    }

/** Parse a srt subtitle source 
	
@param aFrame the srt subtitle frame to be parsed
@param aFilteredByVideoPosition Indicate if search for the next valid frame matching aVideoPosition
@param aVideoPosition The video position
@leave If an error occurs, the method will leave with a system-wide error code. 
*/    
void CSrtFrame::ParseL(const TDesC8& aFrame, TBool aFilteredByVideoPosition, TInt64 aVideoPosition)
    {
    // Sample format: 
    // 1
    // 00:00:45,000 --> 00:00:50,400
    // Hello
    TInt pos = KErrNotFound;
    
    delete iText;
    iText = NULL;
    
    TUint positionNum = 0;
    TInt nextRowPosition = KErrNotFound;
    ParsePresentationPositionRowL(aFrame, positionNum, nextRowPosition);
    
    TPtrC8 ptrRest = aFrame.Right(aFrame.Length() - nextRowPosition);
    
    pos = ptrRest.Locate(KMMFSubtitleLF);
    if (KErrNotFound == pos) 
        {
        // no content text
        pos = ptrRest.Length();
        }
        
    TPtrC8 ptrTimeRow = ptrRest.Left(pos);
         
    TInt64 tStartTime = 0;
    TInt64 tEndTime = 0;
    
    ParseTimeRowL(ptrTimeRow, tStartTime, tEndTime);

    TInt64 tDuration = tEndTime - tStartTime;

    if (aFilteredByVideoPosition)
        {
        if ((tEndTime < aVideoPosition) &&
            (tStartTime < aVideoPosition))
            {
            // Input param aFrame does not match video position
            User::Leave(KErrArgument);
            }
        }
    
    iStart = tStartTime;
    iDuration = tDuration;
    iPosition = positionNum;
    
    if (pos < ptrRest.Length())
        {
        TPtrC8 ptrTextRow = ptrRest.Right(ptrRest.Length() - pos - 1);
        iText = ptrTextRow.AllocL();
        }
    else
        {
        // blank content is considered as an invalid frame
        User::Leave(KErrArgument);
        }
    }

/** Calculate a time value in 64-bit integer
	
@param aHour The value of hour to create a time value in 64-bit integer
@param aMinute The value of minute to create a time value in 64-bit integer
@param aSecond The value of second to create a time value in 64-bit integer
@param aMillisecond The value of millisecond to create a time value in 64-bit integer
@return The calculated time value as 64-bit integer 
*/
TInt64 CSrtFrame::CreateInt64Time(TInt aHour, TInt aMinute, TInt aSecond, TInt aMillisecond)
    {      
    return KMMFMicroSecondsInAnHour * TInt64(aHour) + 
            KMMFMicroSecondsInAMinute * TInt64(aMinute) + 
            KMMFMicroSecondsInASecond * TInt64(aSecond) + 
            KMMFMicroSecondsInAMillisecond * TInt64(aMillisecond);
    }
