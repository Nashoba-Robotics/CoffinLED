import sun.nio.ch.Net;

import javax.swing.*;
import javax.swing.tree.ExpandVetoException;
import java.awt.*;
import java.awt.event.*;

/**
 * Created by Colin on 4/3/2015.
 */
public class Frame extends JFrame implements WindowListener
{
    static Object lock = new Object();
    private static Frame singleton;
    public static Frame getInstance()
    {
        synchronized (lock)
        {
            if(singleton == null)
                singleton = new Frame();
            return singleton;
        }
    }

    JLabel status;
    private Frame()
    {
        status = new JLabel("Not Connected");
        status.setForeground(Color.RED);
        
        Arduino.getInstance().setConnectionStatusListener(new Arduino.ConnectionStatusChangeListener() {
            @Override
            public void statusChanged() {
                if (Arduino.getInstance().getConnectionStatus()) {
                    status.setText("Connected!");
                    status.setForeground(Color.GREEN);
                } else {
                    status.setText("Not Connected");
                    status.setForeground(Color.RED);
                }
            }
        });
        Arduino.getInstance().initialize();

        setupNetworkListener();

        status.setFont(new Font("Arial", Font.PLAIN, 20));
        JPanel mainPanel = new JPanel();
        mainPanel.add(status);

        add(mainPanel, BorderLayout.CENTER);

        setSize(500, 500);
        addWindowListener(this);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setVisible(true);
    }

    private String currentState = "off";
    private void setupNetworkListener()
    {
        Network.getInstance().connect();
        Network.getInstance().setOnMessageReceivedListener(new Network.OnMessageReceivedListener()
        {
            @Override
            public void onMessageReceived(String key, Object value)
            {
                try
                {
                    currentState = Network.getInstance().getString("ArduinoState");
                }
                catch (Exception e)
                {

                }

                int arduinoArgument = 0;
                try
                {
                    arduinoArgument = (int) ((double)Network.getInstance().getNumber("ArduinoArgument"));
                }
                catch (Exception e)
                {

                }
                if(value.equals("Score"))
                    Arduino.getInstance().sendMessage("3 0:");
                else if(value.equals("Off"))
                    Arduino.getInstance().sendMessage("0 0:");
                else if(currentState.equals("Countdown"))
                    Arduino.getInstance().sendMessage("2 " + arduinoArgument + ":");
            }
        });
    }

    @Override
    public void windowOpened(WindowEvent e)
    {

    }

    @Override
    public void windowClosing(WindowEvent e)
    {
        System.out.println("Serial Closed");
        Arduino.getInstance().close();
    }

    @Override
    public void windowClosed(WindowEvent e)
    {

    }

    @Override
    public void windowIconified(WindowEvent e)
    {

    }

    @Override
    public void windowDeiconified(WindowEvent e)
    {

    }

    @Override
    public void windowActivated(WindowEvent e)
    {

    }

    @Override
    public void windowDeactivated(WindowEvent e)
    {

    }
}
