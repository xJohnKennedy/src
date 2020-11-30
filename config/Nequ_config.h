/******************Declaracoes principais ******************/
#define Nequ 42 //  Numero de equacoes do sistema

#ifdef _FORCA_BR_C
int		Nt_temp = 500;
#endif // _FORCA_BR_C

#ifdef _BACIA_GPU
//definicao de diretivas de preprocessamento para impressao de arquivos de log ou RK para uma celula determinada
#define _POINCARE_LOG		true
#define _RUNGEKUTTA_LOG		true
#if (_POINCARE_LOG || _RUNGEKUTTA_LOG || DEBUG)
#define _NUM_CELL_LOG 2
#endif
int numGroupThreads = 2;
#endif // _BACIA_GPU

