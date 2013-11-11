// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Entry point for the ECom dll only
// 
//

/**
 @file main.cpp
*/

#include <e32base.h>
#include <mmf/server/mmfdatasourcesink.hrh>


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfurl.h>
#include <ecom/implementationproxy.h>

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidFileSource,	CMMFFile::NewSourceL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidFileSink,		CMMFFile::NewSinkL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidDescriptorSource,	CMMFDescriptor::NewSourceL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidDescriptorSink,	CMMFDescriptor::NewSinkL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidUrlSource,	CMMFUrlSource::NewSourceL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidUrlSink,	CMMFUrlSink::NewSinkL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

