#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <inttypes.h>
#include <locale.h>

void byte_to_bitstr(unsigned char byte, char *bit_str)
{
    unsigned char mask = 128;
    int i;
    
    for(i=0; mask; mask>>=1,++i)
    {
        bit_str[i]= (byte & mask) ? '1' : '0';
    }
    bit_str[i]='\0';
}


int main()
{
    wchar_t wide_str[2];
    wint_t  wide_int;
    char    buf[16];
    size_t  mb_len;

    char bit_str[9];

    int byte_number;

    wide_str[1] = L'\0';

    setlocale(LC_ALL,"");

    printf("Please enter text in UTF-8\n");

    while((wide_int = fgetwc(stdin)) != WEOF)
    {
        wide_str[0] = wide_int;
        mb_len=wcstombs(buf, wide_str, 16);

        printf("Symbol '%lc' | code %16" PRIu64 " | len %d | ",
                wide_str[0], (uint64_t)wide_str[0], mb_len);
        
        for(byte_number = 0; byte_number < mb_len; ++byte_number)
        {
            byte_to_bitstr(buf[byte_number],bit_str);
            printf(" %s",bit_str);
        }
        putchar('\n');
    }


    return 0;
}

