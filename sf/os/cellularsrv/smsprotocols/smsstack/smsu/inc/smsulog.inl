// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
 Tests whether the Log Engine is installed.
 
 @publishedAll
 @released
 @return True if there is a Log Engine, otherwise false 
 
*/
inline TBool CSmsEventLogger::ClientAvailable() const
    {
    return iLogWrapper->ClientAvailable();
    }

/**
 *  Gets a standard string from the specified resource.
 *  
 *  @publishedAll
 *  @released
 *  @param aString On return, string read from resource
 *  @param aId Resource ID
 *  @return KErrNone, if successful; otherwise, one of the other system wide error
 *  codes 
 */
inline TInt CSmsEventLogger::GetString(TDes& aString, TInt aId) const
    {
    return iLogWrapper->Log().GetString(aString,aId);
    }

/**
 *  Gets the current log event.
 *  
 *  @publishedAll
 *  @released
 *  @return Current log event 
 */
inline CLogEvent& CSmsEventLogger::Event()
    {
    return *iLogEvent;
    }

/**
 *  Gets the current (const) log event.
 *  
 *  @publishedAll
 *  @released
 *  @return Current log event 
 */
inline const CLogEvent& CSmsEventLogger::Event() const
    {
    return *iLogEvent;
    }

/**
 *  Gets the current event's SMS PDU data.
 *  
 *  @publishedAll
 *  @released
 *  @return Current event's SMS PDU data 
 */
inline const TLogSmsPduData& CSmsEventLogger::SmsPDUData() const
    {
    return iSmsPDUData;
    }

/**
 *  Gets the current event's SMS PDU data.
 *  @publishedAll
 */
inline void CSmsEventLogger::GetStringL(TDes& aString, TInt aId) const
    {
    User::LeaveIfError(iLogWrapper->Log().GetString(aString,aId));
    }
