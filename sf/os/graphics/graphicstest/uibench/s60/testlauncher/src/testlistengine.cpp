 // Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "testlistengine.h"
#include "testlauncher.hrh" // enumerations
#include <documenthandler.h> // for launching
#include "testlauncher.pan"

#include <aknnotewrappers.h>
#include <bautils.h>
#include <PathInfo.h>


// Number, name and file size
_LIT(KStringSize,"%d\t%S\t%d bytes");
// Number, name and date modified
_LIT(KStringDate,"%d\t%S\t%S");

_LIT(KSlash, "\\");
_LIT(KTestListC, "c:\\uibench_s60\\");
_LIT(KTestListE, "e:\\uibench_s60\\");
_LIT(KTestListF, "f:\\uibench_s60\\");
_LIT(KTestListZ, "z:\\uibench_s60\\");

const TPtrC KTestExecute = _L("testexecute.exe");
const TPtrC KScriptExt = _L(".script");
const TInt KDateLength(30);


void CFileListEngine::ConstructL(CEikProcess* aProcess /*= NULL*/)
    {
#ifdef __SERIES60_3X__
    aProcess = NULL;
    iDocHandler = CDocumentHandler::NewL();
#else
    iDocHandler = CDocumentHandler::NewL(aProcess);
#endif
    }

CFileListEngine::~CFileListEngine()
    {
    delete iDocHandler;
    iFileList.Close();
    iDirList.Close();
    }

// This Method gets the specific directory by using PathInfo.
TInt CFileListEngine::StartFileList()
    {
    iFileList.Reset();
    iDirList.Reset();

    // Connect to fileserver
    TInt error = iFsSession.Connect();
    if(error != KErrNone)
        {
        return error;
        }

    TFileName pathC; 
    TFileName pathE; 
    TFileName pathF; 
    TFileName pathZ; 
  	pathC.Append(KTestListC);
  	pathE.Append(KTestListE);
  	pathF.Append(KTestListF);
  	pathZ.Append(KTestListZ);
  	
    TRAP(error, GetResourceFilesL(pathC));
    if(error != KErrNone)
        {
        return error;
        }
    TRAP(error, GetResourceFilesL(pathE));
    if(error != KErrNone)
        {
        return error;
        }
    TRAP(error, GetResourceFilesL(pathF));
    if(error != KErrNone)
        {
        return error;
        }
    TRAP(error, GetResourceFilesL(pathZ));
    return error;
    }

// The method get all files including files into subfolders.
void CFileListEngine::GetResourceFilesL (TFileName &aBasePath)
    {
    CDir* dirs = 0;
    CDir* files = 0;

    if(!BaflUtils::PathExists(iFsSession, aBasePath))
        {
        return;
        }

    // Get dir. KEntryAttNormal means that no hidden files or directories are included
    User::LeaveIfError(iFsSession.GetDir(aBasePath, KEntryAttNormal, ESortByName, files, dirs));

    CleanupStack::PushL(dirs);
    CleanupStack::PushL(files);

    //get files in base path
    for(TInt i = 0; i < files->Count(); ++i)
        {
        // Only list files with .script extension
        TInt res =(*files)[i].iName.Find(KScriptExt);    
        if (res != KErrNotFound)
            {
            iFileList.Append((*files)[i]);
            iDirList.Append(aBasePath);
            }
        }

    //get subfolders
    for(TInt i = 0; i < dirs->Count(); ++i)
        {
        // Exclude directories beginning with '_' as they can
        // contain system-generated thumbnails etc.
        if ((*dirs)[i].iName[0] != '_')
            {
            TFileName nextDir = aBasePath;
            nextDir.Append((*dirs)[i].iName);
            nextDir.Append(KSlash);
            GetResourceFilesL(nextDir);
            }
        }
    // Number 2 cannot be considered a "magic number" in this case,
    // and PopAndDestroy does get a parameter. Thus, the CSIs.
    CleanupStack::PopAndDestroy(2, dirs);  // CSI: 47,12 #
  }

// This Method constructs the listbox items with directory
// information
void CFileListEngine::GetFileListItemsL(CDesCArray* aItems)
    {
    for (TInt i = 0; i < iFileList.Count(); ++i)
        {
        TFileName filename(KNullDesC);
        if(iSizeDate == EFileListSize)
            {
            // Show file size
            filename.Format(KStringSize,i+1,&iFileList[i].iName,iFileList[i].iSize);
            }
        else
            {
            // Fix the date and time string of last modification
            TBuf<KDateLength> dateString;
            _LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
            iFileList[i].iModified.FormatL(dateString,KDateString);

            // Show date modified
            filename.Format(KStringDate,i+1,&iFileList[i].iName,&dateString);
            }
        aItems->AppendL(filename);
        }
    }

// This Method sets which directory to list.
void CFileListEngine::SetDirectory(TInt aDirectory)
    {
    if (aDirectory != EFileListDirNoChange)
        {
        iDirectory = aDirectory;
        }
    }

// This Method launches selected test with testtexecute.
void CFileListEngine::LaunchCurrentL(TInt aPosition)
    {
    TFileName descr = iDirList[aPosition];
    if(aPosition <= iFileList.Count())
        {
        // Add filename to be launched
        descr.Append(iFileList[aPosition].iName);
        }
    else
        {
        Panic(EFileListIvalidIndex);
        }  
    
    RProcess p;
    TInt err = p.Create(KTestExecute, descr);
    if (err != KErrNone) 
    	{
    	CEikonEnv::Static()->InfoMsg(_L("Unable to create process"));
    	}
    	
    p.Resume();
    p.Close();
    }

// This Method removes all listbox items when a new list
// needs to be shown.
TBool CFileListEngine::RemoveItems(CDesCArray* aItems)
    {
    if (iFileList.Count())
        {
        aItems->Delete(0,(iFileList.Count()));
        return ETrue;
        }
    return EFalse;
    }

TBool CFileListEngine::IsDirListEmpty()
    {
    return (iFileList.Count() == 0);
    }

// This Method sets whether modification date or file size
// is shown. There is also option for toggling the status.
void CFileListEngine::SetSizeDate(TInt aSizeDate)
    {
    if (aSizeDate==EFileListToggle)
        {
        if (iSizeDate==EFileListSize)
            {
            iSizeDate=EFileListDate;
            }
        else
            {
            iSizeDate=EFileListSize;
            }
        }
    else
        {
        if (aSizeDate!=EFileListSizeDateNoChange)
            {
            iSizeDate=aSizeDate;
            }
        }
    }

// This Method ends the FileList session
void CFileListEngine::EndFileList()
    {
    // Close the file server session
    iFsSession.Close();
    };

#ifdef __SERIES60_3X__
void CFileListEngine::HandleServerAppExit(TInt aReason)
    {
    iDocEmbedded = EFalse;
    MAknServerAppExitObserver::HandleServerAppExit(aReason);
}
#else
void CFileListEngine::NotifyExit(TExitMode /*aMode*/)
    {
    iDocEmbedded = EFalse;
    }
#endif
