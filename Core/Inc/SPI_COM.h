//
// Created by paolo on 16/08/25.
//

#pragma once

#include "cmsis_os2.h"
#include "stm32f4xx.h"

constexpr uint32_t SPI_BUFFER_SIZE = 1024;
struct SPI_Message_t {
    uint8_t data[SPI_BUFFER_SIZE];
    uint16_t length;
    bool valid;
} ;

class SPI_COM {
public:
    SPI_COM();
    void init();
    [[noreturn]] static void StartSpiTask(void * parameter);

    [[noreturn]] static void StartDataReadyTask(void * parameter);

    static void newData();
    [[nodiscard]] osMessageQueueId_t getTxQueue() const {return txQueueHandle;}
    [[nodiscard]] osMessageQueueId_t getRxQueue() const {return rxQueueHandle;}
private:

    HAL_StatusTypeDef ESP32_SPI_Transaction(uint8_t *tx_data, uint16_t tx_len, uint8_t *rx_data, uint16_t *rx_len);

    uint8_t spi_tx_buffer[SPI_BUFFER_SIZE];
    uint8_t spi_rx_buffer[SPI_BUFFER_SIZE];
    osMessageQueueId_t txQueueHandle;
    osMessageQueueId_t rxQueueHandle;
    osThreadId_t spiTaskHandle;
    osThreadId_t dataReadyTaskHandle;
};


