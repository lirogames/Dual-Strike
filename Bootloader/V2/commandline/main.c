/* Name: main.c
 * Project: AVR bootloader HID
 * Author: Christian Starkjohann
 * Creation Date: 2007-03-19
 * Tabsize: 4
 * Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: Proprietary, free under certain conditions. See Documentation.
 * This Revision: $Id: main.c 373 2007-07-04 08:59:36Z cs $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "usbcalls.h"

#define IDENT_VENDOR_NUM        0x16c0
#define IDENT_VENDOR_STRING     "obdev.at"
#define IDENT_PRODUCT_NUM       1503
#define IDENT_PRODUCT_STRING    "HIDBoot"

/* ------------------------------------------------------------------------- */

static char dataBuffer[65536 + 256];    /* buffer for file data */
static int  startAddress, endAddress;
static char leaveBootLoader = 0;

/* ------------------------------------------------------------------------- */

static int  parseUntilColon(FILE *fp)
{
int c;

    do{
        c = getc(fp);
    }while(c != ':' && c != EOF);
    return c;
}

static int  parseHex(FILE *fp, int numDigits)
{
int     i;
char    temp[9];

    for(i = 0; i < numDigits; i++)
        temp[i] = getc(fp);
    temp[i] = 0;
    return strtol(temp, NULL, 16);
}

/* ------------------------------------------------------------------------- */

static int  parseIntelHex(char *hexfile, char buffer[65536 + 256], int *startAddr, int *endAddr)
{
int     address, base, d, segment, i, lineLen, sum;
FILE    *input;

    input = fopen(hexfile, "r");
    if(input == NULL){
        fprintf(stderr, "error opening %s: %s\n", hexfile, strerror(errno));
        return 1;
    }
    while(parseUntilColon(input) == ':'){
        sum = 0;
        sum += lineLen = parseHex(input, 2);
        base = address = parseHex(input, 4);
        sum += address >> 8;
        sum += address;
        sum += segment = parseHex(input, 2);  /* segment value? */
        if(segment != 0)    /* ignore lines where this byte is not 0 */
            continue;
        for(i = 0; i < lineLen ; i++){
            d = parseHex(input, 2);
            buffer[address++] = d;
            sum += d;
        }
        sum += parseHex(input, 2);
        if((sum & 0xff) != 0){
            fprintf(stderr, "Warning: Checksum error between address 0x%x and 0x%x\n", base, address);
        }
        if(*startAddr > base)
            *startAddr = base;
        if(*endAddr < address)
            *endAddr = address;
    }
    fclose(input);
    return 0;
}

/* ------------------------------------------------------------------------- */

char    *usbErrorMessage(int errCode)
{
static char buffer[80];

    switch(errCode){
        case USB_ERROR_ACCESS:      return "Access to device denied";
        case USB_ERROR_NOTFOUND:    return "The specified device was not found";
        case USB_ERROR_BUSY:        return "The device is used by another application";
        case USB_ERROR_IO:          return "Communication error with device";
        default:
            sprintf(buffer, "Unknown USB error %d", errCode);
            return buffer;
    }
    return NULL;    /* not reached */
}

static int  getUsbInt(char *buffer, int numBytes)
{
int shift = 0, value = 0, i;

    for(i = 0; i < numBytes; i++){
        value |= ((int)*buffer & 0xff) << shift;
        shift += 8;
        buffer++;
    }
    return value;
}

static void setUsbInt(char *buffer, int value, int numBytes)
{
int i;

    for(i = 0; i < numBytes; i++){
        *buffer++ = value;
        value >>= 8;
    }
}

/* ------------------------------------------------------------------------- */

typedef struct deviceInfo {
    char    reportId;
    char    pageSize[2];
    char    memorySize[4];
}deviceInfo_t;

typedef struct deviceData {
    char    reportId;
    char    address[3];
    char    data[128];
}deviceData_t;

usbDevice_t *dev = NULL;

union {
    char            bytes[1];
    deviceInfo_t    info;
    deviceData_t    data;
} buffer;

int err = 0;

