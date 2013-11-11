// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __STDRENDERSTAGE_H__
#define __STDRENDERSTAGE_H__

#include "Graphics/WsRenderStage.h"

class MWsObjectProvider;
class MWsGraphicDrawerEnvironment;
class MWsFrontBuffer;
class CFbsBitGc;

/**
This is the implementation of CWsRenderStage which is created by a CStdRenderStageFactory
*/
class CStdRenderStage : public CWsRenderStage
	{
public:
	static CStdRenderStage * NewL(MWsGraphicDrawerEnvironment * aEnv,MWsScreen * aScreen);
	virtual ~CStdRenderStage();

public: // implementation of MWsRenderStage
	virtual CFbsBitGc * Begin();
	virtual void End();	

private:
	CStdRenderStage();
	void ConstructL(MWsGraphicDrawerEnvironment * aEnv,MWsScreen * aScreen);
	
private:
	MWsGraphicDrawerEnvironment * iEnv;
	MWsScreen * iScreen;
	MWsFrontBuffer * iFrontBuffer;
	};

#endif //__STDRENDERSTAGE_H__
