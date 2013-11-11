// wget.h
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

#ifndef __W_GET_H__
#define __W_GET_H__

#include <e32std.h>
#include <es_sock.h>
#include <rhttpsession.h>
#include <mhttpsessioneventcallback.h>
#include <mhttpauthenticationcallback.h>
#include <mhttptransactioncallback.h>
#include <mhttpdatasupplier.h>
#include <fshell/ioutils.h>

using namespace IoUtils;

const TUint32 KDefaultChunkSize = 0x200; // 512 Bytes

//
// CCmdWget
// fshell command to retrieve a file from an HTTP Server on a network
//
class CCmdWget : public CCommandBase, public MHTTPSessionEventCallback, public MHTTPAuthenticationCallback, public MHTTPTransactionCallback, public MHTTPDataSupplier
	{
public:
	static CCommandBase* NewLC();
	~CCmdWget();
private:
	CCmdWget();
	void PrepareDownloadFileL();
	void PrepareUploadFileL();
	void DumpRespHeadersL(RHTTPTransaction& aTransaction);
	void LaunchConnectionL();
	void ConfigureHTTPL();
	void LaunchHTTPTransactionL();
	
	// From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);

	// From MHTTPSessionEventCallback
	virtual void MHFSessionRunL(const THTTPSessionEvent& aEvent);
	virtual TInt MHFSessionRunError(TInt aError, const THTTPSessionEvent& aEvent);
	
	// From MHTTPTransactionCallback
	virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
	virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

	// From MHTTPAuthenticationCallback
	virtual TBool GetCredentialsL(const TUriC8& aURI, RString aRealm, RStringF aAuthenticationType, RString& aUsername, RString& aPassword);
	
	// From MHTTPDataSupplier
	virtual TBool GetNextDataPart(TPtrC8& aDataPart);
	virtual void ReleaseData();
	virtual TInt OverallDataSize();
	virtual TInt Reset();
private:
	RHTTPSession iSession;
	RHTTPTransaction iTransaction;
	RHTTPRequest iRequest;
	RSocketServ iSocketServ;
	RConnection iConnection;
	RFile iLocalFile;
	TInt iLocalFileSize;
	TInt iLocalBytesRead;
	TBuf8<KDefaultChunkSize> iPrevData;
	TBool iSessionIsOpen;
	TBool iReleaseDataCalled;
	
	// user-specified arguments & options
	TBool iContinue;
	TBool iVerbose;
	TBool iPostData;
	HBufC* iUrl;
	HBufC* iUsername;
	HBufC* iPassword;
	TFileName2 iDestinationFilename;
	TFileName2 iUploadFilename;
	TInt iIapId;
	};

#endif // __W_GET_H__
