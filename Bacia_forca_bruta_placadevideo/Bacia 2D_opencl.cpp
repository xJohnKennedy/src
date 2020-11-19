/* PROGRAMA Bac_Din_o1.CPP
   
  Programa para o calculo da bacia de atracao de um sistema de equacoes de moviemnto.

  Utiliza o metodo da forca bruta para calcular o atrator de cada ponto.

  Para cada ponto, vai integrando cada periodo e avaliando se converge para um
  atrator, contando tambem a periodicidade da solucao.

  
*/

#define _BACIA_C

// inclusao dos cabecalhos do Directx
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=nullptr; } }
#endif


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <io.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>

/******************Declaracoes principais ******************/
#include "_config_modelo/Kutta_header_config.h"

double Wf, Tf;
double eta__1, eta__2, PL;
double PL_8C, PL_8S, PL_9C, PL_9S;


int PeriodoMaximo;
double Passo;
double Y1min,Y1max,Y2min,Y2max;
int Num_cel,Cor1,Cor2;
double *q1,*q1p,*q2,*q2p;


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3D11Device*               g_pDevice = nullptr;
ID3D11DeviceContext*        g_pContext = nullptr;
ID3D11ComputeShader*        g_pCS = nullptr;

ID3D11Buffer*               g_pBufferX = nullptr;
ID3D11Buffer*               g_pBufferY = nullptr;
ID3D11Buffer*               g_pBufResult = nullptr;

ID3D11ShaderResourceView*   g_pBuf0SRV = nullptr;
ID3D11ShaderResourceView*   g_pBuf1SRV = nullptr;
ID3D11UnorderedAccessView*  g_pBufResultUAV = nullptr;

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
HRESULT CreateComputeDevice(_Outptr_ ID3D11Device** ppDeviceOut, _Outptr_ ID3D11DeviceContext** ppContextOut, _In_ bool bForceRef);
HRESULT CreateComputeShader(_In_z_ LPCWSTR pSrcFile, _In_z_ LPCSTR pFunctionName,
	_In_ ID3D11Device* pDevice, _Outptr_ ID3D11ComputeShader** ppShaderOut);
HRESULT CreateStructuredBuffer(_In_ ID3D11Device* pDevice, _In_ UINT uElementSize, _In_ UINT uCount,
	_In_reads_(uElementSize*uCount) void* pInitData,
	_Outptr_ ID3D11Buffer** ppBufOut);
HRESULT CreateRawBuffer(_In_ ID3D11Device* pDevice, _In_ UINT uSize, _In_reads_(uSize) void* pInitData, _Outptr_ ID3D11Buffer** ppBufOut);
HRESULT CreateBufferSRV(_In_ ID3D11Device* pDevice, _In_ ID3D11Buffer* pBuffer, _Outptr_ ID3D11ShaderResourceView** ppSRVOut);
HRESULT CreateBufferUAV(_In_ ID3D11Device* pDevice, _In_ ID3D11Buffer* pBuffer, _Outptr_ ID3D11UnorderedAccessView** pUAVOut);
ID3D11Buffer* CreateAndCopyToDebugBuf(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pd3dImmediateContext, _In_ ID3D11Buffer* pBuffer);
void RunComputeShader(_In_ ID3D11DeviceContext* pd3dImmediateContext,
	_In_ ID3D11ComputeShader* pComputeShader,
	_In_ UINT nNumViews, _In_reads_(nNumViews) ID3D11ShaderResourceView** pShaderResourceViews,
	_In_opt_ ID3D11Buffer* pCBCS, _In_reads_opt_(dwNumDataBytes) void* pCSData, _In_ DWORD dwNumDataBytes,
	_In_ ID3D11UnorderedAccessView* pUnorderedAccessView,
	_In_ UINT X, _In_ UINT Y, _In_ UINT Z);
HRESULT FindDXSDKShaderFileCch(_Out_writes_(cchDest) WCHAR* strDestPath,
	_In_ int cchDest,
	_In_z_ LPCWSTR strFilename);


