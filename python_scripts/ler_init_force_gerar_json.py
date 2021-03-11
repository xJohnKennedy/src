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
