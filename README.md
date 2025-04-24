# zaplet

---

![C++](https://img.shields.io/badge/c++-%2300599C.svg?&logo=c%2B%2B&logoColor=white)
![Release](https://img.shields.io/badge/release-v1.0.2-blue)
[![GitHub license](https://img.shields.io/github/license/Naereen/StrapDown.js.svg)](https://github.com/Gliptodont/zaplet/blob/master/LICENSE)

![Windows](https://img.shields.io/badge/Windows-supported-brightgreen?logo=windows)
![Linux](https://img.shields.io/badge/Linux-supported-brightgreen?logo=linux)
![macOS](https://img.shields.io/badge/macOS-not%20supported-red?logo=apple)

![MSVC](https://img.shields.io/badge/MSVC%202022-supported-brightgreen?logo=visualstudio)
![GCC](https://img.shields.io/badge/GCC-supported-brightgreen?logo=gnu)
![Clang](https://img.shields.io/badge/Clang-not%20supported-red?logo=clang)


---

## Introduction

Zaplet is a cross-platform command-line utility written in C++20, designed for testing REST APIs and creating/replaying test scenarios. Zaplet allows you to perform various types of HTTP requests, configure headers, send data, and analyze responses in a convenient format.

Key features of Zaplet:
- Executing HTTP requests (GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS)
- Replaying test scenarios from YAML files
- Support for variables in scenarios
- Conditional step execution
- Flexible response validation system
- Convenient output formatting (JSON, YAML, table view)
- Configurable logging system

---

## Installation

### Installation on Windows

#### Prerequisites

1. Microsoft Visual Studio 2019 or higher with C++ development components
2. Git
3. CMake version 3.30 or higher

#### Installation Steps

1. Clone the Zaplet repository:
```bash
git clone https://github.com/Gliptodont/zaplet.git
cd zaplet
```

2. Initialize and update submodules:
```bash
git submodule update --init --recursive
```

3. Set up vcpkg:
```bash
cd external
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
```

4. Build with CMake:
```bash
cd ..\..\
cmake --preset=Debug-Windows
cmake --build build/debug --config Debug
```

5. For building a release version:
```bash
cmake --preset=Release-Windows
cmake --build build/release --config Release
```

After installation, the executable file will be available in the directory `build/debug/app/zaplet.exe` or `build/release/app/zaplet.exe` depending on the selected configuration.

### Installation on Linux

#### Prerequisites

1. GCC compiler version 10 or higher
2. Git
3. CMake version 3.30 or higher
4. Development tools: build-essential, pkg-config

#### Installation Steps

1. Install the necessary packages:
```bash
sudo apt-get update
sudo apt-get install -y build-essential pkg-config cmake git
```

2. Clone the Zaplet repository:
```bash
git clone https://github.com/your-repo/zaplet.git
cd zaplet
```

3. Initialize and update submodules:
```bash
git submodule update --init --recursive
```

4. Set up vcpkg:
```bash
cd external
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
```

5. Build with CMake:
```bash
cd ../../
cmake --preset=Debug-Linux
cmake --build build/debug
```

6. For building a release version:
```bash
cmake --preset=Release-Linux
cmake --build build/release
```

After installation, the executable file will be available in the directory `build/debug/app/zaplet` or `build/release/app/zaplet` depending on the selected configuration.

### Installation on macOS

#### Prerequisites

1. Xcode Command Line Tools or Xcode
2. Git
3. CMake version 3.30 or higher
4. Homebrew (optional, for installing dependencies)

#### Installation Steps

1. Install the necessary packages:
```bash
brew install cmake
```

2. Clone the Zaplet repository:
```bash
git clone https://github.com/your-repo/zaplet.git
cd zaplet
```

3. Initialize and update submodules:
```bash
git submodule update --init --recursive
```

4. Set up vcpkg:
```bash
cd external
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
```

5. Build with CMake:
```bash
cd ../../
cmake --preset=Debug-macOS
cmake --build build/debug
```

6. For building a release version:
```bash
cmake --preset=Release-macOS
cmake --build build/release
```

After installation, the executable file will be available in the directory `build/debug/app/zaplet` or `build/release/app/zaplet` depending on the selected configuration.

---

## Basic Usage

Zaplet provides a command-line interface for executing HTTP requests and running scenarios. The general command format:

```bash
zaplet-cli <command> [options]
```

To get help on available commands:

```bash
zaplet-cli --help
```

---

### HTTP Requests

Zaplet supports all major HTTP methods. Below are examples of using each of them.

#### GET Request

```bash
zaplet-cli get https://api.example.com/users
```

Additional parameters:
- `-H, --header` - add an HTTP header (can be specified multiple times)
- `-t, --timeout` - request timeout in seconds (default 30)

Example with an authorization header:
```bash
zaplet-cli get https://api.example.com/users -H "Authorization: Bearer token123"
```

#### POST Request

```bash
zaplet-cli post https://api.example.com/users -d '{"name": "John", "email": "john@example.com"}'
```

Additional parameters:
- `-H, --header` - add an HTTP header
- `-d, --data` - data to send in the request body
- `--content-type` - content type (default "application/json")
- `-t, --timeout` - request timeout in seconds

#### PUT Request

```bash
zaplet-cli put https://api.example.com/users/1 -d '{"name": "John Updated", "email": "john@example.com"}'
```

Parameters are similar to the POST request.

#### DELETE Request

```bash
zaplet-cli delete https://api.example.com/users/1
```

Parameters:
- `-H, --header` - add an HTTP header
- `-t, --timeout` - request timeout in seconds

#### PATCH Request

```bash
zaplet-cli patch https://api.example.com/users/1 -d '{"name": "John Patched"}'
```

Parameters are similar to the POST request.

#### HEAD Request

```bash
zaplet-cli head https://api.example.com/users
```

Parameters:
- `-H, --header` - add an HTTP header
- `-t, --timeout` - request timeout in seconds

#### OPTIONS Request

```bash
zaplet-cli options https://api.example.com/users
```

Parameters:
- `-H, --header` - add an HTTP header
- `-t, --timeout` - request timeout in seconds

### Output Formatting

By default, Zaplet formats the output in YAML. You can change the output format using the global `--format` option as follows:

```bash
zaplet-cli --format json get https://api.example.com/users
```

Supported formats:
- `json` - JSON
- `yaml` - YAML (default)
- `table` - table view

## Working with Scenarios

Zaplet allows you to execute previously created API testing scenarios. Scenarios are stored in files with the `.zpl` extension and allow you to run a sequence of HTTP requests with a single command.

Detailed documentation on creating scenarios can be found in the separate file `scenario_writing_guide.md`.

### Running a Scenario

To run a scenario, use the `play` command:

```bash
zaplet-cli play my_scenario.zpl
```

With variables (overriding variables from the scenario file):
```bash
zaplet-cli play my_scenario.zpl -v base_url=https://api.staging.example.com -v auth_token=test_token
```

Variables passed through the command line take precedence over variables defined in the scenario file.

## Advanced Features

### Request Headers

To add HTTP headers to a request, use the `-H` or `--header` option:

```bash
zaplet-cli get https://api.example.com/users -H "Authorization: Bearer token" -H "Accept: application/json"
```

### Timeouts

The request timeout is specified in seconds:

```bash
zaplet-cli get https://api.example.com/users -t 60
```

---

## Logging

Zaplet supports flexible logging with configuration options.

### Logging Configuration

The logging configuration is located in the `config/logger.conf` file:

```ini
[general]
name = zaplet
level = info
pattern = [%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [thread %t] %v
async = false
async_queue_size = 8192
async_thread_count = 1

[console]
enabled = true

[file]
enabled = true
path = logs/zaplet.log
rotating = true
max_size = 10485760
max_files = 10
daily = false
rotation_hour = 0
rotation_minute = 0

[tcp]
enabled = false
host = 127.0.0.1
port = 9000

[udp]
enabled = false
host = 127.0.0.1
port = 9001
```

Available logging levels:
- **trace**: the most detailed level
- **debug**: debug messages
- **info**: information messages (default)
- **warning**: warnings
- **error**: errors
- **fatal**: critical errors
- **off**: logging disabled