/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include "T_RSocketData.h"
#include "T_RHostResolverData.h"
#include <e32property.h>
#include <etelmm.h>   // RMobilePhone
#include <etelpckt.h> // RPacketService


const TInt KBufferSize = 1024;
_LIT( KNoFileSave, "NULL" );
const TUint KNoDataDealer = 0;
const TInt KNoDuration = -1;
const TInt KNoSpeedCheck = -1;

// Misc Types
_LIT( KProtocolHTTP,   "HTTP" );
_LIT( KOff,            "Off" );        // Can be used every2*where off is needed

// HSDPA checks
_LIT( KHsdpaIndicator, "Indicator" );  // Operator supports
_LIT( KHsdpaSupported, "Supported" );  // Device supports
_LIT( KHsdpaInUse,     "Used" );       // In use during PSD

// Constants for creating a HTTP request.
_LIT8(KHTTPGET, 		"GET");
_LIT8(KHTTPSeparator, 	" ");
_LIT8(KHTTPSuffix, 		"HTTP/1.0");
_LIT8(KEmptyLine, 		"\r\n\r\n");
const TInt KEmptySize = 4;

const TInt KHTTPSize = 17;
const TInt KMaxTag = 256;
const TInt KHeaderWithoutData=200;
//
// Manually created HTTP Post request is difficult to maintain.
// Request and server responce is logged into file during test run.
_LIT8(KHTTPPOST, "POST");
_LIT8(KLineBreak, "\r\n");

_LIT8(KAhti, "Ahti");

_LIT8(KServerScript, 	"/dy/upload/upfile.php");
_LIT8(KFrom, 			"From:");
_LIT8(KHost, 			"Host:");
_LIT8(KContentType, 	"Content-Type:");
_LIT8(KContentLength, 	"Content-Length:");
_LIT8(KContentDisposition, "Content-Disposition:");

_LIT8(KMultipartType, "multipart/form-data;");
_LIT8(KOctetType,     "application/octet-stream");
_LIT8(KBoundary,      "boundary=---------------------------sg976436h73");
_LIT8(KBoundaryStart, "-----------------------------sg976436h73");
_LIT8(KBoundaryEnd,   "-----------------------------sg976436h73--");
_LIT8(KDisposition,   "form-data; name=\"userfile\"; filename=\"test.bin\"");

_LIT( KPropertyCategory, 		"PropertyCategory");
_LIT( KPropertyKey,				"PropertyKey");
_LIT( KMaxPropertyLength,		"PropertyLength");

//const TInt 
_LIT( KPort,         					"Port" );
_LIT( KAddress,      					"Address" );
_LIT( KProtocol,     					"Protocol" );
_LIT( KFileName,     					"File" );
_LIT( KSaveName,     					"Save" );
_LIT( KDataDealerID, 					"DataDealer" );    // Data dealer ID is given may not be 0
_LIT( KHsdpaCheck,   					"HSDPA" );         // HSDPA check can be done several ways
_LIT( KDuration,     					"Duration" );      // How long should i.e. download last when it is stopped (THRO)
_LIT( KMinimumSpeed, 					"MinSpeed" );      // Data transfer must be faster than MinSpeed
_LIT( KBytes,        					"Bytes" );         // How many bytes are send
_LIT( KResponseLogFile,					"ResponseLogFile");
_LIT( KRequestLogFile,					"RequestLogFile");
//LIT's to read from ini file
_LIT( KDefaultSection,					"Default");
_LIT(KRConnection, 						"RConnection");
_LIT(KRSocketServ,						"RSocketServ");
_LIT(KRHostResolver,					"RHostResolver");
_LIT(KRMobilePhone,						"RMobilePhone");

/*@}*/

/*@{*/
//LIT's for commands
_LIT(KCmdRSocketOpen,							"Open");
_LIT(KCmdRSocketClose,							"Close");
_LIT(KCmdRSocketConnect, 						"Connect");
_LIT(KCmdRSocketShutDown,						"ShutDown");
_LIT(KCmdRSocketWrite,							"Write");
_LIT(KCmdResponseFromServer, 					"ResponseFromServer");
_LIT(KWriteRequest,								"WriteRequest");
_LIT(KCmdUtilityStartFile,						"StartFile");
_LIT(KCmdUtilityStartDealer,					"StartDealer");
_LIT(KCmdUtilityCheckHSDPA,						"CheckHSDPA");
_LIT(KCmdRecvOneOrMore,							"RecvOneOrMore");
_LIT(KCmdUtilityEndFile,						"EndFile");
_LIT(KCmdUtilityCalcDnloadStats,				"CalcDnloadStats");
_LIT(KCmdUtilityBuildRequest,					"BuildRequest");
_LIT(KCmdSendBytesToSocket,						"SendBytesToSocket");
_LIT(KCmdUtilityEndRequest,						"EndRequest");
_LIT(KCmdInfo,									"Info");
/*@}*/


/**
 * Two phase constructor
 */
CT_RSocketData* CT_RSocketData::NewL()
	{
	CT_RSocketData*	ret=new (ELeave) CT_RSocketData();
	CleanupStack::PushL(ret);
	ret->ConstructL();	
	CleanupStack::Pop(ret);
	return ret;
	}

