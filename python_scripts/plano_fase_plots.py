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

    path = "plots\\"
    if not os.path.exists(path):
        os.makedirs(path)
    return path


# %%
# import data
def ler_dados():
    data_donnell = pandas.read_csv('kutta.txt', header=None).to_numpy()
    return data_donnell


# %%
#https://stackoverflow.com/questions/48912527/how-to-join-two-matplotlib-figures
def juntar_plots(ax, files):
    data = [np.load(filename, allow_pickle=True) for filename in files]
    for datum in data:
        method = getattr(ax, datum['method'].item())
        args = tuple(datum['args'])
        kwargs = datum['kwargs'].item()
        method(*args, **kwargs)


# %%
# funcao gera e imprime grafico
def gera_plot(path, data, ler, shape, total_variaveis):
    for variavel in range(1, int(total_variaveis + 1)):

        nomde_grafico = 'Plano fase C' + str(variavel)
        print(nomde_grafico)
        print("total de pontos plotados = %d" % (ler))

        # matplotlib plot
        # figura deve ser definida como subplots e retornas os axes para posterior configuracao do tick format
        fig, ax = pyplot.subplots(1, 1, figsize=(10, 10))

        data_1__x = []
        data_1__y = []

        print("total de pontos=" + str(shape[0]))
        for j in range(shape[0] - ler, shape[0]):
            data_1__y.append(data[j, 2 * variavel])
            data_1__x.append(data[j, 2 * variavel - 1])

        #tenta juntar o plot da secao de poicanre se ela existir com o plot do plano fase
        try:
            juntar_plots(ax, [path + "poincare_C%d.npz" % (variavel)])
        except:
            None

        config_plot = dict(linewidth=1)
        pyplot.plot(data_1__x, data_1__y, **config_plot)

        # saving file to load in another python file
        np.savez(path + "plano_fase_C%d.npz" % (variavel),
                 method='plot',
                 args=(data_1__x, data_1__y),
                 kwargs=config_plot)

        pyplot.xlim(
            min(data_1__x) - 0.01 * abs(min(data_1__x)),
            max(data_1__x) + 0.01 * abs(max(data_1__x)))
        pyplot.ylim(
            min(data_1__y) - 0.01 * abs(min(data_1__y)),
            max(data_1__y) + 0.01 * abs(max(data_1__y)))
        pyplot.ylabel(r'$\dot{W_{' + str(variavel) + '}}/h$')
        pyplot.xlabel(r'$W_{' + str(variavel) + '}/h$')
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
    shape = data.shape
    # numero de seções a serem lidas
    ler = None
    if (ler == None or ler > shape[0]):
        ler = shape[0]
    total_variaveis = (shape[1] - 1) / 2
    gera_plot(path, data, ler, shape, total_variaveis)
