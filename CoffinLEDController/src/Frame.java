import edu.wpi.first.wpilibj.networktables.NetworkTable;

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

    private NetworkTable diagram;

    private final String[] LED_OPTIONS = {"Rainbow", "Off"};

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
        selection = new JComboBox<>(LED_OPTIONS);
        selection.setSelectedIndex(0);
        selection.addActionListener(e -> updateLEDSelection());
        mainPanel.add(selection);

        connectingToFieldBox = new JCheckBox("Connecting to Field");
        connectingToFieldBox.setSelected(false);
        connectingToFieldBox.addActionListener(e -> {
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
            } else
            {
                selection.setEnabled(true);
                setTitle("Coffin Controller");
                updateLEDSelection();
            }
        });
        mainPanel.add(connectingToFieldBox);
        mainPanel.setBorder(new EmptyBorder(20, 20, 20, 20));
        add(mainPanel, BorderLayout.CENTER);

        Arduino.getInstance().setConnectionStatusListener(() -> {
            if (Arduino.getInstance().getConnectionStatus())
            {
                status.setText("Connected to LEDs");
                status.setForeground(Color.GRAY);
            } else
            {
                status.setText("Not Connected to LEDs");
                status.setForeground(Color.RED);
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

    private String currentState = "OFF";
    private boolean connected = false;

    private void updateLEDSelection()
    {
        String current = (String)selection.getSelectedItem();
        if(current.equals(LED_OPTIONS[0]))
        {
            sendRainbowCommand();
        }
        else if(current.equals(LED_OPTIONS[1]))
        {
            sendOffCommand();
        }
    }

    private void setupNetworkListener()
    {
        Network.getInstance().setConnectionListener(state -> {
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
        });
        Network.getInstance().setOnMessageReceivedListener((key, value) -> {
            diagram = Network.getInstance().getNetworkSubTable("Robot Diagram");
            currentState = Network.getInstance().getString("ArduinoState");

            double hoodPos =  diagram.getNumber("Hood Position");
            double armAngle = diagram.getNumber("Intake Arm Position");
            boolean rollIntake = diagram.getBoolean("Intake Roller Running");
            boolean rollLoader = diagram.getBoolean("Loader Roller Running");
            double shooterMove = diagram.getNumber("Shooter Percent");
            boolean shooterFull = diagram.getBoolean("Shooter Full Speed");
            boolean ballIntake = diagram.getBoolean("Ball In Intake");
            boolean ballLoader = diagram.getBoolean("Ball In Loader");

            sendHoodCommand(hoodPos);
            sendArmAngleCommand(armAngle);
            sendRollIntakeCommand(rollIntake);
            sendRollLoaderCommand(rollLoader);
            sendShooterMoveCommand(shooterMove);
            sendShooterFullCommand(shooterFull);
            sendBallIntakeCommand(ballIntake);
            sendBallLoaderCommand(ballLoader);


            int arduinoArgument = 0;
            arduinoArgument = (int) ((double) Network.getInstance().getNumber("ArduinoArgument"));
            switch (currentState) {
                case "SCORING":
                    sendScoreCommand();
                    break;
                case "OFF":
                    sendConnectedCommand();
                    break;
                case "COUNTDOWN":
                    sendCountdownCommand(arduinoArgument);
                    break;
            }
        });

        Network.getInstance().connect();
    }

    private void sendScoreCommand()
    {
        Arduino.getInstance().sendMessage("[3 0:");
    }

    private void sendOffCommand()
    {
        Arduino.getInstance().sendMessage("[0 0:");
    }

    private void sendRainbowCommand()
    {
        Arduino.getInstance().sendMessage("[1 0:");
    }

    private void sendCountdownCommand(int amount)
    {
        Arduino.getInstance().sendMessage("[2 " + amount + ":");
    }

    public void sendConnectedCommand()
    {
        Arduino.getInstance().sendMessage("[5 0:");
    }

    public void sendDisconnectedCommand()
    {
        Arduino.getInstance().sendMessage("[4 0:");
    }

    public void sendArmAngleCommand(double armAngle) { Arduino.getInstance().sendMessage(">0 " + Math.round(armAngle) + ":");}

    public void sendHoodCommand(double hoodPos) { Arduino.getInstance().sendMessage(">1 " + Math.round(hoodPos) + ":");}

    public void sendRollIntakeCommand(boolean rollIntake) { Arduino.getInstance().sendMessage(">2 " + (rollIntake ? 1 : 0)+ ":");}

    public void sendRollLoaderCommand(boolean rollLoader) { Arduino.getInstance().sendMessage(">3 " + (rollLoader ? 1 : 0)+ ":");}

    public void sendShooterMoveCommand(double shooterMove) { Arduino.getInstance().sendMessage(">4 " + Math.round(shooterMove*100) + ":");} //Gives the value as an int from 0 to 100

    public void sendShooterFullCommand(boolean shooterFull) { Arduino.getInstance().sendMessage(">5 " + (shooterFull ? 1 : 0)+ ":");}

    public void sendBallIntakeCommand(boolean ballIntake) { Arduino.getInstance().sendMessage(">6 " + (ballIntake ? 1 : 0)+ ":");}

    public void sendBallLoaderCommand(boolean ballLoader) { Arduino.getInstance().sendMessage(">7 " + (ballLoader ? 1 : 0)+ ":");}

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
