# ComfyNet: Embedded Network for Thermal Comfort Research

## Overview

ComfyNet is a wireless mesh network of ESP32-based embedded sensor nodes designed to measure environmental factors crucial for thermal comfort research in educational spaces. This project was developed as part of a Performance Evaluation in Built Environment course at Fulbright University Vietnam to investigate thermal comfort conditions in campus meeting rooms.

The system enables automated collection of air velocity, temperature, humidity, and pressure data, providing valuable insights into real-world thermal conditions and their impact on occupant comfort.

## Project Background

The research addresses critical challenges in educational building management:
- Centralized air conditioning (CAC) systems that cannot be adjusted for specific rooms
- Lack of standardized platforms for benchmarking HVAC system performance
- Student complaints regarding thermal discomfort in Fulbright University Vietnam's meeting rooms
- Need for data-driven dialogue with building management about HVAC optimization

## System Architecture

### Hardware Components

- **ESP32 Microcontroller Units (MCUs)**: Primary processing and wireless communication
- **BME680 Sensors**: Measure air temperature, relative humidity, and air pressure
- **Tenmars TM-288 Heat Stress WBGT Meter**: Measure average radiant temperature (updates every 20 seconds)
- **UNI-T UT363BT Digital Anemometer**: Measure air velocity (0.4 m/s resolution)
- **Raspberry Pi 4 Model B**: Central data storage and analysis hub

### Network Topology

The system implements a mesh network architecture with:
- **Client Nodes**: ESP32 devices with attached sensors
  - Temperature nodes (`temp0`, `temp1`): BME680 + NTC thermistor
  - Wind nodes (`wind1`, `wind2`): BME680 + digital anemometer
  - WBGT node: Modified heat stress meter for radiant temperature
- **Station Node**: Central ESP32 acting as WiFi access point and data aggregator
- **Data Logger**: Raspberry Pi 4 running Python logger script for data persistence

### Data Flow

1. **Sensor Reading**: Client nodes read environmental data every 500ms
2. **Data Transmission**: Sensor data transmitted via TCP over local WiFi network
3. **Data Aggregation**: Station node receives and forwards data packets
4. **Data Logging**: Python logger script processes and stores data in CSV format
5. **Analysis**: Data exported for thermal comfort calculations and visualization

## Key Features

### Automated Data Collection
- Continuous monitoring of four thermal comfort factors
- Real-time noise filtering and data validation
- Timestamp-synchronized data logging

### Thermal Comfort Analysis
- PMV (Predicted Mean Vote) calculations using pythermalcomfort library
- Integration with survey data for AMV (Adaptive Mean Vote) comparison
- ASHRAE Standard 55-2023 compliance verification
- CBE Thermal Comfort Tool integration

### Research Capabilities
- 24-hour continuous monitoring across multiple rooms
- Correlation analysis between environmental factors
- Room size impact assessment on thermal comfort
- Simulation data validation through field measurements

## Installation and Setup

### Hardware Setup

1. **Sensor Node Configuration**:
   - Flash appropriate firmware to ESP32 devices using PlatformIO
   - Connect BME680 sensors via I2C interface
   - Modify WBGT meter for automated data extraction
   - Configure digital anemometer for continuous operation

2. **Network Configuration**:
   - Set up WiFi credentials in `platformio.ini`
   - Deploy station node as access point (SSID: "PerfEvalLogger")
   - Position nodes at strategic locations:
     - Center of room (1.2-1.5m height) for WBGT measurement
     - Air conditioning outlet for supply air monitoring
     - Common seating areas for occupant-level measurements

3. **Data Logger Setup**:
   - Install Python dependencies: `pyserial`
   - Configure serial port in `logger_v0.2.py`
   - Run logger script on Raspberry Pi connected to station node

### Software Dependencies

#### PlatformIO Libraries
```ini
lib_deps = 
    adafruit/Adafruit BME680 Library@^2.0.4
    adafruit/Adafruit Unified Sensor@^1.1.14
```

#### Python Requirements
```
pyserial
time
```

## Usage

### Starting Data Collection

