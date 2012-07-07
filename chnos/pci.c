
#include "core.h"

#define PORT_PCI_CONFIG_ADDRESS	0x0cf8	//32bit R/W
#define PORT_PCI_CONFIG_DATA_BASE	0x0cfc	//4ports anysize R/W

typedef union PCI_CONFIG_ADDRESS_REGISTER {
	uint reg;
	struct PCI_CONFIG_ADDRESS_REGISTER_BIT {
		unsigned reserved0 : 2;	//0
		unsigned register_num : 6;
		unsigned function_num : 3;
		unsigned device_num : 5;
		unsigned bus_num : 8;
		unsigned reserved1 : 7;	//0
		unsigned enable : 1;	//PCI_CDRn�ɃA�N�Z�X����Ƃ���1�ɂ���B
	} bit;
} PCI_ConfigurationAddressRegister;

//�S�f�o�C�X���ʂ̃R���t�B�M�����[�V�������W�X�^:
//0x00:00-15:�x���_ID(ReadOnly)
//0x00:16-31:�f�o�C�XID(ReadOnly)
//0x04:00-15:�R�}���h���W�X�^(R/W)
//	00:I/O��ԃC�l�[�u��
//	01:��������ԃC�l�[�u��
//	02:�o�X�}�X�^�[
//	03:�X�y�V�����T�C�N��
//	04:���������C�g�E�C���o���f�[�g�C�l�[�u��
//	05:VGA�p���b�g�X�k�[�v
//	06:�p���e�B�G���[����(0�ŕ񍐂Ȃ�)
//	07:�E�F�C�g�T�C�N������
//	08:SERR#�C�l�[�u��
//	09:�����o�b�N�c�[�o�b�N�C�l�[�u��
//	10-15:���U�[�u
//0x04:16-31:�X�e�[�^�X���W�X�^(R/W Write��True�̃r�b�g���N���A�����)
//	16-20:���U�[�u
//	21:66MHz�Ή��\
//	22:���[�U�[��`�@�\����
//	23:�����o�b�N�c�[�o�b�N�\
//	24:�f�[�^�p���e�B�G���[���m(�o�X�}�X�^�[���̃G���[)
//	25-26:DEVSEL#�^�C�~���O(0:���� 1:���� 2:�ᑬ 3:�\��)
//	27:�^�[�Q�b�g�A�{�[�g�ʒm
//	28:�^�[�Q�b�g�A�{�[�g��M
//	29:�}�X�^�[�A�{�[�g��M
//	30:�V�X�e���G���[�ʒm
//	31:�p���e�B�G���[���m
//0x08:00-07:���r�W����ID(ReadOnly)
//0x08:08-31:�N���X�R�[�h(ReadOnly)
//	08-15:�v���O�����C���^�[�t�F�[�X
//	16-23:�T�u�N���X
//	24-31:�x�[�X�N���X
//0x0c:00-07:�L���b�V�����C���T�C�Y
//0x0c:08-15:�}�X�^���C�e���V�^�C�}
//0x0c:16-23:�w�b�_�^�C�v(ReadOnly)
//	16-22:�f�o�C�X�^�C�v(0:�ʏ��PCI�f�o�C�X 1:PCI-PCI�u���b�W 2:CardBus�u���b�W)
//	23:�}���`�t�@���N�V�����f�o�C�X
//0x0c:24-31:BIST���W�X�^

typedef struct PCI_DEVICE_VENDOR {
	uint id;
	uchar *name;
} PCI_Device_VendorID;

typedef struct PCI_DEVICE_CLASS {
	uint id;
	uchar *name;
} PCI_Device_Class;

typedef struct PCI_DEVICE_TYPE {
	uint id;
	uchar *name;
} PCI_Device_Type;

PCI_Device_VendorID pci_device_vendor[] = {
	{0x10ec, "Realtek Semiconductor Corp."},
	{0x8086, "Intel"},
	{0xffff, "[Unknown]"},
//	{0x, ""},
};

PCI_Device_Class pci_device_class[] = {
	{0x010100, "Ultra ATA storage controller"},
	{0x010180, "bus master IDE controller (UDMA33?)"},
	{0x020000, "ethernet adapter ?"},
	{0x030000, "VGA compatible controller"},
	{0x040100, "audio device"},
	{0x060000, "CPU to PCI bridge"},
	{0x060100, "PCI to ISA bridge"},
	{0x068000, "power management controller"},
	{0xff0000, "[Unknown]"},
};