/**
* Protected constructor. First phase construction
*/
CT_RSocketData::CT_RSocketData():
	iSocket(NULL),    
	iActiveCallback(NULL),
	iBreathSaver(NULL)
	{
	}

CT_RSocketData::~CT_RSocketData()
	{
	if(iBreathSaver)
		{
		delete iBreathSaver;
		iBreathSaver = NULL;
		}
	if(iActiveCallback)
		{
		delete iActiveCallback;
		iActiveCallback = NULL;
		}	
	if(iSocket)
		{
		delete iSocket;
		iSocket = NULL;
		}
	}

void CT_RSocketData::ConstructL()
	{
	iActiveCallback = CActiveCallback::NewL(*this);
	iBreathSaver = CBreathSaver::NewL();
	iSocket = new (ELeave)RSocket();
	}

void CT_RSocketData::InitialiseL()
	{
	CDataWrapperBase::InitialiseL();
	
	TInt propertyCategory;
	GetHexFromConfig(KDefaultSection, KPropertyCategory, propertyCategory);
	iPropertyCategory.iUid = propertyCategory;
	
	TInt propertyKey;
	GetIntFromConfig(KDefaultSection, KPropertyKey, propertyKey );
	iPropertyKey =(TUint)propertyKey;
	
	GetIntFromConfig(KDefaultSection, KMaxPropertyLength, iMaxPropertyLength );
	
	}



/**
	* Process a command read from the ini file
	*
	* @param aCommand			The command to process
	* @param aSection			The section in the ini containing data for the command
	* @param aAsyncErrorIndex	Command index for async calls to return errors to
	*
	* @return					ETrue if the command is processed
	*
	* @leave					System wide error
	*/
TBool CT_RSocketData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool retVal=ETrue;
	
	if (aCommand==KCmdRSocketOpen)
		{	
		DoCmdOpenL(aSection);
		}
	else if (aCommand==KCmdRSocketConnect)
		{
		DoCmdConnectL(aSection, aAsyncErrorIndex);
		}
	else if (aCommand==KCmdRSocketShutDown)
		{
		DoCmdShutDown(aAsyncErrorIndex);
		}
	else if (aCommand==KCmdRSocketClose)
		{
		DoCmdClose();
		}
	else if ( aCommand == KWriteRequest )
		{		
		DoCmdWriteRequest(aSection, aAsyncErrorIndex);
		}
	else if ( aCommand == KCmdUtilityStartFile )
		{
		DoCmdUtilityStartFile(aSection);
		}
	else if (aCommand == KCmdUtilityStartDealer )
		{
		DoCmdUtilityStartDealer(aSection);
		}
	else if (aCommand == KCmdUtilityCheckHSDPA )
		{
		DoCmdUtilityCheckHSDPAL(aSection);
		}
	else if (aCommand==KCmdRecvOneOrMore)
		{
		DoCmdRecvOneOrMore(aSection);
		}
	else if (aCommand == KCmdUtilityEndFile )
		{
		DoCmdUtilityEndFile(aSection);		
		}
	else if (aCommand == KCmdUtilityCalcDnloadStats )
		{
		DoCmdUtilityCalculateDownloadStats(aSection);		
		}
	else if (aCommand==KCmdUtilityBuildRequest)
		{
		DoCmdUtilityBuildRequest(aSection);
		}
	else if (aCommand==KCmdRSocketWrite)
		{
		DoCmdWrite(aAsyncErrorIndex);
		}
	else if (aCommand==KCmdSendBytesToSocket)
		{
		DoCmdSendBytesToSocket(aSection);
		}
	else if (aCommand==KCmdResponseFromServer)
		{
		DoCmdResponseFromServer(aSection);
		}
	else if (aCommand==KCmdUtilityEndRequest)
		{
		DoCmdUtilityEndRequest();
		}
	else if (aCommand==KCmdInfo)
		{
		DoCmdInfoL(aSection);
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		retVal=EFalse;
		}

	return retVal;
	}
	
/**
	* Return a pointer to the object that the data wraps
	*
	* @return pointer to the object that the data wraps
	*/
TAny* CT_RSocketData::GetObject()
	{
	return iSocket;
	}
	

void CT_RSocketData::RunL(CActive* aActive, TInt aIndex)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::RunL"));
	DecOutstanding(); // One of the async calls has completed 
    
	if(aActive == iActiveCallback)
		{
		INFO_PRINTF1(_L("Asynchronous task has completed. RunL  called"));
		}
	else
		{ 
		ERR_PRINTF1(_L("Stray RunL signal"));
		TInt err = aActive->iStatus.Int(); 
		if( err != KErrNone )
			{
	        ERR_PRINTF2(_L("RunL Error %d"), err); 
	        SetAsyncError( aIndex, err );
			}
		}
	
	INFO_PRINTF1(_L("*END* CT_RSocketData::RunL"));
	}
	
/**
 * Opens a socket.
 * @param aSection - Section in config file to get a RSocketServer and RConnection
 * Sets TEF error if not successful.
 */	
