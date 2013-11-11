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
// CRmPartySmlS.h
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __RmPartySmlS_H
#define __RmPartySmlS_H

class CRequestHandler;

//
// Container class only to be created by CommonFramework and hold the main object: CRqstHandler
//
class CRmPartySmlS : public CBase
	{
	public:
	static CRmPartySmlS* NewL();
		~CRmPartySmlS();
	private:
		void ConstructL();
	
	private:
		CRequestHandler* iRqstHdlr;
	
	};

#endif