PCI_Device_Type pci_device_type[] = {
	{0, "Standard PCI Device"},
	{1, "PCI-PCI Bridge"},
	{2, "CardBus Bridge"},
	{3, "[Undefined]"},
};

void PCI_ConfigurationRegister_SelectDevice(uint bus, uint device, uint function);
uint PCI_ConfigurationRegister_Read32(uint addr);
uchar *PCI_GetDeviceVendor(uint id);
uchar *PCI_GetDeviceClass(uint id);
uchar *PCI_GetDeviceType(uint id);

void Initialise_PCI(void)
{
	#ifdef CHNOSPROJECT_DEBUG_PCI
		uint data, bus, device, function;
	#endif

//PCICAR������
	IO_Out32(PORT_PCI_CONFIG_ADDRESS, 0x00000000);

	#ifdef CHNOSPROJECT_DEBUG_PCI
		for(bus = 0; bus < 256; bus++){
			for(device = 0; device < 32; device++){
				for(function = 0; function < 8; function++){
					PCI_ConfigurationRegister_SelectDevice(bus, device, function);
					data = PCI_ConfigurationRegister_Read32(0x00);
					if(data != 0xffffffff){
						debug("PCI:Bus%d.Device%d.Function%d:\n", bus, device, function);
						debug("PCI:    DeviceVendor:%s(0x%04X)\n", PCI_GetDeviceVendor(data & 0xffff), data & 0xffff);

						debug("PCI:    DeviceID:0x%04X\n", data >> 16);

						data = PCI_ConfigurationRegister_Read32(0x08);
						data = CFunction_ExtractBits(data, 8, 31);
						debug("PCI:    ClassCode:%s(0x%06X)\n", PCI_GetDeviceClass(data), data);

						data = PCI_ConfigurationRegister_Read32(0x0c);
						data = CFunction_ExtractBits(data, 16, 22);
						debug("PCI:    DeviceType:%s(%d)\n", PCI_GetDeviceType(data), data);
					}
				}
			}
		}
	#endif

	return;
}

void PCI_ConfigurationRegister_SelectDevice(uint bus, uint device, uint function)
{
	PCI_ConfigurationAddressRegister pcicar;
	pcicar.reg = IO_In32(PORT_PCI_CONFIG_ADDRESS);
	pcicar.bit.bus_num = bus;
	pcicar.bit.device_num = device;
	pcicar.bit.function_num = function;
	IO_Out32(PORT_PCI_CONFIG_ADDRESS, pcicar.reg);

	return;
}

uint PCI_ConfigurationRegister_Read32(uint addr)
{
	PCI_ConfigurationAddressRegister pcicar;
	uint data;

	pcicar.reg = IO_In32(PORT_PCI_CONFIG_ADDRESS);
	pcicar.bit.register_num = addr >> 2;
	pcicar.bit.enable = True;
	IO_Out32(PORT_PCI_CONFIG_ADDRESS, pcicar.reg);
	data = IO_In32(PORT_PCI_CONFIG_DATA_BASE + (addr & 0x03));
	pcicar.bit.enable = False;
	IO_Out32(PORT_PCI_CONFIG_ADDRESS, pcicar.reg);

	return data;
}

uchar *PCI_GetDeviceVendor(uint id)
{
	uint i;

	for(i = 0; pci_device_vendor[i].id != 0xffff; i++){
		if(pci_device_vendor[i].id == id){
			break;
		}
	}

	return pci_device_vendor[i].name;
}

uchar *PCI_GetDeviceClass(uint id)
{
	uint i;

	for(i = 0; pci_device_class[i].id != 0xff0000; i++){
		if(pci_device_class[i].id == id){
			break;
		}
	}

	return pci_device_class[i].name;
}

uchar *PCI_GetDeviceType(uint id)
{
	uint i;

	for(i = 0; pci_device_type[i].id != 3; i++){
		if(pci_device_type[i].id == id){
			break;
		}
	}

	return pci_device_type[i].name;
}
