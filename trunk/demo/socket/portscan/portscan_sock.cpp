/*
 * =====================================================================================
 *
 *       Filename:  echo.cpp
 *
 *    Description:  echo server socket example.
 *
 *        Version:  1.0
 *        Created:  2009-8-2 21:47:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huys (hys), huys03@gmail.com
 *        Company:  hu
 *
 * =====================================================================================
 */
#include "resource.h"
 
#include <windows.h>
#include <stdio.h>
#include "usocket.h"

int scan_port(unsigned long ulAddr, int iPort)
{
    return 0;
}

int main(int argc, char *argv[])
{
    USocket us;

    char buf[256];

    us.init();

    us.gethostname(buf, 256);

    // buf[strlen(buf)-2] = '\0';

    printf("%s\n", buf);

    us.cleanup();

    getchar();

    return 0;
}