/******************Declaracoes das funcoes ******************/
int Runge_Kutta(double *y, double *x, double Step, int Total);
void NewData(void);
void CellsTrajec(void);
void CellsTrajec_core(int const nome_thread, int const cell_inicio, int const cell_fim);


/*===========================  Func  ===========================*/
#include "_config_modelo/arquivo_equacoes.h"

/*===========================  Runge_Kutta  ===========================*/
int Runge_Kutta(double *y, double *x, double Step, int Total)
{

	int i, j;
	double t1, t2, t3, t4, t;
	int count;
	double k1[Nequ], k2[Nequ], k3[Nequ], k4[Nequ], g1[Nequ], g2[Nequ], g3[Nequ], g4[Nequ];

	for (i = 0; i < Nequ; i++)
	{
		k1[i] = 0.0;
		k2[i] = 0.0;
		k3[i] = 0.0;
		k4[i] = 0.0;
		g1[i] = 0.0;
		g2[i] = 0.0;
		g3[i] = 0.0;
		g4[i] = 0.0;

	}

	for (i = 0; i < Nequ; i++)
		y[i] = x[i];

	t = 0.0;
	j = 0;

	/* Integracao de Runge-Kutta para um periodo */
	while (j < Total)
	{
		t1 = t;
		for (i = 0; i < Nequ; i++)
		{
			g1[i] = y[i];
		}
		Func(k1, g1, t1, Wf);
		
		t2 = t + Step * 0.5;
		for (i = 0; i < Nequ; i++)
		{
			g2[i] = y[i] + k1[i] * 0.5*Step;
		}
		Func(k2, g2, t2, Wf);

		t3 = t + Step * 0.5;
		for (i = 0; i < Nequ; i++)
		{
			g3[i] = y[i] + k2[i] * 0.5*Step;
		}
		Func(k3, g3, t3, Wf);

		t4 = t + Step;
		for (i = 0; i < Nequ; i++)
		{
			g4[i] = y[i] + k3[i] * Step;
		}
		Func(k4, g4, t4, Wf);


		for (i = 0; i < Nequ; i++)
		{
			y[i] = y[i] + (k1[i] + 2.0*(k2[i] + k3[i]) + k4[i]) / 6.0 * Step;
		}

		t = t + Step;
		j++;
	}

	return(1);
}

/* ===========================  NewData  ===========================*/
void NewData(void)
{
	FILE    *fdread;
	long int i;
	double a, b, c, d;

	/* Get the new file name from the user and open it. */
	fdread = fopen("bacia.dat", "r");
	if (fdread == NULL) {
		printf("\n Nao foi possivel abrir arquivo bacia.dat!\n");
		exit(0);
		return;
	}

	rewind(fdread);

	/* Dimension doof space */
	fscanf(fdread, "%d\n", &PeriodoMaximo);

	/* Frequencia da forca excitadora, dados de amortecimentos e amplitudes da carga */
	fscanf(fdread, "%lf\n", &Wf);
	fscanf(fdread, "%lf %lf", &eta__1, &eta__2);
	fscanf(fdread, "%lf %lf %lf %lf %lf\n", &PL_8C, &PL_8S, &PL_9C, &PL_9S, &PL);

	/* Numero de células */
	fscanf(fdread, "%d\n", &Num_cel);

	/* Coordenadas de analise */
	fscanf(fdread, "%d %d\n", &Cor1, &Cor2);

	// verificacao das coordernadas a serem analisadas
	if (Cor1 >= Nequ && Cor2 >= Nequ && Cor1 == Cor2)
	{
		printf("Erro nas coordernadas a serem analisadas. Verifique! \n");
		exit(0);
	}

	/* Periodo da forca excitadora */
	Tf = 2 * Pi / Wf;

	/* Passo para integracao no tempo */
	Passo = Tf / 500;

	/* Alocacao na pilha de vetores para o tamanho da celula */
	q1 = (double*)calloc(Num_cel, sizeof(double));
	q1p = (double*)calloc(Num_cel, sizeof(double));


	/* Valor das coordenadas fixas para a analise */
	for (i = 0; i < Num_cel; i++)
	{
		//fscanf( fdread,"%lf %lf %lf %lf\n", &a, &b, &c, &d );
		fscanf(fdread, "%lf %lf\n", &a, &b);
		q1[i] = a;
		q1p[i] = b;

	}
	/*Valor dos limites*/
	fscanf(fdread, "%lf %lf %lf %lf\n", &a, &b, &c, &d);
	Y1min = a;
	Y1max = b;
	Y2min = c;
	Y2max = d;

	return;
}

