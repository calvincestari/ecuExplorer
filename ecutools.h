#pragma once

#include <string>
using namespace std;
#include "densoecu.h"
#include "log.h"

#define log_ecutools 6

class ecutools
{
private:
	log *l;
	densoecu *ecu;
	ecumodel model;
	enum {kt_oecu} kerneltype;
	
	unsigned char *kernel;
	int sz_kernel;

	int sz_block;
	int sz_page;
	int loc_denso_kernel;

	unsigned char *image;
	int sz_image;
	int sz_image_compact;
	int loc_ram;
	int sz_ram_hole;

public:
	ecutools(string portname);
	~ecutools(void);
	void addlog(log *_l);
	void set_model(ecumodel _model);
	bool read_image(string imagefile);
	bool flash_image(bool dowrite);
	bool compare_image();
	bool ready_port();
	void close_port();
	bool read_kernel_from_resource(int nResourceId);
	bool read_kernel(string kernelfile);
	bool check_kernel();
	bool load_kernel();
	bool exit_kernel();
	int  read_memory_word(int addr);
	bool read_memory(unsigned char *data, int addr, int len);
	bool dump_all_memory_to_file(string dumpfilebase);
	bool dump_memory_to_file(string dumpfile,int dumpstart, int dumpend);
	bool enable_openport(unsigned int fn);

private:
	bool flash_page(int addr,bool dowrite);
	bool flash_block(int addr,bool dowrite);
	bool dump_memory_to_file(FILE *fpd, int dumpstart, int dumpend);
	bool compare_flash_pages_to_file(bool *pages_to_update,bool draw_detailed_map);
};
