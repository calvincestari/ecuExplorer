#include "StdAfx.h"
#include ".\ecutools.h"

ecutools::ecutools(string portname)
{
	ecu = new densoecu(portname);
	l = new log();
	image = NULL;
	kernel = NULL;
	kerneltype = kt_oecu;
	set_model(wrx2002);
	ecu->set_ecu_sim_mode(false);
}

ecutools::~ecutools(void)
{
	delete ecu;
}

void ecutools::set_model(ecumodel _model)
{
	ecu->set_model(_model);
	model = _model;
	switch(model)
	{
	case wrx2002:
	case wrx2004:
	case sti2004:
	case fxt2004:
	default:
		sz_block			= 0x01000;
		sz_page				= 0x04000;
		sz_image			= 0x30000;
		sz_image_compact	= 0x28000;
		loc_ram				= 0x20000;
		sz_ram_hole			= 0x08000;
		loc_denso_kernel = loc_ram;
		break;
	}
}

void ecutools::addlog(log *_l)
{
	delete l;
	l = _l;
	ecu->addlog(l);
}

bool ecutools::ready_port()
{
	return ecu->open();
}

void ecutools::close_port()
{
	ecu->close();

	if(kernel != NULL)
		delete[] kernel;
}

bool ecutools::read_kernel_from_resource(int nResourceId)
{
    HGLOBAL hResourceLoaded;  // handle to loaded resource
    HRSRC   hRes;              // handle/ptr to res. info.
    unsigned char    *lpResLock;        // pointer to resource data
    std::string strOutputLocation;
    std::string strAppLocation;

    if (NULL == (hRes = FindResource(NULL,MAKEINTRESOURCE(nResourceId),"BINARY")))
	{
		l->print(log_ecutools,"can't get kernel from resource.\n");
		return false;
	}

    if (NULL == (hResourceLoaded = LoadResource(NULL, hRes)))
	{
		l->print(log_ecutools,"can't get kernel from resource.\n");
		return false;
	}
    if (NULL == (lpResLock = (unsigned char *) LockResource(hResourceLoaded)))
	{
		l->print(log_ecutools,"can't get kernel from resource.\n");
		return false;
	}

    sz_kernel = SizeofResource(NULL, hRes);

	// read kernel
	if (kernel != NULL)
		delete[] kernel;
	kernel = new unsigned char[sz_kernel];
	memcpy(kernel,lpResLock,sz_kernel);

	return check_kernel();
}

bool ecutools::read_kernel(string kernelfile)
{
	FILE *fp;

	if (NULL == (fp = fopen(kernelfile.c_str(),"rb")))
	{
		l->print(log_ecutools,"can't open kernel file.\n");
		return false;
	}

	fseek(fp,0,SEEK_END);
	sz_kernel = ftell(fp);

	// read kernel
	if (kernel != NULL)
		delete[] kernel;
	kernel = new unsigned char[sz_kernel];
	fseek(fp,0,SEEK_SET);

	if (sz_kernel != fread(kernel,1,sz_kernel,fp))
	{
		l->print(log_ecutools,"can't read kernel.\n");
		fclose(fp);
		return false;
	}
	fclose(fp);

	return check_kernel();
}

bool ecutools::check_kernel()
{
	int i;

	l->print(log_ecutools,"%d byte kernel read.\n",sz_kernel);

	if (sz_kernel < 200 || sz_kernel > 4096)
	{
		l->print(log_ecutools,"kernel is to large or small.\n");
		return false;
	}

	// verify some things about the kernel
	if (kernel[2] != 0x39 || kernel[3] != 0x41)
	{
		l->print(log_ecutools,"kernel is missing 0x3941 signature.\n");
		return false;
	}

	int chksum = 0;
	for (i = 2; i < sz_kernel-1; i += 4)
	{
		chksum += (kernel[i] << 8) + kernel[i+1];
		if ((i == 2 || i == 6) && ((chksum & 0xFFFF) == 0x5AA5))
			break;
	}
	if ((chksum & 0xFFFF) != 0x5AA5)
	{
		l->print(log_ecutools,"WARNING: kernel checksum is invalid.\n");
//		return false;
	}
	else
	{
		l->print(log_ecutools,"kernel checksum is valid (n = %d).\n",i/2-1);
		kerneltype = kt_oecu;
	}
	return true;
}

bool ecutools::read_image(string imagefile)
{
	FILE *fp;

	if (NULL == (fp = fopen(imagefile.c_str(),"rb")))
	{
		l->print(log_ecutools,"can't open image file.\n");
		return false;
	}

	fseek(fp,0,SEEK_END);
	int filesize = ftell(fp);
	if (filesize != sz_image && filesize != sz_image_compact)
	{
		l->print(log_ecutools,"image file is incorrect size.\n");
		fclose(fp);
		return false;
	}

	// read image
	if (image != NULL)
		delete[] image;
	image = new unsigned char[sz_image];
	fseek(fp,0,SEEK_SET);

	if (filesize == sz_image)
	{
		if (sz_image != fread(image,1,sz_image,fp))
		{
			l->print(log_ecutools,"can't read image.\n");
			fclose(fp);
			return false;
		}
	}
	else
	{
		if (loc_ram != fread(image,1,loc_ram,fp))
		{
			l->print(log_ecutools,"can't read image.\n");
			fclose(fp);
			return false;
		}
		memset(image+loc_ram,0,sz_ram_hole);
		if (sz_image-loc_ram-sz_ram_hole != fread(image+loc_ram+sz_ram_hole,1,sz_image-loc_ram-sz_ram_hole,fp))
		{
			l->print(log_ecutools,"can't read image.\n");
			fclose(fp);
			return false;
		}
	}
	l->print(log_ecutools,"%d byte image read.\n",sz_image);

	fclose(fp);
	return true;
}

