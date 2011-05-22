#pragma once

#include <string>
using namespace std;
#include "densocomm.h"
#include "log.h"

enum ecustate {normal,bootloader_auth,bootloader,oecukernel};
enum ecumodel {wrx2002,wrx2004,sti2004,fxt2004};

#define log_densoecu 7

const static int szrxbuf = 4096;

const static int szram = 0x8000;
const static int locrambase = 0x20000;

const static int szrom = 0x30000;

const static int szpage4k = 0x1000;
const static int szpage = 0x4000;

class densoecu
{
private:
	log *l;
	densocomm *dc;
	ecustate state;
	ecumodel model;
	bool listenmode;
	bool doecusim;
	bool kernel_flash_enabled;
	unsigned char rxbuf[szrxbuf];

	unsigned char *rom;

	unsigned char *page4k;

	int baudrate;
	unsigned char lastrsp;
	unsigned char lastchallenge[4];
	int validate_addr;
	int validate_len;

public:
	densoecu(string portname);
	~densoecu(void);
	void addlog(log *_l);
	bool open();
	void close();
	bool listen();
	void listenonly(bool _listenmode);
	void set_model(ecumodel _model);
	bool load_ram_from_file(string filename);
	bool save_ram_to_file(string filename);
	bool load_rom_from_file(string filename);
	bool save_rom_to_file(string filename);
	bool write_denso_ram_area(unsigned char *data,int addr,int len);
	bool write_denso_kernel_ram_area(unsigned char *data,int addr,int len);
	bool denso02_write_kernel_ram(unsigned char *data,int addr,int len);
	bool check_for_ecu();
	bool enter_flash_mode();
	bool enter_flash_mode02();
	bool enter_flash_mode04();
	bool enter_kernel();
	bool validate_kernel_ram(int addr,int len);
	bool enable_openport(unsigned int fn);

	bool kernel_flash_disable();
	bool kernel_flash_enable();
	bool kernel_read_area(unsigned char *data,int addr,int len);
	bool kernel_CRC32_area(unsigned int *CRC32,int addr,int len);
	bool kernel_get_version(string *version);
	bool kernel_read_programming_voltage(double *voltage);
	bool kernel_blank_16k_page(int pagenum);
	bool kernel_write_flash_buffer(unsigned char *data,int addr,int len);
	bool kernel_validate_flash_buffer(unsigned int crc32);
	bool kernel_commit_flash_buffer(int addr,unsigned int crc32);

	bool denso_cmd_0x81();
	bool denso_cmd_0x83();
	bool denso_cmd_0x10();
	bool denso_bootloader_cmd_start();
	bool do_challenge_response();
	void set_state(ecustate _state);
	ecustate get_state();
	void set_ecu_sim_mode(bool doecusim);
	void delay(int ms);

	static int byte_to_int32(unsigned char *data);
	static int byte_to_int24(unsigned char *data);
	static int byte_to_int16(unsigned char *data);
	static void int16_to_byte(unsigned char *data,int i);
	static void int24_to_byte(unsigned char *data,int i);
	static void int32_to_byte(unsigned char *data,int i);
	static void barrelshift16right(unsigned short *barrel);

	static void compress_mem_map(int *addr);
	static void expand_mem_map(int *addr);
	static void transform_kernel_block02(unsigned char *data,int length,int offset,bool doencrypt);
	static void transform_kernel_block04(unsigned char *data,int length,bool doencrypt);
	static unsigned int crc32(const unsigned char *buf, unsigned int len);
	static bool is_programming_voltage_safe(double voltage);

	static void crazy_transform(unsigned char *data);
	static int transformnybbles(int n);
	static int bootloader_calculate_odd_checksum(unsigned char *data,int len);

private:
	bool listen_denso();
	bool listen_denso02_bootloader();
	bool listen_denso_bootloader();
	bool listen_kernel();
	bool kernel_verify_command_length(int datalen,int expectedlen);

	int get_baudrate_for_state(ecustate _state);
	bool respond_denso_ok();
	bool respond_denso_fail();
	void dump(unsigned char rsp,unsigned char *data,int datalen);
	bool write_denso_ram_block(unsigned char *data,int addr,int len);
	bool write_denso_kernel_ram_block(unsigned char *data,int addr,int len);

};
