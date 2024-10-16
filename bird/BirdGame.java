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
 * 游戏启动类
 * 使用extends关键字继承JPanel画板类
 */
public class BirdGame extends JPanel {

    //定义游戏状态
    public static final int START = 0;  //开始状态
    public static final int RUNNING = 1;    //运行状态
    public static final int GAME_OVER = 2;  //结束状态

    //游戏的当前状态 默认0 --开始状态
    int start = START;


    int score = 0;  //玩家的得分

    static BufferedImage bg = null;    //背景图片
    static BufferedImage ground = null; //地面图片
    static BufferedImage bird_img = null; //小鸟图
    static BufferedImage col_img = null;    //管道图
    static BufferedImage start_img = null;    //开始的图片
    static BufferedImage over_img = null;    //结束的图片

    //静态代码块，一般用于加载静态资源(视频，音频，图片)
    static {
        try {
            //将本地图片读取到程序中
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


    Ground ground_g;  //声明地面
    Bird Bird_g;
    Column column_g;
    Column column_g2;

    //BirdGame的构造方法(在new BirdGame() 的时候执行)
    public BirdGame() {
        ground_g = new Ground();//创建地面对象
        Bird_g = new Bird();
        column_g = new Column();
        column_g2 = new Column();
        column_g2.x = column_g.x + column_g.distance;

    }


    //用于在绘画板绘制内容的方法:在这个方法里面绘画
    @Override
    public void paint(Graphics g) {
/*        g.setColor(Color.PINK); //画笔颜色设置
        g.fillRect(0,0,1080,1080);*/

        g.drawImage(bg, 0, 0, null);    //画背景

        g.drawImage(column_g.co_image, column_g.x, column_g.y, null);
        g.drawImage(Bird_g.bird_image, Bird_g.x, Bird_g.y, null);
        g.drawImage(column_g2.co_image, column_g2.x, column_g2.y, null);
        g.drawImage(ground_g.img_ground, ground_g.x, ground_g.y, null);

        //开始状态
        if (start == START) {
            g.drawImage(start_img, 0, 0, null);
        }
        //结束状态
        if (start == GAME_OVER) {
            g.drawImage(over_img, 0, 0, null);
        }

        //画分数
        Font font = new Font("微软雅黑", Font.BOLD, 30);    //创建字体
        g.setFont(font);    //给画笔设置字体
        g.setColor(Color.red);
        g.drawString("分数："+score,25,40);
        g.setColor(Color.WHITE);
        g.drawString("分数："+score,23,38);
    }

    //检测游戏是否结束,判断小鸟是否碰撞
    public boolean isGameOver() {
        boolean isHit = Bird_g.hitGround(ground_g) || Bird_g.hitColumn(column_g) || Bird_g.hitColumn(column_g2);
        return isHit;
    }


    //游戏流程控制的方法
    public void action() throws InterruptedException {

        frame.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode()==32){
                    //按下空格键
                    start = RUNNING;
                }
                if (start ==RUNNING){
                    Bird_g.up();
                }
                if (start == GAME_OVER) {  //结束状态 ，单击变成开始状态

                }
            }
        });



        //给当前对象添加鼠标单击事件
        this.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (start == START) {    //如果是开始状态，单击转换为运行状态
                    start = RUNNING;
                }
                if (start == RUNNING) {  //运行状态 ，单击小鸟飞
                    //小鸟飞
                    Bird_g.up();
                }
                if (start == GAME_OVER) {  //结束状态 ，单击变成开始状态
                    Bird_g = new Bird();
                    column_g = new Column();
                    column_g2 = new Column();
                    column_g2.x = column_g.x + column_g.distance;
                    score = 0;
                    start = START;
                }
            }
        });


        //死循环
        while (true) {
            //开始
            if (start == START) {
                ground_g.step();    //地面移动
                Bird_g.fly();
            } else if (start == RUNNING) {
                ground_g.step();    //地面移动
                Bird_g.fly();   //小鸟飞翔
                column_g.star();    //柱子1移动
                column_g2.star();   //柱子2移动
                Bird_g.down();  //小鸟掉落

                if (isGameOver() == true) {  //游戏结束
                    //初始化游戏状态
                    Bird_g = new Bird();
                    column_g = new Column();
                    column_g2 = new Column();
                    column_g2.x = column_g.x + column_g.distance;
                    score = 0;
                    //结束
                    start = GAME_OVER;
                }
                if (Bird_g.x == column_g.x+column_g.width+1 ||
                        Bird_g.x == column_g2.x+column_g2.width+1){
                    //得分
                    score += 5;
                }
            }

            repaint();  //重画，重新执行paint方法
            Thread.sleep(10);

            //System.out.println("撞柱子1"+Bird_g.hitColumn(column_g));
        }
    }
    //创建画框对象
    static JFrame frame = new JFrame();
    public static void main(String[] args) throws InterruptedException {

      /*  //创建画框对象
        JFrame frame = new JFrame();*/
        //创建画板对象
        BirdGame game = new BirdGame();

        frame.setSize(432, 644); //设置宽高
        frame.setLocationRelativeTo(null);  //居中显示
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);   //关闭窗口，结束程序
        //设置可见性
        frame.setVisible(true);
        //将画板添加到画框上
        frame.add(game);


        //调用action方法
        game.action();
    }
}
