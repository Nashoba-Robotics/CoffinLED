import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.text.DecimalFormat;
import java.util.ArrayList;

/**
 * Created by Colin on 4/3/2015.
 */
public class Main extends JFrame implements WindowListener
{
    private class DataValue
    {
        public DataValue(double value, long time)
        {
            this.value = value;
            this.timestamp = time;
        }
        public double value;
        public long timestamp;
    }
    ArrayList<DataValue> data = new ArrayList<DataValue>();

    JLabel numberLabel;
    JLabel avgLabel;
    SerialPortHelper main;
    public Main()
    {
        main = new SerialPortHelper();
        main.initialize();
        main.setMessageListener(new SerialPortHelper.MessageListener()
        {
            @Override
            public void onMessageReceived(String message)
            {
                try
                {

                    for(int i = 0; i < data.size(); i++)
                    {
                        if(data.get(i).timestamp < System.currentTimeMillis() - 100)
                        {
                            data.remove(i);
                        }
                    }
                    double distance = Double.parseDouble(message);
                    data.add(new DataValue(distance, System.currentTimeMillis()));

                    double sum = 0;
                    for(int i = 0; i < data.size(); i++)
                        sum += data.get(i).value;
                    sum /= (float)data.size();

                    sum = Math.round(sum*100)/100.;
                    DecimalFormat df = new DecimalFormat();
                    df.setMaximumFractionDigits(2);
                    numberLabel.setText(df.format(sum) + " cm");

                    avgLabel.setText("Averaging " + data.size() + " values");
                }
                catch (Exception e)
                {

                }
            }
        });

        JPanel panel = new JPanel(new GridLayout(2, 1));

        numberLabel = new JLabel();
        numberLabel.setText("0 cm");
        numberLabel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        numberLabel.setFont(new Font("Arial", Font.PLAIN, 26));
        panel.add(numberLabel);

        avgLabel = new JLabel();
        avgLabel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        avgLabel.setFont(new Font("Arial", Font.PLAIN, 26));
        panel.add(avgLabel);

        add(panel, BorderLayout.CENTER);

        JButton resetButton = new JButton("Reset");
        resetButton.addActionListener(new ActionListener()
        {
            @Override
            public void actionPerformed(ActionEvent e)
            {
                main.close();
                main.initialize();
                numberLabel.setText("0 cm");
            }
        });
        add(resetButton, BorderLayout.SOUTH);

        setSize(500, 500);
        addWindowListener(this);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setVisible(true);
    }

    public static void main(String[] args) throws Exception
    {
        new Main();
        System.out.println("Started");
    }

    @Override
    public void windowOpened(WindowEvent e)
    {

    }

    @Override
    public void windowClosing(WindowEvent e)
    {
        System.out.println("Serial Closed");
        main.close();
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