/* ===========================  Tempo  ===========================*/
struct Tempo
{
	std::chrono::time_point<std::chrono::steady_clock> inicio, fim;
	std::chrono::duration<double> duracao;
	int nome_thd;

public:
	//constroi objeto
	Tempo(int const i)
	{
		nome_thd = i;
		inicio = std::chrono::high_resolution_clock::now();
	}
	// destroi objeto
	~Tempo()
	{
		fim = std::chrono::high_resolution_clock::now();
		duracao = fim - inicio;

		printf("->>Tempo de calculo da thread %d : %f s\n", nome_thd, duracao);
	}
};

void inicializaVariaveisdeEntrada(int const total_celulas, int const cell_inicio, float* x, float* y_old, float* xo, float* y) {

	//printf("\n Inicializando as variaveis de entrada. \n");
	//inicializa os dados de entrada de todas as celulas
	for (int i = 0; i < total_celulas; i++)
	{

		// inicializa todas as variaveis 
		for (int idx = 0; idx < Nequ; idx++)
		{
			x[idx*Num_cel + i] = 1e-4;
		}

		/* Coordenada de cada celula */
		x[Cor1*Num_cel + i] = (float)q1[i + cell_inicio];
		x[Cor2*Num_cel + i] = (float)q1p[i + cell_inicio];

		/* Coordenadas iniciais */
		for (int ij = 0; ij < Nequ; ij++)
		{
			y_old[ij*Num_cel + i] = x[ij*Num_cel + i];
			xo[ij*Num_cel + i] = x[ij*Num_cel + i];
			y[ij*Num_cel + i] = 0.0f;
		}
	}
}

