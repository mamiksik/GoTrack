//
// Created by Martin Miksik on 15/12/2017.
//

#ifndef GOTRACK_BLELIB_H
#define GOTRACK_BLELIB_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"

#include "esp_gatt_common_api.h"

#define GATTS_TABLE_TAG "GATTS_TABLE_DEMO"

#define PROFILE_NUM                    1
#define PROFILE_APP_IDX                0
#define ESP_APP_ID  0
#define SAMPLE_DEVICE_NAME              "ESP_GATTS_DEMO"
#define SVC_INST_ID                        0

#define GATTS_DEMO_CHAR_VAL_LEN_MAX        100
#define PREPARE_BUF_MAX_SIZE 1024
#define CHAR_DECLARATION_SIZE   (sizeof(uint8_t))

#define ADV_CONFIG_FLAG      (1 << 0)
#define SCAN_RSP_CONFIG_FLAG (1 << 1)

enum
{
	IDX_SVC,
	IDX_CHAR_A,
	IDX_CHAR_VAL_A,
	IDX_CHAR_CFG_A,

	IDX_CHAR_B,
	IDX_CHAR_VAL_B,

	IDX_CHAR_C,
	IDX_CHAR_VAL_C,

	HRS_IDX_NB,
};


/* Service */
static const uint16_t GATTS_SERVICE_UUID_TEST    = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A     = 0xFF01;
static const uint16_t GATTS_CHAR_UUID_TEST_B     = 0xFF02;
static const uint16_t GATTS_CHAR_UUID_TEST_C     = 0xFF03;

static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read                =  ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_write               = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_read_write_notify   = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t heart_measurement_ccc[2] = {0x00, 0x00};
static const uint8_t char_value[4] = {0x11, 0x22, 0x33, 0x44};


