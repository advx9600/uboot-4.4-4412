#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define TOP_DIR "../.."

int main()
{
    int i,j,k,ret;
    const char names[][50]={"bl1.bin","bl2.bin","u-boot.bin","tzsw.bin"};
    const int sizes[]={30,32,656,312};

    FILE* fdsave = fopen("super_boot.bin","w");
    if (fdsave == NULL){
	perror("fopen");
	return -1;
    }

    for (i=0;i<ARRAY_SIZE(names);i++)
    {
	char filename[50];
	sprintf(filename,"%s/%s",TOP_DIR,names[i]);
	FILE* fd = fopen(filename,"r");
	if (fd == NULL){
	    perror("fopen");
	    return -1;
	}

	for (j=0;j<sizes[i];j++)
	{
	    char readbuf[512];
	    memset(readbuf,0,sizeof(readbuf));
	    if (fread(readbuf,sizeof(readbuf),1,fd) < 0){
		perror("fread");
		return -1;
	    }
//	    printf("ftell:0x%x\n",ftell(fdsave));
	    if (fwrite(readbuf,sizeof(readbuf),1,fdsave)<0){
		perror("fwrite");
		return -1;
	    }
	}

	fclose(fd);
    }

    fclose(fdsave);

    return 0;
}
