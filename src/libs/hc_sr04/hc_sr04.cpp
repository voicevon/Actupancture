#include "hc_sr04.h"

#define Trig 2 //引脚Tring 连接 IO D2
#define Echo 3 //引脚Echo 连接 IO D3 

float distance_mm; //距离变量

HcSr04::HcSr04(uint8_t pin_trig, uint8_t pin_echo){
    this->__pin_trig = pin_trig;
    this->__pin_echo = pin_echo;
    pinMode(pin_trig, OUTPUT);
    pinMode(pin_echo, INPUT);

}

int HcSr04::CheckDistance(bool show_debug){
    //给Trig发送一个低高低的短时间脉冲,触发测距
    digitalWrite(Trig, LOW); //给Trig发送一个低电平
    delayMicroseconds(2);    //等待 2微妙
    digitalWrite(Trig,HIGH); //给Trig发送一个高电平
    delayMicroseconds(10);    //等待 10微妙
    digitalWrite(Trig, LOW); //给Trig发送一个低电平

    float interval = float(pulseIn(Echo, HIGH)); //存储回波等待时间,
    //pulseIn函数会等待引脚变为HIGH,开始计算时间,再等待变为LOW并停止计时
    //返回脉冲的长度

    //声速是:340m/1s 换算成 34000cm / 1000000μs &#61;&gt; 34 / 1000
    //因为发送到接收,实际是相同距离走了2回,所以要除以2
    //距离(厘米)  =  (回波时间 * (34 / 1000)) / 2
    //简化后的计算公式为 (回波时间 * 17)/ 1000

    distance_mm = (interval * 17 )/100; //把回波时间换算成mm

    if (show_debug){
        Serial.print("Echo");
        Serial.print(interval);//串口输出等待时间的原始数据
        Serial.print(" Distance ");
        Serial.print(distance_mm);//串口输出距离换算成cm的结果
    }
    uint8_t result = distance_mm / 10;
    if (distance_mm > 1000) {
        result = 0;
    }
    return result;

}
