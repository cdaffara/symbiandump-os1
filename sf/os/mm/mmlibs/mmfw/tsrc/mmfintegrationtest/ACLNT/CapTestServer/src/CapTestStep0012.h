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
//

#ifndef __SECUREDEVSOUNDTESTSTEP0012_H__
#define __SECUREDEVSOUNDTESTSTEP0012_H__

#include <mdaaudiosampleeditor.h>
#include <simulprocserver.h>

class CAudPlayUtilTS0012 : public CSimulProcTestStep, public MMdaObjectStateChangeObserver
	{
public:
	static CAudPlayUtilTS0012* NewL();
	void ConstructL();
	virtual void StartProcessing(TRequestStatus& aStatus);
	virtual TVerdict EndProcessingAndReturnResult(TDes8& aMessage);		
	virtual ~CAudPlayUtilTS0012();

	// from MMdaObjectStateChangeObserver	
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
private:
	TRequestStatus* iStatus;
	CMdaAudioRecorderUtility* iPlayer;
	TVerdict iVerdict;
	};
	
	
#endif	// __SECUREDEVSOUNDTESTSTEP0012_H__

