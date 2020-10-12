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
def gera_plot(path, data, total_linhas, correcao_frequencia):
    import hashName
    pasta_hash: str = hashName.nome_hash(os.getcwd() + "\\" + path)

    dx, dy = 0.5, 0.5
    n_div_x = 21
    n_div_y = 21
    y, x = np.mgrid[slice(-5 - dy / 2, 5 + dy, dy),
                    slice(-5 - dx / 2, 5 + dx, dx)]

    #computando norma de z
    z = []
    z0 = np.zeros((n_div_x, n_div_y))

    for i in data:
        norma = np.linalg.norm(np.array([i[3], i[4]]), 2)
        z.append(norma)

        # define linha em x
        x_l = int((i[0] - 1) / n_div_y)
        y_l = int((i[0] - 1) % n_div_y)
        z0[y_l, x_l] = norma

    from matplotlib.colors import BoundaryNorm
    from matplotlib.ticker import MaxNLocator
    from matplotlib.colors import ListedColormap

    levels = MaxNLocator(nbins=15).tick_values(0, max(z))

    cmap = ListedColormap(
        ["white", "blue", "yellow", "green", "red", "gray", "black"])
    norm = BoundaryNorm(levels, ncolors=cmap.N, clip=True)

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
    pyplot.savefig(path + nomde_grafico + '.png', dpi=300, bbox_inches='tight')
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
    total_linhas = shape[0]
    gera_plot(path, data[0], total_linhas, correcao_frequencia)