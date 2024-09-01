# Password-Manager (Under Development)

A simple password manager application built for personal use. This app allows you to securely store and retrieve passwords for various accounts. It is designed with a focus on simplicity, security, and ease of use.

## Features

- Secure password storage
- Minimalistic design
- Built with Qt 5 for a native desktop experience

## Requirements

- CMake 3.5 or higher
- Qt 5 (Widgets module)
- C++11 compatible compiler

## Building the Project

To build the project, follow these steps:

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/iman-zamani/Password-Manager.git
   cd Password-Manager
   ```

2. **Create a Build Directory:**

   It's recommended to build the project in a separate directory:

   ```bash
   mkdir build
   cd build
   ```

3. **Configure the Project with CMake:**

   Generate the necessary files for building:

   ```bash
   cmake ..
   ```

4. **Build the Project:**

   Compile the project using CMake's build command:

   ```bash
   cmake --build .
   ```

5. **Run the Application:**

   After building, you can run the application with:

   ```bash
   ./Password-Generator
   ```

## Project Structure

- `main.cpp`: Entry point of the application.
- `mainwindow.cpp`: Contains the main window logic.
- `mainwindow.h`: Header file for the main window.
- `CMakeLists.txt`: Configuration file for CMake.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

## Contributing

Contributions are welcome! If you'd like to contribute, please fork the repository and submit a pull request.

## Contact

For any questions or feedback, please reach out to `izamanimoghaddam@gmail.com`.
