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



/**
 @file
 @test
 @internalComponent
*/

#ifndef __CTESTFADER_H__
#define __CTESTFADER_H__

#include <graphics/wsplugin.h>
#include "t_testserviceplugin.h"
#include "PARSEINIDATA.H" 
  
class MWsGraphicDrawerEnvironment;

/**
This is an implementation of MWsFader, in the form of a CWsPlugin.
The window server will use this to create test fader if the ini file specifies it.
*/
class CTestFader : public CWsPlugin, public MWsFader
  {
public:
		enum { EImplUid = 0x10285D5f };

public:
	static CTestFader* CreateL();
	~CTestFader();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& aData);
	const TDesC& PluginName() const;

public: // from MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);
	
public: // from MWsFader
	void SetFadingParameters(const TDesC8& aData);
	void FadeArea(CFbsBitGc* aBitGc,const TRegion* aRegion);
	
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
	CTestFader();

protected:
	TUint8 iBlackMap;
	TUint8 iWhiteMap;
	MWsGraphicDrawerEnvironment* iEnv;
	MWsIniFile* iWsIniFile;
	CIniData* iIniData;
	TBuf<255> iPrint;
  };

/**
CTestFader_Data derived from CTestFader with construction data
*/
class CTestFader_Data : public CTestFader
	{
public:
	enum { EImplUid = 0x10285C60 };
public:
	static CTestFader_Data* CreateL(const TDesC& aData);
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& aData);
	const TDesC& PluginName() const;
	void FadeArea(CFbsBitGc* aBitGc,const TRegion * aRegion);
	
private:
	CTestFader_Data(const TDesC& aData);
	};

/**
CTestFader_InvalidConstruct derived from CTestFader with leaving from ConstructL
*/
class CTestFader_InvalidConstruct : public CTestFader
	{
	public:
		enum { EImplUid = 0x10285C62 };
	public:
		static CTestFader_InvalidConstruct* CreateL();
		void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& aData);
		const TDesC& PluginName() const;
	};
		
#endif //__CTESTFADER_H__
