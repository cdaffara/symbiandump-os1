// null.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "null.h"
#include "log.h"

CIoNull* CIoNull::NewLC()
	{
	CIoNull* self = new(ELeave) CIoNull();
	LOG(CIoLog::Printf(_L("Null 0x%08x created"), self));
	CleanupClosePushL(*self);
	return self;
	}

CIoNull::~CIoNull()
	{
	}

TBool CIoNull::IsType(RIoHandle::TType aType) const
	{
	return ((aType == RIoHandle::EEndPoint) || (aType == RIoHandle::ENull));
	}

void CIoNull::IorepReadL(MIoReader&)
	{
	User::Leave(KErrEof);
	}

void CIoNull::IowepWriteL(MIoWriter& aWriter)
	{
	aWriter.IowComplete(KErrNone);
	}

void CIoNull::IowepWriteCancel(MIoWriter&)
	{
	}

CIoNull::CIoNull()
	{
	}

