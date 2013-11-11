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

//#include <ssm/ssmcommandlistresourcereader.h>
//#include <ssm/ssmcommandlist.h>
//#include <ssm/ssmconditionalcallback.h>

#include <barsc2.h>
#include <barsread2.h>
#include "ssmcommandparameters.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
//#include "ssmcommandlistresourcereaderimpl.h"
//#include "ssmcommandlistimpl.h"

TSsmCommandParameters::TSsmCommandParameters(RResourceReader& aMainReader, CResourceFile* aResourceFile, TInt aConditionalInfoResourceId)
	: iMainReader(aMainReader), iResourceFile(aResourceFile), iOptions(ENoOptionalValuesSupplied), iConditionalInfoResourceId(aConditionalInfoResourceId), iState(0, 0), iSwp(0, 0)
	{
	}

TSsmCommandParameters::TSsmCommandParameters(RResourceReader& aMainReader, CResourceFile* aResourceFile, TInt aConditionalInfoResourceId, const TSsmState& aState)
	: iMainReader(aMainReader), iResourceFile(aResourceFile), iOptions(EStateSupplied), iConditionalInfoResourceId(aConditionalInfoResourceId), iState(aState), iSwp(0, 0)
	{
	}

TSsmCommandParameters::TSsmCommandParameters(RResourceReader& aMainReader, CResourceFile* aResourceFile, TInt aConditionalInfoResourceId, const TSsmSwp& aSwp)
	: iMainReader(aMainReader), iResourceFile(aResourceFile), iOptions(ESwpSupplied), iConditionalInfoResourceId(aConditionalInfoResourceId), iState(0, 0), iSwp(aSwp)
	{
	}

RResourceReader& TSsmCommandParameters::MainReader()
	{
	return iMainReader;
	}

void TSsmCommandParameters::OpenReaderForResourceIdL(RResourceReader& aReader, TInt aResourceId)
	{
	aReader.OpenL(iResourceFile, aResourceId);
	}

HBufC8* TSsmCommandParameters::AllocBufferForResourceIdL(TInt aResourceId)
	{
	if (iResourceFile)
		{
		return iResourceFile->AllocReadL(aResourceId);
		}
		
	return NULL;
	}

TSsmState TSsmCommandParameters::StateL()
	{
	SSMLOGLEAVEIFFALSE(iOptions == EStateSupplied, KErrNotSupported);
	return iState;
	}

TSsmSwp TSsmCommandParameters::SwpL()
	{
	SSMLOGLEAVEIFFALSE(iOptions == ESwpSupplied, KErrNotSupported);
	return iSwp;
	}

CResourceFile* TSsmCommandParameters::ResourceFile()
	{
	return iResourceFile;
	}

TInt TSsmCommandParameters::ConditionalInformation()
	{
	return iConditionalInfoResourceId;
	}
