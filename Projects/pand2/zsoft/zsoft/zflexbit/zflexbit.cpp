#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

static void usage(void)
{
    printf("Uso: Secreto y codificado XD no seas re curioso! \n");
    exit(-1);
}



int main(int argc, char **argv)
{
double arg=0;
double max=2147483648;
int valor[32];
int salida[32];
char registro[32][25];
int linea=0;
int busque=0;
int iter=0;
int aux=0;
char cadena[200];
FILE *f = fopen("/mmc/mmca1/.system/QTDownLoad/zFlexbit/ROKR Z6.txt","r");

if(argc==1)
usage();
//printf("%s ",argv[1]);
arg=atof(argv[1]);
//printf("%f",arg);
linea=atoi(argv[2]);

busque=linea*32;
while (fgets(cadena ,200, f) && aux<32)
{
//Aqui leemos y almacenamos cada linea desde el offset seleccionado
if(iter>=busque && aux<32)
{
strcpy(registro[aux],cadena);
for(int i=0;i<25;i++)
{
if(registro[aux][i]=='\n' || registro[aux][i]==' ')
registro[aux][i]='.';
}
aux++;
}

iter++; 
}

fclose(f);

//for(int i=0;i<32;i++)
//printf("%i:%s",i,registro[i]);


for (int i=0;i<32;i++)
{
if(arg>=max && max>0)
{
valor[i]=1;
arg=arg-max;
}
else
{
valor[i]=0;
}
if (max>0)
max=max/2;
}



//Ahora debemos formar la cadena de salida

for(int i=31;i>=0;i--)
{
//printf("%d",valor[31-i]);
salida[i]=valor[31-i];
}


for (int i=0;i<32;i++)
{
printf(" [%X]%s %i",i+(linea*32),registro[i],salida[i]);
}
//printf("%s\n",salida);
return 1;
}
