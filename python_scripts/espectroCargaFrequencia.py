# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as pyplot
import numpy as np
import os
import pandas as pandas
from scanf import scanf

# %%
# template Jonathas Kennedy 26/03/2020
# código deve estar antes de criar figura pelo pyplot.figure(figsize=(10, 10))
# razão de tamnhos de fontes e ticks estabelecida para figuresize=10x10

pyplot.rcParams['xtick.labelsize'] = 30
pyplot.rcParams['ytick.labelsize'] = 30
pyplot.rcParams['xtick.major.size'] = 12
pyplot.rcParams['xtick.minor.size'] = 6
pyplot.rcParams['ytick.major.size'] = 12
pyplot.rcParams['ytick.minor.size'] = 6
pyplot.rcParams['ytick.minor.visible'] = True
pyplot.rcParams['xtick.minor.visible'] = True

pyplot.rcParams['axes.titlesize'] = 20
pyplot.rcParams['axes.labelpad'] = 20
pyplot.rcParams['axes.labelsize'] = 30

pyplot.rcParams['legend.fontsize'] = 16

### https://github.com/matplotlib/matplotlib/issues/5907#issuecomment-179001811
pyplot.rcParams['agg.path.chunksize'] = 10000


# %%
# pasta onde armazena os plots
def cria_pasta_plots(pontos):

    path = "espectro_freq_carga"
    path = path + pontos
    path = path + "\\"
    if not os.path.exists(path):
        os.makedirs(path)
    return path


# %%
# import data
def ler_dados(pontos):
    data = []
    for i in pontos:
        print("..\\ponto %s\\force.txt" % (i))
        data.append(
            pandas.read_csv("..\\ponto %s\\force.txt" % (i),
                            header=None).to_numpy())
    return data


# %%
# filtra dados
def filtraTrechos(data, pontos):
    data_del = []
    indice_pontos = -1
    for data_i in data:
        indice_pontos = indice_pontos + 1

        #numero de pontos a verificar no arquivo
        #le arquivo Forca.dat para extrair a numero de pontos
        arquivo = open("..\\ponto %s\\Forca.dat" % (pontos[indice_pontos]),
                       "r")
        arq_lines = arquivo.readlines()
        #trata os dados com o scanf
        pattern = '%d\n'
        num_pts_max_arquivo = scanf(pattern, arq_lines[2])[0]
        arquivo.close()

        i = -1
        for data_linha in data_i:
            i = i + 1
            #extrai numero do primeiro passo
            if i == 0:
                passo_anterior = data_linha[0]
            else:
                passo_atual = data_linha[0]
                #verifica se o passo atual não é maior que o passo anterior + 1
                if passo_atual > passo_anterior + 1:
                    print("erro no arquivo ponto %s\\force.txt => passo %s" %
                          (pontos[indice_pontos], passo_atual))
                    break
                else:
                    passo_anterior = passo_atual
                pass
        if passo_atual + 1 <= num_pts_max_arquivo:
            print("erro no arquivo ponto %s\\force.txt => passo %s" %
                  (pontos[indice_pontos], passo_atual))
        shape_data = data_i.shape
        colunas_deletar = slice(2, shape_data[1] - 1 - 1)
        data_del.append(np.delete(data_i, colunas_deletar, axis=1))
    return data_del


