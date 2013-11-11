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
// agentnotifierdefs.h - agent Notifier the information
// passing classes/enums
//


#ifndef _AGENTNOTIFIERDEFS_H_
#define _AGENTNOTIFIERDEFS_H_

const TUid KUidAgentDialogNotifier = { 0x101FD0CE};
const TUid KScreenOutputChannel ={0x00000666}; //notifier should go to a dialog channel, not a led or sound channel


enum TAgentDialog
	{
	ENoYesDialog = 1,
	EContinueDialog,
	EError
	};

enum TAgentDialogResult 
	{ 
	EAgentNoPressed = 2,
	EAgentYesPressed =3,
	EAgentContinuePressed
	};


class TAgentDialogInput
{
public:
	enum TBufferSize
		{
		EGenericBufferSize=80
		};

	TAgentDialogInput(){}

	TInt getDialogOperation(){return iDialog;}

public:
	TAgentDialog iDialog;
	TBuf<EGenericBufferSize> iTitle;
	TBuf<EGenericBufferSize> iLabel;
	
};

typedef TPckgBuf<TAgentDialogInput> TAgentDialogInputBuf;
typedef TPckgBuf<TInt> TAgentDialogOutputBuf;

#endif

