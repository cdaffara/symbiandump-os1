// commands.h
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


#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <fshell/common.mmh>
#include <fshell/consoleextensions.h>
#include "parser.h"
#include <hal.h>
#include <fshell/ioutils.h>
#include <fshell/memoryaccesscmd.h>

using namespace IoUtils;


class CCmdHelp : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdHelp();
private:
	CCmdHelp();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	IoUtils::CTextFormatter* iFormatter;
	TBool iCount;
	};


// Note, this command should never execute as 'exit' has handled explicitly by CParser.
// It exists so that 'exit' appears in fshell's help list and also to support 'exit --help'.
class CCmdExit : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdExit();
private:
	CCmdExit();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	};


class MDirReaderObserver
	{
public:
	virtual void DirReadComplete(TInt aError) = 0;
	};


class CCmdLs : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdLs();
private:
	CCmdLs();
	void ConstructL();
	void PrintDirContentL(const CDir& aDir);
	void PrintDirContentL(RDir& aDir);
	void RecursiveScanDirL(const TDesC& aName);
	void DoScanDirL();
	void FormatEntryL(const TEntry& aEntry);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TFileName2 iFileName;
	TPtrC iBaseDir;
	TBuf<256> iTempBuf;
	CTextFormatter* iFormatter;
	TBool iOptAll;
	TBool iOptLong;
	TBool iOptHuman;
	TBool iOptOnePerLine;
	TBool iOptRecurse;
	};


class CCmdCd : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdCd();
private:
	CCmdCd();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TFileName2 iDir;
	};


class CCmdClear : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdClear();
private:
	CCmdClear();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);

private:
	TBool iFormFeed;
	};


class CForegroundAdjuster;

class CCmdFg : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdFg();
private:
	CCmdFg();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iJobSpec;
	CForegroundAdjuster* iForegroundAdjuster;
	};


class CCmdBg : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdBg();
private:
	CCmdBg();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iJobSpec;
	CForegroundAdjuster* iForegroundAdjuster;
	};


class CCmdJobs : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdJobs();
private:
	CCmdJobs();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
private:
	CTextFormatter* iFormatter;
	};


class CCmdRm : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdRm();
private:
	CCmdRm();
	TInt DoDelete(const TDesC& aFileName);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	RArray<TFileName2> iFileNames;
	CFileMan* iFileMan;
	TBool iRecurse;
	TBool iForce;
	RPointerArray<HBufC> iNonExpandedFilenames; // This is to prevent the normal behaviour of fshell expanding a '*' in iFileNames, in the case where the number of matches would be huge
	};


class CCmdCp : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdCp();
private:
	CCmdCp();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	void ForciblyCopyFileL(const TDesC& aSourceFileName, const TDesC& aDestFileName);
private:
	TFileName2 iFrom;
	TFileName2 iTo;
	CFileMan* iFileMan;
	TBool iRecurse;
	TBool iForce;
	TBool iOverwrite;
	};


class CCmdMv : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdMv();
private:
	CCmdMv();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TFileName2 iFrom;
	TFileName2 iTo;
	CFileMan* iFileMan;
	};


class CCmdMkDir : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdMkDir();
private:
	CCmdMkDir();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TFileName2 iDir;
	TBool iAllowExists;
	};


class CCmdRmDir : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdRmDir();
private:
	CCmdRmDir();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	RArray<TFileName2> iDirs;
	CFileMan* iFileMan;
	TBool iRecurse;
	};


class CCmdMatch : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdMatch();
private:
	CCmdMatch();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iToMatch;
	TBool iIgnoreCase;
	TBool iInvertMatch;
	TBool iCount;
	};


class CCmdEcho : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdEcho();
private:
	CCmdEcho();
	void DoWriteL(const TDesC& aDes);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	enum TAttr
		{
		EBold,
		EUnderscore,
		EBlink,
		EInverse,
		EConceal
		};
private:
	HBufC* iToEcho;
	TBool iToStderr;
	TBool iWrap;
	TUint iIndent;
	RArray<TInt> iAttributes;
	ConsoleAttributes::TColor iForegroundColor;
	ConsoleAttributes::TColor iBackgroundColor;
	TBool iBinaryMode;
	TBool iNoNewline;
	};


class CCmdMore : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdMore();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	CCmdMore();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
private:
	TFileName2 iFile;
	};


