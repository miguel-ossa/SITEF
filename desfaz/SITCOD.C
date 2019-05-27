/*********************************************************/
/*                                                       */
/* SITCOD - Declaraci¢ de funcions d'encriptaci¢.        */
/*                                                       */
/*********************************************************/
void encriptacio( void )
{
   char c; char cadena[15360];

   obrir_arxfin_rb();
   fread(reg_tamany, sizeof(reg_tamany), 1, lpszArxFin);

   strcpy(cadena, NULL);
   i = 0;
   do
   {
      c = fgetc(lpszArxFin);
      cadena[i] = c;
      i++;
   }
   while (i != reg_tamany->a);

   fclose(lpszArxFin);
   if ( reg_header->codi_transaccio != 2 )
   {
      remove(sArxFin);
   }

   obrir_arxcnv_wb();
#ifndef DEBUG
   codifica(cadena, i);
#endif
   fwrite(reg_tamany, sizeof(reg_tamany), 1, lpszArxCnv);
   for (i=0; i<reg_tamany->a; i++)
       fputc(cadena[i], lpszArxCnv);
   fclose(lpszArxCnv);

}
void desencriptacio( void )
{
   char c; char cadena[15360];

   fread(reg_tamany, sizeof(reg_tamany), 1, lpszArxEnt);

   strcpy(cadena, NULL);
   i = 0;
   do
   {
      c = fgetc(lpszArxEnt);
      cadena[i] = c;
      i++;
   }
   while (i != reg_tamany->a);

   fclose(lpszArxEnt);
   remove(sArxEnt);

   obrir_arxent_wb();
   fwrite(reg_tamany, sizeof(reg_tamany), 1, lpszArxEnt);
#ifndef DEBUG
   decodifica(cadena, i);
#endif
   for (i=0; i<reg_tamany->a; i++)
       fputc(cadena[i], lpszArxEnt);
   fclose(lpszArxEnt);
   obrir_arxent_rb();
}