/* ===========================  CellsTrajec  ===========================*/
void CellsTrajec(void)
{
	FILE *fd;

	int num_cells_thread;
	int n_max_thread;
	long int cellnum;

	/* Abre arquivo de impressao   */
	fd = fopen("bacia_results.txt", "w");
	if (fd == NULL) {
		printf("\n Nao foi possivel abrir arquivo de bacia_results.txt !\n");
		exit(0);
		return;
	}


	// inicializa threads de execucao
	printf("-->>>> Nao exceder a qtde de nucleos fisicos\n");
	printf("-->>>> Numero de threads : ");
#if 0
	scanf("%d", &n_max_thread);
	printf("\n\n");

	if (n_max_thread < 1)
	{
		printf("\n Numero maximo de threads menor que 1!\n");
		exit(0);
	}
#endif
	n_max_thread = 1;

	/* Imprime cabecalho na tela */
	printf("Thread  Numero            q1_atr               q1p_atr               Tempo   Periodicidade\n");

	int nome_thread = 1, cell_inicio = 0, cell_fim = Num_cel - 1;

	// inicializa timer
		// como ele esta limitado ao escopo da funcao executada pela thread
		// logo que a thread terminar e o escopo do timer acabar ele imprime a duracao
	Tempo tempo(1);

	//imprime executanto thread
	printf("Executando thread %2d: processo: %6d cell_init: %6d cell_fim: %6d\n",
		nome_thread, std::this_thread::get_id(), cell_inicio, cell_fim);

	//int const nome_thread = nome sequencial da thread utilizado para criar arquivo de saida
	//int const cell_inicio = valor de busca da primeira celula no vetor q1 e q1p
	//int const cell_fim = valor de busca da ultima celula no vetor q1 e q1p

	// declaracao das variaveis locais de controle
	int i, j, retorno, Periodo, PeriodoBack, flag, flag_periodica;
	int value, ij, Tempo;
	//declaracao dos dados do host
	float*	x = new float [Nequ*Num_cel];
	float*	y = new float[Nequ*Num_cel];
	float*	y_old = new float[Nequ*Num_cel];
	float*	xo = new float[Nequ*Num_cel];
	// declaracao dos atratores
	float derro, zo0atr, zo1atr;

	FILE *fd_thread;

	/* Abre arquivo de impressao   */
	char nome_arquivo[50];
	int arquivo_criado = sprintf(nome_arquivo, "bacia_results_%d.txt", nome_thread);
	fd_thread = fopen(nome_arquivo, "w");
	if (fd_thread == NULL && arquivo_criado != -1) {
		printf("\n Nao foi possivel abrir arquivo de %s !\n", nome_arquivo);
		exit(0);
		return;
	}

	int const total_celulas = cell_fim - cell_inicio + 1;

	std::vector<ID3D11ComputeShader*> vetorPonteiroComputeShader;

	printf("Inicializando variaveis das celulas...");
	inicializaVariaveisdeEntrada(total_celulas, cell_inicio, x, y_old, xo, y);
	printf("OK\n");

	printf("Criando Dispositivo...");
	if (FAILED(CreateComputeDevice(&g_pDevice, &g_pContext, false)))
		exit(0);
	printf("OK\n");

	printf("Criando Compute Shader...");
	if (FAILED(CreateComputeShader(L"kernel_f1.hlsl", "Func", g_pDevice, &g_pCS)))
		exit(0);
	printf("OK\n");

	vetorPonteiroComputeShader.push_back(g_pCS);

	printf("Criando os buffers na memoria da placa de video...");
	//buffer que contem as variaveis do sistema no inico de cada passo do RK
	CreateRawBuffer(g_pDevice, Nequ*Num_cel * sizeof(float), &x[0], &g_pBufferX);
	//buffer que armazena as variaveis do sistema no fim do passo do RK
	CreateRawBuffer(g_pDevice, Nequ*Num_cel * sizeof(float), &y[0], &g_pBufferY);
	printf("OK\n");


#if 0

	for (int i = 0; i < total_celulas; i++)
	{
		flag_periodica = -1;
		/* Contador de Tempo e de periodicidade da solucao */
		Tempo = 0;
		Periodo = 0;

		flag = 1;


		while (flag && y[Cor1] < 1e10)
		{
			/* Integracao no tempo */
			value = Runge_Kutta(y, x, Passo, 500);
			/* Conta numero de periodos de integracao e contador para periodicidade da solucao */
			Tempo++;
			Periodo++;
			/* Avalia se ponto calculado é o mesmo ao inicial  */
			retorno = 0;
			for (ij = 0; ij < Nequ; ij++)
			{
				derro = fabs(y[ij] - y_old[ij]);
				if (derro > 1.0e-8)
					retorno++;
				PeriodoBack = Periodo;
			}
			/*  Avalia retorno */
			if (retorno == 0)
			{
				flag = 0;
				continue;
			}
			/* Reinicia contador de periodos */
			if (Periodo == PeriodoMaximo)
			{
				Periodo = 0;
				/* Coordenadas iniciais para o novo ponto de analise (vetor x)*/
				for (ij = 0; ij < Nequ; ij++)
					y_old[ij] = y[ij];
			}
			/* Novo valor para vetor x  */
			for (ij = 0; ij < Nequ; ij++)
				x[ij] = y[ij];
			/* Limita o numero de integracoes, caso nao exista solucao periodica  */
			if (Tempo > 8000)
				flag = 0;
		}
		/* Celula mapeada (atrator)  */
		zo0atr = y[Cor1];
		zo1atr = y[Cor2];

		printf("%5d  /%6d / %6d", nome_thread, i + cell_inicio, cell_fim);
		printf("   %15.12e   %15.12e %8d  %2d  \n", y[Cor1], y[Cor2], Tempo, Periodo);
		/* Imprime resultados do atrator da celula */
		if (Tempo < 8000 || retorno == 0)
		{
			if (Y1min <= y[Cor1])
				//if(Y2min<=y[Cor1])
			{
				if (y[Cor1] <= Y1max)
					//if(y[Cor1]<=Y2max)
				{
					if (y[Cor2] >= Y2min)
						//if(y[Cor2]>=Y1min)
					{
						if (y[Cor2] <= Y2max)
							//if(y[Cor2]<=Y1max)
						{
							flag_periodica = 1;
						}
					}
				}
			}
		}

		fprintf(fd_thread, "%d, %16.12e, %16.12e, %16.12e, %16.12e,",
			(i + cell_inicio + 1)*flag_periodica, xo[Cor1], xo[Cor2], y[Cor1], y[Cor2]);
		for (j = 0; j < Nequ; j = j + 2)
		{
			fprintf(fd_thread, "%16.12e,  %16.12e,  ", y[j], y[j + 1]);
		}
		fprintf(fd_thread, "%5d, %5d\n", Tempo, nome_thread);
	}

	fclose(fd_thread);

	//system("pause");

	FILE *fd_thread;
	char arquivo_thd_leitura[50];
	char temp_c;
	int key_arquivo_leitura;
	//transferade dados dos arquivos de cada thread para o arquivo raiz
	for (int i = 0; i < n_max_thread; i++)
	{
		key_arquivo_leitura = sprintf(arquivo_thd_leitura, "bacia_results_%d.txt", i + 1);
		fd_thread = fopen(arquivo_thd_leitura, "r");
		if (fd_thread == NULL && key_arquivo_leitura != -1) {
			printf("\n Nao foi possivel abrir arquivo de %s !\n", arquivo_thd_leitura);
			exit(0);
			return;
		}

		// le o conteudo do arquivo e grava no principal 
		temp_c = fgetc(fd_thread);
		while (temp_c != EOF)
		{
			fputc(temp_c, fd);
			temp_c = fgetc(fd_thread);
		}
		fclose(fd_thread);

		if (remove(arquivo_thd_leitura) == -1)
		{
			perror("Erro de remocao do arqivo:");
		}
	}

	/*free(x);
	free(y_old);
	free(xo);
	free(y);*/
	fclose(fd);
#endif // 1
	return;
}




