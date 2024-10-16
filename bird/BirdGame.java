package com.qiku.bird;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.io.IOException;

/**
 * @ClassNma:BirdGame
 * @Description TODO
 * @Author:Li
 * @CreateTime:2022/09/21 9:28
 */

/**
 * ��Ϸ������
 * ʹ��extends�ؼ��ּ̳�JPanel������
 */
public class BirdGame extends JPanel {

    //������Ϸ״̬
    public static final int START = 0;  //��ʼ״̬
    public static final int RUNNING = 1;    //����״̬
    public static final int GAME_OVER = 2;  //����״̬

    //��Ϸ�ĵ�ǰ״̬ Ĭ��0 --��ʼ״̬
    int start = START;


    int score = 0;  //��ҵĵ÷�

    static BufferedImage bg = null;    //����ͼƬ
    static BufferedImage ground = null; //����ͼƬ
    static BufferedImage bird_img = null; //С��ͼ
    static BufferedImage col_img = null;    //�ܵ�ͼ
    static BufferedImage start_img = null;    //��ʼ��ͼƬ
    static BufferedImage over_img = null;    //������ͼƬ

    //��̬����飬һ�����ڼ��ؾ�̬��Դ(��Ƶ����Ƶ��ͼƬ)
    static {
        try {
            //������ͼƬ��ȡ��������
            bg = ImageIO.read(BirdGame.class.getResourceAsStream("bg.png"));
            ground = ImageIO.read(BirdGame.class.getResourceAsStream("ground.png"));
            bird_img = ImageIO.read(BirdGame.class.getResourceAsStream("0.png"));
            col_img = ImageIO.read(BirdGame.class.getResourceAsStream("column.png"));
            start_img = ImageIO.read(BirdGame.class.getResourceAsStream("start.png"));
            over_img = ImageIO.read(BirdGame.class.getResourceAsStream("gameover.png"));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }


    Ground ground_g;  //��������
    Bird Bird_g;
    Column column_g;
    Column column_g2;

    //BirdGame�Ĺ��췽��(��new BirdGame() ��ʱ��ִ��)
    public BirdGame() {
        ground_g = new Ground();//�����������
        Bird_g = new Bird();
        column_g = new Column();
        column_g2 = new Column();
        column_g2.x = column_g.x + column_g.distance;

    }


    //�����ڻ滭��������ݵķ���:�������������滭
    @Override
    public void paint(Graphics g) {
/*        g.setColor(Color.PINK); //������ɫ����
        g.fillRect(0,0,1080,1080);*/

        g.drawImage(bg, 0, 0, null);    //������

        g.drawImage(column_g.co_image, column_g.x, column_g.y, null);
        g.drawImage(Bird_g.bird_image, Bird_g.x, Bird_g.y, null);
        g.drawImage(column_g2.co_image, column_g2.x, column_g2.y, null);
        g.drawImage(ground_g.img_ground, ground_g.x, ground_g.y, null);

        //��ʼ״̬
        if (start == START) {
            g.drawImage(start_img, 0, 0, null);
        }
        //����״̬
        if (start == GAME_OVER) {
            g.drawImage(over_img, 0, 0, null);
        }

        //������
        Font font = new Font("΢���ź�", Font.BOLD, 30);    //��������
        g.setFont(font);    //��������������
        g.setColor(Color.red);
        g.drawString("������"+score,25,40);
        g.setColor(Color.WHITE);
        g.drawString("������"+score,23,38);
    }

    //�����Ϸ�Ƿ����,�ж�С���Ƿ���ײ
    public boolean isGameOver() {
        boolean isHit = Bird_g.hitGround(ground_g) || Bird_g.hitColumn(column_g) || Bird_g.hitColumn(column_g2);
        return isHit;
    }


    //��Ϸ���̿��Ƶķ���
    public void action() throws InterruptedException {

        frame.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode()==32){
                    //���¿ո��
                    start = RUNNING;
                }
                if (start ==RUNNING){
                    Bird_g.up();
                }
                if (start == GAME_OVER) {  //����״̬ ��������ɿ�ʼ״̬

                }
            }
        });



        //����ǰ���������굥���¼�
        this.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (start == START) {    //����ǿ�ʼ״̬������ת��Ϊ����״̬
                    start = RUNNING;
                }
                if (start == RUNNING) {  //����״̬ ������С���
                    //С���
                    Bird_g.up();
                }
                if (start == GAME_OVER) {  //����״̬ ��������ɿ�ʼ״̬
                    Bird_g = new Bird();
                    column_g = new Column();
                    column_g2 = new Column();
                    column_g2.x = column_g.x + column_g.distance;
                    score = 0;
                    start = START;
                }
            }
        });


        //��ѭ��
        while (true) {
            //��ʼ
            if (start == START) {
                ground_g.step();    //�����ƶ�
                Bird_g.fly();
            } else if (start == RUNNING) {
                ground_g.step();    //�����ƶ�
                Bird_g.fly();   //С�����
                column_g.star();    //����1�ƶ�
                column_g2.star();   //����2�ƶ�
                Bird_g.down();  //С�����

                if (isGameOver() == true) {  //��Ϸ����
                    //��ʼ����Ϸ״̬
                    Bird_g = new Bird();
                    column_g = new Column();
                    column_g2 = new Column();
                    column_g2.x = column_g.x + column_g.distance;
                    score = 0;
                    //����
                    start = GAME_OVER;
                }
                if (Bird_g.x == column_g.x+column_g.width+1 ||
                        Bird_g.x == column_g2.x+column_g2.width+1){
                    //�÷�
                    score += 5;
                }
            }

            repaint();  //�ػ�������ִ��paint����
            Thread.sleep(10);

            //System.out.println("ײ����1"+Bird_g.hitColumn(column_g));
        }
    }
    //�����������
    static JFrame frame = new JFrame();
    public static void main(String[] args) throws InterruptedException {

      /*  //�����������
        JFrame frame = new JFrame();*/
        //�����������
        BirdGame game = new BirdGame();

        frame.setSize(432, 644); //���ÿ��
        frame.setLocationRelativeTo(null);  //������ʾ
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);   //�رմ��ڣ���������
        //���ÿɼ���
        frame.setVisible(true);
        //��������ӵ�������
        frame.add(game);


        //����action����
        game.action();
    }
}
