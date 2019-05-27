/*********************************************************/
/*                                                       */
/* SITEF - Di logo COBOL - SiTef.                        */
/*                                                       */
/* Version 3.00                                          */
/*                                                       */
/* Este programa espera un archivo llamado SITEF.DRV en  */
/* el directorio en el que se halla, con el sig. formato:*/
/* En la 1¦ l¡nea:                                       */
/*    nnn C¢digo de operaci¢n a realizar.                */
/*    nnn Terminal.                                      */
/*    nnn Timeout del Tef. Este valor se almacenar  tras */
/*        realizar una primera llamada con c¢digo de ope-*/
/*        raci¢n = 001. A partir de ese momento no har   */
/*        falta tocarlo.                                 */
/*    n   Si hay que pedir o no pagos desde el terminal  */
/*        utilizando compra con tarjeta de cr‚tito.      */
/*  En el resto de l¡neas:                               */
/*    Par metros con formato ASCII cuando no confirma/   */
/*    desconfirma. Si no, contendr  las informaciones    */
/*    previamente devueltas por este programa.           */
/* Devuelve el mismo fichero, pero con algunos cambios:  */
/* En la 1¦ l¡nea:                                       */
/*    nnn Resultado de la operaci¢n.                     */
/*        000 = OK                                       */
/*        255 = Errores varios Sitef                     */
/*        253 = Linia de comunicaci¢n con problemas      */
/*        1 a 99 = Errores redes autorizadoras           */
/*        999 = Operaci¢n no disponible                  */
/*        998 = No hay fichero de ¢rdenes (SITEF.DRV)    */
/*        997 = Sitef no activado                        */
/*        996 = No se pudo crear fichero trabajo         */
/*        995 = No se pudo crear fichero SiTef           */
/*        El no poder crear SITEF.DRV es un error fatal  */
/*        que sale por pantalla.                         */
/*                                                       */
/*    nnn N£mero de terminal (el original).              */
/*    nnn Timeout del Tef.                               */
/*    n   Pedir o no pagos actualizado.                  */
/*        1 - Si hay que pedir.                          */
/*        0 - No hay que pedir.                          */
/* En el resto de l¡neas:                                */
/*    Depender  de la operaci¢n realizada, reserv ndose  */
/*    la primera posici¢n para indicar el tipo de regis- */
/*    tro.                                               */
/*                                                       */
/*********************************************************/

#include <stdio.h>
#include <dos.h>

#include "sitef.h"

#define CR  0x0d
#define LF  0x0a
#define EOT 0x04
#define SOH 0x01
#define NUL 0x00
#define SINT short int

// Instrucci¢n para la depuraci¢n:
//#define DEBUG

/******************** Puerto COM del Pinpad **************************/
int iPorta_Pinpad = 2;
/*********************************************************************/

static char far *lpParam;

/*----------------------------- PROTOTYPES ---------------------------------*/


/* Prototipus de funcions */
extern void codifica ( char *texto, int tamanho );
extern void decodifica ( char *texto, int tamanho );

/* Variables de trabajo */
char      sFicOrd[12] = "SITEF.DRV";
char      sFicWrk[12] = "SITEF.WRK";
char      sFicOpe[12] = "SITEF.OPE";
char      sFicDez[12] = "SITEF.DEZ";
char      sFic123[12] = "SITEF.123";
char      sArxCnv[12] = "CNVSIT.BIN";
char      sArxFin[12];
char      sArxEnt[80];
char      *lpRutSortida = "\\TX\\";
char      *lpRutEntrada = "\\RX\\";
char      *lpNomArx = "PDV.";
char      *lpTerm;
SINT      iNumBytes = 0;
SINT      iCicle = 0;
SINT      iOpe;
SINT      iAntOpe = NULL;
SINT      iTimeOut;
char      sContestacio[2];
int       iContestacio;
int       iLlargContestacio;
int       iLiniesLlegides;
int       iOrdresObertes;
int       iLlegitP;
int       i, j, k;
ulong     hora_inicial;

FILE* lpszArxEnt;
FILE* lpszArxFin;
FILE* lpszOrdres;
FILE* lpszFicWrk;
FILE* lpszArxCnv;
FILE* lpszFicOpe;
FILE* lpszFic123;
FILE* lpszFicDez;

/* Variables */
char* sLinia[50]; // debe ser 15
SINT  iEspecial[50]; //debe ser 15
char  sLinia_wrk[50][80]; //ibid

