# FMI4cpp (work in progress)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/NTNU-IHB/FMU-proxy/issues)
[![Join the chat at https://gitter.im/NTNU-IHB/FMI4cpp](https://badges.gitter.im/NTNU-IHB/FMI4cpp.svg)](https://gitter.im/NTNU-IHB/FMI4cpp?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)


[![CircleCI](https://circleci.com/gh/NTNU-IHB/FMI4cpp/tree/master.svg?style=svg)](https://circleci.com/gh/NTNU-IHB/FMI4cpp/tree/master)


FMI4cpp is a cross-platform [FMI](https://fmi-standard.org/) 2.0 implementation written in modern C++.

Influenced by it's spiritual brother [FMI4j](https://github.com/SFI-Mechatronics/FMI4j), it aims to be
an easy to install, easy to use, object oriented and fast FMI implementation for C++.    

FMI4cpp supports both **Co-simulation** and **Model Exchange**. <br/>
For Model Exchange, solvers from [odeint](http://headmyshoulder.github.io/odeint-v2/doc/boost_numeric_odeint/getting_started/overview.html) can be used. 

### Why should I use this over other C/C++ FMI Libraries

Because this library provides a clean, easy to use API and is easy to install (through vcpkg).


### How do I use it in my own project?

Recommended way is to install [vcpkg](https://github.com/Microsoft/vcpkg) and run:

```bash
./vcpkg install fmi4cpp
``` 

The alternative is to fetch the dependencies manually, put them in the path somewhere and run the regular CMake install procedures. 


### Development dependencies

Install [vcpkg](https://github.com/Microsoft/vcpkg) and run:

```bash
./vcpkg install boost-property-tree boost-ublas boost-odeint libzip spdlog
``` 

Then tell CMake about your vcpkg installation by passing <br> ```-DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake``` to it. 

Note that ```boost-ublas``` and ```boost-odeint``` are optional and can be omitted by passing ```-DFMI4CPP_WITH_ODEINT=OFF``` to CMake.

On *NIX the dependencies can also be installed using the native package handler. 

```bash
Linux:~/$ sudo apt-get install libzip-dev libboost-dev libspdlog-dev
``` 

### API

```cpp

#include <fmi4cpp/fmi2/fmi2.hpp>
#include <fmi4cpp/common/logger.hpp>

using namespace fmi4cpp::fmi2;

namespace logger = fmi4cpp::logger;

const double stop = 10.0;
const double stepSize = 1.0/100;

int main() {

    fmi2Fmu fmu("path/to/fmu.fmu");
    
    auto cs_fmu = fmu.asCoSimulationFmu();
    auto me_fmu = fmu.asModelExchangeFmu();
    
    auto cs_md = fmu->getModelDescription(); //smart pointer to a CoSimulationModelDescription instance
    logger::info("modelIdentifier={}", cs_fmu->getModelDescription()->modelIdentifier());
    
    auto me_md = fmu->getModelDescription(); //smart pointer to a ModelExchangeModelDescription instance
    logger::info("modelIdentifier={}", me_fmu->getModelDescription()->modelIdentifier());
    
    auto var = cs_md->getVariableByName("my_var").asRealVariable();
    logger::info("Name={}, start={}", var.name(), var.start().value_or(0));
              
    auto slave = cs_fmu->newInstance();
    
    // or 
    // auto solver = make_solver<RK4ClassicSolver>(1E-3);
    // auto slave = me_fmu->newInstance(solver);
         
    slave->setupExperiment();
    slave->enterInitializationMode();
    slave->exitInitializationMode();
    
    double t;
    double value;
    while ( (t = slave->getSimulationTime()) <= stop) {

        if (!slave->doStep(stepSize)) {
            logger::error("Error! doStep() returned with status: {}", to_string(slave->getLastStatus()));
            break;
        }
        
        if (!var.read(*slave, value)) {
            logger::error("Error! readReal() returned with status: {}", to_string(slave->getLastStatus()));
            break;
        }
        logger::info("t={}, {}={}", t, var.name(), value);
     
    }
    
    slave->terminate();
    
}
```


### fmu_driver

FMI4cpp comes with a simple CLI for testing FMUs, called _fmu_driver_. 

```bash
Options:
  -h [ --help ]          Print this help message and quits.
  -f [ --fmu ] arg       Path to FMU.
  -o [ --output ] arg    Where to store the generated CSV results.
  --start arg            Start time.
  --stop arg             Stop time.
  --stepsize arg         StepSize.
  --me                   Treat FMU as an Model Exchange FMU.
  -v [ --variables ] arg Variables to print.
```

It's not built by default. Pass ```FMI4CPP_BUILD_TOOL=ON``` to build it. 
An additional dependency is needed if you do:

```bash
./vcpkg boost-program-options
```
```bash
Linux:~/$ sudo apt-get install libboost-program-options-dev
```

***

#### Running examples/tests

The examples are built by default. To change this pass ```-DFMI4CPP_BUILD_EXAMPLES=OFF``` to CMake.
Test are on the other hand _not_ built by default. To change this pass ```-DFMI4CPP_BUILD_TESTS=ON``` to CMake.

To run the tests you will need an additional dependency:

```
./vcpkg install boost-test
``` 
```bash
Linux:~/$ sudo apt-get install libboost-test-dev
```
