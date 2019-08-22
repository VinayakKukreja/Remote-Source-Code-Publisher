cd Debug
start Server.exe localhost:8080
cd ..
cd Client\bin\x86\Debug
start WpfApp1.exe localhost:8082 localhost:8080
start WpfApp1.exe localhost:8084 localhost:8080
cd ../../../../