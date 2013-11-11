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
//

#ifndef __DEBUGLOG_H__
#define __DEBUGLOG_H__

#pragma warning( disable : 4710 )	//function  abc  not expanded

#include "../SERVER/w32cmd.h"

typedef TBuf<32> TShortBuf;
typedef TBuf<64> TLongBuf;

enum {LogTBufSize=0xA0};


class TDebugLogOverflow : public TDes16Overflow
	{
public:
	inline TDebugLogOverflow();
	inline void Reset();
	inline TBool IsError();
	//Pure virtual function from TDesNnOverflow
	void Overflow(TDes &aDes);
private:
	TBool iError;
	};

class TWsDecoder
	{
public:
	enum {ENewClientClass=0x9999};
	union TWsCmdUnion
		{
		const TAny *any;
		TWsClCmdUnion client;
		TWsWinCmdUnion window;
		TWsGcCmdUnion gc;
		TWsSdCmdUnion sd;
		TWsAnimDllCmdUnion anim;
		};
	TDesC &CommandBuf(TInt aApp);
	TDesC &Command(TInt aClass, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	TDesC &CommandReply(TInt aData);
	TDesC &CommandReplyBuf(const TDesC8 &aDes);
	TDesC &CommandReplyBuf(const TDesC16 &aDes);
	TDesC &NewClient(TUint aConnectionHandle);
	TDesC &SignalEvent(TInt aApp);
	TDesC &Panic(TInt aApp, TInt aReason);
	TDesC &MiscMessage(const TDesC &aFormat,TInt aParam);
	void Disable();
	void Enable();
private:
	void UnKnown();
	void UnKnownOpcode(TBuf<LogTBufSize> &aText, TInt aOpcode);
	void UnKnownReplyBuf(TBuf<LogTBufSize> &aText, TInt aOpcode);
	void AppendPoint(TBuf<LogTBufSize> &aText, const TDesC8 *aReplyDes8);
	void AppendRect(TBuf<LogTBufSize> &aText, const TDesC8 *aReplyDes8);
	void AppendDesc(TBuf<LogTBufSize> &aText, const TDesC8 *aReplyDes8);
	void AppendDesc(TBuf<LogTBufSize> &aText, const TDesC16 *aReplyDes16);
	void decodeWindow(TBuf<LogTBufSize> &aText,TInt aOpcode,const TAny *aCmdData);
	void DecodeWindowGroup(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void DecodeClient(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void DecodeWindow(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void DecodeGc(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void DecodeGc1(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData);
	void DecodeGc2(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData);
	void DecodeGc3(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData);
	void DecodeScreenDevice(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void DecodeAnimDll(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void DecodeSpriteBase(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void DecodeBitmap(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void DecodeDirect(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void DecodeClick(TBuf<LogTBufSize> &aText, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void ReplyBufWindowGroup(TBuf<LogTBufSize> &aText, TInt aOpcode, const TDesC8 *aReplyDes8, const TDesC *aReplyDesText);
	void ReplyBufWindow(TBuf<LogTBufSize> &aText, TInt aOpcode, const TDesC8 *aReplyDes8, const TDesC *aReplyDesText);
	void ReplyBufClient(TBuf<LogTBufSize> &aText, TInt aOpcode, const TDesC8 *aReplyDes8, const TDesC *aReplyDesText);
	void ReplyBufScreenDevice(TBuf<LogTBufSize> &aText, TInt aOpcode, const TDesC8 *aReplyDes8, const TDesC *aReplyDesText);
	TDesC &commandDetails(TInt aClass, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	TDesC &eventDetails(const TWsEvent *aWsEvent);
	TDesC &eventDetails(const TRawEvent *aEvent);
	TDesC &replyBufDetails(TInt aClass, TInt aOpcode, const TDesC8 *aReplyDes8, const TDesC *aReplyDesText);
private:
	TBuf<LogTBufSize> iEventDetailsText;
	TBuf<LogTBufSize> iCommandDetailsText;
	TBuf<LogTBufSize> iText;
	TUint iCommandCount;
	TUint iEventCount;
	TInt iRequestFuncClass;
	TInt iHandle;
	TInt iPrevOpcode;
	TDebugLogOverflow iOverflowHandler;
	};

class TDebugLogTextHandler
	{
public:
	enum TArrayType
		{
		EInt,
		ERgb
		};
	static void Append(TDes &aBuf1, const TDesC &aBuf2);
	static void TestAppend();
	static TBuf<20> FormatBool(TBool aBool);
	static TBuf<32> FormatPoint(const TPoint &aPoint);
	static TBuf<32> FormatSize(const TSize &aSize);
	static TBuf<64> FormatRect(const TRect &aRect);
	static TBuf<40> FormatRgb(const TRgb &aRgb);
	static TBuf<20> PointerEventType(TPointerEvent::TType aType);
	static TBuf<LogTBufSize> FormatArray(TArrayType aArrayType, const TUint8 *aArrayPtr, TUint aNumElems);
private:
	static TBuf<0x20> formatArrayElement(TArrayType aArrayType, const TUint8 *aArrayPtr);
	static void panic(TInt aReason);
	};

class CDebugLogDevice : public CBase
	{
public:
	virtual void ConstructL(TBool aIsFirst, TDesC &aParams)=0;
	virtual void WriteToLogL(const TDesC &aDes, const TDesC &aDes2)=0;
	virtual void WriteToLog8L(const TDesC8 &aDes, const TDesC8 &aDes2)=0;
	};

class CDebugLog: public CDebugLogBase
	{
private:
	enum {KNumProfiles=10};
public:
	IMPORT_C CDebugLog(CDebugLogDevice *aDevice);
	~CDebugLog();
	IMPORT_C void ConstructL(TBool aIsFirst, TDesC &aParams);
	//Pure virtual functions from CDebugLogBase
	void CommandBuf(TInt aApp);
	void Command(TInt aClass, TInt aOpcode, const TAny *aCmdData, TInt aHandle);
	void NewClient(TUint aConnectionHandle);
	void Reply(TInt aData);
	void ReplyBuf(const TDesC8 &aDes);
	void ReplyBuf(const TDesC16 &aDes);
	void SignalEvent(TInt aApp);
	void Panic(TInt aApp, TInt aReason);
	void MiscMessage(TInt aPriority,const TDesC &aFmt,TInt aParam=0);
	void HeapDump();
	void SetLoggingLevel(TInt aLevel);
	void IniFileSettingRead(TInt aScreen, const TDesC& aVarName, TBool aFound, const TDesC& aResult);

private:
	void AppendProfileNum(TDes &aDes, TInt aNum);
	void AppendProfileCount(TDes &aDes, TInt aNum);
	void LogProfiles();
	void WriteToLog(const TDesC &aDes,TInt aLevel=ELogEverything);
	void MiscMessage(const TDesC &aFmt,TInt aParam=0);

private:
	CDebugLogDevice *iDevice;
	TWsDecoder iWsDecoder;
	TBool iIsDisabled;
	TInt iErr;
	TTime iPrevTime;
	TInt iExtraLen;
	TInt iLevel;		//The current logging level
	TInt iLastApp;
	};

GLREF_C TInt hHandleToValue(TUint32 handle);


// Inline functions
/*TDebugLogOverflow*/
inline TDebugLogOverflow::TDebugLogOverflow() :iError(EFalse)
	{}
inline void TDebugLogOverflow::Reset()
	{iError=EFalse;}
inline TBool TDebugLogOverflow::IsError()
	{return iError;}

#endif

