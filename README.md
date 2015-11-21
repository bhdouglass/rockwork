Ubuntu Pebble App

* Compiling:

 - Compile with Ubuntu Phone platform integration:
   - Use the Ubuntu SDK to compile and run the app the phone
   or
   - qmake path/to/source && make

 - Compile on desktop for testing/developemt
   - Use the Ubuntu SDK, edit build configuration and add
     CONFIG+=testing
     to qmake arguments
   or
   - qmake path/to/source CONFIG+=testing && make


