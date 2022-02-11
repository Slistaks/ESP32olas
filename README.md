# Medicion de nivel de agua


## Overview



## How to use example



### Hardware Required

To run this proyect it's needed:
  -("[borrar]poner nombre del sensor como FDX-Sensor / FDX-waterLevelSensor o etc").
  -[borrar]seguir poniendo lo que necesita, como el monitor, el sistema de control, el panel/tablero, y todo eso
  -[borrar]etc

#### Pin Assignment(esp32):

**Note:** The

|                           | SDA    | SCL    |
| ------------------------- | ------ | ------ |
| ESP32/ESP32-S2 I2C Master | GPIO21 | GPIO 22|		// [borrar]corroborar los pines
| ------------------------- | ------ | ------ |

| ESP32 Sync input          |     GPIO5       |
| ESP32 packets sync pulse  |     GPIO4       |		// [borrar]documentar que es un pin para debug, para corroborar sincronismo.
							// [borrar]al inicio de cada paquete invierte el estado del gpio. (corroborar si no recuerdo mal).
- Connection:	[borrar] no creo que haga falta, tiene un solo conector el sensor..
  - 
  - 
  - 


### Configure the project SEGUIR REVISANDO HACIA ABAJO

Open the project configuration menu (`idf.py menuconfig`). Then go into `Example Configuration` menu.

- In the `I2C Master` submenu, you can set the pin number of SDA/SCL according to your board. Also you can modify the I2C port number and freauency of the master.
- .
- In the


### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

#MODIFICAR TODO LO DE ABAJO

```bash
I (6495) i2c-example: TASK[1] test cnt: 1
*******************
TASK[1]  MASTER READ SENSOR( BH1750 )
*******************
data_h: 01
data_l: d0
sensor val: 386.67 [Lux]
I (6695) i2c-example: TASK[0] test cnt: 2
*******************
TASK[0]  MASTER READ SENSOR( BH1750 )
*******************
data_h: 01
data_l: d0
sensor val: 386.67 [Lux]
*******************
TASK[0]  MASTER READ FROM SLAVE
*******************
====TASK[0] Slave buffer data ====
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 
20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 
30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 
40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 
50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 
60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 
70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 

====TASK[0] Master read ====
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 
20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 
30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 
40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 
50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 
60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 
70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 

*******************
TASK[1]  MASTER READ FROM SLAVE
*******************
====TASK[1] Slave buffer data ====
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 
20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 
30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 
40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 
50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 
60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 
70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 

====TASK[1] Master read ====
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 
20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 
30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 
40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 
50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 
60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 
70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 

*******************
TASK[0]  MASTER WRITE TO SLAVE
*******************
----TASK[0] Master write ----
0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 
1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 
2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 
3a 3b 3c 3d 3e 3f 40 41 42 43 44 45 46 47 48 49 
4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 
5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 
6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 
7a 7b 7c 7d 7e 7f 80 81 82 83 84 85 86 87 88 89 

----TASK[0] Slave read: [128] bytes ----
0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 
1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 
2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 
3a 3b 3c 3d 3e 3f 40 41 42 43 44 45 46 47 48 49 
4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 
5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 
6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 
7a 7b 7c 7d 7e 7f 80 81 82 83 84 85 86 87 88 89 
```

## Troubleshooting

- 

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
