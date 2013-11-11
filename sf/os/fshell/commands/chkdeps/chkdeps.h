// chkdeps.h
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

#define __INCLUDE_CAPABILITY_NAMES__
#include <fshell/ioutils.h>
#include <f32image.h>
#include "sf_deflate.h"
#include <e32rom.h>
#include "sf_pgcompr.h"

using namespace IoUtils;

class CDllChecker : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);

	// From ts_std.h
private:
	enum TResultCheck {EPending,EAlreadyOpen,ECouldNotOpenFile,ENotFound,EUidNotSupported,ENoImportData,EUidDifference,EFileFoundAndUidSupported, /*BEGIN TOMSCI added*/ ERequiredCapabilitiesMissing, EOrdinalMissing, EVersionMismatch };
	class CDllInfo : public CBase
		{
	public:
		TBuf<KMaxFileName> iDllName;
		TUid iUid;
		TResultCheck iResult;
		TCapabilitySet iDllCaps;
		TInt iNumExports;
		TInt16 iMajorVersion;
		TInt16 iMinorVersion;
		};
	
	RPointerArray<CDllInfo> iDllArray;	//	Array of Imports already checked
	TInt iCalls;	//	Number of recursive calls of GetImportDataL()
	
	RFile iFile;//file object for reading data from phisical file
	TUint32  iConversionOffset;
	TAny* iStackBase; // Used in debug only to check stack depth
private:
	void GetFileNameAndUid(CDllInfo &aDllInfo, const TDesC8 &aExportName);
	TInt FindDll(TDes& aDllName,TFileName& aFileName, const TDesC& aPath);
	TUint8* NextBlock(TUint8* aBlock);
	
	void LoadFileInflateL(const E32ImageHeaderComp* aHeader, RBuf8& aCode, RBuf8& aRestOfFile);
	void LoadFileNoCompressL(const E32ImageHeaderComp* aHeader, RBuf8& aCode, RBuf8& aRestOfFile);
	void LoadFileBytePairL(const E32ImageHeaderComp* aHeader, RBuf8& aCode, RBuf8& aRestOfFile);

	TInt CheckUid3(TInt32 aUid3,TUid aUid);
	void LoadFileL(const E32ImageHeaderComp* aHeader, RBuf8& aCode, RBuf8& aRestOfFile);
	//void GetDllTableL(TUint8* aImportData,TInt aDllRefTableCount,TUint aFlags);
	//void GetDllTableL(const TDesC& aFileName, E32ImageHeader* aImageHeader, TUint8* aRestOfFile);
	void GetDllTableL(const TDesC& aFileName, const E32ImageHeader* aImageHeader, const TUint8* aCode, const TUint8* aImportData);
public:	
	CDllChecker();
	~CDllChecker();
	void ConstructL();	
	void GetImportDataL(const TDesC& aFileName, CDllInfo& aInfo);	
	void ListArray();
	static TInt CompareDllInfo(const CDllInfo& aDllInfo1, const CDllInfo& aDllInfo2);
private:
	TBool iVerbose, iDebug;
	TFileName2 iFilename;
	CDllInfo* iExeInfo; // Argh I've painted myself into an API corner here... cue quick-and-dirty hack. iRequiredCaps has to point into a CDllInfo::iDllCaps because of refactoring I made without thinking it through. Therefore jump through a few hoops to ensure this is so, to save redoing the code that assumes it. V lazy!
	TCapabilitySet* iRequiredCaps; // Only relevant if we're checking an exe
	};



// Nicked from sf_image.h

class TFileNameInfo
	{
public:
	enum	{
			EIncludeDrive=1,
			EIncludePath=2,
			EIncludeBase=4,
			EIncludeVer=8,
			EForceVer=16,
			EIncludeUid=32,
			EForceUid=64,
			EIncludeExt=128,
			EIncludeDrivePath=EIncludeDrive|EIncludePath,
			EIncludeBaseExt=EIncludeBase|EIncludeExt,
			EIncludeDrivePathBaseExt=EIncludeDrive|EIncludePath|EIncludeBase|EIncludeExt,
			};
	enum	{
			EAllowUid=1,
			EAllowPlaceholder=2,
			EAllowDecimalVersion=4,
			};
public:
	TFileNameInfo();
	TInt Set(const TDesC8& aFileName, TUint aFlags);
	void Dump() const;
public:
	inline TInt DriveLen() const {return iPathPos;}
	inline TInt PathLen() const {return iBasePos-iPathPos;}
	inline TInt BaseLen() const {return iVerPos-iBasePos;}
	inline TInt VerLen() const {return iUidPos-iVerPos;}
	inline TInt UidLen() const {return iExtPos-iUidPos;}
	inline TInt ExtLen() const {return iLen-iExtPos;}
	inline TPtrC8 Drive() const {return TPtrC8(iName, iPathPos);}
	inline TPtrC8 Path() const {return TPtrC8(iName+iPathPos, iBasePos-iPathPos);}
	inline TPtrC8 DriveAndPath() const {return TPtrC8(iName, iBasePos);}
	inline TPtrC8 Base() const {return TPtrC8(iName+iBasePos, iVerPos-iBasePos);}
	inline TPtrC8 VerStr() const {return TPtrC8(iName+iVerPos, iUidPos-iVerPos);}
	inline TPtrC8 UidStr() const {return TPtrC8(iName+iUidPos, iExtPos-iUidPos);}
	inline TPtrC8 Ext() const {return TPtrC8(iName+iExtPos, iLen-iExtPos);}
	inline TUint32 Version() const {return iVersion;}
	inline TUint32 Uid() const {return iUid;}
	void GetName(TDes8& aName, TUint aFlags) const;
public:
	const TText8* iName;
	TInt iPathPos;
	TInt iBasePos;
	TInt iVerPos;
	TInt iUidPos;
	TInt iExtPos;
	TInt iLen;
	TUint32 iVersion;
	TUint32 iUid;
	};
