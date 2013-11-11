// extrabtrace.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef EXTRABTRACE_H
#define EXTRABTRACE_H

#include <fshell/common.mmh>

#ifdef __KERNEL_MODE__
#include <e32def.h>
#else
#include <e32std.h>
#endif

namespace ExtraBTrace
	{

	enum TCategory
		{
		/**
		Supported by memoryaccess-fshell.ldd
		*/
		EPubSub = 213,

		/**
		Filesystem tracing from ffstracer (fshell addition).
		*/
		EFfsTrace = 214,

		};

	enum TPubSub
		{
		EPubSubIntPropertyChanged = 0,
		EPubSubDataPropertyChanged = 1,
		};

	// As used by category EFfsTrace
	enum TFfsTrace
		{
		EFfsDelete = 0,
		EFfsRename,
		EFfsReplace,
		EFfsEntry,
		EFfsFileSubClose,
		EFfsFileOpen,
		EFfsFileCreate,
		EFfsFileReplace,
		EFfsFileTemp,
		EFfsFileRename,

		EFfsPost = 128,
		};
	}

#ifndef __KERNEL_MODE__

class RExtraBtrace
	{
public:
	IMPORT_C RExtraBtrace();
	IMPORT_C TInt Connect();
	IMPORT_C void Close();

	IMPORT_C TInt EnableProfiling(TInt aSamplingPeriod); // in milliseconds. Pass aRate of zero to disable

public:
	IMPORT_C TInt Handle() const;

private:
	TInt32 iImpl[5];
	};

#endif // __KERNEL_MODE__

// Include the actual btrace header - do this after RExtraBtrace so that in principle someone could subclass it in their custom header
#ifdef FSHELL_CUSTOM_BTRACE_HEADER
#include FSHELL_CUSTOM_BTRACE_HEADER
#else
#include <e32btrace.h>
#endif

#endif // EXTRABTRACE_H
