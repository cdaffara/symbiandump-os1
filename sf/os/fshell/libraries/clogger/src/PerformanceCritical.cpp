// PerformanceCritical.cpp
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

#include <fshell/clogger.h>

const TInt KChunkSize = 2*1024*1024; // 2MB

#ifdef __ARMCC__
// RVCT doesn't like inlining DoLog when it's used more than once
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE 
#endif

// We use a minimum of 4KB by having any global data, so no need to scrimp or only stash a pointer

TBuf8<2048> gTempBuf;
TUint8* gTempBufPtr = NULL;

RClogger gClogger;
RChunk gChunk;
TInt* gBufSize = NULL;
TUint8* gBufPtr = NULL;

TBool gInitialised = EFalse; // Since POD is guaranteed to be initialised properly we'll use this to test whether we've already setup the global data
TBuf8<10> gTimeBuf;
TUint8* gTimeBufPtr = NULL;

enum TPanic
	{
	E16BitFunctionsNotSupported,
	ENotEnoughMemoryForBuffer,
	ECouldntConnectToClogger,
	EBufferFull,
	};

void BlowUp(TPanic aPanic)
	{
	User::Panic(_L("CloggerPerfLog"), aPanic);
	}

EXPORT_C TInt RClogger::StaticConnect(const TDesC& aTag)
	{
	if (gInitialised) return KErrNone;

	// Will do these because I not 100% certain about what guarantees the runtime provides
	// about initialisation of non-POD globals
	new(&gClogger) RClogger;
	new(&gChunk) RChunk;
	new(&gTempBuf) TBuf8<2048>;
	new(&gTimeBuf) TBuf8<10>;
	gTimeBufPtr = (TUint8*)gTimeBuf.Ptr();
	gTempBufPtr = (TUint8*)gTempBuf.Ptr();
		
	TInt err = gClogger.Connect(aTag);
	if (err)
		{
		BlowUp(ECouldntConnectToClogger);
		//gChunk.Close();
		}
	
	TAny* arg0 = &gChunk;
	err = gClogger.Reserved(0xC0FFEE, arg0, (TAny*)KChunkSize); // Get chunk from the server using the magic Reserved function
	
	if (err)
		{
		BlowUp(ENotEnoughMemoryForBuffer);
		}

	TUint8* base = gChunk.Base();
	gBufSize = reinterpret_cast<TInt*>(base); // Use first word of chunk to indicate size
	*gBufSize = 0;
	//gBuf.Set(base+4, 0, KChunkSize-4);
	gBufPtr = base+4;

#ifdef _DEBUG
	// For debugging when something failed to write to a part of the chunk
	memset(gBufPtr, 'i', KChunkSize-4);
#endif

	gInitialised = ETrue;
	return err;
	}

EXPORT_C void RClogger::StaticClose()
	{
	if (gInitialised)
		{
		gClogger.Close();
		gChunk.Close();
		gInitialised = EFalse;
		}
	}

//#ifdef __BIG_ENDIAN__
//const TUint16 KCrLf = 0x0d0a;
//#else
//const TUint16 KCrLf = 0x0a0d;
//#endif

// Use this lookup table as a quick way of hex formatting a string
char const * const KHexify = "0123456789ABCDEF";

inline void Hexify4(TUint8* aBuf, TUint32 aVal)
	{
	char const * const hexify = KHexify; // Saves compiler re-reading global
	TUint32 idx;
	idx = aVal & 0xF;
	aBuf[3] = hexify[idx];
	idx = (aVal >> 4) & 0xF;
	aBuf[2] = hexify[idx];
	idx = (aVal >> 8) & 0xF;
	aBuf[1] = hexify[idx];
	idx = (aVal >> 12) & 0xF;
	aBuf[0] = hexify[idx];
	}

void Hexify8(TUint8* aBuf, TUint32 aVal)
	{
	// Don't use the global object, it's slightly quicker to access a local variable
	// If we were going to forceinline this function it would probably be advisable to go back to using the global one
	char const * const hexify = "0123456789ABCDEF";
	TUint32 idx;
	idx = aVal & 0xF;
	aBuf[7] = hexify[idx];
	idx = (aVal >> 4) & 0xF;
	aBuf[6] = hexify[idx];
	idx = (aVal >> 8) & 0xF;
	aBuf[5] = hexify[idx];
	idx = (aVal >> 12) & 0xF;
	aBuf[4] = hexify[idx];
	idx = (aVal >> 16) & 0xF;
	aBuf[3] = hexify[idx];
	idx = (aVal >> 20) & 0xF;
	aBuf[2] = hexify[idx];
	idx = (aVal >> 24) & 0xF;
	aBuf[1] = hexify[idx];
	idx = (aVal >> 28) & 0xF;
	aBuf[0] = hexify[idx];
	}

