// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file ced.cpp
 @internalComponent
*/

#include <e32base.h>
#include <e32cons.h>
#include <e32std.h>
#include <f32file.h>
#include <bacline.h>
#include "dbdef.h"
#include "filedump.h"
#include "input.h"
#include "database.h"
#include "CXMLFile.h"
#include "CXMLContentHandler.h"
#include "CDBSTD.H"
#include <centralrepository.h>

/** max length of path/filename/column/table name */
#define MAX_BUFFER_LEN			256								

/** max length of path/filename/column/table name */
#define MAX_ATTRIB_LEN			32		

/** max length of arg list to program */
#define MAX_ARG_LEN				(MAX_BUFFER_LEN * 3)			

/** Application name  */
#define APPLICATIONNAME			_L("CommsDat Configuration Utility")

/** Version Info  */
#define CEDUNVERSIONED			_L("CED deliberately un-versioned") /** refer DEF092743 */

#ifdef __TOOLS2__

_LIT(CFG_TARGET, "ced.cfg");
_LIT(XML_TARGET, "ced.xml");
_LIT(LOG_TARGET, "ced.log");
_LIT(KMeshPrefaceFile, "meshpreface1.cfg");

	#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		#ifndef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
			_LIT(SUPPORTED_OS, "95");
		#else
			_LIT(SUPPORTED_OS, "");
		#endif
	#else
		#ifndef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
			_LIT(SUPPORTED_OS, "94");
		#else
			_LIT(SUPPORTED_OS, "93");
		#endif
	#endif

#else

_LIT(CFG_TARGET, "c:\\ced.cfg");
_LIT(XML_TARGET, "c:\\ced.xml");
_LIT(LOG_TARGET, "c:\\ced.log");
_LIT(KMeshPrefaceFile, "z:\\system\\data\\meshpreface1.cfg");
_LIT(DEFAULT_SESSION_PATH, "c:\\");
#endif


// GLOBALS
/** Logging  */
CFileDump* gMsg;	
/** Database access */	
DBAccess* gDB;
/** .ini file parsing */
CfgFile gCfg;
/** XML file parsing */		
XMLFile* gXML;
/** Console accessor */		
LOCAL_D CConsoleBase* gConsole;
/** Passed in arg list */		
HBufC* gArgumentLine;
/** Flag to indicate a successful run */
TBool gProcessingSuccessful = ETrue;
/** Flag to indicate whether the '-V' arg was passed in */
TBool gValidityChecking = EFalse;
/** flag to indicate whether the execution format is .dll or .exe*/
TBool gIsExeDLL = EFalse;
/** flag to indicate whether the configuration file is in XML format */
TBool gIsXML = ETrue;
/** flag to keep generic records */
TBool gKeepGenerics = ETrue;

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
TBool gDeprecatedFields = EFalse;
#endif
//SYMBIAN_NETWORKING_3GPPDEFAULTQOS

// PROTOTYPES

TInt ParseCommandLineL(TInt &aDebugOn, TBool &aOverWrite, TBool &aForceXMLProcessing, TDes &aIn, TDes &aOut, TDes &aInPref);
TInt ParseArgument(TInt &aDebugOn, TBool &aOverWrite, TBool &aForceXMLProcessing, TDes &aIn, TDes &aOut);

TInt DoProcessL(TBool aIsCfg);
void DoDeleteL();

TInt DoCfgTemplateL(TDesC &aTable, const TInt &aIndex);
TInt DoCfgInsertsL(TDesC &aTable, const TInt &aIndex);

TInt DoXMLTemplateL(const TInt &aIndex);
TInt DoXMLInsertsL(const TInt &aIndex);

TInt SetColumnDetailsL(TInt aIndex, TInt aRecordCount);
TInt SetXMLColumnDetailsL(TInt aEntryIndex, TInt aIndex, TInt aRecCount);

void DisplayMessage(TPtrC aMessage, TInt aJustTheMessage = EFalse);
void HelpDump();
void MainL();
LOCAL_C void doMainL();

