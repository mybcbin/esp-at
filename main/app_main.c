/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_err.h"

#include "esp_at.h"
#include "esp_at_init.h"

#include "esp_sntp.h"
#include <time.h>
#include <sys/time.h>
#include "esp_at.h"   // 提供 at_port_write 声明

static void sntp_sync_cb(struct timeval *tv)
{
    if (tv == NULL || tv->tv_sec < 1609459200) {
        return; // 过滤 1970 初始时间
    }

    char buf[64];
    int len = snprintf(buf, sizeof(buf),
                       "\r\n+SYSTIMESTAMP::%lld\r\n",
                       (long long)tv->tv_sec);
    if (len > 0 && len < sizeof(buf)) {
        at_port_write((uint8_t *)buf, (uint16_t)len);
    }
}

void app_main(void)
{
    esp_at_main_preprocess();

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_at_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_sntp_set_time_sync_notification_cb(sntp_sync_cb);

    esp_at_init();
}