# %%
# analisa dados e gera os pontos de início e fim(duracao) das barras dos trechos instaveis
def geraDadosBarras(data, pontos):
    num_max_tentativas = 20
    num_max_tentativas = num_max_tentativas - 1

    data_freq = []
    for i in pontos:
        #le arquivo Forca.dat para extrair a frequencia
        arquivo = open("..\\ponto %s\\Forca.dat" % (i), "r")
        arq_lines = arquivo.readlines()
        #trata os dados com o scanf
        pattern = '%d  %f \n'
        dados = scanf(pattern, arq_lines[4])
        data_freq.append(dados[1])

    data_plot_freq = []
    data_plot_duracao_carga = []
    data_plot_carga_inicial = []
    #percorre os dados procurando os trechos intaveis
    contador = -1
    for data_i in data:
        contador = contador + 1
        trecho_estavel = True
        encontrou_trechoAnalise = False
        possivel_fim_trechoAnalise = False
        for data_linha in data_i:
            if data_linha[2] > 1 and encontrou_trechoAnalise == False:
                #possivel trecho instavel
                if data_linha[2] > num_max_tentativas:
                    #trecho instavel
                    trecho_estavel = False
                    encontrou_trechoAnalise = True
            elif data_linha[2] < 2:
                if encontrou_trechoAnalise == False:
                    carga_inicial = data_linha[1]
                elif encontrou_trechoAnalise == True and possivel_fim_trechoAnalise == False:
                    #possivel termino do trecho_de analise instavel
                    possivel_fim_trechoAnalise = True
                    carga_inicial_temp = data_linha[1]
                else:
                    trecho_estavel = True
                    possivel_fim_trechoAnalise = False
            elif possivel_fim_trechoAnalise == True:
                possivel_fim_trechoAnalise = False

            if trecho_estavel == True and encontrou_trechoAnalise == True:
                encontrou_trechoAnalise = False
                trecho_estavel = True
                data_plot_freq.append(data_freq[contador])
                data_plot_duracao_carga.append(carga_inicial_temp -
                                               carga_inicial)
                data_plot_carga_inicial.append(carga_inicial)
                carga_inicial = carga_inicial_temp

        if encontrou_trechoAnalise == True:
            data_plot_freq.append(data_freq[contador])
            data_plot_duracao_carga.append(data_linha[1] - carga_inicial)
            data_plot_carga_inicial.append(carga_inicial)
    return data_plot_freq, data_plot_duracao_carga, data_plot_carga_inicial


# %%
# funcao gera e imprime grafico
def gera_plot(path, data_plot_freq, data_plot_duracao_carga,
              data_plot_carga_inicial, correcao_frequencia, pontos):
    import hashName
    pasta_hash: str = hashName.nome_hash(os.getcwd() + "\\" + path)

    nomde_grafico = pasta_hash + '_Espectro'
    print(nomde_grafico)

    # matplotlib plot
    # figura deve ser definida como subplots e retornas os axes para posterior configuracao do tick format
    fig, ax = pyplot.subplots(1, 1, figsize=(10, 10))

    freq_unica = set(data_plot_freq)
    num_pontos = len(freq_unica)
    minData1X = min(data_plot_freq)
    maxData1X = max(data_plot_freq)
    tamanho_barra = 1.1 * (maxData1X - minData1X) / (num_pontos)

    config_plot = dict(width=tamanho_barra, bottom=data_plot_carga_inicial)
    ax.bar(data_plot_freq, data_plot_duracao_carga, **config_plot)

    # saving file to load in another python file
    # https://stackoverflow.com/questions/48912527/how-to-join-two-matplotlib-figures
    np.savez(path + nomde_grafico + '.npz',
             method='bar',
             args=(data_plot_freq, data_plot_duracao_carga),
             kwargs=config_plot)

    pyplot.xlim(minData1X, maxData1X)
    pyplot.ylim(0, )
    #ax.xaxis.set_major_formatter(pyplot.FuncFormatter('{:.2f}'.format))
    #ax.yaxis.set_major_formatter(pyplot.FuncFormatter('{:.2f}'.format))
    pyplot.savefig(path + nomde_grafico + '.png', dpi=300, bbox_inches='tight')
    pyplot.show()
    pyplot.cla()
    pyplot.clf()
    pyplot.close('all')


# %%
if __name__ == "__main__":
    import stringParserPoints
    pontos = str(input("Caminhos para plotar <caminho 1,caminho 2>: "))
    pontos, palavraPath = stringParserPoints.SeparaString(pontos)
    path = cria_pasta_plots(palavraPath)
    data = ler_dados(pontos)
    data = filtraTrechos(data, pontos)
    data_plot_freq, data_plot_duracao_carga, data_plot_carga_inicial = geraDadosBarras(
        data, pontos)
    shape = data[0].shape
    # correcao de frequencia
    correcao_frequencia = None
    if (correcao_frequencia == None):
        correcao_frequencia = 1.0
    total_variaveis = (shape[1] - 4) / 4
    gera_plot(path, data_plot_freq, data_plot_duracao_carga,
              data_plot_carga_inicial, correcao_frequencia, pontos)
