#include "StdAfx.h"
#include ".\densocomm.h"

densocomm::densocomm(string portname)
{
	s.set_baud(4800);
	s.set_port(portname);
	l = new log();
}

densocomm::~densocomm(void)
{
	s.close();
}

void densocomm::addlog(log *_l)
{
	delete l;
	l = _l;
}

bool densocomm::open()
{
	if (s.open())
	{
		s.set_status(SETDTR);
		s.flush();
		return true;
	}
	return false;
}

void densocomm::close()
{
	s.close();
}

void densocomm::set_baud(int baudrate)
{
	s.set_baud(baudrate);
	s.flush();
}

void densocomm::set_default_timeout(int to)
{
	s.set_default_timeout(to);
}

bool densocomm::LE_low()
{
	s.set_status(SETDTR);
	return true;
}

bool densocomm::LE_high()
{
	s.set_status(CLRDTR);
	return true;
}

bool densocomm::pulse_LE()
{
	s.set_status(SETDTR);
	Sleep(500);
	s.set_status(CLRDTR);
	Sleep(200);
	s.set_status(SETDTR);
	Sleep(200);
	return true;
}

bool densocomm::is_LE_high()
{
	// OpenPort uses DSR line to turn on and off L line
	unsigned int status = s.get_status();
	if (status & MS_DSR_ON)
		return true;
	else
		return false;
}

bool densocomm::write_kernel_cmd_echocheck(unsigned char cmd,unsigned char *data,int datalen)
{
	unsigned char *d = new unsigned char[datalen+6];
	unsigned char *r = new unsigned char[datalen+6];
	unsigned char rsp;
	int rsplen = datalen+6;

	d[0] = 0xBE;
	d[1] = 0xEF;
	d[2] = (datalen+1) >> 8;
	d[3] = (datalen+1) & 0xFF;
	d[4] = cmd;
	for (int i = 0; i < datalen; i++)
	{
		d[i+5] = data[i];
	}

	d[datalen+5] = checksum(d,datalen+5);
	bool result = s.write(d,datalen+6);
	if (result)
	{
		read_kernel_rsp_varlen(&rsp,r,&rsplen);
		if (rsplen != datalen || rsp != cmd || memcmp(&d[5],r,datalen) != 0)
			result = false;
	}
	delete d;
	delete r;
	return result;
}

bool densocomm::write_kernel_cmd(unsigned char cmd,unsigned char *data,int datalen)
{
	unsigned char *d = new unsigned char[datalen+6];

	d[0] = 0xBE;
	d[1] = 0xEF;
	d[2] = (datalen+1) >> 8;
	d[3] = (datalen+1) & 0xFF;
	d[4] = cmd;
	for (int i = 0; i < datalen; i++)
	{
		d[i+5] = data[i];
	}

	d[datalen+5] = checksum(d,datalen+5);
	bool result = s.write(d,datalen+6);
	delete d;
	return result;
}

bool densocomm::write_kernel_rsp(unsigned char rsp,unsigned char *data,int datalen)
{
	unsigned char *d = new unsigned char[datalen+6];

	d[0] = 0xBE;
	d[1] = 0xEF;
	d[2] = (datalen+1) >> 8;
	d[3] = (datalen+1) & 0xFF;
	d[4] = rsp;
	for (int i = 0; i < datalen; i++)
	{
		d[i+5] = data[i];
	}

	d[datalen+5] = checksum(d,datalen+5);
	bool result = s.write(d,datalen+6);
	delete d;
	return result;
}

bool densocomm::write_denso_cmd(unsigned char cmd,unsigned char *data,int datalen)
{
	unsigned char *d = new unsigned char[datalen+6];

	d[0] = 0x80;
	d[1] = 0x10;
	d[2] = 0xF0;
	d[3] = datalen+1;
	d[4] = cmd;
	for (int i = 0; i < datalen; i++)
	{
		d[i+5] = data[i];
	}

	d[datalen+5] = checksum(d,datalen+5);
	bool result = s.write(d,datalen+6);
	delete d;
	return result;
}

bool densocomm::write_raw(unsigned char *data,int datalen)
{
	return s.write(data,datalen);
}

