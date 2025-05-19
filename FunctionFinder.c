#include <Windows.h>
#include <winnt.h>
#include <stdio.h>
int main(int argc, char* argv[]) {

HMODULE hMod = LoadLibraryEx(argv[1], NULL, 0);
if (!hMod) {
    printf("error 1\n");
    return 1;
}

PIMAGE_DOS_HEADER dh = (PIMAGE_DOS_HEADER)hMod;
if (dh->e_magic != IMAGE_DOS_SIGNATURE) {
    printf("error 3\n");
    return 1;
}


PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((BYTE*)dh + dh->e_lfanew);
if (nt->Signature != IMAGE_NT_SIGNATURE) {
    printf("error 2\n");
    return 1;
}


PIMAGE_OPTIONAL_HEADER oh = &nt->OptionalHeader;

PIMAGE_DATA_DIRECTORY exportDataDir = &oh->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

PIMAGE_EXPORT_DIRECTORY exportDir = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)oh->ImageBase + exportDataDir->VirtualAddress);

char* buff = (char*)malloc(8025 * sizeof(buff));
snprintf(buff, 100, "%s.txt", argv[1]);
FILE *file = fopen(buff, "a+");
if (!file) {
    printf("error writing to file\n");
    return 1;
}


int functionNum = 0;
DWORD* nameRVAs = (DWORD*)((BYTE*)oh->ImageBase + exportDir->AddressOfNames);
for (size_t i = 0; i < exportDir->NumberOfNames; i++) {
    functionNum++;
    printf("%i: ", functionNum);
    fprintf(file, "%i: %s\n", functionNum, (char*)oh->ImageBase + nameRVAs[i]);
    printf("Function: %s\n", (char*)oh->ImageBase + nameRVAs[i]);
}

printf("# of functions: %lu\n", exportDir->NumberOfFunctions);
printf("address of functions: %lu\n", exportDir->AddressOfFunctions);
printf("Image base: %llu\n", oh->ImageBase);

fclose(file);

return 0;
}
