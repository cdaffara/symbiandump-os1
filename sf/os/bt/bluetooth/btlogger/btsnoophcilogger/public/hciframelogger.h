// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef HCI_FRAME_LOGGER_H
#define HCI_FRAME_LOGGER_H

#include <e32base.h>

static const TUint KHCILoggerDatalinkTypeH1		= 1001;
static const TUint KHCILoggerDatalinkTypeH4		= 1002;
static const TUint KHCILoggerDatalinkTypeBCSP	= 1003;
static const TUint KHCILoggerDatalinkTypeH5		= 1004;

static const TUint KHCILoggerHostToController	= 0;
static const TUint KHCILoggerControllerToHost	= 0x00000001;

static const TUint KHCILoggerACLDataFrame		= 0; // For future use if required
static const TUint KHCILoggerSynchronousDataFrame = 0; // For future use if required

//KHCILoggerGenericDataFrame can be used instead of the above two flags as BTSnoop logger does not 
//differencaite between the different types of data frames
static const TUint KHCILoggerGenericDataFrame 	= 0; 
static const TUint KHCILoggerCommandOrEvent		= 0x00000002;

static const TUint KHCILoggerFrameFragmented	= 0x80000000;

class CHCIFrameLoggerBase;

class MHCIFrameLogger
	{
public:
	static MHCIFrameLogger* HciLoggerLoadL(TInt aType);
	void HciLoggerUnLoad();
	// Data logging functions
	virtual void LogFrame(TUint aFrameFlags, const TDesC8& aDesc) = 0;
protected:
	// instantiation functions
	virtual void SetLibrary(RLibrary& aLib) = 0;
	virtual void GetLibrary(RLibrary& aLib) = 0;
	virtual void Initialise(TInt aType) = 0;
	virtual void Release() = 0;
	};

#ifdef ENABLE_HCI_FRAME_LOGGING

static const TUid KHCILoggerDllUid = {0x10200D9E};


#ifndef		HCI_LOG_MEMBER_NAME
#define		HCI_LOG_MEMBER_NAME		__iHciFrameLogger
#endif
#define     DECLARE_HCI_LOGGER		MHCIFrameLogger* HCI_LOG_MEMBER_NAME;
#define		HCI_LOG_LOADL(x,y)		{x->HCI_LOG_MEMBER_NAME=MHCIFrameLogger::HciLoggerLoadL(y);}
#define		HCI_LOG_UNLOAD(x)		if(x->HCI_LOG_MEMBER_NAME) {x->HCI_LOG_MEMBER_NAME->HciLoggerUnLoad();}

#define 	HCI_LOG_FRAME(x, aFrameFlags, aDesc)		if (x->HCI_LOG_MEMBER_NAME) {x->HCI_LOG_MEMBER_NAME->LogFrame(aFrameFlags, aDesc);}
#define		HCI_LOG_FRAME_IF_NO_ERROR(err, x, aFrameFlags, aDesc)	if(err == KErrNone) { 	HCI_LOG_FRAME(x, aFrameFlags, aDesc) }

inline MHCIFrameLogger* MHCIFrameLogger::HciLoggerLoadL(TInt aType)
	{
	_LIT(KHCIFrameLoggerLibName, "hciframelogger.dll");
	RLibrary lib;
	TInt err = lib.Load(KHCIFrameLoggerLibName);
	if (err == KErrNone && lib.Type()[1] == KHCILoggerDllUid)
		{
		TLibraryFunction func = lib.Lookup(1);
		MHCIFrameLogger* self = (MHCIFrameLogger*)(*func)();
		User::LeaveIfNull(self);
		self->SetLibrary(lib);
		self->Initialise(aType);
		return self;
		}
	return 0;
	}

inline void MHCIFrameLogger::HciLoggerUnLoad()
	{
	RLibrary lib;
	GetLibrary(lib);
	Release();
	lib.Close();
	}

#else // !ENABLE_HCI_FRAME_LOGGING

#define     DECLARE_HCI_LOGGER
#define		HCI_LOG_LOADL(x,y)
#define		HCI_LOG_UNLOAD(x)

#define 	HCI_LOG_FRAME(x, aFrameFlags, aDesc)
#define		HCI_LOG_FRAME_IF_NO_ERROR(err, x, aFrameFlags, aDesc)

#endif // ENABLE_HCI_FRAME_LOGGING

#endif // HCI_FRAME_LOGGER_H




