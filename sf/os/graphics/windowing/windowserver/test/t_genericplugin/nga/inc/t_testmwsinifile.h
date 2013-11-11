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

#ifndef T_TESTMWSINIFILE_H_
#define T_TESTMWSINIFILE_H_

#include <graphics/wsplugin.h>
#include "t_testserviceplugin.h"
#include "PARSEINIDATA.H" 

/**
This is an implementation of MWsFader, in the form of a CWsPlugin.
The window server will use this to create test fader if the ini file specifies it.
*/
class CTestMWsIniFile : public CWsPlugin
  {
public:
	enum { EImplUid = 0x10285C5F };

public:
	static CTestMWsIniFile* CreateL();
	~CTestMWsIniFile();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& aData);
	const TDesC& PluginName() const;
		
private:
	void RunMWsIniFileTestsL();
	void FindScreenVariableL(TInt aScreen, const TDesC& aIntSection, const TDesC& aStringSection);
	void FindCustomSectionVariableL(const TDesC& aSection, const TDesC& aIntSection, const TDesC& aStringSection);
	void FindDefaultVariableL(const TDesC& aIntSection, const TDesC& aStringSection);
	void FindDefaultMissingVariableL(const TDesC& aMissingVariable);
	void FindScreenMissingVariableL(TInt aScreen, const TDesC& aMissingVariable);
	void FindCustomSectionMissingVariableL(const TDesC& aSection, const TDesC& aMissingVariable);
	void FindDefaultVarOppositeMethodL(const TDesC& aAttribute, const TDesC& aType);
	void FindScreenVarOppositeMethodL(TInt aScreen, const TDesC& aAttribute, const TDesC& aType);
	void FindCustomSectionVarOppositeMethodL(const TDesC& aSection, const TDesC& aAttribute, const TDesC& aType);
	void FindNEScreenVariableL(TInt aScreen, const TDesC& aIntSection, const TDesC& aStringSection);
	void FindNESectionVariableL(const TDesC& aSection, const TDesC& aIntSection, const TDesC& aStringSection);
protected:
	CTestMWsIniFile();

protected:
	MWsIniFile* iWsIniFile;
	CIniData* iIniData;
	TBuf<255> iPrint;
  };

#endif /*T_TESTMWSINIFILE_H_*/