class CCmdTrace : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTrace();
private:
	CCmdTrace();
	void PrintConfig();
	TInt SetFlag(const TDesC& aFlagName, TBool aSet);

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TUint iMask;
	TUint iIndex;
	TBool iOptF32;
	TBool iOptMultiThread;
	TBool iOptLoader;
	TBool iOptFat;
	TBool iOptLffs;
	TBool iOptIso9660;
	TBool iOptNtfs;
	TBool iOptRofs;
	TBool iOptCompfs;
	RPointerArray<HBufC> iEnable;
	RPointerArray<HBufC> iDisable;
	};


class CCmdMemInfo : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdMemInfo();
private:
	CCmdMemInfo();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	void AppendLineL(CTextFormatter& aFormatter, const TDesC& aCaption, HALData::TAttribute aHalAttribute, TBool aNewLine);
private:
	TUint iUpdateRate;
	TBool iHumanReadable;
	TBool iOnlyFreeRam;
	TBool iOnlyTotalRam;
	TInt iLastTotalRamUsage;
	};


class CCmdDump : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdDump();
private:
	CCmdDump();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	RArray<TFileName2> iFileNames;
	TBool iBinaryMode;
	};


class CCmdSleep : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSleep();
private:
	CCmdSleep();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TUint iDuration;
	};


class CCmdEnv : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdEnv();
private:
	CCmdEnv();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	};


class CCmdExport : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdExport();
private:
	CCmdExport();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iKey;
	HBufC* iVal;
	TBool iStdin;
	TBuf<512> iBuf;
	};


class CCmdSort : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSort();
private:
	CCmdSort();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TBool iReverse;
	};


class CCmdExists : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdExists();
private:
	CCmdExists();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TFileName2 iFileName;
	};


class CCmdInfoPrint : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdInfoPrint();
private:
	CCmdInfoPrint();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iToPrint;
	};


class CCmdRDebug : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdRDebug();
private:
	CCmdRDebug();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iToPrint;
	};


class CCmdDate : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdDate();
private:
	CCmdDate();
	void Display(const TTime& aTime);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TBool iUniversalTime;
	HBufC* iDateToSet;
	TInt iUtcOffset;
	TBool iSecure;
	TBool iRaw;
	TBool iJustDisplay;
	TInt64 iRawTimeToSet;
	TBool iUseTimestampFormat;
	TBool iUseKernelFormat;
	};


class CCmdFsck : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdFsck();
private:
	CCmdFsck();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iDriveLetter;
	};


class CCmdDriver : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdDriver();
private:
	enum TOperation
		{
		ELoad,
		EFree,
		EFind
		};
	enum TType
		{
		ELogical,
		EPhysical
		};
private:
	CCmdDriver();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TOperation iOperation;
	TType iType;
	HBufC* iDriverName;
	};


class CCmdChunkInfo : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdChunkInfo();
private:
	CCmdChunkInfo();
	void DoPrintL();
	void ListChunksL();
	void PrintChunkInfoL();
	void PrintSizeL(const TDesC& aCaption, TInt aSize);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TUint iAddress;
	TUint iUpdateRate;
	TBool iHumanReadable;
	TBool iIncludeSize;
	HBufC* iOwningProcess;
	TUint iControllingProcess;
	TName iName;
	TFullName iFullName;
	CTextFormatter* iFormatter;
	CTextBuffer* iBuf;
	};


class CCmdSvrInfo : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSvrInfo();
private:
	CCmdSvrInfo();
	void ListServersL();
	void ListSessionsL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TUint iAddress;
	TName iName;
	TFullName iFullName;
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	TServerKernelInfo iServerInfo;
	TThreadKernelInfo iThreadInfo;
	TProcessKernelInfo iProcessInfo;
	TSessionKernelInfo iSessionInfo;
#endif
	CTextFormatter* iFormatter;
	CTextBuffer* iBuf;
	};


class CCmdTickle : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTickle();
private:
	CCmdTickle();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	};


class CCmdTicks : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTicks();
private:
	CCmdTicks();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TBool iVerbose;
	};


class CCmdUpTime : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdUpTime();
private:
	CCmdUpTime();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TBool iHuman;
	};


