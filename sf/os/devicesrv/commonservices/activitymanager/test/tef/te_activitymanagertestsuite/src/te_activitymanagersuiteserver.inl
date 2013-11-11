/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

/*@{*/
_LIT(KDataActivityManager,	"CUserActivityManager");
/*@}*/

inline CTeActivityManagerSuite::CtActivityManagerBlock::CtActivityManagerBlock()
    {
    }

inline CTeActivityManagerSuite::CtActivityManagerBlock::~CtActivityManagerBlock()
    {
    }

inline CDataWrapper* CTeActivityManagerSuite::CtActivityManagerBlock::CreateDataL( const TDesC& aData )
	{
	CDataWrapper*	wrapper = NULL;
	if( KDataActivityManager() == aData )
		{
		wrapper = CTeActivityManager::NewL();
		}
	return wrapper;
	}

inline CTeActivityManagerSuite::CTeActivityManagerSuite()
    {
    }
    
inline CTeActivityManagerSuite::~CTeActivityManagerSuite()
    {
    }

inline CTestBlockController* CTeActivityManagerSuite::CreateTestBlock()
	{
	return new CtActivityManagerBlock();
	}