/* ===========================  main  ===========================*/
void main(void)
{

  printf("===============Calculo de Bacias de Atracao============\n");
  printf("==================Metodo  da Forca Bruta===============\n\n\n");
  NewData(  );

  //printf("entrei em Cells\n");
  CellsTrajec();
 
}


//--------------------------------------------------------------------------------------
// Create the D3D device and device context suitable for running Compute Shaders(CS)
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT CreateComputeDevice(ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut, bool bForceRef)
{
	*ppDeviceOut = nullptr;
	*ppContextOut = nullptr;

	HRESULT hr = S_OK;

	UINT uCreationFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	uCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL flOut;
	static const D3D_FEATURE_LEVEL flvl[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };

	bool bNeedRefDevice = false;
	if (!bForceRef)
	{
		hr = D3D11CreateDevice(nullptr,                        // Use default graphics card
			D3D_DRIVER_TYPE_HARDWARE,    // Try to create a hardware accelerated device
			nullptr,                        // Do not use external software rasterizer module
			uCreationFlags,              // Device creation flags
			flvl,
			sizeof(flvl) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION,           // SDK version
			ppDeviceOut,                 // Device out
			&flOut,                      // Actual feature level created
			ppContextOut);              // Context out

		if (SUCCEEDED(hr))
		{
			// A hardware accelerated device has been created, so check for Compute Shader support

			// If we have a device >= D3D_FEATURE_LEVEL_11_0 created, full CS5.0 support is guaranteed, no need for further checks
			if (flOut < D3D_FEATURE_LEVEL_11_0)
			{
				// Otherwise, we need further check whether this device support CS4.x (Compute on 10)
				D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
				(*ppDeviceOut)->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));
				if (!hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
				{
					bNeedRefDevice = true;
					printf("No hardware Compute Shader capable device found, trying to create ref device.\n");
				}
			}
		}
	}

	if (bForceRef || FAILED(hr) || bNeedRefDevice)
	{
		// Either because of failure on creating a hardware device or hardware lacking CS capability, we create a ref device here

		SAFE_RELEASE(*ppDeviceOut);
		SAFE_RELEASE(*ppContextOut);

		hr = D3D11CreateDevice(nullptr,                        // Use default graphics card
			D3D_DRIVER_TYPE_REFERENCE,   // Try to create a hardware accelerated device
			nullptr,                        // Do not use external software rasterizer module
			uCreationFlags,              // Device creation flags
			flvl,
			sizeof(flvl) / sizeof(D3D_FEATURE_LEVEL),
			D3D11_SDK_VERSION,           // SDK version
			ppDeviceOut,                 // Device out
			&flOut,                      // Actual feature level created
			ppContextOut);              // Context out
		if (FAILED(hr))
		{
			printf("Reference rasterizer device create failure\n");
			return hr;
		}
	}

	return hr;
}