TInt DoProcessL(TBool aIsCfg)
/** Store the information provided in the configuration file to CommDB

@param aIsCfg Whether the file to be parsed is an older style .cfg rather than .xml
@return TInt refer only called functions
@leave refer only called functions and dependant components
*/
	{
	TRAPD(ret, gDB->InitCommsdatL());

	// connect to DB manager
	if (ret==KErrNone)
		{
		TVersion version = gDB->Version();
		gMsg->Msg(_L("Database Version Maj %d Min %d Build %d"), version.iMajor, version.iMinor, version.iBuild);
		
		gMsg->Dbg(_L(""));
		gMsg->Dbg(_L("Session started"));
		
		// loop through tables
		TInt i = 0;
		TInt j = 0;
		TBuf<MAX_BUFFER_LEN> table = tableArray[i];
		TBuf<MAX_BUFFER_LEN> tempColumn;
		TBool changed;
		
		while (table.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			gMsg->Msg(_L(" "));
			gMsg->Msg(_L("%S Table"), &table);
			// get a count of editable columns
			j = 0;
			tempColumn = ColumnArray[i][j];
			while(tempColumn.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
				{
				j++;
				tempColumn = ColumnArray[i][j];
				}
			gMsg->Dbg(_L("(%d editable fields in table)"), j);
			
			TBool commitIndividualRecords = (table.CompareC(_L("ConnectionPreferences")) == 0);
			
			if(commitIndividualRecords)
				{
				// Commit any current changes
				TInt ret = gDB->CommitTransaction();
				if(ret != KErrNone)
					{
					gMsg->Msg(_L("ERROR: CommitTransaction returned err [%d]"), ret);
					if (gValidityChecking) 
						{
						gProcessingSuccessful = EFalse;
						}
					}
				}
			else
				{
				gDB->MaybeBeginTransactionL();
				}
			gDB->SetCommitIndividualRecords(commitIndividualRecords);
		
			changed = EFalse;			
			//Create a recordset for the table
			ret = KErrNone;
			if(!commitIndividualRecords)
				{
				TRAP(ret, gDB->CreateOrInsertRecordL(DBAccess::ECreateNew,elementIdArray[i], 0));			
				}
			if(ret == KErrNone)
				{
				if (aIsCfg?DoCfgTemplateL(table,i):DoXMLTemplateL(i))	
					{
					gMsg->Msg(_L(" "));
					changed = ETrue;
					}					
				
				if (aIsCfg?DoCfgInsertsL(table, i):DoXMLInsertsL(i))
					{
					gMsg->Msg(_L(" "));
					changed = ETrue;
					}				
				
				if (changed)
					{
					TInt  commitErr = gDB->CommitChanges();

					if (commitErr == KErrNone)
						{
						gMsg->Msg(_L("Insert successful"));
						}
					else
						{
						gMsg->Msg(_L("Insert failed [%d ]"), commitErr);
						if (gValidityChecking) 
							{
							gProcessingSuccessful = EFalse;
							}
						}
					}	
				else
					{
					gMsg->Dbg(_L("Nothing to process"));	
					}
				
				if(gDB->CommitIndividualRecords())
					{
					ret = gDB->CommitChanges();
					}
					gDB->CloseTable();				

				}
			else
				{
				gMsg->Dbg(_L("(Creating Record set Failed [%d ])"), ret);	
				}
			
			i++;
			table = tableArray[i];
			
			}
		ret = gDB->CommitTransaction();
		gDB->Close();
		}
	else
		{
		gMsg->Dbg(_L("(Could not connect to Commsdat [%d ])"), ret);
		}	
	
	return ret;
	}

void DoDeleteL()
/** Completely erase the communications database, not used when the append flag set

@return void
@leave refer only called functions and dependant components
*/
	{
	gMsg->Msg(_L("The old database will be replaced ") );
			
	// Get a session with the central repository
	const TUid KCDCommsRepositoryId = { 0xCCCCCC00 };
	CRepository*  storage = NULL;

	TRAPD(err, storage = CRepository::NewL(KCDCommsRepositoryId));
	if (KErrNone != err)
		{
		gMsg->Msg(_L("Failed to create Central Repository with ID : %x   (error %d)"), KCDCommsRepositoryId, err);
		_LIT(KCEDPanic, "CED");
		User::Panic(KCEDPanic, KErrNotFound);
		}
	CleanupStack::PushL(storage);
	
    // open transaction - if can't do this then fail.
    User::LeaveIfError(storage->StartTransaction(CRepository::EReadWriteTransaction));

	// Find everything in the database
	RArray<TUint32> ids;
    CleanupClosePushL(ids);
	err = storage->FindL(0, 0, ids);
	if(err != KErrNotFound)
		{
		User::LeaveIfError(err);
		}
	
	// delete everything in the database
	TInt keptElements = 0; 
	if (ids.Count())
		{
		for ( TInt i = ids.Count()-1; i >=0 ; i--)
			{
			if( i == ids.Count()-1 || i>=10000 && i%10000==0 || i<10000 && i>= 1000 && i%1000==0 ||
				i<1000 && i>=100 && i%100==0 || i<100 && i>=10 && i%10==0 || i<10)
				{
				gMsg->Msg(_L("%d"),i);
				}
			if(!gKeepGenerics || ids[i] < KCDInitialUDefRecordType || ids[i] > KCDLastUDefRecordType)
			    {
                User::LeaveIfError(storage->Delete(ids[i]));
			    }
			else
			    {
			    ++keptElements;
			    }
			}
		}
	
    gMsg->Msg(_L("Kept %d elements intact (KeepGenerics option == %d"), keptElements, gKeepGenerics);
	TUint32 aErrorId;
	err = storage->CommitTransaction(aErrorId);

	CleanupStack::PopAndDestroy(2);
	
	if (err != KErrNone)
		{
		gMsg->Msg(_L("Central Repository CommitTransaction returned err [%d]"), err);
		}	
	User::LeaveIfError(err);	
	}

void MainL()
/** Central processing unit of CED

@return void
@leave refer only called functions and dependant components
*/
	{
	TInt bDebugOn = EFalse;
	TBool bOverWrite = EFalse;
	TBool bForceXMLProcessing = EFalse;
	
	TBuf<MAX_BUFFER_LEN> fIn;
	TBuf<MAX_BUFFER_LEN> fOut;
	TBuf<MAX_BUFFER_LEN> fInPref;
	RFs fsSession;		// file system session
	
	// connect to file system
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	
	// check command line
	TInt valid;
	
	if (gIsExeDLL == EFalse)
		{
		// exe's get arguments from the command line
		valid = ParseCommandLineL(bDebugOn, bOverWrite, bForceXMLProcessing, fIn, fOut, fInPref);
		}
	else
		{
		// exedll'd get the parameters passed in 
		valid = ParseArgument(bDebugOn, bOverWrite, bForceXMLProcessing, fIn, fOut);
		}
	
	if (valid)
		{					
		// Get the type of the input file i.e. whether it is
		// an XML file or a CFG file
		(fIn.FindF(_L(".XML")) != KErrNotFound)?gIsXML = ETrue:gIsXML = EFalse;
		
		// display title
		gConsole->Printf(_L("------------------------------------------------------------\n"));
		gConsole->Printf(APPLICATIONNAME);
		gConsole->Printf(_L("\n------------------------------------------------------------\n\n"));
		gConsole->Printf(_L("CED is now processing the configuration file from [%S], \n"), &fIn);
		
		if ( bOverWrite )	gConsole->Printf(_L("The old database will be replaced \n") );
		
		if ( gIsXML )
			{
			gConsole->Printf(_L("The config file is in XML format \n") );
			if ( bForceXMLProcessing )
				{
				gConsole->Printf(_L("Invalid table entry links will be allowed \n") );
				}
			else
				{
				gConsole->Printf(_L("Invalid table entry links will not be allowed \n") );
				}
			}
		else
			{
			gConsole->Printf(_L("The config file is in CFG format \n") );
			}
		
		// initialise output log file

#ifndef __TOOLS2__
		// Set up the session path for WINSCW / ARMv5 incase the log file name has no path.
		fsSession.SetSessionPath( DEFAULT_SESSION_PATH );
#endif

		gMsg = CFileDump::NewL(fsSession, fOut, TPtrC(APPLICATIONNAME),
								TPtrC(CEDUNVERSIONED), bDebugOn, gConsole, EFalse);
		CleanupStack::PushL(gMsg);

		if (!gMsg->IsInitialised())
			{
			gConsole->Printf(_L("Failed to open the output log file [%S]\n"), &fOut);
			}
		gMsg->Msg(_L(" "));
		gMsg->Msg(_L("===================================================="));
		gMsg->Msg(APPLICATIONNAME);
		gMsg->Msg(_L("===================================================="));
		gMsg->Msg(_L(" "));
		gMsg->Msg(_L("Processing configuration from [%S]"), &fIn);
		
		gIsXML?gMsg->Msg(_L("The config file is in XML format ")):gMsg->Msg(_L("The config file is in CFG format ") );			
		
		if(gProcessingSuccessful)
			{
			gDB = new (ELeave) DBAccess(gIsXML);
			CleanupStack::PushL(gDB);
			gDB->CheckElementValidity(gValidityChecking);

	        //If the settings are not ovewritten we are appending
		    //This will take care of commdb ids
		    gDB->SetInAppendMode(!bOverWrite);

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
            TBool isMeshCompatible(EFalse);
                        
            if (bOverWrite)
            //The database has to be re-built...
                {
                
                CInputFileParser* fileParser = CInputFileParser::FactoryLC(gIsXML, fIn, gMsg);

                TRAPD(leaveVal, isMeshCompatible = fileParser->IsMeshCompatibleL());
                CleanupStack::PopAndDestroy(fileParser);

                if (leaveVal == KErrNone)
                    {
                    if (!isMeshCompatible)
                        {
        				gMsg->Msg(_L("MESHINFO: Configuration file [%S] is not mesh-compatible, inserting default meshtables"), &fIn);

                        // Insert the new mesh related tables/records into the database.

                        // The preface is a .cfg file, whatever the type of the original input file.
                        //
    			        if (gCfg.OpenConfigFile(fInPref))
    				        {
    						if (bOverWrite)
    							{
    				            DoDeleteL();
    							}
                            /**
                              create a link resolver object which resolves all of the
                              link by tag linking (Link.TableName.Id) to link by recordId
                              (TableName.RecordId).
                              This is important because there can be circular referencing
                              between tables and if these tables contain link by tag refencing
                              to each other commsDat doesn't allow to commit those tables...
                            */
                        	//this object will resolve the linkByTag linking to simple linkByRecId format
            				LinkByTagResolver* resolver = LinkByTagResolver::NewLC(&gCfg, gMsg);
                            
                            //set the resolver object to the gDB object
                            gDB->SetLinkByTagResolver(resolver);
                            
                            //Set the iIsXML field to false as we are processing now a cfg file...
            				gDB->CfgXmlSetting() = EFalse;
            				
            				//The MeshPreface file will be processed first. This is important to know since here
            				//we don't want to do any mappings...
            				gDB->SetMeshProcessing(ETrue); 
            				
            				DoProcessL(ETrue);
            				
            				//if originally xml processing was set then set it back now...
            				gDB->CfgXmlSetting() = gIsXML;
            				
            				//remove the resolver object form the gDB because it's not needed anymore
            				gDB->RemoveLinkByTagResolver();
            				CleanupStack::PopAndDestroy(resolver);
            				resolver = NULL;
            				
            				gCfg.CloseConfigFile();
            				if (!gValidityChecking)
            					{
            					gProcessingSuccessful = ETrue;
            					}
            				}
    			        else
    				        {
            				gMsg->Error(_L("MESHERR: Configuration file [%S] could not be opened"), &fInPref);
            				gProcessingSuccessful = EFalse;
            				}
            			}
            		else
            		    {
        				gMsg->Msg(_L("MESHINFO: Configuration file [%S] is mesh-compatible"), &fIn);
            		    }
            		}
                else
                    {
    				gMsg->Error(_L("MESHERR: Configuration file [%S], processing failed, reason=<%d>"), &fIn, leaveVal);
    				gProcessingSuccessful = EFalse;
    				}
                }
            else
                {
                gMsg->Msg(_L("Appending data to the DB - no need to use the meshpreface config file"));
                }
            
            
            if (gProcessingSuccessful)
                {
#endif
			// Process the XML configuration file
			if (gIsXML)
				{			    
				// Create the XML database
				CXMLDatabase* xmlDb = CXMLDatabase::NewL();
				CleanupStack::PushL(xmlDb);
				
				gXML = new (ELeave) XMLFile;
				CleanupStack::PushL(gXML);
				
				if (gXML->OpenConfigFile(fsSession,fIn,xmlDb,bForceXMLProcessing, !bOverWrite))
					{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                    if (isMeshCompatible)
                        {
#endif
						if (bOverWrite)
    					    {
    					    DoDeleteL();
							}
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        				}


                    if (isMeshCompatible)
                    	{
                    	gDB->SetMeshProcessing(ETrue);
                    	}
                    else
                    	
#endif
						{
                    	gDB->SetMeshProcessing(EFalse);
                    	}
                    
					TRAPD(ret, DoProcessL(EFalse));
                    gXML->CloseConfigFile();
                    
                    if (gValidityChecking && ret != KErrNone)
                    	{
                    	gProcessingSuccessful = EFalse;
                    	}
					}
				else
					{
					gMsg->Msg(_L("ERR: A problem was encountered while processing the configuration file [%S]"), &fIn);
					gProcessingSuccessful = EFalse;
					}
				
				// Cleanup
				CleanupStack::PopAndDestroy(gXML);
				CleanupStack::PopAndDestroy(xmlDb);

				gXML=NULL;
				xmlDb=NULL;
				}
			else
				{
				if (gCfg.OpenConfigFile(fIn))
					{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                    if (isMeshCompatible)
                        {
#endif
						if (bOverWrite)
						    {
    					    DoDeleteL();
							}
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        				}
                    /**
                      create a link resolver object which resolves all of the
                      link by tag linking (Link.TableName.Id) to link by recordId
                      (TableName.RecordId).
                      This is important because there can be circular referencing
                      between tables and if these tables contain link by tag refencing
                      to each other commsDat doesn't allow to commit those tables...
                    */
                    LinkByTagResolver* resolver = LinkByTagResolver::NewLC(&gCfg, gMsg);
                    
                    if (isMeshCompatible)
                    	{
                    	//our config file is a mesh compatible one so we can use the latestVersion in the DBSession
                    	gDB->SetMeshProcessing(ETrue);
                    	}
                    else
                    	{
                    	//our config file is not a mesh compatible one so we can should the version 1_1 in the 
                    	//DBSession
                    	gDB->SetMeshProcessing(EFalse);
                    	}
                    
                    gDB->SetLinkByTagResolver(resolver);
#endif
					TRAPD(ret, DoProcessL(ETrue));
					gCfg.CloseConfigFile();
					
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
					gDB->RemoveLinkByTagResolver();
					CleanupStack::PopAndDestroy(resolver);
					resolver = NULL;
#endif

                    if (gValidityChecking && ret != KErrNone)
                    	{
                    	gProcessingSuccessful = EFalse;
                    	}
					}
				else
					{
					gMsg->Msg(_L("ERR: Configuration file [%S] could not be opened"), &fIn);
					gProcessingSuccessful = EFalse;
					}
				}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                }
#endif
			CleanupStack::PopAndDestroy(gDB);
			gDB=NULL;
			}
		
		gMsg->Dbg(_L(""));
		gMsg->Dbg(_L("Session finished"));
		gMsg->Dbg(_L(""));
		gMsg->Msg(_L("==================="));
		
#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
		if (gDeprecatedFields)
			{
			gMsg->Msg(_L("WARNING: USE OF DEPRECATED FIELDS"));
			}
#endif
//SYMBIAN_NETWORKING_3GPPDEFAULTQOS

		// write our processing indicater to the log file
		gProcessingSuccessful?gMsg->Msg(_L("SUCCESS")):gMsg->Msg(_L("ERROR"));
	
		CleanupStack::PopAndDestroy(gMsg);
		}
		
	CleanupStack::PopAndDestroy(&fsSession);
	}

LOCAL_C void doMainL()
	{
	// allocate a gConsole
	gConsole = Console::NewL(APPLICATIONNAME, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(gConsole);
	
	// call main routine
	MainL();
	
	CleanupStack::PopAndDestroy(gConsole);
	}

TInt E32Main()
	{	
	__UHEAP_MARK;
	
	// set up trap cleanup framework
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	
	// call main routine and trap any exceptions
	TRAPD(ret,doMainL());
	if(ret != KErrNone && gValidityChecking)
		{
		gProcessingSuccessful = EFalse;
		}
	
	// clean up when finished
	delete theCleanup;
	
	__UHEAP_MARKEND;
	
	// Convert the boolean idea of "success" into the customary process idea of errorlevel, where zero
	// is success and anything else designates particular kinds of failure. Specific failure reasons aren't
	// provided, so the caller should only look for process exit != 0 to mean failure, and then examine the
	// log to see what failed.
	return !gProcessingSuccessful;
	}

TBool MatchArg(const TDesC& aArg, const TDesC& aOpt)
    {
    return aArg.Left(aOpt.Length()).CompareF(aOpt) == 0;
    }

TInt ParseCommandLineL(TBool &aDebugOn, TBool &aOverWrite, TBool &aForceXMLProcessing, TDes &aIn, TDes &aOut, TDes &aInPref)
/** Parse the command line for any overriding settings from exe command line 

@param bDebugOn Wether to output debug messages to the log file using CFileDump::Dbg(TPtrC text, ...)
@param bOverWrite Determines whether to append to or erase any existing databases
@param bForceXMLProcessing Determines if invalid table entry links will be allowed
@param fIn Path and filename of the configuration database file
@param fOut Filename of the file to send logging too
@return ETrue if Successful else EFalse 
*/
	{
	TInt valid = EFalse;
	CCommandLineArguments *pCmd;
	pCmd = CCommandLineArguments::NewL();
	CleanupStack::PushL(pCmd);
	
	// set defaults
	aIn  = XML_TARGET;
	aOut = LOG_TARGET;
	aInPref = KMeshPrefaceFile;
	
	// Flags for encountering the specification
	// of an input and an output file
	TBool bInFound  = EFalse;
	TBool bOutFound = EFalse;
	TBool bDatabaseSpecified = EFalse;
	
	//By default delete old database 
	aOverWrite = ETrue;
	
	if (pCmd)
		{
		TBuf<MAX_ARG_LEN> arg;
		
		// check all arguments for switches
		TInt i = 0;
		while( ++i < pCmd->Count() )
			{
			arg = pCmd->Arg(i);
			arg.UpperCase();
			
			// CED will report all the failures on the end, unlike success in all the cases except missing cfg file
			// Switch introduced because of high impact on test results and to avoid BC break
			if ( MatchArg(arg, _L("-V")) )
				{
				gValidityChecking = ETrue;
				continue;
				}

			//Display help
			if ( MatchArg(arg, _L("-H")) )
				{
				HelpDump();
				CleanupStack::Pop(pCmd);
				delete pCmd;
				return valid;
				}
			
			// Append database switch
			if ( MatchArg(arg, _L("-A")) )
				{
				aOverWrite = EFalse;
				continue;
				}
			
			// Debug switch
			if ( MatchArg(arg, _L("-D")) )
				{
				aDebugOn = ETrue;
				continue;
				}

			// Debug switch
			if ( MatchArg(arg, _L("-M")) )
				{
				aInPref = pCmd->Arg(++i);
				continue;
				}
			
            // Keep generics switch
            if ( MatchArg(arg, _L("-KG")) )
                {
                gKeepGenerics = ETrue;
                continue;
                }
			
            // Zap generics switch
            if ( MatchArg(arg, _L("-ZG")) )
                {
                gKeepGenerics = EFalse;
                continue;
                }

			// Presence of invalid table entry links
			// will not cause an error
			if ( MatchArg(arg, _L("-F")) )
				{
				aForceXMLProcessing = ETrue;
				continue;
				}
			
			// Specification of an input file
			if ( MatchArg(arg, _L("-I")) )
				{
				if( i != pCmd->Count()-1 )
					{
					aIn = pCmd->Arg(++i);
					bInFound = ETrue;
					continue;
					}
				else
					{
					gConsole->Printf(_L("Argument missing after '-i' switch\n"));
#ifndef __TOOLS2__
					gConsole->Printf(_L("\nPress any key to finish"));
					gConsole->Getch();
#endif
					CleanupStack::Pop(pCmd);
					delete pCmd;
					return valid;
					}
				}
			// Specification of an output file
			if ( MatchArg(arg, _L("-O")) )
				{
				if( i != pCmd->Count()-1 )
					{
					aOut = pCmd->Arg(++i);
					bOutFound = ETrue;
					continue;
					}
				else
					gConsole->Printf(_L("Argument missing after '-o' switch\n"));
#ifndef __TOOLS2__
					gConsole->Printf(_L("\nPress any key to finish"));
					gConsole->Getch();
#endif
					CleanupStack::Pop(pCmd);
					delete pCmd;
					return valid;
				}
#ifdef __TOOLS2__
			// Specification of the database binary version
			// This must be specified on the tools2 platform.
			TBuf<16> databaseVersion;
			
			if ( MatchArg(arg, _L("-B")) )
				{
				if( i != pCmd->Count()-1 )
					{
					databaseVersion = pCmd->Arg(++i);
					if(databaseVersion.Compare(SUPPORTED_OS) == 0)
						{
						bDatabaseSpecified = ETrue;
						continue;
						}
					else
						{
						gConsole->Printf(_L("O/S version '%S' is not supported by this version of CED"), &databaseVersion);
						CleanupStack::Pop(pCmd);
						delete pCmd;
						return valid;
						}
					}
				else
					{
					gConsole->Printf(_L("Argument missing after '-b' switch"));
					CleanupStack::Pop(pCmd);
					delete pCmd;
					return valid;
					}
				}
#endif
			// No flag is supplied: first file encountered is the input
			// file and the following one is the output file
			
			if ( !bInFound )
				{
				aIn = pCmd->Arg(i);
				bInFound = ETrue;
				continue;
				}
			if ( !bOutFound )
				{
				aOut = pCmd->Arg(i);
				bOutFound = ETrue;
				continue;
				}     
			}
		
		if (aIn.Length() > 0)
			valid = ETrue;
		
		//If here then no prefered config file has been specified.
		//Find default input file
        if ( !bInFound )
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
			TUint dummy;
			
			if(fs.Att(XML_TARGET,dummy)==KErrNone)
				{
				aIn  = XML_TARGET;
				}
			else if(fs.Att(CFG_TARGET,dummy)==KErrNone)
				{
				aIn  = CFG_TARGET;
				}
			else
				{
				gConsole->Printf(_L("No configuration files found please specify a .cfg or .xml file\n"));
#ifndef __TOOLS2__
				gConsole->Printf(_L("\nPress any key to finish"));
				gConsole->Getch();
#endif
				valid=EFalse;
				}
			CleanupStack::PopAndDestroy(&fs);
			}
		
#ifdef __TOOLS2__
		if ( !bDatabaseSpecified )
			{
			gConsole->Printf(_L("Must specify the '-b' switch\n"));
			valid=EFalse;
			}
#endif
		CleanupStack::Pop(pCmd);
		delete pCmd;	
	}
	
	return valid;
}

void HelpDump()
/**
Prints basic help information to the emulator window including command switches
*/
	{
	gConsole->Printf(_L("Creates/Updates a Comms Database using an input file"));
	gConsole->Printf(_L("\n\nCED [-a] [-d] [-f] [-i [path]filename] [-o [path]filename]"));
	gConsole->Printf(_L("\n\n-a\tAppends the data read from the input file to the existing Comms Database."));
	gConsole->Printf(_L("\n-d\tSwitches debug mode on giving more data in the log file."));
	gConsole->Printf(_L("\n-f\tAllows invalid data entries. Primarily used for testing."));
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	gConsole->Printf(_L("\n-m  Specifies the default mesh configuration file to CED. Defaults to reading '%S'."), &KMeshPrefaceFile);
#endif
	gConsole->Printf(_L("\n-v  Enables validation of the configuration data while writing to the database."));
	gConsole->Printf(_L("\n-i  Specifies an input file to CED. Must be either *.xml or *.cfg. Defaults to reading '%S' or '%S'."), &CFG_TARGET, &XML_TARGET);
	gConsole->Printf(_L("\n-o  Specifies an output file for CED to log to. Defaults to '%S'."), &LOG_TARGET);
    gConsole->Printf(_L("\n-kg Keep existing generic-record elements (default)"));
    gConsole->Printf(_L("\n-zg Zap all existing elements including generics (unless appending)"));
#ifdef __TOOLS2__
	gConsole->Printf(_L("\n-b  Specifies the binary output should be compatible with this Symbian OS version.\n"));
#endif
#ifndef __TOOLS2__
	gConsole->Printf(_L("\nPress any key to finish"));
	gConsole->Getch();
#endif
	}

TInt ParseArgument(TBool &aDebugOn, TBool &aOverWrite, TBool &aForceXMLProcessing, TDes &aIn, TDes &aOut)
/** Parse the command line for any overriding settings from DLL call

@param bDebugOn Wether to output debug messages to the log file using CFileDump::Dbg(TPtrC text, ...)
@param bOverWrite Determines whether to append to or erase any existing databases
@param bForceXMLProcessing Determines if invalid table entry links will be allowed
@param fIn Path and filename of the configuration database file
@param fOut Filename of the file to send logging too
@return ETrue if Successful else EFalse 
*/
	{
	// use Tlex to decode the cmd line
	TLex lex(gArgumentLine->Des());
	
	// By default delete old database 
	aOverWrite = ETrue;
	
	// get the input file
	aIn=lex.NextToken();
	if ( aIn.Length() == 0 ) 
		aIn = XML_TARGET;
	
	// get the output file
	aOut = lex.NextToken();
	if ( aOut.Length() == 0 ) // if valid potential 
		aOut = LOG_TARGET;
	
	// get the debug switch parameters
	if ( gArgumentLine->FindF(_L("-D")) != KErrNotFound )
		aDebugOn = ETrue;
	
	if ( gArgumentLine->FindF(_L("-O")) != KErrNotFound )
		aDebugOn = ETrue;
	
	// append switch
	if ( gArgumentLine->FindF(_L("-A")) != KErrNotFound )
		aOverWrite = EFalse;
	
	// force processing of XML file even in the
	// presence of invalid table entry links
	if ( gArgumentLine->FindF(_L("-F")) != KErrNotFound )
		aForceXMLProcessing = ETrue;	
	
	delete gArgumentLine;
	
	return ETrue;
	}

LOCAL_C TInt ProcessCfgDataL(TDesC &aTable,const TInt &aIndex)
/** Process old style .cfg database file 

@param aTable Name of the entry being processed
@param aIndex Counted table number being processed
@return ETrue if Successful else EFalse 
*/
	{
	TInt recordsInserted(0);  
	TInt fatalErr(KErrNone); 
	TBool firstRec(ETrue);
	TInt valid(KErrNotReady);
	
	_LIT(KConnectionPrefTableName, "ConnectionPreferences");
	while(fatalErr == KErrNone && (firstRec || gCfg.StepToNextBlock()))
		{
		if (!firstRec)
			{
			gMsg->Msg(_L(" "));	// blank line between records
			}

		if(gDB->CommitIndividualRecords())
			{
 			gDB->CreateOrInsertRecordL(firstRec? DBAccess::ECreateNew: DBAccess::ECreateInsert,elementIdArray[aIndex], 0);			
 			}
 		firstRec = EFalse;
 		
		TInt value = recordsInserted + 1;
		TPtrC idText;
		_LIT(KCommDbIDTag, "COMMDB_ID ");
		valid = gCfg.GetSetting(KCommDbIDTag, idText);
		if (valid == KErrNone)
			{
			TLex lex(idText);
			if(lex.Val(value) != KErrNone)
				{
				gMsg->Dbg(_L("Missing COMMDB_ID - defaulting to record count"));
				}
			}
		gDB->CreateOrInsertRecordL(DBAccess::EInsert, 0, value); 			
		
		// find out how many fields we are expecting in this insert
		TPtrC result = NO_MORE_RECORDS;
		TInt iValue;
		valid = gCfg.GetSetting(FIELD_COUNT, result);
		if (valid == KErrNone)
			{
			TLex iResult = result;	
			iValue = 0;		
			if (iResult.Val(iValue) == KErrNone)
				gDB->SetFieldCount(iValue);
			}
		
		//Write column info
		TInt err = SetColumnDetailsL(aIndex, recordsInserted + 1);
		iValue = gDB->GetFieldCount(); //If Recordname was not supplied, SetColumnDetailsL fn would have added the misisng name..so refresh he actual field count
		
		if(err == KErrNone)
			{
			if(gDB->GetActualFieldCount() != iValue)
				{
				gMsg->Msg(_L(" ERR Table %S Expected [%d] values, read [%d].Field count mismatch!"), &aTable, iValue, gDB->GetActualFieldCount());
				gDB->RemoveCurrentRecord();
				if (gValidityChecking) 
					{
					gProcessingSuccessful = EFalse;
					}
				continue;	// it's not fatal
				}
			
			if(aTable.Compare(KConnectionPrefTableName) == 0)
				{
				CMDBRecordBase* dontDelete = gDB->GetCurrentRecord();
				if(!gDB->IsConnectionPrefValid(*(static_cast<CCDConnectionPrefsRecord*>(dontDelete))))
					{
					fatalErr = KErrArgument;	// or should we continue?
					}					
				}
			if(gDB->CommitIndividualRecords())
				{
				err = gDB->CommitChanges();
				}
			if(err != KErrNone)
				{
				gMsg->Msg(_L("Error %d inserting record #%d to %S"), err, recordsInserted + 1, &aTable);
				if (gValidityChecking) 
					{
					gProcessingSuccessful = EFalse;
					}
	 			}
			else
	 			{
				recordsInserted++;
	 			}
			}
		else if(err != KErrCancel)	//to ignore cancels called by ced itself, used to process unused entries produced by older ceddumpers
			{
			gMsg->Msg(_L("Error %d inserting record #%d to %S"), err, recordsInserted + 1, &aTable);
			if (gValidityChecking) 
				{
				gProcessingSuccessful = EFalse;
				}
			
			gDB->RemoveRecord(recordsInserted);
			//fatalErr = err;
			}
		}
		
	return ((fatalErr == KErrNone)?recordsInserted:fatalErr);
	}

TInt DoCfgTemplateL(TDesC &aTable, const TInt &aIndex)
/**
Applying the templates

  @param table A reference to a descriptor containing the name of a table
  @param index Index in the Table
  @return ETrue if Successful else EFalse 
  */
	{
	// look for template and load it
	if (!gCfg.OpenTemplateBlock(aTable))
		return EFalse;
	
	DisplayMessage(_L("Processing template"), ETrue);
	gDB->RecordIsTemplate(ETrue); 	
	
	TInt templateRecordsInserted = ProcessCfgDataL(aTable,aIndex);
	
	if (templateRecordsInserted > 0)
		{
		gMsg->Msg(_L(" "));
		gMsg->Msg(_L("[%d] Template record(s) successfully inserted"), templateRecordsInserted);
		}
	else
		{
		gMsg->Msg(_L(" "));
		gMsg->Msg(_L("Insert record failed Err:[%d]"), templateRecordsInserted);
		}
	
	return templateRecordsInserted;
	}

TInt DoCfgInsertsL(TDesC &aTable, const TInt &aIndex)
/**
Runs through all the steps involved in applying an insert to a particular table.

  @param table A reference to a descriptor containing the name of a table
  @param index Index in the Table
  @return ETrue if Successful else EFalse 
  */
	{
	
	// look for any inserts and load the first one
	if (!gCfg.OpenAddBlock(aTable))
		return EFalse;
	
	DisplayMessage(_L("Processing inserts"), ETrue);
	gDB->RecordIsTemplate(EFalse); 	

	TInt recordsInserted = ProcessCfgDataL(aTable,aIndex);
	
	if (recordsInserted >= 0)
		{
		gMsg->Msg(_L(" "));
		gMsg->Msg(_L("[%d] record(s) successfully inserted"), recordsInserted);
		}
	else
		{
		gMsg->Msg(_L(" "));
		gMsg->Msg(_L("Insert record failed Err:[%d]"), recordsInserted);
		}
	return recordsInserted;			
	}

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
/**
Loops through the table of deprecated columns/parameters and gives an information 
to the user question column/parameter is deprecated. 

  @param aIndex    Index in the elementIdArray Table
  @param aColumnName Name of the questioned column/parameter
*/
void VerifyDeprecatedFields(TInt aIndex, TPtrC& aColumnName)
	{
	TBuf<MAX_BUFFER_LEN> tempColumn;
	TInt i = 0;
	if (elementIdArray[aIndex] == KCDTIdOutgoingGprsRecord 
		|| elementIdArray[aIndex] == KCDTIdIncomingGprsRecord )
		{
		tempColumn = DeprecParamArray[i];
		while(tempColumn.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if (aColumnName.Compare(tempColumn) == 0 )
				{
				gMsg->Msg(_L(" Warning - Use of deprecated parameter [%S]"), &aColumnName);
				gDeprecatedFields = ETrue;
				}
			tempColumn = DeprecParamArray[++i];
			}
		}
	}
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS

TBool IsGprsGatewayField(TInt aIndex, const TPtrC& aColumnName)
/**
Determine if a field being processed is the IpGateway field in OutgoingGPRS table

@param aIndex index into elementIdArray[] for identifying table
@param aColumnName descriptor identifying the field
*/
	{
	return (elementIdArray[aIndex] == KCDTIdOutgoingGprsRecord && aColumnName.Compare(TPtrC(GPRS_IP_GATEWAY)) == 0);
	}

TInt SetupIpGatewayFieldDefaultL(TInt aIndex)
/**
Setup a safe default value for the IpGateway field in OutgoingGPRS tables.  If a value has not
been specified in the CFG or XML file, then it would ordinarily default to 0.0.0.0.  However,
this value will mean that the TCP/IP stack will not setup a default route.  Consequently,
we must setup a safe default of 0.0.0.1.

@param aIndex index into elementIdArray[] for identifying the table
*/
	{
	if (elementIdArray[aIndex] == KCDTIdOutgoingGprsRecord)	// only for OutgoingGPRS table records !
		{
		// Only setup IpGateway field if it is not already present in the OutgoingGPRS template record.
		if (!gDB->TemplateFieldPresentL(KCDTIdOutgoingGprsRecord | KCDTIdWCDMAIPGateway))
			{
			TPtrC setting;
			TPtrC column;
			_LIT(KGprsIpGateway, "0.0.0.1");
			setting.Set(KGprsIpGateway());
			column.Set(TPtrC(GPRS_IP_GATEWAY));
			gMsg->Msg(_L(" NOTE  [%S] set to default value of %S"), &column, &setting);
			TInt ret = gDB->SetColAndAttribL(column, setting);
			gDB->SetFieldCount(gDB->GetFieldCount() + 1); //field count increment by one because we added the gateway field
			return ret;
			}
		}
	return KErrNone;
	}
	
void HandleConnPrefWithoutDefaultIap()
/**
Handle special case of ConnPref record without default IAP set (empty db use case)
*/
	{
	if(gDB->GetCurrentRecord()->TableId() == KCDTIdConnectionPrefsRecord)
		{
		CCDConnectionPrefsRecord* connPref = static_cast<CCDConnectionPrefsRecord*>(gDB->GetCurrentRecord());
		if(!(connPref->iDefaultIAP.ElementId() & KCDChangedFlag))
			{
			connPref->iDefaultIAP = 0;
			}
		}
	}

TInt SetColumnDetailsL(TInt aIndex, TInt aRecordCount)
/**
Loops through all the columns of a particular table.  If any of the columns match the setting
in the log file, it is stored, along with the value to write into this field.

@param aTable    A reference to a descriptor containing the name of a table
@param aIndex    Index in the Table
@param aRecCnt   Index of the Record in the table
@return ETrue    if Successful else EFalse 
*/
	{	
	
	static TBuf<MAX_BUFFER_LEN> tempColumn;
	TPtrC column;
	TPtrC setting;
	TInt valid(KErrNone);
	TInt i = 0;
	tempColumn = ColumnArray[aIndex][i];
	TInt fieldValueRead(KErrNone);
	TBool gprsGatewayFieldIsSet = EFalse;		// whether IpGateway field in OutgoingGPRS table record has been set

	// loop through columns of table
	while( (valid==KErrNone) && (tempColumn.Compare(TPtrC(NO_MORE_RECORDS)) != 0))
		{		
		// get a column from the config file
		gMsg->Dbg(_L(" Looking for [%S]"), &tempColumn);
		// set the read value to the column
		fieldValueRead = gCfg.GetSetting(tempColumn,setting);
		if(fieldValueRead == KErrNone)
			{
			column.Set(tempColumn);
         	
#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS			
			VerifyDeprecatedFields(aIndex, column);
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS			

			// Determine whether the GPRS IP Gateway field has been set or not
			if (IsGprsGatewayField(aIndex, column))
				{
				gprsGatewayFieldIsSet = ETrue;
				}

			if( (tempColumn.Compare(TPtrC(COMMDB_NAME)) == 0) &&
				(setting.Compare(TPtrC(COMMDB_UNUSED_NAME)) == 0)
				)
				{
				//this record is not valid,,,remove
				gDB->RemoveCurrentRecord();
				if (gValidityChecking) 
					{
					gProcessingSuccessful = EFalse;
					}
				return KErrCancel;				
				}
			
			valid = gDB->SetColAndAttribL(column,setting);
			}	
		// decrement the field count for fields simply left blank
		else if(fieldValueRead == KErrArgument)
			{
			gDB->SetFieldCount(gDB->GetFieldCount() - 1);	
			}
		i++;
		tempColumn = ColumnArray[aIndex][i];	
		}
	
	//if the record is not empty and also has its name field or fieldvalue missing, set a default name, using the 
	//record id if set, else simply the record count
	//Only do this if not entering a Template Record
    if(gDB->GetCurrentRecord()->RecordId() != KCDDefaultRecord && 
       ((!gDB->IsNameSet() && gDB->GetActualFieldCount()) ||
        (!gDB->IsNameSet() && KErrArgument)) )	
		{
		_LIT(KNameTemplate, "DefaultRecordName-%d");
		TBuf<MAX_BUFFER_LEN> recordName;
		TInt recId = gDB->GetCurrentRecord()->RecordId();
		recordName.Format(KNameTemplate, recId? recId: aRecordCount);
		column.Set(TPtrC(COMMDB_NAME));		
		setting.Set(TPtrC(recordName));	    			
		valid = gDB->SetColAndAttribL(column,setting);			
		gDB->SetFieldCount(gDB->GetFieldCount() + 1); //field count increment by one because we added the missing name
	    }
	
	if (valid == KErrNone && !gprsGatewayFieldIsSet)
		{
		// If required, setup a suitable default value for IpGateway field in OutgoingGPRS table records
		valid = SetupIpGatewayFieldDefaultL(aIndex);
		}
	
	// Handle special case of ConnPref record without default IAP set (empty db use case)
	HandleConnPrefWithoutDefaultIap();

	return valid;
	}

LOCAL_C TInt ProcessXmlDataL(TBool aIsTemplate,const TInt &aIndex)
/** Runs through all the steps involved in applying an insert to a particular table
	(for the XML configuration file) 

@param aIsTemplate Wether the entry being processed is a template or add entry
@param aIndex Counted table number being processed
@return Number of successfully inserted records
*/
	{
	TBuf<MAX_BUFFER_LEN> xmlTableName = xmlTableArray[aIndex];
	xmlTableName.Append(_L("Table"));
	
	// look for template
	TInt startIndex = gXML->GetStartingIndex(xmlTableName);
	TInt lastIndex  = gXML->GetLastIndex(xmlTableName);
	
	if(startIndex == -1 || lastIndex == -1)
		return EFalse;
	
	if(aIsTemplate)
		{
		DisplayMessage(_L("Processing template"), ETrue);	
		gDB->RecordIsTemplate(ETrue); 
		}
	else
		{
		DisplayMessage(_L("Processing inserts"), ETrue);	
		gDB->RecordIsTemplate(EFalse); 
		}
	
	TInt recordsInserted(0);
	TInt ret(KErrNone);
	
	for(TInt iEntryIndex = startIndex; iEntryIndex <= lastIndex; iEntryIndex++)
		{
		if(aIsTemplate)
			{
			if(gXML->GetOperation(iEntryIndex).Compare(_L("template")) != 0)
				{
				continue;	
				}				
			}
		else
			{
			if(gXML->GetOperation(iEntryIndex).Compare(_L("add")) != 0)
				{
				continue;
				}					
			}
		
		gMsg->Msg(_L("Record Number: #[%d]"), gXML->GetElementRecordID(iEntryIndex));
		if(gDB->CommitIndividualRecords())
			{
			gDB->CreateOrInsertRecordL(iEntryIndex == startIndex? DBAccess::ECreateNew: DBAccess::ECreateInsert,elementIdArray[aIndex], 0);			
			}
	    
	    //!!!!!!!!!!!!!!!!!the record ID of the given xmlDBentry should be read here...
	    
		//gDB->CreateOrInsertRecordL(DBAccess::EInsert,0,0); 
		
		gDB->CreateOrInsertRecordL(DBAccess::EInsert,0,gXML->GetElementRecordID(iEntryIndex)); 
		
		// find out how many fields we are expecting in this insert
		TInt iValue = gXML->GetEntryNumberParameters(iEntryIndex);
        gDB->SetFieldCount(iValue);
        
        //Write column info
		ret = SetXMLColumnDetailsL(iEntryIndex,aIndex,iEntryIndex - startIndex + 1);
		/*if(ret != KErrNone)
			return ret;*/
		
		if(gDB->CommitIndividualRecords())
			{
			ret = gDB->CommitChanges();
			}
		if(ret != KErrNone)
			{
			gMsg->Msg(_L("Error %d inserting record #%d to %S"), ret, iEntryIndex - startIndex + 1, &xmlTableName);

			if (gValidityChecking) 
				{
				gProcessingSuccessful = EFalse;
				}
			
			gDB->RemoveRecord(gXML->GetElementRecordID(iEntryIndex)-1);
			
			}
		else
			{
			recordsInserted++;
			}
		}
	
	if (recordsInserted)
		{
		gMsg->Msg(_L(" "));
		gMsg->Msg(_L("[%d] record(s) successfully inserted"), recordsInserted);
		}
	
	return recordsInserted;	
	}


TInt DoXMLInsertsL(const TInt &aIndex)
/**
Runs through all the steps involved in applying an insert to a particular table.

@param index Index in the Table
@return Number of successfully inserted records
*/
	{
	return (ProcessXmlDataL(EFalse,aIndex));
	}

TInt DoXMLTemplateL(const TInt &aIndex)
/**
Applying the templates (for the XML configuration file)

@param index Index in the Table
@return Number of successfully inserted records
*/
	{
	return (ProcessXmlDataL(ETrue,aIndex));
	}

TInt SetXMLColumnDetailsL(TInt aEntryIndex, TInt aIndex, TInt aRecCount)
/** Loops through all the columns of a particular table.  If any of the columns match the setting
	in the log file, it is stored, along with the value to write into this field.

@param aEntryIndex 
@param aIndex 
@param aRecCount 
@return True if successful else false 
*/
	{
	static TBuf<MAX_BUFFER_LEN> tempColumn;
	TPtrC setting;
	TPtrC column;
	TInt valid(KErrNone);
	TInt i = 0;
	TBool gprsGatewayFieldIsSet = EFalse;		// whether IpGateway field in OutgoingGPRS table record has been set

	tempColumn = ColumnArray[aIndex][i];
	
	
	// loop through columns of table
	while( (valid==KErrNone) && tempColumn.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		{
		// get a column from the config file
		gMsg->Dbg(_L(" Looking for [%S]"), &tempColumn);
		
		if (gXML->GetSetting(aEntryIndex, tempColumn, setting))
			{
			column.Set(ColumnArray[aIndex][i]);	
			valid = gDB->SetColAndAttribL(column,setting);

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS			
			VerifyDeprecatedFields(aIndex, column);
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS

			// Determine whether the GPRS IP Gateway field has been set or not
			if (IsGprsGatewayField(aIndex, column))
				{
				gprsGatewayFieldIsSet = ETrue;
				}
			}
		i++;
		tempColumn = ColumnArray[aIndex][i];
		}	
	
	//if the record is not empty and also has its name missing, set a default name
    //only do this if not processing a template record
	if(!gDB->IsNameSet() && 
        gDB->GetActualFieldCount() && 
        gDB->GetCurrentRecord()->RecordId() != KCDDefaultRecord)	
		{		
		_LIT(KNameTemplate, "DefaultRecordName-%d");
		TBuf<MAX_BUFFER_LEN> recordName;
		recordName.Format(KNameTemplate, aRecCount);
		column.Set(TPtrC(COMMDB_NAME));		
		setting.Set(TPtrC(recordName));	    			
		valid = gDB->SetColAndAttribL(column,setting);	
		gDB->SetFieldCount(gDB->GetFieldCount() + 1); //field count increment by one because we added the missing name
	    }

	if (valid == KErrNone && !gprsGatewayFieldIsSet)
		{
		// If required, setup a suitable default value for IpGateway field in OutgoingGPRS table records
		valid = SetupIpGatewayFieldDefaultL(aIndex);
		}
	
	// Handle special case of ConnPref record without default IAP set (empty db use case)
	HandleConnPrefWithoutDefaultIap();

	return valid;
	}

void DisplayMessage(TPtrC aMessage, TInt aJustTheMessage)
/**
Writes a message to the log file, along with any associated error.

@param aMessage Message to be written 
@param aJustTheMessage Output only the plain message to the log file
*/
	{
	if (aJustTheMessage)
		{
		gMsg->Msg(_L("%S"), &aMessage);
		}
	else
		{
		if (gDB->ErrorCode() != KErrAlreadyExists && gDB->ErrorCode() != E_ALREADYEXISTS)
			gMsg->Msg(_L(" "));
		
		gMsg->Msg(_L(" "));
		
		if (gDB->ErrorCode())
			{
			static TBuf<MAX_BUFFER_LEN> errorText;
			errorText = gDB->ErrorText();
			
			if (gDB->ErrorCode() == KErrAlreadyExists || gDB->ErrorCode() == E_ALREADYEXISTS)
				gMsg->Msg(_L("WARN: %S failed: %S [%d]"), &aMessage, &errorText, gDB->ErrorCode());
			else if (gDB->ErrorCode() < 0)
				gMsg->Msg(_L("ERR: %S failed: %S [%d]"), &aMessage, &errorText, gDB->ErrorCode());
			else
				gMsg->Msg(_L("ERR: %S failed: %S"), &aMessage, &errorText);
			}
		else
			{
			gMsg->Msg(_L("ERR: %S failed"), &aMessage);
			}
		
		// don't fail when the record is there already
		if (gDB->ErrorCode() != KErrAlreadyExists && gDB->ErrorCode() != E_ALREADYEXISTS)
			{
			gMsg->Msg(_L(" "));
			gMsg->Msg(_L(" "));
			gProcessingSuccessful = EFalse;
			}
		}
	}


#ifndef __TOOLS2__
EXPORT_C TInt NewApplicationL(const TDesC& aText)
/**
entry point used in WINS when ced is build as a exedll

@param Text Command line argument
*/
	{
	// save the command line info
	gArgumentLine=aText.AllocL();
	
	// record that ced is running as a exe dll
	gIsExeDLL = ETrue;
	
	// call main
	return E32Main();
	}
#endif


