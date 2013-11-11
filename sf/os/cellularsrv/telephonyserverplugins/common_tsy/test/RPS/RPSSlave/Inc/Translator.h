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
// RequestParser.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __REQSTPARSER_H
#define __REQSTPARSER_H


//
// CRequestParser.
// Parses and verifies a request msg from Master.
//
// Not intended for derivation
class RequestParser : public CBase		// TODO  all below!!!
	{
public:
	static CRequestParser* NewL();
	~CRequestParser();
	
	TInt Parse
	
private:
	void ConstructL();

private:
	
	}



#endif
