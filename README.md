# COMP3000-Automatic-Vibration-Alarm-Adjustments

## Building

All dockerised, running docker compose up -d in the root directory should get the system going.
Machine Learning Server not included, this requires running on a linux instance with CUDA installed and an NVIDA CUDA GPU available.

## Components

Each component is a separate git submodule, and compiles to a docker container. Orchestrated through docker compose.

### State Detector

C# application that handles detection of the current operating state of the machine by maintaining and interpretting the machine learning model.

### Machine Learning Server

CUDA C/C++ application providing GPU-accelerated SOM calculations, called by the state detector. This is the only component that is not a docker container, as it is deployed on a different machine to the rest of the system.

### Alarming

Rust application providing all logic for detecting whether the machine is in an alarm condition, which is fed back into the database
Based on data in the database and the output of the state detector

### API 

JavaScript application that just provides a RESTful API for the database, for use by the web app

### Web Application

Typescript web application that provides a very simple GUI to view system state and configure the alarms/states.

### Database

Database set up scripts, to configure a TimeScale database used by the other components

