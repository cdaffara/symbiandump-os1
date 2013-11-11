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

package com.symbian.commdb.convert;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import com.symbian.commdb.data.Database;
import com.symbian.commdb.data.DatabaseException;
import com.symbian.commdb.data.Version;

/**
 * This is a utility which can be used in order to convert one
 * or more CommDB settings files of the pre-XML format (*.cfg) to
 * the XML format.
 */
public class cfg2xml
{
    /**
     * Utility which can be used in order to convert one or
     * more CommDB settings files of the pre-XML format (*.cfg)
     * to the XML format.
     *
     * @param args the command line arguments
     */
    public static void main(String[] args)
    {
        PrintTitle();
        
        // If no or invalid arguments are supplied then display
        // the command line options
        
        if(args.length < 3)
        {
            System.out.println("ERROR: Missing arguments");
            ShowOptions();
            System.exit(TerminationCodes.ERROR_WRONG_NUMBER_OF_ARGUMENTS);
        }
        
        // The first argument has to specify the mode of operation
        
        if(args[0].equals("-mode:file"))
        {
            fileMode = true;
            System.out.println("- Operating in file mode");
        }
        else if(args[0].equals("-mode:dir"))
        {
            fileMode = false;
            System.out.println("- Operating in directory mode");
        }
        else
        {
            System.out.println("ERROR: Missing or wrong -mode option specified");
            System.exit(TerminationCodes.ERROR_WRONG_ARGUMENT);
        }
        
        // If in file mode, the input file must have the .cfg extension and
        // if an output file is not specified the name of the input file is
        // used with the .xml extension
        
        String fileNameIn  = args[2];
        String fileNameOut = null;
        
        if(fileMode)
        {
            int extIndex = fileNameIn.toUpperCase().indexOf(".CFG");
            if(extIndex != -1)
            {
                if(args.length == 3)
                {
                    fileNameOut = fileNameIn.substring(0,extIndex) + ".xml";
                    System.out.println("- The output XML file will be named: " +
                    fileNameOut);
                }
                else
                {
                    fileNameOut = args[3];
                    extIndex = fileNameOut.toUpperCase().indexOf(".XML");
                    
                    if(extIndex == -1)
                    {
                        System.out.println("ERROR: Output file is not a .xml file");
                        System.exit(TerminationCodes.ERROR_WRONG_ARGUMENT);
                    }
                }
            }
            else
            {
                System.out.println("ERROR: Input file is not a .cfg file");
                System.exit(TerminationCodes.ERROR_WRONG_ARGUMENT);
            }
        }
        
        // Create a CommDB database object
        
        commdb = new Database(args[1]);
        
        // Read the structure of the CommDB database to employ for
        // the conversion process
        
        try
        {
            System.out.println("- Reading CommDB contents");
            commdb.ReadContents();
        }
        catch(DatabaseException e)
        {
            // Notify the user that the database creation failed
            
            System.out.println("ERROR: CommDB Database Creation Error");
            System.out.println("MESSAGE: " + e.GetErrorMessage());
            System.exit(TerminationCodes.ERROR_DATABASE_CREATION);
        }
        
        // Import the parameters of the CommDB tables from the XML Schema files
        
        try
        {
            System.out.println("- Reading CommDB XML Schema files");
            javax.swing.JProgressBar progressBar = new javax.swing.JProgressBar();
            commdb.BuildTableTemplates(progressBar);
        }
        catch(DatabaseException e)
        {
            // Notify the user that the import procedure failed
            
            System.out.println("ERROR: Import CommDB Database Error");
            System.out.println("MESSAGE: " + e.GetErrorMessage());
            System.exit(TerminationCodes.ERROR_DATABASE_IMPORT);
        }
        
        // Perform the conversion
        
        if(fileMode)
        {
            // In file mode, simply convert the specified file
            
            try
            {
                System.out.println("- Performing conversion");
                ConvertFile(fileNameIn,fileNameOut);
            }
            catch(Exception e)
            {
                System.exit(TerminationCodes.ERROR_CONVERSION);
            }
        }
        else
        {
            // In directory mode, produce a list of all the .cfg
            // files in the specified directory and convert them
            
            File dirConvert = new File(args[2]);
            
            if(dirConvert == null)
            {
                System.out.println("ERROR: Cannot open the input directory");
                System.exit(TerminationCodes.ERROR_WRONG_ARGUMENT);
            }
            if(!dirConvert.isDirectory())
            {
                System.out.println("ERROR: The specified input directory is not a directory");
                System.exit(TerminationCodes.ERROR_WRONG_ARGUMENT);
            }
            
            ConvertDirectory(dirConvert);
        }
        
        // Program terminates
        
        System.out.println("- Succesfull termination");
        System.exit(TerminationCodes.SUCCESS);
    }
    
