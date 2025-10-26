#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "tinyprotocol.h"

// Circular buffer for received data
#define CIRCULAR_BUFFER_SIZE 1024
typedef struct {
    uint8_t buffer[CIRCULAR_BUFFER_SIZE];
    volatile size_t head;
    volatile size_t tail;
    CRITICAL_SECTION cs;
    HANDLE dataAvailable;
} CircularBuffer;

// Global variables
static volatile HANDLE hSerial;
static volatile bool keepRunning = true;
static CircularBuffer rxBuffer;
static HANDLE readThread;

// Function declarations
int16_t CustomProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size);
int16_t CustomWriteBuffer(const uint8_t* buffer, uint8_t size);
int16_t CustomProcessTelemetryRequest();

static const struct TINYPROTOCOL_Config tinyprotocol_config = {
        CustomProcessTelecommand,
        CustomProcessTelemetryRequest,
        CustomWriteBuffer,
};

// Initialize circular buffer
void InitCircularBuffer(CircularBuffer* cb) {
    cb->head = 0;
    cb->tail = 0;
    InitializeCriticalSection(&cb->cs);
    cb->dataAvailable = CreateEvent(NULL, TRUE, FALSE, NULL);
}

// Write to circular buffer
bool WriteToBuffer(CircularBuffer* cb, uint8_t data) {
    bool result = false;
    EnterCriticalSection(&cb->cs);

    size_t nextHead = (cb->head + 1) % CIRCULAR_BUFFER_SIZE;
    if (nextHead != cb->tail) {
        cb->buffer[cb->head] = data;
        cb->head = nextHead;
        result = true;
        SetEvent(cb->dataAvailable);
    }

    LeaveCriticalSection(&cb->cs);
    return result;
}

// Read from circular buffer
bool ReadFromBuffer(CircularBuffer* cb, uint8_t* data) {
    bool result = false;
    EnterCriticalSection(&cb->cs);

    if (cb->head != cb->tail) {
        *data = cb->buffer[cb->tail];
        cb->tail = (cb->tail + 1) % CIRCULAR_BUFFER_SIZE;
        result = true;

        if (cb->head == cb->tail) {
            ResetEvent(cb->dataAvailable);
        }
    }

    LeaveCriticalSection(&cb->cs);
    return result;
}

// Serial read thread function
DWORD WINAPI SerialReadThread(LPVOID lpParam) {
    uint8_t byte;
    DWORD bytesRead;

    while (keepRunning) {
        if (ReadFile(hSerial, &byte, 1, &bytesRead, NULL)) {
            if (bytesRead == 1) {
                printf("%02X", byte);  // Print byte in hexadecimal format
                fflush(stdout);  // Force output to console
            }
        } else {
            DWORD error = GetLastError();
            if (error != ERROR_TIMEOUT) {
                printf("\nSerial read error: %lu\n", error);
                break;
            }
        }
    }
    return 0;
}

enum CustomProtocolTelecommand {
    TC_TOGGLE_LED = TINYPROTOCOL_TC_RESERVED,
    TC_SET_LED
};

int main() {
    TINYPROTOCOL_Initialize();
    InitCircularBuffer(&rxBuffer);

    // Open COM port
    hSerial = CreateFile("COM6",
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         0,
                         NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error opening port\n");
        return 1;
    }

    // Configure port settings
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error getting port state\n");
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error setting port state\n");
        CloseHandle(hSerial);
        return 1;
    }

    // Set timeouts
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }

    // Create read thread
    readThread = CreateThread(NULL, 0, SerialReadThread, NULL, 0, NULL);
    if (readThread == NULL) {
        printf("Error creating read thread\n");
        CloseHandle(hSerial);
        return 1;
    }

    // Main loop
    while(1) {
        uint8_t byte = 0;
        TINYPROTOCOL_SendTelecommand(&tinyprotocol_config, TC_SET_LED, &byte, 1);
        TINYPROTOCOL_SendTelemetryRequest(&tinyprotocol_config, TINYPROTOCOL_TLM_RESERVED);

        byte = 1;
        TINYPROTOCOL_SendTelecommand(&tinyprotocol_config, TC_SET_LED, &byte, 1);
        TINYPROTOCOL_SendTelemetryRequest(&tinyprotocol_config, TINYPROTOCOL_TLM_RESERVED);

        Sleep(1000);  // Wait before next cycle
    }

    return 0;
}

int16_t CustomProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size) {
    return ETINYPROTOCOL_SUCCESS;
}

int16_t CustomWriteBuffer(const uint8_t* buffer, uint8_t size) {
    DWORD bytesWritten;
    if (!WriteFile(hSerial, buffer, size, &bytesWritten, NULL)) {
        printf("Error writing to port\n");
        return ETINYPROTOCOL_UNKNOWN;
    }
    return ETINYPROTOCOL_SUCCESS;
}

int16_t CustomProcessTelemetryRequest() {
    return ETINYPROTOCOL_SUCCESS;
}