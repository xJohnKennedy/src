# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as pyplot
import numpy as np
import csv


# %%
## template Jonathas Kennedy 26/03/2020
## código deve estar antes de criar figura pelo pyplot.figure(figsize=(10, 10))
## razão de tamnhos de fontes e ticks estabelecida para figuresize=10x10

pyplot.rcParams['xtick.labelsize']=30
pyplot.rcParams['ytick.labelsize']=30
pyplot.rcParams['xtick.major.size']=12
pyplot.rcParams['xtick.minor.size']=6
pyplot.rcParams['ytick.major.size']=12
pyplot.rcParams['ytick.minor.size']=6
pyplot.rcParams['ytick.minor.visible']=True
pyplot.rcParams['xtick.minor.visible']=True


pyplot.rcParams['axes.titlesize']=20
pyplot.rcParams['axes.labelpad']=20
pyplot.rcParams['axes.labelsize']=30

pyplot.rcParams['legend.fontsize']=16

# %%
#pasta onde armazena os plots
import os
path = "bifurcacao\\"
if not os.path.exists(path):
    os.makedirs(path)
    
nome_base_grafico = 'bifurcacao_C'

# %%

total_variaveis = 21
correcao_frequencia = 0.95176

# %%
### import data
data = []
with open("..\\ponto 1\\force.txt", 'r') as arq:
    data_donnell = csv.reader(arq, delimiter=",")
    data.append( np.array(list(data_donnell), dtype=float))
    
### import data
with open("..\\ponto 2\\force.txt", 'r') as arq:
    data_donnell = csv.reader(arq, delimiter=",")
    data.append( np.array(list(data_donnell), dtype=float))

# %%


for variavel in range(1,total_variaveis+1):
    nomde_grafico = nome_base_grafico + str(variavel)
    print(nomde_grafico)
    col = 4*variavel-2
    print("col = %d" %(col) )
    
    # matplotlib plot
    pyplot.figure(figsize=(10, 10))
    
    data_1__x = []
    data_1__y = []

    #plotagem do ponto 1
    shape = data[0].shape
    for j in range(0,shape[0]):
        data_1__y.append(data[0][j,col])
        data_1__x.append(data[0][j,1]*correcao_frequencia)
    print("Nummero de pontos caminho 1 = "+str(shape[0]))
    
    config_plot=dict(marker='o', color='blue', s=10)
    pyplot.scatter(data_1__x,data_1__y, **config_plot)

    # saving file to load in another python file
    # https://stackoverflow.com/questions/48912527/how-to-join-two-matplotlib-figures
    np.savez(path+nomde_grafico+'_caminho_1.npz', method='scatter', \
                            args=(data_1__x, data_1__y), kwargs=config_plot)
    
    #plotagem do ponto 2
    data_1__x = []
    data_1__y = []

    shape = data[1].shape
    for j in range(0,shape[0]):
        data_1__y.append(data[1][j,col])
        data_1__x.append(data[1][j,1]*correcao_frequencia)
    print("Nummero de pontos caminho 2 = "+str(shape[0]))
    
    config_plot=dict(marker='o', color='blue', s=10, label='21 d.o.f')
    pyplot.scatter(data_1__x,data_1__y, **config_plot)

    # saving file to load in another python file
    # https://stackoverflow.com/questions/48912527/how-to-join-two-matplotlib-figures
    np.savez(path+nomde_grafico+'_caminho_2.npz', method='scatter', \
                            args=(data_1__x, data_1__y), kwargs=config_plot)
    
    #pyplot.xlim(min(data_1__x)-0.1*abs(min(data_1__x)),max(data_1__x)+0.1*abs(max(data_1__x)))
    #pyplot.ylim(0,2)
    pyplot.xlabel(r'$\omega_{1}/\omega_{0}$')
    pyplot.ylabel(r'$W_{'+str(variavel)+'}/h$')
    pyplot.legend(loc='upper right',ncol=1)
    pyplot.ticklabel_format(axis='both',style='sci',scilimits=(0,0))
    #pyplot.savefig(path+nomde_grafico+'.svg',dpi=900,bbox_inches='tight')
    pyplot.savefig(path+nomde_grafico+'.png',dpi=300,bbox_inches='tight')
    #pyplot.show()
    pyplot.close()


# %%