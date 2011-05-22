#pragma once

#include "serial.h"
#include "log.h"
#include <string>
using namespace std;

#define log_densocomm 8

const static unsigned char denso_write_ram							= 0xB0;
const static unsigned char denso_reset_trouble_codes				= 0xB8;
const static unsigned char denso_read_param							= 0xA8;
const static unsigned char param_boost								= 0x24;
const static unsigned char denso_0x81								= 0x81;
const static unsigned char denso_0x83								= 0x83;
const static unsigned char denso_0x10								= 0x10;
const static unsigned char denso_0x30								= 0x30;
const static unsigned char denso_cmd_challenge						= 0x27;

const static unsigned char denso_bootloader_cmd_enter_kernel		= 0x31;
const static unsigned char denso_bootloader_validate_kram_addr		= 0x34;
const static unsigned char denso_bootloader_cmd_write_kernel_ram	= 0x36;

const static unsigned char denso_bootloader_rsp_enter_kernel		= 0x71;
const static unsigned char denso_bootloader_rsp_validate_kram_addr	= 0x74;
const static unsigned char denso_bootloader_rsp_write_kernel_ram	= 0x76;
const static unsigned char denso_bootloader_rsp_error				= 0x7F;
const static unsigned char denso_bootloader_rsp_validate_kram_addr_success = 0x84;
const static int denso_bootloader_rsp_ok							= 0x0101;
const static int denso_bootloader_rsp_error_bad_checksum			= 0x3122;
const static int denso_odd_checksum									= 0x5AA5;

const static unsigned char denso_rsp_write_ram						= 0xF0;
const static unsigned char denso_rsp_reset_trouble_codes			= 0xF8;
const static unsigned char denso_rsp_read_param						= 0xE8;
const static unsigned char denso_rsp_0x81							= 0xC1;
const static unsigned char denso_rsp_0x83							= 0xC3;
const static unsigned char denso_rsp_0x10							= 0x50;
const static unsigned char denso_rsp_0x30							= 0x70;
const static unsigned char denso_rsp_challenge						= 0x67;
const static unsigned char denso_rsp_ok								= 0x01;
const static unsigned char denso_rsp_fail							= 0xFF;

const static unsigned char denso02_bootloader_cmd_write_kernel_ram	= 0x53;
const static unsigned char denso02_bootloader_cmd_start		= 0x4D;
const static unsigned char denso02_bootloader_rsp_bad_address		= 0x23;
const static unsigned char denso02_bootloader_rsp_data_overrun		= 0x22;
const static unsigned char denso02_bootloader_rsp_bad_checksum		= 0x03;
const static unsigned char denso02_bootloader_rsp_bad_magic_number	= 0x01;
const static unsigned int  denso_magic_number						= 0x3941;

const static int sz_kernel_comm_buff					= 512;

const static unsigned char kernel_cmd_get_version_info				= 0x01;
const static unsigned char kernel_cmd_CRC_area						= 0x02;
const static unsigned char kernel_cmd_read_area						= 0x03;
const static unsigned char kernel_cmd_read_programming_voltage		= 0x04;

const static unsigned char kernel_cmd_flash_enable					= 0x20;
const static unsigned char kernel_cmd_flash_disable					= 0x21;
const static unsigned char kernel_cmd_write_flash_buffer			= 0x22;
const static unsigned char kernel_cmd_validate_flash_buffer			= 0x23;
const static unsigned char kernel_cmd_commit_flash_buffer			= 0x24;
const static unsigned char kernel_cmd_blank_16k_page				= 0x25;

const static unsigned char kernel_rsp_get_version_info				= 0x81;
const static unsigned char kernel_rsp_CRC_area						= 0x82;
const static unsigned char kernel_rsp_read_area						= 0x83;
const static unsigned char kernel_rsp_read_programming_voltage		= 0x84;

const static unsigned char kernel_rsp_flash_enable					= 0xA0;
const static unsigned char kernel_rsp_flash_disable					= 0xA1;
const static unsigned char kernel_rsp_write_flash_buffer			= 0xA2;
const static unsigned char kernel_rsp_validate_flash_buffer			= 0xA3;
const static unsigned char kernel_rsp_commit_flash_buffer			= 0xA4;
const static unsigned char kernel_rsp_blank_16k_page				= 0xA5;

const static unsigned char kernel_rsp_error_bad_data_length			= 0xF0;
const static unsigned char kernel_rsp_error_bad_data_value			= 0xF1;
const static unsigned char kernel_rsp_error_programming_failure		= 0xF2;
const static unsigned char kernel_rsp_error_prog_voltage_low		= 0xF3;
const static unsigned char kernel_rsp_error_prog_voltage_high		= 0xF4;
const static unsigned char kernel_rsp_error_bad_crc					= 0xF5;
const static unsigned char kernel_rsp_error_bad_command				= 0xFF;

class densocomm
{
private:
	log *l;
	serial s;

public:
	densocomm(string portname);
	~densocomm(void);

	void addlog(log *_l);
	bool open();
	void close();
	void set_baud(int baudrate);
	void set_default_timeout(int to);
	unsigned char checksum(unsigned char *data,int datalen);
	bool write_denso_cmd(unsigned char cmd,unsigned char *data,int datalen);
	bool write_denso_cmd_echocheck(unsigned char cmd,unsigned char *data,int datalen);
	bool write_denso02_cmd(unsigned char cmd,unsigned char *data,int datalen);
	bool write_denso02_cmd_echocheck(unsigned char cmd,unsigned char *data,int datalen);
	bool write_denso_rsp(unsigned char cmd,unsigned char *data,int datalen);
	bool write_denso_rsp_echocheck(unsigned char cmd,unsigned char *data,int datalen);
	bool write_kernel_cmd(unsigned char cmd,unsigned char *data,int datalen);
	bool write_kernel_cmd_echocheck(unsigned char cmd,unsigned char *data,int datalen);
	bool write_kernel_rsp(unsigned char rsp,unsigned char *data,int datalen);
	bool read_denso_rsp_varlen(unsigned char *rsp,unsigned char *data,int *datalen);
	bool read_kernel_rsp_varlen(unsigned char *rsp,unsigned char *data,int *datalen);
	bool read_kernel_specific_rsp(unsigned char rsp,unsigned char *data,int datalen);
	bool read_kernel_specific_rsp_varlen(unsigned char rsp,unsigned char *data,int *datalen);
	bool write_byte(int addr,unsigned char data);
	bool read_byte(int addr,unsigned char *data);
	bool write_raw(unsigned char *data,int datalen);
	bool read_serial_bytes(unsigned char *data,unsigned int *datalen);
	bool is_LE_high();
	bool pulse_LE();
	bool LE_low();
	bool LE_high();
	bool enable_openport(unsigned int fn);

private:
	void log_kernel_response(unsigned char rsp);
};