#include "sitrut.c"
/*************************************************************************/
/*                                                                       */
/*                      F U N C I O   M A I N                            */
/*                      =====================                            */
/*                                                                       */
/*************************************************************************/
void main()
{

   byte_separa->a = NUL;

   llegir_ficord();

   if (iOpe != 3)
   {
      strcpy(sArxEnt, reg_ordres2->unitat);
      strcat(sArxEnt, lpRutEntrada);
      strcat(sArxEnt, lpNomArx);
      strcat(sArxEnt, lpTerm);
   }
   //
   // comprobamos antes que no haya alguna
   // otra operacion pendiente...
   //
   if ( (iOpe != 500) && (iOpe != 128) )
   {
      iAntOpe = iOpe;
      if ( iOpe = leer_operacion() )
      {
         if ( iAntOpe == 129 )
            iAntOpe = NULL;
         goto repetir;
      }
      iOpe = iAntOpe;
      iAntOpe = NULL;
   }
   //
   repetir:
   //
   reg_header->codi_transaccio = iOpe;
   //
   switch (iOpe)
   {
      case   1: case   2:
                escriure_reg_curt();
                break;
      case  33: case  48:
                escriure_registre(1);
                break;
      case  21: case 32: case 39: case 40: case 18:
                escriure_registre(3);
                break;
      case  56: case 34:
                escriure_registre(4);
                break;
      case  22: case 55: case 35: case 54:
                iEspecial[5] = 1;
                escriure_registre(2);
                break;
      case  36: case  38:
                if (strcmp(sLinia[6], "1"))
                  iEspecial[6] = iEspecial[7] = iEspecial[8] = 1;
                escriure_registre(3);
                break;
      case  50: case 11: case 114: case 116: case 240: // este 240 es reimpresion
                escriure_registre(-1);
                break;
      case 112:
                if (!strcmp(sLinia[10], "2"))
                   iEspecial[11] = iEspecial[12] = iEspecial[13] = iEspecial[14] = 1;
                escriure_registre(-1);
                break;
      case 128: llegir_missatge(0);
                remove(sArxEnt);
                missatge_confirmacio();
                break;
      case 129: llegir_missatge(0);
                remove(sArxEnt);
                missatge_no_confirmacio();
                break;
      case 241: // Fininvest
                reg_header->codi_transaccio = 240;
                escriure_registre(5);
                break;
      case 242: // Fininvest
                reg_header->codi_transaccio = 240;
                escriure_registre(6);
                break;
      case 500: // desfazimento
                remove(sArxEnt);
                desfazimento();
                break;
      default: escriure_error(999);
               break;
   }

   //
   //
   // Si fue confirmacion o desconfirmacion, no borramos
   // antes el archivo de respuesta, ya que esta es muy rapida...
   //
   if ((iOpe != 128) && (iOpe != 129) && (iOpe != 500) )
      remove(sArxEnt);

   //
   // generamos archivo para tef
   //
   if ( iOpe != 500 )
      generar_arxfin();

#ifdef DEBUG
   printf("Ahora es el momento de copiar el PDV de respuesta\n"); getch();
#endif

   hora_inicial = llegir_hora();

   while (((llegir_hora() - hora_inicial) < (iTimeOut * 1000L))
            && (llegir_missatge(1)))
       w_nseconds(1);

   if ((iContestacio == 253) || // Problemes de comunicaci¢
       (iContestacio == 254))   // Terminal tancat
   {
      iCicle++;
      if (iCicle < 3)
      {
         if ( (iOpe == 1) ||
              (iOpe == 128) ||
              (iOpe == 129) )
            goto repetir;
      }
   }

   if (obrir_arxent_rb()) // no podemos leer archivo de respuesta
   {
      if ( (iOpe == 128) || (iOpe == 129) )
      {
         // grabar proxima operacion a realizar
         // que antecede a apertura de terminal
         //
         grabar_operacion( iOpe );
      }
      else
      {
         if ( (iOpe > 21) && (iOpe != 500) )
         {
            // grabamos la proxima operacion a realizar
            // antes de la apertura de terminal
            // en este caso, desfazimento
            // 500 es su codigo en el programa.
            // el codigo real es 02
            grabar_operacion( 500 );
         }
      }
      escriure_error(997); // error de comunicacion
   }
   else // hemos abierto el archivo de respuesta
   {
      if ( iContestacio == 0 )
      {
         // Si todo fue perfectamente, y se realizo una operacion
         // pendiente, realizaremos la apertura de terminal.
         // Asi que tenemos que volver al principio...
         remove( sFicOpe );
         if ( iOpe == 500 )
         {
            remove(sFicDez);
            sprintf(reg_ordres13->nid, "%03d", reg_header->nid++);

            // despues del desfazimento, haremos
            // una apertura de terminal...
            if ( iAntOpe != 1 )
            {
               iOpe = 1;
               goto repetir;
            }
         }
         if ( iAntOpe != NULL )
         {
            iOpe = iAntOpe;
            iAntOpe = NULL;
            goto repetir;
         }
      }
      else
      {
         if ( (iOpe > 21) && (iOpe != 128) && (iOpe != 129) )
         {
            //
            // hay respuesta, pero es diferente de cero.
            // asi que marcamos que falta hacer un desfazimento...
            // La respuesta dif. de cero para la confirmacion
            // y la desconfirmacion (128 y 129) no nos importa.
            // la cuestion es que nos responda.
            grabar_operacion( 500 );
         }
         if ( (iOpe == 128) || (iOpe == 129) )
         {
            remove( sFicOpe );
            if ( iAntOpe != NULL )
            {
               iOpe = iAntOpe;
               iAntOpe = NULL;
               goto repetir;
            }
         }
      }
      remove(sFicWrk);
      // si se consiguio realizar la confirmacion
      // o desconfirmacion, entonces no sera preciso
      // hacer el desfazimento...
      if ( (iOpe == 128) || (iOpe == 129) )
      {
         remove(sFicDez);
      }
   }
   final:
}
