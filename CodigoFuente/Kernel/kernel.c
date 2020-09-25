#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <video_driver.h>
#include <interrupts.h>
#include <syscalls.h>
#include <mem_manager.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;
extern void _hlt();

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();
int otraFunc();


int init();

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

int main()
{
	initializeVideoMode();
	load_idt();
	drawWord("Hasta aca 1");
    setUp_syscalls();
	drawWord("Hasta aca 2");
	void ** pargs= (void**)my_malloc(sizeof(void*));
	drawWord("Hasta aca 3");
	pargs[0] = (void*)"init";
	//initProcess(&init,1,pargs);
	drawWord("Hasta aca 4");
	pargs[0] = (void*)"shell";
	//initProcess(&otraFunc,1,pargs);
	initProcess(sampleCodeModuleAddress, 1, NULL);
	((EntryPoint)init)();
	drawWord("Hasta aca 5");
	drawWord("Hasta aca 6");
	return 0;
}

int init(){
	int count = 0;
	while(1) {
		_hlt();
		if(count++%20 == 0)drawWord("Hola");
	}
}

int otraFunc(){
	drawWord("Chau forro");
}