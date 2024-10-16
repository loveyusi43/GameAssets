package com.qiku.bird;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.IOException;

/**
 * @ClassNma:Start
 * @Description TODO
 * @Author:Li
 * @CreateTime:2022/09/21 11:35
 */
public class Bird {
    int x, y;
    int width, height;
    BufferedImage bird_image;   //小鸟图片

    BufferedImage[] images; //小鸟所有状态的图片

    public Bird() {
        images = new BufferedImage[8];
        //循环将小鸟所有图片存入数组
        for (int i = 0; i < images.length; i++) {
            try {
                images[i] = ImageIO.read(Bird.class.getResourceAsStream(i + ".png"));
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }

        bird_image = BirdGame.bird_img;
        width = BirdGame.bird_img.getWidth();
        height = BirdGame.bird_img.getHeight();
        x = 110;
        y = 240;
    }

    int index;

    //小鸟飞的方法
    public void fly() {
        bird_image = images[index % images.length];
        index++;
    }

    int g = 6;  //重力加速度
    double t = 0.15;   //下落时间0.18
    double v = 0;   //初速度
    double h = 0;   //下落的距离

    //小鸟下落一次
    public void down() {
        h = v * t + g * t * t / 2;  //下落的距离
        v = v + g * t;  //末速度
        y = y + (int) h;
    }

    //小鸟向上飞
    public void up() {
        v = -26;    //向上飞给一个初速度
    }


    //小鸟撞地面的方法
    public boolean hitGround(Ground ground) {
        boolean isHit = this.y + this.height >= ground.y +30;
        return isHit;
    }

    //小鸟撞柱子的方法
    public boolean hitColumn(Column column) {
        //撞水平方向的柱子
        boolean b1 = this.x + this.width >= column.x;
        boolean b2 = this.x <= column.x + column.width;
        //撞垂直方向的柱子
        boolean b3 = this.y + 25 <= column.y + column.height / 2 - column.gap / 2;
        boolean b4 = this.y - 30 + this.height >= column.y + column.height / 2 + column.gap / 2;
        //满足b1,b2   证明水平方向与柱子相撞
        //b1 b2 b3 同时满足 撞的是上面的柱子
        // b1 b2 b4 同时满足 撞的是下面的柱子
        return b1 && b2 && (b3 || b4);
    }
}
