# Watermeter Zigbee Telink TLSR8258 (E-BYTE E180-Z5812SP)

[Repository watermeter_zed](https://github.com/slacky1965/watermeter_zed)

---

## На данный момент существует два варианта устройства.

* V1 - только счетчик на два канала. Вынесена в [отдельную ветку](https://github.com/slacky1965/watermeter_zed/tree/version1) и больше не поддерживается.
* V2 - счетчик на два канала + два датчика протечки.
* В обоих вариантах прошивка одинаковая.

---

* [Описание](#description)
* [Железо](#hardware)
* [Софт](#software)  
* [Принцип работы](#firmware)
* [Настройка](#settings)
* [Home Assistant](#home_assistant)

---

## <a id="description">Описание</a>

* Рассчитано на два счетчика воды и на два датчика протечки.
* Не работает с системой namur и счетчиками, где применен датчик "холла". Только замыкание-размыкание, например геркон.
* Ведет подсчет замыканий-размыканий, увеличивая каждый раз количество литров на заданное значение от 1 до 10 литров (по умолчанию 10 литров на один импульс).
* Посылает команду On при обнаружении протечки на любом датчике сразу на два привода.
* Сохраняет показания в энергонезависимой памяти модуля.
* Передает показания по сети Zigbee.
* EndPoint'ы с кластерами On_Off имеют возможность напрямую подключаться к исполнительному устройству минуя координатор.
* Взаимодейстивие с "умными домами" через zigbee2mqtt.
* Первоначальная настройка происходит через web-интерфейс zigbee2mqtt.
* Подключиться к сети zigbee - нажать кнопку 5 раз.
* Покинуть сеть zigbee - нажать кнопку 5 раз.
* Сделать restart модуля - зажать кнопку более чем на 5 секунд (это всего-лишь reset, без изменения каких-либо настроек).
* При одиночном нажатии кнопки модуль просыпается и высылает отчет.

---

## <a id="hardware">Железо</a>

В проекте используется модуль от компании E-BYTE на чипе TLSR8258F512ET32 - E180-Z5812SP.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/E180-Z5812SP.jpg" alt="E-BYTE E180-Z5812SP"/>

Испытывалось все на вот таком dongle от Telink

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/telink_tlsr8258_dongle.jpg" alt="Telink TLSR8258 dongle"/>

---

### Корпус

Корпус изготовлен на 3D принтере.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/box/box_v2.jpg" alt="Watermeter box v2"/>

---

### Схема

Схема модуля.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/schematic_watermeter_zed_v2.jpg" alt="schematic_v2"/>

---

### Плата

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/board_top_v2.jpg" alt="Board top v2"/>

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/board_bottom_v2.jpg" alt="Board bottom v2"/>

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/board_v2.jpg" alt="Board v2"/>

[Ссылка на проект в easyeda](https://oshwlab.com/slacky/watermeterv2_zed)

---

### Датчики протечки

Непосредственно сам датчик протечки представляет из себя два контакта. При погружении контактов в воду меняется сопротивление между ними. Это и фиксирует устройство. 

Можно приобрести готовые, например у фирмы Гидролок - Gidrolock WSP.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/gidrolock_wsp1.jpg" alt="Gidrolock WSP"/>

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/gidrolock_wsp2.jpg" alt="Gidrolock WSP"/>

Или на Aliexpress вот такие

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/wl_sensor.jpg" alt="Water Leak Sensor"/>

Или припаять два провода к двум контактам, например, разъема pin header. Результат будет одинаковый :)

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/pin_header.jpg" alt="Pin Header"/>

---

### Готовое устройство

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/device_v2_big.png" alt="Watermeter device v2"/>

---

## <a id="software">Софт</a>

[Последнюю прошивку](https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/watermeter_zed_V1.3.04.bin) нужно залить в модуль с помощью [github.com/pvvx/TLSRPGM](https://github.com/pvvx/TLSRPGM) или оригинального программатора от Telink.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/telink_pgm.jpg" alt="Telink PGM"/>

Как сделать недорогой программатор на базе модулей TB-03 или TB-04 можно почитать [тут](https://slacky1965.github.io/electricity_meter_zrd/#%D0%B7%D0%B0%D0%B3%D1%80%D1%83%D0%B7%D0%BA%D0%B0-%D0%BF%D1%80%D0%BE%D1%88%D0%B8%D0%B2%D0%BA%D0%B8)

Используется последнее [SDK zigbee](http://wiki.telink-semi.cn/tools_and_sdk/Zigbee/Zigbee_SDK.zip) от Telink'а. Проект сформирован таким образом, что его можно собрать обычным make'ом как под Windows, в оболочке [Git Bash](https://git-scm.com/download/win), а также под Linux'ом (я проверял на Debian 11).

Как добавить проект в Eclipse можно почитать [тут](https://slacky1965.github.io/electricity_meter_zrd/#%D0%BA%D0%BE%D0%BC%D0%BF%D0%B8%D0%BB%D1%8F%D1%86%D0%B8%D1%8F). Все точно так же, только для другого проекта.

Прошивка собрана по схеме, т.е. подключается файл платы `board_8258_diy.h`. Еще адаптирована плата dongle, т.е. `board_8258_dongle.h`. Для других вариантов придется самостоятельно редактировать файл нужной платы.

---

## <a id="firmware">Описание работы модуля</a>

В первый старт происходит попытка подключения к сети Zigbee. Если попытка удалась, модуль переходит в штатный режим работы. 

**Polling**
	
Первые 30 секунд модуль просыпается раз в 3 секунды. По истечению 30 секунд модуль засыпает на 5 минут. Через 5 минут опять просыпается раз в 3 секунды последующие 30 секунд. И опять засыпает на 5 минут. И так по кругу. Можно было сделать просыпание модуля раз в 5 минут, но zigbee2mqtt по умолчанию проверяет устройства на наличие в сети примерно раз в 10 минут. И начинает ругаться в логе, что устройство не найдено и выставляет статус offline. При такой неровной схеме эта проблема устраняется. Это конечно можно настроить в zigbee2mqtt, но я предпочел такой вариант. Сделать же период просыпания всегда раз в 3 секунды - необоснованное расходование ресурса батарейки.

**Reporting**

Модуль высылает четыре разных отчета. Два для батарейки и еще два для счетчика холодной и горячей воды. Период отправки у них разный.

Напряжение на батарейке модуль проверяет один раз в 15 минут. Отчеты о состоянии батарейки (напряжение в мВ и остаточный ресурс в %) высылаются не ранее, чем через 5 минут от предыдущего (если были изменения), но и не позднее 1 часа (даже если не было изменений).

Отчет о значении счетчика воды высылается сразу при увеличении этого значения. И не позднее 5 минут от предыдущего отчета. Если 5 минут кому-то покажется слишком частым, это всегда можно исправить через web-интерфейс zigbee2mqtt в разделе Reporting.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/z2m_reporting.jpg" alt="Reporting"/>

Также принудительно отчеты высылаются в течение 5 секунд, если нажать на кнопку прибора.

Код, примененный в примерах от Telink для работы reporting'а, немного (я бы даже сказал много!) кривой. Он не дает засыпать модулю более, чем на 1 секунду. И если для устройств, которые питаются от стационарных источников, это не критично, то для батареечных устройств это уже катастрофа. Поэтому оригинальный аглоритм reporting'а был заменен на кастомный, который основан на запуске таймеров по MinInterval и MaxInterval, что позволило засыпать модулю ровно на эти интервалы, а не на 1 секунду, как было ранее.

**Сетодиодная индикация режимов модуля**

Если модуль продолжительное время не моргает светодиодом (период более 5 минут), то он находится в режиме глубокого сна. Выйти из этого режима модуль может в двух случаях. Первый - если пользователь нажмет на кнопку. Второй - если сработает (замкнется или разомкнется) геркон в любом счетчике воды.

1. Светодиод с периодичностью от 5 секунд до 5 минут мограет одной вспышкой - модуль находится в сети, работает в штатном режиме.
1. Светодиод с периодичностью 5 секунд моргает двумя вспышками - происходит OTA обновление прошивки.
1. Светодиод с периодичностью от 5 секунд до 5 минут моргает тремя вспышками - модуль не в сети - не был поключен, например вставили батарейки в устройство, но zigbee сети нет или активирован запрет на подключение; или был поключен, но в данный момент какие-то проблемы с сетью. В любом случае в таком режиме модуль проработает примерно 30 минут. Если за это время он не подключится к сети или не восстановит связь, то уйдет в глубокий сон. В этом режиме, чтобы связаться с модулем, нужно его разбудить, нажав на кнопку прибора.

**Память модуля и где хранится конфиг**

Согласно спецификации на чип TLSR8258F512ET32 память распределена следующим образом

		0x00000 Old Firmware bin
		0x34000 NV_1
		0x40000 OTA New bin storage Area
		0x76000 MAC address
		0x77000 C_Cfg_Info
		0x78000 U_Cfg_Info
		0x7A000 NV_2
		0x80000 End Flash

Получается, что прошивка не может быть больше, чем 0x34000 (что собственно и подтверждается, если проверить SDK на предмет определения размера заливаемого файла при обновлении OTA), но при использовании прошивки с адреса 0x40000 видно, что под нее отведено не 0x34000, а 0x36000. ~~Выходит, что 0x2000 никогда не используются. Этим мы и воспользуемся для хранения промежуточного конфига.~~ Промежуточный конфиг записывается в NV_2 (куда-то в область с 0x7a000 по 0x7c000). Используется модуль NV_MODULE_APP с номером NV_ITEM_APP_USER_CFG (для понимания смотрите app_cfg.h и sdk/proj/drivers/drv_nv.h)

После аппаратной заливки прошивки в модуль, он всегда стартует с адреса 0x00000. После обновления OTA, адрес старта меняется. Если до обновления он был 0x00000, то после он становится 0x40000. Если до обновления он был 0x40000, то после - 0x00000. И так по кругу после каждого обновления OTA.

В момент старта модуля происходит проверка, с какого адреса загружается прошивка - с 0x00000 или с 0x40000. Если она грузится с адреса 0x00000, то область с 0x40000 до 0x74000 мы используем для хранения конфига. Если прошивка грузится с адреса 0x40000, то для хранения конфига мы используем уже область с 0x00000 до 0x34000.

Примеры вывода лога устройства после обновления OTA

* Обновили первый раз

		OTA update successful.
		OTA mode enabled. MCU boot from address: 0x40000
		Save restored config to nv_ram in module NV_MODULE_APP (6) item NV_ITEM_APP_USER_CFG (45)
	
* Обновили второй раз

		OTA update successful.
		OTA mode enabled. MCU boot from address: 0x0
		Save restored config to nv_ram in module NV_MODULE_APP (6) item NV_ITEM_APP_USER_CFG (45)

Конфиг пишется в выбранную область каждый раз при срабатывании счетчика воды с шагом 0x100. Т.е. первый раз конфиг запишется по адресу 0x40000 (0x00000), во второй раз 0x40100 (0x00100), в третий - 0x40200 (0x00200) и т.д. пока не достигнет границы 0x74000 (0x34000). И далее начинает опять записываться с начального адреса 0x40000 (0x00000).

* Вывод лога устройства при записи конфига с адреса 0x0

		Save config to flash address - 0x0
		cold counter - 2010
		Save config to flash address - 0x100
		cold counter - 2020
		Save config to flash address - 0x200
		cold counter - 2030
		Save config to flash address - 0x300
		cold counter - 2040
		Save config to flash address - 0x400

* Вывод лога устройства при записи конфига с адреса 0x40000

		Save config to flash address - 0x40000
		cold counter - 2090
		Save config to flash address - 0x40100
		cold counter - 2100
		Save config to flash address - 0x40200
		cold counter - 2110
		Save config to flash address - 0x40300
		cold counter - 2120
		Save config to flash address - 0x40400
		
В момент обновления OTA конфиг сохраняется в nv_ram. И будет там сохраняться, пока обновление OTA удачно не завершится.

После удачного завершения обновления OTA модуль перезагружается, считывает конфиг из nv_ram, проверяет по какому адресу нужно записывать конфиг в штатном режиме и сохраняет его уже по адресу 0x00000 или 0x40000. И так до следующего обновления.

---

**Датчики протечки**

При обнаружении протечки (т.е. контакты любого датчика протечки погружены в воду) меняется состояние кластера IAS и отправляются сразу две команды On в кластеры On_Off в 4 и 5 эндпоинтах. При высыхании воды меняется состояние кластера IAS, никакие команды в кластер On_Off не отсылаются.

Оба кластера On_Off должны напрямую "биндиться" к исполнительному устройству (принудительно настраивается в web-интерфейсе zigbee2mqtt в разделе bind устройсва). Что это дает. Даже при падении сети, когда например координатор не работает, Watermeter все равно пошлет выбранную команду on (off или toggle) напрямую в исполнительное устройство и все сработает (мы же понимаем, что исполнительное устройство в виде привода на кран должно иметь дублирующее автономное питание!).

---

**Zigbee Claster and Attribute**

В прошивке используется 5 endpoint'ов, это как бы 5 разных логических устройства на одном физическом. Для чего. Стандарт Zigbee не позволяет использовать одинаковые кластеры с одинаковыми атрибутами на одном endpoint'е. А у нас два одинаковых счетчика с одинаковыми атрибутами CurrentSummationDelivered в кластере SeMetering. И два одинаковых кластера On_Off. Поэтому в первом enpoint'е счетчик горячей воды, во втором - счетчик холодной, в третьем - настройки, в четвертом - IAS и первый On_Off, в пятом - второй On_Off.

* WATERMETER_ENDPOINT1 содержит все необходимые для работы zigbee базовые кластеры, а также кластер SeMetering (первый счетчик)
* WATERMETER_ENDPOINT2 содержит второй кластер SeMetering (второй счетчик)
* WATERMETER_ENDPOINT3 содержит третий кластер SeMetering - здесь используются кастомные (которых нет в спецификации zcl) атрибуты для первоначальной настройки счетчика.
* WATERMETER_ENDPOINT4 содержит кластер IAS для информирования о протечке, кластер ON_OFF для отправки команды на первый привод крана воды (например горячей) и кластер OnOffSwitchCfg для выбора команды.
* WATERMETER_ENDPOINT5 содержит еще один кластер ON_OFF для отправки команды на второй привод крана воды (например холодной) и кластер OnOffSwitchCfg для выбора команды.

В принципе WATERMETER_ENDPOINT3 можно было не применять, а все поместить в WATERMETER_ENDPOINT2, но сделано так.

---

## <a id="settings">Настройка</a>

Открываем на редактирование файл `configuration.yaml` от zigbee2mqtt. И добавляем в конец файла

		external_converters:
			- watermeter_wleak.js
		ota:
			zigbee_ota_override_index_location: local_ota_index.json
		  
Файлы `watermeter_wleak.js` и `local_ota_index.json` копируем из [папки проекта](https://github.com/slacky1965/watermeter_zed/tree/main/zigbee2mqtt) туда же, где лежит `configuration.yaml` от zigbee2mqtt. Не забываем разрешить подключение новых устройств - `permit_join: true`. Перегружаем zigbee2mqtt. Проверяем его лог, что он запустился и нормально работает.

Далее, вставляем батарейки в устройство. Если питание было уже подано, то нажимаем 5 раз подряд кнопку. Устройство должно подключиться к сети zigbee. Если подключение прошло удачно, то мы обнаружим наше устройство в zigbee2mqtt.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/z2m_device.jpg" alt="Watermeter Device"/>

После того, как устройство подключилось к сети и zigbee2mqtt его обнаружил, можно приступить к заданию начальных значений счетчиков. Для конфигурирования начальных значений были созданы три кастомных атрибута в кластере Smart Energy. Это задание первоначальных значений для горячей воды, холодной и сколько прибавлять литров на один импульс (разные счетчики могут на один импульс прибавлять от 1 литра до 10, смотрите спецификацию на ваш счетчик). 

Нужно перейти в web-интерфейс zigbee2mqtt и зайти в раздел exposes. Задать первоначальные значения.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/exposes.jpg" alt="z2m exposes"/>

Затем нажать кнопку на устройстве, чтобы оно проснулось и приняло данные. После этого в этот раздел лучше больше не заходить, потому что если вы щелкните мышью по какому-то полю настройки, то zigbee2mqtt сразу отправит то значение, которое там отмечено. К сожалению я не нашел, как можно сделать через кнопку подтверждения в web-интерфейсе.

**Датчики протечки**

Заходим в раздел exposes устройства watermeter.

Выбираем, какие команды отправлять при обнаружении протечки.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/z2m_onoffSwitchCfg.jpg" alt="Watermeter Switch Action"/>

* Если выбрать on_off, то, при обнаружении протечки, устройство пошлет команду off.
* Если выбрать off_on, то, при обнаружении протечки, устройство пошлет команду on.
* Если выбрать toggle, то, при обнаружении протечки, устройство пошлет команду toggle.

Далее заходим в раздел bind.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/z2m_bind1.jpg" alt="Watermeter bind1"/>

И добавляем новые "биндинги" для endpoint 4 и endpoint 5.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/z2m_bind2.jpg" alt="Watermeter bind2"/>

Перед тем, как использовать в реальном режиме, нужно пару раз дать "ложную" протечку, т.е. закоротить поочередно датчики протечки и убедиться, что все работает!!!

И еще, 100% гарантию может дать только Бог, помните это :))

---

## OTA

Автоматического обновления в zigbee2mqtt для устройств, добавленных через конвертор, нет. Поэтому, если вышла новая версия, скачиваем обновленный файл прошивки для обновления OTA, например `6565-0204-13043001-watermeter_zed.zigbee`. Переименовываем его в просто `watermeter_zed.zigbee` и кладем его по относительному пути `zigbee2mgtt/images`. Перегружаем zigbee2mqtt. Идем во вкладку OTA. И кликаем на `Check for new updates`

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ota_check_update.jpg" alt="Check for new updates"/>

Если обновление принимается, то кнопка `Check for new updates` станет красной с надписью `Update device firmware`. 

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ota_update.jpg" alt="Update device firmware"/>

Ее нужно кликнуть и обновление начнет загружаться (zigbee обновляется долго, что-то в районе 20 минут). 

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ota_progress.jpg" alt="Check for new updates"/>

Если обновление завершится с ошибкой, то кнопка обновления опять станет красной и ее нужно опять нажать и разбудить модуль нажатием кнопки. Процесс обновления обнулится и пойдет с самого начала.

В SDK нет проверки на разряд батарейки перед загрузкой образа. Поэтому пришлось писать свою реализацию. Устройство вернет координатору ошибку - `Aborted by device`, если заряд батарейки меньше 50%, и файл образа загружаться не будет. 

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ota_update_abort.jpg" alt="Aborted by device"/>

Ну и последнее, чтобы устройство не обновилось какой-нибудь прошивкой с MANUFACTURER_CODE от Telink (например, совпадет номер устройства), MANUFACTURER_CODE заменен на кастомный.

---

## Потребление

Долгих испытаний в реальной работе пока не проводилось. С помощью ppk2 произведены замеры потребления в различных режимах.

**Устройтсво не в сети, без питания. Подаем питание, оно стратует и подключается к сети.**

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ppk2-start_device.jpg" alt="Start new device"/>

**Устройство работает в штатном режиме с POLL RATE 3 секунды.**

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ppk2-poll_rate_3_sec.jpg" alt="Poll rate 3 sec."/>

**Замкнулся геркон на счетчике воды.**

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ppk2-close_counter.jpg" alt="Counter close"/>

**Разомкнулся геркон на счетчике воды.**

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ppk2-open_counter.jpg" alt="Counter open"/>

**Старт модуля, который уже подключен к сети и работа его в течение 6 минут без срабатывания счетчиков.**

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ppk2-start_and_work_6min.jpg" alt="Work 6 min."/>

**Модуль спит.**

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ppk2-sleep_device.jpg" alt="Sleep device"/>

**Обновление OTA.**

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/ppk2-ota_update.jpg" alt="OTA Update."/>

---

## Home Assistant

В Home Assistant счетчик будет выглядеть так.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/HA_device.jpg" alt="HA begin"/>

К сожалению я не знаю, как автоматически отключить пресеты в Control. Поэтому заходим в каждый и отключаем в ручную, чтобы не мешали. 

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/HA_preset_disabled.jpg" alt="HA preset disabled"/>

Получаем такую картинку.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_zed/main/doc/images/HA_all_preset_disabled.jpg" alt="HA all preset disabled"/>

Далее кастомизируем счетчики, если нужно. 

---

## Стоимость

Стоимость основных деталей на февраль 2024 года в России.

* Плата 10 шт. - 1500 р. (Aliexpress)
* Модуль E-BYTE E180-Z5812SP - 300 р. (Aliexpress)
* Battery Holder AA - 42 р. x 2 (Aliexpress)
* Клемник - 49 р. (Aliexpress)
* Кнопка - 3 р. (Aliexpress)
* Светодиод - 12 р. (Chipdip)
* Пара конденсаторов и сопротивление - 200 р.

## TODO
- ~~Сделать светодиодную индикацию режимов работы модуля.~~ Сделано.
- ~~Сделать DEEP SLEEP, если модуль не в сети более 30 минут.~~ Сделано.
- ~~Сделать сброс OTA, если обновление завершилось ошибкой, чтобы не зависеть от действий пользователя.~~ Сделано.
- ~~Добавить два канала для 2-х датчиков протечки.~~ Сделано.

## История версий

- 1.0    - Начало.
- 1.1    - Прошивка без ОТА. 
- 1.2    - Добавлена возможность обновления OTA.
- 1.3.01 - Изменена нумерация версий. Добавлены светодиодная индикация режимов модуля; глубокий сон, если не в сети более 30 минут; сброс OTA, если обновление прошло с ошибкой, обновление начнется с начала, сделано чтобы не потерять основной конфиг.
- 1.3.02 - Изменен адрес записи промежуточного конфига при OTA. Раньше он записывался по адресу 0x74000, теперь он пишется в nv_ram в модуль NV_MODULE_APP с номером NV_ITEM_APP_USER_CFG (см. app_cfg.h).
- 1.3.03 - Устранен небольшой глюк с репортингом.
- 1.3.04 - Изменен MANUFACTURER_CODE на кастомный. Добавлена проверка разряда батарейки при обновлении OTA.
- 1.3.05 - Устранена проблема с factory reset.
- 1.3.06 - Добавлена возможность чтения кастомных атрибутов (нужно для HOMEd).
- 1.3.07 - Устранен баг со считыванием кастомного атрибута ZCL_ATTRID_CUSTOM_WATER_STEP_PRESET.
- 2.0.01 - Добавлены два датчика протечки. Изменена плата и корпус.
- 2.0.02 - Устранен баг с репортингом, когда maxInterval равен 0.
- 2.0.03 - Добавлена конфигурация команд OnOff. Теперь при обнаружениии протечки устройство может послать on, off или toggle. Обновлен конвертор для zigbee2mqtt.

