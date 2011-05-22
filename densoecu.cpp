#include "StdAfx.h"
#include ".\densoecu.h"

densoecu::densoecu(string portname)
{
	model = wrx2002;
	dc = new densocomm(portname);
	rom = new unsigned char[szrom];
	page4k = new unsigned char[szpage4k];

	for (int i = 0; i < szrom; i++)
		rom[i] = 0;
	baudrate = 0;
	set_state(normal);
	listenmode = false;
	l = new log();
	lastrsp = 0;
	doecusim = false;
	kernel_flash_enabled = false;
	validate_len = 0;
	validate_addr = locrambase;
}

densoecu::~densoecu(void)
{
	delete dc;
	delete[] rom;
	delete[] page4k;
}

void densoecu::addlog(log *_l)
{
	delete l;
	l = _l;
	dc->addlog(l);
}

void densoecu::listenonly(bool _listenmode)
{
	listenmode = _listenmode;
}

void densoecu::set_ecu_sim_mode(bool _doecusim)
{
	doecusim = _doecusim;
}


void densoecu::set_model(ecumodel _model)
{
	model = _model;
	set_state(normal);
}

void densoecu::set_state(ecustate _state)
{
	int newbaudrate = get_baudrate_for_state(_state);
	if (newbaudrate != baudrate)
	{
		baudrate = newbaudrate;
		delay(100); // give buffers time to flush out
		dc->set_baud(baudrate);
	}
	state = _state;
}

ecustate densoecu::get_state()
{
	return state;
}

int densoecu::get_baudrate_for_state(ecustate _state)
{
	switch(model)
	{
	case wrx2002:
		switch(_state)
		{
		case normal:
		default:
			return 4800;
		case bootloader:
		case bootloader_auth:
			return 9600;
		case oecukernel:
			return 39473;
		}
		break;
	case wrx2004:
	case sti2004:
	case fxt2004:
	default:
		switch(_state)
		{
		case normal:
		case bootloader_auth:
		default:
			return 4800;
		case bootloader:
			return 15625;
		case oecukernel:
			return 39473;
		}
		break;
	}
}

bool densoecu::open()
{
	if (dc->open())
	{
		if (doecusim || model != wrx2002)
			dc->LE_high();
		else
			dc->LE_low();
		return true;
	}
	return false;
}

void densoecu::close()
{
	dc->close();
}

bool densoecu::listen()
{
	switch(model)
	{
	case wrx2002:
		switch(state)
		{
		case normal:
			return listen_denso();
			break;
		case bootloader_auth:
		case bootloader:
			return listen_denso02_bootloader();
			break;
		case oecukernel:
			return listen_kernel();
			break;
		default:
			return false;
		}
		break;
	case wrx2004:
	case sti2004:
	case fxt2004:
	default:
		switch(state)
		{
		case normal:
		case bootloader_auth:
			return listen_denso();
			break;
		case bootloader:
			return listen_denso_bootloader();
			break;
		case oecukernel:
			return listen_kernel();
			break;
		default:
			return false;
		}
		break;
	}
}

bool densoecu::kernel_verify_command_length(int datalen,int expectedlen)
{
	if (datalen == expectedlen)
		return true;
	
	if (!listenmode)
		dc->write_kernel_rsp(kernel_rsp_error_bad_data_length,NULL,0);
	return false;
}