bool densocomm::write_denso_cmd_echocheck(unsigned char cmd,unsigned char *data,int datalen)
{
	unsigned char *d = new unsigned char[datalen+6];
	unsigned char *r = new unsigned char[datalen+6];
	unsigned char rsp;
	int rsplen = datalen+6;

	d[0] = 0x80;
	d[1] = 0x10;
	d[2] = 0xF0;
	d[3] = datalen+1;
	d[4] = cmd;
	for (int i = 0; i < datalen; i++)
	{
		d[i+5] = data[i];
	}

	d[datalen+5] = checksum(d,datalen+5);
	bool result = s.write(d,datalen+6);
	if (result)
	{
		read_denso_rsp_varlen(&rsp,r,&rsplen);
		if (rsplen != datalen || rsp != cmd || memcmp(&d[5],r,datalen) != 0)
			result = false;
	}
	delete d;
	delete r;
	return result;
}

bool densocomm::write_denso02_cmd_echocheck(unsigned char cmd,unsigned char *data,int datalen)
{
	unsigned char *d = new unsigned char[datalen+2];
	unsigned char *r = new unsigned char[datalen+2];
	unsigned int rsplen = datalen+2;

	d[0] = cmd;
	for (int i = 0; i < datalen; i++)
	{
		d[i+1] = data[i];
	}

	d[datalen+1] = 0x100-checksum(d,datalen+1);
	bool result = s.write(d,datalen+2);
	if (result)
	{
		if (!read_serial_bytes(r,&rsplen) || memcmp(d,r,datalen+2) != 0)
			result = false;
	}
	delete d;
	delete r;
	return result;
}

bool densocomm::write_denso02_cmd(unsigned char cmd,unsigned char *data,int datalen)
{
	unsigned char *d = new unsigned char[datalen+2];
	unsigned char *r = new unsigned char[datalen+2];
	unsigned int rsplen = datalen+2;

	d[0] = cmd;
	for (int i = 0; i < datalen; i++)
	{
		d[i+1] = data[i];
	}

	d[datalen+1] = 0x100-checksum(d,datalen+1);
	bool result = s.write(d,datalen+2);
	if (result)
	delete d;
	delete r;
	return result;
}

bool densocomm::write_denso_rsp(unsigned char cmd,unsigned char *data,int datalen)
{
	unsigned char *d = new unsigned char[datalen+6];

	d[0] = 0x80;
	d[1] = 0xF0;
	d[2] = 0x10;
	d[3] = datalen+1;
	d[4] = cmd;
	for (int i = 0; i < datalen; i++)
	{
		d[i+5] = data[i];
	}

	d[datalen+5] = checksum(d,datalen+5);
	bool result = s.write(d,datalen+6);
	delete d;
	return result;
}

bool densocomm::write_denso_rsp_echocheck(unsigned char cmd,unsigned char *data,int datalen)
{
	unsigned char *d = new unsigned char[datalen+6];
	unsigned char *r = new unsigned char[datalen+6];
	unsigned char rsp;
	int rsplen = datalen+6;

	d[0] = 0x80;
	d[1] = 0xF0;
	d[2] = 0x10;
	d[3] = datalen+1;
	d[4] = cmd;
	for (int i = 0; i < datalen; i++)
	{
		d[i+5] = data[i];
	}

	d[datalen+5] = checksum(d,datalen+5);
	bool result = s.write(d,datalen+6);
	if (result)
	{
		read_denso_rsp_varlen(&rsp,r,&rsplen);
		if (rsplen != datalen || rsp != cmd || memcmp(&d[5],r,datalen) != 0)
			result = false;
	}
	delete d;
	delete r;
	return result;
}

void densocomm::log_kernel_response(unsigned char rsp)
{
	switch(rsp)
	{
	case kernel_rsp_error_bad_data_length:
		l->print(log_densocomm,"kernel error: bad data length\n");
		break;
	case kernel_rsp_error_bad_data_value:
		l->print(log_densocomm,"kernel error: bad data value\n");
		break;
	case kernel_rsp_error_programming_failure:
		l->print(log_densocomm,"kernel error: bad data length\n");
		break;
	case kernel_rsp_error_prog_voltage_low:
		l->print(log_densocomm,"kernel error: programming voltage is too low\n");
		break;
	case kernel_rsp_error_prog_voltage_high:
		l->print(log_densocomm,"kernel error: programming voltage is too high\n");
		break;
	case kernel_rsp_error_bad_crc:
		l->print(log_densocomm,"kernel error: crc32 does not match\n");
		break;
	case kernel_rsp_error_bad_command:
		l->print(log_densocomm,"kernel error: bad command\n");
		break;
	default:
		break;
	}
}