bool ecutools::flash_page(int addr, bool dowrite)
{
	for (int i = 0; i < sz_page/sz_block; i++)
		if (!flash_block(addr+sz_block*i,dowrite))
			return false;
	return true;
}

bool ecutools::flash_block(int addr, bool dowrite)
{
	for (int j = 0; j < sz_block; j += sz_kernel_comm_buff)
		if (!ecu->kernel_write_flash_buffer(image+addr+j,addr+j,min(sz_block-j,sz_kernel_comm_buff)))
			return false;

	if (dowrite)
	{
		if (!ecu->kernel_commit_flash_buffer(addr,ecu->crc32(image+addr,sz_block)))
			return false;
	}
	else
	{
		if (!ecu->kernel_validate_flash_buffer(ecu->crc32(image+addr,sz_block)))
			return false;
	}
	return true;
}

bool ecutools::flash_image(bool dowrite)
{
	double voltage;
	int addr;
	bool pages_to_update[16];

	if (load_kernel())
	{
		ecu->delay(400);
		
		if (!compare_flash_pages_to_file(pages_to_update,true))
		{
			l->print(log_ecutools,"flash image is identical! exiting.\n");
			return true;
		}

		ecu->enable_openport(SETRTS);

		if (!ecu->kernel_read_programming_voltage(&voltage))
		{
			l->print(log_ecutools,"unable to read programming voltage!\n");
			return false;
		}
		l->print(log_ecutools,"programming voltage is %2.2lf volts\n",voltage);
		if (!ecu->is_programming_voltage_safe(voltage))
		{
			l->print(log_ecutools,"programming voltage is too low!\n");
			if (dowrite)
				return false;
		}

		if (dowrite)
			ecu->kernel_flash_enable();
		else
			ecu->kernel_flash_disable();

		for (int i = 11; i >= 0; i--)
		{
			addr = i * 0x4000;
			if (pages_to_update[i])
			{
				if (dowrite && !ecu->kernel_blank_16k_page(addr))
				{
					ecu->kernel_flash_disable();
					return false;
				}
				if (!flash_page(addr,dowrite))
				{
					ecu->kernel_flash_disable();
					return false;
				}
			}
		}
		ecu->kernel_flash_disable();

		l->print(log_ecutools,"verifying result...\n",voltage);

		if (!compare_flash_pages_to_file(pages_to_update,true))
		{
			l->print(log_ecutools,"flash correct! exiting.\n");
			return true;
		}
		if (dowrite)
			l->print(log_ecutools,"*** FLASH FAILURE ***\n");
		return false;
	}
	l->print(log_ecutools,"unable to load kernel\n");
	return false;
}

bool ecutools::compare_image()
{
	if (load_kernel())
	{
		ecu->delay(400);
		
		compare_flash_pages_to_file(NULL,true);
		return true;
	}
	l->print(log_ecutools,"unable to load kernel\n");
	return false;
}


bool ecutools::compare_flash_pages_to_file(bool *pages_to_update,bool draw_detailed_map)
{
	unsigned int ecucrc,imgcrc;
	bool something_to_update = false;
	string same;	

	l->print(log_ecutools,"comparing ECU flash memory pages to image file\n");

	if (draw_detailed_map)
	{
		l->print(log_ecutools,"addr    0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF\n");
		for (int i = 0; i < 12; i++)
		{
			if (pages_to_update)
				pages_to_update[i] = false;
			// ignore the RAM area
			if (i >= 8 && i <= 9)
				continue;
			l->print(log_ecutools,"%06X  ",i*16384);
			for (int j = 0; j < 64; j++)
			{
				if (!ecu->kernel_CRC32_area(&ecucrc,i * 16384 + j * 256, 256))
				{
					l->print(log_ecutools,"error reading CRC!\n");
					return false;
				}
				imgcrc = ecu->crc32(image+i * 16384 + j * 256, 256);
				if (imgcrc == ecucrc)
					l->print(log_ecutools,".");
				else
				{
					l->print(log_ecutools,"*");
					if (pages_to_update)
						pages_to_update[i] = true;
					something_to_update = true;
				}
			}
			l->print(log_ecutools,"\n");
		}
	}
	else
	{
		l->print(log_ecutools,"comparing ECU flash memory pages to image file\n");
		l->print(log_ecutools,"page    ecu CRC32    img CRC32    same?\n");
		for (int i = 0; i < 12; i++)
		{
			if (pages_to_update)
				pages_to_update[i] = false;

			// ignore the RAM area
			if (i >= 8 && i <= 9)
				continue;
			if (!ecu->kernel_CRC32_area(&ecucrc,i * 16384, 16384))
			{
				l->print(log_ecutools,"error reading CRC!\n");
				return false;
			}
			imgcrc = ecu->crc32(&image[i * 16384], 16384);
			if (imgcrc == ecucrc)
				same = " YES";
			else
			{
				same = " NO ";
				if (pages_to_update)
					pages_to_update[i] = true;
				something_to_update = true;
			}
			l->print(log_ecutools,"%4d     %08X     %08X    %s\n",i,ecucrc,imgcrc,same.c_str());
		}
	}
	return something_to_update;
}

