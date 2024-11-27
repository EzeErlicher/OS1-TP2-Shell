# Instrucciones para compilar, ejecutar y utilizar tanto la shell como el programa de monitoreo. #

## 1) Instalación de dependencias previas: ##

### Actualizar linux: ###

Asegurarse de actulizar softwares y paquetes instalados:

`sudo apt update`

`sudo apt upgrade`

### Compilador GCC: ###

`sudo apt install build-essential`

### CMake: ###

`sudo apt install cmake`

### Grafana: ###

Tutorial de descarga para LINUX y configuración:

https://grafana.com/docs/grafana/latest/setup-grafana/installation/debian/

(se recomienda instalar la versión enterprise )



## 2) Compilar la shell y el programa de monitoreo: ##

Para compilar la shell deberá correr los siguientes comandos en orden:

`mkdir build`

`cd build`

`cmake ..`

`make`

Una vez hecho lo anterior, puede correr la shell ejecutando:

`./shell`

## 3) Comandos para controlar el programa de monitoreo desde la shell y elección de métricas ##

Para poder utilizar programa de monitoreo, asegúrese de correr la shell previamente como se indica en el apartado 2. 

`start_monitor`: Inicializa monitoring_programm, revise `localhost:8000/metrics` para poder observar las métricas que se están recolectando. Si desea crear dashboards para visualizar la info de manera mas personalizada, siga los tutoriales indicados en el apartado 1 sobre la instlación y uso de Grafana 

`stop_monitor`: Detiene monitoring_programm

`status_monitor`: Indica si monitoring_programm esta activo o detenido

 archivo `config.json`: Permite elegir que métricas se desean mostrar. El archivo viene configurado por defecto para que se desplieguen los datos de cpu,memoria y paquetes transmitidos. Si se desea cambiar dicha configuración, coloque `false` o `true` según corresponda, por ejemplo, si sólo se quieren observar las métricas de cpu:
 
 ![alt text](<Screenshot from 2024-11-01 18-15-46.png>)
 

 **Importante**: Asegúrese de que el programa de monitoreo este detenido y luego modifique el archivo (de lo contrario, si se modifica mientras el programa está funcionando, no se verán reflejados los cambios en la configuración). Una vez hecho esto, ejecute start_monitor