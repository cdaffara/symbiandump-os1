// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file for ROOTSERVER common defs
// 
//


#if (!defined __ROOTSERVERTEST_H__)
#define __ROOTSERVERTEST_H__

// special values for InitDelay
#define EXIT_BEFORE_RENDEZVOUS  (-1)
#define PANIC_BEFORE_RENDEZVOUS (-2)
#define PANIC_AFTER_RENDEZVOUS  (-3)

#define NO_PREMATURE_DEATH (-1)


typedef struct t_TestModuleIniData
	{
	TInt iInitDelay;
	TInt iDeathDelay;
	TInt iDiscoveryDelay;
	TInt iBindDelay;
	TInt iUnbindDelay;
	TInt iShutdownDelay;
	} TTestModuleIniDataContainer;

class TTestModuleIniData : public TPckg<TTestModuleIniDataContainer>
	{
public:
	TTestModuleIniData():TPckg<TTestModuleIniDataContainer>(iParams) {};
public:
	TTestModuleIniDataContainer iParams;
	};


#endif //(__ROOTSERVERTEST_H__)