bool ecutools::load_kernel()
{
	if (!ecu->enter_flash_mode())
		return false;

	// enter_flash_mode() may have discovered a running kernel
	// if so, we can skip the kernel load
	if (ecu->get_state() != oecukernel)
	{
		unsigned char *encrypted_kernel = new unsigned char[sz_kernel];
		memcpy(encrypted_kernel,kernel,sz_kernel);
		switch(model)
		{
		case wrx2004:
			// validation done 2 times in case chars are dropped on the ecu from 
			// the baud rate change on the previous command.
			ecu->transform_kernel_block04(encrypted_kernel,sz_kernel,true);
			ecu->delay(200);
			ecu->validate_kernel_ram(loc_denso_kernel,sz_kernel);
			ecu->validate_kernel_ram(loc_denso_kernel,sz_kernel);
			ecu->write_denso_kernel_ram_area(encrypted_kernel,loc_denso_kernel,sz_kernel);
			ecu->delay(200);
			// trick TPU code into thinking that we have only downloaded 8 bytes
			// to the kernel so that it will only checksum those bytes (the odd
			// words, actually) and we won't have to have a checksum correct kernel
			ecu->validate_kernel_ram(loc_denso_kernel,8);
			ecu->delay(200);
			ecu->enter_kernel();
			ecu->delay(200);	
			break;
		case wrx2002:
			ecu->transform_kernel_block02(encrypted_kernel,sz_kernel,0,true);
			ecu->delay(200);
			if (!ecu->write_denso_kernel_ram_area(encrypted_kernel,loc_denso_kernel,sz_kernel))
				return false;
			ecu->delay(200);	
			break;
		default:
			return false; // not supported
		}
	}
	else
	{
		l->print(log_ecutools,"kernel already resident - skipping kernel load\n");
	}

	ecu->set_state(oecukernel);
	ecu->delay(1000);
	string version;
	bool result;
	if (result = ecu->kernel_get_version(&version))
		l->print(log_ecutools,"kernel version is : %s\n",version.c_str());
	return result;
}

bool ecutools::exit_kernel()
{
	ecu->kernel_flash_disable();
	return true;
}

bool ecutools::read_memory(unsigned char *data, int addr, int len)
{
	return ecu->kernel_read_area(data,addr,len);
}

bool ecutools::dump_memory_to_file(FILE *fpd, int dumpstart, int dumpend)
{
	int len;
	unsigned char memblock[1024];
	for (int mp = dumpstart; mp <= dumpend; mp += 1024)
	{
		len = min(1024,dumpend-mp+1);
		if (read_memory(memblock,mp,len))
			fwrite(&memblock,1,len,fpd);
		else
		{
			l->print(log_ecutools,"read failure.\n");
			return false;
		}
	}
	return true;
}

bool ecutools::dump_all_memory_to_file(string dumpfilebase)
{
	FILE *fpd;
	string dumpfile;
	bool result = true;

	dumpfile = dumpfilebase+"_cpu.hex";
	if (NULL == (fpd = fopen(dumpfile.c_str(),"wb")))
	{
		l->print(log_ecutools,"can't open dump file.\n");
		return false;
	}
	result &= dump_memory_to_file(fpd,0,0x1FFFF);
	unsigned char d = 0;
	for (int i = 0; i < 32768; i++)
		fwrite(&d,1,1,fpd);
	result &= dump_memory_to_file(fpd,0x28000,0x2FFFF);
	fclose(fpd);
	dumpfile = dumpfilebase+"_tpu.hex";
	if (NULL == (fpd = fopen(dumpfile.c_str(),"wb")))
	{
		l->print(log_ecutools,"can't open dump file.\n");
		return false;
	}
	result &= dump_memory_to_file(fpd,0x60000,0x60FFF);
	fclose(fpd);
	return result;
}

bool ecutools::dump_memory_to_file(string dumpfile,int dumpstart, int dumpend)
{
	FILE *fpd;

	if (NULL == (fpd = fopen(dumpfile.c_str(),"wb")))
	{
		l->print(log_ecutools,"can't open dump file.\n");
		return false;
	}
	bool result = dump_memory_to_file(fpd,dumpstart,dumpend);
	fclose(fpd);
	return result;
}

bool ecutools::enable_openport(unsigned int fn)
{
	return ecu->enable_openport(fn);
}