void CT_RSocketData::DoCmdOpenL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdOpenL"));
	TBool dataOk = ETrue;
	
	TPtrC connectionName;
	if ( !GetStringFromConfig(aSection, KRConnection(), connectionName) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRConnection);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}

	TPtrC socketServName;
	if ( !GetStringFromConfig(aSection, KRSocketServ(), socketServName) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRSocketServ);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}

	if (dataOk)
		{
		INFO_PRINTF1(_L("Create a RConnection for iSocket"));
		RConnection* connection = static_cast<RConnection*>(GetDataObjectL(connectionName));
		INFO_PRINTF1(_L("Create a RSocketServ for iSocket"));
		RSocketServ* socketServ = static_cast<RSocketServ*>(GetDataObjectL(socketServName));

		TInt error = iSocket->Open(*socketServ, KAfInet, KSockStream, KProtocolInetTcp, *connection);
		
		if (error == KErrNone)
			{
			INFO_PRINTF1(_L("Socket opening succeeded"));
			}
		else
			{
			ERR_PRINTF2(_L("Socket opening failed [%d]"), error);
			SetError(error);
			}
		}

	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdOpenL"));
	}

/**
 * Connects socket with a given address and port.
 * @param aSection			- The section in config file to look for the address, port and RHostResolver
 * @param aAsyncErrorIndex	- Command index for async calls to return errors to	
 */
void CT_RSocketData::DoCmdConnectL(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdConnectL"));
	TBool dataOk = ETrue;
	
	TInt port;
    if(!GetIntFromConfig( aSection, KPort, port ))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KPort);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    
    TPtrC address;
    if(!GetStringFromConfig( aSection, KAddress, address ))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAddress);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    
	// Connect ready socket to given host and port number
	TPtrC hostResolverName;
	if(	!GetStringFromConfig(aSection, KRHostResolver, hostResolverName) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRHostResolver);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}

	if (dataOk)
		{
		INFO_PRINTF3(_L("Connecting socket to server \"%S\" at port [%d]"), &address, port);
		
		CT_RHostResolverData* hostResolver = static_cast<CT_RHostResolverData*>(GetDataWrapperL(hostResolverName));
		iNameEntry = hostResolver->GetNameEntry();
		iNameEntry().iAddr.SetPort(port);
		iSocket->Connect(iNameEntry().iAddr, iActiveCallback->iStatus);
		iActiveCallback->Activate(aAsyncErrorIndex);
	    IncOutstanding();
		}

    INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdConnectL"));
    }

/**
 * Shuts down a connected socket - asynchronous.
 * This method is asynchronous as non emergency shutdown may take a while.
 * The shut down method allows input and output to be individually stopped for a protocol endpoint. 
 * @param aAsyncErrorIndex - Command index for async calls to return errors to
 */
void CT_RSocketData::DoCmdShutDown(const TInt aAsyncErrorIndex)
    {
    INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdShutDownSocket"));

	// Shutdown the socket.
	iSocket->Shutdown(RSocket::ENormal, iActiveCallback->iStatus);
	iActiveCallback->Activate(aAsyncErrorIndex);
    IncOutstanding();

    INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdShutDownSocket"));
    }

/**
 * Closes a socket.
 * If a socket has been opened using Open() then it should be closed using Close(). 
 * This will ensure all associated resources are released. 
 * Closing serves two distinct purposes: 
 * - To release resources associated with the IPC channel to the socket server. 
 * - To disconnect a socket if it is connected. 
 */
void CT_RSocketData::DoCmdClose()
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdCloseSocket"));
	iSocket->Close(); // Close does not return error code
    INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdCloseSocket"));
    }


/**
 * Builds an HTTP Get request
 * @param  aSection - The section in config file to look for address, port, protocol, filename and save path.
 * @param aAsyncErrorIndex - Command index for async calls to return errors to
 */
void CT_RSocketData::DoCmdWriteRequest(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdWriteRequest"));
	TBool dataOk = ETrue;

	TPtrC filename; 
	if(!GetStringFromConfig(aSection, KFileName, filename ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KFileName);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}
	
	TPtrC protocol; 
    if(!GetStringFromConfig(aSection, KProtocol, protocol ))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KProtocol);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    
    
    // Send http get request
    if (dataOk && protocol != KProtocolHTTP)
        {
        ERR_PRINTF2(_L("Protocol \"%S\" is unknown"), &protocol);
        SetBlockResult(EFail);
        dataOk = EFalse;
        }

    if (dataOk && filename.Length() > KMaxTag)
        {
        ERR_PRINTF1(_L("Filename is too long, cannot send HTTP request"));
        SetBlockResult(EFail);
        dataOk = EFalse;
        }

    if (dataOk)
    	{
        INFO_PRINTF1(_L("Sending HTTP request"));

    	// Buffer that will hold the request.
    	TBuf8<KHTTPSize + KMaxTag> request;

    	// Construct the final request.
    	request.Append(KHTTPGET);
    	request.Append(KHTTPSeparator);
    	request.Append(filename);
    	request.Append(KHTTPSeparator);
    	request.Append(KHTTPSuffix);
    	request.Append(KEmptyLine);

        // Send the request through socket
    	iSocket->Write(request, iActiveCallback->iStatus);
    	iActiveCallback->Activate(aAsyncErrorIndex);
        IncOutstanding();
    	}

	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdWriteRequest"));
	}


