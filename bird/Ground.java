package com.qiku.bird;

/**
 * @ClassNma:Ground
 * @Description TODO
 * @Author:Li
 * @CreateTime:2022/09/21 11:07
 */

import java.awt.image.BufferedImage;

/**
 * ������
 */
public class Ground {
    int x, y;
    int width, height;
    BufferedImage img_ground;   //����ͼƬ

    public Ground() {
        img_ground = BirdGame.ground;
        x = 0;
        y = BirdGame.bg.getHeight() - img_ground.getHeight();
        width = img_ground.getWidth();
        height = img_ground.getHeight();
    }

    //�����ƶ�
    public void step() {
        x = x - 1;
        if (x <= 432 - width) {
            x = 0;
        }
    }
}
