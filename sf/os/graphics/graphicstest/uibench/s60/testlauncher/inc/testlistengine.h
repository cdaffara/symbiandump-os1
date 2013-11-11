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

#ifndef FILELISTENGINE_H
#define FILELISTENGINE_H

#include <coecntrl.h>
#include <f32file.h>        // for CDir
#include <badesca.h>        // for CDesCArray
#include <apmstd.h>         // for TDataType

#ifdef __SERIES60_3X__
#include <aknserverapp.h>
#else
#include <apparc.h>
#endif

class CDocumentHandler;
class CEikProcess;

class CFileListEngine : public CCoeControl,
#ifdef __SERIES60_3X__
  public MAknServerAppExitObserver
#else
  public MApaEmbeddedDocObserver
#endif
    {
    public:
        void ConstructL(CEikProcess* aProcess = NULL);
        virtual ~CFileListEngine();

    public:
        // This Method gets the specific directory.
        // In the 1st edition use harcoded paths and the S60 2nd and 3rd edition
        // get the specific path using PathInfo.
        TInt StartFileList();

        // This Method constructs the listbox items with directory
        // information
        void GetFileListItemsL(CDesCArray* iItems);

        // This Method sets which directory to list.
        void SetDirectory(TInt aDirectory);

        // CFileListEngine::SetSizeDate(TInt aSizeDate)
        // This Method sets whether modification date or file size
        // is shown. There is also option for toggling the status.
        void SetSizeDate(TInt aSizeDate);

        // This Method ends the FileList session
        void EndFileList();

        // This Method removes all listbox items when a new list
        // needs to be shown.
        TBool RemoveItems(CDesCArray* aItems);

        // This Method launches selected item with DocumentHandler.
        // DocumentHandler will launch correct application depending
        // on the file type.
        // Note that all the extensions do not work on emulator.
        void LaunchCurrentL(TInt aPosition);

        TBool IsDirListEmpty();

        // The method get all files including files into subfolders.
        void GetResourceFilesL ( TFileName &aBasePath );

#ifdef __SERIES60_3X__
        void HandleServerAppExit(TInt aReason);
#else
        void NotifyExit(TExitMode aMode);
#endif

    private:
        // Define local CDir variable
        RArray<TEntry> iFileList;
        RArray<TFileName> iDirList;

        // Fileserver session
        RFs iFsSession;

        // Which directory to show
        TInt iDirectory;
        
        // Show date or size?
        TInt iSizeDate;
        
        TBool iDocEmbedded;
        CDocumentHandler *iDocHandler;
    };

#endif // FILELISTENGINE_H