/**
 * Uses helper class BreathSaver to generate a file and save downloaded data in a given path.
 * @param  aSection - The section in config file to look for address, port, protocol, filename and save path.
 * Sets TEF error if not successful.
 */
void CT_RSocketData::DoCmdUtilityStartFile(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdUtilityStartFile"));
	TBool dataOk = ETrue;
	
    TPtrC savename( KNoFileSave );
    if(!GetStringFromConfig(aSection, KSaveName, savename))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSaveName);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
	
    if ( dataOk && savename == KNoFileSave)
    	{
    	ERR_PRINTF1(_L("Incorrect file name"));
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    
	if (dataOk)
		{
		INFO_PRINTF2(_L("Save incoming file to \"%S\""), &savename);
		TInt error = iBreathSaver->StartFile(savename);
	    
	    if (error != KErrNone)
	        {
	        ERR_PRINTF2(_L("Failed to save data file [%d]"), error);
	        SetError(error);
	        }
		}

	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdUtilityStartFile"));
    }
	
/**
 * Uses helper class BreathSaver to start a Data dealer
 * @param  aSection - The section in config file to look for Data Dealer ID.
 * Sets TEF error if not successful.
 */
void CT_RSocketData::DoCmdUtilityStartDealer(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdUtilityStartDealer"));
	TBool dataOk = ETrue;
	
	TUint datadealerid( KNoDataDealer );
    if(!GetUintFromConfig(aSection,  KDataDealerID, datadealerid ))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDataDealerID);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
	
    if (dataOk && datadealerid != KNoDataDealer)
		{
		INFO_PRINTF2(_L("Use data dealer with id (%u) for incoming file"), datadealerid);
		TInt error = iBreathSaver->StartDealer(datadealerid);
        
        if (error == KErrNone)
            {
            INFO_PRINTF1(_L("iBreathSaver->StartDealer(datadealerid) was executed"));
            }
        else
        	{
            ERR_PRINTF2(_L("Failed to use data dealer [%d]"), error);
            SetError(error);
        	}
		}
    
	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdUtilityStartDealer"));
	}
	


/**
 * Checks HSDPA options.
 * @param  aSection - The section in config file to look for HSDPA parameters.
 * @return error - Error code. KErrNone if successful.
 */
void CT_RSocketData::DoCmdUtilityCheckHSDPAL(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdUtilityCheckHSDPAL"));
	TBool dataOk = ETrue;

    TPtrC hsdpacheck( KNullDesC );
    if(!GetStringFromConfig(aSection, KHsdpaCheck, hsdpacheck))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KHsdpaCheck);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    
    if (dataOk)
    	{
        TPtrC mobilePhoneName;
        RMobilePhone* mobilePhone = NULL;
        if(GetStringFromConfig(aSection, KRMobilePhone(), mobilePhoneName))
        	{
        	mobilePhone = static_cast<RMobilePhone*>(GetDataObjectL(mobilePhoneName));
        	}
        
        INFO_PRINTF1(_L( "Checking HSDPA..." ));

    	if (hsdpacheck == KNullDesC)
            {
            INFO_PRINTF1(_L( "No HSDPA used. Nothing to check." ));
            }
    	else if (hsdpacheck == KHsdpaIndicator)
            {
            if (CheckHSDPAIndicator())
                {
                INFO_PRINTF1(_L("HSDPA indicator available"));
                }
            else
                {
                ERR_PRINTF1(_L("HSDPA indicator not available"));
                SetBlockResult(EFail);
                }
            }
        else if (hsdpacheck == KHsdpaSupported && mobilePhone)
            {
            if (CheckHSDPASupportL(*mobilePhone))
            	{
            	INFO_PRINTF1(_L("HSDPA support available"));
            	}
            else
            	{
            	ERR_PRINTF1(_L("HSDPA support not available"));
            	SetBlockResult(EFail);
            	}
            }
        else if (hsdpacheck == KHsdpaInUse && mobilePhone)
            {
    		if (CheckHSDPAUsageL(*mobilePhone))
    			{
    			INFO_PRINTF1(_L("HSDPA in use"));
    			}
    		else
    			{
    			ERR_PRINTF1(_L("HSDPA not in use"));
    			SetBlockResult(EFail);
    			}
            }
        else if (hsdpacheck == KOff)
            {
            INFO_PRINTF1(_L("Can HSDPA be turned off?"));
            }
        else
            {
            ERR_PRINTF2(_L("Unknown HSDPA usage parameter \"%S\""), &hsdpacheck);
            SetBlockResult(EFail);
            }
    	}
    
	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdUtilityCheckHSDPAL"));
	}

/**
 * Downloads a file from a given address and saves it in specified path.
 * @param  aSection - The section in config file to look for address, port, protocol, filename and save path.
 * @return error - Error code. KErrNone if successful.
 */
