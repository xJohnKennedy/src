# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as pyplot
import numpy as np
import csv
import os
import pandas as pandas

# %%
## template Jonathas Kennedy 26/03/2020
## código deve estar antes de criar figura pelo pyplot.figure(figsize=(10, 10))
## razão de tamnhos de fontes e ticks estabelecida para figuresize=10x10

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


# %%
# pasta onde armazena os plots
def cria_pasta_plots():

    path = ".\\"
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
def gera_plot(path,
              data,
              variavel,
              tipo_plot,
              correcao_frequencia=1.0,
              ax_plt_window=None,
              wdw_aux=None):
    import hashName
    pasta_hash: str = hashName.nome_hash(os.getcwd() + "\\" + path)

    if tipo_plot == 'R':
        nome_wdw = 'RE_'
        nome_base_grafico = '_RESS_C'
        col = 4 * variavel
    if tipo_plot == 'B':
        nome_wdw = 'BP_'
        nome_base_grafico = '_BIFU_C'
        col = 4 * variavel - 2

    for var in range(0, 2):
        nomde_grafico = pasta_hash + nome_base_grafico + "%d_ponto_%d" % (
            variavel, var + 1)
        print(nomde_grafico)
        print("col = %d" % (col))

        if var == 0:
            plt_color = 'blue'
        else:
            plt_color = 'red'

        # matplotlib plot
        fig, ax = pyplot.subplots(num='%s%d_ponto_%d' %
                                  (nome_wdw, variavel, var + 1),
                                  figsize=(10, 10))

        data_1__x = []
        data_1__y = []

        #plotagem do ponto 1
        shape = data[var].shape
        for j in range(0, shape[0]):
            data_1__y.append(data[var][j, col])
            data_1__x.append(data[var][j, 1] * correcao_frequencia)
        print("Nummero de pontos caminho 1 = " + str(shape[0]))

        ax.scatter(data_1__x, data_1__y, marker='o', color=plt_color, s=10)

        if ax_plt_window != None:
            ax_plt_window.scatter(data_1__x,
                                  data_1__y,
                                  marker='o',
                                  color=plt_color,
                                  s=10)

        #pyplot.xlim(min(data_1__x)-0.1*abs(min(data_1__x)),max(data_1__x)+0.1*abs(max(data_1__x)))
        #pyplot.ylim(0,2)
        ax.set_xlabel(r'$\omega_{1}/\omega_{0}$')
        ax.set_ylabel(r'$W_{' + str(variavel) + '}/h$')
        #ax.legend(loc='upper right', ncol=1)
        #ax.ticklabel_format(axis='both', style='sci', scilimits=(0, 0))
        fig.savefig(path + nomde_grafico + '.png',
                    dpi=300,
                    bbox_inches='tight')
        if wdw_aux.upper() == "S" or wdw_aux.upper() == "Y":
            continue
        else:
            pyplot.close(fig)


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

    variavel = int(input("\nVariavel:  "))
    wdw_aux = str(input("\nExibe secundarios [S/N]:  "))

    fig1, ax1 = pyplot.subplots(num='BP_%d' % (variavel), figsize=(10, 10))
    gera_plot(path, data, variavel, 'B', correcao_frequencia, ax1, wdw_aux)

    fig2, ax2 = pyplot.subplots(num='RE_%d' % (variavel), figsize=(10, 10))
    gera_plot(path, data, variavel, 'R', correcao_frequencia, ax2, wdw_aux)

    pyplot.show()