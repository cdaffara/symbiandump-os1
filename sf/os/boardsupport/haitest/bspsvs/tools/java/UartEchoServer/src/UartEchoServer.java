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


import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;

import java.util.logging.Logger;

import org.apache.commons.cli.Options;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.PosixParser;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.HelpFormatter;

public class UartEchoServer implements SerialPortEventListener
	{
	//	Command line options
	private static final String	OPTION_HELP			="help";
	private static final String	OPTION_PORT			="port";
	private static final String	OPTION_BAUD			="baud_rate";
	private static final String	OPTION_DATA			="data_bits";
	private static final String	OPTION_STOP			="stop_bits";
	private static final String	OPTION_PARITY		="parity";
	private static final String	OPTION_FLOW			="flow_control";

	//	Default settings for serial port
	private static final String	DEFAULT_PORT_NAME	= "COM1";
	private static final int	DEFAULT_BAUD_RATE	= 9600;
	private static final int	DEFAULT_DATA_BITS	= SerialPort.DATABITS_8;
	private static final int	DEFAULT_STOP_BITS	= SerialPort.STOPBITS_1;
	private static final int	DEFAULT_PARITY		= SerialPort.PARITY_NONE;
	private static final int	DEFAULT_FLOW_CONTROL= SerialPort.FLOWCONTROL_NONE;

	//	Command format limiters
	private static final byte	BYTE_IGNORE			= '\0'; // Null
	private static final byte	BYTE_CLEARBI		= '\1'; // Ctrl A
	private static final byte	BYTE_QUERYBI		= '\2'; // Ctrl B
	private static final byte	BYTE_RESET			= '\3'; // Ctrl C
	private static final byte	BYTE_BLOCK_START	= '[';
	private static final byte	BYTE_BLOCK_END		= ']';
	private static final String	BLOCK_ASSIGN		= "=";
	private static final String	BLOCK_SEPERATOR		= ",";

	//	Supported commands
	private static final String	CMD_BAUD_RATE		= "baud_rate";
	private static final String	CMD_DATA_BITS		= "data_bits";
	private static final String	CMD_DELAY			= "delay";
	private static final String	CMD_DISCONNECT		= "disconnect";
	private static final String	CMD_ECHO			= "echo";
	private static final String	CMD_FLOW_CONTROL	= "flow_control";
	private static final String	CMD_LOG				= "log";
	private static final String	CMD_PARITY			= "parity";
	private static final String	CMD_STOP_BITS		= "stop_bits";

	//	Supported data bits
	private static final String	DATA_BITS_5			= "5";
	private static final String	DATA_BITS_6			= "6";
	private static final String	DATA_BITS_7			= "7";
	private static final String	DATA_BITS_8			= "8";

	//	Supported flow control
	private static final String	FLOWCONTROL_NONE	= "none";
	private static final String	FLOWCONTROL_RTSCTS	= "rtscts";
	private static final String	FLOWCONTROL_XONXOFF	= "xonxoff";

	//	Supported parity
	private static final String	PARITY_NONE			= "none";
	private static final String	PARITY_EVEN			= "even";
	private static final String	PARITY_ODD			= "odd";
	private static final String	PARITY_MARK			= "mark";
	private static final String	PARITY_SPACE		= "space";

	//	Supported stop bits
	private static final String	STOP_BITS_1			= "1";
	private static final String	STOP_BITS_1_5		= "1.5";
	private static final String	STOP_BITS_2			= "2";

	//	Constants
	private static final int	BUFFER_SIZE			= 1024;
	private static final int	SLEEP_PERIOD		= 200000;

	private enum EStatus
		{
		EStatusEcho,
		EStatusCommandStart,
		};

	private Thread				iMainThread;
	private int					iRestartingDelay;
	private boolean				iRestarting;
	private boolean				iRunning;
	private Logger				iLogger;
	private byte[]				iBuffer;
	private EStatus				iStatus;
	private String				iCommand;
	private String				iPortName;
	private byte				iBI;

	private int					iBaudRate;
	private int					iDataBits;
	private int					iStopBits;
	private int					iParity;
	private int					iFlowControl;

	private int					iStartupBaudRate;
	private int					iStartupDataBits;
	private int					iStartupStopBits;
	private int					iStartupParity;
	private int					iStartupFlowControl;

	private CommPortIdentifier	iPortIdentifier;
	private SerialPort			iSerialPort;

	/*
	 *	Constructor
	 */
	protected UartEchoServer() throws Exception
		{
		iRestartingDelay=0;
		iRestarting=true;
		iRunning=true;
		iLogger=Logger.getLogger("UartEchoServer");
		iBuffer=new byte[BUFFER_SIZE];
		iStatus = EStatus.EStatusEcho;
		iCommand = "";
		iBI=0;
		iPortName=DEFAULT_PORT_NAME;
		iBaudRate=DEFAULT_BAUD_RATE;
		iDataBits=DEFAULT_DATA_BITS;
		iStopBits=DEFAULT_STOP_BITS;
		iParity=DEFAULT_PARITY;
		iFlowControl=DEFAULT_FLOW_CONTROL;
		}

	/*
	 *	Second pahse constructor
	 *
	 *	@param	aCommandLine	Command line parameters
	 */
	protected void Construct(final CommandLine aCommandLine) throws Exception
		{
		/*
		 *	Set port name if passed in command line
		 */
		if ( aCommandLine.hasOption(OPTION_PORT) )
			{
			iPortName=aCommandLine.getOptionValue(OPTION_PORT);
			iLogger.info("PortName:" + iPortName);
			}

		/*
		 *	Set baud rate if passed in command line
		 */
		if ( aCommandLine.hasOption(OPTION_BAUD) )
			{
			setBaudRate(aCommandLine.getOptionValue(OPTION_BAUD));
			iLogger.info("Baud Rate:" + iBaudRate);
			}

		/*
		 *	Set data bits if passed in command line
		 */
		if ( aCommandLine.hasOption(OPTION_DATA) )
			{
			setDataBits(aCommandLine.getOptionValue(OPTION_DATA));
			iLogger.info("Data Bits:" + iDataBits);
			}

		/*
		 *	Set stop bits if passed in command line
		 */
		if ( aCommandLine.hasOption(OPTION_STOP) )
			{
			setStopBits(aCommandLine.getOptionValue(OPTION_STOP));
			iLogger.info("Stop Bits:" + iStopBits);
			}

		/*
		 *	Set parity if passed in command line
		 */
		if ( aCommandLine.hasOption(OPTION_PARITY) )
			{
			setParity(aCommandLine.getOptionValue(OPTION_PARITY));
			iLogger.info("Parity:" + iParity);
			}

		/*
		 *	Set flow control if passed in command line
		 */
		if ( aCommandLine.hasOption(OPTION_FLOW) )
			{
			setFlowControl(aCommandLine.getOptionValue(OPTION_FLOW));
			iLogger.info("Flow Control:" + iFlowControl);
			}

		/*
		 *	Save startup values. Used by reset command
		 */
		iStartupBaudRate=iBaudRate;
		iStartupDataBits=iDataBits;
		iStartupStopBits=iStopBits;
		iStartupParity=iParity;
		iStartupFlowControl=iFlowControl;

		/*
		 *	Make sure port is not in use
		 */
		iPortIdentifier=CommPortIdentifier.getPortIdentifier(iPortName);
		if ( iPortIdentifier.isCurrentlyOwned() )
			{
			throw new Exception("Error: Port is currently in use");
			}

		/*
		 *	Port not in use so open it
		 */
		CommPort	commPort=iPortIdentifier.open(this.getClass().getName(), 2000);

		/*
		 *	Save thread
		 */
		iMainThread=Thread.currentThread();

		/*
		 *	Make sure the port is of type serial
		 */
		if ( commPort instanceof SerialPort )
			{
			iSerialPort = (SerialPort) commPort;
			iFlowControl=iSerialPort.getFlowControlMode();
			while ( iRunning )
				{
				initPort();

				iRestarting=false;
				while ( iRunning && !iRestarting )
					{
					try
						{
						iMainThread.sleep(SLEEP_PERIOD);
						}
					catch ( InterruptedException e )
						{
						}
					}
				iSerialPort.close();
				if ( iRestarting )
					{
					iLogger.finest("Restarting");
					iMainThread.sleep(iRestartingDelay);
					commPort=iPortIdentifier.open(this.getClass().getName(), 2000);
					iSerialPort = (SerialPort) commPort;
					}
				}
			}
		else
			{
			throw new Exception("Error: Only serial ports are handled by this example.");
			}
		}

	/*
	 *	Initialise the port
	 */
	private void initPort() throws Exception
		{
		iSerialPort.setSerialPortParams(iBaudRate, iDataBits, iStopBits, iParity);
		iSerialPort.setFlowControlMode(iFlowControl);
		iSerialPort.addEventListener(this);
		iSerialPort.notifyOnBreakInterrupt(true);
		iSerialPort.notifyOnCarrierDetect(true);
		iSerialPort.notifyOnCTS(true);
		iSerialPort.notifyOnDataAvailable(true);
		iSerialPort.notifyOnDSR(true);
		iSerialPort.notifyOnFramingError(true);
		iSerialPort.notifyOnOutputEmpty(true);
		iSerialPort.notifyOnOverrunError(true);
		iSerialPort.notifyOnParityError(true);
		iSerialPort.notifyOnRingIndicator(true);
		iSerialPort.setInputBufferSize(BUFFER_SIZE);
		}

	/*
	 *	Set the baud rate
	 *
	 *	@param	aValue	String representation of the baud rate
	 */
	private void setBaudRate(final String aValue)
		{
		try
			{
			iBaudRate=Integer.parseInt(aValue);
			}
		catch (Exception e)
			{
			iLogger.severe("convertToBaudRate(" + aValue + "):exception" + e);
			}
		}

	/*
	 *	Set the data bits
	 *
	 *	@param	aValue	String representation of the data bits
	 */
	private void setDataBits(final String aValue)
		{
		try
			{
			if ( aValue.compareTo(DATA_BITS_5)==0 )
				{
				iDataBits=SerialPort.DATABITS_5;
				}
			else if ( aValue.compareTo(DATA_BITS_6)==0 )
				{
				iDataBits=SerialPort.DATABITS_6;
				}
			else if ( aValue.compareTo(DATA_BITS_7)==0 )
				{
				iDataBits=SerialPort.DATABITS_7;
				}
			else if ( aValue.compareTo(DATA_BITS_8)==0 )
				{
				iDataBits=SerialPort.DATABITS_8;
				}
			else
				{
				iLogger.severe("setDataBits(" + aValue + ")");
				}
			}
		catch (Exception e)
			{
			iLogger.severe("setDataBits(" + aValue + "):excpetion" + e);
			}
		}

	/*
	 *	Set the flow control
	 *
	 *	@param	aValue	String representation of the flow control
	 */
	private void setFlowControl(final String aValue)
		{
		try
			{
			if ( aValue.compareTo(FLOWCONTROL_NONE)==0 )
				{
				iFlowControl=SerialPort.FLOWCONTROL_NONE;
				}
			else if ( aValue.compareTo(FLOWCONTROL_RTSCTS)==0 )
				{
				iFlowControl=SerialPort.FLOWCONTROL_RTSCTS_IN | SerialPort.FLOWCONTROL_RTSCTS_OUT;
				}
			else if ( aValue.compareTo(FLOWCONTROL_XONXOFF)==0 )
				{
				iFlowControl=SerialPort.FLOWCONTROL_XONXOFF_IN | SerialPort.FLOWCONTROL_XONXOFF_OUT;
				}
			else
				{
				iLogger.severe("setFlowControl(" + aValue + ")");
				}
			}
		catch (Exception e)
			{
			iLogger.severe("setFlowControl(" + aValue + "):exception" + e);
			}
		}

	/*
	 *	Set the parity
	 *
	 *	@param	aValue	String representation of the parity
	 */
	private void setParity(final String aValue)
		{
		try
			{
			if ( aValue.compareTo(PARITY_NONE)==0 )
				{
				iParity=SerialPort.PARITY_NONE;
				}
			else if ( aValue.compareTo(PARITY_EVEN)==0 )
				{
				iParity=SerialPort.PARITY_EVEN;
				}
			else if ( aValue.compareTo(PARITY_ODD)==0 )
				{
				iParity=SerialPort.PARITY_ODD;
				}
			else if ( aValue.compareTo(PARITY_MARK)==0 )
				{
				iParity=SerialPort.PARITY_MARK;
				}
			else if ( aValue.compareTo(PARITY_SPACE)==0 )
				{
				iParity=SerialPort.PARITY_SPACE;
				}
			else
				{
				iLogger.severe("setParity(" + aValue + ")");
				}
			}
		catch (Exception e)
			{
			iLogger.severe("setParity(" + aValue + "):exception" + e);
			}
		}

	/*
	 *	Set the stop bits
	 *
	 *	@param	aValue	String representation of the stop bits
	 */
	private void setStopBits(final String aValue)
		{
		try
			{
			if ( aValue.compareTo(STOP_BITS_1)==0 )
				{
				iStopBits=SerialPort.STOPBITS_1;
				}
			else if ( aValue.compareTo(STOP_BITS_1_5)==0 )
				{
				iStopBits=SerialPort.STOPBITS_1_5;
				}
			else if ( aValue.compareTo(STOP_BITS_2)==0 )
				{
				iStopBits=SerialPort.STOPBITS_2;
				}
			else
				{
				iLogger.severe("setStopBits(" + aValue + ")");
				}
			}
		catch (Exception e)
			{
			iLogger.severe("setStopBits(" + aValue + "):exception" + e);
			}
		}

	/*
	 *	Process a command from the input
	 */
	private void processCommand() throws Exception
		{
		final String	values[] = iCommand.split(BLOCK_SEPERATOR);

		iSerialPort.getOutputStream().flush();
		for ( int i=0; i<values.length; i++ )
			{
			/*
			 *	Commands should be of the type variable=value
			 */
			final String	parts[]=values[i].split(BLOCK_ASSIGN);

			if ( parts.length==2 )
				{
				/*
				 *	Set baud rate command
				 */
				if ( parts[0].compareTo(CMD_BAUD_RATE)==0 )
					{
					setBaudRate(parts[1]);
					iSerialPort.setSerialPortParams(iBaudRate, iDataBits, iStopBits, iParity);
					}
				/*
				 *	Set data bits command
				 */
				else if ( parts[0].compareTo(CMD_DATA_BITS)==0 )
					{
					setDataBits(parts[1]);
					iSerialPort.setSerialPortParams(iBaudRate, iDataBits, iStopBits, iParity);
					}
				/*
				 *	Delay command
				 */
				else if ( parts[0].compareTo(CMD_DELAY)==0 )
					{
					final int	delay=Integer.parseInt(parts[1]);
					Thread.sleep(delay);
					}
				/*
				 *	Disconnect command
				 */
				else if ( parts[0].compareTo(CMD_DISCONNECT)==0 )
					{
					iRestartingDelay=Integer.parseInt(parts[1]);
					iRestarting=true;
					}
				/*
				 *	Echo command
				 */
				else if ( parts[0].compareTo(CMD_ECHO)==0 )
					{
					final int		length=parts[1].length();
					for ( int index=0; index<length; ++index )
						{
						final byte	out=(byte)(parts[1].charAt(index));
						iLogger.finest("<< " + out);
						iSerialPort.getOutputStream().write(out);
						}
					}
				/*
				 *	Set flow control command
				 */
				else if ( parts[0].compareTo(CMD_FLOW_CONTROL)==0 )
					{
					setFlowControl(parts[1]);
					iSerialPort.setFlowControlMode(iFlowControl);
					}
				/*
				 *	Log command
				 */
				else if ( parts[0].compareTo(CMD_LOG)==0 )
					{
					iLogger.info(parts[1]);
					}
				/*
				 *	Set parity command
				 */
				else if ( parts[0].compareTo(CMD_PARITY)==0 )
					{
					setParity(parts[1]);
					iSerialPort.setSerialPortParams(iBaudRate, iDataBits, iStopBits, iParity);
					}
				/*
				 *	Set stop bits command
				 */
				else if ( parts[0].compareTo(CMD_STOP_BITS)==0 )
					{
					setStopBits(parts[1]);
					iSerialPort.setSerialPortParams(iBaudRate, iDataBits, iStopBits, iParity);
					}
				/*
				 *	Error command
				 */
				else
					{
					iLogger.severe("Bad command(" + parts[0] + ")");
					}
				}
			else
				{
				iLogger.severe("Bad data");
				}
			}
		}

	/*
	 *	Process data read from input stream
	 *
	 *	@param	aLength	Length of the data in the buffer
	 */
	private void processInput(final int aLength) throws Exception
		{
		final String	buffer=new String(iBuffer, 0, aLength);
		iLogger.finest(">> " + buffer);
		for ( int index=0; index<aLength; ++index )
			{
			switch ( iBuffer[index] )
				{
			/*
			 *	Ignored data
			 */
			case BYTE_IGNORE:
				break;
			/*
			 *	Clear the break interrupt count
			 */
			case BYTE_CLEARBI:
				iBI=0;
				break;
			/*
			 *	Query the break interrupt count
			 */
			case BYTE_QUERYBI:
				iLogger.finest("BI Count=" + iBI);
				iLogger.finest("<< " + iBI);
				iSerialPort.getOutputStream().write(iBI);
				break;
			/*
			 *	Reset port setting to startup values
			 */
			case BYTE_RESET:
				iBaudRate=iStartupBaudRate;
				iDataBits=iStartupDataBits;
				iStopBits=iStartupStopBits;
				iParity=iStartupParity;
				iFlowControl=iStartupFlowControl;
				iSerialPort.setFlowControlMode(iFlowControl);
				iSerialPort.setSerialPortParams(iBaudRate, iDataBits, iStopBits, iParity);
				iStatus=EStatus.EStatusEcho;
				break;
			default:
				/*
				 *	If in command mode add the byte to the command buffer
				 *	unless we read the end of command block character
				 */
				if ( iStatus==EStatus.EStatusCommandStart ) 
					{
					if ( iBuffer[index]==BYTE_BLOCK_END )
						{
						processCommand();
						iStatus=EStatus.EStatusEcho;
						}
					else
						{
						iCommand += buffer.charAt(index);
						}
					}
				/*
				 *	If in echo mode, echo the character unless we read the
				 *	start of command block character
				 */
				else if ( iStatus==EStatus.EStatusEcho )
					{
					if ( iBuffer[index]==BYTE_BLOCK_START )
						{
						iCommand="";
						iStatus=EStatus.EStatusCommandStart;
						}
					else
						{
						iLogger.finest("<< " + iBuffer[index]);
						iSerialPort.getOutputStream().write(iBuffer[index]);
						}
					}
				break;
				}
			}
		}

	/*
	 *	Serial port event received
	 *
	 *	@param	aEvent	Event received
	 */
	public void serialEvent(SerialPortEvent aEvent)
		{
		switch ( aEvent.getEventType() )
			{
		case SerialPortEvent.DATA_AVAILABLE:
			/*
			 *	Process data in input buffer
			 */
			try
				{
				final int	length = iSerialPort.getInputStream().read(iBuffer);

				if ( length>0 )
					{
					processInput(length);
					}
				}
			catch (Exception e)
				{
				e.printStackTrace();
				}
			if ( iRestarting )
				{
				iMainThread.interrupt();
				}
			break;
		case SerialPortEvent.BI:
			/*
			 *	Increment break interrupt count
			 */
			++iBI;
			iLogger.finest("Break Interrupt");
			break;
		case SerialPortEvent.CD:
			/*
			 *	Ignore
			 */
			iLogger.finest("Carrier Detect");
			break;
		case SerialPortEvent.CTS:
			/*
			 *	Ignore
			 */
			iLogger.finest("Clear To Send");
			break;
		case SerialPortEvent.DSR:
			/*
			 *	Ignore
			 */
			iLogger.finest("Data Set Ready");
			break;
		case SerialPortEvent.FE:
			/*
			 *	Ignore
			 */
			iLogger.finest("Framing Error");
			break;
		case SerialPortEvent.OE:
			/*
			 *	Ignore
			 */
			iLogger.finest("Overflow Error");
			break;
		case SerialPortEvent.OUTPUT_BUFFER_EMPTY:
			/*
			 *	Ignore
			 */
			iLogger.finest("Output Buffer Empty");
			break;
		case SerialPortEvent.PE:
			/*
			 *	Ignore
			 */
			iLogger.finest("Parity Error");
			break;
		case SerialPortEvent.RI:
			/*
			 *	Ignore
			 */
			iLogger.finest("Ring Interrupt");
			break;
		default:
			iLogger.finest("Unknown event");
			break;
			}
		}

	/*
	 *	Application entry point
	 *
	 *	@param	aArgs	COmmand line arguments
	 */
	public static void main(String[] aArgs)
		{
		final Options	options = new Options();

		options.addOption(new Option(OPTION_HELP, "print this message"));

		OptionBuilder.withLongOpt(OPTION_PORT);
		OptionBuilder.withDescription("set port COMx");
		OptionBuilder.withValueSeparator( '=' );
		OptionBuilder.hasArg();
		options.addOption(OptionBuilder.create());

		OptionBuilder.withLongOpt(OPTION_BAUD);
		OptionBuilder.withDescription("set the baud rate");
		OptionBuilder.withValueSeparator( '=' );
		OptionBuilder.hasArg();
		options.addOption(OptionBuilder.create());

		OptionBuilder.withLongOpt(OPTION_DATA);
		OptionBuilder.withDescription("set the data bits [" + DATA_BITS_5 +"|" + DATA_BITS_6 + "|" + DATA_BITS_7 + "|" + DATA_BITS_8 + "]");
		OptionBuilder.withValueSeparator( '=' );
		OptionBuilder.hasArg();
		options.addOption(OptionBuilder.create());

		OptionBuilder.withLongOpt(OPTION_STOP);
		OptionBuilder.withDescription("set the stop bits [" + STOP_BITS_1 + "|" + STOP_BITS_1_5 + "|" + STOP_BITS_2 + "]");
		OptionBuilder.withValueSeparator( '=' );
		OptionBuilder.hasArg();
		options.addOption(OptionBuilder.create());

		OptionBuilder.withLongOpt(OPTION_PARITY);
		OptionBuilder.withDescription("set the parity [" + PARITY_NONE + "|" + PARITY_EVEN + "|" + PARITY_ODD + "|" + PARITY_MARK + "|" + PARITY_SPACE + "]");
		OptionBuilder.withValueSeparator( '=' );
		OptionBuilder.hasArg();
		options.addOption(OptionBuilder.create());

		OptionBuilder.withLongOpt(OPTION_FLOW);
		OptionBuilder.withDescription("set the flow control [" + FLOWCONTROL_NONE + "|" + FLOWCONTROL_RTSCTS + "|" + FLOWCONTROL_XONXOFF + "]");
		OptionBuilder.withValueSeparator( '=' );
		OptionBuilder.hasArg();
		options.addOption(OptionBuilder.create());

		final CommandLineParser	parser = new PosixParser();

		try
			{
			// parse the command line arguments
			final CommandLine	commandLine = parser.parse(options, aArgs);

			if ( commandLine.hasOption(OPTION_HELP) )
				{
				final HelpFormatter	formatter = new HelpFormatter();
				formatter.printHelp("UartEchoServer", options);
				}
			else
				{
				final UartEchoServer	echoServer=new UartEchoServer();
				echoServer.Construct(commandLine);
				}
			}
		catch ( Exception e )
			{
			e.printStackTrace();
			}
		}
	}
