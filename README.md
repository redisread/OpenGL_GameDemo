# OpenGL_GameDemo
使用OpenGL制作的一个游戏小Demo



## 设计思路:

1. 虚拟场景：沙漠、风车、地球表面等。(可以更换场景)
2. 虚拟物体：机器人、雪人、
3. 纹理贴图：利用skybox的概念，为地面、天空和四周的墙壁贴上一套图共6张，让场景更真实，为正方形贴图。
4. 光照、材质、阴影：机器人使用光照模型，实现Phong光照模型，设定机器人的材质属性，实现阴影效果。
5. 对场景的任意浏览：设计为通过摄像机视角的变化去观察整个场景，包括拉远拉进视角、左右旋转视角、上下旋转视角。还可以通过机器人的第一视角行走去观察整个场景。
6. 通过交互控制物体：设计为可控制机器人进行第一视角和第三视角的行走，并且行走时会自动做出类似人类行走时的肢体动作。

## 实现

### 天空盒

使用天空盒样例：

![image-20200704003224847](https://i.loli.net/2020/07/04/gQ3wb4qhnUSBj8x.png)



场景贴图效果：

![img](https://i.loli.net/2020/07/04/rjWlcmt9w7zoL63.jpg)





### 导入人物模型

机器人的层级模型

![image-20200704003451454](https://i.loli.net/2020/07/04/VF6EWGn52ro1Mbs.png)

初步实现：

![img](https://i.loli.net/2020/07/04/dURBXDck6yJZ2Sq.jpg)



附上颜色：

![img](https://i.loli.net/2020/07/04/lK4C5wmpsuG63zE.jpg)



添加材质光照：

![img](https://i.loli.net/2020/07/04/LJnZ3DhMBv29qW4.jpg)



添加键盘鼠标操作移动事件：

![img](https://i.loli.net/2020/07/04/3GqBdvlZt45Xpkj.jpg)



### 加入相机键盘鼠标操作最终的效果如下：

1. 相机操作

‘w’：视角向上                ‘s‘：视角向下

![img](https://i.loli.net/2020/07/04/OsYjBKq78UvE9Hw.jpg) ![img](https://i.loli.net/2020/07/04/OCDTVWecN3t5o2j.jpg)

 

‘a’：镜头左转                ‘d’: 镜头右转

![img](https://i.loli.net/2020/07/04/eYF3cXUdBHNhEla.jpg) ![img](https://i.loli.net/2020/07/04/nqFrfeAs7a3C4QV.jpg)

其他按键:

‘z’:视线距离变小；‘Z’视线距离变大‘x’:透视距离变远；’X‘透视距离变近

 

2. 机器人操作：

​    GLUT_UP_KEY机器人前进         GLUT_DOWN_KEY机器人后退

![img](https://i.loli.net/2020/07/04/B2pvjktVASf7rI6.jpg)     ![img](https://i.loli.net/2020/07/04/iuXZcqx9NrF5BIe.jpg)

 

GLUT_LEFT_KEY机器人左转        GLUT_RIGHT_KEY机器人右转

 ![img](https://i.loli.net/2020/07/04/KELhbF8R3JDitIV.jpg)     ![img](https://i.loli.net/2020/07/04/wvd5JGZj61unyQl.jpg)

其他按键 : ‘Y’:开启跟随模式;’I’:进入上帝视角；‘u’机器人跳舞，‘*’相机到天空盒外部

 

**特殊操作**

‘f’:换脸

![img](https://i.loli.net/2020/07/04/lQrpLj6FAkOX58W.jpg) ![img](https://i.loli.net/2020/07/04/neplOv3wFbNWKmr.jpg) ![img](https://i.loli.net/2020/07/04/HoBgPA8k5umeC1w.jpg)

 

‘F’:换肤

![img](https://i.loli.net/2020/07/04/No5dmhQVGHw93t4.jpg) ![img](https://i.loli.net/2020/07/04/jA6T9qCg2bFmBHc.jpg) ![img](https://i.loli.net/2020/07/04/3zBxAKf4SXmIGb8.jpg)

 

按下’t’进行场景的切换场景切换

![img](https://i.loli.net/2020/07/04/MzG7D8RYCP6HsXi.jpg) ![img](https://i.loli.net/2020/07/04/HrIqQlGwNgkaheb.jpg)





### 导入雪人

![img](https://i.loli.net/2020/07/04/F8iaNSpGtX9bmrk.jpg) ![img](https://i.loli.net/2020/07/04/YRhNGUBXecj6C2K.jpg)





#### 一览：

![img](https://i.loli.net/2020/07/04/VuriLGDFv3ICtzx.jpg)![img](https://i.loli.net/2020/07/04/YRhNGUBXecj6C2K.jpg)![img](C:/Users/Victor/AppData/Local/Temp/msohtmlclip1/01/clip_image006.jpg) ![img](C:/Users/Victor/AppData/Local/Temp/msohtmlclip1/01/clip_image008.jpg)

 