1. **Power on hardware**:
   ```bash
   # Station node starts WiFi AP automatically
   # Client nodes connect and begin data transmission
   ```

2. **Start data logging**:
   ```bash
   python logger_v0.2.py
   ```

3. **Monitor real-time data**:
   - Serial output shows parsed sensor readings
   - CSV file created with timestamp for data persistence

### Data Format

The system logs data in CSV format with the following structure:
```
Date,Time,uid,TempNTC,WindSpeed,Temperature,Humidity,Pressure,Gas
2024-03-30,21:34:38,wind0,23.28,0.0,29.09,65.1,1014.11,75.28
2024-03-30,21:34:39,temp0,23.32,0.0,29.09,65.1,1014.11,75.28
```

## Research Findings

### Key Results

1. **Room Size Impact**: Smaller meeting rooms (MR3: 13m³, MR4: 14m³) consistently showed cooler temperatures and PMV values outside ASHRAE acceptable ranges compared to larger rooms.

2. **Air Velocity Correlation**: Active supply air velocity directly correlates with maintaining cooler environments, with PMV values ranging from -1 to -2 (slightly cool to cool).

3. **ASHRAE Compliance**: Only 2 out of 4 meeting rooms (MR2 and MR5) met ASHRAE Standard 55-2023 requirements for thermal comfort.

4. **Field vs. Simulation**: Field measurements yielded PMV values closer to occupant AMV ratings compared to CFD simulation results, validating the importance of real-world data collection.

### Performance Metrics

- **Measurement Accuracy**: Custom calibration algorithms for sensor drift compensation
- **Data Coverage**: 24-hour continuous monitoring across 4 meeting rooms
- **Survey Integration**: 31 occupant responses for PMV vs. AMV comparison
- **Temporal Resolution**: 0.5-second sensor readings with intelligent filtering

## Technical Specifications

### Sensor Specifications
- **Temperature Range**: 20°C to 30°C (typical classroom conditions)
- **Humidity Range**: 40% to 80% RH
- **Air Velocity**: 0.4 m/s minimum detectable (anemometer limitation)
- **Pressure Accuracy**: ±1 hPa

### Power Requirements
- **ESP32 Nodes**: 5V adapter or USB power
- **WBGT Meter**: Internal battery
- **Digital Anemometer**: Internal battery with auto-wake modification
- **Raspberry Pi**: 5V adapter

### Communication Protocol
- **Network**: 802.11 WiFi (2.4 GHz)
- **Data Protocol**: TCP over IP
- **Data Rate**: 2 Hz sensor updates
- **Range**: Typical indoor WiFi coverage

## Limitations and Considerations

### Current Limitations

1. **Spatial Coverage**: Limited to 2 sensor positions per room
2. **Calibration Reference**: No direct access to building HVAC sensors for validation
3. **Survey Subjectivity**: AMV data dependent on occupant reporting accuracy
4. **Temporal Constraints**: Single 24-hour measurement period per room

### Future Improvements

1. **Enhanced Coverage**: Deploy additional sensors at multiple seating locations
2. **Extended Monitoring**: Multi-day data collection for pattern analysis
3. **Advanced Analytics**: Machine learning integration for predictive comfort modeling
4. **Building Integration**: Direct HVAC system interface for real-time control feedback

## Research Applications

### Educational Building Management
- Data-driven HVAC optimization
- Energy efficiency improvements
- Occupant comfort enhancement
- Building performance benchmarking

### Thermal Comfort Research
- Field validation of simulation models
- Regional climate adaptation studies
- Occupant behavior analysis
- Standard compliance verification

### Smart Building Integration
- IoT sensor network deployment
- Real-time environmental monitoring
- Automated comfort control systems
- Predictive maintenance insights

## Documentation

- **Project Report**: `project-report.pdf` - Complete research methodology and findings
- **Project Poster**: `project-poster.pdf` - Visual summary of results
- **Hardware Schematics**: Embedded in source code comments
- **Calibration Data**: WBGT sensor linear regression analysis included

## Project Update

For questions about this project or collaboration opportunities, please refer to the teammate's repository for the most updated information: https://github.com/JINODK/ComfyNet/tree/master
