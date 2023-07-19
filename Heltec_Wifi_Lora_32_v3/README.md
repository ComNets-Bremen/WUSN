# Wireless Underground Sensor Network for Precision Agriculture

This project aims to develop a wireless underground sensor network for precision agriculture. The network consists of three wireless sensor nodes that are installed underground, along with a gateway node located aboveground. The wireless sensor nodes communicate with the gateway node using LORA (Long Range) technology at either 434MHz or 868MHz frequency. These sensor nodes are designed to measure water quality, temperature, and NPK values in the soil they are buried in.

The sensor nodes operate on [Molenet](https://blogs.uni-bremen.de/molenet/) v5 and the gateway nodes operate using the [Heltec Wifi Lora 32 v3](https://heltec.org/project/wifi-lora-32-v3/).

## Features

1. **Wireless Sensor Nodes**: The project utilizes three wireless sensor nodes that are buried underground. These nodes are equipped with sensors to measure water quality, temperature, and NPK values in the soil.

2. **Gateway Node**: The gateway node is placed aboveground and serves as the central hub for the wireless sensor network. It receives data from the sensor nodes and facilitates communication with external devices.

3. **LORA Communication**: The wireless sensor nodes communicate with the gateway node using LORA technology. LORA enables long-range communication with low power consumption, making it ideal for underground deployments.

4. **Mobile Phone Connectivity**: The gateway node connects to a mobile phone using its soft access point feature. This allows the mobile phone to establish a connection with the wireless sensor network and access the data collected by the sensor nodes.

5. **Cloud Database**: The gateway node connects to a cloud database, where it stores the data collected from the sensor nodes. This enables remote access to the sensor data for analysis and decision-making.

6. **Mobile App with Machine Learning**: The mobile phone is equipped with an application specifically designed for this project. The app facilitates connectivity between the devices and the cloud database, allowing users to access and analyze the sensor data. Additionally, the app includes a built-in machine learning module.

7. **Machine Learning for Signal Optimization**: The machine learning module in the mobile app analyzes the relationship between soil moisture and signal quality. It uses this information to train and improve the transmission efficiency of the sensor nodes. By optimizing the signal quality, the network can operate more efficiently and provide more accurate data.

## Contributors

This project was developed by General Sir John Kotelawala Defence University, Sri Lanka and University of Bremen, Germany