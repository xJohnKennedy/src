# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as pyplot
import numpy as np
import os
import pandas as pandas
import json

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
# import data
def ler_dados(pasta):
    data = []
    data.append(
        pandas.read_csv(".\\" + pasta + "\\init_force.txt",
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
        col = 4 * variavel - 2
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
    pasta_name = 'f4_'
    pasta = 'ponto ' + pasta_name
    data = ler_dados(pasta + '1')
    shape = data[0].shape
    tamanho_data = len(data[0])
    namejobs = []
    cont = 1
    dd = {}
    my_data_file = open('data_temp.json', 'w')

    for i in range(tamanho_data):
        line = i + 1
        namejobs_temp = "job " + pasta_name + str(i + 1)
        if line < tamanho_data:
            if data[0][i][1] != data[0][i + 1][1]:
                namejobs.append(namejobs_temp)

                dd.update({
                    namejobs_temp: {
                        "_desc_1": "novo trabalho para executar",
                        "_desc_2":
                        "nome do processo que ira executar para monitoramento",
                        "process": "Forca_br",
                        "_desc_3":
                        "modo execucao: 0 = mesma janela, 1 = nova janela",
                        "mode": 1,
                        "cwd": ".\\" + pasta + str(cont),
                        "bat": "Forca_br.bat",
                        "args": "2 " + str(line)
                    }
                })
                cont = cont + 1
                #print(dd)
        else:
            namejobs.append(namejobs_temp)
            dd.update({
                namejobs_temp: {
                    "_desc_1": "novo trabalho para executar",
                    "_desc_2":
                    "nome do processo que ira executar para monitoramento",
                    "process": "Forca_br",
                    "_desc_3":
                    "modo execucao: 0 = mesma janela, 1 = nova janela",
                    "mode": 1,
                    "cwd": ".\\" + pasta + str(cont),
                    "bat": "Forca_br.bat",
                    "args": "2 " + str(line)
                }
            })
            cont = cont + 1
            #print(dd)
        dd.update({"nameJobs": namejobs})

json.dump(dd, my_data_file)
