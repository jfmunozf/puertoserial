#include<windows.h>
#include<stdio.h>
#include <assert.h>
int main()
{
    HANDLE hComm;
    DCB dcb;
    BOOL fSuccess;
    char ReadBuffer[30] = { 0 };
    OVERLAPPED o;
    DWORD dwEvtMask;

   

    hComm = CreateFileA("\\\\.\\COM3",                //port name
        GENERIC_READ | GENERIC_WRITE, //Read/Write
        0,                            // No Sharing
        NULL,                         // No Security
        OPEN_EXISTING,// Open existing port only
        0,            // Non Overlapped I/O
        NULL);        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE)
        printf("Error en la apertura del puerto serial\n");
    else
    {
        printf("Puerto serial abierto satisfactoriamente\n");
        //  Initialize the DCB structure.
        SecureZeroMemory(&dcb, sizeof(DCB));
        dcb.DCBlength = sizeof(DCB);
        dcb.BaudRate = CBR_115200;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;

        fSuccess = SetCommState(hComm, &dcb);

        if (!fSuccess)
        {
            //  Handle the error.
            printf("SetCommState failed with error %d.\n", GetLastError());
            return (3);
        }
        else {
            printf("Puerto serial reconfigurado satisfactoriamente\n");
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

            if (WaitCommEvent(hComm, &dwEvtMask, &o))
            {
                if (dwEvtMask & EV_DSR)
                {
                    // To do.
                    printf("Se produjo este EV_DSR");
                }

                if (dwEvtMask & EV_CTS)
                {
                    printf("Se produjo este EV_CTS");
                }
                if (dwEvtMask & EV_RXCHAR)
                {
                    fSuccess = ReadFile(hComm, ReadBuffer, 29, NULL, NULL);

                    if (fSuccess) {
                        printf("*** El apocalipsis, misil disparado...***\n");
                        printf(ReadBuffer);
                    }
                    else {

                        //  Handle the error.
                        printf("ReadFile failed with error %d.\n", GetLastError());
                        return (3);
                    }
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
                    printf("Wait failed with error %d.\n", GetLastError());
            }


            
            


        }

    }



    CloseHandle(hComm);//Closing the Serial Port

    return 0;
}