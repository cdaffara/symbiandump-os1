// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CHTTP_DOWNLOAD___INCLUDE
#define __CHTTP_DOWNLOAD___INCLUDE


#include <test/testexecutestepbase.h>
#include <in_sock.h>
#include "cctsyintegrationtestsuitebase.h"

class CHTTPDownload : public CBase
/**
 * This is a class that can be used in order to download an HTTP page from the web, while working on 
 * other stuff. To use it, one should create an instance, and then call the StartDownloadL method. 
 */
    {
public:
    CHTTPDownload(CCTSYIntegrationTestSuiteStepBase *aTestStep);
    virtual ~CHTTPDownload();

    TBool StartDownloadL();
    TBool StartDownloadL(const TDesC &aHost, const TDesC &aPage);
    TBool StartDownloadL(const TDesC &aHost, const TDesC &aPage, TInt aPort);
    TBool StartDownloadWithSecondaryContextL(const TDesC &aHost, const TDesC &aPage,TInt aPort);
    
    void StopDownload();
    TBool VerifyDownloading();
    TInt GetByteCount();

private:
    
    TInt InitSocket();
    TInt Connect();
    TInt Get();
    TInt StartRead();
    void Close();
    
private:
    static RSocketServ iSocketServer;
    static TInt iActiveDownload;
    TBool iOpenSocketServer;
    CCTSYIntegrationTestSuiteStepBase *iTestStep;
    RSocket iSocket;
    TBool iSocketOpen;
    RHostResolver iHostResolver;
    TBool iHostResolverOpen;
    RConnection iConnection;
    TBool iConnectionOpen;
    RSubConnection iSubConnection;
    TBool iSubConnectionOpen;
    TBool iUseSubConnection;
	RBuf8 iWebPage;
    TBool iWebPageOpen;
    TRequestStatus iRecvStatus;
    TInt iByteCount;
	TNameEntry iHostNameEntry;
    const TDesC *iHost;
    const TDesC *iPage;
    TInt iPort;
    TRequestStatus iStatus;
	TSockXfrLength iHttpHeaderLen;
	
    };


#endif // __CHTTP_DOWNLOAD___INCLUDE
