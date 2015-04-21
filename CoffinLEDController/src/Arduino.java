import java.io.BufferedReader;
import java.io.InputStreamReader;

import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;

import java.io.PrintWriter;
import java.util.Enumeration;


public class Arduino implements SerialPortEventListener
{
    private static Arduino singleton;
    public static Arduino getInstance()
    {
        if(singleton == null)
            singleton = new Arduino();
        return singleton;
    }

    private MessageListener messageListener;

    SerialPort serialPort;
    /** The port we're normally going to use. */
    private static final String PORT_NAMES[] = {
            "/dev/tty.usbserial-A9007UX1", // Mac OS X
            "/dev/ttyACM0", // Raspberry Pi
            "/dev/ttyUSB0", // Linux
            "COM3",
            "COM4", // Windows
            "COM5"
    };

    private Arduino(){}

    /**
     * A BufferedReader which will be fed by a InputStreamReader
     * converting the bytes into characters
     * making the displayed results codepage independent
     */
    private BufferedReader input;
    /** The output stream to the port */
    private PrintWriter output;
    /** Milliseconds to block while waiting for port open */
    private static final int TIME_OUT = 2000;
    /** Default bits per second for COM port. */
    private static final int DATA_RATE = 9600;

    private boolean connected = false;

    public void setMessageListener(MessageListener listener)
    {
        messageListener = listener;
    }

    private ConnectionStatusChangeListener connectionListener;
    public void setConnectionStatusListener(ConnectionStatusChangeListener listener)
    {
        this.connectionListener = listener;
    }

    public void initialize()
    {
        // the next line is for Raspberry Pi and
        // gets us into the while loop and was suggested here was suggested http://www.raspberrypi.org/phpBB3/viewtopic.php?f=81&t=32186
        //System.setProperty("gnu.io.rxtx.SerialPorts", "COM5");
        new Thread(new Runnable() {
            @Override
            public void run()
            {
                while(true)
                {
                    CommPortIdentifier portId = null;
                    Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

                    //First, Find an instance of serial port as set in PORT_NAMES.
                    while (portEnum.hasMoreElements())
                    {
                        CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement();
                        for (String portName : PORT_NAMES)
                        {
                            if (currPortId.getName().equals(portName))
                            {
                                portId = currPortId;
                                break;
                            }
                        }
                    }
                    if (portId == null) {
                        System.out.println("Could not find COM port.");
                        return;
                    }

                    try {
                        // open serial port, and use class name for the appName.
                        serialPort = (SerialPort) portId.open(this.getClass().getName(),
                                TIME_OUT);

                        // set port parameters
                        serialPort.setSerialPortParams(DATA_RATE,
                                SerialPort.DATABITS_8,
                                SerialPort.STOPBITS_1,
                                SerialPort.PARITY_NONE);

                        // open the streams
                        input = new BufferedReader(new InputStreamReader(serialPort.getInputStream()));
                        output = new PrintWriter(serialPort.getOutputStream());

                        // add event listeners
                        serialPort.addEventListener(Arduino.this);
                        serialPort.notifyOnDataAvailable(true);
                        connected = true;
                        if(connectionListener != null)
                            connectionListener.statusChanged();
                        break;
                    }
                    catch (Exception e)
                    {
                        System.err.println(e.toString());
                        try {
                            Thread.sleep(2000);
                        } catch (InterruptedException e1) {
                            e1.printStackTrace();
                        }
                    }
                }
            }
        }).start();

    }

    /**
     * This should be called when you stop using the port.
     * This will prevent port locking on platforms like Linux.
     */
    public synchronized void close() {
        if (serialPort != null) {
            serialPort.removeEventListener();
            serialPort.close();
        }
    }

    /**
     * Handle an event on the serial port. Read the data and print it.
     */
    public synchronized void serialEvent(SerialPortEvent oEvent) {
        if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE)
        {
            try
            {
                String inputLine=input.readLine();
                if(inputLine != null)
                    messageListener.onMessageReceived(inputLine);
            }
            catch (Exception e) {
            }
        }
        // Ignore all the other eventTypes, but you should consider the other ones.
    }

    public void sendMessage(String message)
    {
        if(connected)
        {
            output.println(message);
            output.flush();
        }
    }

    public interface ConnectionStatusChangeListener
    {
        public void statusChanged();
    }

    public boolean getConnectionStatus()
    {
        return connected;
    }

    public interface MessageListener
    {
        public void onMessageReceived(String message);
    }
}