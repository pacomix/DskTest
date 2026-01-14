
#ifndef  __cpcrslib_h__
#define __cpcrslib_h__

void 						cpc_SetMode( char color); // reg A
void 						cpc_SetColour(unsigned char num,  char color); // reg A, reg L
void						cpc_DisableFirmware(void);

void 						cpc_ScanKeyboard(void);
//void 						cpc_AssignKey(int valor, unsigned char tecla); // reg a reg de
char 						cpc_TestKeyF(char number); // REG a; number : REG

extern unsigned int g_tecla_0;
extern unsigned int g_tecla_1;
extern unsigned int g_tecla_2;
extern unsigned int g_tecla_3;
extern unsigned int g_tecla_4;
extern unsigned int g_tecla_5;
extern unsigned int g_tecla_6;
extern unsigned int g_tecla_7;
extern unsigned int g_tecla_8;
extern unsigned int g_tecla_9;
extern unsigned int g_tecla_10;
extern unsigned int g_tecla_11;
extern unsigned int g_tecla_12;
extern unsigned int g_tecla_13;
extern unsigned int g_tecla_14;
extern unsigned int g_tecla_15;

/*
void  						cpc_SetInk(unsigned char num,  unsigned char color);
void  						cpc_SetBorder( char color);
unsigned char 				cpc_Random(void);

void 						cpc_ClrScr(void);

void 						cpc_PutSprite(char *sprite, int posicion);
void 						cpc_PutSp(char *sprite, char height, char width, int address);
void						cpc_PutSp4x14(char *sprite, int address);
void 						cpc_PutSpriteXOR(char *sprite, int posicion);
void 						cpc_PutSpXOR(char *sprite, char height, char width, int address);
void 						cpc_PutSpriteTr(char *sprite, int *posicion);
void 						cpc_PutSpTr(char *sprite, char height, char width, int address);
void						cpc_GetSp(char *sprite, char alto, char ancho, int posicion);
void 						cpc_PutMaskSprite(char *sprite,unsigned int addr);
//void 						cpc_PutMaskSprite(struct sprite *spr,unsigned int *addr);
void    					cpc_PutMaskSp(char *sprite, char alto, char ancho, int posicion);
void 						cpc_PutMaskSp4x16(char *sprite,unsigned int addr);
void 						cpc_PutMaskSp2x8(char *sprite,unsigned int addr);


unsigned char				cpc_CollSp(char *sprite, char *sprite2);


// TILE MAP:
void						cpc_InitTileMap(void);
void 						cpc_SetTile(unsigned char x, unsigned char y, unsigned char b);
void						cpc_ShowTileMap();
void						cpc_ShowTileMap2(void);
void						cpc_ResetTouchedTiles(void);

void						cpc_PutSpTileMap(char *sprite);
void						cpc_PutSpTileMapF(char *sprite);
void						cpc_UpdScr(void);
void						cpc_PutSpTileMap2b(char *sprite);
void						cpc_PutMaskSpTileMap2b(char *sprite);
void						cpc_PutTrSpTileMap2b(char *sprite);
void						cpc_PutTrSpriteTileMap2b(char *sprite);


void						cpc_SpUpdY(char *sprite, char valor);
void						cpc_SpUpdX(char *sprite, char valor);

void						cpc_ScrollRight00(void);
void						cpc_ScrollRight01(void);
void						cpc_ScrollLeft00(void);
void						cpc_ScrollLeft01(void);
void						cpc_ScrollRight(void);
void						cpc_ScrollLeft(void);

void						cpc_SetTouchTileXY(unsigned char x, unsigned char y, unsigned char t);
unsigned char				cpc_ReadTile(unsigned char x, unsigned char y);
void						cpc_SuperbufferAddress(char *sprite);

// ****************






void     					cpc_RRI(unsigned int pos, unsigned char w, unsigned char h);
void  		   				cpc_RLI(unsigned int pos, unsigned char w, unsigned char h);


int 						cpc_AnyKeyPressed(void);
void 						cpc_ScanKeyboard(void);

void						cpc_DeleteKeys(void);
void 						cpc_AssignKey(unsigned char tecla, int valor);
unsigned char 				cpc_TestKey(unsigned char tecla);
void 						cpc_RedefineKey(unsigned char tecla);

int							cpc_GetScrAddress(char x, char y);

void 						cpc_PrintStr(char *text);

void						cpc_EnableFirmware(void);


void						cpc_PrintGphStr(char *text, int *destino);
void						cpc_PrintGphStrM1(char *text, int *destino);
void						cpc_PrintGphStr2X(char *text, int *destino);
void						cpc_PrintGphStrM12X(char *text, int *destino);

void						cpc_PrintGphStrXY(char *text, char a, char b);
void						cpc_PrintGphStrXYM1(char *text, char a, char b);
void						cpc_PrintGphStrXY2X(char *text, char a, char b);
void						cpc_PrintGphStrXYM12X(char *text, char a, char b);
void						cpc_SetInkGphStr(char a, char b);
void						cpc_SetInkGphStrM1(char a, char b);

void     					cpc_PrintGphStrStd(char color, char *cadena, int *destino);
void  		   				cpc_PrintGphStrStdXY(char color, char *cadena, char x, char y);
*/

#endif /* __cpcrslib_h__ */
