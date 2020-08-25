# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import matplotlib.pyplot as pyplot
import numpy as np
import os
import pandas as pandas


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

### https://github.com/matplotlib/matplotlib/issues/5907#issuecomment-179001811
pyplot.rcParams['agg.path.chunksize'] = 10000

# %%
#pasta onde armazena os plots

path = "plots\\"
if not os.path.exists(path):
    os.makedirs(path)

# %%

### import data

data_donnell = pandas.read_csv('kutta.txt', header=None).to_numpy()



# %%

shape = data_donnell.shape

total_variaveis = 21

# %%

#numero de seções a serem lidas
ler=6000
ler=int(shape[0]*1.0)
	
	
	
# %%
for variavel in range(1,total_variaveis+1):

    nomde_grafico = 'Resposta no tempo C'+str(variavel)
    print(nomde_grafico)
    print("total de pontos plotados = %d" %(ler))

    # matplotlib plot
    pyplot.figure(figsize=(23, 10))

    data_1__x = []
    data_1__y = []

    print("total de pontos="+str(shape[0]))
    for j in range(shape[0]-ler,shape[0]):
        data_1__y.append(data_donnell[j,2*variavel])
        data_1__x.append(data_donnell[j,0])

    pyplot.plot(data_1__x,data_1__y,linewidth=1)
    deltax  = abs(min(data_1__x)- max(data_1__x))
    deltay  = abs(min(data_1__y)- max(data_1__y))

    pyplot.xlim(min(data_1__x)-0.0*deltax,max(data_1__x)+0.0*deltax)
    pyplot.ylim(min(data_1__y)-0.0*deltay,max(data_1__y)+0.0*deltay)
    pyplot.ylabel(r'$W_{'+str(variavel)+'}/h$')
    pyplot.xlabel(r't(s)')
    pyplot.ticklabel_format(axis='both',style='sci',scilimits=(0,0))
    pyplot.savefig("plots\\"+nomde_grafico+'.png',dpi=300,bbox_inches='tight')
    #pyplot.show()
    pyplot.close()