bool densocomm::read_kernel_specific_rsp(unsigned char rsp,unsigned char *data,int datalen)
{
	int len = datalen;
	unsigned char krsp;
	if (!read_kernel_rsp_varlen(&krsp,data,&len))
		return false;
	if (krsp != rsp)
		return false;
	if (datalen != len)
		return false;
	return true;
}

bool densocomm::read_kernel_specific_rsp_varlen(unsigned char rsp,unsigned char *data,int *datalen)
{
	unsigned char krsp;
	if (!read_kernel_rsp_varlen(&krsp,data,datalen))
		return false;
	if (krsp != rsp)
		return false;
	return true;
}

bool densocomm::read_kernel_rsp_varlen(unsigned char *rsp,unsigned char *data,int *datalen)
{
	unsigned char d[5];
	unsigned char chksum;
	if (!s.read(&d[0],1))
	{
		l->print(log_densocomm,"rkrv: preamble 1 no response\n");
		return false;
	}	
	if (d[0] != 0xBE)
	{
		l->print(log_densocomm,"rkrv: preamble 1 error\n");
		return false;
	}
	if (!s.read(&d[1],1))
	{
		l->print(log_densocomm,"rkrv: preamble 2 no response\n");
		return false;
	}
	if (d[1] != 0xEF)
	{
		l->print(log_densocomm,"rkrv: preamble 2 error\n");
		return false;
	}
	if (!s.read(&d[2],1))
	{
		l->print(log_densocomm,"rkrv: length 1 no response\n");
		return false;
	}
	if (!s.read(&d[3],1))
	{
		l->print(log_densocomm,"rkrv: length 2 no response\n");
		return false;
	}

	int szpayload = (d[2] << 8) + d[3];
	if (szpayload == 0)
	{
		l->print(log_densocomm,"rkrv: zero size payload\n");
		return false;
	}

	if (!s.read(&d[4],1))
	{
		l->print(log_densocomm,"rkrv: no response ID\n");
		return false;
	}

	*rsp = d[4];

	szpayload--;
	if (szpayload > *datalen)
	{
		l->print(log_densocomm,"rkrv: response payload too large for storage\n");
		return false;
	}
	
	*datalen = szpayload;

	if (!s.read(data,*datalen))
	{
		l->print(log_densocomm,"rkrv: can't read response payload\n");
		return false;
	}

	if (!s.read(&chksum,1))
	{
		l->print(log_densocomm,"rkrv: can't read checksum\n");
		return false;
	}

	if (chksum != ((checksum(d,5)+checksum(data,*datalen)) & 0xFF))
	{
		l->print(log_densocomm,"rkrv: bad checksum\n");
		return false;
	}
	
	log_kernel_response(*rsp);
	return true;
}

bool densocomm::read_denso_rsp_varlen(unsigned char *rsp,unsigned char *data,int *datalen)
{
	unsigned char d[5];
	unsigned char chksum;
	if (!s.read(&d[0],1) || d[0] != 0x80)
		return false;
	if (!s.read(&d[1],1))
		return false;
	if (d[1] == 0xF0)
	{
		if (!s.read(&d[2],1) || d[2] != 0x10)
			return false;
	}
	else if (d[1] == 0x10)
	{
		if (!s.read(&d[2],1) || d[2] != 0xF0)
			return false;
	}
	else
		return false;

	if (!s.read(&d[3],1))
		return false;
	if (!s.read(&d[4],1))
		return false;

	int szpayload = d[3];
	*rsp = d[4];

	szpayload--;
	if (szpayload > *datalen)
		return false;
	
	*datalen = szpayload;

	if (!s.read(data,*datalen))
		return false;

	if (!s.read(&chksum,1))
		return false;

	if (chksum != ((checksum(d,5)+checksum(data,*datalen)) & 0xFF))
		return false;
	
	return true;
}

bool densocomm::read_serial_bytes(unsigned char *data,unsigned int *datalen)
{
	if (!s.read(data,*datalen))
		return false;
	return true;
}

unsigned char densocomm::checksum(unsigned char *data,int datalen)
{
	int sum = 0;
	for (int i = 0; i < datalen; i++)
		sum += *data++;
	return sum & 0xFF;
}

bool densocomm::enable_openport(unsigned int fn)
{
	return s.set_status(fn);
}