// fzip.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __FZIP_H__
#define __FZIP_H__

#include "fzipup.h"

class CZipFile;
class CZipFileMember;
class CZipFileMemberIterator;
class CCmdZip : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdZip();

	// from CCommandBase
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	CCmdZip();
	void CreateArchiveL();
	void ExpandArchiveL();

	// gz (gzip) format support
	void CreateGzArchiveL();
	void ExpandGzArchiveL();

	// zip format support
	void CreateZipArchiveL();
	void ExpandZipArchiveL();
	void ExtractZipFileL(CZipFile& aZip, const CZipFileMember& aMember);
	void AddFileL(CZipItUp& aZipArchive, const TFileName2& aFile);
private:
	// user-specified command-line options and arguments
	TBool iVerbose;
	TBool iUnzip;
	TBool iRecurse;
	enum TType
		{
		EZip,
		EGZip
		} iCompressionType;
	TFileName2 iArchive;
	TFileName2 iUnzipPath;
	RArray<TFileName2> iFileToZip;
	TBool iOverwrite;
	};


#endif // __FZIP_H__
