#include "program_flash.h"
#include "string.h"
#include "debug.h"
#include "stdlib.h"
#include "stdint.h"

#define BOOTLOADER_SIZE	0x02000
#define APP_ADDR        (0x08000000U+BOOTLOADER_SIZE)
#define FLASH_END       0x08007FFFU       // last flash address
#define SRAM_START      0x20000000U
#define SRAM_END        0x20001FFFU       // 8 KB SRAM


static uint8_t frame_buf[128];
static uint16_t frame_len = 0;
static uint8_t in_frame = 0;

void Program_Flash_Process_Debug_Data(void)
{
    if (!Debug_Data_Available())
        return;

    uint16_t len = Debug_Get_Data_Len();
    uint8_t buf[DEBUG_RX_BUF_SIZE];
    Debug_Data_Copy_Buf(buf);
    Debug_Clear_Data_Available_Flag();

    for (uint16_t i = 0; i < len; i++)
    {
        uint8_t c = buf[i];

        if (c == '<')           // start of new frame
        {
            in_frame = 1;
            frame_len = 0;
            frame_buf[frame_len++] = c;
        }
        else if (in_frame)
        {
            frame_buf[frame_len++] = c;

            if (c == '>')       // complete frame detected
            {
                // Echo entire frame
                Debug_Tx_Buf(frame_buf, frame_len);
								uint32_t address;
								uint8_t data[128];
								uint16_t data_len;
								uint8_t checksum;
								Program_Flash_Parse_Frame(frame_buf,&address,data,&data_len,&checksum);
								uint8_t k = 0;
								for(uint32_t adrss = address; adrss < data_len; adrss++){
									flash_write_word(adrss,data[k++]);
								}
								//Debug_Tx_Text_NL("ACK");
								Debug_Flush_Buf();
                // Reset state for next frames
                in_frame = 0;
                frame_len = 0;
            }
        }
    }
}



void Program_Flash_Jump_To_App(void)
{

		
    typedef void (*app_fn)(void);

    uint32_t app_sp    = *(volatile uint32_t*)(APP_ADDR + 0);
    uint32_t app_reset = *(volatile uint32_t*)(APP_ADDR + 4);


    if (app_sp == 0xFFFFFFFF || app_reset == 0xFFFFFFFF){
				Debug_Tx_Text_NL("Empty Flash! No Program Loaded!");
        return;
		}


    if (app_sp < SRAM_START || app_sp > SRAM_END){
				Debug_Tx_Text_NL("STACK POINTER OUT OF SRAM");
        return;
		}

    if (app_reset < APP_ADDR || app_reset > FLASH_END){
				Debug_Tx_Text_NL("APP_RST OUT OF FLASH!");
        return;
		}
    
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    RCC->IOPENR   = 0;
    RCC->APBENR1  = 0;
    RCC->APBENR2  = 0;

    __DSB();
    __ISB();


    app_fn jump_to_app = (app_fn)app_reset;
    jump_to_app();

    while(1);   // never return
		
	
	
}


void Program_Flash_Parse_Frame(char* frame, uint32_t* address, uint8_t* data,uint8_t* data_lenth, uint8_t* checksum)
{
    if (frame[0] != '<') return;
    if (frame[strlen(frame)-1] != '>') return;

    uint8_t first_col = strchr(frame, ':') - frame;
    uint8_t last_col  = strrchr(frame, ':') - frame;


    char addr[9];
    memcpy(addr, frame + 1, 8);
    addr[8] = '\0';
    *address = (uint32_t)strtoul(addr, NULL, 16);


    uint16_t ascii_len = last_col - (first_col + 1);

    uint16_t data_len = ascii_len / 2; // 2 chars = 1 byte
		*data_lenth = data_len;

    char byte_str[3];
    byte_str[2] = '\0';

    for (uint16_t i = 0; i < data_len; i++)
    {
        byte_str[0] = frame[first_col + 1 + (i * 2)];
        byte_str[1] = frame[first_col + 2 + (i * 2)];
        data[i] = (uint8_t)strtoul(byte_str, NULL, 16);
    }

    char checksumstr[3];
    checksumstr[0] = frame[last_col + 1];
    checksumstr[1] = frame[last_col + 2];
    checksumstr[2] = '\0';

    *checksum = (uint8_t)strtoul(checksumstr, NULL, 16);
}