bool densoecu::listen_kernel()
{
	unsigned char rsp;
	int datalen = szrxbuf;
	int i,addr,len;
	unsigned int crc;

	if (!dc->read_kernel_rsp_varlen(&rsp,rxbuf,&datalen))
		return false;

	dump(rsp,rxbuf,datalen);
	// todo emulate kernel

	switch(rsp)
	{
	case kernel_cmd_get_version_info:
		if (!kernel_verify_command_length(datalen,0))
			break;
		l->print(log_densoecu,"kernel get version info\n");
		strcpy((char*)rxbuf,"OpenECU Kernel V0.01");
		dc->write_kernel_rsp(kernel_rsp_get_version_info,rxbuf,(int)strlen((char*)rxbuf));
		break;
	case kernel_rsp_get_version_info:
		l->print(log_densoecu,"kernel response version [%s]\n",rxbuf);
		break;

	case kernel_cmd_CRC_area:
		if (!kernel_verify_command_length(datalen,5))
			break;
		addr = byte_to_int24(rxbuf);
		len = byte_to_int16(rxbuf+3);
		l->print(log_densoecu,"kernel crc32 area addr: %06X len %04X\n",addr,len);
		crc = crc32(rom+addr,len);
		l->print(log_densoecu,"crc32 result: %08X\n",crc);
		int32_to_byte(rxbuf,crc);
		dc->write_kernel_rsp(kernel_rsp_CRC_area,rxbuf,4);
		break;
	case kernel_rsp_CRC_area:
		if (!kernel_verify_command_length(datalen,4))
			break;
		crc = byte_to_int32(rxbuf);
		l->print(log_densoecu,"kernel response crc32: %08X\n",crc);
		break;

	case kernel_cmd_read_area:
		if (!kernel_verify_command_length(datalen,5))
			break;
		addr = byte_to_int24(rxbuf);
		len = byte_to_int16(rxbuf+3);
		l->print(log_densoecu,"kernel read area addr: %06X len %04X\n",addr,len);
		dc->write_kernel_rsp(kernel_rsp_read_area,rom+addr,len);
		break;
	case kernel_rsp_read_area:
		l->print(log_densoecu,"kernel response read area bytes: %04X\n",datalen);
		break;

	case kernel_cmd_read_programming_voltage:
		if (!kernel_verify_command_length(datalen,0))
			break;
		l->print(log_densoecu,"kernel read programming voltage\n");
		int16_to_byte(rxbuf,12*50);
		dc->write_kernel_rsp(kernel_rsp_read_programming_voltage,rxbuf,2);
		break;
	case kernel_rsp_read_programming_voltage:
		if (!kernel_verify_command_length(datalen,2))
			break;
		l->print(log_densoecu,"kernel response programming voltage: %2.2lfV\n",byte_to_int16(rxbuf)/50.0);
		break;

	case kernel_cmd_flash_enable:
		if (!kernel_verify_command_length(datalen,0))
			break;
		l->print(log_densoecu,"kernel flash enable\n");
		kernel_flash_enabled = true;
		if (!listenmode)
			dc->write_kernel_rsp(kernel_rsp_flash_enable,NULL,0);
		break;
	case kernel_rsp_flash_enable:
		if (!kernel_verify_command_length(datalen,0))
			break;
		l->print(log_densoecu,"kernel response flash enabled\n");
		break;

	case kernel_cmd_flash_disable:
		if (!kernel_verify_command_length(datalen,0))
			break;
		l->print(log_densoecu,"kernel flash disnable\n");
		kernel_flash_enabled = false;
		if (!listenmode)
			dc->write_kernel_rsp(kernel_rsp_flash_disable,NULL,0);
		break;
	case kernel_rsp_flash_disable:
		if (!kernel_verify_command_length(datalen,0))
			break;
		l->print(log_densoecu,"kernel response flash disabled\n");
		break;

	case kernel_cmd_write_flash_buffer:
		if (datalen < 4)
		{
			l->print(log_densoecu,"kernel write flash buffer data length invalid!\n");
			break;
		}

		addr = byte_to_int24(rxbuf);
		l->print(log_densoecu,"kernel write flash buffer addr:%06X len:%02X\n",addr,datalen-3);
		addr = addr % szpage4k;
		if (addr+datalen-3 > szpage4k)
		{
			l->print(log_densoecu,"simulation error!\n");
		}
		else
		{
			memcpy(page4k+addr,rxbuf+3,datalen-3);
			if (!listenmode)
				dc->write_kernel_rsp(kernel_rsp_write_flash_buffer,NULL,0);
		}
		break;
	case kernel_rsp_write_flash_buffer:
		if (!kernel_verify_command_length(datalen,0))
			break;
		l->print(log_densoecu,"kernel response write flash buffer OK\n");
		break;

	case kernel_cmd_commit_flash_buffer:
		if (!kernel_verify_command_length(datalen,7))
			break;
		addr = byte_to_int24(rxbuf);
		crc = byte_to_int32(rxbuf+3);
		l->print(log_densoecu,"kernel commit flash buffer addr:%06X crc:%08X\n",addr,crc);
		// verify crc
		if (crc != crc32(page4k,szpage4k))
		{
			if (!listenmode)
				dc->write_kernel_rsp(kernel_rsp_error_bad_crc,NULL,0);
			break;
		}

		// do the full simulation of writing to flash
		for (i = 0; i < szpage4k; i++)
		{
			rom[addr+i] &= page4k[i]; // Vpp only turns 1's into 0's
			// todo: delay
			if (rom[addr+i] != page4k[i] || !kernel_flash_enabled)
			{
				if (!listenmode)
					dc->write_kernel_rsp(kernel_rsp_error_programming_failure,NULL,0);
				break;
			}
		}
		if (!listenmode)
		{
			delay(330);
			rxbuf[0] = 1; // flashed in 1 attempt
			dc->write_kernel_rsp(kernel_rsp_commit_flash_buffer,rxbuf,1);
		}
		break;
	case kernel_rsp_commit_flash_buffer:
		if (!kernel_verify_command_length(datalen,1))
			break;
		l->print(log_densoecu,"kernel response commit flash buffer OK (%d pulses)\n",rxbuf[0]);

		break;

	case kernel_cmd_validate_flash_buffer:
		if (!kernel_verify_command_length(datalen,4))
			break;
		crc = byte_to_int32(rxbuf);
		l->print(log_densoecu,"kernel validate flash buffer crc:%08X\n",crc);
		// verify crc
		if (crc != crc32(page4k,szpage4k))
		{
			if (!listenmode)
				dc->write_kernel_rsp(kernel_rsp_error_bad_crc,NULL,0);
			break;
		}

		if (!listenmode)
		{
			delay(330);
			dc->write_kernel_rsp(kernel_rsp_validate_flash_buffer,NULL,0);
		}
		break;
	case kernel_rsp_validate_flash_buffer:
		if (!kernel_verify_command_length(datalen,0))
			break;
		l->print(log_densoecu,"kernel response validate flash buffer OK )\n");

		break;

	case kernel_cmd_blank_16k_page:
		if (!kernel_verify_command_length(datalen,3))
			break;
		addr = byte_to_int24(rxbuf);
		l->print(log_densoecu,"kernel blank page addr:%06X \n",addr);
		if (kernel_flash_enabled)
			memset(rom+addr,0xFF,szpage);
		if (!listenmode)
		{
			delay(330);
			if (!kernel_flash_enabled)
				dc->write_kernel_rsp(kernel_rsp_error_programming_failure,NULL,0);
			else
			{
				rxbuf[0] = 1; // blanked in 1 attempt
				dc->write_kernel_rsp(kernel_rsp_blank_16k_page,rxbuf,1);
			}
		}
		break;
	case kernel_rsp_blank_16k_page:
		if (!kernel_verify_command_length(datalen,1))
			break;
		l->print(log_densoecu,"kernel response blank page OK (%d pulses)\n",rxbuf[0]);
		break;

	default:
		if (!listenmode)
				dc->write_kernel_rsp(kernel_rsp_error_bad_command,NULL,0);
		break;
	}
	return true;
}

void densoecu::compress_mem_map(int *addr)
{
	if (*addr >= 0x28000)
		*addr -= 0x8000;
}

void densoecu::expand_mem_map(int *addr)
{
	if (*addr >= 0x20000)
		*addr += 0x8000;
}