//--------------------------------------------------------------------------------------
// Compile and create the CS
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT CreateComputeShader(LPCWSTR pSrcFile, LPCSTR pFunctionName,
	ID3D11Device* pDevice, ID3D11ComputeShader** ppShaderOut)
{
	if (!pDevice || !ppShaderOut)
		return E_INVALIDARG;

	// Finds the correct path for the shader file.
	// This is only required for this sample to be run correctly from within the Sample Browser,
	// in your own projects, these lines could be removed safely
	WCHAR str[MAX_PATH];
	HRESULT hr = FindDXSDKShaderFileCch(str, MAX_PATH, pSrcFile);
	if (FAILED(hr))
		return hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		nullptr, nullptr
	};

	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR pProfile = (pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pBlob = nullptr;
	hr = D3DCompileFromFile(str, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, pFunctionName, pProfile,
		dwShaderFlags, 0, &pBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		SAFE_RELEASE(pErrorBlob);
		SAFE_RELEASE(pBlob);

		return hr;
	}

	hr = pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, ppShaderOut);

	SAFE_RELEASE(pErrorBlob);
	SAFE_RELEASE(pBlob);

#if defined(_DEBUG) || defined(PROFILE)
	if (SUCCEEDED(hr))
	{
		(*ppShaderOut)->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(pFunctionName), pFunctionName);
	}
#endif

	return hr;
}

//--------------------------------------------------------------------------------------
// Tries to find the location of the shader file
// This is a trimmed down version of DXUTFindDXSDKMediaFileCch.
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT FindDXSDKShaderFileCch(WCHAR* strDestPath,
	int cchDest,
	LPCWSTR strFilename)
{
	if (!strFilename || strFilename[0] == 0 || !strDestPath || cchDest < 10)
		return E_INVALIDARG;

	// Get the exe name, and exe path
	WCHAR strExePath[MAX_PATH] =
	{
		0
	};
	WCHAR strExeName[MAX_PATH] =
	{
		0
	};
	WCHAR* strLastSlash = nullptr;
	GetModuleFileName(nullptr, strExePath, MAX_PATH);
	strExePath[MAX_PATH - 1] = 0;
	strLastSlash = wcsrchr(strExePath, TEXT('\\'));
	if (strLastSlash)
	{
		wcscpy_s(strExeName, MAX_PATH, &strLastSlash[1]);

		// Chop the exe name from the exe path
		*strLastSlash = 0;

		// Chop the .exe from the exe name
		strLastSlash = wcsrchr(strExeName, TEXT('.'));
		if (strLastSlash)
			*strLastSlash = 0;
	}

	// Search in directories:
	//      .\
    //      %EXE_DIR%\..\..\%EXE_NAME%

	wcscpy_s(strDestPath, cchDest, strFilename);
	if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
		return S_OK;

	swprintf_s(strDestPath, cchDest, L"%s\\%s", strExePath, strFilename);
	if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
		return S_OK;

	// On failure, return the file as the path but also return an error code
	wcscpy_s(strDestPath, cchDest, strFilename);

	return E_FAIL;
}


//--------------------------------------------------------------------------------------
// Create Raw Buffer
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT CreateRawBuffer(ID3D11Device* pDevice, UINT uSize, void* pInitData, ID3D11Buffer** ppBufOut)
{
	*ppBufOut = nullptr;

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER | D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = uSize;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return pDevice->CreateBuffer(&desc, nullptr, ppBufOut);
}