class CCmdVar : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdVar();
private:
	CCmdVar();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	enum
		{
		EDefined,
		ENotDefined,
		EEqual,
		ENotEqual,
		EAdd,
		ESubtract,
		EMultiply,
		} iOperation;
	HBufC* iVar1;
	HBufC* iArg;
	};


class CCmdSource : public CCommandBase, public MParserObserver
	{
public:
	static CCommandBase* NewLC();
	~CCmdSource();
protected:
	CCmdSource();
private:
	void BackupVarL(const TDesC& aKey, HBufC*& aBuf);
	void RestoreVarL(const TDesC& aKey, const HBufC* aBuf);
protected: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private: // From MParserObserver.
	virtual void HandleParserComplete(CParser& aParser, const TError& aError);
protected:
	TFileName2 iFileName;
	HBufC* iArgs;
	TBool iKeepGoing;
	HBufC* iScriptData;
	CParser* iParser;
	};


class CCmdStart : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdStart();
private:
	CCmdStart();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TBool iRendezvous;
	TBool iWait;
	TInt iTimeout;
	HBufC* iExe;
	HBufC* iCommandLine;
	TBool iMeasure;
	};


class CCmdCompare : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdCompare();
private:
	CCmdCompare();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TFileName2 iFileName1;
	TFileName2 iFileName2;
	TBool iVerbose;
	};


class CCmdTime : public CCommandBase, public MParserObserver
	{
public:
	static CCommandBase* NewLC();
	~CCmdTime();
private:
	CCmdTime();
	void NextIterationL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private: // From MParserObserver.
	virtual void HandleParserComplete(CParser& aParser, const TError& aError);
private:
	HBufC* iCommandLine;
	TUint32 iCountBefore;
	CParser* iParser;
	TBool iHuman;
	TBool iFastCounter;
	TInt iRepeatCount;
	TInt iIteration;
	TUint64 iTotalTime;
	};


class CCmdRepeat : public CCommandBase, public MParserObserver
	{
public:
	static CCommandBase* NewLC();
	~CCmdRepeat();
private:
	CCmdRepeat();
	void CreateParserL();
	void HandleParserCompleteL(const TError& aError);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private: // From MParserObserver.
	virtual void HandleParserComplete(CParser& aParser, const TError& aError);
private:
	HBufC* iCommandLine;
	CParser* iParser;
	TUint iCount;
	TUint iNumRepeats;
	TBool iKeepGoing;
	TBool iForever;
	TInt iWaitTime;
	};


class CCmdDebug : public CCommandBase, public MParserObserver
	{
public:
	static CCommandBase* NewLC();
	~CCmdDebug();
private:
	CCmdDebug();
	void InteractL(const TDesC& aExpandedLine);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private: // From MParserObserver.
	virtual void HandleParserComplete(CParser& aParser, const TError& aError);
	virtual void AboutToExecuteLine(const TDesC& aOrignalLine, const TDesC& aExpandedLine);
	virtual void LineReturned(TInt aError);
private:
	TFileName2 iFileName;
	HBufC* iArgs;
	TBool iKeepGoing;
	HBufC* iScriptData;
	CParser* iParser;
	};


class CCmdReadMem : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdReadMem();
private:
	CCmdReadMem();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TUint iAddress;
	TUint iSize;
	TFileName2 iFileName;
	TInt iThreadId;
	RFile iFile;
	};


class CCmdE32Header : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdE32Header();
private:
	CCmdE32Header();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TFileName2 iFileName;
	TBool iXip;
	TBool iNotXip;
	CTextFormatter* iFormatter;
	};


class CCmdObjInfo : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdObjInfo();
private:
	CCmdObjInfo();
	void PrintObjectDetailsL(TUint aObjectAddress);
	void PrintObjectReferencersL(TUint aObjectAddress);
	void PrintReferencedObjectDetailsL(TOwnerType aOwnerType, TUint aId);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TUint iObjectAddress;
	TBool iReferencers;
	TUint iProcessId;
	TUint iThreadId;
	TBool iAll;
	};

class CCmdTouch : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTouch();
private:
	CCmdTouch();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TFileName2 iFileName;
	};

