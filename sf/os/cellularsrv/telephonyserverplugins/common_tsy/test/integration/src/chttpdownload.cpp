// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "chttpdownload.h"
#include "cctsytestlogging.h"
#include <cs_subconparams.h>
#include <networking/qos3gpp_subconparams.h>

// Declare a static, single, server socket session to be used with all active downloads
RSocketServ CHTTPDownload::iSocketServer;
// Declare a static, counter fot active downloads, to know when to open and close the server socket session
TInt CHTTPDownload::iActiveDownload = 0;

_LIT(KDefaultHost, "developer.symbian.org");
_LIT(KDefaultPage, "/wiki/images/1/12/Common_Design_Patterns_for_Symbian_OS_Sample_Chapter.pdf");
const TInt KDefaultPort = 80;
const TInt KBufferSize = 4096;

CHTTPDownload::CHTTPDownload(CCTSYIntegrationTestSuiteStepBase *aTestStep): iOpenSocketServer(EFalse),
																			iTestStep(aTestStep),
                                                                            iSocketOpen(EFalse),
                                                                            iHostResolverOpen(EFalse),
                                                                            iConnectionOpen(EFalse),
                                                                            iSubConnectionOpen(EFalse),
                                                                            iWebPageOpen(EFalse)
/*
 * Constructor
 * 
 * @param aTestStep a pointer to the test step object
 */
                               
    {
    }

CHTTPDownload::~CHTTPDownload()
/*
 * Destructor
 * 
 */
    {
    StopDownload();
    if(iWebPageOpen)
    	{
    	iWebPage.Close();
    	iWebPageOpen=EFalse;
    	}
    if(iOpenSocketServer)
    	{
    	iActiveDownload--;
    	TEST_FRAMEWORK_LOG2(_L("iActiveDownload changed  iActiveDownload = %d"),iActiveDownload);    
    	if(iActiveDownload==0)
    		{
    		iSocketServer.Close();
    		TEST_FRAMEWORK_LOG2(_L("iSocketServer Closed iActiveDownload = %d"),iActiveDownload);    
    		}
    	}
    }


TBool CHTTPDownload::StartDownloadL()
/*
 * Start download an http page with default parameters
 * 
 *
 * @return ETrue if the download started
 */
    {
    iHost = &KDefaultHost;
    iPage = &KDefaultPage;
    iPort = KDefaultPort;
    iUseSubConnection = EFalse;
    if(InitSocket()!=KErrNone)
    	{
    	Close();
    	return EFalse;
    	}
	return ETrue;
    }

TBool CHTTPDownload::StartDownloadL(const TDesC &aHost, const TDesC &aPage)
/*
 * Start download a requested http page with default port 
 * 
 * @param aHost a descriptor of the web host (i.e. www.symbian.com)
 * @param aPage The actual page (i.e. /symbianos/index.html)
 *
 * @return ETrue if the download started
 */
    {
    iHost = &aHost;
    iPage = &aPage;
    iPort = KDefaultPort;
    iUseSubConnection = EFalse;
    if(InitSocket()!=KErrNone)
    	{
    	Close();
    	return EFalse;
    	}
	return ETrue;
    }

TBool CHTTPDownload::StartDownloadL(const TDesC &aHost, const TDesC &aPage,TInt aPort)
/*
 * Start download a requested http page with a requested port
 * 
 * @param aHost a descriptor of the web host (i.e. www.symbian.com)
 * @param aPage The actual page (i.e. /symbianos/index.html)
 * @param aPort the protocol port (80 for HTTP)
 *
 * @return ETrue if the download started
 */
    {
    iHost = &aHost;
    iPage = &aPage;
    iPort = aPort;
    iUseSubConnection = EFalse;
    if(InitSocket()!=KErrNone)
    	{
    	Close();
    	return EFalse;
    	}
	return ETrue;
    }


