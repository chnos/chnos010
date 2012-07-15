
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
		unsigned enable : 1;	//PCI_CDRnにアクセスするときに1にする。
	} bit;
} PCI_ConfigurationAddressRegister;

//全デバイス共通のコンフィギュレーションレジスタ:
//0x00:00-15:ベンダID(ReadOnly)
//0x00:16-31:デバイスID(ReadOnly)
//0x04:00-15:コマンドレジスタ(R/W)
//	00:I/O空間イネーブル
//	01:メモリ空間イネーブル
//	02:バスマスター
//	03:スペシャルサイクル
//	04:メモリライト・インバリデートイネーブル
//	05:VGAパレットスヌープ
//	06:パリティエラー応答(0で報告なし)
//	07:ウェイトサイクル制御
//	08:SERR#イネーブル
//	09:高速バックツーバックイネーブル
//	10-15:リザーブ
//0x04:16-31:ステータスレジスタ(R/W Write時Trueのビットがクリアされる)
//	16-20:リザーブ
//	21:66MHz対応可能
//	22:ユーザー定義機能あり
//	23:高速バックツーバック可能
//	24:データパリティエラー検知(バスマスター時のエラー)
//	25-26:DEVSEL#タイミング(0:高速 1:中速 2:低速 3:予約)
//	27:ターゲットアボート通知
//	28:ターゲットアボート受信
//	29:マスターアボート受信
//	30:システムエラー通知
//	31:パリティエラー検知
//0x08:00-07:リビジョンID(ReadOnly)
//0x08:08-31:クラスコード(ReadOnly)
//	08-15:プログラムインターフェース
//	16-23:サブクラス
//	24-31:ベースクラス
//0x0c:00-07:キャッシュラインサイズ
//0x0c:08-15:マスタレイテンシタイマ
//0x0c:16-23:ヘッダタイプ(ReadOnly)
//	16-22:デバイスタイプ(0:通常のPCIデバイス 1:PCI-PCIブリッジ 2:CardBusブリッジ)
//	23:マルチファンクションデバイス
//0x0c:24-31:BISTレジスタ

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

void Initialize_PCI(void)
{
	#ifdef CHNOSPROJECT_DEBUG_PCI
		uint data, bus, device, function;
	#endif

//PCICAR初期化
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
