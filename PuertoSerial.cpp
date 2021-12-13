/*
    13 dic 2021

    Código con modificaciones realizado por Juan Felipe Muñoz Fernández
    
    Programa para leer del puerto serial el comando enviado por una placa
    de arduino que conecta un botón. El botón es ON/OFF y arduino envía un
    1 cuando botón está en estado ON. Arduino envía un 0 cuando botón está 
    en estado OFF.

    Se lee el puerto serial y del buffer se extrae el caracter 17 que es donde 
    está el 1 (para el estado ON) o el 0 (para el estado OFF).

    Programa como primera fase en el desarrollo de un plugin de X Plane.
*/

#include<windows.h>
#include<stdio.h>
#include <assert.h>
int main()
{
    // Handle del puerto serial
    HANDLE hComm;
    // Estructura para modificar la configuración del puerto serial
    DCB dcb;
    // Para control de llamado a funciones
    BOOL fSuccess;
    // Buffer de 20 Bytes
    char ReadBuffer[20] = { 0 };
    // Usu futuro para eventos
    OVERLAPPED o;
    // Uso futuro para eventos
    DWORD dwEvtMask;
      

    hComm = CreateFileA("\\\\.\\COM3",    //Port name
            GENERIC_READ | GENERIC_WRITE, //Read/Write
            0,                            // No Sharing
            NULL,                         // No Security
            OPEN_EXISTING,                // Open existing port only
            0,                            // Non Overlapped I/O
            NULL);                        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE)
        printf("Error en la apertura del puerto serial\n");
    else
    {
        printf("Puerto serial abierto satisfactoriamente\n");
        // Inicialización de la estructura DCB
        SecureZeroMemory(&dcb, sizeof(DCB));
        dcb.DCBlength = sizeof(DCB);
        // Velocidad del puerto
        dcb.BaudRate = CBR_115200;
        // Tamaño de datos
        dcb.ByteSize = 8;
        // Sin paridad
        dcb.Parity = NOPARITY;
        // Un bit de parada
        dcb.StopBits = ONESTOPBIT;
     
        // Modificar parámetros del puerto serial
        fSuccess = SetCommState(hComm, &dcb);

        if (!fSuccess)
        {
            //  Handle the error.
            printf("SetCommState() falló con error %d.\n", GetLastError());
            return (3);
        }
        else {
            printf("Puerto serial reconfigurado satisfactoriamente\n");
            
            // Para uso futuro en eventos: defininción de un evento para
            // el S.O que notifique cuando señales: CTD, DSR o RX de datos
            fSuccess = SetCommMask(hComm, EV_CTS | EV_DSR | EV_RXCHAR);
            o.hEvent = CreateEvent(
                NULL,   // default security attributes 
                TRUE,   // manual-reset event 
                FALSE,  // not signaled 
                NULL    // no name
            );
            o.Internal = 0;
            o.InternalHigh = 0;
            o.Offset = 0;
            o.OffsetHigh = 0;

            assert(o.hEvent);

            // Número de bytes leídos
            DWORD NumBytesRead;
            // Para almacenar el caracter 17 del buffer
            char* buttonVal = NULL;
            // Representación entero del caracter17 en el buffer
            int value = -1;

            while (true) {
                // Lectura del puerto serial
                fSuccess = ReadFile(hComm, ReadBuffer, sizeof(ReadBuffer) - 1, &NumBytesRead, NULL);
                if (fSuccess) {                    
                    //printf(ReadBuffer);
                    // Obtener caracter en posición 17 (0 ó 1)
                    buttonVal = &ReadBuffer[17];
                    // Convertir el valor leído a entero
                    value = (int)*buttonVal - 48;
                    // Imprimir el valor leído
                    printf("%d\n", value);                    
                    // Limpar el buffer
                    PurgeComm(hComm, PURGE_RXCLEAR);
                }
                else {
                    // Control de error
                    printf("ReadFile() falló con error %d.\n", GetLastError());
                    return (3);
                }
                printf("\n");

            }

            /* Para uso futuro en eventos
            if (WaitCommEvent(hComm, &dwEvtMask, &o))
            {
                if (dwEvtMask & EV_DSR)
                {
                    // To do.
                    
                }

                if (dwEvtMask & EV_CTS)
                {
                    // To do.
                }
                if (dwEvtMask & EV_RXCHAR)
                {
                    // Evento que se produce cuando se reciben 
                    // datos desde el puerto serial
                    // To do.
                }
            }
            else
            {
                DWORD dwRet = GetLastError();
                if (ERROR_IO_PENDING == dwRet)
                {
                    printf("I/O is pending...\n");

                    // To do.
                }
                else
                    printf("Wait falló con error %d.\n", GetLastError());
            }
            */                   

        }

    }
    // Se cierra el puerto serial
    CloseHandle(hComm);
    return 0;
}