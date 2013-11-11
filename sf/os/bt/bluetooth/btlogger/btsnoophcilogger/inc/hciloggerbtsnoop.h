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

#ifndef HCI_LOGGER_BT_SNOOP_H
#define HCI_LOGGER_BT_SNOOP_H

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <bluetooth/hciframelogger.h>

enum TBtSnoopLoggerPanicCodes
	{
	EBtSnoopLoggerPanicNoMyLibHandle,
	EBtSnoopLoggerPanicHandleAlreadyExists,
	EBtSnoopLoggerPanicRefCountTooLow,
	EBtSnoopLoggerPanicHasntBeenInitialised,
	EBtSnoopLoggerPanicShouldNotBeInitialised,
	};

class TBtSnoopPacketRecord
	{
public:
	TUint32	iOriginalLength;
	TUint32	iIncludedLength;
	TUint32	iPacketFlags;
	TUint32	iCumulativeDrops;
	TInt64	iTimestampMicroseconds;
	};

class CHCILoggerFileCloseTimer;
class CHCILoggerBufferedFrame;

class CHCILoggerBtSnoop : public CBase, private MHCIFrameLogger
	{
public:
	IMPORT_C static MHCIFrameLogger* GetInstanceL();
	static CHCILoggerBtSnoop* NewLC();
	~CHCILoggerBtSnoop();
	
private:
	// From MHCIFrameLogger
	virtual void SetLibrary(RLibrary& aLib);
	virtual void GetLibrary(RLibrary& aLib);
	virtual void Release();
	virtual void Initialise(TInt aType);
	virtual void LogFrame(TUint aFrameFlags, const TDesC8& aDesc);

	void DoInitialise(TInt aType);
	CHCILoggerBtSnoop();
	void ConstructL();

	TInt OpenFile();
	TInt FileWriteHeader();
	void FileWriteRecord(const TBtSnoopPacketRecord& aRecord, const TDesC8& aBufferedData, const TDesC8& aNewData);
	void FileWriteRecord(const TBtSnoopPacketRecord& aRecord, const TDesC8& aNewData); // overload

private:
	TInt iRefCount;
	RLibrary iMyLib;
	TBool iInitialised;

	RFs iFs;
	RFile iLogFile;
	CHCILoggerFileCloseTimer* iFileCloseTimer;

	RPointerArray<CHCILoggerBufferedFrame> iBuffers;

	TTime iCurrentTime;
	TUint iDatalinkType;
	TUint iCumulativeDrops;
	};

class CHCILoggerFileCloseTimer : public CTimer
	{
public:
	static CHCILoggerFileCloseTimer* NewL(RFile& aLogFile);
	void RunL();
	void DoCancel();
	void ResetTimer();
	~CHCILoggerFileCloseTimer();
private:
	CHCILoggerFileCloseTimer(RFile& aLogFile);
	void ConstructL();
private:
	RFile& iLogFile;
	};

class CHCILoggerBufferedFrame : public CBase
	{
public:
	CHCILoggerBufferedFrame();
	~CHCILoggerBufferedFrame();
	void BufferData(const TDesC8& aDesc);
	void Reset();
	TInt OriginalLength() const;
	TDesC8& Buffer() const;
private:
	TInt iOriginalLength;
	HBufC8* iBuffer;
	};

#endif // HCI_LOGGER_BT_SNOOP_H