class CCmdDialog : public CCommandBase, public MCommandExtensionsV2
	{
public:
	static CCommandBase* NewLC();
	~CCmdDialog();
private:
	CCmdDialog();
	void ClearLineL(RIoConsoleWriteHandle& aWriteHandle);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private: // From CActive
	void RunL();
	void DoCancel();
private: // From MCommandExtensionsV2
	void CtrlCPressed();

private:
	HBufC* iTitle;
	HBufC* iBody;
	HBufC* iButton1;
	HBufC* iButton2;
	enum TMode
		{
		EModeNotifier,
		EModeConsole,
		EModeNull
		};
	TMode iMode;

	RNotifier iNotifier;
	TInt iReturnValue;
	};

#ifdef __WINS__

class CCmdJit : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdJit();
private:
	CCmdJit();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	enum
		{
		EStatus,
		EOn,
		EOff
		} iOperation;
	};

#endif // __WINS__

class CCmdConsole : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdConsole();
private:
	CCmdConsole();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TBool iVerbose;
	TBool iIsRemote;
	TBool iIsNull;
	};
	
class CCmdPcons : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdPcons();
private:
	CCmdPcons();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	
	void CreateL(RIoPersistentConsole& aPcons);
	void AttachL(RIoPersistentConsole& aPcons, const TDesC& aNewOrExisting, TBool aForce);
	TInt DoAttach(RIoPersistentConsole& aPcons, RIoConsole& aNew, RIoPersistentConsole::TCloseBehaviour aOnClose);
private:
	enum
		{
		EList,
		ENew,
		EConnect,
		EStart,
		EDisconnect,
		} iOperation;
	HBufC* iName;
	HBufC* iCommand;
	HBufC* iCommandArgs;
	TBool iVerbose;
	};

class CCmdIoInfo : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdIoInfo();
private:
	CCmdIoInfo();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RIoHandle::TType iObjectType;
	HBufC* iMatchString;
	};

class CCmdReattach : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdReattach();
private:
	CCmdReattach();
	void OpenEndPointLC(RIoEndPoint& aEndPoint, const TDesC& aName);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iJobSpec;
	HBufC* iStdinEndPointName;
	HBufC* iStdoutEndPointName;
	HBufC* iStderrEndPointName;
	};

class CCmdDisown : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdDisown();
private:
	CCmdDisown();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iJobSpec;
	};

class CCmdDebugPort : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdDebugPort();
private:
	CCmdDebugPort();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TInt iPort;
	TBool iForce;
	};

class CCmdRom : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdRom();
private:
	CCmdRom();
	void PrintIntL(TInt32 aInt32, const TDesC& aLabel, const TDesC& aDescription);
	void PrintIntL(TInt64 aInt64, const TDesC& aLabel, const TDesC& aDescription);
	void PrintUintL(TUint32 aUint32, const TDesC& aLabel, const TDesC& aDescription);
	void PrintUintL(TUint64 aUint64, const TDesC& aLabel, const TDesC& aDescription);
	void PrintAddressL(TLinAddr aAddress, const TDesC& aLabel, const TDesC& aDescription);
	void PrintSizeL(TInt32 aSize, const TDesC& aLabel, const TDesC& aDescription);
	void PrintTimeL(TInt64 aTime, const TDesC& aLabel, const TDesC& aDescription);
	void PrintVersionL(TVersion aVersion, const TDesC& aLabel, const TDesC& aDescription);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	CTextBuffer* iBuffer;
	TBool iVerbose;
	TBool iHumanReadable;
	};

class CCmdWhich : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdWhich();
private:
	CCmdWhich();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	
private:
	HBufC* iCommand;
	};

class CCmdTee : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTee();
private:
	CCmdTee();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	
private:
	RArray<TFileName2> iFileNames;
	RArray<RFile> iFiles;
	TBool iAppend;
	TBool iRdebug;
	};

class CCmdError : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdError();
private:
	CCmdError();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	
private:
	TInt iErrorVal;
	HBufC* iErrorText;
	};

class CCmdReboot : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdReboot();
private:
	CCmdReboot();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TUint iReason;
	};
	
class CCmdForEach : public CCmdSource
	{
public:
	static CCommandBase* NewLC();
	~CCmdForEach();
private:
	CCmdForEach();
	void DoNextL(TBool aFirstTime=EFalse);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private: // From MParserObserver.
	virtual void HandleParserComplete(CParser& aParser, const TError& aError);
private:
	TFileName2 iDirName;
	RDir iDir;
	TInt iLastError;
	};
	
#endif // __COMMANDS_H__
