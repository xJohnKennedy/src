# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as pyplot
import numpy as np
import os
import pandas as pandas
import sys

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
def cria_pasta_plots():

    path = "plots_bacia_atracao\\"
    if not os.path.exists(path):
        os.makedirs(path)
    return path


# %%
# import data
def ler_dados():
    data = []
    data.append(
        pandas.read_csv(".\\bacia_results.txt", header=None).to_numpy())
    return data


# %%
# funcao gera e imprime grafico
def gera_plot(path, data, total_linhas, total_colunas, correcao_frequencia, x1,
              x2, y1, y2, n_div_x: int, n_div_y: int):
    import hashName
    pasta_hash: str = hashName.nome_hash(os.getcwd() + "\\" + path)

    #lista de cores
    lista_cores = [
        "white", "blue", "yellow", "green", "red", "purple", "black"
    ]
    lista_cores_indices = [0, 1, 2, 3, 4, 5, 6]

    print(lista_cores, lista_cores_indices)
    user_input = str(
        input(
            "\n Mudar indices da lista de cores?: \n ex= Digitar 0,1,2,3,5,4,6 troca o purple e o red \n Nova lista?: "
        ))
    if user_input != '':
        nova_lista_cores = []
        user_input = user_input.split(",")
        for temp in user_input:
            nova_lista_cores.append(lista_cores[int(temp)])
        lista_cores = nova_lista_cores

    # calculo de dx e dy
    dx = abs((x2 - x1) / (n_div_x - 1))
    dy = abs((y2 - y1) / (n_div_y - 1))

    y, x = np.mgrid[slice(y1 - dy / 2, y2 + dy, dy),
                    slice(x1 - dx / 2, x2 + dx, dx)]

    z = []
    z0 = np.zeros((n_div_y, n_div_x))

    ################
    #cria um array com os atratorez de cada célula
    from scipy import spatial

    listaAtratores = np.zeros((total_linhas, 2))
    listaIndices = np.zeros(total_linhas, dtype='int32')
    for i in range(total_linhas):
        listaAtratores[i] = np.array([data[i][3], data[i][4]])
        listaIndices[i] = i

    ## variavel de controle do nível de plotagem de cada atrator
    nivel = 6
    ArquivoAberto = False

    #verifica se a lista de atratores nao esta vazia
    while listaAtratores.size != 0:
        id_periodico = 0
        flag_convergiu = False
        #pesquisa os atratores que convergiram
        while (id_periodico + 1) < listaIndices.size:
            indice = data[listaIndices[id_periodico]][0]
            if indice < 0:
                id_periodico = id_periodico + 1
            else:
                flag_convergiu = True
                break
        #monta e pesquisa os pontos proximos
        if flag_convergiu == True:
            #https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.KDTree.query_ball_point.html#scipy.spatial.KDTree.query_ball_point
            tree = spatial.KDTree(listaAtratores)
            pontosProximos = sorted(
                tree.query_ball_point(listaAtratores[id_periodico], 0.01))
            # imprime na tela o atrator que está sob analise
            print("==> Atrator encontrado[{:d}] d1 = {:f}, d2 = {:f}\n".format(
                listaIndices[id_periodico] + 1,
                listaAtratores[id_periodico][0],
                listaAtratores[id_periodico][1]))
        else:
            pontosProximos = np.zeros(listaIndices.size, dtype='int32')
            for i in range(listaIndices.size):
                pontosProximos[i] = i

        if flag_convergiu == True:
            for j in range(total_colunas - 6):
                sys.stdout.flush()
                print("verificando dados coluna {:d} para o atrator".format(
                    int(j + 6)),
                      end="\r")
                listaAtratoresVerificacao = np.zeros((len(pontosProximos), 1))
                for i in range(len(pontosProximos)):
                    linhaLerData = listaIndices[pontosProximos[i]]
                    listaAtratoresVerificacao[i] = np.array(
                        data[linhaLerData][5 + j])
                    pass
                #https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.KDTree.query_ball_point.html#scipy.spatial.KDTree.query_ball_point
                tree = spatial.KDTree(listaAtratoresVerificacao)
                pontosProximosVerificacao = sorted(
                    tree.query_ball_point(listaAtratoresVerificacao[0], 0.01))
                #extrai de pontos proximos somente aqueles que passaram na verificacao
                pontosProximos = sorted(
                    np.take(pontosProximos, pontosProximosVerificacao))
                pass
            sys.stdout.flush()
            pass

        #definida a lista de pontos proximos determina para ela um nivel de plotagem em base de 2
        listaPontosExcluir = []
        listaIndicesConvergidosImprimir = []
        indice = 0
        for i in pontosProximos:
            if flag_convergiu == True and nivel > 0:
                norma = 64 - (6 - nivel) * 10
            elif flag_convergiu == True and nivel == 0:
                raise ValueError(
                    'A quantidade de atratores eh igual ao maximo de cores do Cmap,' \
                    + 'nao sendo possivel representar os pontos que nao convergiram!'
                )
            else:
                norma = 0
            indiceBuscarData = listaIndices[i]
            numCelula = data[indiceBuscarData][0]
            if numCelula < 0 and flag_convergiu == True:
                listaPontosExcluir.append(indice)
            else:
                listaIndicesConvergidosImprimir.append(indiceBuscarData + 1)
            # define linha em x
            numCelula = abs(numCelula)
            x_l = int((numCelula - 1) / n_div_y)
            y_l = int((numCelula - 1) % n_div_y)
            z0[y_l, x_l] = norma
            indice += 1
        if flag_convergiu == True:
            print("==> Nivel = {:f}\n".format(norma))
            if ArquivoAberto == False:
                f = open(path + 'impBaciaLog_' + pasta_hash[0:12] + '.txt',
                         "w")
                ArquivoAberto = True
            else:
                f = open(path + 'impBaciaLog_' + pasta_hash[0:12] + '.txt',
                         "a")
            f.write("Atrator encontrado[{:d}] d1 = {:f}, d2 = {:f}\n".format(
                listaIndices[id_periodico] + 1,
                listaAtratores[id_periodico][0],
                listaAtratores[id_periodico][1]))
            f.write("Nivel de cor = {:d}, Nome da cor = {:s}\n".format(
                norma, lista_cores[nivel]))
            f.write("Numero de pontos = {:d}\n".format(
                len(listaIndicesConvergidosImprimir)))
            np.savetxt(f, listaIndicesConvergidosImprimir, fmt='%.0f')
            f.write("\n\n")
            f.close()

        #deleta os pontos proximos encontrados mas que nao convergiram da lista de pontos proximos
        pontosProximos = np.delete(pontosProximos, listaPontosExcluir, 0)
        #pontosProximos = np.insert(pontosProximos, 1, id_periodico)
        listaAtratores = np.delete(listaAtratores, pontosProximos, 0)
        listaIndices = np.delete(listaIndices, pontosProximos)
        nivel -= 1

    from matplotlib.colors import BoundaryNorm
    from matplotlib.ticker import MaxNLocator
    from matplotlib.colors import ListedColormap

    cmap = ListedColormap(lista_cores)
    levels = MaxNLocator(nbins=len(lista_cores)).tick_values(0, 64)
    norm = BoundaryNorm(levels, ncolors=cmap.N, clip=False)

    # matplotlib plot
    # figura deve ser definida como subplots e retornas os axes para posterior configuracao do tick format
    fig, ax = pyplot.subplots(1, 1, figsize=(10, 10))

    nomde_grafico = pasta_hash
    print(nomde_grafico)

    config_plot = dict(cmap=cmap, norm=norm)
    arg_plot = (x, y, z0)
    im = ax.pcolormesh(*arg_plot, **config_plot)
    #fig.colorbar(im, ax=ax)

    # saving file to load in another python file
    # https://stackoverflow.com/questions/48912527/how-to-join-two-matplotlib-figures
    np.savez(path + nomde_grafico + '.npz',
             method='pcolormesh',
             args=arg_plot,
             kwargs=config_plot)

    minData1X = x.min()
    maxData1X = x.max()
    minData1Y = y.min()
    maxData1Y = y.max()

    pyplot.xlim(minData1X - 0.01 * abs(minData1X),
                maxData1X + 0.01 * abs(maxData1X))
    pyplot.ylim(minData1Y - 0.01 * abs(minData1Y),
                maxData1Y + 0.01 * abs(maxData1Y))

    pyplot.ylabel(r'$\dot{W}_{' + str(1) + '}/h$')
    pyplot.xlabel(r'$W_{' + str(1) + '}/h$')
    pyplot.ticklabel_format(axis='both',
                            style='sci',
                            scilimits=(0, 0),
                            useOffset=False)
    #ax.xaxis.set_major_formatter(pyplot.FuncFormatter('{:.2f}'.format))
    #ax.yaxis.set_major_formatter(pyplot.FuncFormatter('{:.2f}'.format))
    #pyplot.colorbar(im, ax=ax)
    pyplot.savefig(path + nomde_grafico + '.png', dpi=300, bbox_inches='tight')
    #pyplot.show()
    pyplot.cla()
    pyplot.clf()
    pyplot.close('all')


