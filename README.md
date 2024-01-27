# ESP32_HTTP_OTA

This project enables an ESP32 device to perform Over-The-Air (OTA) firmware updates by pulling the firmware from an HTTP server.

## Running the HTTP Server

To start a simple HTTP server that will serve files from the current directory, execute the following Python command in your terminal:

```bash
python -m http.server <server_port>
```

Replace `<server_port>` with the desired port number on which you want the server to run.

For example, starting the server on port `8000` would use the command:

```bash
python -m http.server 8000
```

After running the command, you should see output similar to the following, which indicates that the server is active:

```
Serving HTTP on :: port 8000 (http://[::]:8000/) ...
```

## Configuring File Paths

Ensure that the firmware binary and the version header file are placed correctly on the server. Specify the paths in the ESP32 application code as shown below:

```c
const char* firmware_url = "http://<server_IP>:<PORT_NO>/firmware.bin";
const char* firmware_ver_url = "http://<server_IP>:<PORT_NO>/src/firmware_ver.h";
```

> [!IMPORTANT]
> Make sure to keep the fw_version.h file.


### Example

If `firmware.bin` is located at `C:/esp_firmware/firmware.bin` and `firmware_ver.h` is at `C:/esp_firmware/src/firmware_ver.h`, and you run the HTTP server from the `C:/esp_firmware/` directory, the definitions in your ESP32 firmware should look like this:

```c
const char* firmware_url = "http://<server_IP>:8000/firmware.bin";
const char* firmware_ver_url = "http://<server_IP>:8000/src/firmware_ver.h";
```

Make sure to replace `<server_IP>` with the IP address of your server and `8000` with the port number used when starting the HTTP server.

> [!TIP]
> Port forwarding can allow you to download firmware over the internet. However, ensure you understand the security implications and take appropriate measures to secure the update mechanism.