bool densoecu::listen_denso02_bootloader()
{
	unsigned char rsp;
	unsigned int datalen = 1;
	int checksum = 0;

	dc->set_default_timeout(3000);
	if (!dc->read_serial_bytes(&rsp,&datalen))
		return false;

	checksum += rsp;
	int addr,len;

	switch(rsp)
	{
	case denso02_bootloader_cmd_start:
		l->print(log_densoecu,"denso02_bootloader_cmd_start\n");
		datalen = 1;
		if (!dc->read_serial_bytes(&rsp,&datalen))
			return false;
		checksum += rsp;
		// verify checksum
		datalen = 1;
		if (!dc->read_serial_bytes(&rsp,&datalen))
			return false;
		if (rsp != 0x100-(checksum & 0xFF))
		{
			l->print(log_densoecu,"bad checksum.\n");
			return false;
		}
		rxbuf[0] = 0x00;
		if (state == bootloader_auth)
		{
			if (!dc->write_denso02_cmd(denso02_bootloader_cmd_start,rxbuf,1))
				return false;
			set_state(bootloader);
		}
		break;
	case denso02_bootloader_cmd_write_kernel_ram:
		// read address and length
		datalen = 5;
		if (!dc->read_serial_bytes(rxbuf,&datalen))
		{
			l->print(log_densoecu,"denso write kernel area timeout!");
			return false;
		}
		checksum += dc->checksum(rxbuf,datalen);
		addr = byte_to_int24(rxbuf);
		len = byte_to_int16(rxbuf+3);
		l->print(log_densoecu,"denso write kernel area addr:%06X len:%02X\n",addr,len);
		if (addr < locrambase || addr+len > locrambase + 0x1800)
		{
			l->print(log_densoecu,"bad address range.\n");
			rsp = denso02_bootloader_rsp_bad_address;
			if (!dc->write_raw(&rsp,1))
				return false;
		}
		/*
		// todo: verify kernel length, return 0x22 if overrun
		if (len > datalen - 5)
		{
			if (!dc->write_raw(densocomm::denso02_bootloader_rsp_data_overrun,1))
				return false;
		}
		*/
		// now read data block
		datalen = len;
		if (!dc->read_serial_bytes(rxbuf,&datalen))
			return false;
		checksum += dc->checksum(rxbuf,datalen);
		// verify checksum
		datalen = 1;
		if (!dc->read_serial_bytes(&rsp,&datalen))
			return false;
		if (rsp != 0x100-(checksum & 0xFF))
		{
			l->print(log_densoecu,"bad checksum.\n");
			// todo: send 0x03
			rsp = denso02_bootloader_rsp_bad_checksum;
			if (!dc->write_raw(&rsp,1))
				return false;
			return false;
		}

		transform_kernel_block02(rxbuf,len,0,false);
		memcpy(rom+addr,rxbuf,len);
		// verify valid kernel
		if (byte_to_int16(rom+locrambase+2) != denso_magic_number)
		{
			// invalid_kernel magic number
			l->print(log_densoecu,"invalid magic number.\n");
			rsp = denso02_bootloader_rsp_bad_magic_number;
			if (!dc->write_raw(&rsp,1))
				return false;
		}
		l->print(log_densoecu,"valid kernel loaded.\n");

		delay(100);
		set_state(oecukernel);
		break;
	default:
		l->print(log_densoecu,"unk [%02X]\n",rsp);
	}
	return true;
}


bool densoecu::listen_denso()
{
	unsigned char rsp;
	int datalen = szrxbuf;

	if (model == wrx2002 && state == normal)
	{
		// look for 200ms pulse DSR/DTE (for Denso init sequence)

		if (!dc->is_LE_high())
		{
			for (int d = 0; d < 300; d+=10)
			{
				if (dc->is_LE_high())
					break;
				delay(10);
			}

			if (dc->is_LE_high())
			{
				l->print(log_densoecu,"LE low pulse\n");
				// correctly signalled
				set_state(bootloader_auth);
				return true;
			}
		}
	}

	if (model == wrx2002 && state == normal)
		dc->set_default_timeout(100);

	if (!dc->read_denso_rsp_varlen(&rsp,rxbuf,&datalen))
	{
		dc->set_default_timeout(3000);
		return false;
	}
	dc->set_default_timeout(3000);

	int addr;
	dump(rsp,rxbuf,datalen);

	switch(rsp)
	{
	case denso_write_ram:
		addr = byte_to_int24(rxbuf);
		l->print(log_densoecu,"denso write ram addr:%06X len:%02X\n",addr,datalen-3);
		if (addr < locrambase || addr+datalen-3 > szram+locrambase)
			respond_denso_fail();
		else
		{
			memcpy(&rom[addr-locrambase],&rxbuf[3],datalen-3);
			if (!listenmode)
				dc->write_denso_rsp(denso_rsp_write_ram,&rxbuf[3],datalen-3);
		}
		break;
	case denso_rsp_write_ram:
		addr = byte_to_int24(rxbuf);
		l->print(log_densoecu,"denso response write ram addr:%06X len:%02X\n",addr,datalen);
		break;
	case denso_reset_trouble_codes:
		l->print(log_densoecu,"denso reset trouble codesh\n");
		respond_denso_ok();
		break;
	case denso_rsp_reset_trouble_codes:
		l->print(log_densoecu,"denso response reset trouble codesh\n");
		break;
	case denso_read_param:
		l->print(log_densoecu,"denso read param %02x\n",rxbuf[0]);
		if (!listenmode)
		{
			// fake params for now
			//case param_boost:
			rxbuf[0] = 77;
			dc->write_denso_rsp(denso_rsp_read_param,rxbuf,1);
		}
		break;
	case denso_rsp_read_param:
		l->print(log_densoecu,"denso response read param %02X : %02X\n",rxbuf[0],rxbuf[1]);
		break;
	case denso_0x81:
		l->print(log_densoecu,"denso command 0x81\n");
		if (!listenmode)
		{
			rxbuf[0] = 0xEF;
			rxbuf[1] = 0x8F;
			dc->write_denso_rsp(denso_rsp_0x81,rxbuf,2);
		}
		break;
	case denso_rsp_0x81:
		l->print(log_densoecu,"denso response 0xC1 %02X %02X\n",rxbuf[0],rxbuf[1]);
		break;
	case denso_0x83:
		l->print(log_densoecu,"denso command 0x83 %02X\n",rxbuf[0]);
		if (!listenmode)
		{
			rxbuf[0] = 0x00;
			rxbuf[1] = 0x00;
			rxbuf[2] = 0xFF;
			rxbuf[3] = 0x00;
			rxbuf[4] = 0xFF;
			rxbuf[5] = 0x00;
			dc->write_denso_rsp(denso_rsp_0x83,rxbuf,6);
		}
		break;
	case denso_0x30:
		respond_denso_ok();
		break;
	case denso_0x10:
		l->print(log_densoecu,"denso reflash state\n");
		if (state == bootloader_auth)
		{
			respond_denso_ok();
			// now we should change into reflash state, 
			set_state(bootloader);
		}
		else
			respond_denso_fail();
		break;
	case denso_cmd_challenge:
		switch(rxbuf[0])
		{
		case 1: // get challenge number
			l->print(log_densoecu,"denso get challenge\n");
			if (!listenmode)
			{
				rxbuf[1] = rand() % 256;
				rxbuf[2] = rand() % 256;
				rxbuf[3] = rand() % 256;
				rxbuf[4] = rand() % 256;
				memcpy(lastchallenge,rxbuf+1,4);
				crazy_transform(lastchallenge);
				dc->write_denso_rsp(denso_rsp_challenge,rxbuf,5);
			}
			break;
		case 2: // responding to challenge
			l->print(log_densoecu,"denso respond to challenge %02X %02X %02X %02X\n",rxbuf[1],rxbuf[2],rxbuf[3],rxbuf[4]);
			if (!listenmode)
			{// test validity
				if (memcmp(lastchallenge,rxbuf+1,4) == 0)
				{
					l->print(log_densoecu,"denso challenge is correct\n");
					rxbuf[1] = 0x34;
					dc->write_denso_rsp(denso_rsp_challenge,rxbuf,2);
					set_state(bootloader_auth);
				}
				else
				{
					l->print(log_densoecu,"denso challenge is incorrect\n");
					l->print(log_densoecu,"correct response is %02X %02X %02X %02X\n",lastchallenge[0],lastchallenge[1],lastchallenge[2],lastchallenge[3]);
					respond_denso_fail();
				}
			}
			break;
		}
	case denso_rsp_challenge:
		l->print(log_densoecu,"denso response challenge\n");
		break;
	}
	lastrsp = rsp;
	return true;
}