def perguntaConfigPlotagemBacia():
    user_input = str(
        input(
            "\n =>> Digite configuracoes de plotagem no formato [x1, x2, y1, y2, numDiv_dx, numDiv_dy]:  "
        ))
    user_input = user_input.split(",")
    return float(user_input[0]), float(user_input[1]), float(
        user_input[2]), float(user_input[3]), int(user_input[4]), int(
            user_input[5])


def verificaConfigPlotagemBacia():
    arquivoPasta = os.path.dirname(__file__)
    arquivoPath = arquivoPasta + "\\baciaPlotConfig.txt"

    try:
        #tenta abrir o arquivo de configuracao da ultima plotagem da bacia de atracao
        #o arquivo de configuracao fica no mesmo diretorio que o arquivo bacia_atracao.py

        #abre o arquivo
        arquivo = open(arquivoPath, "r")
        x1 = float(arquivo.readline())
        x2 = float(arquivo.readline())
        y1 = float(arquivo.readline())
        y2 = float(arquivo.readline())
        numDiv_dx = int(arquivo.readline())
        numDiv_dy = int(arquivo.readline())
        arquivo.close()

        print("\n =>> Configuracoes de plotagem da bacia de atracao")
        print(
            "\n x1= {:.5f}  x2= {:.5f}  y1= {:.5f}  y2= {:.5f}  numDiv_dx= {:d}  numDiv_dy= {:d}"
            .format(x1, x2, y1, y2, numDiv_dx, numDiv_dy))
        user_input = str(input("\n Manter estes dados [y/n]:  "))
        if user_input.lower() == "n":
            x1, x2, y1, y2, numDiv_dx, numDiv_dy = perguntaConfigPlotagemBacia(
            )
    except FileNotFoundError:
        x1, x2, y1, y2, numDiv_dx, numDiv_dy = perguntaConfigPlotagemBacia()

    #imprime o arquivo de configuracao caso nenhum erro tenha ocorrido e retorna os dados de config
    arquivo = open(arquivoPath, "w")
    arquivo.write("{:.5f}\n{:.5f}\n{:.5f}\n{:.5f}\n{}\n{}".format(
        x1, x2, y1, y2, numDiv_dx, numDiv_dy))
    arquivo.close()

    return x1, x2, y1, y2, numDiv_dx, numDiv_dy


def main_func():
    #solucao para erro recursivo quando executando a busca kd-tree em grandes matrizes
    #https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.KDTree.html#scipy.spatial.KDTree
    sys.setrecursionlimit(10000)
    x1, x2, y1, y2, numDiv_dx, numDiv_dy = verificaConfigPlotagemBacia()
    path = cria_pasta_plots()
    data = ler_dados()
    shape = data[0].shape
    # correcao de frequencia
    correcao_frequencia = None
    if (correcao_frequencia == None):
        correcao_frequencia = 1.0
    total_linhas = shape[0]
    total_colunas = shape[1]
    gera_plot(path, data[0], total_linhas, total_colunas, correcao_frequencia,
              x1, x2, y1, y2, numDiv_dx, numDiv_dy)


# %%
if __name__ == "__main__":
    main_func()
