/*
 * uvloader.c - Userland Vita Loader entry point
 * Copyright 2012 Yifan Lu
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define START_SECTION __attribute__ ((section (".text.start"), naked))

// make sure code is PIE
#ifndef __PIE__
#error "Must compile with -fPIE"
#endif

/********************************************//**
 *  \brief Starting point from exploit
 *
 *  Call this from your exploit to run UVLoader.
 *  It will first cache all loaded modules and
 *  attempt to resolve its own NIDs which
 *  should only depend on sceLibKernel.
 *  \returns Zero on success, otherwise error
 ***********************************************/

int START_SECTION
uvl_start (void)
{
    int *data;
    char *sp;
    int (*FS_MOUNTSDMC)(const char *dev) = 0x001E61F8;
    int (*IFile_Open)(void *this, const short *path, int flags) = 0x00205764;
    int (*IFile_Write)(void *this, unsigned int *written, void *src, unsigned int len) = 0x001F5870;
    int *init = 0x0090F910;
    int *tmp = 0x0090F914;
    void *this = 0x0090F918;
    void *ret = 0x00187B84+12;

    asm volatile ("stmfd sp!,{r0-r11,lr}\t\n"
                  "mov %0,sp\t\n"
                  "mov %1,r0\t\n" : "=r" (sp), "=r" (data));

    if (!*init)
    {
        FS_MOUNTSDMC("sdxx:");
        IFile_Open(this, L"sdxx:/data.bin", 0x6);
        *init = 1;
    }
    /*
    if (data[0] == 0x00340101 && *init == 1)
    {
        data[4] = 0;
        *init = 2;
    }
    */
    *tmp = 0x11111111; // separator
    IFile_Write(this, tmp, tmp, 4);
    IFile_Write(this, tmp, data, 0x1000);
    *tmp = 0x22222222; // separator
    IFile_Write(this, tmp, tmp, 4);
    IFile_Write(this, tmp, sp, 0x1000);
    /*
    *tmp = 0xc0c0c0c0; // separator
    IFile_Write(this, tmp, tmp, 4);
    IFile_Write(this, tmp, **vtable, 0x40);
    */

    asm volatile ("mov r12,%0\t\n"
                  "ldmfd sp!,{r0-r11,lr}\t\n"
                  "MOV     R2, #1\t\n"
                  "MOV     R5, #0\t\n"
                  "STR     R3, [R4]\t\n"
                  "bx r12\t\n" :: "r" (ret));
}