TBool CHTTPDownload::StartDownloadWithSecondaryContextL(const TDesC &aHost, const TDesC &aPage, TInt aPort)
/*
 * Start download a requested http page with a requested port using a sub connection
 * 
 * @param aHost a descriptor of the web host (i.e. www.symbian.com)
 * @param aPage The actual page (i.e. /symbianos/index.html)
 * @param aPort the protocol port (80 for HTTP)
 *
 * @return ETrue if the download started
 */
	{
    iHost = &aHost;
    iPage = &aPage;
    iPort = aPort;
    iUseSubConnection = ETrue;
    if(InitSocket()!=KErrNone)
    	{
    	Close();
    	return EFalse;
    	}
	return ETrue;
	}

TInt CHTTPDownload::GetByteCount()
/*
 * Return the number of bytes downloaded so far
 * 
 * @return The number of bytes downloaded so far
 */
    {
    VerifyDownloading();
    return iByteCount;
    }

void CHTTPDownload::StopDownload()
/*
 * Stop a active download; waits for the current socket read to complete, rather than force an immediate terminate.
 * 
 */
    {
    if(iSocketOpen)
    	{
    	iSocket.CancelRecv();
    	}
    Close();
    }

TBool CHTTPDownload::VerifyDownloading()
/*
 * Return ETrue if the download is still active
 * 
 * @return ETrue if the download is still active
 */
    {
    if(iSocketOpen)
    	{
    	if(iStatus.Int()==KRequestPending)
    		{
    		return ETrue;
    		}
    	else if (iStatus.Int()==KErrNone)
    		{
    		iByteCount += iWebPage.Length();

            // Logging the first 180 characters of the text...
            TBuf<200> temp;
            temp.Copy(iWebPage.Left(180));
            TEST_FRAMEWORK_LOG3(_L("Got %S... in %d chars"),&temp,iWebPage.Length());
    		
    		iSocket.RecvOneOrMore(iWebPage,0,iStatus,iHttpHeaderLen);
    		return  ETrue;
    		}
    	else 
    		{
    		iByteCount += iWebPage.Length();
    		Close();
    		return EFalse;
    		}
    		
    	}
    else
    	{
    	return EFalse;
    	}
    }




