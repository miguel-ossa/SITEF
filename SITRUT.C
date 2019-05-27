/*********************************************************/
/*                                                       */
/* SITRUT - Declaraci¢ de funcions de SITEF.C            */
/*                                                       */
/*********************************************************/
void obrir_ordres_wb( void )
{
   if ((lpszOrdres = fopen(sFicOrd, "w+b")) == NULL)
   {
      printf("NAO SE PODE CRIAR SITEF.DRV\n");
      exit(1);
   }
}
void obrir_FicWrk_wb( void )
{
   if ((lpszFicWrk = fopen(sFicWrk, "w+b")) == NULL)
   {
      printf("NAO SE PODE CRIAR SITEF.WRK\n");
      exit(1);
   }
}
void obrir_Fic123_wb( void )
{
   if ((lpszFic123 = fopen(sFic123, "w+b")) == NULL)
   {
      printf("NAO SE PODE CRIAR SITEF.123\n");
      exit(1);
   }
}
void escriure_salt_registre( void )
{
   fputc(CR, lpszOrdres);
   fputc(LF, lpszOrdres);
}
int obrir_arxent_rb( void )
{
     return ((lpszArxEnt = fopen(sArxEnt, "rb")) == NULL);
}
int obrir_FicWrk_rb( void )
{
     return ((lpszFicWrk = fopen(sFicWrk, "rb")) == NULL);
}
int obrir_Fic123_rb( void )
{
     return ((lpszFic123 = fopen(sFic123, "rb")) == NULL);
}
int obrir_FicDez_rb( void )
{
     return ((lpszFicDez = fopen(sFicDez, "rb")) == NULL);
}
void obrir_arxcnv_wb( void )
{
   if ((lpszArxCnv = fopen(sArxCnv, "w+b")) == NULL) // dest¡
   {
      printf("NAO SE PODE CRIAR CNVSIT.BIN\n");
      exit(1);
   }
}
void obrir_arxfin_rb( void )
{
   if ((lpszArxFin = fopen(sArxFin, "rb")) == NULL) // origen PDV.nnn
      exit(1);
}
void obrir_arxent_wb( void )
{
   if ((lpszArxEnt = fopen(sArxEnt, "w+b")) == NULL)
   {
      printf("NAO SE PODE CRIAR ARQUIVO DE ENTRADA\n");
      exit(1);
   }
}
void obrir_ficope_wb( void )
{
   if ((lpszFicOpe = fopen(sFicOpe, "w+b")) == NULL)
   {
      printf("NAO SE PODE CRIAR SITEF.OPE\n");
      exit(1);
   }
}
int obrir_ficope_rb( void )
{
   return ((lpszFicOpe = fopen(sFicOpe, "rb")) == NULL);
}
void grabar_operacion( SINT iOpe )
{
   obrir_ficope_wb();

   reg_ficope->iOpe = iOpe;

   fwrite(reg_ficope, sizeof(reg_ficope), 1, lpszFicOpe);

   fclose( lpszFicOpe );
}
SINT leer_operacion( void )
{
   SINT iOpe;

   if ( !obrir_ficope_rb() )
   {
      fread(reg_ficope, sizeof( reg_ficope ), 1, lpszFicOpe);
      iOpe = reg_ficope->iOpe;
      fclose( lpszFicOpe );

      return( iOpe );
   }
   else return( NULL );
}
void escriure_linies( int sLiniafin, int calcul_tamany )
{
   int j;

   for (i=0; i<=sLiniafin; i++)
   {
      // si campo informado o campo especial,
      // escribir el campo
      if ((sLinia[i] != NULL) || (iEspecial[i]))
      {
         if (sLinia[i] != NULL)
         {
            if (calcul_tamany)
               reg_header->tamany_dades += strlen(sLinia[i]);
            else
               for (j=0; j < strlen(sLinia[i]); j++)
                   fputc(sLinia[i][j], lpszArxFin);
         }
      }
      // si campo normal, separador NULL
      if (!iEspecial[i])
         if (calcul_tamany)
            reg_header->tamany_dades++;
         else
            fputc(NUL, lpszArxFin);

      // si especial y siguiente campo informado,
      // separador EOT
      if ((iEspecial[i]) && (sLinia[i+1] != NULL))
         if (calcul_tamany)
            reg_header->tamany_dades++;
         else
            fputc(EOT, lpszArxFin);
   }
}
ulong llegir_hora()
{
   ulong milleseconds;
   union REGS r;

   r.h.ah = 0x2c;
   int86(0x21, &r, &r);

   milleseconds = (long)r.h.dl * 10;        // dl: centesimas
   milleseconds += (long)r.h.dh * 1000;     // dh: segundos
   milleseconds += (long)r.h.cl * 60000L;   // cl: minutos
   milleseconds += (long)r.h.ch * 3600000L; // ch:horas
   return (milleseconds);
}
void w_nseconds( ulong l )
{
   ulong timetow;

   timetow = llegir_hora() + (l * 1000L);
   while (llegir_hora() < timetow);
}
void separa( void )
{
   fwrite(byte_separa, sizeof(byte_separa), 1, lpszArxFin);
}
void header( void )
{
   int i; int j;

   reg_header->codi_versio = 768;  /* version 3.00 */
   reg_header->codi_missatge = 1; /* Solicitud */
   strcpy(reg_header->identificacio_terminal,"TE000");
   strcat(reg_header->identificacio_terminal, reg_ordres3->terminal);
   strcpy(reg_header->reservat1,"000");
   if ( iOpe == 1 )
   {
      strcpy( reg_ordres13->nid, "0" );
   }
   reg_header->nid = atoi(reg_ordres13->nid) + 1;

   sprintf(reg_ordres13->nid, "%03d", reg_header->nid + 1);

   reg_header->reservat2 = 0;
   strcpy(reg_header->empresa,"00000000");
}
void calc_tamany_std( void )
     { reg_tamany->a = sizeof(reg_header) + reg_header->tamany_dades;}
