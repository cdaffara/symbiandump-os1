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
// Header file for the AO used to cancel EtelPckt AOs
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef _TE_PCKTCANCELLER_H_
#define _TE_PCKTCANCELLER_H_

class CTestCanceller : public CAsyncOneShot
	{
public:
	CTestCanceller(CActive*);
	~CTestCanceller();
private:
	
	void ConstructL();
	void RunL();
private:
	CActive* iRetrieve;// not owned by this
	};


#endif
