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

#ifndef __SSMCOMMANDPARAMETERS_H__
#define __SSMCOMMANDPARAMETERS_H__

#include <e32base.h>
#include <f32file.h>

#include <ssm/ssmstate.h>
#include <ssm/ssmswp.h>
#include <ssm/ssmcmd.hrh>
#include <barsread2.h>

class CResourceFile;

class TSsmCommandParameters
	{
public:
	TSsmCommandParameters(RResourceReader& aMainReader, CResourceFile* aResourceFile, TInt aConditionalInfoResourceId);
	TSsmCommandParameters(RResourceReader& aMainReader, CResourceFile* aResourceFile, TInt aConditionalInfoResourceId, const TSsmState& aState);
	TSsmCommandParameters(RResourceReader& aMainReader, CResourceFile* aResourceFile, TInt aConditionalInfoResourceId, const TSsmSwp& aSwp);
	
	RResourceReader& MainReader();
	void OpenReaderForResourceIdL(RResourceReader& aReader, TInt aResourceId);
	HBufC8* AllocBufferForResourceIdL(TInt aResourceId);
	TSsmState StateL();
	TSsmSwp SwpL();
	CResourceFile* ResourceFile();
	TInt ConditionalInformation();
private:
	enum TOptions
		{
		ENoOptionalValuesSupplied,
		EStateSupplied,
		ESwpSupplied
		};
	RResourceReader& iMainReader;
	CResourceFile* iResourceFile;
	TOptions iOptions;
	TInt iConditionalInfoResourceId;
	TSsmState iState;
	TSsmSwp iSwp;
	};

#endif // __SSMCOMMANDPARAMETERS_H__
