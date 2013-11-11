// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TSU_MMF_DRMPLUGIN_SUITE_U_H_
#define __TSU_MMF_DRMPLUGIN_SUITE_U_H_

#include <testframework.h>

/**
 *
 * CTestSuite, which is the base class for 
 * the CTestSuite_MMF_DRMPlugin.
 *
 * @lib "TSU_MMF_DRMPlugin.lib"
 *
 * @xxxx
 *
 */
class  CTestSuite_MMF_DRMPlugin : public CTestSuite
	{
public:
	
	void InitialiseL( void );
	virtual ~CTestSuite_MMF_DRMPlugin();
	virtual TPtrC GetVersion( void ) const;
	
private:

	};


/**
 *
 * Listener to CFSMNotifier's callback
 *
 * @lib "TSU_MMF_DRMPlugin.lib"
 *
 * @xxxx
 *
 */
class MFSBListener
	{
public:
	virtual void StateActivate(TInt aState) = 0;
	};

/**
 *
 * Helper class to manager the states of FSB.
 *
 * @lib "TSU_MMF_DRMPlugin.lib"
 *
 * @xxxx
 *
 */
class CFSMNotifier : public CTimer
	{
public:
	static CFSMNotifier* NewL(MFSBListener* aListener);
	virtual ~CFSMNotifier();
	// Next state event will be issued in anInterval micro-secs after this
	// call. The state machine starts the state counter from 0
	void NextState(TTimeIntervalMicroSeconds32 anInterval);
	TInt CurrentState() const { return iState; };

private:
	CFSMNotifier(MFSBListener* aListener);
	void ConstructL();
	void RunL();
private:
	TInt iState;
	MFSBListener* iListener;
	};

#endif /* TSU_MMF_DRMPLUGIN_SUITE_U_H_ */