static int uploadFlashData(char *dataBuffer, int startAddr, int endAddr) {
	int len, mask, pageSize, deviceSize;
	
    len = sizeof(buffer);

	printf("Uploading flash data...\n");
	
    if((err = usbGetReport(dev, USB_HID_REPORT_TYPE_FEATURE, 1, buffer.bytes, &len)) != 0){
        fprintf(stderr, "Error reading page size: %s\n", usbErrorMessage(err));
        goto errorOccurred;
    }
    if(len < sizeof(buffer.info)){
        fprintf(stderr, "Not enough bytes in device info report (%d instead of %d)\n", len, (int)sizeof(buffer.info));
        err = -1;
        goto errorOccurred;
    }
    pageSize = getUsbInt(buffer.info.pageSize, 2);
    deviceSize = getUsbInt(buffer.info.memorySize, 4);
    printf("Page size   = %d (0x%x)\n", pageSize, pageSize);
    printf("Device size = %d (0x%x); %d bytes remaining\n", deviceSize, deviceSize, deviceSize - 2048);
    if(endAddr > deviceSize - 2048){
        fprintf(stderr, "Data (%d bytes) exceeds remaining flash size!\n", endAddr);
        err = -1;
        goto errorOccurred;
    }
    if(pageSize < 128){
        mask = 127;
    }else{
        mask = pageSize - 1;
    }
    startAddr &= ~mask;                  /* round down */
    endAddr = (endAddr + mask) & ~mask;  /* round up */
    printf("Uploading %d (0x%x) bytes starting at %d (0x%x)\n", endAddr - startAddr, endAddr - startAddr, startAddr, startAddr);
    while(startAddr < endAddr){
        buffer.data.reportId = 2;
        memcpy(buffer.data.data, dataBuffer + startAddr, 128);
        setUsbInt(buffer.data.address, startAddr, 3);
        printf("\r0x%05x ... 0x%05x", startAddr, startAddr + (int)sizeof(buffer.data.data));
        fflush(stdout);
        if((err = usbSetReport(dev, USB_HID_REPORT_TYPE_FEATURE, buffer.bytes, sizeof(buffer.data))) != 0){
            fprintf(stderr, "Error uploading data block: %s\n", usbErrorMessage(err));
            goto errorOccurred;
        }
        startAddr += sizeof(buffer.data.data);
    }
    printf("\n");
errorOccurred:
    if(dev != NULL)
        usbCloseDevice(dev);
    return err;
}

static int uploadEEPROMData(char *dataBuffer, int startAddr, int endAddr) {
	int err = 0, len, mask, pageSize, deviceSize;
	
    len = sizeof(buffer);

	printf("Uploading EEPROM data...\n");
	
    if((err = usbGetReport(dev, USB_HID_REPORT_TYPE_FEATURE, 3, buffer.bytes, &len)) != 0) {
        fprintf(stderr, "Error reading page size: %s\n", usbErrorMessage(err));
        goto errorOccurred;
    }
    if(len < sizeof(buffer.info)) {
        fprintf(stderr, "Not enough bytes in device info report (%d instead of %d)\n", len, (int)sizeof(buffer.info));
        err = -1;
        goto errorOccurred;
    }
	
    pageSize = getUsbInt(buffer.info.pageSize, 2);
    deviceSize = getUsbInt(buffer.info.memorySize, 4);
    printf("Page size   = %d (0x%x)\n", pageSize, pageSize);
    printf("Device size = %d (0x%x); %d bytes remaining\n", deviceSize, deviceSize, deviceSize - 2048);
	
    if(endAddr > deviceSize) {
        fprintf(stderr, "Data (%d bytes) exceeds remaining EEPROM size!\n", endAddr);
        err = -1;
        goto errorOccurred;
    }
	
    if(pageSize < 128)
        mask = 127;
	else
        mask = pageSize - 1;
		
    startAddr &= ~mask;                  /* round down */
    endAddr = (endAddr + mask) & ~mask;  /* round up */
	
    printf("Uploading %d (0x%x) bytes starting at %d (0x%x)\n", endAddr - startAddr, endAddr - startAddr, startAddr, startAddr);
	
    while(startAddr < endAddr){
        buffer.data.reportId = 4;
        memcpy(buffer.data.data, dataBuffer + startAddr, 128);
        setUsbInt(buffer.data.address, startAddr, 3);
        printf("\r0x%05x ... 0x%05x", startAddr, startAddr + (int)sizeof(buffer.data.data));
        fflush(stdout);
        if((err = usbSetReport(dev, USB_HID_REPORT_TYPE_FEATURE, buffer.bytes, sizeof(buffer.data))) != 0){
            fprintf(stderr, "Error uploading data block: %s\n", usbErrorMessage(err));
            goto errorOccurred;
        }
        startAddr += sizeof(buffer.data.data);
    }
    printf("\n");
errorOccurred:
    if(dev != NULL)
        usbCloseDevice(dev);
    return err;
}

