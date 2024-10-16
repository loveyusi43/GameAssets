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
    BufferedImage bird_image;   //С��ͼƬ

    BufferedImage[] images; //С������״̬��ͼƬ

    public Bird() {
        images = new BufferedImage[8];
        //ѭ����С������ͼƬ��������
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

    //С��ɵķ���
    public void fly() {
        bird_image = images[index % images.length];
        index++;
    }

    int g = 6;  //�������ٶ�
    double t = 0.15;   //����ʱ��0.18
    double v = 0;   //���ٶ�
    double h = 0;   //����ľ���

    //С������һ��
    public void down() {
        h = v * t + g * t * t / 2;  //����ľ���
        v = v + g * t;  //ĩ�ٶ�
        y = y + (int) h;
    }

    //С�����Ϸ�
    public void up() {
        v = -26;    //���Ϸɸ�һ�����ٶ�
    }


    //С��ײ����ķ���
    public boolean hitGround(Ground ground) {
        boolean isHit = this.y + this.height >= ground.y +30;
        return isHit;
    }

    //С��ײ���ӵķ���
    public boolean hitColumn(Column column) {
        //ײˮƽ���������
        boolean b1 = this.x + this.width >= column.x;
        boolean b2 = this.x <= column.x + column.width;
        //ײ��ֱ���������
        boolean b3 = this.y + 25 <= column.y + column.height / 2 - column.gap / 2;
        boolean b4 = this.y - 30 + this.height >= column.y + column.height / 2 + column.gap / 2;
        //����b1,b2   ֤��ˮƽ������������ײ
        //b1 b2 b3 ͬʱ���� ײ�������������
        // b1 b2 b4 ͬʱ���� ײ�������������
        return b1 && b2 && (b3 || b4);
    }
}
