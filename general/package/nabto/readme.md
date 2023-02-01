[Nabto Edge Embedded SDK](https://github.com/nabto/nabto-embedded-sdk)

Пакет включает сборку tcp_tunnel_device из sdk. В примере показано как можно получить доступ к rtsp потоку из любой точки мира посредством платформы [Nabto](https://www.nabto.com/).


###### Регистрация

Для настройки камеры понадобится регистрация в облаке [Nabto Cloud Console](https://console.cloud.nabto.com) и получение `Product-ID` (pr-xxxxxxxx) и `Deviсe-ID` (de-xxxxxxxx) для каждого подключаемого устройства, `fingerprint` для завершения будет получен только после настройки камеры.

###### Настройка камеры

Запустить в консоли `tcp_tunnel_device` с ключом `--demo-init` и ввести данные (pr-xxxxxxxx & de-xxxxxxxx) полученные при регистрации в облаке [Nabto Cloud Console](https://console.cloud.nabto.com) и создании устройства.

Далее выбрать проксируемый сервис, в данном случае `rtsp` и указать endpoint `/stream=0`:

```
Next step is to add TCP tunnel services.
What type of service do you want to add?
[0]: continue
[1]: ssh
[2]: http
[3]: rtsp
Enter a valid number (default: 1) [0-3]: 3
Enter the port of your RTSP server (default: 8554) [0-65535]: 554
Enter your RTSP endpoint (default: /video): /stream=0
Added rtsp service on localhost port 554 with metadata rtsp-path => /stream=0
```

По окончании будет выдан `fingerprint`, который необходимо сохранить в настройках устройства в облаке.

###### Запуск
```
tcp_tunnel_device
```
убедиться что подключение прошло успешно (пример):

```
root@openipc-t31:# tcp_tunnel_device 

######## Nabto TCP Tunnel Device ########
# Product ID:        pr-ydk3xhyn
# Device ID:         de-orruyc4n
# Fingerprint:       3de2ec6ded2cc975bf87ed63b2303e02b8051fb7359b19a3d950ed0158bb3813
# Version:           0.0.0-branch.heads/master.commits.1910+a49d27fc.dirty
# Local UDP Port:    5592
# Friendly Name:     "Tcp Tunnel"
# 
# The device offers Local Open Pairing
# 
# The device has Password Open Pairing enabled
# Open Pairing Password:  X9NphkArpzLU
# Open Pairing SCT:       9jLgbUb4FWhe
# Open Pairing String:    p=pr-ydk3xhyn,d=de-orruyc4n,pwd=X9NphkArpzLU,sct=9jLgbUb4FWhe
# 
######## Configured TCP Services ########
# Id               Type             Host             Port             
# rtsp             rtsp             127.0.0.1        554              
########

Attached to the basestation
```

###### Получение потока

В примере будем забирать поток на linux машине, но в релизах на [github](https://github.com/nabto/nabto-client-edge-tunnel) есть версия клиента под windows.

Клонируем и собираем клиента (качаем из релизов).

Есть 2 способа "спаривания" - `Local Open` и `Password Open`. Первый режим позволяет легко найти и привязать устройство, если оно находится в одной локальной сети, второй вариант если устройство удаленное. 
В качестве универсального примера используем вариант с удаленным устройством.

Запускаем `edge_tunnel_client` с ключом `--pair-string` и строкой с необходимыми учетными данными устройства `p=pr-ydk3xhyn,d=de-orruyc4n,pwd=X9NphkArpzLU,sct=9jLgbUb4FWhe`. 
Строка в готовом виде присутствует в выводе подключенной к облаку камеры, потому просто копируем ее из предыдущего шага настройки камеры и выбираем `(0) Open Password`, при запросе username укажем `admin`:

```
$./edge_tunnel_client --pair-string p=pr-ydk3xhyn,d=de-orruyc4n,pwd=X9NphkArpzLU,sct=9jLgbUb4FWhe

Connected to device ProductId: pr-ydk3xhyn DeviceId: de-orruyc4n
Several pairing modes exists choose one of the following.
[0]: Password Open
[1]: Local Open
Choose a pairing mode: 0
Open Password Pairing requires a username. The username is a name you choose for the new user, the username has to be unique among the registered users on the device.
New Username: admin
The device [0] pr-ydk3xhyn.de-orruyc4n has been set into the bookmarks as index 0

```
Настройка завершена. Запускаем клиент и проверяем список предоставляемых камерой сервисов:

```
$ ./edge_tunnel_client --services
Connected to the device [0] pr-ydk3xhyn.de-orruyc4n
Available services ...
Service: rtsp       Type: rtsp       Host: 127.0.0.1  Port: 554
```

В данном примере камера предоставляет доступ только к rtsp потоку. Подключаемся:

```
./edge_tunnel_client --service rtsp
Connected to the device [0] pr-ydk3xhyn.de-orruyc4n
TCP Tunnel opened for the service rtsp listening on the local port 44391
```

На `127.0.0.1:44391` будет доступен rtsp поток, используем ffplay обязательно выбрав транспорт tcp:

```
ffplay -rtsp_transport tcp rtsp://127.0.0.1:44391/stream=0
```

Более подробно о возможностях платформы и настройках в официальной [документации](https://docs.nabto.com/developer/guides.html)