void CT_RSocketData::DoCmdRecvOneOrMore(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdDownloadToFile"));
	TBool dataOk = ETrue;

	TInt duration( KNoDuration );
	if(!GetIntFromConfig(aSection, KDuration, duration  ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDuration);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	
	TPtrC protocol;
	if(!GetStringFromConfig(aSection, KProtocol, protocol ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KProtocol);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}

	TUint datadealerid( KNoDataDealer );
	if(!GetUintFromConfig(aSection,  KDataDealerID, datadealerid ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDataDealerID);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	
	TPtrC savename( KNoFileSave );
    if(!GetStringFromConfig(aSection, KSaveName, savename))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSaveName);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
	
	TBuf8<KBufferSize> buffer;
	if (protocol != KProtocolHTTP)
		{
    	ERR_PRINTF3(_L("Error, %S protocol is not supported. %S protocol is expected"), &protocol, &KProtocolHTTP);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}
	
	if (dataOk)
		{
	    INFO_PRINTF1(_L("Download starts"));
	    
		TInt error = KErrNone;
		TBool header = ETrue;
		TBool writeFail = EFalse;
		TRequestStatus status;
		TSockXfrLength received;

		// Start and end time for calculating the transfer speed.
		iStart.HomeTime();
		
		// receive until RecvOneOrMore fails.
		do
			{
			iSocket->RecvOneOrMore(buffer, 0, status, received);
			User::WaitForRequest(status);

			if (status == KErrNone)
				{
				iTotalReceived += received();

				if (header)
					{
					// HTTP server first sends header and then data
					TInt location = buffer.Find(KEmptyLine);
					// If the separator (two line breaks) is found...
					if (location != KErrNotFound)
						{
						TInt i = location + KEmptySize;
						// Remove the header from the buffer
						buffer.Delete(0, i);
				        if (savename != KNoFileSave ||
	                        datadealerid != KNoDataDealer)
				            {
				            error = iBreathSaver->Append(buffer);
				            if (error != KErrNone)
								{
								WARN_PRINTF2(_L("Data append failed [%d]"), error);
								iBreathSaver->End();
								writeFail = ETrue;
								}
				            }
						header = EFalse;
						}
					}
				// Header is already removed from the input data
				else
					{
					// Write data into file
			        if (savename != KNoFileSave ||
	                    datadealerid != KNoDataDealer)
			            {
			            error = iBreathSaver->Append(buffer);
			            if (error != KErrNone)
							{
							WARN_PRINTF2(_L("Data append failed [%d]"), error);
							iBreathSaver->End();
							writeFail = ETrue;
							}
			            }
					}
				}
	        else
	            {
	            // KErrEof means that socket was closed or there is not any data to be read anymore
	            ERR_PRINTF2(_L("RecvOneOrMore returned error status [%d]"), status.Int());
	            }
	        // Check if duration parameter is given
	        if (duration != KNoDuration)
	            {
	            iEnd.HomeTime();
	            TTimeIntervalSeconds seconds (0);
	            error = iEnd.SecondsFrom(iStart, seconds);
	            if(error != KErrNone)
	            	{
	            	ERR_PRINTF2(_L("Failed to get time interval with error %d"), error);
	            	SetError(error);
	            	break;
	            	}
	            if ( seconds.Int() >= duration )
	                {
	                WARN_PRINTF2(_L("Transfer is stopped because defined duration [%d] exceeded"), seconds.Int());
	                break;
	                }
	            }
			}
		while(status == KErrNone && writeFail == EFalse);
		}
	
	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdDownloadToFile"));
	}

/**
 * Ends downloaded file.
 * @param  aSection - The section in config file to look for file's name.
 * @return error - Error code. KErrNone if successful.
 */
void CT_RSocketData::DoCmdUtilityEndFile(const TTEFSectionName& aSection)
	{	
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdUtilityEndFile"));
	TBool dataOk = ETrue;

	TPtrC savename( KNoFileSave );
    if(!GetStringFromConfig(aSection, KSaveName, savename))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KSaveName);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    
    if (dataOk)
    	{
        if (savename != KNoFileSave)
            {
            iBreathSaver->End();
            }

    	iEnd.HomeTime();
    	
    	INFO_PRINTF1(_L("Download ends"));
    	}

	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdUtilityEndFile"));
	}	

/**
 * Calculates download time and compares with a given duration and speed
 * @param  aSection - The section in config file to look for speed and duration parameters. 
 */