static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] =
		{
				// Service Declaration
				[IDX_SVC]        =
						{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
								sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_TEST), (uint8_t *)&GATTS_SERVICE_UUID_TEST}},

				/* Characteristic Declaration */
				[IDX_CHAR_A]     =
						{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
								CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

				/* Characteristic Value */
				[IDX_CHAR_VAL_A] =
						{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_A, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
								GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

				/* Client Characteristic Configuration Descriptor */
				[IDX_CHAR_CFG_A]  =
						{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
								sizeof(uint16_t), sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},

				/* Characteristic Declaration */
				[IDX_CHAR_B]      =
						{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
								CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

				/* Characteristic Value */
				[IDX_CHAR_VAL_B]  =
						{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_B, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
								GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

				/* Characteristic Declaration */
				[IDX_CHAR_C]      =
						{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
								CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},

				/* Characteristic Value */
				[IDX_CHAR_VAL_C]  =
						{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_C, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
								GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

		};

typedef struct {
	uint8_t                 *prepare_buf;
	int                     prepare_len;
} prepare_type_env_t;

static prepare_type_env_t prepare_write_env;

static uint8_t service_uuid[16] = {
		/* LSB <--------------------------------------------------------------------------------> MSB */
		//first uuid, 16bit, [12],[13] is the value
		0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

/* The length of adv data must be less than 31 bytes */
static esp_ble_adv_data_t adv_data = {
		.set_scan_rsp        = false,
		.include_name        = true,
		.include_txpower     = true,
		.min_interval        = 0x20,
		.max_interval        = 0x40,
		.appearance          = 0x00,
		.manufacturer_len    = 0,    //TEST_MANUFACTURER_DATA_LEN,
		.p_manufacturer_data = NULL, //test_manufacturer,
		.service_data_len    = 0,
		.p_service_data      = NULL,
		.service_uuid_len    = sizeof(service_uuid),
		.p_service_uuid      = service_uuid,
		.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
		.set_scan_rsp = true,
		.include_name = true,
		.include_txpower = true,
		.min_interval = 0x20,
		.max_interval = 0x40,
		.appearance = 0x00,
		.manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
		.p_manufacturer_data =  NULL, //&test_manufacturer[0],
		.service_data_len = 0,
		.p_service_data = NULL,
		.service_uuid_len = 16,
		.p_service_uuid = service_uuid,
		.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};


static esp_ble_adv_params_t adv_params = {
		.adv_int_min        = 0x20,
		.adv_int_max        = 0x40,
		.adv_type           = ADV_TYPE_IND,
		.own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
		.peer_addr            = { 0, 0, 0, 0, 0, 0 },
		.peer_addr_type       = BLE_ADDR_TYPE_PUBLIC,
		.channel_map        = ADV_CHNL_ALL,
		.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
	esp_gatts_cb_t gatts_cb;
	uint16_t gatts_if;
	uint16_t app_id;
	uint16_t conn_id;
	uint16_t service_handle;
	esp_gatt_srvc_id_t service_id;
	uint16_t char_handle;
	esp_bt_uuid_t char_uuid;
	esp_gatt_perm_t perm;
	esp_gatt_char_prop_t property;
	uint16_t descr_handle;
	esp_bt_uuid_t descr_uuid;
};

static uint8_t adv_config_done = 0;


class BLELib
{
public:
	BLELib( )
	{
		esp_err_t ret;
		/* Initialize NVS. */
		ret = nvs_flash_init();
		if ( ret == ESP_ERR_NVS_NO_FREE_PAGES ) {
			ESP_ERROR_CHECK( nvs_flash_erase() );
			ret = nvs_flash_init();
		}
		ESP_ERROR_CHECK( ret );

		esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
		ret = esp_bt_controller_init( & bt_cfg );
		if ( ret ) {
			ESP_LOGE( GATTS_TABLE_TAG, "%s enable controller failed\n", __func__ );
			return;
		}

		ret = esp_bt_controller_enable( ESP_BT_MODE_BTDM );
		if ( ret ) {
			ESP_LOGE( GATTS_TABLE_TAG, "%s enable controller failed\n", __func__ );
			return;
		}

		ESP_LOGI( GATTS_TABLE_TAG, "%s init bluetooth\n", __func__ );
		ret = esp_bluedroid_init();
		if ( ret ) {
			ESP_LOGE( GATTS_TABLE_TAG, "%s init bluetooth failed\n", __func__ );
			return;
		}


		ESP_LOGI( GATTS_TABLE_TAG, "enable bluedroid");
		ret = esp_bluedroid_enable();
		if ( ret ) {
			ESP_LOGE( GATTS_TABLE_TAG, "%s enable bluetooth failed\n", __func__ );
			return;
		}

		ESP_LOGI( GATTS_TABLE_TAG, "Set device name");
		ret = esp_ble_gap_set_device_name("HELLO ESP");
		if ( ret ) {
			ESP_LOGE( GATTS_TABLE_TAG, "gap set device name error, error code = %x", ret );
			return;
		}

		ESP_LOGI( GATTS_TABLE_TAG, "gatts register ");
		ret = esp_ble_gatts_register_callback( BLELib::gatts_event_handler );
		if ( ret ) {
			ESP_LOGE( GATTS_TABLE_TAG, "gatts register error, error code = %x", ret );
			return;
		}

		ESP_LOGI( GATTS_TABLE_TAG, "gap register");
		ret = esp_ble_gap_register_callback( BLELib::gap_event_handler );
		if ( ret ) {
			ESP_LOGE( GATTS_TABLE_TAG, "gap register error, error code = %x", ret );
			return;
		}

		ESP_LOGI( GATTS_TABLE_TAG, "gatts app register");
		ret = esp_ble_gatts_app_register( ESP_APP_ID );
		if ( ret ) {
			ESP_LOGE( GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret );
			return;
		}

		ESP_LOGI( GATTS_TABLE_TAG, "set local  MTU");
		esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu( 500 );
		if ( local_mtu_ret ) {
			ESP_LOGE( GATTS_TABLE_TAG, "set local  MTU failed, error code = %x", local_mtu_ret );
		}

		ESP_LOGI( GATTS_TABLE_TAG, "FINISHED");
	};

	static void gap_event_handler( esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param );

	static void
	gatts_profile_event_handler( esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param );

	static void
	gatts_event_handler( esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param );

	static void example_prepare_write_event_env( esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env,
	                                             esp_ble_gatts_cb_param_t *param );

	static void example_exec_write_event_env( prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param );

public:
	static uint16_t heart_rate_handle_table[HRS_IDX_NB];
	int ad;
	static int x;
};

int BLELib::heart_rate_handle_table = 100;

//static uint16_t heart_rate_handle_table[HRS_IDX_NB];
//uint16_t BLELib::heart_rate_handle_table[HRS_IDX_NB] = {0};

static struct gatts_profile_inst profile_tab[PROFILE_NUM] = {
		[PROFILE_APP_IDX] = {
				.gatts_cb = BLELib::gatts_profile_event_handler,
				.gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
		},
};

#endif //GOTRACK_BLELIB_H