FORCEINLINE void DoLog(TRefByValue<const TDesC8> aFmt, VA_LIST aList)
	{
	TDes8& tempBuf(gTempBuf);
	tempBuf.FormatList(aFmt, aList);
	//__ASSERT_ALWAYS(memBuf.MaxLength()-memBuf.Length() >= tempBuf.Length() + 8 + 3, BlowUp(EBufferFull)); // 8 for timebuf, 3 for the space and newlines
	// This costs 10 instructions - not much compared to FormatList, but what's the point of checking then panicking anyway?
		
	TUint8* ptr = gBufPtr;
	// Copy timebuf plus a space
	//gTimeBuf.NumFixedWidthUC(User::FastCounter(), EHex, 8);
	//memcpy(ptr, gTimeBufPtr, 8);
	Hexify8(ptr, User::NTickCount());
	ptr[8] = ' ';
	ptr += 9; // for the space too

	// And append the formatted string plus a newline
	TInt formatLen = tempBuf.Length();
	memcpy(ptr, gTempBufPtr, formatLen);
	ptr += formatLen;
	//*((TUint16*)ptr) = KCrLf;
	// ^ Can't do the above because of alignment issues on ARM
	ptr[0] = '\r';
	ptr[1] = '\n';
	gBufPtr = ptr + 2;
	*gBufSize += formatLen + 11; // 8 for timeBuf, 3 for space&CRLF
	}

EXPORT_C void RClogger::Slog(TUint32 /*aLogMask*/, TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	DoLog(aFmt, args);
	}

EXPORT_C void RClogger::Slog(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	DoLog(aFmt, args);
	}

EXPORT_C void RClogger::Slog(const char* aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	TPtrC8 ptr((const TUint8*)aFmt);
	DoLog(ptr, args);
	}

EXPORT_C void RClogger::Slog(TUint32 /*aLogMask*/, const char* aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	TPtrC8 ptr((const TUint8*)aFmt);
	DoLog(ptr, args);
	}

EXPORT_C void RClogger::SlogList(TRefByValue<const TDesC8> aFmt, VA_LIST aList)
	{
	DoLog(aFmt, aList);
	}

EXPORT_C void RClogger::SlogList(const char* aFmt, VA_LIST aList)
	{
	TPtrC8 ptr((const TUint8*)aFmt);
	DoLog(ptr, aList);
	}

EXPORT_C void RClogger::SlogList(TUint32 /*aLogMask*/, TRefByValue<const TDesC8> aFmt, VA_LIST aList)
	{
	DoLog(aFmt, aList);
	}

EXPORT_C void RClogger::SlogList(TUint32 /*aLogMask*/, const char* aFmt, VA_LIST aList)
	{
	TPtrC8 ptr((const TUint8*)aFmt);
	DoLog(ptr, aList);
	}

// Screw the 16-bit versions, if you want performance you should be logging in 8-bit
EXPORT_C void RClogger::Slog(TRefByValue<const TDesC>, ...) { BlowUp(E16BitFunctionsNotSupported); }
EXPORT_C void RClogger::Slog(TUint32, TRefByValue<const TDesC>, ...) { BlowUp(E16BitFunctionsNotSupported); }
EXPORT_C void RClogger::SlogList(TRefByValue<const TDesC>, VA_LIST) { BlowUp(E16BitFunctionsNotSupported); }
EXPORT_C void RClogger::SlogList(TUint32, TRefByValue<const TDesC>, VA_LIST) { BlowUp(E16BitFunctionsNotSupported); }

// Doesn't need to do anything
EXPORT_C void RClogger::SetStaticLogBehaviour(TUint /*aLogBehaviour*/)
	{
	}

