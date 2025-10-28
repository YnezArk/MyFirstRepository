import java.awt.*;
import javax.swing.*;

public class AppGraphInOut8
{
    public static void main( String args[] )
    {
        new AppFrame();
    }
}

class  AppFrame extends JFrame
{
    JTextField in = new JTextField(10);
    JButton btn = new JButton("算平方");
    JLabel out = new JLabel("asd");
    JLabel out1 = new JLabel("as");

    public AppFrame()
    {
        setLayout( new FlowLayout() );
        getContentPane().add( in );
        getContentPane().add( btn );
        getContentPane().add( out );
        getContentPane().add( out1 );
        btn.addActionListener( e->{
            String s = in.getText();
            double d = Double.parseDouble( s );
            double sq = d * d;
            out.setText( d + "平方是" + sq );
        });
        setSize( 400,100 );
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setVisible(true);
    }
}
