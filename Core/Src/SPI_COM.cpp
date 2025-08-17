//
// Created by paolo on 16/08/25.
//

#include <cstdint>
#include <cmsis_os2.h>
#include <FreeRTOS.h>
#include "../Inc/SPI_COM.h"

#include <cstring>

#include "main.h"

#include "stm32f4xx_hal_gpio.h"

constexpr uint32_t SPI_TIMEOUT=1000;
constexpr uint32_t QUEUE_SIZE=10;
constexpr uint32_t SPI_STACK_SIZE=2048;
constexpr uint32_t DATA_READY_STACK_SIZE=512;
constexpr uint32_t QUEUE_ITEM_SIZE=sizeof(SPI_Message_t);
constexpr uint32_t QUEUE_BUFFER_SIZE=((QUEUE_SIZE * QUEUE_ITEM_SIZE) + 64);


extern SPI_HandleTypeDef hspi2;
extern SPI_COM spi_com;

static uint8_t queue_buffer_tx[QUEUE_BUFFER_SIZE];
static uint64_t queue_cb_tx[16];
static uint8_t queue_buffer_rx[QUEUE_BUFFER_SIZE];
static uint64_t queue_cb_rx[16];

const osMessageQueueAttr_t txQueue_attributes = {
    .name = "txQueue",
    .attr_bits = 0U,
    .cb_mem = queue_cb_tx,
    .cb_size = sizeof(queue_cb_tx),
    .mq_mem = queue_buffer_tx,
    .mq_size = sizeof(queue_buffer_tx)
  };

const osMessageQueueAttr_t rxQueue_attributes = {
    .name = "txQueue",
    .attr_bits = 0U,
    .cb_mem = queue_cb_rx,
    .cb_size = sizeof(queue_cb_rx),
    .mq_mem = queue_buffer_rx,
    .mq_size = sizeof(queue_buffer_rx)
  };

uint8_t txThreadStack[SPI_STACK_SIZE];
static StaticTask_t spiThreadCB;

const osThreadAttr_t spiThread_attributes = {
    .name = "spiThread",
    .cb_mem = (void*) &spiThreadCB,
    .cb_size = sizeof(spiThreadCB),
    .stack_mem =  txThreadStack,
    .stack_size = SPI_STACK_SIZE

  };

uint8_t rxThreadStack[DATA_READY_STACK_SIZE];
static StaticTask_t dataReadyThreadCB;

const osThreadAttr_t data_Ready_thread_attributes = {
    .name = "dataReadyThread",
    .cb_mem = (void*) &dataReadyThreadCB,
    .cb_size = sizeof(dataReadyThreadCB),
    .stack_mem =  rxThreadStack,
    .stack_size = DATA_READY_STACK_SIZE

  };


SPI_COM::SPI_COM() :txQueueHandle(nullptr),rxQueueHandle(nullptr){

}

void SPI_COM::init() {
    txQueueHandle = osMessageQueueNew (QUEUE_SIZE, QUEUE_ITEM_SIZE, &txQueue_attributes);
    rxQueueHandle = osMessageQueueNew (QUEUE_SIZE, QUEUE_ITEM_SIZE, &rxQueue_attributes);
    spiTaskHandle = osThreadNew(&SPI_COM::StartSpiTask,nullptr,&spiThread_attributes );
    dataReadyTaskHandle = osThreadNew(&SPI_COM::StartDataReadyTask,nullptr,&data_Ready_thread_attributes );
}

[[noreturn]] void SPI_COM::StartSpiTask(void * parameter) {
    SPI_Message_t tx_msg, rx_msg;
    uint16_t rx_length;
    uint32_t flags;

    while (true) {
        // Attendi segnale da Data Ready interrupt o timeout
        flags = osThreadFlagsWait(0x01, osFlagsWaitAny, 100);

        if (flags == 0x01 || HAL_GPIO_ReadPin(DATA_READY_GPIO_Port, DATA_READY_Pin) == GPIO_PIN_SET) {
            // Controlla se ci sono dati da inviare
            bool has_tx_data = false;
            if (osMessageQueueGet(spi_com.txQueueHandle, &tx_msg, nullptr, 0) == osOK) {
                has_tx_data = true;
            }

            // Esegui transazione SPI
            HAL_StatusTypeDef status;
            if (has_tx_data) {
                status = spi_com.ESP32_SPI_Transaction(tx_msg.data, tx_msg.length, rx_msg.data, &rx_length);
            } else {
                status = spi_com.ESP32_SPI_Transaction(nullptr, 0, rx_msg.data, &rx_length);
            }

            if (status == HAL_OK && rx_length > 0) {
                rx_msg.length = rx_length;
                rx_msg.valid = true;

                osMessageQueuePut(spi_com.rxQueueHandle, &rx_msg, 0, 0);
            }

        }
    }
}

[[noreturn]] void SPI_COM::StartDataReadyTask(void * parameter) {
    uint8_t last_state = 0;
    while (true) {
        uint8_t current_state = HAL_GPIO_ReadPin(DATA_READY_GPIO_Port, DATA_READY_Pin);

        if (current_state != last_state) {
            last_state = current_state;

            if (current_state == GPIO_PIN_SET) {
                osThreadFlagsSet(spi_com.spiTaskHandle, 0x01);
            }
        }

        osDelay(10);
    }
}

void SPI_COM::newData() {
    osThreadFlagsSet(spi_com.spiTaskHandle, 0x01);
}


HAL_StatusTypeDef SPI_COM::ESP32_SPI_Transaction(uint8_t *tx_data, uint16_t tx_len, uint8_t *rx_data, uint16_t *rx_len) {
    memset(spi_tx_buffer, 0, SPI_BUFFER_SIZE);
    memset(spi_rx_buffer, 0, SPI_BUFFER_SIZE);

    // Copia i dati da inviare nel buffer SPI
    if (tx_data != nullptr && tx_len > 0) {
        memcpy(spi_tx_buffer, tx_data, tx_len < SPI_BUFFER_SIZE ? tx_len : SPI_BUFFER_SIZE);
    }

    // Esegui la transazione SPI
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi2, spi_tx_buffer, spi_rx_buffer, SPI_BUFFER_SIZE, SPI_TIMEOUT);

    if (status == HAL_OK) {
        // Trova la lunghezza effettiva dei dati ricevuti
        *rx_len = 0;
        for (int i = SPI_BUFFER_SIZE - 1; i >= 0; i--) {
            if (spi_rx_buffer[i] != 0) {
                *rx_len = i + 1;
                break;
            }
        }

        // Copia i dati ricevuti
        if (*rx_len > 0 && rx_data != nullptr) {
            memcpy(rx_data, spi_rx_buffer, *rx_len);
        }

    }

    return status;
}