/*

Actually, the compiler's may not be optimal but it uses one less register, so avoids accessing main mem as much!

__NAKED__ void Hexify4(TUint8* aBuf, TUint32 aVal, TUint8 const*const aHexify)
	{
	// Compiler refuses to generate optimal assembly, so screw it

	// r0 is aBuf, r1 is aVal, r2 is hexify
	// r4 is 0xF for convenience of ANDing
	asm("push {r4}");
	asm("mov r4, #0xF");

	asm("and r3, r4, r1"); // r3 = aVal & 0xF
	asm("ldrb r3, [r2, r3]"); // r3 = aHexify[r3]
	asm("strb r3, [r0, #3]"); // aBuf[3] = r3

	asm("and r3, r4, r1, lsr #4"); // r3 = 0xF & (aVal >> 4)
	asm("ldrb r3, [r2, r3]"); // r3 = aHexify[r3]
	asm("strb r3, [r0, #2]"); // aBuf[2] = r3
	
	asm("and r3, r4, r1, lsr #8"); // r3 = 0xF & (aVal >> 8)
	asm("ldrb r3, [r2, r3]"); // r3 = aHexify[r3]
	asm("strb r3, [r0, #1]"); // aBuf[1] = r3

	asm("and r3, r4, r1, lsr #16"); // r3 = 0xF & (aVal >> 16)
	asm("ldrb r3, [r2, r3]"); // r3 = aHexify[r3]
	asm("strb r3, [r0, #0]"); // aBuf[0] = r3

	asm("pop {r4}");
	asm("bx lr");
	}
*/
		
FORCEINLINE void DoHexDump(const TDesC8& aHeader, const TDesC8& aData)
	{
	//__DEBUGGER();
	TUint8 * const origBufPtr = gBufPtr;

	const TInt KLineSize = 16;
	const TInt KExtra = 19; // 8-digit timestamp, space, 4-digit idx, space colon space, space, CRLF
	TUint32 tickCount = User::NTickCount();
	TUint8 timestampBuf[8];
	//TPtr8(timestampBuf, 8, 8).NumFixedWidthUC(tickCount, EHex, 8); // TODO replace with custom hexifier?
	Hexify8(timestampBuf, tickCount);

	TUint8 const * dataPtr = aData.Ptr();
	TUint8 const * const dataEnd = dataPtr + aData.Length();
	TInt headerLen = aHeader.Length();
	// Rather than messing around with putting spaces in where needed, fill the whole buffer with spaces. It'll probably be more efficient to do one big fill like this
	memset(origBufPtr, ' ', (headerLen + 4*KLineSize + KExtra)*(aData.Length()+8)/16); // I think this is linelen*numLines

	// Do "Timestamp Header : "
	memcpy(origBufPtr, timestampBuf, 8);
	TUint8* bufPtr = origBufPtr + 9;
	memcpy(bufPtr, aHeader.Ptr(), headerLen);
	bufPtr += headerLen;

	TInt offset = 0;
	while (dataPtr < dataEnd)
		{
		const TInt fragLen = Min(KLineSize, dataEnd-dataPtr);
		
		if (offset)
			{
			// Write continuation empty space
			//bufPtr = memset(bufPtr, ' ', headerLen);
			memcpy(bufPtr, timestampBuf, 8);
			bufPtr += 9 + headerLen; // 8-byte timestamp, space
			}

		// Do "0000 : "
		//gTempBuf.NumFixedWidthUC(offset++, EHex, 4);
		//memcpy(bufPtr, gTempBufPtr, 4);
		Hexify4(bufPtr, offset);
		offset += 16;

		bufPtr += 4;
		//bufPtr[0] = ' ';
		bufPtr[1] = ':';
		//bufPtr[2] = ' ';
		bufPtr += 3;

		// Do the hexdump of this line's data
		TInt i = fragLen;
		while(i--)
			{
			TUint8 c = dataPtr[i];
			TInt idx = i*3;
			//TInt idx = fragLen<<1 + fragLen;
			bufPtr[idx] = KHexify[c>>4];
			bufPtr[idx+1] = KHexify[c & 0xF];
			//bufPtr[idx+2] = ' ';
			}
		bufPtr += KLineSize*3 + 1; // Plus 1 for the extra space char between the hex rep and the nonhex
		memcpy(bufPtr, dataPtr, fragLen);
		bufPtr[fragLen] = '\r';
		bufPtr[fragLen+1] = '\n';

		// Now escape anything nonprintable with '.'
		i = fragLen;
		while (i--)
			{
			TUint8 c = bufPtr[i];
			if (c < 0x20 || c > 0x7E) bufPtr[i] = '.';
			}
		bufPtr += fragLen + 2; // For the newline
		dataPtr += fragLen;
		}
	gBufPtr = bufPtr;
	*gBufSize += gBufPtr - origBufPtr;
	}

EXPORT_C void RClogger::StaticHexDump(TUint32 /*aLogMask*/, const TDesC8& aHeader, const TDesC8& aData)
	{
	//TODO
	//gClogger.HexDump(aLogMask, aHeader, aData);
	DoHexDump(aHeader, aData);
	}

EXPORT_C void RClogger::StaticHexDump(const TDesC8& aHeader, const TDesC8& aData)
	{
	//TODO
	//gClogger.HexDump(EAllEnabled, aHeader, aData);
	DoHexDump(aHeader, aData);
	}
