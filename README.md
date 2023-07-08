# Watermeter Zigbee Telink TLSR8258 (E-BYTE E180-Z5812SP)

[Repository watermeter_zed](https://github.com/slacky1965/watermeter_zed)

---

**Описание**

* Рассчитано на два счетчика воды.
* Не работает с системой namur, только замыкание-размыкание.
* Ведет подсчет замыканий-размыканий, увеличивая каждый раз количество литров на заданное значение от 1 до 10 литров (по умолчанию 10 литров на один импульс).
* Сохраняет показания в энергонезависимой памяти модуля.
* Передает показания по сети Zigbee.
* Взаимодейстивие с "умными домами" через zigbee2mqtt.
* Первоначальная настройка происходит через web-интерфейс zigbee2mqtt. 

---

**Железо**

В проекте используется модуль от компании E-BYTE на чипе TLSR8258 - E180-Z5812SP.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/E180-Z5812SP.jpg" alt="E-BYTE E180-Z5812SP"/>

Испытывалось все на вот таком модуле от Ai-Thinker TB-04-KIT

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/telink_tlsr8258_dongle.jpg" alt="Telink TLSR8258 dongle"/>

---

**Корпус**

Решение самодостаточного модуля было подсмотрено в проекте [waterius](https://github.com/dontsovcmc/waterius).
Чтобы не изобретать, куда это потом все прятать, берем, так называемый, Battery Holder Box на 3 батарейки АА. Переделываем на 2 батарейки и получаем питание 3 вольта и один сегмент под плату.
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/box/box1.jpg" alt="BOX 3AA"/>
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/box/box2.jpg" alt="BOX 3AA"/>
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/box/box3.jpg" alt="BOX 3AA"/>
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/box/box4.jpg" alt="BOX 3AA"/>

---

**Плата**

Платат разведена, но пока не заказана и не проверена.

---

**Готовое устройство**

Пока нет.

---

**Софт**

[Последнюю прошивку](https://github.com/slacky1965/watermeter_zed/raw/main/watermeter_zed_V1.2.bin) нужно залить в модуль с помощью [github.com/pvvx/TLSRPGM](https://github.com/pvvx/TLSRPGM) или оригинального программатора от Telink
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/telink_pgm.jpg" alt="Telink PGM"/>

---

**Настройка**

---

**История версий**

- 1.0 - Начало.
- 1.1 - Прошивка без ОТА. 
- 1.2 - Добавлена возможность обновления OTA. 