/* ------------------------------------------------------------------------- */

static void printUsage(char *pname)
{
    fprintf(stderr,
	"usage: %s ([-h|--help] | [-r] [<intel-hexfile>] [-e <intel-hexfile>])\n \"-h\" or \"--help\" prints this message.\n \"-r\" restarts the device in the end.\n \"<intel-hexfile\" specifies a file to write into the flash.\n \"-e <intel-hexfile>\" specifies a file to write to the EEPROM.\nAt least one of the arguments not including \"-r\" must be used.\n", pname);
}

static int testArgs(int argc, char **argv) {
	int currentIndex;
	
	if(argc < 2 || argc > 5)
		return 1;
		
	if(argc == 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
		return 0;
		
	if(strcmp(argv[1], "-r") == 0)
		currentIndex = 2;
	else
		currentIndex = 1;

	if(argc == currentIndex + 1 && strcmp(argv[currentIndex], "-e") != 0)
		return 0;

	if((argc == currentIndex + 2 && strcmp(argv[currentIndex], "-e") == 0) ||
	   (argc == currentIndex + 3 && strcmp(argv[currentIndex + 1], "-e") == 0))
		return 0;
	
	return 1;
}

int main(int argc, char **argv) {
	int currentIndex;
	char *flashFile = NULL;
	char *eepromFile = NULL;

    if(testArgs(argc, argv)) {
        printUsage(argv[0]);
        return 1;
    }
	
    if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printUsage(argv[0]);
        return 0;
    }
	
	if(strcmp(argv[1], "-r") == 0) {
        leaveBootLoader = 1;
		currentIndex = 2;
	}
	else
		currentIndex = 1;
	
	if(strcmp(argv[currentIndex], "-e") == 0) {
		eepromFile = argv[currentIndex + 1];
	}
	else {
		flashFile = argv[currentIndex];
		
		if(argc == currentIndex + 3)
			eepromFile = argv[currentIndex + 2];
	}
		
    if((err = usbOpenDevice(&dev, IDENT_VENDOR_NUM, IDENT_VENDOR_STRING, IDENT_PRODUCT_NUM, IDENT_PRODUCT_STRING, 1)) != 0){
        fprintf(stderr, "Error opening HIDBoot device: %s\n", usbErrorMessage(err));
		return 1;
    }
	
	if(flashFile != NULL) {
		startAddress = sizeof(dataBuffer);
		endAddress = 0;
		memset(dataBuffer, -1, sizeof(dataBuffer));

		if(parseIntelHex(flashFile, dataBuffer, &startAddress, &endAddress))
			return 1;
		
		if(startAddress >= endAddress)
			fprintf(stderr, "No data in flash input file.\n");
		else if(uploadFlashData(dataBuffer, startAddress, endAddress))
				return 1;
	}
	
	if(eepromFile != NULL) {
		startAddress = sizeof(dataBuffer);
		endAddress = 0;
		memset(dataBuffer, -1, sizeof(dataBuffer));

		if(parseIntelHex(eepromFile, dataBuffer, &startAddress, &endAddress))
			return 1;

		if(startAddress >= endAddress)
			fprintf(stderr, "No data in EEPROM input file.\n");
		else if(uploadEEPROMData(dataBuffer, startAddress, endAddress))
			return 1;
	}
	
	if(leaveBootLoader){
        // and now leave boot loader:
		printf("Telling device to leave bootloader...");
        buffer.info.reportId = 1;
        usbSetReport(dev, USB_HID_REPORT_TYPE_FEATURE, buffer.bytes, sizeof(buffer.info));
        // Ignore errors here. If the device reboots before we poll the response,
        // this request fails.
    }

    return 0;
}

/* ------------------------------------------------------------------------- */


