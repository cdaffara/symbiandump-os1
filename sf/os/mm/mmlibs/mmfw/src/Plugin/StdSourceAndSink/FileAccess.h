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

#ifndef __FILE_ACCESS_H__
#define __FILE_ACCESS_H__

#include <caf/content.h>
#include <caf/data.h>
#include <mmf/common/mmcaf.h>

// Abstract class providing generic file access
class MGenericFile
	{
public:
	virtual ~MGenericFile();

	virtual TInt Seek(TSeek aSeekMode, TInt aPosition)=0;
	virtual TInt Read(TDes8& aDes,TInt aLength)=0;
	virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus)=0;

	virtual TInt Write(const TDesC8& aDes,TInt aLength)=0;
	virtual void Write(const TDesC8& aDes, TInt aLength, TRequestStatus& aStatus)=0;

	virtual TInt Size(TInt& aSize)=0;
	virtual TInt SetSize(TInt aSize)=0;

	virtual TInt EvaluateIntent(const ContentAccess::TIntent aIntent) const = 0;
	virtual TInt ExecuteIntent(const ContentAccess::TIntent aIntent) = 0;
	virtual TBool IsProtected()const = 0;

	// legacy file support - better to use Data()
	virtual RFile& FileL()=0;

	virtual TInt SetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue) = 0;

	virtual TInt Read(TInt aPosition, TDes8& aDes, TInt aLength, TRequestStatus& aStatus) = 0;
	// access to CData
    virtual TInt Data(ContentAccess::CData*& aData) = 0;

	};

// Provide RFile based file access - only now used for recording
class CF32File : public CBase, public MGenericFile
	{
public:
	static CF32File* NewL(RFs& aSession, TDesC& aPath, TUint aMode);	
	static CF32File* NewL(RFile& aFile);	
	void ConstructL(RFile& aFile);
	void ConstructL(RFs& aSession, TDesC& aPath, TUint aMode);

	virtual ~CF32File();

	// from MGenericFile
	virtual TInt Seek(TSeek aSeekMode, TInt aPosition);
	virtual TInt Read(TDes8& aDes,TInt aLength);
	virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
	virtual TInt Write(const TDesC8& aDes,TInt aLength);
	virtual void Write(const TDesC8& aDes, TInt aLength, TRequestStatus& aStatus);
	virtual TInt Size(TInt& aSize);
	virtual TInt SetSize(TInt aSize);
	virtual TInt EvaluateIntent(const ContentAccess::TIntent aIntent) const;
	virtual TInt ExecuteIntent(const ContentAccess::TIntent aIntent);
	virtual TBool IsProtected() const;
	virtual RFile& FileL();

	virtual TInt SetAgentProperty(ContentAccess::TAgentProperty, TInt aValue);
	
	virtual TInt Read(TInt aPosition, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
	virtual TInt Data(ContentAccess::CData*& aData);
private:
	CF32File();

private:
	RFs* iSession;	// not owned
	HBufC* iFilePath;
	RFile iFile;
	
	TBool iFileHandle;
	TInt iFileSize;
	};

// Provides content access framework based file access
class CContentFile : public CBase, public MGenericFile
	{
public:
	static CContentFile* NewL(RFs& aSession, const TDesC& aPath, const TDesC& aUniqueId, TUint aMode, TBool aEnableUI);
	void ConstructL(RFs& aSession, const TDesC& aPath, const TDesC& aUniqueId, TUint aMode, TBool aEnableUI);

	static CContentFile* NewL(RFile& aFile, const TDesC& aUniqueId, TBool aEnableUI);
	void ConstructL(RFile& aFile, const TDesC& aUniqueId, TBool aEnableUI);

	virtual ~CContentFile();

	// from MGenericFile
	virtual TInt Seek(TSeek aSeekMode, TInt aPosition);
	virtual TInt Read(TDes8& aDes,TInt aLength);
	virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
	virtual TInt Write(const TDesC8& aDes,TInt aLength);
	virtual void Write(const TDesC8& aDes, TInt aLength, TRequestStatus& aStatus);
	virtual TInt Size(TInt& aSize);
	virtual TInt SetSize(TInt aSize);
	virtual TInt EvaluateIntent(const ContentAccess::TIntent aIntent) const;
	virtual TInt ExecuteIntent(const ContentAccess::TIntent aIntent);
	virtual TBool IsProtected() const;
	virtual RFile& FileL();
	
	virtual TInt SetAgentProperty(ContentAccess::TAgentProperty, TInt aValue);
	virtual TInt Read(TInt aPosition, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
	virtual TInt Data(ContentAccess::CData*& aData);


private:
	CContentFile();


private:
	RFs* iSession; // not owned
	HBufC* iFilePath;
	ContentAccess::CData* iData;

	RFile   iLegacyFile;
	RFile64 iFile64; //For defect EASA-84ZC6J 
	TBool   iLegacyFileOpen;
	};


inline CF32File::CF32File() 
	{
	}

inline CContentFile::CContentFile() 
	{
	}

inline MGenericFile::~MGenericFile()
	{
	}

#endif //__FILE_ACCESS_H__
