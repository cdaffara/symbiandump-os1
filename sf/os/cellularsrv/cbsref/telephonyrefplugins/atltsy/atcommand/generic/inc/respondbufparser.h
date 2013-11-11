// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CRespondBufParser which provide an algorithm to analize the passed data from baseband
//

#ifndef __CRESPONDBUFPARSER_H__
#define __CRESPONDBUFPARSER_H__
#include <e32cmn.h> 
#include <e32base.h>

/**
* This class is used to analyze the passed data and put the value in the buffer store
*
*/
class CRespondBufParser : public CBase
	{
public:
    /**
    * Constructor
    */
	CRespondBufParser();
	
	/**
	* Destructor
	*/
	virtual ~CRespondBufParser();
	
	/**
	* Parse the responsed data
	*/
	void ParseRespondedBuffer(RArray<TPtrC8>& aArray, const TDesC8& aBuf);
	};

#endif