bool densoecu::listen_denso_bootloader()
{
	unsigned char rsp;
	int datalen = szrxbuf;

	if (!dc->read_denso_rsp_varlen(&rsp,rxbuf,&datalen))
	{
		dc->set_default_timeout(3000);
		return false;
	}
	dc->set_default_timeout(3000);

	int addr;
	dump(rsp,rxbuf,datalen);

	switch(rsp)
	{
	case denso_bootloader_cmd_write_kernel_ram:
		addr = byte_to_int24(rxbuf);
		l->print(log_densoecu,"denso write flash addr:%06X len:%02X\n",addr,datalen-3);
		if (!listenmode)
		{
		if (addr < validate_addr 
			|| addr+datalen-3 > validate_addr+validate_len 
			|| addr+datalen-3 > szram+locrambase)
				respond_denso_fail();
			else
			{
				transform_kernel_block04(rxbuf+3,datalen-3,false);
				memcpy(rom+addr,rxbuf+3,datalen-3);
				dc->write_denso_rsp(denso_bootloader_rsp_write_kernel_ram,NULL,0);
			}
		}
		break;
	case denso_bootloader_rsp_write_kernel_ram:
		addr = byte_to_int24(rxbuf);
		l->print(log_densoecu,"denso response write flash addr:%06X len:%02X\n",addr,datalen);
		break;
	case denso_bootloader_cmd_enter_kernel:
		l->print(log_densoecu,"denso enter kernel\n");
		// todo: verify flash mode

		// verify correct parameters
		if (datalen != 2 || byte_to_int16(rxbuf) != denso_bootloader_rsp_ok)
		{
			l->print(log_densoecu,"enter kernel: invalid parameters.\n");
			// this actually uses bad checksum error code
			int16_to_byte(rxbuf,denso_bootloader_rsp_error_bad_checksum);
			dc->write_denso_rsp_echocheck(denso_bootloader_rsp_error,rxbuf,2);
		}
		// verify valid kernel
		else if (bootloader_calculate_odd_checksum(rom+validate_addr,validate_len) != denso_odd_checksum)
		{
			// invalid_kernel magic number
			l->print(log_densoecu,"invalid checksum.\n");
			int16_to_byte(rxbuf,denso_bootloader_rsp_error_bad_checksum);
			dc->write_denso_rsp_echocheck(denso_bootloader_rsp_error,rxbuf,2);
		}
		else
		{

			l->print(log_densoecu,"valid kernel loaded.\n");
			int16_to_byte(rxbuf,denso_bootloader_rsp_ok);
			dc->write_denso_rsp_echocheck(denso_bootloader_rsp_enter_kernel,rxbuf,2);
			// now we should change into kernel state, 
			set_state(oecukernel);
		}
		break;	
	case denso_bootloader_rsp_enter_kernel:
		if (datalen == 2)
			l->print(log_densoecu,"denso response enter kernel: [%02X} [%02X]\n",rxbuf[0],rxbuf[1]);
		break;
	case denso_bootloader_validate_kram_addr:
		validate_addr = byte_to_int24(rxbuf);
		validate_len = byte_to_int16(&rxbuf[5]);
		l->print(log_densoecu,"denso validate flash addr:%06X len:%04X\n",validate_addr,validate_len);
		if (lastrsp != denso_0x10 && !listenmode)
		{
			rxbuf[0] = 0x84;
			dc->write_denso_rsp(denso_bootloader_rsp_validate_kram_addr,rxbuf,1);
		}
		break;
	case denso_bootloader_rsp_validate_kram_addr:
		l->print(log_densoecu,"denso response blank flash %02X\n",rxbuf[0]);
		break;
	}
	lastrsp = rsp;
	return true;
}

