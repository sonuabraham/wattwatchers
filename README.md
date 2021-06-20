# Wattwatchers Auditor configuration library 

A library to configure Wattwatchers Auditor configuration files.

# Build Instructions

1. The library uses CMake and depends on libconfig library. 
Install the above two by running the following commands.

apt-get -y install cmake

apt-get install libconfig-dev

2. Clone the respository from github 
git clone https://github.com/sonuabraham/wattwatchers.git

3. Edit the lib/wwconfig.c file to change the path where the config file is placed.

4. Run the following command from the wattwatchers directory. This will build the library and the associated test application.
   cmake . 

   make 

   sudo make install 

5. Run the application 
   
   ./wwtest


