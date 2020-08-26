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
def cria_pasta_plots():

    path = "ressonancia\\"
    if not os.path.exists(path):
        os.makedirs(path)
    return path


# %%
# import data
def ler_dados():
    data = []
    data.append(
        pandas.read_csv("..\\ponto 1\\force.txt", header=None).to_numpy())
    data.append(
        pandas.read_csv("..\\ponto 2\\force.txt", header=None).to_numpy())
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

        data_1__x = []
        data_1__y = []

        #plotagem do ponto 1
        shape = data[0].shape
        for j in range(0, shape[0]):
            data_1__y.append(data[0][j, col])
            data_1__x.append(data[0][j, 1] * correcao_frequencia)
        print("Nummero de pontos caminho 1 = " + str(shape[0]))

        config_plot = dict(marker='o', color='blue', s=10)
        pyplot.scatter(data_1__x, data_1__y, **config_plot)

        # saving file to load in another python file
        # https://stackoverflow.com/questions/48912527/how-to-join-two-matplotlib-figures
        np.savez(path + nomde_grafico + '_c_1.npz',
                 method='scatter',
                 args=(data_1__x, data_1__y),
                 kwargs=config_plot)

        #plotagem do ponto 2
        data_1__x = []
        data_1__y = []

        shape = data[1].shape
        for j in range(0, shape[0]):
            data_1__y.append(data[1][j, col])
            data_1__x.append(data[1][j, 1] * correcao_frequencia)
        print("Nummero de pontos caminho 2 = " + str(shape[0]))

        config_plot = dict(marker='o', color='blue', s=10, label='21 d.o.f')
        pyplot.scatter(data_1__x, data_1__y, **config_plot)

        # saving file to load in another python file
        # https://stackoverflow.com/questions/48912527/how-to-join-two-matplotlib-figures
        np.savez(path + nomde_grafico + '_c_2.npz',
                 method='scatter',
                 args=(data_1__x, data_1__y),
                 kwargs=config_plot)

        pyplot.xlim(
            min(data_1__x) - 0.01 * abs(min(data_1__x)),
            max(data_1__x) + 0.01 * abs(max(data_1__x)))
        pyplot.ylim(
            min(data_1__y) - 0.01 * abs(min(data_1__y)),
            max(data_1__y) + 0.01 * abs(max(data_1__y)))
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
    path = cria_pasta_plots()
    data = ler_dados()
    shape = data[0].shape
    # correcao de frequencia
    correcao_frequencia = None
    if (correcao_frequencia == None):
        correcao_frequencia = 1.0
    total_variaveis = (shape[1] - 4) / 4
    gera_plot(path, data, total_variaveis, correcao_frequencia)
