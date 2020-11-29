/******************Declaracoes principais ******************/
#define Nequ 42 //  Numero de equacoes do sistema

#ifdef _FORCA_BR_C
// nao confundir esta variavel com o numero de divisoes do RK,
//esta variavel determina o numero de periodos que o forca_bruta ira executar antes de testar a convergencia novamente
int		Nt_temp = 500;
#endif // _FORCA_BR_C