    /**
     * This function displays the title of the conversion
     * utility
     */
    private static void PrintTitle()
    {
        System.out.println();
        System.out.println("CommDB Configuration File Conversion Utility");
        System.out.println("Version 1.2 [Release] Build 20031126");
        System.out.println("using the CommDB Editor XML Processing Engine");
        System.out.println("Version " + Version.GetVersion());
        System.out.println("Copyright (C) 2003, Symbian Ltd. All Rights Reserved.");
        System.out.println();
    }
    
    /**
     * This function displays the options of this utility
     */
    private static void ShowOptions()
    {
        System.out.println();
        System.out.println("The purpose of this utility is to convert one or more CommDB");
        System.out.println("settings files of the CFG format (*.cfg) to the XML format (*.xml)");
        System.out.println("The utility should be executed as:");
        System.out.println();
        System.out.println("java -jar cfg2xml.jar -mode:[file|dir] schema-dir in [out]");
        System.out.println();
        System.out.println("where:");
        System.out.println();
        System.out.println("-mode:[file|dir] - specifies whether the utility should process");
        System.out.println("                   a single file (-mode:file) or a directory with");
        System.out.println("                   a number of .cfg files (-mode:dir)");
        System.out.println("schema-dir       - the directory which contains the XML Schema");
        System.out.println("                   files which define the version of CommDB");
        System.out.println("                   appropriate for the settings file to convert");
        System.out.println("in               - The file in the pre-XML format (*.cfg) to convert,");
        System.out.println("                   if -mode:file is specified, else the directory");
        System.out.println("                   which contains .cfg files to convert. When operating");
        System.out.println("                   in directory mode this utility will also process all");
        System.out.println("                   subdirectories and their contents for .cfg files");
        System.out.println("out              - The XML settings file (.xml) to generate as the result");
        System.out.println("                   of the conversion process. This parameter can be");
        System.out.println("                   supplied only if the utility operates in file mode");
        System.out.println("                   (-mode:file). If not specified, the converted file");
        System.out.println("                   has the same name as the original .cfg one");
        System.out.println();
    }
    
    /**
     * This function performs the conversion of a .CFG file to an .XML file
     *
     * @param fileIn The name of the input (CFG) file to convert
     * @param fileOut The name of the output (XML) file to generate
     * @throws DatabaseException, FileNotFoundException, IOException
     */
    private static void ConvertFile(String fileIn, String fileOut)
    throws DatabaseException, FileNotFoundException, IOException
    {
        // Import the CFG file
        
        try
        {
            commdb.EraseTableEntries();
            commdb.ImportOldDatabase(fileIn);
        }
        catch(DatabaseException e)
        {
            // Notify the user that the import procedure failed
            
            System.out.println("ERROR: Import CFG File Error");
            System.out.println("MESSAGE: " + e.GetErrorMessage());
        }
        
        // Export the database in the XML format
        
        File xmlFile = new File(fileOut);
        
        try
        {
            commdb.ExportDatabase(xmlFile);
        }
        catch(FileNotFoundException e)
        {
            // Notify the user that the export procedure failed
            
            System.out.println("ERROR: Export to XML Error");
            System.out.println("MESSAGE: " + e.getMessage());
        }
        catch(IOException e)
        {
            // Notify the user that the export procedure failed
            
            System.out.println("ERROR: Export to XML Error");
            System.out.println("MESSAGE: " + e.getMessage());
        }
    }
    
    /**
     * This function parses a directory and converts all .cfg files
     * to .xml files. If subdirectories are encountered, they are
     * also examined for .cfg files and this function is called
     * recursively to handle all subdirectories.
     *
     * @param dirConvert The directory to convert all the .cfg files
     * it contains to .xml
     */
    private static void ConvertDirectory(File dirConvert)
    {
        File files[] = dirConvert.listFiles();
        
        // If no files exist the exit this function
        
        if(files.length == 0)
            return;
        
        // Conversion loop
        
        for(int i=0; i < files.length; i++)
        {
            File fileIn = files[i];
            
            // If this is a file then check that it is a .cfg file and
            // convert it
            
            if(fileIn.isFile())
            {
                String fileNameIn = null;
                
                try
                {
                    fileNameIn = fileIn.getCanonicalFile().toString();
                }
                catch(IOException e)
                {
                    continue;
                }
                
                int extIndex = fileNameIn.toUpperCase().indexOf(".CFG");
                if(extIndex == -1)
                    continue;
                
                String fileNameOut  = fileNameIn.substring(0,extIndex) + ".xml";
                
                System.out.println("- Converting file: " + fileNameIn +
                " to file: "          + fileNameOut);
                
                try
                {
                    ConvertFile(fileNameIn,fileNameOut);
                }
                catch(Exception e)
                {
                    continue;
                }
            }
            
            // If this is a directory then call this function recursively
            // in order to convert all .cfg files in this directory as well
            // as all its subdirectories
            
            else if(fileIn.isDirectory())
            {
                ConvertDirectory(fileIn);
            }
        }
    }
    
    // Data members
    
    /**
     * The CommDB database to create
     */
    private static Database commdb;
    
    /**
     * A flag which determines whether this utility operates in file
     * mode (true) or in directory mode (false)
     */
    private static boolean fileMode;
}
