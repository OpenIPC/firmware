[Nabto Edge Embedded SDK](https://github.com/nabto/nabto-embedded-sdk)

The package includes the tcp_tunnel_device assembly from sdk. The example shows how you can access an rtsp stream from anywhere in the world through the [Nabto](https://www.nabto.com/) platform.

###### Registration

To configure the camera, you will need to register in the [Nabto Cloud Console](https://console.cloud.nabto.com) and get the `Product-ID` (pr-xxxxxxxx) and `Deviсe-ID` (de-xxxxxxxx).
for each device to be connected, the `fingerprint` for completion will be received only after the camera has been configured.

###### Camera setup

Start `tcp_tunnel_device` with key `--demo-init` and enter data (pr-xxxxxxxx & de-xxxxxxxx) obtained during registration in [Nabto Cloud Console](https://console.cloud.nabto.com) and creation of device.

Then select the proxied service, in this case `rtsp` and specify endpoint `/stream=0`:

```
root@openipc-t31:# tcp_tunnel_device --demo-init

No device configuration found. Creating configuration: /home/user/.nabto/edge/config/device.json.
The device configuration requires a Product ID and a Device ID, created in the Nabto Cloud Console.
Product Id: pr-ydk3xhyn
Device Id: de-orruyc4n


Demo initialization will make a simple IAM setup, be aware that this is not what you want in production.
'Local Open Pairing' and 'Password Open Pairing' are enabled. Newly paired users get the 'Administrator' role.

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

When finished, a `fingerprint` will be issued, which must be saved in the device settings in the cloud.

```
The configuration and state has been initialized
The Fingerprint must be configured for this device in the Nabto Cloud Console before it will be allowed to attach to the Basestation. If you want to reuse an already configured fingerprint, you can copy the corresponding private key to /home/user/.nabto/edge/keys/device.key
The device Fingerprint is: 3de2ec6ded2cc975bf87ed63b2303e02b8051fb7359b19a3d950ed0158bb3813
```

###### Launch

```
tcp_tunnel_device
```

Make sure that the connection was successful (example):

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

###### Retrieving a stream

In the example we will fetch the thread on the linux machine, but the releases on [github](https://github.com/nabto/nabto-client-edge-tunnel) have a windows version of the client.

Clone and build the client (downloaded from releases).

There are 2 ways of "pairing" - `Local Open` and `Password Open`. The first mode allows you to easily find and bind the device if it is in the same local network, the second option if the device is remote...
As a universal example, we will use the variant with a remote device.

Run `edge_tunnel_client` with the key `--pair-string` and a string with the necessary device credentials `p=pr-ydk3xhyn,d=de-orruyc4n,pwd=X9NphkArpzLU,sct=9jLgbUb4FWhe`.
The string is already present in the output of the camera connected to the cloud, so just copy it from the previous step of the camera setup and select `0` - Password Open, when asked for New username we specify `admin`:

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

The configuration is complete. Launch the client and check the list of services provided by the camera:

```
$ ./edge_tunnel_client --services
Connected to the device [0] pr-ydk3xhyn.de-orruyc4n
Available services ...
Service: rtsp       Type: rtsp       Host: 127.0.0.1  Port: 554
```

In this example, the camera only provides access to the rtsp stream. Connecting:

```
./edge_tunnel_client --service rtsp
Connected to the device [0] pr-ydk3xhyn.de-orruyc4n
TCP Tunnel opened for the service rtsp listening on the local port 44391
```

At `127.0.0.1:44391` an rtsp stream will be available, use ffplay making sure to select the tcp transport:

```
ffplay -rtsp_transport tcp rtsp://127.0.0.1:44391/stream=0
```

For more details on platform features and settings, see the official [documentation](https://docs.nabto.com/developer/guides.html)