TInt CHTTPDownload::InitSocket()
/*
 * Initilize a socket 
 * 
 * @return KErrNone if it had no problem, or the specific system error code that cause the problem.
 */
	{
	TInt status;
    if(!iWebPageOpen)
    	{
    	status = iWebPage.Create(KBufferSize);
    	if(status !=KErrNone)
    		{
    		TEST_FRAMEWORK_LOG2(_L("RRBuf8::Create Could not create a buffer, got %d"),status);
    		return status;
    		}
    	iWebPageOpen = ETrue;
    	}
    if(!iOpenSocketServer)
    	{
    	if(iActiveDownload==0)
    		{
    		status = iSocketServer.Connect();
    		TEST_FRAMEWORK_LOG3(_L("iSocketServer.Connected, status = %d, iActiveDownload = %d"),status, iActiveDownload);
    		if(status !=KErrNone)
    			{
    			TEST_FRAMEWORK_LOG2(_L("RConnection::Open Could not open a socket servet session, got %d"),status);
    			return status;
    			}
    		}
    	iActiveDownload++;
    	iOpenSocketServer = ETrue;
    	}
    TEST_FRAMEWORK_LOG2(_L("iActiveDownload changed  iActiveDownload = %d"),iActiveDownload);
    if(iUseSubConnection)
    	{
    	if(!iConnectionOpen)
    		{
    		status = iConnection.Open(iSocketServer);
    		if(status != KErrNone) 
    			{
    			TEST_FRAMEWORK_LOG2(_L("RConnection::Open Could not open a connection, got %d"),status);
    			return status;
    			}
    		iConnectionOpen=ETrue;
    		}
        status=iConnection.Start();
        if(status != KErrNone)
            {
            TEST_FRAMEWORK_LOG2(_L("RConnection::Start Could not start a connection, got %d"),status);
            return status;
            }
        if(!iSubConnectionOpen)
            {
            status = iSubConnection.Open(iSocketServer,RSubConnection::ECreateNew,iConnection);
            if(status != KErrNone)
                {
                TEST_FRAMEWORK_LOG2(_L("RSubConnection::Open Could not open a sub connection, got %d"),status);
                return status;
                }
            iSubConnectionOpen=ETrue;
            }
        }
    if(!iSocketOpen)
    	{
    	if(iSubConnectionOpen)
    		{
        	status = iSocket.Open(iSocketServer, KAfInet, KSockStream, KProtocolInetTcp,iSubConnection);
        	}
        else
        	{
        	status = iSocket.Open(iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
        	}
        if(status != KErrNone)
            {
            TEST_FRAMEWORK_LOG2(_L("RSocket::Open Could not open a socket, got %d"),status);
            return status;
            }
        iSocketOpen=ETrue;
        }
    if(!iHostResolverOpen)
        {
        status = iHostResolver.Open(iSocketServer, KAfInet, KProtocolInetUdp);
        if(status != KErrNone)
            {
            TEST_FRAMEWORK_LOG2(_L("RHostResolver::Open Could not open an host resolver, got %d"),status);
            return status;
            }
        iHostResolverOpen=ETrue;
        }
	iHostResolver.GetByName(*iHost, iHostNameEntry, iStatus);
	User::WaitForRequest(iStatus);
	if(iStatus.Int()!=KErrNone)
		{
        TEST_FRAMEWORK_LOG2(_L("RHostResolver::GetByName Could get host by name, got %d"),iStatus.Int());
        return iStatus.Int(); 
		}
	return Connect();
	}

TInt CHTTPDownload::Connect()
/*
 * Connect to the host 
 * 
 * @return KErrNone, if it had no problem, or the specific system error code that cause the problem.
 */
	{
	TInetAddr addr(iHostNameEntry().iAddr);
	addr.SetPort(iPort);
	
	TCoreEtelRequestStatus<RSocket> connectStatus(iSocket, &RSocket::CancelConnect);
	iSocket.Connect(addr, connectStatus);
	iTestStep->WaitForRequestWithTimeOut(connectStatus,ETimeLong);
	if(connectStatus.Int()!=KErrNone)
		{
        TEST_FRAMEWORK_LOG2(_L("RSocket::Connect Could not connect to the host, got %d"),connectStatus.Int());
        return connectStatus.Int();
		}
	return Get();
	}

TInt CHTTPDownload::Get()
/*
 * sends the GET request to the server 
 * 
 * @return KErrNone, if it had no problem, or the specific system error code that cause the problem.
 */
	{
    TBuf8<200> httpGetLine;
    TBuf8<200> host8;
    TBuf8<200> page8;
    host8.Copy(*iHost);
    page8.Copy(*iPage);
    httpGetLine.Format(_L8("GET %S HTTP/1.0\r\nHost: %S\r\n\r\n"),&page8,&host8);

    TCoreEtelRequestStatus<RSocket> getStatus(iSocket, &RSocket::CancelWrite);
    iSocket.Write(httpGetLine, getStatus);
	iTestStep->WaitForRequestWithTimeOut(getStatus,ETimeLong);
	if(getStatus.Int()!=KErrNone)
		{
        TEST_FRAMEWORK_LOG2(_L("RSocket::Write Could not write to the host, got %d"),getStatus.Int());
        return getStatus.Int();
		}
	
    iByteCount = 0;
	iSocket.RecvOneOrMore(iWebPage,0,iStatus,iHttpHeaderLen);
	if(iStatus==KRequestPending)
		{
		return KErrNone;
		}
    return iStatus.Int();
	}



void CHTTPDownload::Close()
/*
 * Close everything... 
 * 
 */
	{
    if(iHostResolverOpen)
        {
        iHostResolver.Close();
        iHostResolverOpen=EFalse;
        }
    if(iSocketOpen)
    	{
        iSocket.Close();
        iSocketOpen=EFalse;
    	}
    if(iSubConnectionOpen)
    	{
    	iSubConnection.Close();
    	iSubConnectionOpen = EFalse;
    	}
    if(iConnectionOpen)
    	{
    	iConnection.Close();
    	iConnectionOpen = EFalse;
    	}
	}

