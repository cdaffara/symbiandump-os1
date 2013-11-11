// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains internal class used by mmfcontroller framework
// 
//

#ifndef MMFMATCHDATA_H
#define MMFMATCHDATA_H

/**
class used to hold both the Matching data and uri scheme 
*/

class CMatchData; // declared here.

NONSHARABLE_CLASS( CMatchData ): public CBase
	{
public:
	static CMatchData* CreateL();
	
	void SetMatchDataL(const TDesC8& aMatchData);
	
	void SetMatchUriSchemeL(const TDesC8& aMatchUriScheme);
	
	~CMatchData();
	
	const TDesC8& MatchData() const;
	const TDesC8& MatchUriScheme() const;
	
private:
	HBufC8* iMatchData;
	HBufC8* iMatchUriScheme;
	};
  
#endif  
  
