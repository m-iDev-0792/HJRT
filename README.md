# HJRT

HJRT is a portable C++14 path tracing renderer which renders image using naive brute force path tracing algorithm.

![](log/img/demo.gif)

**Features**

+ basic material and texture
+ multi-thread and spiral block rendering
+ BVH 
+ depth of field
+ obj model loading 
+ participating media rendering, like fog 
+ multi-format image export
+ multiple importance sampling

HJRT uses [Dear Imgui](https://github.com/ocornut/imgui) as program GUI to show rendering result and progress real-time, check out.

![](log/img/huajigeUI.png)

More results:

![](log/img/dragon500samples-1720s.png)

Mesh rendering

![](log/img/fog.png)

Participating media rendering

![](log/img/diamondGood.png)

Caustic rendering

![](log/img/Texture.png)

Texture mapping

![](log/img/MIS20spp27s.png) ![](log/img/PT20spp35s.png) ![](log/img/PT200spp287s.png) 
 
left: MIS@20spp took 27s; center:PT@20spp took 35s;  right:PT@200spp took 287s

Multiple Importance Sampling

**Acknowledge**

This project is inspired by

[Ray Tracing from the Ground Up 1st Edition](https://www.amazon.com/Ray-Tracing-Ground-Kevin-Suffern-ebook-dp-B01E6SGV8Q/dp/B01E6SGV8Q/ref=mt_kindle?_encoding=UTF8&me=&qid=1191938342)

[smallpt](http://www.kevinbeason.com/smallpt/)

[ray tracing in one weekend](https://github.com/petershirley/raytracinginoneweekend)

**More functions come soon or never**


