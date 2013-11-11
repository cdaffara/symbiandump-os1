// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mp4file.h"
#include "mp4memwrap.h"
#include "mp4atom.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/reent.h>
#include <f32file.h>
#include <f32file64.h>
#include <e32des16.h>
#include <caf/caf.h>

using namespace ContentAccess;

// Debug print macro
#ifdef _DEBUG
#include <e32svr.h>
#define PRINT(x)     //comment this line and uncomment the line below to enable logging for this file
//#define PRINT(x) RDebug::Print x
#else
#define PRINT(x)
#endif

_LIT(KTmpDirectoryName, "\\System\\Temp\\3GPLibTmpDir\\"); // Temporary output directory name


/*
 * Function:
 *
 *   mp4_i32 saveFileName(MP4FileName filename,
 *                        MP4HandleImp handle)
 *
 * Description:
 *
 *   Save file name for later use.
 *
 * Parameters:
 *
 *   filename   Filename
 *   handle     MP4 library handle
 *
 * Return value:
 *
 *   0          Success
 *   -1         Error
 *
 */
mp4_i32 saveFileName(MP4FileName filename, MP4HandleImp handle)
{
  handle->fileName = (MP4FileName)mp4malloc(2 * wcslen(filename) + 2);
  if (handle->fileName == NULL)
    return -1;

  wcscat(handle->fileName, filename);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 initFileRead(MP4FileName filename,
 *                        MP4HandleImp handle)
 *
 * Description:
 *
 *   Open a file for reading.
 *
 *   Note: filename is a Unicode string in Symbian OS.
 *
 * Parameters:
 *
 *   filename   Filename
 *   handle     MP4 library handle
 *
 * Return value:
 *
 *   0          Success
 *   -1         Error
 *
 */
mp4_i32 initFileRead(MP4FileName filename, MP4HandleImp handle)
{
  RFs   *fs;
  RFile64 *file;


  fs = new(RFs);
  handle->fs = (void *)fs;
  if (fs == NULL)
    return -1;

  if (fs->Connect() != KErrNone)
    return -1;

  file = new(RFile64);
  handle->rfile = (void *)file;
  if (file == NULL)
    return -1;

  if (file->Open(*fs, TPtrC((const TUint16 *)filename), EFileRead | EFileShareAny) != KErrNone)
  {
    // for compatibility, if opening in Any mode fails try more restrictive approach.
    if (file->Open(*fs, TPtrC((const TUint16 *)filename), EFileRead | EFileShareReadersOnly) != KErrNone)
    {
        return -1;
    }
  }
  handle->file = handle->rfile;
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 initFileWrite(MP4FileName filename,
 *                         MP4HandleImp handle)
 *
 * Description:
 *
 *   Open a file for writing.
 *
 *   Note: filename is a Unicode string in Symbian OS.
 *
 * Parameters:
 *
 *   filename   Filename
 *   handle     MP4 library handle
 *
 * Return value:
 *
 *   0          Success
 *   -1         Error
 *
 */
mp4_i32 initFileWrite(MP4FileName filename, MP4HandleImp handle)
{
  RFs   *fs;
  RFile64 *file;
  TParse fp;
  TFileName path;
  TInt error;


  fs = new(RFs);
  handle->fs = (void *)fs;
  if (fs == NULL)
    return -1;

  if (fs->Connect() != KErrNone)
    return -1;

  file = new(RFile64);
  handle->rfile = (void *)file;
  if (file == NULL)
    return -1;

  if (((RFs *)(handle->fs))->Parse(TPtrC((const TUint16 *)filename), fp) != KErrNone)
    return -1;

  path.Insert(0, fp.DriveAndPath() );
  error = ((RFs *)(handle->fs))->MkDirAll(path);
  if (error != KErrNone && error != KErrAlreadyExists)
    return -1;

  if (file->Replace(*fs, TPtrC((const TUint16 *)filename), EFileWrite|EFileShareExclusive|EFileWriteDirectIO ) != KErrNone)
  {
    return -1;
  }

  TBuf8<16> buf;
  buf.Copy(fp.Drive());
  buf.LowerCase();
  TInt drvNum = (*buf.Ptr()) - 'a';
  PRINT((_L("drvNum = %d"), drvNum));
  
  TVolumeInfo volInfo;
  error = fs->Volume(volInfo, drvNum);
  if (error != KErrNone) 
      {
      return -1;
      }
  
  PRINT((_L("volInfo.iFree = %Ld"), volInfo.iFree));
  PRINT((_L("volInfo.iSize = %Ld"), volInfo.iSize));    

  TVolumeIOParamInfo ioInfo;
  error = fs->VolumeIOParam(drvNum, ioInfo);
  if (error != KErrNone) 
      {
      return -1;
      }
  
  PRINT((_L("ioInfo.iBlockSize = %d"), ioInfo.iBlockSize));
  PRINT((_L("ioInfo.iClusterSize = %d"), ioInfo.iClusterSize));
  
  if (ioInfo.iClusterSize <= 0 || (ioInfo.iClusterSize & 0x1)) // if for some reason we got wrong value for the cluster - ignore it 
     {
     PRINT(_L("Wrong cluster size, set 0x8000"));
     ioInfo.iClusterSize = 0x8000;
     }
  
  // We want to have size of writing buffer to be a multiple of cluster size. Small buffer should be 1 cluster, large buffer should be 8 clusters.
  TInt writeBufferSizeSmall = ioInfo.iClusterSize;
  TInt writeBufferSizeLarge = ioInfo.iClusterSize * 8;
  
  // Now need to make sure that writeBufferSizeLarge is not too small (<128K) or too big (>256K) whilst keeping it a multiple of cluster size
  if (writeBufferSizeLarge < KFileWriterBufferSizeLarge/2)
      {
      writeBufferSizeLarge = KFileWriterBufferSizeLarge/2;
      }
    
  if (writeBufferSizeLarge > KFileWriterBufferSizeLarge)
	  {
	  writeBufferSizeLarge = (KFileWriterBufferSizeLarge / ioInfo.iClusterSize) * ioInfo.iClusterSize;
  	  }

  if (writeBufferSizeLarge < ioInfo.iClusterSize) 
      {
      writeBufferSizeLarge = ioInfo.iClusterSize;
      }
    
  PRINT((_L("writeBufferSizeLarge = %d"), writeBufferSizeLarge));

  TInt incSetSize = writeBufferSizeLarge * (KFileWriterHardBufLimit >> 1); // 2Mb if cluster size if 32767
  TInt initSetSize = incSetSize * 1; // set initial set size for 2Mb
  
  if (initSetSize > volInfo.iFree) 
      {
      initSetSize = (volInfo.iFree / incSetSize) * incSetSize;
      }

  PRINT((_L("initSetSize = %d"), initSetSize));
    
  PRINT((_L("e_SetSize 1")));  
  file->SetSize(initSetSize);
  PRINT((_L("e_SetSize 0")));  

  handle->file = handle->rfile;

  TRAP(error, handle->filewriter = CFileWriter::NewL( *file, initSetSize, writeBufferSizeSmall, writeBufferSizeLarge));
  if ( error != KErrNone )
  {
    return -1;    
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 initTmpFileWrite(MP4FileName filename,
 *                            MP4HandleImp handle)
 *
 * Description:
 *
 *   Open a temporary file for writing.
 *
 *   Note: filename is a Unicode string in Symbian OS.
 *
 * Parameters:
 *
 *   filename   Filename
 *   handle     MP4 library handle
 *
 * Return value:
 *
 *   0          Success
 *   -1         Error
 *
 */
mp4_i32 initTmpFileWrite(MP4FileName filename, MP4HandleImp handle)
{
  TInt err;
  RFile64 * file = new RFile64;
  TBuf16<KMaxFileName> name(reinterpret_cast<const TUint16*>(filename));
  ASSERT(handle->fs != NULL);
  err = file->Replace(*(RFs*)(handle->fs), name, EFileStream | EFileRead | EFileWrite | EFileWriteDirectIO);
  if (err != KErrNone)
  {
	delete file;
	return -1;
  }

  handle->tmpfile = (void *)file;
  return 0;
}
 

/*
 * Function:
 *
 *   mp4_i32 closeFile(MP4HandleImp handle)
 *
 * Description:
 *
 *   Close a file.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *
 * Return value:
 *
 *   0        Success
 *
 */
mp4_i32 closeFile(MP4HandleImp handle)
{
  PRINT((_L("e_closefile 1")));
  if (handle->rfile)
  {
    if (handle->filewriter)
    {
      PRINT((_L("e_closefile_flush_filewriter 1")));        
      (handle->filewriter)->Flush(KNullDesC8);
      PRINT((_L("e_closefile_flush_filewriter 0")));        
      PRINT((_L("e_SetSize 1")));  
      ((RFile64 *)(handle->file))->SetSize((handle->filewriter)->OutputFileSize());
      PRINT((_L("e_SetSize 0: iOutputFileSize = %Ld"), (handle->filewriter)->OutputFileSize()));  
	  
	  delete handle->filewriter;
      handle->filewriter = NULL;
    }
  }

  if (handle->asyncReader)
  {
	delete handle->asyncReader;
	handle->asyncReader = NULL;
  }

  if (handle->rfile)
  {
    if ( !handle->FileHandleFromOutside )
        {
  		PRINT((_L("e_closefile_close_file 1")));        
        ((RFile64 *)(handle->rfile))->Close();
  		PRINT((_L("e_closefile_close_file 0"))); 
		PRINT((_L("e_closefile_delete_fileptr 1")));  		
        delete(handle->rfile);
		PRINT((_L("e_closefile_delete_fileptr 0")));        
        }
    handle->rfile = NULL;
  }

  if (handle->cfile)
  {
    if ( !handle->FileHandleFromOutside )
        {
        delete(handle->cfile);
        }
    handle->cfile = NULL;
  }

  PRINT((_L("e_closefile_close_rfs 1")));
  if (handle->fs)
  {
    ((RFs *)(handle->fs))->Close();

    delete(handle->fs);

    handle->fs = NULL;
  }
  PRINT((_L("e_closefile_close_rfs 0")));  

  handle->file = NULL;
  PRINT((_L("e_closefile 0")));
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 closeTmpFile(MP4HandleImp handle)
 *
 * Description:
 *
 *   Close a temporary file.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *
 * Return value:
 *
 *   0        Success
 *
 */
mp4_i32 closeTmpFile(MP4HandleImp handle)
{
  if (handle->tmpfile)
	((RFile64*)handle->tmpfile)->Close();

  handle->tmpfile = NULL;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 deleteTmpFile(MP4HandleImp handle)
 *
 * Description:
 *
 *   Remove a temporary file.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 deleteTmpFile(MP4HandleImp handle)
{
  if (handle->tmpFileName)
	{
	  TInt err;
	  TBuf<KMaxFileName> name(reinterpret_cast<const TUint16*>(handle->tmpFileName));
	  ASSERT(handle->fs != NULL);
	  err = ((RFs*)handle->fs)->Delete(name);
	  if (err != KErrNone)
		return -1;
	}

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 readFile(MP4HandleImp handle,
 *                    mp4_u8 *buffer,
 *                    mp4_u32 bytestoread)
 *
 * Description:
 *
 *   Read data from a file.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   buffer       Buffer to read data into
 *   bytestoread  Number of bytes to read from file
 *
 * Return value:
 *
 *   0            Success
 *   -1           File has not been opened
 *   -2           Requested number of bytes could not be read
 *
 */
mp4_i32 readFile(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread)
{
  return bufferedRead(handle, buffer, bytestoread);
}

/*
 * Function:
 *
 *   mp4_i32 readTmpFile(MP4HandleImp handle,
 *                       mp4_u8 *buffer,
 *                       mp4_u32 bytestoread)
 *
 * Description:
 *
 *   Read data from a temporary file.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   buffer       Buffer to read data into
 *   bytestoread  Number of bytes to read from file
 *
 * Return value:
 *
 *   0            Success
 *   -1           File has not been opened
 *   -2           Requested number of bytes could not be read
 *
 */
mp4_i32 readTmpFile(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread)
{
  if (!handle->tmpfile)
    return -1;

  TInt err;
  TPtr8 ptrBuffer(buffer, bytestoread);
  err = ((RFile64*)handle->tmpfile)->Read(ptrBuffer, bytestoread);
  if (err != KErrNone)
	return -2;

  if (ptrBuffer.Length() != bytestoread)
	return -2;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 peekFile(MP4HandleImp handle,
 *                    mp4_u8 *buffer,
 *                    mp4_u32 bytestoread)
 *
 * Description:
 *
 *   Read data from a file but don't move the current position in the file
 *   forward.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   buffer       Buffer to read data into
 *   bytestoread  Number of bytes to read from file
 *
 * Return value:
 *
 *   0            Success
 *   -1           File has not been opened
 *   -2           Requested number of bytes could not be read
 *   -3           Current position in the file could not be set to original
 *                value
 *
 */
mp4_i32 peekFile(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread)
{
  mp4_i32 ret;
  mp4_i32 amount;


  ret = bufferedRead(handle, buffer, bytestoread);
  if (ret != 0)
    return ret;

  amount = -(mp4_i32)bytestoread;

  ret = bufferedSeek(handle, amount);
  if (ret != 0)
    return ret;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 seekFile(MP4HandleImp handle,
 *                    mp4_i32 amount)
 *
 * Description:
 *
 *   Seek in a file.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   amount   Amount to seek from current position
 *
 * Return value:
 *
 *   0        Success
 *   -1       File has not been opened
 *   -3       Current position in the file could not be set
 *
 */
mp4_i32 seekFile(MP4HandleImp handle, mp4_i64 amount)
{
  return bufferedSeek(handle, amount);
}


/*
 * Function:
 *
 *   mp4_i32 seekFileAbs(MP4HandleImp handle,
 *                       mp4_i32 amount)
 *
 * Description:
 *
 *   Seek from the beginning of a file.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   amount   Amount to seek from the beginning of the file
 *
 * Return value:
 *
 *   0        Success
 *   -1       File has not been opened
 *   -3       Current position in the file could not be set
 *
 */
mp4_i32 seekFileAbs(MP4HandleImp handle, mp4_i64 amount)
{
  return bufferedSeekAbs(handle, amount);
}


/*
 * Function:
 *
 *   mp4_i32 seekFileWrite(MP4HandleImp handle,
 *                         mp4_i64 amount)
 *
 * Description:
 *
 *   Seek in a file that has been opened for writing.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   amount   Amount to seek from current position
 *
 * Return value:
 *
 *   0        Success
 *   -1       File has not been opened
 *   -2       Can't write buffers to file
 *   -3       Current position in the file could not be set
 *
 */
mp4_i32 seekFileWrite(MP4HandleImp handle, mp4_i64 amount)
{

  if (!handle->rfile)
    return -1;

  if ( handle->filewriter )
  {
    PRINT((_L("e_seekfilewrite_flush_filewriter 1")));        
    if ( (handle->filewriter)->Flush(KNullDesC8) != KErrNone )
    {
      PRINT((_L("e_seekfilewrite_flush_filewriter 0")));        
      return -2;
    }
    PRINT((_L("e_seekfilewrite_flush_filewriter 0")));        
  }
  else
  {
    return -1;
  }

  PRINT((_L("e_seekfilewrite_seek_rfile 1")));        
  if (((RFile64 *)(handle->rfile))->Seek(ESeekCurrent,amount) != KErrNone)
  {
    return -3;
  }
  
  PRINT((_L("e_seekfilewrite_seek_rfile 0")));        

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 seekFileAbsWrite(MP4HandleImp handle,
 *                            mp4_i32 amount)
 *
 * Description:
 *
 *   Seek from the beginning of a file that has been opened for writing.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   amount   Amount to seek from the beginning of the file
 *
 * Return value:
 *
 *   0        Success
 *   -1       File has not been opened
 *   -2       Can't write buffers to file
 *   -3       Current position in the file could not be set
 *
 */
mp4_i32 seekFileAbsWrite(MP4HandleImp handle, mp4_i64 amount)
{
  if (!handle->rfile)
    return -1;

  if ( handle->filewriter )
  {
    PRINT((_L("e_seekfileabswrite_flush 1")));        
    if ( (handle->filewriter)->Flush(KNullDesC8) != KErrNone )
    {
      PRINT((_L("e_seekfileabswrite_flush 0")));        
      return -2;
    }
    PRINT((_L("e_seekfileabswrite_flush 0")));        
  }
  else
  {
    return -1;
  }

  PRINT((_L("e_seekfileabswrite_seek 1")));  
  if (((RFile64 *)(handle->rfile))->Seek(ESeekStart, amount) != KErrNone)
  {
    return -3;
  }
  PRINT((_L("e_seekfileabswrite_seek 0")));        

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 seekTmpFileAbs(MP4HandleImp handle,
 *                          mp4_i32 amount)
 *
 * Description:
 *
 *   Seek from the beginning of a temporary file.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   amount   Amount to seek from the beginning of the file
 *
 * Return value:
 *
 *   0        Success
 *   -1       File has not been opened
 *   -3       Current position in the file could not be set
 *
 */
mp4_i32 seekTmpFileAbs(MP4HandleImp handle, mp4_i64 amount)
{
  if (!handle->tmpfile)
    return -1;

  TInt err;
  TInt64 amount64 = amount;
  err = ((RFile64*)handle->tmpfile)->Seek(ESeekStart, amount64);
  if (err != KErrNone)
	return -3;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeFile(MP4HandleImp handle,
 *                     mp4_u8 *buffer,
 *                     mp4_u32 bytestowrite)
 *
 * Description:
 *
 *   Write data into a file.
 *
 * Parameters:
 *
 *   handle        MP4 library handle
 *   buffer        Buffer containing the data
 *   bytestowrite  Number of bytes to write
 *
 * Return value:
 *
 *   0             Success
 *   -1            File has not been opened
 *   -2            Number of bytes written is not equal to bytestowrite
 *
 */
mp4_i32 writeFile(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite)
{
  return bufferedWrite(handle, buffer, bytestowrite);
}


/*
 * Function:
 *
 *   mp4_i32 writeFileUnbuffered(MP4HandleImp handle,
 *                               mp4_u8 *buffer,
 *                               mp4_u32 bytestowrite)
 *
 * Description:
 *
 *   Write data into a file without buffering.
 *
 * Parameters:
 *
 *   handle        MP4 library handle
 *   buffer        Buffer containing the data
 *   bytestowrite  Number of bytes to write
 *
 * Return value:
 *
 *   0             Success
 *   -1            File has not been opened
 *   -2            Number of bytes written is not equal to bytestowrite
 *
 */
mp4_i32 writeFileUnbuffered(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite)
{
  if(handle->bufferWrite)
  {
	mp4memcpy(handle->composeBuffer+FTYP_SIZE + handle->ftypdelta,buffer,bytestowrite);
	return 0;
  }
  if (!handle->rfile)
    return -1;

  if ( handle->filewriter )
  {
    TPtrC8 buf = TPtrC8((TUint8 *)(buffer), bytestowrite);
    PRINT((_L("e_writefileunbuffered_flush 1")));        
    if ( (handle->filewriter)->Flush( buf ) != KErrNone )
    {
      PRINT((_L("e_writefileunbuffered_flush 0")));        
      return -2;
    }
    PRINT((_L("e_writefileunbuffered_flush 0")));        
  }
  else
  {
    return -1;
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeTmpFile(MP4HandleImp handle,
 *                        mp4_u8 *buffer,
 *                        mp4_u32 bytestowrite)
 *
 * Description:
 *
 *   Write data into a temporary file.
 *
 * Parameters:
 *
 *   handle        MP4 library handle
 *   buffer        Buffer containing the data
 *   bytestowrite  Number of bytes to write
 *
 * Return value:
 *
 *   0             Success
 *   -1            File has not been opened
 *   -2            Number of bytes written is not equal to bytestowrite
 *
 */
mp4_i32 writeTmpFile(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite)
{
  if (!handle->tmpfile)
    return -1;

  TInt err;
  TPtrC8 ptrBuffer(buffer, bytestowrite);
  err = ((RFile64*)handle->tmpfile)->Write(ptrBuffer, bytestowrite);
  if (err != KErrNone)
	return -2;

  if (ptrBuffer.Length() != bytestowrite)
	return -2;

  handle->bytesInTmpFile += bytestowrite;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 createTmpFileName(MP4FileName filename,
 *                             MP4FileName *tmpfilename)
 *
 * Description:
 *
 *   Create a temporary file name by adding .tmp to the end of a file name.
 *
 * Parameters:
 *
 *   filename     Original file name
 *   tmpfilename  Temporary file name is returned here
 *
 * Return value:
 *
 *   0            Success
 *   -1           Memory could not be allocated for the new name
 *
 */
mp4_i32 createTmpFileName(MP4FileName filename, MP4FileName *tmpfilename)
{
  *tmpfilename = (MP4FileName)mp4malloc(2 * wcslen(filename) + 10);
  if (*tmpfilename == NULL)
    return -1;

  wcscat(*tmpfilename, filename);
  wcscat(*tmpfilename, L".tmp");

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeTmpFileName(MP4FileName filename)
 *
 * Description:
 *
 *   Free memory allocated for the temporary file name.
 *
 * Parameters:
 *
 *   filename   Buffer containing file name
 *
 * Return value:
 *
 *   0          Success
 *
 */
mp4_i32 freeTmpFileName(MP4FileName filename)
{
  if (filename)
    mp4free(filename);

  return 0;
}


/*
 * Function:
 *
 *   void closeStdlib()
 *
 * Description:
 *
 *   Free memory allocated by stdlib wrapper functions (Symbian OS only).
 *
 * Parameters:
 *
 *   None
 *
 * Return value:
 *
 *   None
 *
 */
void closeStdlib()
	{
	// retrieves the TLS which stores the number of currently active instances of 
	// composer/parser
	TInt p = (TInt) Dll::Tls();
	
	// decrement the counter as one instance is being closed at this point
	p--;
	Dll::SetTls((TAny*) p);		

	if (p == 0)
		{
		// Since memory allocated for stdlib is shared amongst all composers and/or parsers  
		// within a thread, stdlib close should ONLY be called for the last instance of   
		// composer / parser.
		//
		// If there are no other active instances other than this,
		// the memory allocated for stdlib for this thread can now be released.
		Dll::FreeTls();
		CloseSTDLIB();
		}
	}

/*
 * Function:
 *
 *   void openStdlib()
 *
 * Description:
 *
 *   Register the use of stdlib.  
 *
 * Parameters:
 *
 *   None
 *
 * Return value:
 *
 *   0 			Success
 *
 */
mp4_i32 openStdlib()
	{
	// retrieve the the value stored in TLS for this DLL 
	TInt p = (TInt) Dll::Tls();
	
	// increment it.  This becomes a reference counter of 
	// how many instances of the composer/parser is currently active 
	p++;
	return (Dll::SetTls((TAny*) p));		
	}

/*
 * Function:
 *
 *   mp4_i32 bufferedRead(MP4HandleImp handle,
 *                        mp4_u8 *buffer,
 *                        mp4_u32 bytestoread)
 *
 * Description:
 *
 *   Read data from a file in a buffered manner.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   buffer       Buffer to read data into
 *   bytestoread  Number of bytes to read from file
 *
 * Return value:
 *
 *   0            Success
 *   -1           File has not been opened
 *   -2           Requested number of bytes could not be read
 *
 */
mp4_i32 bufferedRead(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread)
{
  mp4_u32 bytesread = 0;

  if (!handle->file)
    return -1;

  while (bytesread < bytestoread)
  {
    mp4_u32 available;


    available = handle->diskReadSize - handle->diskReadBufPos;
    if (available > bytestoread - bytesread)
      available = bytestoread - bytesread;

    if (available > 0) /* Copy data from memory buffer */
    {
      mp4memcpy(buffer + bytesread, handle->diskReadBuf + handle->diskReadBufPos, available);
      handle->diskReadBufPos += available;
      bytesread += available;
    }
    else /* Read more data from file into memory buffer */
    {
      TInt readBufferSize = 0;
      if ( handle->readBufferSize == 0)
        {
        readBufferSize = READBUFSIZE;
        }
      else
        {
        readBufferSize = handle->readBufferSize;
        }

      TPtr8 buf = TPtr8((TUint8 *)(handle->diskReadBuf), readBufferSize);

      switch (handle->sourceType)
      {
        case MP4_SOURCE_RFILE:
          if (((RFile64 *)(handle->rfile))->Read(buf, readBufferSize) != KErrNone)
            return -2;
          break;
        case MP4_SOURCE_CAF:
          handle->cafError = handle->cfile->Read(buf, readBufferSize);
          if ( handle->cafError != KErrNone)
            return -2;
          break;
        default:
          return -1;
      }

      if ((mp4_u32)buf.Length() == 0) /* EOF or error */
        return -2;

      handle->diskReadBufPos = 0;
      handle->diskReadSize = (mp4_u32)buf.Length();
      handle->diskReadBufStart = handle->diskReadPos;
      handle->diskReadPos += handle->diskReadSize;
    }
  }

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 bufferedSeek(MP4HandleImp handle,
 *                        mp4_i32 amount)
 *
 * Description:
 *
 *   Seek in a buffered file.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   amount   Amount to seek from current position
 *
 * Return value:
 *
 *   0        Success
 *   -1       File has not been opened
 *   -3       Current position in the file could not be set
 *
 */
mp4_i32 bufferedSeek(MP4HandleImp handle, mp4_i64 amount)
{
  if (!handle->file)
    return -1;

  /* Is the new seek point inside the current memory buffer? */
  if ((mp4_i32)handle->diskReadBufPos + amount >= 0 &&
      handle->diskReadBufPos + amount < handle->diskReadSize)
  {
    /* Yes */

    handle->diskReadBufPos += amount;
  }
  else
  {
    /* No */

    TInt64 am;


    am = amount - ((mp4_i32)handle->diskReadSize - (mp4_i32)handle->diskReadBufPos);

    if ( handle->rfile )
        {
        mp4_i64 maxsize = 0;
        ((RFile64 *)(handle->rfile))->Size(maxsize);
        maxsize -= (mp4_i64)handle->diskReadPos;
        
        if (am > maxsize)
            {
            return -3;
            }
        }
	// Coverity thinks that cfile can never be not NULL, which is incorrect; it can be a proper value. The comment below silences the Coverity warning
	//coverity[var_compare_op]
    else if ( handle->cfile )
        {
        mp4_i64 maxsize = 0;
        TRAPD(caferr, handle->cfile->DataSize64L(maxsize));
        maxsize -= (mp4_i64)handle->diskReadPos;
        
        if (!caferr && am > maxsize)
            {
            return -3;
            }
        }

    switch (handle->sourceType)
    {
      case MP4_SOURCE_RFILE:
		{
        PRINT((_L("e_bufferedseek_seek_rfile 1")));        
        if (((RFile64 *)(handle->rfile))->Seek(ESeekCurrent, am) != KErrNone)
		  {
		    return -3;
		  }
        PRINT((_L("e_bufferedseek_seek_rfile 0")));        
        break;
		}
      case MP4_SOURCE_CAF:
        PRINT((_L("e_bufferedseek_seek_cfile 1")));        
		// See comment on handle->cfile dereference above on why the coverity[] comment below is needed
		//coverity[var_deref_model]
      	handle->cafError = handle->cfile->Seek64(ESeekCurrent, am);
        if ( handle->cafError != KErrNone)
          return -3;
        PRINT((_L("e_bufferedseek_seek_cfile 0")));        
        break;
      default:
        return -1;
    }

    handle->diskReadPos = handle->diskReadBufStart + handle->diskReadBufPos + amount;
    handle->diskReadBufPos = 0;
    handle->diskReadBufStart = 0;
    handle->diskReadSize = 0;
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 bufferedSeekAbs(MP4HandleImp handle,
 *                           mp4_i32 amount)
 *
 * Description:
 *
 *   Seek in a buffered file from the start of the file.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   amount   Amount to seek from start of the file
 *
 * Return value:
 *
 *   0        Success
 *   -1       File has not been opened
 *   -3       Current position in the file could not be set
 *
 */
mp4_i32 bufferedSeekAbs(MP4HandleImp handle, mp4_i64 amount)
{
  if (!handle->file)
    return -1;

  /* Is the new seek point inside the current memory buffer? */
  if (handle->diskReadBufStart <= (mp4_u64)amount &&
      handle->diskReadBufStart + handle->diskReadSize > (mp4_u64)amount)
  {
    /* Yes */

    handle->diskReadBufPos = amount - handle->diskReadBufStart;
  }
  else
  {
    /* No */

    mp4_i64 am = amount;

    switch (handle->sourceType)
    {
      case MP4_SOURCE_RFILE:
		{
        PRINT((_L("e_bufferedseekabs_seek_rfile 1")));
        if (((RFile64 *)(handle->rfile))->Seek(ESeekStart, am) != KErrNone)
		  {
		    return -3;
		  }
        PRINT((_L("e_bufferedseekabs_seek_rfile 0")));        
        break;
		}
      case MP4_SOURCE_CAF:
        PRINT((_L("e_bufferedseekabs_seek_cfile 1")));
      	handle->cafError = handle->cfile->Seek64(ESeekStart, am);
        PRINT((_L("e_bufferedseekabs_seek_cfile 0")));
        if ( handle->cafError != KErrNone)
          return -3;
        break;
      default:
        return -1;
    }

    handle->diskReadPos = (mp4_u64)amount;
    handle->diskReadBufPos = 0;
    handle->diskReadBufStart = 0;
    handle->diskReadSize = 0;
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 bufferedWrite(MP4HandleImp handle,
 *                         mp4_u8 *buffer,
 *                         mp4_u32 bytestowrite)
 *
 * Description:
 *
 *   Write data into a file in a buffered manner.
 *
 * Parameters:
 *
 *   handle        MP4 library handle
 *   buffer        Buffer containing the data
 *   bytestowrite  Number of bytes to write
 *
 * Return value:
 *
 *   0             Success
 *   -1            File has not been opened
 *   -2            Number of bytes written is not equal to bytestowrite
 *
 */
mp4_i32 bufferedWrite(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite)
{
	if(handle->bufferWrite)
	{
		if(handle->bytesProgressed+bytestowrite > *(handle->composedSize))
		{
			return MP4_OUTPUT_BUFFER_TOO_SMALL; //-1;
		}
		else
		{
			mp4memcpy(handle->composeBuffer+handle->bytesProgressed,buffer,bytestowrite);
			handle->bytesProgressed+=bytestowrite;
			return 0;
		}
	}

  if (!handle->rfile)
    return -1;

  if ( handle->filewriter )
  {
    TPtrC8 buf = TPtrC8((TUint8 *)(buffer), bytestowrite);
    PRINT((_L("e_file_bufferedwrite_write 1")));
    if ( (handle->filewriter)->Write( buf ) != KErrNone )
    {
      return -2;
    }
    PRINT((_L("e_file_bufferedwrite_write 0")));
  }
  else
  {
    return -1;    
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 initMetaDataFiles(MP4HandleImp handle)
 *
 * Description:
 *
 *   Open temporary files for writing.
 *
 * Parameters:
 *
 *   handle     MP4 library handle
 *
 * Return value:
 *
 *   0          		Success
 *   -1         		General Error
 *	 MP4_OUT_OF_MEMORY	Out of memory
 *
 */
mp4_i32 initMetaDataFiles(MP4HandleImp handle)
{
  PRINT(_L("3GPLib::initMetaDataFiles() in"));
  TFileName filename;
  TFileName path;
  TInt error;

  TDriveList driveList;
  TBool pathSet = EFalse;
  
  // As ram drive access is faster, try to set temp file directory to available ram drive.
  if (((RFs *)(handle->fs))->DriveList(driveList) == KErrNone)
	{
	for ( TInt i = 0; i < driveList.Length(); i++ )
		{
		TDriveInfo driveInfo;
		if (((RFs *)(handle->fs))->Drive(driveInfo, i) == KErrNone)
			{
			if (driveInfo.iType == EMediaRam)
				{
				TChar driveLetter;
				((RFs *)(handle->fs))->DriveToChar(i, driveLetter);
				path.Append(driveLetter);
				path.Append(_L(":"));
				path.Append(KTmpDirectoryName);
				pathSet = ETrue;
				break;
				}
			}
		}
	}
	  
  // If no ram drive was found create a directory for the files on current drive
  if (!pathSet)
	{
	if ( handle->fileName )
		{
		filename = (TText *)handle->fileName;
    
		TParse fp;
		path = KTmpDirectoryName;
		if (((RFs *)(handle->fs))->Parse(filename, fp) != KErrNone)
			return -1;
		path.Insert(0, fp.Drive());
		}
	else
		{
		TChar drive;
		if (((RFs *)(handle->fs))->DriveToChar(handle->fileHandleDrive, drive ) != KErrNone )
			return -1;
		path.Append( drive );
		path.Append( _L(":") );
		path.Append( KTmpDirectoryName );
		}
	}
    
  // Try to delete the temp folder from leftovers
  // If other instance is using it then delete will just fail
  PRINT((_L("e_initmetadatafiles_newl_fileman 1")));
  CFileMan* fileMan = 0;
  TRAP( error, fileMan = CFileMan::NewL(*(RFs *)(handle->fs)));
  if ( error )
    {
        return -1;
    }
  PRINT((_L("e_initmetadatafiles_newl_fileman 0")));

  PRINT((_L("e_initmetadatafiles_deletedirectory 1")));  
  error = fileMan->RmDir( path ); // Delete directory + all files
  delete( fileMan );
  PRINT((_L("e_initmetadatafiles_deletedirectory 0")));  

  error = ((RFs *)(handle->fs))->MkDirAll(path);
  if (error != KErrNone && error != KErrAlreadyExists)
    return -1;

  ((RFs *)(handle->fs))->SetEntry(path, TTime(0), KEntryAttHidden, NULL);

  // Create files
  TFileName metadatafilename;

  for (TUint i = 0; i < NUM_MDF; i++)
  {
    handle->metaDataFile[i] = (void *)new(RFile64);
    if (handle->metaDataFile[i] == NULL)
      return MP4_OUT_OF_MEMORY;

    if (((RFile64 *)(handle->metaDataFile[i]))->Temp(*((RFs *)(handle->fs)), path, metadatafilename, EFileWrite | EFileShareExclusive) != KErrNone)
      return -1;

    handle->metaDataFileEmpty[i] = EFalse;
    
    // save file name, used later for deleting
    handle->metaDataFileName[i] = (MP4FileName)mp4malloc(2 * wcslen((MP4FileName)metadatafilename.Ptr()) + 2);
    if (handle->metaDataFileName[i] == NULL)
      return -1;
    
    wcscat(handle->metaDataFileName[i], (MP4FileName)metadatafilename.Ptr());
    handle->metaDataFileName[i][metadatafilename.Length()] = 0;
    PRINT((_L("3GPLib::initMetaDataFiles() handle->metaDataFileName[%d]=%s, length=%d "),i,  handle->metaDataFileName[i], metadatafilename.Length()));
    
  }

  PRINT(_L("3GPLib::initMetaDataFiles() creating MetaDataWriter"));
  TRAPD(err,   handle->metadatafilewriter = CMetaDataFileWriter::NewL());
  if ( err != KErrNone )
  	{
    PRINT((_L("3GPLib::initMetaDataFiles() MetaDataWriter creation error: %d"), err));
    if ( err == KErrNoMemory )
    	{
    	return MP4_OUT_OF_MEMORY;    
    	}
    else
    	{
    	return -1;    
	    }
  	}

  PRINT(_L("3GPLib::initMetaDataFiles() MetaDataWriter created"));
  PRINT(_L("3GPLib::initMetaDataFiles() out"));
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeMetaDataFileNum(MP4HandleImp handle,
 *                                mp4_u8 *buffer,
 *                                mp4_u32 bytestowrite,
 *                                mp4_u32 filenumber)
 *
 * Description:
 *
 *   Write data into a numbered file.
 *
 * Parameters:
 *
 *   handle        MP4 library handle
 *   buffer        Buffer containing the data
 *   bytestowrite  Number of bytes to write
 *   filenumber    Index of temporary file
 *
 * Return value:
 *
 *   0             Success
 *   -1            File has not been opened
 *   -2            Number of bytes written is not equal to bytestowrite
 *
 */
mp4_i32 writeMetaDataFileNum(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite, mp4_u32 filenumber)
{
  PRINT(_L("3GPLib::writeMetaDataFileNum() in"));

  if (!handle->metaDataFile[filenumber])
    return -1;

  if ( handle->metadatafilewriter )
  {
    TPtrC8 buf = TPtrC8((TUint8 *)(buffer), bytestowrite);
    PRINT((_L("e_file_writeMetaDataFileNum_write 1")));
    if ( (handle->metadatafilewriter)->Write( *((RFile64 *)(handle->metaDataFile[filenumber])),
                                              filenumber,
                                              buf ) != KErrNone )
    {
      return -2;
    }
    PRINT((_L("e_file_writeMetaDataFileNum_write 0")));
  }
  else
  {
    return -1;
  } 

  PRINT(_L("3GPLib::writeMetaDataFileNum() out"));
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 seekMetaDataFileNumAbs(MP4HandleImp handle,
 *                                  mp4_i32 amount,
 *                                  mp4_u32 filenumber)
 *
 * Description:
 *
 *   Seek from the beginning of a numbered file.
 *
 * Parameters:
 *
 *   handle        MP4 library handle
 *   amount        Amount to seek from the beginning of the file
 *   filenumber    Index of temporary file
 *
 * Return value:
 *
 *   0        Success
 *   -1       File has not been opened
 *   -3       Current position in the file could not be set
 *
 */
mp4_i32 seekMetaDataFileNumAbs(MP4HandleImp handle, mp4_i64 amount, mp4_u32 filenumber)
{
  PRINT(_L("3GPLib::seekMetaDataFileNumAbs() in"));

  if (!handle->metaDataFile[filenumber])
    return -1;


  if ( handle->metadatafilewriter )
  {
    PRINT((_L("e_seekmetadatafilenumabs_flush 1")));        
    if ( (handle->metadatafilewriter)->Flush() != KErrNone )
    {
      PRINT((_L("e_seekmetadatafilenumabs_flush 0")));        
      return -2;
    }
    PRINT((_L("e_seekmetadatafilenumabs_flush 0")));        
  }

  PRINT((_L("e_seekmetadatafilenumabs_seek 1")));
  if (((RFile64 *)(handle->metaDataFile[filenumber]))->Seek(ESeekStart, amount) != KErrNone)
  {
    return -3;
  }
  PRINT((_L("e_seekmetadatafilenumabs_seek 0")));        

  PRINT(_L("3GPLib::seekMetaDataFileNumAbs() out"));
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 readMetaDataFileNum(MP4HandleImp handle,
 *                               mp4_u8 *buffer,
 *                               mp4_u32 bytestoread,
 *                               mp4_u32 filenumber)
 *
 * Description:
 *
 *   Read data from a numbered file.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   buffer       Buffer to read data into
 *   bytestoread  Number of bytes to read from file
 *   filenumber   Index of temporary file
 *
 * Return value:
 *
 *   0            Success
 *   -1           File has not been opened
 *   -2           Requested number of bytes could not be read
 *
 */
mp4_i32 readMetaDataFileNum(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread, mp4_u32 filenumber)
{
  PRINT(_L("3GPLib::readMetaDataFileNum() in"));
  PRINT((_L("e_readmetadatafiles 1")));

  mp4_u32 bytesread = 0;
  TPtr8 buf = TPtr8((TUint8 *)buffer, bytestoread);

  if (!handle->metaDataFile[filenumber])
    return -1;

  if ( handle->metadatafilewriter )
  {
    PRINT((_L("e_readmetadatafiles_flush 1")));
    if ( (handle->metadatafilewriter)->Flush() != KErrNone )
    {
      return -2;
    }
    PRINT((_L("e_readmetadatafiles_flush 0")));
  }

  PRINT((_L("e_readmetadatafiles_read 1")));
  
  if (handle->metaDataFileEmpty[filenumber])
    {
    if (handle->metadatafilewriter)
    	{	
    	// if the file is empty, then read the data from temp buffer
    	bytesread = (handle->metadatafilewriter)->ReadBuffer( filenumber, buf, bytestoread);    	
    	}
    if ( bytesread != bytestoread )
        return -2;
    }
  else if (((RFile64 *)(handle->metaDataFile[filenumber]))->Read(buf, bytestoread) != KErrNone)
    {
    handle->metaDataFileEmpty[filenumber] = ETrue;
    // if the file is empty, then read the data from temp buffer
    if (handle->metadatafilewriter)
    	{	
    	bytesread = (handle->metadatafilewriter)->ReadBuffer( filenumber, buf, bytestoread);    	
    	}
    if ( bytesread != bytestoread )
        return -2;
    }
  else if ( buf.Length() < bytestoread ) // Got only part of data from file, rest is in buffer
    {
    handle->metaDataFileEmpty[filenumber] = ETrue;
    // if the file is empty, then read the data from temp buffer
    if (handle->metadatafilewriter)
    	{
    	bytesread = (handle->metadatafilewriter)->ReadBuffer( filenumber, buf, bytestoread - buf.Length());    	
    	}
    if ( buf.Length() != bytestoread )
        return -2;
    }
    
  PRINT((_L("e_readmetadatafiles_read 0")));

  if ((mp4_u32)buf.Length() == 0) /* EOF or error */
    return -2;

  PRINT(_L("3GPLib::readMetaDataFileNum() out"));
  PRINT((_L("e_readmetadatafiles 0")));
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 deleteMetaDataFiles(MP4HandleImp handle)
 *
 * Description:
 *
 *   Remove a numbered temporary file.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 deleteMetaDataFiles(MP4HandleImp handle)
{
  PRINT(_L("3GPLib::deleteMetaDataFiles() in"));
  PRINT((_L("e_deletemetadatafiles 1")));

  TFileName filename;
  TFileName path;
  TInt err;

  if ( handle->metadatafilewriter )
  {
    PRINT((_L("e_deletemetadatafiles_flush 1")));
    if ( (handle->metadatafilewriter)->Flush() != KErrNone )
    {
      PRINT((_L("e_deletemetadatafiles_flush 0")));
      return -2;
    }
    PRINT((_L("e_deletemetadatafiles_flush 0")));
    delete handle->metadatafilewriter;
    handle->metadatafilewriter = NULL;
  }
    
  /* Determine file names */
  PRINT((_L("e_deletemetadatafiles_determinenames 1")));
  if ( handle->fileName )
    {  
     filename = (TText *)handle->fileName;
     path = KTmpDirectoryName;

    TParse fp;
    if (((RFs *)(handle->fs))->Parse(filename, fp) != KErrNone)
        return -1;
    path.Insert(0, fp.Drive());
    }
  else
    {
    TChar drive;
    if (((RFs *)(handle->fs))->DriveToChar(handle->fileHandleDrive, drive ) != KErrNone )
        return -1;
    path.Append( drive );
    path.Append( _L(":") );
    path.Append( KTmpDirectoryName );    
    }
  PRINT((_L("e_deletemetadatafiles_determinenames 0")));    
    
  PRINT((_L("e_deletemetadatafiles_deletemetadatafilenames 1")));    
  for (TUint i = 0; i < NUM_MDF; i++)
  {
    if (handle->metaDataFileName[i])
      {
      // check if client has defined async delete observer
      if ( handle->tempFileRemoverObserver )
        {
        PRINT((_L("3GPLib::deleteMetaDataFiles() nro=%d sending %x to observer: %s"), i,handle->metaDataFileName[i], handle->metaDataFileName[i]));
        handle->tempFileRemoverObserver->M3GPMP4LibDeleteTempFileName( handle->metaDataFileName[i] );
        }
      else // no observer, delete right here, ignore errors
        {
        PRINT((_L("3GPLib::deleteMetaDataFiles() nro=%d wremove: %s"), i, handle->metaDataFileName[i]));
        err = wremove(handle->metaDataFileName[i]);
        PRINT((_L("3GPLib::deleteMetaDataFiles() wremove err=%d"), err));
        err++; // to remove compile warnings
        mp4free(handle->metaDataFileName[i]);
        }
      handle->metaDataFileName[i] = 0;
      }
  PRINT((_L("e_deletemetadatafiles_deletemetadatafilenames 0")));    

  }

  PRINT(_L("3GPLib::deleteMetaDataFiles() out"));
  PRINT((_L("e_deletemetadatafiles 0")));
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 closeMetaDataFiles(MP4HandleImp handle)
 *
 * Description:
 *
 *   Close numbered tmp files.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *
 * Return value:
 *
 *   0        Success
 *
 */
mp4_i32 closeMetaDataFiles(MP4HandleImp handle)
{
  PRINT(_L("3GPLib::closeMetaDataFiles() in"));
  PRINT((_L("e_close_metadatafiles 1")));

  if ( handle->metadatafilewriter )
  {
    PRINT((_L("e_close_metadatafiles_flush 1")));
    if ( (handle->metadatafilewriter)->Flush() != KErrNone )
    {
      PRINT((_L("e_close_metadatafiles_flush 0")));
      return -2;
    }
    PRINT((_L("e_close_metadatafiles_flush 0")));
  }

  for (TUint i = 0; i < NUM_MDF; i++)
  {
    if (handle->metaDataFile[i])
    {
	  PRINT((_L("e_close_metadatafiles_closefile 1")));    
      ((RFile64 *)(handle->metaDataFile[i]))->Close();
      PRINT((_L("e_close_metadatafiles_closefile 0")));

      PRINT((_L("e_close_metadatafiles_delfile 1")));
      delete((RFile64 *)(handle->metaDataFile[i]));
      PRINT((_L("e_close_metadatafiles_delfile 0")));
      handle->metaDataFile[i] = NULL;
    }
  }

  PRINT(_L("3GPLib::closeMetaDataFiles() out"));
  PRINT((_L("e_close_metadatafiles 0")));
  return 0;
}


TInt RecommendedBufferSize(MP4HandleImp aHandle)
	{
	TInt recommendedSize = READBUFSIZE;
	
    MP4HandleImp handle = (MP4HandleImp)aHandle;
    if (handle)
    	{
		// handle->rfile will be set in the cases of
		// - MP4ParseOpen(MP4FileName) <if the filename is set>
		// - MP4ParseOpenFileHandle64(RFile64)
		// - MP4ParseOpenFileHandle(RFile) <via MP4ParseOpenFileHandle64()>
		//
		// It will not be set by MP4ParseOpenCAF()

		RFs* fs = (RFs*)handle->fs;
		RFile64* file64 = (RFile64*)handle->rfile;
		
		if (fs && file64)
			{
			TInt driveNumber = 0;
			TDriveInfo driveInfo;
			TVolumeIOParamInfo volumeInfo;
			
			TInt err = file64->Drive(driveNumber, driveInfo);
			if (err == KErrNone)
				{
				err = fs->VolumeIOParam(driveNumber, volumeInfo);
				}
			
			if (err == KErrNone)
				{
				if (volumeInfo.iRecReadBufSize != KErrNotSupported)
					{
					recommendedSize = Max(recommendedSize, volumeInfo.iRecReadBufSize);
					}
				}
				
			}
    	}

    return recommendedSize;
	}
	

// End of File