void escriure_tamany( void )
     { fwrite(reg_tamany, sizeof(reg_tamany),1, lpszArxFin);}
void escriure_header( void )
     { fwrite(reg_header, sizeof(reg_header), 1, lpszArxFin);}
void escriure_ordres( void )
{
#ifdef DEBUG
printf("\n\nescriure_ordres, timeout %s", reg_ordres4->timeout); getch();
#endif
   reg_tipo->tipo_reg = '0';
   fwrite(reg_tipo,                            1, 1, lpszOrdres);
   fwrite(reg_ordres->ordre,                   3, 1, lpszOrdres);
//   if ( reg_ordres2->unitat[1] != ':' )
//      strcpy( reg_ordres2->unitat, "E:" );
   fwrite(reg_ordres2->unitat,                 2, 1, lpszOrdres);
   fwrite(reg_ordres3->terminal,               3, 1, lpszOrdres);
   fwrite(reg_ordres4->timeout,                3, 1, lpszOrdres);
   fwrite(reg_ordres5->dempag,                 1, 1, lpszOrdres);
   fwrite(reg_ordres6->limite_forcada,        16, 1, lpszOrdres);
   fwrite(reg_ordres7->data_agenda,            4, 1, lpszOrdres);
   fwrite(reg_ordres8->numero_pagaments,       2, 1, lpszOrdres);
   fwrite(reg_ordres9->interval_pagaments,     2, 1, lpszOrdres);
   fwrite(reg_ordres10->primer_pagament_vista, 1, 1, lpszOrdres);
   fwrite(reg_ordres11->mes_tancat,            1, 1, lpszOrdres);
   fwrite(reg_ordres12->compra_forcada,        1, 1, lpszOrdres);
   fwrite(reg_ordres13->nid,                   3, 1, lpszOrdres);
   fwrite(reg_ordres14->nota_promissoria,      1, 1, lpszOrdres);
   escriure_salt_registre();
}
void generar_ficord( void )
{
   int j;

   // Convertimos el resultado a alfab‚tico PIC XXX.
   if (iContestacio == 255)
   {
      strcpy(reg_ordres->ordre, registre_H->codi_resposta);
      strcat(reg_ordres->ordre, " ");
   }
   else
   {
     // paso de iContestacio a reg_ordres->buffer con
     // formato %03d sin utilizar sprintf
     itoa(iContestacio, buffer, 10);
     strcpy(reg_ordres->ordre, "000");
     for (i=(strlen(buffer) - 1), j=2; i > -1; i--, j--)
         reg_ordres->ordre[j] = buffer[i];
   }
   obrir_ordres_wb();
   escriure_ordres();
}
void escriure_error( int ierror )
{
    iContestacio = ierror;
    iOrdresObertes = 1;
    generar_ficord();
    fclose(lpszOrdres);
    exit(1);
}
void obrir_ficord_rb( void )
{
   if ((lpszOrdres = fopen(sFicOrd, "rb")) == NULL)
      escriure_error(998);
}
void obrir_arxtre_wb( void )
{
   strcpy(sArxFin, lpNomArx);
   strcat(sArxFin, lpTerm);

   if ((lpszArxFin = fopen(sArxFin, "w+b")) == NULL)
      escriure_error(996);
}
void llegir_ficord( void )
{
   int j;
   char c;
                 //temporal quitar antes <15
   for (i=0; i<50; i++, sLinia[i] = NULL, iEspecial[i] = 0);

   obrir_ficord_rb();
   fread(reg_tipo,                            1, 1, lpszOrdres);
   fread(reg_ordres->ordre,                   3, 1, lpszOrdres);
   fread(reg_ordres2->unitat,                 2, 1, lpszOrdres);
   fread(reg_ordres3->terminal,               3, 1, lpszOrdres);
   fread(reg_ordres4->timeout,                3, 1, lpszOrdres);
   fread(reg_ordres5->dempag,                 1, 1, lpszOrdres);
   fread(reg_ordres6->limite_forcada,        16, 1, lpszOrdres);
   fread(reg_ordres7->data_agenda,            4, 1, lpszOrdres);
   fread(reg_ordres8->numero_pagaments,       2, 1, lpszOrdres);
   fread(reg_ordres9->interval_pagaments,     2, 1, lpszOrdres);
   fread(reg_ordres10->primer_pagament_vista, 1, 1, lpszOrdres);
   fread(reg_ordres11->mes_tancat,            1, 1, lpszOrdres);
   fread(reg_ordres12->compra_forcada,        1, 1, lpszOrdres);
   fread(reg_ordres13->nid,                   3, 1, lpszOrdres);
   fread(reg_ordres14->nota_promissoria,      1, 1, lpszOrdres);
   if (fread(reg_0d0a, sizeof(reg_0d0a), 1, lpszOrdres) != 1)
      goto final;

   c = EOF; i = 0;
   do
   {
      j = 0;
      do
      {
         c = fgetc(lpszOrdres);
         if ((c != EOF) && (c != LF) && (c != CR))
            sLinia_wrk[i][j] = c;
         j++;
      }
      while ((c != EOF) && (c != CR));
      sLinia_wrk[i][j-1] = '\0';
      sLinia[i] = (char *)sLinia_wrk[i];
      if (!strcmp(sLinia[i], "|"))
         sLinia[i] = NULL;
      if (c != EOF) c = fgetc(lpszOrdres);
      i++;
   }
   while ((c != EOF) && (i < 50)); //temporal antes 15

   iLiniesLlegides = i - 1;
   for (i=iLiniesLlegides; i>0; i--)
   {
      if (sLinia[i] != NULL)
         break;
      else
         iLiniesLlegides--;
   }

   //
   final:
   //
   iOpe = atoi(reg_ordres->ordre);
   lpTerm = reg_ordres3->terminal;
   iTimeOut = atoi(reg_ordres4->timeout);
   if (iTimeOut < 30) iTimeOut = 10;
   fclose(lpszOrdres);

   //remove( sFicOrd );
}
#include "sitcod.c"
void generar_arxfin( void )
{
   char comando[100];

   strcpy(sArxFin, lpNomArx);
   strcat(sArxFin, lpTerm);

#ifdef DEBUG
   strcpy(comando, "copy ");
   strcat(comando, sArxFin);
   strcat(comando, " C:\\SALIDA.TXT >NULL");
   system(comando);
#endif

   if ( (iOpe > 21) && (iOpe != 128) && (iOpe != 129) )
   {
      if ( reg_header->codi_transaccio != 2 )
      {
         // copia para posible desfazimento
         strcpy(comando, "copy ");
         strcat(comando, sArxFin);
         strcat(comando, " ");
         strcat(comando, sFicDez);
         strcat(comando, " >NUL");
         system(comando);
      }
   }
   encriptacio();

   strcpy(sArxSor, reg_ordres2->unitat);
   strcat(sArxSor, lpRutSortida);
   strcat(sArxSor, lpNomArx);
   strcat(sArxSor, lpTerm);
   remove (sArxSor);

   strcpy(comando, "copy ");
   strcat(comando, sArxCnv);
   strcat(comando, " ");
   strcat(comando, sArxSor);
   strcat(comando, " > NUL");
   system(comando);

   remove(sArxCnv);
}
void desfazimento()
{
   char sArxSor[80];
   char comando[100], ch;
   int  swRed = 0, iPoscar;

   j = k = 0;
   strcpy(sArxFin, lpNomArx);
   strcat(sArxFin, lpTerm);


   if ( !obrir_FicDez_rb() )
   {
      fread(reg_tamany, sizeof(reg_tamany), 1, lpszFicDez);
      fread(reg_header, sizeof(reg_header), 1, lpszFicDez);

      reg_header->codi_missatge = 2;
      reg_header->nid = reg_header->nid++;
      iPoscar = reg_header->poscar + 1;
//      reg_header->poscar = 5;
//      reg_header->tamany_dades = 14;
      reg_header->poscar = 6;
      reg_header->tamany_dades = 15;

      if ((lpszArxFin = fopen(sArxFin, "w+b")) == NULL)
      {
         printf("NAO SE PODE CRIAR ARXFIN %s\n", sArxFin);
         fclose( lpszFicDez );
         exit(1);
      }

      fwrite(reg_tamany, sizeof(reg_tamany), 1, lpszArxFin);
      fwrite(reg_header, sizeof(reg_header), 1, lpszArxFin);

      do
      {
         ch = fgetc( lpszFicDez );
         j++;
         ///Grabamos la red en formato 9999//////////////////////
         // despreciamos la antigua, formato 9
         if ( !swRed )
         {
           if ( !obrir_Fic123_rb() )
           {
              swRed = 1;
              fread(registre_13, 43, 1, lpszFic123);

              fclose( lpszFic123 );
              for( i=0; i<4; i++ )
              {
                 ch = registre_13->a[i];
                 fputc( ch, lpszArxFin );
              }
              fputc( NUL, lpszArxFin );
              fputc( NUL, lpszArxFin );
           }
         }
         /////////////////////////////////////////////
         if ( ch != EOF )
         {
            if ( j >= iPoscar )
            {
               fputc( ch, lpszArxFin );
               k++;
               if ( k == 9 )
               {
                  break;
               }
            }
         }
      } while ( ch != EOF );
      //
      fclose( lpszArxFin );
      fclose( lpszFicDez );
   }

   encriptacio();
   strcpy(sArxSor, reg_ordres2->unitat);
   strcat(sArxSor, lpRutSortida);
   strcat(sArxSor, lpNomArx);
   strcat(sArxSor, lpTerm);
   remove (sArxSor);
   strcpy(comando, "copy ");
   strcat(comando, sArxCnv);
   strcat(comando, " ");
   strcat(comando, sArxSor);
   strcat(comando, " > NUL");
   system(comando);
   remove(sArxCnv);
}
void escriure_confirmacio( void )
{
   if ( reg_header->codi_versio == 768 )
   {
      obrir_arxtre_wb();

      reg_tamany->a = 42;
      reg_header->tamany_dades = 12;

      escriure_tamany();
      escriure_header();

      fwrite(reg_confirmacio, sizeof(reg_confirmacio), 1, lpszArxFin);

      fclose(lpszArxFin);
   }

}
void missatge_confirmacio( void )
{
   header();
   reg_header->codi_missatge = 130;

   if ( !obrir_FicWrk_rb() )
   {
      fread(reg_confirmacio->data,                  4, 1, lpszFicWrk);
      fread(reg_confirmacio->nsu_sitef,             6, 1, lpszFicWrk);
      fread(reg_confirmacio->codi_xarxa,            2, 1, lpszFicWrk);
      fclose( lpszFicWrk );
   }

   if ( strlen( reg_confirmacio->nsu_sitef ) > 5  &&
        strlen( reg_confirmacio->codi_xarxa ) > 1 )
   {
      remove(sArxEnt);
      escriure_confirmacio();
   }
   else exit(1);
}
void missatge_no_confirmacio( void )
{
   header();
   reg_header->codi_missatge = 131;

   if ( !obrir_FicWrk_rb() )
   {
      fread(reg_confirmacio->data,                  4, 1, lpszFicWrk);
      fread(reg_confirmacio->nsu_sitef,             6, 1, lpszFicWrk);
      fread(reg_confirmacio->codi_xarxa,            2, 1, lpszFicWrk);
      fclose( lpszFicWrk );
   }

   if ( strlen( reg_confirmacio->nsu_sitef ) > 5  &&
        strlen( reg_confirmacio->codi_xarxa ) > 1 )
   {
      remove(sArxEnt);
      escriure_confirmacio();
   }
   else exit(1);
}
void escriure_reg_curt( void )
{
   header();
   reg_tamany->a = 30;

   reg_header->tamany_dades = reg_header->poscar = 0;

   obrir_arxtre_wb();
   escriure_tamany();
   escriure_header();

   fclose(lpszArxFin);
}
// El parametro 'poscar' indica el desplazamiento de trilha2+trilha1
// en el registro. Si esta en el tercer campo, sera 4 (el primer
// campo es cero).
void escriure_registre( int poscar )
{
   int j, k;

   header();

   if (poscar != -1)
      iEspecial[poscar] = 1;

   // La funcion "strlen" da un valor incorrecto para los campos con valor
   // nulo (NULL), asi que se fuerza a 1, en este caso.
   for (i=0, reg_header->poscar = 0; i < poscar; i++)
      reg_header->poscar += ( sLinia[i] == NULL ) ? 1: strlen(sLinia[i]) + 1;
      //reg_header->poscar += strlen(sLinia[i]) + 1;
   //
   // Calculo del tamaño identico a escritura de lineas.
   //
   escriure_linies(iLiniesLlegides - 1, 1);

   calc_tamany_std();

   obrir_arxtre_wb();
   escriure_tamany();
   escriure_header();

   escriure_linies(iLiniesLlegides - 1, 0);

   fclose(lpszArxFin);

}
void llegir_reg_C( void )
{
   int j;

#ifdef DEBUG
printf("\n\nllegir_reg_C"); getch();
#endif

   fread(registre_C->grup1, 9, 1, lpszArxEnt);
   fread(registre_C->compra_forcada, 1, 1, lpszArxEnt);
   if ((registre_C->compra_forcada[0] &= '\x02') == '\x02')
      reg_ordres12->compra_forcada[0] = '1';
   else
      reg_ordres12->compra_forcada[0] = '0';
   fread(registre_C->grup1a, 34, 1, lpszArxEnt);
   fread(registre_C->limite_forcada, 16, 1, lpszArxEnt);
   fread(registre_C->data_agenda, 4, 1, lpszArxEnt);
   if (strlen(registre_C->data_agenda) != 4)
      strcpy(registre_C->data_agenda, "0000");
   fread(regC_dempag, sizeof(regC_dempag), 1, lpszArxEnt);
   fread(registre_C->grup2, 6, 1, lpszArxEnt);
   fread(regC_timeout_sitef, sizeof(regC_timeout_sitef), 1, lpszArxEnt);
   fread(regC_numero_pagaments, sizeof(regC_numero_pagaments), 1, lpszArxEnt);
   fread(regC_interval_pagaments, sizeof(regC_interval_pagaments), 1, lpszArxEnt);
   fread(registre_C->primer_pagament_vista, 1, 1, lpszArxEnt);
   fread(registre_C->mes_tancat, 1, 1, lpszArxEnt);
   fread(registre_C->nota_promissoria, 1, 1, lpszArxEnt);
   iTimeOut = regC_timeout_sitef->a * 1.3;

   sprintf(reg_ordres4->timeout, "%03d", iTimeOut);
#ifdef DEBUG
   printf("\n\ntimeout %s", reg_ordres4->timeout); getch();
#endif

   sprintf(reg_ordres5->dempag, "%1d", regC_dempag->a);

   // el origen nos viene justificado a la izquierda con basura en el resto
   sprintf(reg_ordres6->limite_forcada, "%016d", atoi(registre_C->limite_forcada));

   strcpy(reg_ordres7->data_agenda, registre_C->data_agenda);

   sprintf(reg_ordres8->numero_pagaments, "%02d", regC_numero_pagaments->a);
   sprintf(reg_ordres9->interval_pagaments, "%02d", regC_interval_pagaments->a);
   strcpy(reg_ordres10->primer_pagament_vista, "0");
   if (registre_C->primer_pagament_vista[0] == SOH)
      strcpy(reg_ordres10->primer_pagament_vista, "1");
   strcpy(reg_ordres11->mes_tancat, "0");
   if (registre_C->mes_tancat[0] == SOH)
      strcpy(reg_ordres11->mes_tancat, "1");
   strcpy(reg_ordres14->nota_promissoria, "0");
   if (registre_C->nota_promissoria[0] == SOH)
      strcpy(reg_ordres14->nota_promissoria, "1");
#ifdef DEBUG
      printf("\n\nasigne valor a nota promissora %c",
           reg_ordres14->nota_promissoria[0]);getch();
#endif

   if (!iOrdresObertes)
   {
      iOrdresObertes = 1;
      generar_ficord();
   }

   iNumBytes += 81;
}
void llegir_reg_H( void )
{
   fread(registre_H->codi_resposta,         2, 1, lpszArxEnt);
   fread(registre_H->data,                  4, 1, lpszArxEnt);
   fread(registre_H->hora,                  6, 1, lpszArxEnt);
   fread(registre_H->document,              9, 1, lpszArxEnt);
   fread(registre_H->codi_establiment,     15, 1, lpszArxEnt);
   fread(registre_H->numero_autoritzacio,   6, 1, lpszArxEnt);
   fread(registre_H->document_cancelat,     9, 1, lpszArxEnt);
   fread(registre_H->data_compra_cancelada, 4, 1, lpszArxEnt);
   fread(registre_H->hora_compra_cancelada, 6, 1, lpszArxEnt);
   fread(registre_H->nom_institucio,       16, 1, lpszArxEnt);
   fread(registre_H->nsu_sitef,             6, 1, lpszArxEnt);
   fread(registre_H->codi_xarxa,            2, 1, lpszArxEnt);

   if (!iOrdresObertes)     //  eliminado por problemas con la reimpresion
   {
      iOrdresObertes = 1;
      generar_ficord();
   }

   iNumBytes += sizeof(registre_H) - 12;

   if ( strlen(registre_H->data) > 3 )
   {
      reg_tipo->tipo_reg = 'H';
      fwrite(reg_tipo,                          1, 1, lpszOrdres);
      fwrite(registre_H->codi_resposta,         2, 1, lpszOrdres);
      fwrite(registre_H->data,                  4, 1, lpszOrdres);
      fwrite(registre_H->hora,                  6, 1, lpszOrdres);
      fwrite(registre_H->document,              9, 1, lpszOrdres);
      fwrite(registre_H->codi_establiment,     15, 1, lpszOrdres);
      fwrite(registre_H->numero_autoritzacio,   6, 1, lpszOrdres);
      fwrite(registre_H->document_cancelat,     9, 1, lpszOrdres);
      fwrite(registre_H->data_compra_cancelada, 4, 1, lpszOrdres);
      fwrite(registre_H->hora_compra_cancelada, 6, 1, lpszOrdres);
      fwrite(registre_H->nom_institucio,       16, 1, lpszOrdres);
      fwrite(registre_H->nsu_sitef,             6, 1, lpszOrdres);
      fwrite(registre_H->codi_xarxa,            2, 1, lpszOrdres);

      escriure_salt_registre();

      if ( (!iContestacio) || (iContestacio == 1) )
      // quitar el 1 en produccion
      {
         obrir_FicWrk_wb();

         fwrite(registre_H->data,                  4, 1, lpszFicWrk);
         fwrite(registre_H->nsu_sitef,             6, 1, lpszFicWrk);
         fwrite(registre_H->codi_xarxa,            2, 1, lpszFicWrk);

         fclose( lpszFicWrk );
      }
   }
}
void llegir_reg_J( void )
{
   fread(registre_J->valor_venda,         12, 1, lpszArxEnt);
   fread(registre_J->taxa_obertura_credit, 5, 1, lpszArxEnt);
   fread(registre_J->valor_total,         12, 1, lpszArxEnt);
   fread(registre_J->data_primer_pagament, 4, 1, lpszArxEnt);
   fread(registre_J->nom_portador,        14, 1, lpszArxEnt);
   fread(registre_J->numero_ocurrencies,   2, 1, lpszArxEnt);

   //Inicializaci¢n de los cinco elementos de la tabla
   memset(registre_J->numero_pagaments1, '0',  2);
   memset(registre_J->taxa_juros1,       '0',  4);
   memset(registre_J->valor_pagament1,   '0', 12);

   memset(registre_J->numero_pagaments2, '0',  2);
   memset(registre_J->taxa_juros2,       '0',  4);
   memset(registre_J->valor_pagament2,   '0', 12);

   memset(registre_J->numero_pagaments3, '0',  2);
   memset(registre_J->taxa_juros3,       '0',  4);
   memset(registre_J->valor_pagament3,   '0', 12);

   memset(registre_J->numero_pagaments4, '0',  2);
   memset(registre_J->taxa_juros4,       '0',  4);
   memset(registre_J->valor_pagament4,   '0', 12);

   memset(registre_J->numero_pagaments5, '0',  2);
   memset(registre_J->taxa_juros5,       '0',  4);
   memset(registre_J->valor_pagament5,   '0', 12);

   for (i=0; i < 5; i++)
   {
      switch (i)
      {
         case 0:
              fread(registre_J->numero_pagaments1, 2, 1, lpszArxEnt);
              fread(registre_J->taxa_juros1,       4, 1, lpszArxEnt);
              fread(registre_J->valor_pagament1,  12, 1, lpszArxEnt);
              break;
         case 1:
              fread(registre_J->numero_pagaments2, 2, 1, lpszArxEnt);
              fread(registre_J->taxa_juros2,       4, 1, lpszArxEnt);
              fread(registre_J->valor_pagament2,  12, 1, lpszArxEnt);
              break;
         case 2:
              fread(registre_J->numero_pagaments3, 2, 1, lpszArxEnt);
              fread(registre_J->taxa_juros3,       4, 1, lpszArxEnt);
              fread(registre_J->valor_pagament3,  12, 1, lpszArxEnt);
              break;
         case 3:
              fread(registre_J->numero_pagaments4, 2, 1, lpszArxEnt);
              fread(registre_J->taxa_juros4,       4, 1, lpszArxEnt);
              fread(registre_J->valor_pagament4,  12, 1, lpszArxEnt);
              break;
         case 4:
              fread(registre_J->numero_pagaments5, 2, 1, lpszArxEnt);
              fread(registre_J->taxa_juros5,       4, 1, lpszArxEnt);
              fread(registre_J->valor_pagament5,  12, 1, lpszArxEnt);
              break;
      }
   }
   if (!iOrdresObertes)
   {
      iOrdresObertes = 1;
      generar_ficord();
   }

   iNumBytes += sizeof(registre_J) - 21;

   reg_tipo->tipo_reg = 'J';
   fwrite(reg_tipo,                         1, 1, lpszOrdres);
   fwrite(registre_J->valor_venda,         12, 1, lpszOrdres);
   fwrite(registre_J->taxa_obertura_credit, 5, 1, lpszOrdres);
   fwrite(registre_J->valor_total,         12, 1, lpszOrdres);
   fwrite(registre_J->data_primer_pagament, 4, 1, lpszOrdres);
   fwrite(registre_J->nom_portador,        14, 1, lpszOrdres);
   fwrite(registre_J->numero_ocurrencies,   2, 1, lpszOrdres);

   fwrite(registre_J->numero_pagaments1, 2, 1, lpszOrdres);
   fwrite(registre_J->taxa_juros1,       4, 1, lpszOrdres);
   fwrite(registre_J->valor_pagament1,  12, 1, lpszOrdres);

   fwrite(registre_J->numero_pagaments2, 2, 1, lpszOrdres);
   fwrite(registre_J->taxa_juros2,       4, 1, lpszOrdres);
   fwrite(registre_J->valor_pagament2,  12, 1, lpszOrdres);

   fwrite(registre_J->numero_pagaments3, 2, 1, lpszOrdres);
   fwrite(registre_J->taxa_juros3,       4, 1, lpszOrdres);
   fwrite(registre_J->valor_pagament3,  12, 1, lpszOrdres);

   fwrite(registre_J->numero_pagaments4, 2, 1, lpszOrdres);
   fwrite(registre_J->taxa_juros4,       4, 1, lpszOrdres);
   fwrite(registre_J->valor_pagament4,  12, 1, lpszOrdres);

   fwrite(registre_J->numero_pagaments5, 2, 1, lpszOrdres);
   fwrite(registre_J->taxa_juros5,       4, 1, lpszOrdres);
   fwrite(registre_J->valor_pagament5,  12, 1, lpszOrdres);

   escriure_salt_registre();
}
void llegir_reg_13( int longitud )
{
   fread(registre_13, longitud, 1, lpszArxEnt);

   if (!iOrdresObertes)
   {
      iOrdresObertes = 1;
      generar_ficord();
   }

   if ( longitud == 43 )
   {
      obrir_Fic123_wb();
      fwrite(registre_13, 43, 1, lpszFic123);
      fclose( lpszFic123 );
   }

   reg_tipo->tipo_reg = '1';
   fwrite(reg_tipo, 1, 1, lpszOrdres);
   fwrite(registre_13, longitud, 1, lpszOrdres);

   escriure_salt_registre();

   iNumBytes += longitud;
}
void llegir_ficvar( void )
{
   int contador;
   char c;

   if (((!iOrdresObertes) && (tipus_registre->a == 'I')) ||
       ((!iOrdresObertes) && (tipus_registre->a == 'P')) ||
       ((!iOrdresObertes) && (tipus_registre->a == 'X')) ||
       ((!iOrdresObertes) && (tipus_registre->a == 'D')))
   {
      iOrdresObertes = 1;
      generar_ficord();
   }

   if ( tipus_registre->a == 'X' )
   {
      if ( iOpe != 243 )
      {
         fread(reg_tamany2,     sizeof(reg_tamany2),     1, lpszArxEnt);
         fread(tipus_registre2, sizeof(tipus_registre2), 1, lpszArxEnt);
      }
      reg_tipo->tipo_reg = 'X';
      fwrite(reg_tipo, 1, 1, lpszOrdres);

      if ( iOpe != 243 )
      {
         for (i=0; i < (reg_tamany2->a - 1); i++ )
         {
            buffer[i] = fgetc( lpszArxEnt );
            if ( buffer[i] == '@' )
            {
               escriure_salt_registre();
               reg_tipo->tipo_reg = 'X';
               fwrite(reg_tipo, 1, 1, lpszOrdres);
            }
            else
                fputc(buffer[i], lpszOrdres );
         }
      } else
      // cheques bankinform
      {
         for (i=0; i<4; i++)
            c = fgetc( lpszArxEnt );
         contador = 0;
         do
         {
            c = fgetc( lpszArxEnt );
            if ( c != EOF )
            {
               fputc(c, lpszOrdres);
               contador++;
               if ( contador == 32 )
               {
                  escriure_salt_registre();
                  reg_tipo->tipo_reg = 'X';
                  fwrite(reg_tipo, 1, 1, lpszOrdres);
                  contador = 0;
               }
            }
         } while ( c != EOF );
      }
   }
   else
   {
      reg_tipo->tipo_reg = tipus_registre->a;
      fwrite(reg_tipo, 1, 1, lpszOrdres);

      for (i=0; i < (tamany_byte->a - 1); i++)
      {
         buffer[i] = fgetc(lpszArxEnt);

         if ( (tipus_registre->a == 'I') || (tipus_registre->a == 'P') ||
              (tipus_registre->a == 'D') )
         {
            fputc(buffer[i], lpszOrdres);
         }
      }
   }
   if ( (tipus_registre->a == 'I') || (tipus_registre->a == 'P') ||
        (tipus_registre->a == 'X') || (tipus_registre->a == 'D') )
   {
      escriure_salt_registre();
   }

   iNumBytes+=tamany_byte->a;
}
int llegir_missatge( int desencriptar )
{
   char c;

   if (obrir_arxent_rb())
   {
      // La operacion de desfazimento solamente se grabara
      // si se envio una operacion definitiva, y el tef consiguio
      // leer el archivo. Posteriormente, si se recibe respuesta,
      // se eliminara SITEF.OPE, para no enviarlo....
      if ( (iOpe > 21) && (iOpe != 128) && (iOpe != 129) && (iOpe != 500) )
      {
         if ( obrir_ficope_rb() ) // si SITEF.OPE no existe...
            grabar_operacion( 500 ); // generar operacion desfaz.
         else
             fclose( lpszFicOpe );
      }
      return 1;
   }

   if (desencriptar) desencriptacio();

   fread(reg_tamany, sizeof(reg_tamany), 1, lpszArxEnt);
   fread(reg_header, sizeof(reg_header), 1, lpszArxEnt);
   fread((char *) tamany_byte,        1, 1, lpszArxEnt);

   iContestacio = (int) reg_header->codi_missatge;
   iLlargContestacio = (int) reg_tamany->a;
   //
   // Contestaci¢n a COBOL
   //
   iOrdresObertes = 0;

   iNumBytes = 32;

   while (fread(tipus_registre, 1, 1, lpszArxEnt) == 1)
   {
      iNumBytes++;

      switch (tipus_registre->a)
      {
         case 'D': case 'I': case 'P': case 'X':
              if ( tipus_registre->a == 'I' )
              {
                 remove( sFicDez );
                 remove( sFic123 );
              }
              llegir_ficvar();
              break;
         case 'J':
              llegir_reg_J();
              break;
         case 'H':
              llegir_reg_H();
              break;
         case 'C':
              llegir_reg_C();
              break;
         case '1':
              llegir_reg_13(43);
              break;
         case '2':
              if ((iOpe == 50) || (iOpe == 48) ||
                  (iOpe == 54) || (iOpe == 55) ||
                  (iOpe == 56))
                 llegir_reg_13(10);
              else
              {
                 if (iOpe == 21) // consulta bins
                 {
                    if (!(strcmp( sLinia[2], "1"))) // debito
                       llegir_reg_13(10);
                    else llegir_reg_13(11);
                 }
                 else
                    llegir_reg_13(11);
              }
              break;
         case '3':
              llegir_reg_13(20);
              break;
         default:
            for (i=0; i < (tamany_byte->a - 1); i++)
                buffer[i] = fgetc(lpszArxEnt); //por ejemplo, el "/"
            iNumBytes+=tamany_byte->a;
            break;
      }
      if (iNumBytes < iLlargContestacio)
      {
         fread((char *) tamany_byte, 1, 1, lpszArxEnt);
         iNumBytes++;
      }
   }
   finalizar:

   fclose(lpszArxEnt); fclose(lpszOrdres);

   return 0;
}