void CT_RSocketData::DoCmdUtilityCalculateDownloadStats(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdUtilityCalculateDownloadStats"));
	TBool dataOk = ETrue;
	
	TInt speed( KNoSpeedCheck );
	if(!GetIntFromConfig(aSection, KMinimumSpeed, speed))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMinimumSpeed);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    
	TInt duration( KNoDuration );
	if(!GetIntFromConfig(aSection, KDuration, duration  ))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDuration);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	
	if (dataOk)
		{
		// Calculate the duration of the download.
		TTimeIntervalMicroSeconds microseconds = iEnd.MicroSecondsFrom(iStart);
	    TInt error = KErrNone;
	    TReal throughput = 0;
	    
	    if (iTotalReceived != 0)
	        {
	        throughput = ThroughputInKiloBits( microseconds, iTotalReceived );
	        }
	    
	    error = SetThroughput(throughput);
	    if (error == KErrNone)
	        {
	        TBool speedEnough = ETrue;
		    if (speed != KNoSpeedCheck)
		        {
		        INFO_PRINTF2(_L("Check that the download speed was faster than (%d kpbs)"), speed);
		        if ( speed > throughput )
		            {
		            ERR_PRINTF1(_L("Speed was not fast enough"));
		            SetBlockResult(EFail);
		            speedEnough = EFalse;
		            }
		        else
		            {
		            INFO_PRINTF1(_L("Speed was fast enough"));
		            }
		        }

		    if (speedEnough && duration != KNoDuration)
		        {
		        TTimeIntervalSeconds seconds (0);
		        error = iEnd.SecondsFrom(iStart, seconds);
		        if(error != KErrNone)
		        	{
		        	ERR_PRINTF2(_L("Failed to get time interval with error %d"), error);
		        	SetError(error);
		        	}
		        else if ( seconds.Int() < duration )
		            {
		            ERR_PRINTF2(_L("Transfer failed, transfer did not last the defined duration [%d]"), duration);
		            SetBlockResult(EFail);
		            }
		        }
	        }
	    else
	    	{
	        ERR_PRINTF2(_L("SetThroughput failed [%d]"), error);
	        SetError(error);
	    	}
		}
    
    INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdUtilityCalculateDownloadStats"));
    }


void CT_RSocketData::DoCmdInfoL(const TTEFSectionName& aSection)	
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdInfoL"));
	TBool dataOk = ETrue;
	
	TPtrC protocol;
    if(!GetStringFromConfig(aSection, KProtocol, protocol))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KProtocol);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
	
    if(dataOk)
    	{
    	TProtocolDesc protocolDesc;
    	TInt err = iSocket->Info(protocolDesc);
    	if(err != KErrNone)
    		{
    		ERR_PRINTF2(_L("Getting Info failed with error %d"), err);
    		SetError(err);
    		}
    	
    	TInt* protocolApi = static_cast<TInt*>(GetDataObjectL(protocolDesc.iName));
    	TInt* protocolIni = static_cast<TInt*>(GetDataObjectL(protocol));
    	
    	if(protocolApi != protocolIni)
    		{
    		ERR_PRINTF3(_L("Mismatch between expected protocol [%S] and current protocol [%S]"),protocolIni, protocolApi);
    		SetBlockResult(EFail);
    		}
    	}
	
	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdInfoL"));
	}	


/**
 * Build http post request
 */
void CT_RSocketData::DoCmdUtilityBuildRequest(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdUtilityBuildRequest"));
	TBool dataOk = ETrue;

	INFO_PRINTF1(_L("Sending HTTP request"));
	TPtrC address( KNullDesC );
	if(!GetStringFromConfig(aSection, KAddress, address ))
	  	{
	  	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAddress);
	   	SetBlockResult(EFail);
	   	dataOk = EFalse;
	   	}
	
	TInt bytes;
    if(!GetIntFromConfig(aSection, KBytes, bytes ))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KBytes);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    TPtrC requestLogFile;
    if(!GetStringFromConfig(aSection, KRequestLogFile, requestLogFile ))
	  	{
	  	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRequestLogFile);
	   	SetBlockResult(EFail);
	   	dataOk = EFalse;
	   	}
    
    if (dataOk)
    	{
    	// Construct request
    	iRequest.Append(KHTTPPOST);
    	iRequest.Append(KHTTPSeparator);
    	iRequest.Append(KServerScript);
    	iRequest.Append(KHTTPSeparator);
    	iRequest.Append(KHTTPSuffix);
    	iRequest.Append(KLineBreak);
    	
    	iRequest.Append(KHost);
    	iRequest.Append(KHTTPSeparator);
    	iRequest.Append(address);
    	iRequest.Append(KLineBreak);
    	
    	iRequest.Append(KFrom);
    	iRequest.Append(KHTTPSeparator);
    	iRequest.Append(KAhti);
    	iRequest.Append(KLineBreak);
    	
    	iRequest.Append(KContentType);
    	iRequest.Append(KHTTPSeparator);
    	iRequest.Append(KMultipartType);
    	iRequest.Append(KHTTPSeparator);
    	iRequest.Append(KBoundary);
    	iRequest.Append(KLineBreak);
    	
    	iRequest.Append(KContentLength);
    	iRequest.Append(KHTTPSeparator);
    	// Request size + size of the data to be sent. Server must know how much
    	// data is coming.
    	iRequest.AppendNum(KHeaderWithoutData+bytes);
    	iRequest.Append(KLineBreak);
    	
    	// extra line break
    	iRequest.Append(KLineBreak);
    	
    	iRequest.Append(KBoundaryStart);
    	iRequest.Append(KLineBreak);
    	
    	iRequest.Append(KContentDisposition);
    	iRequest.Append(KHTTPSeparator);
    	iRequest.Append(KDisposition);
    	iRequest.Append(KLineBreak);
    	
    	iRequest.Append(KContentType);
    	iRequest.Append(KHTTPSeparator);
    	iRequest.Append(KOctetType);
    	iRequest.Append(KLineBreak);
    	
    	iRequest.Append(KLineBreak);
    	
    	// Make a log file of the request
    	TFileName fileName;
    	TDriveUnit driveUnit(EDriveC);
    	fileName.Zero();
    	fileName.Append(driveUnit.Name()); // c:
    	fileName.Append(requestLogFile);	
    	iBreathSaver->StartFile(fileName);

    	iBreathSaver->Append(iRequest);
    	}

	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdUtilityBuildRequest"));
	}

