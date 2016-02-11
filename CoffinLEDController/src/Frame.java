import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;
import java.awt.event.*;

/**
 * Created by Colin on 4/3/2015.
 */
public class Frame extends JFrame implements WindowListener
{
    static final Object lock = new Object();
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
    JPanel statusPanel;

    JPanel mainPanel;
    JComboBox<String> selection;
    JCheckBox connectingToFieldBox;

    private final String[] LED_OPTIONS = {"Off", "Rainbow", "Bounce", "Score"};

    private Frame()
    {
        setLayout(new BorderLayout());

        status = new JLabel("Not Connected to LEDs");
        status.setForeground(Color.RED);
        status.setFont(new Font("Arial", Font.PLAIN, 20));
        statusPanel = new JPanel();
        statusPanel.add(status);

        add(statusPanel, BorderLayout.NORTH);

        mainPanel = new JPanel(new GridLayout(2, 1));
        selection = new JComboBox<String>(LED_OPTIONS);
        selection.setSelectedIndex(0);
        selection.addActionListener(new ActionListener()
        {
            @Override
            public void actionPerformed(ActionEvent e)
            {
                updateLEDSelection();
            }
        });
        mainPanel.add(selection);

        connectingToFieldBox = new JCheckBox("Connecting to Field");
        connectingToFieldBox.setSelected(false);
        connectingToFieldBox.addActionListener(new ActionListener()
        {
            @Override
            public void actionPerformed(ActionEvent e)
            {
                if (connectingToFieldBox.isSelected())
                {
                    selection.setEnabled(false);
                    if (connected)
                    {
                        sendConnectedCommand();
                        setTitle("Coffin Controller - Connected to Field");
                    }
                    else
                    {
                        sendDisconnectedCommand();
                        Frame.getInstance().setTitle("Coffin Controller - Not Connected to Field");
                    }
                }
                else
                {
                    selection.setEnabled(true);
                    setTitle("Coffin Controller");
                    updateLEDSelection();
                }
            }
        });
        mainPanel.add(connectingToFieldBox);
        mainPanel.setBorder(new EmptyBorder(20, 20, 20, 20));
        add(mainPanel, BorderLayout.CENTER);

        Arduino.getInstance().setConnectionStatusListener(new Arduino.ConnectionStatusChangeListener()
        {
            @Override
            public void statusChanged()
            {
                if (Arduino.getInstance().getConnectionStatus())
                {
                    status.setText("Connected to LEDs");
                    status.setForeground(Color.GRAY);
                } else
                {
                    status.setText("Not Connected to LEDs");
                    status.setForeground(Color.RED);
                }
            }
        });
        Arduino.getInstance().initialize();

        setupNetworkListener();

        setSize(500, 300);
        addWindowListener(this);
        setTitle("Coffin Controller");
        setLocationRelativeTo(null);
        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        setVisible(true);
    }

    private String currentState = "off";
    private boolean connected = false;

    private void updateLEDSelection()
    {
        String current = (String)selection.getSelectedItem();
        if(current.equals(LED_OPTIONS[0]))
        {
            sendOffCommand();
        }
        else if(current.equals(LED_OPTIONS[1]))
        {
            sendRainbowCommand();
        }
        else if(current.equals(LED_OPTIONS[2]))
        {
            sendBounceCommand();
        }
        else if(current.equals(LED_OPTIONS[3]))
        {
            sendScoreCommand();
        }
    }

    private void sendBounceCommand()
    {
        Arduino.getInstance().sendMessage("6 0:");
    }

    private void setupNetworkListener()
    {
        Network.getInstance().setConnectionListener(new Network.ConnectionListener()
        {
            @Override
            public void onConnectionStateChanged(boolean state)
            {
                connected = state;
                if(connectingToFieldBox.isSelected())
                {
                    if(state)
                    {
                        sendConnectedCommand();
                    }
                    else
                    {
                        sendDisconnectedCommand();
                    }
                }
            }
        });
        Network.getInstance().setOnMessageReceivedListener(new Network.OnMessageReceivedListener() {
            @Override
            public void onMessageReceived(String key, Object value) {
                try {
                    currentState = Network.getInstance().getString("ArduinoState");
                } catch (Exception e) {

                }

                int arduinoArgument = 0;
                try {
                    arduinoArgument = (int) ((double) Network.getInstance().getNumber("ArduinoArgument"));
                } catch (Exception e) {

                }
                if(connectingToFieldBox.isSelected())
                {
                    if (currentState.equals("Score"))
                        sendScoreCommand();
                    else if (currentState.equals("Off"))
                    {
                        sendConnectedCommand();
                    }
                    else if (currentState.equals("Countdown"))
                        sendCountdownCommand(arduinoArgument);
                }
            }
        });

        Network.getInstance().connect();
    }

    private void sendScoreCommand()
    {
        Arduino.getInstance().sendMessage("3 0:");
    }

    private void sendOffCommand()
    {
        Arduino.getInstance().sendMessage("0 0:");
    }

    private void sendRainbowCommand()
    {
        Arduino.getInstance().sendMessage("1 0:");
    }

    private void sendCountdownCommand(int amount)
    {
        Arduino.getInstance().sendMessage("2 " + amount + ":");
    }

    public void sendConnectedCommand()
    {
        Arduino.getInstance().sendMessage("5 0:");
    }

    public void sendDisconnectedCommand()
    {
        Arduino.getInstance().sendMessage("4 0:");
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
