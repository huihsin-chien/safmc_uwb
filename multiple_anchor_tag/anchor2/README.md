# multi-anchor-tag-project/multi-anchor-tag-project/README.md

# Multi Anchor Tag Project

This project implements a multi-anchor ranging system using the DW1000Ng library. It consists of two anchors and one tag, allowing for accurate distance measurements between the tag and the anchors.

## Project Structure

```
multi-anchor-tag-project
├── src
│   ├── tag.cpp          # Implementation for the tag functionality
│   ├── anchor1.cpp      # Implementation for the first anchor
│   ├── anchor2.cpp      # Implementation for the second anchor
│   └── common
│       ├── utils.cpp    # Utility functions shared between tag and anchors
│       └── utils.h      # Header file for utility functions
├── include
│   ├── DW1000Ng.hpp     # Main interface for the DW1000Ng library
│   ├── DW1000NgUtils.hpp# Utility functions for the DW1000Ng library
│   ├── DW1000NgTime.hpp  # Time management functions for the DW1000Ng library
│   ├── DW1000NgConstants.hpp # Constant definitions for the DW1000Ng library
│   └── DW1000NgRanging.hpp # Ranging calculations functions
├── platformio.ini       # Configuration file for PlatformIO
└── README.md            # Project documentation
```

## Setup Instructions

1. **Clone the repository**:
   ```
   git clone <repository-url>
   cd multi-anchor-tag-project
   ```

2. **Install PlatformIO**:
   Ensure you have PlatformIO installed. You can install it as a plugin for Visual Studio Code or use the command line interface.

3. **Open the project**:
   Open the project folder in PlatformIO or Visual Studio Code.

4. **Build the project**:
   Use the PlatformIO build command to compile the project:
   ```
   pio run
   ```

5. **Upload to your device**:
   Connect your microcontroller and upload the firmware:
   ```
   pio run --target upload
   ```

## Usage Guidelines

- **Tag**: The tag will communicate with both anchors to request ranging data. Ensure the tag is powered and within range of the anchors.
- **Anchors**: Each anchor will listen for requests from the tag and respond with the necessary ranging information. Make sure both anchors are powered and properly configured.

## Additional Information

- The project uses the DW1000Ng library for communication with the DW1000 chip. Refer to the library documentation for more details on its usage.
- For any issues or contributions, please open an issue or pull request in the repository.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.