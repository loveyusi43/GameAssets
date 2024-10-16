package com.qiku.bird;

import java.awt.image.BufferedImage;

/**
 * @ClassNma:Column
 * @Description TODO
 * @Author:Li
 * @CreateTime:2022/09/21 13:56
 */
public class Column {
    int x, y;
    int width, height;
    BufferedImage co_image;

    int gap;    //上下间隙
    int distance;   //水平间隙

    int min = -(1200 / 2 - 144 / 2);
    int max = 644 - 146 - 144 / 2 - 1200 / 2;

    public Column() {
        co_image = BirdGame.col_img;
        width = co_image.getWidth();
        // width = (int) (Math.random() * 20 + 150);
        height = co_image.getHeight();
        x = 432;
        y = (int) (Math.random() * (max - min) + min);
        gap = 144;  //144
        distance = 244;
    }

    public void star() {
        x = x - 1;
        if (x <= -width) {  //柱子越界
            x = 432;
            //重新随机
            y = (int) (Math.random() * (max - min) + min);
        }
    }
}