int densoecu::byte_to_int32(unsigned char *data)
{
	return (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
}

int densoecu::byte_to_int24(unsigned char *data)
{
	return (data[0] << 16) + (data[1] << 8) + data[2];
}

int densoecu::byte_to_int16(unsigned char *data)
{
	return (data[0] << 8) + data[1];
}

void densoecu::int16_to_byte(unsigned char *data,int i)
{
	data[0] = i >> 8;
	data[1] = i & 0xFF;
}

void densoecu::int24_to_byte(unsigned char *data,int i)
{
	data[0] = i >> 16;
	data[1] = (i >> 8) & 0xFF;
	data[2] = i & 0xFF;
}

void densoecu::int32_to_byte(unsigned char *data,int i)
{
	data[0] = i >> 24;
	data[1] = (i >> 16) & 0xFF;
	data[2] = (i >> 8) & 0xFF;
	data[3] = i & 0xFF;
}

bool densoecu::respond_denso_ok()
{
	if (listenmode)
		return true;

	return dc->write_denso_rsp(denso_rsp_ok,NULL,0);
}

bool densoecu::respond_denso_fail()
{
	if (listenmode)
		return true;

	return dc->write_denso_rsp(denso_rsp_fail,NULL,0);
}

void densoecu::delay(int ms)
{
	if (listenmode)
		return; // don't do delays if listening

	Sleep(ms);
}

void densoecu::dump(unsigned char rsp,unsigned char *data,int datalen)
{
	int szbuf = datalen*4+64;
	char *buf = new char[szbuf];
	char *pbuf = buf;

	pbuf += _snprintf(pbuf,szbuf-(pbuf-buf),"[%02X] ",rsp);
	for (int i = 0; i < datalen; i++)
	{
		if (i > 15 && (i & 0xF) == 0)
			pbuf += _snprintf(pbuf,szbuf-(pbuf-buf),"\n   ");
		pbuf += _snprintf(pbuf,szbuf-(pbuf-buf),"%02X ",data[i]);
	}
	pbuf += _snprintf(pbuf,szbuf-(pbuf-buf),"\n");
	l->print(log_densoecu,buf);
	delete buf;
}

bool densoecu::load_ram_from_file(string filename)
{
	FILE *fp;
	if (NULL == (fp = fopen(filename.c_str(),"rb")))
	{
		l->print(log_densoecu,"can't open ram input file.\n");
		return false;
	}

	fseek(fp,0,SEEK_END);
	int filesize = min(szram,ftell(fp));
	fseek(fp,0,SEEK_SET);

	if (filesize != fread(rom+locrambase,1,filesize,fp))
	{
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

bool densoecu::save_ram_to_file(string filename)
{
	FILE *fp;
	if (NULL == (fp = fopen(filename.c_str(),"wb")))
	{
		l->print(log_densoecu,"can't open ram output file.\n");
		return false;
	}

	if (szram != fwrite(rom+locrambase,1,szram,fp))
	{
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

bool densoecu::load_rom_from_file(string filename)
{
	FILE *fp;
	if (NULL == (fp = fopen(filename.c_str(),"rb")))
	{
		l->print(log_densoecu,"can't open rom input file.\n");
		return false;
	}

	fseek(fp,0,SEEK_END);
	int filesize = min(szrom,ftell(fp));
	fseek(fp,0,SEEK_SET);

	if (filesize != fread(rom,1,filesize,fp))
	{
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

bool densoecu::save_rom_to_file(string filename)
{
	FILE *fp;
	if (NULL == (fp = fopen(filename.c_str(),"wb")))
	{
		l->print(log_densoecu,"can't open rom output file.\n");
		return false;
	}

	if (szrom != fwrite(rom,1,szrom,fp))
	{
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

void densoecu::crazy_transform(unsigned char *data)
{
	int table2[16] = 
	{
		0x53DA, 0x33BC, 0x72EB, 0x437D,
		0x7CA3, 0x3382, 0x834F, 0x3608,
		0xAFB8, 0x503D, 0xDBA3, 0x9D34,
		0x3563, 0x6B70, 0x6E74, 0x88F0
	};

	int loword = byte_to_int16(&data[2]);
	int hiword = byte_to_int16(&data[0]);
	int num;
	for (int i = 15; i >= 0; i--)
	{
		num = table2[i];
		num ^= loword;
		num = transformnybbles(num);
		num = ((num >> 3) + (num << 13)) & 0xFFFF; // barrel shift right 3
		num ^= hiword;
		hiword = loword;
		loword = num;
	}

	// words reversed in output for some reason
	int16_to_byte(&data[2],hiword);
	int16_to_byte(&data[0],loword);
}

int densoecu::transformnybbles(int n)
{
	int table1[32] =
	{
		0x5, 0x6, 0x7, 0x1,  
		0x9, 0xC, 0xD, 0x8, 
		0xA, 0xD, 0x2, 0xB,
		0xF, 0x4, 0x0, 0x3,
		0xB, 0x4, 0x6, 0x0, 
		0xF, 0x2, 0xD, 0x9, 
		0x5, 0xC, 0x1, 0xA,
		0x3, 0xD, 0xE, 0x8
	};

	int result = 0;
	// fix n for simulated wrap-around
	n += (n & 0xFF) << 16;

	for (int i = 0; i < 4; i++)
	{
		result += (table1[(n >> (i*4)) % 32]) << (i*4);
	}

	return result;
}

bool densoecu::write_denso_ram_area(unsigned char *data,int addr,int len)
{
	l->print(log_densoecu,"writing denso ram area addr: %06X len: %04X\n",addr,len);
	delay(200);
	for (int i = 0; i < len;)
	{
		int blksz = min(len-i,64);
		if (!write_denso_ram_block(&data[i],addr+i,blksz))
			return false;
		i += blksz;
	}
	return true;
}

bool densoecu::write_denso_ram_block(unsigned char *data,int addr,int len)
{
	int datalen = len+3;
	unsigned char rsp;
	unsigned char *cmddata = new unsigned char[datalen];
	unsigned char *rxbuf = new unsigned char[datalen];

	l->print(log_densoecu,"writing denso ram block addr: %06X len: %04X\n",addr,len);
	int24_to_byte(cmddata,addr);
	memcpy(&cmddata[3],data,len);
	if (!dc->write_denso_cmd_echocheck(denso_write_ram,cmddata,datalen))
		return false;
	if (!dc->read_denso_rsp_varlen(&rsp,rxbuf,&datalen))
		return false;
	if (rsp != denso_rsp_write_ram)
		return false;
	if (datalen != len)
		return false;
	if (memcmp(data,rxbuf,datalen) != 0)
		return false;
	return true;
}

bool densoecu::write_denso_kernel_ram_area(unsigned char *data,int addr,int len)
{
	int i;

	l->print(log_densoecu,"writing denso kernel area addr: %06X len: %04X\n",addr,len);
	switch(model)
	{
	case wrx2004:
		for (i = 0; i < len;)
		{
			int blksz = min(len-i,128);
			delay(200);
			if (!write_denso_kernel_ram_block(&data[i],addr+i,blksz))
				return false;
			i += blksz;
		}
	case wrx2002:
		if (!denso02_write_kernel_ram(data,addr,len))
			return false;
		break;
	default:
		return false; // not supported
	}
	return true;
}

bool densoecu::denso02_write_kernel_ram(unsigned char *data,int addr,int len)
{
	unsigned char *txbuf = new unsigned char[len+5];
	int24_to_byte(txbuf,addr);
	int16_to_byte(txbuf+3,len);
	memcpy(txbuf+5,data,len);
	if (!dc->write_denso02_cmd_echocheck(denso02_bootloader_cmd_write_kernel_ram,txbuf,len+5))
	{
		l->print(log_densoecu,"flash failed echo test\n");
		delete txbuf;
		return false;
	}
	delete txbuf;
	return true;
}

bool densoecu::write_denso_kernel_ram_block(unsigned char *data,int addr,int len)
{
	int datalen = len+3;
	unsigned char rsp;
	unsigned char *cmddata = new unsigned char[datalen];

	l->print(log_densoecu,"writing denso flash block addr: %06X len: %04X\n",addr,len);
	int24_to_byte(cmddata,addr);
	memcpy(&cmddata[3],data,len);
	if (!dc->write_denso_cmd_echocheck(denso_bootloader_cmd_write_kernel_ram,cmddata,datalen))
		return false;
	datalen = 0;
	if (!dc->read_denso_rsp_varlen(&rsp,rxbuf,&datalen))
		return false;
	if (rsp != denso_bootloader_rsp_write_kernel_ram)
		return false;
	return true;
}

bool densoecu::check_for_ecu()
{
	return denso_cmd_0x81();
}

bool densoecu::enter_kernel()
{
	unsigned char data[2] = {0x01,0x01};
	l->print(log_densoecu,"entering kernel\n");
	if (!dc->write_denso_cmd_echocheck(denso_bootloader_cmd_enter_kernel,data,2))
		return false;
	return true;
}

bool densoecu::validate_kernel_ram(int addr,int len)
{
	int datalen;
	unsigned char rsp;
	unsigned char cmddata[7];
	unsigned char rxbuf[7];

	l->print(log_densoecu,"validating denso kernel ram area addr: %06X len: %04X\n",addr,len);
	int24_to_byte(cmddata,addr);
	cmddata[3] = 0x04;
	cmddata[4] = 0x00;
	int16_to_byte(cmddata+5,len);
	if (!dc->write_denso_cmd_echocheck(denso_bootloader_validate_kram_addr,cmddata,7))
		return false;
	datalen = 7;
	if (!dc->read_denso_rsp_varlen(&rsp,rxbuf,&datalen))
		return false;
	if (rsp != denso_bootloader_rsp_validate_kram_addr)
		return false;
	if (datalen != 1)
		return false;
	if (rxbuf[0] != denso_bootloader_rsp_validate_kram_addr_success)
		return false;
	return true;
}

bool densoecu::kernel_CRC32_area(unsigned int *CRC32,int addr,int len)
{
	unsigned char cmddata[5];
	unsigned char rxbuf[4];

//	l->print(log_densoecu,"kernel CRC32 area: addr: %06X len: %04X\n",addr,len);
	int24_to_byte(cmddata,addr);
	int16_to_byte(cmddata+3,len);
	if (!dc->write_kernel_cmd_echocheck(kernel_cmd_CRC_area,cmddata,5))
		return false;

	if (!dc->read_kernel_specific_rsp(kernel_rsp_CRC_area,rxbuf,4))
		return false;

	*CRC32 = byte_to_int32(rxbuf);
	return true;
}

bool densoecu::kernel_read_area(unsigned char *data,int addr,int len)
{
	unsigned char cmddata[5];

	l->print(log_densoecu,"kernel read area: addr: %06X len: %04X\n",addr,len);
	int24_to_byte(cmddata,addr);
	int16_to_byte(cmddata+3,len);
	if (!dc->write_kernel_cmd_echocheck(kernel_cmd_read_area,cmddata,5))
		return false;
	if (!dc->read_kernel_specific_rsp(kernel_rsp_read_area,data,len))
		return false;
	return true;
}

bool densoecu::kernel_get_version(string *version)
{
	unsigned char versioninfo[256];
	int len = 256;

	l->print(log_densoecu,"kernel get version\n");
	if (!dc->write_kernel_cmd_echocheck(kernel_cmd_get_version_info,NULL,0))
		return false;
	if (!dc->read_kernel_specific_rsp_varlen(kernel_rsp_get_version_info,versioninfo,&len))
		return false;

	versioninfo[len] = 0;
	*version = (char *)versioninfo;
	return true;
}

bool densoecu::kernel_read_programming_voltage(double *voltage)
{
	unsigned char rxbuf[2];

	l->print(log_densoecu,"reading ecu programming voltage\n");
	if (!dc->write_kernel_cmd_echocheck(kernel_cmd_read_programming_voltage,NULL,0))
		return false;
	if (!dc->read_kernel_specific_rsp(kernel_rsp_read_programming_voltage,rxbuf,2))
		return false;

	*voltage = byte_to_int16(rxbuf) / 50.0;
	return true;
}

bool densoecu::kernel_flash_enable()
{
	l->print(log_densoecu,"kernel flash enable\n");
	if (!dc->write_kernel_cmd_echocheck(kernel_cmd_flash_enable,NULL,0))
		return false;
	if (!dc->read_kernel_specific_rsp(kernel_rsp_flash_enable,NULL,0))
		return false;
	return true;
}

bool densoecu::kernel_flash_disable()
{
	l->print(log_densoecu,"kernel flash disable\n");
	if (!dc->write_kernel_cmd_echocheck(kernel_cmd_flash_disable,NULL,0))
		return false;
	if (!dc->read_kernel_specific_rsp(kernel_rsp_flash_disable,NULL,0))
		return false;
	return true;
}

bool densoecu::kernel_write_flash_buffer(unsigned char *data,int addr,int len)
{
	int datalen = len+3;
	unsigned char *cmddata = new unsigned char[datalen];

	l->print(log_densoecu,"kernel write flash buffer addr: %06X len: %04X\n",addr,len);
	int24_to_byte(cmddata,addr);
	memcpy(&cmddata[3],data,len);
	if (!dc->write_kernel_cmd_echocheck(kernel_cmd_write_flash_buffer,cmddata,datalen))
		return false;
	datalen = 0;
	if (!dc->read_kernel_specific_rsp(kernel_rsp_write_flash_buffer,NULL,0))
	{
		return false;
	}
	return true;
}

bool densoecu::kernel_validate_flash_buffer(unsigned int crc)
{
	unsigned char cmddata[4];

	l->print(log_densoecu,"kernel validate flash crc32: %08x\n",crc);
	int32_to_byte(cmddata,crc);
	if (!dc->write_kernel_cmd_echocheck(kernel_cmd_validate_flash_buffer,cmddata,4))
		return false;
	if (!dc->read_kernel_specific_rsp(kernel_rsp_validate_flash_buffer,NULL,0))
	{
		l->print(log_densoecu,"failed to validate flash buffer!\n");
		return false;
	}
	return true;
}

bool densoecu::kernel_commit_flash_buffer(int addr,unsigned int crc)
{
	unsigned char cmddata[7];

	l->print(log_densoecu,"kernel commit flash addr: %06X crc32 %08x\n",addr,crc);
	int24_to_byte(cmddata,addr);
	int32_to_byte(cmddata+3,crc);
	if (!dc->write_kernel_cmd_echocheck(kernel_cmd_commit_flash_buffer,cmddata,7))
		return false;
	if (!dc->read_kernel_specific_rsp(kernel_rsp_commit_flash_buffer,cmddata,1))
	{
		l->print(log_densoecu,"failed to commit flash at %06X!\n",addr);
		return false;
	}
	return true;
}

bool densoecu::kernel_blank_16k_page(int addr)
{
	unsigned char cmddata[3];

	l->print(log_densoecu,"kernel blank 16k flash page: addr: %06X\n",addr);
	int24_to_byte(cmddata,addr);
	if (!dc->write_kernel_cmd_echocheck(kernel_cmd_blank_16k_page,cmddata,3))
		return false;
	if (!dc->read_kernel_specific_rsp(kernel_rsp_blank_16k_page,cmddata,1))
	{
		l->print(log_densoecu,"failed to erase 16k page at %06X!\n",addr);
		return false;
	}
	return true;
}

bool densoecu::denso_cmd_0x81()
{
	int datalen = 3;
	unsigned char rsp;
	unsigned char rxbuf[3];

	l->print(log_densoecu,"denso_cmd_0x81\n");

	if (!dc->write_denso_cmd_echocheck(denso_0x81,NULL,0))
		return false;
	if (!dc->read_denso_rsp_varlen(&rsp,rxbuf,&datalen))
		return false;
	if (rsp != denso_rsp_0x81)
		return false;
	if (datalen != 2)
		return false;
	return true;
}

bool densoecu::denso_cmd_0x83()
{
	int datalen = 6;
	unsigned char rsp;
	unsigned char cmdbuf[1] = {0x00};
	unsigned char rxbuf[6];

	l->print(log_densoecu,"denso_cmd_0x83\n");

	if (!dc->write_denso_cmd_echocheck(denso_0x83,cmdbuf,1))
		return false;
	if (!dc->read_denso_rsp_varlen(&rsp,rxbuf,&datalen))
		return false;
	if (rsp != denso_rsp_0x83)
		return false;
	if (datalen != 6)
		return false;
	return true;
}

bool densoecu::denso_cmd_0x10()
{
	unsigned char cmdbuf[2] = {0x85,0x02};

	l->print(log_densoecu,"denso_cmd_0x10\n");

	if (!dc->write_denso_cmd_echocheck(denso_0x10,cmdbuf,2))
		return false;
	return true;
}

bool densoecu::denso_bootloader_cmd_start()
{
	unsigned int datalen = 3;
	unsigned char cmdbuf[1] = {0xFF};
	unsigned char rxbuf[3];

	l->print(log_densoecu,"denso02_bootloader_cmd_start\n");

	if (!dc->write_denso02_cmd_echocheck(denso02_bootloader_cmd_start,cmdbuf,1))
		return false;
	if (!dc->read_serial_bytes(rxbuf,&datalen))
		return false;
	if (rxbuf[0] != denso02_bootloader_cmd_start
		|| rxbuf[1] != 0x00
		|| rxbuf[2] != 0xB3)
		return false;
	return true;
}

bool densoecu::enter_flash_mode()
{
	switch(model)
	{
	case wrx2002:
		return enter_flash_mode02();
	case wrx2004:
		return enter_flash_mode04();
	default:
		return false; // not supported
	}
}

bool densoecu::enter_flash_mode02()
{
	set_state(bootloader_auth);
	for (int i = 0; i < 5; i++)
	{
		dc->pulse_LE();
		if (denso_bootloader_cmd_start())
		{
			set_state(bootloader);
			return true;
		}
	}
	// are we in the oecu kernel already?
	set_state(oecukernel);
	if (kernel_flash_disable())
	{
		l->print(log_densoecu,"found oecu kernel!\n");
		return true;
	}

	set_state(normal);
	return false;
}

bool densoecu::enter_flash_mode04()
{
	set_state(normal);
	delay(200);
	if (!denso_cmd_0x81())
	{
		// see if the ecu isn't already in the flash mode (failure?)
		set_state(bootloader);
		delay(200);
		if (validate_kernel_ram(locrambase,8))
			return true; // already in mode

		// are we in the oecu kernel already?
		set_state(oecukernel);
		delay(200);
		if (kernel_flash_disable())
			return true;

		set_state(normal);
		return false;
	}
	if (!denso_cmd_0x83())
		return false;
	if (!do_challenge_response())
		return false;
	if (!denso_cmd_0x10())
		return false;
	set_state(bootloader);
	return true;
}

bool densoecu::do_challenge_response()
{
	int datalen;
	unsigned char rsp;
	unsigned char cmdbuf[5] = {0x01};
	unsigned char rxbuf[5];
	unsigned char challenge[4];

	l->print(log_densoecu,"denso challenge-response\n");

	if (!dc->write_denso_cmd_echocheck(denso_cmd_challenge,cmdbuf,1))
		return false;
	datalen = 5;
	if (!dc->read_denso_rsp_varlen(&rsp,rxbuf,&datalen))
		return false;
	if (rsp != denso_rsp_challenge)
		return false;
	if (datalen != 5)
		return false;
	if (rxbuf[0] != 0x01)
		return false;
	memcpy(challenge,rxbuf+1,4);
	crazy_transform(challenge);
	cmdbuf[0] = 0x02;
	memcpy(cmdbuf+1,challenge,4);
	if (!dc->write_denso_cmd_echocheck(denso_cmd_challenge,cmdbuf,5))
		return false;
	datalen = 5;
	if (!dc->read_denso_rsp_varlen(&rsp,rxbuf,&datalen))
		return false;
	if (rsp != denso_rsp_challenge)
		return false;
	if (datalen != 2)
		return false;
	if (rxbuf[0] != 0x02)
		return false;
	if (rxbuf[1] != 0x34)
		return false;
	return true;
}

void densoecu::barrelshift16right(unsigned short *barrel)
{
	if (*barrel & 1)
		*barrel = (*barrel >> 1) + 0x8000;
	else
		*barrel = *barrel >> 1;
}

void densoecu::transform_kernel_block04(unsigned char *data,int length,bool doencrypt)
{
	unsigned short crypto_tableA[4] = 
	{
		0x7856, 0xCE22, 0xF513, 0x6E86
	};

	unsigned char crypto_tableB[32] = 
	{
		0x5, 0x6, 0x7, 0x1, 0x9, 0xC, 0xD, 0x8, 
		0xA, 0xD, 0x2, 0xB, 0xF, 0x4, 0x0, 0x3,
		0xB, 0x4, 0x6, 0x0, 0xF, 0x2, 0xD, 0x9,
		0x5, 0xC, 0x1, 0xA, 0x3, 0xD, 0xE, 0x8,
	};

	int i,j;
	unsigned short word1,word2,idx2,temp,key16;

	for (i = 0; i < length; i += 4)
	{

		if (doencrypt)
		{
			word2 = (*(data+0) << 8) + *(data+1);
			word1 = (*(data+2) << 8) + *(data+3);
			for (j = 1; j <= 4; j++)
			{
				idx2 = word1 ^ crypto_tableA[j-1];
				key16 =
						crypto_tableB[(idx2 >> 0) & 0x1F]
					+ (crypto_tableB[(idx2 >> 4) & 0x1F] << 4)
					+ (crypto_tableB[(idx2 >> 8) & 0x1F] << 8)
					+ (crypto_tableB[(((idx2 & 0x1) << 4) + (idx2 >> 12)) & 0x1F] << 12);
				barrelshift16right(&key16);
				barrelshift16right(&key16);
				barrelshift16right(&key16);

				temp = word1;
				word1 = key16 ^ word2;
				word2 = temp;

			}
			*(data+0) = word1 >> 8;
			*(data+1) = word1 & 0xFF;
			*(data+2) = word2 >> 8;
			*(data+3) = word2 & 0xFF;
		}
		else
		{
			word1 = (*(data+0) << 8) + *(data+1);
			word2 = (*(data+2) << 8) + *(data+3);
			for (j = 4; j > 0; j--)
			{
				idx2 = word2 ^ crypto_tableA[j-1];
				key16 =
						crypto_tableB[(idx2 >> 0) & 0x1F]
					+ (crypto_tableB[(idx2 >> 4) & 0x1F] << 4)
					+ (crypto_tableB[(idx2 >> 8) & 0x1F] << 8)
					+ (crypto_tableB[(((idx2 & 0x1) << 4) + (idx2 >> 12)) & 0x1F] << 12);
				barrelshift16right(&key16);
				barrelshift16right(&key16);
				barrelshift16right(&key16);
				temp = word2;
				word2 = key16 ^ word1;
				word1 = temp;
			}
			*(data+0) = word2 >> 8;
			*(data+1) = word2 & 0xFF;
			*(data+2) = word1 >> 8;
			*(data+3) = word1 & 0xFF;
		}

		data += 4;
	}
}

void densoecu::transform_kernel_block02(unsigned char *data,int length,int offset,bool doencrypt)
{
	int n;
	int decrypt[16] = 
	{
		0xA, 0x5, 0x4, 0x7,
		0x6, 0x1, 0x0, 0x3,
		0x2, 0xD, 0xC, 0xF,
		0xE, 0x9, 0x8, 0xB
	};
	int encrypt[16] = 
	{
		0x6, 0x5, 0x8, 0x7,
		0x2, 0x1, 0x4, 0x3,
		0xE, 0xD, 0x0, 0xF,
		0xA, 0x9, 0xC, 0xB
	};


	for (int i = 0; i < length; i++, data++)
	{
		if (i+offset == 2 || i+offset == 3)
			continue; // don't transform these two bytes

		n = (*data & 0x0F) ^ 0x05; // lower nybble is XORed with 0x05

		// upper nybble is transformed using above maps
		if (doencrypt)
			n += encrypt[*data >> 4] << 4;
		else
			n += decrypt[*data >> 4] << 4;
		
		*data = n; 
	}
}

static unsigned int crc_table[256] = {
   0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
   0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
   0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
   0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
   0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
   0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
   0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
   0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
   0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
   0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
   0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
   0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
   0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
   0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
   0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
   0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
   0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
   0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
   0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
   0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
   0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
   0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
   0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
   0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
   0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
   0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
   0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
   0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
   0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
   0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
   0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
   0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
   0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
   0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
   0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
   0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
   0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
   0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
   0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
   0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
   0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
   0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
   0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
   0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
   0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
   0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
   0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
   0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
   0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
   0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
   0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
   0x2d02ef8dL
};
 
unsigned int densoecu::crc32(const unsigned char *buf, unsigned int len)
{
	unsigned int crc = 0xFFFFFFFF;
	if (buf == NULL) 
		return 0L;
	while (len--)
		crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);

	return crc ^ 0xFFFFFFFF;
}

int densoecu::bootloader_calculate_odd_checksum(unsigned char *data,int len)
{
	int sum = 0;
	for (int i = 2; i < len; i += 4)
		sum += byte_to_int16(data+i);
	return sum;
}

bool densoecu::is_programming_voltage_safe(double voltage)
{
	return (voltage > 11.4 && voltage < 12.6);
}

bool densoecu::enable_openport(unsigned int fn)
{
	return dc->enable_openport(fn);
}