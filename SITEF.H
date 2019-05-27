/*********************************************************/
/*                                                       */
/* SITEF.H - Estructures dels registres del TEF.         */
/*                                                       */
/*********************************************************/
/* Estructures de dades */
struct st_tamany         { short int a; } reg_tamany[1];
struct st_tamany_byte    { short int a; } tamany_byte[1];
struct st_tipus_registre { unsigned char a; } tipus_registre[1];
struct st_tamany2        { short int a; } reg_tamany2[1];
struct st_tipus_registre2 { unsigned char a; } tipus_registre2[1];
struct st_header {
       short    int  codi_versio;
       unsigned char codi_missatge;
       unsigned char codi_transaccio;
       char          identificacio_terminal[8];
       char          reservat1[3];
       unsigned char nid;
       short    int  reservat2;
       short    int  poscar;
       char          empresa[8];
       short    int  tamany_dades;
       } reg_header[1];
struct st_confirmacio {
       char data[4];
       char nsu_sitef[6];
       char codi_xarxa[2];
       } reg_confirmacio[1];
//struct st_registre_13 { char a[25]; } registre_13[1];
struct st_registre_13 { char a[43]; } registre_13[1];
struct st_registre_H {
       char codi_resposta[3];
       char data[5];
       char hora[7];
       char document[10];
       char codi_establiment[16];
       char numero_autoritzacio[7];
       char document_cancelat[10];
       char data_compra_cancelada[5];
       char hora_compra_cancelada[7];
       char nom_institucio[17];
       char nsu_sitef[7];
       char codi_xarxa[3];
       } registre_H[1];
struct st_registre_J {
       char valor_venda[13];
       char taxa_obertura_credit[6];
       char valor_total[13];
       char data_primer_pagament[5];
       char nom_portador[15];
       char numero_ocurrencies[3];
       char numero_pagaments1[3];
       char taxa_juros1[5];
       char valor_pagament1[13];
       char numero_pagaments2[3];
       char taxa_juros2[5];
       char valor_pagament2[13];
       char numero_pagaments3[3];
       char taxa_juros3[5];
       char valor_pagament3[13];
       char numero_pagaments4[3];
       char taxa_juros4[5];
       char valor_pagament4[13];
       char numero_pagaments5[3];
       char taxa_juros5[5];
       char valor_pagament5[13];
       } registre_J[1];
struct st_separa             { unsigned char a; } byte_separa[1];
struct st_estructura         { char *a; }         estructura[1];
struct st_0d0a               { char a[2]; }       reg_0d0a[1];

struct st_tipo              { unsigned char tipo_reg; }
                                  reg_tipo[1];
struct st_ficope            { int iOpe; }
                                  reg_ficope[1];
struct st_ordres            { char ordre[4]; }
                                   reg_ordres[1];
struct st_ordres2           { char unitat[3]; }
                                   reg_ordres2[1];
struct st_ordres3           { char terminal[4]; }
                                   reg_ordres3[1];
struct st_ordres4           { char timeout[4]; }
                                   reg_ordres4[1];
struct st_ordres5           { char dempag[2]; }
                                   reg_ordres5[1];
struct st_ordres6           { char limite_forcada[17]; }
                                   reg_ordres6[1];
struct st_ordres7           { char data_agenda[5]; }
                                   reg_ordres7[1];
struct st_ordres8           { char numero_pagaments[3]; }
                                   reg_ordres8[1];
struct st_ordres9           { char interval_pagaments[3]; }
                                   reg_ordres9[1];
struct st_ordres10          { char primer_pagament_vista[2]; }
                                   reg_ordres10[1];
struct st_ordres11          { char mes_tancat[2]; }
                                   reg_ordres11[1];
struct st_ordres12          { char compra_forcada[2]; }
                                   reg_ordres12[1];
struct st_ordres13          { char nid[4]; }
                                   reg_ordres13[1];
struct st_ordres14          { char nota_promissoria[2]; }
                                   reg_ordres14[1];

/* Estructura del registre C */
struct st_registre_C {
       char grup1[10];
       char compra_forcada[2];
       char grup1a[35];
       char limite_forcada[17];
       char data_agenda[5];
       char grup2[7];
       unsigned char primer_pagament_vista[2];
       unsigned char mes_tancat[2];
       char nota_promissoria[2];
       } registre_C[1];
struct st_Cdempag         { short int a; }
       regC_dempag[1];
struct st_Ctimeout_sitef          { short int a; }
       regC_timeout_sitef[1];
struct st_Cnumero_pagaments        { short int a; }
       regC_numero_pagaments[1];
struct st_Cinterval_pagaments     { short int a; }
       regC_interval_pagaments[1];

/* Buffer */
unsigned char buffer[512];

typedef unsigned long  ulong;