void CT_RSocketData::DoCmdWrite(const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdWrite"));
	
	// Send the header before data
   	iSocket->Write(iRequest, iActiveCallback->iStatus);
   	iActiveCallback->Activate(aAsyncErrorIndex);
   	IncOutstanding();
   	
   	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdWrite"));
	}

void CT_RSocketData::DoCmdSendBytesToSocket(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdSendBytesToSocket"));
	TBool dataOk = ETrue;
	
	TInt bytes;
	if(!GetIntFromConfig(aSection, KBytes, bytes ))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KBytes);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
	TInt duration ( KNoDuration );
	if(!GetIntFromConfig(aSection, KDuration, duration ))
		{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KDuration);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
	
	if (dataOk)
		{
		TInt error = SendBytesToSocket(bytes, duration);
		if (error != KErrNone)
			{
			ERR_PRINTF1(_L("Error in sending bytes to socket"));
			SetError(error);
			}
		}

	INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdSendBytesToSocket"));
	}

void CT_RSocketData::DoCmdUtilityEndRequest()
	{
	INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdUtilityEndRequest"));

	iRequest.Delete(0, iRequest.Length());
    // Now the rest of the header data
    iRequest.Append(KLineBreak);

    iRequest.Append(KBoundaryEnd);
    iRequest.Append(KLineBreak);

    // add the rest of the reaquest to log file
    iBreathSaver->Append(iRequest);
    iBreathSaver->End(); // Close the file

    INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdUtilityEndRequest"));
	}

void CT_RSocketData::DoCmdResponseFromServer(const TTEFSectionName& aSection)
	{
    INFO_PRINTF1(_L("*START* CT_RSocketData::DoCmdResponseFromServer"));
    TBool dataOk = ETrue;
    TPtrC responseLogFile;
    if(!GetStringFromConfig(aSection, KResponseLogFile, responseLogFile ))
	  	{
	  	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KResponseLogFile);
	   	SetBlockResult(EFail);
	   	dataOk = EFalse;
	   	}
    if(dataOk)
    	{
    	// After file was succesfully sent to HTTP server we can receive the response
    	// from the server.
    	// Create variables for receive buffer and received data counting variables.
    	
    	TBuf8<KBufferSize> buffer;
    	TSockXfrLength received;
    	TRequestStatus status;
    	
    	TFileName fileName;
    	TDriveUnit driveUnit(EDriveC);
    	fileName.Zero();
    	fileName.Append(driveUnit.Name()); // c:
    	fileName.Append(responseLogFile);
    	
    	iBreathSaver->StartFile(fileName);
    	// receive until RecvOneOrMore fails.
    	do
    		{
    		iSocket->RecvOneOrMore(buffer, 0, status, received);
    		User::WaitForRequest(status);
    	
    	    iBreathSaver->Append(buffer);
    		}
    	while(status.Int() == KErrNone);
    	
    	iBreathSaver->End();
    	}
    
	
    INFO_PRINTF1(_L("*END* CT_RSocketData::DoCmdResponseFromServer"));
	}

/**
 * Send Bytes to the socket
 * @param TInt How many bytes are sent
 * @return Symbian error code
 */
TInt CT_RSocketData::SendBytesToSocket(TInt aBytes, TInt aDuration)
    {
    const TInt KDataBufferSize = 256;
    TBuf8<KDataBufferSize> data;
    data.SetMax();
    data.FillZ();
    TInt bytesSent = 0;

    TRequestStatus status;

    INFO_PRINTF1(_L("Uploading bytes to server"));
	iStart.HomeTime();

    // Loop while enough bytes are sent to socket
    while (bytesSent < aBytes)
        {
        if ((aBytes - bytesSent) < KDataBufferSize)
            {
            // this is the last round where we don't send full buffer anymore
            data.SetLength(aBytes - bytesSent);
            iSocket->Write(data, status);
            User::WaitForRequest(status);
            if (status.Int() != KErrNone)
                {
                ERR_PRINTF2(_L("Failed to send data to socket [%d]"), status.Int());
                return status.Int();
                }

            bytesSent += (aBytes - bytesSent);
            // We can break because there are no more data to be send
            break;
            }
        iSocket->Write(data, status);
        User::WaitForRequest(status);
        if (status.Int() != KErrNone)
            {
            ERR_PRINTF2(_L("Failed to send data to socket [%d]"), status.Int());
            return status.Int();
            }

        bytesSent += KDataBufferSize;
        
        if (aDuration != KNoDuration)
            {
            iEnd.HomeTime();
            TTimeIntervalSeconds seconds (0);
            TInt error = iEnd.SecondsFrom(iStart, seconds);
            if (error != KErrNone)
                {
            	ERR_PRINTF2(_L("Failed to get time interval with error %d"), error);
                return error;
                }
            if ( seconds.Int() >= aDuration )
                {
                // Transfer has lasted long enough. Let's stop transfer
                INFO_PRINTF2(_L("Transfer is stopped because defined duration [%d] exceeded"), seconds.Int());
                break;
                }
            }
        }

    iEnd.HomeTime();

    TTimeIntervalMicroSeconds microseconds = iEnd.MicroSecondsFrom(iStart);

	// Calculate the upload speed
    TInt error=0;
    TReal throughput = 0;
    if (bytesSent != 0)
        {
        throughput = ThroughputInKiloBits( microseconds, bytesSent );
        }
    	error= SetThroughput(throughput);

    if (error != KErrNone)
        {
        ERR_PRINTF2(_L("SetThroughput failed [%d]"), error);
        return error;
        }

    if (aDuration != KNoDuration)
        {
        // Close the socket because next we try to read response from the server and it won't come
        // because transfer is cut early (server waits until all bytes are sent or socket is closed)
        iSocket->Shutdown(RSocket::ENormal, status);
        User::WaitForRequest(status);
        
        TTimeIntervalSeconds seconds (0);
        TInt error = iEnd.SecondsFrom(iStart, seconds);
        if (error != KErrNone)
            {
        	ERR_PRINTF2(_L("Failed to get time interval with error %d"), error);
            return error;
            }
        if ( seconds.Int() < aDuration )
            {
            // Transfer didn't last the defined duration!
            ERR_PRINTF2(_L("Transfer failed, transfer did not last the defined duration [%d]"), aDuration);
            return KErrEof;
            }
        else
            {
            // If duration was used and socket was closed early return KErrCancel (it won't cause the
            // whole test case to fail
            return KErrCancel;
            }
        }

    return KErrNone;
    }



