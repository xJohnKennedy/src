# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as pyplot
import numpy as np
import os
import pandas as pandas

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

    path = "ressonancia"
    for i in pontos:
        path = path + "_c%s" % (i)
    path = path + "\\"
    if not os.path.exists(path):
        os.makedirs(path)
    return path


# %%
# import data
def ler_dados(pontos):
    data = []
    for i in pontos:
        data.append(
            pandas.read_csv("..\\ponto %s\\force.txt" % (i),
                            header=None).to_numpy())
    return data


# %%
# funcao gera e imprime grafico
def gera_plot(path, data, total_variaveis, correcao_frequencia):
    import hashName
    pasta_hash: str = hashName.nome_hash(os.getcwd() + "\\" + path)
    for variavel in range(1, int(total_variaveis + 1)):
        nomde_grafico = pasta_hash + '_C%s' % (variavel)
        print(nomde_grafico)
        col = 4 * variavel
        print("col = %d" % (col))

        # matplotlib plot
        # figura deve ser definida como subplots e retornas os axes para posterior configuracao do tick format
        fig, ax = pyplot.subplots(1, 1, figsize=(10, 10))

        Data_caminhos = [0, 1]
        minData1X = 1e9
        maxData1X = -1e9
        minData1Y = 1e9
        maxData1Y = -1e9

        for i in Data_caminhos:
            data_1__x = []
            data_1__y = []

            shape = data[i].shape
            for j in range(0, shape[0]):
                data_1__y.append(data[i][j, col])
                data_1__x.append(data[i][j, 1] * correcao_frequencia)
            print("Nummero de pontos caminho %i = %i" % (i + 1, shape[0]))

            config_plot = dict(marker='o', color='blue', s=10)
            pyplot.scatter(data_1__x, data_1__y, **config_plot)

            # saving file to load in another python file
            # https://stackoverflow.com/questions/48912527/how-to-join-two-matplotlib-figures
            np.savez(path + nomde_grafico + '_c_' + str(i + 1) + '.npz',
                     method='scatter',
                     args=(data_1__x, data_1__y),
                     kwargs=config_plot)

            #calculo maximos e minimos do grafico
            minData1X = min([min(data_1__x), minData1X])
            maxData1X = max([max(data_1__x), maxData1X])
            minData1Y = min([min(data_1__y), minData1Y])
            maxData1Y = max([max(data_1__y), maxData1Y])

        pyplot.xlim(minData1X - 0.01 * abs(minData1X),
                    maxData1X + 0.01 * abs(maxData1X))
        pyplot.ylim(minData1Y - 0.01 * abs(minData1Y),
                    maxData1Y + 0.01 * abs(maxData1Y))
        pyplot.ylabel(r'$W_{' + str(variavel) + '}/h$')
        pyplot.xlabel(r'$\omega_{1}/\omega_{0}$')
        pyplot.ticklabel_format(axis='both',
                                style='sci',
                                scilimits=(0, 0),
                                useOffset=False)
        #ax.xaxis.set_major_formatter(pyplot.FuncFormatter('{:.2f}'.format))
        #ax.yaxis.set_major_formatter(pyplot.FuncFormatter('{:.2f}'.format))
        pyplot.savefig(path + nomde_grafico + '.png',
                       dpi=300,
                       bbox_inches='tight')
        #pyplot.show()
        pyplot.cla()
        pyplot.clf()
        pyplot.close('all')


# %%
if __name__ == "__main__":
    pontos = str(input("Caminhos para plotar <caminho 1,caminho 2>: "))
    pontos = pontos.split(",")
    path = cria_pasta_plots(pontos)
    data = ler_dados(pontos)
    shape = data[0].shape
    # correcao de frequencia
    correcao_frequencia = None
    if (correcao_frequencia == None):
        correcao_frequencia = 1.0
    total_variaveis = (shape[1] - 4) / 4
    gera_plot(path, data, total_variaveis, correcao_frequencia)