/**
 * Calculates throughput based on duration of a data transfer and total transferred bytes.
 * @param aDuration Duration of the transfer.
 * @param aTotalTransferred Total transferred data length in bytes.
 * @return Throughput in KBps.
 */
TReal CT_RSocketData::ThroughputInKiloBits( TTimeIntervalMicroSeconds aDuration, TInt aBytes )
	{
	const TReal KKiloConversion(1000.0);
	TReal throughput = ThroughputInMegaBits( aDuration, aBytes ) * KKiloConversion;
	
	return throughput;
	}


/**
 * Calculates throughput based on duration of a data transfer and total transferred bytes.
 * @param aDuration Duration of the transfer.
 * @param aTotalTransferred Total transferred data length in bytes.
 * @return Throughput in MBps.
 */
TReal CT_RSocketData::ThroughputInMegaBits( TTimeIntervalMicroSeconds aDuration, TInt aBytes )
	{
	const TReal KBitsInByte(8.0);
	TReal throughput = ( KBitsInByte * (TReal) aBytes ) / (TReal) aDuration.Int64();
	
	return throughput;
	}

/**
 * Set throughput into RProperty
 * @param aThroughput The calculated throughput
 */
TInt CT_RSocketData::SetThroughput( const TReal aThroughput )
    {
	TInt err = RProperty::Define( iPropertyCategory, iPropertyKey, RProperty::EByteArray );

	if( err == KErrAlreadyExists )
		{
		INFO_PRINTF1(_L( "Property already exists. Contents will be overridden") );
		err = KErrNone;
		}
	
	if (err == KErrNone)
		{
	    // Convert throughput value (TReal) into descriptor
	    TBuf8<256> amount;
	    TRealFormat realFormat;
	    err = amount.AppendNum( aThroughput, realFormat );
	    // Set property value
	    err = RProperty::Set( iPropertyCategory, iPropertyKey, amount );
		}
	else
		{
		ERR_PRINTF2(_L( "Could not create pub&sub property: %d"), err );
		}

    return err;
    }

/**
Check if RMobileCall has HSDPA indicator
@return TBool value of the indicator
*/
TBool CT_RSocketData::CheckHSDPAIndicator()
    {
    RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfo;
    return networkInfo.iHsdpaAvailableIndicator;
    }

/**
Check if RPacketService has HSDPA support
@param RMobileCall aMobileCall
@return TBool value of the indicator
*/
TBool CT_RSocketData::CheckHSDPASupportL(RMobilePhone &aMobilePhone)
    {
    RPacketService packetservice;
    
    CleanupClosePushL(packetservice);
    User::LeaveIfError(packetservice.Open(aMobilePhone));
    TUint caps;
    // Ask for static capabilities using CDMA context
    TInt error = packetservice.GetStaticCaps(caps, RPacketContext::EPdpTypeCDPD);
    CleanupStack::PopAndDestroy(&packetservice); // RPacketService.Close()

    User::LeaveIfError(error);
    
    return caps & RPacketService::KCapsHSDPASupported;
    }
    
/**
Check if RPacketService is currently using HSDPA
@param RMobileCall aMobileCall
@return TBool value of the indicator
*/
TBool CT_RSocketData::CheckHSDPAUsageL(RMobilePhone &aMobilePhone) 
    {
    RPacketService ps;
    
    CleanupClosePushL(ps); 
    User::LeaveIfError(ps.Open(aMobilePhone)); 
    RPacketService::TDynamicCapsFlags caps;
    TInt error = ps.GetDynamicCaps(caps);
	CleanupStack::PopAndDestroy(&ps); // RPacketService

	User::LeaveIfError(error); 
    
	return caps & RPacketService::KCapsHSDPA;
    }
